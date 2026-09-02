#!/usr/bin/env python3
"""Rebuild every Hoenn surf and fishing table, and place the species lines that
were unobtainable anywhere, in src/data/wild_encounters.json.

Why
---
The land tables were designed (12 unique species per map, 1% hunts, themed
dungeons). The water layer was not: 158 surf tables collapsed to 72 distinct
ones, 30 of them literally "Tentacool x3", Magikarp sat in 97 maps, and early
fishing spots served Lv 30-45 fully evolved Pokémon at a Lv 14 cap.

What this does
--------------
* Every Hoenn map with water gets a surf table (5 slots: 60/30/5/4/1 %) and a
  fishing table (Old Rod 2, Good Rod 3, Super Rod 5 slots) drawn from one of
  four sea regions - west shelf, inland fresh water, east sea, deep sea - so
  the coast a route belongs to is readable from what you pull out of it.
* Adjacent routes rotate through their region's pool so neighbours differ, and
  each region's 1% Super Rod / surf slot is a real hunt (Dratini, Feebas,
  Lapras, Relicanth, Dondozo + Tatsugiri, Wiglett, Cetoddle ...).
* Species are promoted to their evolved form only where the route's cap makes
  that legal (evolution level + 5), so no table ever contradicts the
  evolution-floor gate.
* Table levels are the route's design level for that access tier (Old Rod 20,
  Good Rod 30, Super Rod 60, Surf 55). The runtime clamp in wild_encounter.c
  guarantees nothing spawns above the live cap regardless.
* 40 land-dwelling species lines that had no acquisition route at all are
  placed into region-appropriate land tables, replacing the most
  over-represented slot in that table; Route 121 gets two mid-tier anchors.

The script is idempotent and never touches Kanto/FRLG maps or Legendary Sign
slots.
"""
from __future__ import annotations

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
WILD = ROOT / "src" / "data" / "wild_encounters.json"
MASTER = ROOT / "docs" / "emerald_champions_master_battle_design.txt"
SPECIES_H = ROOT / "include" / "constants" / "species.h"
SIGNS = ROOT / "src" / "data" / "pokemon" / "legendary_signs.h"
ROUTE_SHEET = ROOT / "docs" / "wild_route_sheet.json"


def load_route_sheet() -> dict[str, dict]:
    sheet = json.loads(ROUTE_SHEET.read_text())
    return {k: v for k, v in sheet.items() if k.startswith("MAP_")}

KANTO = re.compile(r"ISLAND|FRLG|CERULEAN|CELADON|CINNABAR|FUCHSIA|PALLET|VERMILION|VIRIDIAN|SSANNE|SEAFOAM|_ROUTE([1-9]|1[0-9]|2[0-9])(_|$)|SAFARI_ZONE_NORTH_FRLG|POKEMON_TOWER|POWER_PLANT|MT_MOON|MT_EMBER|DIGLETTS_CAVE|VIRIDIAN_FOREST|ROCK_TUNNEL|CERULEAN_CAVE|BERRY_FOREST|ICEFALL|LOST_CAVE|TANOBY|TRAINER_TOWER|PATTERN_BUSH|ALTERING_CAVE$|OUTCAST|RUIN_VALLEY|GREEN_PATH|WATER_PATH|MEMORIAL_PILLAR|RESORT_GORGEOUS|WATER_LABYRINTH|KINDLE_ROAD|TREASURE_BEACH|CAPE_BRINK|BOND_BRIDGE|SEVAULT")

SURF_PCT = [60, 30, 5, 4, 1]
FISH_TIERS = [("old", 2, 20), ("good", 3, 30), ("super", 5, 60)]
SURF_TIER_LEVEL = 55

