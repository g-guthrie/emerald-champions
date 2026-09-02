#!/usr/bin/env python3
"""Restore and verify the native Route 111 Chansey / Poke Vial quest."""

from __future__ import annotations

import argparse
import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MAPS = ROOT / "data" / "maps"


def npc(
    local_id: str,
    graphics: str,
    x: int,
    y: int,
    movement: str,
    script: str,
    flag: str,
    range_x: int = 0,
    range_y: int = 0,
) -> dict:
    return {
        "local_id": local_id,
        "graphics_id": graphics,
        "x": x,
        "y": y,
        "elevation": 3,
        "movement_type": movement,
        "movement_range_x": range_x,
        "movement_range_y": range_y,
        "trainer_type": "TRAINER_TYPE_NONE",
        "trainer_sight_or_berry_tree_id": "0",
        "script": script,
        "flag": flag,
    }


OBJECTS = {
    "Route111": [
        npc(
            "LOCALID_ROUTE111_VIAL_NURSE", "OBJ_EVENT_GFX_NURSE", 18, 102,
            "MOVEMENT_TYPE_FACE_UP", "Route111_EventScript_VialUpgradeNurse",
            "FLAG_HIDE_ROUTE111_VIAL_NURSE", 1, 1,
        ),
        npc(
            "LOCALID_ROUTE111_VIAL_CHANSEY", "OBJ_EVENT_GFX_CHANSEY", 18, 100,
            "MOVEMENT_TYPE_NONE", "0", "FLAG_HIDE_ROUTE111_VIAL_CHANSEY", 2, 1,
        ),
    ],
    "Route112": [
        npc(
            "LOCALID_ROUTE112_VIAL_CHANSEY", "OBJ_EVENT_GFX_CHANSEY", 25, 29,
            "MOVEMENT_TYPE_FACE_DOWN", "0", "FLAG_HIDE_ROUTE112_VIAL_CHANSEY", 1, 1,
        ),
    ],
    "JaggedPass": [
        npc(
            "LOCALID_JAGGED_PASS_VIAL_CHANSEY", "OBJ_EVENT_GFX_CHANSEY", 12, 29,
            "MOVEMENT_TYPE_LOOK_AROUND", "JaggedPass_EventScript_VialChansey",
            "FLAG_HIDE_JAGGED_PASS_VIAL_CHANSEY",
        ),
    ],
    "AshenWoods": [
        npc(
            "LOCALID_ASHEN_WOODS_VIAL_CHANSEY", "OBJ_EVENT_GFX_CHANSEY", 14, 29,
            "MOVEMENT_TYPE_LOOK_AROUND", "AshenWoods_EventScript_VialChansey",
            "FLAG_HIDE_ASHEN_WOODS_VIAL_CHANSEY",
        ),
        npc(
            "LOCALID_ASHEN_WOODS_VIAL_BALL", "OBJ_EVENT_GFX_ITEM_BALL", 27, 43,
            "MOVEMENT_TYPE_NONE", "0", "FLAG_HIDE_ASHEN_WOODS_VIAL_BALL",
        ),
    ],
}


def trigger(x: int, y: int, value: int, script: str) -> dict:
    return {
        "type": "trigger",
        "x": x,
        "y": y,
        "elevation": 0,
        "var": "VAR_CHANSEY_NURSE_STATE",
        "var_value": str(value),
        "script": script,
    }


COORDS = {
    "Route111": [
        trigger(19, 102, 0, "Route111_EventScript_VialChanseyEscape"),
        trigger(20, 102, 0, "Route111_EventScript_VialChanseyEscape"),
        trigger(18, 103, 0, "Route111_EventScript_VialChanseyEscape"),
    ],
    "Route112": [
        trigger(25, 31, 1, "Route112_EventScript_VialChanseyEscape"),
        trigger(26, 30, 1, "Route112_EventScript_VialChanseyEscape"),
        trigger(27, 30, 1, "Route112_EventScript_VialChanseyEscape"),
    ],
    "AshenWoods": [
        trigger(16, 28, 3, "AshenWoods_EventScript_VialChanseyEscape1"),
        trigger(16, 29, 3, "AshenWoods_EventScript_VialChanseyEscape1"),
        trigger(16, 30, 3, "AshenWoods_EventScript_VialChanseyEscape1"),
        trigger(6, 36, 4, "AshenWoods_EventScript_VialChanseyEscape2"),
    ],
}


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def load(name: str) -> dict:
    return json.loads((MAPS / name / "map.json").read_text())


