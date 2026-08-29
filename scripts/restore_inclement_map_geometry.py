#!/usr/bin/env python3
"""Restore Inclement-era map geometry without importing obsolete scripts."""

from __future__ import annotations

import argparse
import json
import shutil
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MAP_NAMES = (
    "AlteringCave_1F",
    "AlteringCave_B1F",
    "AshenWoods",
    "CaveOfOrigin_DianciesRoom",
    "DewfordManor_1F",
    "DewfordMeadow",
    "EmberPath",
    "MeteorFalls_JirachisRoom",
    "MirageTower_B1F",
    "PetalburgWoods_2",
    "PetalburgWoods_3",
    "Route111_RuinsExterior",
    "SandstrewnRuins",
    "SandstrewnRuins_2F",
    "SandstrewnRuins_3F",
    "SandstrewnRuins_B1F",
    "ScorchedSlab_B1F",
    "ScorchedSlab_B2F",
    "ScorchedSlab_HeatransRoom",
    "Seaspray_Cave",
    "Seaspray_Cave_B1F",
    "VerdanturfMeadow",
)

MAP_SECTION_FALLBACKS = {
    "MAPSEC_ASHEN_WOODS": "MAPSEC_JAGGED_PASS",
    "MAPSEC_EMBER_PATH": "MAPSEC_JAGGED_PASS",
    "MAPSEC_DEWFORD_MANOR": "MAPSEC_DEWFORD_TOWN",
    "MAPSEC_DEWFORD_MEADOW": "MAPSEC_DEWFORD_TOWN",
    "MAPSEC_SANDSTREWN_RUINS": "MAPSEC_ROUTE_111",
    "MAPSEC_SEASPRAY_CAVE": "MAPSEC_ROUTE_115",
    "MAPSEC_VERDANTURF_MEADOW": "MAPSEC_VERDANTURF_TOWN",
}

MUSIC_FALLBACKS = {
    "HG_SEQ_GS_D_IWAYAMA": "MUS_RG_SEVII_CAVE",
    "HG_SEQ_GS_D_UNKNOWN_ISEKI": "MUS_CAVE_OF_ORIGIN",
    "MUS_ROUTE111": "MUS_ROUTE110",
}


def load(path: Path):
    return json.loads(path.read_text())


def save(path: Path, payload) -> None:
    path.write_text(json.dumps(payload, indent=2) + "\n")


def copy_layout_files(old_root: Path, layout: dict) -> None:
    for key in ("border_filepath", "blockdata_filepath"):
        relative = Path(layout[key])
        source = old_root / relative
        destination = ROOT / relative
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, destination)


