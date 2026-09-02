#!/usr/bin/env python3
"""Reject an incomplete or internally inconsistent campaign battle master."""

from __future__ import annotations

import argparse
import json
import os
import re
import statistics
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_MASTER = ROOT / "docs" / "emerald_champions_master_battle_design.txt"
ENCOUNTER_RE = re.compile(r"(?m)^=== ENCOUNTER (\d{4}) ===$")
BRANCH_RE = re.compile(r"(?m)^--- BRANCH ([A-Z0-9_]+) ---$")
MON_RE = re.compile(
    r"(?m)^  (\d+)\. (SPECIES_[A-Z0-9_]+) @ (ITEM_[A-Z0-9_]+) \| "
    r"level_offset=(-?\d+) \| ability=(ABILITY_[A-Z0-9_]+) \| "
    r"nature=(NATURE_[A-Z0-9_]+) \| stat_points=([0-9/]+) \| "
    r"moves=(MOVE_[A-Z0-9_]+(?:,MOVE_[A-Z0-9_]+){0,3})$"
)

PLANNED_RESTORE_TRAINERS = {
    "TRAINER_ALANNAH",
    "TRAINER_ARCHIE_SLATEPORT",
    "TRAINER_BUFFEL",
    "TRAINER_COURTNEY_MAGMA_HIDEOUT",
    "TRAINER_COURTNEY_METEOR_FALLS",
    "TRAINER_COURTNEY_MOSSDEEP",
    "TRAINER_CYNTHIA_1",
    "TRAINER_ELMER",
    "TRAINER_GRETA_SLATEPORT",
    "TRAINER_GRUNT_METEOR_FALLS",
    "TRAINER_LEAF_ALTERING_CAVE",
    "TRAINER_LUCY_LAVARIDGE",
    "TRAINER_MARTIN",
    "TRAINER_MAGIKARP_GUY",
    "TRAINER_MATT_MT_PYRE",
    "TRAINER_SPENSER_FORTREE",
    "TRAINER_ROMAN",
    "TRAINER_WALLACE_DOUBLES_LEGENDS",
}

REMATCH_TRAINERS = {
    f"TRAINER_{leader}_{tier}"
    for leader in ("ROXANNE", "BRAWLY", "WATTSON", "FLANNERY", "NORMAN", "WINONA", "JUAN")
    for tier in range(2, 6)
}
REMATCH_TRAINERS.add("TRAINER_CYNTHIA_2")

MARQUEE_TOKENS = (
    "ROXANNE", "BRAWLY", "WATTSON", "FLANNERY", "NORMAN", "WINONA",
    "TATE_AND_LIZA", "JUAN", "SIDNEY", "PHOEBE", "GLACIA", "DRAKE",
    "WALLACE", "MAXIE", "ARCHIE", "STEVEN", "CYNTHIA",
)
MINIBOSS_TOKENS = ("TABITHA", "COURTNEY", "MATT", "SHELLY", "WALLY", "BRENDAN", "MAY_")


def constants(path: str, prefix: str) -> set[str]:
    return set(re.findall(rf"\b{prefix}[A-Z0-9_]+\b", (ROOT / path).read_text()))


SPECIES = constants("include/constants/species.h", "SPECIES_")
ITEMS = constants("include/constants/items.h", "ITEM_")
MOVES = constants("include/constants/moves.h", "MOVE_")
ABILITIES = constants("include/constants/abilities.h", "ABILITY_")
NATURES = constants("include/constants/pokemon.h", "NATURE_")
TRAINERS = constants("include/constants/opponents.h", "TRAINER_")
MOVES_BY_ID = {}
for _move in sorted(MOVES, key=lambda token: (token.count("_"), len(token)), reverse=True):
    MOVES_BY_ID.setdefault(re.sub(r"[^a-z0-9]", "", _move.removeprefix("MOVE_").lower()), _move)
MEGA_STONES = set(re.findall(
    r"ITEM_[A-Z0-9_]+",
    (ROOT / "src" / "data" / "emerald_champions_mega_stones.h").read_text(),
))
SIGN_SPECIES = {
    "SPECIES_" + species
    for species in re.findall(
        r"(?:WILD|OTHER)_SIGN\([^,]+,\s*([A-Z0-9_]+)",
        (ROOT / "src" / "data" / "pokemon" / "legendary_signs.h").read_text(),
    )
}
LEGENDARY_SHOWCASE_ALIASES = {
    # The acquisition root is the base family, while trainer data uses the
    # battle-ready Power Construct form explicitly.  Either form is a real
    # Zygarde showcase; requiring a third base-form copy would be duplication,
    # not additional campaign coverage.
    "SPECIES_ZYGARDE": {
        "SPECIES_ZYGARDE",
        "SPECIES_ZYGARDE_50",
        "SPECIES_ZYGARDE_50_POWER_CONSTRUCT",
        "SPECIES_ZYGARDE_10",
        "SPECIES_ZYGARDE_10_POWER_CONSTRUCT",
        "SPECIES_ZYGARDE_COMPLETE",
        "SPECIES_ZYGARDE_MEGA",
    },
}


def move_categories() -> dict[str, str]:
    """Read the compiled move category used by the campaign battle engine."""
    text = (ROOT / "src" / "data" / "moves_info.h").read_text()
    markers = list(re.finditer(r"(?m)^\s*\[(MOVE_[A-Z0-9_]+)\]\s*=\s*\{", text))
    result = {}
    for index, marker in enumerate(markers):
        body = text[marker.end():markers[index + 1].start() if index + 1 < len(markers) else len(text)]
        category_field = re.search(r"\.category\s*=\s*([^,]+)", body)
        categories = re.findall(r"DAMAGE_CATEGORY_[A-Z]+", category_field.group(1)) if category_field else []
        if categories:
            # Updated-data ternaries put the modern category first. Emerald
            # Champions always builds with the latest move-data generation.
            result[marker.group(1)] = categories[0]
    return result


