#!/usr/bin/env python3
"""Reject trainer sets whose authored data contradicts their executable plan."""

from __future__ import annotations

import os
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PARTIES = Path(os.environ.get("EC_TRAINERS_PARTY", ROOT / "src/data/trainers.party"))

# No campaign team runs Trick Room and Tailwind together any more; the last
# four were resolved on 2026-09-02. Any new Trick Room + Tailwind party must
# be reviewed before joining this set.
APPROVED_DUAL_SPEED: set[str] = set()

LOWERS_ATTACK = {
    "NATURE_BOLD",
    "NATURE_MODEST",
    "NATURE_CALM",
    "NATURE_TIMID",
}
LOWERS_SP_ATTACK = {
    "NATURE_ADAMANT",
    "NATURE_IMPISH",
    "NATURE_CAREFUL",
    "NATURE_JOLLY",
}
ALTERNATE_DAMAGE_MOVES = {
    # These moves do not use the user's ordinary Attack or Sp. Atk stat, so
    # they must not make an otherwise support-only set fail the nature check.
    "MOVE_BIDE",
    "MOVE_BODY_PRESS",
    "MOVE_COUNTER",
    "MOVE_ENDEAVOR",
    "MOVE_FINAL_GAMBIT",
    "MOVE_FOUL_PLAY",
    "MOVE_METAL_BURST",
    "MOVE_MIRROR_COAT",
    "MOVE_NIGHT_SHADE",
    "MOVE_RUINATION",
    "MOVE_SEISMIC_TOSS",
    "MOVE_SUPER_FANG",
}
REQUIRED_SMART_AI = {
    "TRAINER_EDGAR",
    "TRAINER_CAROLINE",
}
# "Assumptions" is the AI_FLAG_ASSUMPTIONS composite and supplies Assume Stab,
# Assume Status Moves and Weigh Ability Prediction in one flag, which is how every
# campaign trainer now declares them.
SMART_AI_BASELINE = {
    "Basic Trainer",
    "Hp Aware",
    "Smart Mon Choices",
    "Assumptions",
}
SUN_SOURCES = {
    "ABILITY_DROUGHT",
    "ABILITY_ORICHALCUM_PULSE",
    "MOVE_SUNNY_DAY",
}
CHARGED_BY_SUN = {"MOVE_SOLAR_BEAM", "MOVE_SOLAR_BLADE"}
EXPLOSION_MOVES = {"MOVE_EXPLOSION", "MOVE_SELF_DESTRUCT", "MOVE_MISTY_EXPLOSION"}
CHARGE_MOVES = {
    "MOVE_SOLAR_BEAM",
    "MOVE_SOLAR_BLADE",
    "MOVE_SKY_ATTACK",
    "MOVE_GEOMANCY",
    "MOVE_METEOR_BEAM",
    "MOVE_ELECTRO_SHOT",
    "MOVE_SKULL_BASH",
    "MOVE_FREEZE_SHOCK",
    "MOVE_ICE_BURN",
}
SELF_LOWERING_MOVES = {
    "MOVE_CLOSE_COMBAT",
    "MOVE_SUPERPOWER",
    "MOVE_OVERHEAT",
    "MOVE_DRACO_METEOR",
    "MOVE_LEAF_STORM",
    "MOVE_PSYCHO_BOOST",
    "MOVE_V_CREATE",
    "MOVE_HAMMER_ARM",
    "MOVE_ICE_HAMMER",
    "MOVE_SHELL_SMASH",
    "MOVE_HEADLONG_RUSH",
    "MOVE_ARMOR_CANNON",
    "MOVE_MAKE_IT_RAIN",
}
SCREEN_MOVES = {"MOVE_REFLECT", "MOVE_LIGHT_SCREEN", "MOVE_AURORA_VEIL"}
TEAM_FIELD_ITEM_REQUIREMENTS = {
    "ITEM_DAMP_ROCK": {"MOVE_RAIN_DANCE", "ABILITY_DRIZZLE"},
    "ITEM_HEAT_ROCK": {"MOVE_SUNNY_DAY", "ABILITY_DROUGHT", "ABILITY_ORICHALCUM_PULSE"},
    "ITEM_ICY_ROCK": {"MOVE_SNOWSCAPE", "MOVE_HAIL", "ABILITY_SNOW_WARNING"},
    "ITEM_SMOOTH_ROCK": {"MOVE_SANDSTORM", "ABILITY_SAND_STREAM"},
    "ITEM_ELECTRIC_SEED": {"MOVE_ELECTRIC_TERRAIN", "ABILITY_ELECTRIC_SURGE"},
    "ITEM_GRASSY_SEED": {"MOVE_GRASSY_TERRAIN", "ABILITY_GRASSY_SURGE"},
    "ITEM_MISTY_SEED": {"MOVE_MISTY_TERRAIN", "ABILITY_MISTY_SURGE"},
    "ITEM_PSYCHIC_SEED": {"MOVE_PSYCHIC_TERRAIN", "ABILITY_PSYCHIC_SURGE"},
}
PHYSICAL_SETUP_MOVES = {
    "MOVE_SWORDS_DANCE",
    "MOVE_BULK_UP",
    "MOVE_DRAGON_DANCE",
    "MOVE_COIL",
    "MOVE_BELLY_DRUM",
    "MOVE_VICTORY_DANCE",
    "MOVE_HONE_CLAWS",
    "MOVE_HOWL",
}
SPECIAL_SETUP_MOVES = {
    "MOVE_CALM_MIND",
    "MOVE_NASTY_PLOT",
    "MOVE_QUIVER_DANCE",
    "MOVE_TAIL_GLOW",
    "MOVE_TAKE_HEART",
}
SINGLES_DEAD_PARTNER_ABILITIES = {
    "ABILITY_TELEPATHY",
    "ABILITY_FRIEND_GUARD",
    "ABILITY_BATTERY",
    "ABILITY_SYMBIOSIS",
    "ABILITY_COSTAR",
    "ABILITY_RECEIVER",
    "ABILITY_POWER_OF_ALCHEMY",
    "ABILITY_HEALER",
}

