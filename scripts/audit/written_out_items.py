#!/usr/bin/env python3
"""Release gate: items written out of the game stay out of it.

Every Pokemon joining the player arrives with a full EV spread, Center move
tutors re-plan whole spreads, battles give no EVs and the party menu switches
Abilities for free. The six EV vitamins, the six Power items, Ability Capsule
and Ability Patch have no job left, so none may come back as shop stock, a
pickup, a gift, a prize, an exchange, a wild or trainer held item (Thief and
Covet keep what they take) or a Pickup find.

Their definitions and use code stay for save compatibility and the engine;
only the files below may name them. Everything else in data/, src/ and
include/ is scanned. Generated map event files are rebuilt from map.json and
are not sources of truth.
"""
from __future__ import annotations

import os
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
WRITTEN_OUT = (
    "HP_UP", "PROTEIN", "IRON", "CALCIUM", "ZINC", "CARBOS",
    "POWER_WEIGHT", "POWER_BRACER", "POWER_BELT", "POWER_LENS", "POWER_BAND", "POWER_ANKLET",
    "ABILITY_CAPSULE", "ABILITY_PATCH",
)
DEFINITIONS = {
    "include/constants/items.h",       # item ids
    "src/data/items.h",                # names, prices, descriptions, use callbacks
    "src/data/pokemon/item_effects.h", # what using one would do
    "src/data/graphics/items.h",       # icons
    "src/item.c",                      # Ability Capsule/Patch party-menu check
}
SOURCE_DIRS = ("data", "src", "include")
SOURCE_SUFFIXES = (".inc", ".s", ".json", ".c", ".h", ".txt", ".party", ".pory")
GENERATED = ("events.inc", "header.inc", "connections.inc")

NAMES = "|".join(WRITTEN_OUT)
DISPLAY = "|".join(name.replace("_", "[ _]") for name in WRITTEN_OUT)
CONSTANT = re.compile(r"\bITEM_(?:" + NAMES + r")\b")
# Team shorthand: "TREECKO @PROTEIN OVERGROW ..." (IRON_BALL is its own token).
HELD_TOKEN = re.compile(r"@\s*(?:ITEM_)?(?:" + NAMES + r")\b", re.I)
# Showdown-style party line: the held item runs to the end ("Mon @ Power Anklet").
HELD_LINE = re.compile(r"@\s*(?:ITEM_)?(?:" + DISPLAY + r")\s*$", re.I)
# Showdown-style JSON: "item": "Power Anklet".
JSON_NAME = re.compile(r'"\w*item\w*"\s*:\s*"(?:ITEM_)?(?:' + DISPLAY + r')"', re.I)


def patterns_for(path: Path):
    yield CONSTANT
    if path.suffix == ".txt":
        yield HELD_TOKEN
    if path.suffix in (".party", ".txt"):
        yield HELD_LINE
    if path.suffix == ".json":
        yield JSON_NAME


def find_references() -> list[str]:
    hits = []
    for directory in SOURCE_DIRS:
        for dirpath, _, filenames in os.walk(ROOT / directory):
            for filename in sorted(filenames):
                path = Path(dirpath) / filename
                rel = path.relative_to(ROOT).as_posix()
                if not filename.endswith(SOURCE_SUFFIXES) or filename in GENERATED or rel in DEFINITIONS:
                    continue
                try:
                    lines = path.read_text(errors="ignore").splitlines()
                except OSError:
                    continue
                for number, line in enumerate(lines, 1):
                    if any(pattern.search(line) for pattern in patterns_for(path)):
                        hits.append(f"{rel}:{number}: {line.strip()}")
    return hits


def main() -> int:
    # Guard the guard: the ids must still exist under these names, or the scan
    # below would pass by matching nothing.
    header = (ROOT / "include/constants/items.h").read_text()
    missing = [name for name in WRITTEN_OUT if not re.search(r"\bITEM_" + name + r"\b", header)]
    if missing:
        print("FAIL: written-out item ids are missing from include/constants/items.h: " + ", ".join(missing))
        return 1
    hits = find_references()
    if not hits:
        print(f"PASS: none of the {len(WRITTEN_OUT)} written-out items (vitamins, Power items, "
              "Ability Capsule and Patch) is handed out, sold, held or found anywhere")
        return 0
    print(f"FAIL: {len(hits)} references to written-out items outside their definitions")
    for hit in hits:
        print("  " + hit)
    return 1


if __name__ == "__main__":
    sys.exit(main())