MOVE_CATEGORIES = move_categories()
CHOICE_ITEMS = {"ITEM_CHOICE_BAND", "ITEM_CHOICE_SPECS", "ITEM_CHOICE_SCARF"}
CHOICE_INCOHERENT_STATUS_MOVES = {
    "MOVE_PROTECT", "MOVE_DETECT", "MOVE_SWORDS_DANCE", "MOVE_DRAGON_DANCE",
    "MOVE_SHIFT_GEAR", "MOVE_CALM_MIND", "MOVE_NASTY_PLOT", "MOVE_SHELL_SMASH",
    "MOVE_BULK_UP", "MOVE_QUIVER_DANCE", "MOVE_IRON_DEFENSE", "MOVE_COTTON_GUARD",
    "MOVE_AGILITY", "MOVE_AUTOTOMIZE", "MOVE_WORK_UP", "MOVE_COIL", "MOVE_BELLY_DRUM",
}
SINGLES_DEAD_ALLY_MOVES = {
    "MOVE_HELPING_HAND",
    "MOVE_DECORATE",
    "MOVE_COACHING",
    "MOVE_ALLY_SWITCH",
    "MOVE_AROMATIC_MIST",
    "MOVE_GEAR_UP",
    "MOVE_HOLD_HANDS",
    "MOVE_HEAL_PULSE",
    "MOVE_SPOTLIGHT",
}
REDUNDANT_STATUS_GROUPS = {
    "sleep": {
        "MOVE_SPORE", "MOVE_SLEEP_POWDER", "MOVE_HYPNOSIS", "MOVE_SING",
        "MOVE_LOVELY_KISS", "MOVE_GRASS_WHISTLE", "MOVE_DARK_VOID",
    },
    "protection": {"MOVE_PROTECT", "MOVE_DETECT"},
    "physical defense boost": {
        "MOVE_IRON_DEFENSE", "MOVE_COTTON_GUARD", "MOVE_ACID_ARMOR",
        "MOVE_DEFENSE_CURL", "MOVE_COSMIC_POWER", "MOVE_STOCKPILE",
    },
}
BERRY_DEPENDENT_ABILITIES = {
    "ABILITY_HARVEST", "ABILITY_RIPEN", "ABILITY_CHEEK_POUCH",
    "ABILITY_CUD_CHEW", "ABILITY_GLUTTONY",
}
DOSSIER_SETUP_MOVES = {
    "MOVE_BELLY_DRUM", "MOVE_BULK_UP", "MOVE_CALM_MIND", "MOVE_COIL",
    "MOVE_DRAGON_DANCE", "MOVE_IRON_DEFENSE", "MOVE_NASTY_PLOT",
    "MOVE_QUIVER_DANCE", "MOVE_SHELL_SMASH", "MOVE_SWORDS_DANCE",
    "MOVE_TAIL_GLOW", "MOVE_TIDY_UP", "MOVE_VICTORY_DANCE",
}
DOSSIER_REDIRECTION_MOVES = {"MOVE_FOLLOW_ME", "MOVE_RAGE_POWDER", "MOVE_SPOTLIGHT"}
DOSSIER_HAZARD_MOVES = {"MOVE_STEALTH_ROCK", "MOVE_SPIKES", "MOVE_TOXIC_SPIKES", "MOVE_STICKY_WEB"}
DOSSIER_SPREAD_MOVES = {
    "MOVE_ROCK_SLIDE", "MOVE_HEAT_WAVE", "MOVE_MUDDY_WATER", "MOVE_HYPER_VOICE",
    "MOVE_DAZZLING_GLEAM", "MOVE_BLIZZARD", "MOVE_EARTHQUAKE", "MOVE_DISCHARGE", "MOVE_SURF",
}
DOSSIER_TACTICAL_MOVES = DOSSIER_SETUP_MOVES | DOSSIER_REDIRECTION_MOVES | DOSSIER_HAZARD_MOVES | {
    "MOVE_TAILWIND", "MOVE_TRICK_ROOM", "MOVE_ICY_WIND", "MOVE_ELECTROWEB", "MOVE_THUNDER_WAVE",
    "MOVE_PERISH_SONG", "MOVE_FAKE_OUT", "MOVE_HELPING_HAND", "MOVE_WIDE_GUARD",
}
DOSSIER_PROTECT_MOVES = {
    "MOVE_PROTECT", "MOVE_DETECT", "MOVE_BANEFUL_BUNKER", "MOVE_KINGS_SHIELD", "MOVE_SPIKY_SHIELD",
}
DOSSIER_ALLY_ONLY_MOVES = {
    "MOVE_FOLLOW_ME", "MOVE_RAGE_POWDER", "MOVE_HELPING_HAND", "MOVE_ALLY_SWITCH",
    "MOVE_COACHING", "MOVE_DECORATE", "MOVE_SPOTLIGHT", "MOVE_AROMATIC_MIST", "MOVE_HOLD_HANDS",
}
PROTECTED_DOSSIER_TRAINER_TOKENS = (
    "TRAINER_ARCHIE_SLATEPORT", "TRAINER_ALYSSA", "TRAINER_DALE", "TRAINER_WATTSON",
    "TRAINER_WALLACE", "TRAINER_VICTOR", "TRAINER_VICTORIA", "TRAINER_VIVI", "TRAINER_VICKY",
)
SOURCE_VERIFIED_DIALOGUE_STATUS = (
    "implemented native source dialogue; literal width verified; bespoke team-specific rewrite not claimed"
)


def move_names() -> dict[str, str]:
    text = (ROOT / "src" / "data" / "moves_info.h").read_text()
    markers = list(re.finditer(r"(?m)^\s*\[(MOVE_[A-Z0-9_]+)\]\s*=\s*\{", text))
    result = {}
    for index, marker in enumerate(markers):
        body = text[marker.end():markers[index + 1].start() if index + 1 < len(markers) else len(text)]
        name = re.search(r'\.name\s*=\s*COMPOUND_STRING\("([^"]+)"\)', body)
        if name:
            result[marker.group(1)] = name.group(1)
    return result


MOVE_NAMES = move_names()


def display_constant(value: str) -> str:
    return value.split("_", 1)[-1].replace("_", " ").title()


def display_move(move: str) -> str:
    return MOVE_NAMES.get(move, display_constant(move))


def dossier_archetypes(mons: list[tuple[str, str, str, list[str]]], single: bool) -> list[str]:
    moves = {move for _species, _item, _ability, mon_moves in mons for move in mon_moves}
    abilities = {ability for _species, _item, ability, _moves in mons}
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
    if not single and moves & DOSSIER_REDIRECTION_MOVES:
        result.append("redirection")
    if moves & DOSSIER_HAZARD_MOVES:
        result.append("hazard pressure")
    if moves & DOSSIER_SETUP_MOVES:
        result.append("setup")
    if "MOVE_PERISH_SONG" in moves:
        result.append("Perish Song")
    if not single and moves & DOSSIER_SPREAD_MOVES:
        result.append("spread pressure")
    return result or (["direct tempo"] if single else ["balanced tempo"])


def dossier_key_move(mon: tuple[str, str, str, list[str]], single: bool) -> str:
    moves = mon[3]
    excluded = DOSSIER_PROTECT_MOVES | (DOSSIER_ALLY_ONLY_MOVES if single else set())
    return next(
        (move for move in moves if move in DOSSIER_TACTICAL_MOVES and move not in excluded),
        next((move for move in moves if move not in excluded and move != "MOVE_NONE"), moves[0]),
    )


def exact_loadout_theme(location: str, fmt: str, mons: list[tuple[str, str, str, list[str]]]) -> str:
    arcs = ", ".join(dossier_archetypes(mons, fmt == "single"))
    if fmt == "single":
        opening = f"This {location.replace('_', ' ')} single battle emphasizes {arcs}."
    else:
        opening = f"This {location.replace('_', ' ')} encounter is a {arcs} puzzle."
    facts = []
    for species, item, ability, moves in mons:
        move = dossier_key_move((species, item, ability, moves), fmt == "single")
        facts.append(
            f"{display_constant(species)} carries {display_constant(item)} with {display_constant(ability)} "
            f"and lists {display_move(move)} among its public options"
        )
    return opening + " " + "; ".join(facts) + "."


