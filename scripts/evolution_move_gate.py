#!/usr/bin/env python3
"""The tutor's evolution-move gate, for the manifest and pool models.

Mirrors IsEmeraldChampionsEvolutionMoveLocked (src/emerald_champions_battle_sets.c):
a move that would evolve its learner (an IF_KNOWS_MOVE evolution condition)
is taught only from the level at which the species learns it naturally; a
trigger move the species never learns by level waits for the fourth badge.
"""
from __future__ import annotations

import re
from functools import lru_cache
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SPECIES_INFO = ROOT / "src/data/pokemon/species_info"
LEARNSETS = ROOT / "src/data/pokemon/level_up_learnsets"
BADGE4 = "FLAG_BADGE04_GET"


@lru_cache(maxsize=None)
def _learnset_symbols() -> dict[str, str]:
    """SPECIES_X -> its level-up learnset array name."""
    out = {}
    for path in sorted(SPECIES_INFO.glob("*.h")):
        for species, body in re.findall(r"\[(SPECIES_\w+)\]\s*=\s*\{(.*?)\n    \},", path.read_text(), re.S):
            match = re.search(r"\.levelUpLearnset\s*=\s*(\w+)", body)
            if match:
                out[species] = match.group(1)
    return out


@lru_cache(maxsize=None)
def _learnsets() -> dict[str, dict[str, int]]:
    """Learnset array name -> {MOVE_X: first level it is learned}."""
    out = {}
    for path in sorted(LEARNSETS.glob("*.h")):
        for name, body in re.findall(r"static const struct LevelUpMove (\w+)\[\] = \{(.*?)\};", path.read_text(), re.S):
            moves: dict[str, int] = {}
            for level, move in re.findall(r"LEVEL_UP_MOVE\(\s*(\d+),\s*(MOVE_\w+)\)", body):
                moves.setdefault(move, int(level))
            out[name] = moves
    return out


def natural_level(species: str, move: str) -> int | None:
    """The level at which `species` learns `move` by level-up, or None."""
    symbol = _learnset_symbols().get(species)
    return _learnsets().get(symbol, {}).get(move) if symbol else None


def ready(species: str, move: str, level: int, flags) -> bool:
    """Can a `level` Pokemon of `species` be taught the trigger `move`?"""
    natural = natural_level(species, move)
    if natural is None:
        return BADGE4 in flags
    return level >= natural
