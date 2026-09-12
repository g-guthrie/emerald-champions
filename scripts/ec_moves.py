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
