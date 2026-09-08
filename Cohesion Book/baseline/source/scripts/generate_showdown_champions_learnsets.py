#!/usr/bin/env python3
"""Freeze Showdown's Champions learnsets plus current-mainline fallbacks."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
from functools import lru_cache
from pathlib import Path


from showdown_import import PINNED_COMMIT, read_pinned_source, to_id, verify_checkout
ROOT = Path(__file__).resolve().parents[1]
DEFAULT_OUTPUT = ROOT / "data/emerald_champions/showdown_champions_learnsets.json"


def top_level_entries(text: str) -> dict[str, str]:
    markers = list(re.finditer(r"^\t([a-z0-9]+):\s*\{$", text, re.M))
    return {
        marker.group(1): text[marker.end():markers[index + 1].start() if index + 1 < len(markers) else len(text)]
        for index, marker in enumerate(markers)
    }


def moves_from_entries(entries: dict[str, str]) -> dict[str, set[str]]:
    return {
        species: set(re.findall(r"^\t\t\t([a-z0-9]+):\s*\[", body, re.M))
        for species, body in entries.items()
    }


def latest_generation_moves(entries: dict[str, str]) -> dict[str, set[str]]:
    result = {}
    for species, body in entries.items():
        sources_by_move = {}
        for move, sources in re.findall(r'^\t\t\t([a-z0-9]+):\s*\[([^\]]*)\]', body, re.M):
            generations = {int(value) for value in re.findall(r'[\"\']([1-9])', sources)}
            if generations:
                sources_by_move[move] = generations
        latest = max((generation for generations in sources_by_move.values() for generation in generations), default=0)
        result[species] = {move for move, generations in sources_by_move.items() if latest in generations}
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("showdown_root", type=Path)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    args = parser.parse_args()
    showdown = args.showdown_root.resolve()
    verify_checkout(showdown)

    base_path = showdown / "data" / "learnsets.ts"
    mod_path = showdown / "data" / "mods" / "champions" / "learnsets.ts"
    pokedex_path = showdown / "data" / "pokedex.ts"
    sources = {path: read_pinned_source(showdown, str(path.relative_to(showdown)))
               for path in (base_path, mod_path, pokedex_path)}
    base_entries = top_level_entries(sources[base_path].decode())
    mod_entries = top_level_entries(sources[mod_path].decode())
    pokedex_entries = top_level_entries(sources[pokedex_path].decode())
    base_moves = latest_generation_moves(base_entries)
    mod_moves = moves_from_entries(mod_entries)
    mod_inherits = {species for species, body in mod_entries.items() if re.search(r"^\t\tinherit:\s*true", body, re.M)}
    parents = {}
    pre_evolutions = {}
    for species, body in pokedex_entries.items():
        match = re.search(r"^\t\tbaseSpecies:\s*[\"']([^\"']+)", body, re.M)
        if match:
            parents[species] = to_id(match.group(1))
        match = re.search(r"^\t\tprevo:\s*[\"']([^\"']+)", body, re.M)
        if match:
            pre_evolutions[species] = to_id(match.group(1))

    @lru_cache(None)
    def own_moves(species: str) -> frozenset[str]:
        base = base_moves.get(species, set())
        if species not in mod_entries:
            return frozenset(base)
        mod = mod_moves.get(species, set())
        return frozenset(base | mod if species in mod_inherits else mod)

    @lru_cache(None)
    def legal_moves(species: str) -> frozenset[str]:
        moves = set(own_moves(species))
        parent = parents.get(species)
        if parent and parent != species:
            moves.update(legal_moves(parent))
        prevo = pre_evolutions.get(species)
        if prevo and prevo != species:
            moves.update(legal_moves(prevo))
        return frozenset(moves)

    species_ids = sorted(set(base_entries) | set(mod_entries) | set(pokedex_entries))
    payload = {
        "schema_version": 1,
        "source": "smogon/pokemon-showdown",
        "source_commit": PINNED_COMMIT,
        "license": "MIT; see THIRD_PARTY_NOTICES.md",
        "policy": "Champions mod overrides for supported species; each other species uses its most recent official mainline generation in Showdown; forms and evolved species inherit legal moves from their base form and pre-evolution chains.",
        "source_files": {
            str(path.relative_to(showdown)): hashlib.sha256(sources[path]).hexdigest()
            for path in (base_path, mod_path, pokedex_path)
        },
        "champions_species": sorted(mod_entries),
        "form_parents": dict(sorted(parents.items())),
        "evolution_parents": dict(sorted(pre_evolutions.items())),
        "learnsets": {species: sorted(legal_moves(species)) for species in species_ids},
    }
    args.output.write_text(json.dumps(payload, indent=2, sort_keys=False) + "\n")
    print(f"wrote {args.output}")
    print(f"species={len(species_ids)} champions_species={len(mod_entries)}")


if __name__ == "__main__":
    main()
