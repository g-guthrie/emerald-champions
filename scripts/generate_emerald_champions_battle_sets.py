#!/usr/bin/env python3
"""Generate Champions-native runtime presets from the preserved authored corpus."""

from __future__ import annotations

import json
import re
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
SOURCE_COMMIT = "33202c162ebc34a1dbe2000acd26b0720baa109d"
DEFAULT_SOURCE = "docs/verdant_battle_set_presets.json"
ALTERNATIVE_SOURCE = "docs/verdant_multi_battle_sets.json"
HANDBOOK_SOURCE = "docs/pokemon_champions_handbook_sets.json"
JSON_OUTPUT = ROOT / "docs" / "emerald_champions_battle_sets.json"
C_OUTPUT = ROOT / "src" / "data" / "pokemon" / "emerald_champions_battle_sets.h"

PHYSICAL_NATURES = {
    "NATURE_ADAMANT", "NATURE_BRAVE", "NATURE_JOLLY", "NATURE_LONELY",
    "NATURE_NAUGHTY", "NATURE_IMPISH", "NATURE_RELAXED", "NATURE_LAX",
}
SPECIAL_NATURES = {
    "NATURE_MODEST", "NATURE_QUIET", "NATURE_TIMID", "NATURE_MILD",
    "NATURE_RASH", "NATURE_BOLD", "NATURE_CALM", "NATURE_CAREFUL",
    "NATURE_SASSY", "NATURE_GENTLE",
}
PHYSICAL_WALL_NATURES = {"NATURE_BOLD", "NATURE_IMPISH", "NATURE_LAX", "NATURE_RELAXED"}
SPECIAL_WALL_NATURES = {"NATURE_CALM", "NATURE_CAREFUL", "NATURE_GENTLE", "NATURE_SASSY"}

PHYSICAL_MOVE_WORDS = {
    "PUNCH", "KICK", "BLADE", "FANG", "CLAW", "SLASH", "TACKLE", "HEADBUTT",
    "EARTHQUAKE", "ROCK_SLIDE", "CLOSE_COMBAT", "KNOCK_OFF", "PLAY_ROUGH",
    "FLARE_BLITZ", "BRAVE_BIRD", "WATERFALL", "LIQUIDATION", "IRON_HEAD",
    "SUCKER_PUNCH", "EXTREME_SPEED", "U_TURN", "KOWTOW", "BODY_PRESS",
}
SPECIAL_MOVE_WORDS = {
    "BEAM", "BLAST", "PULSE", "WAVE", "BOLT", "THUNDER", "FLAMETHROWER",
    "SCALD", "SURF", "HYDRO_PUMP", "ENERGY_BALL", "SHADOW_BALL", "PSYCHIC",
    "MOONBLAST", "DAZZLING_GLEAM", "EARTH_POWER", "SLUDGE_BOMB", "HEAT_WAVE",
    "HYPER_VOICE", "ICY_WIND", "ELECTROWEB", "MAKE_IT_RAIN",
}

SPECIES_ALIASES = {
    "SPECIES_PIKACHU_PH_D": "SPECIES_PIKACHU_PHD",
    "SPECIES_BURMY_SANDY_CLOAK": "SPECIES_BURMY_SANDY",
    "SPECIES_BURMY_TRASH_CLOAK": "SPECIES_BURMY_TRASH",
    "SPECIES_WORMADAM_SANDY_CLOAK": "SPECIES_WORMADAM_SANDY",
    "SPECIES_WORMADAM_TRASH_CLOAK": "SPECIES_WORMADAM_TRASH",
    "SPECIES_SHELLOS_EAST_SEA": "SPECIES_SHELLOS_EAST",
    "SPECIES_GASTRODON_EAST_SEA": "SPECIES_GASTRODON_EAST",
    "SPECIES_VIVILLON_POKE_BALL": "SPECIES_VIVILLON_POKEBALL",
    "SPECIES_FLOETTE_ETERNAL_FLOWER": "SPECIES_FLOETTE_ETERNAL",
    "SPECIES_MEOWSTIC_FEMALE": "SPECIES_MEOWSTIC_F",
    "SPECIES_MAGEARNA_ORIGINAL_COLOR": "SPECIES_MAGEARNA_ORIGINAL",
    "SPECIES_INDEEDEE_FEMALE": "SPECIES_INDEEDEE_F",
    "SPECIES_ZACIAN_CROWNED_SWORD": "SPECIES_ZACIAN_CROWNED",
    "SPECIES_ZAMAZENTA_CROWNED_SHIELD": "SPECIES_ZAMAZENTA_CROWNED",
    "SPECIES_URSHIFU_RAPID_STRIKE_STYLE": "SPECIES_URSHIFU_RAPID_STRIKE",
    "SPECIES_CALYREX_ICE_RIDER": "SPECIES_CALYREX_ICE",
    "SPECIES_CALYREX_SHADOW_RIDER": "SPECIES_CALYREX_SHADOW",
}