# Access-tier levels are the *design* levels. Runtime clamps to the live cap.
# Region pools: (common..., ) then rare hunts. Names are checked against
# species.h and silently dropped if this build lacks them.
REGIONS = {
    "west": {
        "surf": ["WINGULL", "TENTACOOL", "SHELLOS", "WAILMER", "STARYU", "KRABBY", "CORSOLA", "MANTYKE",
                 "CLAMPERL", "FINNEON", "FRILLISH", "ARROKUDA", "BINACLE", "WISHIWASHI", "PYUKUMUKU"],
        "surf_rare": ["WIGLETT", "MANTYKE", "CORSOLA"],
        "fish": ["MAGIKARP", "REMORAID", "HORSEA", "WISHIWASHI", "CLAUNCHER", "SKRELP", "CARVANHA",
                 "LUVDISC", "BRUXISH", "KRABBY", "SHELLDER", "STARYU", "QWILFISH", "CLAMPERL"],
        "fish_rare": ["WIGLETT", "FEEBAS", "DHELMISE", "MAREANIE"],
    },
    "inland": {
        "surf": ["POLIWAG", "LOTAD", "SURSKIT", "MARILL", "WOOPER", "PSYDUCK", "DUCKLETT", "BUIZEL",
                 "TYMPOLE", "SLOWPOKE", "CHEWTLE", "BARBOACH", "CORPHISH", "GOLDEEN", "BASCULIN", "SHELLOS"],
        "surf_rare": ["DRATINI", "CHEWTLE", "FEEBAS"],
        "fish": ["MAGIKARP", "GOLDEEN", "BARBOACH", "CORPHISH", "BASCULIN", "CHINCHOU", "POLIWAG",
                 "TYMPOLE", "CHEWTLE", "CARVANHA", "REMORAID", "PSYDUCK", "LOTAD", "WOOPER"],
        "fish_rare": ["DRATINI", "FEEBAS", "DHELMISE"],
    },
    "east": {
        "surf": ["WAILMER", "WINGULL", "TENTACOOL", "MANTYKE", "CARVANHA", "SPHEAL", "CRAMORANT",
                 "FRILLISH", "ALOMOMOLA", "MAREANIE", "PYUKUMUKU", "WISHIWASHI", "CLAMPERL", "SEEL", "STARYU",
                 "FINNEON", "FINIZEN"],
        "surf_rare": ["LAPRAS", "TAUROS_PALDEA_AQUA", "FINIZEN", "ALOMOMOLA"],
        "fish": ["CHINCHOU", "HORSEA", "REMORAID", "QWILFISH", "LUVDISC", "CORSOLA", "SKRELP", "CLAUNCHER",
                 "KRABBY", "SHELLDER", "BRUXISH", "ARROKUDA", "MAGIKARP", "WAILMER", "CARVANHA"],
        "fish_rare": ["RELICANTH", "LAPRAS", "DHELMISE", "FEEBAS"],
    },
    "deep": {
        "surf": ["WAILMER", "TENTACOOL", "WINGULL", "MANTYKE", "FRILLISH", "CARVANHA", "MAGIKARP",
                 "WISHIWASHI", "ALOMOMOLA", "CRAMORANT", "SPHEAL", "PYUKUMUKU", "SEEL", "CLAMPERL", "FINIZEN", "FINNEON"],
        "surf_rare": ["DONDOZO", "LAPRAS", "FINIZEN", "GYARADOS"],
        "fish": ["CHINCHOU", "QWILFISH", "REMORAID", "HORSEA", "RELICANTH", "SKRELP", "CLAUNCHER",
                 "SHELLDER", "ARROKUDA", "BRUXISH", "CLAMPERL", "LUVDISC", "MAGIKARP", "KRABBY", "QWILFISH_HISUI"],
        "fish_rare": ["TATSUGIRI", "DONDOZO", "BASCULEGION", "LAPRAS", "RELICANTH"],
    },
    "ice": {
        "surf": ["SPHEAL", "SEEL", "CLAMPERL", "TENTACOOL", "WAILMER", "FRILLISH", "SHELLDER", "CETODDLE", "BERGMITE", "SNOM"],
        "surf_rare": ["CETODDLE", "LAPRAS"],
        "fish": ["CHINCHOU", "SHELLDER", "QWILFISH_HISUI", "CLAMPERL", "RELICANTH", "HORSEA", "MAGIKARP", "SPHEAL", "SEEL", "REMORAID", "LUVDISC"],
        "fish_rare": ["CETODDLE", "QWILFISH_HISUI", "LAPRAS", "RELICANTH"],
    },
}

