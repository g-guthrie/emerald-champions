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
            designs[trainer] = Design(encounter_number, trainer, fmt, difficulty, mons)
    return designs


def ai_flags(design: Design) -> str:
    if design.trainer in SMART_AI_OVERRIDES:
        flags = ["Basic Trainer", "Hp Aware", "Smart Mon Choices", "Assume Stab", "Assume Status Moves"]
    elif design.difficulty >= 9.5:
        flags = [
            "Smart Trainer", "Prediction", "Know Opponent Party", "Powerful Status",
            "Hp Aware", "Ability Omniscience", "Item Omniscience", "Move Omniscience",
        ]
    elif design.difficulty >= 8.0:
        flags = [
            "Basic Trainer", "Hp Aware", "Smart Switching", "Predict Switch",
            "Predict Incoming Mon", "Pp Stall Prevention", "Assume Stab", "Assume Status Moves",
        ]
    elif design.difficulty >= 7.0:
        flags = ["Basic Trainer", "Hp Aware", "Smart Mon Choices", "Assume Stab", "Assume Status Moves"]
    else:
        flags = ["Basic Trainer"]
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
