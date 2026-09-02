#!/usr/bin/env python3
"""Apply the frozen campaign master to trainerproc source in bounded batches."""

from __future__ import annotations

import argparse
import re
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MASTER = ROOT / "docs" / "emerald_champions_master_battle_design.txt"
TRAINERS_PARTY = ROOT / "src" / "data" / "trainers.party"
ENCOUNTER_RE = re.compile(r"(?m)^=== ENCOUNTER (\d{4}) ===$")
TRAINER_BLOCK_RE = re.compile(r"(?m)^=== (TRAINER_[A-Z0-9_]+) ===$")
BRANCH_RE = re.compile(r"(?m)^--- BRANCH ([A-Z0-9_]+) ---$")
MON_RE = re.compile(
    r"(?m)^  \d+\. (SPECIES_[A-Z0-9_]+) @ (ITEM_[A-Z0-9_]+) \| "
    r"level_offset=(-?\d+) \| ability=(ABILITY_[A-Z0-9_]+) \| "
    r"nature=(NATURE_[A-Z0-9_]+) \| stat_points=([0-9/]+) \| moves=([A-Z0-9_,]+)$"
)
SMART_AI_OVERRIDES = {
    # These are intentionally 6.8 difficulty breathers, but their six-slot
    # Victory Road weather/role sequences still require coherent move choice.
    "TRAINER_EDGAR",
    "TRAINER_CAROLINE",
}


@dataclass
class Mon:
    species: str
    item: str
    level: int
    ability: str
    nature: str
    points: list[int]
    moves: list[str]


@dataclass
class Design:
    encounter: int
    trainer: str
    format: str
    difficulty: float
    mons: list[Mon]
    tier: int = 1


def line_value(text: str, key: str) -> str:
    match = re.search(rf"(?m)^{re.escape(key)}: (.*)$", text)
    return match.group(1) if match else ""


def split_by_markers(text: str, pattern: re.Pattern[str]) -> tuple[str, list[tuple[re.Match[str], str]]]:
    markers = list(pattern.finditer(text))
    prefix = text[:markers[0].start()] if markers else text
    return prefix, [
        (marker, text[marker.start():markers[index + 1].start() if index + 1 < len(markers) else len(text)])
        for index, marker in enumerate(markers)
    ]


def read_designs() -> dict[str, Design]:
    text = MASTER.read_text()
    _header, encounters = split_by_markers(text, ENCOUNTER_RE)
    designs = {}
    for encounter_marker, encounter in encounters:
        encounter_number = int(encounter_marker.group(1))
        cap = int(line_value(encounter, "strict_cap"))
        difficulty = float(line_value(encounter, "difficulty_target"))
        location = line_value(encounter, "location")
        fatigue_role = line_value(encounter, "fatigue_role")
        _prefix, branches = split_by_markers(encounter, BRANCH_RE)
        for _branch_marker, branch in branches:
            trainer = line_value(branch, "trainer_id")
            fmt = line_value(branch, "format")
            mons = []
            for match in MON_RE.finditer(branch):
                points = [int(value) for value in match.group(6).split("/")]
                mons.append(Mon(
                    species=match.group(1),
                    item=match.group(2),
                    level=max(1, min(100, cap + int(match.group(3)))),
                    ability=match.group(4),
                    nature=match.group(5),
                    points=points,
                    moves=match.group(7).split(","),
                ))
            if trainer in designs:
                raise ValueError(f"duplicate trainer design {trainer}")
            tier = trainer_tier(trainer, location, fatigue_role)
            apply_tier(mons, tier, cap)
            designs[trainer] = Design(encounter_number, trainer, fmt, difficulty, mons, tier)
    return designs


TRAINER_CLASSES: dict[str, str] = {}


def trainer_class(trainer: str) -> str:
    if not TRAINER_CLASSES:
        for block in TRAINERS_PARTY.read_text().split("=== TRAINER_")[1:]:
            name = "TRAINER_" + block.split(" ===", 1)[0]
            match = re.search(r"(?m)^Class: (.*)$", block)
            TRAINER_CLASSES[name] = match.group(1) if match else ""
    return TRAINER_CLASSES.get(trainer, "")


ACE_CLASSES = {"Cooltrainer", "Cooltrainer 2", "Expert", "Pkmn Ranger", "Dragon Tamer", "Winstrate"}
TEAM_CLASSES = {"Team Magma", "Team Aqua"}
ADMIN_CLASSES = {"Magma Admin", "Aqua Admin"}
BOSS_CLASSES = {
    "Leader", "Magma Leader", "Aqua Leader", "Rival", "Elite Four", "Champion",
    "Arena Tycoon", "Pike Queen", "Palace Maven", "Salon Maiden", "Dome Ace",
    "Factory Head", "Pyramid King",
}

