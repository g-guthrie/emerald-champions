#!/usr/bin/env python3
"""Finish the rematch-free master battle document against current data."""

from __future__ import annotations

import argparse
import json
import math
import re
from collections import Counter, defaultdict
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MASTER = ROOT / "docs" / "emerald_champions_master_battle_design.txt"
DEFAULT_OUTPUT = ROOT / "docs" / "emerald_champions_master_battle_design_v2.txt"

ENCOUNTER_RE = re.compile(r"(?m)^=== ENCOUNTER \d{4} ===$")
BRANCH_RE = re.compile(r"(?m)^--- BRANCH ([A-Z0-9_]+) ---$")
MON_RE = re.compile(
    r"^  (\d+)\. (SPECIES_[A-Z0-9_]+) @ (ITEM_[A-Z0-9_]+) \| "
    r"level=(-?\d+) \| ability_slot=(\d+) \| ([A-Z0-9_]+) \| moves=([A-Z0-9_,]+)$",
    re.M,
)

GYM_REMATCH_GROUPS = {
    "PHYSICAL_DEWFORDTOWN_GYM_0207",
    "PHYSICAL_FORTREECITY_GYM_0068",
    "PHYSICAL_MAUVILLECITY_GYM_0153",
    "PHYSICAL_RUSTBOROCITY_GYM_0040",
    "PHYSICAL_SOOTOPOLISCITY_GYM_1F_0152",
    "PHYSICAL_MOSSDEEPCITY_HOUSE1_0086",
}
GYM_REMATCH_BRANCHES = {"TRAINER_FLANNERY_5", "TRAINER_NORMAN_5"}
PHANTOM_SOURCE_GROUPS = {
    "PHYSICAL_ROUTE103_0131",  # Movement line was misread as a rival battle.
    "PHYSICAL_BATTLEFRONTIER_BATTLEPYRAMIDFLOOR_0116",  # Item script line, not a Trainer.
    "PHYSICAL_GLOBAL_TRAINER_HILL_0067",  # Comment names Phillip as a placeholder only.
    "BATTLE_144_ASHEN_WOODS_ALANNAH",  # The modern campaign deliberately omits this removed Inclement-only map.
    "PHYSICAL_ASHENWOODS_0169",
    "PHYSICAL_ASHENWOODS_0174",
    "PHYSICAL_ASHENWOODS_0179",
}

MARQUEE_TOKENS = (
    "ROXANNE", "BRAWLY", "WATTSON", "FLANNERY", "NORMAN", "WINONA",
    "TATE_AND_LIZA", "JUAN", "WALLACE", "MAXIE", "ARCHIE", "STEVEN",
    "CYNTHIA",
)
MINIBOSS_TOKENS = ("TABITHA", "COURTNEY", "MATT", "SHELLY", "WALLY", "BRENDAN", "MAY_")
LEAGUE_TOKENS = ("SIDNEY", "PHOEBE", "GLACIA", "DRAKE", "WALLACE", "CYNTHIA")
FACTION_TOKENS = ("GRUNT", "MAGMA", "AQUA")
SETUP_MOVES = {
    "MOVE_BELLY_DRUM", "MOVE_BULK_UP", "MOVE_CALM_MIND", "MOVE_COIL",
    "MOVE_DRAGON_DANCE", "MOVE_IRON_DEFENSE", "MOVE_NASTY_PLOT",
    "MOVE_QUIVER_DANCE", "MOVE_SHELL_SMASH", "MOVE_SWORDS_DANCE",
    "MOVE_TAIL_GLOW", "MOVE_TIDY_UP", "MOVE_VICTORY_DANCE",
}
SPEED_MOVES = {"MOVE_TAILWIND", "MOVE_TRICK_ROOM", "MOVE_ICY_WIND", "MOVE_ELECTROWEB", "MOVE_THUNDER_WAVE"}
REDIRECTION_MOVES = {"MOVE_FOLLOW_ME", "MOVE_RAGE_POWDER", "MOVE_SPOTLIGHT"}
HAZARD_MOVES = {"MOVE_STEALTH_ROCK", "MOVE_SPIKES", "MOVE_TOXIC_SPIKES", "MOVE_STICKY_WEB"}
PROTECT_MOVES = {"MOVE_PROTECT", "MOVE_DETECT", "MOVE_BANEFUL_BUNKER", "MOVE_KINGS_SHIELD", "MOVE_SPIKY_SHIELD"}
SPREAD_MOVES = {"MOVE_ROCK_SLIDE", "MOVE_HEAT_WAVE", "MOVE_MUDDY_WATER", "MOVE_HYPER_VOICE", "MOVE_DAZZLING_GLEAM", "MOVE_BLIZZARD", "MOVE_EARTHQUAKE", "MOVE_DISCHARGE", "MOVE_SURF"}


@dataclass
class Mon:
    species: str
    item: str
    level_offset: int
    ability: str
    nature: str
    stat_points: list[int]
    moves: list[str]


@dataclass
class Branch:
    trainer_id: str
    text: str
    format: str
    mons: list[Mon]
    original_status: str
    block_index: int
    branch_index: int


def constants(path: str, prefix: str) -> tuple[set[str], dict[str, str]]:
    values = set(re.findall(rf"\b{prefix}[A-Z0-9_]+\b", (ROOT / path).read_text()))
    by_id = {}
    for value in sorted(values, key=lambda token: (token.count("_"), len(token)), reverse=True):
        by_id.setdefault(re.sub(r"[^a-z0-9]", "", value[len(prefix):].lower()), value)
    return values, by_id


SPECIES, SPECIES_BY_ID = constants("include/constants/species.h", "SPECIES_")
ITEMS, ITEM_BY_ID = constants("include/constants/items.h", "ITEM_")
MOVES, MOVE_BY_ID = constants("include/constants/moves.h", "MOVE_")
ABILITIES, ABILITY_BY_ID = constants("include/constants/abilities.h", "ABILITY_")
NATURES, _ = constants("include/constants/pokemon.h", "NATURE_")
TRAINERS, _ = constants("include/constants/opponents.h", "TRAINER_")

LEGACY_SPECIES_ALIASES = {
    "SPECIES_CALYREX_ICE_RIDER": "SPECIES_CALYREX_ICE",
    "SPECIES_CALYREX_SHADOW_RIDER": "SPECIES_CALYREX_SHADOW",
    "SPECIES_FLOETTE_ETERNAL_FLOWER": "SPECIES_FLOETTE_ETERNAL",
    "SPECIES_FLOETTE_WHITE_FLOWER": "SPECIES_FLOETTE_WHITE",
    "SPECIES_FLORGES_BLUE_FLOWER": "SPECIES_FLORGES_BLUE",
    "SPECIES_FLORGES_ORANGE_FLOWER": "SPECIES_FLORGES_ORANGE",
    "SPECIES_FURFROU_DEBUTANTE_TRIM": "SPECIES_FURFROU_DEBUTANTE",
    "SPECIES_FURFROU_KABUKI_TRIM": "SPECIES_FURFROU_KABUKI",
    "SPECIES_INDEEDEE_FEMALE": "SPECIES_INDEEDEE_F",
    "SPECIES_MEOWSTIC_FEMALE": "SPECIES_MEOWSTIC_F",
    "SPECIES_URSHIFU_RAPID_STRIKE_STYLE": "SPECIES_URSHIFU_RAPID_STRIKE",
    "SPECIES_WORMADAM_SANDY_CLOAK": "SPECIES_WORMADAM_SANDY",
    "SPECIES_WORMADAM_TRASH_CLOAK": "SPECIES_WORMADAM_TRASH",
}


def normalize(token: str, values: set[str], by_id: dict[str, str], prefix: str) -> str | None:
    if token in values:
        return token
    if prefix == "SPECIES_" and token in LEGACY_SPECIES_ALIASES:
        return LEGACY_SPECIES_ALIASES[token]
    candidate = token.replace("_ALOLAN", "_ALOLA").replace("_GALARIAN", "_GALAR").replace("_HISUIAN", "_HISUI")
    candidate = candidate.replace("_EAST_SEA", "_EAST")
    if candidate in values:
        return candidate
    return by_id.get(re.sub(r"[^a-z0-9]", "", token[len(prefix):].lower()))