# Weather/terrain-sensitive Abilities and moves are only authored when the
# executable party can create their field.  Map WEATHER_SUNNY is ordinary
# overworld lighting, not battle sunlight; none of the campaign trainer maps
# currently starts rain, sun, sand, or snow in battle.
FIELD_ABILITY_REQUIREMENTS = {
    "ABILITY_SWIFT_SWIM": {"ABILITY_DRIZZLE", "ABILITY_PRIMORDIAL_SEA", "MOVE_RAIN_DANCE"},
    "ABILITY_RAIN_DISH": {"ABILITY_DRIZZLE", "ABILITY_PRIMORDIAL_SEA", "MOVE_RAIN_DANCE"},
    "ABILITY_HYDRATION": {"ABILITY_DRIZZLE", "ABILITY_PRIMORDIAL_SEA", "MOVE_RAIN_DANCE"},
    "ABILITY_CHLOROPHYLL": {
        "ABILITY_DROUGHT", "ABILITY_DESOLATE_LAND", "ABILITY_ORICHALCUM_PULSE", "MOVE_SUNNY_DAY",
    },
    "ABILITY_SOLAR_POWER": {
        "ABILITY_DROUGHT", "ABILITY_DESOLATE_LAND", "ABILITY_ORICHALCUM_PULSE", "MOVE_SUNNY_DAY",
    },
    "ABILITY_FLOWER_GIFT": {
        "ABILITY_DROUGHT", "ABILITY_DESOLATE_LAND", "ABILITY_ORICHALCUM_PULSE", "MOVE_SUNNY_DAY",
    },
    "ABILITY_SAND_RUSH": {"ABILITY_SAND_STREAM", "MOVE_SANDSTORM"},
    "ABILITY_SAND_VEIL": {"ABILITY_SAND_STREAM", "MOVE_SANDSTORM"},
    "ABILITY_SLUSH_RUSH": {"ABILITY_SNOW_WARNING", "MOVE_SNOWSCAPE", "MOVE_HAIL"},
    "ABILITY_SNOW_CLOAK": {"ABILITY_SNOW_WARNING", "MOVE_SNOWSCAPE", "MOVE_HAIL"},
    "ABILITY_ICE_BODY": {"ABILITY_SNOW_WARNING", "MOVE_SNOWSCAPE", "MOVE_HAIL"},
    "ABILITY_SURGE_SURFER": {
        "ABILITY_ELECTRIC_SURGE", "ABILITY_HADRON_ENGINE", "MOVE_ELECTRIC_TERRAIN",
    },
}
FIELD_MOVE_REQUIREMENTS = {
    "MOVE_WEATHER_BALL": {
        "ABILITY_DRIZZLE", "ABILITY_PRIMORDIAL_SEA", "MOVE_RAIN_DANCE",
        "ABILITY_DROUGHT", "ABILITY_DESOLATE_LAND", "ABILITY_ORICHALCUM_PULSE", "MOVE_SUNNY_DAY",
        "ABILITY_SAND_STREAM", "MOVE_SANDSTORM",
        "ABILITY_SNOW_WARNING", "MOVE_SNOWSCAPE", "MOVE_HAIL",
    },
    "MOVE_RISING_VOLTAGE": {
        "ABILITY_ELECTRIC_SURGE", "ABILITY_HADRON_ENGINE", "MOVE_ELECTRIC_TERRAIN",
    },
    "MOVE_EXPANDING_FORCE": {"ABILITY_PSYCHIC_SURGE", "MOVE_PSYCHIC_TERRAIN"},
    "MOVE_GRASSY_GLIDE": {"ABILITY_GRASSY_SURGE", "MOVE_GRASSY_TERRAIN"},
    "MOVE_MISTY_EXPLOSION": {"ABILITY_MISTY_SURGE", "MOVE_MISTY_TERRAIN"},
    "MOVE_TERRAIN_PULSE": {
        "ABILITY_ELECTRIC_SURGE", "ABILITY_HADRON_ENGINE", "MOVE_ELECTRIC_TERRAIN",
        "ABILITY_PSYCHIC_SURGE", "MOVE_PSYCHIC_TERRAIN",
        "ABILITY_GRASSY_SURGE", "MOVE_GRASSY_TERRAIN",
        "ABILITY_MISTY_SURGE", "MOVE_MISTY_TERRAIN",
    },
}

