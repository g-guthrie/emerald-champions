#!/usr/bin/env python3
"""Ground Mega Stone placement gate.

Every OBJ_EVENT_GFX_MEGA_STONE sparkle must sit on a designer-authored pickup tile
(data/emerald_champions/authored_pickup_tiles.json, snapshotted from Inclement Emerald)
or on an explicitly approved tile below; no two stones may share one screen (15x10
metatiles); and no item ball may sit within three tiles of a stone. Invented sparkles
fail; use a scripted gift instead. Exit 1 on any violation.
"""
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
AUTHORED = json.loads((ROOT / "data/emerald_champions/authored_pickup_tiles.json").read_text())["maps"]
# Reviewed by hand on native contact sheets (work/stone-sweep, September 15, 2026).
APPROVED = {
    ("JaggedPass", 8, 10): "Scraftinite, ledge pocket on the hop trail",
    ("JaggedPass", 7, 29): "Scovillainite, shelf above the cave mouth",
    ("AshenWoods", 6, 40): "Pinsirite, between the trunks",
    ("Route109", 25, 5): "Sharpedonite, nudged under the umbrella",
}
PICKUPS = ("OBJ_EVENT_GFX_MEGA_STONE", "OBJ_EVENT_GFX_ITEM_BALL", "OBJ_EVENT_GFX_GOLD_ITEM_BALL")
HALF_W, HALF_H = 7, 5
BALL_RADIUS = 3


def main() -> int:
    groups = json.loads((ROOT / "data/maps/map_groups.json").read_text())
    errors = []
    stones = 0
    for group in groups["group_order"]:
        for name in groups[group]:
            path = ROOT / "data/maps" / name / "map.json"
            if not path.exists():
                continue
            objs = json.loads(path.read_text()).get("object_events", [])
            pickups = [o for o in objs if o.get("graphics_id") in PICKUPS]
            for o in pickups:
                if o["graphics_id"] != "OBJ_EVENT_GFX_MEGA_STONE":
                    continue
                stones += 1
                item = o["trainer_sight_or_berry_tree_id"]
                tile = (o["x"], o["y"])
                if [o["x"], o["y"]] not in AUTHORED.get(name, []) and (name, *tile) not in APPROVED:
                    errors.append(f"{name} {item} at {tile}: not an authored or approved tile")
                for p in pickups:
                    if p is o:
                        continue
                    dx, dy = abs(p["x"] - o["x"]), abs(p["y"] - o["y"])
                    other = p["trainer_sight_or_berry_tree_id"]
                    if p["graphics_id"] == "OBJ_EVENT_GFX_MEGA_STONE" and dx <= HALF_W and dy <= HALF_H:
                        errors.append(f"{name} {item} at {tile}: shares a screen with {other} at ({p['x']}, {p['y']})")
                    elif p["graphics_id"] != "OBJ_EVENT_GFX_MEGA_STONE" and max(dx, dy) <= BALL_RADIUS:
                        errors.append(f"{name} {item} at {tile}: item ball {other} within {BALL_RADIUS} tiles at ({p['x']}, {p['y']})")
    for e in errors:
        print("FAIL", e)
    print(f"{stones} ground stones checked, {len(errors)} violations")
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main())
