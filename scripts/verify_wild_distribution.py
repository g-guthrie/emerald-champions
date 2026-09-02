#!/usr/bin/env python3
"""Wild distribution gate for the Hoenn campaign tables.

Checks src/data/wild_encounters.json (Hoenn maps only) for the properties the
2026-09-02 rebuild established:

* every table has the exact slot count the engine reads (12/5/10/5);
* every land table is 12 unique species; surf >= 5 unique; fishing >= 7 unique;
* no species sits in more than MAX_MAPS Hoenn maps (water species are allowed
  a higher ceiling because there is one sea);
* no evolved form is listed below its evolution level;
* fishing/surf table levels respect their access tier (Old Rod <= 30,
  Good Rod <= max(cap, 30), Super Rod <= max(cap, 60), Surf <= max(cap, 55));
  the runtime clamp in wild_encounter.c is also asserted;
* no starter, trade-exclusive, or legendary species outside the Sign system;
Campaign-wide acquisition reachability is deliberately left to
verify_emerald_champions_campaign_roster.py, which follows physical scripts
and does not mistake dormant FRLG trade-table rows for live Hoenn rewards.
"""
from __future__ import annotations

import glob
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
FAILURES: list[str] = []
REVIEWED_SINGLE_SPECIES_MAPS = {
    "MAP_ARTISAN_CAVE_1F", "MAP_ARTISAN_CAVE_B1F",   # Smeargle cave by design
    "MAP_ROUTE130",                                 # Mirage Island: Wynaut only
    "MAP_CAVE_OF_ORIGIN_DIANCIES_ROOM",             # single-species story room
}
KANTO = re.compile(r"ISLAND|FRLG|CERULEAN|CELADON|CINNABAR|FUCHSIA|PALLET|VERMILION|VIRIDIAN|SSANNE|SEAFOAM|_ROUTE([1-9]|1[0-9]|2[0-9])(_|$)|POKEMON_TOWER|POKEMON_MANSION|POWER_PLANT|MT_MOON|MT_EMBER|DIGLETTS_CAVE|VIRIDIAN_FOREST|ROCK_TUNNEL|CERULEAN_CAVE|BERRY_FOREST|ICEFALL|LOST_CAVE|TANOBY|TRAINER_TOWER|PATTERN_BUSH|ALTERING_CAVE$|OUTCAST|RUIN_VALLEY|GREEN_PATH|WATER_PATH|MEMORIAL_PILLAR|RESORT_GORGEOUS|WATER_LABYRINTH|KINDLE_ROAD|TREASURE_BEACH|CAPE_BRINK|BOND_BRIDGE|SEVAULT")
SLOT_COUNTS = {"land_mons": 12, "water_mons": 5, "rock_smash_mons": 5, "fishing_mons": 10, "hidden_mons": 3}
MAX_MAPS_LAND = 24
MAX_MAPS_WATER = 45
# Species that are deliberately not obtainable in the wild or by evolution.
REVIEWED_UNOBTAINABLE = {
    # trade exclusives (see src/data/trade.h)
    "SPECIES_FIDOUGH", "SPECIES_DACHSBUN", "SPECIES_BOMBIRDIER", "SPECIES_CYCLIZAR", "SPECIES_TYPE_NULL",
    "SPECIES_SILVALLY", "SPECIES_HAPPINY", "SPECIES_CHANSEY", "SPECIES_BLISSEY",
    # Paldean Wooper has no competitive preset in this build yet, so Clodsire waits
    "SPECIES_CLODSIRE",
    # Galarian Zigzagoon / Meowth lines are not in this build's species table
    "SPECIES_OBSTAGOON", "SPECIES_PERRSERKER",
}


def require(condition: bool, message: str) -> None:
    if condition:
        print(f"PASS: {message}")
    else:
        print(f"FAIL: {message}")
        FAILURES.append(message)


def species_data():
    evo_level: dict[str, int] = {}
    evolutions: dict[str, list[str]] = {}
    legendary: set[str] = set()
    base_forms: list[str] = []
    for path in glob.glob(str(ROOT / "src/data/pokemon/species_info/gen_*_families.h")):
        text = Path(path).read_text()
        for match in re.finditer(r"\[(SPECIES_[A-Z0-9_]+)\] =\s*\{(.*?)\n    \},", text, re.S):
            species, body = match.group(1), match.group(2)
            if "baseHP = 0" in body:
                continue
            base_forms.append(species)
            for method, param, target in re.findall(r"\{(EVO_[A-Z_]+),\s*([A-Z0-9_]+),\s*(SPECIES_[A-Z0-9_]+)", body):
                evolutions.setdefault(species, []).append(target)
                if method == "EVO_LEVEL" and param.isdigit():
                    evo_level[target] = min(evo_level.get(target, 1000), int(param))
            # Ultra Beasts and Paradox Pokémon are ordinary wild encounters in this
            # campaign; only true legendaries/mythicals must come through Signs.
            if re.search(r"\.is(?:Legendary|RestrictedLegendary|SubLegendary|Mythical)[A-Za-z]*\s*=\s*TRUE", body):
                legendary.add(species)
    return evo_level, evolutions, legendary, base_forms


