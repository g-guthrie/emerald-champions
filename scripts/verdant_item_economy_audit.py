#!/usr/bin/env python3
"""Generate and verify Verdant's source-backed item-economy ledger."""

from __future__ import annotations

import argparse
import json
import re
from collections import defaultdict
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
JSON_PATH = ROOT / "docs/verdant_item_economy_ledger.json"
MD_PATH = ROOT / "docs/verdant_item_economy_ledger.md"


def read(path: str | Path) -> str:
    return (ROOT / path).read_text()


def label_block(path: str, label: str) -> str:
    text = read(path)
    match = re.search(
        rf"^{re.escape(label)}(?:::|:).*?(?=^[A-Za-z0-9_]+(?:::|:)|\Z)",
        text,
        re.M | re.S,
    )
    if not match:
        raise ValueError(f"missing label {label} in {path}")
    return match.group(0)


def item_metadata() -> dict[str, dict]:
    text = read("src/data/items.h")
    starts = list(re.finditer(r"^\s*\[(ITEM_[A-Z0-9_]+)\]\s*=\s*\{", text, re.M))
    result = {}
    for index, match in enumerate(starts):
        block = text[match.start(): starts[index + 1].start() if index + 1 < len(starts) else len(text)]

        def field(pattern: str):
            found = re.search(pattern, block)
            return found.group(1) if found else None

        result[match.group(1)] = {
            "name": field(r'\.name\s*=\s*_\("([^"]+)"\)'),
            "price": int(field(r"\.price\s*=\s*(\d+)") or 0),
            "pocket": field(r"\.pocket\s*=\s*([A-Z0-9_]+)"),
            "holdEffect": field(r"\.holdEffect\s*=\s*([A-Z0-9_]+)"),
        }
    return result


def unlock_table() -> dict[str, int | None]:
    text = read("src/item.c")
    body = re.search(r"sBattleItemUnlocks\[\].*?=\s*\{(.*?)\n\};", text, re.S).group(1)
    result = {}
    for item, minimum in re.findall(r"\{(ITEM_[A-Z0-9_]+),\s*([A-Z0-9_]+)\}", body):
        result[item] = None if minimum == "DISCOVERY_ONLY" else int(minimum)
    return result


def guide_phases() -> tuple[dict[str, dict], dict[int, int]]:
    entries = json.loads(read("docs/verdant_battle_guide.json"))["entries"]
    by_map = {}
    for entry in entries:
        if entry.get("badge") is None or not entry.get("sourceMap"):
            continue
        phase = {
            "order": entry["order"],
            "badge": entry["badge"],
            "cap": entry["levelCap"],
        }
        old = by_map.get(entry["sourceMap"])
        if old is None or phase["order"] < old["order"]:
            by_map[entry["sourceMap"]] = phase
    badge_boss_names = {
        1: "Roxanne",
        2: "Brawly",
        3: "Wattson",
        4: "Flannery",
        5: "Norman",
        6: "Winona",
        7: "Tate&Liza",
        8: "Juan",
    }
    badge_unlock_order = {0: 0}
    for badge, boss_name in badge_boss_names.items():
        matches = [
            entry for entry in entries
            if entry.get("name") == boss_name and entry.get("badge") is not None
        ]
        if matches:
            badge_unlock_order[badge] = min(entry["order"] for entry in matches) + 1
    return by_map, badge_unlock_order


ROOT_PHASE_OVERRIDES = {
    "GraniteCave": {"order": 53, "badge": 1, "cap": 20},
    "DewfordManor": {"order": 53, "badge": 1, "cap": 20},
    "Seaspray_Cave": {"order": 53, "badge": 1, "cap": 20},
    "VerdanturfTown": {"order": 215, "badge": 3, "cap": 40},
    "FallarborTown": {"order": 160, "badge": 3, "cap": 40},
    "LilycoveCity": {"order": 375, "badge": 6, "cap": 60},
    "SouthernIsland": {"order": 520, "badge": 8, "cap": 80},
    "ShoalCave": {"order": 390, "badge": 6, "cap": 60},
}

def phase_for_map(map_name: str, by_map: dict[str, dict]) -> dict | None:
    if map_name in by_map:
        return by_map[map_name]
    root = map_name.split("_", 1)[0]
    candidates = [phase for name, phase in by_map.items() if name.startswith(root)]
    if candidates:
        return min(candidates, key=lambda phase: phase["order"])
    for prefix, phase in ROOT_PHASE_OVERRIDES.items():
        if map_name.startswith(prefix):
            return phase
    return None