# This is a true physical multi battle: Courtney's lead Drought Ninetales is
# beside Maxie's lead Chlorophyll Victreebel.  The individual trainer block is
# not independently reachable, so its partner field is executable support.
APPROVED_EXTERNAL_FIELD_SUPPORT = {
    ("TRAINER_MAXIE_MOSSDEEP", "ABILITY_CHLOROPHYLL"),
}

# Drake deliberately changes from Koraidon's sun phase into Rayquaza's Air
# Lock phase.  Every other authored weather source plus suppressor is rejected.
APPROVED_WEATHER_SUPPRESSION = {"TRAINER_DRAKE"}
WEATHER_SOURCES = {
    source for sources in FIELD_ABILITY_REQUIREMENTS.values() for source in sources
    if "TERRAIN" not in source and source != "ABILITY_HADRON_ENGINE"
}
WEATHER_SUPPRESSORS = {"ABILITY_CLOUD_NINE", "ABILITY_AIR_LOCK"}

ACROBATICS_TRIGGER_ITEMS = {
    "ITEM_FLYING_GEM",
    "ITEM_FOCUS_SASH",
    "ITEM_GRASSY_SEED",
    "ITEM_POWER_HERB",
    "ITEM_PSYCHIC_SEED",
    "ITEM_SITRUS_BERRY",
    "ITEM_WEAKNESS_POLICY",
    "ITEM_WHITE_HERB",
}
UNBURDEN_TRIGGER_ITEMS = ACROBATICS_TRIGGER_ITEMS | {"ITEM_BUG_GEM"}
RELIABLE_POISON_MOVES = {
    "MOVE_BANEFUL_BUNKER",
    "MOVE_POISON_GAS",
    "MOVE_POISON_POWDER",
    "MOVE_TOXIC",
    "MOVE_TOXIC_SPIKES",
}
ORB_BENEFIT_ABILITIES = {
    "ABILITY_FLARE_BOOST",
    "ABILITY_GUTS",
    "ABILITY_MAGIC_GUARD",
    "ABILITY_MARVEL_SCALE",
    "ABILITY_POISON_HEAL",
    "ABILITY_QUICK_FEET",
    "ABILITY_TOXIC_BOOST",
}
ORB_BENEFIT_MOVES = {"MOVE_FACADE", "MOVE_PSYCHO_SHIFT", "MOVE_SWITCHEROO", "MOVE_TRICK"}
ABILITY_MOVE_FLAGS = {
    "ABILITY_BLITZ_BOXER": "punchingMove",
    "ABILITY_IRON_FIST": "punchingMove",
    "ABILITY_LIQUID_VOICE": "soundMove",
    "ABILITY_MEGA_LAUNCHER": "pulseMove",
    "ABILITY_PUNK_ROCK": "soundMove",
    "ABILITY_SHARPNESS": "slicingMove",
    "ABILITY_STRONG_JAW": "bitingMove",
}
NORMAL_CONVERSION_ABILITIES = {
    "ABILITY_AERILATE",
    "ABILITY_GALVANIZE",
    "ABILITY_PIXILATE",
    "ABILITY_REFRIGERATE",
}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def move_categories() -> dict[str, str]:
    text = (ROOT / "src/data/moves_info.h").read_text()
    markers = list(re.finditer(r"(?m)^\s*\[(MOVE_[A-Z0-9_]+)\]\s*=\s*\{", text))
    result: dict[str, str] = {}
    for index, marker in enumerate(markers):
        end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
        body = text[marker.end():end]
        category = re.search(r"\.category\s*=\s*(DAMAGE_CATEGORY_[A-Z]+)", body)
        result[marker.group(1)] = category.group(1) if category else "DAMAGE_CATEGORY_STATUS"
    return result


