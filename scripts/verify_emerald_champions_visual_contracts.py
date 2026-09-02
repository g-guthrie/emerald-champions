#!/usr/bin/env python3
"""Fail closed on unclassified or contradictory Emerald Champions visual anchors."""

from __future__ import annotations

import collections
import copy
import hashlib
import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]

DIRECT_MAP_EFFECT_CONTRACTS = {
    "FLDEFF_HALL_OF_FAME_RECORD": {
        "space": "screen_dedicated_map",
        "calls": (("EverGrandeCity_HallOfFame", 1),),
    },
    "FLDEFF_NPCFLY_OUT": {
        "space": "screen_transition",
        "calls": (("LilycoveCity", 1), ("Route128", 2)),
    },
    "FLDEFF_DESTROY_DEOXYS_ROCK": {
        "space": "object_anchor",
        "calls": (("BirthIsland_Exterior", 1),),
    },
}

FIXED_FIELD_EFFECT_PLACEMENTS = {
    ("FldEff_RayquazaSpotlight", 120, -24): "screen_overlay",
    ("FldEff_NPCFlyOut", 120, 0): "screen_transition",
    ("CreateFlyBirdSprite", 255, 180): "screen_transition",
    ("FldEff_PokecenterHeal", "tFirstBallX", 93): "screen_map_aligned",
    ("FldEff_PokecenterHeal", "tFirstBallY", 36): "screen_map_aligned",
    ("FldEff_PokecenterHeal", "tMonitorX", 124): "screen_map_aligned",
    ("FldEff_PokecenterHeal", "tMonitorY", 24): "screen_map_aligned",
    ("FldEff_HallOfFameRecord", "tFirstBallX", 117): "screen_dedicated_map",
}

DIRECT_EFFECT = re.compile(r"(?m)^\s*dofieldeffect\s+(FLDEFF_[A-Z0-9_]+)\s*$")
SPARKLE_LITERAL = re.compile(r"dofieldeffectsparkle\s+(\d+),\s*(\d+),")
COMMON_NURSE_CALL = "call Common_EventScript_PkmnCenterNurse"
SCRIPT_COORDINATES = {
    "setobjectxy": re.compile(r"\bsetobjectxy(?:perm)?\s+[^,]+,\s*(\d+),\s*(\d+)"),
    "setmetatile": re.compile(r"\bsetmetatile\s+(\d+),\s*(\d+),"),
}

# These are inherited, reviewed edge/staging records.  They are intentionally
# outside their owning layout; every other compiled map event must be in-bounds.
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

REVIEWED_UNCONDITIONAL_OBJECT_STACKS = {
    ("BattleFrontier_BattlePalaceBattleRoom", 13, 1, 3): {
        "OBJ_EVENT_GFX_VAR_0",
        "OBJ_EVENT_GFX_DUSCLOPS",
        "OBJ_EVENT_GFX_AZURILL",
    },
}

LIFECYCLE_FIXED_MAPS = {
    "FallarborTown",
    "RustboroCity_Gym",
    "RustboroCity_Flat2_2F",
}

SCRIPTED_WARP = re.compile(
    r"^\s*(warp(?:silent|teleport|door|whitefade|spinenter)?|setwarp|setdynamicwarp|"
    r"setescapewarp|setdivewarp|warpmossdeepgym|warphole|setholewarp)\s+"
    r"(.+?)(?:\s+@.*)?$"
)
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

LOCAL_ID_VISUAL_COMMAND = re.compile(
    r"^\s*(applymovement|turnobject|removeobject|addobject|getobjectcurrentxy|"
    r"setobjectxy(?:perm)?|showobject|hideobject|showobjectat|hideobjectat)\s+"
    r"(LOCALID_[A-Z0-9_]+)(?:,\s*(.*?))?(?:\s+@.*)?$"
)
PSEUDO_LOCAL_IDS = {"LOCALID_PLAYER", "LOCALID_CAMERA"}
REVIEWED_BRINEY_CROSS_MAP_OBJECT_CALLS = {
    ("DewfordTown", "setobjectxyperm", "LOCALID_ROUTE109_BRINEY", "Route109"),
    ("DewfordTown", "addobject", "LOCALID_ROUTE109_BRINEY", "Route109"),
    ("DewfordTown", "applymovement", "LOCALID_ROUTE109_BRINEY", "Route109"),
    ("DewfordTown", "addobject", "LOCALID_ROUTE109_BOAT", "Route109"),
    ("Route104", "setobjectxyperm", "LOCALID_DEWFORD_BRINEY", "DewfordTown"),
    ("Route104", "addobject", "LOCALID_DEWFORD_BRINEY", "DewfordTown"),
    ("Route104", "applymovement", "LOCALID_DEWFORD_BRINEY", "DewfordTown"),
    ("Route104", "addobject", "LOCALID_DEWFORD_BOAT", "DewfordTown"),
    ("Route109", "addobject", "LOCALID_DEWFORD_BOAT", "DewfordTown"),
    ("Route109", "setobjectxyperm", "LOCALID_DEWFORD_BRINEY", "DewfordTown"),
    ("Route109", "addobject", "LOCALID_DEWFORD_BRINEY", "DewfordTown"),
    ("Route109", "applymovement", "LOCALID_DEWFORD_BRINEY", "DewfordTown"),
}

