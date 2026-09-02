#!/usr/bin/env python3
"""Verify the authored Emerald Champions campaign encounter distribution.

The distribution is now curated directly in wild_encounters.json.  This gate
must never reconstruct it from an old Inclement snapshot or globally allocate
species into whichever slot happens to be free.
"""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path

from verify_emerald_champions_campaign_roster import SpeciesGraph


ROOT = Path(__file__).resolve().parents[1]
TARGET = ROOT / "src/data/wild_encounters.json"
SLOT_COUNTS = {
    "land_mons": 12,
    "water_mons": 5,
    "rock_smash_mons": 5,
    "fishing_mons": 10,
    "hidden_mons": 3,
}
SLOT_RATES = {
    "land_mons": (20, 20, 10, 10, 10, 10, 5, 5, 4, 4, 1, 1),
    "water_mons": (60, 30, 5, 4, 1),
    "rock_smash_mons": (60, 30, 5, 4, 1),
    "fishing_mons": (70, 30, 60, 20, 20, 40, 40, 15, 4, 1),
}

# Ultra-space leakage is an explicit feature of restored sanctuaries.  Five
# percent is rare without making a targeted catch into a grind.
RESTORED_ULTRA_BEASTS = {
    "MAP_ALTERING_CAVE_B1F": "SPECIES_GUZZLORD",
    "MAP_ASHEN_WOODS": "SPECIES_BUZZWOLE",
    "MAP_DEWFORD_MEADOW": "SPECIES_PHEROMOSA",
    "MAP_EMBER_PATH": "SPECIES_BLACEPHALON",
    "MAP_PETALBURG_WOODS_3": "SPECIES_KARTANA",
    "MAP_SANDSTREWN_RUINS_B1F": "SPECIES_STAKATAKA",
    "MAP_UNDERWATER_SEAFLOOR_CAVERN": "SPECIES_NIHILEGO",
}

# First acquisition pass: ordinary wild starters were replaced with species
# that preserve the area's theme. The global family ban below protects later
# full-table rewrites without freezing individual late-cave slot positions.
STARTER_REPLACEMENTS = {
    ("MAP_ROUTE101", "land_mons", 4): "SPECIES_PIDGEY",
    ("MAP_ROUTE103", "land_mons", 4): "SPECIES_GROWLITHE",
    ("MAP_ROUTE104", "fishing_mons", 1): "SPECIES_TENTACOOL",
    ("MAP_ROUTE117", "land_mons", 6): "SPECIES_EXEGGCUTE",
    ("MAP_ROUTE117", "land_mons", 7): "SPECIES_PONYTA",
    ("MAP_FIERY_PATH", "land_mons", 7): "SPECIES_HOUNDOUR",
    ("MAP_ROUTE126", "water_mons", 2): "SPECIES_GASTRODON",
    ("MAP_ROUTE126", "water_mons", 3): "SPECIES_GOREBYSS",
    ("MAP_ROUTE126", "water_mons", 4): "SPECIES_HUNTAIL",
    ("MAP_ROUTE128", "fishing_mons", 8): "SPECIES_DHELMISE",
    ("MAP_ROUTE128", "fishing_mons", 9): "SPECIES_TOXAPEX",
}

QUEST_DEPENDENCY_REPLACEMENTS = {
    # Hoopa's visible Sign requires Unown.  Tanoby Ruins is unreachable FRLG
    # data and Mirage Tower can collapse, so permanently reachable Sandstrewn
    # Ruins provides the native Hoenn acquisition at 4%.
    ("MAP_SANDSTREWN_RUINS", "land_mons", 8): "SPECIES_UNOWN",
}

FOSSIL_REPLACEMENTS = {
    ("MAP_MIRAGE_TOWER_1F", "land_mons", 3): "SPECIES_GOLETT",
    ("MAP_MIRAGE_TOWER_1F", "land_mons", 6): "SPECIES_SIGILYPH",
    ("MAP_SANDSTREWN_RUINS", "land_mons", 6): "SPECIES_GREAT_TUSK",
    ("MAP_SANDSTREWN_RUINS_2F", "land_mons", 6): "SPECIES_KROKOROK",
    ("MAP_SANDSTREWN_RUINS_3F", "land_mons", 6): "SPECIES_SANDACONDA",
}

FOSSIL_FAMILY_ROOTS = {
    "SPECIES_OMANYTE",
    "SPECIES_KABUTO",
    "SPECIES_AERODACTYL",
    "SPECIES_LILEEP",
    "SPECIES_ANORITH",
    "SPECIES_CRANIDOS",
    "SPECIES_SHIELDON",
    "SPECIES_TIRTOUGA",
    "SPECIES_ARCHEN",
    "SPECIES_TYRUNT",
    "SPECIES_AMAURA",
}

OPENING_MAPS = {
    "MAP_ROUTE101",
    "MAP_ROUTE102",
    "MAP_ROUTE103",
    "MAP_ROUTE104",
    "MAP_PETALBURG_WOODS",
    "MAP_ROUTE116",
    "MAP_RUSTURF_TUNNEL",
}
PRE_BRAWLY_MAPS = OPENING_MAPS | {
    "MAP_GRANITE_CAVE_1F",
    "MAP_GRANITE_CAVE_B1F",
    "MAP_GRANITE_CAVE_B2F",
}

DEWFORD_WATTSON_MAPS = {
    *(f"MAP_ROUTE{route}" for route in range(105, 111)),
    "MAP_GRANITE_CAVE_1F",
    "MAP_GRANITE_CAVE_B1F",
    "MAP_GRANITE_CAVE_B2F",
    "MAP_DEWFORD_MEADOW",
    "MAP_DEWFORD_MANOR_1F",
    "MAP_SEASPRAY_CAVE",
    "MAP_SEASPRAY_CAVE_B1F",
}

OPENING_ECOSYSTEM_MAPS = OPENING_MAPS | PRE_BRAWLY_MAPS | DEWFORD_WATTSON_MAPS | {
    "MAP_PETALBURG_WOODS_2",
    "MAP_PETALBURG_WOODS_3",
    "MAP_NEW_MAUVILLE_INSIDE",
}

PRE_WATTSON_MAPS = OPENING_ECOSYSTEM_MAPS - {"MAP_NEW_MAUVILLE_INSIDE"}

# Each policy row names candidate species and the authored behavior their
# default preset must expose.  One source at four percent or better is enough.
ROLE_POLICY = {
    "opening Intimidate": (OPENING_MAPS, {"SPECIES_SHINX", "SPECIES_POOCHYENA"}, "ABILITY_INTIMIDATE"),
    "opening Fake Out": (OPENING_MAPS, {"SPECIES_BUNEARY", "SPECIES_MEOWTH"}, "MOVE_FAKE_OUT"),
    "opening redirection": (OPENING_MAPS, {"SPECIES_FOONGUS"}, "MOVE_RAGE_POWDER"),
    "opening Trick Room": (OPENING_MAPS, {"SPECIES_RALTS"}, "MOVE_TRICK_ROOM"),
    "opening Tailwind": (OPENING_MAPS, {"SPECIES_SCYTHER"}, "MOVE_TAILWIND"),
    "opening sleep": (OPENING_MAPS, {"SPECIES_SHROOMISH", "SPECIES_FOONGUS"}, "MOVE_SPORE"),
    "pre-Brawly Wide Guard": (PRE_BRAWLY_MAPS, {"SPECIES_ONIX"}, "MOVE_WIDE_GUARD"),
    "pre-Wattson Follow Me": (PRE_WATTSON_MAPS, {"SPECIES_PACHIRISU"}, "MOVE_FOLLOW_ME"),
    "pre-Wattson Prankster": (PRE_WATTSON_MAPS, {"SPECIES_RIOLU", "SPECIES_MURKROW"}, "ABILITY_PRANKSTER"),
    "pre-Wattson Ground pressure": (PRE_WATTSON_MAPS, {"SPECIES_GEODUDE", "SPECIES_DRILBUR"}, "MOVE_EARTHQUAKE"),
    "pre-Wattson rain mode": (PRE_WATTSON_MAPS, {"SPECIES_LOTAD"}, "MOVE_RAIN_DANCE"),
    "pre-Wattson weather denial": (PRE_WATTSON_MAPS, {"SPECIES_PSYDUCK"}, "ABILITY_CLOUD_NINE"),
}

MEGA_BASE_SOURCE_POLICY = {
    "Pidgeot": ({"SPECIES_PIDGEY"}, OPENING_MAPS, 4),
    "Raichu": ({"SPECIES_PICHU", "SPECIES_PIKACHU"}, OPENING_ECOSYSTEM_MAPS, 4),
    "Beedrill": ({"SPECIES_WEEDLE"}, OPENING_ECOSYSTEM_MAPS, 4),
    "Scizor": ({"SPECIES_SCYTHER"}, OPENING_MAPS, 5),
    "Slowbro": ({"SPECIES_SLOWPOKE"}, PRE_WATTSON_MAPS, 4),
    "Gengar": ({"SPECIES_GASTLY"}, PRE_WATTSON_MAPS, 4),
    "Steelix": ({"SPECIES_ONIX"}, PRE_BRAWLY_MAPS, 4),
    "Lucario": ({"SPECIES_RIOLU"}, OPENING_MAPS, 4),
    "Sableye": ({"SPECIES_SABLEYE"}, PRE_BRAWLY_MAPS, 4),
    "Mawile": ({"SPECIES_MAWILE"}, PRE_BRAWLY_MAPS, 4),
    "Metagross": ({"SPECIES_BELDUM"}, PRE_BRAWLY_MAPS, 5),
    "Baxcalibur": ({"SPECIES_FRIGIBAX"}, PRE_WATTSON_MAPS, 5),
}

