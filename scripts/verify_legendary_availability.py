#!/usr/bin/env python3
"""Prove one obtainable Hoenn root for every legendary-class family."""

from __future__ import annotations

import glob
import json
import re
from pathlib import Path

from verify_emerald_champions_campaign_roster import SpeciesGraph, direct_species


ROOT = Path(__file__).resolve().parents[1]
EVOLUTION_ROOTS = {
    "NATIONAL_DEX_COSMOEM": "NATIONAL_DEX_COSMOG",
    "NATIONAL_DEX_SOLGALEO": "NATIONAL_DEX_COSMOG",
    "NATIONAL_DEX_LUNALA": "NATIONAL_DEX_COSMOG",
    "NATIONAL_DEX_MELMETAL": "NATIONAL_DEX_MELTAN",
    "NATIONAL_DEX_NAGANADEL": "NATIONAL_DEX_POIPOLE",
    "NATIONAL_DEX_URSHIFU": "NATIONAL_DEX_KUBFU",
}
NATIVE_ROOTS = {
    "NATIONAL_DEX_GROUDON": ("data/maps/TerraCave_End/scripts.inc", "SPECIES_GROUDON"),
    "NATIONAL_DEX_KYOGRE": ("data/maps/MarineCave_End/scripts.inc", "SPECIES_KYOGRE"),
    "NATIONAL_DEX_RAYQUAZA": ("data/maps/SkyPillar_Top/scripts.inc", "SPECIES_RAYQUAZA"),
    "NATIONAL_DEX_REGIROCK": ("data/maps/DesertRuins/scripts.inc", "SPECIES_REGIROCK"),
    "NATIONAL_DEX_REGICE": ("data/maps/IslandCave/scripts.inc", "SPECIES_REGICE"),
    "NATIONAL_DEX_REGISTEEL": ("data/maps/AncientTomb/scripts.inc", "SPECIES_REGISTEEL"),
    "NATIONAL_DEX_LATIAS": ("src/roamer.c", "SPECIES_LATIAS"),
    "NATIONAL_DEX_LATIOS": ("src/roamer.c", "SPECIES_LATIOS"),
    "NATIONAL_DEX_LUGIA": ("data/maps/NavelRock_Bottom/scripts.inc", "SPECIES_LUGIA"),
    "NATIONAL_DEX_HO_OH": ("data/maps/NavelRock_Top/scripts.inc", "SPECIES_HO_OH"),
    "NATIONAL_DEX_MEW": ("data/maps/FarawayIsland_Interior/scripts.inc", "SPECIES_MEW"),
    "NATIONAL_DEX_DEOXYS": ("data/maps/BirthIsland_Exterior/scripts.inc", "SPECIES_DEOXYS"),
    "NATIONAL_DEX_JIRACHI": ("data/maps/MeteorFalls_JirachisRoom/scripts.inc", "SPECIES_JIRACHI"),
    "NATIONAL_DEX_DIANCIE": ("data/maps/CaveOfOrigin_DianciesRoom/scripts.inc", "SPECIES_DIANCIE"),
    "NATIONAL_DEX_HEATRAN": ("data/maps/ScorchedSlab_HeatransRoom/scripts.inc", "SPECIES_HEATRAN"),
    "NATIONAL_DEX_MOLTRES": ("data/maps/EmberPath/scripts.inc", "SPECIES_MOLTRES"),
}
FIXED_INCLEMENT_GFX = {
    "ARTICUNO": "OBJ_EVENT_GFX_INCLEMENT_ARTICUNO",
    "ZAPDOS": "OBJ_EVENT_GFX_INCLEMENT_ZAPDOS",
    "MOLTRES": "OBJ_EVENT_GFX_INCLEMENT_MOLTRES",
    "MEWTWO": "OBJ_EVENT_GFX_INCLEMENT_MEWTWO",
    "JIRACHI": "OBJ_EVENT_GFX_INCLEMENT_JIRACHI",
    "HEATRAN": "OBJ_EVENT_GFX_INCLEMENT_HEATRAN",
    "DIANCIE": "OBJ_EVENT_GFX_INCLEMENT_DIANCIE",
    "REGIGIGAS": "OBJ_EVENT_GFX_REGIGIGAS_STATUE",
}
# These two LEGENDARY_SOURCE_VISIBLE entries intentionally use an existing
# environmental/NPC interaction instead of placing a species body in the map.
SCRIPTED_VISIBLE_ROOTS = {
    "MAGEARNA": (
        "data/maps/RustboroCity_DevonCorp_2F/scripts.inc",
        ("EC_SIGN_MAGEARNA_ID", "TryGiveSelectedLegendarySignReward", "FLAG_EC_CAUGHT_MAGEARNA"),
    ),
    "PECHARUNT": (
        "data/maps/MtPyre_6F/scripts.inc",
        ("MtPyre_6F_EventScript_Pecharunt", "CreateSelectedLegendarySignEncounter", "FLAG_EC_CAUGHT_PECHARUNT"),
    ),
}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def species_catalog() -> tuple[dict[str, str], dict[str, str], set[str]]:
    species_to_dex: dict[str, str] = {}
    representative: dict[str, str] = {}
    legendary_species: set[str] = set()
    for path in glob.glob(str(ROOT / "src/data/pokemon/species_info/*families.h")):
        source = Path(path).read_text()
        starts = list(re.finditer(r"\[(SPECIES_[A-Z0-9_]+)\]\s*=\s*", source))
        for index, match in enumerate(starts):
            block = source[match.start(): starts[index + 1].start() if index + 1 < len(starts) else len(source)]
            dex = re.search(r"\.natDexNum\s*=\s*(NATIONAL_DEX_[A-Z0-9_]+)", block)
            if dex is None:
                continue
            species = match.group(1)
            species_to_dex[species] = dex.group(1)
            representative.setdefault(dex.group(1), species)
            if re.search(r"\.is(?:RestrictedLegendary|SubLegendary|Mythical|UltraBeast)\s*=\s*TRUE", block):
                legendary_species.add(species)

    constants = (ROOT / "include/constants/species.h").read_text()
    for alias, target in re.findall(r"\b(SPECIES_[A-Z0-9_]+)\s*=\s*(SPECIES_[A-Z0-9_]+)", constants):
        if target in species_to_dex:
            species_to_dex[alias] = species_to_dex[target]
    return species_to_dex, representative, legendary_species


