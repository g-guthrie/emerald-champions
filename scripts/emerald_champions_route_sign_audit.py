#!/usr/bin/env python3
"""Verify the native route-sign encounter list and its physical-map coverage."""

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]

SIGNS_BY_MAP = {
    "Route101": ("Route101_EventScript_RouteSign",),
    "Route102": (
        "Route102_EventScript_RouteSignOldale",
        "Route102_EventScript_RouteSignPetalburg",
    ),
    "Route103": ("Route103_EventScript_RouteSign",),
    "Route104": (
        "Route104_EventScript_RouteSignPetalburg",
        "Route104_EventScript_RouteSignRustboro",
    ),
    "Route110": (
        "Route110_EventScript_SlateportCitySign",
        "Route110_EventScript_Route103Sign",
        "Route110_EventScript_MauvilleCitySign",
    ),
    "Route111": (
        "Route111_EventScript_RouteSignMauville",
        "Route111_EventScript_RouteSign112",
        "Route111_EventScript_RouteSign113",
    ),
    "Route112": (
        "Route112_EventScript_MtChimneyCableCarSign",
        "Route112_EventScript_MtChimneySign",
        "Route112_EventScript_RouteSignLavaridge",
    ),
    "Route113": (
        "Route113_EventScript_RouteSign111",
        "Route113_EventScript_RouteSignFallarbor",
    ),
    "Route114": ("Route114_EventScript_MeteorFallsSign",),
    "Route115": (
        "Route115_EventScript_RouteSignRustboro",
        "Route115_EventScript_MeteorFallsSign",
    ),
    "Route116": (
        "Route116_EventScript_RouteSignRustboro",
        "Route116_EventScript_RusturfTunnelSign",
    ),
    "Route117": (
        "Route117_EventScript_RouteSignVerdanturf",
        "Route117_EventScript_RouteSignMauville",
    ),
    "Route118": (
        "Route118_EventScript_RouteSignMauville",
        "Route118_EventScript_RouteSign119",
    ),
    "Route119": ("Route119_EventScript_RouteSignFortree",),
    "Route120": (
        "Route120_EventScript_RouteSignFortree",
        "Route120_EventScript_RouteSign121",
    ),
    "Route121": ("Route121_EventScript_MtPyrePierSign",),
    "Route123": (
        "Route123_EventScript_RouteSign",
        "Route123_EventScript_RouteSignMtPyre",
    ),
}

ENCOUNTER_FIELDS = (
    "land_mons",
    "water_mons",
    "rock_smash_mons",
    "fishing_mons",
    "honey_mons",
)


def read(path: str) -> str:
    return (ROOT / path).read_text()


def script_body(source: str, label: str) -> str:
    marker = f"{label}::"
    if marker not in source:
        raise AssertionError(f"missing script label {label}")
    body = source.split(marker, 1)[1]
    next_label = body.find("\n\n")
    return body if next_label < 0 else body[:next_label]


wild_data = json.loads(read("src/data/wild_encounters.json"))
encounters = {
    encounter["map"]: encounter
    for group in wild_data["wild_encounter_groups"]
    for encounter in group["encounters"]
    if "map" in encounter
}

failures = []
species_counts = {}

for map_name, expected_scripts in SIGNS_BY_MAP.items():
    map_path = ROOT / "data" / "maps" / map_name / "map.json"
    script_path = ROOT / "data" / "maps" / map_name / "scripts.inc"
    map_data = json.loads(map_path.read_text())
    source = script_path.read_text()
    sign_scripts = {
        event["script"]
        for event in map_data.get("bg_events", [])
        if event.get("type") == "sign"
    }

    encounter_key = f"MAP_{map_name.upper()}"
    encounter = encounters.get(encounter_key)
    if encounter is None:
        failures.append(f"{map_name}: no physical-map wild encounter table")
        continue

    species = []
    for field in ENCOUNTER_FIELDS:
        for slot in encounter.get(field, {}).get("mons", []):
            candidate = slot["species"]
            if candidate != "SPECIES_NONE" and candidate not in species:
                species.append(candidate)
    if map_name == "Route119" and "SPECIES_FEEBAS" not in species:
        species.append("SPECIES_FEEBAS")
    if not species:
        failures.append(f"{map_name}: route sign would have no species to show")
    species_counts[map_name] = len(species)

    for label in expected_scripts:
        if label not in sign_scripts:
            failures.append(f"{map_name}: {label} is not an interactive sign event")
            continue
        try:
            body = script_body(source, label)
        except AssertionError as error:
            failures.append(str(error))
            continue
        if "goto Common_EventScript_ShowRouteSpecies" not in body:
            failures.append(f"{map_name}: {label} does not show the live species list")

route_sign_count = sum(len(labels) for labels in SIGNS_BY_MAP.values())
if route_sign_count != 32:
    failures.append(f"expected 32 audited wayfinding signs, found {route_sign_count}")

ui_sources = "\n".join(
    read(path)
    for path in (
        "src/pokedex.c",
        "src/start_menu.c",
        "ld_script.txt",
        "sym_ewram.txt",
    )
)
for dead_token in (
    "CB2_InitAreaDex",
    "MENU_ACTION_AREA_DEX",
    "PAGE_CURRENT_AREA",
    "Task_LoadCurrentAreaPage",
    "src/area_dex.o",
):
    if dead_token in ui_sources:
        failures.append(f"removed Area Dex UI token remains: {dead_token}")

for dead_path in ("include/area_dex.h", "src/area_dex.c"):
    if (ROOT / dead_path).exists():
        failures.append(f"removed Area Dex provider remains: {dead_path}")

pokedex_source = read("src/pokedex.c")
if "case 0: //BACK TO LIST" not in pokedex_source:
    failures.append("native Pokedex start-menu behavior was not restored")

wild_source = read("src/wild_encounter.c")
for required_token in (
    "BufferCurrentMapRouteSignSpecies",
    "GetCurrentMapWildMonHeaderId()",
    "GetStringWidth(1, name, 0)",
    "ROUTE_SIGN_MAX_LINE_WIDTH 200",
    "ROUTE_SIGN_LINES_PER_PAGE 2",
    "SPECIES_FEEBAS",
    "header->landMonsInfo",
    "header->waterMonsInfo",
    "header->rockSmashMonsInfo",
    "header->fishingMonsInfo",
    "header->honeyMonsInfo",
):
    if required_token not in wild_source:
        failures.append(f"route-sign formatter is missing {required_token}")

if "def_special BufferCurrentMapRouteSignSpecies" not in read("data/specials.inc"):
    failures.append("route-sign formatter is not registered as a field special")
if "Common_EventScript_ShowRouteSpecies::" not in read("data/event_scripts.s"):
    failures.append("shared route-sign species script is missing")

if failures:
    for failure in failures:
        print(f"FAIL: {failure}")
    raise SystemExit(f"{len(failures)} route-sign audit failure(s)")

print(
    f"PASS: {route_sign_count} wayfinding signs on {len(SIGNS_BY_MAP)} physical route maps "
    "show live, unique encounter species with native pagination"
)
print(
    "PASS: encounter coverage ranges from "
    f"{min(species_counts.values())} to {max(species_counts.values())} unique species per signed route"
)
print("PASS: no Area Dex or extra Pokedex encounter UI remains")
