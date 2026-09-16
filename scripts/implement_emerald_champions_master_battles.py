#!/usr/bin/env python3
"""Apply the campaign battle master to trainerproc source.

Every campaign branch in ``data/emerald_champions/emerald_champions_master_battle_design.txt``
is materialized exactly: species, held item, level (strict cap plus the
authored offset), Ability, nature, EVs and moves. Nothing here nudges
levels or trims EVs. Team/design fields are generated from battle_teams.txt;
edit that canonical source, not the materialized master.

AI comes from the encounter's ``ai_profile`` line and the compiled per-trainer
battle plan. Every campaign trainer uses the same expert information. The
bounded doubles evaluator owns response forecasting; the old generic
Prediction flag redundantly searches predicted switches during initialization.
Difficulty is expressed through levels, team size and team composition.
"""

from __future__ import annotations

import argparse
import re
from dataclasses import dataclass
from pathlib import Path

from emerald_champions_evs import validate_evs
from emerald_champions_teams import BRANCH_RE, ENCOUNTER_RE, line_value

ROOT = Path(__file__).resolve().parents[1]
MASTER = ROOT / "data/emerald_champions/emerald_champions_master_battle_design.txt"
TRAINERS_PARTY = ROOT / "src" / "data" / "trainers.party"
TRAINER_BLOCK_RE = re.compile(r"(?m)^=== (TRAINER_[A-Z0-9_]+) ===$")
MON_RE = re.compile(
    r"(?m)^  \d+\. (SPECIES_[A-Z0-9_]+) @ (ITEM_[A-Z0-9_]+) \| "
    r"level_offset=(-?\d+) \| ability=(ABILITY_[A-Z0-9_]+) \| "
    r"nature=(NATURE_[A-Z0-9_]+) \| evs=([0-9/]+) \| moves=([A-Z0-9_,]+) \| ivs=([0-9/]+) \| friendship=(\d+)$"
)

# Class names remain compatibility aliases. Every campaign trainer uses the
# same expert information and decision capabilities; levels/rosters set difficulty.
EXPERT_AI_PROFILE = [
    "Basic Trainer", "Omniscient", "Smart Switching", "Smart Mon Choices",
    "Pp Stall Prevention", "Hp Aware", "Try To 2HKO",
    "Powerful Status", "Know Opponent Party",
]
AI_PROFILES = {"sharp": EXPERT_AI_PROFILE, "master": EXPERT_AI_PROFILE}
SUICIDE_MOVES = {
    "MOVE_EXPLOSION", "MOVE_SELF_DESTRUCT", "MOVE_MISTY_EXPLOSION", "MOVE_FINAL_GAMBIT",
    "MOVE_MEMENTO", "MOVE_HEALING_WISH", "MOVE_LUNAR_DANCE",
}


@dataclass
class Mon:
    species: str
    item: str
    level: int
    offset: int
    ability: str
    nature: str
    evs: list[int]
    moves: list[str]
    ivs: list[int]
    friendship: int


@dataclass
class Design:
    encounter: int
    trainer: str
    format: str
    ai_profile: str
    ai_extra: list[str]
    mons: list[Mon]
    prize_multiplier: int
    field: list[str]


def split_by_markers(text: str, pattern: re.Pattern[str]) -> tuple[str, list[tuple[re.Match[str], str]]]:
    markers = list(pattern.finditer(text))
    prefix = text[:markers[0].start()] if markers else text
    return prefix, [
        (marker, text[marker.start():markers[index + 1].start() if index + 1 < len(markers) else len(text)])
        for index, marker in enumerate(markers)
    ]


def read_designs(master: Path = MASTER) -> dict[str, Design]:
    text = master.read_text()
    _header, encounters = split_by_markers(text, ENCOUNTER_RE)
    designs: dict[str, Design] = {}
    for encounter_marker, encounter in encounters:
        encounter_number = int(encounter_marker.group(1))
        cap = int(line_value(encounter, "strict_cap"))
        ai_profile = line_value(encounter, "ai_profile") or "sharp"
        if ai_profile not in AI_PROFILES:
            raise ValueError(f"encounter {encounter_number}: unknown ai_profile {ai_profile!r}")
        _prefix, branches = split_by_markers(encounter, BRANCH_RE)
        for _branch_marker, branch in branches:
            trainer = line_value(branch, "trainer_id")
            fmt = line_value(branch, "format")
            mons = []
            for match in MON_RE.finditer(branch):
                offset = int(match.group(3))
                if not -254 <= offset <= 254:
                    raise ValueError(f"{trainer}: level offset {offset} is outside the native -254..254 representation")
                level = min(255, max(1, cap + offset))
                mons.append(Mon(
                    species=match.group(1),
                    item=match.group(2),
                    level=level,
                    offset=int(match.group(3)),
                    ability=match.group(4),
                    nature=match.group(5),
                    evs=validate_evs([int(value) for value in match.group(6).split("/")]),
                    moves=match.group(7).split(","),
                    ivs=[int(v) for v in match.group(8).split("/")],
                    friendship=int(match.group(9)),
                ))
            if not mons:
                raise ValueError(f"{trainer}: no EV-encoded Pokemon records")
            if trainer in designs:
                raise ValueError(f"duplicate trainer design {trainer}")
            extra = line_value(branch, "ai_extra")
            ai_extra = [trait.strip() for trait in extra.split(",") if trait.strip()] if extra else []
            cls = line_value(encounter, "battle_class")
            rate = 5 if cls in {"regular", "casual", "grunt"} else 10 if cls in {"ace", "gym", "rival", "brain"} else 25
            if 493 <= encounter_number <= 496:
                rate = 40
            elif encounter_number == 497:
                rate = 50
            field_line = line_value(branch, "field")
            field = [value.strip() for value in field_line.split(",") if value.strip()] if field_line else []
            designs[trainer] = Design(encounter_number, trainer, fmt, ai_profile, ai_extra, mons, rate, field)
    return designs