def caps_by_map() -> dict[str, int]:
    # One source of truth: the generator merges the battle master, its fallback
    # table and docs/wild_route_sheet.json.
    import importlib.util
    spec = importlib.util.spec_from_file_location("rebuild", ROOT / "scripts/emerald_champions_rebuild_wild_water.py")
    rebuild = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(rebuild)
    return rebuild.caps_by_map()


def main() -> int:
    data = json.loads((ROOT / "src/data/wild_encounters.json").read_text())
    hoenn = [g for g in data["wild_encounter_groups"] if g.get("label") == "gWildMonHeaders"][0]
    evo_level, evolutions, legendary, base_forms = species_data()
    caps = caps_by_map()
    import importlib.util
    rebuild_spec = importlib.util.spec_from_file_location(
        "wild_rebuild", ROOT / "scripts/emerald_champions_rebuild_wild_water.py"
    )
    rebuild = importlib.util.module_from_spec(rebuild_spec)
    rebuild_spec.loader.exec_module(rebuild)
    route_sheet = rebuild.load_route_sheet()
    available_species, rebuild_evolutions, _ = rebuild.load_species_data()
    seen: set[str] = set()
    maps_of: dict[str, set[str]] = {}
    water_species: set[str] = set()
    all_wild: set[str] = set()
    slot_errors, unique_errors, floor_errors, tier_errors = [], [], [], []
    for entry in hoenn["encounters"]:
        map_id = entry["map"]
        if KANTO.search(map_id) or map_id in seen:
            continue
        seen.add(map_id)
        cap = caps.get(map_id, 55)
        for area, expected in SLOT_COUNTS.items():
            table = entry.get(area)
            if not table:
                continue
            mons = table["mons"]
            if len(mons) != expected:
                slot_errors.append(f"{map_id}/{area}: {len(mons)} slots")
            species = [m["species"] for m in mons]
            uniq = len(set(species))
            minimum = {"land_mons": 12, "water_mons": 5, "fishing_mons": 7, "rock_smash_mons": 3, "hidden_mons": 3}[area]
            if uniq < minimum and map_id not in REVIEWED_SINGLE_SPECIES_MAPS:
                unique_errors.append(f"{map_id}/{area}: {uniq} unique")
            for i, mon in enumerate(mons):
                s = mon["species"]
                all_wild.add(s)
                maps_of.setdefault(s, set()).add(map_id)
                if area in ("water_mons", "fishing_mons"):
                    water_species.add(s)
                floor = evo_level.get(s)
                if floor and mon["min_level"] < floor:
                    floor_errors.append(f"{map_id}/{area}: {s} at Lv {mon['min_level']} below evolution Lv {floor}")
                if area == "fishing_mons":
                    limit = 30 if i < 2 else max(cap, 30) if i < 5 else max(cap, 60)
                elif area == "water_mons":
                    limit = max(cap, 55)
                else:
                    limit = cap + 2
                if mon["max_level"] > limit:
                    tier_errors.append(f"{map_id}/{area}: {s} Lv {mon['max_level']} > tier limit {limit}")
    require(not slot_errors, f"every Hoenn table has the engine's slot count {slot_errors[:5]}")
    require(not unique_errors, f"table variety floors hold (land 12, surf 5, fishing 7) {unique_errors[:8]}")
    require(not floor_errors, f"no evolved form below its evolution level {floor_errors[:5]}")
    require(not tier_errors, f"table levels respect access tiers {tier_errors[:5]}")
    # Magikarp is the universal Old Rod pull by design; everything else has a ceiling.
    over = [f"{s}:{len(m)}" for s, m in maps_of.items()
            if s != "SPECIES_MAGIKARP" and len(m) > (MAX_MAPS_WATER if s in water_species else MAX_MAPS_LAND)]
    require(not over, f"no species floods the region {over[:10]}")

    # Bind every generated surf/fishing table to the authored route sheet.
    # This catches a stale JSON output, a silently ignored row, or a generator
    # change that no longer expresses the reviewed anchors and hunts.
    water_maps: set[str] = set()
    water_mismatches: list[str] = []
    ordered_seen: set[str] = set()
    for entry in hoenn["encounters"]:
        map_id = entry["map"]
        if KANTO.search(map_id) or map_id in ordered_seen:
            continue
        if not (entry.get("water_mons") or entry.get("fishing_mons")):
            continue
        ordered_seen.add(map_id)
        water_maps.add(map_id)
        row = route_sheet.get(map_id)
        if row is None:
            water_mismatches.append(f"{map_id}: missing route-sheet row")
            continue
        region_name = row.get("region") or rebuild.REGION_OF.get(map_id, "inland")
        region = rebuild.REGIONS[region_name]
        cap = caps.get(map_id, 55)
        offset = sum(ord(c) for c in map_id) % 7 + len(ordered_seen)
        if entry.get("water_mons"):
            expected = rebuild.build_water(region, offset, cap, rebuild_evolutions, available_species, row)
            if entry["water_mons"] != expected:
                water_mismatches.append(f"{map_id}: surf table is stale")
        if entry.get("fishing_mons"):
            expected = rebuild.build_fishing(region, offset, cap, rebuild_evolutions, available_species, row)
            if entry["fishing_mons"] != expected:
                water_mismatches.append(f"{map_id}: fishing table is stale")
    require(set(route_sheet) == water_maps,
            f"route sheet covers exactly all generated water maps (sheet-only={sorted(set(route_sheet)-water_maps)}, map-only={sorted(water_maps-set(route_sheet))})")
    require(not water_mismatches, f"all surf/fishing tables match the authored route sheet {water_mismatches[:8]}")

    starters = set(re.findall(r"SPECIES_[A-Z0-9_]+", (ROOT / "src/starter_choose.c").read_text()))
    signs_text = (ROOT / "src/data/pokemon/legendary_signs.h").read_text()
    sign_species = {"SPECIES_" + m for m in re.findall(r"_SIGN\([A-Z_]+, ([A-Z0-9_]+),", signs_text)}
    gift_signs = set(sign_species)
    require(not (all_wild & starters), f"no starter in Hoenn wild tables {sorted(all_wild & starters)[:5]}")
    stray = sorted((all_wild & legendary) - sign_species)
    require(not stray, f"legendary-class wild species all belong to the Sign system {stray[:5]}")
    trades = set(re.findall(r"\.species = (SPECIES_[A-Z0-9_]+)", (ROOT / "src/data/trade.h").read_text()))
    require(not (all_wild & trades & REVIEWED_UNOBTAINABLE), "trade-exclusive species stay exclusive")

    wild_code = (ROOT / "src/wild_encounter.c").read_text()
    require(wild_code.count("level = min(level, GetCurrentLevelCap());") >= 2,
            "runtime clamps every wild level (land/water/rocks and fishing) to the live cap")

    # The slot weights are a hand-maintained design contract, not generator
    # output: a flattened curve so the rarest slot on a route is 4%, not 1%,
    # and Sweet Scent's full slot reversal turns that 4% slot into the 14% one.
    encounters = json.loads((ROOT / "src/data/wild_encounters.json").read_text())
    rates = {field["type"]: field["encounter_rates"] for field in encounters["wild_encounter_groups"][0]["fields"]}
    require(rates["land_mons"] == [14, 12, 11, 10, 9, 9, 8, 7, 6, 5, 5, 4],
            f"land slot weights are the flattened 14..4 curve, got {rates['land_mons']}")
    require(rates["water_mons"] == [35, 25, 18, 12, 10] and rates["rock_smash_mons"] == [35, 25, 18, 12, 10],
            "water and Rock Smash slot weights are the flattened 35..10 curve")
    require(rates["fishing_mons"] == [60, 40, 45, 30, 25, 30, 25, 20, 15, 10],
            "fishing slot weights are unchanged (100 per rod)")
    require(all(sum(rates[key]) == 100 for key in ("land_mons", "water_mons", "rock_smash_mons")),
            "land, water and Rock Smash slot weights each sum to 100")
    require("wildMonIndex = 11 - wildMonIndex;" in wild_code
            and wild_code.count("if (sSweetScentInverted || (LURE_STEP_COUNT != 0") == 4,
            "Sweet Scent reverses the full slot order in every table (land/water/rocks/fishing)")

    if FAILURES:
        print(f"\n{len(FAILURES)} wild-distribution check(s) failed")
        return 1
    print("\nAll wild-distribution checks passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
