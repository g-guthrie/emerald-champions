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
    "ITEM_X_SP_DEF",
    "ITEM_X_SPEED",
    "ITEM_X_ACCURACY",
    "ITEM_DIRE_HIT",
    "ITEM_GUARD_SPEC",
}

INERT_VITAMINS = {
    "ITEM_HP_UP",
    "ITEM_PROTEIN",
    "ITEM_IRON",
    "ITEM_CALCIUM",
    "ITEM_ZINC",
    "ITEM_CARBOS",
}

INERT_ACQUISITIONS = X_ITEMS | INERT_VITAMINS

CAPTURE_VENDOR_ITEMS = (
    "ITEM_QUICK_BALL",
    "ITEM_DUSK_BALL",
    "ITEM_TIMER_BALL",
    "ITEM_REPEAT_BALL",
    "ITEM_DIVE_BALL",
    "ITEM_LUXURY_BALL",
)

SLATEPORT_CAPTURE_VENDOR_ITEMS = (
    "ITEM_HEAL_BALL",
    "ITEM_NET_BALL",
    "ITEM_NEST_BALL",
    "ITEM_DIVE_BALL",
    "ITEM_TIMER_BALL",
    "ITEM_REPEAT_BALL",
)

FINITE_ECONOMY_PRIZES = (
    "ITEM_NUGGET",
    "ITEM_STAR_PIECE",
    "ITEM_BIG_PEARL",
    "ITEM_BALM_MUSHROOM",
    "ITEM_RARE_BONE",
    "ITEM_PEARL_STRING",
)

BERRY_POWDER_SUPPLIES = (
    ("ITEM_ETHER", 500),
    ("ITEM_MAX_ETHER", 1000),
    ("ITEM_ELIXIR", 1500),
    ("ITEM_MAX_ELIXIR", 3000),
    ("ITEM_PP_UP", 3000),
    ("ITEM_PP_MAX", 9000),
    ("ITEM_SACRED_ASH", 12000),
)

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
    match = re.search(rf"\b{name}\[\]\s*=\s*\{{(.*?)\}};", text, re.S)
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


