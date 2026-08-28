#!/usr/bin/env python3
"""Install the shared move tutor and battle vendor in every Hoenn Center."""

from __future__ import annotations

import json
from pathlib import Path


CENTERS = (
    "BattleFrontier_PokemonCenter_1F",
    "DewfordTown_PokemonCenter_1F",
    "EverGrandeCity_PokemonCenter_1F",
    "FallarborTown_PokemonCenter_1F",
    "FortreeCity_PokemonCenter_1F",
    "LavaridgeTown_PokemonCenter_1F",
    "LilycoveCity_PokemonCenter_1F",
    "MauvilleCity_PokemonCenter_1F",
    "MossdeepCity_PokemonCenter_1F",
    "OldaleTown_PokemonCenter_1F",
    "PacifidlogTown_PokemonCenter_1F",
    "PetalburgCity_PokemonCenter_1F",
    "RustboroCity_PokemonCenter_1F",
    "SlateportCity_PokemonCenter_1F",
    "SootopolisCity_PokemonCenter_1F",
    "VerdanturfTown_PokemonCenter_1F",
)

TUTOR_SCRIPT = "Common_EventScript_EmeraldChampionsMoveTutor"
VENDOR_SCRIPT = "Common_EventScript_EmeraldChampionsBattleVendor"


def new_object(graphics: str, x: int, y: int, script: str) -> dict[str, object]:
    return {
        "graphics_id": graphics,
        "x": x,
        "y": y,
        "elevation": 3,
        "movement_type": "MOVEMENT_TYPE_FACE_DOWN",
        "movement_range_x": 0,
        "movement_range_y": 0,
        "trainer_type": "TRAINER_TYPE_NONE",
        "trainer_sight_or_berry_tree_id": "0",
        "script": script,
        "flag": "0",
    }


def choose_coordinate(occupied: set[tuple[int, int]], candidates: tuple[tuple[int, int], ...]) -> tuple[int, int]:
    for coordinate in candidates:
        if coordinate not in occupied:
            occupied.add(coordinate)
            return coordinate
    raise RuntimeError("No free counter coordinate remains for a Center service")


def main() -> None:
    root = Path(__file__).resolve().parents[1]
    installed = 0

    for center in CENTERS:
        path = root / "data" / "maps" / center / "map.json"
        data = json.loads(path.read_text())
        objects = data["object_events"]
        occupied = {(int(obj["x"]), int(obj["y"])) for obj in objects}
        scripts = {obj["script"] for obj in objects}
        additions: list[dict[str, object]] = []

        if VENDOR_SCRIPT not in scripts:
            x, y = choose_coordinate(occupied, ((2, 2), (1, 2), (3, 2), (2, 3), (1, 3)))
            additions.append(new_object("OBJ_EVENT_GFX_MART_EMPLOYEE", x, y, VENDOR_SCRIPT))
        if TUTOR_SCRIPT not in scripts:
            x, y = choose_coordinate(occupied, ((13, 2), (12, 2), (14, 2), (13, 3), (12, 3)))
            additions.append(new_object("OBJ_EVENT_GFX_OLD_MAN", x, y, TUTOR_SCRIPT))

        if additions:
            insert_at = 1 if objects else 0
            objects[insert_at:insert_at] = additions
            path.write_text(json.dumps(data, indent=2, ensure_ascii=False) + "\n")
            installed += len(additions)

    print(f"centers={len(CENTERS)}")
    print(f"services_installed={installed}")


if __name__ == "__main__":
    main()
