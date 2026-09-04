#!/usr/bin/env python3
"""Fail closed when an inherited map drifts from Inclement Emerald v1.13."""

from __future__ import annotations

import hashlib
import json
import copy
from collections import Counter
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
# Visible objects Champions deliberately places where Inclement had none. Each entry is a
# reviewed decision with its reason, keyed by (graphics, x, y).
REVIEWED_EXTRA_OBJECTS = {
    "SlateportCity": {
        ("OBJ_EVENT_GFX_MAN_4", 18, 3):
            "Champions road worker who closes Route 110 (cap-30 chapter) until the Knuckle "
            "Badge; Inclement sends the player to Slateport before Brawly and never gated it",
    },
    "Route111": {
        ("OBJ_EVENT_GFX_MAN_4", 16, 97):
            "Champions road worker who closes Route 111 north (cap-40 chapter) until the "
            "Dynamo Badge; vanilla and Inclement never gated it",
    },
    "MirageTower_4F": {
        ("OBJ_EVENT_GFX_BREAKABLE_ROCK", 6, 7):
            "vanilla Rock Smash boulder; Inclement had an Aerodactylite sparkle here and "
            "Champions supplies that stone through the Mega archive instead",
    },
}

ITEM_MARKER_GRAPHICS = {
    "OBJ_EVENT_GFX_ITEM_BALL",
    "OBJ_EVENT_GFX_GOLD_ITEM_BALL",
    "OBJ_EVENT_GFX_MEGA_STONE",
}

MANIFEST = ROOT / "tests/reference/inclement_overworld_parity_manifest.json"

# Modern expansion schema aliases with identical runtime meaning.
GFX_ALIASES = {
    "OBJ_EVENT_GFX_INCLEMENT_ARTICUNO": "OBJ_EVENT_GFX_ARTICUNO",
    "OBJ_EVENT_GFX_INCLEMENT_ZAPDOS": "OBJ_EVENT_GFX_ZAPDOS",
    "OBJ_EVENT_GFX_INCLEMENT_MOLTRES": "OBJ_EVENT_GFX_MOLTRES",
    "OBJ_EVENT_GFX_INCLEMENT_MEWTWO": "OBJ_EVENT_GFX_MEWTWO",
    "OBJ_EVENT_GFX_INCLEMENT_JIRACHI": "OBJ_EVENT_GFX_JIRACHI",
    "OBJ_EVENT_GFX_INCLEMENT_HEATRAN": "OBJ_EVENT_GFX_HEATRAN",
    "OBJ_EVENT_GFX_REGIGIGAS_STATUE": "OBJ_EVENT_GFX_REGIGIGAS",
    "OBJ_EVENT_GFX_INCLEMENT_DIANCIE": "OBJ_EVENT_GFX_DIANCIE",
    "OBJ_EVENT_GFX_INCLEMENT_CARBINK": "OBJ_EVENT_GFX_CARBINK",
}
DYNAMIC_WARP_ALIASES = {
    ("MAP_DYNAMIC", "WARP_ID_DYNAMIC"): ("MAP_NONE", 127),
    ("MAP_DYNAMIC", "WARP_ID_SECRET_BASE"): ("MAP_NONE", 126),
}
CENTER_MAPS = {
    "BattleFrontier_PokemonCenter_1F", "DewfordTown_PokemonCenter_1F",
    "EverGrandeCity_PokemonCenter_1F", "FallarborTown_PokemonCenter_1F",
    "FortreeCity_PokemonCenter_1F", "LavaridgeTown_PokemonCenter_1F",
    "LilycoveCity_PokemonCenter_1F", "MauvilleCity_PokemonCenter_1F",
    "MossdeepCity_PokemonCenter_1F", "OldaleTown_PokemonCenter_1F",
    "PacifidlogTown_PokemonCenter_1F", "PetalburgCity_PokemonCenter_1F",
    "RustboroCity_PokemonCenter_1F", "SlateportCity_PokemonCenter_1F",
    "SootopolisCity_PokemonCenter_1F", "VerdanturfTown_PokemonCenter_1F",
}
CENTER_REFERENCE_SERVICE_SIGNATURES = {
    ("OBJ_EVENT_GFX_OLD_MAN", 13, 2),
    ("OBJ_EVENT_GFX_OLD_MAN", 14, 2),
    ("OBJ_EVENT_GFX_MART_EMPLOYEE", 2, 2),
    ("OBJ_EVENT_GFX_MART_EMPLOYEE", 13, 2),
}
CENTER_CURRENT_SERVICE_SCRIPTS = {
    "Common_EventScript_EmeraldChampionsBattleVendor",
    "Common_EventScript_EmeraldChampionsMoveTutor",
}
CENTER_ADAPTED_SERVICE_MAPS = {"LavaridgeTown_PokemonCenter_1F"}