# Skill tiers. Every campaign trainer belongs to exactly one, and the tier
# decides its AI, how much of the 66-point Stat Point budget its team spends,
# and a level nudge relative to the encounter's authored offset. This is what
# makes a Youngster feel different from a Cooltrainer, a Grunt from an Admin,
# and a Gym trainer from the Elite Four, before any team-composition choice.
TIER_BREATHER = 0   # deliberate breather: a person with a hobby
TIER_STANDARD = 6   # ordinary route trainer: competent, not clever
TIER_ACE = 1        # route ace / notable optional: plays real tactics
TIER_TEAM = 2       # Magma/Aqua grunt: coordinated but not clever
TIER_GYM = 3        # Gym trainer / mini-boss: teaches one mechanic well
TIER_ADMIN = 4      # Team admin: switches and predicts
TIER_BOSS = 5       # Leader / Rival / Elite Four / Champion / Frontier Brain

TIER_AI = {
    TIER_BREATHER: ["Basic Trainer", "Hp Aware"],
    TIER_STANDARD: ["Basic Trainer", "Hp Aware", "Smart Mon Choices"],
    TIER_ACE: ["Basic Trainer", "Hp Aware", "Smart Mon Choices", "Assume Stab", "Assume Status Moves"],
    TIER_TEAM: ["Basic Trainer", "Hp Aware", "Smart Mon Choices", "Assume Stab"],
    TIER_GYM: ["Basic Trainer", "Hp Aware", "Smart Mon Choices", "Pp Stall Prevention", "Assume Stab", "Assume Status Moves"],
    TIER_ADMIN: [
        "Basic Trainer", "Hp Aware", "Smart Switching", "Predict Switch",
        "Predict Incoming Mon", "Pp Stall Prevention", "Assume Stab", "Assume Status Moves",
    ],
    TIER_BOSS: [
        "Smart Trainer", "Prediction", "Know Opponent Party", "Powerful Status",
        "Hp Aware", "Ability Omniscience", "Item Omniscience", "Move Omniscience",
    ],
}
# Fraction of the 66 Stat Point budget each tier is allowed to spend.
TIER_STAT_BUDGET = {
    TIER_BREATHER: 0.55,
    TIER_STANDARD: 0.70,
    TIER_ACE: 0.85,
    TIER_TEAM: 0.70,
    TIER_GYM: 0.90,
    TIER_ADMIN: 1.00,
    TIER_BOSS: 1.00,
}
# Level nudge applied on top of the authored level_offset.
TIER_LEVEL_DELTA = {
    TIER_BREATHER: -1,
    TIER_STANDARD: 0,
    TIER_ACE: 0,
    TIER_TEAM: 0,
    TIER_GYM: 0,
    TIER_ADMIN: 1,
    TIER_BOSS: 1,
}


def trainer_tier(trainer: str, location: str, fatigue_role: str) -> int:
    cls = trainer_class(trainer)
    if cls in BOSS_CLASSES or fatigue_role == "marquee_boss":
        if trainer == "TRAINER_WALLY_MAUVILLE":
            return TIER_GYM
        return TIER_BOSS
    if cls in ADMIN_CLASSES:
        return TIER_ADMIN
    if "Gym" in location or fatigue_role == "mini_boss_or_exceptional_trainer":
        return TIER_GYM
    if cls in TEAM_CLASSES:
        return TIER_TEAM
    if cls in ACE_CLASSES or fatigue_role == "notable_optional_or_route_ace":
        return TIER_ACE
    if fatigue_role == "ordinary_breather":
        return TIER_BREATHER
    return TIER_STANDARD


EVOLUTION_LEVELS: dict[str, int] = {}


def evolution_level(species: str) -> int | None:
    if not EVOLUTION_LEVELS:
        for path in (ROOT / "src" / "data" / "pokemon" / "species_info").glob("gen_*_families.h"):
            text = path.read_text()
            for level, evolved in re.findall(r"\{EVO_LEVEL,\s*(\d+),\s*(SPECIES_[A-Z0-9_]+)", text):
                EVOLUTION_LEVELS[evolved] = min(EVOLUTION_LEVELS.get(evolved, 1000), int(level))
    return EVOLUTION_LEVELS.get(species)


def scale_points(points: list[int], budget: float) -> list[int]:
    # Scale the authored spread down to the tier's budget, keeping its shape.
    # Bosses keep the full 66; a breather spends about half of it.
    if budget >= 1.0:
        return points
    scaled = [int(value * budget) for value in points]
    # Keep the spread's dominant stats meaningful: never drop a 32 below 16.
    return [max(value, 16) if original == 32 else value for value, original in zip(scaled, points)]