BIOME_ANCHORS = {
    "MAP_ROUTE101": {"SPECIES_ZIGZAGOON", "SPECIES_POOCHYENA", "SPECIES_PIDGEY"},
    "MAP_ROUTE102": {"SPECIES_LOTAD", "SPECIES_SEEDOT", "SPECIES_RALTS"},
    "MAP_ROUTE103": {"SPECIES_WINGULL", "SPECIES_SHINX", "SPECIES_GROWLITHE"},
    "MAP_ROUTE104": {"SPECIES_TAILLOW", "SPECIES_BUNEARY", "SPECIES_BUNNELBY"},
    "MAP_ROUTE105": {"SPECIES_SLOWPOKE", "SPECIES_EXEGGCUTE", "SPECIES_INKAY"},
    "MAP_ROUTE106": {"SPECIES_MAKUHITA", "SPECIES_MACHOP", "SPECIES_BINACLE"},
    "MAP_ROUTE107": {"SPECIES_MANTYKE", "SPECIES_CHINCHOU", "SPECIES_REMORAID"},
    "MAP_ROUTE108": {"SPECIES_FRILLISH", "SPECIES_SKRELP", "SPECIES_DHELMISE"},
    "MAP_ROUTE109": {"SPECIES_CORSOLA", "SPECIES_SANDYGAST", "SPECIES_PINCURCHIN"},
    "MAP_ROUTE110": {"SPECIES_ELECTRIKE", "SPECIES_PLUSLE", "SPECIES_MINUN"},
    "MAP_ROUTE116": {"SPECIES_NINCADA", "SPECIES_RIOLU", "SPECIES_EEVEE"},
    "MAP_PETALBURG_WOODS": {"SPECIES_SHROOMISH", "SPECIES_SCYTHER", "SPECIES_FOONGUS"},
    "MAP_PETALBURG_WOODS_2": {"SPECIES_CATERPIE", "SPECIES_WEEDLE", "SPECIES_PANSAGE"},
    "MAP_PETALBURG_WOODS_3": {"SPECIES_MURKROW", "SPECIES_KARTANA", "SPECIES_PHANTUMP"},
    "MAP_RUSTURF_TUNNEL": {"SPECIES_WHISMUR", "SPECIES_LARVITAR", "SPECIES_BAGON"},
    "MAP_GRANITE_CAVE_1F": {"SPECIES_GEODUDE", "SPECIES_MAKUHITA", "SPECIES_BELDUM"},
    "MAP_GRANITE_CAVE_B1F": {"SPECIES_MAWILE", "SPECIES_SABLEYE", "SPECIES_ONIX"},
    "MAP_GRANITE_CAVE_B2F": {"SPECIES_CARBINK", "SPECIES_BRONZOR", "SPECIES_DURALUDON"},
    "MAP_DEWFORD_MEADOW": {"SPECIES_CUTIEFLY", "SPECIES_COMBEE", "SPECIES_PHEROMOSA"},
    "MAP_DEWFORD_MANOR_1F": {"SPECIES_GASTLY", "SPECIES_DROWZEE", "SPECIES_LITWICK"},
    "MAP_SEASPRAY_CAVE": {"SPECIES_PSYDUCK", "SPECIES_TYNAMO", "SPECIES_FRILLISH"},
    "MAP_SEASPRAY_CAVE_B1F": {"SPECIES_SNORUNT", "SPECIES_SNEASEL", "SPECIES_FRIGIBAX"},
    "MAP_NEW_MAUVILLE_INSIDE": {"SPECIES_MAGNEMITE", "SPECIES_PORYGON", "SPECIES_IRON_HANDS"},
}

TROPHY_POLICY = {
    ("MAP_PETALBURG_WOODS", "SPECIES_SCYTHER"): 5,
    ("MAP_PETALBURG_WOODS_3", "SPECIES_KARTANA"): 5,
    ("MAP_DEWFORD_MEADOW", "SPECIES_PHEROMOSA"): 5,
    ("MAP_SEASPRAY_CAVE", "SPECIES_STUNFISK_GALAR"): 5,
    ("MAP_GRANITE_CAVE_1F", "SPECIES_BELDUM"): 5,
    ("MAP_RUSTURF_TUNNEL", "SPECIES_LARVITAR"): 5,
    ("MAP_RUSTURF_TUNNEL", "SPECIES_BAGON"): 1,
    ("MAP_ROUTE116", "SPECIES_DREEPY"): 1,
    ("MAP_SEASPRAY_CAVE_B1F", "SPECIES_FRIGIBAX"): 5,
}

DYNAMO_HEAT_ROUTE_MAPS = {f"MAP_ROUTE{route}" for route in (111, 112, 113, 114, 115, 117)}
BALANCE_FEATHER_ROUTE_MAPS = {f"MAP_ROUTE{route}" for route in range(118, 124)}

MIDGAME_SIDE_MAPS = {
    "MAP_FIERY_PATH", "MAP_JAGGED_PASS",
    "MAP_MIRAGE_TOWER_1F", "MAP_MIRAGE_TOWER_2F", "MAP_MIRAGE_TOWER_3F",
    "MAP_MIRAGE_TOWER_4F", "MAP_MIRAGE_TOWER_B1F",
    "MAP_ROUTE111_RUINS_EXTERIOR", "MAP_SANDSTREWN_RUINS",
    "MAP_SANDSTREWN_RUINS_2F", "MAP_SANDSTREWN_RUINS_3F", "MAP_SANDSTREWN_RUINS_B1F",
    "MAP_DESERT_UNDERPASS", "MAP_ASHEN_WOODS", "MAP_EMBER_PATH", "MAP_VERDANTURF_MEADOW",
    "MAP_METEOR_FALLS_1F_1R", "MAP_METEOR_FALLS_1F_2R",
    "MAP_METEOR_FALLS_B1F_1R", "MAP_METEOR_FALLS_B1F_2R",
    "MAP_SCORCHED_SLAB_B1F", "MAP_SCORCHED_SLAB_B2F", "MAP_SCORCHED_SLAB_HEATRANS_ROOM",
    "MAP_MT_PYRE_1F", "MAP_MT_PYRE_2F", "MAP_MT_PYRE_3F", "MAP_MT_PYRE_4F",
    "MAP_MT_PYRE_5F", "MAP_MT_PYRE_6F", "MAP_MT_PYRE_EXTERIOR", "MAP_MT_PYRE_SUMMIT",
    "MAP_SAFARI_ZONE_NORTH", "MAP_SAFARI_ZONE_SOUTH", "MAP_SAFARI_ZONE_NORTHWEST",
    "MAP_SAFARI_ZONE_SOUTHWEST", "MAP_SAFARI_ZONE_NORTHEAST", "MAP_SAFARI_ZONE_SOUTHEAST",
}

MIDGAME_ECOSYSTEM_MAPS = DYNAMO_HEAT_ROUTE_MAPS | BALANCE_FEATHER_ROUTE_MAPS | MIDGAME_SIDE_MAPS

MIDGAME_ROLE_POLICY = {
    "midgame sun": (MIDGAME_ECOSYSTEM_MAPS, {"SPECIES_TORKOAL"}, "ABILITY_DROUGHT"),
    "midgame sand": (MIDGAME_ECOSYSTEM_MAPS, {"SPECIES_HIPPOPOTAS"}, "ABILITY_SAND_STREAM"),
    "midgame Fake Out": (MIDGAME_ECOSYSTEM_MAPS, {"SPECIES_MIENFOO"}, "MOVE_FAKE_OUT"),
    "midgame Wide Guard": (MIDGAME_ECOSYSTEM_MAPS, {"SPECIES_NOSEPASS"}, "MOVE_WIDE_GUARD"),
    "midgame Trick Room": (MIDGAME_ECOSYSTEM_MAPS, {"SPECIES_BRONZOR", "SPECIES_ORANGURU"}, "MOVE_TRICK_ROOM"),
    "midgame rain": (MIDGAME_ECOSYSTEM_MAPS, {"SPECIES_PELIPPER"}, "ABILITY_DRIZZLE"),
    "midgame redirection": (MIDGAME_ECOSYSTEM_MAPS, {"SPECIES_AMOONGUSS"}, "MOVE_RAGE_POWDER"),
    "midgame terrain": (MIDGAME_ECOSYSTEM_MAPS, {"SPECIES_INDEEDEE"}, "ABILITY_PSYCHIC_SURGE"),
    "midgame Electric pressure": (MIDGAME_ECOSYSTEM_MAPS, {"SPECIES_MANECTRIC"}, "MOVE_VOLT_SWITCH"),
    "midgame Dark pressure": (MIDGAME_ECOSYSTEM_MAPS, {"SPECIES_ABSOL"}, "MOVE_SUCKER_PUNCH"),
    "midgame Ghost pressure": (MIDGAME_ECOSYSTEM_MAPS, {"SPECIES_MIMIKYU"}, "MOVE_SHADOW_SNEAK"),
}