ABILITY_ALIASES = {
    ("SPECIES_EXEGGCUTE", "ABILITY_CHLOROPLAST"): "ABILITY_CHLOROPHYLL",
    ("SPECIES_EXEGGUTOR_ALOLA", "ABILITY_CHLOROPLAST"): "ABILITY_HARVEST",
    ("SPECIES_FENNEKIN", "ABILITY_PYROMANCY"): "ABILITY_BLAZE",
    ("SPECIES_BRAIXEN", "ABILITY_PYROMANCY"): "ABILITY_BLAZE",
    ("SPECIES_DELPHOX", "ABILITY_PYROMANCY"): "ABILITY_BLAZE",
    ("SPECIES_HITMONCHAN", "ABILITY_BLITZ_BOXER"): "ABILITY_IRON_FIST",
    ("SPECIES_SQUIRTLE", "ABILITY_OVERCOAT"): "ABILITY_RAIN_DISH",
    ("SPECIES_BLASTOISE", "ABILITY_MEGA_LAUNCHER"): "ABILITY_RAIN_DISH",
    ("SPECIES_PIDGEY", "ABILITY_NO_GUARD"): "ABILITY_BIG_PECKS",
    ("SPECIES_PIDGEOTTO", "ABILITY_NO_GUARD"): "ABILITY_BIG_PECKS",
    ("SPECIES_PIDGEOT", "ABILITY_NO_GUARD"): "ABILITY_BIG_PECKS",
    ("SPECIES_FEAROW", "ABILITY_INTIMIDATE"): "ABILITY_SNIPER",
    ("SPECIES_PONYTA", "ABILITY_RECKLESS"): "ABILITY_FLAME_BODY",
    ("SPECIES_RAPIDASH", "ABILITY_RECKLESS"): "ABILITY_FLAME_BODY",
    ("SPECIES_DODRIO", "ABILITY_MOXIE"): "ABILITY_TANGLED_FEET",
    ("SPECIES_SEEL", "ABILITY_FUR_COAT"): "ABILITY_ICE_BODY",
    ("SPECIES_DEWGONG", "ABILITY_FUR_COAT"): "ABILITY_ICE_BODY",
    ("SPECIES_GENGAR", "ABILITY_LEVITATE"): "ABILITY_CURSED_BODY",
    ("SPECIES_ELECTRODE", "ABILITY_ELECTRIC_SURGE"): "ABILITY_AFTERMATH",
    ("SPECIES_CHIKORITA", "ABILITY_TRIAGE"): "ABILITY_LEAF_GUARD",
    ("SPECIES_BAYLEEF", "ABILITY_TRIAGE"): "ABILITY_LEAF_GUARD",
    ("SPECIES_MEGANIUM", "ABILITY_TRIAGE"): "ABILITY_LEAF_GUARD",
    ("SPECIES_SENTRET", "ABILITY_FUR_COAT"): "ABILITY_FRISK",
    ("SPECIES_FURRET", "ABILITY_FUR_COAT"): "ABILITY_FRISK",
    ("SPECIES_LEDYBA", "ABILITY_AERILATE"): "ABILITY_RATTLED",
    ("SPECIES_SPINARAK", "ABILITY_MERCILESS"): "ABILITY_SNIPER",
    ("SPECIES_SUNFLORA", "ABILITY_DROUGHT"): "ABILITY_EARLY_BIRD",
    ("SPECIES_GIRAFARIG", "ABILITY_STRONG_JAW"): "ABILITY_SAP_SIPPER",
    ("SPECIES_MAGCARGO", "ABILITY_SIMPLE"): "ABILITY_WEAK_ARMOR",
    ("SPECIES_DELIBIRD", "ABILITY_REFRIGERATE"): "ABILITY_INSOMNIA",
    ("SPECIES_WURMPLE", "ABILITY_POISON_POINT"): "ABILITY_RUN_AWAY",
    ("SPECIES_BEAUTIFLY", "ABILITY_BERSERK"): "ABILITY_RIVALRY",
    ("SPECIES_DUSTOX", "ABILITY_UNAWARE"): "ABILITY_COMPOUND_EYES",
    ("SPECIES_SLAKOTH", "ABILITY_STALL"): "ABILITY_TRUANT",
    ("SPECIES_WAILORD", "ABILITY_DRIZZLE"): "ABILITY_PRESSURE",
    ("SPECIES_FLYGON", "ABILITY_TINTED_LENS"): "ABILITY_LEVITATE",
    ("SPECIES_TROPIUS", "ABILITY_AERILATE"): "ABILITY_HARVEST",
    ("SPECIES_GLALIE", "ABILITY_REFRIGERATE"): "ABILITY_MOODY",
    ("SPECIES_LUVDISC", "ABILITY_SOUL_HEART"): "ABILITY_HYDRATION",
    ("SPECIES_TURTWIG", "ABILITY_SOLID_ROCK"): "ABILITY_SHELL_ARMOR",
    ("SPECIES_GROTLE", "ABILITY_SOLID_ROCK"): "ABILITY_SHELL_ARMOR",
    ("SPECIES_TORTERRA", "ABILITY_SOLID_ROCK"): "ABILITY_SHELL_ARMOR",
    ("SPECIES_VESPIQUEN", "ABILITY_INTIMIDATE"): "ABILITY_UNNERVE",
    ("SPECIES_LOPUNNY", "ABILITY_SCRAPPY"): "ABILITY_LIMBER",
    ("SPECIES_MISMAGIUS", "ABILITY_PIXILATE"): "ABILITY_LEVITATE",
    ("SPECIES_MAGMORTAR", "ABILITY_MEGA_LAUNCHER"): "ABILITY_VITAL_SPIRIT",
    ("SPECIES_GLACEON", "ABILITY_SLUSH_RUSH"): "ABILITY_ICE_BODY",
    ("SPECIES_REGIGIGAS", "ABILITY_CLEAR_BODY"): "ABILITY_SLOW_START",
    ("SPECIES_WATCHOG", "ABILITY_DAZZLING"): "ABILITY_ANALYTIC",
    ("SPECIES_EMOLGA", "ABILITY_LIGHTNING_ROD"): "ABILITY_MOTOR_DRIVE",
    ("SPECIES_HEATMOR", "ABILITY_TOUGH_CLAWS"): "ABILITY_WHITE_SMOKE",
    ("SPECIES_PYROAR", "ABILITY_COMPETITIVE"): "ABILITY_MOXIE",
    ("SPECIES_FLABEBE_RED", "ABILITY_HEALER"): "ABILITY_SYMBIOSIS",
    ("SPECIES_FLABEBE", "ABILITY_HEALER"): "ABILITY_SYMBIOSIS",
    ("SPECIES_FLOETTE_RED", "ABILITY_HEALER"): "ABILITY_SYMBIOSIS",
    ("SPECIES_FLOETTE", "ABILITY_HEALER"): "ABILITY_SYMBIOSIS",
    ("SPECIES_FLORGES_RED", "ABILITY_MISTY_SURGE"): "ABILITY_SYMBIOSIS",
    ("SPECIES_FLORGES", "ABILITY_MISTY_SURGE"): "ABILITY_SYMBIOSIS",
    ("SPECIES_GOGOAT", "ABILITY_GRASSY_SURGE"): "ABILITY_GRASS_PELT",
    ("SPECIES_GOODRA", "ABILITY_POISON_HEAL"): "ABILITY_GOOEY",
    ("SPECIES_TREVENANT", "ABILITY_GRASSY_SURGE"): "ABILITY_HARVEST",
    ("SPECIES_GOURGEIST_AVERAGE", "ABILITY_FLARE_BOOST"): "ABILITY_INSOMNIA",
    ("SPECIES_GOURGEIST", "ABILITY_FLARE_BOOST"): "ABILITY_INSOMNIA",
    ("SPECIES_ROWLET", "ABILITY_TINTED_LENS"): "ABILITY_LONG_REACH",
    ("SPECIES_DARTRIX", "ABILITY_TINTED_LENS"): "ABILITY_LONG_REACH",
    ("SPECIES_DECIDUEYE", "ABILITY_TINTED_LENS"): "ABILITY_LONG_REACH",
    ("SPECIES_PALOSSAND", "ABILITY_SAND_STREAM"): "ABILITY_SAND_VEIL",
    ("SPECIES_ROTOM_FAN", "ABILITY_MOTOR_DRIVE"): "ABILITY_LEVITATE",
    ("SPECIES_GOURGEIST_SMALL", "ABILITY_FLARE_BOOST"): "ABILITY_INSOMNIA",
    ("SPECIES_GOURGEIST_LARGE", "ABILITY_FLARE_BOOST"): "ABILITY_INSOMNIA",
    ("SPECIES_GOURGEIST_SUPER", "ABILITY_FLARE_BOOST"): "ABILITY_INSOMNIA",
}