def ai_flags(design: Design) -> str:
    flags = list(AI_PROFILES[design.ai_profile])
    for trait in design.ai_extra:
        if trait not in flags:
            flags.append(trait)
    moves = {move for mon in design.mons for move in mon.moves}
    if moves & SUICIDE_MOVES and "Will Suicide" not in flags:
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
            f"Level Offset: {mon.offset}",
            f"Ability: {mon.ability}",
            "IVs: " + " / ".join(f"{value} {name}" for value, name in zip(mon.ivs, stat_names)),
            f"Happiness: {mon.friendship}",
            "EVs: " + " / ".join(f"{value} {name}" for value, name in zip(mon.evs, stat_names)),
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
    header = replace_attribute(header, "Prize Multiplier", str(design.prize_multiplier))
    # A re-implemented design must not inherit a retirement's "Party Size: 0"
    # stamp: trainerproc would compile the authored party as an empty battle.
    header = re.sub(r"(?m)^Party Size:.*\n?", "", header)
    # Campaign battles are competitive puzzles: no Bag healing on either side.
    header = re.sub(r"(?m)^Items:.*\n?", "", header)
    # The authored starting field (Gym terrain) is the engine's only setup-side
    # weather/terrain channel; trainerproc takes the human form of STARTING_STATUS_*.
    header = re.sub(r"(?m)^Starting Status:.*\n?", "", header)
    if design.field:
        header = replace_attribute(header, "Starting Status",
                                   " / ".join(value.replace("_", " ").title() for value in design.field))
    if design.format == "multi":
        header = replace_attribute(header, "Multi Party", "Half")
    else:
        header = re.sub(r"(?m)^Multi Party:.*\n?", "", header)
    return header.rstrip() + "\n\n" + render_party(design) + "\n\n"


def implement(through_encounter: int, master: Path, party: Path) -> tuple[str, int, list[str]]:
    designs = read_designs(master)
    source = party.read_text()
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
        elif design is None and trainer != "TRAINER_NONE":
            # Keep names/classes and numeric IDs for saved flags/Match Call,
            # but never compile a discarded campaign party as hidden content.
            header = re.split(r"\n\s*\n", block, maxsplit=1)[0]
            header = replace_attribute(header, "Party Size", "0")
            block = header.rstrip() + "\n\n/* Retired: metadata only; no battle party in the canonical book. */\n\n"
        rendered.append(block)
    expected = {trainer for trainer, design in designs.items() if design.encounter <= through_encounter}
    missing = sorted(expected - seen)
    return "".join(rendered).rstrip() + "\n", applied, missing


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--through-encounter", type=int, help="optional authoring prefix; defaults to every current encounter")
    parser.add_argument("--verify-only", action="store_true")
    parser.add_argument("--master", type=Path, default=MASTER)
    parser.add_argument("--output", type=Path, default=None,
                        help="write the materialized party here instead of src/data/trainers.party")
    args = parser.parse_args()
    through = args.through_encounter
    if through is None:
        designs = read_designs(args.master)
        if not designs:
            raise SystemExit("master has no trainer designs")
        through = max(design.encounter for design in designs.values())
    if through < 1:
        raise SystemExit("--through-encounter must be positive")
    output, applied, missing = implement(through, args.master, TRAINERS_PARTY)
    if missing:
        raise SystemExit("master trainers missing from output: " + ", ".join(missing))
    if args.verify_only:
        if output != TRAINERS_PARTY.read_text():
            raise SystemExit("trainer source differs from the requested master prefix")
    else:
        (args.output or TRAINERS_PARTY).write_text(output)
    print(f"implemented_trainer_branches={applied} through_encounter={through}")
    if args.verify_only:
        print("trainer_master_prefix_verification=PASS")


if __name__ == "__main__":
    main()