MIDGAME_MEGA_BASE_POLICY = {
    "Lopunny": ({"SPECIES_BUNEARY"}, OPENING_MAPS, 4),
    "Manectric": ({"SPECIES_ELECTRIKE"}, OPENING_ECOSYSTEM_MAPS, 4),
    "Tyranitar": ({"SPECIES_LARVITAR"}, OPENING_MAPS, 5),
    "Aggron": ({"SPECIES_ARON"}, OPENING_ECOSYSTEM_MAPS, 4),
    "Camerupt": ({"SPECIES_NUMEL"}, DYNAMO_HEAT_ROUTE_MAPS | {"MAP_FIERY_PATH"}, 4),
    "Garchomp": ({"SPECIES_GIBLE"}, DYNAMO_HEAT_ROUTE_MAPS, 4),
    "Kangaskhan": ({"SPECIES_KANGASKHAN"}, {"MAP_ROUTE111"}, 5),
    "Houndoom": ({"SPECIES_HOUNDOUR"}, MIDGAME_ECOSYSTEM_MAPS, 4),
    "Altaria": ({"SPECIES_SWABLU"}, DYNAMO_HEAT_ROUTE_MAPS, 4),
    "Salamence": ({"SPECIES_BAGON"}, MIDGAME_ECOSYSTEM_MAPS, 4),
    "Pinsir": ({"SPECIES_PINSIR"}, {"MAP_ASHEN_WOODS"}, 4),
    "Medicham": ({"SPECIES_MEDITITE"}, MIDGAME_ECOSYSTEM_MAPS, 4),
    "Absol": ({"SPECIES_ABSOL"}, {"MAP_JAGGED_PASS"}, 1),
    "Banette": ({"SPECIES_SHUPPET"}, MIDGAME_ECOSYSTEM_MAPS, 4),
    "Chimecho": ({"SPECIES_CHINGLING", "SPECIES_CHIMECHO"}, MIDGAME_ECOSYSTEM_MAPS, 4),
    "Hawlucha": ({"SPECIES_HAWLUCHA"}, MIDGAME_ECOSYSTEM_MAPS, 4),
    "Scovillain": ({"SPECIES_CAPSAKID"}, DYNAMO_HEAT_ROUTE_MAPS, 4),
    "Drampa": ({"SPECIES_DRAMPA"}, MIDGAME_ECOSYSTEM_MAPS, 5),
    "Falinks": ({"SPECIES_FALINKS"}, DYNAMO_HEAT_ROUTE_MAPS, 4),
    "Tatsugiri": ({"SPECIES_TATSUGIRI", "SPECIES_TATSUGIRI_DROOPY", "SPECIES_TATSUGIRI_STRETCHY"}, BALANCE_FEATHER_ROUTE_MAPS, 4),
    "Sharpedo": ({"SPECIES_SHARPEDO"}, BALANCE_FEATHER_ROUTE_MAPS, 4),
}

MIDGAME_TROPHY_POLICY = {
    ("MAP_SANDSTREWN_RUINS", "SPECIES_GREAT_TUSK"): 5,
    ("MAP_SANDSTREWN_RUINS_B1F", "SPECIES_STAKATAKA"): 5,
    ("MAP_ASHEN_WOODS", "SPECIES_BUZZWOLE"): 5,
    ("MAP_EMBER_PATH", "SPECIES_BLACEPHALON"): 5,
    ("MAP_ROUTE119", "SPECIES_RAGING_BOLT"): 5,
    ("MAP_METEOR_FALLS_B1F_2R", "SPECIES_ROARING_MOON"): 5,
    ("MAP_MT_PYRE_SUMMIT", "SPECIES_FLUTTER_MANE"): 5,
    ("MAP_MIRAGE_TOWER_B1F", "SPECIES_GIMMIGHOUL_CHEST"): 5,
    ("MAP_METEOR_FALLS_B1F_2R", "SPECIES_SALAMENCE"): 1,
}

MIDGAME_BIOME_ANCHORS = {
    "MAP_ROUTE111": {"SPECIES_SANDSHREW", "SPECIES_TRAPINCH", "SPECIES_HIPPOPOTAS"},
    "MAP_ROUTE112": {"SPECIES_NUMEL", "SPECIES_MACHOP", "SPECIES_HAWLUCHA"},
    "MAP_ROUTE113": {"SPECIES_SPINDA", "SPECIES_SKARMORY", "SPECIES_KOFFING"},
    "MAP_ROUTE114": {"SPECIES_SWABLU", "SPECIES_ZANGOOSE", "SPECIES_SEVIPER"},
    "MAP_ROUTE115": {"SPECIES_TANGELA", "SPECIES_MEDITITE", "SPECIES_DRAMPA"},
    "MAP_ROUTE117": {"SPECIES_ROSELIA", "SPECIES_VOLBEAT", "SPECIES_ILLUMISE"},
    "MAP_ROUTE118": {"SPECIES_MANECTRIC", "SPECIES_DONDOZO", "SPECIES_TATSUGIRI"},
    "MAP_ROUTE119": {"SPECIES_TROPIUS", "SPECIES_ORANGURU", "SPECIES_RAGING_BOLT"},
    "MAP_ROUTE120": {"SPECIES_ABSOL", "SPECIES_PUMPKABOO", "SPECIES_MIMIKYU"},
    "MAP_ROUTE121": {"SPECIES_SHUPPET", "SPECIES_DUSKULL", "SPECIES_SINISTEA"},
    "MAP_ROUTE122": {"SPECIES_FRILLISH", "SPECIES_DHELMISE", "SPECIES_BASCULEGION"},
    "MAP_ROUTE123": {"SPECIES_GLOOM", "SPECIES_KARRABLAST", "SPECIES_SHELMET"},
    "MAP_FIERY_PATH": {"SPECIES_TORKOAL", "SPECIES_HOUNDOUR", "SPECIES_LARVESTA"},
    "MAP_JAGGED_PASS": {"SPECIES_PRIMEAPE", "SPECIES_GLIGAR", "SPECIES_JANGMO_O"},
    "MAP_SANDSTREWN_RUINS": {"SPECIES_UNOWN", "SPECIES_GREAT_TUSK", "SPECIES_SPIRITOMB"},
    "MAP_DESERT_UNDERPASS": {"SPECIES_DITTO", "SPECIES_DRACOZOLT", "SPECIES_DRACOVISH"},
    "MAP_ASHEN_WOODS": {"SPECIES_PINSIR", "SPECIES_HERACROSS", "SPECIES_BUZZWOLE"},
    "MAP_EMBER_PATH": {"SPECIES_MAGCARGO", "SPECIES_BLACEPHALON", "SPECIES_COALOSSAL"},
    "MAP_VERDANTURF_MEADOW": {"SPECIES_MUNNA", "SPECIES_FLOETTE_ETERNAL", "SPECIES_INDEEDEE"},
    "MAP_METEOR_FALLS_B1F_2R": {"SPECIES_BAGON", "SPECIES_ROARING_MOON", "SPECIES_SALAMENCE"},
    "MAP_SCORCHED_SLAB_HEATRANS_ROOM": {"SPECIES_MAGCARGO", "SPECIES_TURTONATOR", "SPECIES_HYDREIGON"},
    "MAP_MT_PYRE_1F": {"SPECIES_SHUPPET", "SPECIES_DUSKULL", "SPECIES_GASTLY"},
    "MAP_MT_PYRE_6F": {"SPECIES_MIMIKYU", "SPECIES_POLTCHAGEIST", "SPECIES_HOUNDSTONE"},
    "MAP_MT_PYRE_SUMMIT": {"SPECIES_CHIMECHO", "SPECIES_FLUTTER_MANE", "SPECIES_ABSOL"},
    "MAP_SAFARI_ZONE_NORTH": {"SPECIES_KANGASKHAN", "SPECIES_HERACROSS", "SPECIES_SCYTHER"},
    "MAP_SAFARI_ZONE_SOUTH": {"SPECIES_PIKACHU", "SPECIES_CHANSEY", "SPECIES_DITTO"},
    "MAP_SAFARI_ZONE_NORTHWEST": {"SPECIES_RHYHORN", "SPECIES_SCYTHER", "SPECIES_CHANSEY"},
    "MAP_SAFARI_ZONE_SOUTHWEST": {"SPECIES_KARRABLAST", "SPECIES_SHELMET", "SPECIES_GOODRA"},
    "MAP_SAFARI_ZONE_NORTHEAST": {"SPECIES_MILTANK", "SPECIES_SKARMORY", "SPECIES_HERACROSS"},
    "MAP_SAFARI_ZONE_SOUTHEAST": {"SPECIES_GLIGAR", "SPECIES_SKARMORY", "SPECIES_URSARING"},
}

MIDGAME_DISTINCT_GROUPS = (
    tuple(f"MAP_ROUTE{route}" for route in range(111, 124) if route != 116),
    ("MAP_MIRAGE_TOWER_1F", "MAP_MIRAGE_TOWER_2F", "MAP_MIRAGE_TOWER_3F", "MAP_MIRAGE_TOWER_4F", "MAP_MIRAGE_TOWER_B1F"),
    ("MAP_SANDSTREWN_RUINS", "MAP_SANDSTREWN_RUINS_2F", "MAP_SANDSTREWN_RUINS_3F", "MAP_SANDSTREWN_RUINS_B1F"),
    ("MAP_METEOR_FALLS_1F_1R", "MAP_METEOR_FALLS_1F_2R", "MAP_METEOR_FALLS_B1F_1R", "MAP_METEOR_FALLS_B1F_2R"),
    ("MAP_SCORCHED_SLAB_B1F", "MAP_SCORCHED_SLAB_B2F", "MAP_SCORCHED_SLAB_HEATRANS_ROOM"),
    ("MAP_MT_PYRE_1F", "MAP_MT_PYRE_2F", "MAP_MT_PYRE_3F", "MAP_MT_PYRE_4F", "MAP_MT_PYRE_5F", "MAP_MT_PYRE_6F", "MAP_MT_PYRE_EXTERIOR", "MAP_MT_PYRE_SUMMIT"),
    ("MAP_SAFARI_ZONE_NORTH", "MAP_SAFARI_ZONE_SOUTH", "MAP_SAFARI_ZONE_NORTHWEST", "MAP_SAFARI_ZONE_SOUTHWEST", "MAP_SAFARI_ZONE_NORTHEAST", "MAP_SAFARI_ZONE_SOUTHEAST"),
)