HANDBOOK_FORM_ROLES = {
    "Arcanine-Hisui": "SPECIES_ARCANINE_HISUI",
    "Typhlosion-Hisui": "SPECIES_TYPHLOSION_HISUI",
    "Samurott-Hisui": "SPECIES_SAMUROTT_HISUI",
    "Zoroark-Hisui": "SPECIES_ZOROARK_HISUI",
    "Goodra-Hisui": "SPECIES_GOODRA_HISUI",
    "Avalugg-Hisui": "SPECIES_AVALUGG_HISUI",
    "Decidueye-Hisui": "SPECIES_DECIDUEYE_HISUI",
    "Basculegion-F": "SPECIES_BASCULEGION_F",
}

# The preserved handbook predates the Quaxly family.  These three sets close
# the only hole in the nine-generation starter roster and deliberately follow
# the same doubles-first, 66-Stat-Point contract as the authored corpus.
SUPPLEMENTAL_DEFAULTS = [
    {
        "species": "SPECIES_QUAXLY",
        "name": "Rapid Spin Support",
        "moves": ["MOVE_LIQUIDATION", "MOVE_AQUA_JET", "MOVE_RAPID_SPIN", "MOVE_PROTECT"],
        "nature": "NATURE_JOLLY",
        "ability": "ABILITY_MOXIE",
        "item": "ITEM_EVIOLITE",
        "required_item": "ITEM_NONE",
        "stat_points": [2, 32, 0, 0, 0, 32],
        "role": "Fast physical support",
        "source": "modern-expansion-supplement",
    },
    {
        "species": "SPECIES_QUAXWELL",
        "name": "Bulky Moxie",
        "moves": ["MOVE_LIQUIDATION", "MOVE_LOW_SWEEP", "MOVE_ROOST", "MOVE_PROTECT"],
        "nature": "NATURE_JOLLY",
        "ability": "ABILITY_MOXIE",
        "item": "ITEM_EVIOLITE",
        "required_item": "ITEM_NONE",
        "stat_points": [2, 32, 0, 0, 0, 32],
        "role": "Physical tempo attacker",
        "source": "modern-expansion-supplement",
    },
    {
        "species": "SPECIES_QUAQUAVAL",
        "name": "Moxie Dancer",
        "moves": ["MOVE_AQUA_STEP", "MOVE_CLOSE_COMBAT", "MOVE_KNOCK_OFF", "MOVE_DETECT"],
        "nature": "NATURE_JOLLY",
        "ability": "ABILITY_MOXIE",
        "item": "ITEM_CLEAR_AMULET",
        "required_item": "ITEM_NONE",
        "stat_points": [2, 32, 0, 0, 0, 32],
        "role": "Showdown offensive Protect attacker",
        "source": "Showdown Champions plus modern expansion",
    },
    {
        "species": "SPECIES_ENAMORUS",
        "name": "Contrary Attacker",
        "moves": ["MOVE_SPRINGTIDE_STORM", "MOVE_EARTH_POWER", "MOVE_MYSTICAL_FIRE", "MOVE_PROTECT"],
        "nature": "NATURE_TIMID",
        "ability": "ABILITY_CONTRARY",
        "item": "ITEM_LIFE_ORB",
        "required_item": "ITEM_NONE",
        "stat_points": [2, 0, 0, 32, 0, 32],
        "role": "Contrary special attacker",
        "source": "modern-expansion-supplement",
    },
    {
        "species": "SPECIES_FEZANDIPITI",
        "name": "Toxic Wind Support",
        "moves": ["MOVE_MOONBLAST", "MOVE_ICY_WIND", "MOVE_TAILWIND", "MOVE_PROTECT"],
        "nature": "NATURE_TIMID",
        "ability": "ABILITY_TOXIC_CHAIN",
        "item": "ITEM_COVERT_CLOAK",
        "required_item": "ITEM_NONE",
        "stat_points": [32, 0, 2, 0, 32, 0],
        "role": "Toxic Chain speed support",
        "source": "modern-expansion-supplement",
    },
    {
        "species": "SPECIES_KORAIDON",
        "name": "Sun Vanguard",
        "moves": ["MOVE_COLLISION_COURSE", "MOVE_DRAGON_CLAW", "MOVE_FLARE_BLITZ", "MOVE_PROTECT"],
        "nature": "NATURE_JOLLY",
        "ability": "ABILITY_ORICHALCUM_PULSE",
        "item": "ITEM_CLEAR_AMULET",
        "required_item": "ITEM_NONE",
        "stat_points": [2, 32, 0, 0, 0, 32],
        "role": "Orichalcum Pulse physical attacker",
        "source": "modern-expansion-supplement",
    },
    {
        "species": "SPECIES_MIRAIDON",
        "name": "Terrain Cannon",
        "moves": ["MOVE_ELECTRO_DRIFT", "MOVE_DRACO_METEOR", "MOVE_VOLT_SWITCH", "MOVE_PROTECT"],
        "nature": "NATURE_TIMID",
        "ability": "ABILITY_HADRON_ENGINE",
        "item": "ITEM_LIFE_ORB",
        "required_item": "ITEM_NONE",
        "stat_points": [2, 0, 0, 32, 0, 32],
        "role": "Hadron Engine special attacker",
        "source": "modern-expansion-supplement",
    },
    {
        "species": "SPECIES_MUNKIDORI",
        "name": "Toxic Pivot",
        "moves": ["MOVE_PSYCHIC", "MOVE_SLUDGE_BOMB", "MOVE_PARTING_SHOT", "MOVE_PROTECT"],
        "nature": "NATURE_TIMID",
        "ability": "ABILITY_TOXIC_CHAIN",
        "item": "ITEM_FOCUS_SASH",
        "required_item": "ITEM_NONE",
        "stat_points": [2, 0, 0, 32, 0, 32],
        "role": "Toxic Chain special pivot",
        "source": "modern-expansion-supplement",
    },
    {
        "species": "SPECIES_OKIDOGI",
        "name": "Toxic Bruiser",
        "moves": ["MOVE_DRAIN_PUNCH", "MOVE_POISON_JAB", "MOVE_KNOCK_OFF", "MOVE_PROTECT"],
        "nature": "NATURE_ADAMANT",
        "ability": "ABILITY_TOXIC_CHAIN",
        "item": "ITEM_CLEAR_AMULET",
        "required_item": "ITEM_NONE",
        "stat_points": [32, 32, 2, 0, 0, 0],
        "role": "Toxic Chain physical bruiser",
        "source": "modern-expansion-supplement",
    },
    {
        "species": "SPECIES_PECHARUNT",
        "name": "Poison Puppeteer",
        "moves": ["MOVE_MALIGNANT_CHAIN", "MOVE_SHADOW_BALL", "MOVE_RECOVER", "MOVE_PROTECT"],
        "nature": "NATURE_BOLD",
        "ability": "ABILITY_POISON_PUPPETEER",
        "item": "ITEM_LEFTOVERS",
        "required_item": "ITEM_NONE",
        "stat_points": [32, 0, 32, 0, 2, 0],
        "role": "Poison Puppeteer control wall",
        "source": "modern-expansion-supplement",
    },
    {
        "species": "SPECIES_TERAPAGOS",
        "name": "Crystal Anchor",
        "moves": ["MOVE_TERA_STARSTORM", "MOVE_EARTH_POWER", "MOVE_CALM_MIND", "MOVE_PROTECT"],
        "nature": "NATURE_MODEST",
        "ability": "ABILITY_TERA_SHIFT",
        "item": "ITEM_LEFTOVERS",
        "required_item": "ITEM_NONE",
        "stat_points": [32, 0, 2, 32, 0, 0],
        "role": "bulky crystal setup attacker",
        "source": "modern-expansion-supplement",
    },
    {
        "species": "SPECIES_WO_CHIEN",
        "name": "Tablets Wall",
        "moves": ["MOVE_GIGA_DRAIN", "MOVE_FOUL_PLAY", "MOVE_LEECH_SEED", "MOVE_PROTECT"],
        "nature": "NATURE_CALM",
        "ability": "ABILITY_TABLETS_OF_RUIN",
        "item": "ITEM_LEFTOVERS",
        "required_item": "ITEM_NONE",
        "stat_points": [32, 0, 2, 0, 32, 0],
        "role": "Tablets of Ruin special wall",
        "source": "modern-expansion-supplement",
    },
]


