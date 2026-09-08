"""Shared current-map geometry and dynamic-warp metadata, without scene snapshots."""

from __future__ import annotations

import collections
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

REVIEWED_OFF_MAP_EVENTS = {
    ("SlateportCity", "warp_events", 40, 7): "east-edge map connection warp",
    ("SlateportCity_Harbor", "warp_events", 19, 15): "south-edge harbor exit",
    ("SlateportCity_Harbor", "warp_events", 20, 15): "south-edge harbor exit",
    ("LilycoveCity_DepartmentStore_1F", "bg_events", 0, 8): "counter-edge elevator sign",
    ("BattleFrontier_BattleDomeCorridor", "warp_events", 6, 8): "off-map corridor staging",
    ("BattleFrontier_BattleDomeCorridor", "warp_events", 7, 8): "off-map corridor staging",
    ("BattleFrontier_BattleDomePreBattleRoom", "warp_events", 6, 8): "off-map room staging",
    ("BattleFrontier_BattleDomePreBattleRoom", "warp_events", 7, 8): "off-map room staging",
    ("CeruleanCity_Frlg", "object_events", 50, 18): "dormant FRLG connected-map object",
    ("CeladonCity_Frlg", "object_events", -7, 21): "dormant FRLG connected-map object",
    ("FiveIsland_Frlg", "object_events", 32, 9): "dormant FRLG connected-map object",
    ("Route2_Frlg", "object_events", 6, 85): "dormant FRLG connected-map object",
    ("Route4_Frlg", "object_events", 109, 3): "dormant FRLG connected-map object",
    ("Route7_Frlg", "object_events", -8, 12): "dormant FRLG connected-map object",
    ("Route15_Frlg", "object_events", 73, 7): "dormant FRLG connected-map object",
    ("Route21_North_Frlg", "object_events", 13, -3): "dormant FRLG scene staging",
    ("SevenIsland_SevaultCanyon_Entrance_Frlg", "object_events", 7, -2):
        "dormant FRLG scene staging",
}

REVIEWED_DYNAMIC_SCRIPTED_WARPS = {
    (
        "data/maps/PetalburgCity_Gym/scripts.inc",
        "warpdoor",
        "MAP_PETALBURG_CITY_GYM, VAR_0x8008, VAR_0x8009",
    ): "room-selection variables are constrained by the gym script",
    (
        "data/scripts/cave_hole.inc",
        "warphole",
        "MAP_UNDEFINED",
    ): "destination is resolved from the current hole warp",
}

def fail(message: str) -> None:
    raise SystemExit(message)

def require(condition: bool, message: str) -> None:
    if not condition:
        fail(message)

def read(relative: str) -> str:
    return (ROOT / relative).read_text(errors="ignore")

def registered_map_names() -> list[str]:
    groups = json.loads(read("data/maps/map_groups.json"))
    names = [name for group in groups["group_order"] for name in groups[group]]
    require(len(names) == len(set(names)), "compiled map groups contain duplicate map names")
    return names

def map_event_geometry_errors(
    *,
    overrides: dict[str, dict] | None = None,
) -> tuple[list[str], collections.Counter[str], set[tuple[str, str, int, int]], int]:
    overrides = overrides or {}
    dimensions = layout_dimensions()
    names = registered_map_names()
    payloads: dict[str, dict] = {}
    by_id: dict[str, tuple[str, dict]] = {}
    errors: list[str] = []
    counts: collections.Counter[str] = collections.Counter()
    off_map: set[tuple[str, str, int, int]] = set()

    for name in names:
        path = ROOT / "data" / "maps" / name / "map.json"
        if not path.is_file():
            errors.append(f"{name}: compiled map data is missing")
            continue
        payload = overrides.get(name, json.loads(path.read_text()))
        payloads[name] = payload
        map_id = payload.get("id")
        if map_id in by_id:
            errors.append(f"{name}: duplicate compiled map id {map_id}")
        else:
            by_id[map_id] = (name, payload)

    warp_count = 0
    for name, payload in payloads.items():
        if payload.get("layout") not in dimensions:
            errors.append(f"{name}: missing layout {payload.get('layout')}")
            continue
        width, height = dimensions[payload["layout"]]
        explicit_local_ids: set[str] = set()
        for kind in ("object_events", "warp_events", "coord_events", "bg_events"):
            for index, event in enumerate(payload.get(kind) or []):
                counts[kind] += 1
                try:
                    x, y = int(event["x"]), int(event["y"])
                except (KeyError, TypeError, ValueError):
                    errors.append(f"{name}:{kind}[{index}]: nonnumeric or missing coordinate")
                    continue
                if not (0 <= x < width and 0 <= y < height):
                    key = (name, kind, x, y)
                    off_map.add(key)
                    if key not in REVIEWED_OFF_MAP_EVENTS:
                        errors.append(
                            f"{name}:{kind}[{index}]: ({x},{y}) outside {width}x{height}"
                        )

                if kind == "object_events":
                    local_id = event.get("local_id")
                    if local_id:
                        if local_id in explicit_local_ids:
                            errors.append(f"{name}: duplicate object local id {local_id}")
                        explicit_local_ids.add(local_id)

                if kind != "warp_events":
                    continue
                warp_count += 1
                dest_map = event.get("dest_map")
                dest_warp = event.get("dest_warp_id")
                if dest_map in {"MAP_DYNAMIC", "MAP_NONE"} or dest_warp in {
                    "WARP_ID_DYNAMIC", "WARP_ID_SECRET_BASE", 126, 127, "126", "127"
                }:
                    continue
                target = by_id.get(dest_map)
                if target is None:
                    errors.append(f"{name}: warp {index} targets missing map {dest_map}")
                    continue
                try:
                    target_index = int(dest_warp)
                except (TypeError, ValueError):
                    errors.append(f"{name}: warp {index} has nonnumeric target {dest_warp}")
                    continue
                target_warps = target[1].get("warp_events") or []
                if not 0 <= target_index < len(target_warps):
                    errors.append(
                        f"{name}: warp {index} targets {dest_map}[{target_index}], "
                        f"which has {len(target_warps)} warps"
                    )

        for connection in payload.get("connections") or []:
            counts["connections"] += 1
            target_map = connection.get("map")
            if target_map not in by_id:
                errors.append(f"{name}: connection targets missing map {target_map}")

    return errors, counts, off_map, warp_count

def layout_dimensions() -> dict[str, tuple[int, int]]:
    return {
        layout["id"]: (layout["width"], layout["height"])
        for layout in json.loads(read("data/layouts/layouts.json"))["layouts"]
    }