MAP_SPECIAL = re.compile(
    r"(?m)^[ \t]*special(?:var[ \t]+[^,\n]+,)?[ \t]+"
    r"([A-Za-z_][A-Za-z0-9_]*)[ \t]*(?:@.*)?$"
)
MAP_SPECIAL_CALLS = 979
MAP_SPECIAL_NAMES = 255
MAP_SPECIAL_TOPOLOGY_SHA256 = "ef63593dc20ead54dd0c6614935d75c3e646d66a7d0ccdd28bc6ef21e0619d9f"
SCRIPTED_WARP_LITERAL_COORDS = 192
LITERAL_LOCAL_ID_VISUAL_CALLS = 1844
VISUAL_SPECIAL_CLASSIFICATION = {
    "SpawnCameraObject": "camera_anchor",
    "RemoveCameraObject": "camera_anchor",
    "OffsetCameraForBattle": "camera_anchor",
    "ShakeCamera": "camera_effect",
    "Script_DoRayquazaScene": "dedicated_story_scene",
    "DoOrbEffect": "screen_overlay",
    "FadeOutOrbEffect": "screen_overlay",
    "CableCarWarp": "dedicated_transition",
    "CableCar": "dedicated_transition",
    "MoveElevator": "dedicated_transition",
    "StartMirageTowerShake": "map_geometry_animation",
    "StartPlayerDescendMirageTower": "map_geometry_animation",
    "StartMirageTowerDisintegration": "map_geometry_animation",
    "StartMirageTowerFossilFallAndSink": "map_geometry_animation",
    "DoMirageTowerCeilingCrumble": "map_geometry_animation",
    "DoSealedChamberShakingEffect_Long": "camera_effect",
    "DoSealedChamberShakingEffect_Short": "camera_effect",
    "DrawWholeMapView": "map_redraw",
    "MauvilleGymPressSwitch": "map_geometry_animation",
    "MauvilleGymDeactivatePuzzle": "map_geometry_animation",
    "MauvilleGymSetDefaultBarriers": "map_geometry_animation",
    "PetalburgGymSlideOpenRoomDoors": "map_geometry_animation",
    "PetalburgGymUnlockRoomDoors": "map_geometry_animation",
    "RotatingGate_InitPuzzle": "map_geometry_animation",
    "RotatingGate_InitPuzzleAndGraphics": "map_geometry_animation",
    "SetSootopolisGymCrackedIceMetatiles": "map_geometry_animation",
    "DoDomeConfetti": "screen_overlay",
    "SetMewAboveGrass": "object_sprite_effect",
    "DestroyMewEmergingGrassSprite": "object_sprite_effect",
    "SetMirageTowerVisibility": "object_visibility",
    "SetRoute119Weather": "weather",
    "SetRoute123Weather": "weather",
    "WaitWeather": "weather",
    "CreateAbnormalWeatherEvent": "weather",
    "UpdateShoalTideFlag": "map_geometry_state",
    "CloseBattlePikeCurtain": "map_geometry_animation",
}


def fail(message: str) -> None:
    raise SystemExit(message)


def require(condition: bool, message: str) -> None:
    if not condition:
        fail(message)


def read(relative: str) -> str:
    return (ROOT / relative).read_text(errors="ignore")


def c_function(source: str, name: str) -> str:
    match = re.search(
        rf"(?m)^(?:static\s+)?(?:u8|u16|u32|s8|s16|s32|void|bool8|bool32)\s+"
        rf"{re.escape(name)}\([^;\n]*\)\s*\{{",
        source,
    )
    require(match is not None, f"missing C function: {name}")
    depth = 0
    for index in range(match.end() - 1, len(source)):
        if source[index] == "{":
            depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0:
                return source[match.start() : index + 1]
    fail(f"unterminated C function: {name}")


def live_map_paths() -> list[Path]:
    return [
        path
        for path in sorted((ROOT / "data/maps").glob("*/scripts.inc"))
        if not path.parent.name.endswith("_Frlg")
    ]


def map_by_id() -> dict[str, tuple[Path, dict]]:
    result: dict[str, tuple[Path, dict]] = {}
    for path in (ROOT / "data/maps").glob("*/map.json"):
        data = json.loads(path.read_text())
        result[data["id"]] = (path, data)
    return result


def compiled_map_names() -> list[str]:
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
    names = compiled_map_names()
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


def verify_map_event_geometry() -> tuple[int, int, int]:
    errors, counts, off_map, warp_count = map_event_geometry_errors()
    require(not errors, "invalid compiled map geometry:\n" + "\n".join(errors))
    require(
        off_map == set(REVIEWED_OFF_MAP_EVENTS),
        "reviewed off-map event ledger drifted: "
        f"missing={sorted(set(REVIEWED_OFF_MAP_EVENTS) - off_map)} "
        f"new={sorted(off_map - set(REVIEWED_OFF_MAP_EVENTS))}",
    )
    return sum(counts.values()), warp_count, len(off_map)