def source(kind: str, item: str, path: str, line: int | None, label: str | None,
           map_name: str | None, fixed: bool, phase: dict | None, note: str = "") -> dict:
    return {
        "kind": kind,
        "item": item,
        "path": path,
        "line": line,
        "label": label,
        "map": map_name,
        "fixed": fixed,
        "order": phase["order"] if phase else None,
        "badge": phase["badge"] if phase else None,
        "cap": phase["cap"] if phase else None,
        "note": note,
    }


def collect_sources(unlocks: dict[str, int | None], by_map: dict[str, dict],
                    badge_unlock_order: dict[int, int]) -> tuple[dict[str, list[dict]], list[dict]]:
    sources: dict[str, list[dict]] = defaultdict(list)
    all_map_objects: dict[str, list[tuple[str, str, int]]] = defaultdict(list)
    hidden = []
    for path in (ROOT / "data/maps").rglob("map.json"):
        data = json.loads(path.read_text())
        map_name = path.parent.name
        for obj in data.get("object_events", []):
            if obj.get("script"):
                all_map_objects[obj["script"]].append((map_name, str(path.relative_to(ROOT)), obj.get("flag", "0")))
        for event in data.get("bg_events", []):
            if event.get("type") != "hidden_item":
                continue
            item = event["item"]
            entry = source(
                "hidden_item", item, str(path.relative_to(ROOT)), None, None, map_name,
                True, phase_for_map(map_name, by_map), event.get("flag", ""),
            )
            sources[item].append(entry)
            hidden.append(entry)

    ball_text = read("data/scripts/item_ball_scripts.inc")
    for match in re.finditer(
        r"^([A-Za-z0-9_]+)(?:::|:)\s*(?:@[^\n]*)?\n\s*finditem\s+(ITEM_[A-Z0-9_]+)",
        ball_text,
        re.M,
    ):
        label, item = match.groups()
        line = ball_text[:match.start()].count("\n") + 1
        for map_name, map_path, flag in all_map_objects.get(label, []):
            sources[item].append(source(
                "visible_pickup", item, "data/scripts/item_ball_scripts.inc", line, label,
                map_name, True, phase_for_map(map_name, by_map), f"{map_path}; {flag}",
            ))

    script_paths = list((ROOT / "data/maps").rglob("scripts.inc"))
    for path in script_paths:
        text = path.read_text()
        labels = list(re.finditer(r"^([A-Za-z0-9_]+)(?:::|:).*$", text, re.M))
        for match in re.finditer(r"^\s*(?:giveitem|additem)\s+(ITEM_[A-Z0-9_]+)", text, re.M):
            previous = [label for label in labels if label.start() < match.start()]
            label = previous[-1].group(1) if previous else None
            item = match.group(1)
            map_name = path.parent.name
            sources[item].append(source(
                "fixed_gift", item, str(path.relative_to(ROOT)), text[:match.start()].count("\n") + 1,
                label, map_name, True, phase_for_map(map_name, by_map),
            ))

        for mart in re.finditer(r"^\s*pokemart\s+([A-Za-z0-9_]+)", text, re.M):
            inventory_label = mart.group(1)
            try:
                inventory = label_block(str(path.relative_to(ROOT)), inventory_label)
            except ValueError:
                continue
            for item in re.findall(r"\.2byte\s+(ITEM_[A-Z0-9_]+)", inventory):
                if item == "ITEM_NONE":
                    continue
                map_name = path.parent.name
                sources[item].append(source(
                    "specialty_shop", item, str(path.relative_to(ROOT)),
                    text[:mart.start()].count("\n") + 1, inventory_label, map_name, True,
                    phase_for_map(map_name, by_map),
                ))

    for item, minimum_badges in unlocks.items():
        if minimum_badges is None:
            continue
        order = badge_unlock_order.get(minimum_badges)
        phase = {"order": order, "badge": minimum_badges, "cap": None} if order is not None else None
        sources[item].append(source(
            "badge_shop", item, "src/item.c", None, "sBattleItemUnlocks", None, True, phase,
            f"available with {minimum_badges} badge(s)",
        ))

    bundle_sources = {
        "ITEM_WIDE_LENS": ("GraniteCave_StevensRoom", "Steven reward bundle"),
        "ITEM_SCEPTILITE": ("GraniteCave_StevensRoom", "Steven reward bundle"),
        "ITEM_BLAZIKENITE": ("GraniteCave_StevensRoom", "Steven reward bundle"),
        "ITEM_SWAMPERTITE": ("GraniteCave_StevensRoom", "Steven reward bundle"),
        "ITEM_LATIOSITE": ("SouthernIsland_Interior", "atomic Lati Stone bundle"),
        "ITEM_LATIASITE": ("SouthernIsland_Interior", "atomic Lati Stone bundle"),
        "ITEM_HEAT_ROCK": ("Route119_WeatherInstitute_1F", "atomic weather-rock bundle"),
        "ITEM_DAMP_ROCK": ("Route119_WeatherInstitute_1F", "atomic weather-rock bundle"),
        "ITEM_ICY_ROCK": ("Route119_WeatherInstitute_1F", "atomic weather-rock bundle"),
        "ITEM_SMOOTH_ROCK": ("Route119_WeatherInstitute_1F", "atomic weather-rock bundle"),
    }
    for item, (map_name, note) in bundle_sources.items():
        sources[item].append(source(
            "fixed_bundle", item, "src/item.c", None, None, map_name, True,
            phase_for_map(map_name, by_map), note,
        ))

    base_stats = read("src/data/pokemon/base_stats.h")
    species_starts = list(re.finditer(r"^\s*\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{", base_stats, re.M))
    held_by_species: dict[str, list[tuple[str, str]]] = defaultdict(list)
    for index, match in enumerate(species_starts):
        block = base_stats[match.start(): species_starts[index + 1].start() if index + 1 < len(species_starts) else len(base_stats)]
        for slot, item in re.findall(r"\.(item[12])\s*=\s*(ITEM_[A-Z0-9_]+)", block):
            held_by_species[match.group(1)].append((item, slot))
    wild = json.loads(read("src/data/wild_encounters.json"))["wild_encounter_groups"][0]
    for encounter in wild["encounters"]:
        map_id = encounter.get("map")
        if not map_id:
            continue
        map_name = map_id.removeprefix("MAP_")
        for field, data in encounter.items():
            if not isinstance(data, dict) or "mons" not in data:
                continue
            for mon in data["mons"]:
                for item, slot in held_by_species.get(mon["species"], []):
                    sources[item].append(source(
                        "random_wild_hold", item, "src/data/pokemon/base_stats.h", None,
                        mon["species"], map_name, False, phase_for_map(map_name, by_map),
                        f"{slot}; {field}",
                    ))

    cut_items = [
        "ITEM_MENTAL_HERB", "ITEM_POWER_HERB", "ITEM_WHITE_HERB",
        "ITEM_ELECTRIC_SEED", "ITEM_GRASSY_SEED", "ITEM_MISTY_SEED", "ITEM_PSYCHIC_SEED",
    ]
    for item in cut_items:
        sources[item].append(source(
            "random_cut_drop", item, "data/scripts/field_move_scripts.inc", 72, "EventScript_CutTreeItem",
            None, False, {"order": 28, "badge": 1, "cap": 20}, "available after the Stone Badge",
        ))

    for item in sources:
        sources[item].sort(key=lambda entry: (
            entry["order"] is None,
            entry["order"] if entry["order"] is not None else 1_000_000,
            entry["kind"], entry["path"], entry["line"] or 0,
        ))
    return sources, hidden