def authored_modern_set(
    species: str,
    name: str,
    moves: list[str],
    nature: str,
    ability: str,
    item: str,
    stat_points: list[int],
    role: str,
) -> dict:
    return {
        "species": species,
        "name": name,
        "moves": moves,
        "nature": nature,
        "ability": ability,
        "item": item,
        "required_item": "ITEM_NONE",
        "stat_points": stat_points,
        "role": role,
        "source": "Emerald Champions authored modern doubles supplement",
    }


# Modern and regional roots added to the campaign distribution after the
# original 1,025-species handbook was frozen.  Every one receives two distinct
# legal doubles roles instead of a generated placeholder.
SUPPLEMENTAL_DEFAULTS.extend([
    authored_modern_set("SPECIES_BASCULIN_WHITE_STRIPED", "Adaptability Breaker", ["MOVE_WAVE_CRASH", "MOVE_AQUA_JET", "MOVE_LAST_RESPECTS", "MOVE_PROTECT"], "NATURE_JOLLY", "ABILITY_ADAPTABILITY", "ITEM_LIFE_ORB", [2, 32, 0, 0, 0, 32], "fast physical breaker and priority cleaner"),
    authored_modern_set("SPECIES_CAPSAKID", "Rage Powder Support", ["MOVE_SEED_BOMB", "MOVE_RAGE_POWDER", "MOVE_HELPING_HAND", "MOVE_PROTECT"], "NATURE_CAREFUL", "ABILITY_CHLOROPHYLL", "ITEM_EVIOLITE", [32, 0, 2, 0, 32, 0], "sun-enabled redirection and partner support"),
    authored_modern_set("SPECIES_CHARCADET", "Flame Body Control", ["MOVE_HEAT_WAVE", "MOVE_WILL_O_WISP", "MOVE_CLEAR_SMOG", "MOVE_PROTECT"], "NATURE_BOLD", "ABILITY_FLAME_BODY", "ITEM_EVIOLITE", [32, 0, 32, 0, 2, 0], "burn and setup-control support"),
    authored_modern_set("SPECIES_FLITTLE", "Speed Boost Hypnosis", ["MOVE_PSYCHIC", "MOVE_HYPNOSIS", "MOVE_HELPING_HAND", "MOVE_PROTECT"], "NATURE_TIMID", "ABILITY_SPEED_BOOST", "ITEM_FOCUS_SASH", [2, 0, 0, 32, 0, 32], "fast sleep and partner support"),
    authored_modern_set("SPECIES_GREAVARD", "Fluffy Priority", ["MOVE_POLTERGEIST", "MOVE_SHADOW_SNEAK", "MOVE_PLAY_ROUGH", "MOVE_PROTECT"], "NATURE_ADAMANT", "ABILITY_FLUFFY", "ITEM_EVIOLITE", [32, 32, 2, 0, 0, 0], "bulky physical attacker and priority cleaner"),
    authored_modern_set("SPECIES_GROWLITHE_HISUI", "Rock Head Breaker", ["MOVE_FLARE_BLITZ", "MOVE_HEAD_SMASH", "MOVE_CLOSE_COMBAT", "MOVE_PROTECT"], "NATURE_JOLLY", "ABILITY_ROCK_HEAD", "ITEM_LIFE_ORB", [2, 32, 0, 0, 0, 32], "recoil-free physical wallbreaker"),
    authored_modern_set("SPECIES_ORTHWORM", "Earth Eater Body Press", ["MOVE_BODY_PRESS", "MOVE_SHED_TAIL", "MOVE_IRON_DEFENSE", "MOVE_PROTECT"], "NATURE_RELAXED", "ABILITY_EARTH_EATER", "ITEM_SITRUS_BERRY", [32, 0, 32, 0, 2, 0], "physical wall, pivot, and Body Press win condition"),
    authored_modern_set("SPECIES_POLTCHAGEIST", "Hospitality Trick Room", ["MOVE_LEAF_STORM", "MOVE_RAGE_POWDER", "MOVE_TRICK_ROOM", "MOVE_PROTECT"], "NATURE_QUIET", "ABILITY_HOSPITALITY", "ITEM_EVIOLITE", [32, 0, 2, 32, 0, 0], "Trick Room redirection support"),
    authored_modern_set("SPECIES_QWILFISH_HISUI", "Intimidate Utility", ["MOVE_BARB_BARRAGE", "MOVE_CRUNCH", "MOVE_ICY_WIND", "MOVE_PROTECT"], "NATURE_CAREFUL", "ABILITY_INTIMIDATE", "ITEM_EVIOLITE", [32, 0, 2, 0, 32, 0], "Intimidate status and speed control"),
    authored_modern_set("SPECIES_SNEASEL_HISUI", "Inner Focus Opener", ["MOVE_CLOSE_COMBAT", "MOVE_POISON_JAB", "MOVE_FAKE_OUT", "MOVE_PROTECT"], "NATURE_JOLLY", "ABILITY_INNER_FOCUS", "ITEM_FOCUS_SASH", [2, 32, 0, 0, 0, 32], "fast Fake Out physical attacker"),
    authored_modern_set("SPECIES_TADBULB", "Damp Tempo Control", ["MOVE_ELECTROWEB", "MOVE_ACID_SPRAY", "MOVE_EERIE_IMPULSE", "MOVE_PROTECT"], "NATURE_CALM", "ABILITY_DAMP", "ITEM_EVIOLITE", [32, 0, 2, 0, 32, 0], "speed, damage, and special-attack control"),
    authored_modern_set("SPECIES_TANDEMAUS", "Population Support", ["MOVE_POPULATION_BOMB", "MOVE_SUPER_FANG", "MOVE_HELPING_HAND", "MOVE_PROTECT"], "NATURE_JOLLY", "ABILITY_OWN_TEMPO", "ITEM_EVIOLITE", [2, 32, 0, 0, 0, 32], "chip damage and partner amplification"),
    authored_modern_set("SPECIES_TAUROS_PALDEA_COMBAT", "Combat Intimidator", ["MOVE_CLOSE_COMBAT", "MOVE_RAGING_BULL", "MOVE_HIGH_HORSEPOWER", "MOVE_PROTECT"], "NATURE_JOLLY", "ABILITY_INTIMIDATE", "ITEM_CLEAR_AMULET", [2, 32, 0, 0, 0, 32], "fast Intimidate physical attacker"),
    authored_modern_set("SPECIES_TAUROS_PALDEA_BLAZE", "Blaze Intimidator", ["MOVE_RAGING_BULL", "MOVE_CLOSE_COMBAT", "MOVE_HIGH_HORSEPOWER", "MOVE_PROTECT"], "NATURE_JOLLY", "ABILITY_INTIMIDATE", "ITEM_CLEAR_AMULET", [2, 32, 0, 0, 0, 32], "fast Fire and Fighting physical attacker"),
    authored_modern_set("SPECIES_TAUROS_PALDEA_AQUA", "Aqua Intimidator", ["MOVE_WAVE_CRASH", "MOVE_CLOSE_COMBAT", "MOVE_AQUA_JET", "MOVE_PROTECT"], "NATURE_JOLLY", "ABILITY_INTIMIDATE", "ITEM_CLEAR_AMULET", [2, 32, 0, 0, 0, 32], "fast Water and Fighting priority attacker"),
    authored_modern_set("SPECIES_TINKATINK", "Mold Breaker Opener", ["MOVE_FAKE_OUT", "MOVE_KNOCK_OFF", "MOVE_PLAY_ROUGH", "MOVE_PROTECT"], "NATURE_CAREFUL", "ABILITY_MOLD_BREAKER", "ITEM_EVIOLITE", [32, 0, 2, 0, 32, 0], "Fake Out and item-removal support"),
    authored_modern_set("SPECIES_ZORUA_HISUI", "Illusion Burn Attacker", ["MOVE_BITTER_MALICE", "MOVE_HYPER_VOICE", "MOVE_WILL_O_WISP", "MOVE_PROTECT"], "NATURE_TIMID", "ABILITY_ILLUSION", "ITEM_FOCUS_SASH", [2, 0, 0, 32, 0, 32], "Illusion special attacker and burn control"),
])