def object_lifecycle_errors(
    *,
    overrides: dict[str, dict] | None = None,
) -> tuple[list[str], int]:
    overrides = overrides or {}
    errors: list[str] = []
    same_tile_groups = 0

    payloads: dict[str, dict] = {}
    for name in compiled_map_names():
        path = ROOT / "data" / "maps" / name / "map.json"
        if path.is_file():
            payloads[name] = overrides.get(name, json.loads(path.read_text()))

    for name, payload in payloads.items():
        by_tile: dict[tuple[int, int, int], list[dict]] = collections.defaultdict(list)
        for event in payload.get("object_events") or []:
            by_tile[(int(event["x"]), int(event["y"]), int(event.get("elevation", 0)))].append(event)
            if (
                name in LIFECYCLE_FIXED_MAPS
                and event.get("script") == "Common_EventScript_InclementRestoredNPC"
            ):
                errors.append(f"{name}: fixed lifecycle still uses generic restored-NPC dialogue")

        for (x, y, elevation), events in by_tile.items():
            always_visible = [
                event for event in events if event.get("flag") in {None, "0", 0}
            ]
            if len(always_visible) < 2:
                continue
            same_tile_groups += 1
            key = (name, x, y, elevation)
            actual_gfx = {event.get("graphics_id") for event in always_visible}
            expected_gfx = REVIEWED_UNCONDITIONAL_OBJECT_STACKS.get(key)
            if expected_gfx is None:
                errors.append(
                    f"{name}: {len(always_visible)} unconditional objects overlap at "
                    f"({x},{y},{elevation}): {sorted(actual_gfx)}"
                )
            elif actual_gfx != expected_gfx:
                errors.append(
                    f"{name}: reviewed object stack at ({x},{y},{elevation}) drifted: "
                    f"expected={sorted(expected_gfx)} actual={sorted(actual_gfx)}"
                )

    fallarbor = payloads["FallarborTown"]
    fallarbor_objects = {
        event.get("local_id"): event for event in fallarbor.get("object_events") or []
    }
    expected_fallarbor = {
        "LOCALID_FALLARBOR_RIVAL": ("FLAG_HIDE_FALLARBOR_RIVAL", "NULL"),
        "LOCALID_FALLARBOR_RIVAL_ON_BIKE": (
            "FLAG_HIDE_FALLARBOR_RIVAL_ON_BIKE",
            "NULL",
        ),
    }
    for local_id, expected in expected_fallarbor.items():
        event = fallarbor_objects.get(local_id)
        if event is None or (event.get("flag"), event.get("script")) != expected:
            errors.append(f"FallarborTown: {local_id} lifecycle drifted")
    fallarbor_triggers = [
        event
        for event in fallarbor.get("coord_events") or []
        if event.get("var") == "VAR_FALLARBOR_TOWN_STATE"
    ]
    if {
        (int(event["x"]), int(event["y"]), event.get("var_value"), event.get("script"))
        for event in fallarbor_triggers
    } != {
        (13, 8, "0", "FallarborTown_EventScript_RivalTrigger1"),
        (13, 9, "0", "FallarborTown_EventScript_RivalTrigger2"),
        (13, 10, "0", "FallarborTown_EventScript_RivalTrigger3"),
        (13, 11, "0", "FallarborTown_EventScript_RivalTrigger4"),
        (13, 12, "0", "FallarborTown_EventScript_RivalTrigger5"),
    }:
        errors.append("FallarborTown: rival trigger corridor drifted")

    rustboro_gym = payloads["RustboroCity_Gym"]
    guide = next(
        (
            event
            for event in rustboro_gym.get("object_events") or []
            if event.get("local_id") == "LOCALID_RUSTBORO_GYM_GUIDE"
        ),
        None,
    )
    if guide is None or guide.get("script") != "RustboroCity_Gym_EventScript_GymGuide":
        errors.append("RustboroCity_Gym: guide object lifecycle drifted")
    guide_triggers = [
        event
        for event in rustboro_gym.get("coord_events") or []
        if event.get("var") == "VAR_RUSTBORO_GYM_GUIDE_STATE"
    ]
    if len(guide_triggers) != 4 or {event.get("var_value") for event in guide_triggers} != {"0"}:
        errors.append("RustboroCity_Gym: guide entrance triggers drifted")

    rustboro_gym_script = read("data/maps/RustboroCity_Gym/scripts.inc")
    for token in (
        "map_script MAP_SCRIPT_ON_TRANSITION, RustboroCity_Gym_OnTransition",
        "goto_if_unset FLAG_BADGE01_GET, RustboroCity_Gym_EventScript_GuideStateReady",
        "setvar VAR_RUSTBORO_GYM_GUIDE_STATE, 1",
    ):
        if token not in rustboro_gym_script:
            errors.append(f"RustboroCity_Gym: missing self-healing guide step: {token}")

    rustboro_flat = payloads["RustboroCity_Flat2_2F"]
    flat_objects = {
        event.get("local_id"): event for event in rustboro_flat.get("object_events") or []
    }
    expected_flat = {
        "LOCALID_RUSTBORO_FLAT_ACE": (
            "FLAG_ITEM_RUSTBORO_FLOAT_STONE",
            "RustboroCity_Flat2_2F_EventScript_GiveFloatStone",
        ),
        "LOCALID_RUSTBORO_FLAT_HIKER": (
            "FLAG_HIDE_RUSTBORO_FLAT_HIKER",
            "RustboroCity_Flat2_2F_EventScript_FatMan",
        ),
    }
    for local_id, expected in expected_flat.items():
        event = flat_objects.get(local_id)
        if event is None or (event.get("flag"), event.get("script")) != expected:
            errors.append(f"RustboroCity_Flat2_2F: {local_id} lifecycle drifted")

    fallarbor_script = read("data/maps/FallarborTown/scripts.inc")
    for token in (
        "setflag FLAG_HIDE_FALLARBOR_RIVAL_ON_BIKE",
        "call_if_ne VAR_METEOR_FALLS_STATE, 0, FallarborTown_EventScript_MarkRivalSceneComplete",
        "call_if_eq VAR_FALLARBOR_TOWN_STATE, 1, FallarborTown_EventScript_HideRivals",
        "setflag FLAG_HIDE_FALLARBOR_RIVAL",
    ):
        if token not in fallarbor_script:
            errors.append(f"FallarborTown: missing self-healing lifecycle step: {token}")
    flat_script = read("data/maps/RustboroCity_Flat2_2F/scripts.inc")
    if (
        "setflag FLAG_HIDE_RUSTBORO_FLAT_HIKER" not in flat_script
        or "clearflag FLAG_HIDE_RUSTBORO_FLAT_HIKER" not in flat_script
        or "removeobject LOCALID_RUSTBORO_FLAT_ACE" not in flat_script
    ):
        errors.append("RustboroCity_Flat2_2F: Ace-to-Hiker transition drifted")

    return errors, same_tile_groups


