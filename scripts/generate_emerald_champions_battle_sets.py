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
    default_species = {entry["species"] for entry in defaults}
    assert len(default_species) == len(defaults), "Species aliases collapsed two default presets"
    raw_alternatives = [
        normalize_alternative(entry)
        for entry in alternative_source["alternatives"]
        if normalize_species(entry["species"]) in default_species
    ]
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
