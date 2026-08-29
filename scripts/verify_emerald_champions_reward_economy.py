#!/usr/bin/env python3
"""Verify Emerald Champions' finite reward economy and one-time world stones."""

from __future__ import annotations

import json
import re
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]

X_ITEMS = {
    "ITEM_X_ATTACK",
    "ITEM_X_DEFEND",
    "ITEM_X_DEFENSE",
    "ITEM_X_SP_ATK",
    "ITEM_X_SPECIAL",
    "ITEM_X_SPEED",
    "ITEM_X_ACCURACY",
    "ITEM_DIRE_HIT",
    "ITEM_GUARD_SPEC",
}

TRAINER_HILL_GRAND_PRIZES = {
    "ITEM_LEVEL_BALL",
    "ITEM_LURE_BALL",
    "ITEM_MOON_BALL",
    "ITEM_FRIEND_BALL",
    "ITEM_LOVE_BALL",
    "ITEM_FAST_BALL",
    "ITEM_HEAVY_BALL",
    "ITEM_DREAM_BALL",
    "ITEM_SPORT_BALL",
    "ITEM_BEAST_BALL",
}

FRONTIER_SUPPLIES = (
    "ITEM_PP_UP",
    "ITEM_PP_MAX",
    "ITEM_MAX_REVIVE",
    "ITEM_SACRED_ASH",
    "ITEM_DREAM_BALL",
    "ITEM_BEAST_BALL",
)

FRONTIER_EVOLUTION_ITEMS = (
    "ITEM_LINKING_CORD",
    "ITEM_PROTECTOR",
    "ITEM_ELECTIRIZER",
    "ITEM_MAGMARIZER",
    "ITEM_REAPER_CLOTH",
    "ITEM_RAZOR_CLAW",
    "ITEM_SWEET_APPLE",
    "ITEM_TART_APPLE",
    "ITEM_PRISM_SCALE",
)

UNIQUE_WORLD_STONE_REPLACEMENTS = {
    "Seaspray_Cave_B1F": "ITEM_SLOWBRONITE",
    "DewfordManor_1F": "ITEM_SABLENITE",
    "EmberPath": "ITEM_BLAZIKENITE",
    "SeafloorCavern_Room9": "ITEM_SHARPEDONITE",
    "Route111_RuinsExterior": "ITEM_STEELIXITE",
    "ScorchedSlab_B2F": "ITEM_CHARIZARDITE_X",
}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def read(relative: str) -> str:
    return (ROOT / relative).read_text()


def c_array(text: str, name: str) -> tuple[str, ...]:
    match = re.search(rf"\b{name}\[\]\s*=\s*\{{(.*?)\n\s*\}};", text, re.S)
    require(match is not None, f"missing C array {name}")
    return tuple(
        item for item in re.findall(r"\bITEM_[A-Z0-9_]+\b", match.group(1))
        if item != "ITEM_LIST_END"
    )


def script_block(text: str, label: str) -> str:
    match = re.search(
        rf"(?ms)^{re.escape(label)}::\n(.*?)(?=^[A-Za-z0-9_]+(?:::|:)\n|\Z)",
        text,
    )
    require(match is not None, f"missing script label {label}")
    return match.group(1)


def verify_trainer_hill() -> None:
    text = read("src/trainer_hill.c")
    lists = re.findall(
        r"static const enum Item sPrizeList[A-Za-z0-9_]+\[\]\s*=\s*\{(.*?)\};",
        text,
    )
    require(len(lists) == 20, f"expected 20 Trainer Hill prize lists, found {len(lists)}")
    first_items = [re.search(r"ITEM_[A-Z0-9_]+", body).group(0) for body in lists]
    require(not any(item.startswith("ITEM_TM_") for body in lists for item in re.findall(r"ITEM_[A-Z0-9_]+", body)),
            "Trainer Hill still awards a redundant TM")
    require(set(first_items[5:10] + first_items[15:20]) == TRAINER_HILL_GRAND_PRIZES,
            "Trainer Hill's ten grand prizes are not the ten scarce rare Balls")


