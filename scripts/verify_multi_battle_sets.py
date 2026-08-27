#!/usr/bin/env python3
"""Verify native multi-set tutor data, legality, and menu routing."""

from __future__ import annotations

import json
import re
import subprocess
import sys
from collections import Counter
from pathlib import Path

import verdant_battle_set_presets as presets


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


def check(name: str, condition: bool) -> None:
    if not condition:
        raise AssertionError(name)
    print(f"PASS: {name}")


subprocess.run(
    [sys.executable, str(ROOT / "scripts/verdant_handbook_battle_sets.py"), "--check"],
    cwd=ROOT,
    check=True,
)

payload = json.loads(read("docs/verdant_multi_battle_sets.json"))
raw = json.loads(read("docs/pokemon_champions_handbook_sets.json"))
defaults = {row["species"]: row for row in json.loads(read("docs/verdant_battle_set_presets.json"))["presets"]}
dex = presets.LocalDex()

check(
    "the complete supplied handbook was extracted deterministically",
    len(raw["sets"]) == raw["declared_set_count"] == 1216
    and len({row["national_dex"] for row in raw["sets"]}) == raw["declared_species_count"] == 1025,
)
check(
    "the authored local default remains Set 1 for every supported species/form",
    payload["set_count"] == len(defaults) + payload["alternative_count"],
)
check(
    "the two handbook default improvements are explicit and source-reviewed",
    {row["species"] for row in payload["default_promotions"]} == {"SPECIES_AZUMARILL", "SPECIES_BLAZIKEN"}
    and defaults["SPECIES_AZUMARILL"]["moves"][2] == "MOVE_PLAY_ROUGH"
    and defaults["SPECIES_BLAZIKEN"]["moves"][1] == "MOVE_CLOSE_COMBAT",
)
check(
    "the handbook contributes a substantial but bounded alternate-set library",
    payload["species_with_choices"] >= 250
    and payload["alternative_count"] >= 300
    and all(1 <= row["count"] <= 2 for row in payload["ranges"].values()),
)

offset = 0
range_errors = []
for species, row in payload["ranges"].items():
    if row["offset"] != offset:
        range_errors.append(species)
    offset += row["count"]
check("all generated alternative ranges are contiguous and cover the bank exactly", not range_errors and offset == len(payload["alternatives"]))

choice_errors = []
for species, row in payload["ranges"].items():
    default = defaults[species]
    legal = dex.legal_moves(species)
    signatures = {(frozenset(move for move in default["moves"] if move != "MOVE_NONE"), default["nature"], default["ability"])}
    choices = payload["alternatives"][row["offset"]:row["offset"] + row["count"]]
    if not payload["default_names"].get(species):
        choice_errors.append(f"{species}: missing default label")
    for choice in choices:
        moves = [move for move in choice["moves"] if move != "MOVE_NONE"]
        signature = (frozenset(moves), choice["nature"], choice["ability"])
        valid = (
            1 <= len(moves) <= 4
            and len(moves) == len(set(moves))
            and all(move in legal for move in moves)
            and choice["ability"] in dex.stats[species].abilities
            and dex.stats[species].abilities[choice["ability_slot"]] == choice["ability"]
            and signature not in signatures
            and len(choice["name"]) < 24
        )
        if not valid:
            choice_errors.append(f"{species}: {choice['name']}")
        signatures.add(signature)
    labels = [payload["default_names"].get(species)] + [choice["name"] for choice in choices]
    if len(labels) != len(set(labels)):
        choice_errors.append(f"{species}: duplicate menu labels")
check("every generated choice is labeled, legal, distinct, and runtime-safe", not choice_errors)

evidence = Counter(row["handbook"]["evidence"] for row in payload["alternatives"])
check(
    "retained choices are dominated by direct ladder or Smogon doubles evidence",
    evidence["M-B ladder data"] >= 160
    and evidence["Smogon doubles pool"] >= 140
    and evidence["Projected"] <= 20,
)

font_widths = [
    int(value)
    for line in read("graphics/fonts/font1_latin_widths.inc").splitlines()
    for value in re.findall(r"\d+", line)
]
charmap = {}
for line in read("charmap.txt").splitlines():
    match = re.match(r"'(.*)'\s*=\s*([0-9A-Fa-f]{2})\s*$", line)
    if match and len(match.group(1)) == 1:
        charmap[match.group(1)] = int(match.group(2), 16)


def width(text: str) -> int:
    return sum(font_widths[charmap[char]] for char in text if char in charmap)


all_names = list(payload["default_names"].values()) + [row["name"] for row in payload["alternatives"]] + ["Exit"]
check(
    "every generated role label fits the native scrolling menu",
    max(map(width, all_names)) <= 200,
)

runtime = read("src/verdant_battle_sets.c")
field = read("src/field_specials.c")
tutor = read("data/scripts/pokemon_center_move_tutor.inc")
capture = read("src/battle_script_commands.c")
check(
    "wild catches continue to receive only the authored default Set 1",
    "ApplyVerdantBattleSetPreset(&gEnemyParty[partyIndex])" in capture
    and "ApplyVerdantBattleSetChoice(&gEnemyParty[partyIndex]" not in capture,
)
check(
    "the tutor applies a bounds-checked selected preset without equipping an item",
    all(token in runtime for token in (
        "ApplyVerdantBattleSetChoice",
        "choice > range->count",
        "ApplyValidatedBattleSetPreset",
    ))
    and "MON_DATA_HELD_ITEM" not in runtime,
)
check(
    "the role picker uses the existing native scrolling menu and a maximum of three sets",
    "case SCROLL_MULTI_BATTLE_SET_STYLE:" in field
    and "task->tNumItems = GetVerdantBattleSetCount" in field
    and "text = GetVerdantBattleSetName" in field
    and "task->tMaxItemsOnScreen = task->tNumItems" in field,
)
check(
    "the script preserves the chosen party slot, set index, confirmation, and cancellation paths",
    all(token in tutor for token in (
        "copyvar VAR_0x800A, VAR_0x8004",
        "special GetSelectedMonBattleSetCount",
        "SCROLL_MULTI_BATTLE_SET_STYLE",
        "copyvar VAR_0x8005, VAR_0x8006",
        "special BufferSelectedMonBattleSetName",
        "special ApplySelectedMonBattleSet",
    )),
)

print(
    "Multi-set tutor verification: "
    f"{payload['set_count']} total sets, "
    f"{payload['species_with_choices']} species/forms with choices, "
    f"evidence={dict(evidence)}"
)