def apply_tier(mons: list[Mon], tier: int, cap: int) -> None:
    delta = TIER_LEVEL_DELTA[tier]
    budget = TIER_STAT_BUDGET[tier]
    for mon in mons:
        mon.points = scale_points(mon.points, budget)
        level = max(1, min(100, mon.level + delta))
        evo = evolution_level(mon.species)
        if delta < 0 and evo is not None and level < evo <= mon.level:
            level = mon.level
        mon.level = level


def ai_flags(design: Design) -> str:
    if design.trainer in SMART_AI_OVERRIDES:
        flags = ["Basic Trainer", "Hp Aware", "Smart Mon Choices", "Assume Stab", "Assume Status Moves"]
    else:
        flags = list(TIER_AI[design.tier])
    moves = {move for mon in design.mons for move in mon.moves}
    if moves.intersection({"MOVE_EXPLOSION", "MOVE_SELF_DESTRUCT", "MOVE_MISTY_EXPLOSION"}):
        flags.append("Will Suicide")
    return " / ".join(flags)


def replace_attribute(header: str, key: str, value: str) -> str:
    pattern = rf"(?m)^{re.escape(key)}:.*$"
    if re.search(pattern, header):
        return re.sub(pattern, f"{key}: {value}", header, count=1)
    return header.rstrip() + f"\n{key}: {value}"


def render_party(design: Design) -> str:
    rows = []
    stat_names = ("HP", "Atk", "Def", "SpA", "SpD", "Spe")
    for mon in design.mons:
        title = mon.species if mon.item == "ITEM_NONE" else f"{mon.species} @ {mon.item}"
        rows.extend([
            title,
            f"Level: {mon.level}",
            f"Ability: {mon.ability}",
            "IVs: 31 HP / 31 Atk / 31 Def / 31 SpA / 31 SpD / 31 Spe",
            "EVs: " + " / ".join(f"{value} {name}" for value, name in zip(mon.points, stat_names)),
            f"Nature: {mon.nature}",
        ])
        rows.extend(f"- {move}" for move in mon.moves if move != "MOVE_NONE")
        rows.append("")
    return "\n".join(rows).rstrip()


def rewrite_trainer_block(block: str, design: Design) -> str:
    section_end = block.find("\n\n")
    header = block[:section_end] if section_end >= 0 else block.rstrip()
    header = replace_attribute(header, "Double Battle", "Yes" if design.format in ("double", "multi") else "No")
    header = replace_attribute(header, "AI", ai_flags(design))
    # Campaign battles are competitive puzzles.  The player and opposing
    # Trainer both rely on held loadouts rather than Potion/Full Restore AI.
    header = re.sub(r"(?m)^Items:.*\n?", "", header)
    if design.format == "multi":
        header = replace_attribute(header, "Multi Party", "Half")
    else:
        header = re.sub(r"(?m)^Multi Party:.*\n?", "", header)
    return header.rstrip() + "\n\n" + render_party(design) + "\n\n"


def implement(through_encounter: int) -> tuple[str, int, list[str]]:
    designs = read_designs()
    source = TRAINERS_PARTY.read_text()
    prefix, blocks = split_by_markers(source, TRAINER_BLOCK_RE)
    rendered = [prefix]
    applied = 0
    seen = set()
    for marker, block in blocks:
        trainer = marker.group(1)
        design = designs.get(trainer)
        if design is not None and design.encounter <= through_encounter:
            block = rewrite_trainer_block(block, design)
            applied += 1
            seen.add(trainer)
        rendered.append(block)
    expected = {
        trainer for trainer, design in designs.items()
        if design.encounter <= through_encounter
    }
    missing = sorted(expected - seen)
    # Canonical source uses one terminal newline; block rendering may otherwise
    # accumulate an irrelevant extra blank line at end of file.
    return "".join(rendered).rstrip() + "\n", applied, missing


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--through-encounter", type=int, required=True)
    parser.add_argument("--verify-only", action="store_true")
    args = parser.parse_args()
    if not 1 <= args.through_encounter <= 513:
        raise SystemExit("--through-encounter must be in 1..513")
    output, applied, missing = implement(args.through_encounter)
    if args.verify_only:
        if output != TRAINERS_PARTY.read_text():
            raise SystemExit("trainer source differs from the requested master prefix")
    else:
        TRAINERS_PARTY.write_text(output)
    print(f"implemented_trainer_branches={applied} through_encounter={args.through_encounter}")
    if missing:
        print("planned_restore_trainers_missing_from_current_source=" + ",".join(missing))
    if args.verify_only:
        print("trainer_master_prefix_verification=PASS")


if __name__ == "__main__":
    main()