def verify_object_lifecycles() -> int:
    errors, same_tile_groups = object_lifecycle_errors()
    require(not errors, "invalid object visibility lifecycle:\n" + "\n".join(errors))
    require(
        same_tile_groups == len(REVIEWED_UNCONDITIONAL_OBJECT_STACKS),
        f"reviewed unconditional object-stack count drifted: {same_tile_groups}",
    )
    return same_tile_groups


def consumer_script_paths() -> list[Path]:
    paths = list(live_map_paths())
    paths.extend(
        path
        for path in sorted((ROOT / "data/scripts").glob("*.inc"))
        if not path.stem.endswith("_frlg")
    )
    return paths


def scripted_warp_errors(
    *,
    overrides: dict[str, str] | None = None,
) -> tuple[list[str], int, set[tuple[str, str, str]]]:
    overrides = overrides or {}
    maps = map_by_id()
    dimensions = layout_dimensions()
    errors: list[str] = []
    literal_coordinates = 0
    reviewed_dynamic: set[tuple[str, str, str]] = set()

    for path in consumer_script_paths():
        relative = path.relative_to(ROOT).as_posix()
        source = overrides.get(relative, path.read_text())
        for line_number, line in enumerate(source.splitlines(), 1):
            match = SCRIPTED_WARP.match(line)
            if match is None:
                continue
            command, raw_args = match.groups()
            args = [arg.strip() for arg in raw_args.split(",")]
            key = (relative, command, raw_args.strip())

            if key in REVIEWED_DYNAMIC_SCRIPTED_WARPS:
                reviewed_dynamic.add(key)
                continue
            if command in {"warphole", "setholewarp"}:
                if len(args) != 1 or args[0] not in maps:
                    errors.append(
                        f"{relative}:{line_number}: unresolved target-only warp: {line.strip()}"
                    )
                continue

            if len(args) == 4 and args[1] in {"255", "WARP_ID_NONE"}:
                map_id, x_text, y_text = args[0], args[2], args[3]
            elif len(args) == 3:
                map_id, x_text, y_text = args
            else:
                errors.append(f"{relative}:{line_number}: unreviewed warp syntax: {line.strip()}")
                continue

            target = maps.get(map_id)
            if target is None:
                errors.append(f"{relative}:{line_number}: scripted warp targets missing {map_id}")
                continue
            try:
                x, y = int(x_text, 0), int(y_text, 0)
            except ValueError:
                errors.append(f"{relative}:{line_number}: unreviewed dynamic warp: {line.strip()}")
                continue
            literal_coordinates += 1
            width, height = dimensions[target[1]["layout"]]
            if not (0 <= x < width and 0 <= y < height):
                errors.append(
                    f"{relative}:{line_number}: {command} ({x},{y}) outside "
                    f"{target[0].parent.name} {width}x{height}"
                )

    return errors, literal_coordinates, reviewed_dynamic


def verify_scripted_warps() -> tuple[int, int]:
    errors, literal_coordinates, reviewed_dynamic = scripted_warp_errors()
    require(not errors, "invalid scripted warp consumers:\n" + "\n".join(errors))
    require(
        reviewed_dynamic == set(REVIEWED_DYNAMIC_SCRIPTED_WARPS),
        "reviewed dynamic scripted-warps drifted: "
        f"missing={sorted(set(REVIEWED_DYNAMIC_SCRIPTED_WARPS) - reviewed_dynamic)} "
        f"new={sorted(reviewed_dynamic - set(REVIEWED_DYNAMIC_SCRIPTED_WARPS))}",
    )
    require(
        literal_coordinates == SCRIPTED_WARP_LITERAL_COORDS,
        f"reviewed literal scripted-warp count drifted: {literal_coordinates}",
    )
    return literal_coordinates, len(reviewed_dynamic)


def local_id_consumer_errors(
    *,
    overrides: dict[str, str] | None = None,
) -> tuple[list[str], int, set[tuple[str, str, str, str]]]:
    overrides = overrides or {}
    maps = map_by_id()
    errors: list[str] = []
    checked = 0
    reviewed_cross_map: set[tuple[str, str, str, str]] = set()

    objects_by_name: dict[str, set[str]] = {}
    objects_by_id: dict[str, tuple[str, set[str]]] = {}
    for map_id, (path, payload) in maps.items():
        local_ids = {
            event.get("local_id")
            for event in payload.get("object_events") or []
            if event.get("local_id")
        }
        objects_by_name[path.parent.name] = local_ids
        objects_by_id[map_id] = (path.parent.name, local_ids)

    for path in live_map_paths():
        source_name = path.parent.name
        relative = path.relative_to(ROOT).as_posix()
        source = overrides.get(relative, path.read_text())
        source_ids = objects_by_name[source_name]
        for line_number, line in enumerate(source.splitlines(), 1):
            match = LOCAL_ID_VISUAL_COMMAND.match(line)
            if match is None:
                continue
            command, local_id, remaining = match.groups()
            if local_id in PSEUDO_LOCAL_IDS:
                continue
            checked += 1

            if command in {"showobjectat", "hideobjectat"}:
                target_map_id = (remaining or "").split(",", 1)[0].strip()
                target = objects_by_id.get(target_map_id)
                if target is None:
                    errors.append(
                        f"{relative}:{line_number}: {command} targets missing {target_map_id}"
                    )
                elif local_id not in target[1]:
                    errors.append(
                        f"{relative}:{line_number}: {local_id} missing from {target[0]}"
                    )
                continue

            if local_id in source_ids:
                continue
            candidates = [
                row
                for row in REVIEWED_BRINEY_CROSS_MAP_OBJECT_CALLS
                if row[:3] == (source_name, command, local_id)
            ]
            if len(candidates) != 1:
                errors.append(
                    f"{relative}:{line_number}: {command} references nonlocal {local_id}"
                )
                continue
            contract = candidates[0]
            target_ids = objects_by_name.get(contract[3], set())
            if local_id not in target_ids:
                errors.append(
                    f"{relative}:{line_number}: reviewed cross-map {local_id} missing from "
                    f"{contract[3]}"
                )
                continue
            reviewed_cross_map.add(contract)

    return errors, checked, reviewed_cross_map