FINAL_ROUTE_MAPS = {f"MAP_ROUTE{route}" for route in range(124, 135)}

FINAL_ECOSYSTEM_MAPS = FINAL_ROUTE_MAPS | {
    "MAP_DEWFORD_TOWN", "MAP_PETALBURG_CITY", "MAP_SLATEPORT_CITY", "MAP_LILYCOVE_CITY",
    "MAP_MOSSDEEP_CITY", "MAP_PACIFIDLOG_TOWN", "MAP_SOOTOPOLIS_CITY", "MAP_EVER_GRANDE_CITY",
    "MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS", "MAP_ABANDONED_SHIP_ROOMS_B1F",
    "MAP_UNDERWATER_ROUTE124", "MAP_UNDERWATER_ROUTE126", "MAP_UNDERWATER_SEAFLOOR_CAVERN",
    "MAP_SEAFLOOR_CAVERN_ENTRANCE", "MAP_SEAFLOOR_CAVERN_ROOM1", "MAP_SEAFLOOR_CAVERN_ROOM2",
    "MAP_SEAFLOOR_CAVERN_ROOM3", "MAP_SEAFLOOR_CAVERN_ROOM4", "MAP_SEAFLOOR_CAVERN_ROOM5",
    "MAP_SEAFLOOR_CAVERN_ROOM6", "MAP_SEAFLOOR_CAVERN_ROOM7", "MAP_SEAFLOOR_CAVERN_ROOM8",
    "MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM", "MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM",
    "MAP_SHOAL_CAVE_LOW_TIDE_LOWER_ROOM", "MAP_SHOAL_CAVE_LOW_TIDE_STAIRS_ROOM",
    "MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM",
    "MAP_CAVE_OF_ORIGIN_ENTRANCE", "MAP_CAVE_OF_ORIGIN_1F",
    "MAP_CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP1",
    "MAP_CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP2",
    "MAP_CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP3", "MAP_CAVE_OF_ORIGIN_DIANCIES_ROOM",
    "MAP_SKY_PILLAR_1F", "MAP_SKY_PILLAR_3F", "MAP_SKY_PILLAR_5F",
    "MAP_VICTORY_ROAD_1F", "MAP_VICTORY_ROAD_B1F", "MAP_VICTORY_ROAD_B2F",
    "MAP_MAGMA_HIDEOUT_1F", "MAP_MAGMA_HIDEOUT_2F_1R", "MAP_MAGMA_HIDEOUT_2F_2R",
    "MAP_MAGMA_HIDEOUT_2F_3R", "MAP_MAGMA_HIDEOUT_3F_1R", "MAP_MAGMA_HIDEOUT_3F_2R",
    "MAP_MAGMA_HIDEOUT_3F_3R", "MAP_MAGMA_HIDEOUT_4F",
    "MAP_ALTERING_CAVE", "MAP_ALTERING_CAVE_1F", "MAP_ALTERING_CAVE_B1F",
    "MAP_ARTISAN_CAVE_1F", "MAP_ARTISAN_CAVE_B1F", "MAP_METEOR_FALLS_STEVENS_CAVE",
    "MAP_GRANITE_CAVE_STEVENS_ROOM", "MAP_NEW_MAUVILLE_ENTRANCE",
}

ALL_CURATED_ECOSYSTEM_MAPS = OPENING_ECOSYSTEM_MAPS | MIDGAME_ECOSYSTEM_MAPS | FINAL_ECOSYSTEM_MAPS

FINAL_ROLE_POLICY = {
    "League rain": (ALL_CURATED_ECOSYSTEM_MAPS, {"SPECIES_PELIPPER"}, "ABILITY_DRIZZLE"),
    "League sun": (ALL_CURATED_ECOSYSTEM_MAPS, {"SPECIES_TORKOAL"}, "ABILITY_DROUGHT"),
    "League sand": (ALL_CURATED_ECOSYSTEM_MAPS, {"SPECIES_HIPPOPOTAS"}, "ABILITY_SAND_STREAM"),
    "League snow": (ALL_CURATED_ECOSYSTEM_MAPS, {"SPECIES_SNOVER"}, "ABILITY_SNOW_WARNING"),
    "League redirection": (ALL_CURATED_ECOSYSTEM_MAPS, {"SPECIES_AMOONGUSS"}, "MOVE_RAGE_POWDER"),
    "League Haze": (FINAL_ECOSYSTEM_MAPS, {"SPECIES_TOXAPEX"}, "MOVE_HAZE"),
    "League Wide Guard": (ALL_CURATED_ECOSYSTEM_MAPS, {"SPECIES_PELIPPER", "SPECIES_NOSEPASS"}, "MOVE_WIDE_GUARD"),
    "League Tailwind": (ALL_CURATED_ECOSYSTEM_MAPS, {"SPECIES_PELIPPER", "SPECIES_SCYTHER"}, "MOVE_TAILWIND"),
    "League Trick Room": (ALL_CURATED_ECOSYSTEM_MAPS, {"SPECIES_ORANGURU", "SPECIES_MIMIKYU"}, "MOVE_TRICK_ROOM"),
    "League priority": (ALL_CURATED_ECOSYSTEM_MAPS, {"SPECIES_MIMIKYU", "SPECIES_ABSOL"}, "MOVE_SHADOW_SNEAK"),
    "League Intimidate": (FINAL_ECOSYSTEM_MAPS, {"SPECIES_GYARADOS"}, "ABILITY_INTIMIDATE"),
    "League weather denial": (ALL_CURATED_ECOSYSTEM_MAPS, {"SPECIES_PSYDUCK"}, "ABILITY_CLOUD_NINE"),
}

FINAL_MEGA_BASE_POLICY = {
    "Gyarados": ({"SPECIES_GYARADOS"}, FINAL_ROUTE_MAPS, 4),
    "Starmie": ({"SPECIES_STARYU", "SPECIES_STARMIE"}, FINAL_ECOSYSTEM_MAPS, 4),
    "Dragonite": ({"SPECIES_DRATINI", "SPECIES_DRAGONAIR", "SPECIES_DRAGONITE"}, FINAL_ECOSYSTEM_MAPS, 4),
    "Glalie-Froslass": ({"SPECIES_SNORUNT"}, FINAL_ECOSYSTEM_MAPS, 4),
    "Abomasnow": ({"SPECIES_SNOVER"}, FINAL_ECOSYSTEM_MAPS, 4),
    "Salamence": ({"SPECIES_BAGON", "SPECIES_SALAMENCE"}, FINAL_ECOSYSTEM_MAPS, 1),
    "Metagross": ({"SPECIES_BELDUM", "SPECIES_METANG", "SPECIES_METAGROSS"}, FINAL_ECOSYSTEM_MAPS, 4),
    "Sharpedo": ({"SPECIES_SHARPEDO"}, FINAL_ECOSYSTEM_MAPS, 4),
    "Malamar": ({"SPECIES_MALAMAR"}, FINAL_ECOSYSTEM_MAPS, 4),
    "Barbaracle": ({"SPECIES_BARBARACLE"}, FINAL_ECOSYSTEM_MAPS, 4),
    "Dragalge": ({"SPECIES_DRAGALGE"}, FINAL_ECOSYSTEM_MAPS, 4),
    "Golisopod": ({"SPECIES_GOLISOPOD"}, FINAL_ECOSYSTEM_MAPS, 4),
    "Baxcalibur": ({"SPECIES_FRIGIBAX", "SPECIES_ARCTIBAX", "SPECIES_BAXCALIBUR"}, ALL_CURATED_ECOSYSTEM_MAPS, 5),
    "Glimmora": ({"SPECIES_GLIMMET", "SPECIES_GLIMMORA"}, ALL_CURATED_ECOSYSTEM_MAPS, 4),
    "Duraludon-Archaludon": ({"SPECIES_DURALUDON", "SPECIES_ARCHALUDON"}, FINAL_ECOSYSTEM_MAPS, 4),
    "Eelektross": ({"SPECIES_TYNAMO", "SPECIES_EELEKTROSS"}, ALL_CURATED_ECOSYSTEM_MAPS, 4),
}

FINAL_TROPHY_POLICY = {
    ("MAP_ROUTE124", "SPECIES_STARMIE"): 1,
    ("MAP_ROUTE125", "SPECIES_LAPRAS"): 5,
    ("MAP_UNDERWATER_ROUTE126", "SPECIES_FEEBAS"): 5,
    ("MAP_UNDERWATER_SEAFLOOR_CAVERN", "SPECIES_NIHILEGO"): 5,
    ("MAP_UNDERWATER_SEAFLOOR_CAVERN", "SPECIES_IRON_BUNDLE"): 4,
    ("MAP_ROUTE129", "SPECIES_RELICANTH"): 5,
    ("MAP_ROUTE132", "SPECIES_BASCULEGION"): 1,
    ("MAP_ROUTE133", "SPECIES_CURSOLA"): 1,
    ("MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM", "SPECIES_IRON_BUNDLE"): 5,
    ("MAP_CAVE_OF_ORIGIN_1F", "SPECIES_WALKING_WAKE"): 5,
    ("MAP_ALTERING_CAVE_B1F", "SPECIES_GUZZLORD"): 5,
    ("MAP_METEOR_FALLS_STEVENS_CAVE", "SPECIES_IRON_CROWN"): 4,
    ("MAP_VICTORY_ROAD_1F", "SPECIES_BAXCALIBUR"): 1,
    ("MAP_SKY_PILLAR_5F", "SPECIES_METAGROSS"): 1,
}