REGION_OF = {
    "MAP_PETALBURG_CITY": "west", "MAP_ROUTE103": "west", "MAP_ROUTE104": "west", "MAP_ROUTE105": "west",
    "MAP_ROUTE106": "west", "MAP_ROUTE107": "west", "MAP_ROUTE108": "west", "MAP_ROUTE109": "west",
    "MAP_DEWFORD_TOWN": "west", "MAP_SLATEPORT_CITY": "west", "MAP_ROUTE110": "west", "MAP_ROUTE115": "west",
    "MAP_ROUTE102": "inland", "MAP_ROUTE111": "inland", "MAP_ROUTE114": "inland", "MAP_ROUTE117": "inland",
    "MAP_ROUTE118": "inland", "MAP_ROUTE119": "inland", "MAP_ROUTE120": "inland", "MAP_ROUTE121": "inland",
    "MAP_ROUTE123": "inland", "MAP_METEOR_FALLS_1F_1R": "inland", "MAP_METEOR_FALLS_1F_2R": "inland",
    "MAP_METEOR_FALLS_B1F_1R": "inland", "MAP_METEOR_FALLS_B1F_2R": "inland", "MAP_PETALBURG_WOODS_3": "inland",
    "MAP_SAFARI_ZONE_CENTER": "inland", "MAP_SAFARI_ZONE_EAST": "inland", "MAP_SAFARI_ZONE_NORTHWEST": "inland",
    "MAP_SAFARI_ZONE_SOUTHEAST": "inland", "MAP_SAFARI_ZONE_SOUTHWEST": "inland", "MAP_SAFARI_ZONE_WEST": "inland",
    "MAP_SANDSTREWN_RUINS": "inland", "MAP_SCORCHED_SLAB_B1F": "inland", "MAP_SEASPRAY_CAVE": "west",
    "MAP_ALTERING_CAVE_B1F": "inland", "MAP_VICTORY_ROAD_B2F": "inland", "MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS": "east",
    "MAP_ABANDONED_SHIP_ROOMS_B1F": "east", "MAP_ROUTE122": "east", "MAP_ROUTE124": "east", "MAP_ROUTE125": "east",
    "MAP_ROUTE126": "east", "MAP_ROUTE127": "east", "MAP_ROUTE128": "east", "MAP_LILYCOVE_CITY": "east",
    "MAP_MOSSDEEP_CITY": "east", "MAP_EVER_GRANDE_CITY": "east", "MAP_ROUTE129": "deep", "MAP_ROUTE130": "deep",
    "MAP_ROUTE131": "deep", "MAP_ROUTE132": "deep", "MAP_ROUTE133": "deep", "MAP_ROUTE134": "deep",
    "MAP_PACIFIDLOG_TOWN": "deep", "MAP_SOOTOPOLIS_CITY": "deep", "MAP_SEAFLOOR_CAVERN_ENTRANCE": "deep",
    "MAP_SEAFLOOR_CAVERN_ROOM6": "deep", "MAP_SEAFLOOR_CAVERN_ROOM7": "deep",
    "MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM": "ice", "MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM": "ice",
}

# Caps for maps that have no master encounter of their own.
EXTRA_CAPS = {
    "MAP_PETALBURG_CITY": 14, "MAP_DEWFORD_TOWN": 20, "MAP_PETALBURG_WOODS_3": 14, "MAP_ROUTE122": 60,
    "MAP_MOSSDEEP_CITY": 60, "MAP_EVER_GRANDE_CITY": 80, "MAP_PACIFIDLOG_TOWN": 70, "MAP_SOOTOPOLIS_CITY": 70,
    "MAP_METEOR_FALLS_B1F_1R": 60, "MAP_METEOR_FALLS_B1F_2R": 60, "MAP_SAFARI_ZONE_CENTER": 55,
    "MAP_SAFARI_ZONE_EAST": 55, "MAP_SAFARI_ZONE_NORTHWEST": 55, "MAP_SAFARI_ZONE_SOUTHEAST": 55,
    "MAP_SAFARI_ZONE_SOUTHWEST": 55, "MAP_SAFARI_ZONE_WEST": 55, "MAP_SANDSTREWN_RUINS": 30,
    "MAP_SCORCHED_SLAB_B1F": 55, "MAP_SEAFLOOR_CAVERN_ENTRANCE": 70, "MAP_SEAFLOOR_CAVERN_ROOM6": 70,
    "MAP_SEAFLOOR_CAVERN_ROOM7": 70, "MAP_SEASPRAY_CAVE": 30, "MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM": 60,
    "MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM": 60, "MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS": 55,
    "MAP_ABANDONED_SHIP_ROOMS_B1F": 55, "MAP_ALTERING_CAVE_B1F": 100, "MAP_VICTORY_ROAD_B2F": 80,
}

