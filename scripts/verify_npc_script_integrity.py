#!/usr/bin/env python3
"""Every NPC's script must exist and do something, and every jump must resolve.

These are absolute invariants rather than comparisons to Inclement, so they hold no
matter how much Champions redesigns a scene:

  1. Every object_event script label is defined somewhere.
  2. No NPC that Inclement scripted has been left standing on the same tile with no
     script at all (a silent NPC is almost always an accident, not a design).
  3. Every msgbox / applymovement / goto / call target resolves to a real symbol.

Deliberately NOT checked: how much branching a script has. Champions modernized the old
`compare` + `goto_if_eq` idiom into one instruction and rewrote many scenes, so counting
conditionals produces hundreds of false alarms and proves nothing.
"""
from __future__ import annotations

import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BASELINE = "cf41a95b68a39ca74fefeb934c460f6f47eb0b3b"
NULL_SCRIPTS = {"0x0", "0", "", "NULL", None}
LABEL_RE = re.compile(r"(?m)^([A-Za-z_][A-Za-z0-9_]*)::?[ \t]*(?:@.*)?$")


def show(path: str) -> str | None:
    try:
        return subprocess.check_output(
            ["git", "show", f"{BASELINE}:{path}"], cwd=ROOT, text=True, stderr=subprocess.DEVNULL
        )
    except subprocess.CalledProcessError:
        return None


def main() -> None:
    script_sources = list((ROOT / "data").rglob("*.inc")) + list((ROOT / "data").rglob("*.s"))
    bodies = {path: path.read_text(errors="ignore") for path in script_sources}

    defined: set[str] = set()
    for text in bodies.values():
        defined |= set(LABEL_RE.findall(text))
    # Symbols the scripts borrow from C (gStringVar4, gText_*) and constants from headers
    # (MSGBOX_SIGN, DIR_NORTH), which appear where a macro takes them as an argument.
    for path in list((ROOT / "src").rglob("*.c")) + list((ROOT / "include").rglob("*.h")):
        try:
            text = path.read_text(errors="ignore")
        except OSError:
            continue
        defined |= set(re.findall(r"\b(g[A-Z][A-Za-z0-9_]*)\b", text))
        defined |= set(re.findall(r"(?m)^\s*#\s*define\s+([A-Z_][A-Z0-9_]*)", text))

    failures: list[str] = []
    npc_count = 0
    silent_checked = 0

    for map_json in sorted((ROOT / "data/maps").glob("*/map.json")):
        name = map_json.parent.name
        payload = json.loads(map_json.read_text())
        current = payload.get("object_events") or []
        for obj in current:
            script = obj.get("script")
            if script in NULL_SCRIPTS:
                continue
            npc_count += 1
            if script not in defined:
                failures.append(f"{name}: {obj.get('graphics_id')} points at missing script {script}")

        if name.endswith("_Frlg"):
            continue
        old_raw = show(f"data/maps/{name}/map.json")
        if old_raw is None:
            continue
        # Several tiles hold two objects (a Kecleon and its shadow, a rival and a parent),
        # so match on sprite as well as position and accept any scripted twin on the tile.
        by_tile: dict[tuple, list] = {}
        for o in current:
            by_tile.setdefault((o.get("x"), o.get("y")), []).append(o)
        for obj in json.loads(old_raw).get("object_events") or []:
            if obj.get("script") in NULL_SCRIPTS:
                continue
            twins = by_tile.get((obj.get("x"), obj.get("y")))
            if not twins:
                continue
            silent_checked += 1
            same_sprite = [t for t in twins if t.get("graphics_id") == obj.get("graphics_id")]
            candidates = same_sprite or twins
            if all(t.get("script") in NULL_SCRIPTS for t in candidates):
                failures.append(
                    f"{name}: {obj.get('graphics_id')} at "
                    f"({obj.get('x')},{obj.get('y')}) lost its script entirely"
                )

    jump = re.compile(r"(?m)^\s*(?:msgbox|applymovement\s+[^,\n]+,|goto|call)\s+([A-Za-z_][A-Za-z0-9_]*)")
    targets = 0
    for path, text in bodies.items():
        for match in jump.finditer(text):
            targets += 1
            if match.group(1) not in defined:
                failures.append(f"{path.relative_to(ROOT)}: jump to undefined {match.group(1)}")

    if failures:
        print(f"NPC SCRIPT INTEGRITY: FAIL ({len(failures)})")
        for failure in failures[:40]:
            print(f"  - {failure}")
        sys.exit(1)
    print(
        f"NPC SCRIPT INTEGRITY: PASS ({npc_count} NPC scripts, {silent_checked} compared "
        f"against Inclement, {targets} jump targets resolved)"
    )


if __name__ == "__main__":
    main()
