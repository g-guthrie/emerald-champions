#!/usr/bin/env python3
"""Reachability regression audit against the Inclement Emerald baseline.

Finds progression-blocking collision defects (softlocks): a warp whose every approach
tile is occupied by a permanently visible object, or one that opens into a pocket the
player cannot leave. It reads real collision bits from each layout's blockdata, so it
asserts a property of the world and cannot be satisfied by renaming scripts.

It reports only situations that DIFFER from Inclement. Vanilla has warps that are
deliberately staffed (the Cable Club attendants stand in their doorways, and the Safari
Zone exit attendant blocks the gate), so an absolute rule produces false alarms. Anything
Inclement already did is the reference; anything Champions introduced is a regression.
"""
from __future__ import annotations

import json
import struct
import subprocess
import sys
from collections import deque
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BASELINE = "cf41a95b68a39ca74fefeb934c460f6f47eb0b3b"  # Inclement Emerald v1.13


def show(rev: str, path: str) -> bytes | None:
    try:
        return subprocess.check_output(["git", "show", f"{rev}:{path}"], cwd=ROOT, stderr=subprocess.DEVNULL)
    except subprocess.CalledProcessError:
        return None


def layout_index(rev: str | None) -> dict:
    raw = (ROOT / "data/layouts/layouts.json").read_bytes() if rev is None else show(rev, "data/layouts/layouts.json")
    if raw is None:
        return {}
    return {entry["id"]: entry for entry in json.loads(raw)["layouts"]}


def collision(rev: str | None, layout: dict):
    rel = layout.get("blockdata_filepath")
    if not rel:
        return None
    raw = (ROOT / rel).read_bytes() if rev is None else show(rev, rel)
    if raw is None:
        return None
    width, height = layout["width"], layout["height"]
    if len(raw) < width * height * 2:
        return None
    blocks = struct.unpack(f"<{width * height}H", raw[: width * height * 2])
    # pokeemerald block: bits 0-9 metatile, 10-11 collision, 12-15 elevation.
    return width, height, [(block >> 10) & 0x3 for block in blocks]


def sealed_warps(rev: str | None, name: str, layouts: dict) -> set[tuple[int, int, str]] | None:
    """Return {(x, y, kind)} for warps that are unusable on this revision."""
    rel = f"data/maps/{name}/map.json"
    raw = (ROOT / rel).read_bytes() if rev is None else show(rev, rel)
    if raw is None:
        return None
    payload = json.loads(raw)
    grid = collision(rev, layouts.get(payload.get("layout"), {}))
    if grid is None:
        return None
    width, height, coll = grid

    solid = {
        (obj.get("x"), obj.get("y"))
        for obj in payload.get("object_events") or []
        if str(obj.get("flag", "0")) in ("0", "FLAG_0", "")
    }

    def walkable(x: int, y: int) -> bool:
        return 0 <= x < width and 0 <= y < height and coll[y * width + x] == 0

    bad: set[tuple[int, int, str]] = set()
    for warp in payload.get("warp_events") or []:
        wx, wy = warp.get("x"), warp.get("y")
        approaches = [
            (wx + dx, wy + dy)
            for dx, dy in ((0, 1), (0, -1), (1, 0), (-1, 0))
            if walkable(wx + dx, wy + dy)
        ]
        if not approaches:
            continue
        open_approaches = [tile for tile in approaches if tile not in solid]
        if not open_approaches:
            bad.add((wx, wy, "sealed by a permanent object"))
            continue
        reached = set(open_approaches)
        queue = deque(open_approaches)
        while queue:
            x, y = queue.popleft()
            for dx, dy in ((0, 1), (0, -1), (1, 0), (-1, 0)):
                nx, ny = x + dx, y + dy
                if (nx, ny) in reached or not walkable(nx, ny) or (nx, ny) in solid:
                    continue
                reached.add((nx, ny))
                queue.append((nx, ny))
        if len(reached) < 2:
            bad.add((wx, wy, "opens into a pocket the player cannot leave"))
    return bad


def main() -> None:
    current_layouts = layout_index(None)
    baseline_layouts = layout_index(BASELINE)
    failures: list[str] = []
    checked = 0

    for map_json in sorted((ROOT / "data/maps").glob("*/map.json")):
        name = map_json.parent.name
        if name.endswith("_Frlg"):
            continue
        now = sealed_warps(None, name, current_layouts)
        if now is None:
            continue
        checked += 1
        if not now:
            continue
        before = sealed_warps(BASELINE, name, baseline_layouts)
        introduced = now if before is None else (now - before)
        for x, y, kind in sorted(introduced):
            failures.append(f"{name}: warp ({x},{y}) {kind} — Inclement did not")

    if failures:
        print(f"MAP REACHABILITY: FAIL ({len(failures)} regression(s))")
        for failure in failures:
            print(f"  - {failure}")
        sys.exit(1)
    print(f"MAP REACHABILITY: PASS ({checked} maps compared against Inclement {BASELINE[:8]})")


if __name__ == "__main__":
    main()
