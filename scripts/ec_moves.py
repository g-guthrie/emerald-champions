#!/usr/bin/env python3
"""Print pinned legal moves, Abilities and types for species (authoring aid).

    python3 scripts/ec_moves.py PARAS SAWK            # full legal move lists
    python3 scripts/ec_moves.py PARAS -- KNOCK_OFF SPORE   # check candidates
"""
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def constants(path: str, prefix: str) -> set[str]:
    return set(re.findall(rf"\b{prefix}[A-Z0-9_]+\b", (ROOT / path).read_text()))


ITEMS = constants("include/constants/items.h", "ITEM_")


MOVES = constants("include/constants/moves.h", "MOVE_")


NATURES = constants("include/constants/pokemon.h", "NATURE_")


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
    # E0475 uses Unbound; count the Hoopa family without claiming both forms
    # or their distinct tactical roles have been demonstrated.
    "SPECIES_HOOPA": {"SPECIES_HOOPA", "SPECIES_HOOPA_UNBOUND"},
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
    "debutante", "kabuki", "disguised", "busted", "shield", "standard", "solo",
    "plant", "baile", "blue", "orange", "red", "white", "yellow", "dandy",
    "diamond", "heart", "lareine", "matron", "pharaoh", "star", "50", "10",
)


# Species whose Showdown id cannot be derived by simple prefix/suffix matching
# (gendered defaults, irregular totem/style/gmax compounds, or Zygarde's bare
# 50/10 aliases). Checked before the generic suffix table.
SHOWDOWN_ID_OVERRIDES = {
    "SPECIES_MIMIKYU_TOTEM_BUSTED": "mimikyubustedtotem",
    "SPECIES_INDEEDEE_M": "indeedee",
    "SPECIES_MEOWSTIC_M": "meowstic",
    "SPECIES_BASCULEGION_M": "basculegion",
    "SPECIES_DARMANITAN_GALAR_STANDARD": "darmanitangalar",
    "SPECIES_TOXTRICITY_AMPED_GMAX": "toxtricitygmax",
    "SPECIES_ZYGARDE_10_AURA_BREAK": "zygarde10",
    "SPECIES_ZYGARDE_50": "zygarde",
    "SPECIES_URSHIFU_RAPID_STRIKE_STYLE_GMAX": "urshifurapidstrikegmax",
    "SPECIES_URSHIFU_SINGLE_STRIKE": "urshifu",
    "SPECIES_URSHIFU_SINGLE_STRIKE_GMAX": "urshifugmax",
    "SPECIES_URSHIFU_SINGLE_STRIKE_STYLE_GMAX": "urshifugmax",
}


def showdown_id_for_species(species: str) -> str | None:
    if species in SHOWDOWN_ID_OVERRIDES:
        return SHOWDOWN_ID_OVERRIDES[species]
    showdown_id = re.sub(r"[^a-z0-9]", "", species.removeprefix("SPECIES_").lower())
    if showdown_id in SHOWDOWN_LEARNSETS:
        return showdown_id
    for suffix in SHOWDOWN_FORM_SUFFIXES:
        if showdown_id.endswith(suffix) and showdown_id[:-len(suffix)] in SHOWDOWN_LEARNSETS:
            return showdown_id[:-len(suffix)]
    return None


def species_is_resolvable(species: str) -> bool:
    """False only when neither the Showdown pin nor a reviewed extension can
    tell us this species' legal moves; callers must report that as an error
    instead of silently treating it as zero legal moves."""
    return showdown_id_for_species(species) is not None or bool(REVIEWED_MOVE_EXTENSIONS.get(species))


ALL_LEARNABLES = json.loads((ROOT / "src/data/pokemon/all_learnables.json").read_text())


# Species whose ALL_LEARNABLES entry lives only under a form-qualified key,
# with no bare/base-form key to fall back to (e.g. Wormadam's default in-game
# form is Plant, but all_learnables.json only has WORMADAM_PLANT/_SANDY/_TRASH).
ROM_LEARNABLE_ALIASES = {
    "WORMADAM": "WORMADAM_PLANT",
}


def rom_learnable_moves(species: str) -> set[str]:
    """The ROM's own learnset data (level-up/TM/tutor/egg, already flattened)
    for a species, falling back to its base form when a form-specific entry
    is absent (e.g. AEGISLASH_BLADE has no entry of its own; AEGISLASH does)."""
    token = species.removeprefix("SPECIES_")
    if token in ROM_LEARNABLE_ALIASES:
        return set(ALL_LEARNABLES.get(ROM_LEARNABLE_ALIASES[token], ()))
    while token:
        moves = ALL_LEARNABLES.get(token)
        if moves is not None:
            return set(moves)
        if "_" not in token:
            break
        token = token.rsplit("_", 1)[0]
    return set()


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