def sound_moves() -> set[str]:
    text = (ROOT / "src/data/moves_info.h").read_text()
    markers = list(re.finditer(r"(?m)^\s*\[(MOVE_[A-Z0-9_]+)\]\s*=\s*\{", text))
    result = set()
    for index, marker in enumerate(markers):
        end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
        if ".soundMove = TRUE" in text[marker.end():end]:
            result.add(marker.group(1))
    return result


def move_types_and_flags() -> tuple[dict[str, str], dict[str, set[str]], set[str], set[str]]:
    text = (ROOT / "src/data/moves_info.h").read_text()
    markers = list(re.finditer(r"(?m)^\s*\[(MOVE_[A-Z0-9_]+)\]\s*=\s*\{", text))
    types: dict[str, str] = {}
    flags: dict[str, set[str]] = {}
    healing: set[str] = set()
    recoil: set[str] = set()
    for index, marker in enumerate(markers):
        end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
        body = text[marker.end():end]
        move = marker.group(1)
        move_type = re.search(r"\.type\s*=\s*(TYPE_[A-Z]+)", body)
        types[move] = move_type.group(1) if move_type else "TYPE_NORMAL"
        flags[move] = {
            flag for flag in set(ABILITY_MOVE_FLAGS.values())
            if (match := re.search(rf"\.{flag}\s*=\s*([^,\n]+)", body)) is not None
            and match.group(1).strip() not in {"0", "FALSE"}
        }
        if (match := re.search(r"\.healingMove\s*=\s*([^,\n]+)", body)) is not None:
            if match.group(1).strip() not in {"0", "FALSE"}:
                healing.add(move)
        if "recoilPercentage" in body or "EFFECT_MAX_HP_50_RECOIL" in body:
            recoil.add(move)
    return types, flags, healing, recoil


def party_blocks() -> dict[str, str]:
    text = PARTIES.read_text()
    markers = list(re.finditer(r"(?m)^=== (TRAINER_[A-Z0-9_]+) ===$", text))
    return {
        marker.group(1): text[marker.end():markers[index + 1].start() if index + 1 < len(markers) else len(text)]
        for index, marker in enumerate(markers)
    }


