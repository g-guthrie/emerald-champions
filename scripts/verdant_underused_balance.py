#!/usr/bin/env python3
"""Apply restrained stat corrections to Verdant's remaining weak species."""

from __future__ import annotations

import argparse
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BASE_STATS = ROOT / "src/data/pokemon/base_stats.h"

# Inclement already substantially rebalanced most weak Pokémon. These targets
# add only 10-20 BST (or 9 for Galarian Stunfisk) to remaining laggards.
TARGETS = {
    "SPECIES_DELIBIRD": {"baseHP": 55, "baseDefense": 50, "baseSpDefense": 50},
    "SPECIES_BEAUTIFLY": {"baseHP": 65, "baseDefense": 65, "baseSpeed": 90, "baseSpDefense": 65},
    "SPECIES_CHATOT": {"baseHP": 80, "baseDefense": 50, "baseSpDefense": 47},
    "SPECIES_MORPEKO": {"baseHP": 64, "baseDefense": 62, "baseSpDefense": 62},
    "SPECIES_MOTHIM": {"baseHP": 75, "baseSpeed": 91, "baseSpAttack": 100},
    "SPECIES_WORMADAM": {"baseHP": 86, "baseSpeed": 40},
    "SPECIES_WORMADAM_SANDY_CLOAK": {"baseHP": 86, "baseSpeed": 40},
    "SPECIES_WORMADAM_TRASH_CLOAK": {"baseHP": 86, "baseSpeed": 40},
    "SPECIES_WATCHOG": {"baseAttack": 100, "baseSpeed": 87},
    "SPECIES_DELCATTY": {"baseHP": 75, "baseDefense": 70, "baseSpDefense": 60},
    "SPECIES_STUNFISK_GALARIAN": {"baseAttack": 85, "baseSpDefense": 89},
    "SPECIES_CHIMECHO": {"baseHP": 90, "baseSpeed": 75},
    "SPECIES_CARNIVINE": {"baseHP": 80, "baseAttack": 105, "baseSpAttack": 95},
    "SPECIES_BEEDRILL": {"baseDefense": 45, "baseSpeed": 95},
    "SPECIES_KRICKETUNE": {"baseDefense": 56, "baseSpDefense": 56},
    "SPECIES_THIEVUL": {"baseSpeed": 95, "baseSpAttack": 95},
    "SPECIES_PHIONE": {"baseSpeed": 85, "baseSpAttack": 85},
}

STAT_FIELDS = ("baseHP", "baseAttack", "baseDefense", "baseSpeed", "baseSpAttack", "baseSpDefense")


def species_block(text: str, species: str) -> re.Match:
    match = re.search(
        rf"(^\s*\[{re.escape(species)}\]\s*=\s*\{{)(.*?)(?=^\s*\[SPECIES_|\Z)",
        text,
        re.M | re.S,
    )
    if not match:
        raise ValueError(f"missing base-stat block: {species}")
    return match


def active_rebalanced(text: str) -> str:
    output, stack, active = [], [], True
    for line in text.splitlines():
        match = re.match(r"^\s*#\s*(ifdef|ifndef)\s+REBALANCED_VERSION\s*$", line)
        if match:
            condition = match.group(1) == "ifdef"
            stack.append((active, condition))
            active = active and condition
            continue
        if re.match(r"^\s*#\s*else\s*$", line) and stack:
            parent, condition = stack[-1]
            stack[-1] = (parent, not condition)
            active = parent and not condition
            continue
        if re.match(r"^\s*#\s*endif\s*$", line) and stack:
            active = stack.pop()[0]
            continue
        if active:
            output.append(line)
    return "\n".join(output)


def current_stats(text: str, species: str) -> dict[str, int]:
    block = species_block(active_rebalanced(text), species).group(2)
    values = {}
    for field in STAT_FIELDS:
        match = re.search(rf"\.{field}\s*=\s*(\d+)", block)
        if not match:
            raise ValueError(f"{species} has no {field}")
        values[field] = int(match.group(1))
    return values


def apply() -> None:
    text = BASE_STATS.read_text()
    for species, targets in TARGETS.items():
        match = species_block(text, species)
        block = match.group(0)
        active = current_stats(text, species)
        for field, target in targets.items():
            current = active[field]
            if current == target:
                continue
            if current > target:
                raise ValueError(f"refusing to reduce {species} {field}: {current} -> {target}")
            field_match = re.search(rf"(\.{field}\s*=\s*){current}\b", block)
            if not field_match:
                raise ValueError(f"could not locate active {species} {field}={current}")
            block = block[:field_match.start()] + field_match.group(1) + str(target) + block[field_match.end():]
            active[field] = target
        text = text[:match.start()] + block + text[match.end():]
    BASE_STATS.write_text(text)
    print(f"updated {len(TARGETS)} underused species/forms")


def check() -> None:
    text = BASE_STATS.read_text()
    problems = []
    for species, targets in TARGETS.items():
        stats = current_stats(text, species)
        for field, target in targets.items():
            if stats[field] != target:
                problems.append(f"{species} {field}: {stats[field]} != {target}")
        bst = sum(stats.values())
        if bst > 510:
            problems.append(f"{species} BST exceeded restraint cap: {bst}")
    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))
    print(f"PASS: {len(TARGETS)} restrained underused-Pokémon corrections validated")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--apply", action="store_true")
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()
    if not args.apply and not args.check:
        parser.error("choose --apply or --check")
    if args.apply:
        apply()
    if args.check:
        check()


if __name__ == "__main__":
    main()