# Land placements for lines that had no acquisition route: species -> map.
LAND_PLACEMENTS = {
    "LECHONK": "MAP_ROUTE102", "NYMBLE": "MAP_ROUTE102", "PIDOVE": "MAP_ROUTE104", "YUNGOOS": "MAP_ROUTE104",
    "PIKIPEK": "MAP_ROUTE104", "PATRAT": "MAP_ROUTE103", "TAROUNTULA": "MAP_PETALBURG_WOODS",
    "SEWADDLE": "MAP_PETALBURG_WOODS", "BLIPBUG": "MAP_PETALBURG_WOODS", "SKWOVET": "MAP_ROUTE116",
    "PURRLOIN": "MAP_ROUTE116", "THROH": "MAP_ROUTE116", "SNUBBULL": "MAP_ROUTE115", "SAWK": "MAP_ROUTE115",
    "CLEFFA": "MAP_ROUTE115", "MINCCINO": "MAP_ROUTE117", "GLAMEOW": "MAP_ROUTE117", "FOMANTIS": "MAP_ROUTE117",
    "PETILIL": "MAP_ROUTE117", "IGGLYBUFF": "MAP_ROUTE117", "STUNKY": "MAP_ROUTE110", "SHROODLE": "MAP_ROUTE110",
    "VAROOM": "MAP_ROUTE110", "SPOINK": "MAP_ROUTE113", "MASCHIFF": "MAP_ROUTE111", "RELLOR": "MAP_ROUTE111",
    "BRAMBLIN": "MAP_ROUTE111", "SKIDDO": "MAP_ROUTE114", "KLAWF": "MAP_ROUTE112", "TOEDSCOOL": "MAP_ROUTE119",
    "HOPPIP": "MAP_ROUTE104", "EKANS": "MAP_ROUTE121", "TRUBBISH": "MAP_ROUTE110", "GOTHITA": "MAP_ROUTE120",
    "AUDINO": "MAP_ROUTE117", "STUFFUL": "MAP_ROUTE114", "MANKEY": "MAP_ROUTE112", "KRICKETOT": "MAP_ROUTE102",
    "MARACTUS": "MAP_ROUTE111", "NICKIT": "MAP_ROUTE116", "PANCHAM": "MAP_ROUTE115",
    "MORELULL": "MAP_ROUTE120", "FLAMIGO": "MAP_ROUTE123", "CETODDLE": "MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM",
    "GOSSIFLEUR": "MAP_ROUTE117", "TANDEMAUS": "MAP_ROUTE117", "FARFETCHD": "MAP_ROUTE115",
    # Route 121 mid-tier anchors (it read as a base-form foyer at campaign order 290)
    "SKUNTANK": "MAP_ROUTE121", "TOEDSCRUEL": "MAP_ROUTE121", "MABOSSTIFF": "MAP_ROUTE121",
}

# Older source-first gates protect these exact slots because they carry a
# campaign promise (no wild starters, reachable Unown for Hoopa, fossils only
# through revival). Missing-line placement must never evict them.
PINNED_REPLACEMENTS = {
    ("MAP_ROUTE101", "land_mons", 4): "SPECIES_PIDGEY",
    ("MAP_ROUTE103", "land_mons", 4): "SPECIES_GROWLITHE",
    ("MAP_ROUTE117", "land_mons", 6): "SPECIES_EXEGGCUTE",
    ("MAP_ROUTE117", "land_mons", 7): "SPECIES_PONYTA",
    ("MAP_FIERY_PATH", "land_mons", 7): "SPECIES_HOUNDOUR",
    ("MAP_SANDSTREWN_RUINS", "land_mons", 8): "SPECIES_UNOWN",
    ("MAP_MIRAGE_TOWER_1F", "land_mons", 3): "SPECIES_GOLETT",
    ("MAP_MIRAGE_TOWER_1F", "land_mons", 6): "SPECIES_SIGILYPH",
    ("MAP_SANDSTREWN_RUINS", "land_mons", 6): "SPECIES_GREAT_TUSK",
    ("MAP_SANDSTREWN_RUINS_2F", "land_mons", 6): "SPECIES_KROKOROK",
    ("MAP_SANDSTREWN_RUINS_3F", "land_mons", 6): "SPECIES_SANDACONDA",
}


def species_exists(name: str, available: set[str]) -> bool:
    return f"SPECIES_{name}" in available