BOSS_REWARDS = [
    ("Roxanne", "Roxanne", "ITEM_EXPERT_BELT", "data/maps/RustboroCity_Gym/scripts.inc", "RustboroCity_Gym_EventScript_GiveRockTomb"),
    ("Brawly", "Brawly", "ITEM_FLAME_ORB", "data/maps/DewfordTown_Gym/scripts.inc", "DewfordTown_Gym_EventScript_GiveBulkUp"),
    ("Wattson", "Wattson", "ITEM_WISE_GLASSES", "data/maps/MauvilleCity_Gym/scripts.inc", "MauvilleCity_Gym_EventScript_GiveVoltSwitch"),
    ("Flannery", "Flannery", "ITEM_EJECT_PACK", "data/maps/LavaridgeTown_Gym_1F/scripts.inc", "LavaridgeTown_Gym_1F_EventScript_GiveOverheat"),
    ("Norman", "Norman", "ITEM_TOXIC_ORB", "data/maps/PetalburgCity_Gym/scripts.inc", "PetalburgCity_Gym_EventScript_GiveFacade"),
    ("Winona", "Winona", "ITEM_ADRENALINE_ORB", "data/maps/FortreeCity_Gym/scripts.inc", "FortreeCity_Gym_EventScript_GiveRoost"),
    ("Tate & Liza", "Tate&Liza", "ITEM_LIGHT_CLAY", "data/maps/MossdeepCity_Gym/scripts.inc", "MossdeepCity_Gym_EventScript_GiveCalmMind"),
    ("Juan", "Juan", "ITEM_UTILITY_UMBRELLA", "data/maps/SootopolisCity_Gym_1F/scripts.inc", "SootopolisCity_Gym_1F_EventScript_GiveScald"),
]