def verify_local_id_consumers() -> tuple[int, int]:
    errors, checked, reviewed_cross_map = local_id_consumer_errors()
    require(not errors, "invalid literal local-ID consumers:\n" + "\n".join(errors))
    require(
        reviewed_cross_map == REVIEWED_BRINEY_CROSS_MAP_OBJECT_CALLS,
        "Briney cross-map object-call ledger drifted: "
        f"missing={sorted(REVIEWED_BRINEY_CROSS_MAP_OBJECT_CALLS - reviewed_cross_map)} "
        f"new={sorted(reviewed_cross_map - REVIEWED_BRINEY_CROSS_MAP_OBJECT_CALLS)}",
    )
    require(
        checked == LITERAL_LOCAL_ID_VISUAL_CALLS,
        f"reviewed literal local-ID visual-call count drifted: {checked}",
    )
    return checked, len(reviewed_cross_map)


def collect_map_special_topology(
    *,
    overrides: dict[str, str] | None = None,
) -> tuple[collections.Counter[str], collections.Counter[tuple[str, str]]]:
    overrides = overrides or {}
    names: collections.Counter[str] = collections.Counter()
    topology: collections.Counter[tuple[str, str]] = collections.Counter()
    for path in live_map_paths():
        relative = path.relative_to(ROOT).as_posix()
        source = overrides.get(relative, path.read_text())
        for special in MAP_SPECIAL.findall(source):
            names[special] += 1
            topology[(path.parent.name, special)] += 1
    return names, topology


def map_special_topology_digest(topology: collections.Counter[tuple[str, str]]) -> str:
    payload = "\n".join(
        f"{map_name}|{special}|{count}"
        for (map_name, special), count in sorted(topology.items())
    ) + "\n"
    return hashlib.sha256(payload.encode()).hexdigest()


def verify_map_special_inventory() -> tuple[int, int, int]:
    names, topology = collect_map_special_topology()
    digest = map_special_topology_digest(topology)
    require(sum(names.values()) == MAP_SPECIAL_CALLS, f"map special-call count drifted: {sum(names.values())}")
    require(len(names) == MAP_SPECIAL_NAMES, f"map special-name count drifted: {len(names)}")
    require(
        digest == MAP_SPECIAL_TOPOLOGY_SHA256,
        f"map special-call topology changed without review: {digest}",
    )
    missing_visual = set(VISUAL_SPECIAL_CLASSIFICATION) - set(names)
    require(not missing_visual, f"classified visual specials disappeared: {sorted(missing_visual)}")
    visual_calls = sum(names[name] for name in VISUAL_SPECIAL_CLASSIFICATION)
    return sum(names.values()), len(names), visual_calls


def collect_direct_map_effects() -> dict[str, collections.Counter[str]]:
    result: dict[str, collections.Counter[str]] = collections.defaultdict(collections.Counter)
    for path in live_map_paths():
        for effect in DIRECT_EFFECT.findall(path.read_text()):
            result[effect][path.parent.name] += 1
    return result


def verify_direct_map_effects() -> int:
    actual = collect_direct_map_effects()
    require(
        set(actual) == set(DIRECT_MAP_EFFECT_CONTRACTS),
        "unclassified direct map effects: "
        f"missing={sorted(set(DIRECT_MAP_EFFECT_CONTRACTS) - set(actual))} "
        f"new={sorted(set(actual) - set(DIRECT_MAP_EFFECT_CONTRACTS))}",
    )
    total = 0
    for effect, contract in DIRECT_MAP_EFFECT_CONTRACTS.items():
        expected = collections.Counter(dict(contract["calls"]))
        require(
            actual[effect] == expected,
            f"{effect} call topology drifted: expected={dict(expected)} actual={dict(actual[effect])}",
        )
        total += sum(actual[effect].values())
    return total


def layout_dimensions() -> dict[str, tuple[int, int]]:
    return {
        layout["id"]: (layout["width"], layout["height"])
        for layout in json.loads(read("data/layouts/layouts.json"))["layouts"]
    }


def literal_sparkle_errors(
    *,
    overrides: dict[str, str] | None = None,
) -> tuple[list[str], int]:
    dimensions = layout_dimensions()
    errors: list[str] = []
    checked = 0
    overrides = overrides or {}
    for path in live_map_paths():
        relative = path.relative_to(ROOT).as_posix()
        source = overrides.get(relative, path.read_text())
        map_data = json.loads((path.parent / "map.json").read_text())
        width, height = dimensions[map_data["layout"]]
        for line_number, line in enumerate(source.splitlines(), 1):
            match = SPARKLE_LITERAL.search(line)
            if match is None:
                continue
            checked += 1
            x, y = map(int, match.groups())
            if not (0 <= x < width and 0 <= y < height):
                errors.append(
                    f"{relative}:{line_number}: sparkle ({x},{y}) outside {width}x{height}"
                )
    return errors, checked