def wild_locations() -> tuple[dict[str, set[str]], dict[str, dict]]:
    payload = json.loads((ROOT / "src/data/wild_encounters.json").read_text())
    by_species: dict[str, set[str]] = {}
    by_map: dict[str, dict] = {}
    for group in payload["wild_encounter_groups"]:
        if group.get("label") != "gWildMonHeaders":
            continue
        for entry in group["encounters"]:
            by_map[entry["map"]] = entry
            for field, method in entry.items():
                if not field.endswith("_mons") or not isinstance(method, dict):
                    continue
                for mon in method.get("mons", []):
                    by_species.setdefault(mon["species"], set()).add(entry["map"])
    return by_species, by_map


def main() -> None:
    species_to_dex, representative, flagged_species = species_catalog()
    legendary_families = {species_to_dex[species] for species in flagged_species}
    definitions = (ROOT / "src/data/pokemon/legendary_signs.h").read_text()
    sign_rows = re.findall(
        r"(WILD_SIGN|VISIBLE_SIGN|OTHER_SIGN|ORDINARY_WILD_SIGN)\("
        r"(LEGENDARY_SIGN_[A-Z0-9_]+),\s*([A-Z0-9_]+)(?:,\s*([A-Z0-9_]+))?",
        definitions,
    )
    sign_species = {"SPECIES_" + species for _, _, species, _ in sign_rows}
    sign_families = {species_to_dex[species] for species in sign_species if species in species_to_dex}

    for dex, (relative, token) in NATIVE_ROOTS.items():
        require(token in (ROOT / relative).read_text(), f"native root {dex} is missing from {relative}")
    direct_families = sign_families | set(NATIVE_ROOTS)
    unresolved = {
        dex for dex in legendary_families
        if dex not in direct_families and EVOLUTION_ROOTS.get(dex) not in direct_families
    }
    require(not unresolved, f"legendary families lack acquisition roots: {sorted(unresolved)}")

    graph = SpeciesGraph()
    sign_dependencies = {}
    sign_min_badges = {}
    sign_sources = {}
    for line in definitions.splitlines():
        wild = re.match(
            r"WILD_SIGN\((LEGENDARY_SIGN_[A-Z0-9_]+),\s*([A-Z0-9_]+),.*?,\s*"
            r"(\d+),\s*(-?\d+),\s*([A-Z0-9_]+),\s*([A-Z0-9_]+)\),",
            line,
        )
        visible = re.match(
            r"VISIBLE_SIGN\((LEGENDARY_SIGN_[A-Z0-9_]+),\s*([A-Z0-9_]+),\s*[A-Z0-9_]+,\s*"
            r"(\d+),\s*-?\d+,\s*([A-Z0-9_]+),\s*([A-Z0-9_]+)\),",
            line,
        )
        other = re.match(
            r"OTHER_SIGN\((LEGENDARY_SIGN_[A-Z0-9_]+),\s*([A-Z0-9_]+),\s*(LEGENDARY_SOURCE_[A-Z0-9_]+)\),",
            line,
        )
        ordinary = re.match(
            r"ORDINARY_WILD_SIGN\((LEGENDARY_SIGN_[A-Z0-9_]+),\s*([A-Z0-9_]+),",
            line,
        )
        if wild:
            sign_id, species_name, badges, _offset, required, _flag = wild.groups()
            sign_dependencies[sign_id] = ("SPECIES_" + species_name, "SPECIES_" + required)
            sign_min_badges[sign_id] = int(badges)
            sign_sources[sign_id] = "LEGENDARY_SOURCE_CONDITIONAL_WILD"
        elif visible:
            sign_id, species_name, badges, required, _flag = visible.groups()
            sign_dependencies[sign_id] = ("SPECIES_" + species_name, "SPECIES_" + required)
            sign_min_badges[sign_id] = int(badges)
            sign_sources[sign_id] = "LEGENDARY_SOURCE_VISIBLE"
        elif other:
            sign_id, species_name, source = other.groups()
            sign_dependencies[sign_id] = ("SPECIES_" + species_name, "SPECIES_NONE")
            sign_min_badges[sign_id] = 0
            sign_sources[sign_id] = source
        elif ordinary:
            sign_id, species_name = ordinary.groups()
            sign_dependencies[sign_id] = ("SPECIES_" + species_name, "SPECIES_NONE")
            sign_min_badges[sign_id] = 0
            sign_sources[sign_id] = "LEGENDARY_SOURCE_ORDINARY_WILD"

    sign_components = {
        graph.find(species)
        for species, _required in sign_dependencies.values()
        if species in graph.species
    }
    available_components = {
        graph.find(species)
        for species in direct_species(pre_league=True)
        if species in graph.species
    } - sign_components
    for species, _required in sign_dependencies.values():
        sign_id = next(key for key, value in sign_dependencies.items() if value[0] == species)
        if sign_sources[sign_id] not in {
            "LEGENDARY_SOURCE_CONDITIONAL_WILD",
            "LEGENDARY_SOURCE_VISIBLE",
        }:
            available_components.add(graph.find(species))
    for dex, (_relative, token) in NATIVE_ROOTS.items():
        species = representative.get(dex)
        if species in graph.species:
            available_components.add(graph.find(species))

    unresolved_signs = {
        sign_id
        for sign_id, source in sign_sources.items()
        if source in {"LEGENDARY_SOURCE_CONDITIONAL_WILD", "LEGENDARY_SOURCE_VISIBLE"}
    }
    while unresolved_signs:
        progressed = False
        for sign_id in sorted(unresolved_signs):
            species, required = sign_dependencies[sign_id]
            if required == "SPECIES_NONE" or graph.find(required) in available_components:
                available_components.add(graph.find(species))
                unresolved_signs.remove(sign_id)
                progressed = True
                break
        if not progressed:
            break
    require(
        not unresolved_signs,
        "legendary prerequisite cycle or unavailable family: " + ", ".join(sorted(unresolved_signs)),
    )

    wild_species, wild_maps = wild_locations()
    require(
        "MAP_SANDSTREWN_RUINS" in wild_species.get("SPECIES_UNOWN", set()),
        "Hoopa prerequisite Unown must have a permanent Hoenn source in Sandstrewn Ruins",
    )
    desert_underpass = json.loads((ROOT / "data/maps/DesertUnderpass/map.json").read_text())
    require(
        any(warp.get("dest_map") == "MAP_SANDSTREWN_RUINS" for warp in desert_underpass["warp_events"]),
        "Sandstrewn Ruins lost its Mirage-collapse-independent Desert Underpass entrance",
    )
    ordinary_rows = [row for row in sign_rows if row[0] == "ORDINARY_WILD_SIGN"]
    for _, sign_id, species_name, map_name in ordinary_rows:
        species = "SPECIES_" + species_name
        map_id = "MAP_" + map_name
        require(map_id in wild_species.get(species, set()), f"{sign_id}: {species} is absent from {map_id}")

    visible_rows = [row for row in sign_rows if row[0] == "VISIBLE_SIGN"]
    visible_species = set()
    for _, sign_id, species_name, map_name in visible_rows:
        map_path = ROOT / "data/maps" / next(
            name for name in json.loads((ROOT / "data/maps/map_groups.json").read_text())["gMapGroup_EmeraldChampionsExpansion"]
            if json.loads((ROOT / "data/maps" / name / "map.json").read_text())["id"] == "MAP_" + map_name
        ) / "map.json" if "MAP_" + map_name in {
            json.loads((ROOT / "data/maps" / name / "map.json").read_text())["id"]
            for name in json.loads((ROOT / "data/maps/map_groups.json").read_text())["gMapGroup_EmeraldChampionsExpansion"]
        } else None
        if map_path is None:
            candidates = [
                path for path in (ROOT / "data/maps").glob("*/map.json")
                if json.loads(path.read_text())["id"] == "MAP_" + map_name
            ]
            require(len(candidates) == 1, f"{sign_id}: map {map_name} is missing or duplicated")
            map_path = candidates[0]
        payload = json.loads(map_path.read_text())
        graphics = {
            f"OBJ_EVENT_GFX_SPECIES({species_name})",
            FIXED_INCLEMENT_GFX.get(species_name),
        } - {None}
        if species_name in SCRIPTED_VISIBLE_ROOTS:
            relative, tokens = SCRIPTED_VISIBLE_ROOTS[species_name]
            script = (ROOT / relative).read_text()
            require(all(token in script for token in tokens),
                    f"{sign_id}: scripted visible root is incomplete in {relative}")
        else:
            require(any(row.get("graphics_id") in graphics for row in payload.get("object_events", [])),
                    f"{sign_id}: visible {species_name} object missing from {map_name}; expected {sorted(graphics)}")
        visible_species.add("SPECIES_" + species_name)

    nonordinary_sign_species = sign_species - {"SPECIES_" + row[2] for row in ordinary_rows}
    leaked = nonordinary_sign_species & wild_species.keys()
    require(not leaked, f"bespoke legendary species duplicated in ordinary tables: {sorted(leaked)}")

    circuit_rows = [row for row in sign_rows if row[0] == "OTHER_SIGN" and "LEGENDARY_SOURCE_CIRCUIT" in next(
        line for line in definitions.splitlines() if f"{row[1]}," in line
    )]
    require(len(circuit_rows) == 12, f"Circuit reward count drifted: {len(circuit_rows)}")
    require(len(circuit_rows) * 2 < 40, "finite Circuit rewards collide with the Eternatus milestone")

    harbor = (ROOT / "data/maps/LilycoveCity_Harbor/scripts.inc").read_text()
    for item, flag in (
        ("ITEM_EON_TICKET", "FLAG_ENABLE_SHIP_SOUTHERN_ISLAND"),
        ("ITEM_MYSTIC_TICKET", "FLAG_ENABLE_SHIP_NAVEL_ROCK"),
        ("ITEM_AURORA_TICKET", "FLAG_ENABLE_SHIP_BIRTH_ISLAND"),
        ("ITEM_OLD_SEA_MAP", "FLAG_ENABLE_SHIP_FARAWAY_ISLAND"),
    ):
        require(f"giveitem {item}" in harbor and f"setflag {flag}" in harbor,
                f"postgame harbor does not unlock {item}")

    wild_code = (ROOT / "src/wild_encounter.c").read_text()
    create_wild = wild_code.split("void CreateWildMon", 1)[1].split("#ifdef BUGFIX", 1)[0]
    require("IsEmeraldChampionsOrdinaryWildSpecies(species)" in create_wild,
            "ordinary table encounters do not receive competitive sets")
    require("IsLegendarySignOrdinaryWildSpecies(species)" not in create_wild,
            "finite legendary encounters incorrectly enter ordinary wild-set randomization")
    require("GetCurrentLevelCap()" in wild_code,
            "ordinary-wild legendary roots are not normalized to the live cap")

    regigigas_info = (ROOT / "src/data/pokemon/species_info/gen_4_families.h").read_text()
    regigigas_block = regigigas_info[regigigas_info.index("[SPECIES_REGIGIGAS]"):]
    require("SIZE_64x64" in regigigas_block[:5000], "Regigigas lost its giant 64x64 overworld sprite")

    print(f"PASS: all {len(legendary_families)} legendary-class families have a Hoenn acquisition root")
    print(f"PASS: all {len(sign_dependencies)} legendary prerequisite chains terminate")
    print(f"PASS: {len(visible_rows)} visible quests and {len(ordinary_rows)} ordinary-wild roots are wired")
    print(f"PASS: {len(circuit_rows)} finite Circuit rewards finish before the win-40 mastery reward")
    print("PASS: all four postgame island passes are obtainable through the native harbor")


if __name__ == "__main__":
    main()