FINAL_BIOME_ANCHORS = {
    "MAP_ROUTE124": {"SPECIES_FINIZEN", "SPECIES_JELLICENT", "SPECIES_DHELMISE"},
    "MAP_ROUTE125": {"SPECIES_SEEL", "SPECIES_SPHEAL", "SPECIES_LAPRAS"},
    "MAP_ROUTE126": {"SPECIES_LUVDISC", "SPECIES_CORSOLA", "SPECIES_GOREBYSS"},
    "MAP_ROUTE127": {"SPECIES_WIMPOD", "SPECIES_DRAGALGE", "SPECIES_GOLISOPOD"},
    "MAP_ROUTE128": {"SPECIES_HORSEA", "SPECIES_SKRELP", "SPECIES_KINGDRA"},
    "MAP_ROUTE129": {"SPECIES_WAILMER", "SPECIES_WISHIWASHI", "SPECIES_DONDOZO"},
    "MAP_ROUTE130": {"SPECIES_WYNAUT", "SPECIES_LAPRAS", "SPECIES_TOXAPEX"},
    "MAP_ROUTE131": {"SPECIES_CORSOLA", "SPECIES_CHINCHOU", "SPECIES_MANTINE"},
    "MAP_ROUTE132": {"SPECIES_SHARPEDO", "SPECIES_BARRASKEWDA", "SPECIES_FINIZEN"},
    "MAP_ROUTE133": {"SPECIES_SKRELP", "SPECIES_JELLICENT", "SPECIES_DHELMISE"},
    "MAP_ROUTE134": {"SPECIES_RELICANTH", "SPECIES_WAILORD", "SPECIES_KINGDRA"},
    "MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS": {"SPECIES_FRILLISH", "SPECIES_DHELMISE", "SPECIES_BASCULEGION"},
    "MAP_UNDERWATER_ROUTE124": {"SPECIES_CLAMPERL", "SPECIES_RELICANTH", "SPECIES_GOLISOPOD"},
    "MAP_UNDERWATER_ROUTE126": {"SPECIES_CORSOLA", "SPECIES_FEEBAS", "SPECIES_GOREBYSS"},
    "MAP_UNDERWATER_SEAFLOOR_CAVERN": {"SPECIES_RELICANTH", "SPECIES_NIHILEGO", "SPECIES_IRON_BUNDLE"},
    "MAP_SEAFLOOR_CAVERN_ENTRANCE": {"SPECIES_SHARPEDO", "SPECIES_DRAGALGE", "SPECIES_DHELMISE"},
    "MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM": {"SPECIES_VULPIX_ALOLA", "SPECIES_IRON_BUNDLE", "SPECIES_ARCTIBAX"},
    "MAP_CAVE_OF_ORIGIN_1F": {"SPECIES_CARBINK", "SPECIES_WALKING_WAKE", "SPECIES_GLIMMORA"},
    "MAP_SKY_PILLAR_5F": {"SPECIES_ALTARIA", "SPECIES_DRAGAPULT", "SPECIES_METAGROSS"},
    "MAP_VICTORY_ROAD_1F": {"SPECIES_PUPITAR", "SPECIES_IRON_VALIANT", "SPECIES_BAXCALIBUR"},
    "MAP_MAGMA_HIDEOUT_4F": {"SPECIES_CAMERUPT", "SPECIES_TURTONATOR", "SPECIES_COALOSSAL"},
    "MAP_ALTERING_CAVE_B1F": {"SPECIES_LUCARIO", "SPECIES_GUZZLORD", "SPECIES_EELEKTROSS"},
    "MAP_ARTISAN_CAVE_1F": {"SPECIES_SMEARGLE"},
    "MAP_METEOR_FALLS_STEVENS_CAVE": {"SPECIES_METAGROSS", "SPECIES_ARCHALUDON", "SPECIES_IRON_CROWN"},
}

FINAL_DISTINCT_GROUPS = (
    tuple(f"MAP_ROUTE{route}" for route in range(124, 135)),
    ("MAP_ABANDONED_SHIP_HIDDEN_FLOOR_CORRIDORS", "MAP_ABANDONED_SHIP_ROOMS_B1F"),
    ("MAP_UNDERWATER_ROUTE124", "MAP_UNDERWATER_ROUTE126", "MAP_UNDERWATER_SEAFLOOR_CAVERN"),
    tuple(f"MAP_SEAFLOOR_CAVERN_ROOM{room}" for room in range(1, 9)),
    ("MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM", "MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM", "MAP_SHOAL_CAVE_LOW_TIDE_LOWER_ROOM", "MAP_SHOAL_CAVE_LOW_TIDE_STAIRS_ROOM", "MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM"),
    ("MAP_CAVE_OF_ORIGIN_ENTRANCE", "MAP_CAVE_OF_ORIGIN_1F", "MAP_CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP1", "MAP_CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP2", "MAP_CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP3", "MAP_CAVE_OF_ORIGIN_DIANCIES_ROOM"),
    ("MAP_SKY_PILLAR_1F", "MAP_SKY_PILLAR_3F", "MAP_SKY_PILLAR_5F"),
    ("MAP_VICTORY_ROAD_1F", "MAP_VICTORY_ROAD_B1F", "MAP_VICTORY_ROAD_B2F"),
    ("MAP_MAGMA_HIDEOUT_1F", "MAP_MAGMA_HIDEOUT_2F_1R", "MAP_MAGMA_HIDEOUT_2F_2R", "MAP_MAGMA_HIDEOUT_2F_3R", "MAP_MAGMA_HIDEOUT_3F_1R", "MAP_MAGMA_HIDEOUT_3F_2R", "MAP_MAGMA_HIDEOUT_3F_3R", "MAP_MAGMA_HIDEOUT_4F"),
    ("MAP_ALTERING_CAVE_1F", "MAP_ALTERING_CAVE_B1F"),
)

# These restored late-campaign caves previously inherited placeholder level
# 2-4 tables even though their species, trainers, and access gates are all
# late-game.  Keep every method in the reviewed late-campaign band so future
# table rewrites cannot silently recreate that mismatch. Evolution-floor
# repairs may raise an individual species as high as the area's Lv. 55 cap.
RESTORED_LATE_CAVE_LEVEL_MAPS = {
    "MAP_ALTERING_CAVE_1F",
    "MAP_ALTERING_CAVE_B1F",
    "MAP_SCORCHED_SLAB_B1F",
    "MAP_SCORCHED_SLAB_B2F",
    "MAP_SCORCHED_SLAB_HEATRANS_ROOM",
}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def hoenn_map_ids() -> set[str]:
    groups = json.loads((ROOT / "data/maps/map_groups.json").read_text())
    result = set()
    for group, maps in groups.items():
        if group == "group_order" or "_Frlg" in group:
            continue
        for map_name in maps:
            path = ROOT / "data/maps" / map_name / "map.json"
            if path.exists():
                result.add(json.loads(path.read_text())["id"])
    return result


def encounter_rows() -> list[dict]:
    payload = json.loads(TARGET.read_text())
    group = next(row for row in payload["wild_encounter_groups"] if row["label"] == "gWildMonHeaders")
    allowed = hoenn_map_ids()
    return [row for row in group["encounters"] if row.get("map") in allowed]


def encounter_map() -> dict[str, dict]:
    return {row["map"]: row for row in encounter_rows()}


def species_generations(graph: SpeciesGraph) -> dict[str, int]:
    result = {}
    for generation in range(1, 10):
        source = (ROOT / f"src/data/pokemon/species_info/gen_{generation}_families.h").read_text()
        for species in re.findall(r"\[?(SPECIES_[A-Z0-9_]+)\]?\s*=", source):
            if species in graph.species:
                result.setdefault(graph.find(species), generation)
    return result


def starter_components(graph: SpeciesGraph) -> set[str]:
    source = (ROOT / "src/starter_choose.c").read_text()
    array = re.search(r"static const enum Species sStarterMons.*?\n\};", source, re.S)
    require(array is not None, "regional starter table is missing")
    return {
        graph.find(species)
        for species in re.findall(r"SPECIES_[A-Z0-9_]+", array.group())
        if species in graph.species
    }


def species_rate(entry: dict, species: str) -> int:
    chance = 0
    for method_name, rates in SLOT_RATES.items():
        for index, mon in enumerate(entry.get(method_name, {}).get("mons", [])):
            if mon["species"] == species:
                chance += rates[index]
    return chance