def verify_inert_item_cleanup() -> None:
    caps = read("include/config/caps.h")
    require(
        re.search(r"^#define\s+B_EV_CAP_TYPE\s+EV_CAP_NO_GAIN\b", caps, re.M) is not None,
        "reward-economy gate must be revisited if EV gain is re-enabled",
    )

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
            require(not listed.intersection(INERT_ACQUISITIONS),
                    f"{label} still stocks an inert vitamin or unusable X-item")

    route = json.loads(read("data/maps/Route116/map.json"))
    item = next(
        obj["trainer_sight_or_berry_tree_id"]
        for obj in route["object_events"]
        if obj.get("flag") == "FLAG_ITEM_ROUTE_116_THUNDER_STONE"
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
        found = INERT_ACQUISITIONS.intersection(
            re.findall(r"\bITEM_[A-Z0-9_]+\b", path.read_text(errors="ignore"))
        )
        if found:
            violations.append(f"{path.relative_to(ROOT)}: {sorted(found)}")

    # These C tables are acquisition paths outside map scripts: direct facility
    # prizes, Pyramid floor pickups, and the Lilycove Favor Lady's inputs/prize.
    native_reward_files = (
        "src/battle_arena.c",
        "src/battle_palace.c",
        "src/battle_pyramid.c",
        "src/trainer_tower.c",
        "src/data/lilycove_lady.h",
    )
    for relative in native_reward_files:
        found = INERT_ACQUISITIONS.intersection(
            re.findall(r"\bITEM_[A-Z0-9_]+\b", read(relative))
        )
        if found:
            violations.append(f"{relative}: {sorted(found)}")

    require(
        not violations,
        "inert vitamins or unusable X-items remain obtainable in the active game:\n"
        + "\n".join(violations),
    )

    lilycove = read("data/maps/LilycoveCity_DepartmentStore_3F/scripts.inc")
    lilycove_shop = lilycove.split(
        "LilycoveCity_DepartmentStore_3F_Pokemart_CaptureBalls:", 1
    )[1].split("pokemartlistend", 1)[0]
    require(
        tuple(re.findall(r"\bITEM_[A-Z0-9_]+\b", lilycove_shop))
        == ("ITEM_ULTRA_BALL",) + CAPTURE_VENDOR_ITEMS[:4] + ("ITEM_LUXURY_BALL",),
        "Lilycove's former vitamin counter is not a coherent capture counter",
    )

    frontier_mart = read("data/maps/BattleFrontier_Mart/scripts.inc")
    frontier_shop = frontier_mart.split("BattleFrontier_Mart_Pokemart:", 1)[1].split(
        "pokemartlistend", 1
    )[0]
    frontier_items = tuple(re.findall(r"\bITEM_[A-Z0-9_]+\b", frontier_shop))
    require(
        frontier_items[-6:] == CAPTURE_VENDOR_ITEMS,
        "Battle Frontier Mart's former vitamin shelf is not a capture shelf",
    )

    slateport = read("data/maps/SlateportCity/scripts.inc")
    slateport_shop = slateport.split("SlateportCity_Pokemart_CatchingGuru:", 1)[1].split(
        "pokemartlistend", 1
    )[0]
    require(
        tuple(re.findall(r"\bITEM_[A-Z0-9_]+\b", slateport_shop))
        == SLATEPORT_CAPTURE_VENDOR_ITEMS,
        "Slateport's former vitamin vendor is not a coherent capture counter",
    )
    powder_block = slateport.split("SlateportCity_EventScript_EnergyPowder::", 1)[1].split(
        "SlateportCity_EventScript_CancelPowderItemSelect::", 1
    )[0]
    powder_entries = tuple(
        (item, int(price))
        for item, repeated, price in re.findall(
            r"bufferitemname STR_VAR_1, (ITEM_[A-Z0-9_]+)\s+"
            r"setvar VAR_0x8008, (ITEM_[A-Z0-9_]+)\s+"
            r"setvar VAR_0x8009, (\d+)",
            powder_block,
        )
        if item == repeated
    )
    require(
        powder_entries[-7:] == BERRY_POWDER_SUPPLIES,
        "Berry Powder's former vitamins are not coherent endurance supplies",
    )
    require(
        len({item for item, _ in powder_entries}) == len(powder_entries),
        "Berry Powder exchange contains a duplicate item",
    )
    require(
        "Special_AreLeadMonEVsMaxedOut" not in slateport
        and "GiveLeadMonEffortRibbon" not in slateport,
        "Slateport still exposes the unreachable EV-training reward loop",
    )
    for stale_copy in ("PROTEIN", "CALCIUM", "EFFORT RIBBON"):
        require(stale_copy not in slateport, f"Slateport still advertises {stale_copy}")

    stale_copy_files = (
        "data/maps/LilycoveCity_DepartmentStore_3F/scripts.inc",
        "data/maps/BattleFrontier_Mart/scripts.inc",
        "data/maps/SlateportCity_PokemonFanClub/scripts.inc",
        "data/text/pokemon_news.inc",
    )
    for relative in stale_copy_files:
        text = read(relative)
        for stale_copy in ("HP UP", "PROTEIN", "CARBOS", "CALCIUM", "ZINC", "EFFORT RIBBON"):
            require(stale_copy not in text, f"{relative} still advertises {stale_copy}")

    stale_live_symbols = (
        ("data/maps/VerdanturfTown_Mart/scripts.inc", "XSpecialIsCrucial"),
        ("data/maps/VerdanturfTown_Mart/scripts.inc", "NestBallOnWeakenedPokemon"),
        ("data/maps/LavaridgeTown_Mart/scripts.inc", "XSpeedFirstStrike"),
        ("data/maps/SlateportCity_Mart/scripts.inc", "SomeItemsOnlyAtMart"),
        ("data/maps/SlateportCity/map.json", "EffortRibbonWoman"),
        ("data/maps/SlateportCity/scripts.inc", "EffortRibbonWoman"),
    )
    for relative, token in stale_live_symbols:
        require(token not in read(relative), f"{relative} still exposes stale reward symbol {token}")
    verdanturf_mart = read("data/maps/VerdanturfTown_Mart/scripts.inc")
    require(
        "lower-level wild POKéMON" in verdanturf_mart
        and "only place you can" not in verdanturf_mart,
        "Verdanturf still describes Nest Ball as an HP check or exclusive stock",
    )
    mossdeep_mart = read("data/maps/MossdeepCity_Mart/scripts.inc")
    require(
        "only made in MOSSDEEP" not in mossdeep_mart
        and all(phrase in mossdeep_mart for phrase in ("BUG-", "WATER-type", "surfing", "fishing", "underwater")),
        "Mossdeep still claims exclusive Net/Dive Ball stock or describes pre-Gen-4 Dive Ball mechanics",
    )
    overworld_config = read("include/config/overworld.h")
    petalburg_mart = read("data/maps/PetalburgCity_Mart/scripts.inc")
    require("#define OW_POISON_DAMAGE                GEN_LATEST" in overworld_config,
            "poison-dialogue contract must be revisited if overworld poison damage changes")
    require(
        "Poison no longer drains HP while" in petalburg_mart
        and "lose HP until it faints" not in petalburg_mart,
        "Petalburg still describes disabled overworld poison damage",
    )
    pokemon_school = read("data/maps/RustboroCity_PokemonSchool/scripts.inc")
    require(
        "Poison remains after battle, but it no" in pokemon_school
        and "longer causes damage while traveling" in pokemon_school
        and "HP will drop" not in pokemon_school,
        "Rustboro School still teaches disabled overworld poison damage",
    )

    stale_battle_item_copy = (
        "X ATTACK",
        "X DEFEND",
        "X DEFENSE",
        "X SP. ATK",
        "X SPECIAL",
        "X SP. DEF",
        "X SPEED",
        "X ACCURACY",
        "DIRE HIT",
        "GUARD SPEC",
        "HP UP",
        "PROTEIN",
        "CALCIUM",
        "ZINC",
        "CARBOS",
        "EFFORT RIBBON",
        "MACHO BRACE",
        "VITAMIN",
    )
    dialogue_sources = [
        path for path in (ROOT / "data/maps").glob("*/scripts.inc")
        if not path.parent.name.endswith("_Frlg")
    ]
    dialogue_sources.extend(
        path for path in (ROOT / "data/scripts").glob("*.inc")
        if "frlg" not in path.name.lower() and path.name != "debug.inc"
    )
    dialogue_sources.extend(
        path for path in (ROOT / "data/text").glob("*.inc")
        if "frlg" not in path.name.lower() and path.name != "trainers.inc"
    )
    stale_dialogue: list[str] = []
    for path in dialogue_sources:
        for line_number, line in enumerate(path.read_text(errors="ignore").splitlines(), 1):
            if ".string" not in line:
                continue
            upper = line.upper()
            for phrase in stale_battle_item_copy:
                if phrase in upper:
                    stale_dialogue.append(f"{path.relative_to(ROOT)}:{line_number}: {phrase}")
    require(
        not stale_dialogue,
        "live non-trainer dialogue still advertises removed X-item/vitamin/EV rewards:\n"
        + "\n".join(stale_dialogue),
    )

    exclusivity_phrases = (
        "ONLY PLACE",
        "ONLY SHOP",
        "ONLY MADE",
        "MADE ONLY",
        "ONLY SOLD",
        "SOLD ONLY",
        "CAN ONLY GET",
        "BUY ONLY",
        "NOWHERE ELSE",
    )
    commerce_terms = (
        "ITEM",
        "BALL",
        "MART",
        "MARKET",
        "SHOP",
        "BUY",
        "SOLD",
        "STOCK",
        "MERCHANDISE",
        "DECOR",
        "SPECIALTY",
        "GOODS",
        "SUPPL",
    )
    exclusivity_claims: list[str] = []
    for path in dialogue_sources:
        text = path.read_text(errors="ignore")
        labels = list(re.finditer(r"(?m)^([A-Za-z_][A-Za-z0-9_]*):{1,2}\s*$", text))
        for index, match in enumerate(labels):
            block = text[match.end():labels[index + 1].start() if index + 1 < len(labels) else len(text)]
            dialogue = " ".join(re.findall(r'\.string "(.*?)"', block)).upper()
            if (any(phrase in dialogue for phrase in exclusivity_phrases)
                    and any(term in dialogue for term in commerce_terms)):
                exclusivity_claims.append(f"{path.relative_to(ROOT)}:{match.group(1)}")
    require(
        not exclusivity_claims,
        "live commerce dialogue makes an unverified exclusivity claim:\n"
        + "\n".join(exclusivity_claims),
    )

    stale_flag_suffixes = ("_HP_UP", "_PROTEIN", "_IRON", "_CALCIUM", "_ZINC", "_CARBOS")
    for path in (ROOT / "data/maps").glob("*/map.json"):
        if path.parent.name.endswith("_Frlg"):
            continue
        payload = json.loads(path.read_text())
        for section in ("object_events", "bg_events"):
            for event in payload.get(section, []):
                flag = str(event.get("flag", ""))
                require(
                    not flag.endswith(stale_flag_suffixes),
                    f"{path.relative_to(ROOT)} retains stale reward flag {flag}",
                )

    require(
        c_array(read("src/battle_arena.c"), "sShortStreakPrizeItems") == FINITE_ECONOMY_PRIZES,
        "Battle Arena early prizes drifted from finite economy rewards",
    )
    require(
        c_array(read("src/battle_palace.c"), "sBattlePalaceEarlyPrizes") == FINITE_ECONOMY_PRIZES,
        "Battle Palace early prizes drifted from finite economy rewards",
    )
    require(
        c_array(read("src/battle_pyramid.c"), "sShortStreakRewardItems") == FINITE_ECONOMY_PRIZES,
        "Battle Pyramid early prizes drifted from finite economy rewards",
    )
    require(
        c_array(read("src/trainer_tower.c"), "sPrizeList")[:6] == FINITE_ECONOMY_PRIZES,
        "Trainer Tower's first prize tier drifted from finite economy rewards",
    )


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
    roxanne = roxanne_text.split("RustboroCity_Gym_EventScript_GiveRoxanneRewards::", 1)[1].split(
        "RustboroCity_Gym_EventScript_RoxanneRematch::", 1
    )[0]
    order = (
        "goto_if_set FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE",
        "giveitem ITEM_AERODACTYLITE",
        "setflag FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE",
        "giveitem ITEM_OLD_AMBER",
        "setflag FLAG_RECEIVED_ROXANNE_OLD_AMBER",
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
        "MauvilleCity_GameCorner_EventScript_PrizeCornerPokemon::",
        "MauvilleCity_GameCorner_EventScript_SelectGenesect::",
        "MauvilleCity_GameCorner_EventScript_SelectPoipole::",
        "MauvilleCity_GameCorner_EventScript_PrizeCornerDolls::",
    ):
        require(active in game_corner, f"active Game Corner prize path was lost: {active}")


def verify_no_redundant_tm_economy() -> None:
    active_sources = [
        path
        for path in (ROOT / "data/maps").glob("*/scripts.inc")
        if not path.parent.name.endswith("_Frlg")
    ]
    active_sources.extend(
        path
        for path in (ROOT / "data/maps").glob("*/map.json")
        if not path.parent.name.endswith("_Frlg")
    )
    violations = [
        str(path.relative_to(ROOT))
        for path in active_sources
        if re.search(
            r"(?:giveitem|\.2byte|trainer_sight_or_berry_tree_id[^\n]*)\s+(?:\"?)ITEM_TM_",
            path.read_text(),
        )
    ]
    require(
        not violations,
        "a Hoenn acquisition path still sells or gives a redundant TM: " + ", ".join(violations),
    )

    flags = read("include/constants/flags.h")
    require(
        "FLAG_RECEIVED_TM_" not in flags and "FLAG_GOT_TM_" not in flags,
        "a live reward flag still describes a deleted TM reward",
    )

    slateport = read("data/maps/SlateportCity/scripts.inc")
    field_supply_block = slateport.split("SlateportCity_Pokemart_FieldSupplies:", 1)[1].split(
        "pokemartlistend", 1
    )[0]
    require(
        tuple(re.findall(r"ITEM_[A-Z0-9_]+", field_supply_block))
        == (
            "ITEM_HONEY",
            "ITEM_POKE_DOLL",
            "ITEM_FLUFFY_TAIL",
            "ITEM_ESCAPE_ROPE",
            "ITEM_REPEL",
            "ITEM_SUPER_REPEL",
        ),
        "Slateport's deleted TM stall is not a coherent field-supply vendor",
    )

    lilycove = read("data/maps/LilycoveCity_DepartmentStore_4F/scripts.inc")
    require("\tpokemart " not in lilycove and "ITEM_TM_" not in lilycove,
            "Lilycove 4F still exposes a redundant TM shop")
    for concept in ("every move a species may legally learn", "competitive sets", "Mega sets"):
        require(concept in lilycove, f"Lilycove's move-study floor omits {concept!r}")


def verify_frontier_exchange() -> None:
    header = read("src/data/battle_frontier/battle_frontier_exchange_corner.h")
    require(c_array(header, "sFrontierExchangeCorner_Supplies") == FRONTIER_SUPPLIES,
            "Frontier supply shelf drifted")
    require(c_array(header, "sFrontierExchangeCorner_EvolutionItems") == FRONTIER_EVOLUTION_ITEMS,
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
    by_map: dict[str, set[str]] = {}
    for map_name, item, _ in pickups:
        by_map.setdefault(map_name, set()).add(item)
    for map_name, item in UNIQUE_WORLD_STONE_REPLACEMENTS.items():
        require(item in by_map.get(map_name, set()), f"{map_name} should hold {item}, found {sorted(by_map.get(map_name, set()))}")
    print(f"world_mega_stone_pickups={len(pickups)} unique={len(counts)}")


def verify_pickup_flag_names_match_rewards() -> None:
    mismatches: list[str] = []
    checked = 0
    hidden_checked = 0
    legacy_key_aliases = {
        "ITEM_KEY_TO_ROOM_1": "RM_1_KEY",
        "ITEM_KEY_TO_ROOM_2": "RM_2_KEY",
        "ITEM_KEY_TO_ROOM_4": "RM_4_KEY",
        "ITEM_KEY_TO_ROOM_6": "RM_6_KEY",
    }
    for path in sorted((ROOT / "data/maps").glob("*/map.json")):
        if path.parent.name.endswith("_Frlg"):
            continue
        payload = json.loads(path.read_text())
        for event in payload.get("object_events", []):
            if event.get("script") != "Common_EventScript_FindItem":
                continue
            checked += 1
            item = str(event.get("trainer_sight_or_berry_tree_id"))
            flag = str(event.get("flag"))
            if item.removeprefix("ITEM_") not in flag:
                mismatches.append(f"{path.parent.name}: {item} uses {flag}")
        for event in payload.get("bg_events", []):
            if event.get("type") != "hidden_item":
                continue
            hidden_checked += 1
            item = str(event.get("item"))
            flag = str(event.get("flag"))
            expected = legacy_key_aliases.get(item, item.removeprefix("ITEM_"))
            if expected not in flag:
                mismatches.append(f"{path.parent.name}: hidden {item} uses {flag}")
    require(
        not mismatches,
        "world pickup flags still describe deleted rewards:\n" + "\n".join(mismatches),
    )
    print(f"world_pickup_flags={checked} hidden_pickup_flags={hidden_checked} reward_names_match")


def verify_direct_reward_flag_names() -> None:
    generic_state_exceptions = {
        ("PacifidlogTown_House2", "PacifidlogTown_House2_EventScript_GiveReturn"),
        ("PacifidlogTown_House2", "PacifidlogTown_House2_EventScript_GiveFrustration"),
        ("Route111", "Route111_EventScript_Girl"),
        ("RustboroCity", "RustboroCity_EventScript_ReturnGoods"),
        ("RustboroCity_Gym", "RustboroCity_Gym_EventScript_GiveLegacyOldAmber"),
        ("SlateportCity_Harbor", "SlateportCity_Harbor_EventScript_DeepSeaTooth"),
        ("SlateportCity_Harbor", "SlateportCity_Harbor_EventScript_DeepSeaScale"),
    }
    checked = 0
    mismatches: list[str] = []
    for path in sorted((ROOT / "data/maps").glob("*/scripts.inc")):
        if path.parent.name.endswith("_Frlg"):
            continue
        text = path.read_text()
        labels = list(re.finditer(r"(?m)^([A-Za-z_][A-Za-z0-9_]*)::?\s*$", text))
        for index, match in enumerate(labels):
            block = text[match.end():labels[index + 1].start() if index + 1 < len(labels) else len(text)]
            items = re.findall(r"\bgiveitem\s+(ITEM_[A-Z0-9_]+)", block)
            flags = [
                flag
                for flag in re.findall(r"\bsetflag\s+(FLAG_[A-Z0-9_]+)", block)
                if any(token in flag for token in ("RECEIVED", "GOT_", "EXCHANGED", "RETURNED"))
            ]
            if not items or not flags:
                continue
            checked += 1
            if (path.parent.name, match.group(1)) in generic_state_exceptions:
                continue
            for item in items:
                if not any(item.removeprefix("ITEM_") in flag for flag in flags):
                    mismatches.append(
                        f"{path.parent.name}:{match.group(1)} gives {item} but records {flags}"
                    )
    require(
        not mismatches,
        "direct gift flags still describe deleted rewards:\n" + "\n".join(mismatches),
    )
    print(f"direct_reward_flags={checked} reward_names_match")


def main() -> None:
    verify_trainer_hill()
    verify_inert_item_cleanup()
    verify_finite_side_rewards()
    verify_no_redundant_tm_economy()
    verify_frontier_exchange()
    verify_unique_world_stones()
    verify_pickup_flag_names_match_rewards()
    verify_direct_reward_flag_names()
    print("PASS: finite reward economy is coherent and one-time world Mega Stones are unique")


if __name__ == "__main__":
    main()