def verify_x_item_cleanup() -> None:
    marts = {
        "data/maps/FallarborTown_Mart/scripts.inc": ("FallarborTown_Mart_Pokemart",),
        "data/maps/LavaridgeTown_Mart/scripts.inc": ("LavaridgeTown_Mart_Pokemart",),
        "data/maps/TrainerHill_Entrance/scripts.inc": (
            "TrainerHill_Entrance_Pokemart_Basic",
            "TrainerHill_Entrance_Pokemart_Expanded",
        ),
    }
    for relative, labels in marts.items():
        text = read(relative)
        for label in labels:
            block = text.split(f"{label}:", 1)[1].split("pokemartlistend", 1)[0]
            listed = set(re.findall(r"ITEM_[A-Z0-9_]+", block))
            require(not listed.intersection(X_ITEMS), f"{label} still stocks unusable X-items")

    route = json.loads(read("data/maps/Route116/map.json"))
    item = next(
        obj["trainer_sight_or_berry_tree_id"]
        for obj in route["object_events"]
        if obj.get("flag") == "FLAG_ITEM_ROUTE_116_X_SPECIAL"
    )
    require(item == "ITEM_THUNDER_STONE", "Route 116's obsolete X Special was not replaced")
    lavaridge = read("data/maps/LavaridgeTown_Mart/scripts.inc")
    require("Use X SPEED" not in lavaridge and "don't allow items" in lavaridge,
            "Lavaridge still teaches an unusable X-item instead of the Bag rule")

    violations: list[str] = []
    campaign_files = [
        path for path in (ROOT / "data/maps").glob("*/*")
        if path.name in {"map.json", "scripts.inc"} and "_Frlg" not in path.parent.name
    ]
    campaign_files.extend(
        path for path in (ROOT / "data/scripts").glob("*.inc")
        if "frlg" not in path.name.lower()
    )
    for path in campaign_files:
        found = X_ITEMS.intersection(re.findall(r"\bITEM_[A-Z0-9_]+\b", path.read_text(errors="ignore")))
        if found:
            violations.append(f"{path.relative_to(ROOT)}: {sorted(found)}")
    require(not violations, "unusable X-items remain in the Hoenn campaign:\n" + "\n".join(violations))


def verify_finite_side_rewards() -> None:
    tent = read("src/battle_tent.c")
    require("static const u16 sFallarborTentRewards[] = {ITEM_PP_MAX};" in tent,
            "Fallarbor Battle Tent still has a disposable medicine prize")

    house = read("data/maps/FallarborTown_MoveRelearnersHouse/scripts.inc")
    for obsolete in ("TeachMoveRelearnerMove", "setmoverelearnerstate", "chooseboxmon"):
        require(obsolete not in house, f"paid Fallarbor move relearner remains: {obsolete}")
    required = (
        "checkitem ITEM_HEART_SCALE, 1",
        "checkitemspace ITEM_PP_UP, 1",
        "removeitem ITEM_HEART_SCALE, 1",
        "giveitem ITEM_PP_UP, 1",
    )
    require(all(line in house for line in required), "Heart Scale to PP Up exchange is incomplete")
    require([house.index(line) for line in required] == sorted(house.index(line) for line in required),
            "Heart Scale exchange can charge before checking reward space")

    roxanne_text = read("data/maps/RustboroCity_Gym/scripts.inc")
    roxanne = roxanne_text.split("RustboroCity_Gym_EventScript_GiveRockTomb::", 1)[1].split(
        "RustboroCity_Gym_EventScript_RoxanneRematch::", 1
    )[0]
    order = (
        "goto_if_set FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE",
        "giveitem ITEM_AERODACTYLITE",
        "setflag FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE",
        "giveitem ITEM_OLD_AMBER",
        "setflag FLAG_RECEIVED_TM_ROCK_TOMB",
    )
    require(all(token in roxanne for token in order), "Roxanne's Aerodactyl project reward is incomplete")
    require([roxanne.index(token) for token in order] == sorted(roxanne.index(token) for token in order),
            "Roxanne's two-item reward is not retry-safe")
    require(
        "goto_if_unset FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE, RustboroCity_Gym_EventScript_GiveLegacyOldAmber"
        in roxanne_text
        and "RustboroCity_Gym_EventScript_GiveLegacyOldAmber::\n\tgiveitem ITEM_OLD_AMBER" in roxanne_text,
        "pre-Old-Amber saves cannot claim Roxanne's added fossil",
    )
    ruins = json.loads(read("data/maps/SandstrewnRuins/map.json"))
    items = {obj.get("trainer_sight_or_berry_tree_id") for obj in ruins["object_events"]}
    require("ITEM_OLD_AMBER" not in items and "ITEM_BLACK_AUGURITE" in items,
            "Sandstrewn still duplicates Roxanne's Old Amber")

    game_corner = read("data/maps/MauvilleCity_GameCorner/scripts.inc")
    for obsolete in (
        "TM_DOUBLE_TEAM_COINS",
        "EventScript_ChooseTMPrizeMessage",
        "EventScript_ConfirmTMPrize",
        "EventScript_CancelTMSelect",
        "Text_SoYourChoiceIsTheTMX",
    ):
        require(obsolete not in game_corner, f"dead Game Corner TM prize path remains: {obsolete}")
    for active in (
        "MauvilleCity_GameCorner_EventScript_PrizeCornerTMs::",
        "MauvilleCity_GameCorner_EventScript_SelectGenesect::",
        "MauvilleCity_GameCorner_EventScript_SelectPoipole::",
        "MauvilleCity_GameCorner_EventScript_PrizeCornerDolls::",
    ):
        require(active in game_corner, f"active Game Corner prize path was lost: {active}")