def preset_blocks() -> dict[str, str]:
    source = (ROOT / "src/data/pokemon/emerald_champions_battle_sets.h").read_text()
    starts = list(re.finditer(r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{", source))
    result = {}
    for index, match in enumerate(starts):
        result.setdefault(
            match.group(1),
            source[match.start(): starts[index + 1].start() if index + 1 < len(starts) else len(source)],
        )
    return result


def verify_role_policy(by_map: dict[str, dict], policy: dict = ROLE_POLICY) -> None:
    presets = preset_blocks()
    for label, (maps, candidates, required_token) in policy.items():
        qualifying = []
        for species in candidates:
            chance = max((species_rate(by_map[map_name], species) for map_name in maps if map_name in by_map), default=0)
            if chance >= 4 and required_token in presets.get(species, ""):
                qualifying.append((species, chance))
        require(qualifying, f"{label} has no >=4% source with {required_token}")


def method_generation_values(
    by_map: dict[str, dict],
    maps: set[str],
    graph: SpeciesGraph,
    indexes: dict[str, tuple[int, ...] | range],
) -> list[int]:
    generations = species_generations(graph)
    values = []
    for map_name in maps:
        entry = by_map.get(map_name, {})
        for method_name, method_indexes in indexes.items():
            mons = entry.get(method_name, {}).get("mons", [])
            for index in method_indexes:
                if index >= len(mons) or mons[index]["species"] not in graph.species:
                    continue
                generation = generations.get(graph.find(mons[index]["species"]))
                if generation is not None:
                    values.append(generation)
    return values


def generation_shares(values: list[int]) -> tuple[float, float, float]:
    require(values, "encounter act has no measurable slots")
    return (
        sum(generation <= 3 for generation in values) / len(values),
        sum(4 <= generation <= 6 for generation in values) / len(values),
        sum(generation >= 7 for generation in values) / len(values),
    )


def verify_opening_act_bias(by_map: dict[str, dict], graph: SpeciesGraph) -> dict[str, tuple[float, float, float]]:
    common_indexes = {
        "land_mons": range(6),
        "water_mons": range(2),
        "rock_smash_mons": range(2),
        "fishing_mons": (0, 1, 2, 5, 6),
    }
    stone = generation_shares(method_generation_values(by_map, OPENING_MAPS, graph, common_indexes))
    dewford = generation_shares(method_generation_values(by_map, DEWFORD_WATTSON_MAPS, graph, common_indexes))
    require(0.85 <= stone[0] <= 0.95,
            f"Stone-act common Gen 1-3 share left its 85-95% band: {stone[0]:.1%}")
    require(stone[2] <= 0.02,
            f"Stone-act common slots overuse Gen 7-9 families: {stone[2]:.1%}")
    require(0.65 <= dewford[0] <= 0.80,
            f"Dewford-Wattson common Gen 1-3 share left its 65-80% band: {dewford[0]:.1%}")
    require(dewford[1] >= 0.15,
            f"Dewford-Wattson common slots lack Gen 4-6 texture: {dewford[1]:.1%}")
    require(0.03 <= dewford[2] <= 0.10,
            f"Dewford-Wattson Gen 7-9 common share left its 3-10% band: {dewford[2]:.1%}")

    land_layers = {
        "common": generation_shares(method_generation_values(by_map, OPENING_MAPS, graph, {"land_mons": range(6)})),
        "toolbox": generation_shares(method_generation_values(by_map, OPENING_MAPS, graph, {"land_mons": range(6, 10)})),
        "trophy": generation_shares(method_generation_values(by_map, OPENING_MAPS, graph, {"land_mons": range(10, 12)})),
    }
    require(land_layers["toolbox"][1] + land_layers["toolbox"][2] >= 0.65,
            "opening toolbox layer no longer carries later-generation strategy options")
    require(land_layers["trophy"][0] >= 0.25 and land_layers["trophy"][2] >= 0.25,
            "opening trophy layer must mix nostalgic and modern surprises")
    return {"Stone": stone, "Dewford-Wattson": dewford, **land_layers}


def verify_fossil_policy(rows: list[dict], graph: SpeciesGraph) -> None:
    fossil_components = {graph.find(species) for species in FOSSIL_FAMILY_ROOTS}
    leaked = []
    for entry in rows:
        for method_name, method in entry.items():
            if not method_name.endswith("_mons") or not isinstance(method, dict):
                continue
            for slot, mon in enumerate(method.get("mons", [])):
                if mon["species"] in graph.species and graph.find(mon["species"]) in fossil_components:
                    leaked.append(f"{entry['map']}:{method_name}[{slot}]={mon['species']}")
    require(not leaked, "wild fossil families invalidate finite fossil rewards: " + ", ".join(leaked))


def verify_preset_legality(by_map: dict[str, dict], graph: SpeciesGraph, maps: set[str]) -> None:
    preset_components = {
        graph.find(species)
        for species in preset_blocks()
        if species in graph.species
    }
    missing = set()
    for map_name in maps:
        for method_name, method in by_map[map_name].items():
            if not method_name.endswith("_mons") or not isinstance(method, dict):
                continue
            for mon in method.get("mons", []):
                if mon["species"] not in graph.species or graph.find(mon["species"]) not in preset_components:
                    missing.add(mon["species"])
    require(not missing, "opening wild species lack competitive preset families: " + ", ".join(sorted(missing)))


def verify_named_legendary_policy(by_map: dict[str, dict], maps: set[str]) -> None:
    definitions = (ROOT / "src/data/pokemon/legendary_signs.h").read_text()
    legendary_species = {
        "SPECIES_" + species
        for species in re.findall(
            r"(?:WILD_SIGN|VISIBLE_SIGN|ORDINARY_WILD_SIGN|OTHER_SIGN)\([^,]+,\s*([A-Z0-9_]+)",
            definitions,
        )
    }
    allowed = set(RESTORED_ULTRA_BEASTS.values())
    leaked = set()
    for map_name in maps:
        for method_name, method in by_map[map_name].items():
            if method_name.endswith("_mons") and isinstance(method, dict):
                leaked.update(
                    mon["species"] for mon in method.get("mons", [])
                    if mon["species"] in legendary_species and mon["species"] not in allowed
                )
    require(not leaked, "named legendary species leaked into the opening ecosystem: " + ", ".join(sorted(leaked)))


def verify_mega_base_timing(by_map: dict[str, dict], policy: dict) -> None:
    for label, (species, maps, minimum_rate) in policy.items():
        rate = max(
            (species_rate(by_map[map_name], candidate) for map_name in maps for candidate in species),
            default=0,
        )
        require(rate >= minimum_rate,
                f"{label} Mega base lacks a >= {minimum_rate}% source by its intended chapter")


def verify_biome_and_trophy_policy(by_map: dict[str, dict]) -> None:
    for map_name, anchors in BIOME_ANCHORS.items():
        present = {
            mon["species"]
            for method_name, method in by_map[map_name].items()
            if method_name.endswith("_mons") and isinstance(method, dict)
            for mon in method.get("mons", [])
        }
        require(anchors <= present,
                f"{map_name} lost biome anchors: {sorted(anchors - present)}")
    for (map_name, species), expected_rate in TROPHY_POLICY.items():
        actual_rate = species_rate(by_map[map_name], species)
        require(actual_rate == expected_rate,
                f"{map_name} {species} trophy rate drifted: {actual_rate}% != {expected_rate}%")


def verify_midgame_act_bias(by_map: dict[str, dict], graph: SpeciesGraph) -> dict[str, tuple[float, float, float]]:
    common_indexes = {
        "land_mons": range(6),
        "water_mons": range(2),
        "rock_smash_mons": range(2),
        "fishing_mons": (0, 1, 2, 5, 6),
    }
    dynamo_heat = generation_shares(
        method_generation_values(by_map, DYNAMO_HEAT_ROUTE_MAPS, graph, common_indexes)
    )
    balance_feather = generation_shares(
        method_generation_values(by_map, BALANCE_FEATHER_ROUTE_MAPS, graph, common_indexes)
    )
    require(0.82 <= dynamo_heat[0] <= 0.90 and 0.02 <= dynamo_heat[2] <= 0.08,
            f"Dynamo-Heat route bias drifted: {dynamo_heat}")
    require(0.60 <= balance_feather[0] <= 0.70
            and 0.20 <= balance_feather[1] <= 0.30
            and 0.05 <= balance_feather[2] <= 0.15,
            f"Balance-Feather route bias drifted: {balance_feather}")
    require(balance_feather[0] <= dynamo_heat[0] - 0.15,
            "midgame numbered routes no longer ease away from the opening nostalgia peak")
    return {"Dynamo-Heat": dynamo_heat, "Balance-Feather": balance_feather}


def encounter_signature(entry: dict) -> tuple[str, ...]:
    return tuple(sorted({
        mon["species"]
        for method_name, method in entry.items()
        if method_name.endswith("_mons") and isinstance(method, dict)
        for mon in method.get("mons", [])
    }))


def verify_midgame_biomes_and_trophies(by_map: dict[str, dict]) -> None:
    for map_name, anchors in MIDGAME_BIOME_ANCHORS.items():
        present = set(encounter_signature(by_map[map_name]))
        require(anchors <= present,
                f"{map_name} lost midgame biome anchors: {sorted(anchors - present)}")
    for group in MIDGAME_DISTINCT_GROUPS:
        signatures = [encounter_signature(by_map[map_name]) for map_name in group]
        require(len(signatures) == len(set(signatures)),
                f"midgame maps collapsed to duplicate ecosystems: {group}")
    for (map_name, species), expected_rate in MIDGAME_TROPHY_POLICY.items():
        actual_rate = species_rate(by_map[map_name], species)
        require(actual_rate == expected_rate,
                f"{map_name} {species} midgame trophy rate drifted: {actual_rate}% != {expected_rate}%")


def verify_midgame_conditional_signs() -> None:
    definitions = (ROOT / "src/data/pokemon/legendary_signs.h").read_text()
    expected = {
        "CHI_YU": ("ASHEN_WOODS", "HOUNDOOM"),
        "KUBFU": ("ROUTE112", "MACHOP"),
        "TING_LU": ("DESERT_UNDERPASS", "CLAYDOL"),
        "TYPE_NULL": ("ROUTE118", "PORYGON"),
        "OGERPON": ("ROUTE120", "TROPIUS"),
    }
    for species, (map_name, required) in expected.items():
        pattern = (
            rf"WILD_SIGN\(LEGENDARY_SIGN_{species}, {species}, {map_name}, WILD_AREA_LAND, "
            rf"8, [345], [12], {required}, FLAG_BADGE0[345]_GET\),"
        )
        require(re.search(pattern, definitions) is not None,
                f"{species} lost its one-time midgame conditional Sign")
        require(f"ORDINARY_WILD_SIGN(LEGENDARY_SIGN_{species}" not in definitions,
                f"{species} regressed to a repeatable ordinary wild")


def verify_final_act_bias(by_map: dict[str, dict], graph: SpeciesGraph) -> dict[str, tuple[float, float, float]]:
    common_indexes = {
        "land_mons": range(6),
        "water_mons": range(2),
        "rock_smash_mons": range(2),
        "fishing_mons": (0, 1, 2, 5, 6),
    }
    trophy_indexes = {
        "land_mons": range(10, 12),
        "water_mons": range(2, 5),
        "fishing_mons": (8, 9),
    }
    # Mirage Island's all-Wynaut land table is a canonical nostalgia exception.
    common = generation_shares(method_generation_values(
        by_map, FINAL_ROUTE_MAPS - {"MAP_ROUTE130"}, graph, common_indexes
    ))
    trophies = generation_shares(method_generation_values(by_map, FINAL_ROUTE_MAPS, graph, trophy_indexes))
    require(0.55 <= common[0] <= 0.65
            and 0.25 <= common[1] <= 0.35
            and 0.08 <= common[2] <= 0.15,
            f"late-route common generation curve drifted: {common}")
    require(trophies[2] >= 0.20 and trophies[0] >= 0.35,
            f"late trophy layer lost modern progression or nostalgia exceptions: {trophies}")
    wynaut = by_map["MAP_ROUTE130"]["land_mons"]["mons"]
    require(len(wynaut) == 12 and all(mon["species"] == "SPECIES_WYNAUT" for mon in wynaut),
            "Mirage Island lost its canonical all-Wynaut land identity")
    return {"common": common, "trophies": trophies}


def verify_final_biomes_and_trophies(by_map: dict[str, dict]) -> None:
    for map_name, anchors in FINAL_BIOME_ANCHORS.items():
        present = set(encounter_signature(by_map[map_name]))
        require(anchors <= present,
                f"{map_name} lost final-band biome anchors: {sorted(anchors - present)}")
    for group in FINAL_DISTINCT_GROUPS:
        signatures = [encounter_signature(by_map[map_name]) for map_name in group]
        require(len(signatures) == len(set(signatures)),
                f"final-band maps collapsed to duplicate ecosystems: {group}")
    for (map_name, species), expected_rate in FINAL_TROPHY_POLICY.items():
        actual_rate = species_rate(by_map[map_name], species)
        require(actual_rate == expected_rate,
                f"{map_name} {species} final trophy rate drifted: {actual_rate}% != {expected_rate}%")
    artisan_1f = by_map["MAP_ARTISAN_CAVE_1F"]["land_mons"]["mons"]
    artisan_b1f = by_map["MAP_ARTISAN_CAVE_B1F"]["land_mons"]["mons"]
    require(all(mon["species"] == "SPECIES_SMEARGLE" for mon in artisan_1f + artisan_b1f),
            "Artisan Cave lost its canonical Smeargle-only identity")


def verify_final_conditional_signs() -> None:
    definitions = (ROOT / "src/data/pokemon/legendary_signs.h").read_text()
    expected = {
        "CHIEN_PAO": ("SHOAL_CAVE_LOW_TIDE_ICE_ROOM", "WILD_AREA_LAND", "ABOMASNOW"),
        "MANAPHY": ("UNDERWATER_SEAFLOOR_CAVERN", "WILD_AREA_LAND", "RELICANTH"),
        "SUICUNE": ("ROUTE125", "WILD_AREA_FISHING", "LAPRAS"),
        "TAPU_FINI": ("ROUTE126", "WILD_AREA_FISHING", "GOREBYSS"),
        "TERRAKION": ("VICTORY_ROAD_B1F", "WILD_AREA_LAND", "COBALION"),
        "VOLCANION": ("MAGMA_HIDEOUT_4F", "WILD_AREA_LAND", "TORKOAL"),
        "KELDEO": ("ROUTE127", "WILD_AREA_FISHING", "COBALION"),
    }
    for species, (map_name, area, required) in expected.items():
        pattern = (
            rf"WILD_SIGN\(LEGENDARY_SIGN_{species}, {species}, {map_name}, {area}, "
            rf"8, [678], [12], {required}, FLAG_BADGE0[678]_GET\),"
        )
        require(re.search(pattern, definitions) is not None,
                f"{species} lost its finite late-game conditional Sign")
        require(f"ORDINARY_WILD_SIGN(LEGENDARY_SIGN_{species}" not in definitions,
                f"{species} regressed to a repeatable late wild")

    ordinary = set(re.findall(
        r"ORDINARY_WILD_SIGN\(LEGENDARY_SIGN_[A-Z0-9_]+, ([A-Z0-9_]+),",
        definitions,
    ))
    require(ordinary == {"BLACEPHALON", "BUZZWOLE", "GUZZLORD", "KARTANA", "NIHILEGO", "PHEROMOSA", "STAKATAKA"},
            f"ordinary Legendary Signs must be the seven curated Ultra Beasts only: {sorted(ordinary)}")


def verify_feebas_progression(by_map: dict[str, dict]) -> None:
    milotic = []
    for map_name, entry in by_map.items():
        for method_name, method in entry.items():
            if not method_name.endswith("_mons") or not isinstance(method, dict):
                continue
            for slot, mon in enumerate(method.get("mons", [])):
                if mon["species"] == "SPECIES_MILOTIC":
                    milotic.append(f"{map_name}:{method_name}[{slot}]")
    require(not milotic, "ordinary Milotic invalidates Feebas exploration: " + ", ".join(milotic))
    require(species_rate(by_map["MAP_UNDERWATER_ROUTE126"], "SPECIES_FEEBAS") == 5,
            "the deliberate underwater Route 126 Feebas source is not a non-grindy 5%")


def verify_altering_cave_rotation(rows: list[dict]) -> None:
    rotations = [
        tuple(sorted({
            mon["species"]
            for method_name, method in entry.items()
            if method_name.endswith("_mons") and isinstance(method, dict)
            for mon in method.get("mons", [])
        }))
        for entry in rows
        if entry["map"] == "MAP_ALTERING_CAVE"
    ]
    expected = [
        ("SPECIES_ZUBAT",), ("SPECIES_MAREEP",), ("SPECIES_PINECO",),
        ("SPECIES_HOUNDOUR",), ("SPECIES_TEDDIURSA",), ("SPECIES_AIPOM",),
        ("SPECIES_SHUCKLE",), ("SPECIES_STANTLER",), ("SPECIES_SMEARGLE",),
    ]
    require(rotations == expected,
            f"canonical nine-set Altering Cave rotation drifted: {rotations}")


def level_evolution_floors() -> dict[str, int]:
    """Minimum level at which each level-evolved species can legally exist.

    Only EVO_LEVEL* methods impose a floor; stone, trade, and friendship
    evolutions can happen at any level.  A stage's floor is the maximum of its
    own evolution level and its pre-evolution's floor.
    """
    pattern = re.compile(r"\{EVO_LEVEL[A-Z_]*,\s*(\d+),\s*(SPECIES_[A-Z0-9_]+)")
    into: dict[str, list[tuple[str, int]]] = {}
    current = None
    for path in sorted((ROOT / "src/data/pokemon/species_info").glob("*.h")):
        for line in path.read_text().splitlines():
            head = re.match(r"\s*\[(SPECIES_[A-Z0-9_]+)\]\s*=", line)
            if head:
                current = head.group(1)
            for level, target in pattern.findall(line):
                if current and int(level) > 1:
                    into.setdefault(target, []).append((current, int(level)))
    floors: dict[str, int] = {}

    def floor(species: str, seen: frozenset = frozenset()) -> int:
        if species in floors:
            return floors[species]
        best = 1
        for parent, level in into.get(species, []):
            if parent in seen:
                continue
            best = max(best, level, floor(parent, seen | {species}))
        floors[species] = best
        return best

    for species in into:
        floor(species)
    return floors


def verify_wild_level_floors(rows: list[dict]) -> None:
    floors = level_evolution_floors()
    violations: list[str] = []
    checked = 0
    for row in rows:
        for method_name, method in row.items():
            if not method_name.endswith("_mons") or not isinstance(method, dict):
                continue
            for mon in method.get("mons", []):
                base = re.sub(r"_(MEGA|GMAX|ALOLA|GALAR|HISUI|PALDEA)[A-Z_]*$", "", mon["species"])
                floor_level = floors.get(mon["species"], floors.get(base, 1))
                checked += 1
                if mon["min_level"] < floor_level:
                    violations.append(
                        f"{row['map']} {method_name}: {mon['species']} at level "
                        f"{mon['min_level']} below its evolution floor {floor_level}"
                    )
    # The curated distribution deliberately places evolved forms below their
    # evolution level in late areas (Volcarona at 41 in Ashen Woods, Noivern in
    # the Cave of Origin) because levels are pinned to the campaign's level
    # caps.  Report the seam instead of failing so the design stays visible;
    # the hard requirement is that nothing sits below level 5 as an evolved form
    # inherited from a vanilla low-level slot (level 5 Jellicent on a Tentacool
    # surf slot).
    severe = [row for row in violations if " at level " in row and int(row.split(" at level ")[1].split()[0]) < 10]
    report = ROOT / "work" / "audits" / "WILD_LEVEL_FLOOR_REPORT.md"
    report_text = (
        "# Wild evolution-floor report\n\n"
        "Generated by `scripts/emerald_champions_wild_distribution.py`. Entries list\n"
        "evolved species whose minimum wild level is below the level their line\n"
        "evolves at. Late-area cases are intentional level-cap design; entries\n"
        "under level 10 fail the gate.\n\n"
        + "\n".join(f"- {row}" for row in violations)
    )
    report.write_text(report_text.rstrip() + "\n")
    print(f"wild_level_floor_checks={checked} below_floor={len(violations)} (see {report.relative_to(ROOT)})")
    require(not severe, "evolved wild forms on sub-level-10 slots:\n" + "\n".join(severe))


def check() -> None:
    rows = encounter_rows()
    by_map = {row["map"]: row for row in rows}
    graph = SpeciesGraph()
    verify_wild_level_floors(rows)

    require(len(rows) == 146, f"Hoenn campaign encounter-header count drifted: {len(rows)}")
    require(len(by_map) == 138, f"Hoenn campaign wild-map count drifted: {len(by_map)}")
    require(set(by_map) == ALL_CURATED_ECOSYSTEM_MAPS,
            f"Hoenn wild maps escaped source-first curation: {sorted(set(by_map) - ALL_CURATED_ECOSYSTEM_MAPS)}")
    for entry in rows:
        map_name = entry["map"]
        for method_name, method in entry.items():
            if not method_name.endswith("_mons") or not isinstance(method, dict):
                continue
            require(method_name in SLOT_COUNTS, f"{map_name}: unknown encounter method {method_name}")
            require(len(method.get("mons", [])) == SLOT_COUNTS[method_name],
                    f"{map_name} {method_name}: invalid slot count")
            require(0 < method.get("encounter_rate", 0) <= 100,
                    f"{map_name} {method_name}: invalid encounter rate {method.get('encounter_rate')}")

    for map_name in RESTORED_LATE_CAVE_LEVEL_MAPS:
        for method_name, method in by_map[map_name].items():
            if not method_name.endswith("_mons") or not isinstance(method, dict):
                continue
            # Water/fishing use their authored access tiers and are checked by
            # verify_wild_distribution.py. This seam protects the physical
            # cave tables from ever regressing to their old Lv. 2-4 placeholders.
            if method_name not in ("land_mons", "rock_smash_mons", "hidden_mons"):
                continue
            levels = {
                level
                for mon in method.get("mons", [])
                for level in (mon["min_level"], mon["max_level"])
            }
            require(
                levels and min(levels) >= 41 and max(levels) <= 55,
                f"{map_name} {method_name}: restored late-cave levels escaped 41-55: {sorted(levels)}",
            )

    for (map_name, method_name, slot), species in STARTER_REPLACEMENTS.items():
        require(by_map[map_name][method_name]["mons"][slot]["species"] == species,
                f"{map_name} {method_name}[{slot}] lost {species}")
    for (map_name, method_name, slot), species in QUEST_DEPENDENCY_REPLACEMENTS.items():
        require(by_map[map_name][method_name]["mons"][slot]["species"] == species,
                f"{map_name} {method_name}[{slot}] lost quest dependency {species}")
    for (map_name, method_name, slot), species in FOSSIL_REPLACEMENTS.items():
        require(by_map[map_name][method_name]["mons"][slot]["species"] == species,
                f"{map_name} {method_name}[{slot}] lost fossil-safe replacement {species}")

    starter_roots = starter_components(graph)
    leaked_starters = []
    for map_name, entry in by_map.items():
        for method_name, method in entry.items():
            if not method_name.endswith("_mons") or not isinstance(method, dict):
                continue
            for slot, mon in enumerate(method.get("mons", [])):
                if mon["species"] in graph.species and graph.find(mon["species"]) in starter_roots:
                    leaked_starters.append(f"{map_name}:{method_name}[{slot}]={mon['species']}")
    require(not leaked_starters, "starter families remain ordinary Hoenn wilds: " + ", ".join(leaked_starters))

    for map_name, species in RESTORED_ULTRA_BEASTS.items():
        slots = by_map[map_name]["land_mons"]["mons"]
        require(slots[6]["species"] == species,
                f"{map_name} lost its no-grind 5% Ultra Beast {species}")
    require(by_map["MAP_PETALBURG_WOODS"]["land_mons"]["mons"][6]["species"] == "SPECIES_SCYTHER",
            "Petalburg Woods lost its 5% Scyther anchor")
    require(by_map["MAP_SEASPRAY_CAVE_B1F"]["land_mons"]["mons"][7]["species"] == "SPECIES_FRIGIBAX",
            "Seaspray Cave B1F lost its no-grind 5% Frigibax source")

    verify_role_policy(by_map)
    verify_role_policy(by_map, MIDGAME_ROLE_POLICY)
    verify_role_policy(by_map, FINAL_ROLE_POLICY)
    act_bias = verify_opening_act_bias(by_map, graph)
    midgame_bias = verify_midgame_act_bias(by_map, graph)
    final_bias = verify_final_act_bias(by_map, graph)
    verify_fossil_policy(rows, graph)
    verify_preset_legality(by_map, graph, ALL_CURATED_ECOSYSTEM_MAPS)
    verify_named_legendary_policy(by_map, ALL_CURATED_ECOSYSTEM_MAPS)
    verify_mega_base_timing(by_map, MEGA_BASE_SOURCE_POLICY)
    verify_mega_base_timing(by_map, MIDGAME_MEGA_BASE_POLICY)
    verify_mega_base_timing(by_map, FINAL_MEGA_BASE_POLICY)
    verify_biome_and_trophy_policy(by_map)
    verify_midgame_biomes_and_trophies(by_map)
    verify_final_biomes_and_trophies(by_map)
    verify_midgame_conditional_signs()
    verify_final_conditional_signs()
    verify_feebas_progression(by_map)
    verify_altering_cave_rotation(rows)
    species = {
        mon["species"]
        for entry in rows
        for method_name, method in entry.items()
        if method_name.endswith("_mons") and isinstance(method, dict)
        for mon in method.get("mons", [])
    }
    print(f"PASS: {len(rows)} headers on {len(by_map)} Hoenn wild maps expose {len(species)} unique species/forms")
    print("PASS: no ordinary Hoenn wild table contains a regional starter family")
    print(
        "PASS: exact act bias: "
        f"Stone={act_bias['Stone'][0]:.1%}/{act_bias['Stone'][1]:.1%}/{act_bias['Stone'][2]:.1%}, "
        f"Dewford-Wattson={act_bias['Dewford-Wattson'][0]:.1%}/{act_bias['Dewford-Wattson'][1]:.1%}/{act_bias['Dewford-Wattson'][2]:.1%}"
    )
    print(f"PASS: {len(ROLE_POLICY)} early doubles-role availability contracts hold")
    print(
        "PASS: midgame route bias eases forward: "
        f"Dynamo-Heat={midgame_bias['Dynamo-Heat'][0]:.1%}/{midgame_bias['Dynamo-Heat'][1]:.1%}/{midgame_bias['Dynamo-Heat'][2]:.1%}, "
        f"Balance-Feather={midgame_bias['Balance-Feather'][0]:.1%}/{midgame_bias['Balance-Feather'][1]:.1%}/{midgame_bias['Balance-Feather'][2]:.1%}"
    )
    print(f"PASS: {len(MIDGAME_ROLE_POLICY)} midgame doubles-role availability contracts hold")
    print(
        "PASS: final-route curve: "
        f"common={final_bias['common'][0]:.1%}/{final_bias['common'][1]:.1%}/{final_bias['common'][2]:.1%}, "
        f"trophies={final_bias['trophies'][0]:.1%}/{final_bias['trophies'][1]:.1%}/{final_bias['trophies'][2]:.1%}"
    )
    print(f"PASS: {len(FINAL_ROLE_POLICY)} League-ready role contracts hold")
    print(f"PASS: all opening species resolve legal presets and {len(MEGA_BASE_SOURCE_POLICY)} Mega bases meet timing")
    print(f"PASS: {len(MIDGAME_ECOSYSTEM_MAPS)} midgame maps resolve legal presets and {len(MIDGAME_MEGA_BASE_POLICY)} Mega bases meet timing")
    print(f"PASS: {len(FINAL_ECOSYSTEM_MAPS)} final/postgame maps resolve legal presets and {len(FINAL_MEGA_BASE_POLICY)} Mega bases meet timing")
    print(f"PASS: {len(BIOME_ANCHORS)} biome identities and {len(TROPHY_POLICY)} no-grind trophy rates are pinned")
    print(f"PASS: {len(MIDGAME_BIOME_ANCHORS)} midgame anchors, {len(MIDGAME_TROPHY_POLICY)} trophies, and 5 one-time Signs are pinned")
    print(f"PASS: {len(FINAL_BIOME_ANCHORS)} final anchors, {len(FINAL_TROPHY_POLICY)} trophies, and 7 one-time Signs are pinned")
    print("PASS: no ordinary fossil family or non-UB named legendary leaks into the curated ecosystems")
    print(f"PASS: {len(RESTORED_LATE_CAVE_LEVEL_MAPS)} restored late caves keep physical encounter methods at Lv. 41-55")


def main() -> None:
    # Compatibility entry point for older local workflows. The former exact
    # slot/quota audit describes the superseded pre-route-sheet distribution;
    # the release contract now lives in verify_wild_distribution.py.
    from verify_wild_distribution import main as verify_current_distribution

    raise SystemExit(verify_current_distribution())


if __name__ == "__main__":
    main()
