#!/usr/bin/env python3
"""Render one native frame beside every ground Mega Stone for placement review.

Usage:
  stone_sweep.py header            # overwrite the map-sweep fixture header with stone rows
  stone_sweep.py render OUT --rom ROM --elf ELF [--inclement DIR]
                                   # render every stone (player one tile below it) and
                                   # write 4-column contact sheets, 16 stones per sheet
Restore the header afterwards with:
  git checkout include/emerald_champions_headless_map_sweep.h
The fixture header swap is temporary review tooling, not release evidence.
"""
import argparse
import hashlib
import json
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import render_emerald_champions_ui as R  # noqa: E402
from audit.map_sweep_render import scenario_id  # noqa: E402

HEADER = ROOT / "include/emerald_champions_headless_map_sweep.h"
BALLS = ("OBJ_EVENT_GFX_ITEM_BALL", "OBJ_EVENT_GFX_GOLD_ITEM_BALL", "OBJ_EVENT_GFX_MEGA_STONE")


def maps(root):
    groups = json.loads((root / "data/maps/map_groups.json").read_text())
    out = {}
    for group in groups["group_order"]:
        for name in groups[group]:
            path = root / "data/maps" / name / "map.json"
            if path.exists():
                out[name] = json.loads(path.read_text())
    return out


def stones(inclement):
    ec = maps(ROOT)
    inc = maps(inclement) if inclement and inclement.exists() else {}
    rows = []
    for name, data in ec.items():
        authored = {(o["x"], o["y"]): o.get("graphics_id") for o in inc.get(name, {}).get("object_events", [])}
        for o in data.get("object_events", []):
            if o.get("graphics_id") != "OBJ_EVENT_GFX_MEGA_STONE":
                continue
            gfx = authored.get((o["x"], o["y"]))
            status = ("authored-ball" if gfx == "OBJ_EVENT_GFX_ITEM_BALL"
                      else "authored-stone" if gfx in BALLS
                      else "reused-npc-tile" if gfx else "INVENTED")
            rows.append({"map": name, "id": data["id"], "x": o["x"], "y": o["y"],
                         "item": o["trainer_sight_or_berry_tree_id"].replace("ITEM_", ""),
                         "status": status})
    return rows


def write_header(rows):
    lines = ["// TEMPORARY stone sweep rows written by scripts/audit/stone_sweep.py;",
             "// restore with: git checkout include/emerald_champions_headless_map_sweep.h"]
    lines += [f"EC_HEADLESS_MAP_SWEEP({r['id']}, {r['x']}, {r['y'] + 1})" for r in rows]
    HEADER.write_text("\n".join(lines) + "\n")


def render(rows, out, rom, elf):
    out.mkdir(parents=True, exist_ok=True)
    runner = R.build_runner()
    addresses = {key: R.resolve_symbol(elf, symbol) for key, symbol in (
        ("scenario_address", R.SCENARIO_SYMBOL), ("param_address", "gEcHeadlessFixtureParam"),
        ("trigger_address", "gEcHeadlessFixtureTrigger"), ("setup_address", "gEcHeadlessFixtureSetupResult"),
        ("observed_address", "gEcHeadlessFixtureObservedResult"))}
    sweep = scenario_id()
    panels = []
    for index, r in enumerate(rows):
        name = f"{index:02d}-{r['map']}-{r['item']}"
        try:
            result = R.render_one(name, {"id": sweep, "param": index, "frames": 800, "keys": [], "verify": True},
                                  runner=runner, rom=rom, out=out, **addresses)
            r["verified"] = bool(result.get("verified_runtime_state"))
        except Exception as error:  # keep going; the sheet marks the failure
            r["verified"] = False
            r["error"] = f"{type(error).__name__}: {error}"
            print(f"{name}: {r['error']}", file=sys.stderr)
        r["png"] = f"{name}.png"
        flag = "" if r["verified"] else " [RENDER FAILED]"
        panels.append({"path": r["png"], "label": f"{r['item']} {r['map']} ({r['x']},{r['y']}) {r['status']}{flag}"})
    build = {"rom_sha256": hashlib.sha256(rom.read_bytes()).hexdigest(),
             "elf_sha256": hashlib.sha256(elf.read_bytes()).hexdigest()}
    (out / "stone_sweep.json").write_text(json.dumps({"build": build, "stones": rows}, indent=2) + "\n")
    sheets = []
    for start in range(0, len(panels), 16):
        n = start // 16 + 1
        manifest = out / f"sheet-{n}.json"
        manifest.write_text(json.dumps({"title": f"Ground Mega Stones, sheet {n}: stones {start + 1}-{min(start + 16, len(panels))} of {len(panels)}",
                                        "scope": "Player stands one tile below each stone. Native frames; placement review only.",
                                        "build": build, "panels": panels[start:start + 16]}, indent=2))
        sheet = out / f"sheet-{n}.png"
        subprocess.run(["/usr/bin/python3", str(ROOT / "scripts/audit/render_contact_sheet.py"), str(manifest),
                        "--out", str(sheet), "--columns", "4"], check=True)
        sheets.append(sheet)
    return sheets


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("mode", choices=("header", "render", "list", "spots-header", "spots-render"))
    parser.add_argument("--spot", action="append", default=[], help="MAP_ID:x:y (player stands here); repeatable")
    parser.add_argument("out", type=Path, nargs="?")
    parser.add_argument("--rom", type=Path)
    parser.add_argument("--elf", type=Path)
    parser.add_argument("--inclement", type=Path, default=ROOT.parent / "inclement-game-source")
    args = parser.parse_args(argv)
    if args.mode.startswith("spots"):
        rows = []
        for spec in args.spot:
            map_id, x, y = spec.split(":")
            rows.append({"map": map_id.replace("MAP_", ""), "id": map_id, "x": int(x), "y": int(y) - 1,
                         "item": "SPOT", "status": "spot"})
        if args.mode == "spots-header":
            write_header(rows)
            print(f"wrote {len(rows)} spot rows")
            return 0
        if not (args.out and args.rom and args.elf):
            parser.error("spots-render needs OUT --rom --elf")
        for sheet in render(rows, args.out, args.rom.resolve(), args.elf.resolve()):
            print(sheet)
        return 0
    rows = stones(args.inclement)
    if args.mode == "list":
        for r in rows:
            print(f"{r['item']:18} {r['map']:28} ({r['x']},{r['y']}) {r['status']}")
        return 0
    if args.mode == "header":
        write_header(rows)
        print(f"wrote {len(rows)} stone rows to {HEADER.relative_to(ROOT)}")
        return 0
    if not (args.out and args.rom and args.elf):
        parser.error("render needs OUT --rom --elf")
    for sheet in render(rows, args.out, args.rom.resolve(), args.elf.resolve()):
        print(sheet)
    return 0


if __name__ == "__main__":
    sys.exit(main())
