#!/usr/bin/env python3
"""Verify the curated pre-League roster without turning it into a global quota."""

from __future__ import annotations

import glob
import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
POST_LEAGUE_MAP_PARTS = {
    "ALTERING_CAVE",
    "BATTLE_FRONTIER",
    "BIRTH_ISLAND",
    "FARAWAY_ISLAND",
    "MARINE_CAVE",
    "NAVEL_ROCK",
    "SOUTHERN_ISLAND",
    "TERRA_CAVE",
}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


class SpeciesGraph:
    def __init__(self) -> None:
        constants = (ROOT / "include/constants/species.h").read_text()
        self.species = set(re.findall(r"\bSPECIES_[A-Z0-9_]+\b", constants))
        self.parent = {species: species for species in self.species}

        for alias, target in re.findall(
            r"\b(SPECIES_[A-Z0-9_]+)\s*=\s*(SPECIES_[A-Z0-9_]+)", constants
        ):
            self.union(alias, target)

        for path in glob.glob(str(ROOT / "src/data/pokemon/species_info/*families.h")):
            source = Path(path).read_text()
            starts = list(re.finditer(r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*\{", source))
            for index, match in enumerate(starts):
                block = source[
                    match.start(): starts[index + 1].start() if index + 1 < len(starts) else len(source)
                ]
                evolution = re.search(r"\.evolutions\s*=\s*EVOLUTION\((.*?)\),\s*\n", block, re.S)
                if evolution:
                    for target in re.findall(r"\bSPECIES_[A-Z0-9_]+\b", evolution.group(1)):
                        self.union(match.group(1), target)

        form_source = (ROOT / "src/data/pokemon/form_change_tables.h").read_text()
        for block in re.findall(
            r"static const struct FormChange\s+[A-Za-z0-9_]+\[\]\s*=\s*\{(.*?)\n\};",
            form_source,
            re.S,
        ):
            forms = list(dict.fromkeys(re.findall(r"\bSPECIES_[A-Z0-9_]+\b", block)))
            for form in forms[1:]:
                self.union(forms[0], form)

        # These evolution families are authored through token-pasting macros,
        # so the lightweight source parser above cannot see their exact targets.
        self.union_group({
            species for species in self.species
            if species.startswith(("SPECIES_FLABEBE", "SPECIES_FLOETTE", "SPECIES_FLORGES"))
            and "MEGA" not in species
        })
        self.union_group({
            species for species in self.species
            if species in {"SPECIES_SCATTERBUG", "SPECIES_SPEWPA"}
            or species.startswith("SPECIES_VIVILLON")
        })
        self.union("SPECIES_ROCKRUFF_OWN_TEMPO", "SPECIES_LYCANROC_DUSK")

    def find(self, species: str) -> str:
        while self.parent[species] != species:
            self.parent[species] = self.parent[self.parent[species]]
            species = self.parent[species]
        return species

    def union(self, left: str, right: str) -> None:
        if left not in self.parent or right not in self.parent:
            return
        left_root = self.find(left)
        right_root = self.find(right)
        if left_root != right_root:
            self.parent[right_root] = left_root

    def union_group(self, species: set[str]) -> None:
        values = sorted(species)
        for value in values[1:]:
            self.union(values[0], value)


def map_is_allowed(map_name: str, pre_league: bool) -> bool:
    if "_FRLG" in map_name or "BATTLE_FRONTIER" in map_name:
        return False
    return not pre_league or not any(part in map_name for part in POST_LEAGUE_MAP_PARTS)


def direct_species(pre_league: bool) -> set[str]:
    result: set[str] = set()
    wild = json.loads((ROOT / "src/data/wild_encounters.json").read_text())
    for group in wild["wild_encounter_groups"]:
        if group.get("label") != "gWildMonHeaders":
            continue
        for entry in group["encounters"]:
            if not map_is_allowed(entry.get("map", ""), pre_league):
                continue
            for field, method in entry.items():
                if field.endswith("_mons") and isinstance(method, dict):
                    result.update(mon["species"] for mon in method.get("mons", []))

    paths = list((ROOT / "data/maps").glob("*/scripts.inc"))
    paths.extend((ROOT / "data/scripts").glob("*.inc"))
    for path in paths:
        map_name = path.parent.name.upper()
        if path.name == "debug.inc" or not map_is_allowed(map_name, pre_league):
            continue
        source = path.read_text()
        result.update(re.findall(
            r"\b(?:givemon|giveegg|setwildbattle)\s+(SPECIES_[A-Z0-9_]+)", source
        ))
        result.update(re.findall(
            r"\bsetvar\s+VAR_0x8004,\s*(SPECIES_[A-Z0-9_]+)", source
        ))

    # The native regional selector is itself the acquisition route for all
    # twenty-seven starter roots.
    result.update(re.findall(
        r"\bSPECIES_[A-Z0-9_]+\b", (ROOT / "src/starter_choose.c").read_text()
    ))
    if not pre_league:
        for line in (ROOT / "src/data/pokemon/legendary_signs.h").read_text().splitlines():
            if "LEGENDARY_SOURCE_CIRCUIT" in line:
                continue
            match = re.match(
                r"(?:WILD_SIGN|VISIBLE_SIGN|ORDINARY_WILD_SIGN|OTHER_SIGN)\("
                r"LEGENDARY_SIGN_[A-Z0-9_]+,\s*([A-Z0-9_]+)",
                line,
            )
            if match:
                result.add("SPECIES_" + match.group(1))
    return result


def kanto_species() -> set[str]:
    pokedex = (ROOT / "include/constants/pokedex.h").read_text()
    kanto = pokedex.split("// Kanto", 1)[1].split("// Johto", 1)[0]
    names = re.findall(r"NATIONAL_DEX_([A-Z0-9_]+)", kanto)
    return {f"SPECIES_{name}" for name in names}


def main() -> None:
    graph = SpeciesGraph()
    manifest = json.loads((ROOT / "docs/showdown_champions_random_doubles.json").read_text())

    pre_league_components = {
        graph.find(species) for species in direct_species(pre_league=True)
        if species in graph.species
    }
    champions_components = {
        graph.find(variant["party_species"])
        for variant in manifest["variants"]
    }
    missing_champions = champions_components - pre_league_components
    representatives = {}
    for variant in manifest["variants"]:
        representatives.setdefault(graph.find(variant["party_species"]), variant["party_species"])
    require(
        not missing_champions,
        "Champions families unavailable before the League: "
        + ", ".join(sorted(representatives[root] for root in missing_champions)),
    )

    campaign_components = {
        graph.find(species) for species in direct_species(pre_league=False)
        if species in graph.species
    }
    kanto = kanto_species()
    require(kanto <= graph.species, f"Kanto constants missing: {sorted(kanto - graph.species)}")
    missing_kanto = {graph.find(species) for species in kanto} - campaign_components
    kanto_representatives = {}
    for species in sorted(kanto):
        kanto_representatives.setdefault(graph.find(species), species)
    require(
        not missing_kanto,
        "Kanto families unavailable in the campaign: "
        + ", ".join(sorted(kanto_representatives[root] for root in missing_kanto)),
    )

    scripts = (ROOT / "data/scripts/emerald_champions.inc").read_text()
    mega_archive = (ROOT / "src/data/emerald_champions_mega_stones.h").read_text()
    evolution_archive = (ROOT / "src/data/emerald_champions_evolution_items.h").read_text()
    require(
        "goto_if_set FLAG_BADGE08_GET, EmeraldChampions_EventScript_BattleVendorCompleteArchive" in scripts,
        "complete team-building archives are not available before the League",
    )
    required_mega_items = {
        variant["required_item"] for variant in manifest["variants"]
        if variant["required_item"] != "ITEM_NONE"
    }
    require(
        required_mega_items <= set(re.findall(r"ITEM_[A-Z0-9_]+", mega_archive)),
        "Champions Mega Stones missing from the badge-eight archive: "
        + ", ".join(sorted(required_mega_items - set(re.findall(r"ITEM_[A-Z0-9_]+", mega_archive)))),
    )
    all_species_info = "\n".join(
        path.read_text()
        for path in sorted((ROOT / "src/data/pokemon/species_info").glob("*families.h"))
    )
    required_evolution_items = set(re.findall(
        r"\{EVO_ITEM(?:_MALE|_FEMALE|_DAY|_NIGHT)?\s*,\s*(ITEM_[A-Z0-9_]+)",
        all_species_info,
    ))
    require(
        required_evolution_items <= set(re.findall(r"ITEM_[A-Z0-9_]+", evolution_archive)),
        "evolution archive is incomplete",
    )

    sets_code = (ROOT / "src/emerald_champions_battle_sets.c").read_text()
    require(
        "RandomUniform(RNG_NONE, 0, count - 1)" in sets_code,
        "wild competitive presets are not sampled uniformly",
    )

    print(f"PASS: all {len(champions_components)} Champions families are obtainable before the League")
    print(f"PASS: all {len({graph.find(species) for species in kanto})} original Kanto families are obtainable")
    print(f"PASS: {len(required_mega_items)} Champions Mega Stones unlock with badge eight")
    print(f"PASS: {len(required_evolution_items)} evolution items unlock with badge eight")


if __name__ == "__main__":
    main()
