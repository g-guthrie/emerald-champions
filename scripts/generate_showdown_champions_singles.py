#!/usr/bin/env python3
"""Preserve pinned Pokemon Showdown Champions singles templates for the tutor."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PINNED_COMMIT = "bb179fbf8449e3c31632bd56f671ffb4404fa6e7"
DEFAULT_SHOWDOWN = Path("/private/tmp/showdown-champions-audit.oiAZXl/repo")
DATASETS = {
    "champions": {
        "source_file": "data/random-battles/champions/sets.json",
        "source_sha256": "7b189d6de33367aca7191e484069b74757097fc34fed0402b52bb6fa41447421",
        "source_name": "Pokemon Showdown Champions random singles",
        "output": ROOT / "docs" / "showdown_champions_random_singles.json",
    },
    "gen9": {
        "source_file": "data/random-battles/gen9/sets.json",
        "source_sha256": "d18992314222060dda9a2a9bea09331478991d469babd95662517668099669f9",
        "source_name": "Pokemon Showdown Gen 9 random singles",
        "output": ROOT / "docs" / "showdown_gen9_random_singles.json",
    },
}

# Emerald Champions deliberately retains a small Inclement-derived Ability
# layer. Translate those identities at import time instead of silently falling
# back to Ability slot zero in the ROM.
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


def aliases(path: Path, prefix: str) -> dict[str, str]:
    return dict(re.findall(
        rf"\b({prefix}[A-Z0-9_]+)\s*=\s*({prefix}[A-Z0-9_]+)\b",
        path.read_text(),
    ))


def mega_suffix(species_id: str) -> str | None:
    for suffix in ("megax", "megay", "megaz", "mega"):
        if species_id.endswith(suffix) and species_id != "meganium":
            return suffix
    return None


def build(showdown_root: Path, dataset: dict) -> dict:
    source = showdown_root / dataset["source_file"]
    raw_bytes = source.read_bytes()
    source_sha256 = hashlib.sha256(raw_bytes).hexdigest()
    if source_sha256 != dataset["source_sha256"]:
        raise SystemExit(
            f"Singles source drifted: {source_sha256} != {dataset['source_sha256']}"
        )
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
    parser.add_argument("--showdown-root", type=Path, default=DEFAULT_SHOWDOWN)
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