# Regional and battle-distinct forms not separated into their own National
# Dex entries in the handbook still need an exact legal doubles identity.
SUPPLEMENTAL_DEFAULTS.extend([
    authored_modern_set("SPECIES_VOLTORB_HISUI", "Fast Seed Support", ["MOVE_ELECTROWEB", "MOVE_GIGA_DRAIN", "MOVE_LEECH_SEED", "MOVE_PROTECT"], "NATURE_TIMID", "ABILITY_SOUNDPROOF", "ITEM_EVIOLITE", [2, 0, 0, 32, 0, 32], "fast Grass and Electric support"),
    authored_modern_set("SPECIES_ELECTRODE_HISUI", "Chloroblast Pivot", ["MOVE_CHLOROBLAST", "MOVE_ELECTROWEB", "MOVE_VOLT_SWITCH", "MOVE_TAUNT"], "NATURE_TIMID", "ABILITY_SOUNDPROOF", "ITEM_FOCUS_SASH", [2, 0, 0, 32, 0, 32], "fast special pivot and speed control"),
    authored_modern_set("SPECIES_LILLIGANT_HISUI", "Victory Dance", ["MOVE_VICTORY_DANCE", "MOVE_CLOSE_COMBAT", "MOVE_LEAF_BLADE", "MOVE_SLEEP_POWDER"], "NATURE_JOLLY", "ABILITY_CHLOROPHYLL", "ITEM_FOCUS_SASH", [2, 32, 0, 0, 0, 32], "sleep pressure and physical setup"),
    authored_modern_set("SPECIES_BRAVIARY_HISUI", "Tinted Lens", ["MOVE_ESPER_WING", "MOVE_AIR_SLASH", "MOVE_HEAT_WAVE", "MOVE_PROTECT"], "NATURE_MODEST", "ABILITY_TINTED_LENS", "ITEM_LIFE_ORB", [2, 0, 0, 32, 0, 32], "Tinted Lens special attacker"),
    authored_modern_set("SPECIES_SLIGGOO_HISUI", "Shelter Press", ["MOVE_BODY_PRESS", "MOVE_HEAVY_SLAM", "MOVE_SHELTER", "MOVE_PROTECT"], "NATURE_RELAXED", "ABILITY_SHELL_ARMOR", "ITEM_EVIOLITE", [32, 0, 32, 0, 2, 0], "Eviolite Body Press win condition"),
    authored_modern_set("SPECIES_URSALUNA_BLOODMOON", "Blood Moon Voice", ["MOVE_BLOOD_MOON", "MOVE_EARTH_POWER", "MOVE_HYPER_VOICE", "MOVE_PROTECT"], "NATURE_QUIET", "ABILITY_MINDS_EYE", "ITEM_THROAT_SPRAY", [32, 0, 2, 32, 0, 0], "slow special wallbreaker"),
])