def normalized_dossier_fields(
    location: str,
    fmt: str,
    mons: list[tuple[str, str, str, list[str]]],
    difficulty: float,
) -> dict[str, str]:
    lead = mons[0]
    ace = mons[-1]
    lead_name = display_constant(lead[0])
    ace_name = display_constant(ace[0])
    single = fmt == "single"
    lead_move = display_move(dossier_key_move(lead, single))
    ace_move = display_move(dossier_key_move(ace, single))
    middle_names = [display_constant(mon[0]) for mon in mons[1:-1]]
    middle = ", ".join(middle_names) if middle_names else "the reserve"
    arcs = dossier_archetypes(mons, single)
    arc = ", ".join(arcs)
    result = {"theme_and_tempo": exact_loadout_theme(location, fmt, mons)}
    if single:
        result.update({
            "primary_question": (
                f"Can the player manage {lead_name}'s {lead_move} opening, adapt through {middle}, "
                f"and preserve an answer for {ace_name}'s {ace_move} finish?"
            ),
            "intentional_weakness": (
                f"The player can contest {lead_name}'s speed or setup plan, scout public item commitments, "
                f"use Protect, status, and pivots to expose attacks, and preserve the best defensive matchup "
                f"for {ace_name}; no partner-only tactic or one exact counter is required."
            ),
            "first_loss_lesson": (
                f"Identify what {lead_name}'s {lead_move} commits on the first exchange, then save the answer "
                f"to {ace_name}'s {ace_move} instead of spending it on {middle}."
            ),
            "strongest_part": (
                f"{lead_name}'s {lead_move} and {ace_name}'s {ace_move} create a clear opening-to-finish sequence "
                "while the middle slots change the attack axis without pretending this singles fight has a partner board."
            ),
            "weakest_link": (
                f"Once the player checks {lead_name}'s opening, the team must earn each later exchange through "
                f"ordinary singles positioning; that visible seam keeps the {arc} plan fair at difficulty {difficulty:.1f}."
            ),
            "reservation_status": (
                f"spends the {lead_name} to {ace_name} {arc} singles sequence here; "
                "no partner-only interaction is claimed"
            ),
        })
        return result
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
    result.update({
        "primary_question": (
            f"Can the player read {lead_name}'s {lead_move} opening, solve the {arc} board, "
            f"and still preserve an answer for {ace_name}'s {ace_move} finish?"
        ),
        "intentional_weakness": (
            f"The broad answers are to {counters}. The player can also pressure {lead_name} before the plan "
            f"stabilizes or isolate {ace_name}; no single species or exact move order is required."
        ),
        "first_loss_lesson": (
            f"Decide whether {lead_name} is damage or infrastructure, then preserve the answer that best denies "
            f"{ace_name}'s {ace_move} rather than spending it on {middle}."
        ),
        "strongest_part": (
            f"{lead_name}'s {lead_move} creates a readable handoff through {middle} into {ace_name}'s {ace_move}, "
            "so the team has one identity without becoming one scripted solution."
        ),
        "weakest_link": (
            f"If the player breaks the {arcs[0]} layer or removes {lead_name} early, the remaining members must win "
            f"through ordinary positioning; that intentional seam keeps difficulty {difficulty:.1f} honest."
        ),
        "reservation_status": (
            f"spends the {lead_name} plus {ace_name} {arc} pairing here; checked against campaign species, "
            "Mega, legendary, and rolling-strategy ledgers"
        ),
    })
    return result


def is_protected_dossier(block: str) -> bool:
    trainers = line_value(block, "trainer_ids")
    return (
        any(token in trainers for token in PROTECTED_DOSSIER_TRAINER_TOKENS)
        or "MossdeepCity_SpaceCenter" in line_value(block, "location")
    )


def evolution_level_requirements() -> dict[str, int]:
    result = {}
    for path in sorted((ROOT / "src" / "data" / "pokemon" / "species_info").glob("gen_*_families.h")):
        text = path.read_text()
        for level, species in re.findall(r"\{EVO_LEVEL,\s*(\d+),\s*(SPECIES_[A-Z0-9_]+)", text):
            result[species] = min(result.get(species, 1000), int(level))
    return result


EVOLUTION_LEVEL_REQUIREMENTS = evolution_level_requirements()

SHOWDOWN_DATA = json.loads((ROOT / "docs" / "showdown_champions_learnsets.json").read_text())
SHOWDOWN_LEARNSETS = {species: set(moves) for species, moves in SHOWDOWN_DATA["learnsets"].items()}
MOVE_ACCESS_REVIEW = json.loads((ROOT / "docs" / "emerald_champions_move_access_review.json").read_text())
REVIEWED_MOVE_EXTENSIONS: dict[str, set[str]] = {}
for _assignment in MOVE_ACCESS_REVIEW["assignments"]:
    if _assignment["action"] == "retain_inclement_custom_extension":
        REVIEWED_MOVE_EXTENSIONS.setdefault(_assignment["species"], set()).add(_assignment["move"])
SHOWDOWN_FORM_SUFFIXES = (
    "50powerconstruct", "10powerconstruct", "powerconstruct", "curly", "droopy", "stretchy",
    "incarnate", "ordinary", "aria", "amped", "midday", "male", "female", "natural",
    "west", "east", "normal", "altered", "land", "sky", "small", "large", "super",
    "average", "antique", "phony", "rubycream", "marine", "autumn", "roaming",
    "debutante", "kabuki",
)


def showdown_id_for_species(species: str) -> str | None:
    showdown_id = re.sub(r"[^a-z0-9]", "", species.removeprefix("SPECIES_").lower())
    if showdown_id in SHOWDOWN_LEARNSETS:
        return showdown_id
    for suffix in SHOWDOWN_FORM_SUFFIXES:
        if showdown_id.endswith(suffix) and showdown_id[:-len(suffix)] in SHOWDOWN_LEARNSETS:
            return showdown_id[:-len(suffix)]
    return None


def pinned_legal_moves(species: str) -> set[str]:
    showdown_id = showdown_id_for_species(species)
    if showdown_id is None:
        return set(REVIEWED_MOVE_EXTENSIONS.get(species, set()))
    official = {
        move
        for move_id in SHOWDOWN_LEARNSETS[showdown_id]
        if (move := MOVES_BY_ID.get(move_id)) is not None
    }
    return official | REVIEWED_MOVE_EXTENSIONS.get(species, set())


GYM_TYPES = {
    "RustboroCity_Gym": "TYPE_ROCK",
    "DewfordTown_Gym": "TYPE_FIGHTING",
    "MauvilleCity_Gym": "TYPE_ELECTRIC",
    "LavaridgeTown_Gym_1F": "TYPE_FIRE",
    "LavaridgeTown_Gym_B1F": "TYPE_FIRE",
    "PetalburgCity_Gym": "TYPE_NORMAL",
    "FortreeCity_Gym": "TYPE_FLYING",
    "MossdeepCity_Gym": "TYPE_PSYCHIC",
    "SootopolisCity_Gym_1F": "TYPE_WATER",
}


