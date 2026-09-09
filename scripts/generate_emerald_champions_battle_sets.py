#!/usr/bin/env python3
"""Generate runtime presets from the single current hand-authored EV catalog."""

from __future__ import annotations

import argparse
import copy
import json
import re
import shutil
import subprocess
import tempfile
from pathlib import Path

from emerald_champions_evs import EV_PER_STAT_MAX, EV_TOTAL_MAX, validate_evs
from verify_trainer_ability_legality import (
    SPECIES_MARKER,
    configured_species_abilities,
    preprocess_species_info,
    resolve_species,
    species_aliases,
)

ROOT = Path(__file__).resolve().parents[1]
HAND_AUDITED_SOURCE = ROOT / "data/emerald_champions/emerald_champions_hand_audited_battle_sets.json"
JSON_OUTPUT = ROOT / "data/emerald_champions/emerald_champions_battle_sets.json"
C_OUTPUT = ROOT / "src/data/pokemon/emerald_champions_battle_sets.h"
MOVE_ACCESS_REVIEW = ROOT / "data/emerald_champions/emerald_champions_move_access_review.json"
MOVE_ACCESS_C_OUTPUT = ROOT / "src/data/pokemon/emerald_champions_move_access_review.h"


def load_hand_audited_source() -> dict:
    """Read the sole editable preset owner, with EV units and explicit order."""
    source = json.loads(HAND_AUDITED_SOURCE.read_text())
    assert source["schema_version"] == 2
    order = source["species_order"]
    assert len(order) == len(set(order))
    assert set(order) == set(source["species"])
    return source



def load_hand_audited_catalog() -> dict:
    """Load explicit reviews and resolve only named, mechanically identical aliases."""
    source = load_hand_audited_source()
    resolved = {}
    for species, review in source["species"].items():
        if "inherits_species" not in review:
            resolved[species] = review
            continue
        parent_species = review["inherits_species"]
        assert parent_species in resolved, (species, parent_species)
        assert set(review) == {"inherits_species", "references"}, species
        inherited = copy.deepcopy(resolved[parent_species])
        inherited["references"] = review["references"]
        for format_name in ("doubles", "singles"):
            for entry in inherited[format_name]:
                entry["species"] = species
                entry["source"] = (
                    f"Hand-audited mechanical alias of {parent_species}: {entry['source']}"
                )
        resolved[species] = inherited
    source["species"] = resolved
    return source


def constants(path: Path, prefix: str) -> set[str]:
    return set(re.findall(rf"\b{prefix}[A-Z0-9_]+\b", path.read_text()))


def configured_integer_values(expressions: list[str]) -> dict[str, int]:
    """Let C evaluate table constants, including signed values and Gen ternaries."""
    expressions = list(dict.fromkeys(expressions))
    compiler = shutil.which("cc") or shutil.which("clang") or shutil.which("gcc")
    if compiler is None:
        raise RuntimeError("a host C compiler is required for configured table values")
    source = (
        '#include <stdio.h>\n'
        '#include "config/general.h"\n'
        '#include "constants/global.h"\n'
        'int main(void) {\n'
        '    const int values[] = {\n'
        + ",\n".join(f"        ({expression})" for expression in expressions)
        + '\n    };\n'
        '    for (unsigned i = 0; i < sizeof(values) / sizeof(values[0]); i++)\n'
        '        printf("%d\\n", values[i]);\n'
        '    return 0;\n}\n'
    )
    with tempfile.TemporaryDirectory() as directory:
        executable = Path(directory) / "table_values"
        subprocess.run(
            [compiler, "-std=c11", "-x", "c", "-DTRUE=1", "-DFALSE=0",
             "-I", str(ROOT / "include"), "-", "-o", str(executable)],
            input=source, text=True, check=True,
        )
        values = subprocess.check_output([str(executable)], text=True).splitlines()
    return dict(zip(expressions, map(int, values), strict=True))


# Retained public helper: generate_showdown_champions_circuit.py consumes this
# configured species metadata. It does not contribute preset sources.


def species_build_metadata() -> dict[str, dict]:
    """Resolve the same configured species table consumed by the ROM."""
    text = preprocess_species_info()
    table_start = text.find("const struct SpeciesInfo gSpeciesInfo[]")
    assert table_start >= 0
    text = text[table_start:]
    stat_values = configured_integer_values(re.findall(
        r"\.base(?:HP|Attack|Defense|SpAttack|SpDefense|Speed)\s*=\s*([^,]+)", text
    ))
    markers = list(SPECIES_MARKER.finditer(text))
    result: dict[str, dict] = {}
    for index, marker in enumerate(markers):
        end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
        block = text[marker.start():end]

        def stat(field: str) -> int:
            match = re.search(rf"\.{field}\s*=\s*([^,]+)", block)
            return stat_values[match.group(1)] if match else 0

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


