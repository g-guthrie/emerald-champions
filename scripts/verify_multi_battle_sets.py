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
    "every mapped handbook orientation is represented exactly once",
    payload["alternative_count"] == payload["expected_alternative_count"]
    and payload["species_with_choices"] >= 150
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
    signatures = {(frozenset(move for move in default["moves"] if move != "MOVE_NONE"), default["nature"], default["ability"], default["runtime_item"], "ITEM_NONE")}
    choices = payload["alternatives"][row["offset"]:row["offset"] + row["count"]]
    if not payload["default_names"].get(species):
        choice_errors.append(f"{species}: missing default label")
    for choice in choices:
        moves = [move for move in choice["moves"] if move != "MOVE_NONE"]
        signature = (frozenset(moves), choice["nature"], choice["ability"], choice["runtime_item"], choice["required_item"])
        valid = (
            1 <= len(moves) <= 4
            and len(moves) == len(set(moves))
            and all(move in legal for move in moves)
            and choice["ability"] in dex.stats[species].abilities
            and dex.stats[species].abilities[choice["ability_slot"]] == choice["ability"]
            and choice["runtime_item"] not in presets.PROTECTED_SET_ITEMS
            and (
                choice["required_item"] == "ITEM_NONE"
                or (
                    choice["required_item"] in presets.PROTECTED_SET_ITEMS
                    and choice["runtime_item"] == "ITEM_NONE"
                    and choice["name"].startswith("Mega ")
                )
            )
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
    "all retained handbook evidence classes remain explicit after local adaptation",
    sum(evidence.values()) == payload["alternative_count"]
    and set(evidence) <= {"M-B ladder data", "Smogon doubles pool", "Projected"},
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
wild = read("src/wild_encounter.c")
shop = read("src/shop.c")
item = read("src/item.c")
check(
    "ordinary wild Pokémon uniformly roll the tutor's actual one-to-three set count",
    "ApplyVerdantRandomWildBattleSet(&gEnemyParty[0])" in wild
    and "Random() % count" in runtime,
)
check(
    "the tutor maps visible choices safely and replaces only ordinary held items",
    all(token in runtime for token in (
        "ApplyVerdantBattleSetChoice",
        "ResolveBattleSetChoice",
        "visibleChoice++ == choice",
        "ApplyValidatedBattleSetPreset",
    ))
    and "SetMonData(mon, MON_DATA_HELD_ITEM, &preset->item)" in runtime
    and "BATTLE_SET_APPLY_SPECIAL_ITEM" in runtime,
)
check(
    "ordinary non-Berry runtime items are free only at the Center vendor",
    payload["free_item_count"] >= 60
    and not (set(payload["free_items"]) & presets.PROTECTED_SET_ITEMS)
    and not any(item.endswith("_BERRY") for item in payload["free_items"])
    and all(required in payload["free_items"] for required in ("ITEM_EVIOLITE", "ITEM_FOCUS_SASH", "ITEM_LEFTOVERS"))
    and "ITEM_SITRUS_BERRY" not in payload["free_items"]
    and "CreateFreePokemartMenu(sUnlockedBattleItemMart)" in field
    and "gVerdantFreeBattleItems" in item
    and "sMartInfo.freeItems" in shop
    and 'static const u8 sText_Free[] = _("FREE")' in shop
    and "if (sMartInfo.freeItems)\n        maxQuantity = 99;" in shop
    and "martType == MART_TYPE_NORMAL && !sMartInfo.freeItems" in shop
    and "void CreatePokemartMenu" in shop
    and "sMartInfo.freeItems = FALSE;" in shop,
)
check(
    "Mega presets stay hidden until the Bracelet and never create their stone",
    any(choice["required_item"] != "ITEM_NONE" for choice in payload["alternatives"])
    and "!FlagGet(FLAG_SYS_RECEIVED_KEYSTONE)" in runtime
    and "preset->requiredItem" in runtime
    and "BATTLE_SET_APPLY_MEGA_SET" in runtime
    and "I will not supply that Mega Stone" in tutor
    and "Use this set with {STR_VAR_3}" in tutor,
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