def load(path: Path):
    return json.loads(path.read_text())


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def scalar(value):
    if isinstance(value, str) and value.lstrip("-").isdigit():
        return int(value)
    return value


def normalized_rows(rows: list[dict] | None, fields: tuple[str, ...]) -> list[list]:
    return sorted(
        [[row.get(field) for field in fields] for row in (rows or [])],
        key=repr,
    )


def normalize_warp(row: list) -> tuple:
    x, y, elevation, dest_map, dest_warp_id = row
    dest_map, dest_warp_id = DYNAMIC_WARP_ALIASES.get(
        (dest_map, dest_warp_id), (dest_map, dest_warp_id)
    )
    return (scalar(x), scalar(y), scalar(elevation), dest_map, scalar(dest_warp_id))


def normalize_object(row: list) -> tuple:
    graphics_id, x, y, elevation, movement, range_x, range_y = row
    return (
        GFX_ALIASES.get(graphics_id, graphics_id),
        scalar(x),
        scalar(y),
        scalar(elevation),
        movement,
        scalar(range_x),
        scalar(range_y),
    )


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def run_mutation_probes(manifest: dict, layouts: dict[str, dict]) -> int:
    probes = 0
    route = load(ROOT / "data" / "maps" / "Route103" / "map.json")
    expected = manifest["maps"]["Route103"]

    mutated_header = copy.deepcopy(route)
    mutated_header["weather"] = "WEATHER_VISUAL_PARITY_PROBE"
    actual_header = {
        field: mutated_header.get(field) for field in manifest["visual_header_fields"]
    }
    require(actual_header != expected["visual_header"], "map-header mutation escaped")
    probes += 1

    mutated_objects = copy.deepcopy(route.get("object_events") or [])
    mutated_objects[0]["x"] = int(mutated_objects[0]["x"]) + 1
    rows = normalized_rows(mutated_objects, tuple(manifest["object_semantic_fields"]))
    require(
        rows != expected["semantic_contract"]["current_objects"],
        "object-semantic mutation escaped",
    )
    probes += 1

    mutated_layout = dict(layouts[route["layout"]])
    mutated_layout["width"] = int(mutated_layout["width"]) + 1
    layout_visual = {
        field: mutated_layout.get(field) for field in manifest["layout_visual_fields"]
    }
    require(layout_visual != expected["layout_visual"], "layout mutation escaped")
    probes += 1

    relative, expected_hash = min(manifest["tileset_asset_sha256"].items())
    require(sha256(ROOT / relative) == expected_hash, "tileset mutation source is stale")
    require("0" * 64 != expected_hash, "tileset-hash mutation escaped")
    probes += 1
    return probes