SUPPLEMENTAL_ALTERNATIVES = [
    authored_modern_set("SPECIES_BASCULIN_WHITE_STRIPED", "Scarf Final Gambit", ["MOVE_FINAL_GAMBIT", "MOVE_WAVE_CRASH", "MOVE_FLIP_TURN", "MOVE_CRUNCH"], "NATURE_JOLLY", "ABILITY_ADAPTABILITY", "ITEM_CHOICE_SCARF", [32, 0, 2, 0, 0, 32], "fast sacrifice and pivot attacker"),
    authored_modern_set("SPECIES_CAPSAKID", "Sun Super Fang", ["MOVE_LEAF_STORM", "MOVE_SUPER_FANG", "MOVE_LEECH_SEED", "MOVE_PROTECT"], "NATURE_TIMID", "ABILITY_CHLOROPHYLL", "ITEM_FOCUS_SASH", [2, 0, 0, 32, 0, 32], "sun disruption and percentage damage"),
    authored_modern_set("SPECIES_CHARCADET", "Flash Fire Ambush", ["MOVE_FLARE_BLITZ", "MOVE_HELPING_HAND", "MOVE_DESTINY_BOND", "MOVE_PROTECT"], "NATURE_ADAMANT", "ABILITY_FLASH_FIRE", "ITEM_EVIOLITE", [32, 32, 2, 0, 0, 0], "Flash Fire physical ambush and trade support"),
    authored_modern_set("SPECIES_FLITTLE", "Reverse The Room", ["MOVE_TRICK_ROOM", "MOVE_FOUL_PLAY", "MOVE_REFLECT", "MOVE_PROTECT"], "NATURE_RELAXED", "ABILITY_FRISK", "ITEM_EVIOLITE", [32, 0, 32, 0, 2, 0], "Trick Room reversal and physical disruption"),
    authored_modern_set("SPECIES_GIMMIGHOUL", "Dual Screens Chest", ["MOVE_SHADOW_BALL", "MOVE_REFLECT", "MOVE_LIGHT_SCREEN", "MOVE_PROTECT"], "NATURE_CALM", "ABILITY_RATTLED", "ITEM_EVIOLITE", [32, 0, 2, 0, 32, 0], "slow dual-screen support"),
    authored_modern_set("SPECIES_GREAVARD", "Memento Support", ["MOVE_POLTERGEIST", "MOVE_SNARL", "MOVE_MEMENTO", "MOVE_PROTECT"], "NATURE_CAREFUL", "ABILITY_FLUFFY", "ITEM_EVIOLITE", [32, 0, 2, 0, 32, 0], "special suppression and Memento positioning"),
    authored_modern_set("SPECIES_GROWLITHE_HISUI", "Intimidate Control", ["MOVE_ROCK_SLIDE", "MOVE_WILL_O_WISP", "MOVE_SNARL", "MOVE_PROTECT"], "NATURE_CAREFUL", "ABILITY_INTIMIDATE", "ITEM_EVIOLITE", [32, 0, 2, 0, 32, 0], "Intimidate burn and special-damage control"),
    authored_modern_set("SPECIES_ORTHWORM", "Coil Heavy Slam", ["MOVE_HEAVY_SLAM", "MOVE_HIGH_HORSEPOWER", "MOVE_COIL", "MOVE_PROTECT"], "NATURE_CAREFUL", "ABILITY_EARTH_EATER", "ITEM_LEFTOVERS", [32, 32, 2, 0, 0, 0], "Coil setup physical attacker"),
    authored_modern_set("SPECIES_POLTCHAGEIST", "Bulky Tea Support", ["MOVE_GIGA_DRAIN", "MOVE_LIFE_DEW", "MOVE_SHADOW_BALL", "MOVE_PROTECT"], "NATURE_BOLD", "ABILITY_HOSPITALITY", "ITEM_EVIOLITE", [32, 0, 32, 0, 2, 0], "team healing and bulky special pressure"),
    authored_modern_set("SPECIES_QWILFISH_HISUI", "Swift Swim Attacker", ["MOVE_GUNK_SHOT", "MOVE_LIQUIDATION", "MOVE_AQUA_JET", "MOVE_PROTECT"], "NATURE_ADAMANT", "ABILITY_SWIFT_SWIM", "ITEM_LIFE_ORB", [2, 32, 0, 0, 0, 32], "rain physical attacker and priority cleaner"),
    authored_modern_set("SPECIES_SNEASEL_HISUI", "Coaching Disruptor", ["MOVE_COACHING", "MOVE_TAUNT", "MOVE_FEINT", "MOVE_PROTECT"], "NATURE_JOLLY", "ABILITY_INNER_FOCUS", "ITEM_EVIOLITE", [32, 0, 2, 0, 0, 32], "fast partner coaching and Protect denial"),
    authored_modern_set("SPECIES_TADBULB", "Parabolic Discharge", ["MOVE_DISCHARGE", "MOVE_PARABOLIC_CHARGE", "MOVE_MUDDY_WATER", "MOVE_PROTECT"], "NATURE_MODEST", "ABILITY_OWN_TEMPO", "ITEM_EVIOLITE", [32, 0, 2, 32, 0, 0], "bulky spread special attacker"),
    authored_modern_set("SPECIES_TANDEMAUS", "Encore Team Support", ["MOVE_SUPER_FANG", "MOVE_ENCORE", "MOVE_HELPING_HAND", "MOVE_PROTECT"], "NATURE_TIMID", "ABILITY_OWN_TEMPO", "ITEM_EVIOLITE", [32, 0, 2, 0, 0, 32], "Encore and partner support"),
    authored_modern_set("SPECIES_TAUROS_PALDEA_COMBAT", "Anger Point Setup", ["MOVE_RAGING_BULL", "MOVE_ROCK_SLIDE", "MOVE_BULK_UP", "MOVE_PROTECT"], "NATURE_JOLLY", "ABILITY_ANGER_POINT", "ITEM_SITRUS_BERRY", [2, 32, 0, 0, 0, 32], "Anger Point or Bulk Up sweeper"),
    authored_modern_set("SPECIES_TAUROS_PALDEA_BLAZE", "Blaze Team Control", ["MOVE_RAGING_BULL", "MOVE_WILL_O_WISP", "MOVE_ROCK_SLIDE", "MOVE_HELPING_HAND"], "NATURE_CAREFUL", "ABILITY_INTIMIDATE", "ITEM_SITRUS_BERRY", [32, 0, 2, 0, 32, 0], "Intimidate burn and partner support"),
    authored_modern_set("SPECIES_TAUROS_PALDEA_AQUA", "Aqua Speed Control", ["MOVE_RAGING_BULL", "MOVE_ICY_WIND", "MOVE_HELPING_HAND", "MOVE_PROTECT"], "NATURE_JOLLY", "ABILITY_INTIMIDATE", "ITEM_SITRUS_BERRY", [32, 0, 2, 0, 0, 32], "Intimidate speed and partner support"),
    authored_modern_set("SPECIES_TINKATINK", "Fairy Screen Support", ["MOVE_FOUL_PLAY", "MOVE_HELPING_HAND", "MOVE_REFLECT", "MOVE_PROTECT"], "NATURE_CAREFUL", "ABILITY_OWN_TEMPO", "ITEM_EVIOLITE", [32, 0, 2, 0, 32, 0], "screen and partner support"),
    authored_modern_set("SPECIES_ZORUA_HISUI", "Illusion Nasty Plot", ["MOVE_SHADOW_BALL", "MOVE_SNARL", "MOVE_NASTY_PLOT", "MOVE_PROTECT"], "NATURE_TIMID", "ABILITY_ILLUSION", "ITEM_EVIOLITE", [2, 0, 0, 32, 0, 32], "Illusion setup attacker and Snarl control"),
]

SUPPLEMENTAL_DEFAULT_OVERRIDES = {
    "SPECIES_GIMMIGHOUL": authored_modern_set(
        "SPECIES_GIMMIGHOUL",
        "Nasty Plot Chest",
        ["MOVE_SHADOW_BALL", "MOVE_POWER_GEM", "MOVE_NASTY_PLOT", "MOVE_PROTECT"],
        "NATURE_QUIET",
        "ABILITY_RATTLED",
        "ITEM_EVIOLITE",
        [32, 0, 2, 32, 0, 0],
        "slow bulky special setup attacker",
    ),
}


def git_json(path: str) -> dict:
    result = subprocess.run(
        ["git", "show", f"{SOURCE_COMMIT}:{path}"],
        check=True,
        stdout=subprocess.PIPE,
    )
    return json.loads(result.stdout)


def shorten_name(name: str) -> str:
    name = name.replace(" — ", " ").replace("-Mega", " Mega").strip()
    return name[:23]