def load_species_data() -> tuple[set[str], dict[str, list[tuple[str, int]]], dict[str, str]]:
    available = set(re.findall(r"\b(SPECIES_[A-Z0-9_]+)\b", SPECIES_H.read_text()))
    evolutions: dict[str, list[tuple[str, int]]] = {}
    pre: dict[str, str] = {}
    for path in (ROOT / "src" / "data" / "pokemon" / "species_info").glob("gen_*_families.h"):
        text = path.read_text()
        for match in re.finditer(r"\[(SPECIES_[A-Z0-9_]+)\] =\s*\{(.*?)\n    \},", text, re.S):
            species, body = match.group(1), match.group(2)
            for method, param, target in re.findall(r"\{(EVO_[A-Z_]+),\s*([A-Z0-9_]+),\s*(SPECIES_[A-Z0-9_]+)", body):
                level = int(param) if method == "EVO_LEVEL" and param.isdigit() else 0
                evolutions.setdefault(species, []).append((target, level))
                pre.setdefault(target, species)
    return available, evolutions, pre


def caps_by_map() -> dict[str, int]:
    caps: dict[str, int] = {}
    text = MASTER.read_text()
    for block in re.split(r"(?m)^=== ENCOUNTER \d{4} ===$", text)[1:]:
        location = re.search(r"location: (\S+)", block).group(1).rstrip(";")
        cap = int(re.search(r"strict_cap: (\d+)", block).group(1))
        key = "MAP_" + re.sub(r"(?<=[a-z])(?=[A-Z])", "_", location).upper()
        caps.setdefault(key, cap)
    caps.update(EXTRA_CAPS)
    for map_id, row in load_route_sheet().items():
        if "cap" in row:
            caps[map_id] = row["cap"]
    return caps


NEVER_PROMOTE = {"MAGIKARP"}


def promote(species: str, level: int, evolutions, available) -> str:
    """Return the most evolved form legal at `level` (evolution level + 5)."""
    current = f"SPECIES_{species}"
    if species in NEVER_PROMOTE:
        return current
    while True:
        options = [
            (target, evo_level) for target, evo_level in evolutions.get(current, [])
            if evo_level and evo_level + 5 <= level and target in available and "_" not in target.replace("SPECIES_", "")
        ]
        if not options:
            return current
        current = max(options, key=lambda item: item[1])[0]


def rotate(pool: list[str], offset: int, count: int) -> list[str]:
    picked: list[str] = []
    index = offset
    while len(picked) < count and len(picked) < len(pool):
        candidate = pool[index % len(pool)]
        if candidate not in picked:
            picked.append(candidate)
        index += 1
    return picked


def build_water(region: dict, offset: int, cap: int, evolutions, available, row: dict | None = None) -> dict:
    level = max(cap, SURF_TIER_LEVEL)
    pool = [s for s in region["surf"] if species_exists(s, available)]
    rare = [s for s in region["surf_rare"] if species_exists(s, available)]
    anchors = [a for a in (row or {}).get("surf_anchors", []) if species_exists(a, available)][:2]
    commons = anchors + [s for s in rotate(pool, offset, 6) if s not in anchors][: 4 - len(anchors)]
    hunt = (row or {}).get("surf_hunt")
    if not hunt or not species_exists(hunt, available):
        hunt = rare[offset % len(rare)]
    if hunt in commons:
        hunt = next((r for r in rare if r not in commons), hunt)
    mons = []
    for i, name in enumerate(commons + [hunt]):
        # slot 0 (60%) stays a base form so every line is catchable; the hunt stays as written
        promoted = name if i in (0, 4) else promote(name, level, evolutions, available).replace("SPECIES_", "")
        mons.append({"min_level": max(2, level - 5), "max_level": level, "species": f"SPECIES_{promoted}"})
    return {"encounter_rate": 4, "mons": mons}


def build_fishing(region: dict, offset: int, cap: int, evolutions, available, row: dict | None = None) -> dict:
    pool = [s for s in region["fish"] if species_exists(s, available)]
    rare = [s for s in region["fish_rare"] if species_exists(s, available)]
    anchors = [a for a in (row or {}).get("fish_anchors", []) if species_exists(a, available)][:2]
    authored_hunt = (row or {}).get("fish_hunt")
    mons = []
    used: list[str] = ["MAGIKARP"]
    for tier_index, (_tier, count, tier_level) in enumerate(FISH_TIERS):
        level = max(cap, tier_level) if _tier != "old" else min(max(cap, tier_level), 30)
        names = rotate([s for s in pool if s not in used], offset + tier_index * 3, count)
        if _tier == "old":
            # the Old Rod always pulls Magikarp first, then the route's first anchor
            names = ["MAGIKARP"] + ([anchors[0]] if anchors else names[:1])
        elif _tier == "good" and len(anchors) > 1:
            names = [anchors[1]] + [n for n in names if n != anchors[1]][:2]
        if len(names) < count:  # small pools (ice): allow repeats rather than short tables
            names += rotate([s for s in pool if s not in names], offset, count - len(names))
        if _tier == "super":
            hunt = authored_hunt if authored_hunt and species_exists(authored_hunt, available) else rare[offset % len(rare)]
            if hunt in used or hunt in names:
                hunt = next((r for r in rare if r not in used and r not in names), hunt)
            names = names[:4] + [hunt]
        used.extend(names)
        for i, name in enumerate(names):
            if (_tier == "super" and i == 4) or _tier == "old":
                promoted = name
            else:
                promoted = promote(name, level, evolutions, available).replace("SPECIES_", "")
            mons.append({"min_level": max(2, level - 5), "max_level": level, "species": f"SPECIES_{promoted}"})
    return {"encounter_rate": 30, "mons": mons}


