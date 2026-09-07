#!/usr/bin/env python3
"""Preserve pinned Pokemon Showdown Champions singles templates for the tutor."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
from pathlib import Path


from showdown_import import ABILITY_OVERRIDES, PINNED_COMMIT, constants, mega_suffix, read_pinned_source, to_id, verify_checkout

ROOT = Path(__file__).resolve().parents[1]
DATASETS = {
    "champions": {
        "source_file": "data/random-battles/champions/sets.json",
        "source_name": "Pokemon Showdown Champions random singles",
        "output": ROOT / "data/emerald_champions/showdown_champions_random_singles.json",
    },
    "gen9": {
        "source_file": "data/random-battles/gen9/sets.json",
        "source_name": "Pokemon Showdown Gen 9 random singles",
        "output": ROOT / "data/emerald_champions/showdown_gen9_random_singles.json",
    },
}

def aliases(path: Path, prefix: str) -> dict[str, str]:
    return dict(re.findall(
        rf"\b({prefix}[A-Z0-9_]+)\s*=\s*({prefix}[A-Z0-9_]+)\b",
        path.read_text(),
    ))


def build(showdown_root: Path, dataset: dict) -> dict:
    verify_checkout(showdown_root)
    raw_bytes = read_pinned_source(showdown_root, dataset["source_file"])
    source_sha256 = hashlib.sha256(raw_bytes).hexdigest()
    raw = json.loads(raw_bytes)
    species_map = constants(ROOT / "include" / "constants" / "species.h", "SPECIES_")
    move_map = constants(ROOT / "include" / "constants" / "moves.h", "MOVE_")
    move_aliases = aliases(ROOT / "include" / "constants" / "moves.h", "MOVE_")
    ability_map = constants(ROOT / "include" / "constants" / "abilities.h", "ABILITY_")
    type_map = constants(ROOT / "include" / "constants" / "pokemon.h", "TYPE_")
    form_text = (ROOT / "src" / "data" / "pokemon" / "form_change_tables.h").read_text()
    mega_items = dict(re.findall(
        r"FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM,\s*(SPECIES_[A-Z0-9_]+),\s*(ITEM_[A-Z0-9_]+)",
        form_text,
    ))

    variants: list[dict] = []
    templates: list[dict] = []
    skipped: list[str] = []
    for species_id, species_data in raw.items():
        if species_id not in species_map:
            skipped.append(species_id)
            continue
        form_species = species_map[species_id]
        suffix = mega_suffix(species_id)
        if suffix and form_species in mega_items:
            base_id = species_id[:-len(suffix)]
            if base_id not in species_map or form_species not in mega_items:
                skipped.append(species_id)
                continue
            party_species = species_map[base_id]
            required_item = mega_items[form_species]
        else:
            party_species = form_species
            required_item = "ITEM_NONE"

        offset = len(templates)
        for source_set in species_data["sets"]:
            moves = [
                move_aliases.get(move_map[to_id(move)], move_map[to_id(move)])
                for move in source_set["movepool"]
                if to_id(move) in move_map
            ]
            abilities = [
                ability_map[to_id(ability)]
                for ability in source_set.get("abilities", [])
                if to_id(ability) in ability_map
            ]
            abilities = [ABILITY_OVERRIDES.get((party_species, ability), ability) for ability in abilities]
            preferred = source_set.get("preferredTypes", [])
            if len(moves) < 4 or not abilities:
                continue
            templates.append({
                "role": source_set["role"],
                "moves": moves,
                "abilities": abilities,
                "preferred_type": type_map.get(to_id(preferred[0]), "TYPE_NONE") if preferred else "TYPE_NONE",
            })
        count = len(templates) - offset
        if count:
            variants.append({
                "showdown_id": species_id,
                "party_species": party_species,
                "form_species": form_species,
                "required_item": required_item,
                "template_offset": offset,
                "template_count": count,
            })

    return {
        "schema_version": 1,
        "source": dataset["source_name"],
        "source_commit": PINNED_COMMIT,
        "source_file": dataset["source_file"],
        "source_sha256": source_sha256,
        "license": "MIT; copyright 2011-2026 Guangcong Luo and other contributors",
        "policy": {
            "runtime": "deterministic named tutor presets derived from each ranked singles role",
            "adaptations": "Emerald move legality, fixed Champions Stat Points, Mega-only gimmick",
            "ability_overrides": {
                f"{species}/{ability}": replacement
                for (species, ability), replacement in sorted(ABILITY_OVERRIDES.items())
            },
        },
        "variant_count": len(variants),
        "template_count": len(templates),
        "skipped_showdown_ids": skipped,
        "variants": variants,
        "templates": templates,
    }


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--showdown-root", type=Path, required=True)
    parser.add_argument("--dataset", choices=sorted(DATASETS), default="champions")
    args = parser.parse_args()
    dataset = DATASETS[args.dataset]
    manifest = build(args.showdown_root, dataset)
    dataset["output"].write_text(json.dumps(manifest, indent=2) + "\n")
    print(
        f"generated {manifest['variant_count']} singles variants and "
        f"{manifest['template_count']} role templates"
    )


if __name__ == "__main__":
    main()