def normalize_species(species: str) -> str:
    species = SPECIES_ALIASES.get(species, species)
    species = species.replace("_ALOLAN", "_ALOLA").replace("_GALARIAN", "_GALAR").replace("_HISUIAN", "_HISUI")
    species = re.sub(r"_(ORIGINAL|HOENN|SINNOH|UNOVA|KALOS|ALOLA|PARTNER|WORLD)_CAP$", r"_\1", species)
    species = re.sub(r"_(YELLOW|ORANGE|BLUE|WHITE)_FLOWER$", r"_\1", species)
    species = re.sub(r"_(HEART|STAR|DIAMOND|DEBUTANTE|MATRON|DANDY|LA_REINE|KABUKI|PHARAOH)_TRIM$", r"_\1", species)
    species = re.sub(r"_(DOUSE|SHOCK|BURN|CHILL)_DRIVE$", r"_\1", species)
    return SPECIES_ALIASES.get(species, species)


def normalize_ability(species: str, ability: str) -> str:
    return ABILITY_ALIASES.get((species, ability), ability)


def normalize_moves(moves: list[str]) -> list[str]:
    moves = list(moves)
    while moves and moves[-1] == "MOVE_NONE":
        moves.pop()
    return moves


def count_damage_bias(moves: list[str]) -> tuple[int, int]:
    physical = special = 0
    for move in moves:
        token = move.removeprefix("MOVE_")
        if any(word in token for word in PHYSICAL_MOVE_WORDS):
            physical += 1
        if any(word in token for word in SPECIAL_MOVE_WORDS):
            special += 1
    return physical, special


def infer_stat_points(nature: str, moves: list[str], role: str) -> list[int]:
    text = (role + " " + " ".join(moves)).upper()
    trick_room = "TRICK_ROOM" in text or "TRICK ROOM" in text or nature in {"NATURE_BRAVE", "NATURE_QUIET"}
    physical, special = count_damage_bias(moves)

    if nature in PHYSICAL_WALL_NATURES or any(word in text for word in ("PHYSICAL WALL", "PHYSICALLY DEFENSIVE")):
        return [32, 0, 32, 0, 2, 0]
    if nature in SPECIAL_WALL_NATURES or any(word in text for word in ("SPECIAL WALL", "SPECIALLY DEFENSIVE")):
        return [32, 0, 2, 0, 32, 0]
    if any(word in text for word in ("MIXED WALL", "BULKY SUPPORT", "REDIRECTION", "PERISH", "TRICK ROOM SETTER")):
        return [32, 0, 16, 0, 18, 0]

    use_physical = nature in PHYSICAL_NATURES
    use_special = nature in SPECIAL_NATURES
    if not use_physical and not use_special:
        use_physical = physical >= special
        use_special = special > physical
    if physical and special and abs(physical - special) <= 1 and "MIXED" in text:
        return [2, 32, 0, 32, 0, 0]
    if use_physical:
        return [32, 32, 2, 0, 0, 0] if trick_room else [2, 32, 0, 0, 0, 32]
    if use_special:
        return [32, 0, 2, 32, 0, 0] if trick_room else [2, 0, 0, 32, 0, 32]
    return [32, 0, 16, 0, 18, 0]


def normalize_default(entry: dict, default_names: dict[str, str]) -> dict:
    review = entry.get("authored_review") or {}
    role = review.get("role") or entry.get("source_kind", "Recommended")
    species = normalize_species(entry["species"])
    moves = normalize_moves(entry["moves"])
    return {
        "species": species,
        "name": shorten_name(default_names.get(entry["species"], "Recommended")),
        "moves": moves,
        "nature": entry["nature"],
        "ability": normalize_ability(species, entry["ability"]),
        "item": entry.get("runtime_item", "ITEM_NONE"),
        "required_item": "ITEM_NONE",
        "stat_points": infer_stat_points(entry["nature"], moves, role),
        "role": role,
        "source": "preserved-authored-default",
    }


def normalize_alternative(entry: dict) -> dict:
    role = entry.get("handbook", {}).get("role") or entry["name"]
    species = normalize_species(entry["species"])
    moves = normalize_moves(entry["moves"])
    return {
        "species": species,
        "name": shorten_name(entry["name"]),
        "moves": moves,
        "nature": entry["nature"],
        "ability": normalize_ability(species, entry["ability"]),
        "item": entry.get("runtime_item", "ITEM_NONE"),
        "required_item": entry.get("required_item", "ITEM_NONE"),
        "stat_points": infer_stat_points(entry["nature"], moves, role),
        "role": role,
        "source": "preserved-handbook-alternative",
    }


def constants(path: Path, prefix: str) -> set[str]:
    return set(re.findall(rf"\b{prefix}[A-Z0-9_]+\b", path.read_text()))


def showdown_id(value: str) -> str:
    return re.sub(r"[^a-z0-9]", "", value.lower())


def constant_id_map(path: Path, prefix: str) -> dict[str, str]:
    result: dict[str, str] = {}
    for token in sorted(constants(path, prefix)):
        result.setdefault(showdown_id(token.removeprefix(prefix)), token)
    return result


def national_species_order() -> list[str]:
    source = (ROOT / "include/constants/pokedex.h").read_text()
    enum_body = source.split("enum NationalDexOrder", 1)[1].split("};", 1)[0]
    dex_tokens = list(dict.fromkeys(re.findall(r"\bNATIONAL_DEX_[A-Z0-9_]+\b", enum_body)))
    assert dex_tokens[0] == "NATIONAL_DEX_NONE"
    return ["SPECIES_" + token.removeprefix("NATIONAL_DEX_") for token in dex_tokens]


def normalize_handbook_set(
    source: dict,
    species: str,
    move_map: dict[str, str],
    item_map: dict[str, str],
    ability_map: dict[str, str],
    nature_map: dict[str, str],
) -> dict:
    moves = [move_map[showdown_id(move)] for move in source["moves"]]
    nature = nature_map[showdown_id(source["nature"].replace(" nature", ""))]
    role = source["role"].split(" — ", 1)[0]
    item = item_map[showdown_id(source["item"])]
    required_item = item if "Mega" in source["role"] and item != "ITEM_EVIOLITE" else "ITEM_NONE"
    return {
        "species": species,
        "name": shorten_name(role),
        "moves": moves,
        "nature": nature,
        "ability": normalize_ability(species, ability_map[showdown_id(source["ability"])]),
        "item": "ITEM_NONE" if required_item != "ITEM_NONE" else item,
        "required_item": required_item,
        "stat_points": infer_stat_points(nature, moves, role),
        "role": role,
        "source": f"Pokemon Champions doubles handbook: {source['evidence']}",
    }


def handbook_supplements(present_species: set[str]) -> tuple[list[dict], list[dict]]:
    handbook = git_json(HANDBOOK_SOURCE)
    species_constants = constants(ROOT / "include/constants/species.h", "SPECIES_")
    move_map = constant_id_map(ROOT / "include/constants/moves.h", "MOVE_")
    item_map = constant_id_map(ROOT / "include/constants/items.h", "ITEM_")
    ability_map = constant_id_map(ROOT / "include/constants/abilities.h", "ABILITY_")
    nature_map = constant_id_map(ROOT / "include/constants/pokemon.h", "NATURE_")
    national_species = national_species_order()
    by_dex: dict[int, list[dict]] = {}
    for entry in handbook["sets"]:
        by_dex.setdefault(entry["national_dex"], []).append(entry)

    defaults: list[dict] = []
    alternatives: list[dict] = []
    for dex_number, species in enumerate(national_species[1:], 1):
        if species not in species_constants or species in present_species:
            continue
        source_sets = by_dex.get(dex_number, [])
        if not source_sets:
            continue
        converted = [
            normalize_handbook_set(entry, species, move_map, item_map, ability_map, nature_map)
            for entry in source_sets
        ]
        defaults.append(converted[0])
        alternatives.extend(converted[1:])
        present_species.add(species)

    for role_suffix, species in HANDBOOK_FORM_ROLES.items():
        if species in present_species:
            continue
        source = next(
            entry for entry in handbook["sets"]
            if entry["role"].endswith("— " + role_suffix)
        )
        defaults.append(
            normalize_handbook_set(source, species, move_map, item_map, ability_map, nature_map)
        )
        present_species.add(species)

    return defaults, alternatives


