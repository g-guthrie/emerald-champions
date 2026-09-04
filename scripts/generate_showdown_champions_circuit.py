#!/usr/bin/env python3
"""Convert pinned Showdown Champions doubles data into compact GBA tables."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PINNED_COMMIT = "bb179fbf8449e3c31632bd56f671ffb4404fa6e7"
DEFAULT_SHOWDOWN = Path("/private/tmp/showdown-champions-audit.oiAZXl/repo")
MANIFEST = ROOT / "data/emerald_champions/showdown_champions_random_doubles.json"
C_OUTPUT = ROOT / "src" / "data" / "pokemon" / "showdown_champions_circuit.h"

ROLES = {
    "Offensive Protect": "SHOWDOWN_ROLE_OFFENSIVE_PROTECT",
    "Doubles Support": "SHOWDOWN_ROLE_SUPPORT",
    "Doubles Bulky Setup": "SHOWDOWN_ROLE_BULKY_SETUP",
    "Doubles Bulky Attacker": "SHOWDOWN_ROLE_BULKY_ATTACKER",
    "Doubles Setup Sweeper": "SHOWDOWN_ROLE_SETUP_SWEEPER",
    "Choice Item user": "SHOWDOWN_ROLE_CHOICE_ITEM",
    "Doubles Wallbreaker": "SHOWDOWN_ROLE_WALLBREAKER",
    "Doubles Fast Attacker": "SHOWDOWN_ROLE_FAST_ATTACKER",
}

COMPATIBILITY_FLAGS = {
    "web": "SHOWDOWN_COMPAT_WEB_SETTER",
    "screen": "SHOWDOWN_COMPAT_SCREEN_SETTER",
    "screen_cleaner": "SHOWDOWN_COMPAT_SCREEN_CLEANER",
    "dry_skin_sun": "SHOWDOWN_COMPAT_DRY_SKIN_SUN",
    "lightning_rod": "SHOWDOWN_COMPAT_LIGHTNING_ROD",
    "sun": "SHOWDOWN_COMPAT_SUN_SETTER",
    "rain": "SHOWDOWN_COMPAT_RAIN_SETTER",
    "sand": "SHOWDOWN_COMPAT_SAND_SETTER",
    "snow": "SHOWDOWN_COMPAT_SNOW_SETTER",
}

# Showdown's pinned Champions data uses the official Ability roster. Emerald
# Champions deliberately retains a small Inclement-derived rebalance layer, so
# these replaced Abilities must be translated rather than silently falling
# back to party Ability slot zero at runtime.
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


def compatibility_flags(species_id: str) -> list[str]:
    flags: list[str] = []
    groups = {
        "web": {"ariados", "slurpuff", "araquanid"},
        "screen": {"ninetalesalola", "abomasnow", "abomasnowmega", "froslassmega", "vanilluxe", "aurorus", "grimmsnarl", "meowstic", "klefki"},
        "screen_cleaner": {"mrrime"},
        "dry_skin_sun": {"toxicroak", "heliolisk"},
        "lightning_rod": {"pikachu", "raichu", "manectric"},
        "sun": {"charizardmegay", "ninetales", "torkoal"},
        "rain": {"politoed", "pelipper"},
        "sand": {"tyranitar", "tyranitarmega", "hippowdon"},
        "snow": {"ninetalesalola", "abomasnow", "abomasnowmega", "froslassmega", "vanilluxe", "aurorus"},
    }
    for group, members in groups.items():
        if species_id in members:
            flags.append(COMPATIBILITY_FLAGS[group])
    return flags


def build(showdown_root: Path) -> tuple[dict, str]:
    source = showdown_root / "data" / "random-battles" / "champions" / "doubles-sets.json"
    raw_bytes = source.read_bytes()
    raw = json.loads(raw_bytes)
    species_map = constants(ROOT / "include" / "constants" / "species.h", "SPECIES_")
    move_map = constants(ROOT / "include" / "constants" / "moves.h", "MOVE_")
    ability_map = constants(ROOT / "include" / "constants" / "abilities.h", "ABILITY_")
    type_map = constants(ROOT / "include" / "constants" / "pokemon.h", "TYPE_")
    form_text = (ROOT / "src" / "data" / "pokemon" / "form_change_tables.h").read_text()
    mega_items = dict(re.findall(
        r"FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM,\s*(SPECIES_[A-Z0-9_]+),\s*(ITEM_[A-Z0-9_]+)",
        form_text,
    ))

    variants: list[dict] = []
    templates: list[dict] = []
    for species_id, species_data in raw.items():
        form_species = species_map[species_id]
        suffix = mega_suffix(species_id)
        if suffix:
            base_id = species_id[:-len(suffix)]
            party_species = species_map[base_id]
            required_item = mega_items[form_species]
        else:
            party_species = form_species
            required_item = "ITEM_NONE"

        offset = len(templates)
        for source_set in species_data["sets"]:
            preferred = source_set.get("preferredTypes", [])
            abilities = [ability_map[to_id(ability)] for ability in source_set.get("abilities", [])]
            abilities = [ABILITY_OVERRIDES.get((party_species, ability), ability) for ability in abilities]
            template = {
                "role": ROLES[source_set["role"]],
                "moves": [move_map[to_id(move)] for move in source_set["movepool"]],
                "abilities": abilities,
                "preferred_type": type_map[to_id(preferred[0])] if preferred else "TYPE_NONE",
            }
            templates.append(template)
        variants.append({
            "showdown_id": species_id,
            "party_species": party_species,
            "form_species": form_species,
            "required_item": required_item,
            "template_offset": offset,
            "template_count": len(species_data["sets"]),
            "compatibility_flags": compatibility_flags(species_id),
        })

    assert len(variants) == 311
    assert len(templates) == 444
    assert all(1 <= len(entry["moves"]) <= 8 for entry in templates)
    assert all(1 <= len(entry["abilities"]) <= 2 for entry in templates)
    manifest = {
        "schema_version": 1,
        "source": "Pokemon Showdown Champions random doubles",
        "source_commit": PINNED_COMMIT,
        "source_file": "data/random-battles/champions/doubles-sets.json",
        "source_sha256": hashlib.sha256(raw_bytes).hexdigest(),
        "license": "MIT; copyright 2011-2026 Guangcong Luo and other contributors",
        "policy": {
            "runtime": "teams and moves are selected on demand in the GBA ROM",
            "adaptations": "Circuit level escalation, Emerald AI, Mega-only selectable gimmick",
            "ability_overrides": {
                f"{species}/{ability}": replacement
                for (species, ability), replacement in sorted(ABILITY_OVERRIDES.items())
            },
        },
        "variant_count": len(variants),
        "template_count": len(templates),
        "variants": variants,
        "templates": templates,
    }
    return manifest, hashlib.sha256(raw_bytes).hexdigest()


def write_c(manifest: dict) -> None:
    lines = [
        "// Generated by scripts/generate_showdown_champions_circuit.py. Do not edit.",
        "// Derived from Pokemon Showdown at commit " + PINNED_COMMIT + ".",
        "// MIT License; copyright 2011-2026 Guangcong Luo and other contributors.",
        "",
        "const struct ShowdownCircuitVariant gShowdownCircuitVariants[SHOWDOWN_CIRCUIT_VARIANT_COUNT] =",
        "{",
    ]
    for entry in manifest["variants"]:
        flags = " | ".join(entry["compatibility_flags"]) or "0"
        lines.extend([
            "    {",
            f"        .partySpecies = {entry['party_species']},",
            f"        .formSpecies = {entry['form_species']},",
            f"        .requiredItem = {entry['required_item']},",
            f"        .templateOffset = {entry['template_offset']},",
            f"        .templateCount = {entry['template_count']},",
            f"        .compatibilityFlags = {flags},",
            "    },",
        ])
    lines.extend([
        "};",
        "",
        "const struct ShowdownCircuitTemplate gShowdownCircuitTemplates[SHOWDOWN_CIRCUIT_TEMPLATE_COUNT] =",
        "{",
    ])
    for entry in manifest["templates"]:
        moves = entry["moves"] + ["MOVE_NONE"] * (8 - len(entry["moves"]))
        abilities = entry["abilities"] + ["ABILITY_NONE"] * (2 - len(entry["abilities"]))
        lines.extend([
            "    {",
            "        .moves = {" + ", ".join(moves) + "},",
            "        .abilities = {" + ", ".join(abilities) + "},",
            f"        .preferredType = {entry['preferred_type']},",
            f"        .role = {entry['role']},",
            f"        .moveCount = {len(entry['moves'])},",
            f"        .abilityCount = {len(entry['abilities'])},",
            "    },",
        ])
    lines.extend(["};", ""])
    C_OUTPUT.write_text("\n".join(lines))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--showdown-root", type=Path, default=DEFAULT_SHOWDOWN)
    args = parser.parse_args()
    manifest, _ = build(args.showdown_root)
    MANIFEST.write_text(json.dumps(manifest, indent=2) + "\n")
    write_c(manifest)
    print(f"generated {manifest['variant_count']} variants and {manifest['template_count']} templates")


if __name__ == "__main__":
    main()