def validate(entries: list[dict]) -> None:
    species = constants(ROOT / "include" / "constants" / "species.h", "SPECIES_")
    moves = constants(ROOT / "include" / "constants" / "moves.h", "MOVE_")
    items = constants(ROOT / "include" / "constants" / "items.h", "ITEM_")
    abilities = constants(ROOT / "include" / "constants" / "abilities.h", "ABILITY_")
    natures = constants(ROOT / "include" / "constants" / "pokemon.h", "NATURE_")
    configured_abilities = configured_species_abilities()
    aliases = species_aliases()
    for entry in entries:
        assert entry["species"] in species, entry["species"]
        assert entry["nature"] in natures, entry["nature"]
        assert entry["ability"] in abilities, entry["ability"]
        # Transformation presets name the post-transform Ability; ordinary
        # presets must not rely on ApplyPreset's fallback to a base Ability.
        if entry["required_item"] == "ITEM_NONE" and entry.get("required_move", "MOVE_NONE") == "MOVE_NONE":
            legal = configured_abilities.get(resolve_species(entry["species"], aliases), frozenset())
            assert entry["ability"] in legal, (entry["species"], entry["ability"], sorted(legal))
        assert entry["item"] in items, entry["item"]
        assert entry["required_item"] in items, entry["required_item"]
        if entry.get("required_move", "MOVE_NONE") != "MOVE_NONE":
            assert entry["required_move"] in moves, entry["required_move"]
            assert entry["required_move"] in entry["moves"], entry
        assert 1 <= len(entry["moves"]) <= 4, (entry["species"], entry["name"], entry["moves"])
        assert len(entry["moves"]) == len(set(entry["moves"])), (entry["species"], entry["moves"])
        assert all(move in moves for move in entry["moves"])
        validate_evs(entry["evs"])


def c_preset(entry: dict, indent: str = "        ") -> list[str]:
    moves = entry["moves"] + ["MOVE_NONE"] * (4 - len(entry["moves"]))
    evs = ", ".join(str(value) for value in entry["evs"])
    return [
        indent + ".moves = {" + ", ".join(moves) + "},",
        indent + f'.item = {entry["item"]},',
        indent + f'.requiredItem = {entry["required_item"]},',
        indent + f'.requiredMove = {entry.get("required_move", "MOVE_NONE")},',
        indent + f'.nature = {entry["nature"]},',
        indent + f'.ability = {entry["ability"]},',
        indent + f".evs = {{{evs}}},",
    ]


def render_c(
    defaults: list[dict],
    alternatives: list[dict],
    singles_defaults: list[dict],
    singles_alternatives: list[dict],
) -> str:
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
            # The runtime count includes the default and is returned as u8.
            assert len(choices) < 255 and offset <= 65535, entry["species"]
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
            assert len(choices) < 255 and offset <= 65535, entry["species"]
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
    return "\n".join(lines)


def render_move_access_review_c() -> str:
    review = json.loads(MOVE_ACCESS_REVIEW.read_text())
    retained = [row for row in review["assignments"] if row["action"] != "replace"]
    assert review["reviewed_assignment_count"] == len(review["assignments"])
    lines = [
        "// Generated by scripts/generate_emerald_champions_battle_sets.py. Do not edit by hand.",
        f"#define EC_REVIEWED_MOVE_ACCESS_COUNT {len(retained)}",
    ]
    lines.extend(
        f'    {{{row["species"]}, {row["move"]}}},'
        for row in retained
    )
    lines.append("")
    return "\n".join(lines)


def main() -> None:
    if not __debug__:
        raise SystemExit("battle-set generation requires assertions; do not run Python with -O")
    parser = argparse.ArgumentParser(description=__doc__)
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument("--write", action="store_true")
    mode.add_argument("--check", action="store_true")
    args = parser.parse_args()

    # Nothing is synthesized or imported over this catalog. The ordering also
    # belongs to the catalog, not old git objects or an earlier generated file.
    source = load_hand_audited_catalog()
    buckets = {}
    for format_name in ("doubles", "singles"):
        defaults = []
        alternatives = []
        for species in source["species_order"]:
            choices = source["species"][species][format_name]
            assert choices, (species, format_name, "missing default preset")
            assert all(entry["species"] == species for entry in choices)
            defaults.append(choices[0])
            alternatives.extend(choices[1:])
        buckets[format_name] = (defaults, alternatives)
    defaults, alternatives = buckets["doubles"]
    singles_defaults, singles_alternatives = buckets["singles"]
    entries = defaults + alternatives + singles_defaults + singles_alternatives
    validate(entries)

    output = {
        "schema_version": 4,
        "canonical_source": str(HAND_AUDITED_SOURCE.relative_to(ROOT)),
        "policy": {
            "format": "named Doubles and Singles buckets; Doubles remains the wild/evolution default",
            "evs": (
                f"Gen 9: {EV_TOTAL_MAX} total, {EV_PER_STAT_MAX} maximum per stat; "
                f"generated spreads use at most {EV_TOTAL_MAX // 4 * 4} effective EVs"
            ),
            "ability": "resolved by Ability identity against current species data",
            "protected_items": "never supplied by a preset",
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
    rendered = {
        JSON_OUTPUT: json.dumps(output, indent=2, ensure_ascii=False) + "\n",
        C_OUTPUT: render_c(defaults, alternatives, singles_defaults, singles_alternatives),
        # Move-access exceptions have their own existing owner; this output
        # neither contributes presets nor duplicates authored EV allocations.
        MOVE_ACCESS_C_OUTPUT: render_move_access_review_c(),
    }
    if args.check:
        stale = [str(path.relative_to(ROOT)) for path, text in rendered.items()
                 if not path.is_file() or path.read_text() != text]
        if stale:
            raise SystemExit("generated battle-set outputs are stale: " + ", ".join(stale))
        print("PASS: generated battle-set outputs match current canonical inputs byte-for-byte")
    else:
        for path, text in rendered.items():
            path.write_text(text)
    print(f"defaults={len(defaults)}")
    print(f"alternatives={len(alternatives)}")
    print(f"singles_defaults={len(singles_defaults)}")
    print(f"singles_alternatives={len(singles_alternatives)}")
    print(f"sets={len(entries)}")


if __name__ == "__main__":
    main()
