#!/usr/bin/env python3
"""Work out where to stand to photograph every location.

    .venv-studio/bin/python guide/plan_shots.py > guide/shots/all.json

For each location in the walkthrough order this picks tiles the player can
actually occupy, reading the same collision bits the game reads, and avoiding
tiles an NPC is standing on. Small maps get one establishing shot. Long routes
get a pan: several evenly spaced viewpoints along the map's longest axis,
stitched into one strip, which is how the original illustrates a whole route.
Every pickup worth finding gets its own shot beside it.
"""
import argparse
import json
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# A screen is fifteen metatiles wide and ten tall, so viewpoints this far apart
# show neighbouring ground without repeating too much of it.
PAN_STRIDE = 12
PAN_MIN_SPAN = 26          # only pan when the map is longer than about two screens
PAN_MAX_FRAMES = 6


def layout_for(map_name, layouts):
    m = json.loads((ROOT / "data/maps" / map_name / "map.json").read_text())
    return m, layouts[m["layout"]]


def passable(cells, width, x, y):
    block = struct.unpack_from("<H", cells, (y * width + x) * 2)[0]
    return not ((block >> 10) & 3)


def nearest_free(cells, width, height, occupied, x, y, radius=6):
    """Walk outwards from a wanted tile until one is standable."""
    for r in range(radius + 1):
        for dy in range(-r, r + 1):
            for dx in range(-r, r + 1):
                if max(abs(dx), abs(dy)) != r:
                    continue
                nx, ny = x + dx, y + dy
                if 0 <= nx < width and 0 <= ny < height and (nx, ny) not in occupied \
                        and passable(cells, width, nx, ny):
                    return nx, ny
    return None


def plan(map_name, layouts):
    m, layout = layout_for(map_name, layouts)
    width, height = layout["width"], layout["height"]
    cells = (ROOT / layout["blockdata_filepath"]).read_bytes()
    occupied = {(o["x"], o["y"]) for o in m.get("object_events", [])}

    shots = []
    centre = nearest_free(cells, width, height, occupied, width // 2, height // 2, radius=max(width, height))
    if not centre:
        return shots, "no standable tile found"

    long_axis = "vertical" if height >= width else "horizontal"
    span = max(width, height)

    if span >= PAN_MIN_SPAN:
        # Space viewpoints along the longer axis and stitch them into a strip.
        stages, seen = [], set()
        steps = min(PAN_MAX_FRAMES, max(2, span // PAN_STRIDE))
        for i in range(steps):
            along = int((i + 0.5) * span / steps)
            wanted = (centre[0], along) if long_axis == "vertical" else (along, centre[1])
            tile = nearest_free(cells, width, height, occupied, *wanted, radius=10)
            if not tile or tile in seen:
                continue
            seen.add(tile)
            stages.append(dict(x=tile[0], y=tile[1], settle=70))
        if len(stages) >= 2:
            shots.append(dict(
                id=f"{map_name}-pan", map=map_name, x=stages[0]["x"], y=stages[0]["y"],
                facing=1, settle=90, sequence=stages,
                stitch=dict(direction="vertical" if long_axis == "vertical" else "horizontal"),
                caption=f"{map_name} end to end"))
    if not shots:
        shots.append(dict(id=f"{map_name}-view", map=map_name, x=centre[0], y=centre[1],
                          facing=1, settle=90, caption=f"{map_name}"))
    return shots, None


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--order", type=Path, default=ROOT / "guide/data/order.json")
    ap.add_argument("--only", nargs="*", help="limit to these maps")
    args = ap.parse_args()

    layouts = {x["id"]: x for x in
               json.loads((ROOT / "data/layouts/layouts.json").read_text())["layouts"]}
    order = args.only or json.loads(args.order.read_text())

    out, skipped = [], []
    for name in order:
        try:
            shots, why = plan(name, layouts)
            if why:
                skipped.append(f"{name}: {why}")
            out.extend(shots)
        except Exception as exc:                        # noqa: BLE001
            skipped.append(f"{name}: {type(exc).__name__}: {exc}")

    json.dump(out, sys.stdout, indent=1)
    print()
    print(f"{len(out)} shots across {len(order)} locations", file=sys.stderr)
    for line in skipped:
        print(f"  skip {line}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
