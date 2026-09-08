"""Shared identity translation and verified sources for pinned Showdown imports."""
from __future__ import annotations

import hashlib
import re
import subprocess
from pathlib import Path

PINNED_COMMIT = "bb179fbf8449e3c31632bd56f671ffb4404fa6e7"
SOURCE_HASHES = {
    "data/random-battles/champions/teams.ts": "caf4a6c018ecddc0a492fe4a28d5ca745ff76ed25fb29dd857e3ad35411502e0",
    "data/random-battles/gen9/teams.ts": "71faeaf5f94ead62e26940d08fc23b2ed9dea32ad72c43d1e3be74ee5daaefa3",
    "data/random-battles/gen9/doubles-sets.json": "323e83026c32d9876bc295bc0e3481c834308d62b99ae8ec6362e4278f2ad000",
    "data/random-battles/champions/doubles-sets.json": "851114e68805aafbecbed0aaee7994164d199d890deed78bafc2beaf3c2221d8",
    "data/random-battles/champions/sets.json": "7b189d6de33367aca7191e484069b74757097fc34fed0402b52bb6fa41447421",
    "data/random-battles/gen9/sets.json": "d18992314222060dda9a2a9bea09331478991d469babd95662517668099669f9",
    "data/learnsets.ts": "725fe7c63817ee2847a21e2e49b195035341adc75c389db73172773e630c3feb",
    "data/mods/champions/learnsets.ts": "78fc395c01406008e054f63b168fe0b4ec7f7fec6ff6454f4d14a502147d2a9b",
    "data/pokedex.ts": "1ef21d85befbcba8111b13827e04b54c8a689679a465552d27b386969c9f8a44",
}

# The Inclement-derived Ability adaptations are identical for both formats.
ABILITY_OVERRIDES = {
    ("SPECIES_MEGANIUM", "ABILITY_LEAF_GUARD"): "ABILITY_TRIAGE",
    ("SPECIES_TORTERRA", "ABILITY_SHELL_ARMOR"): "ABILITY_SOLID_ROCK",
    ("SPECIES_ROTOM_FAN", "ABILITY_LEVITATE"): "ABILITY_MOTOR_DRIVE",
    ("SPECIES_PYROAR", "ABILITY_UNNERVE"): "ABILITY_COMPETITIVE",
    ("SPECIES_GOODRA", "ABILITY_SAP_SIPPER"): "ABILITY_GOOEY",
    ("SPECIES_GOURGEIST", "ABILITY_FRISK"): "ABILITY_INSOMNIA",
    ("SPECIES_GOURGEIST_SMALL", "ABILITY_FRISK"): "ABILITY_INSOMNIA",
    ("SPECIES_GOURGEIST_LARGE", "ABILITY_FRISK"): "ABILITY_INSOMNIA",
    ("SPECIES_GOURGEIST_SUPER", "ABILITY_FRISK"): "ABILITY_INSOMNIA",
}


def to_id(value: str) -> str:
    return re.sub(r"[^a-z0-9]", "", value.lower())


def constants(path: Path, prefix: str) -> dict[str, str]:
    tokens = set(re.findall(rf"\b{prefix}[A-Z0-9_]+\b", path.read_text()))
    result: dict[str, str] = {}
    for token in sorted(tokens):
        result.setdefault(to_id(token[len(prefix):]), token)
    return result


def mega_suffix(species_id: str) -> str | None:
    for suffix in ("megax", "megay", "megaz", "mega"):
        if species_id.endswith(suffix) and species_id != "meganium":
            return suffix
    return None


def verify_checkout(root: Path) -> None:
    commit = subprocess.check_output(["git", "-C", str(root), "rev-parse", "HEAD"], text=True).strip()
    if commit != PINNED_COMMIT:
        raise ValueError(f"expected Showdown {PINNED_COMMIT}, found {commit}")


def read_pinned_source(root: Path, relative: str) -> bytes:
    data = (root / relative).read_bytes()
    digest = hashlib.sha256(data).hexdigest()
    if digest != SOURCE_HASHES[relative]:
        raise ValueError(f"Showdown source drifted: {relative}: {digest} != {SOURCE_HASHES[relative]}")
    return data
