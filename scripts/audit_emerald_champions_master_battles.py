#!/usr/bin/env python3
"""Legacy authoring helpers retained for existing importers.

The obsolete monolithic closure audit and CLI were removed. Pinned move pools,
raw-source types and historical dossier helpers are not native legality or
strategic-quality checks; see docs/static_check_audit/trainers.md.
"""

from __future__ import annotations

import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_MASTER = ROOT / "data/emerald_champions/emerald_champions_master_battle_design.txt"
ENCOUNTER_RE = re.compile(r"(?m)^=== ENCOUNTER (\d{4}) ===$")
BRANCH_RE = re.compile(r"(?m)^--- BRANCH ([A-Z0-9_]+) ---$")
MON_RE = re.compile(
    r"(?m)^  (\d+)\. (SPECIES_[A-Z0-9_]+) @ (ITEM_[A-Z0-9_]+) \| "
    r"level_offset=(-?\d+) \| ability=(ABILITY_[A-Z0-9_]+) \| "
    r"nature=(NATURE_[A-Z0-9_]+) \| evs=([0-9/]+) \| "
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
                "ordinary singles positioning."
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
            "through ordinary positioning."
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

SHOWDOWN_DATA = json.loads((ROOT / "data/emerald_champions/showdown_champions_learnsets.json").read_text())
SHOWDOWN_LEARNSETS = {species: set(moves) for species, moves in SHOWDOWN_DATA["learnsets"].items()}
MOVE_ACCESS_REVIEW = json.loads((ROOT / "data/emerald_champions/emerald_champions_move_access_review.json").read_text())
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
        "SPECIES_WHIMSICOTT": ("TYPE_GRASS", "TYPE_FAIRY"),
        "SPECIES_ROTOM": ("TYPE_ELECTRIC", "TYPE_GHOST"),
        "SPECIES_MAGNETON": ("TYPE_ELECTRIC", "TYPE_STEEL"),
        "SPECIES_TOGEKISS": ("TYPE_FAIRY", "TYPE_FLYING"),
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