def species_types() -> dict[str, tuple[str, ...]]:
    paths = sorted((ROOT / "src" / "data" / "pokemon" / "species_info").glob("gen_*_families.h"))
    macros = {}
    for path in paths:
        text = path.read_text()
        for name, first, second in re.findall(
            r"#define\s+([A-Z0-9_]+)\s+MON_TYPES\((TYPE_[A-Z0-9_]+)(?:,\s*(TYPE_[A-Z0-9_]+))?\)", text
        ):
            macros.setdefault(name, tuple(value for value in (first, second) if value))
    result = {}
    for path in paths:
        text = path.read_text()
        markers = list(re.finditer(r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{", text))
        for index, marker in enumerate(markers):
            body = text[marker.end():markers[index + 1].start() if index + 1 < len(markers) else len(text)]
            direct = re.search(r"\.types\s*=\s*MON_TYPES\((TYPE_[A-Z0-9_]+)(?:,\s*(TYPE_[A-Z0-9_]+))?\)", body)
            if direct:
                result[marker.group(1)] = tuple(value for value in direct.groups() if value)
                continue
            macro = re.search(r"\.types\s*=\s*([A-Z0-9_]+)", body)
            if macro and macro.group(1) in macros:
                result[marker.group(1)] = macros[macro.group(1)]
    aliases = dict(re.findall(
        r"(?m)^\s*(SPECIES_[A-Z0-9_]+)\s*=\s*(SPECIES_[A-Z0-9_]+)\s*,",
        (ROOT / "include" / "constants" / "species.h").read_text(),
    ))
    for alias, target in aliases.items():
        if target in result:
            result[alias] = result[target]
    result.update({
        "SPECIES_KIRLIA": ("TYPE_PSYCHIC", "TYPE_FAIRY"),
        "SPECIES_MELOETTA": ("TYPE_NORMAL", "TYPE_PSYCHIC"),
        "SPECIES_TORNADUS": ("TYPE_FLYING",),
        "SPECIES_GASTRODON": ("TYPE_WATER", "TYPE_GROUND"),
        "SPECIES_FURFROU": ("TYPE_NORMAL",),
        "SPECIES_WIGGLYTUFF": ("TYPE_NORMAL", "TYPE_FAIRY"),
        "SPECIES_SILVALLY": ("TYPE_NORMAL",),
        "SPECIES_GARDEVOIR": ("TYPE_PSYCHIC", "TYPE_FAIRY"),
        "SPECIES_MINIOR": ("TYPE_ROCK", "TYPE_FLYING"),
    })
    return result


SPECIES_TYPES = species_types()


def line_value(text: str, key: str) -> str:
    match = re.search(rf"(?m)^{re.escape(key)}: (.*)$", text)
    return match.group(1) if match else ""


def blocks(text: str) -> list[str]:
    marks = list(ENCOUNTER_RE.finditer(text))
    return [text[m.start():marks[i + 1].start() if i + 1 < len(marks) else len(text)] for i, m in enumerate(marks)]


EARLIEST_REACHABLE_CAPS = {
    **{group: 14 for group in (
        "PHYSICAL_ROUTE115_0067", "PHYSICAL_ROUTE115_0089", "PHYSICAL_ROUTE115_0111", "PHYSICAL_ROUTE115_0136",
        "BATTLE_020_ROUTE_116_JOEY", "BATTLE_021_ROUTE_116_JOSE", "BATTLE_022_ROUTE_116_KAREN",
        "BATTLE_023_ROUTE_116_CLARK_JOHNSON", "BATTLE_024_ROUTE_116_DEVAN",
        "BATTLE_025_ROUTE_116_SARAH_DAWSON", "BATTLE_026_ROUTE_116_JANICE_JERRY",
    )},
    **{group: 30 for group in (
        "BATTLE_069_ROUTE_117_ANNA_AND_MEG", "BATTLE_070_ROUTE_117_ISAAC", "BATTLE_071_ROUTE_117_DYLAN",
        "BATTLE_072_ROUTE_117_MARIA", "BATTLE_073_ROUTE_117_DEREK", "BATTLE_074_ROUTE_117_AISHA_MELINA_BRANDI",
        "BATTLE_075_ROUTE_117_LYDIA", "BATTLE_077_ROUTE_111_VICTOR", "BATTLE_078_ROUTE_111_VICTORIA",
        "BATTLE_079_ROUTE_111_VIVI", "BATTLE_080_ROUTE_111_VICKY", "BATTLE_137_ROUTE_111_HAYDEN",
        "BATTLE_138_ROUTE_111_BIANCA", "BATTLE_139_ROUTE_111_TYRON", "BATTLE_140_ROUTE_111_CELINA",
        "PHYSICAL_ROUTE118_0193", "PHYSICAL_ROUTE118_0220", "PHYSICAL_ROUTE118_0225", "PHYSICAL_ROUTE118_0257",
    )},
    **{group: 40 for group in (
        "PHYSICAL_GLOBAL_GABBY_AND_TY_0128", "BATTLE_124_MT_CHIMNEY_SHELBY", "BATTLE_125_MT_CHIMNEY_MELISSA",
        "BATTLE_126_MT_CHIMNEY_SHEILA", "BATTLE_127_MT_CHIMNEY_SHIRLEY", "BATTLE_128_MT_CHIMNEY_SAWYER",
        "BATTLE_135_ROUTE_111_WILTON", "BATTLE_136_ROUTE_111_BROOKE", "BATTLE_141_ROUTE_111_CELIA",
        "BATTLE_142_ROUTE_111_BRYAN", "BATTLE_143_ROUTE_111_BRANDEN",
    )},
}


def campaign_chronology_errors(groups: list[str]) -> list[str]:
    """Protect the live Hoenn story spine from documentation-order drift."""
    errors: list[str] = []
    physical_ids = [line_value(block, "physical_group_id") for block in groups]
    positions = {physical_id: index for index, physical_id in enumerate(physical_ids)}

    for block in groups:
        physical_id = line_value(block, "physical_group_id")
        expected_cap = EARLIEST_REACHABLE_CAPS.get(physical_id)
        if expected_cap is not None and line_value(block, "strict_cap") != str(expected_cap):
            errors.append(f"campaign chronology: {physical_id} must use earliest reachable cap {expected_cap}")

    def require_sequence(label: str, ordered_ids: tuple[str, ...]) -> None:
        missing = [physical_id for physical_id in ordered_ids if physical_id not in positions]
        if missing:
            errors.append(f"campaign chronology {label}: missing {', '.join(missing)}")
            return
        actual = [positions[physical_id] for physical_id in ordered_ids]
        if actual != sorted(actual):
            errors.append(f"campaign chronology {label}: milestone order is wrong")

    def require_location_before(first: str, second: str) -> None:
        first_positions = [
            index for index, block in enumerate(groups)
            if line_value(block, "location").startswith(first)
        ]
        second_positions = [
            index for index, block in enumerate(groups)
            if line_value(block, "location").startswith(second)
        ]
        if not first_positions or not second_positions:
            errors.append(f"campaign chronology: missing location group {first} or {second}")
        elif max(first_positions) >= min(second_positions):
            errors.append(f"campaign chronology: {first} must finish before {second} begins")

    # These are the finite trainer milestones in the exact order enforced by
    # the live map scripts. The Sootopolis crisis and Rayquaza awakening contain
    # no Trainer battle block, so their position is represented by Archie before
    # the Sootopolis Gym and Juan after its students.
    require_sequence("late-story spine", (
        "PHYSICAL_MTPYRE_SUMMIT_0619",
        "PHYSICAL_MAGMAHIDEOUT_4F_0056",
        "PHYSICAL_AQUAHIDEOUT_B2F_0029",
        "PHYSICAL_MOSSDEEPCITY_GYM_0056",
        "PHYSICAL_MOSSDEEPCITY_SPACECENTER_2F_0269",
        "PHYSICAL_SEAFLOORCAVERN_ROOM9_0071",
        "PHYSICAL_SOOTOPOLISCITY_GYM_1F_0088",
        "PHYSICAL_EVERGRANDECITY_SIDNEYSROOM_0053",
        "PHYSICAL_EVERGRANDECITY_PHOEBESROOM_0047",
        "PHYSICAL_EVERGRANDECITY_GLACIASROOM_0047",
        "PHYSICAL_EVERGRANDECITY_DRAKESROOM_0048",
        "PHYSICAL_EVERGRANDECITY_CHAMPIONSROOM_0048",
        "PHYSICAL_CAVE_OF_ORIGIN_DIANCIES_ROOM_WALLACE_EXHIBITION",
    ))
    require_location_before("MtPyre_", "MagmaHideout_")
    require_location_before("MagmaHideout_", "AquaHideout_")
    require_location_before("AquaHideout_", "MossdeepCity_Gym")
    require_location_before("MossdeepCity_Gym", "MossdeepCity_SpaceCenter_")
    require_location_before("MossdeepCity_SpaceCenter_", "SeafloorCavern_")
    require_location_before("SeafloorCavern_", "SootopolisCity_Gym")

    wallace_id = "PHYSICAL_EVERGRANDECITY_CHAMPIONSROOM_0048"
    if wallace_id in positions:
        wallace_position = positions[wallace_id]
        for index, block in enumerate(groups):
            if line_value(block, "chapter").startswith("Postgame") and index <= wallace_position:
                errors.append(
                    "campaign chronology: postgame encounter appears before the League Champion"
                )
                break

    return errors


def current_campaign_trainer_refs() -> set[str]:
    paths = [p for p in (ROOT / "data" / "maps").rglob("*.inc") if "_Frlg" not in str(p)]
    paths += [p for p in (ROOT / "data" / "scripts").rglob("*.inc") if p.name != "trainers_frlg.inc"]
    paths.append(ROOT / "data" / "event_scripts.s")
    result = set()
    for path in paths:
        for line in path.read_text(errors="ignore").splitlines():
            if "trainerbattle" in line or "multi_2_vs_2" in line:
                result.update(re.findall(r"\bTRAINER_[A-Z0-9_]+\b", line))
    return result


def source_verified_trainer_dialogue() -> set[str]:
    """Return Trainers whose runtime battle command points only to defined text labels."""
    paths = [
        path for path in (ROOT / "data").rglob("*")
        if path.is_file()
        and path.suffix in (".inc", ".s")
        and "_Frlg" not in str(path)
        and "frlg" not in path.name.lower()
    ]
    labels = set()
    battle_lines = []
    for path in paths:
        text = path.read_text(errors="ignore")
        labels.update(re.findall(r"(?m)^([A-Za-z_][A-Za-z0-9_]*):{1,2}\s*$", text))
        battle_lines.extend(
            line for line in text.splitlines()
            if "trainerbattle" in line or "multi_2_vs_2" in line
        )
    verified = set()
    for line in battle_lines:
        trainers = re.findall(r"\bTRAINER_[A-Z0-9_]+\b", line)
        text_labels = [token for token in re.findall(r"\b[A-Za-z_][A-Za-z0-9_]*\b", line) if "Text" in token]
        if text_labels and all(label in labels for label in text_labels):
            verified.update(trainers)
    return verified


def audit(path: Path) -> tuple[list[str], list[str]]:
    text = path.read_text()
    errors: list[str] = []
    notes: list[str] = []
    groups = blocks(text)

    if not groups:
        return ["no encounter blocks"], notes
    numbers = [int(ENCOUNTER_RE.search(block).group(1)) for block in groups]
    if numbers != list(range(1, len(groups) + 1)):
        errors.append("encounter numbers are not contiguous from 1")
    campaign_orders = [line_value(block, "campaign_order") for block in groups]
    if campaign_orders != [str(i) for i in range(1, len(groups) + 1)]:
        errors.append("campaign_order values do not match encounter order")
    atlas_ordinals = [line_value(block, "atlas_ordinal") for block in groups]
    if atlas_ordinals != [str(i) for i in range(1, len(groups) + 1)]:
        errors.append("atlas_ordinal values do not match canonical encounter order")
    errors.extend(campaign_chronology_errors(groups))
    if text.count("=== END ENCOUNTER ===") != len(groups):
        errors.append("every encounter must have exactly one END ENCOUNTER marker")

    encounter_body = "\n".join(groups)
    for forbidden in ("PENDING", "audit_pending", "design_pending_source_baseline_only", "ITEM_MILOTICITE", "level="):
        if forbidden in encounter_body:
            errors.append(f"unfinished or stale token remains: {forbidden}")

    all_trainers: list[str] = []
    all_species: list[str] = []
    all_items: list[str] = []
    fingerprint_encounters: dict[tuple, set[int]] = {}
    formats = Counter()
    difficulties: list[float] = []
    ordinary_difficulties: list[float] = []
    team_sizes: Counter[int] = Counter()
    encounter_species_sets: list[set[str]] = []
    primary_strategies: list[str] = []
    expected_party_sizes: dict[str, int] = {}
    expected_multi_trainers: set[str] = set()
    source_dialogue = source_verified_trainer_dialogue()
    exact_dossier_blocks = 0
    exact_dossier_facts = 0
    source_verified_dialogue_blocks = 0

    strategy_patterns = (
        ("Trick Room", r"MOVE_TRICK_ROOM"),
        ("Tailwind", r"MOVE_TAILWIND"),
        ("rain", r"ABILITY_DRIZZLE|MOVE_RAIN_DANCE"),
        ("sun", r"ABILITY_DROUGHT|MOVE_SUNNY_DAY"),
        ("sand", r"ABILITY_SAND_STREAM|MOVE_SANDSTORM"),
        ("snow", r"ABILITY_SNOW_WARNING|MOVE_SNOWSCAPE"),
        ("redirection", r"MOVE_FOLLOW_ME|MOVE_RAGE_POWDER"),
        ("Perish Song", r"MOVE_PERISH_SONG"),
        ("setup", r"MOVE_SWORDS_DANCE|MOVE_CALM_MIND|MOVE_DRAGON_DANCE|MOVE_QUIVER_DANCE|MOVE_SHELL_SMASH|MOVE_BULK_UP"),
        ("spread pressure", r"MOVE_ROCK_SLIDE|MOVE_HEAT_WAVE|MOVE_SURF|MOVE_EARTHQUAKE|MOVE_DAZZLING_GLEAM|MOVE_HYPER_VOICE"),
    )

    required_fields = (
        "physical_group_id", "proposed_encounter_id", "campaign_order", "chapter",
        "strict_cap", "location", "requirement", "status", "quality_target",
        "difficulty_target", "difficulty_observed", "fatigue_role", "primary_question",
        "theme_and_tempo", "intentional_weakness", "first_loss_lesson", "strongest_part",
        "weakest_link", "competitive_references", "dialogue_status", "reservation_status",
        "trainer_ids",
    )

    for encounter_index, block in enumerate(groups, 1):
        encounter_species_sets.append(set(re.findall(r"(?m)^  \d+\. (SPECIES_[A-Z0-9_]+)", block)))
        # A team can deliberately layer weather, speed control, setup, and
        # spread pressure.  Treat that combination as its strategy signature;
        # collapsing every such team to the first matching token made distinct
        # League battles look like six copies of "Tailwind."
        signature = tuple(name for name, pattern in strategy_patterns if re.search(pattern, block))
        primary_strategies.append(" + ".join(signature) if signature else "balanced tempo")
        for field in required_fields:
            if not line_value(block, field):
                errors.append(f"encounter {encounter_index}: missing {field}")
        if line_value(block, "status") != "master_audited_ready_for_implementation":
            errors.append(f"encounter {encounter_index}: status is not implementation-ready")
        cap = line_value(block, "strict_cap")
        if not cap.isdigit() or not 1 <= int(cap) <= 100:
            errors.append(f"encounter {encounter_index}: invalid strict cap {cap!r}")
        try:
            difficulty = float(line_value(block, "difficulty_target"))
        except ValueError:
            errors.append(f"encounter {encounter_index}: invalid difficulty")
            difficulty = 0.0
        difficulties.append(difficulty)
        if any(token in line_value(block, "trainer_ids") for token in MARQUEE_TOKENS) and difficulty != 10.0:
            errors.append(f"encounter {encounter_index}: marquee boss difficulty must be 10.0, found {difficulty:.1f}")
        trainer_line = set(line_value(block, "trainer_ids").split("; "))
        location = line_value(block, "location")
        marks = list(BRANCH_RE.finditer(block))
        branch_trainers = set()
        branch_formats = set()
        dossier_branches: list[tuple[str, list[tuple[str, str, str, list[str]]]]] = []
        if not marks:
            errors.append(f"encounter {encounter_index}: no branches")
        for branch_index, mark in enumerate(marks):
            branch = block[mark.start():marks[branch_index + 1].start() if branch_index + 1 < len(marks) else len(block)]
            trainer = line_value(branch, "trainer_id")
            branch_trainers.add(trainer)
            all_trainers.append(trainer)
            if trainer not in TRAINERS and trainer not in PLANNED_RESTORE_TRAINERS:
                errors.append(f"encounter {encounter_index}: unknown trainer {trainer}")
            if trainer in REMATCH_TRAINERS:
                errors.append(f"encounter {encounter_index}: excluded Gym rematch {trainer}")
            fmt = line_value(branch, "format")
            if fmt not in ("single", "double", "multi"):
                errors.append(f"encounter {encounter_index}: invalid format {fmt!r}")
            branch_formats.add(fmt)
            formats[fmt] += 1
            mons = list(MON_RE.finditer(branch))
            dossier_branches.append((
                fmt,
                [
                    (mon.group(2), mon.group(3), mon.group(5), mon.group(8).split(","))
                    for mon in mons
                ],
            ))
            team_sizes[len(mons)] += 1
            expected_party_sizes[trainer] = len(mons)
            if fmt == "multi":
                expected_multi_trainers.add(trainer)
            if not 1 <= len(mons) <= 6:
                errors.append(f"encounter {encounter_index}/{trainer}: invalid team size {len(mons)}")
            if fmt in ("double", "multi") and len(mons) < 2:
                errors.append(f"encounter {encounter_index}/{trainer}: doubles team has fewer than two Pokemon")
            species_in_team = []
            items_in_team = []
            fingerprint = []
            for expected_slot, mon in enumerate(mons, 1):
                slot, species, item, level, ability, nature, points, moves_text = mon.groups()
                moves = moves_text.split(",")
                points_list = [int(value) for value in points.split("/")]
                if int(slot) != expected_slot:
                    errors.append(f"encounter {encounter_index}/{trainer}: noncontiguous party slots")
                if species not in SPECIES:
                    errors.append(f"encounter {encounter_index}/{trainer}: unknown species {species}")
                if item not in ITEMS:
                    errors.append(f"encounter {encounter_index}/{trainer}: unknown item {item}")
                if ability not in ABILITIES:
                    errors.append(f"encounter {encounter_index}/{trainer}: unknown ability {ability}")
                if nature not in NATURES:
                    errors.append(f"encounter {encounter_index}/{trainer}: unknown nature {nature}")
                if len(points_list) != 6 or any(value < 0 or value > 32 for value in points_list) or sum(points_list) > 66:
                    errors.append(f"encounter {encounter_index}/{trainer}/{species}: illegal Stat Points {points}")
                bad_moves = set(moves) - MOVES
                if bad_moves:
                    errors.append(f"encounter {encounter_index}/{trainer}/{species}: unknown moves {sorted(bad_moves)}")
                legal_moves = pinned_legal_moves(species)
                if not legal_moves:
                    errors.append(f"encounter {encounter_index}/{trainer}/{species}: no pinned Showdown learnset mapping")
                elif species != "SPECIES_SMEARGLE":
                    illegal = set(moves) - legal_moves - {"MOVE_NONE"}
                    if illegal:
                        errors.append(
                            f"encounter {encounter_index}/{trainer}/{species}: moves outside pinned Champions/mainline learnset {sorted(illegal)}"
                        )
                real_moves = [move for move in moves if move != "MOVE_NONE"]
                if not real_moves or len(real_moves) != len(set(real_moves)):
                    errors.append(f"encounter {encounter_index}/{trainer}/{species}: empty or duplicate moves")
                dead_singles_moves = set(real_moves) & SINGLES_DEAD_ALLY_MOVES
                if fmt == "single" and dead_singles_moves:
                    errors.append(
                        f"encounter {encounter_index}/{trainer}/{species}: ally-only moves have no legal purpose "
                        f"in a singles battle {sorted(dead_singles_moves)}"
                    )
                uncategorized_moves = {move for move in real_moves if move not in MOVE_CATEGORIES}
                if uncategorized_moves:
                    errors.append(
                        f"encounter {encounter_index}/{trainer}/{species}: move category unresolved "
                        f"{sorted(uncategorized_moves)}"
                    )
                status_moves = {
                    move for move in real_moves
                    if MOVE_CATEGORIES.get(move) == "DAMAGE_CATEGORY_STATUS"
                }
                physical_moves = {
                    move for move in real_moves
                    if MOVE_CATEGORIES.get(move) == "DAMAGE_CATEGORY_PHYSICAL"
                }
                special_moves = {
                    move for move in real_moves
                    if MOVE_CATEGORIES.get(move) == "DAMAGE_CATEGORY_SPECIAL"
                }
                if item == "ITEM_ASSAULT_VEST" and status_moves:
                    errors.append(
                        f"encounter {encounter_index}/{trainer}/{species}: Assault Vest blocks status moves "
                        f"{sorted(status_moves)}"
                    )
                if item in CHOICE_ITEMS:
                    incoherent = set(real_moves) & CHOICE_INCOHERENT_STATUS_MOVES
                    if incoherent:
                        errors.append(
                            f"encounter {encounter_index}/{trainer}/{species}: Choice item makes protection/setup "
                            f"nonfunctional {sorted(incoherent)}"
                        )
                for purpose, group in REDUNDANT_STATUS_GROUPS.items():
                    redundant = status_moves & group
                    if len(redundant) > 1:
                        errors.append(
                            f"encounter {encounter_index}/{trainer}/{species}: redundant {purpose} moves "
                            f"{sorted(redundant)}"
                        )
                if points_list[1] and not physical_moves and special_moves:
                    errors.append(
                        f"encounter {encounter_index}/{trainer}/{species}: Attack Stat Points have no physical move"
                    )
                if points_list[3] and not special_moves and physical_moves:
                    errors.append(
                        f"encounter {encounter_index}/{trainer}/{species}: Sp. Atk Stat Points have no special move"
                    )
                if points_list[1] and not points_list[3] and len(special_moves) >= 3 and len(physical_moves) <= 1:
                    errors.append(
                        f"encounter {encounter_index}/{trainer}/{species}: Stat Points oppose a dominant special set"
                    )
                if points_list[3] and not points_list[1] and len(physical_moves) >= 3 and len(special_moves) <= 1:
                    errors.append(
                        f"encounter {encounter_index}/{trainer}/{species}: Stat Points oppose a dominant physical set"
                    )
                if ability in BERRY_DEPENDENT_ABILITIES and not item.endswith("_BERRY"):
                    errors.append(
                        f"encounter {encounter_index}/{trainer}/{species}: {ability} has no held Berry"
                    )
                if ability in {"ABILITY_POISON_HEAL", "ABILITY_TOXIC_BOOST"} and item != "ITEM_TOXIC_ORB":
                    errors.append(
                        f"encounter {encounter_index}/{trainer}/{species}: {ability} has no Toxic Orb"
                    )
                if ability == "ABILITY_FLARE_BOOST" and item != "ITEM_FLAME_ORB":
                    errors.append(
                        f"encounter {encounter_index}/{trainer}/{species}: Flare Boost has no Flame Orb"
                    )
                if ability == "ABILITY_UNBURDEN" and item == "ITEM_CLEAR_AMULET":
                    errors.append(
                        f"encounter {encounter_index}/{trainer}/{species}: Unburden cannot consume Clear Amulet"
                    )
                if not -10 <= int(level) <= 10:
                    errors.append(f"encounter {encounter_index}/{trainer}/{species}: unreasonable level offset {level}")
                if not 1 <= int(cap) + int(level) <= 100:
                    errors.append(f"encounter {encounter_index}/{trainer}/{species}: effective level {int(cap) + int(level)} is outside 1-100")
                evolution_level = EVOLUTION_LEVEL_REQUIREMENTS.get(species)
                if int(cap) <= 45 and evolution_level is not None and int(cap) + int(level) < evolution_level:
                    errors.append(
                        f"encounter {encounter_index}/{trainer}/{species}: appears at level {int(cap) + int(level)} "
                        f"before its level-{evolution_level} evolution"
                    )
                species_in_team.append(species)
                items_in_team.append(item)
                all_species.append(species)
                all_items.append(item)
                fingerprint.append((species, item, ability, nature, tuple(moves)))
            if len(species_in_team) != len(set(species_in_team)):
                errors.append(f"encounter {encounter_index}/{trainer}: duplicate species within party")
            held_items = [item for item in items_in_team if item != "ITEM_NONE"]
            if len(held_items) != len(set(held_items)):
                errors.append(f"encounter {encounter_index}/{trainer}: duplicate held item violates Item Clause")
            if sum(item in MEGA_STONES for item in items_in_team) > 1:
                errors.append(f"encounter {encounter_index}/{trainer}: more than one Mega Stone")
            if int(cap) < 30 and any(item in MEGA_STONES for item in items_in_team):
                errors.append(f"encounter {encounter_index}/{trainer}: Mega appears before the post-Brawly bracelet")
            if location in GYM_TYPES:
                specialty = GYM_TYPES[location]
                unknown_types = [species for species in species_in_team if species not in SPECIES_TYPES]
                if unknown_types:
                    errors.append(f"encounter {encounter_index}/{trainer}: unresolved Gym species types {unknown_types}")
                specialty_count = sum(specialty in SPECIES_TYPES.get(species, ()) for species in species_in_team)
                if specialty_count * 2 < len(species_in_team):
                    errors.append(
                        f"encounter {encounter_index}/{trainer}: only {specialty_count}/{len(species_in_team)} Pokemon match {specialty}"
                    )
            fingerprint_encounters.setdefault(tuple(sorted(fingerprint)), set()).add(encounter_index)
        if branch_trainers != trainer_line:
            errors.append(f"encounter {encounter_index}: trainer_ids field differs from branches")
        requirement = line_value(block, "requirement").lower()
        if re.search(r"\bsingle\b", requirement) and branch_formats != {"single"}:
            errors.append(
                f"encounter {encounter_index}: requirement says single but branch formats are {sorted(branch_formats)}"
            )
        if re.search(r"\bdouble\b", requirement) and not branch_formats <= {"double", "multi"}:
            errors.append(
                f"encounter {encounter_index}: requirement says double but branch formats are {sorted(branch_formats)}"
            )
        if re.search(r"\bmulti\b", requirement) and branch_formats != {"multi"}:
            errors.append(
                f"encounter {encounter_index}: requirement says multi but branch formats are {sorted(branch_formats)}"
            )
        theme = line_value(block, "theme_and_tempo")
        protected_dossier = is_protected_dossier(block)
        if "make its job public" in theme and not protected_dossier:
            errors.append(
                f"encounter {encounter_index}: stale preset-role boilerplate remains in an unprotected dossier"
            )
        if " among its public options" in theme:
            if len(dossier_branches) != 1:
                errors.append(
                    f"encounter {encounter_index}: one-branch loadout prose cannot represent {len(dossier_branches)} branches"
                )
            else:
                fmt, dossier_mons = dossier_branches[0]
                expected_fields = normalized_dossier_fields(location, fmt, dossier_mons, difficulty)
                mismatched_fields = [
                    field for field, expected in expected_fields.items()
                    if line_value(block, field) != expected
                ]
                if mismatched_fields:
                    errors.append(
                        f"encounter {encounter_index}: normalized dossier fields differ from exact branch facts: "
                        f"{mismatched_fields}"
                    )
                exact_dossier_blocks += 1
                exact_dossier_facts += len(dossier_mons) * 4
        dialogue_status = line_value(block, "dialogue_status")
        if (
            dialogue_status == "native intent preserved; converted-format and width gate required at implementation"
            and not protected_dossier
        ):
            errors.append(f"encounter {encounter_index}: dialogue status still claims completed implementation is pending")
        if dialogue_status == SOURCE_VERIFIED_DIALOGUE_STATUS:
            missing_dialogue = sorted(branch_trainers - source_dialogue)
            if missing_dialogue:
                errors.append(
                    f"encounter {encounter_index}: dialogue source claim is unverified for {missing_dialogue}"
                )
            source_verified_dialogue_blocks += 1
        effective_levels = {
            int(cap) + int(offset)
            for offset in re.findall(r"level_offset=(-?\d+)", block)
        }
        for prose_field in ("theme_and_tempo", "weakest_link"):
            claimed_levels = {
                int(value)
                for value in re.findall(r"(?i)\blevel[- ](\d+)\b", line_value(block, prose_field))
            }
            stale_levels = sorted(claimed_levels - effective_levels)
            if stale_levels:
                errors.append(
                    f"encounter {encounter_index}: {prose_field} names absent effective levels {stale_levels}; "
                    f"branch levels are {sorted(effective_levels)}"
                )
        if not any(token in line_value(block, "trainer_ids") for token in MARQUEE_TOKENS + MINIBOSS_TOKENS):
            ordinary_difficulties.append(difficulty)

    if len(all_trainers) != len(set(all_trainers)):
        duplicates = sorted(trainer for trainer, count in Counter(all_trainers).items() if count > 1)
        errors.append(f"trainer branches occur in multiple encounter groups: {duplicates[:20]}")
    branches = len(all_trainers)
    doubles = formats["double"] + formats["multi"]
    doubles_pct = doubles / branches * 100
    if not 83 <= doubles_pct <= 87:
        errors.append(f"doubles share {doubles_pct:.2f}% is outside 83-87%")
    duplicate_teams = sum(len(encounters) - 1 for encounters in fingerprint_encounters.values() if len(encounters) > 1)
    if duplicate_teams:
        errors.append(f"{duplicate_teams} exact duplicate team fingerprints remain")

    missing_megas = sorted(MEGA_STONES - set(all_items))
    if missing_megas:
        errors.append(f"missing Mega showcases: {missing_megas}")
    used_species = set(all_species)
    missing_signs = sorted(
        species
        for species in SIGN_SPECIES
        if not (LEGENDARY_SHOWCASE_ALIASES.get(species, {species}) & used_species)
    )
    if missing_signs:
        errors.append(f"missing legendary showcases: {missing_signs}")

    current_refs = current_campaign_trainer_refs()
    documented = set(all_trainers)
    missing_current = sorted(current_refs - documented)
    if missing_current:
        errors.append(f"current Hoenn battle references absent from master: {missing_current}")
    planned = documented - current_refs
    unclassified_planned = sorted(planned - PLANNED_RESTORE_TRAINERS)
    if unclassified_planned:
        errors.append(f"non-runtime trainers not declared for restoration: {unclassified_planned}")
    missing_planned = sorted(PLANNED_RESTORE_TRAINERS - documented)
    if missing_planned:
        errors.append(f"declared restoration trainers absent from master: {missing_planned}")

    party_source = Path(os.environ.get("EC_TRAINERS_PARTY", ROOT / "src" / "data" / "trainers.party")).read_text()
    party_blocks = {
        match.group(1): match.group(2)
        for match in re.finditer(
            r"(?ms)^=== (TRAINER_[A-Z0-9_]+) ===\n(.*?)(?=^=== |\Z)",
            party_source,
        )
    }
    missing_parties = sorted(current_refs - set(party_blocks))
    if missing_parties:
        errors.append(f"runtime trainers absent from trainers.party: {missing_parties}")
    for trainer in sorted(current_refs & set(party_blocks)):
        actual_size = len(re.findall(r"(?m)^SPECIES_[A-Z0-9_]+(?: @ ITEM_[A-Z0-9_]+)?$", party_blocks[trainer]))
        expected_size = expected_party_sizes.get(trainer)
        if expected_size is not None and actual_size != expected_size:
            errors.append(
                f"{trainer}: trainers.party has {actual_size} Pokemon but master branch has {expected_size}"
            )
        has_half_party = bool(re.search(r"(?m)^Multi Party: Half$", party_blocks[trainer]))
        if trainer in expected_multi_trainers and not has_half_party:
            errors.append(f"{trainer}: multi branch is missing Multi Party: Half")
        if trainer not in expected_multi_trainers and has_half_party:
            errors.append(f"{trainer}: non-multi branch unexpectedly has Multi Party: Half")
    campaign_bag_users = sorted(
        trainer for trainer in current_refs
        if trainer in party_blocks and re.search(r"(?m)^Items:", party_blocks[trainer])
    )
    if campaign_bag_users:
        errors.append(
            "campaign trainers still carry Bag healing items despite the no-Bag ruleset: "
            + ", ".join(campaign_bag_users)
        )

    ordinary_bands = Counter(min(9, int(value)) for value in ordinary_difficulties)
    ordinary_total = len(ordinary_difficulties)
    low_share = sum(6.0 <= value < 7.0 for value in ordinary_difficulties) / ordinary_total * 100
    if not 20 <= low_share <= 40:
        errors.append(f"ordinary 6.x share {low_share:.1f}% is outside fatigue-safe 20-40%")
    high_share = sum(value >= 9.0 for value in ordinary_difficulties) / ordinary_total * 100
    if high_share > 12:
        errors.append(f"ordinary 9.x share {high_share:.1f}% exceeds 12%")
    if any(value < 6.0 or value > 9.5 for value in ordinary_difficulties):
        errors.append("ordinary encounter difficulty falls outside 6.0-9.5")

    run_start = 0
    while run_start < len(primary_strategies):
        run_end = run_start + 1
        while run_end < len(primary_strategies) and primary_strategies[run_end] == primary_strategies[run_start]:
            run_end += 1
        if run_end - run_start >= 5:
            errors.append(
                f"primary strategy {primary_strategies[run_start]} repeats from encounters {run_start + 1}-{run_end}"
            )
        run_start = run_end
    rolling_repeat_encounters = 0
    for index, species_set in enumerate(encounter_species_sets):
        recent = set().union(*encounter_species_sets[max(0, index - 2):index]) if index else set()
        if species_set & recent:
            rolling_repeat_encounters += 1
    if rolling_repeat_encounters > 35:
        errors.append(f"species repeat in the prior-two window occurs in {rolling_repeat_encounters} encounters (max 35)")

    usage = Counter(all_species)
    notes.extend([
        f"encounters={len(groups)} branches={branches} formats={dict(formats)} doubles={doubles_pct:.2f}%",
        f"difficulty mean={statistics.mean(difficulties):.2f} median={statistics.median(difficulties):.1f}",
        f"ordinary bands={dict(sorted(ordinary_bands.items()))} 6.x={low_share:.1f}% 9.x={high_share:.1f}%",
        f"team sizes={dict(sorted(team_sizes.items()))}",
        f"unique species={len(usage)} top usage={usage.most_common(15)}",
        f"Mega showcases={len(MEGA_STONES - set(missing_megas))}/{len(MEGA_STONES)} legendary showcases={len(SIGN_SPECIES - set(missing_signs))}/{len(SIGN_SPECIES)}",
        f"current runtime trainer ids={len(current_refs)} planned restores={len(planned)}",
        f"primary strategies={dict(Counter(primary_strategies))} prior-two species-repeat encounters={rolling_repeat_encounters}",
        f"dossier exact loadout blocks={exact_dossier_blocks} facts={exact_dossier_facts} source-verified dialogue blocks={source_verified_dialogue_blocks}",
    ])
    return errors, notes


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("master", nargs="?", type=Path, default=DEFAULT_MASTER)
    args = parser.parse_args()
    errors, notes = audit(args.master)
    for note in notes:
        print(f"INFO: {note}")
    if errors:
        for error in errors:
            print(f"FAIL: {error}")
        raise SystemExit(1)
    print("PASS: campaign battle master satisfies all static closure gates")


if __name__ == "__main__":
    main()