def verify_sparkle_anchors() -> int:
    errors, checked = literal_sparkle_errors()
    require(not errors, "invalid world-space sparkles:\n" + "\n".join(errors))
    magma = read("data/maps/MagmaHideout_4F/scripts.inc")
    require(
        "getobjectcurrentxy LOCALID_MAGMA_HIDEOUT_4F_MAXIE, CURRENT_POSITION, VAR_0x8004, VAR_0x8005"
        in magma
        and "dofieldeffectsparkle VAR_0x8004, VAR_0x8005, 0" in magma,
        "Magma's Orb sparkle is not object-anchored to Maxie",
    )
    return checked + 1


def script_coordinate_errors(
    *,
    overrides: dict[str, str] | None = None,
) -> tuple[list[str], int, int]:
    dimensions = layout_dimensions()
    errors: list[str] = []
    checked = 0
    reviewed_off_map = 0
    overrides = overrides or {}
    route109 = json.loads(read("data/maps/Route109/map.json"))
    route109_dimensions = dimensions[route109["layout"]]
    puzzle_dimensions = {
        dimensions[
            json.loads(read(f"data/maps/Route110_TrickHousePuzzle{index}/map.json"))["layout"]
        ]
        for index in range(1, 9)
    }
    require(
        puzzle_dimensions == {(15, 22)},
        f"Trick House shared-door target layouts drifted: {sorted(puzzle_dimensions)}",
    )

    for path in live_map_paths():
        relative = path.relative_to(ROOT).as_posix()
        source = overrides.get(relative, path.read_text())
        map_data = json.loads((path.parent / "map.json").read_text())
        default_dimensions = dimensions[map_data["layout"]]
        for line_number, line in enumerate(source.splitlines(), 1):
            if line.lstrip().startswith("@"):
                continue
            for kind, pattern in SCRIPT_COORDINATES.items():
                match = pattern.search(line)
                if match is None:
                    continue
                checked += 1
                x, y = map(int, match.groups())
                target_dimensions = default_dimensions

                if (
                    relative == "data/maps/Route110_TrickHouseEntrance/scripts.inc"
                    and kind == "setmetatile"
                    and (x, y) == (13, 1)
                ):
                    target_dimensions = (15, 22)
                elif (
                    relative == "data/maps/DewfordTown/scripts.inc"
                    and "LOCALID_ROUTE109_BRINEY" in line
                ):
                    target_dimensions = route109_dimensions
                elif (
                    relative == "data/maps/BattleFrontier_BattlePalaceBattleRoom/scripts.inc"
                    and "setobjectxyperm LOCALID_PALACE_BATTLE_OPPONENT, 15, 1" in line
                ):
                    required_sequence = (
                        "setobjectxyperm LOCALID_PALACE_BATTLE_OPPONENT, 15, 1",
                        "addobject LOCALID_PALACE_BATTLE_OPPONENT",
                        "hideobjectat LOCALID_PALACE_BATTLE_OPPONENT, MAP_BATTLE_FRONTIER_BATTLE_PALACE_BATTLE_ROOM",
                        "setobjectxy LOCALID_PALACE_BATTLE_OPPONENT, 13, 1",
                    )
                    sequence_start = source.find(required_sequence[0])
                    sequence_region = source[sequence_start : sequence_start + 1000]
                    positions = [sequence_region.find(token) for token in required_sequence]
                    if all(position >= 0 for position in positions) and positions == sorted(positions):
                        reviewed_off_map += 1
                        continue
                    errors.append(f"{relative}:{line_number}: Palace off-map entrance sequence drifted")
                    continue

                width, height = target_dimensions
                if not (0 <= x < width and 0 <= y < height):
                    errors.append(
                        f"{relative}:{line_number}: {kind} ({x},{y}) outside {width}x{height} target"
                    )
    return errors, checked, reviewed_off_map


def verify_script_coordinates() -> tuple[int, int]:
    errors, checked, reviewed = script_coordinate_errors()
    require(not errors, "invalid script world coordinates:\n" + "\n".join(errors))
    require(reviewed == 1, f"reviewed off-map entrance count drifted: {reviewed}")
    return checked, reviewed


def healer_alignment_errors(heal_locations: list[dict]) -> tuple[list[str], int]:
    maps = map_by_id()
    errors: list[str] = []
    checked = 0
    for entry in heal_locations:
        respawn_map = entry.get("respawn_map")
        local_id = entry.get("respawn_npc")
        if not respawn_map or not local_id or local_id == "LOCALID_NONE":
            continue
        row = maps.get(respawn_map)
        if row is None:
            continue
        path, map_data = row
        if map_data.get("region") != "REGION_HOENN":
            continue
        nurse = next(
            (
                obj
                for obj in map_data.get("object_events", [])
                if obj.get("local_id") == local_id
            ),
            None,
        )
        if nurse is None:
            errors.append(f"{entry['id']}: {local_id} missing from {path.parent.name}")
            continue
        checked += 1
        respawn_x = entry.get("respawn_x", 7)
        if respawn_x != nurse.get("x"):
            errors.append(
                f"{entry['id']}: respawn x {respawn_x} != nurse x {nurse.get('x')}"
            )
    return errors, checked