def place_land(encounters: list[dict], available, caps, evolutions) -> list[str]:
    by_map: dict[str, dict] = {}
    for entry in encounters:
        if KANTO.search(entry["map"]):
            continue
        by_map.setdefault(entry["map"], entry)
    frequency: dict[str, int] = {}
    for entry in by_map.values():
        land = entry.get("land_mons")
        if land:
            for mon in {m["species"] for m in land["mons"]}:
                frequency[mon] = frequency.get(mon, 0) + 1
    sign_species = set("SPECIES_" + m for m in re.findall(r"WILD_SIGN\([A-Z_]+, ([A-Z0-9_]+),", SIGNS.read_text()))
    placed: list[str] = []
    for name, map_id in LAND_PLACEMENTS.items():
        species = f"SPECIES_{name}"
        if species not in available or map_id not in by_map or not by_map[map_id].get("land_mons"):
            continue
        land = by_map[map_id]["land_mons"]["mons"]
        if any(m["species"] == species for m in land):
            continue
        # Replace the most over-represented species in this table (never a Sign species,
        # never the 1% slots which are curated hunts).
        protected = sign_species | {f"SPECIES_{n}" for n in LAND_PLACEMENTS}
        pinned_slots = {
            slot for (pinned_map, method, slot), _species in PINNED_REPLACEMENTS.items()
            if pinned_map == map_id and method == "land_mons"
        }
        candidates = [
            (frequency.get(m["species"], 0), i) for i, m in enumerate(land[:10])
            if m["species"] not in protected and i not in pinned_slots
        ]
        if not candidates:
            continue
        _count, index = max(candidates)
        old = land[index]["species"]
        land[index]["species"] = species
        frequency[old] = frequency.get(old, 1) - 1
        frequency[species] = frequency.get(species, 0) + 1
        placed.append(f"{map_id}: {old} -> {species}")
    return placed


def restore_pinned_replacements(encounters: list[dict]) -> list[str]:
    """Restore source-of-truth slot contracts before filling missing lines."""
    by_map = {entry["map"]: entry for entry in encounters}
    fixes: list[str] = []
    for (map_id, method, slot), species in PINNED_REPLACEMENTS.items():
        table = by_map.get(map_id, {}).get(method)
        expected = f"SPECIES_{species}" if not species.startswith("SPECIES_") else species
        if table is None or slot >= len(table["mons"]):
            continue
        mon = table["mons"][slot]
        if mon["species"] != expected:
            fixes.append(f"{map_id}/{method}[{slot}]: {mon['species']} -> {expected} (pinned)")
            mon["species"] = expected
    return fixes


CAVE_BAT_ALTERNATES = ["NOIBAT", "WOOBAT", "GLIGAR", "SWABLU"]

# Friendship evolutions encode EVO_LEVEL with parameter 0, so the simple
# numeric parser below cannot infer the engine's reviewed encounter floor.
MINIMUM_WILD_LEVELS = {
    "SPECIES_PERSIAN_ALOLA": 28,
}

# The eight Seafloor Cavern rooms inherited the same Noibat slot. Keep one as
# the cave bat and give the other rooms distinct pressure-cave residents.
SEAFLOOR_CAVE_VARIANTS = {
    "MAP_SEAFLOOR_CAVERN_ROOM1": "WOOBAT",
    "MAP_SEAFLOOR_CAVERN_ROOM2": "ZUBAT",
    "MAP_SEAFLOOR_CAVERN_ROOM3": "SABLEYE",
    "MAP_SEAFLOOR_CAVERN_ROOM4": "CARBINK",
    "MAP_SEAFLOOR_CAVERN_ROOM5": "TYNAMO",
    "MAP_SEAFLOOR_CAVERN_ROOM6": "GLIGAR",
    "MAP_SEAFLOOR_CAVERN_ROOM7": "MAWILE",
    "MAP_SEAFLOOR_CAVERN_ROOM8": "DEWPIDER",
}