KNOWN_DUPLICATE_FIXED_BOSS_REWARDS = set()

PROPOSED_BOSS_DIRECTIONS = {
    "Brawly": ("ITEM_FLAME_ORB", "keep"),
    "Norman": ("ITEM_TOXIC_ORB", "keep"),
    "Tate & Liza": ("ITEM_LIGHT_CLAY", "keep"),
    "Flannery": ("ITEM_EJECT_PACK", "keep"),
    "Winona": ("ITEM_ADRENALINE_ORB", "keep"),
    "Juan": ("ITEM_UTILITY_UMBRELLA", "keep"),
}

EXPECTED_EARLY_CANDY_LABELS = {
    "Route111_EventScript_ItemTM37",
    "Route116_EventScript_TM77StruggleBug",
    "RustboroCity_EventScript_ItemAbilityCapsule",
    "PetalburgWoods_2_Item_TM80Venoshock",
    "PetalburgWoods_3_TM34_SludgeWave",
    "Seaspray_Cave_Stealth_Rock",
    "Seaspray_Cave_ItemStoneEdge",
    "Seaspray_Cave_B1F_ItemFreezeDry",
    "Granite_Cave_B2F_TM31_Brick_Break",
    "GraniteCave_B1F_EventScript_ItemTM65ShadowClaw",
    "DewfordManor_EventScript_TM100Curse",
}


RETRY_CONTRACTS = [
    ("Slowbronite", "data/maps/ShoalCave_LowTideEntranceRoom/scripts.inc", "ShoalCave_LowTideEntranceRoom_EventScript_Slowbronite", ["compare VAR_RESULT, FALSE", "setflag FLAG_SHOALCAVE_SLOWBRONITE"]),
    ("Galladite", "data/maps/FallarborTown_CozmosHouse/scripts.inc", "FallarborTown_CozmosHouse_EventScript_GiveGalladite", ["compare VAR_RESULT, FALSE", "setflag FLAG_RECEIVED_GALLADITE"]),
    ("Gyaradosite", "data/maps/Route118/scripts.inc", "Route118_EventScript_GiveGyaradosite", ["compare VAR_RESULT, FALSE", "setflag FLAG_ROUTE118_GYARADOSITE"]),
    ("Gardevoirite", "data/maps/VerdanturfTown_WandasHouse/scripts.inc", "VerdanturfTown_WandasHouse_EventScript_WandaGardevoirite", ["compare VAR_RESULT, FALSE", "setflag FLAG_WANDA_GARDEVOIRITE"]),
    ("Altarianite", "data/maps/LilycoveCity/scripts.inc", "LilycoveCity_EventScript_YesAltaria", ["compare VAR_RESULT, FALSE", "setflag FLAG_ITEM_LILYCOVE_CITY_ALTARIANITE"]),
    ("Lati Stone bundle", "data/maps/SouthernIsland_Interior/scripts.inc", "SouthernIsland_Interior_EventScript_GiveLatiStones", ["special TryGiveVerdantLatiStoneBundle", "compare VAR_RESULT, FALSE", "setflag FLAG_RECEIVED_LATI_STONES"]),
    ("Lucy bundle", "data/maps/LavaridgeTown_PokemonCenter_1F/scripts.inc", "LavaridgeTown_PokemonCenter_1F_EventScript_LucyReward", ["checkitemspace ITEM_BLACK_SLUDGE, 1", "checkitemspace ITEM_BOTTLE_CAP, 3", "setvar VAR_LAVARIDGE_LUCY_STATE, 2"]),
    ("Spenser bundle", "data/maps/FortreeCity_Mart/scripts.inc", "FortreeCity_Mart_EventScript_SpenserReward", ["checkitemspace ITEM_TERRAIN_EXTENDER, 1", "checkitemspace ITEM_BOTTLE_CAP, 3"]),
    ("Greta bundle", "data/maps/SlateportCity/scripts.inc", "SlateportCity_EventScript_GretaReward", ["checkitemspace ITEM_THROAT_SPRAY, 1", "checkitemspace ITEM_BOTTLE_CAP, 6", "setflag FLAG_DEFEATED_SLATEPORT_GRETA"]),
    ("Weather Rock bundle", "data/maps/Route119_WeatherInstitute_1F/scripts.inc", "Route119_WeatherInstitute_1F_EventScript_LittleBoy", ["special TryGiveVerdantWeatherRockBundle", "compare VAR_RESULT, FALSE", "setflag FLAG_WEATHER_INSTITUTE_ROCKS"]),
    ("Float Stone", "data/maps/RustboroCity_Flat2_2F/scripts.inc", "RustboroCity_Flat2_2F_EventScript_GiveFloatStone", ["compare VAR_RESULT, FALSE", "setflag FLAG_ITEM_RUSTBORO_FLOAT_STONE"]),
    ("Silver Powder", "data/maps/VerdanturfTown_PokemonCenter_1F/scripts.inc", "VerdanturfTown_PokemonCenter_1F_EventScript_XscissorTM", ["compare VAR_RESULT, FALSE", "setflag FLAG_VERDANT_GIFT_X_SCISSOR"]),
    ("May goggles", "data/maps/LavaridgeTown/scripts.inc", "LavaridgeTown_EventScript_MayGiveGoGoggles", ["checkitemspace ITEM_GO_GOGGLES, 1", "checkitemspace ITEM_SAFETY_GOGGLES, 1"]),
    ("Brendan goggles", "data/maps/LavaridgeTown/scripts.inc", "LavaridgeTown_EventScript_BrendanGiveGoGoggles", ["checkitemspace ITEM_GO_GOGGLES, 1", "checkitemspace ITEM_SAFETY_GOGGLES, 1"]),
]