def merge_unique(rows: list[dict], additions: list[dict]) -> list[dict]:
    encoded = {json.dumps(row, sort_keys=True) for row in rows}
    for row in additions:
        key = json.dumps(row, sort_keys=True)
        if key not in encoded:
            rows.append(row)
            encoded.add(key)
    return rows


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("old_root", type=Path)
    args = parser.parse_args()
    old_root = args.old_root.resolve()
    old_maps_root = old_root / "data" / "maps"
    current_maps_root = ROOT / "data" / "maps"
    old_layouts_path = old_root / "data" / "layouts" / "layouts.json"
    current_layouts_path = ROOT / "data" / "layouts" / "layouts.json"
    old_layouts_payload = load(old_layouts_path)
    current_layouts_payload = load(current_layouts_path)
    old_layouts = {layout["id"]: layout for layout in old_layouts_payload["layouts"]}
    current_layouts = {layout["id"]: layout for layout in current_layouts_payload["layouts"]}
    restored_map_constants = {
        load(old_maps_root / name / "map.json")["id"]
        for name in MAP_NAMES
    }

    # Restore every new map with only geometry, weather transitions, and warps.
    for name in MAP_NAMES:
        old_map_path = old_maps_root / name / "map.json"
        if not old_map_path.exists():
            raise SystemExit(f"missing source map {name}")
        payload = load(old_map_path)
        payload["region"] = "REGION_HOENN"
        payload["music"] = MUSIC_FALLBACKS.get(payload["music"], payload["music"])
        payload["region_map_section"] = MAP_SECTION_FALLBACKS.get(
            payload["region_map_section"], payload["region_map_section"]
        )
        payload["object_events"] = []
        payload["coord_events"] = [event for event in payload.get("coord_events", []) if event.get("type") == "weather"]
        payload["bg_events"] = []
        destination = current_maps_root / name
        destination.mkdir(parents=True, exist_ok=True)
        save(destination / "map.json", payload)
        (destination / "scripts.inc").write_text(f"{name}_MapScripts::\n\t.byte 0\n")
        layout = old_layouts[payload["layout"]]
        current_layouts[payload["layout"]] = layout
        copy_layout_files(old_root, layout)

    # Restore reciprocal entrances on retained maps, and their matching parent
    # geometry, while preserving all current scripts, objects, flags, and signs.
    touched_parents = set()
    for old_map_path in sorted(old_maps_root.glob("*/map.json")):
        name = old_map_path.parent.name
        current_map_path = current_maps_root / name / "map.json"
        if name in MAP_NAMES or not current_map_path.exists():
            continue
        old_map = load(old_map_path)
        new_map = load(current_map_path)
        added_warps = [
            warp for warp in old_map.get("warp_events", [])
            if warp.get("dest_map") in restored_map_constants
        ]
        added_connections = [
            connection for connection in (old_map.get("connections") or [])
            if connection.get("map") in restored_map_constants
        ]
        if not added_warps and not added_connections:
            continue
        new_map["warp_events"] = merge_unique(new_map.get("warp_events", []), added_warps)
        new_map["connections"] = merge_unique(new_map.get("connections") or [], added_connections) or None
        save(current_map_path, new_map)
        old_layout = old_layouts[old_map["layout"]]
        current_layouts[old_map["layout"]] = old_layout
        copy_layout_files(old_root, old_layout)
        touched_parents.add(name)

    # Diancie's chamber uses the three unused Ruby/Sapphire Cave of Origin
    # floors.  Its entrance is an Inclement addition to an otherwise retained
    # map, so it must be restored as one directed chain instead of merging a
    # duplicate warp onto the old B1F exit tile.
    cave_1f_path = current_maps_root / "CaveOfOrigin_1F" / "map.json"
    cave_1f = load(cave_1f_path)
    old_cave_1f = load(old_maps_root / "CaveOfOrigin_1F" / "map.json")
    cave_1f["object_events"] = merge_unique(cave_1f.get("object_events", []), [{
        "graphics_id": "OBJ_EVENT_GFX_SPECIES(CARBINK)",
        "x": 5,
        "y": 8,
        "elevation": 3,
        "movement_type": "MOVEMENT_TYPE_NONE",
        "movement_range_x": 0,
        "movement_range_y": 0,
        "trainer_type": "TRAINER_TYPE_NONE",
        "trainer_sight_or_berry_tree_id": "0",
        "script": "CaveOfOrigin_1F_EventScript_Carbink",
        "flag": "FLAG_BADGE08_GET",
    }])
    cave_1f["warp_events"] = [
        warp for warp in cave_1f.get("warp_events", [])
        if warp.get("dest_map") != "MAP_CAVE_OF_ORIGIN_UNUSED_RUBY_SAPPHIRE_MAP1"
    ] + [old_cave_1f["warp_events"][2]]
    save(cave_1f_path, cave_1f)
    copy_layout_files(old_root, old_layouts[old_cave_1f["layout"]])

    cave_map1_path = current_maps_root / "CaveOfOrigin_UnusedRubySapphireMap1" / "map.json"
    cave_map1 = load(cave_map1_path)
    for warp in cave_map1["warp_events"]:
        if warp["dest_map"] == "MAP_CAVE_OF_ORIGIN_1F":
            warp["dest_warp_id"] = 2
    save(cave_map1_path, cave_map1)

    cave_map3_path = current_maps_root / "CaveOfOrigin_UnusedRubySapphireMap3" / "map.json"
    cave_map3 = load(cave_map3_path)
    cave_map3["warp_events"] = load(
        old_maps_root / "CaveOfOrigin_UnusedRubySapphireMap3" / "map.json"
    )["warp_events"]
    save(cave_map3_path, cave_map3)
    touched_parents.update({
        "CaveOfOrigin_1F",
        "CaveOfOrigin_UnusedRubySapphireMap1",
        "CaveOfOrigin_UnusedRubySapphireMap3",
    })

    # Preserve current layout ordering; replace touched definitions in-place and
    # append only genuinely new layout IDs.
    ordered = []
    seen = set()
    for layout in current_layouts_payload["layouts"]:
        replacement = current_layouts[layout["id"]]
        ordered.append(replacement)
        seen.add(replacement["id"])
    for name in MAP_NAMES:
        layout_id = load(current_maps_root / name / "map.json")["layout"]
        if layout_id not in seen:
            ordered.append(current_layouts[layout_id])
            seen.add(layout_id)
    current_layouts_payload["layouts"] = ordered
    save(current_layouts_path, current_layouts_payload)

    groups_path = current_maps_root / "map_groups.json"
    groups = load(groups_path)
    group_name = "gMapGroup_EmeraldChampionsExpansion"
    if group_name not in groups["group_order"]:
        groups["group_order"].insert(groups["group_order"].index("gMapGroup_Link_Frlg"), group_name)
    groups[group_name] = list(MAP_NAMES)
    save(groups_path, groups)

    event_scripts_path = ROOT / "data" / "event_scripts.s"
    event_scripts = event_scripts_path.read_text().rstrip() + "\n"
    for name in MAP_NAMES:
        include = f'\t.include "data/maps/{name}/scripts.inc"'
        if include not in event_scripts:
            event_scripts += include + "\n"
    event_scripts_path.write_text(event_scripts)

    print(f"restored_maps={len(MAP_NAMES)}")
    print(f"restored_layouts={len({load(current_maps_root / name / 'map.json')['layout'] for name in MAP_NAMES})}")
    print("touched_parent_maps=" + ",".join(sorted(touched_parents)))


if __name__ == "__main__":
    main()