def verify_frontier_exchange() -> None:
    header = read("src/data/battle_frontier/battle_frontier_exchange_corner.h")
    require(c_array(header, "sFrontierExchangeCorner_Vitamins") == FRONTIER_SUPPLIES,
            "Frontier supply shelf drifted")
    require(c_array(header, "sFrontierExchangeCorner_HoldItems") == FRONTIER_EVOLUTION_ITEMS,
            "Frontier evolution shelf drifted")

    scripts = read("data/maps/BattleFrontier_ExchangeServiceCorner/scripts.inc")
    priced = tuple(
        (item, int(price))
        for item, price in re.findall(r"setitemandprice (ITEM_[A-Z0-9_]+), (\d+)", scripts)
    )
    expected_items = FRONTIER_SUPPLIES + FRONTIER_EVOLUTION_ITEMS
    require(tuple(item for item, _ in priced) == expected_items,
            "Frontier script selection does not match the displayed reward arrays")
    require(all(price > 0 for _, price in priced), "Frontier reward bypasses the BP economy")
    require(not any("_BERRY" in item for item in expected_items), "Frontier exchange makes berries non-scarce")

    field_specials = read("src/field_specials.c")
    free_block = field_specials.split("sEmeraldChampionsFreeBattleItems[]", 1)[1].split("};", 1)[0]
    free_items = set(re.findall(r"ITEM_[A-Z0-9_]+", free_block))
    require(not free_items.intersection(expected_items),
            "Frontier charges BP for an item already free at every Center")
    protected = mega_stone_items() | {"ITEM_RED_ORB", "ITEM_BLUE_ORB"}
    protected_parts = ("_DRIVE", "_MASK", "_MEMORY", "_PLATE")
    require(not protected.intersection(expected_items)
            and not any(any(part in item for part in protected_parts) for item in expected_items),
            "Frontier exchange leaked a protected transformation item")


def mega_stone_items() -> set[str]:
    items = read("src/data/items.h")
    return {
        match.group(1)
        for match in re.finditer(r"\[(ITEM_[A-Z0-9_]+)\]\s*=\s*\{(.*?)\n\s*\},", items, re.S)
        if "HOLD_EFFECT_MEGA_STONE" in match.group(2)
    }


def verify_unique_world_stones() -> None:
    mega_stones = mega_stone_items()
    pickups: list[tuple[str, str, str]] = []
    for path in sorted((ROOT / "data/maps").glob("*/map.json")):
        if path.parent.name.endswith("_Frlg"):
            continue
        payload = json.loads(path.read_text())
        for event in payload.get("object_events", []):
            if event.get("script") != "Common_EventScript_FindItem":
                continue
            item = event.get("trainer_sight_or_berry_tree_id")
            if item in mega_stones:
                pickups.append((path.parent.name, item, event.get("flag", "0")))

    counts = Counter(item for _, item, _ in pickups)
    duplicates = {item: count for item, count in counts.items() if count > 1}
    require(not duplicates, f"duplicate one-time world Mega Stone pickups: {duplicates}")
    flags = [flag for _, _, flag in pickups]
    require(all(flag != "0" for flag in flags), "a world Mega Stone pickup is not one-time")
    require(len(flags) == len(set(flags)), "world Mega Stone pickups share a collection flag")
    by_map = {map_name: item for map_name, item, _ in pickups}
    for map_name, item in UNIQUE_WORLD_STONE_REPLACEMENTS.items():
        require(by_map.get(map_name) == item, f"{map_name} should hold {item}, found {by_map.get(map_name)}")
    print(f"world_mega_stone_pickups={len(pickups)} unique={len(counts)}")


def main() -> None:
    verify_trainer_hill()
    verify_x_item_cleanup()
    verify_finite_side_rewards()
    verify_frontier_exchange()
    verify_unique_world_stones()
    print("PASS: finite reward economy is coherent and one-time world Mega Stones are unique")


if __name__ == "__main__":
    main()
