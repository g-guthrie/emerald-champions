#!/usr/bin/env python3
"""Verify the selective Inclement Emerald base-stat port.

The manifest is the only accepted list of Emerald Champions stat changes.  This
gate preprocesses the same configured species table as the ROM, rather than
trusting source spelling or design documentation, and checks every target plus
the HP invariants of affected Mega families.
"""

from __future__ import annotations

import hashlib
import json
import re
import shutil
import subprocess
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "data/emerald_champions/emerald_champions_stat_rebalances.json"
SPECIES_CONSTANTS = ROOT / "include/constants/species.h"
FORM_TABLES = ROOT / "src/data/pokemon/form_species_tables.h"
FIELDS = (
    "baseHP",
    "baseAttack",
    "baseDefense",
    "baseSpeed",
    "baseSpAttack",
    "baseSpDefense",
)
SPECIES_MARKER = re.compile(r"(?m)^\s*\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{")
SPECIES_ALIAS = re.compile(
    r"(?m)^\s*(SPECIES_[A-Z0-9_]+)\s*=\s*(SPECIES_[A-Z0-9_]+)\s*,"
)
STAT_ASSIGNMENT = {
    field: re.compile(rf"\.{field}\s*=\s*([^,\n]+)") for field in FIELDS
}
SPECIAL_FLAGS = (
    "isSubLegendary",
    "isLegendary",
    "isMythical",
    "isUltraBeast",
    "isParadox",
)


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def preprocess_species_info() -> str:
    compiler = shutil.which("cc") or shutil.which("clang") or shutil.which("gcc")
    require(compiler is not None, "a host C preprocessor is required")
    probe = (
        '#include "config/general.h"\n'
        '#include "constants/global.h"\n'
        '#include "constants/abilities.h"\n'
        '#include "data/pokemon/species_info.h"\n'
    )
    command = (
        compiler,
        "-E",
        "-P",
        "-x",
        "c",
        "-DTRUE=1",
        "-DFALSE=0",
        f"-I{ROOT / 'include'}",
        f"-I{ROOT / 'src'}",
        f"-I{ROOT}",
        "-",
    )
    result = subprocess.run(
        command,
        input=probe,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    require(
        result.returncode == 0,
        "failed to preprocess configured species data:\n" + result.stderr,
    )
    return result.stdout


def strip_outer_parentheses(expression: str) -> str:
    expression = expression.strip()
    while expression.startswith("(") and expression.endswith(")"):
        depth = 0
        encloses_all = True
        for index, character in enumerate(expression):
            if character == "(":
                depth += 1
            elif character == ")":
                depth -= 1
            if depth == 0 and index != len(expression) - 1:
                encloses_all = False
                break
        if not encloses_all:
            break
        expression = expression[1:-1].strip()
    return expression


def evaluate_integer(expression: str) -> int:
    expression = strip_outer_parentheses(expression)
    if "?" in expression:
        require(expression.count("?") == 1, f"nested stat expression: {expression}")
        condition, choices = expression.split("?", 1)
        require(choices.count(":") == 1, f"invalid stat ternary: {expression}")
        when_true, when_false = choices.split(":", 1)
        condition = condition.replace("||", " or ").replace("&&", " and ")
        require(
            re.fullmatch(r"[0-9+\-*/%() <>=!&|orand]+", condition) is not None,
            f"unsafe stat condition: {condition}",
        )
        branch = when_true if bool(eval(condition, {"__builtins__": {}}, {})) else when_false
        return evaluate_integer(branch)
    require(
        re.fullmatch(r"[0-9+\-*/%() <>=!&|]+", expression) is not None,
        f"unsafe stat expression: {expression}",
    )
    return int(eval(expression, {"__builtins__": {}}, {}))


def configured_species() -> tuple[dict[str, dict[str, int]], dict[str, str]]:
    text = preprocess_species_info()
    table_start = text.find("const struct SpeciesInfo gSpeciesInfo[]")
    require(table_start >= 0, "preprocessed source has no gSpeciesInfo table")
    text = text[table_start:]
    markers = list(SPECIES_MARKER.finditer(text))
    require(markers, "configured species table has no entries")

    stats: dict[str, dict[str, int]] = {}
    blocks: dict[str, str] = {}
    for index, marker in enumerate(markers):
        end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
        species = marker.group(1)
        block = text[marker.start():end]
        blocks[species] = block
        values = {}
        for field, pattern in STAT_ASSIGNMENT.items():
            match = pattern.search(block)
            if match is not None:
                values[field] = evaluate_integer(match.group(1))
        if len(values) == len(FIELDS):
            stats[species] = values
    return stats, blocks


def species_aliases() -> dict[str, str]:
    return dict(SPECIES_ALIAS.findall(SPECIES_CONSTANTS.read_text()))


def resolve_species(species: str, aliases: dict[str, str]) -> str:
    seen: set[str] = set()
    while species in aliases:
        require(species not in seen, f"cyclic species alias involving {species}")
        seen.add(species)
        species = aliases[species]
    return species


def mega_forms() -> dict[str, tuple[str, ...]]:
    result: dict[str, tuple[str, ...]] = {}
    arrays = re.findall(
        r"static const u16 \w+\[\]\s*=\s*\{(.*?)\};",
        FORM_TABLES.read_text(),
        re.DOTALL,
    )
    for body in arrays:
        species = re.findall(r"SPECIES_[A-Z0-9_]+", body)
        if not species:
            continue
        forms = tuple(
            form for form in species[1:] if re.search(r"_MEGA(?:_|$)", form)
        )
        if forms:
            result[species[0]] = forms
    return result


def main() -> None:
    manifest = json.loads(MANIFEST.read_text())
    require(manifest.get("schema_version") == 1, "unsupported stat manifest schema")
    entries = manifest.get("port_entries", [])
    paired = manifest.get("paired_form_adjustments", [])
    counts = manifest.get("counts", {})
    require(len(entries) == 102, f"stat manifest must contain 102 ports, found {len(entries)}")
    require(len(paired) == 3, f"stat manifest must contain 3 paired forms, found {len(paired)}")
    require(counts.get("ported_species_forms") == len(entries), "manifest port count drifted")
    require(counts.get("paired_form_adjustments") == len(paired), "paired count drifted")
    require(counts.get("excluded_or_deferred") == 162, "deferred count drifted")

    names = [entry["species"] for entry in entries]
    require(len(names) == len(set(names)), "stat manifest contains duplicate species")
    decisions = [entry["decision"] for entry in entries]
    require(decisions.count("ability_coupled") == 29, "ability-coupled port count drifted")
    require(decisions.count("conservative") == 73, "conservative port count drifted")

    stats, blocks = configured_species()
    aliases = species_aliases()
    failures: list[str] = []
    targets: dict[str, dict[str, int]] = {}
    for entry in entries:
        species = resolve_species(entry["species"], aliases)
        target = entry["stats_target"]
        require(set(target) == set(FIELDS), f"{species}: incomplete target stats")
        require(species in stats, f"{species}: no configured species stats")
        targets[species] = target
        if stats[species] != target:
            failures.append(f"{species}: configured={stats[species]} target={target}")
        block = blocks[species]
        if ".evolutions" in block:
            failures.append(f"{species}: selected stat port is not final-stage")
        if re.search(r"\.isMegaEvolution\s*=\s*1\b", block):
            failures.append(f"{species}: Mega form cannot be a direct port entry")
        for flag in SPECIAL_FLAGS:
            if re.search(rf"\.{flag}\s*=\s*1\b", block):
                failures.append(f"{species}: special Pokemon cannot be a direct port entry ({flag})")

    paired_names: set[str] = set()
    for adjustment in paired:
        species = resolve_species(adjustment["species"], aliases)
        base_species = resolve_species(adjustment["base_species"], aliases)
        paired_names.add(species)
        require(species in stats and base_species in stats, f"{species}: missing paired form")
        require(base_species in targets, f"{species}: paired base is not a selected port")
        target_hp = adjustment["baseHP_target"]
        if stats[species]["baseHP"] != target_hp:
            failures.append(
                f"{species}: paired HP={stats[species]['baseHP']} target={target_hp}"
            )
        if stats[base_species]["baseHP"] != target_hp:
            failures.append(
                f"{base_species}: base HP={stats[base_species]['baseHP']} paired target={target_hp}"
            )
        if not re.search(r"\.isMegaEvolution\s*=\s*1\b", blocks[species]):
            failures.append(f"{species}: paired adjustment is not a Mega form")

    forms = mega_forms()
    for base_species in targets:
        for form in forms.get(base_species, ()):
            form = resolve_species(form, aliases)
            if form not in stats:
                continue
            if stats[form]["baseHP"] != stats[base_species]["baseHP"]:
                failures.append(
                    f"{base_species}/{form}: Mega-family HP mismatch "
                    f"{stats[base_species]['baseHP']} != {stats[form]['baseHP']}"
                )

    selected = set(targets) | paired_names
    unselected = {
        species: values for species, values in stats.items() if species not in selected
    }
    guard = manifest.get("unselected_configured_stats_guard", {})
    require(
        guard.get("species_count") == len(unselected),
        "unselected configured species count drifted",
    )
    digest = hashlib.sha256(
        json.dumps(unselected, sort_keys=True, separators=(",", ":")).encode()
    ).hexdigest()
    require(
        guard.get("sha256") == digest,
        "an unmanifested configured base-stat change was detected",
    )

    require(not failures, f"{len(failures)} stat policy failures:\n" + "\n".join(failures))
    print(
        "PASS: 102 selective Inclement stat ports and 3 paired Mega HP adjustments "
        "match the configured species table"
    )


if __name__ == "__main__":
    main()