def fix_evolution_floors(encounters: list[dict], caps, available) -> list[str]:
    """Any evolved form listed below its evolution level is either raised to that
    level (when the map's cap allows) or replaced by its pre-evolution."""
    evo_level: dict[str, int] = {}
    pre: dict[str, str] = {}
    for path in (ROOT / "src" / "data" / "pokemon" / "species_info").glob("gen_*_families.h"):
        for match in re.finditer(r"\[(SPECIES_[A-Z0-9_]+)\] =\s*\{(.*?)\n    \},", path.read_text(), re.S):
            for method, param, target in re.findall(r"\{(EVO_[A-Z_]+),\s*([A-Z0-9_]+),\s*(SPECIES_[A-Z0-9_]+)", match.group(2)):
                if method == "EVO_LEVEL" and param.isdigit():
                    evo_level[target] = min(evo_level.get(target, 1000), int(param))
                pre.setdefault(target, match.group(1))
    evo_level.update(MINIMUM_WILD_LEVELS)
    fixes: list[str] = []
    seen: set[str] = set()
    bat_toggle = 0
    for entry in encounters:
        map_id = entry["map"]
        if KANTO.search(map_id) or map_id in seen:
            continue
        seen.add(map_id)
        cap = caps.get(map_id, 55)
        for area in ("land_mons", "rock_smash_mons", "hidden_mons"):
            table = entry.get(area)
            if not table:
                continue
            # Physical encounter levels never exceed the route's cap + 2; the
            # runtime clamp is the safety net, the table is the design.
            for mon in table["mons"]:
                mon["max_level"] = min(mon["max_level"], cap + 2)
                mon["min_level"] = min(mon["min_level"], mon["max_level"])
            present = {m["species"] for m in table["mons"]}
            for mon in table["mons"]:
                species = mon["species"]
                if species == "SPECIES_GOLBAT" and area == "land_mons":
                    bat_toggle += 1
                    if bat_toggle % 2 == 0:
                        for alt in CAVE_BAT_ALTERNATES:
                            if f"SPECIES_{alt}" in available and f"SPECIES_{alt}" not in present:
                                mon["species"] = f"SPECIES_{alt}"
                                present.add(mon["species"])
                                fixes.append(f"{map_id}: GOLBAT -> {alt} (de-flood)")
                                break
                        species = mon["species"]
                floor = evo_level.get(species)
                if not floor or mon["min_level"] >= floor:
                    continue
                if floor <= cap:
                    mon["min_level"] = floor
                    mon["max_level"] = max(mon["max_level"], floor)
                    fixes.append(f"{map_id}: {species} raised to Lv {floor}")
                else:
                    base = species
                    while base in pre and evo_level.get(base, 0) > cap:
                        base = pre[base]
                    if base != species and base not in present:
                        mon["species"] = base
                        present.add(base)
                        base_floor = evo_level.get(base)
                        if base_floor and mon["min_level"] < base_floor:
                            mon["min_level"] = base_floor
                            mon["max_level"] = max(mon["max_level"], base_floor)
                        fixes.append(f"{map_id}: {species} -> {base} (cap {cap})")
                    else:
                        pool = next((v for k, v in THEMED_FILLERS.items() if k in map_id), []) + DEDUPE_FILLERS
                        for alt in pool:
                            candidate = f"SPECIES_{alt}"
                            if candidate in available and candidate not in present:
                                mon["species"] = candidate
                                present.add(candidate)
                                mon["min_level"] = min(mon["min_level"], cap)
                                mon["max_level"] = min(mon["max_level"], cap)
                                fixes.append(f"{map_id}: {species} -> {candidate} (over cap, pre-evolution present)")
                                break
    return fixes


def diversify_seafloor_caves(encounters: list[dict], available: set[str]) -> list[str]:
    fixes: list[str] = []
    for entry in encounters:
        replacement = SEAFLOOR_CAVE_VARIANTS.get(entry["map"])
        table = entry.get("land_mons")
        if replacement is None or table is None or f"SPECIES_{replacement}" not in available:
            continue
        present = {mon["species"] for mon in table["mons"]}
        if f"SPECIES_{replacement}" in present:
            continue
        for mon in table["mons"]:
            if mon["species"] == "SPECIES_NOIBAT":
                mon["species"] = f"SPECIES_{replacement}"
                fixes.append(f'{entry["map"]}: NOIBAT -> {replacement} (seafloor identity)')
                break
    return fixes