def verify_healer_anchors() -> tuple[int, int]:
    heal_locations = json.loads(read("src/data/heal_locations.json"))["heal_locations"]
    errors, destinations = healer_alignment_errors(heal_locations)
    require(not errors, "invalid Hoenn healer anchors:\n" + "\n".join(errors))

    callers = 0
    for path in live_map_paths():
        lines = path.read_text().splitlines()
        for index, line in enumerate(lines):
            if COMMON_NURSE_CALL not in line:
                continue
            callers += 1
            prefix = "\n".join(lines[max(0, index - 4) : index])
            local_id = re.search(r"setvar\s+VAR_0x800B,\s*(LOCALID_[A-Z0-9_]+)", prefix)
            require(local_id is not None, f"{path.relative_to(ROOT)}:{index + 1}: no nurse anchor")
            map_data = json.loads((path.parent / "map.json").read_text())
            obj = next(
                (
                    row
                    for row in map_data.get("object_events", [])
                    if row.get("local_id") == local_id.group(1)
                ),
                None,
            )
            require(
                obj is not None and obj.get("graphics_id") == "OBJ_EVENT_GFX_NURSE",
                f"{path.relative_to(ROOT)}:{index + 1}: {local_id.group(1)} is not a nurse",
            )

    require(callers == 18, f"live shared-nurse caller count drifted: {callers}")
    heal_source = read("src/heal_location.c")
    whiteout = c_function(heal_source, "SetWhiteoutRespawnWarpAndHealerNPC")
    require(
        "gSpecialVar_LastTalked = healNpcLocalId;" in whiteout
        and "gSpecialVar_0x800B = healNpcLocalId;" in whiteout,
        "whiteout does not publish one consistent healer identity",
    )
    return destinations, callers


def enclosing_function(source: str, position: int) -> str:
    matches = list(
        re.finditer(
            r"(?m)^(?:static\s+)?(?:u8|u16|u32|s8|s16|s32|void|bool8|bool32)\s+"
            r"([A-Za-z0-9_]+)\([^;\n]*\)\s*\{",
            source[:position],
        )
    )
    require(matches, f"fixed placement at byte {position} has no enclosing function")
    return matches[-1].group(1)


def fixed_field_effect_placements() -> dict[tuple, str]:
    source = read("src/field_effect.c")
    result: dict[tuple, str] = {}
    create = re.compile(
        r"CreateSprite\([^;\n]*?,\s*(-?(?:0x[0-9A-Fa-f]+|\d+))\s*,\s*"
        r"(-?(?:0x[0-9A-Fa-f]+|\d+))\s*,"
    )
    for match in create.finditer(source):
        key = (
            enclosing_function(source, match.start()),
            int(match.group(1), 0),
            int(match.group(2), 0),
        )
        result[key] = "raw_sprite_xy"

    assignment = re.compile(
        r"task->(t[A-Za-z0-9_]*(?:X|Y))\s*=\s*(-?(?:0x[0-9A-Fa-f]+|\d+))\s*;"
    )
    for match in assignment.finditer(source):
        key = (
            enclosing_function(source, match.start()),
            match.group(1),
            int(match.group(2), 0),
        )
        result[key] = "task_coordinate"
    return result


def verify_fixed_field_effect_placements() -> int:
    actual = fixed_field_effect_placements()
    require(
        set(actual) == set(FIXED_FIELD_EFFECT_PLACEMENTS),
        "raw field-effect placements changed without classification: "
        f"missing={sorted(set(FIXED_FIELD_EFFECT_PLACEMENTS) - set(actual), key=str)} "
        f"new={sorted(set(actual) - set(FIXED_FIELD_EFFECT_PLACEMENTS), key=str)}",
    )
    return len(actual)


def dormant_frlg_findings() -> list[str]:
    maps = map_by_id()
    findings: list[str] = []
    for entry in json.loads(read("src/data/heal_locations.json"))["heal_locations"]:
        respawn_map = entry.get("respawn_map")
        local_id = entry.get("respawn_npc")
        row = maps.get(respawn_map)
        if row is None or not local_id or local_id == "LOCALID_NONE":
            continue
        _, map_data = row
        if map_data.get("region") != "REGION_KANTO":
            continue
        if not any(obj.get("local_id") == local_id for obj in map_data.get("object_events", [])):
            findings.append(f"{entry['id']}: {local_id} missing from {respawn_map}")
    return findings