def validate(entries: list[dict]) -> None:
    species = constants(ROOT / "include" / "constants" / "species.h", "SPECIES_")
    moves = constants(ROOT / "include" / "constants" / "moves.h", "MOVE_")
    items = constants(ROOT / "include" / "constants" / "items.h", "ITEM_")
    abilities = constants(ROOT / "include" / "constants" / "abilities.h", "ABILITY_")
    natures = constants(ROOT / "include" / "constants" / "pokemon.h", "NATURE_")
    for entry in entries:
        assert entry["species"] in species, entry["species"]
        assert entry["nature"] in natures, entry["nature"]
        assert entry["ability"] in abilities, entry["ability"]
        assert entry["item"] in items, entry["item"]
        assert entry["required_item"] in items, entry["required_item"]
        assert 1 <= len(entry["moves"]) <= 4
        assert len(entry["moves"]) == len(set(entry["moves"])), (entry["species"], entry["moves"])
        assert all(move in moves for move in entry["moves"])
        assert len(entry["stat_points"]) == 6
        assert sum(entry["stat_points"]) == 66
        assert max(entry["stat_points"]) <= 32


def c_preset(entry: dict, indent: str = "        ") -> list[str]:
    moves = entry["moves"] + ["MOVE_NONE"] * (4 - len(entry["moves"]))
    points = ", ".join(str(value) for value in entry["stat_points"])
    return [
        indent + ".moves = {" + ", ".join(moves) + "},",
        indent + f'.item = {entry["item"]},',
        indent + f'.requiredItem = {entry["required_item"]},',
        indent + f'.nature = {entry["nature"]},',
        indent + f'.ability = {entry["ability"]},',
        indent + f".statPoints = {{{points}}},",
    ]


def write_c(defaults: list[dict], alternatives: list[dict]) -> None:
    by_species: dict[str, list[dict]] = {}
    for entry in alternatives:
        by_species.setdefault(entry["species"], []).append(entry)

    lines = [
        "// Generated by scripts/generate_emerald_champions_battle_sets.py. Do not edit by hand.",
    ]
    for entry in defaults:
        if entry["name"] != "Recommended":
            lines.append(f'static const u8 sEmeraldChampionsSetName_{entry["species"]}[] = _("{entry["name"]}");')
    lines.extend([
        "",
        "const struct EmeraldChampionsBattleSet gEmeraldChampionsDefaultBattleSets[NUM_SPECIES] =",
        "{",
    ])
    for entry in defaults:
        lines.append(f'    [{entry["species"]}] =')
        lines.append("    {")
        lines.extend(c_preset(entry))
        lines.append("    },")
    lines.extend(["};", "", "const u8 *const gEmeraldChampionsDefaultBattleSetNames[NUM_SPECIES] =", "{"])
    for entry in defaults:
        if entry["name"] != "Recommended":
            lines.append(f'    [{entry["species"]}] = sEmeraldChampionsSetName_{entry["species"]},')
    lines.extend(["};", "", "const struct EmeraldChampionsBattleSetRange gEmeraldChampionsBattleSetRanges[NUM_SPECIES] =", "{"])

    offset = 0
    for entry in defaults:
        choices = by_species.get(entry["species"], [])
        if choices:
            lines.append(f'    [{entry["species"]}] = {{.offset = {offset}, .count = {len(choices)}}},')
            offset += len(choices)
    lines.extend(["};", "", "const struct EmeraldChampionsBattleSetChoice gEmeraldChampionsBattleSetAlternatives[] =", "{"])
    for entry in alternatives:
        lines.append("    {")
        lines.append(f'        .name = _("{entry["name"]}"),')
        lines.append("        .preset =")
        lines.append("        {")
        lines.extend(c_preset(entry, "            "))
        lines.append("        },")
        lines.append("    },")
    lines.extend(["};", ""])
    C_OUTPUT.write_text("\n".join(lines))


def main() -> None:
    default_source = git_json(DEFAULT_SOURCE)
    alternative_source = git_json(ALTERNATIVE_SOURCE)
    default_names = alternative_source.get("default_names", {})
    defaults = [normalize_default(entry, default_names) for entry in default_source["presets"]]
    defaults = [SUPPLEMENTAL_DEFAULT_OVERRIDES.get(entry["species"], entry) for entry in defaults]
    present_species = {entry["species"] for entry in defaults}
    defaults.extend(entry for entry in SUPPLEMENTAL_DEFAULTS if entry["species"] not in present_species)
    present_species = {entry["species"] for entry in defaults}
    handbook_defaults, handbook_alternatives = handbook_supplements(present_species)
    defaults.extend(handbook_defaults)
    default_species = {entry["species"] for entry in defaults}
    assert len(default_species) == len(defaults), "Species aliases collapsed two default presets"
    raw_alternatives = [
        normalize_alternative(entry)
        for entry in alternative_source["alternatives"]
        if normalize_species(entry["species"]) in default_species
    ]
    raw_alternatives.extend(
        entry for entry in SUPPLEMENTAL_ALTERNATIVES
        if entry["species"] in default_species
    )
    raw_alternatives.extend(handbook_alternatives)
    alternatives_by_species: dict[str, list[dict]] = {}
    for entry in raw_alternatives:
        alternatives_by_species.setdefault(entry["species"], []).append(entry)
    alternatives = [
        choice
        for default in defaults
        for choice in alternatives_by_species.get(default["species"], [])
    ]
    entries = defaults + alternatives
    validate(entries)

    output = {
        "schema_version": 2,
        "source_commit": SOURCE_COMMIT,
        "policy": {
            "format": "doubles-first",
            "stat_points": "66 total, 32 maximum per stat",
            "ability": "resolved by Ability identity against current species data",
            "protected_items": "never supplied by a preset",
        },
        "default_count": len(defaults),
        "alternative_count": len(alternatives),
        "set_count": len(entries),
        "defaults": defaults,
        "alternatives": alternatives,
    }
    JSON_OUTPUT.write_text(json.dumps(output, indent=2, ensure_ascii=False) + "\n")
    write_c(defaults, alternatives)
    print(f"defaults={len(defaults)}")
    print(f"alternatives={len(alternatives)}")
    print(f"sets={len(entries)}")


if __name__ == "__main__":
    main()
