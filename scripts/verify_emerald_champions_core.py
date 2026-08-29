#!/usr/bin/env python3
"""Static invariants for the Emerald Champions core-service checkpoint."""

from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def read(path: str) -> str:
    return (ROOT / path).read_text()


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def main() -> None:
    option_menu = read("src/option_menu.c")
    battle_setup = read("src/battle_setup.c")
    new_game = read("src/new_game.c")
    pokemon_config = read("include/config/pokemon.h")
    summary_config = read("include/config/summary_screen.h")
    nurse = read("data/scripts/pkmn_center_nurse.inc")
    items = read("src/data/items.h")
    field_specials = read("src/field_specials.c")
    party_menu = read("src/party_menu.c")
    party_menu_data = read("src/data/party_menu.h")
    vendor_scripts = read("data/scripts/emerald_champions.inc")

    require("COMPOUND_STRING(\"DIFFICULTY\")" in option_menu, "Options no longer exposes Difficulty")
    require(all(label in option_menu for label in ("DifficultyHard", "DifficultyMedium", "DifficultyEasy")), "Difficulty choices are incomplete")
    require("SetCurrentDifficultyLevel(DIFFICULTY_HARD);" in new_game, "Hard is not the new-game default")
    require(battle_setup.count("ApplyTrainerLevelDifficulty(&gParties[B_TRAINER_OPPONENT_") == 2, "Difficulty must affect exactly both enemy trainer parties")
    require("P_LEVEL_UP_MOVE_LEARNING    FALSE" in pokemon_config, "Level-up prompts are not disabled")
    require(all(value in summary_config for value in (
        "P_ENABLE_MOVE_RELEARNERS         TRUE",
        "P_PRE_EVO_MOVES                  TRUE",
        "P_ENABLE_ALL_LEVEL_UP_MOVES      TRUE",
        "P_TM_MOVES_RELEARNER             TRUE",
        "P_ENABLE_ALL_TM_MOVES            TRUE",
    )), "Complete legal tutor access is not enabled")

    require("giveitem ITEM_POKE_VIAL" in nurse and "giveitem ITEM_LEVELER" in nurse, "Center does not grant both tools")
    require("copyvar VAR_POKE_VIAL_CHARGES, VAR_POKE_VIAL_MAX_CHARGES" in nurse, "Center does not refill the Vial")
    require(re.search(r"\[ITEM_RARE_CANDY\].*?\.price = 1000,", items, re.S) is not None, "Rare Candy price is not 1,000")
    route111 = read("data/maps/Route111/scripts.inc")
    route133 = read("data/maps/Route133/scripts.inc")
    require(
        "setvar VAR_POKE_VIAL_MAX_CHARGES, 2" in route111
        and "setvar VAR_CHANSEY_NURSE_STATE, 7" in route111,
        "the one-time Chansey quest does not grant the second Vial charge",
    )
    require(
        "setvar VAR_POKE_VIAL_MAX_CHARGES, 3" in route133,
        "Route 133 does not grant the final Vial charge",
    )

    oldale = read("data/maps/OldaleTown_Mart/scripts.inc")
    expanded_oldale = oldale.split("OldaleTown_Mart_Pokemart_Expanded:", 1)[1].split("pokemartlistend", 1)[0]
    require("ITEM_POKE_BALL" in expanded_oldale, "Oldale Mart never stocks Poke Balls after the adventure starts")

    require(
        "AppendToList(sPartyMenuInternal->actions, &sPartyMenuInternal->numActions, MENU_OPEN_ABILITY)" in party_menu,
        "the normal party menu lacks on-the-fly Ability switching",
    )
    require(
        "SELECTWINDOW_ABILITY" in party_menu
        and all(token in party_menu_data for token in ("MENU_ABILITY_SLOT_0", "MENU_ABILITY_SLOT_1", "MENU_ABILITY_SLOT_2")),
        "the native Ability chooser is incomplete",
    )

    centers = tuple((ROOT / "data" / "maps").glob("*PokemonCenter_1F/map.json"))
    target_centers = []
    for path in centers:
        data = json.loads(path.read_text())
        scripts = [obj["script"] for obj in data["object_events"]]
        if "Common_EventScript_EmeraldChampionsBattleVendor" in scripts or "Common_EventScript_EmeraldChampionsMoveTutor" in scripts:
            target_centers.append(path)
            require(scripts.count("Common_EventScript_EmeraldChampionsBattleVendor") == 1, f"Battle vendor count wrong in {path.parent.name}")
            require(scripts.count("Common_EventScript_EmeraldChampionsMoveTutor") == 1, f"Move tutor count wrong in {path.parent.name}")
            coordinates = [(obj["x"], obj["y"]) for obj in data["object_events"]]
            require(len(coordinates) == len(set(coordinates)), f"Object overlap in {path.parent.name}")
    require(len(target_centers) == 16, f"Expected 16 serviced Hoenn Centers, found {len(target_centers)}")

    medicine = {"ITEM_POTION", "ITEM_SUPER_POTION", "ITEM_HYPER_POTION", "ITEM_MAX_POTION", "ITEM_FULL_RESTORE"}
    medicine_lists = 0
    paths = [ROOT / "data" / "scripts" / "mart_clerk.inc"]
    paths.extend(path for path in (ROOT / "data" / "maps").glob("*/scripts.inc") if "_Frlg" not in path.parent.name)
    for path in paths:
        lines = path.read_text().splitlines()
        for index, line in enumerate(lines):
            if line.strip() != "pokemartlistend":
                continue
            cursor = index - 1
            listed: set[str] = set()
            while cursor >= 0 and lines[cursor].lstrip().startswith(".2byte ITEM_"):
                listed.add(lines[cursor].strip().split()[-1])
                cursor -= 1
            if listed.intersection(medicine):
                medicine_lists += 1
                require("ITEM_RARE_CANDY" in listed, f"Medicine mart lacks Rare Candy: {path}")
    require(medicine_lists == 20, f"Expected 20 Hoenn medicine lists, found {medicine_lists}")

    free_block = field_specials.split("sEmeraldChampionsFreeBattleItems[]", 1)[1].split("};", 1)[0]
    free_items = set(re.findall(r"ITEM_[A-Z0-9_]+", free_block))
    require(not any("_BERRY" in item for item in free_items), "Berries leaked into the free vendor")

    mega_items = set()
    for match in re.finditer(r"\[(ITEM_[A-Z0-9_]+)\]\s*=\s*\{(.*?)\n\s*\},", items, re.S):
        if "HOLD_EFFECT_MEGA_STONE" in match.group(2):
            mega_items.add(match.group(1))
    require(not free_items.intersection(mega_items), "Mega Stones leaked into the free vendor")
    forbidden_parts = ("_PLATE", "_MEMORY", "_DRIVE", "_MASK", "_Z_CRYSTAL", "TERA_SHARD")
    require(not any(any(part in item for part in forbidden_parts) for item in free_items), "Progression held items leaked into the free vendor")
    require(not free_items.intersection({"ITEM_RED_ORB", "ITEM_BLUE_ORB", "ITEM_RUSTED_SWORD", "ITEM_RUSTED_SHIELD"}), "Transformation items leaked into the free vendor")

    category_names = (
        "sEmeraldChampionsOffenseItems",
        "sEmeraldChampionsDefenseItems",
        "sEmeraldChampionsFieldItems",
        "sEmeraldChampionsTypeItems",
        "sEmeraldChampionsGemItems",
        "sEmeraldChampionsSpeciesItems",
    )
    categories = []
    for name in category_names:
        block = field_specials.split(f"{name}[]", 1)[1].split("};", 1)[0]
        categories.append(set(re.findall(r"ITEM_[A-Z0-9_]+", block)) - {"ITEM_NONE"})
    require(set().union(*categories) == free_items - {"ITEM_NONE"}, "held-item categories do not cover the free vendor exactly")
    require(sum(map(len, categories)) == len(set().union(*categories)), "a held item appears in multiple vendor categories")
    require(
        all(token in vendor_scripts for token in (
            "EmeraldChampions_Text_HeldItems",
            "EmeraldChampions_Text_OffenseItems",
            "EmeraldChampions_Text_DefenseItems",
            "EmeraldChampions_Text_FieldItems",
            "EmeraldChampions_Text_TypeItems",
            "EmeraldChampions_Text_GemItems",
            "EmeraldChampions_Text_SpeciesItems",
        )),
        "the Pokemon Center held-item category menu is incomplete",
    )

    presets = json.loads(read("docs/emerald_champions_battle_sets.json"))
    preset_items = {
        entry[field]
        for group in ("defaults", "alternatives")
        for entry in presets[group]
        for field in ("item", "required_item")
    }
    preset_protected = mega_items | {
        item for item in preset_items
        if any(part in item for part in forbidden_parts)
    } | {"ITEM_RED_ORB", "ITEM_BLUE_ORB", "ITEM_RUSTED_SWORD", "ITEM_RUSTED_SHIELD"}
    preset_berries = {item for item in preset_items if "_BERRY" in item}
    ordinary_preset_items = preset_items - preset_protected - preset_berries - {"ITEM_NONE"}
    require(
        ordinary_preset_items <= free_items,
        f"competitive presets use unavailable ordinary held items: {sorted(ordinary_preset_items - free_items)}",
    )

    print("core_service_static_checks=PASS")
    print(f"pokemon_centers={len(target_centers)}")
    print(f"medicine_mart_lists={medicine_lists}")
    print(f"free_battle_items={len(free_items) - 1}")


if __name__ == "__main__":
    main()
