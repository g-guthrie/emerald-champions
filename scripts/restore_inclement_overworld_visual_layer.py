#!/usr/bin/env python3
"""Restore every inherited map's Inclement v1.13 visual object layer.

Current scripts, trainer assignments, flags, and item contents are retained
when an object can be matched.  Objects absent from the current engine are
restored at their Inclement position.  Removed pickups use an otherwise-unused
Emerald Champions pickup flag and keep the old reward when it can be inferred;
otherwise they become Rare Candies.  This is intentionally a visual/spatial
restore, not a wholesale import of the old script engine.
"""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
VISUAL_FIELDS = (
    "graphics_id",
    "x",
    "y",
    "elevation",
    "movement_type",
    "movement_range_x",
    "movement_range_y",
)
REFERENCE_TO_CURRENT_GFX = {
    "OBJ_EVENT_GFX_ARTICUNO": "OBJ_EVENT_GFX_INCLEMENT_ARTICUNO",
    "OBJ_EVENT_GFX_ZAPDOS": "OBJ_EVENT_GFX_INCLEMENT_ZAPDOS",
    "OBJ_EVENT_GFX_MOLTRES": "OBJ_EVENT_GFX_INCLEMENT_MOLTRES",
    "OBJ_EVENT_GFX_MEWTWO": "OBJ_EVENT_GFX_INCLEMENT_MEWTWO",
    "OBJ_EVENT_GFX_JIRACHI": "OBJ_EVENT_GFX_INCLEMENT_JIRACHI",
    "OBJ_EVENT_GFX_HEATRAN": "OBJ_EVENT_GFX_INCLEMENT_HEATRAN",
    "OBJ_EVENT_GFX_REGIGIGAS": "OBJ_EVENT_GFX_REGIGIGAS_STATUE",
    "OBJ_EVENT_GFX_DIANCIE": "OBJ_EVENT_GFX_INCLEMENT_DIANCIE",
    "OBJ_EVENT_GFX_CARBINK": "OBJ_EVENT_GFX_INCLEMENT_CARBINK",
}
CURRENT_TO_REFERENCE_GFX = {value: key for key, value in REFERENCE_TO_CURRENT_GFX.items()}
CURRENT_TO_REFERENCE_GFX.update({
    "OBJ_EVENT_GFX_SPECIES(CHANSEY)": "OBJ_EVENT_GFX_CHANSEY",
    "OBJ_EVENT_GFX_SPECIES(CARBINK)": "OBJ_EVENT_GFX_CARBINK",
    "OBJ_EVENT_GFX_POKE_BALL": "OBJ_EVENT_GFX_ITEM_BALL",
})
ITEM_GFX = {
    "OBJ_EVENT_GFX_ITEM_BALL",
    "OBJ_EVENT_GFX_GOLD_ITEM_BALL",
    "OBJ_EVENT_GFX_MEGA_STONE",
}
SERVICE_SCRIPT_ALIASES = {
    "PKMN_Center_Move_Tutor": "Common_EventScript_EmeraldChampionsMoveTutor",
    "General_Mart_Script": "Common_EventScript_EmeraldChampionsBattleVendor",
}
CENTER_MAPS = {
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
}
CENTER_REFERENCE_SERVICE_SCRIPTS = {"PKMN_Center_Move_Tutor", "General_Mart_Script"}


def standardized_center_services() -> list[dict]:
    common = {
        "elevation": 3,
        "movement_range_x": 0,
        "movement_range_y": 0,
        "trainer_type": "TRAINER_TYPE_NONE",
        "trainer_sight_or_berry_tree_id": "0",
        "flag": "0",
    }
    return [
        {
            **common,
            "graphics_id": "OBJ_EVENT_GFX_MART_EMPLOYEE",
            "x": 2,
            "y": 2,
            "movement_type": "MOVEMENT_TYPE_FACE_DOWN",
            "script": "Common_EventScript_EmeraldChampionsBattleVendor",
        },
        {
            **common,
            "graphics_id": "OBJ_EVENT_GFX_OLD_MAN",
            "x": 13,
            "y": 2,
            "movement_type": "MOVEMENT_TYPE_FACE_DOWN",
            "script": "Common_EventScript_EmeraldChampionsMoveTutor",
        },
    ]