def run_mutation_probes() -> int:
    probes = 0

    heal_locations = json.loads(read("src/data/heal_locations.json"))["heal_locations"]
    mutated_heals = copy.deepcopy(heal_locations)
    oldale = next(row for row in mutated_heals if row["id"] == "HEAL_LOCATION_OLDALE_TOWN")
    oldale["respawn_x"] = 7
    errors, _ = healer_alignment_errors(mutated_heals)
    require(any("HEAL_LOCATION_OLDALE_TOWN" in error for error in errors), "camera mutation escaped")
    probes += 1

    metatile_path = "data/maps/AncientTomb/scripts.inc"
    metatile_source = read(metatile_path)
    valid_metatile = re.search(r"setmetatile\s+(\d+),\s*(\d+),", metatile_source)
    require(valid_metatile is not None, "mutation source lacks a literal metatile")
    mutated_metatile = metatile_source.replace(
        valid_metatile.group(0),
        "setmetatile 999, 999,",
        1,
    )
    errors, _, _ = script_coordinate_errors(overrides={metatile_path: mutated_metatile})
    require(any(metatile_path in error for error in errors), "script-coordinate mutation escaped")
    probes += 1

    magma_path = "data/maps/MagmaHideout_4F/scripts.inc"
    mutated_magma = read(magma_path).replace(
        "dofieldeffectsparkle VAR_0x8004, VAR_0x8005, 0",
        "dofieldeffectsparkle 18, 42, 0",
    )
    errors, _ = literal_sparkle_errors(overrides={magma_path: mutated_magma})
    require(any(magma_path in error for error in errors), "world-coordinate mutation escaped")
    probes += 1

    unknown = collect_direct_map_effects()
    unknown["FLDEFF_UNREVIEWED_EFFECT"]["SyntheticMap"] += 1
    require(
        set(unknown) - set(DIRECT_MAP_EFFECT_CONTRACTS) == {"FLDEFF_UNREVIEWED_EFFECT"},
        "unclassified-effect mutation escaped",
    )
    probes += 1

    oldale = json.loads(read("data/maps/OldaleTown_PokemonCenter_1F/map.json"))
    mutated_oldale = copy.deepcopy(oldale)
    mutated_oldale["object_events"][0]["x"] = 999
    errors, _, _, _ = map_event_geometry_errors(
        overrides={"OldaleTown_PokemonCenter_1F": mutated_oldale}
    )
    require(
        any("OldaleTown_PokemonCenter_1F:object_events" in error for error in errors),
        "embedded map-coordinate mutation escaped",
    )
    probes += 1

    mutated_warp = copy.deepcopy(oldale)
    mutated_warp["warp_events"][0]["dest_map"] = "MAP_MISSING_VISUAL_PROBE"
    errors, _, _, _ = map_event_geometry_errors(
        overrides={"OldaleTown_PokemonCenter_1F": mutated_warp}
    )
    require(any("targets missing map" in error for error in errors), "warp mutation escaped")
    probes += 1

    fallarbor = json.loads(read("data/maps/FallarborTown/map.json"))
    mutated_fallarbor = copy.deepcopy(fallarbor)
    for event in mutated_fallarbor["object_events"]:
        if event.get("local_id") in {
            "LOCALID_FALLARBOR_RIVAL",
            "LOCALID_FALLARBOR_RIVAL_ON_BIKE",
        }:
            event["flag"] = "0"
    errors, _ = object_lifecycle_errors(overrides={"FallarborTown": mutated_fallarbor})
    require(
        any("unconditional objects overlap" in error for error in errors),
        "object-lifecycle collision mutation escaped",
    )
    probes += 1

    warp_path = "data/maps/SootopolisCity_MysteryEventsHouse_B1F/scripts.inc"
    mutated_warp_source = read(warp_path).replace(
        "warp MAP_SOOTOPOLIS_CITY_MYSTERY_EVENTS_HOUSE_1F, 3, 1",
        "warp MAP_SOOTOPOLIS_CITY_MYSTERY_EVENTS_HOUSE_1F, 999, 999",
        1,
    )
    errors, _, _ = scripted_warp_errors(overrides={warp_path: mutated_warp_source})
    require(any(warp_path in error and "outside" in error for error in errors), "scripted-warp mutation escaped")
    probes += 1

    fallarbor_script_path = "data/maps/FallarborTown/scripts.inc"
    mutated_fallarbor_script = read(fallarbor_script_path).replace(
        "LOCALID_FALLARBOR_RIVAL",
        "LOCALID_MISSING_VISUAL_PROBE",
        1,
    )
    errors, _, _ = local_id_consumer_errors(
        overrides={fallarbor_script_path: mutated_fallarbor_script}
    )
    require(
        any("LOCALID_MISSING_VISUAL_PROBE" in error for error in errors),
        "literal local-ID mutation escaped",
    )
    probes += 1

    special_path = "data/maps/FallarborTown/scripts.inc"
    mutated_special_source = read(special_path) + "\n\tspecial UnreviewedVisualProbe\n"
    names, topology = collect_map_special_topology(
        overrides={special_path: mutated_special_source}
    )
    require(
        "UnreviewedVisualProbe" in names
        and map_special_topology_digest(topology) != MAP_SPECIAL_TOPOLOGY_SHA256,
        "map-special topology mutation escaped",
    )
    probes += 1
    return probes


def main() -> None:
    map_events, compiled_warps, reviewed_map_edges = verify_map_event_geometry()
    reviewed_object_stacks = verify_object_lifecycles()
    scripted_warps, dynamic_scripted_warps = verify_scripted_warps()
    local_id_calls, briney_cross_map_calls = verify_local_id_consumers()
    map_special_calls, map_special_names, visual_special_calls = verify_map_special_inventory()
    direct_calls = verify_direct_map_effects()
    sparkle_anchors = verify_sparkle_anchors()
    script_coordinates, reviewed_off_map = verify_script_coordinates()
    healer_destinations, nurse_callers = verify_healer_anchors()
    fixed_placements = verify_fixed_field_effect_placements()
    mutation_probes = run_mutation_probes()
    dormant = dormant_frlg_findings()

    print("EMERALD CHAMPIONS VISUAL CONTRACTS: PASS")
    print(
        f"compiled_maps={len(compiled_map_names())} map_event_records={map_events} "
        f"compiled_warps={compiled_warps} reviewed_off_map_events={reviewed_map_edges}"
    )
    print(f"reviewed_unconditional_object_stacks={reviewed_object_stacks}")
    print(
        f"scripted_warp_coordinates={scripted_warps} "
        f"reviewed_dynamic_scripted_warps={dynamic_scripted_warps}"
    )
    print(
        f"literal_local_id_visual_calls={local_id_calls} "
        f"briney_cross_map_calls={briney_cross_map_calls}"
    )
    print(
        f"map_special_calls={map_special_calls} map_special_names={map_special_names} "
        f"classified_visual_special_calls={visual_special_calls}"
    )
    print(f"direct_map_effect_calls={direct_calls} classified_effects={len(DIRECT_MAP_EFFECT_CONTRACTS)}")
    print(f"world_sparkle_anchors={sparkle_anchors} healer_destinations={healer_destinations}")
    print(f"script_world_coordinates={script_coordinates} reviewed_off_map={reviewed_off_map}")
    print(f"shared_nurse_callers={nurse_callers} fixed_screen_placements={fixed_placements}")
    print(f"mutation_probes={mutation_probes} dormant_frlg_findings={len(dormant)}")
    for finding in dormant:
        print(f"INFO dormant FRLG: {finding}")


if __name__ == "__main__":
    main()