DIALOGUE_CONTRACTS = [
    ("data/maps/RustboroCity_Mart/scripts.inc", "RustboroCity_Mart_Text_HaveTM98", "Zoom Lens", ["False Swipe"]),
    ("data/maps/Route114_FossilManiacsHouse/scripts.inc", "Route114_FossilManiacsHouse_Text_DigReturnsYouToEntrance", "Air Balloon", ["returned to the entrance"]),
    ("data/maps/FortreeCity_House2/scripts.inc", "FortreeCity_Text_HaveTM49", "Safety Goggles", ["Sleep Talk"]),
    ("data/maps/FortreeCity_House2/scripts.inc", "FortreeCity_House2_Text_ExplainHiddenPower", "Wide Lens", ["Hidden Power is a move"]),
    ("data/maps/DewfordTown_Hall/scripts.inc", "DewfordTown_Hall_Text_GiveYouSludgeBomb", "Black Sludge", ["Sludge Bomb"]),
    ("data/maps/SSTidalRooms/scripts.inc", "SSTidalRooms_Text_ExplainSnatch", "Safety Goggles", ["Snatch steals"]),
    ("data/maps/SlateportCity_PokemonFanClub/scripts.inc", "SlateportCity_PokemonFanClub_Text_HaveTM58", "Focus Band", ["Endure allows"]),
    ("data/maps/PacifidlogTown_PokemonCenter_1F/scripts.inc", "PacifidlogTown_PokemonCenter_1F_Text_HaveExplosion", "Room Service", ["taught Pokémon", "use Explosion"]),
    ("data/maps/LavaridgeTown_Gym_1F/scripts.inc", "LavaridgeTown_Gym_1F_Text_ExplainOverheat", "Eject Pack", ["Flame Orb"]),
    ("data/maps/Route114/scripts.inc", "Route114_Text_ExplainRoar", "Shed Shell", ["Eject Pack"]),
    ("data/maps/Route111_WinstrateFamilysHouse/scripts.inc", "Route111_WinstrateFamilysHouse_Text_LikeYouToHaveLifeOrb", "Destiny Knot", ["Toxic Orb"]),
    ("data/maps/FortreeCity_Gym/scripts.inc", "FortreeCity_Gym_Text_ExplainRoost", "Adrenaline Orb", ["Safety Goggles"]),
    ("data/maps/SootopolisCity_Gym_1F/scripts.inc", "SootopolisCity_Gym_1F_Text_ExplainScald", "Utility Umbrella", ["Weakness Policy"]),
]