def load(path: Path):
    return json.loads(path.read_text())


def save(path: Path, payload) -> None:
    path.write_text(json.dumps(payload, indent=2) + "\n")


def reference_gfx(obj: dict) -> str:
    return CURRENT_TO_REFERENCE_GFX.get(obj.get("graphics_id"), obj.get("graphics_id"))


def is_item(obj: dict) -> bool:
    return reference_gfx(obj) in ITEM_GFX


def choose(rows: list[tuple[int, dict]], reference: dict) -> int | None:
    if not rows:
        return None
    return min(
        rows,
        key=lambda row: (
            abs(int(row[1]["x"]) - int(reference["x"]))
            + abs(int(row[1]["y"]) - int(reference["y"])),
            row[0],
        ),
    )[0]


def match(reference: dict, current: list[dict], used: set[int]) -> int | None:
    candidates = [(index, row) for index, row in enumerate(current) if index not in used]
    script = reference.get("script")
    if script not in {None, "0x0", "NULL"}:
        result = choose([(i, row) for i, row in candidates if row.get("script") == script], reference)
        if result is not None:
            return result
    flag = reference.get("flag")
    if flag not in {None, "0"}:
        result = choose([(i, row) for i, row in candidates if row.get("flag") == flag], reference)
        if result is not None:
            return result
    result = choose([
        (i, row) for i, row in candidates
        if (int(row["x"]), int(row["y"])) == (int(reference["x"]), int(reference["y"]))
        and (reference_gfx(row) == reference_gfx(reference) or (is_item(row) and is_item(reference)))
    ], reference)
    if result is not None:
        return result
    return choose([
        (i, row) for i, row in candidates
        if (int(row["x"]), int(row["y"])) == (int(reference["x"]), int(reference["y"]))
    ], reference)


def defined_labels() -> set[str]:
    labels = set()
    for root in (ROOT / "data", ROOT / "src"):
        for path in root.rglob("*"):
            if path.is_file() and path.suffix in {".inc", ".s", ".c", ".h"}:
                try:
                    labels.update(re.findall(r"^([A-Za-z_][A-Za-z0-9_]*)(?:::|:)", path.read_text(), re.M))
                except UnicodeDecodeError:
                    pass
    return labels


def flag_definitions() -> tuple[set[str], list[str]]:
    text = (ROOT / "include/constants/flags.h").read_text()
    defined = set(re.findall(r"^#define\s+(FLAG_[A-Z0-9_]+)\s+", text, re.M))
    ec = re.findall(r"^#define\s+(FLAG_EC_ITEM_[A-Z0-9_]+)\s+", text, re.M)
    unused = re.findall(r"^#define\s+(FLAG_UNUSED_[A-Za-z0-9_]+)\s+", text, re.M)
    return defined, ec + unused


