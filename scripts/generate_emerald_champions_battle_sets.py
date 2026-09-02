#!/usr/bin/env python3
"""Generate Champions-native runtime presets from the preserved authored corpus."""

from __future__ import annotations

import json
import re
import subprocess
from collections import defaultdict
from pathlib import Path

from verify_trainer_ability_legality import (
    SPECIES_MARKER,
    preprocess_species_info,
    resolve_species,
    species_aliases,
)


ROOT = Path(__file__).resolve().parents[1]
# This reachable repository checkpoint preserves the three authored source
# corpora.  Keeping the object reachable makes preset generation reproducible
# in clean clones and CI.
SOURCE_COMMIT = "0b2bc96c7d6480187f70f5b83a705c081780983e"
DEFAULT_SOURCE = "docs/verdant_battle_set_presets.json"
ALTERNATIVE_SOURCE = "docs/verdant_multi_battle_sets.json"
HANDBOOK_SOURCE = "docs/pokemon_champions_handbook_sets.json"
HANDBOOK_SHA256 = "a6a98dd09849c80c46e4d39b3fdaac161f56d80c69fdf8422bd4b7596cb714d5"
JSON_OUTPUT = ROOT / "docs" / "emerald_champions_battle_sets.json"
C_OUTPUT = ROOT / "src" / "data" / "pokemon" / "emerald_champions_battle_sets.h"
MOVE_ACCESS_REVIEW = ROOT / "docs" / "emerald_champions_move_access_review.json"
MOVE_ACCESS_C_OUTPUT = ROOT / "src" / "data" / "pokemon" / "emerald_champions_move_access_review.h"
SHOWDOWN_SINGLES_SOURCE = ROOT / "docs" / "showdown_champions_random_singles.json"
SHOWDOWN_GEN9_SINGLES_SOURCE = ROOT / "docs" / "showdown_gen9_random_singles.json"

SINGLES_EXCLUDED_MOVES = {
    "MOVE_AFTER_YOU", "MOVE_ALLY_SWITCH", "MOVE_AROMATIC_MIST",
    "MOVE_COACHING", "MOVE_DECORATE", "MOVE_FOLLOW_ME", "MOVE_HEAL_PULSE",
    "MOVE_HELPING_HAND", "MOVE_HOLD_HANDS", "MOVE_INSTRUCT", "MOVE_LIFE_DEW",
    "MOVE_QUICK_GUARD", "MOVE_RAGE_POWDER", "MOVE_SPOTLIGHT",
    "MOVE_WIDE_GUARD",
}

SINGLES_SETUP_MOVES = (
    "MOVE_SHELL_SMASH", "MOVE_QUIVER_DANCE", "MOVE_DRAGON_DANCE",
    "MOVE_VICTORY_DANCE", "MOVE_SHIFT_GEAR", "MOVE_GEOMANCY",
    "MOVE_TAIL_GLOW", "MOVE_NASTY_PLOT", "MOVE_SWORDS_DANCE",
    "MOVE_CALM_MIND", "MOVE_BULK_UP", "MOVE_COIL", "MOVE_IRON_DEFENSE",
)

SINGLES_RECOVERY_MOVES = (
    "MOVE_RECOVER", "MOVE_ROOST", "MOVE_SLACK_OFF", "MOVE_SHORE_UP",
    "MOVE_SOFT_BOILED", "MOVE_STRENGTH_SAP", "MOVE_SYNTHESIS",
    "MOVE_MOONLIGHT", "MOVE_MORNING_SUN", "MOVE_WISH", "MOVE_REST",
)

SINGLES_UTILITY_MOVES = (
    "MOVE_STEALTH_ROCK", "MOVE_SPIKES", "MOVE_TOXIC_SPIKES",
    "MOVE_STICKY_WEB", "MOVE_RAPID_SPIN", "MOVE_MORTAL_SPIN",
    "MOVE_DEFOG", "MOVE_KNOCK_OFF", "MOVE_TOXIC", "MOVE_WILL_O_WISP",
    "MOVE_THUNDER_WAVE", "MOVE_GLARE", "MOVE_SLEEP_POWDER", "MOVE_SPORE",
    "MOVE_LEECH_SEED", "MOVE_SUBSTITUTE", "MOVE_ENCORE", "MOVE_TAUNT",
    "MOVE_DISABLE", "MOVE_TRICK", "MOVE_SWITCHEROO", "MOVE_ROAR",
)

SINGLES_DAMAGE_INDEPENDENT_MOVES = {
    "MOVE_ACID_SPRAY", "MOVE_BODY_PRESS", "MOVE_COUNTER", "MOVE_ELECTROWEB",
    "MOVE_ENDEAVOR", "MOVE_FAKE_OUT", "MOVE_FEINT", "MOVE_FINAL_GAMBIT",
    "MOVE_FOUL_PLAY", "MOVE_ICY_WIND", "MOVE_METAL_BURST", "MOVE_MIRROR_COAT",
    "MOVE_NIGHT_SHADE", "MOVE_NUZZLE", "MOVE_RAPID_SPIN", "MOVE_RUINATION",
    "MOVE_SALT_CURE", "MOVE_SEISMIC_TOSS", "MOVE_SNARL", "MOVE_SUPER_FANG",
}

# A second set is useful only when it asks the Pokemon to do something
# meaningfully different.  These authored role families are intentionally
# small and doubles-specific; the synthesizer below chooses among them from
# the species' live learnable pool instead of fabricating illegal coverage.
ROLE_BLUEPRINTS = (
    ("Trick Room Control", "MOVE_TRICK_ROOM", (
        "MOVE_HELPING_HAND", "MOVE_FAKE_OUT", "MOVE_WIDE_GUARD",
        "MOVE_ALLY_SWITCH", "MOVE_HEAL_PULSE", "MOVE_LIFE_DEW",
        "MOVE_TAUNT", "MOVE_ENCORE", "MOVE_IMPRISON",
    ), "slow_support"),
    ("Redirection Support", "MOVE_FOLLOW_ME", (
        "MOVE_HELPING_HAND", "MOVE_FAKE_OUT", "MOVE_HEAL_PULSE",
        "MOVE_LIFE_DEW", "MOVE_THUNDER_WAVE", "MOVE_WILL_O_WISP",
        "MOVE_ENCORE", "MOVE_TAUNT",
    ), "bulky_support"),
    ("Rage Powder Support", "MOVE_RAGE_POWDER", (
        "MOVE_HELPING_HAND", "MOVE_POLLEN_PUFF", "MOVE_SLEEP_POWDER",
        "MOVE_SPORE", "MOVE_STUN_SPORE", "MOVE_LEECH_SEED",
    ), "bulky_support"),
    ("Tailwind Control", "MOVE_TAILWIND", (
        "MOVE_HELPING_HAND", "MOVE_TAUNT", "MOVE_ENCORE", "MOVE_FAKE_OUT",
        "MOVE_ICY_WIND", "MOVE_ELECTROWEB", "MOVE_SNARL",
    ), "fast_support"),
    ("Wide Guard Support", "MOVE_WIDE_GUARD", (
        "MOVE_FAKE_OUT", "MOVE_HELPING_HAND", "MOVE_COACHING",
        "MOVE_QUICK_GUARD", "MOVE_TAUNT", "MOVE_KNOCK_OFF",
    ), "bulky_support"),
    ("Fake Out Control", "MOVE_FAKE_OUT", (
        "MOVE_HELPING_HAND", "MOVE_TAUNT", "MOVE_ENCORE", "MOVE_NUZZLE",
        "MOVE_THUNDER_WAVE", "MOVE_ICY_WIND", "MOVE_SNARL",
    ), "fast_support"),
    ("Dual Screens", "MOVE_REFLECT", (
        "MOVE_LIGHT_SCREEN", "MOVE_AURORA_VEIL", "MOVE_HELPING_HAND",
        "MOVE_THUNDER_WAVE", "MOVE_ICY_WIND", "MOVE_ELECTROWEB",
    ), "bulky_support"),
    ("Sleep Control", "MOVE_SPORE", (
        "MOVE_RAGE_POWDER", "MOVE_POLLEN_PUFF", "MOVE_HELPING_HAND",
        "MOVE_LEECH_SEED", "MOVE_TAILWIND",
    ), "bulky_support"),
    ("Sleep Control", "MOVE_SLEEP_POWDER", (
        "MOVE_RAGE_POWDER", "MOVE_POLLEN_PUFF", "MOVE_HELPING_HAND",
        "MOVE_LEECH_SEED", "MOVE_TAILWIND",
    ), "fast_support"),
    ("Tempo Control", "MOVE_ICY_WIND", (
        "MOVE_HELPING_HAND", "MOVE_TAUNT", "MOVE_ENCORE", "MOVE_SNARL",
        "MOVE_FAKE_TEARS", "MOVE_HEAL_PULSE",
    ), "fast_support"),
    ("Tempo Control", "MOVE_ELECTROWEB", (
        "MOVE_HELPING_HAND", "MOVE_TAUNT", "MOVE_ENCORE", "MOVE_SNARL",
        "MOVE_FAKE_TEARS", "MOVE_EERIE_IMPULSE",
    ), "fast_support"),
    ("Disruption Support", "MOVE_TAUNT", (
        "MOVE_ENCORE", "MOVE_DISABLE", "MOVE_HELPING_HAND", "MOVE_FAKE_OUT",
        "MOVE_WILL_O_WISP", "MOVE_THUNDER_WAVE", "MOVE_SNARL",
    ), "fast_support"),
)

PHYSICAL_SETUP_MOVES = (
    "MOVE_SHELL_SMASH", "MOVE_DRAGON_DANCE", "MOVE_VICTORY_DANCE",
    "MOVE_SHIFT_GEAR", "MOVE_SWORDS_DANCE", "MOVE_BULK_UP", "MOVE_COIL",
    "MOVE_TIDY_UP", "MOVE_HONE_CLAWS", "MOVE_HOWL",
)
SPECIAL_SETUP_MOVES = (
    "MOVE_SHELL_SMASH", "MOVE_QUIVER_DANCE", "MOVE_TAIL_GLOW",
    "MOVE_NASTY_PLOT", "MOVE_CALM_MIND", "MOVE_GEOMANCY",
)
GENERAL_SUPPORT_MOVES = (
    "MOVE_HELPING_HAND", "MOVE_FAKE_OUT", "MOVE_WIDE_GUARD",
    "MOVE_QUICK_GUARD", "MOVE_TAILWIND", "MOVE_TRICK_ROOM",
    "MOVE_FOLLOW_ME", "MOVE_RAGE_POWDER", "MOVE_SPORE",
    "MOVE_SLEEP_POWDER", "MOVE_ICY_WIND", "MOVE_ELECTROWEB", "MOVE_SNARL",
    "MOVE_TAUNT", "MOVE_ENCORE", "MOVE_DISABLE", "MOVE_WILL_O_WISP",
    "MOVE_THUNDER_WAVE", "MOVE_NUZZLE", "MOVE_FAKE_TEARS",
    "MOVE_EERIE_IMPULSE", "MOVE_COACHING", "MOVE_DECORATE",
    "MOVE_HEAL_PULSE", "MOVE_LIFE_DEW", "MOVE_POLLEN_PUFF",
    "MOVE_REFLECT", "MOVE_LIGHT_SCREEN", "MOVE_AURORA_VEIL",
    "MOVE_LEECH_SEED", "MOVE_RECOVER", "MOVE_ROOST", "MOVE_SYNTHESIS",
    "MOVE_MOONLIGHT", "MOVE_MORNING_SUN", "MOVE_SLACK_OFF",
    "MOVE_STRENGTH_SAP", "MOVE_SHORE_UP", "MOVE_SOFT_BOILED",
    "MOVE_IRON_DEFENSE", "MOVE_ACID_ARMOR", "MOVE_AMNESIA",
    "MOVE_COSMIC_POWER", "MOVE_AGILITY", "MOVE_STRING_SHOT",
    "MOVE_SCREECH", "MOVE_CHARM", "MOVE_SCARY_FACE", "MOVE_GLARE",
    "MOVE_SAFEGUARD",
)