def validate_contracts() -> list[str]:
    problems = []
    for name, path, label, required in RETRY_CONTRACTS:
        block = label_block(path, label)
        for token in required:
            if token not in block:
                problems.append(f"{name}: retry contract lost {token}")
    for path, label, required, forbidden in DIALOGUE_CONTRACTS:
        block = label_block(path, label)
        if required not in block:
            problems.append(f"{label}: dialogue no longer names {required}")
        for token in forbidden:
            if token in block:
                problems.append(f"{label}: stale dialogue still contains {token}")
    migration_contracts = {
        "include/constants/flags.h": (
            "FLAG_EMERALD_CHAMPIONS_MIGRATED_GYM_REWARDS  FLAG_UNUSED_0x91E",
            "FLAG_EMERALD_CHAMPIONS_MIGRATED_ITEM_BALLS   FLAG_UNUSED_0x91F",
        ),
        "src/new_game.c": (
            "FlagSet(FLAG_EMERALD_CHAMPIONS_MIGRATED_GYM_REWARDS);",
            "FlagSet(FLAG_EMERALD_CHAMPIONS_MIGRATED_ITEM_BALLS);",
        ),
        "src/save.c": (
            "{FLAG_RECEIVED_TM05, ITEM_SHED_SHELL, FLAG_VERDANT_MIGRATED_SHED_SHELL}",
            "TryAddEmeraldChampionsGymRewardMigration()",
            "TryAddEmeraldChampionsItemBallMigration()",
        ),
        "src/item.c": (
            "bool8 TryAddEmeraldChampionsGymRewardMigration(void)",
            "bool8 TryAddEmeraldChampionsItemBallMigration(void)",
            "FlagGet(FLAG_ITEM_VICTORY_ROAD_B1F_TM_29)",
            "FlagGet(FLAG_TM93_WILD_CHARGE)",
        ),
    }
    for path, tokens in migration_contracts.items():
        text = read(path)
        for token in tokens:
            if token not in text:
                problems.append(f"{path}: save migration contract lost {token}")
    return problems


def compact_source(entry: dict) -> dict:
    return {key: entry[key] for key in (
        "kind", "path", "line", "label", "map", "fixed", "order", "badge", "cap", "note"
    )}