def infer_old_item(source_map: Path, label: str | None) -> str:
    if not label or label in {"0x0", "NULL"}:
        return "ITEM_ULTRA_BALL"
    text = source_map.read_text()
    match = re.search(rf"^{re.escape(label)}::\s*$", text, re.M)
    if not match:
        return "ITEM_ULTRA_BALL"
    remainder = text[match.end():]
    next_label = re.search(r"^[A-Za-z_][A-Za-z0-9_]*::\s*$", remainder, re.M)
    block = remainder[: next_label.start()] if next_label else remainder
    item = re.search(r"\b(?:giveitem|itemball|checkitem)\s+(ITEM_[A-Z0-9_]+)", block)
    if item:
        constants = (ROOT / "include/constants/items.h").read_text()
        if re.search(rf"\b{re.escape(item.group(1))}\b", constants):
            return item.group(1)
    return "ITEM_ULTRA_BALL"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("inclement_root", type=Path)
    args = parser.parse_args()
    source = args.inclement_root.resolve()
    source_maps = source / "data/maps"
    current_maps = ROOT / "data/maps"
    labels = defined_labels()
    defined_flags, pickup_flag_pool = flag_definitions()

    # Reserve only flags on objects that survive the Inclement roster restore.
    # Flags on removed story-hint bodies and extra pickups are intentionally
    # available for the restored Inclement pickup locations.
    used_flags = set()
    for source_path in sorted(source_maps.glob("*/map.json")):
        destination = current_maps / source_path.parent.name / "map.json"
        old = load(source_path)
        current_objects = load(destination).get("object_events", [])
        used_indices: set[int] = set()
        references = old.get("object_events", [])
        if source_path.parent.name in CENTER_MAPS:
            references = [
                row for row in references
                if row.get("script") not in CENTER_REFERENCE_SERVICE_SCRIPTS
            ]
        for reference in references:
            index = match(reference, current_objects, used_indices)
            if index is not None:
                used_indices.add(index)
                flag = current_objects[index].get("flag")
                if flag:
                    used_flags.add(flag)
    available_pickup_flags = [flag for flag in pickup_flag_pool if flag not in used_flags]

    restored = removed = matched = pickups = fallback_npcs = 0
    for source_path in sorted(source_maps.glob("*/map.json")):
        destination = current_maps / source_path.parent.name / "map.json"
        if not destination.exists():
            raise SystemExit(f"missing inherited map {source_path.parent.name}")
        old = load(source_path)
        new = load(destination)

        # Copy the four genuinely drifted warp tables.  Dynamic-warp schema aliases
        # are left alone on every other map.
        if source_path.parent.name in {
            "AlteringCave",
            "Route110",
            "Route110_SeasideCyclingRoadNorthEntrance",
            "Route110_SeasideCyclingRoadSouthEntrance",
        }:
            new["warp_events"] = old.get("warp_events", [])

        current_objects = new.get("object_events", [])
        used: set[int] = set()
        output = []
        references = old.get("object_events", [])
        if source_path.parent.name in CENTER_MAPS:
            references = [
                row for row in references
                if row.get("script") not in CENTER_REFERENCE_SERVICE_SCRIPTS
            ]
        for reference in references:
            index = match(reference, current_objects, used)
            if index is not None:
                used.add(index)
                obj = dict(current_objects[index])
                matched += 1
            else:
                obj = dict(reference)
                restored += 1
                script = SERVICE_SCRIPT_ALIASES.get(obj.get("script"), obj.get("script"))
                if is_item(obj):
                    if not available_pickup_flags:
                        raise SystemExit("not enough unused FLAG_EC_ITEM slots for restored pickups")
                    item_flag = obj.get("flag")
                    if item_flag not in defined_flags or item_flag in used_flags:
                        item_flag = available_pickup_flags.pop(0)
                    used_flags.add(item_flag)
                    obj["flag"] = item_flag
                    obj["script"] = "Common_EventScript_FindItem"
                    obj["trainer_sight_or_berry_tree_id"] = infer_old_item(
                        source_path.parent / "scripts.inc", reference.get("script")
                    )
                    pickups += 1
                else:
                    if script in {"0x0", "NULL", None}:
                        obj["script"] = "0x0"
                    elif script not in labels:
                        raise SystemExit(
                            f"{source_path.parent.name}: unresolved Inclement NPC script "
                            f"{script}; add an explicit modern mapping before restoring"
                        )
                    else:
                        obj["script"] = script
                    if obj.get("flag") not in {"0", None} and obj.get("flag") not in defined_flags:
                        raise SystemExit(
                            f"{source_path.parent.name}: unresolved Inclement object flag "
                            f"{obj.get('flag')}; add an explicit lifecycle mapping before restoring"
                        )
                if obj.get("trainer_type") == "0":
                    obj["trainer_type"] = "TRAINER_TYPE_NONE"

            for field in VISUAL_FIELDS:
                obj[field] = reference[field]
            obj["graphics_id"] = REFERENCE_TO_CURRENT_GFX.get(
                reference["graphics_id"], reference["graphics_id"]
            )
            output.append(obj)

        removed += len(current_objects) - len(used)
        if source_path.parent.name in CENTER_MAPS:
            output.extend(standardized_center_services())
        new["object_events"] = output
        save(destination, new)

    print(f"matched={matched}")
    print(f"restored={restored} (pickups={pickups}, fallback_npcs={fallback_npcs})")
    print(f"removed_non_inclement_objects={removed}")


if __name__ == "__main__":
    main()
