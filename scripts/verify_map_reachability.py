#!/usr/bin/env python3
"""Local warp collision heuristic against a required Inclement reference.

Flags candidate defects: a warp whose collision-free approach tiles all contain
unflagged objects, or whose approach flood fill contains fewer than two tiles.
This models collision bits and initial object coordinates only, not elevation,
metatile behavior, object movement, script state, or campaign reachability. Warps
on the map border or without collision-free adjacent tiles are counted as
unmodeled, not certified safe.

It reports only situations that DIFFER from Inclement. Vanilla has warps that are
deliberately staffed (the Cable Club attendants stand in their doorways, and the Safari
Zone exit attendant blocks the gate), so an absolute rule produces false alarms. Anything
Inclement already did is the reference. Newly added maps have no historical parity
claim; any candidate defect on them is reported separately.
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


def reference_paths(rev: str) -> set[str]:
    subprocess.check_output(
        ["git", "rev-parse", "--verify", f"{rev}^{{commit}}"], cwd=ROOT, stderr=subprocess.PIPE
    )
    raw = subprocess.check_output(
        ["git", "ls-tree", "-r", "--name-only", "-z", rev], cwd=ROOT, stderr=subprocess.PIPE
    )
    paths = set(filter(None, raw.decode().split("\0")))
    if "data/layouts/layouts.json" not in paths:
        raise ValueError("reference is missing data/layouts/layouts.json")
    return paths


def show(rev: str, path: str, paths: set[str]) -> bytes | None:
    if path not in paths:
        return None
    return subprocess.check_output(["git", "show", f"{rev}:{path}"], cwd=ROOT, stderr=subprocess.PIPE)


def layout_index(rev: str | None, paths: set[str]) -> dict:
    raw = (ROOT / "data/layouts/layouts.json").read_bytes() if rev is None else show(rev, "data/layouts/layouts.json", paths)
    if raw is None:
        raise ValueError(f"{rev}: missing layout index")
    entries = json.loads(raw)["layouts"]
    layouts = {entry["id"]: entry for entry in entries}
    if not layouts or len(layouts) != len(entries):
        raise ValueError(f"{rev or 'current'}: empty layout index or duplicate layout IDs")
    return layouts


def collision(rev: str | None, layout: dict, paths: set[str]):
    rel = layout.get("blockdata_filepath")
    if not rel:
        raise ValueError(f"{rev or 'current'}: layout has no blockdata_filepath")
    raw = (ROOT / rel).read_bytes() if rev is None else show(rev, rel, paths)
    if raw is None:
        raise ValueError(f"{rev}: missing blockdata {rel}")
    width, height = layout["width"], layout["height"]
    if not isinstance(width, int) or not isinstance(height, int) or width <= 0 or height <= 0:
        raise ValueError(f"{rev or 'current'}: invalid layout dimensions for {rel}")
    if len(raw) != width * height * 2:
        raise ValueError(f"{rev or 'current'}: blockdata size mismatch for {rel}")
    blocks = struct.unpack(f"<{width * height}H", raw)
    # pokeemerald block: bits 0-9 metatile, 10-11 collision, 12-15 elevation.
    return width, height, [(block >> 10) & 0x3 for block in blocks]


def sealed_warps(rev: str | None, name: str, layouts: dict, paths: set[str]) -> tuple[set[tuple[int, int, str]], int, int] | None:
    """Return candidate defects, total warps, and unmodeled warp count."""
    rel = f"data/maps/{name}/map.json"
    raw = (ROOT / rel).read_bytes() if rev is None else show(rev, rel, paths)
    if raw is None:
        return None
    payload = json.loads(raw)
    layout_id = payload.get("layout")
    if layout_id not in layouts:
        raise ValueError(f"{rev or 'current'}: {name} references missing layout {layout_id}")
    grid = collision(rev, layouts[layout_id], paths)
    width, height, coll = grid

    solid = {
        (obj.get("x"), obj.get("y"))
        for obj in payload.get("object_events") or []
        if str(obj.get("flag", "0")) in ("0", "FLAG_0", "")
    }

    def walkable(x: int, y: int) -> bool:
        return 0 <= x < width and 0 <= y < height and coll[y * width + x] == 0

    bad: set[tuple[int, int, str]] = set()
    unmodeled = 0
    warps = payload.get("warp_events") or []
    for warp in warps:
        wx, wy = warp.get("x"), warp.get("y")
        if not isinstance(wx, int) or not isinstance(wy, int) or not (0 <= wx <= width + 1 and 0 <= wy <= height + 1):
            raise ValueError(f"{rev or 'current'}: {name} has invalid warp coordinates ({wx}, {wy})")
        # Warp matching uses map coordinates, including native border exits
        # (Battle Dome corridor uses y=height+1). The raw layout blockdata does
        # not include the engine's padded border, so this heuristic cannot
        # establish their collision/approach behavior.
        if wx >= width or wy >= height:
            unmodeled += 1
            continue
        approaches = [
            (wx + dx, wy + dy)
            for dx, dy in ((0, 1), (0, -1), (1, 0), (-1, 0))
            if walkable(wx + dx, wy + dy)
        ]
        if not approaches:
            unmodeled += 1
            continue
        open_approaches = [tile for tile in approaches if tile not in solid]
        if not open_approaches:
            bad.add((wx, wy, "has all collision-free approaches occupied by unflagged objects"))
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
            bad.add((wx, wy, "has an approach flood fill smaller than two tiles"))
    return bad, len(warps), unmodeled


def verify() -> None:
    paths = reference_paths(BASELINE)
    current_layouts = layout_index(None, paths)
    baseline_layouts = layout_index(BASELINE, paths)
    failures: list[str] = []
    compared = new_maps = total_warps = unmodeled = baseline_warps = baseline_unmodeled = 0

    for map_json in sorted((ROOT / "data/maps").glob("*/map.json")):
        name = map_json.parent.name
        if name.endswith("_Frlg"):
            continue
        now, count, skipped = sealed_warps(None, name, current_layouts, paths)
        total_warps += count
        unmodeled += skipped
        # Read every historical counterpart, including when the current heuristic
        # finds nothing; otherwise a missing/corrupt reference can falsely pass.
        before = sealed_warps(BASELINE, name, baseline_layouts, paths)
        if before is None:
            new_maps += 1
            introduced = now
            context = "new map; no historical counterpart"
        else:
            compared += 1
            old_bad, old_count, old_skipped = before
            baseline_warps += old_count
            baseline_unmodeled += old_skipped
            introduced = now - old_bad
            context = "not flagged in the Inclement reference"
        for x, y, kind in sorted(introduced):
            failures.append(f"{name}: warp ({x},{y}) {kind} ({context})")
    if not compared:
        raise ValueError("no current maps could be compared with the reference")
    removed_maps = sum(
        p.startswith("data/maps/") and p.endswith("/map.json")
        and not p.split("/")[-2].endswith("_Frlg") and not (ROOT / p).is_file()
        for p in paths
    )
    print(
        f"Coverage: {compared} map pairs compared; {new_maps} new maps checked without a historical counterpart; "
        f"{removed_maps} reference-only maps outside this comparison; "
        f"{total_warps} current and {baseline_warps} reference warps inspected; "
        f"{unmodeled} current and {baseline_unmodeled} reference warps unmodeled (border or no collision-free adjacent tile)"
    )

    if failures:
        print(f"MAP REACHABILITY: FAIL ({len(failures)} local collision candidate(s))")
        for failure in failures:
            print(f"  - {failure}")
        sys.exit(1)
    print(f"MAP REACHABILITY: PASS (local collision heuristic only; Inclement {BASELINE[:8]}; campaign reachability not verified)")


def main() -> None:
    try:
        verify()
    except (OSError, subprocess.CalledProcessError, ValueError, KeyError) as exc:
        print(f"MAP REACHABILITY: FAIL (required input/reference unavailable or invalid: {exc})")
        sys.exit(1)


if __name__ == "__main__":
    main()