# These moves can be excellent on a deliberately authored set, but their
# conditional cost makes them unsafe as generic role-selection candidates.
UNSUITABLE_SYNTHETIC_ATTACKS = {
    "MOVE_AVALANCHE", "MOVE_BELCH", "MOVE_BIDE", "MOVE_BODY_PRESS", "MOVE_COUNTER",
    "MOVE_CRUSH_GRIP", "MOVE_DIG", "MOVE_DIVE", "MOVE_ELECTRO_BALL",
    "MOVE_EXPLOSION", "MOVE_FINAL_GAMBIT", "MOVE_FISSURE", "MOVE_FLAIL",
    "MOVE_FLING", "MOVE_FLY", "MOVE_FOCUS_PUNCH", "MOVE_FOUL_PLAY",
    "MOVE_FREEZE_SHOCK", "MOVE_FRUSTRATION", "MOVE_GEOMANCY",
    "MOVE_DREAM_EATER", "MOVE_FUTURE_SIGHT", "MOVE_GIGA_IMPACT", "MOVE_GRASS_KNOT", "MOVE_GUILLOTINE",
    "MOVE_GYRO_BALL", "MOVE_HEAT_CRASH", "MOVE_HEAVY_SLAM",
    "MOVE_HIDDEN_POWER", "MOVE_HORN_DRILL", "MOVE_HYPER_BEAM",
    "MOVE_ICE_BALL", "MOVE_ICE_BURN", "MOVE_LAST_RESORT",
    "MOVE_LAST_RESPECTS", "MOVE_LOW_KICK", "MOVE_MAGNITUDE",
    "MOVE_METAL_BURST", "MOVE_METEOR_ASSAULT", "MOVE_METEOR_BEAM",
    "MOVE_MIRROR_COAT", "MOVE_MISTY_EXPLOSION", "MOVE_NATURAL_GIFT",
    "MOVE_OUTRAGE", "MOVE_PETAL_DANCE", "MOVE_PHANTOM_FORCE",
    "MOVE_POWER_TRIP", "MOVE_PRESENT", "MOVE_PRISMATIC_LASER",
    "MOVE_RAGE_FIST", "MOVE_RAZOR_WIND", "MOVE_RETURN", "MOVE_REVENGE", "MOVE_REVERSAL",
    "MOVE_ROAR_OF_TIME", "MOVE_ROCK_WRECKER", "MOVE_ROLLOUT",
    "MOVE_SELF_DESTRUCT", "MOVE_SHADOW_FORCE", "MOVE_SHEER_COLD",
    "MOVE_SKULL_BASH", "MOVE_SKY_ATTACK", "MOVE_SLEEP_TALK", "MOVE_SNORE",
    "MOVE_SOLAR_BEAM", "MOVE_SOLAR_BLADE", "MOVE_STEEL_ROLLER",
    "MOVE_STORED_POWER", "MOVE_SYNCHRONOISE", "MOVE_DRAGON_TAIL",
    "MOVE_TERA_BLAST",
    "MOVE_THRASH", "MOVE_TRUMP_CARD", "MOVE_UPROAR", "MOVE_WRING_OUT",
    "MOVE_BLAST_BURN", "MOVE_FRENZY_PLANT", "MOVE_HYDRO_CANNON",
    "MOVE_ETERNABEAM", "MOVE_ELECTRO_SHOT",
}

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
    ("SPECIES_SQUIRTLE", "ABILITY_OVERCOAT"): "ABILITY_RAIN_DISH",
    ("SPECIES_PIDGEY", "ABILITY_NO_GUARD"): "ABILITY_BIG_PECKS",
    ("SPECIES_PIDGEOTTO", "ABILITY_NO_GUARD"): "ABILITY_BIG_PECKS",
    ("SPECIES_PONYTA", "ABILITY_RECKLESS"): "ABILITY_FLAME_BODY",
    ("SPECIES_SEEL", "ABILITY_FUR_COAT"): "ABILITY_ICE_BODY",
    ("SPECIES_CHIKORITA", "ABILITY_TRIAGE"): "ABILITY_LEAF_GUARD",
    ("SPECIES_BAYLEEF", "ABILITY_TRIAGE"): "ABILITY_LEAF_GUARD",
    ("SPECIES_SENTRET", "ABILITY_FUR_COAT"): "ABILITY_FRISK",
    ("SPECIES_FURRET", "ABILITY_FUR_COAT"): "ABILITY_FRISK",
    ("SPECIES_LEDYBA", "ABILITY_AERILATE"): "ABILITY_RATTLED",
    ("SPECIES_SPINARAK", "ABILITY_MERCILESS"): "ABILITY_SNIPER",
    ("SPECIES_SUNFLORA", "ABILITY_DROUGHT"): "ABILITY_EARLY_BIRD",
    ("SPECIES_DELIBIRD", "ABILITY_REFRIGERATE"): "ABILITY_INSOMNIA",
    ("SPECIES_WURMPLE", "ABILITY_POISON_POINT"): "ABILITY_RUN_AWAY",
    ("SPECIES_SLAKOTH", "ABILITY_STALL"): "ABILITY_TRUANT",
    ("SPECIES_GLALIE", "ABILITY_REFRIGERATE"): "ABILITY_MOODY",
    ("SPECIES_TURTWIG", "ABILITY_SOLID_ROCK"): "ABILITY_SHELL_ARMOR",
    ("SPECIES_GROTLE", "ABILITY_SOLID_ROCK"): "ABILITY_SHELL_ARMOR",
    ("SPECIES_WATCHOG", "ABILITY_DAZZLING"): "ABILITY_ANALYTIC",
    ("SPECIES_FLABEBE_RED", "ABILITY_HEALER"): "ABILITY_SYMBIOSIS",
    ("SPECIES_FLABEBE", "ABILITY_HEALER"): "ABILITY_SYMBIOSIS",
    ("SPECIES_ROWLET", "ABILITY_TINTED_LENS"): "ABILITY_LONG_REACH",
    ("SPECIES_DARTRIX", "ABILITY_TINTED_LENS"): "ABILITY_LONG_REACH",
    # This pair was introduced by a broad Vital Spirit rewrite.  Durant's
    # authored physical set is the standard Hustle orientation.
    ("SPECIES_DURANT", "ABILITY_VITAL_SPIRIT"): "ABILITY_HUSTLE",
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