def build_ledger() -> tuple[dict, list[str]]:
    metadata = item_metadata()
    unlocks = unlock_table()
    by_map, badge_unlock_order = guide_phases()
    sources, hidden = collect_sources(unlocks, by_map, badge_unlock_order)
    problems = validate_contracts()
    unlock_order = list(unlocks)
    if unlock_order[-1:] != ["ITEM_UTILITY_UMBRELLA"] or unlock_order.index("ITEM_UTILITY_UMBRELLA") != 54:
        problems.append("Utility Umbrella must remain append-only at discovery index 54")
    if (
        "if (index == 54)\n        return FLAG_VERDANT_BATTLE_ITEM_UTILITY_UMBRELLA;" not in read("src/item.c")
        or "FLAG_VERDANT_BATTLE_ITEM_UTILITY_UMBRELLA    FLAG_UNUSED_0x91D" not in read("include/constants/flags.h")
    ):
        problems.append("Utility Umbrella discovery flag no longer preserves existing unlock-bit meanings")

    items = []
    relevant_items = sorted(
        item for item in sources | unlocks.keys() | {item for item, _ in PROPOSED_BOSS_DIRECTIONS.values()}
        if metadata.get(item, {}).get("pocket") in ("POCKET_BATTLE", "POCKET_MEGA_STONES")
    )
    for item in relevant_items:
        fixed_sources = [entry for entry in sources.get(item, []) if entry["fixed"]]
        known_fixed = [entry for entry in fixed_sources if entry["order"] is not None]
        first_fixed = min(known_fixed, key=lambda entry: entry["order"]) if known_fixed else (fixed_sources[0] if fixed_sources else None)
        if item in unlocks and not fixed_sources:
            problems.append(f"{item}: unlock-table item has no fixed acquisition or badge source")
        items.append({
            "item": item,
            **metadata.get(item, {}),
            "minimumBadges": unlocks.get(item, "not_in_discovery_shop"),
            "firstFixedSource": compact_source(first_fixed) if first_fixed else None,
            "sources": [compact_source(entry) for entry in sources.get(item, [])],
        })

    boss_rows = []
    duplicate_bosses = set()
    guide_entries = json.loads(read("docs/verdant_battle_guide.json"))["entries"]
    boss_phases = {}
    for boss, guide_name, *_ in BOSS_REWARDS:
        matches = [
            entry for entry in guide_entries
            if entry.get("name") == guide_name and entry.get("badge") is not None
        ]
        if not matches:
            problems.append(f"{boss}: no canonical campaign row in generated battle guide")
            continue
        boss_phases[boss] = min(matches, key=lambda entry: entry["order"])

    for boss, _guide_name, item, path, label in BOSS_REWARDS:
        if boss not in boss_phases:
            continue
        order = boss_phases[boss]["order"]
        badge = boss_phases[boss]["badge"]
        earlier = [
            entry for entry in sources.get(item, [])
            if entry["fixed"] and entry["order"] is not None and entry["order"] < order
        ]
        if earlier:
            duplicate_bosses.add(boss)
        boss_rows.append({
            "boss": boss,
            "guideOrder": order,
            "badgesBeforeBattle": badge,
            "item": item,
            "source": {"path": path, "label": label},
            "earlierFixedSources": [compact_source(entry) for entry in earlier],
            "duplicateFixedReward": bool(earlier),
        })
    if duplicate_bosses != KNOWN_DUPLICATE_FIXED_BOSS_REWARDS:
        problems.append(
            "fixed boss duplicate set changed: expected "
            f"{sorted(KNOWN_DUPLICATE_FIXED_BOSS_REWARDS)}, observed {sorted(duplicate_bosses)}"
        )

    boss_order = {boss: phase["order"] for boss, phase in boss_phases.items()}
    proposals = []
    for boss, (item, action) in PROPOSED_BOSS_DIRECTIONS.items():
        order = boss_order[boss]
        earlier = [
            compact_source(entry) for entry in sources.get(item, [])
            if entry["fixed"] and entry["order"] is not None and entry["order"] < order
        ]
        proposals.append({
            "boss": boss,
            "item": item,
            "actionNeeded": action,
            "inDiscoveryShop": item in unlocks,
            "earlierFixedSources": earlier,
            "readyWithoutRelocation": not earlier and (item in unlocks or boss == "Brawly"),
        })

    ball_text = read("data/scripts/item_ball_scripts.inc")
    candy_labels = []
    candy_source_by_label = {
        entry["label"]: entry
        for entry in sources.get("ITEM_RARE_CANDY", [])
        if entry["kind"] == "visible_pickup" and entry["label"]
    }
    for match in re.finditer(
        r"^([A-Za-z0-9_]+)(?:::|:)\s*(?:@[^\n]*)?\n\s*finditem\s+ITEM_RARE_CANDY",
        ball_text,
        re.M,
    ):
        label = match.group(1)
        pickup_source = candy_source_by_label.get(label)
        candy_labels.append({
            "label": label,
            "line": ball_text[:match.start()].count("\n") + 1,
            "formerTechnicalReward": True,
            "map": pickup_source["map"] if pickup_source else None,
            "order": pickup_source["order"] if pickup_source else None,
            "badge": pickup_source["badge"] if pickup_source else None,
            "cap": pickup_source["cap"] if pickup_source else None,
            "retention": "intentional early leveling convenience",
        })
    observed_candy_labels = {row["label"] for row in candy_labels}
    if observed_candy_labels != EXPECTED_EARLY_CANDY_LABELS:
        problems.append(
            "intentional early Rare Candy set changed: expected "
            f"{sorted(EXPECTED_EARLY_CANDY_LABELS)}, observed {sorted(observed_candy_labels)}"
        )
    hidden_candy = [entry for entry in hidden if entry["item"] == "ITEM_RARE_CANDY"]

    ledger = {
        "meta": {
            "version": 2,
            "source": "current ROM source plus docs/verdant_battle_guide.json",
            "itemCount": len(items),
            "unlockTableCount": len(unlocks),
            "knownDuplicateFixedBossRewardCount": len(duplicate_bosses),
            "retryContractCount": len(RETRY_CONTRACTS),
            "dialogueContractCount": len(DIALOGUE_CONTRACTS),
            "saveMigrationContractCount": 4,
        },
        "bossRewards": boss_rows,
        "testedBossRewardDirections": proposals,
        "rareCandyExploration": {
            "visiblePickupCount": len(candy_labels),
            "formerTechnicalRewardCount": sum(row["formerTechnicalReward"] for row in candy_labels),
            "hiddenCount": len(hidden_candy),
            "policy": "The eleven visible sources are intentionally retained at early campaign gates; mid and late former-technical filler has been replaced with finite rewards.",
            "visiblePickups": candy_labels,
            "hiddenSources": [compact_source(entry) for entry in hidden_candy],
        },
        "items": items,
    }
    return ledger, problems


