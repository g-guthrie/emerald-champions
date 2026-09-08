#!/usr/bin/env python3
"""Convert pinned Showdown Champions doubles data into compact GBA tables."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
from collections import defaultdict
from pathlib import Path


from showdown_import import ABILITY_OVERRIDES, PINNED_COMMIT, SOURCE_HASHES, constants, mega_suffix, read_pinned_source, to_id, verify_checkout
from verify_trainer_ability_legality import configured_species_abilities, resolve_species, species_aliases

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "data/emerald_champions/showdown_champions_random_doubles.json"
C_OUTPUT = ROOT / "src" / "data" / "pokemon" / "showdown_champions_circuit.h"
COUNTS_OUTPUT = ROOT / "include/showdown_champions_circuit.h"
SOURCE_FILE = "data/random-battles/champions/doubles-sets.json"
GEN9_SOURCE_FILE = "data/random-battles/gen9/doubles-sets.json"

ROLES = {
    "Bulky Protect": "SHOWDOWN_ROLE_BULKY_ATTACKER",
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
    from generate_showdown_champions_learnsets import top_level_entries
    from generate_emerald_champions_battle_sets import species_build_metadata
    verify_checkout(showdown_root)
    sources = {name: read_pinned_source(showdown_root, name)
               for name in (SOURCE_FILE, GEN9_SOURCE_FILE, "data/pokedex.ts",
                            "data/random-battles/champions/teams.ts", "data/random-battles/gen9/teams.ts")}
    raw = json.loads(sources[SOURCE_FILE])
    origins = dict.fromkeys(raw, SOURCE_FILE)
    for species, value in json.loads(sources[GEN9_SOURCE_FILE]).items():
        if species not in raw:
            raw[species] = value
            origins[species] = GEN9_SOURCE_FILE
    pokedex = top_level_entries(sources["data/pokedex.ts"].decode())
    species_map = constants(ROOT / "include/constants/species.h", "SPECIES_")
    move_map = constants(ROOT / "include/constants/moves.h", "MOVE_")
    ability_map = constants(ROOT / "include/constants/abilities.h", "ABILITY_")
    ability_map.update(asoneglastrier="ABILITY_AS_ONE_ICE_RIDER", asonespectrier="ABILITY_AS_ONE_SHADOW_RIDER")
    item_map = constants(ROOT / "include/constants/items.h", "ITEM_")
    type_map = constants(ROOT / "include/constants/pokemon.h", "TYPE_")
    aliases = species_aliases()
    metadata = species_build_metadata()
    form_text = (ROOT / "src/data/pokemon/form_change_tables.h").read_text()
    mega_items = dict(re.findall(
        r"FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM,\s*(SPECIES_[A-Z0-9_]+),\s*(ITEM_[A-Z0-9_]+)", form_text))
    authored_path = ROOT / "data/emerald_champions/emerald_champions_battle_sets.json"
    authored = json.loads(authored_path.read_text())
    authored_by_species = defaultdict(list)
    for entry in authored["defaults"] + authored["alternatives"]:
        authored_by_species[resolve_species(entry["species"], aliases)].append(entry)

    variants, templates, omitted = [], [], []
    represented = set()

    def add_variant(species_id, party, form, item, rows, source):
        if not rows:
            return
        canonical = resolve_species(party, aliases)
        key = (canonical, form, item)
        if key in represented:
            return
        represented.add(key)
        variants.append({
            "showdown_id": species_id, "party_species": party,
            "form_species": form, "required_item": item,
            "template_offset": len(templates), "template_count": len(rows),
            "compatibility_flags": compatibility_flags(species_id), "source": source,
        })
        templates.extend(rows)

    for species_id, species_data in raw.items():
        if species_id not in species_map:
            omitted.append({"id": species_id, "reason": "no matching engine form"})
            continue
        form = species_map[species_id]
        suffix = mega_suffix(species_id) if form in mega_items else None
        party = species_map[species_id[:-len(suffix)]] if suffix else form
        info = metadata.get(party, {})
        item = mega_items[form] if suffix else "ITEM_NONE"
        required = re.search(r'requiredItems?:\s*\[?["\']([^"\']+)', pokedex.get(species_id, ""))
        if required and not suffix:
            item = item_map[to_id(required.group(1))]
        rows = []
        for source_set in species_data["sets"]:
            if source_set["role"] == "Tera Blast user":
                omitted.append({"id": species_id, "reason": "Tera-dependent role; no Tera in this format"})
                continue
            abilities = [ABILITY_OVERRIDES.get((party, ability_map[to_id(a)]), ability_map[to_id(a)])
                         for a in source_set["abilities"]]
            abilities = [a for a in abilities if a in info.get("abilities", ())]
            # Unsupported Ability adaptations use the existing authored fallback
            # below, never an arbitrary Ability unrelated to the upstream role.
            if not abilities:
                omitted.append({"id": species_id, "reason": "no configured Ability for role"})
                continue
            moves = [move_map[to_id(m)] for m in source_set["movepool"] if to_id(m) != "terablast"]
            if len(moves) < 4 and len(source_set["movepool"]) >= 4:
                omitted.append({"id": species_id, "reason": "incomplete role after removing Tera Blast"})
                continue
            preferred = source_set.get("preferredTypes", [])
            rows.append({"role": ROLES[source_set["role"]], "moves": moves,
                         "abilities": abilities,
                         "preferred_type": type_map[to_id(preferred[0])] if preferred else "TYPE_NONE"})
        add_variant(species_id, party, form, item, rows, origins[species_id])

    # Legacy species absent from the two upstream pools retain their reviewed
    # individual doubles sets. No opponent teams are stored or precomputed.
    engine_ids = {resolve_species(token, aliases): key for key, token in species_map.items()
                  if key in pokedex and "baseSpecies:" not in pokedex[key]}
    present = {resolve_species(v["party_species"], aliases) for v in variants}
    for species, entries in authored_by_species.items():
        info = metadata.get(species, {})
        species_id = engine_ids.get(species, to_id(species.removeprefix("SPECIES_")))
        body = pokedex.get(species_id, "")
        if species in present or not info or (info.get("evolves") and species_id not in raw):
            continue
        # Cosmetic/battle-only formes are represented by their ordinary owner;
        # the upstream datasets explicitly select other competitively useful forms.
        if not body or "battleOnly:" in body or mega_suffix(species_id) or "baseSpecies:" in body:
            continue
        rows = []
        for entry in entries:
            if entry["required_item"] != "ITEM_NONE" or entry["ability"] not in info["abilities"]:
                continue
            rows.append({"role": "SHOWDOWN_ROLE_SUPPORT", "moves": entry["moves"],
                         "abilities": [entry["ability"]], "preferred_type": "TYPE_NONE",
                         "authored": True, "item": entry["item"], "nature": entry["nature"],
                         "stat_points": [entry["stat_points"][i] for i in (0, 1, 2, 5, 3, 4)],
                         "dependency": "CIRCUIT_DEPENDENCY_" + (entry.get("field_dependency") or "none").upper().replace("-", "_"),
                         "name": entry["name"]})
        add_variant(species_id, species, species, "ITEM_NONE", rows, "authored doubles supplement")

    def dex_number(variant):
        body = pokedex.get(variant["showdown_id"], "")
        match = re.search(r"num: (\d+)", body)
        if not match:
            raise ValueError("missing National Dex identity: " + variant["showdown_id"])
        return int(match.group(1))

    # The ROM samples families, not forms; its linear family scan requires
    # contiguous National Dex groups after merging sources.
    variants.sort(key=lambda v: (dex_number(v), v["showdown_id"]))
    manifest = {
        "schema_version": 2,
        "source": "Pokemon Showdown Champions and Gen 9 random doubles with legacy doubles supplements",
        "source_commit": PINNED_COMMIT, "source_file": SOURCE_FILE,
        "source_sha256": hashlib.sha256(sources[SOURCE_FILE]).hexdigest(),
        "source_files": {name: hashlib.sha256(data).hexdigest() for name, data in sources.items()},
        "supplement_source_sha256": hashlib.sha256(authored_path.read_bytes()).hexdigest(),
        "license": "MIT; copyright 2011-2026 Guangcong Luo and other contributors",
        "policy": {
            "runtime": "teams and moves are selected on demand in the GBA ROM",
            "adaptations": "Champions mechanics; no Tera-dependent roles; form items retained; legacy individual doubles sets",
            "eligibility": "upstream competitive roster plus fully evolved legacy species; no blanket weak unevolved filler",
            "ability_overrides": {f"{species}/{ability}": replacement
                for (species, ability), replacement in sorted(ABILITY_OVERRIDES.items())},
        },
        "variant_count": len(variants), "template_count": len(templates),
        "omitted_roles": omitted, "variants": variants, "templates": templates,
    }
    return manifest, manifest["source_sha256"]


def render_c(manifest: dict) -> str:
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
        moves = entry["moves"] + ["MOVE_NONE"] * (9 - len(entry["moves"]))
        abilities = entry["abilities"] + ["ABILITY_NONE"] * (3 - len(entry["abilities"]))
        lines.extend([
            "    {",
            "        .moves = {" + ", ".join(moves) + "},",
            "        .abilities = {" + ", ".join(abilities) + "},",
            f"        .preferredType = {entry['preferred_type']},",
            f"        .role = {entry['role']},",
            f"        .moveCount = {len(entry['moves'])},",
            f"        .abilityCount = {len(entry['abilities'])},",
            *( ["        .authored = TRUE,", f"        .item = {entry['item']},",
                 f"        .nature = {entry['nature']},",
                 f"        .dependency = {entry['dependency']},",
                 "        .statPoints = {" + ", ".join(map(str, entry["stat_points"])) + "},"]
               if entry.get("authored") else [] ),
            "    },",
        ])
    lines.extend(["};", ""])
    return "\n".join(lines)


def validate_manifest(manifest: dict) -> None:
    if (manifest["source_commit"] != PINNED_COMMIT
        or manifest["source_file"] != SOURCE_FILE
        or manifest["source_sha256"] != SOURCE_HASHES[SOURCE_FILE]):
        raise ValueError("Circuit manifest provenance does not match the pinned source")
    if manifest.get("schema_version") == 2:
        for name in (SOURCE_FILE, GEN9_SOURCE_FILE, "data/pokedex.ts",
                            "data/random-battles/champions/teams.ts", "data/random-battles/gen9/teams.ts"):
            if manifest.get("source_files", {}).get(name) != SOURCE_HASHES[name]:
                raise ValueError("Circuit combined source provenance mismatch: " + name)
    for kind in ("variant", "template"):
        if manifest[kind + "_count"] != len(manifest[kind + "s"]):
            raise ValueError(f"Circuit {kind} count does not match its records")
    for template in manifest["templates"]:
        if not (1 <= len(template["moves"]) <= 9 and 1 <= len(template["abilities"]) <= 3):
            raise ValueError("Circuit template exceeds the runtime move/Ability bounds")
        if len(set(template["moves"])) != len(template["moves"]) or "MOVE_NONE" in template["moves"]:
            raise ValueError("Circuit template contains duplicate or empty moves")
        if template.get("authored"):
            points = template.get("stat_points", [])
            if len(template["moves"]) > 4 or len(points) != 6 or sum(points) > 66 or any(type(p) is not int or p < 0 or p > 32 for p in points):
                raise ValueError("Circuit authored supplement violates the competitive set budget")
            if template.get("dependency") not in {
                "CIRCUIT_DEPENDENCY_" + name for name in
                ("NONE", "RAIN", "SUN", "SAND", "SNOW", "TRICK_ROOM", "TERRAIN", "GRAVITY")
            }:
                raise ValueError("Circuit authored supplement has an unknown field dependency")
    configured_abilities = configured_species_abilities()
    aliases = species_aliases()
    for variant in manifest["variants"]:
        if not (0 <= variant["template_offset"] < len(manifest["templates"])
                and 0 < variant["template_count"] <= len(manifest["templates"]) - variant["template_offset"]):
            raise ValueError("Circuit template range is outside the manifest")
        legal = configured_abilities.get(resolve_species(variant["party_species"], aliases), frozenset())
        start = variant["template_offset"]
        stop = start + variant["template_count"]
        for index, template in enumerate(manifest["templates"][start:stop], start):
            illegal = set(template["abilities"]) - legal
            if illegal:
                raise ValueError(
                    f"Circuit {variant['party_species']} template {index} requests "
                    f"unconfigured Abilities: {', '.join(sorted(illegal))}"
                )


def render_counts(manifest: dict, header: str) -> str:
    counts = ("// Generated counts: scripts/generate_showdown_champions_circuit.py\n"
              f"#define SHOWDOWN_CIRCUIT_VARIANT_COUNT {len(manifest['variants'])}\n"
              f"#define SHOWDOWN_CIRCUIT_TEMPLATE_COUNT {len(manifest['templates'])}\n")
    rendered, matches = re.subn(
        r"(?m)(?:// Generated counts: scripts/generate_showdown_champions_circuit\.py\n)?"
        r"#define SHOWDOWN_CIRCUIT_VARIANT_COUNT [^\n]+\n"
        r"#define SHOWDOWN_CIRCUIT_TEMPLATE_COUNT [^\n]+\n", counts, header)
    if matches != 1:
        raise ValueError("Circuit header must have exactly one count declaration pair")
    return rendered


def project(manifest: dict, *, check: bool, c_output: Path = C_OUTPUT, counts_output: Path = COUNTS_OUTPUT) -> None:
    validate_manifest(manifest)
    outputs = {c_output: render_c(manifest), counts_output: render_counts(manifest, counts_output.read_text())}
    if check:
        stale = [str(path) for path, rendered in outputs.items() if path.read_text() != rendered]
        if stale:
            raise ValueError("Circuit generated outputs are stale: " + ", ".join(stale))
    else:
        for path, rendered in outputs.items():
            path.write_text(rendered)


def main() -> None:
    parser = argparse.ArgumentParser()
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--showdown-root", type=Path, help="explicitly import verified pinned upstream data")
    mode.add_argument("--check", action="store_true", help="compare local manifest projections without writing")
    args = parser.parse_args()
    if args.showdown_root is not None:
        manifest, _ = build(args.showdown_root)
    else:
        manifest = json.loads(MANIFEST.read_text())
    project(manifest, check=args.check)
    if args.showdown_root is not None:
        MANIFEST.write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"{'checked' if args.check else 'generated'} {manifest['variant_count']} variants and {manifest['template_count']} templates")


if __name__ == "__main__":
    main()