HANDBOOK_MEGA_BASE_ROLES = {
    # The National Dex row is Meowstic-M, but this role explicitly targets the
    # separately configured female Mega form.
    "Meowstic-F-Mega": "SPECIES_MEOWSTIC_F",
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


def authored_mega_set(
    species: str,
    name: str,
    moves: list[str],
    nature: str,
    ability: str,
    required_item: str,
    stat_points: list[int],
    role: str,
) -> dict:
    entry = authored_modern_set(
        species,
        name,
        moves,
        nature,
        ability,
        "ITEM_NONE",
        stat_points,
        role,
    )
    entry["required_item"] = required_item
    entry["source"] = "Emerald Champions custom Mega extension: current form data and authored doubles corpus"
    return entry


# The Champions M-B handbook covers 75 of the 92 campaign stones.  These
# orientations cover the remaining current forms, using their executable
# stats/Abilities and the existing authored doubles corpus. Tatsugirinite has
# one legal orientation for each of its three independently catchable forms.
CUSTOM_MEGA_ROLES = [
    authored_mega_set("SPECIES_MEWTWO", "Mega X Physical", ["MOVE_PSYCHO_CUT", "MOVE_DRAIN_PUNCH", "MOVE_ICE_PUNCH", "MOVE_PROTECT"], "NATURE_JOLLY", "ABILITY_STEADFAST", "ITEM_MEWTWONITE_X", [2, 32, 0, 0, 0, 32], "Mega X fast physical attacker"),
    authored_mega_set("SPECIES_MEWTWO", "Mega Y Special", ["MOVE_PSYSTRIKE", "MOVE_AURA_SPHERE", "MOVE_ICE_BEAM", "MOVE_PROTECT"], "NATURE_TIMID", "ABILITY_INSOMNIA", "ITEM_MEWTWONITE_Y", [2, 0, 0, 32, 0, 32], "Mega Y fast special attacker"),
    authored_mega_set("SPECIES_ABSOL", "Mega Z Physical", ["MOVE_PLAY_ROUGH", "MOVE_KNOCK_OFF", "MOVE_SUCKER_PUNCH", "MOVE_PROTECT"], "NATURE_JOLLY", "ABILITY_MAGIC_BOUNCE", "ITEM_ABSOLITE_Z", [2, 32, 0, 0, 0, 32], "Mega Z Magic Bounce physical attacker"),
    authored_mega_set("SPECIES_SALAMENCE", "Mega Tailwind", ["MOVE_DOUBLE_EDGE", "MOVE_EARTHQUAKE", "MOVE_TAILWIND", "MOVE_PROTECT"], "NATURE_JOLLY", "ABILITY_AERILATE", "ITEM_SALAMENCITE", [2, 32, 0, 0, 0, 32], "Mega Aerilate Tailwind attacker"),
    authored_mega_set("SPECIES_LATIAS", "Mega Bulky Tailwind", ["MOVE_DRACO_METEOR", "MOVE_MIST_BALL", "MOVE_TAILWIND", "MOVE_RECOVER"], "NATURE_TIMID", "ABILITY_LEVITATE", "ITEM_LATIASITE", [32, 0, 2, 0, 0, 32], "Mega bulky Tailwind support attacker"),
    authored_mega_set("SPECIES_LATIOS", "Mega Tailwind", ["MOVE_DRACO_METEOR", "MOVE_LUSTER_PURGE", "MOVE_TAILWIND", "MOVE_PROTECT"], "NATURE_TIMID", "ABILITY_LEVITATE", "ITEM_LATIOSITE", [2, 0, 0, 32, 0, 32], "Mega fast Tailwind special attacker"),
    authored_mega_set("SPECIES_GARCHOMP", "Mega Z Special", ["MOVE_DRACO_METEOR", "MOVE_EARTH_POWER", "MOVE_FLAMETHROWER", "MOVE_PROTECT"], "NATURE_TIMID", "ABILITY_SAND_VEIL", "ITEM_GARCHOMPITE_Z", [2, 0, 0, 32, 0, 32], "Mega Z fast special attacker"),
    authored_mega_set("SPECIES_LUCARIO", "Mega Z Special", ["MOVE_AURA_SPHERE", "MOVE_FLASH_CANNON", "MOVE_VACUUM_WAVE", "MOVE_PROTECT"], "NATURE_TIMID", "ABILITY_STEADFAST", "ITEM_LUCARIONITE_Z", [2, 0, 0, 32, 0, 32], "Mega Z fast special priority attacker"),
    authored_mega_set("SPECIES_HEATRAN", "Mega Trick Room", ["MOVE_HEAT_WAVE", "MOVE_EARTH_POWER", "MOVE_FLASH_CANNON", "MOVE_PROTECT"], "NATURE_QUIET", "ABILITY_FLASH_FIRE", "ITEM_HEATRANITE", [32, 0, 2, 32, 0, 0], "Mega slow bulky spread attacker"),
    authored_mega_set("SPECIES_DARKRAI", "Mega Sleep Control", ["MOVE_DARK_VOID", "MOVE_DARK_PULSE", "MOVE_ICE_BEAM", "MOVE_PROTECT"], "NATURE_TIMID", "ABILITY_BAD_DREAMS", "ITEM_DARKRANITE", [2, 0, 0, 32, 0, 32], "Mega Bad Dreams sleep-control attacker"),
    authored_mega_set("SPECIES_ZYGARDE_50_POWER_CONSTRUCT", "Mega Special Control", ["MOVE_DRACO_METEOR", "MOVE_EARTH_POWER", "MOVE_GLARE", "MOVE_PROTECT"], "NATURE_TIMID", "ABILITY_AURA_BREAK", "ITEM_ZYGARDITE", [2, 0, 0, 32, 0, 32], "Power Construct into Complete, then Mega special control"),
    authored_mega_set("SPECIES_DIANCIE", "Mega Mixed Attacker", ["MOVE_DIAMOND_STORM", "MOVE_MOONBLAST", "MOVE_EARTH_POWER", "MOVE_PROTECT"], "NATURE_NAIVE", "ABILITY_MAGIC_BOUNCE", "ITEM_DIANCITE", [2, 16, 0, 16, 0, 32], "Mega Magic Bounce mixed attacker"),
    authored_mega_set("SPECIES_GOLISOPOD", "Mega Trick Room", ["MOVE_FIRST_IMPRESSION", "MOVE_LIQUIDATION", "MOVE_LEECH_LIFE", "MOVE_WIDE_GUARD"], "NATURE_BRAVE", "ABILITY_EMERGENCY_EXIT", "ITEM_GOLISOPITE", [32, 32, 2, 0, 0, 0], "Mega slow physical attacker and Wide Guard support"),
    authored_mega_set("SPECIES_MAGEARNA", "Mega Trick Room", ["MOVE_FLEUR_CANNON", "MOVE_FLASH_CANNON", "MOVE_TRICK_ROOM", "MOVE_PROTECT"], "NATURE_QUIET", "ABILITY_SOUL_HEART", "ITEM_MAGEARNITE", [32, 0, 2, 32, 0, 0], "Mega Soul-Heart Trick Room attacker"),
    authored_mega_set("SPECIES_MAGEARNA_ORIGINAL", "Mega Trick Room", ["MOVE_FLEUR_CANNON", "MOVE_FLASH_CANNON", "MOVE_TRICK_ROOM", "MOVE_PROTECT"], "NATURE_QUIET", "ABILITY_SOUL_HEART", "ITEM_MAGEARNITE", [32, 0, 2, 32, 0, 0], "Original Color Mega Soul-Heart Trick Room attacker"),
    authored_mega_set("SPECIES_ZERAORA", "Mega Physical", ["MOVE_FAKE_OUT", "MOVE_PLASMA_FISTS", "MOVE_CLOSE_COMBAT", "MOVE_PROTECT"], "NATURE_JOLLY", "ABILITY_VOLT_ABSORB", "ITEM_ZERAORITE", [2, 32, 0, 0, 0, 32], "Mega fast Fake Out physical attacker"),
    authored_mega_set("SPECIES_TATSUGIRI", "Mega Storm Drain", ["MOVE_DRACO_METEOR", "MOVE_MUDDY_WATER", "MOVE_ICY_WIND", "MOVE_PROTECT"], "NATURE_TIMID", "ABILITY_STORM_DRAIN", "ITEM_TATSUGIRINITE", [2, 0, 0, 32, 0, 32], "Mega Curly Storm Drain special attacker"),
    authored_mega_set("SPECIES_TATSUGIRI_DROOPY", "Mega Storm Drain", ["MOVE_DRACO_METEOR", "MOVE_MUDDY_WATER", "MOVE_ICY_WIND", "MOVE_PROTECT"], "NATURE_TIMID", "ABILITY_STORM_DRAIN", "ITEM_TATSUGIRINITE", [2, 0, 0, 32, 0, 32], "Mega Droopy Storm Drain special attacker"),
    authored_mega_set("SPECIES_TATSUGIRI_STRETCHY", "Mega Storm Drain", ["MOVE_DRACO_METEOR", "MOVE_MUDDY_WATER", "MOVE_ICY_WIND", "MOVE_PROTECT"], "NATURE_TIMID", "ABILITY_STORM_DRAIN", "ITEM_TATSUGIRINITE", [2, 0, 0, 32, 0, 32], "Mega Stretchy Storm Drain special attacker"),
    authored_mega_set("SPECIES_BAXCALIBUR", "Mega Physical", ["MOVE_GLAIVE_RUSH", "MOVE_ICICLE_CRASH", "MOVE_ICE_SHARD", "MOVE_PROTECT"], "NATURE_ADAMANT", "ABILITY_THERMAL_EXCHANGE", "ITEM_BAXCALIBRITE", [2, 32, 0, 0, 0, 32], "Mega physical breaker and priority cleaner"),
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

SUPPLEMENTAL_ALTERNATIVE_OVERRIDES = {
    ("SPECIES_FLYGON", "Bulky Attacker"): authored_modern_set(
        "SPECIES_FLYGON",
        "Bulky Attacker",
        ["MOVE_PROTECT", "MOVE_SCALE_SHOT", "MOVE_TAILWIND", "MOVE_EARTHQUAKE"],
        "NATURE_JOLLY",
        "ABILITY_LEVITATE",
        "ITEM_YACHE_BERRY",
        [2, 32, 0, 0, 0, 32],
        "Yache physical attacker and Tailwind setter",
    ),
    ("SPECIES_HITMONCHAN", "Fake Out Control"): authored_modern_set(
        "SPECIES_HITMONCHAN",
        "Fake Out Control",
        ["MOVE_FAKE_OUT", "MOVE_HELPING_HAND", "MOVE_DRAIN_PUNCH", "MOVE_PROTECT"],
        "NATURE_JOLLY",
        "ABILITY_IRON_FIST",
        "ITEM_SITRUS_BERRY",
        [32, 0, 2, 0, 0, 32],
        "distinct doubles Fake Out and Drain Punch control role",
    ),
}

# Source-backed sets still need an executable review after import.  These are
# narrowly scoped corrections for configurations that cannot perform their
# authored role (for example, Protect on an Assault Vest set), not a template
# pass or an alternative-set quota.
AUDITED_SET_FIELD_OVERRIDES = {
    # Preserve support bulk while putting otherwise wasted offensive Stat
    # Points into HP on fast, damage-independent doubles utility sets.
    ("SPECIES_GOLBAT", "Recommended"): {"stat_points": [32, 0, 2, 0, 0, 32]},
    ("SPECIES_HITMONCHAN", "Recommended"): {"ability": "ABILITY_BLITZ_BOXER"},
    ("SPECIES_METAPOD", "Tempo Control"): {"nature": "NATURE_JOLLY"},
    ("SPECIES_HITMONCHAN", "Fake Out Control"): {
        "moves": ["MOVE_FAKE_OUT", "MOVE_HELPING_HAND", "MOVE_DRAIN_PUNCH", "MOVE_PROTECT"],
        "ability": "ABILITY_IRON_FIST",
        "role": "distinct doubles Fake Out and Drain Punch control role",
    },
    ("SPECIES_CHIMCHAR", "Fake Out Control"): {
        "moves": ["MOVE_FAKE_OUT", "MOVE_HELPING_HAND", "MOVE_FIRE_PUNCH", "MOVE_PROTECT"],
    },
    ("SPECIES_MONFERNO", "Fake Out Control"): {
        "moves": ["MOVE_FAKE_OUT", "MOVE_HELPING_HAND", "MOVE_DRAIN_PUNCH", "MOVE_PROTECT"],
    },
    ("SPECIES_GOLETT", "Tempo Control"): {
        "moves": ["MOVE_ICY_WIND", "MOVE_HELPING_HAND", "MOVE_SHADOW_PUNCH", "MOVE_PROTECT"],
    },
    ("SPECIES_CRABRAWLER", "Wide Guard Support"): {
        "moves": ["MOVE_WIDE_GUARD", "MOVE_HELPING_HAND", "MOVE_DRAIN_PUNCH", "MOVE_PROTECT"],
    },
    ("SPECIES_LEDIAN", "Recommended"): {"stat_points": [32, 0, 2, 0, 0, 32]},
    ("SPECIES_MISDREAVUS", "Recommended"): {"stat_points": [32, 0, 2, 0, 0, 32]},
    ("SPECIES_RIOLU", "Recommended"): {"stat_points": [32, 0, 2, 0, 0, 32]},
    ("SPECIES_LIEPARD", "Recommended"): {"stat_points": [32, 0, 2, 0, 0, 32]},
    ("SPECIES_COTTONEE", "Recommended"): {"stat_points": [32, 0, 2, 0, 0, 32]},
    ("SPECIES_SOLOSIS", "Recommended"): {"stat_points": [32, 0, 16, 0, 18, 0]},
    ("SPECIES_DEDENNE", "Recommended"): {"stat_points": [32, 0, 2, 0, 0, 32]},
    ("SPECIES_NOIBAT", "Recommended"): {"stat_points": [32, 0, 2, 0, 0, 32]},
    ("SPECIES_TOGEDEMARU", "Recommended"): {"stat_points": [32, 0, 2, 0, 0, 32]},
    ("SPECIES_SCATTERBUG", "Recommended"): {
        "moves": ["MOVE_RAGE_POWDER", "MOVE_STUN_SPORE", "MOVE_STRING_SHOT", "MOVE_PROTECT"],
    },
    ("SPECIES_DACHSBUN", "Doubles Support"): {"nature": "NATURE_CAREFUL"},
    ("SPECIES_SKIDDO", "Recommended"): {
        "moves": ["MOVE_BULK_UP", "MOVE_HORN_LEECH", "MOVE_STOMPING_TANTRUM", "MOVE_PROTECT"],
    },
    ("SPECIES_GIMMIGHOUL_ROAMING", "Recommended"): {
        "name": "Dual Screens",
        "moves": ["MOVE_SHADOW_BALL", "MOVE_REFLECT", "MOVE_LIGHT_SCREEN", "MOVE_PROTECT"],
        "item": "ITEM_LIGHT_CLAY",
        "role": "Fast dual-screen support",
    },
    ("SPECIES_SLIGGOO_HISUI", "Shelter Press"): {
        "name": "Curse Tank",
        "moves": ["MOVE_HEAVY_SLAM", "MOVE_BODY_SLAM", "MOVE_CURSE", "MOVE_PROTECT"],
        "stat_points": [32, 32, 2, 0, 0, 0],
        "role": "Eviolite Curse physical win condition",
    },
    ("SPECIES_ROTOM_FROST", "Recommended"): {
        "moves": ["MOVE_BLIZZARD", "MOVE_THUNDERBOLT", "MOVE_ELECTROWEB", "MOVE_PROTECT"],
    },
    ("SPECIES_ROTOM_FAN", "Recommended"): {
        "moves": ["MOVE_AIR_SLASH", "MOVE_THUNDERBOLT", "MOVE_ELECTROWEB", "MOVE_PROTECT"],
    },

    # Abilities whose activation condition was impossible in the authored
    # orientation are replaced by a legal, useful Ability for that role.
    ("SPECIES_EXEGGUTOR_ALOLA", "Recommended"): {"ability": "ABILITY_FRISK"},
    ("SPECIES_GOODRA", "Special Attacker II"): {"ability": "ABILITY_GOOEY"},
    ("SPECIES_GOURGEIST", "Trick Room"): {"ability": "ABILITY_INSOMNIA"},

    # White Herb must have a trigger; Clear Amulet is Groudon's coherent
    # physical setup item, while Drednaw's set was missing Shell Smash.
    ("SPECIES_GROUDON", "Recommended"): {"item": "ITEM_CLEAR_AMULET"},
    ("SPECIES_DREDNAW", "Bulky Setup"): {
        "name": "Shell Smash",
        "moves": ["MOVE_SHELL_SMASH", "MOVE_ROCK_SLIDE", "MOVE_LIQUIDATION", "MOVE_PROTECT"],
        "role": "White Herb Shell Smash attacker",
    },

    # Choice and Assault Vest roles cannot select Protect.  Each replacement
    # is in the species' current authored learnable pool and preserves the
    # role rather than changing the item to conceal the contradiction.
    ("SPECIES_DODRIO", "Wallbreaker"): {
        "moves": ["MOVE_DOUBLE_EDGE", "MOVE_QUICK_ATTACK", "MOVE_BRAVE_BIRD", "MOVE_KNOCK_OFF"],
    },
    ("SPECIES_LANTURN", "Bulky Attacker"): {
        "moves": ["MOVE_VOLT_SWITCH", "MOVE_SCALD", "MOVE_ICE_BEAM", "MOVE_ELECTROWEB"],
    },
    ("SPECIES_TYRANITAR", "Choice Attacker"): {
        "moves": ["MOVE_ROCK_SLIDE", "MOVE_LOW_KICK", "MOVE_ICE_PUNCH", "MOVE_CRUNCH"],
    },
    ("SPECIES_FLOATZEL", "Wallbreaker"): {
        "moves": ["MOVE_AQUA_JET", "MOVE_CLOSE_COMBAT", "MOVE_GIGA_IMPACT", "MOVE_FLIP_TURN"],
    },
    ("SPECIES_ELECTIVIRE", "Bulky Attacker"): {
        "moves": ["MOVE_ELECTROWEB", "MOVE_ICE_PUNCH", "MOVE_WILD_CHARGE", "MOVE_STOMPING_TANTRUM"],
    },
    ("SPECIES_ROTOM", "Choice Attacker"): {
        "moves": ["MOVE_VOLT_SWITCH", "MOVE_THUNDERBOLT", "MOVE_DISCHARGE", "MOVE_TRICK"],
    },
    ("SPECIES_BASCULIN", "Wallbreaker"): {
        "moves": ["MOVE_FLIP_TURN", "MOVE_PSYCHIC_FANGS", "MOVE_AQUA_JET", "MOVE_WAVE_CRASH"],
    },
    ("SPECIES_TERRAKION", "Wallbreaker"): {
        "moves": ["MOVE_CLOSE_COMBAT", "MOVE_ROCK_SLIDE", "MOVE_STONE_EDGE", "MOVE_HIGH_HORSEPOWER"],
    },
    ("SPECIES_FLOETTE", "Choice Attacker"): {
        "moves": ["MOVE_MOONBLAST", "MOVE_DAZZLING_GLEAM", "MOVE_ENERGY_BALL", "MOVE_PSYCHIC"],
    },
    ("SPECIES_BRUXISH", "Choice Attacker"): {
        "moves": ["MOVE_CRUNCH", "MOVE_PSYCHIC_FANGS", "MOVE_LIQUIDATION", "MOVE_ICE_FANG"],
    },
    ("SPECIES_STONJOURNER", "Choice Attacker"): {
        "moves": ["MOVE_HEAT_CRASH", "MOVE_STONE_EDGE", "MOVE_ROCK_SLIDE", "MOVE_HEAVY_SLAM"],
    },
    ("SPECIES_TATSUGIRI", "Bulky Attacker"): {
        "moves": ["MOVE_MUDDY_WATER", "MOVE_RAPID_SPIN", "MOVE_ICY_WIND", "MOVE_DRAGON_PULSE"],
    },
    ("SPECIES_CHI_YU", "Choice Attacker"): {
        "moves": ["MOVE_DARK_PULSE", "MOVE_OVERHEAT", "MOVE_HYPER_BEAM", "MOVE_HEAT_WAVE"],
    },
    ("SPECIES_RAGING_BOLT", "Bulky Attacker"): {
        "moves": ["MOVE_THUNDERCLAP", "MOVE_THUNDERBOLT", "MOVE_DRAGON_PULSE", "MOVE_VOLT_SWITCH"],
    },

    # This was the only real attack-category allocation reversal.
    ("SPECIES_KINGDRA", "Setup Sweeper"): {
        "name": "Rain Sweeper",
        "nature": "NATURE_MODEST",
        "stat_points": [2, 0, 0, 32, 0, 32],
        "role": "Swift Swim Rain Dance special sweeper",
    },

    # These three handbook roles describe Megas that now exist in the current
    # engine.  Keep them hidden until Mega access, use the transformed Ability,
    # and never place the progression stone in the supplied-item field.
    ("SPECIES_CLEFABLE", "Special Attacker"): {
        "name": "Mega Special Attacker",
        "ability": "ABILITY_MAGIC_BOUNCE",
        "item": "ITEM_NONE",
        "required_item": "ITEM_CLEFABLITE",
    },
    ("SPECIES_MEOWSTIC", "Special Attacker"): {
        "species": "SPECIES_MEOWSTIC_F",
        "name": "Mega Special Attacker",
        "ability": "ABILITY_TRACE",
        "item": "ITEM_NONE",
        "required_item": "ITEM_MEOWSTICITE",
    },
    ("SPECIES_DRAMPA", "Special Attacker"): {
        "name": "Mega Special Attacker",
        "moves": ["MOVE_PROTECT", "MOVE_HYPER_VOICE", "MOVE_EARTH_POWER", "MOVE_DRAGON_PULSE"],
        "item": "ITEM_NONE",
        "required_item": "ITEM_DRAMPANITE",
    },
}


def apply_audited_set_override(entry: dict) -> dict:
    changes = AUDITED_SET_FIELD_OVERRIDES.get((entry["species"], entry["name"]))
    if changes is None:
        return entry
    result = {**entry, **changes}
    result["audit_note"] = "Emerald Champions executable set-coherence review"
    return result


def git_json(path: str) -> dict:
    result = subprocess.run(
        ["git", "show", f"{SOURCE_COMMIT}:{path}"],
        check=True,
        stdout=subprocess.PIPE,
    )
    return json.loads(result.stdout)


def handbook_json() -> dict:
    handbook = git_json(HANDBOOK_SOURCE)
    assert handbook["source_file"] == "pokemon_champions_all_species_doubles_handbook.docx"
    assert handbook["source_sha256"] == HANDBOOK_SHA256
    assert handbook["declared_species_count"] == 1025
    assert handbook["declared_set_count"] == len(handbook["sets"]) == 1216
    return handbook


def shorten_name(name: str) -> str:
    name = name.replace(" — ", " ").replace("-Mega", " Mega").strip()
    return name[:23]


def compact_role_name(role: str) -> str:
    """Turn authored prose into a short native menu label."""
    value = role.replace("—", " ").replace("/", " ")
    value = re.sub(r"\b(?:distinct|doubles|singles|self-contained|reliable)\b", "", value, flags=re.I)
    value = re.sub(r"^(?:minimal)\s+", "", value, flags=re.I)
    value = re.sub(r"\b(?:and|with)\b", "", value, flags=re.I)
    value = re.sub(r"\s+", " ", value).strip().title()
    replacements = {
        "Speed-Control": "Speed Control",
        "Partner Support": "Support",
        "Role Synthesis": "Utility",
        "Role": "Utility",
    }
    for old, new in replacements.items():
        value = value.replace(old, new)
    if not value or value == "Recommended":
        value = "Competitive Utility"
    if len(value) <= 23:
        return value
    # Preserve whole words; a clean short label is preferable to a clipped
    # final word in the native scrolling menu.
    words: list[str] = []
    for word in value.split():
        if len(" ".join([*words, word])) > 23:
            break
        words.append(word)
    return " ".join(words) or value[:23]


def name_doubles_defaults(defaults: list[dict]) -> list[dict]:
    result: list[dict] = []
    for source in defaults:
        entry = dict(source)
        if entry["name"] == "Recommended":
            entry["name"] = compact_role_name(entry["role"])
        result.append(entry)
    return result


def repair_retired_ability_labels(entries: list[dict]) -> list[dict]:
    """Keep exposed names/roles consistent with the Ability actually applied."""
    result: list[dict] = []
    for source in entries:
        entry = dict(source)
        for (species, retired), replacement in ABILITY_ALIASES.items():
            if entry["species"] != species or entry["ability"] != replacement:
                continue
            retired_name = retired.removeprefix("ABILITY_").replace("_", " ").title()
            replacement_name = replacement.removeprefix("ABILITY_").replace("_", " ").title()
            entry["role"] = re.sub(
                re.escape(retired_name), replacement_name, entry["role"], flags=re.I
            )
            entry["name"] = re.sub(
                re.escape(retired_name), replacement_name, entry["name"], flags=re.I
            )
        result.append(entry)
    return result


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

    if nature in PHYSICAL_WALL_NATURES or re.search(r"\bPHYSICAL WALL\b", text) or "PHYSICALLY DEFENSIVE" in text:
        return [32, 0, 32, 0, 2, 0]
    if nature in SPECIAL_WALL_NATURES or re.search(r"\bSPECIAL WALL\b", text) or "SPECIALLY DEFENSIVE" in text:
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


def move_metadata() -> dict[str, dict]:
    """Read the configured move table closely enough to rank legal options."""
    text = (ROOT / "src/data/moves_info.h").read_text()
    markers = list(re.finditer(r"(?m)^\s*\[(MOVE_[A-Z0-9_]+)\]\s*=\s*\{", text))
    result: dict[str, dict] = {}
    for index, marker in enumerate(markers):
        end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
        block = text[marker.end():end]
        category = re.search(r"\.category\s*=\s*(DAMAGE_CATEGORY_[A-Z]+)", block)
        move_type = re.search(r"\.type\s*=\s*[^,]*(TYPE_[A-Z0-9_]+)", block)
        target = re.search(r"\.target\s*=\s*(TARGET_[A-Z0-9_]+)", block)
        power_expr = re.search(r"\.power\s*=\s*([^,]+)", block)
        accuracy_expr = re.search(r"\.accuracy\s*=\s*([^,]+)", block)
        priority_expr = re.search(r"\.priority\s*=\s*([^,]+)", block)

        def numeric(match: re.Match[str] | None, fallback: int) -> int:
            if match is None:
                return fallback
            values = [int(value) for value in re.findall(r"\b\d+\b", match.group(1))]
            return max(values) if values else fallback

        result[marker.group(1)] = {
            "category": category.group(1) if category else "DAMAGE_CATEGORY_STATUS",
            "type": move_type.group(1) if move_type else "TYPE_NORMAL",
            "target": target.group(1) if target else "TARGET_SELECTED",
            "power": numeric(power_expr, 0),
            "accuracy": numeric(accuracy_expr, 100),
            "priority": numeric(priority_expr, 0),
            "biting": ".bitingMove = TRUE" in block,
            "contact": ".makesContact = TRUE" in block,
            "punching": ".punchingMove = TRUE" in block,
            "pulse": ".pulseMove = TRUE" in block,
            "slicing": ".slicingMove = TRUE" in block,
            "sound": ".soundMove = TRUE" in block,
        }
    return result


def species_build_metadata() -> dict[str, dict]:
    """Resolve the same configured species table consumed by the ROM."""
    text = preprocess_species_info()
    table_start = text.find("const struct SpeciesInfo gSpeciesInfo[]")
    assert table_start >= 0
    text = text[table_start:]
    markers = list(SPECIES_MARKER.finditer(text))
    result: dict[str, dict] = {}
    for index, marker in enumerate(markers):
        end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
        block = text[marker.start():end]

        def stat(field: str) -> int:
            match = re.search(rf"\.{field}\s*=\s*(\d+)", block)
            return int(match.group(1)) if match else 0

        type_match = re.search(r"\.types\s*=\s*\{([^}]+)\}", block)
        learnset_match = re.search(
            r"\.teachableLearnset\s*=\s*s([A-Za-z0-9]+)TeachableLearnset",
            block,
        )
        ability_match = re.search(r"\.abilities\s*=\s*\{([^}]+)\}", block)
        result[marker.group(1)] = {
            "hp": stat("baseHP"),
            "attack": stat("baseAttack"),
            "defense": stat("baseDefense"),
            "sp_attack": stat("baseSpAttack"),
            "sp_defense": stat("baseSpDefense"),
            "speed": stat("baseSpeed"),
            "types": tuple(dict.fromkeys(re.findall(
                r"TYPE_[A-Z0-9_]+", type_match.group(1) if type_match else ""
            ))),
            "abilities": tuple(dict.fromkeys(re.findall(
                r"ABILITY_[A-Z0-9_]+", ability_match.group(1) if ability_match else ""
            ))),
            "learnset_key": (
                re.sub(r"([a-z0-9])([A-Z])", r"\1_\2", learnset_match.group(1)).upper()
                if learnset_match else ""
            ),
            "evolves": ".evolutions" in block,
        }

    aliases = species_aliases()
    for alias in aliases:
        target = resolve_species(alias, aliases)
        if target in result:
            result.setdefault(alias, result[target])
    return result


def legal_moves_for_species(
    species: str,
    existing: list[dict],
    metadata: dict[str, dict],
    learnables: dict[str, list[str]],
) -> set[str]:
    info = metadata.get(species, {})
    keys = {
        species.removeprefix("SPECIES_"),
        info.get("learnset_key", ""),
    }
    legal = set().union(*(set(learnables.get(key, [])) for key in keys if key))
    # Reviewed imported moves are executable and legal even when they enter
    # through the small explicit move-access extension manifest.
    legal.update(move for entry in existing for move in entry["moves"])
    return legal


def ranked_attacks(
    legal: set[str],
    category: str,
    species_types: tuple[str, ...],
    moves: dict[str, dict],
    ability: str,
) -> list[str]:
    candidates: list[tuple[int, str]] = []
    spread_targets = {
        "TARGET_BOTH", "TARGET_FOES_AND_ALLY", "TARGET_OPPONENTS_FIELD",
        "TARGET_ALL_BATTLERS", "TARGET_ALLY_AND_USER",
    }
    skill_link_moves = {
        "MOVE_ARM_THRUST", "MOVE_BARRAGE", "MOVE_BONE_RUSH",
        "MOVE_BULLET_SEED", "MOVE_DOUBLE_SLAP", "MOVE_FURY_ATTACK",
        "MOVE_FURY_SWIPES", "MOVE_ICICLE_SPEAR", "MOVE_PIN_MISSILE",
        "MOVE_POPULATION_BOMB", "MOVE_ROCK_BLAST", "MOVE_SCALE_SHOT",
        "MOVE_SPIKE_CANNON", "MOVE_TAIL_SLAP", "MOVE_TRIPLE_KICK",
    }
    for move in legal:
        info = moves.get(move)
        if info is None or info["category"] != category:
            continue
        if move in UNSUITABLE_SYNTHETIC_ATTACKS:
            continue
        ate_abilities = {
            "ABILITY_AERILATE", "ABILITY_GALVANIZE", "ABILITY_PIXILATE",
            "ABILITY_REFRIGERATE",
        }
        if (
            move == "MOVE_DOUBLE_EDGE"
            and "TYPE_NORMAL" not in species_types
            and ability not in ate_abilities
        ):
            continue
        power = info["power"]
        if (
            power < 40
            and info["priority"] <= 0
            and not (ability == "ABILITY_SKILL_LINK" and move in skill_link_moves)
        ):
            continue
        score = power
        if ability == "ABILITY_SKILL_LINK" and move in skill_link_moves:
            score = max(score, power * 5)
        if info["type"] in species_types or (
            info["type"] == "TYPE_NORMAL" and ability in ate_abilities
        ):
            score += 42
        if (
            move == "MOVE_FACADE"
            and ability in {
                "ABILITY_GUTS", "ABILITY_POISON_HEAL", "ABILITY_QUICK_FEET",
                "ABILITY_TOXIC_BOOST",
            }
        ):
            score += 80
        if ability == "ABILITY_TRIAGE" and move in {
            "MOVE_DRAIN_PUNCH", "MOVE_DRAINING_KISS", "MOVE_GIGA_DRAIN",
            "MOVE_HORN_LEECH", "MOVE_LEECH_LIFE", "MOVE_PARABOLIC_CHARGE",
        }:
            score += 80
        if ability == "ABILITY_TECHNICIAN" and power <= 60:
            score += 35
        if ability == "ABILITY_IRON_FIST" and info["punching"]:
            score += 35
        if ability == "ABILITY_STRONG_JAW" and info["biting"]:
            score += 35
        if ability == "ABILITY_SHARPNESS" and info["slicing"]:
            score += 35
        if ability == "ABILITY_MEGA_LAUNCHER" and info["pulse"]:
            score += 35
        if ability == "ABILITY_PUNK_ROCK" and info["sound"]:
            score += 35
        if ability == "ABILITY_TOUGH_CLAWS" and info["contact"]:
            score += 20
        if info["target"] in spread_targets:
            score += 15
        if info["priority"] > 0:
            score += 18 + 4 * info["priority"]
        if info["accuracy"] and info["accuracy"] < 90:
            score -= 35
        candidates.append((score, move))
    return [move for _, move in sorted(candidates, key=lambda row: (-row[0], row[1]))]


def diverse_attacks(ranked: list[str], moves: dict[str, dict], limit: int) -> list[str]:
    selected: list[str] = []
    seen_types: set[str] = set()
    for move in ranked:
        move_type = moves[move]["type"]
        if move_type in seen_types:
            continue
        selected.append(move)
        seen_types.add(move_type)
        if len(selected) == limit:
            return selected
    for move in ranked:
        if move not in selected:
            selected.append(move)
            if len(selected) == limit:
                break
    return selected


def unique_available(sequence: tuple[str, ...] | list[str], legal: set[str]) -> list[str]:
    return list(dict.fromkeys(move for move in sequence if move in legal))


def coherent_item(ability: str, preferred: str) -> str:
    if ability == "ABILITY_GUTS":
        return "ITEM_FLAME_ORB"
    if ability in {"ABILITY_QUICK_FEET", "ABILITY_TOXIC_BOOST"}:
        return "ITEM_TOXIC_ORB"
    if ability == "ABILITY_POISON_HEAL":
        return "ITEM_TOXIC_ORB"
    if ability == "ABILITY_FLARE_BOOST":
        return "ITEM_FLAME_ORB"
    if ability == "ABILITY_HARVEST":
        return "ITEM_SITRUS_BERRY"
    if ability == "ABILITY_UNBURDEN" and preferred not in {
        "ITEM_FOCUS_SASH", "ITEM_SITRUS_BERRY", "ITEM_WHITE_HERB",
        "ITEM_ELECTRIC_SEED", "ITEM_GRASSY_SEED", "ITEM_MISTY_SEED",
        "ITEM_PSYCHIC_SEED", "ITEM_POWER_HERB", "ITEM_WEAKNESS_POLICY",
    }:
        return "ITEM_SITRUS_BERRY"
    return preferred


def make_synthetic_set(
    base: dict,
    name: str,
    move_list: list[str],
    nature: str,
    item: str,
    stat_points: list[int],
    role: str,
    ability: str | None = None,
) -> dict:
    return {
        "species": base["species"],
        "name": shorten_name(name),
        "moves": list(dict.fromkeys(move_list))[:4],
        "nature": nature,
        "ability": ability or base["ability"],
        "item": coherent_item(ability or base["ability"], item),
        "required_item": "ITEM_NONE",
        "stat_points": stat_points,
        "role": role,
        "source": (
            "Emerald Champions legal doubles role synthesis; supplied handbook "
            "plus current learnables, stats, and Abilities"
        ),
    }


def is_genuinely_distinct(candidate: dict, existing: list[dict]) -> bool:
    """Reject renames, move-order shuffles, and item-only alternatives."""
    candidate_moves = frozenset(candidate["moves"])
    for current in existing:
        if current["required_item"] != "ITEM_NONE":
            continue
        moves_changed = candidate_moves != frozenset(current["moves"])
        ability_changed = candidate["ability"] != current["ability"]
        build_changed = (
            candidate["nature"] != current["nature"]
            or tuple(candidate["stat_points"]) != tuple(current["stat_points"])
        )
        if not (moves_changed or ability_changed or build_changed):
            return False
    return True


def offensive_candidates(
    base: dict,
    legal: set[str],
    info: dict,
    move_info: dict[str, dict],
) -> list[dict]:
    result: list[dict] = []
    protect = (
        ["MOVE_PROTECT"]
        if "MOVE_PROTECT" in legal and base["ability"] != "ABILITY_GORILLA_TACTICS"
        else []
    )
    categories = [
        ("physical", "DAMAGE_CATEGORY_PHYSICAL", info.get("attack", 0)),
        ("special", "DAMAGE_CATEGORY_SPECIAL", info.get("sp_attack", 0)),
    ]
    categories.sort(key=lambda row: (-row[2], row[0]))

    for label, category, attack_stat in categories:
        ranked = ranked_attacks(
            legal, category, info.get("types", ()), move_info, base["ability"]
        )
        if not ranked:
            continue
        selected = diverse_attacks(ranked, move_info, 3 if protect else 4)
        if len(selected) < 2:
            continue
        setup_pool = PHYSICAL_SETUP_MOVES if label == "physical" else SPECIAL_SETUP_MOVES
        setup = unique_available(setup_pool, legal)
        if base["ability"] in {"ABILITY_CONTRARY", "ABILITY_GORILLA_TACTICS"}:
            setup = []
        fast = info.get("speed", 0) >= 70
        if label == "physical":
            nature = "NATURE_JOLLY" if fast else "NATURE_ADAMANT"
            points = [2, 32, 0, 0, 0, 32] if fast else [32, 32, 2, 0, 0, 0]
        else:
            nature = "NATURE_TIMID" if fast else "NATURE_MODEST"
            points = [2, 0, 0, 32, 0, 32] if fast else [32, 0, 2, 32, 0, 0]

        if setup and len(selected) >= 2:
            setup_move = setup[0]
            setup_name = setup_move.removeprefix("MOVE_").replace("_", " ").title()
            if setup_move == "MOVE_SHELL_SMASH":
                setup_item = "ITEM_WHITE_HERB"
            elif label == "special":
                setup_item = "ITEM_LIFE_ORB"
            elif info.get("speed", 0) < 60:
                setup_item = "ITEM_SITRUS_BERRY"
            else:
                setup_item = "ITEM_CLEAR_AMULET"
            result.append(make_synthetic_set(
                base,
                f"{setup_name} Setup",
                [setup_move, *selected[:2], *protect],
                nature,
                setup_item,
                points,
                f"distinct doubles {label} setup pressure",
            ))

        support = unique_available(GENERAL_SUPPORT_MOVES, legal)
        pressure_moves = [*selected, *protect]
        if len(pressure_moves) < 4:
            pressure_moves.extend(move for move in support if move not in pressure_moves)
        result.append(make_synthetic_set(
            base,
            "Physical Pressure" if label == "physical" else "Special Pressure",
            pressure_moves,
            nature,
            "ITEM_LIFE_ORB" if attack_stat >= 75 else "ITEM_FOCUS_SASH",
            points,
            f"doubles {label} attacker with coverage and positioning",
        ))
    return result


def support_candidates(
    base: dict,
    legal: set[str],
    info: dict,
    move_info: dict[str, dict],
) -> list[dict]:
    if base["ability"] == "ABILITY_GORILLA_TACTICS":
        return []
    result: list[dict] = []
    physical = ranked_attacks(
        legal, "DAMAGE_CATEGORY_PHYSICAL", info.get("types", ()), move_info,
        base["ability"],
    )
    special = ranked_attacks(
        legal, "DAMAGE_CATEGORY_SPECIAL", info.get("types", ()), move_info,
        base["ability"],
    )
    attack_pool = physical if info.get("attack", 0) >= info.get("sp_attack", 0) else special
    attack_category = "physical" if attack_pool is physical else "special"
    best_attack = attack_pool[:1] or (special[:1] if attack_pool is physical else physical[:1])

    for name, primary, companions, style in ROLE_BLUEPRINTS:
        if primary not in legal:
            continue
        if name == "Dual Screens" and not {
            "MOVE_LIGHT_SCREEN", "MOVE_AURORA_VEIL"
        }.intersection(legal):
            continue
        chosen = [primary]
        chosen.extend(unique_available(companions, legal)[:1])
        chosen.extend(move for move in best_attack if move != primary)
        if "MOVE_PROTECT" in legal:
            chosen.append("MOVE_PROTECT")
        if len(chosen) < 4:
            chosen.extend(
                move for move in unique_available(GENERAL_SUPPORT_MOVES, legal)
                if move not in chosen
            )
        if style == "slow_support":
            nature = "NATURE_BRAVE" if attack_category == "physical" else "NATURE_QUIET"
            points = [32, 0, 16, 0, 18, 0]
        elif style == "fast_support":
            nature = "NATURE_JOLLY" if attack_category == "physical" else "NATURE_TIMID"
            points = [32, 0, 2, 0, 0, 32]
        else:
            nature = "NATURE_CAREFUL" if attack_category == "physical" else "NATURE_BOLD"
            points = [32, 0, 16, 0, 18, 0]

        if name == "Dual Screens":
            item = "ITEM_LIGHT_CLAY"
        elif info.get("evolves", False):
            item = "ITEM_EVIOLITE"
        else:
            item = "ITEM_SITRUS_BERRY"
        result.append(make_synthetic_set(
            base,
            name,
            chosen,
            nature,
            item,
            points,
            f"distinct doubles {name.lower()} role",
        ))
    return result


def fallback_candidate(
    base: dict,
    legal: set[str],
    info: dict,
    move_info: dict[str, dict],
) -> dict:
    """Cover legitimately narrow species without pretending they have moves."""
    species = base["species"]
    if species == "SPECIES_DITTO":
        return make_synthetic_set(
            base,
            "Manual Transform",
            ["MOVE_TRANSFORM"],
            "NATURE_TIMID",
            "ITEM_QUICK_POWDER",
            [32, 0, 2, 0, 0, 32],
            "Limber manual-Transform speed orientation",
            ability="ABILITY_LIMBER",
        )

    physical = [
        move for move in legal
        if move_info.get(move, {}).get("category") == "DAMAGE_CATEGORY_PHYSICAL"
    ]
    special = [
        move for move in legal
        if move_info.get(move, {}).get("category") == "DAMAGE_CATEGORY_SPECIAL"
    ]
    default_physical = sum(
        move_info.get(move, {}).get("category") == "DAMAGE_CATEGORY_PHYSICAL"
        for move in base["moves"]
    )
    default_special = sum(
        move_info.get(move, {}).get("category") == "DAMAGE_CATEGORY_SPECIAL"
        for move in base["moves"]
    )
    use_physical = bool(physical) and (
        not special or info.get("attack", 0) >= info.get("sp_attack", 0)
    )
    if use_physical:
        nature = "NATURE_JOLLY" if base["nature"] != "NATURE_JOLLY" else "NATURE_CAREFUL"
        points = [2, 32, 0, 0, 0, 32] if nature == "NATURE_JOLLY" else [32, 32, 2, 0, 0, 0]
        name = "Fast Utility" if nature == "NATURE_JOLLY" else "Bulky Utility"
        item = "ITEM_CHOICE_SCARF" if not any(
            move_info.get(move, {}).get("category") == "DAMAGE_CATEGORY_STATUS"
            for move in base["moves"]
        ) else "ITEM_FOCUS_SASH"
    else:
        nature = "NATURE_TIMID" if base["nature"] != "NATURE_TIMID" else "NATURE_BOLD"
        points = [2, 0, 0, 32, 0, 32] if nature == "NATURE_TIMID" else [32, 0, 32, 0, 2, 0]
        name = "Fast Utility" if nature == "NATURE_TIMID" else "Bulky Utility"
        item = "ITEM_CHOICE_SPECS" if not any(
            move_info.get(move, {}).get("category") == "DAMAGE_CATEGORY_STATUS"
            for move in base["moves"]
        ) else "ITEM_FOCUS_SASH"

    # Add one legal unused move when the species has one; Metapod-like narrow
    # pools therefore become real control-versus-defense choices. Ditto and
    # Unown still receive materially different nature/Stat Point orientations.
    moves = list(base["moves"])
    unused = sorted(legal - set(moves))
    if unused:
        if len(moves) == 4:
            moves[-1] = unused[0]
        else:
            moves.append(unused[0])
    return make_synthetic_set(
        base,
        name,
        moves,
        nature,
        item,
        points,
        (
            "speed-oriented narrow-pool utility" if "Fast" in name
            else "bulk-oriented narrow-pool utility"
        ),
    )


def ensure_minimum_non_mega_orientations(
    defaults: list[dict], alternatives: list[dict]
) -> list[dict]:
    """Give every direct species/form two distinct pre-Mega choices."""
    by_species: dict[str, list[dict]] = defaultdict(list)
    for entry in defaults + alternatives:
        by_species[entry["species"]].append(entry)

    metadata = species_build_metadata()
    move_info = move_metadata()
    learnables = json.loads((ROOT / "src/data/pokemon/all_learnables.json").read_text())
    synthesized: list[dict] = []
    for default in defaults:
        species = default["species"]
        existing = by_species[species]
        if sum(entry["required_item"] == "ITEM_NONE" for entry in existing) >= 2:
            continue
        base = next(entry for entry in existing if entry["required_item"] == "ITEM_NONE")
        info = metadata.get(species, {})
        legal = legal_moves_for_species(species, existing, metadata, learnables)
        status_count = sum(
            move_info.get(move, {}).get("category") == "DAMAGE_CATEGORY_STATUS"
            and move not in {"MOVE_PROTECT", "MOVE_DETECT", "MOVE_ENDURE"}
            for move in base["moves"]
        )
        candidates = (
            offensive_candidates(base, legal, info, move_info)
            + support_candidates(base, legal, info, move_info)
            if status_count >= 2
            else support_candidates(base, legal, info, move_info)
            + offensive_candidates(base, legal, info, move_info)
        )
        candidates.append(fallback_candidate(base, legal, info, move_info))
        used_names = {entry["name"] for entry in existing}
        choice = next(
            (
                candidate for candidate in candidates
                if candidate["name"] not in used_names
                and candidate["moves"]
                and is_genuinely_distinct(candidate, existing)
            ),
            None,
        )
        assert choice is not None, f"no genuinely distinct second orientation for {species}"
        synthesized.append(choice)
        existing.append(choice)
    # Scovillain now contributes its reviewed ordinary default directly
    # instead of consuming one emergency synthetic orientation.
    assert len(synthesized) == 1086, len(synthesized)
    return alternatives + synthesized


def complete_battle_sets(defaults: list[dict], alternatives: list[dict]) -> tuple[list[dict], list[dict]]:
    """Fill every set to four moves unless the species truly lacks four."""
    metadata = species_build_metadata()
    move_info = move_metadata()
    learnables = json.loads((ROOT / "src/data/pokemon/all_learnables.json").read_text())
    by_species: dict[str, list[dict]] = defaultdict(list)
    for entry in defaults + alternatives:
        by_species[entry["species"]].append(entry)

    def complete(source: dict) -> dict:
        entry = dict(source)
        moves = list(entry["moves"])
        if len(moves) >= 4:
            return entry
        species = entry["species"]
        legal = legal_moves_for_species(species, by_species[species], metadata, learnables)
        if len(legal) < 4:
            return entry
        info = metadata.get(species, {})
        # Preserve the species' authored default vocabulary first. This makes
        # narrow lines such as Wurmple/Cascoon complete without inventing an
        # unrelated fourth role.
        default = next(row for row in defaults if row["species"] == species)
        candidates = list(default["moves"])
        physical = ranked_attacks(
            legal, "DAMAGE_CATEGORY_PHYSICAL", info.get("types", ()), move_info,
            entry["ability"],
        )
        special = ranked_attacks(
            legal, "DAMAGE_CATEGORY_SPECIAL", info.get("types", ()), move_info,
            entry["ability"],
        )
        candidates.extend(physical if info.get("attack", 0) >= info.get("sp_attack", 0) else special)
        candidates.extend(special if info.get("attack", 0) >= info.get("sp_attack", 0) else physical)
        candidates.extend(unique_available(GENERAL_SUPPORT_MOVES, legal))
        candidates.extend(sorted(legal))
        for move in candidates:
            if move not in moves:
                moves.append(move)
                if len(moves) == 4:
                    break
        assert len(moves) == 4, (species, entry["name"], moves, len(legal))
        entry["moves"] = moves
        return entry

    return [complete(entry) for entry in defaults], [complete(entry) for entry in alternatives]


def _rank_showdown_single_attacks(
    pool: set[str],
    species_types: tuple[str, ...],
    move_info: dict[str, dict],
    preferred_type: str,
) -> list[str]:
    scored: list[tuple[int, str]] = []
    for move in pool:
        info = move_info.get(move)
        if info is None or info["category"] == "DAMAGE_CATEGORY_STATUS":
            continue
        score = info["power"]
        if info["type"] in species_types:
            score += 45
        if preferred_type != "TYPE_NONE" and info["type"] == preferred_type:
            score += 20
        if info["priority"] > 0:
            score += 20
        if info["accuracy"] and info["accuracy"] < 90:
            score -= 25
        scored.append((score, move))
    return [move for _, move in sorted(scored, key=lambda row: (-row[0], row[1]))]


def _choose_showdown_single_moves(
    template: dict,
    info: dict,
    legal: set[str],
    move_info: dict[str, dict],
) -> list[str]:
    pool = {
        move for move in template["moves"]
        if move in legal and move not in SINGLES_EXCLUDED_MOVES
    }
    role = template["role"]
    selected: list[str] = []

    def add(move: str) -> None:
        if move in pool and move not in selected and len(selected) < 4:
            selected.append(move)

    if "Setup" in role:
        for move in SINGLES_SETUP_MOVES:
            if move in pool:
                add(move)
                break
    if role.startswith("Bulky"):
        for move in SINGLES_RECOVERY_MOVES:
            if move in pool:
                add(move)
                break
    if "Support" in role:
        for move in SINGLES_UTILITY_MOVES:
            add(move)
            if len(selected) >= 2:
                break

    ranked = _rank_showdown_single_attacks(
        pool, info.get("types", ()), move_info, template.get("preferred_type", "TYPE_NONE")
    )
    attack_limit = 1 if "Support" in role else 2
    for move in diverse_attacks(ranked, move_info, attack_limit):
        add(move)

    # Showdown's movepool encodes deliberate combinations. Prefer setup,
    # recovery, and utility before merely taking the remaining moves in source
    # order.
    for move in (*SINGLES_SETUP_MOVES, *SINGLES_RECOVERY_MOVES, *SINGLES_UTILITY_MOVES):
        add(move)
    for move in ranked:
        add(move)
    for move in template["moves"]:
        add(move)

    if len(selected) < 4:
        fallback = _rank_showdown_single_attacks(
            legal - SINGLES_EXCLUDED_MOVES,
            info.get("types", ()),
            move_info,
            template.get("preferred_type", "TYPE_NONE"),
        )
        for move in fallback:
            add(move)
    for move in (*SINGLES_RECOVERY_MOVES, *SINGLES_UTILITY_MOVES, "MOVE_PROTECT"):
        if move in legal:
            pool.add(move)
            add(move)
    return selected


def _single_build(
    role: str,
    moves: list[str],
    info: dict,
    move_info: dict[str, dict],
) -> tuple[str, list[int]]:
    physical = sum(
        move_info.get(move, {}).get("category") == "DAMAGE_CATEGORY_PHYSICAL"
        and move not in SINGLES_DAMAGE_INDEPENDENT_MOVES
        for move in moves
    )
    special = sum(
        move_info.get(move, {}).get("category") == "DAMAGE_CATEGORY_SPECIAL"
        and move not in SINGLES_DAMAGE_INDEPENDENT_MOVES
        for move in moves
    )
    slow = "MOVE_TRICK_ROOM" in moves or info.get("speed", 0) <= 45
    support = (
        any(marker in role for marker in ("Support", "Control", "Utility", "Redirection", "Disruption"))
        or re.search(r"\bWall\b", role) is not None
    ) and max(physical, special) <= 1
    bulky = role.startswith("Bulky")
    if support:
        if slow:
            return ("NATURE_SASSY" if special >= physical else "NATURE_CAREFUL", [32, 0, 17, 0, 17, 0])
        return ("NATURE_CALM" if special >= physical else "NATURE_CAREFUL", [32, 0, 17, 0, 17, 0])
    if physical > special:
        if slow:
            return "NATURE_BRAVE", [32, 32, 2, 0, 0, 0]
        if bulky:
            return "NATURE_ADAMANT", [32, 32, 2, 0, 0, 0]
        return "NATURE_JOLLY", [2, 32, 0, 0, 0, 32]
    if slow:
        return "NATURE_QUIET", [32, 0, 2, 32, 0, 0]
    if bulky:
        return "NATURE_MODEST", [32, 0, 2, 32, 0, 0]
    return "NATURE_TIMID", [2, 0, 0, 32, 0, 32]


def _single_item(
    role: str,
    moves: list[str],
    ability: str,
    info: dict,
    move_info: dict[str, dict],
) -> str:
    if ability in {"ABILITY_GUTS", "ABILITY_FLARE_BOOST"}:
        return "ITEM_FLAME_ORB"
    if ability in {"ABILITY_POISON_HEAL", "ABILITY_QUICK_FEET", "ABILITY_TOXIC_BOOST"}:
        return "ITEM_TOXIC_ORB"
    if "MOVE_SHELL_SMASH" in moves:
        return "ITEM_WHITE_HERB"
    if "MOVE_AURORA_VEIL" in moves or {"MOVE_REFLECT", "MOVE_LIGHT_SCREEN"}.issubset(moves):
        return "ITEM_LIGHT_CLAY"
    if ability in {"ABILITY_MAGIC_GUARD", "ABILITY_SHEER_FORCE"} and "Support" not in role:
        return "ITEM_LIFE_ORB"
    if role == "Wallbreaker":
        if any(
            move_info.get(move, {}).get("category") == "DAMAGE_CATEGORY_STATUS"
            and move not in {"MOVE_TRANSFORM", "MOVE_TRICK", "MOVE_SWITCHEROO"}
            for move in moves
        ):
            return "ITEM_LIFE_ORB"
        physical = sum(move_info.get(move, {}).get("category") == "DAMAGE_CATEGORY_PHYSICAL" for move in moves)
        special = sum(move_info.get(move, {}).get("category") == "DAMAGE_CATEGORY_SPECIAL" for move in moves)
        return "ITEM_CHOICE_BAND" if physical > special else "ITEM_CHOICE_SPECS"
    if role in {"Fast Attacker", "Setup Sweeper"}:
        return "ITEM_LIFE_ORB" if info.get("hp", 0) + info.get("defense", 0) + info.get("sp_defense", 0) >= 210 else "ITEM_FOCUS_SASH"
    return "ITEM_LEFTOVERS"


def _fallback_single_set(
    base: dict,
    existing: list[dict],
    metadata: dict[str, dict],
    learnables: dict[str, list[str]],
    move_info: dict[str, dict],
) -> dict:
    species = base["species"]
    info = metadata.get(species, {})
    legal = legal_moves_for_species(species, existing, metadata, learnables)
    if species == "SPECIES_SMEARGLE":
        if "Redirection" in base["name"]:
            return {
                "species": species,
                "name": "Hazard Control",
                "moves": ["MOVE_SPORE", "MOVE_STEALTH_ROCK", "MOVE_MORTAL_SPIN", "MOVE_TAUNT"],
                "nature": "NATURE_JOLLY",
                "ability": base["ability"],
                "item": "ITEM_FOCUS_SASH",
                "required_item": "ITEM_NONE",
                "stat_points": [2, 0, 0, 0, 32, 32],
                "role": "fast singles hazard control",
                "source": "Emerald Champions authored Smeargle singles role using Sketch legality",
            }
        return {
            "species": species,
            "name": "Hazard Disruption",
            "moves": ["MOVE_SPORE", "MOVE_STICKY_WEB", "MOVE_NUZZLE", "MOVE_PARTING_SHOT"],
            "nature": "NATURE_JOLLY",
            "ability": base["ability"],
            "item": "ITEM_FOCUS_SASH",
            "required_item": "ITEM_NONE",
            "stat_points": [2, 0, 0, 0, 32, 32],
            "role": "fast singles hazard and status disruption",
            "source": "Emerald Champions authored Smeargle singles role using Sketch legality",
        }
    if species == "SPECIES_UNOWN":
        entry = dict(base)
        entry["name"] = compact_role_name(base["name"])
        entry["source"] = "Emerald Champions singles adaptation of Unown's complete legal pool"
        return entry
    selected = [
        move for move in base["moves"]
        if move not in SINGLES_EXCLUDED_MOVES and move != "MOVE_PROTECT"
    ]
    selected = list(dict.fromkeys(selected))[:4]
    ranked_physical = ranked_attacks(
        legal, "DAMAGE_CATEGORY_PHYSICAL", info.get("types", ()), move_info, base["ability"]
    )
    ranked_special = ranked_attacks(
        legal, "DAMAGE_CATEGORY_SPECIAL", info.get("types", ()), move_info, base["ability"]
    )
    base_physical = sum(
        move_info.get(move, {}).get("category") == "DAMAGE_CATEGORY_PHYSICAL"
        for move in selected
    )
    base_special = sum(
        move_info.get(move, {}).get("category") == "DAMAGE_CATEGORY_SPECIAL"
        for move in selected
    )
    if base_physical != base_special:
        ranked = ranked_physical if base_physical > base_special else ranked_special
    else:
        ranked = ranked_physical if info.get("attack", 0) >= info.get("sp_attack", 0) else ranked_special
    authored_moves = {move for entry in existing for move in entry["moves"]}
    ranked_authored = [move for move in ranked if move in authored_moves]
    for move in [*ranked_authored, *ranked, *SINGLES_RECOVERY_MOVES, *SINGLES_UTILITY_MOVES, *SINGLES_SETUP_MOVES, *base["moves"], *sorted(legal)]:
        if len(selected) >= 4:
            break
        if move in legal and move not in SINGLES_EXCLUDED_MOVES and move not in selected:
            selected.append(move)
    selected = selected[:4]
    role = compact_role_name(base["role"])
    removed_doubles_identity = any(
        move in SINGLES_EXCLUDED_MOVES and move not in selected
        for move in base["moves"]
    )
    if removed_doubles_identity:
        setup = next((move for move in SINGLES_SETUP_MOVES if move in selected), None)
        if setup is not None:
            role = compact_role_name(
                setup.removeprefix("MOVE_").replace("_", " ").title() + " Setup"
            )
        else:
            meaningful_physical = sum(
                move_info.get(move, {}).get("category") == "DAMAGE_CATEGORY_PHYSICAL"
                and move not in SINGLES_DAMAGE_INDEPENDENT_MOVES
                for move in selected
            )
            meaningful_special = sum(
                move_info.get(move, {}).get("category") == "DAMAGE_CATEGORY_SPECIAL"
                and move not in SINGLES_DAMAGE_INDEPENDENT_MOVES
                for move in selected
            )
            role = "Physical Utility" if meaningful_physical > meaningful_special else "Special Utility"
    nature, points = _single_build(role, selected, info, move_info)
    return {
        "species": species,
        "name": role,
        "moves": selected,
        "nature": nature,
        "ability": base["ability"],
        "item": base["item"],
        "required_item": "ITEM_NONE",
        "stat_points": points,
        "role": role,
        "source": "Emerald Champions singles adaptation of the authored competitive default",
    }


def build_singles_sets(defaults: list[dict], alternatives: list[dict]) -> tuple[list[dict], list[dict]]:
    """Build named singles choices from pinned Showdown roles plus legal fallbacks."""
    source = json.loads(SHOWDOWN_SINGLES_SOURCE.read_text())
    gen9_source = json.loads(SHOWDOWN_GEN9_SINGLES_SOURCE.read_text())
    assert source["source_commit"] == gen9_source["source_commit"] == "bb179fbf8449e3c31632bd56f671ffb4404fa6e7"
    metadata = species_build_metadata()
    move_info = move_metadata()
    learnables = json.loads((ROOT / "src/data/pokemon/all_learnables.json").read_text())
    existing_by_species: dict[str, list[dict]] = defaultdict(list)
    for entry in defaults + alternatives:
        existing_by_species[entry["species"]].append(entry)
    variants_by_species: dict[str, list[tuple[dict, dict]]] = defaultdict(list)
    for variant in source["variants"]:
        variants_by_species[variant["party_species"]].append((variant, source))
    gen9_variants_by_species: dict[str, list[tuple[dict, dict]]] = defaultdict(list)
    for variant in gen9_source["variants"]:
        gen9_variants_by_species[variant["party_species"]].append((variant, gen9_source))

    single_defaults: list[dict] = []
    single_alternatives: list[dict] = []
    for default in defaults:
        species = default["species"]
        info = metadata.get(species, {})
        legal = legal_moves_for_species(
            species, existing_by_species[species], metadata, learnables
        )
        choices: list[dict] = []
        source_variants = list(variants_by_species.get(species, []))
        if not any(variant["required_item"] == "ITEM_NONE" for variant, _ in source_variants):
            source_variants.extend(
                pair for pair in gen9_variants_by_species.get(species, [])
                if pair[0]["required_item"] == "ITEM_NONE"
            )
        for variant, variant_source in source_variants:
            templates = variant_source["templates"]
            form_info = metadata.get(variant["form_species"], info)
            for index in range(variant["template_offset"], variant["template_offset"] + variant["template_count"]):
                template = templates[index]
                if variant["required_item"] != "ITEM_NONE" and form_info.get("abilities"):
                    ability = form_info["abilities"][0]
                else:
                    ability = next(
                        (candidate for candidate in template["abilities"] if candidate in info.get("abilities", ())),
                        default["ability"],
                    )
                moves = _choose_showdown_single_moves(template, form_info, legal, move_info)
                if len(moves) < min(4, len(legal)):
                    continue
                nature, points = _single_build(template["role"], moves, form_info, move_info)
                required_item = variant["required_item"]
                if required_item == "ITEM_NONE":
                    name = compact_role_name(template["role"])
                    item = _single_item(template["role"], moves, ability, form_info, move_info)
                else:
                    suffix = ""
                    for marker, label in (("megax", "Mega X"), ("megay", "Mega Y"), ("megaz", "Mega Z"), ("mega", "Mega")):
                        if variant["showdown_id"].endswith(marker):
                            suffix = label
                            break
                    name = compact_role_name(f"{suffix} {template['role']}")
                    item = "ITEM_NONE"
                choices.append({
                    "species": species,
                    "name": name,
                    "moves": moves,
                    "nature": nature,
                    "ability": ability,
                    "item": (
                        "ITEM_NONE"
                        if required_item != "ITEM_NONE"
                        else coherent_item(ability, item)
                    ),
                    "required_item": required_item,
                    "stat_points": points,
                    "role": template["role"],
                    "source": (
                        f"{variant_source['source']} ranked role; "
                        f"commit {variant_source['source_commit']}"
                    ),
                })

        represented_mega_items = {
            entry["required_item"]
            for entry in choices
            if entry["required_item"] != "ITEM_NONE"
        }
        for doubles_role in existing_by_species[species]:
            required_item = doubles_role["required_item"]
            if required_item == "ITEM_NONE" or required_item in represented_mega_items:
                continue
            moves = [
                move for move in doubles_role["moves"]
                if move not in SINGLES_EXCLUDED_MOVES and move != "MOVE_PROTECT"
            ]
            ranked_physical = ranked_attacks(
                legal, "DAMAGE_CATEGORY_PHYSICAL", info.get("types", ()), move_info,
                doubles_role["ability"],
            )
            ranked_special = ranked_attacks(
                legal, "DAMAGE_CATEGORY_SPECIAL", info.get("types", ()), move_info,
                doubles_role["ability"],
            )
            for move in [*ranked_physical, *ranked_special, *SINGLES_RECOVERY_MOVES, *SINGLES_UTILITY_MOVES, *doubles_role["moves"]]:
                if len(moves) >= 4:
                    break
                if move in legal and move not in SINGLES_EXCLUDED_MOVES and move not in moves:
                    moves.append(move)
            if len(moves) < 4:
                continue
            choices.append({
                "species": species,
                "name": compact_role_name(doubles_role["name"]),
                "moves": moves[:4],
                "nature": doubles_role["nature"],
                "ability": doubles_role["ability"],
                "item": "ITEM_NONE",
                "required_item": required_item,
                "stat_points": doubles_role["stat_points"],
                "role": doubles_role["role"],
                "source": "Emerald Champions Singles adaptation of an authored Mega role",
            })
            represented_mega_items.add(required_item)

        if sum(entry["required_item"] == "ITEM_NONE" for entry in choices) < 2:
            for doubles_role in existing_by_species[species]:
                if doubles_role["required_item"] != "ITEM_NONE":
                    continue
                adapted = _fallback_single_set(
                    doubles_role,
                    existing_by_species[species],
                    metadata,
                    learnables,
                    move_info,
                )
                identity = (
                    frozenset(adapted["moves"]), adapted["ability"], adapted["item"],
                    adapted["nature"], tuple(adapted["stat_points"]),
                )
                if any(
                    identity == (
                        frozenset(entry["moves"]), entry["ability"], entry["item"],
                        entry["nature"], tuple(entry["stat_points"]),
                    )
                    for entry in choices
                    if entry["required_item"] == "ITEM_NONE"
                ):
                    continue
                choices.append(adapted)
                if sum(entry["required_item"] == "ITEM_NONE" for entry in choices) >= 2:
                    break

        if sum(entry["required_item"] == "ITEM_NONE" for entry in choices) < 2:
            first = next(entry for entry in choices if entry["required_item"] == "ITEM_NONE")
            physical = sum(
                move_info.get(move, {}).get("category") == "DAMAGE_CATEGORY_PHYSICAL"
                and move not in SINGLES_DAMAGE_INDEPENDENT_MOVES
                for move in first["moves"]
            )
            special = sum(
                move_info.get(move, {}).get("category") == "DAMAGE_CATEGORY_SPECIAL"
                and move not in SINGLES_DAMAGE_INDEPENDENT_MOVES
                for move in first["moves"]
            )
            if first["stat_points"][5] == 32:
                nature = "NATURE_CAREFUL" if physical > special else "NATURE_CALM"
                points = [32, 0, 17, 0, 17, 0]
                name = "Bulky Utility"
            elif physical > special:
                nature = "NATURE_JOLLY"
                points = [2, 32, 0, 0, 0, 32]
                name = "Fast Physical"
            else:
                nature = "NATURE_TIMID"
                points = [2, 0, 0, 32, 0, 32]
                name = "Fast Special"
            choices.append({
                **first,
                "name": name,
                "nature": nature,
                "stat_points": points,
                "source": "Emerald Champions alternate Singles benchmark spread",
            })

        non_mega = [entry for entry in choices if entry["required_item"] == "ITEM_NONE"]
        if not non_mega:
            fallback = _fallback_single_set(
                default, existing_by_species[species], metadata, learnables, move_info
            )
            choices.insert(0, fallback)
        else:
            first = non_mega[0]
            choices.remove(first)
            choices.insert(0, first)

        # Eliminate exact duplicate orientations and make every visible label
        # unambiguous within this species' Singles menu.
        unique: list[dict] = []
        seen_orientations: set[tuple] = set()
        used_names: set[str] = set()
        for choice in choices:
            identity = (
                tuple(choice["moves"]), choice["ability"], choice["item"],
                choice["required_item"], choice["nature"], tuple(choice["stat_points"]),
            )
            if identity in seen_orientations:
                continue
            seen_orientations.add(identity)
            if choice["name"] in used_names:
                choice = dict(choice)
                choice["name"] = compact_role_name(
                    f"{choice['name']} {len(used_names) + 1}"
                )
            used_names.add(choice["name"])
            unique.append(choice)
        single_defaults.append(unique[0])
        single_alternatives.extend(unique[1:])

    final_by_species: dict[str, list[dict]] = defaultdict(list)
    for entry in single_defaults + single_alternatives:
        final_by_species[entry["species"]].append(entry)
    missing_second = [
        species for species, entries in final_by_species.items()
        if sum(entry["required_item"] == "ITEM_NONE" for entry in entries) < 2
    ]
    assert not missing_second, missing_second
    return single_defaults, single_alternatives


def remove_superficial_non_mega_alternatives(
    defaults: list[dict], alternatives: list[dict]
) -> list[dict]:
    """Drop preserved alternatives that only reorder moves or swap an item."""
    by_species: dict[str, list[dict]] = defaultdict(list)
    for entry in defaults:
        by_species[entry["species"]].append(entry)
    retained: list[dict] = []
    removed: list[tuple[str, str]] = []
    for entry in alternatives:
        if (
            entry["required_item"] == "ITEM_NONE"
            and not is_genuinely_distinct(entry, by_species[entry["species"]])
        ):
            removed.append((entry["species"], entry["name"]))
            continue
        retained.append(entry)
        by_species[entry["species"]].append(entry)
    assert removed == [
        ("SPECIES_CHIEN_PAO", "Wallbreaker"),
        ("SPECIES_KILOWATTREL", "Doubles Fast Attacker"),
    ], removed
    return retained


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
    ability = ability_map[showdown_id(source["ability"])]
    return {
        "species": species,
        "name": shorten_name(role),
        "moves": moves,
        "nature": nature,
        # A Mega role records the transformed Ability, which need not be legal
        # on the base species.  Ordinary roles still use the compatibility
        # aliases needed by the current base-species tables.
        "ability": ability if required_item != "ITEM_NONE" else normalize_ability(species, ability),
        "item": "ITEM_NONE" if required_item != "ITEM_NONE" else item,
        "required_item": required_item,
        "stat_points": infer_stat_points(nature, moves, role),
        "role": role,
        "source": f"Pokemon Champions doubles handbook: {source['evidence']}",
    }


def handbook_supplements(present_species: set[str]) -> tuple[list[dict], list[dict]]:
    handbook = handbook_json()
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


def handbook_mega_roles() -> list[dict]:
    """Return every ladder-backed Mega role that the current engine supports."""
    handbook = handbook_json()
    move_map = constant_id_map(ROOT / "include/constants/moves.h", "MOVE_")
    item_map = constant_id_map(ROOT / "include/constants/items.h", "ITEM_")
    ability_map = constant_id_map(ROOT / "include/constants/abilities.h", "ABILITY_")
    nature_map = constant_id_map(ROOT / "include/constants/pokemon.h", "NATURE_")
    national_species = national_species_order()
    form_changes = (ROOT / "src/data/pokemon/form_change_tables.h").read_text()
    supported_items = set(re.findall(
        r"FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM,\s*SPECIES_[A-Z0-9_]+,\s*(ITEM_[A-Z0-9_]+)",
        form_changes,
    ))
    result: list[dict] = []

    for source in handbook["sets"]:
        if "Mega" not in source["role"]:
            continue
        role_suffix = source["role"].split("—", 1)[-1].strip()
        species = HANDBOOK_MEGA_BASE_ROLES.get(
            role_suffix,
            national_species[source["national_dex"]],
        )
        entry = normalize_handbook_set(
            source,
            species,
            move_map,
            item_map,
            ability_map,
            nature_map,
        )
        assert entry["required_item"] in supported_items, (
            species,
            entry["required_item"],
            source["role"],
        )
        result.append(entry)

    assert len(result) == 75, len(result)
    assert len({(entry["species"], entry["required_item"]) for entry in result}) == len(result)
    return result


def merge_handbook_mega_roles(
    defaults: list[dict], alternatives: list[dict]
) -> tuple[list[dict], list[dict]]:
    """Replace adapted legacy Mega rows and append every missing Mega role."""
    mega_roles = handbook_mega_roles()
    default_index = {
        (entry["species"], entry["required_item"]): index
        for index, entry in enumerate(defaults)
        if entry["required_item"] != "ITEM_NONE"
    }
    alternative_index = {
        (entry["species"], entry["required_item"]): index
        for index, entry in enumerate(alternatives)
        if entry["required_item"] != "ITEM_NONE"
    }

    for entry in mega_roles:
        key = (entry["species"], entry["required_item"])
        if key in default_index:
            defaults[default_index[key]] = entry
        elif key in alternative_index:
            alternatives[alternative_index[key]] = entry
        else:
            alternative_index[key] = len(alternatives)
            alternatives.append(entry)

    for entry in CUSTOM_MEGA_ROLES:
        key = (entry["species"], entry["required_item"])
        assert key not in default_index and key not in alternative_index, key
        alternative_index[key] = len(alternatives)
        alternatives.append(entry)
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
        narrow_move_counts = {
            "SPECIES_DITTO": 1,
            "SPECIES_UNOWN": 2,
        }
        expected_moves = narrow_move_counts.get(entry["species"], 4)
        assert len(entry["moves"]) == expected_moves, (
            entry["species"], entry["name"], entry["moves"], expected_moves
        )
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


def write_c(
    defaults: list[dict],
    alternatives: list[dict],
    singles_defaults: list[dict],
    singles_alternatives: list[dict],
) -> None:
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

    singles_by_species: dict[str, list[dict]] = {}
    for entry in singles_alternatives:
        singles_by_species.setdefault(entry["species"], []).append(entry)
    for entry in singles_defaults:
        lines.append(
            f'static const u8 sEmeraldChampionsSinglesSetName_{entry["species"]}[] = _("{entry["name"]}");'
        )
    lines.extend([
        "",
        "const struct EmeraldChampionsBattleSet gEmeraldChampionsSinglesDefaultBattleSets[NUM_SPECIES] =",
        "{",
    ])
    for entry in singles_defaults:
        lines.append(f'    [{entry["species"]}] =')
        lines.append("    {")
        lines.extend(c_preset(entry))
        lines.append("    },")
    lines.extend([
        "};",
        "",
        "const u8 *const gEmeraldChampionsSinglesDefaultBattleSetNames[NUM_SPECIES] =",
        "{",
    ])
    for entry in singles_defaults:
        lines.append(
            f'    [{entry["species"]}] = sEmeraldChampionsSinglesSetName_{entry["species"]},'
        )
    lines.extend([
        "};",
        "",
        "const struct EmeraldChampionsBattleSetRange gEmeraldChampionsSinglesBattleSetRanges[NUM_SPECIES] =",
        "{",
    ])
    offset = 0
    for entry in singles_defaults:
        choices = singles_by_species.get(entry["species"], [])
        if choices:
            lines.append(f'    [{entry["species"]}] = {{.offset = {offset}, .count = {len(choices)}}},')
            offset += len(choices)
    lines.extend([
        "};",
        "",
        "const struct EmeraldChampionsBattleSetChoice gEmeraldChampionsSinglesBattleSetAlternatives[] =",
        "{",
    ])
    for entry in singles_alternatives:
        lines.append("    {")
        lines.append(f'        .name = _("{entry["name"]}"),')
        lines.append("        .preset =")
        lines.append("        {")
        lines.extend(c_preset(entry, "            "))
        lines.append("        },")
        lines.append("    },")
    lines.extend(["};", ""])
    C_OUTPUT.write_text("\n".join(lines))


def write_move_access_review_c() -> None:
    review = json.loads(MOVE_ACCESS_REVIEW.read_text())
    retained = [row for row in review["assignments"] if row["action"] != "replace"]
    assert len(review["assignments"]) == 72
    assert len(retained) == 65
    lines = [
        "// Generated by scripts/generate_emerald_champions_battle_sets.py. Do not edit by hand.",
    ]
    lines.extend(
        f'    {{{row["species"]}, {row["move"]}}},'
        for row in retained
    )
    lines.append("")
    MOVE_ACCESS_C_OUTPUT.write_text("\n".join(lines))


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
    # Scovillain enters through the handbook supplement as a Mega-only row.
    # Wild defaults must always be immediately usable, while the later Mega
    # merge keeps the stone-gated role as a separate tutor choice.
    defaults = [
        {
            **authored_modern_set(
                "SPECIES_SCOVILLAIN",
                "Rage Powder Support",
                ["MOVE_RAGE_POWDER", "MOVE_HELPING_HAND", "MOVE_OVERHEAT", "MOVE_PROTECT"],
                "NATURE_CALM",
                "ABILITY_MOODY",
                "ITEM_SITRUS_BERRY",
                [32, 0, 16, 0, 18, 0],
                "bulky redirection and Helping Hand support",
            ),
            "source": entry["source"],
        }
        if entry["species"] == "SPECIES_SCOVILLAIN" and entry["required_item"] != "ITEM_NONE"
        else entry
        for entry in defaults
    ]
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
    raw_alternatives = [
        SUPPLEMENTAL_ALTERNATIVE_OVERRIDES.get(
            (entry["species"], entry["name"]), entry
        )
        for entry in raw_alternatives
    ]
    defaults = [apply_audited_set_override(entry) for entry in defaults]
    raw_alternatives = [apply_audited_set_override(entry) for entry in raw_alternatives]
    defaults, raw_alternatives = merge_handbook_mega_roles(defaults, raw_alternatives)
    raw_alternatives = remove_superficial_non_mega_alternatives(defaults, raw_alternatives)
    raw_alternatives = ensure_minimum_non_mega_orientations(defaults, raw_alternatives)
    # Some alternatives are synthesized by the minimum-orientation pass, so
    # they need the same executable-coherence review after synthesis.
    raw_alternatives = [apply_audited_set_override(entry) for entry in raw_alternatives]
    defaults, raw_alternatives = complete_battle_sets(defaults, raw_alternatives)
    defaults = repair_retired_ability_labels(defaults)
    raw_alternatives = repair_retired_ability_labels(raw_alternatives)
    defaults = name_doubles_defaults(defaults)
    alternatives_by_species: dict[str, list[dict]] = {}
    for entry in raw_alternatives:
        alternatives_by_species.setdefault(entry["species"], []).append(entry)
    alternatives = [
        choice
        for default in defaults
        for choice in alternatives_by_species.get(default["species"], [])
    ]
    singles_defaults, singles_alternatives = build_singles_sets(defaults, alternatives)
    entries = defaults + alternatives + singles_defaults + singles_alternatives
    validate(entries)

    output = {
        "schema_version": 3,
        "source_commit": SOURCE_COMMIT,
        "policy": {
            "format": "named Doubles and Singles buckets; Doubles remains the wild/evolution default",
            "stat_points": "66 total, 32 maximum per stat",
            "ability": "resolved by Ability identity against current species data",
            "protected_items": "never supplied by a preset",
            "minimum_non_mega_orientations": "two genuinely distinct choices per direct species/form",
            "wild_sampling": "uniform across every non-Mega orientation",
        },
        "default_count": len(defaults),
        "alternative_count": len(alternatives),
        "singles_default_count": len(singles_defaults),
        "singles_alternative_count": len(singles_alternatives),
        "set_count": len(entries),
        "defaults": defaults,
        "alternatives": alternatives,
        "singles_defaults": singles_defaults,
        "singles_alternatives": singles_alternatives,
    }
    JSON_OUTPUT.write_text(json.dumps(output, indent=2, ensure_ascii=False) + "\n")
    write_c(defaults, alternatives, singles_defaults, singles_alternatives)
    write_move_access_review_c()
    print(f"defaults={len(defaults)}")
    print(f"alternatives={len(alternatives)}")
    print(f"singles_defaults={len(singles_defaults)}")
    print(f"singles_alternatives={len(singles_alternatives)}")
    print(f"sets={len(entries)}")


if __name__ == "__main__":
    main()