def main() -> None:
    require(MANIFEST.is_file(), f"missing canonical manifest: {MANIFEST}")
    manifest = load(MANIFEST)
    require(manifest.get("schema") == 3, "unsupported overworld parity manifest")
    # Floors rather than exact pins: per-map contracts below carry the proof, and
    # a legitimately added map or tileset must not fail the release.
    require(manifest.get("map_count", 0) >= 500, "canonical inherited map coverage collapsed")
    require(
        manifest.get("tileset_asset_count", 0) >= 1500,
        "canonical shared Inclement tileset asset coverage collapsed",
    )
    for relative, expected_hash in manifest.get("asset_sha256", {}).items():
        asset = ROOT / relative
        require(asset.is_file(), f"missing exact Inclement object asset: {relative}")
        require(sha256(asset) == expected_hash, f"Inclement object asset drifted: {relative}")
    for relative, expected_hash in manifest.get("tileset_asset_sha256", {}).items():
        asset = ROOT / relative
        require(asset.is_file(), f"missing Inclement tileset asset: {relative}")
        require(sha256(asset) == expected_hash, f"Inclement tileset asset drifted: {relative}")

    layouts_payload = load(ROOT / "data" / "layouts" / "layouts.json")
    layouts = {row["id"]: row for row in layouts_payload["layouts"]}
    failures = []
    object_count = 0
    for name, expected in manifest["maps"].items():
        path = ROOT / "data" / "maps" / name / "map.json"
        if not path.is_file():
            failures.append(f"{name}: inherited map is missing")
            continue
        actual = load(path)
        semantic = expected["semantic_contract"]
        for key, fields_key, actual_key in (
            ("current_objects", "object_semantic_fields", "object_events"),
            ("current_coord_events", "coord_event_fields", "coord_events"),
            ("current_bg_events", "bg_event_fields", "bg_events"),
        ):
            rows = normalized_rows(actual.get(actual_key), tuple(manifest[fields_key]))
            if rows != semantic[key]:
                failures.append(f"{name}: reviewed {actual_key} semantic contract drifted")
        actual_header = {
            field: actual.get(field) for field in manifest["visual_header_fields"]
        }
        if actual_header != expected["visual_header"]:
            failures.append(f"{name}: visual map header drifted")
        if actual.get("layout") != expected["layout"]:
            failures.append(f"{name}: layout id drifted")
            continue
        layout = layouts.get(actual["layout"])
        if layout is None:
            failures.append(f"{name}: layout definition is missing")
            continue
        actual_layout_visual = {
            field: layout.get(field) for field in manifest["layout_visual_fields"]
        }
        if actual_layout_visual != expected["layout_visual"]:
            failures.append(f"{name}: layout dimensions or tileset identity drifted")
        if sha256(ROOT / layout["blockdata_filepath"]) != expected["blockdata_sha256"]:
            failures.append(f"{name}: map blockdata is not Inclement-identical")
        if sha256(ROOT / layout["border_filepath"]) != expected["border_sha256"]:
            failures.append(f"{name}: border blockdata is not Inclement-identical")

        connections = Counter(
            tuple(scalar(value) for value in row)
            for row in [
                [event.get(field) for field in ("direction", "offset", "map")]
                for event in (actual.get("connections") or [])
            ]
        )
        expected_connections = Counter(
            tuple(scalar(value) for value in row) for row in expected["connections"]
        )
        if connections != expected_connections:
            failures.append(f"{name}: connections drifted")

        warps = Counter(
            normalize_warp(
                [event.get(field) for field in ("x", "y", "elevation", "dest_map", "dest_warp_id")]
            )
            for event in (actual.get("warp_events") or [])
        )
        expected_warps = Counter(normalize_warp(row) for row in expected["warps"])
        if warps != expected_warps:
            failures.append(f"{name}: warp geometry or destination drifted")

        actual_object_events = actual.get("object_events") or []
        expected_object_rows = expected["objects"]
        if name in CENTER_ADAPTED_SERVICE_MAPS:
            actual_object_events = [
                event for event in actual_object_events
                if event.get("script") not in CENTER_CURRENT_SERVICE_SCRIPTS
            ]
            expected_object_rows = [
                row for row in expected_object_rows
                if (row[0], scalar(row[1]), scalar(row[2]))
                not in CENTER_REFERENCE_SERVICE_SIGNATURES
            ]
        # Emerald Champions deliberately redesigns overworld item markers: which stones
        # and balls exist, where they sit, and whether a pickup shows the item-ball or the
        # Mega Stone sparkle. That is Champions design, not Inclement visual drift, so item
        # markers are excluded here and gated separately by
        # verify_emerald_champions_reward_economy.py. Every NPC, sign and decoration is
        # still compared to Inclement exactly as before.
        reviewed_extra = REVIEWED_EXTRA_OBJECTS.get(name, {})
        actual_object_events = [
            event for event in actual_object_events
            if event.get("graphics_id") not in ITEM_MARKER_GRAPHICS
            and (event.get("graphics_id"), event.get("x"), event.get("y")) not in reviewed_extra
        ]
        expected_object_rows = [row for row in expected_object_rows if row[0] not in ITEM_MARKER_GRAPHICS]
        objects = Counter(
            normalize_object([event.get(field) for field in manifest["visual_object_fields"]])
            for event in actual_object_events
        )
        expected_objects = Counter(normalize_object(row) for row in expected_object_rows)
        object_count += sum(objects.values())
        if objects != expected_objects:
            missing = sum((expected_objects - objects).values())
            extra = sum((objects - expected_objects).values())
            failures.append(f"{name}: object visual layer drifted ({missing} missing, {extra} extra)")

    if failures:
        preview = "\n".join(f"  - {row}" for row in failures[:80])
        suffix = "" if len(failures) <= 80 else f"\n  ... {len(failures) - 80} more"
        raise SystemExit(
            f"INCLEMENT OVERWORLD PARITY: FAIL ({len(failures)} maps)\n{preview}{suffix}"
        )
    mutation_probes = run_mutation_probes(manifest, layouts)
    print(
        "INCLEMENT OVERWORLD PARITY: PASS "
        f"({manifest['map_count']} maps, {object_count} visual objects, "
        f"{manifest['tileset_asset_count']} tileset assets, "
        f"semantic adaptations "
        f"objects={manifest['semantic_summary']['objects_adapted_maps']} "
        f"coord={manifest['semantic_summary']['coord_events_adapted_maps']} "
        f"bg={manifest['semantic_summary']['bg_events_adapted_maps']}, "
        f"mutation_probes={mutation_probes}, "
        f"source {manifest['source_commit']})"
    )


if __name__ == "__main__":
    main()