def markdown(ledger: dict) -> str:
    lines = [
        "# Emerald Champions item-economy ledger",
        "",
        "This report is generated from current ROM source and the generated battle guide by",
        "`python3 scripts/verdant_item_economy_audit.py --write`.",
        "",
        "## Fixed boss rewards",
        "",
        "| Boss | Current item | Earlier fixed source? | Status |",
        "|---|---|---|---|",
    ]
    for row in ledger["bossRewards"]:
        earlier = row["earlierFixedSources"]
        summary = ", ".join(
            f"{entry['map'] or entry['path']} ({entry['kind']})" for entry in earlier[:3]
        ) or "None"
        status = "DESIGN REVIEW" if row["duplicateFixedReward"] else "Clean"
        lines.append(f"| {row['boss']} | {row['item']} | {summary} | {status} |")

    lines.extend([
        "",
        "## Tested canonical direction",
        "",
        "| Boss | Proposed item | Earlier collision | Required action |",
        "|---|---|---|---|",
    ])
    for row in ledger["testedBossRewardDirections"]:
        collision = ", ".join(
            f"{entry['map'] or entry['path']}" for entry in row["earlierFixedSources"][:3]
        ) or "None"
        lines.append(f"| {row['boss']} | {row['item']} | {collision} | {row['actionNeeded']} |")

    candy = ledger["rareCandyExploration"]
    lines.extend([
        "",
        "## Exploration summary",
        "",
        f"- Visible Rare Candy pickups: **{candy['visiblePickupCount']}**.",
        f"- Former technical-reward Rare Candy pickups: **{candy['formerTechnicalRewardCount']}**.",
        f"- Hidden Rare Candy pickups: **{candy['hiddenCount']}**.",
        f"- {candy['policy']}",
        "",
        "| Retained label | Area | Guide order | Badges |",
        "|---|---|---:|---:|",
    ])
    for row in candy["visiblePickups"]:
        lines.append(
            f"| {row['label']} | {row['map'] or 'Unknown'} | "
            f"{row['order'] if row['order'] is not None else 'Unknown'} | "
            f"{row['badge'] if row['badge'] is not None else 'Unknown'} |"
        )
    lines.extend([
        "",
        "## First fixed acquisition",
        "",
        "| Item | Shop gate | First fixed source |",
        "|---|---:|---|",
    ])
    for row in ledger["items"]:
        first = row["firstFixedSource"]
        if first:
            where = first["map"] or first["path"]
            first_text = f"{where} / {first['kind']}"
        else:
            first_text = "None"
        gate = row["minimumBadges"]
        gate_text = "discovery" if gate is None else str(gate)
        lines.append(f"| {row['item']} | {gate_text} | {first_text} |")
    lines.append("")
    return "\n".join(lines)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true", help="write generated ledger artifacts")
    parser.add_argument("--check", action="store_true", help="verify generated artifacts (default)")
    args = parser.parse_args()
    ledger, problems = build_ledger()
    json_text = json.dumps(ledger, indent=2) + "\n"
    md_text = markdown(ledger)

    if args.write:
        JSON_PATH.write_text(json_text)
        MD_PATH.write_text(md_text)
    else:
        for path, expected in ((JSON_PATH, json_text), (MD_PATH, md_text)):
            if not path.exists() or path.read_text() != expected:
                problems.append(f"generated artifact is stale: {path.relative_to(ROOT)}")

    if problems:
        raise SystemExit("\n".join(f"FAIL: {problem}" for problem in problems))

    duplicates = [row["boss"] for row in ledger["bossRewards"] if row["duplicateFixedReward"]]
    print(f"PASS: {ledger['meta']['retryContractCount']} retry-safe reward contracts")
    print(f"PASS: {ledger['meta']['dialogueContractCount']} stale-dialogue contracts")
    print(f"PASS: {ledger['meta']['saveMigrationContractCount']} save-migration contract groups")
    print(f"PASS: {ledger['meta']['unlockTableCount']} discovery-shop items have fixed acquisition records")
    if duplicates:
        print(f"REVIEW: fixed boss reward duplicates remain for {', '.join(duplicates)}")
    else:
        print("PASS: fixed boss rewards have no earlier fixed source")
    print(
        "PASS: "
        f"{ledger['rareCandyExploration']['formerTechnicalRewardCount']} intentional early technical-reward Rare Candy pickups are explicitly gated"
    )
    print("PASS: Emerald Champions item-economy ledger is source-backed and current")


if __name__ == "__main__":
    main()