def legal_moves_with_rom_union(species: str) -> tuple[set[str], set[str]]:
    """(legal moves = pinned | ROM learnset, moves only legal via the ROM
    learnset union -- i.e. the Showdown pin lacks them but the ROM's own
    all_learnables.json has them)."""
    pinned = pinned_legal_moves(species)
    rom_only = rom_learnable_moves(species) - pinned
    return pinned | rom_only, rom_only


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


TYPES = species_types()


def _move_blocks() -> list[str]:
    text = (ROOT / "src/data/moves_info.h").read_text()
    return re.split(r"\n    \[MOVE_", text)[1:]


def move_types() -> dict[str, str]:
    """MOVE_x -> its TYPE_x, read from src/data/moves_info.h."""
    result = {}
    for block in _move_blocks():
        name = "MOVE_" + block.split("]")[0].strip()
        match = re.search(r"\.type\s*=\s*(TYPE_[A-Z0-9_]+)", block)
        if match:
            result[name] = match.group(1)
    return result


def move_categories() -> dict[str, str]:
    """MOVE_x -> PHYSICAL/SPECIAL/STATUS, read from src/data/moves_info.h."""
    result = {}
    for block in _move_blocks():
        name = "MOVE_" + block.split("]")[0].strip()
        match = re.search(r"\.category\s*=\s*DAMAGE_CATEGORY_([A-Z]+)", block)
        if match:
            result[name] = match.group(1)
    return result


TYPE_CHART_COLUMNS = (
    "TYPE_NONE", "TYPE_NORMAL", "TYPE_FIGHTING", "TYPE_FLYING", "TYPE_POISON", "TYPE_GROUND",
    "TYPE_ROCK", "TYPE_BUG", "TYPE_GHOST", "TYPE_STEEL", "TYPE_MYSTERY", "TYPE_FIRE", "TYPE_WATER",
    "TYPE_GRASS", "TYPE_ELECTRIC", "TYPE_PSYCHIC", "TYPE_ICE", "TYPE_DRAGON", "TYPE_DARK",
    "TYPE_FAIRY", "TYPE_STELLAR",
)


def type_chart() -> dict[str, dict[str, float]]:
    """Attacker TYPE_x -> {defender TYPE_x: multiplier}, read from
    src/data/types_info.h. B_UPDATED_TYPE_MATCHUPS is GEN_LATEST in this repo
    (include/config/battle.h), so generation-conditional matchup macros
    (STL_RS, PSN_RS, ...) resolve to their modern/current-gen value."""
    text = (ROOT / "src" / "data" / "types_info.h").read_text()
    macro_values = {}
    for macro in re.finditer(
        r"#define\s+([A-Z0-9_]+)\s+\(B_UPDATED_TYPE_MATCHUPS\s*>=\s*GEN_\d+\s*\?\s*X\(([\d.]+)\)\s*:\s*X\([\d.]+\)\)",
        text,
    ):
        macro_values[macro.group(1)] = float(macro.group(2))
    table = re.search(r"gTypeEffectivenessTable\[[^\]]*\]\[[^\]]*\]\s*=\s*\{(.*?)\n\};", text, re.S)
    chart: dict[str, dict[str, float]] = {}
    for row in re.finditer(r"\[(TYPE_[A-Z0-9_]+)\]\s*=\s*\{([^}]*)\}", table.group(1)):
        attacker = row.group(1)
        cells = [cell.strip() for cell in row.group(2).split(",") if cell.strip()]
        line = {}
        for column, cell in zip(TYPE_CHART_COLUMNS, cells):
            if cell == "______":
                line[column] = 1.0
            elif cell.startswith("X("):
                line[column] = float(cell[2:-1])
            else:
                line[column] = macro_values.get(cell, 1.0)
        chart[attacker] = line
    return chart


def abilities() -> dict[str, list[str]]:
    result = {}
    for path in (ROOT / "src/data/pokemon/species_info").glob("*.h"):
        text = path.read_text(errors="ignore")
        for block in re.split(r"\n    \[SPECIES_", text)[1:]:
            name = "SPECIES_" + block.split("]")[0].strip()
            match = re.search(r"\.abilities\s*=\s*\{([^}]*)\}", block)
            if match:
                result[name] = [
                    token.strip()[8:] for token in match.group(1).split(",")
                    if token.strip() and "NONE" not in token
                ]
    return result


def main() -> None:
    args = sys.argv[1:]
    candidates = []
    if "--" in args:
        index = args.index("--")
        candidates = [f"MOVE_{move}" for move in args[index + 1:]]
        args = args[:index]
    ability_table = abilities()
    for name in args:
        species = f"SPECIES_{name}"
        legal = pinned_legal_moves(species)
        types = "/".join(t[5:] for t in TYPES.get(species, ()))
        print(f"== {name} [{types}] abilities={ability_table.get(species)}")
        if candidates:
            for move in candidates:
                print(f"   {'OK ' if move in legal else 'NO '} {move[5:]}")
        else:
            print("   " + ", ".join(sorted(move[5:] for move in legal)))


if __name__ == "__main__":
    main()
