#!/usr/bin/env python3
"""Verify every authored trainer Ability against the configured species data.

The ROM resolves a trainer's requested Ability by searching that species'
``gSpeciesInfo[].abilities`` array.  An illegal request used to fall through to
Ability slot zero in release builds because the diagnostic assert is compiled
out.  This gate preprocesses the same configured species table that the ROM
build consumes, resolves enum aliases such as ``SPECIES_MIMIKYU``, and rejects
every illegal explicit Ability in ``src/data/trainers.party``.
"""

from __future__ import annotations

import os
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TRAINERS = Path(os.environ.get("EC_TRAINERS_PARTY", ROOT / "src/data/trainers.party"))
SPECIES_CONSTANTS = ROOT / "include/constants/species.h"

SPECIES_MARKER = re.compile(
    r"(?m)^\s*\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{"
)
SPECIES_ALIAS = re.compile(
    r"(?m)^\s*(SPECIES_[A-Z0-9_]+)\s*=\s*(SPECIES_[A-Z0-9_]+)\s*,"
)
ABILITY_LIST = re.compile(r"\.abilities\s*=\s*\{([^}]*)\}", re.DOTALL)
ABILITY_TOKEN = re.compile(r"ABILITY_[A-Z0-9_]+")


@dataclass(frozen=True)
class AuthoredAbility:
    line: int
    trainer: str
    species: str
    ability: str


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def preprocess_species_info() -> str:
    compiler = shutil.which("cc") or shutil.which("clang") or shutil.which("gcc")
    require(compiler is not None, "a host C preprocessor (cc, clang, or gcc) is required")
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


def configured_species_abilities() -> dict[str, frozenset[str]]:
    text = preprocess_species_info()
    table_start = text.find("const struct SpeciesInfo gSpeciesInfo[]")
    require(table_start >= 0, "preprocessed source does not define gSpeciesInfo")
    text = text[table_start:]
    markers = list(SPECIES_MARKER.finditer(text))
    require(markers, "preprocessed gSpeciesInfo contains no species entries")

    abilities: dict[str, frozenset[str]] = {}
    for index, marker in enumerate(markers):
        end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
        block = text[marker.start():end]
        match = ABILITY_LIST.search(block)
        # SPECIES_NONE and a handful of reserved table entries deliberately
        # have no Ability data.  Any trainer that names one still fails below.
        abilities[marker.group(1)] = (
            frozenset(ABILITY_TOKEN.findall(match.group(1)))
            if match is not None
            else frozenset()
        )
    aliases = species_aliases()
    normalized = dict(abilities)
    for species, legal in abilities.items():
        normalized[resolve_species(species, aliases)] = legal
    return normalized


def species_aliases() -> dict[str, str]:
    return dict(SPECIES_ALIAS.findall(SPECIES_CONSTANTS.read_text()))


def resolve_species(species: str, aliases: dict[str, str]) -> str:
    seen: set[str] = set()
    while species in aliases:
        require(species not in seen, f"cyclic species alias involving {species}")
        seen.add(species)
        species = aliases[species]
    return species


def authored_abilities() -> list[AuthoredAbility]:
    result: list[AuthoredAbility] = []
    trainer = ""
    species = ""
    for line_number, line in enumerate(TRAINERS.read_text().splitlines(), 1):
        if line.startswith("=== "):
            trainer = line.strip("= ")
            species = ""
        elif line.startswith("SPECIES_"):
            species = line.split()[0]
        elif line.startswith("Ability:"):
            require(trainer != "", f"{TRAINERS}:{line_number}: Ability outside a trainer block")
            require(species != "", f"{TRAINERS}:{line_number}: Ability without a species")
            result.append(
                AuthoredAbility(
                    line=line_number,
                    trainer=trainer,
                    species=species,
                    ability=line.partition(":")[2].strip(),
                )
            )
    require(result, f"{TRAINERS} contains no authored Abilities")
    return result


def main() -> None:
    abilities = configured_species_abilities()
    aliases = species_aliases()
    authored = authored_abilities()
    failures: list[str] = []

    for entry in authored:
        configured_species = resolve_species(entry.species, aliases)
        legal = abilities.get(configured_species)
        if legal is None:
            failures.append(
                f"{(TRAINERS.relative_to(ROOT) if TRAINERS.is_relative_to(ROOT) else TRAINERS)}:{entry.line}: {entry.trainer}/{entry.species}: "
                f"configured species {configured_species} has no gSpeciesInfo entry"
            )
        elif entry.ability not in legal:
            failures.append(
                f"{(TRAINERS.relative_to(ROOT) if TRAINERS.is_relative_to(ROOT) else TRAINERS)}:{entry.line}: {entry.trainer}/{entry.species}: "
                f"illegal {entry.ability}; legal={','.join(sorted(legal))}"
            )

    require(
        not failures,
        f"{len(failures)} trainer Pokemon request an illegal Ability:\n" + "\n".join(failures),
    )
    print(
        "PASS: "
        f"{len(authored)} authored trainer Abilities are legal for the configured species/form tables"
    )


if __name__ == "__main__":
    main()
