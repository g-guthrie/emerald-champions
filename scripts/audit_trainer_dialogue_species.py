#!/usr/bin/env python3
"""Reject live trainer dialogue that names an unrelated absent species.

It resolves actual trainerbattle commands, literal text labels, and materialized
trainer parties.  A tiny reviewed allowlist covers story dialogue where the
speaker is discussing the plot rather than claiming to use that Pokemon.
"""

from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]

# These are plot nouns, not claims about the speaker's own party.
APPROVED_CONTEXTUAL_MENTIONS = {
    ("TRAINER_TABITHA_MAGMA_HIDEOUT", "GROUDON"),
    ("TRAINER_GRUNT_WEATHER_INST_2", "CASTFORM"),
}


def species_names() -> dict[str, str]:
    names: dict[str, str] = {}
    for path in sorted((ROOT / "src/data/pokemon/species_info").glob("gen_*_families.h")):
        text = path.read_text(errors="ignore")
        markers = list(re.finditer(r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{", text))
        for index, marker in enumerate(markers):
            end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
            body = text[marker.end():end]
            name = re.search(r'\.speciesName\s*=\s*_\("([^"]+)"\)', body)
            if name is not None:
                names[marker.group(1)] = name.group(1)
    return names


def trainer_parties(names: dict[str, str]) -> dict[str, set[str]]:
    text = (ROOT / "src/data/trainers.party").read_text()
    markers = list(re.finditer(r"(?m)^=== (TRAINER_[A-Z0-9_]+) ===$", text))
    parties: dict[str, set[str]] = {}
    for index, marker in enumerate(markers):
        end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
        body = text[marker.end():end]
        species = re.findall(r"(?m)^(SPECIES_[A-Z0-9_]+)(?: @|$)", body)
        parties[marker.group(1)] = {
            names.get(value, value.removeprefix("SPECIES_").replace("_", " ").title()).upper()
            for value in species
        }
    return parties


def literal_labels() -> dict[str, str]:
    labels: dict[str, str] = {}
    sources = [
        path
        for path in (ROOT / "data/maps").rglob("scripts.inc")
        if "_Frlg" not in str(path)
    ]
    sources.extend(
        path
        for path in (ROOT / "data/scripts").glob("*.inc")
        if "frlg" not in path.name.lower()
    )
    sources.extend((ROOT / "data/text").glob("*.inc"))
    sources.append(ROOT / "data/event_scripts.s")
    for path in sources:
        text = path.read_text(errors="ignore")
        markers = list(re.finditer(r"(?m)^([A-Za-z_][A-Za-z0-9_]*):{1,2}\s*$", text))
        for index, marker in enumerate(markers):
            end = markers[index + 1].start() if index + 1 < len(markers) else len(text)
            strings = re.findall(r'\.string\s+"(.*)"', text[marker.end():end])
            if strings:
                labels[marker.group(1)] = " ".join(strings)
    return labels


def main() -> None:
    names = species_names()
    parties = trainer_parties(names)
    labels = literal_labels()
    candidate_names = sorted(
        {name.upper() for name in names.values() if len(name) >= 4},
        key=len,
        reverse=True,
    )
    species_pattern = re.compile(
        r"(?<![A-Z])(?:" + "|".join(map(re.escape, candidate_names)) + r")(?![A-Z])"
    )
    findings: list[str] = []
    sources = [
        path
        for path in (ROOT / "data/maps").rglob("scripts.inc")
        if "_Frlg" not in str(path)
    ]
    sources.extend(
        path
        for path in (ROOT / "data/scripts").rglob("*.inc")
        if "frlg" not in path.name.lower()
    )
    approved_seen: set[tuple[str, str]] = set()
    for path in sources:
        for line_number, line in enumerate(path.read_text(errors="ignore").splitlines(), 1):
            if "trainerbattle" not in line and "multi_2_vs_2" not in line:
                continue
            trainers = re.findall(r"\bTRAINER_[A-Z0-9_]+\b", line)
            text_labels = [
                token
                for token in re.findall(r"\b[A-Za-z_][A-Za-z0-9_]*\b", line)
                if "Text" in token
            ]
            if not trainers or not text_labels:
                continue
            literal = " ".join(labels.get(label, "") for label in text_labels).upper()
            party_names = {
                name
                for trainer in trainers
                for name in parties.get(trainer, set())
            }
            mentioned = {match.group(0) for match in species_pattern.finditer(literal)}
            mismatches = sorted(mentioned - party_names)
            unexpected = []
            for name in mismatches:
                keys = {(trainer, name) for trainer in trainers}
                approved = keys & APPROVED_CONTEXTUAL_MENTIONS
                if approved:
                    approved_seen.update(approved)
                else:
                    unexpected.append(name)
            if unexpected:
                findings.append(
                    f"{path.relative_to(ROOT)}:{line_number}: "
                    f"trainers={','.join(trainers)} "
                    f"mentions={','.join(unexpected)} "
                    f"party={','.join(sorted(party_names))}"
                )
    missing_approved = APPROVED_CONTEXTUAL_MENTIONS - approved_seen
    if missing_approved:
        findings.append(f"stale contextual allowlist entries={sorted(missing_approved)}")
    if findings:
        raise SystemExit(
            f"{len(findings)} trainer dialogue/species inconsistencies:\n"
            + "\n".join(findings)
        )
    print(
        "PASS: every live Hoenn trainer species mention matches its materialized "
        f"party or one of {len(approved_seen)} reviewed plot references"
    )


if __name__ == "__main__":
    main()