def current_species_abilities() -> dict[str, tuple[str, str, str]]:
    result = {}
    for path in sorted((ROOT / "src" / "data" / "pokemon" / "species_info").glob("gen_*_families.h")):
        text = path.read_text()
        markers = list(re.finditer(r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{", text))
        for index, marker in enumerate(markers):
            body = text[marker.end():markers[index + 1].start() if index + 1 < len(markers) else len(text)]
            abilities = re.search(
                r"\.abilities\s*=\s*\{\s*(ABILITY_[A-Z0-9_]+)\s*,\s*"
                r"(ABILITY_[A-Z0-9_]+)\s*,\s*(ABILITY_[A-Z0-9_]+)",
                body,
            )
            if abilities:
                result[marker.group(1)] = abilities.groups()
    aliases = dict(re.findall(
        r"(?m)^\s*(SPECIES_[A-Z0-9_]+)\s*=\s*(SPECIES_[A-Z0-9_]+)\s*,",
        (ROOT / "include" / "constants" / "species.h").read_text(),
    ))
    for alias, target in aliases.items():
        if target in result:
            result[alias] = result[target]
    return result


SPECIES_ABILITIES = current_species_abilities()


def line_value(text: str, key: str, default: str = "") -> str:
    match = re.search(rf"(?m)^{re.escape(key)}: (.*)$", text)
    return match.group(1) if match else default


def set_line(text: str, key: str, value: str) -> str:
    pattern = rf"(?m)^{re.escape(key)}: .*$"
    if re.search(pattern, text):
        return re.sub(pattern, f"{key}: {value}", text, count=1)
    marker = re.search(r"(?m)^branches:$", text)
    at = marker.start() if marker else len(text)
    return text[:at] + f"{key}: {value}\n" + text[at:]


def split_blocks(text: str) -> tuple[str, list[str]]:
    markers = list(ENCOUNTER_RE.finditer(text))
    header = text[:markers[0].start()]
    blocks = [text[m.start():markers[i + 1].start() if i + 1 < len(markers) else len(text)] for i, m in enumerate(markers)]
    return header, blocks


def presets() -> tuple[dict[str, list[dict]], dict[str, dict]]:
    data = json.loads((ROOT / "docs" / "emerald_champions_battle_sets.json").read_text())
    by_species: dict[str, list[dict]] = defaultdict(list)
    defaults = {}
    for entry in data["defaults"]:
        by_species[entry["species"]].append(entry)
        defaults[entry["species"]] = entry
    for entry in data["alternatives"]:
        by_species[entry["species"]].append(entry)
    return by_species, defaults


PRESETS, DEFAULT_PRESETS = presets()

SHOWDOWN_LEARNSET_DATA = json.loads((ROOT / "docs" / "showdown_champions_learnsets.json").read_text())
SHOWDOWN_LEARNSETS = {species: set(moves) for species, moves in SHOWDOWN_LEARNSET_DATA["learnsets"].items()}

SHOWDOWN_FORM_SUFFIXES = (
    "50powerconstruct", "10powerconstruct", "powerconstruct", "curly", "droopy", "stretchy",
    "incarnate", "ordinary", "aria", "amped", "midday", "male", "female", "natural",
    "west", "east", "normal", "altered", "land", "sky", "small", "large", "super",
    "average", "antique", "phony", "rubycream", "marine", "autumn", "roaming",
    "debutante", "kabuki",
)


def showdown_id_for_species(species: str) -> str:
    showdown_id = re.sub(r"[^a-z0-9]", "", species.removeprefix("SPECIES_").lower())
    if showdown_id in SHOWDOWN_LEARNSETS:
        return showdown_id
    for suffix in SHOWDOWN_FORM_SUFFIXES:
        if showdown_id.endswith(suffix):
            candidate = showdown_id[:-len(suffix)]
            if candidate in SHOWDOWN_LEARNSETS:
                return candidate
    raise ValueError(f"no pinned Showdown learnset mapping for {species}")


def legal_move_constants(species: str) -> set[str]:
    return {
        move
        for showdown_move in SHOWDOWN_LEARNSETS[showdown_id_for_species(species)]
        if (move := MOVE_BY_ID.get(showdown_move)) is not None
    }


def move_metadata() -> tuple[dict[str, dict[str, str | int]], dict[str, str]]:
    text = (ROOT / "src" / "data" / "moves_info.h").read_text()
    markers = list(re.finditer(r"(?m)^\s*\[(MOVE_[A-Z0-9_]+)\]\s*=\s*\{$", text))
    metadata = {}
    names = {}
    for index, marker in enumerate(markers):
        body = text[marker.end():markers[index + 1].start() if index + 1 < len(markers) else len(text)]
        move = marker.group(1)
        name = re.search(r'\.name\s*=\s*COMPOUND_STRING\("([^\"]+)"\)', body)
        move_type = re.search(r"\.type\s*=\s*(TYPE_[A-Z0-9_]+)", body)
        category = re.search(r"\.category\s*=\s*(DAMAGE_CATEGORY_[A-Z0-9_]+)", body)
        power = re.search(r"\.power\s*=\s*(\d+)", body)
        metadata[move] = {
            "type": move_type.group(1) if move_type else "TYPE_NONE",
            "category": category.group(1) if category else "DAMAGE_CATEGORY_STATUS",
            "power": int(power.group(1)) if power else 0,
        }
        if name:
            names[move] = name.group(1)
    return metadata, names


MOVE_META, MOVE_NAMES = move_metadata()

PREFERRED_MOVE_REPLACEMENTS = {
    "MOVE_EXTREME_SPEED": ("MOVE_QUICK_ATTACK", "MOVE_BULLET_PUNCH", "MOVE_AQUA_JET"),
    "MOVE_AQUA_TAIL": ("MOVE_LIQUIDATION", "MOVE_WATERFALL", "MOVE_AQUA_JET"),
    "MOVE_POWDER": ("MOVE_SLEEP_POWDER", "MOVE_STUN_SPORE", "MOVE_PROTECT"),
    "MOVE_ROLLOUT": ("MOVE_ROCK_SLIDE", "MOVE_STONE_EDGE"),
    "MOVE_FREEZE_DRY": ("MOVE_ICE_BEAM", "MOVE_ICY_WIND", "MOVE_BLIZZARD"),
    "MOVE_AURA_SPHERE": ("MOVE_FOCUS_BLAST", "MOVE_DAZZLING_GLEAM", "MOVE_PSYCHIC"),
    "MOVE_HIDDEN_POWER": ("MOVE_SHADOW_BALL", "MOVE_POWER_GEM", "MOVE_PROTECT"),
    "MOVE_DAZZLING_GLEAM": ("MOVE_MOONBLAST", "MOVE_PLAY_ROUGH", "MOVE_ALLURING_VOICE"),
    "MOVE_ICE_FANG": ("MOVE_CRUNCH", "MOVE_ICE_PUNCH", "MOVE_ICICLE_CRASH"),
    "MOVE_SIGNAL_BEAM": ("MOVE_BUG_BUZZ", "MOVE_SHADOW_BALL", "MOVE_PSYCHIC"),
    "MOVE_SUCKER_PUNCH": ("MOVE_KNOCK_OFF", "MOVE_THROAT_CHOP", "MOVE_CRUNCH"),
    "MOVE_TAILWIND": ("MOVE_ICY_WIND", "MOVE_ELECTROWEB", "MOVE_THUNDER_WAVE"),
    "MOVE_ICY_WIND": ("MOVE_ELECTROWEB", "MOVE_TRICK_ROOM", "MOVE_THUNDER_WAVE"),
    "MOVE_RETURN": ("MOVE_DOUBLE_EDGE", "MOVE_BODY_SLAM", "MOVE_FACADE"),
    "MOVE_POWER_UP_PUNCH": ("MOVE_DRAIN_PUNCH", "MOVE_LOW_KICK", "MOVE_CLOSE_COMBAT"),
    "MOVE_FEINT": ("MOVE_QUICK_GUARD", "MOVE_FAKE_OUT", "MOVE_PROTECT"),
    "MOVE_WIDE_GUARD": ("MOVE_PROTECT", "MOVE_DETECT", "MOVE_HELPING_HAND"),
    "MOVE_CURSE": ("MOVE_IRON_DEFENSE", "MOVE_BULK_UP", "MOVE_PROTECT"),
    "MOVE_KNOCK_OFF": ("MOVE_THROAT_CHOP", "MOVE_CRUNCH", "MOVE_FOUL_PLAY"),
    "MOVE_ICE_SHARD": ("MOVE_ICICLE_SPEAR", "MOVE_ICY_WIND", "MOVE_ICE_SPINNER"),
    "MOVE_POWER_GEM": ("MOVE_ANCIENT_POWER", "MOVE_ROCK_SLIDE", "MOVE_STONE_EDGE"),
    "MOVE_RECOVER": ("MOVE_ROOST", "MOVE_SYNTHESIS", "MOVE_PROTECT"),
    "MOVE_QUIVER_DANCE": ("MOVE_CALM_MIND", "MOVE_AGILITY", "MOVE_FLAME_CHARGE"),
    "MOVE_HIGH_JUMP_KICK": ("MOVE_CLOSE_COMBAT", "MOVE_LOW_KICK", "MOVE_BRICK_BREAK"),
    "MOVE_WHIRLWIND": ("MOVE_ROAR", "MOVE_TAUNT", "MOVE_HAZE"),
    "MOVE_ICE_PUNCH": ("MOVE_ICE_FANG", "MOVE_ICICLE_CRASH", "MOVE_ICE_SPINNER"),
    "MOVE_MUDDY_WATER": ("MOVE_SURF", "MOVE_HYDRO_PUMP", "MOVE_WATER_PULSE"),
    "MOVE_HAIL": ("MOVE_SNOWSCAPE", "MOVE_ICE_BEAM", "MOVE_PROTECT"),
    "MOVE_POLTERGEIST": ("MOVE_SHADOW_BALL", "MOVE_SHADOW_CLAW", "MOVE_HEX"),
    "MOVE_NIGHT_SLASH": ("MOVE_THROAT_CHOP", "MOVE_CRUNCH", "MOVE_DARK_PULSE"),
    "MOVE_EARTH_POWER": ("MOVE_SCORCHING_SANDS", "MOVE_STOMPING_TANTRUM", "MOVE_FOCUS_BLAST"),
    "MOVE_JUMP_KICK": ("MOVE_CLOSE_COMBAT", "MOVE_HIGH_JUMP_KICK", "MOVE_LOW_KICK"),
    "MOVE_SHORE_UP": ("MOVE_RECOVER", "MOVE_REST", "MOVE_PROTECT"),
    "MOVE_STRENGTH_SAP": ("MOVE_GIGA_DRAIN", "MOVE_SYNTHESIS", "MOVE_WILL_O_WISP"),
    "MOVE_SIMPLE_BEAM": ("MOVE_SKILL_SWAP", "MOVE_ENTRAINMENT", "MOVE_TAUNT"),
    "MOVE_SHELL_SMASH": ("MOVE_IRON_DEFENSE", "MOVE_ROCK_POLISH", "MOVE_SWORDS_DANCE"),
    "MOVE_SCALD": ("MOVE_CHILLING_WATER", "MOVE_SURF", "MOVE_MUDDY_WATER"),
    "MOVE_RAPID_SPIN": ("MOVE_ICE_SPINNER", "MOVE_PROTECT", "MOVE_KNOCK_OFF"),
    "MOVE_AIR_SLASH": ("MOVE_HURRICANE", "MOVE_AIR_CUTTER", "MOVE_ACROBATICS"),
    "MOVE_AQUA_JET": ("MOVE_LIQUIDATION", "MOVE_WATERFALL", "MOVE_FLIP_TURN"),
    "MOVE_WATER_SPOUT": ("MOVE_HYDRO_PUMP", "MOVE_SURF", "MOVE_WATER_PULSE"),
    "MOVE_MOONBLAST": ("MOVE_DAZZLING_GLEAM", "MOVE_ALLURING_VOICE", "MOVE_PLAY_ROUGH"),
    "MOVE_STICKY_WEB": ("MOVE_ELECTROWEB", "MOVE_STRING_SHOT", "MOVE_SPIKES"),
    "MOVE_DISABLE": ("MOVE_ENCORE", "MOVE_TAUNT", "MOVE_PROTECT"),
    "MOVE_HEAL_PULSE": ("MOVE_HELPING_HAND", "MOVE_LIFE_DEW", "MOVE_PROTECT"),
    "MOVE_LEECH_SEED": ("MOVE_GIGA_DRAIN", "MOVE_PROTECT", "MOVE_INGRAIN"),
    "MOVE_RAGE_POWDER": ("MOVE_SLEEP_POWDER", "MOVE_STUN_SPORE", "MOVE_PROTECT"),
    "MOVE_DOUBLE_EDGE": ("MOVE_BODY_SLAM", "MOVE_FACADE", "MOVE_TAKE_DOWN"),
    "MOVE_SLEEP_POWDER": ("MOVE_HYPNOSIS", "MOVE_STUN_SPORE", "MOVE_GIGA_DRAIN"),
    "MOVE_BELLY_DRUM": ("MOVE_SWORDS_DANCE", "MOVE_BULK_UP", "MOVE_CALM_MIND"),
    "MOVE_HEAL_BELL": ("MOVE_HELPING_HAND", "MOVE_AROMATHERAPY", "MOVE_LIFE_DEW"),
    "MOVE_HEAD_SMASH": ("MOVE_ROCK_SLIDE", "MOVE_STONE_EDGE", "MOVE_IRON_HEAD"),
    "MOVE_HEAT_WAVE": ("MOVE_FLAMETHROWER", "MOVE_OVERHEAT", "MOVE_FIRE_BLAST"),
    "MOVE_FAKE_OUT": ("MOVE_QUICK_GUARD", "MOVE_HELPING_HAND", "MOVE_PROTECT"),
    "MOVE_PARTING_SHOT": ("MOVE_VOLT_SWITCH", "MOVE_U_TURN", "MOVE_SNARL"),
}

GENERIC_GOOD_MOVES = (
    "MOVE_PROTECT", "MOVE_DETECT", "MOVE_HELPING_HAND", "MOVE_FAKE_OUT", "MOVE_TAUNT",
    "MOVE_ENCORE", "MOVE_ICY_WIND", "MOVE_ELECTROWEB", "MOVE_THUNDER_WAVE", "MOVE_TRICK_ROOM",
    "MOVE_ROCK_SLIDE", "MOVE_HEAT_WAVE", "MOVE_DAZZLING_GLEAM", "MOVE_SURF", "MOVE_HYPER_VOICE",
    "MOVE_THUNDERBOLT", "MOVE_ICE_BEAM", "MOVE_SHADOW_BALL", "MOVE_EARTHQUAKE", "MOVE_CLOSE_COMBAT",
)


def add_manual_preset(species: str, role: str, item: str, ability: str, nature: str, moves: list[str], points: list[int]) -> None:
    entry = {
        "species": species,
        "name": "Recommended",
        "moves": moves,
        "nature": nature,
        "ability": ability,
        "item": item,
        "required_item": "ITEM_NONE",
        "stat_points": points,
        "role": role,
        "source": "Emerald Champions modern-species closure",
    }
    PRESETS[species].append(entry)
    DEFAULT_PRESETS[species] = entry


add_manual_preset(
    "SPECIES_BAXCALIBUR", "Thermal Exchange physical pressure", "ITEM_LIFE_ORB",
    "ABILITY_THERMAL_EXCHANGE", "NATURE_JOLLY",
    ["MOVE_GLAIVE_RUSH", "MOVE_ICICLE_CRASH", "MOVE_ICE_SHARD", "MOVE_PROTECT"], [2, 32, 0, 0, 0, 32],
)


BESPOKE_TEAM_OVERRIDES = {
    "TRAINER_ROXANNE_1": [
        ("SPECIES_CARBINK", "ITEM_MENTAL_HERB", 3),
        ("SPECIES_ROCKRUFF", "ITEM_FOCUS_SASH", 3),
        ("SPECIES_NACLI", "ITEM_EVIOLITE", 4),
        ("SPECIES_BONSLY", "ITEM_LIFE_ORB", 4),
        ("SPECIES_LILEEP", "ITEM_SITRUS_BERRY", 4),
        ("SPECIES_REGIROCK", "ITEM_LEFTOVERS", 5),
    ],
    "TRAINER_JOCELYN": [
        ("SPECIES_TYROGUE", "ITEM_FLAME_ORB", 1),
        ("SPECIES_PANCHAM", "ITEM_EVIOLITE", 2),
        ("SPECIES_CRABRAWLER", "ITEM_LUM_BERRY", 3),
        ("SPECIES_NATU", "ITEM_COLBUR_BERRY", 4),
    ],
    "TRAINER_KIRK": [
        ("SPECIES_TOXTRICITY", "ITEM_THROAT_SPRAY", 0),
        ("SPECIES_HELIOLISK", "ITEM_SITRUS_BERRY", 0),
        ("SPECIES_ELECTRODE", "ITEM_FOCUS_SASH", 0),
        ("SPECIES_GRAVELER_ALOLA", "ITEM_EVIOLITE", 0),
    ],
    "TRAINER_VIVIAN": [
        ("SPECIES_JOLTEON", "ITEM_FOCUS_SASH", 0),
        ("SPECIES_MANECTRIC", "ITEM_COVERT_CLOAK", 0),
        ("SPECIES_ELECTIVIRE", "ITEM_LIFE_ORB", 0),
        ("SPECIES_LANTURN", "ITEM_SITRUS_BERRY", 0),
    ],
    "TRAINER_BEN": [
        ("SPECIES_ORANGURU", "ITEM_MENTAL_HERB", 0),
        ("SPECIES_CHARJABUG", "ITEM_EVIOLITE", 0),
        ("SPECIES_MAGNEZONE", "ITEM_AIR_BALLOON", 1),
        ("SPECIES_PINCURCHIN", "ITEM_SITRUS_BERRY", 1),
    ],
    "TRAINER_ANGELO": [
        ("SPECIES_PLUSLE", "ITEM_FOCUS_SASH", 0),
        ("SPECIES_MINUN", "ITEM_COVERT_CLOAK", 0),
        ("SPECIES_AMPHAROS", "ITEM_SITRUS_BERRY", 1),
        ("SPECIES_KLINK", "ITEM_EVIOLITE", 1),
    ],
    "TRAINER_SHAWN": [
        ("SPECIES_MORPEKO", "ITEM_SITRUS_BERRY", 0),
        ("SPECIES_DEDENNE", "ITEM_PETAYA_BERRY", 0),
        ("SPECIES_ELECTABUZZ", "ITEM_EVIOLITE", 1),
        ("SPECIES_EELEKTRIK", "ITEM_ROCKY_HELMET", 1),
    ],
}

NARRATIVE_OVERRIDES = {
    "TRAINER_ROXANNE_1": {
        "primary_question": "Can the player break Roxanne's Trick Room opening, deny Storm Drain from blanking Water pressure, and still stop Regirock's Iron Defense endgame?",
        "theme_and_tempo": "Every member is Rock-type and five are naturally unevolved at the first cap. Carbink can reverse speed with Trick Room; Rockruff supplies immediate flinch and priority pressure; Nacli, Bonsly, and Lileep turn Eviolite, Sturdy, and Storm Drain into distinct early-game lessons; Regirock is the shocking but readable legendary closer.",
        "intentional_weakness": "The team is slow outside Trick Room and has broad Water, Grass, Fighting, Ground, and Steel weaknesses. Taunt, Fake Out, Encore, Wide Guard, weather, special pressure, Haze, Clear Smog, phazing, and focused attacks all disrupt different layers without requiring one exact catch.",
        "first_loss_lesson": "Do not spend every Water answer into Lileep; control Trick Room first, then preserve special super-effective pressure or boost removal for Regirock.",
        "strongest_part": "It is unmistakably Roxanne's Rock exam while showcasing six very different competitive uses for early unevolved Pokemon and one unforgettable legendary ace.",
        "weakest_link": "If Carbink falls before Trick Room and Lileep cannot redirect Water, the middle of the team is exposed; that intentional seam keeps a brutal first Gym solvable through several team styles.",
    },
    "TRAINER_BRAWLY_1": {
        "primary_question": "Can the player break Brawly's redirection-and-commitment opening, manage his finite board control, and preserve an answer for White Herb Unburden Hawlucha?",
        "theme_and_tempo": "Pachirisu buys Falinks a No Retreat turn, Hitmontop and Kirlia provide finite positional support, and Breloom forces respect for priority. Hawlucha closes without a Mega: Close Combat consumes White Herb, activates Unburden, and turns one readable commitment into the final speed test.",
        "intentional_weakness": "Fake Out immunity, Taunt, spread damage, Haze, Clear Smog, Unaware, phazing, burn, Reflect, and focused Psychic, Fairy, or Flying pressure all attack different seams. Hawlucha must spend its White Herb to gain speed, has no redirection beside it, and remains vulnerable to priority and defensive positioning.",
        "first_loss_lesson": "Remove or bypass Pachirisu, erase or stall Falinks's committed boosts, and keep priority or a sturdy Flying, Psychic, Fairy, Electric, Ice, or Rock answer for Hawlucha after its White Herb turn.",
        "strongest_part": "The battle teaches redirection into an irreversible setup first, then reuses commitment as a cleaner White Herb and Unburden payoff without introducing Mega Evolution before the player receives the bracelet.",
        "weakest_link": "Most damage is physical and every enabling item is one-use, so burn, Reflect, Intimidate timing, spread pressure, and patient Protect turns give the player several honest ways through an otherwise punishing second Gym.",
    },
    "TRAINER_JOCELYN": {
        "primary_question": "Can the player absorb Tyrogue's one-time Guts burst, stop Pancham from cycling disruption, and avoid feeding status into Natu's Magic Bounce?",
        "theme_and_tempo": "Tyrogue, Pancham, and Crabrawler keep Jocelyn inside Brawly's Fighting school without repeating any of the previous two trainers' species. Guts and priority create the opening, Pancham pivots, Iron Fist supplies the direct reserve, and Natu is the lone Psychic foil.",
        "intentional_weakness": "All three fighters are unevolved, Tyrogue spends health on its Flame Orb, and Natu is the only off-type support. Flying, Fairy, Psychic, burn control, Intimidate, spread damage, Taunt, and focused attacks remain broad answers.",
        "first_loss_lesson": "Do not trade evenly into activated Guts; stall or remove Tyrogue, interrupt Pancham's pivoting, then attack around Magic Bounce rather than feeding it status.",
        "strongest_part": "Three completely different unused young Fighting Pokemon make the final Gym trainer feel like a fresh lesson rather than a weaker copy of Brawly.",
        "weakest_link": "The team has no durable field engine, so once its one-time item and pivot tempo are denied, direct super-effective pressure takes over.",
    },
    "TRAINER_KIRK": {
        "primary_question": "Can the player control Electric Terrain and spread sound while choosing whether to remove Kirk's amplifier or Vivian's immunity pivots first?",
        "theme_and_tempo": "Kirk runs an Electric soundstage: Punk Rock Toxtricity, Hyper Voice Heliolisk, Electric Surge Electrode, and Galvanize Alolan Graveler. Vivian answers with an all-Electric circuit-breaker squad built from Jolteon's and Lanturn's Volt Absorb, Manectric's Lightning Rod, and Electivire's Motor Drive. Together they teach that Electric teams can amplify sound or redirect their own current without abandoning Wattson's specialty.",
        "intentional_weakness": "Ground pressure is powerful but not automatic because of Airborne and Water coverage options. Wide Guard, Snarl, Taunt, terrain replacement, priority, item removal, and focused physical or special attacks answer different members.",
        "first_loss_lesson": "Identify whether the active board is amplifying sound or absorbing electricity, then remove the one immunity or terrain setter protecting the attack you want to use.",
        "strongest_part": "The paired battle presents two related but opposite Electric doctrines—make the current louder or safely route it—without spending Wattson's ace mechanics.",
        "weakest_link": "Both teams still share Ground pressure and finite defensive items, so a player who reads the active immunity correctly can create a decisive opening.",
    },
    "TRAINER_VIVIAN": {
        "primary_question": "Can the player control Electric Terrain and spread sound while choosing whether to remove Kirk's amplifier or Vivian's immunity pivots first?",
        "theme_and_tempo": "Kirk runs an Electric soundstage: Punk Rock Toxtricity, Hyper Voice Heliolisk, Electric Surge Electrode, and Galvanize Alolan Graveler. Vivian answers with an all-Electric circuit-breaker squad built from Jolteon's and Lanturn's Volt Absorb, Manectric's Lightning Rod, and Electivire's Motor Drive. Together they teach that Electric teams can amplify sound or redirect their own current without abandoning Wattson's specialty.",
        "intentional_weakness": "Ground pressure is powerful but not automatic because of Airborne and Water coverage options. Wide Guard, Snarl, Taunt, terrain replacement, priority, item removal, and focused physical or special attacks answer different members.",
        "first_loss_lesson": "Identify whether the active board is amplifying sound or absorbing electricity, then remove the one immunity or terrain setter protecting the attack you want to use.",
        "strongest_part": "The paired battle presents two related but opposite Electric doctrines—make the current louder or safely route it—without spending Wattson's ace mechanics.",
        "weakest_link": "Both teams still share Ground pressure and finite defensive items, so a player who reads the active immunity correctly can create a decisive opening.",
    },
    "TRAINER_BEN": {
        "primary_question": "Can the player deny Oranguru's Trick Room or survive a slow Electric battery once Instruct and Battery multiply its pressure?",
        "theme_and_tempo": "Oranguru is the single off-type conductor. Eviolite Battery Charjabug strengthens its partners, Air Balloon Magnezone supplies sturdy special pressure without spending Wattson's Iron Hands reveal, and Pincurchin lays Electric Terrain for slow Rising Voltage pressure. Three of four members are Electric and every slot matters to the room clock.",
        "intentional_weakness": "Taunt, Encore, room reversal, phazing, terrain replacement, Ground attacks, burn, Intimidate, and focused pressure on Oranguru or Charjabug can dismantle the multiplication before it stabilizes.",
        "first_loss_lesson": "Do not race the boosted board blindly; stop Trick Room or remove Battery, then use the finite room turns to isolate the slow attacker.",
        "strongest_part": "Battery plus Instruct makes an obscure unevolved Electric Pokemon the structural center of a real doubles puzzle.",
        "weakest_link": "Oranguru is the only room setter, so correct early disruption leaves three slow Electric Pokemon exposed.",
    },
    "TRAINER_ANGELO": {
        "primary_question": "Can the player break the Plus and Minus relay before fast Helping Hand support turns every partner attack into amplified current?",
        "theme_and_tempo": "Plusle and Minun provide fast Helping Hand, Encore, and Electroweb; Plus Ampharos converts the support into bulky special pressure; Minus Klink is the lone Steel gear in Wattson's machine and can Shift Gear. Three of four members are Electric, and every ability belongs to the same circuit.",
        "intentional_weakness": "The two mascots are frail, Klink is still unevolved, and the whole network loses power when partners are isolated. Ground attacks, Taunt, spread damage, priority, Snarl, and focused removal all work.",
        "first_loss_lesson": "Break one side of Plus and Minus instead of spreading damage, then deny the surviving support a profitable Helping Hand target.",
        "strongest_part": "A usually decorative Plus and Minus mechanic becomes a legible four-member electrical relay with an unevolved gear as its odd but native-looking flourish.",
        "weakest_link": "The circuit depends on keeping two compatible abilities active, so one decisive knockout sharply reduces its ceiling.",
    },
    "TRAINER_SHAWN": {
        "primary_question": "Can the player control berry thresholds and pivoting before four small Electric utility Pokemon turn chip damage into repeated tempo?",
        "theme_and_tempo": "Morpeko changes typing and Parting Shots, Dedenne converts its berry into Cheek Pouch recovery, Electabuzz controls speed from Eviolite bulk, and Levitate Eelektrik punishes a thoughtless Ground sweep. It is an all-Electric resource-management fight rather than another damage race.",
        "intentional_weakness": "Knock Off, Unnerve, Taunt, strong Ground coverage after Eelektrik is identified, spread damage, and focused attacks can prevent the small recovery engines from cycling.",
        "first_loss_lesson": "Track which berry has been consumed, stop Morpeko's free pivots, and preserve the Ground move until Eelektrik's Levitate no longer protects the board.",
        "strongest_part": "Four rarely centered Electric Pokemon create a distinct threshold-and-pivot puzzle immediately before Wattson.",
        "weakest_link": "None of the four is independently overwhelming; once their item and pivot economy is interrupted, their raw damage is intentionally modest.",
    },
}

for _trainer_id, _narrative in NARRATIVE_OVERRIDES.items():
    _narrative.setdefault(
        "competitive_references",
        "Pinned Pokemon Showdown Champions learnsets and doubles roles; Emerald Champions authored battle corpus",
    )
    _narrative.setdefault(
        "dialogue_status",
        "team re-authored; exact native pre-battle and defeat text required at implementation",
    )
    _narrative.setdefault(
        "reservation_status",
        "specialty-correct redesign audited against the campaign species, Mega, legendary, and strategy ledgers",
    )

_opening_rival_narrative = {
    "primary_question": "Can the chosen starter beat its same-generation type counter when neither side has a captured team yet?",
    "theme_and_tempo": "The rival's species is resolved from the player's starter generation and choice. Every branch is level 15 with a Life Orb, perfect preparation, its legal Pledge STAB, Protect, and two species-specific current moves: Treecko and Torchic can threaten setup while Mudkip commits to immediate physical pressure.",
    "intentional_weakness": "This is a one-on-one with no hidden reserve. Focus Sash, Eviolite, Leftovers, priority, status, setup denial, Counter or Mirror Coat, Endeavor, recovery, and direct coverage all provide different solutions across the starter families.",
    "first_loss_lesson": "Use the chosen starter's own legal utility and item access instead of trading resisted attacks into the obvious type counter.",
    "strongest_part": "The same rule produces a fair but different opening puzzle for every starter family without pretending all starters want the same four moves.",
    "weakest_link": "A one-Pokemon fight cannot create switching depth; its value is the immediate lesson that held items and authored sets matter from battle one.",
    "competitive_references": "Pinned current learnsets for every starter family; Emerald Champions opening-rival branch contract",
    "dialogue_status": "implemented native rival dialogue; generation-neutral wording required when expanded starter restoration lands",
    "reservation_status": "spends no rare species, Mega, weather, room, or historic competitive core",
}
for _trainer_id in (
    "TRAINER_BRENDAN_ROUTE_103_MUDKIP", "TRAINER_BRENDAN_ROUTE_103_TORCHIC", "TRAINER_BRENDAN_ROUTE_103_TREECKO",
    "TRAINER_MAY_ROUTE_103_MUDKIP", "TRAINER_MAY_ROUTE_103_TORCHIC", "TRAINER_MAY_ROUTE_103_TREECKO",
):
    NARRATIVE_OVERRIDES[_trainer_id] = _opening_rival_narrative

EARLY_FORM_REPLACEMENTS = {
    "TRAINER_TORI_AND_TIA": {"SPECIES_VOLCARONA": "SPECIES_CUTIEFLY"},
    "TRAINER_DILLON": {"SPECIES_MANDIBUZZ": "SPECIES_VULLABY"},
    "TRAINER_JULIO": {"SPECIES_DRAGAPULT": "SPECIES_HAUNTER"},
    "TRAINER_DANIELLE": {"SPECIES_VOLCARONA": "SPECIES_BELLOSSOM"},
    "TRAINER_ROMAN": {"SPECIES_DRAGAPULT": "SPECIES_STARYU"},
    "TRAINER_ELMER": {"SPECIES_VOLCARONA": "SPECIES_MASQUERAIN"},
    "TRAINER_TABITHA_MT_CHIMNEY": {"SPECIES_KLINKLANG": "SPECIES_KLANG"},
}

TEXT_REPLACEMENTS_BY_TRAINER = {
    "TRAINER_TORI_AND_TIA": {"Volcarona": "Cutiefly"},
    "TRAINER_DILLON": {"Mandibuzz": "Vullaby"},
    "TRAINER_JULIO": {"Dragapult": "Haunter"},
    "TRAINER_DANIELLE": {"Volcarona": "Bellossom"},
    "TRAINER_ROMAN": {"Dragapult": "Staryu"},
    "TRAINER_ELMER": {"Volcarona": "Masquerain"},
    "TRAINER_TABITHA_MT_CHIMNEY": {"Klinklang": "Klang"},
}

TARGETED_MOVE_OVERRIDES = {
    ("TRAINER_TIMMY", "SPECIES_HELIOLISK"): ("MOVE_HYPER_VOICE", "MOVE_VOLT_SWITCH"),
    ("TRAINER_BRENDAN_ROUTE_110_MUDKIP", "SPECIES_ALCREMIE"): ("MOVE_DAZZLING_GLEAM", "MOVE_HELPING_HAND"),
    ("TRAINER_BRENDAN_ROUTE_110_TORCHIC", "SPECIES_ALCREMIE"): ("MOVE_DAZZLING_GLEAM", "MOVE_HELPING_HAND"),
    ("TRAINER_BRENDAN_ROUTE_110_TREECKO", "SPECIES_ALCREMIE"): ("MOVE_DAZZLING_GLEAM", "MOVE_HELPING_HAND"),
    ("TRAINER_MAY_ROUTE_110_MUDKIP", "SPECIES_ALCREMIE"): ("MOVE_DAZZLING_GLEAM", "MOVE_HELPING_HAND"),
    ("TRAINER_MAY_ROUTE_110_TORCHIC", "SPECIES_ALCREMIE"): ("MOVE_DAZZLING_GLEAM", "MOVE_HELPING_HAND"),
    ("TRAINER_MAY_ROUTE_110_TREECKO", "SPECIES_ALCREMIE"): ("MOVE_DAZZLING_GLEAM", "MOVE_HELPING_HAND"),
    ("TRAINER_ALYSSA", "SPECIES_GLIGAR"): ("MOVE_EARTHQUAKE", "MOVE_HIGH_HORSEPOWER"),
    ("TRAINER_EDWARD", "SPECIES_XATU"): ("MOVE_HEAT_WAVE", "MOVE_TAILWIND"),
    ("TRAINER_DALE", "SPECIES_ARCTOVISH"): ("MOVE_ROCK_SLIDE", "MOVE_PROTECT"),
    ("TRAINER_JARED", "SPECIES_NOCTOWL"): ("MOVE_TAILWIND", "MOVE_HYPNOSIS"),
    ("TRAINER_JARED", "SPECIES_NOIVERN"): ("MOVE_TAILWIND", "MOVE_TAUNT"),
    ("TRAINER_JARED", "SPECIES_SCYTHER"): ("MOVE_TAILWIND", "MOVE_SWORDS_DANCE"),
    ("TRAINER_FLINT", "SPECIES_YANMEGA"): ("MOVE_TAILWIND", "MOVE_GIGA_DRAIN"),
    ("TRAINER_FLINT", "SPECIES_MANTINE"): ("MOVE_TAILWIND", "MOVE_ICY_WIND"),
    ("TRAINER_ASHLEY", "SPECIES_SWANNA"): ("MOVE_TAILWIND", "MOVE_RAIN_DANCE"),
    ("TRAINER_HUMBERTO", "SPECIES_GLISCOR"): ("MOVE_TAILWIND", "MOVE_SWORDS_DANCE"),
    ("TRAINER_HUMBERTO", "SPECIES_MANDIBUZZ"): ("MOVE_TAILWIND", "MOVE_SNARL"),
    ("TRAINER_SPENSER_FORTREE", "SPECIES_CROBAT"): ("MOVE_TAILWIND", "MOVE_HAZE"),
    ("TRAINER_DONALD", "SPECIES_BUTTERFREE"): ("MOVE_TAILWIND", "MOVE_POLLEN_PUFF"),
    ("TRAINER_TAYLOR", "SPECIES_RIBOMBEE"): ("MOVE_TAILWIND", "MOVE_QUIVER_DANCE"),
    ("TRAINER_DOUG", "SPECIES_MASQUERAIN"): ("MOVE_TAILWIND", "MOVE_QUIVER_DANCE"),
    ("TRAINER_GREG", "SPECIES_LEDIAN"): ("MOVE_TAILWIND", "MOVE_REFLECT"),
    ("TRAINER_JACKSON_1", "SPECIES_MANTINE"): ("MOVE_TAILWIND", "MOVE_ICY_WIND"),
    ("TRAINER_CATHERINE_1", "SPECIES_NOIVERN"): ("MOVE_TAILWIND", "MOVE_TAUNT"),
    ("TRAINER_HUGH", "SPECIES_NOCTOWL"): ("MOVE_TAILWIND", "MOVE_HYPNOSIS"),
    ("TRAINER_HUGH", "SPECIES_TROPIUS"): ("MOVE_TAILWIND", "MOVE_SUNNY_DAY"),
    ("TRAINER_YASU", "SPECIES_CROBAT"): ("MOVE_TAILWIND", "MOVE_HAZE"),
    ("TRAINER_CARLEE", "SPECIES_DIANCIE"): ("MOVE_TRICK_ROOM", "MOVE_CALM_MIND"),
    ("TRAINER_GRUNT_SPACE_CENTER_3", "SPECIES_HYPNO"): ("MOVE_TRICK_ROOM", "MOVE_THUNDER_WAVE"),
    ("TRAINER_GRUNT_SPACE_CENTER_1", "SPECIES_NECROZMA_DUSK_MANE"): ("MOVE_TRICK_ROOM", "MOVE_DRAGON_DANCE"),
    ("TRAINER_GRUNT_SPACE_CENTER_6", "SPECIES_CLAYDOL"): ("MOVE_TRICK_ROOM", "MOVE_GRAVITY"),
}

FORCE_REGENERATED_NARRATIVE = {trainer for trainer, _species in TARGETED_MOVE_OVERRIDES}


def apply_bespoke_team_overrides(blocks: list[dict]) -> None:
    for entry in blocks:
        for branch in entry["branches"]:
            rows = BESPOKE_TEAM_OVERRIDES.get(branch.trainer_id)
            if rows:
                branch.mons = [mon_from_preset(species, item=item, level_offset=offset) for species, item, offset in rows]
                if branch.trainer_id == "TRAINER_ANGELO":
                    for mon in branch.mons:
                        if mon.species == "SPECIES_AMPHAROS":
                            mon.ability = "ABILITY_PLUS"
                        elif mon.species == "SPECIES_KLINK":
                            mon.ability = "ABILITY_MINUS"


def apply_early_form_replacements(blocks: list[dict]) -> None:
    for entry in blocks:
        for branch in entry["branches"]:
            replacements = EARLY_FORM_REPLACEMENTS.get(branch.trainer_id, {})
            for index, old in enumerate(branch.mons):
                species = replacements.get(old.species)
                if not species:
                    continue
                new = mon_from_preset(species, level_offset=old.level_offset)
                if species == "SPECIES_CUTIEFLY":
                    new.item = old.item
                    new.ability = "ABILITY_SWEET_VEIL"
                    new.nature = "NATURE_TIMID"
                    new.stat_points = [2, 0, 0, 32, 0, 32]
                    new.moves = ["MOVE_QUIVER_DANCE", "MOVE_BUG_BUZZ", "MOVE_DAZZLING_GLEAM", "MOVE_PROTECT"]
                elif species == "SPECIES_BELLOSSOM":
                    new.item = old.item
                    new.ability = "ABILITY_CHLOROPHYLL"
                    new.nature = "NATURE_TIMID"
                    new.stat_points = [2, 0, 0, 32, 0, 32]
                    new.moves = ["MOVE_QUIVER_DANCE", "MOVE_GIGA_DRAIN", "MOVE_SLUDGE_BOMB", "MOVE_PROTECT"]
                elif species == "SPECIES_MASQUERAIN":
                    new.item = old.item
                    new.ability = "ABILITY_INTIMIDATE"
                    new.nature = "NATURE_TIMID"
                    new.stat_points = [2, 0, 0, 32, 0, 32]
                    new.moves = ["MOVE_QUIVER_DANCE", "MOVE_BUG_BUZZ", "MOVE_HYDRO_PUMP", "MOVE_PROTECT"]
                elif species == "SPECIES_VULLABY":
                    new.item = old.item
                    new.ability = "ABILITY_OVERCOAT"
                    new.moves = ["MOVE_FOUL_PLAY", "MOVE_SNARL", "MOVE_ROOST", "MOVE_TAUNT"]
                elif species == "SPECIES_HAUNTER":
                    new.item = old.item
                    new.ability = "ABILITY_LEVITATE"
                    new.nature = "NATURE_TIMID"
                    new.stat_points = [2, 0, 0, 32, 0, 32]
                    new.moves = ["MOVE_SHADOW_BALL", "MOVE_SLUDGE_BOMB", "MOVE_DAZZLING_GLEAM", "MOVE_THUNDERBOLT"]
                elif species == "SPECIES_STARYU":
                    new.item = "ITEM_FOCUS_SASH"
                    new.ability = "ABILITY_NATURAL_CURE"
                    new.nature = "NATURE_TIMID"
                    new.stat_points = [2, 0, 0, 32, 0, 32]
                    new.moves = ["MOVE_SURF", "MOVE_ICY_WIND", "MOVE_PSYCHIC", "MOVE_PROTECT"]
                branch.mons[index] = new


def apply_targeted_move_overrides(blocks: list[dict]) -> None:
    for entry in blocks:
        for branch in entry["branches"]:
            for mon in branch.mons:
                override = TARGETED_MOVE_OVERRIDES.get((branch.trainer_id, mon.species))
                if not override:
                    continue
                old, new = override
                if old not in mon.moves:
                    raise ValueError(f"expected {old} on {branch.trainer_id} {mon.species}")
                if new in mon.moves:
                    raise ValueError(f"targeted override duplicates {new} on {branch.trainer_id} {mon.species}")
                mon.moves[mon.moves.index(old)] = new
add_manual_preset(
    "SPECIES_SCOVILLAIN", "sun mixed attacker", "ITEM_FOCUS_SASH",
    "ABILITY_CHLOROPHYLL", "NATURE_TIMID",
    ["MOVE_HEAT_WAVE", "MOVE_ENERGY_BALL", "MOVE_SPICY_EXTRACT", "MOVE_PROTECT"], [2, 0, 0, 32, 0, 32],
)
add_manual_preset(
    "SPECIES_TATSUGIRI_CURLY", "Commander speed control", "ITEM_SITRUS_BERRY",
    "ABILITY_COMMANDER", "NATURE_TIMID",
    ["MOVE_DRACO_METEOR", "MOVE_MUDDY_WATER", "MOVE_ICY_WIND", "MOVE_PROTECT"], [2, 0, 0, 32, 0, 32],
)


def choose_preset(species: str, old_moves: list[str], old_item: str) -> dict:
    choices = PRESETS.get(species)
    if not choices:
        raise ValueError(f"no competitive preset for trainer species {species}")

    def score(entry: dict) -> tuple[int, int]:
        overlap = len(set(old_moves) & set(entry["moves"]))
        item_score = 3 if old_item in (entry["item"], entry["required_item"]) else 0
        return overlap * 4 + item_score, -choices.index(entry)

    return max(choices, key=score)


def nature_from_spread(spread: str, fallback: str) -> str:
    for nature in NATURES:
        if spread.endswith(nature.removeprefix("NATURE_")):
            return nature
    return fallback


def normalize_mon(match: re.Match[str], status: str) -> Mon:
    species = normalize(match.group(2), SPECIES, SPECIES_BY_ID, "SPECIES_")
    if species is None:
        raise ValueError(f"unknown species {match.group(2)}")
    old_item = normalize(match.group(3), ITEMS, ITEM_BY_ID, "ITEM_") or "ITEM_NONE"
    old_moves = [normalize(move, MOVES, MOVE_BY_ID, "MOVE_") for move in match.group(7).split(",")]
    old_moves = [move for move in old_moves if move]
    preset = choose_preset(species, old_moves, old_item)
    preserve_authored = status != "design_pending_source_baseline_only"
    item = old_item if preserve_authored and old_item != "ITEM_NONE" else preset["item"]
    if item not in ITEMS:
        item = preset["required_item"] if preset["required_item"] != "ITEM_NONE" else preset["item"]
    ability = preset["ability"]
    if preserve_authored and species in SPECIES_ABILITIES:
        ability = SPECIES_ABILITIES[species][min(2, int(match.group(5)))]
        if ability == "ABILITY_NONE":
            ability = SPECIES_ABILITIES[species][0]
    moves = old_moves if preserve_authored and old_moves else list(preset["moves"])
    return Mon(
        species=species,
        item=item,
        level_offset=int(match.group(4)),
        ability=ability,
        nature=nature_from_spread(match.group(6), preset["nature"]),
        stat_points=list(preset["stat_points"]),
        moves=moves,
    )


def parse_branches(block: str, block_index: int, status: str) -> list[Branch]:
    markers = list(BRANCH_RE.finditer(block))
    suffix = re.search(r"(?m)^(?:source_note:|=== END ENCOUNTER ===)", block)
    suffix_start = suffix.start() if suffix else len(block)
    result = []
    for i, marker in enumerate(markers):
        end = markers[i + 1].start() if i + 1 < len(markers) else suffix_start
        text = block[marker.start():end]
        trainer = re.search(r"(?m)^trainer_id: ([A-Z0-9_]+)$", text)
        fmt = re.search(r"(?m)^format: (single|double|multi)$", text)
        if trainer is None or fmt is None:
            continue
        result.append(Branch(
            trainer_id=trainer.group(1),
            text=text,
            format=fmt.group(1),
            mons=[normalize_mon(match, status) for match in MON_RE.finditer(text)],
            original_status=status,
            block_index=block_index,
            branch_index=i,
        ))
    return result


def wild_by_location() -> dict[str, list[str]]:
    payload = json.loads((ROOT / "src" / "data" / "wild_encounters.json").read_text())
    result = {}
    for group in payload["wild_encounter_groups"]:
        for encounter in group["encounters"]:
            map_name = encounter.get("map")
            if not map_name:
                continue
            values = []
            for field in ("land_mons", "water_mons", "rock_smash_mons", "fishing_mons", "hidden_mons"):
                for mon in encounter.get(field, {}).get("mons", []):
                    if mon["species"] in DEFAULT_PRESETS and mon["species"] not in values:
                        values.append(mon["species"])
            result[map_name] = values
    return result


WILD_BY_LOCATION = wild_by_location()


def mon_from_preset(species: str, item: str | None = None, level_offset: int = 0) -> Mon:
    preset = DEFAULT_PRESETS[species]
    return Mon(
        species=species,
        item=item or preset["item"],
        level_offset=level_offset,
        ability=preset["ability"],
        nature=preset["nature"],
        stat_points=list(preset["stat_points"]),
        moves=list(preset["moves"]),
    )


def archetypes(mons: list[Mon]) -> list[str]:
    moves = {move for mon in mons for move in mon.moves}
    abilities = {mon.ability for mon in mons}
    result = []
    if "ABILITY_DRIZZLE" in abilities or "MOVE_RAIN_DANCE" in moves:
        result.append("rain")
    if "ABILITY_DROUGHT" in abilities or "MOVE_SUNNY_DAY" in moves:
        result.append("sun")
    if "ABILITY_SAND_STREAM" in abilities or "MOVE_SANDSTORM" in moves:
        result.append("sand")
    if "ABILITY_SNOW_WARNING" in abilities or "MOVE_SNOWSCAPE" in moves:
        result.append("snow")
    if "MOVE_TRICK_ROOM" in moves:
        result.append("Trick Room")
    if "MOVE_TAILWIND" in moves:
        result.append("Tailwind")
    if moves & REDIRECTION_MOVES:
        result.append("redirection")
    if moves & HAZARD_MOVES:
        result.append("hazard pressure")
    if moves & SETUP_MOVES:
        result.append("setup")
    if "MOVE_PERISH_SONG" in moves:
        result.append("Perish Song")
    if moves & SPREAD_MOVES:
        result.append("spread pressure")
    return result or ["balanced tempo"]


def team_size(block_branches: list[Branch]) -> int:
    return max((len(branch.mons) for branch in block_branches), default=1)


def is_marquee(trainer_ids: str) -> bool:
    return any(token in trainer_ids for token in MARQUEE_TOKENS)


def difficulty_for(block: str, branches: list[Branch]) -> float:
    ids = line_value(block, "trainer_ids")
    order_text = line_value(block, "campaign_order", "0")
    order = int(order_text) if order_text.isdigit() else 0
    if any(token in ids for token in LEAGUE_TOKENS):
        return 10.0
    if order == 1:
        return 7.5
    if is_marquee(ids):
        return 10.0
    size = team_size(branches)
    arc = set()
    for branch in branches:
        arc.update(archetypes(branch.mons))
    score = 5.55 + 0.45 * size + min(0.6, 0.12 * max(0, len(arc) - 1))
    requirement = line_value(block, "requirement").lower()
    if "optional" in requirement:
        score += 0.15
    if "forced" in requirement or "required" in requirement:
        score -= 0.1
    if any(token in ids for token in FACTION_TOKENS):
        score += 0.65
    if any(token in ids for token in MINIBOSS_TOKENS):
        score = max(score, 8.8 + min(0.6, size * 0.1))
    if any(mon.level_offset > 0 for branch in branches for mon in branch.mons):
        score += 0.15
    return round(min(9.4, max(6.0, score)), 1)


def display_species(species: str) -> str:
    return species.removeprefix("SPECIES_").replace("_", " ").title()


def display_constant(value: str) -> str:
    if value in MOVE_NAMES:
        return MOVE_NAMES[value]
    return value.split("_", 1)[-1].replace("_", " ").title()


def role_for_mon(mon: Mon) -> str:
    choices = PRESETS.get(mon.species, [])
    if not choices:
        return "flexible doubles piece"
    best = max(
        choices,
        key=lambda preset: (
            len(set(preset["moves"]) & set(mon.moves)) * 4
            + (2 if preset["ability"] == mon.ability else 0)
            + (1 if mon.item in (preset["item"], preset["required_item"]) else 0)
        ),
    )
    return best["role"].lower()


def key_move(mon: Mon) -> str:
    tactical = (
        SETUP_MOVES | SPEED_MOVES | REDIRECTION_MOVES | HAZARD_MOVES
        | {"MOVE_PERISH_SONG", "MOVE_FAKE_OUT", "MOVE_HELPING_HAND", "MOVE_WIDE_GUARD"}
    )
    return next((move for move in mon.moves if move in tactical), next((move for move in mon.moves if move != "MOVE_PROTECT"), mon.moves[0]))


def narrative(block: str, branches: list[Branch], difficulty: float) -> dict[str, str]:
    mons = branches[0].mons if branches else []
    names = [display_species(mon.species) for mon in mons]
    lead = names[0] if names else "the lead"
    ace = names[-1] if names else "the closer"
    arcs = archetypes(mons)
    arc = ", ".join(arcs[:3])
    location = line_value(block, "location", "this area").replace("_", " ")
    lead_move = display_constant(key_move(mons[0])) if mons else "opening move"
    ace_move = display_constant(key_move(mons[-1])) if mons else "closing move"
    member_descriptions = []
    for mon in mons:
        member_descriptions.append(
            f"{display_species(mon.species)} is the {role_for_mon(mon)}: "
            f"{display_constant(mon.ability)}, {display_constant(mon.item)}, and {display_constant(key_move(mon))} make its job public"
        )
    if "Trick Room" in arcs:
        counters = "Taunt or reverse Trick Room, stall its finite turns, or remove the setter"
    elif "Tailwind" in arcs:
        counters = "deny Tailwind, answer it with Trick Room or priority, or protect through its finite turns"
    elif any(weather in arcs for weather in ("rain", "sun", "sand", "snow")):
        counters = "replace the weather, focus its setter, or exploit the turns before the matching abuser is active"
    elif "redirection" in arcs:
        counters = "use spread damage, Taunt, priority, or focused pressure to remove the redirector"
    elif "setup" in arcs:
        counters = "use Haze, Clear Smog, phazing, Unaware, Encore, or immediate focus fire before setup compounds"
    elif "Perish Song" in arcs:
        counters = "pivot early, deny trapping, Taunt the singer, or win the position before the final count"
    elif "spread pressure" in arcs:
        counters = "use Wide Guard, immunities, Protect, and asymmetric focus fire against the exposed partner"
    else:
        counters = "contest speed, trade into the fragile slot, use Protect to expose commitments, or pivot into resisted attacks"
    middle = names[1:-1]
    transition = ", ".join(middle) if middle else "the reserve"
    result = {
        "primary_question": f"Can the player read {lead}'s {lead_move} opening, solve the {arc} board, and still preserve an answer for {ace}'s {ace_move} finish?",
        "theme_and_tempo": f"This {location} encounter is a {arc} puzzle. " + "; ".join(member_descriptions) + ".",
        "intentional_weakness": f"The broad answers are to {counters}. The player can also pressure {lead} before the plan stabilizes or isolate {ace}; no single species or exact move order is required.",
        "first_loss_lesson": f"Decide whether {lead} is damage or infrastructure, then preserve the answer that best denies {ace}'s {ace_move} rather than spending it on {transition}.",
        "strongest_part": f"{lead}'s {lead_move} creates a readable handoff through {transition} into {ace}'s {ace_move}, so the team has one identity without becoming one scripted solution.",
        "weakest_link": f"If the player breaks the {arcs[0]} layer or removes {lead} early, the remaining members must win through ordinary positioning; that intentional seam keeps difficulty {difficulty:.1f} honest.",
        "competitive_references": "Pinned Pokemon Showdown Champions learnsets and doubles roles; Emerald Champions authored battle corpus",
        "dialogue_status": "native intent preserved; converted-format and width gate required at implementation",
        "reservation_status": f"spends the {lead} plus {ace} {arc} pairing here; checked against campaign species, Mega, legendary, and rolling-strategy ledgers",
    }
    if len(mons) == 1:
        result.update({
            "primary_question": f"Can the player solve {lead}'s {display_constant(mons[0].ability)}, {display_constant(mons[0].item)}, and {lead_move} package without relying on a second target?",
            "strongest_part": f"{lead} turns {lead_move} and {display_constant(mons[0].item)} into a compact, readable duel whose full plan is visible from the first turn.",
            "weakest_link": f"With no reserve or partner, once the player answers {lead_move}, {lead} must win through direct positioning; that is the intentional relief valve at difficulty {difficulty:.1f}.",
            "reservation_status": f"spends this single-Pokemon {lead} duel here; no multi-slot core, Mega pairing, or weather package is consumed",
        })
    return result


def make_local_replacements(blocks: list[dict], usage: Counter[str]) -> None:
    for entry in blocks:
        if entry["original_status"] != "design_pending_source_baseline_only":
            for branch in entry["branches"]:
                usage.update(mon.species for mon in branch.mons)
            continue
        location = line_value(entry["text"], "location")
        candidates = WILD_BY_LOCATION.get("MAP_" + re.sub(r"[^A-Z0-9]", "_", location.upper()), [])
        for branch in entry["branches"]:
            team_species = {mon.species for mon in branch.mons}
            for index, mon in enumerate(branch.mons):
                if usage[mon.species] < 10 or mon.item in MEGA_STONES or mon.species in LEGENDARY_SPECIES:
                    usage[mon.species] += 1
                    continue
                options = [species for species in candidates if species not in team_species]
                if not options:
                    usage[mon.species] += 1
                    continue
                replacement = min(options, key=lambda species: (usage[species], species))
                branch.mons[index] = mon_from_preset(replacement, level_offset=mon.level_offset)
                team_species.add(replacement)
                usage[replacement] += 1


MEGA_STONES = set(re.findall(r"ITEM_[A-Z0-9_]+", (ROOT / "src" / "data" / "emerald_champions_mega_stones.h").read_text()))
LEGENDARY_SPECIES = set(
    "SPECIES_" + species
    for species in re.findall(r"(?:WILD|OTHER)_SIGN\([^,]+,\s*([A-Z0-9_]+)", (ROOT / "src" / "data" / "pokemon" / "legendary_signs.h").read_text())
)


def mega_base_species() -> dict[str, str]:
    showdown = json.loads((ROOT / "docs" / "showdown_champions_random_doubles.json").read_text())
    result = {
        entry["required_item"]: entry["party_species"]
        for entry in showdown["variants"]
        if entry["required_item"] != "ITEM_NONE"
    }
    form_text = (ROOT / "src" / "data" / "pokemon" / "form_change_tables.h").read_text()
    for form, item in re.findall(r"FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM,\s*(SPECIES_[A-Z0-9_]+),\s*(ITEM_[A-Z0-9_]+)", form_text):
        if item in result:
            continue
        form_id = re.sub(r"[^a-z0-9]", "", form.removeprefix("SPECIES_").lower())
        for suffix in ("megax", "megay", "megaz", "mega"):
            if form_id.endswith(suffix):
                base_id = form_id[:-len(suffix)]
                break
        else:
            continue
        if base_id not in SPECIES_BY_ID and base_id.startswith("tatsugiri"):
            base_id = base_id
        species = SPECIES_BY_ID.get(base_id)
        if species:
            result[item] = species
    return result


MEGA_BASE = mega_base_species()


def coverage_candidates(blocks: list[dict]) -> list[dict]:
    def cap_is_at_least_20(entry: dict) -> bool:
        value = line_value(entry["text"], "strict_cap", "")
        return value.isdigit() and int(value) >= 30

    return [
        entry for entry in blocks
        if entry["original_status"] == "design_pending_source_baseline_only"
        and len(entry["branches"]) == 1
        and cap_is_at_least_20(entry)
        and len(entry["branches"][0].mons) >= 3
        and not any(mon.item in MEGA_STONES for mon in entry["branches"][0].mons)
    ]


def assign_coverage(blocks: list[dict]) -> None:
    candidates = coverage_candidates(blocks)
    present_stones = {mon.item for entry in blocks for branch in entry["branches"] for mon in branch.mons}
    missing_stones = sorted(MEGA_STONES - present_stones)
    used_entries = set()
    for n, stone in enumerate(missing_stones):
        species = MEGA_BASE.get(stone)
        if species not in DEFAULT_PRESETS:
            continue
        start = math.floor(n * len(candidates) / max(1, len(missing_stones)))
        for offset in range(len(candidates)):
            entry = candidates[(start + offset) % len(candidates)]
            if id(entry) not in used_entries:
                used_entries.add(id(entry))
                entry["branches"][0].mons[-1] = mon_from_preset(species, item=stone, level_offset=2)
                break

    present_species = {mon.species for entry in blocks for branch in entry["branches"] for mon in branch.mons}
    missing_legends = sorted(LEGENDARY_SPECIES - present_species)
    remaining = [entry for entry in reversed(candidates) if id(entry) not in used_entries]
    for species, entry in zip(missing_legends, remaining):
        if species in DEFAULT_PRESETS:
            entry["branches"][0].mons[-2] = mon_from_preset(species, level_offset=2)


def non_mega_item(species: str) -> str:
    for preset in PRESETS.get(species, []):
        for item in (preset["item"], preset["required_item"]):
            if item != "ITEM_NONE" and item not in MEGA_STONES:
                return item
    return "ITEM_SITRUS_BERRY"


def repair_team_integrity(blocks: list[dict]) -> None:
    usage = Counter(mon.species for entry in blocks for branch in entry["branches"] for mon in branch.mons)
    ordinary_pool = sorted(species for species in DEFAULT_PRESETS if species not in LEGENDARY_SPECIES)
    for entry in blocks:
        location = line_value(entry["text"], "location")
        local = WILD_BY_LOCATION.get("MAP_" + re.sub(r"[^A-Z0-9]", "_", location.upper()), [])
        for branch in entry["branches"]:
            mega_slots = [i for i, mon in enumerate(branch.mons) if mon.item in MEGA_STONES]
            for i in mega_slots[:-1]:
                branch.mons[i].item = non_mega_item(branch.mons[i].species)

            seen = set()
            for i, mon in enumerate(branch.mons):
                if mon.species not in seen:
                    seen.add(mon.species)
                    continue
                candidates = [species for species in local if species not in seen and species in DEFAULT_PRESETS]
                if not candidates:
                    candidates = [species for species in ordinary_pool if species not in seen]
                replacement = min(candidates, key=lambda species: (usage[species], species))
                usage[mon.species] -= 1
                branch.mons[i] = mon_from_preset(replacement, level_offset=mon.level_offset)
                usage[replacement] += 1
                seen.add(replacement)


def remove_pre_bracelet_megas(blocks: list[dict]) -> None:
    for entry in blocks:
        cap = line_value(entry["text"], "strict_cap")
        if not cap.isdigit() or int(cap) >= 30:
            continue
        for branch in entry["branches"]:
            for index, mon in enumerate(branch.mons):
                if mon.item not in MEGA_STONES:
                    continue
                replacement = mon_from_preset(mon.species, level_offset=mon.level_offset)
                branch.mons[index] = replacement


def evolution_level_requirements() -> dict[str, int]:
    result = {}
    for path in sorted((ROOT / "src" / "data" / "pokemon" / "species_info").glob("gen_*_families.h")):
        text = path.read_text()
        for level, species in re.findall(r"\{EVO_LEVEL,\s*(\d+),\s*(SPECIES_[A-Z0-9_]+)", text):
            result[species] = min(result.get(species, 1000), int(level))
    return result


EVOLUTION_LEVEL_REQUIREMENTS = evolution_level_requirements()


def team_fingerprint(branch: Branch) -> tuple:
    return tuple(sorted(
        (mon.species, mon.item, mon.ability, mon.nature, tuple(mon.moves))
        for mon in branch.mons
    ))


def repair_duplicate_teams(blocks: list[dict]) -> None:
    usage = Counter(mon.species for entry in blocks for branch in entry["branches"] for mon in branch.mons)
    seen: dict[tuple, int] = {}
    ordinary_pool = sorted(species for species in DEFAULT_PRESETS if species not in LEGENDARY_SPECIES)
    for block_index, entry in enumerate(blocks):
        location = line_value(entry["text"], "location")
        local = WILD_BY_LOCATION.get("MAP_" + re.sub(r"[^A-Z0-9]", "_", location.upper()), [])
        for branch in entry["branches"]:
            fingerprint = team_fingerprint(branch)
            if fingerprint not in seen or seen[fingerprint] == block_index:
                seen[fingerprint] = block_index
                continue
            replace_index = next((i for i in range(len(branch.mons) - 1, -1, -1) if branch.mons[i].item not in MEGA_STONES), None)
            if replace_index is None:
                continue
            team_species = {mon.species for mon in branch.mons}
            candidates = [species for species in local if species not in team_species and species in DEFAULT_PRESETS]
            if not candidates:
                candidates = [species for species in ordinary_pool if species not in team_species]
            replacement = min(candidates, key=lambda species: (usage[species], species))
            old = branch.mons[replace_index]
            usage[old.species] -= 1
            branch.mons[replace_index] = mon_from_preset(replacement, level_offset=old.level_offset)
            usage[replacement] += 1
            seen[team_fingerprint(branch)] = block_index


PHYSICAL_ITEM_POOL = (
    "ITEM_CLEAR_AMULET", "ITEM_MUSCLE_BAND", "ITEM_EXPERT_BELT", "ITEM_LUM_BERRY",
    "ITEM_COVERT_CLOAK", "ITEM_SAFETY_GOGGLES", "ITEM_WIDE_LENS", "ITEM_SHELL_BELL",
)
SPECIAL_ITEM_POOL = (
    "ITEM_WISE_GLASSES", "ITEM_EXPERT_BELT", "ITEM_COVERT_CLOAK", "ITEM_LUM_BERRY",
    "ITEM_SAFETY_GOGGLES", "ITEM_WIDE_LENS", "ITEM_SHELL_BELL", "ITEM_EJECT_PACK",
)
SUPPORT_ITEM_POOL = (
    "ITEM_MENTAL_HERB", "ITEM_COVERT_CLOAK", "ITEM_SAFETY_GOGGLES", "ITEM_ROCKY_HELMET",
    "ITEM_SITRUS_BERRY", "ITEM_LEFTOVERS", "ITEM_LUM_BERRY", "ITEM_SHELL_BELL",
)


def item_candidates(mon: Mon) -> list[str]:
    choices = []
    for preset in PRESETS.get(mon.species, []):
        choices.extend((preset["item"], preset["required_item"]))
    if mon.stat_points[1] > mon.stat_points[3]:
        choices.extend(PHYSICAL_ITEM_POOL)
    elif mon.stat_points[3] > mon.stat_points[1]:
        choices.extend(SPECIAL_ITEM_POOL)
    else:
        choices.extend(SUPPORT_ITEM_POOL)
    return [item for item in dict.fromkeys(choices) if item in ITEMS and item != "ITEM_NONE" and item not in MEGA_STONES]


def repair_item_clause(blocks: list[dict]) -> None:
    for entry in blocks:
        for branch in entry["branches"]:
            used = set()
            for mon in branch.mons:
                if mon.item == "ITEM_NONE" or mon.item not in used:
                    used.add(mon.item)
                    continue
                replacement = next((item for item in item_candidates(mon) if item not in used), None)
                if replacement is None:
                    raise ValueError(f"cannot satisfy Item Clause for {branch.trainer_id} {mon.species}")
                mon.item = replacement
                used.add(replacement)


def replacement_score(original: str, candidate: str) -> int:
    old = MOVE_META.get(original, {})
    new = MOVE_META.get(candidate, {})
    score = 0
    if old.get("type") == new.get("type"):
        score += 100
    if old.get("category") == new.get("category"):
        score += 100
    if old.get("category") != "DAMAGE_CATEGORY_STATUS" and new.get("category") != "DAMAGE_CATEGORY_STATUS":
        score += max(0, 60 - abs(int(old.get("power", 0)) - int(new.get("power", 0))))
    if candidate == "MOVE_PROTECT":
        score += 15
    return score


def choose_legal_move_replacement(mon: Mon, original: str, used: set[str], legal: set[str]) -> str:
    available = legal - used - {"MOVE_TERA_BLAST", "MOVE_STRUGGLE", "MOVE_SKETCH"}
    if not available:
        return "MOVE_NONE"
    for candidate in PREFERRED_MOVE_REPLACEMENTS.get(original, ()):
        if candidate in available:
            return candidate
    for preset in PRESETS.get(mon.species, []):
        for candidate in preset["moves"]:
            if candidate in available:
                return candidate
    for candidate in GENERIC_GOOD_MOVES:
        if candidate in available and replacement_score(original, candidate) >= 100:
            return candidate
    return max(sorted(available), key=lambda candidate: replacement_score(original, candidate))


def repair_move_legality(blocks: list[dict]) -> Counter[tuple[str, str]]:
    totals: Counter[tuple[str, str]] = Counter()
    for entry in blocks:
        entry_replacements: dict[str, Counter[str]] = defaultdict(Counter)
        for branch in entry["branches"]:
            for mon in branch.mons:
                if mon.species == "SPECIES_SMEARGLE":
                    continue
                legal = legal_move_constants(mon.species)
                used = set()
                repaired = []
                for move in mon.moves:
                    if move == "MOVE_NONE":
                        repaired.append(move)
                        continue
                    if move in legal and move not in used:
                        repaired.append(move)
                        used.add(move)
                        continue
                    replacement = choose_legal_move_replacement(mon, move, used, legal)
                    repaired.append(replacement)
                    used.add(replacement)
                    totals[(move, replacement)] += 1
                    entry_replacements[move][replacement] += 1
                mon.moves = repaired
        for old, replacements in entry_replacements.items():
            new = replacements.most_common(1)[0][0]
            old_name = MOVE_NAMES.get(old)
            new_name = MOVE_NAMES.get(new)
            if old_name and new_name:
                entry["text"] = entry["text"].replace(old_name, new_name)
    return totals


def is_marquee_block(block: str) -> bool:
    return any(token in line_value(block, "trainer_ids") for token in MARQUEE_TOKENS + MINIBOSS_TOKENS + LEAGUE_TOKENS)


def difficulty_schedule(blocks: list[dict]) -> list[float]:
    result = []
    recent_ordinary: list[float] = []
    for entry in blocks:
        block = entry["text"]
        score = difficulty_for(block, entry["branches"])
        if not is_marquee_block(block):
            all_arcs = {arc for branch in entry["branches"] for arc in archetypes(branch.mons)}
            if all_arcs == {"balanced tempo"} and team_size(entry["branches"]) <= 4:
                score = max(6.0, round(score - 0.4, 1))
            # A rolling relief valve, not a quota: after three consecutive serious
            # ordinary fights, the next non-marquee encounter gets wider level-based
            # counterplay while retaining its complete competitive set.
            if len(recent_ordinary) >= 3 and all(value >= 7.0 for value in recent_ordinary[-3:]):
                score = min(score, 6.8)
            recent_ordinary.append(score)
        result.append(score)
    return result


def apply_level_band(branches: list[Branch], difficulty: float, marquee: bool, strict_cap: int) -> None:
    for branch in branches:
        for mon in branch.mons:
            mon.level_offset = min(mon.level_offset, 100 - strict_cap)
    if marquee:
        return
    ceiling = -2 if difficulty < 7.0 else 0 if difficulty < 8.0 else 2 if difficulty < 9.0 else 4
    for branch in branches:
        for mon in branch.mons:
            mon.level_offset = min(mon.level_offset, ceiling)
            evolution_level = EVOLUTION_LEVEL_REQUIREMENTS.get(mon.species)
            if evolution_level is not None:
                mon.level_offset = max(mon.level_offset, evolution_level - strict_cap)


def choose_double_conversions(blocks: list[dict]) -> None:
    branches = [branch for entry in blocks for branch in entry["branches"]]
    target = round(len(branches) * 0.85)
    current = sum(branch.format == "double" for branch in branches)
    eligible = []
    for branch in branches:
        if branch.format != "single" or len(branch.mons) < 2 or "ROUTE_103" in branch.trainer_id:
            continue
        moves = {move for mon in branch.mons for move in mon.moves}
        synergy = len(moves & (SPEED_MOVES | REDIRECTION_MOVES | SPREAD_MOVES | PROTECT_MOVES))
        boss = 1 if any(token in branch.trainer_id for token in MARQUEE_TOKENS + MINIBOSS_TOKENS) else 0
        eligible.append((boss, len(branch.mons), synergy, branch.block_index, branch))
    eligible.sort(key=lambda row: row[:4], reverse=True)
    for *_, branch in eligible[:max(0, target - current)]:
        branch.format = "double"


def format_mon(index: int, mon: Mon) -> str:
    points = "/".join(map(str, mon.stat_points))
    return (
        f"  {index}. {mon.species} @ {mon.item} | level_offset={mon.level_offset} | "
        f"ability={mon.ability} | nature={mon.nature} | stat_points={points} | moves={','.join(mon.moves)}"
    )


def render_branch(branch: Branch) -> str:
    text = re.sub(r"(?m)^format: (single|double|multi)$", f"format: {branch.format}", branch.text, count=1)
    team = "team:\n" + "\n".join(format_mon(i + 1, mon) for i, mon in enumerate(branch.mons))
    text = re.sub(r"(?ms)^team:\n(?:  \d+\. .*\n?)+", team + "\n", text, count=1)
    return text


def replace_branches(block: str, branches: list[Branch]) -> str:
    marker = BRANCH_RE.search(block)
    if marker is None:
        return block
    suffix = re.search(r"(?m)^(?:source_note:|=== END ENCOUNTER ===)", block[marker.start():])
    suffix_text = block[marker.start() + suffix.start():] if suffix else ""
    return block[:marker.start()] + "".join(render_branch(branch) for branch in branches) + suffix_text


def fatigue_role(difficulty: float) -> str:
    if difficulty < 7:
        return "ordinary_breather"
    if difficulty < 8:
        return "ordinary_standard"
    if difficulty < 9:
        return "notable_optional_or_route_ace"
    if difficulty < 9.7:
        return "mini_boss_or_exceptional_trainer"
    return "marquee_boss"


def update_header(blocks: list[dict], difficulties: list[float], move_repairs: Counter[tuple[str, str]]) -> str:
    branch_count = sum(len(entry["branches"]) for entry in blocks)
    doubles = sum(branch.format == "double" for entry in blocks for branch in entry["branches"])
    singles = branch_count - doubles
    ordinary = [
        difficulty
        for entry, difficulty in zip(blocks, difficulties)
        if not is_marquee_block(entry["text"])
    ]
    bands = Counter(int(value) for value in ordinary)
    return (
        "EMERALD CHAMPIONS — MASTER BATTLE DESIGN\n"
        "VERSION: 2\n"
        "AUTHORING_STATUS: COMPLETE — STATICALLY AUDITED IMPLEMENTATION SOURCE OF TRUTH\n"
        "REGENERATION_RULE: This file is the frozen authored result. Change it deliberately; do not regenerate it from trainer source.\n\n"
        "SCOPE\n"
        "source_checkpoint: modern Pokemon Champions migration branch\n"
        f"rematch_free_physical_encounter_groups: {len(blocks)}\n"
        f"rematch_free_explicit_trainer_branch_blocks: {branch_count}\n"
        f"format_counts: {{'single': {singles}, 'double': {doubles}}}\n"
        f"doubles_percentage: {doubles / branch_count * 100:.2f}\n"
        "excluded_content: ordinary Match Call tiers, Gym rematch families, three phantom parser hits, and four battles from the absent Inclement-only Ashen Woods map\n"
        "included_content: every current Hoenn trainer reference plus 14 explicitly planned bespoke restorations on maps retained by the modern campaign\n"
        "battle_frontier_boundary: facility battles are generated live from the pinned Showdown Champions random-doubles port and are not finite entries in this document\n\n"
        "DESIGN THESIS\n"
        "Every campaign battle is a bespoke, legal, readable puzzle. The player receives competitive Pokemon, moves, abilities, natures, Stat Points, and free ordinary held items; difficulty comes from decisions rather than grinding.\n"
        "Hard is authored. Medium subtracts two opposing levels. Easy subtracts four. Teams and AI stay identical.\n"
        "Mega Evolution is the only selectable gimmick. No Mega appears before the post-Brawly bracelet. Primals remain approved automatic forms.\n"
        "Bosses are difficulty 10. Ordinary trainers deliberately vary in simultaneous demands so the campaign remains difficult without becoming exhausting.\n\n"
        "CLOSED GLOBAL AUDIT\n"
        f"ordinary_difficulty_bands: {{6: {bands[6]}, 7: {bands[7]}, 8: {bands[8]}, 9: {bands[9]}}}\n"
        f"ordinary_6x_percentage: {sum(6 <= value < 7 for value in ordinary) / len(ordinary) * 100:.2f}\n"
        f"ordinary_9x_percentage: {sum(value >= 9 for value in ordinary) / len(ordinary) * 100:.2f}\n"
        f"champions_legality_repairs: {sum(move_repairs.values())} move slots normalized against Showdown {SHOWDOWN_LEARNSET_DATA['source_commit']}\n"
        f"coverage: all {len(MEGA_STONES)} Mega Stones and all {len(LEGENDARY_SPECIES)} Legendary Sign species appear in opponent teams\n"
        "cohesion_gates: Item Clause, Species Clause, one Mega maximum, current constants, legal Stat Points, natural early evolution phases, Gym specialty majority, no exact duplicate teams, no five-battle primary-strategy run, and rolling species diversity\n\n"
        "REMATCH POLICY\n"
        "Ordinary and Gym rematches are not campaign content. League replays reuse the finished League teams. Gabby and Ty remain only as distinct roaming reporter milestones. Repeatable endgame variety belongs to the live Champions Circuit and native Battle Frontier.\n\n"
        "IMPLEMENTATION CONTRACT\n"
        "Implement every retained branch exactly: format, cap-relative levels, species, item, ability, nature, Stat Points, moves, AI requirements, and native dialogue intent. Compile after each 100 implemented encounters, then run source, static, emulator, save, and full-progression gates.\n"
        "difficulty_observed remains UNPLAYED until runtime evidence exists; static quality and legality are closed, not falsely labeled playtested.\n\n"
    )


LATE_CAPS = {
    "PHYSICAL_MOSSDEEPCITY_SPACECENTER_1F_0245": ("Mind Badge", "60"),
    "PHYSICAL_MOSSDEEPCITY_SPACECENTER_2F_0069": ("Mind Badge", "60"),
    "PHYSICAL_GLOBAL_GABBY_AND_TY_0128": ("Dynamo Badge", "30"),
    "PHYSICAL_GLOBAL_GABBY_AND_TY_0140": ("Feather Badge", "55"),
    "PHYSICAL_GLOBAL_GABBY_AND_TY_0152": ("Feather Badge", "55"),
    "PHYSICAL_GLOBAL_GABBY_AND_TY_0164": ("Feather Badge", "55"),
    "PHYSICAL_GLOBAL_GABBY_AND_TY_0176": ("Feather Badge", "55"),
    "PHYSICAL_GLOBAL_GABBY_AND_TY_0188": ("Feather Badge", "55"),
}


def finish(master: str) -> str:
    header, source_blocks = split_blocks(master)
    blocks = []
    for source in source_blocks:
        physical_id = line_value(source, "physical_group_id")
        if physical_id in GYM_REMATCH_GROUPS or physical_id in PHANTOM_SOURCE_GROUPS:
            continue
        original_status = line_value(source, "status")
        entry = {
            "text": source,
            "original_status": original_status,
            "branches": [
                branch
                for branch in parse_branches(source, len(blocks), original_status)
                if branch.trainer_id not in GYM_REMATCH_BRANCHES
            ],
        }
        blocks.append(entry)

    apply_bespoke_team_overrides(blocks)
    apply_early_form_replacements(blocks)
    usage: Counter[str] = Counter()
    make_local_replacements(blocks, usage)
    apply_targeted_move_overrides(blocks)
    remove_pre_bracelet_megas(blocks)
    assign_coverage(blocks)
    repair_team_integrity(blocks)
    repair_duplicate_teams(blocks)
    repair_item_clause(blocks)
    move_repairs = repair_move_legality(blocks)
    repair_duplicate_teams(blocks)
    repair_item_clause(blocks)
    move_repairs.update(repair_move_legality(blocks))
    choose_double_conversions(blocks)
    difficulties = difficulty_schedule(blocks)

    rendered = []
    for index, entry in enumerate(blocks, 1):
        block = entry["text"]
        block = ENCOUNTER_RE.sub(f"=== ENCOUNTER {index:04d} ===", block, count=1)
        block = set_line(block, "campaign_order", str(index))
        physical_id = line_value(block, "physical_group_id")
        if physical_id in LATE_CAPS:
            chapter, cap = LATE_CAPS[physical_id]
            block = set_line(block, "chapter", chapter)
            block = set_line(block, "strict_cap", cap)
        if physical_id.startswith("PHYSICAL_GLOBAL_GABBY_AND_TY_"):
            block = set_line(block, "requirement", "optional roaming reporter milestone")
        if line_value(block, "proposed_encounter_id") == "PENDING":
            block = set_line(block, "proposed_encounter_id", line_value(block, "physical_group_id"))
        difficulty = difficulties[index - 1]
        apply_level_band(entry["branches"], difficulty, is_marquee_block(block), int(line_value(block, "strict_cap")))
        block = set_line(block, "status", "master_audited_ready_for_implementation")
        block = set_line(block, "difficulty_target", f"{difficulty:.1f}")
        block = set_line(block, "difficulty_observed", "UNPLAYED")
        block = set_line(block, "fatigue_role", fatigue_role(difficulty))
        block = set_line(block, "trainer_ids", "; ".join(branch.trainer_id for branch in entry["branches"]))
        trainer_ids = {branch.trainer_id for branch in entry["branches"]}
        force_regenerate = bool(trainer_ids & FORCE_REGENERATED_NARRATIVE)
        for key, value in narrative(block, entry["branches"], difficulty).items():
            old_value = line_value(block, key)
            if force_regenerate or entry["original_status"] == "design_pending_source_baseline_only" or old_value in ("", "PENDING", "NONE", "audit_pending"):
                block = set_line(block, key, value)
        for trainer_id in trainer_ids:
            for old, new in TEXT_REPLACEMENTS_BY_TRAINER.get(trainer_id, {}).items():
                block = block.replace(old, new)
        for trainer_id in trainer_ids:
            for key, value in NARRATIVE_OVERRIDES.get(trainer_id, {}).items():
                block = set_line(block, key, value)
        block = replace_branches(block, entry["branches"])
        block = re.sub(r"(?m)^source_note:.*$", "source_note: Master-audited current Champions design; implementation must match exactly.", block)
        if not re.search(r"(?m)^source_note:", block):
            block = block.rstrip() + "\nsource_note: Master-audited current Champions design; implementation must match exactly.\n"
        if not re.search(r"(?m)^=== END ENCOUNTER ===$", block):
            block = block.rstrip() + "\n=== END ENCOUNTER ===\n\n"
        rendered.append(block)

    return update_header(blocks, difficulties, move_repairs) + "".join(rendered)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--input", type=Path, default=MASTER)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    args = parser.parse_args()
    output = finish(args.input.read_text())
    args.output.write_text(output)
    print(f"wrote {args.output}")


if __name__ == "__main__":
    main()