def fire_species() -> set[str]:
    result: set[str] = set()
    for path in sorted((ROOT / "src/data/pokemon/species_info").glob("gen_*_families.h")):
        text = path.read_text()
        markers = list(re.finditer(r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{", text))
        for index, marker in enumerate(markers):
            end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
            body = text[marker.end():end]
            types = re.search(r"\.types\s*=\s*MON_TYPES\(([^)]*)\)", body)
            if types is not None and "TYPE_FIRE" in types.group(1):
                result.add(marker.group(1))
    return result


def mon_blocks(party: str) -> list[str]:
    markers = list(re.finditer(r"(?m)^SPECIES_[A-Z0-9_]+(?: @ ITEM_[A-Z0-9_]+)?$", party))
    return [
        party[marker.start():markers[index + 1].start() if index + 1 < len(markers) else len(party)]
        for index, marker in enumerate(markers)
    ]


def field(block: str, name: str) -> str:
    match = re.search(rf"(?m)^{re.escape(name)}: (.+)$", block)
    return match.group(1).strip() if match else ""


def main() -> None:
    categories = move_categories()
    sound_move_set = sound_moves()
    move_types, move_flags, healing_moves, recoil_moves = move_types_and_flags()
    parties = party_blocks()
    fire_type_species = fire_species()
    failures: list[str] = []
    mon_count = 0
    all_protect_singles: list[str] = []

    dual_speed = {
        trainer for trainer, party in parties.items()
        if "MOVE_TRICK_ROOM" in party and "MOVE_TAILWIND" in party
    }
    if dual_speed != APPROVED_DUAL_SPEED:
        failures.append(
            "unreviewed Trick Room + Tailwind parties: "
            f"added={sorted(dual_speed - APPROVED_DUAL_SPEED)} "
            f"removed={sorted(APPROVED_DUAL_SPEED - dual_speed)}"
        )

    for trainer, party in parties.items():
        has_sun = any(source in party for source in SUN_SOURCES)
        party_tokens = set(re.findall(r"\b(?:MOVE|ABILITY)_[A-Z0-9_]+\b", party))
        is_single = "Double Battle: No" in party
        ai_flags = field(party, "AI").split(" / ")
        if trainer in REQUIRED_SMART_AI and not SMART_AI_BASELINE.issubset(ai_flags):
            failures.append(
                f"{trainer}: late six-Pokemon Victory Road battle lacks the reviewed smart AI baseline"
            )
        if (
            "MOVE_BEAT_UP" in party_tokens
            and "ABILITY_JUSTIFIED" in party_tokens
            and "Attacks Partner" in ai_flags
        ):
            failures.append(
                f"{trainer}: Attacks Partner disables the proven Beat Up/Justified support-combo scorer"
            )
        if not is_single and party_tokens & EXPLOSION_MOVES and "Will Suicide" not in ai_flags:
            failures.append(
                f"{trainer}: doubles Explosion plan lacks the Will Suicide AI flag"
            )
        if (
            party_tokens & WEATHER_SOURCES
            and party_tokens & WEATHER_SUPPRESSORS
            and trainer not in APPROVED_WEATHER_SUPPRESSION
        ):
            failures.append(
                f"{trainer}: authored weather is suppressed by "
                f"{sorted(party_tokens & WEATHER_SUPPRESSORS)}"
            )
        if is_single:
            dead_partner_abilities = sorted(
                ability for ability in SINGLES_DEAD_PARTNER_ABILITIES if ability in party
            )
            if dead_partner_abilities:
                failures.append(
                    f"{trainer}: partner-only Abilities are dead in its reachable singles branch "
                    f"{dead_partner_abilities}"
                )
        plus_minus_count = len(re.findall(r"(?m)^Ability: ABILITY_(?:PLUS|MINUS)$", party))
        if plus_minus_count == 1:
            failures.append(
                f"{trainer}: Plus/Minus has no compatible party partner"
            )
        if (
            "ABILITY_COMMANDER" in party
            and "SPECIES_DONDOZO" not in party
            and "ITEM_TATSUGIRINITE" not in party
        ):
            failures.append(
                f"{trainer}: Commander has neither Dondozo nor a Tatsugirinite Mega exit"
            )
        for mon in mon_blocks(party):
            mon_count += 1
            species = re.match(r"(SPECIES_[A-Z0-9_]+)", mon).group(1)
            nature = field(mon, "Nature")
            evs = field(mon, "EVs")
            points = {
                stat: int(value)
                for value, stat in re.findall(r"(\d+) (HP|Atk|Def|SpA|SpD|Spe)", evs)
            }
            moves = re.findall(r"(?m)^- (MOVE_[A-Z0-9_]+)$", mon)
            level = int(field(mon, "Level"))
            physical = [move for move in moves if categories.get(move) == "DAMAGE_CATEGORY_PHYSICAL"]
            special = [move for move in moves if categories.get(move) == "DAMAGE_CATEGORY_SPECIAL"]
            own_physical = [move for move in physical if move not in ALTERNATE_DAMAGE_MOVES]
            own_special = [move for move in special if move not in ALTERNATE_DAMAGE_MOVES]

            # Attract is matchup-dead against genderless and same-gender foes,
            # so it cannot carry a deterministic campaign puzzle slot.
            if "MOVE_ATTRACT" in moves:
                failures.append(f"{trainer}/{species}: Attract is not a reliable authored battle plan")

            physical_setup = PHYSICAL_SETUP_MOVES.intersection(moves)
            special_setup = SPECIAL_SETUP_MOVES.intersection(moves)
            if physical_setup and special and not physical:
                failures.append(
                    f"{trainer}/{species}: {sorted(physical_setup)} boosts no authored physical move"
                )
            if special_setup and physical and not special:
                failures.append(
                    f"{trainer}/{species}: {sorted(special_setup)} boosts no authored special move"
                )

            # Schooling cannot create School Form below level 20.  A legal
            # Ability assignment is therefore still a dead strategy when an
            # early trainer authors ordinary Wishiwashi as its ace.
            if species in {"SPECIES_WISHIWASHI", "SPECIES_WISHIWASHI_SOLO"} and level < 20:
                failures.append(
                    f"{trainer}/{species}: Schooling cannot activate at level {level}"
                )

            if own_physical and not own_special and nature in LOWERS_ATTACK:
                failures.append(f"{trainer}/{species}: {nature} lowers its only authored attack category")
            if own_special and not own_physical and nature in LOWERS_SP_ATTACK:
                failures.append(f"{trainer}/{species}: {nature} lowers its only authored attack category")

            item_match = re.search(r"@ (ITEM_[A-Z0-9_]+)", mon.splitlines()[0])
            item = item_match.group(1) if item_match else "ITEM_NONE"
            ability = field(mon, "Ability")

            field_sources = FIELD_ABILITY_REQUIREMENTS.get(ability)
            if (
                field_sources is not None
                and not party_tokens & field_sources
                and (trainer, ability) not in APPROVED_EXTERNAL_FIELD_SUPPORT
            ):
                failures.append(
                    f"{trainer}/{species}: {ability} has no executable party or approved multi-battle field source"
                )
            for move in moves:
                required_sources = FIELD_MOVE_REQUIREMENTS.get(move)
                if required_sources is not None and not party_tokens & required_sources:
                    failures.append(
                        f"{trainer}/{species}: {move} has no executable field source"
                    )

            if "MOVE_ACROBATICS" in moves and item not in ACROBATICS_TRIGGER_ITEMS:
                failures.append(
                    f"{trainer}/{species}: Acrobatics cannot shed {item} through its authored plan"
                )
            if ability == "ABILITY_UNBURDEN" and item not in UNBURDEN_TRIGGER_ITEMS:
                failures.append(
                    f"{trainer}/{species}: Unburden cannot shed {item} through its authored plan"
                )
            if "MOVE_VENOSHOCK" in moves and not party_tokens & RELIABLE_POISON_MOVES:
                failures.append(
                    f"{trainer}/{species}: Venoshock has no reliable authored poison source"
                )

            required_flag = ABILITY_MOVE_FLAGS.get(ability)
            if required_flag is not None and not any(required_flag in move_flags.get(move, set()) for move in moves):
                failures.append(
                    f"{trainer}/{species}: {ability} amplifies none of its authored moves"
                )
            if ability in {"ABILITY_RECKLESS", "ABILITY_ROCK_HEAD"} and not set(moves) & recoil_moves:
                failures.append(
                    f"{trainer}/{species}: {ability} has no recoil move"
                )
            if ability in NORMAL_CONVERSION_ABILITIES and not any(move_types.get(move) == "TYPE_NORMAL" for move in moves):
                failures.append(
                    f"{trainer}/{species}: {ability} has no Normal-type move to convert"
                )
            if ability == "ABILITY_TRIAGE" and not set(moves) & healing_moves:
                failures.append(f"{trainer}/{species}: Triage has no healing move")
            if ability == "ABILITY_PRANKSTER" and not any(categories.get(move) == "DAMAGE_CATEGORY_STATUS" for move in moves):
                failures.append(f"{trainer}/{species}: Prankster has no status move")

            if item in {"ITEM_FLAME_ORB", "ITEM_TOXIC_ORB"}:
                if ability not in ORB_BENEFIT_ABILITIES and not set(moves) & ORB_BENEFIT_MOVES:
                    failures.append(
                        f"{trainer}/{species}: {item} only self-damages this authored set"
                    )
            if item == "ITEM_FLAME_ORB" and species in fire_type_species:
                failures.append(f"{trainer}/{species}: Fire typing prevents Flame Orb from activating")
            if item == "ITEM_ASSAULT_VEST" and any(categories.get(move) == "DAMAGE_CATEGORY_STATUS" for move in moves):
                failures.append(f"{trainer}/{species}: Assault Vest blocks an authored status move")
            if ability == "ABILITY_DEFIANT" and item == "ITEM_CLEAR_AMULET":
                failures.append(f"{trainer}/{species}: Clear Amulet prevents Defiant's authored trigger")
            if ability == "ABILITY_STALL" and item == "ITEM_IRON_BALL":
                failures.append(f"{trainer}/{species}: Iron Ball adds no speed effect beyond Stall")
            category_item = {
                "ITEM_CHOICE_BAND": "DAMAGE_CATEGORY_PHYSICAL",
                "ITEM_MUSCLE_BAND": "DAMAGE_CATEGORY_PHYSICAL",
                "ITEM_CHOICE_SPECS": "DAMAGE_CATEGORY_SPECIAL",
                "ITEM_WISE_GLASSES": "DAMAGE_CATEGORY_SPECIAL",
            }.get(item)
            if category_item is not None and not any(categories.get(move) == category_item for move in moves):
                failures.append(
                    f"{trainer}/{species}: {item} amplifies none of its authored moves"
                )
            gem = re.fullmatch(r"ITEM_([A-Z]+)_GEM", item)
            if gem is not None and not any(move_types.get(move) == f"TYPE_{gem.group(1)}" for move in moves):
                failures.append(f"{trainer}/{species}: {item} has no matching move type")

            required_field_sources = TEAM_FIELD_ITEM_REQUIREMENTS.get(item)
            if required_field_sources is not None and not (party_tokens & required_field_sources):
                failures.append(
                    f"{trainer}/{species}: {item} has no authored field setter"
                )
            if item == "ITEM_LIGHT_CLAY" and not (set(moves) & SCREEN_MOVES):
                failures.append(f"{trainer}/{species}: Light Clay has no screen move")
            if item == "ITEM_POWER_HERB" and not (set(moves) & CHARGE_MOVES):
                failures.append(f"{trainer}/{species}: Power Herb has no charge move")
            if item == "ITEM_EJECT_PACK" and not (set(moves) & SELF_LOWERING_MOVES):
                failures.append(f"{trainer}/{species}: Eject Pack has no self-lowering move")
            if item == "ITEM_THROAT_SPRAY" and not (set(moves) & sound_move_set):
                failures.append(f"{trainer}/{species}: Throat Spray has no sound move")
            if item == "ITEM_BOOSTER_ENERGY" and field(mon, "Ability") not in {
                "ABILITY_PROTOSYNTHESIS",
                "ABILITY_QUARK_DRIVE",
            }:
                failures.append(f"{trainer}/{species}: Booster Energy cannot activate its Ability")
            unsupported_charge = CHARGED_BY_SUN.intersection(moves)
            if unsupported_charge and not has_sun and item != "ITEM_POWER_HERB":
                failures.append(
                    f"{trainer}/{species}: {sorted(unsupported_charge)} has no Sun or Power Herb"
                )

        if is_single:
            mons = mon_blocks(party)
            if mons and all("MOVE_PROTECT" in mon for mon in mons):
                all_protect_singles.append(trainer)

    require(not failures, f"{len(failures)} trainer runtime-coherence failures:\n" + "\n".join(failures))
    print(
        "PASS: "
        f"{mon_count} trainer Pokemon have coherent attack natures, charge support, "
        "usable Schooling levels, activatable held items, executable fields, and live move-dependent Abilities; "
        f"{len(dual_speed)} reviewed dual-speed parties remain; "
        f"{len(all_protect_singles)} singles parties still put Protect on every member"
    )


if __name__ == "__main__":
    main()