THEMED_FILLERS = {
    "EMBER": ["SLUGMA", "NUMEL", "TORKOAL", "HOUNDOUR", "SALANDIT", "CHARCADET", "LITWICK", "MAGBY"],
    "SCORCHED": ["SLUGMA", "NUMEL", "TORKOAL", "HOUNDOUR", "SALANDIT", "CHARCADET", "LITWICK", "MAGBY"],
    "MAGMA": ["SLUGMA", "NUMEL", "TORKOAL", "HOUNDOUR", "SALANDIT", "CHARCADET", "LITWICK", "MAGBY"],
    "VICTORY": ["LAIRON", "SNEASEL", "GLIGAR", "SABLEYE", "MAWILE", "CARBINK", "LARVITAR", "DEINO"],
    "METEOR": ["SWABLU", "BAGON", "GIBLE", "DEINO", "GOOMY", "JANGMO_O", "DRATINI", "AXEW"],
}
DEDUPE_FILLERS = ["WOOBAT", "GLIGAR", "SABLEYE", "MAWILE", "ARON", "CARBINK", "DODUO", "GIRAFARIG", "WOBBUFFET",
                  "PINSIR", "HERACROSS", "RHYHORN", "PHANPY", "NATU", "SWABLU", "NUZLEAF", "KECLEON", "TROPIUS",
                  "SKARMORY", "NUMEL", "SPINDA", "ABSOL", "CHINGLING", "BRONZOR", "LARVITAR", "SNEASEL", "MEDITITE"]


def dedupe_land(encounters: list[dict], available) -> list[str]:
    fixes = []
    seen: set[str] = set()
    for entry in encounters:
        map_id = entry["map"]
        if KANTO.search(map_id) or map_id in seen:
            continue
        seen.add(map_id)
        land = entry.get("land_mons")
        if not land:
            continue
        present: set[str] = set()
        for mon in land["mons"]:
            if mon["species"] in present:
                for alt in DEDUPE_FILLERS:
                    candidate = f"SPECIES_{alt}"
                    if candidate in available and candidate not in present:
                        fixes.append(f"{map_id}: duplicate {mon['species']} -> {candidate}")
                        mon["species"] = candidate
                        break
            present.add(mon["species"])
    return fixes


def main() -> None:
    data = json.loads(WILD.read_text())
    available, evolutions, _pre = load_species_data()
    caps = caps_by_map()
    sheet = load_route_sheet()
    hoenn = [g for g in data["wild_encounter_groups"] if g.get("label") == "gWildMonHeaders"][0]
    seen: set[str] = set()
    rebuilt = 0
    for entry in hoenn["encounters"]:
        map_id = entry["map"]
        if KANTO.search(map_id) or map_id in seen:
            continue
        if not (entry.get("water_mons") or entry.get("fishing_mons")):
            continue
        seen.add(map_id)
        row = sheet.get(map_id)
        if row is None:
            print(f"WARNING: {map_id} has water but no row in docs/wild_route_sheet.json; using region defaults")
        region_name = (row or {}).get("region") or REGION_OF.get(map_id, "inland")
        region = REGIONS[region_name]
        cap = caps.get(map_id, 55)
        offset = sum(ord(c) for c in map_id) % 7 + len(seen)
        if entry.get("water_mons"):
            entry["water_mons"] = build_water(region, offset, cap, evolutions, available, row)
        if entry.get("fishing_mons"):
            entry["fishing_mons"] = build_fishing(region, offset, cap, evolutions, available, row)
        rebuilt += 1
    fixes = restore_pinned_replacements(hoenn["encounters"])
    placed = place_land(hoenn["encounters"], available, caps, evolutions)
    fixes += fix_evolution_floors(hoenn["encounters"], caps, available)
    fixes += dedupe_land(hoenn["encounters"], available)
    fixes += diversify_seafloor_caves(hoenn["encounters"], available)
    fixes += fix_evolution_floors(hoenn["encounters"], caps, available)
    WILD.write_text(json.dumps(data, indent=2) + "\n")
    print(f"evolution-floor / de-flood fixes: {len(fixes)}")
    print(f"rebuilt water/fishing tables for {rebuilt} Hoenn maps")
    print(f"placed {len(placed)} land species:")
    for line in placed:
        print("  " + line)


if __name__ == "__main__":
    main()