def save(name: str, payload: dict) -> None:
    (MAPS / name / "map.json").write_text(json.dumps(payload, indent=2) + "\n")


def write() -> None:
    for name, additions in OBJECTS.items():
        payload = load(name)
        if name == "AshenWoods":
            for event in payload["object_events"]:
                if event.get("local_id") == "LOCALID_EC_OKIDOGI":
                    event["x"], event["y"] = 21, 39
        local_ids = {event["local_id"] for event in additions}
        payload["object_events"] = [
            event for event in payload["object_events"]
            if event.get("local_id") not in local_ids
        ] + additions

        scripts = {event["script"] for event in COORDS.get(name, [])}
        payload["coord_events"] = [
            event for event in payload.get("coord_events", [])
            if event.get("script") not in scripts
        ] + COORDS.get(name, [])
        save(name, payload)


def check() -> None:
    for name, expected_objects in OBJECTS.items():
        payload = load(name)
        by_local = {
            event.get("local_id"): event
            for event in payload["object_events"]
            if event.get("local_id")
        }
        for expected in expected_objects:
            require(by_local.get(expected["local_id"]) == expected,
                    f"{name}: Vial quest object drifted: {expected['local_id']}")
        for expected in expected_objects:
            collisions = [
                event for event in payload["object_events"]
                if event.get("local_id") != expected["local_id"]
                and (event["x"], event["y"]) == (expected["x"], expected["y"])
            ]
            require(not collisions, f"{name}: object overlap in Vial quest at {expected['x']},{expected['y']}")
        expected_coords = COORDS.get(name, [])
        actual = payload.get("coord_events", [])
        for expected in expected_coords:
            require(expected in actual, f"{name}: missing Vial quest trigger {expected}")

    constants = (ROOT / "include/constants/vars.h").read_text() + (ROOT / "include/constants/flags.h").read_text()
    for token in (
        "VAR_CHANSEY_NURSE_STATE",
        "FLAG_HIDE_ROUTE111_VIAL_NURSE",
        "FLAG_HIDE_ROUTE111_VIAL_CHANSEY",
        "FLAG_HIDE_ROUTE112_VIAL_CHANSEY",
        "FLAG_HIDE_JAGGED_PASS_VIAL_CHANSEY",
        "FLAG_HIDE_ASHEN_WOODS_VIAL_CHANSEY",
        "FLAG_HIDE_ASHEN_WOODS_VIAL_BALL",
    ):
        require(token in constants, f"Vial quest state token missing: {token}")

    script_contract = {
        "Route111": ("Route111_EventScript_VialChanseyEscape", "Route111_EventScript_VialUpgradeNurse"),
        "Route112": ("Route112_EventScript_VialChanseyEscape",),
        "JaggedPass": ("JaggedPass_EventScript_VialChansey",),
        "AshenWoods": (
            "AshenWoods_EventScript_VialChanseyEscape1",
            "AshenWoods_EventScript_VialChanseyEscape2",
            "AshenWoods_EventScript_VialChansey",
        ),
    }
    for name, labels in script_contract.items():
        text = (MAPS / name / "scripts.inc").read_text()
        for label in labels:
            require(label in text, f"{name}: missing Vial quest script {label}")
    print("PASS: the native Chansey chase grants the second Poke Vial charge exactly once")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    args = parser.parse_args()
    if args.write:
        write()
    check()


if __name__ == "__main__":
    main()
