#!/usr/bin/env python3
"""Render one frame of every Hoenn map with the headless fixture ROM (scenario MAP_SWEEP).
Usage: map_sweep_render.py OUT_DIR [start] [end]"""
import sys, json, hashlib
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import render_emerald_champions_ui as R
names = json.load(open(ROOT / "scripts/audit/map_sweep_names.json"))
out = Path(sys.argv[1]); out.mkdir(parents=True, exist_ok=True)
start = int(sys.argv[2]) if len(sys.argv) > 2 else 0
end = int(sys.argv[3]) if len(sys.argv) > 3 else len(names)
rom = R.require_resident_file(R.DEFAULT_ROM, "headless fixture ROM")
elf = R.require_resident_file(R.DEFAULT_ELF, "headless fixture ELF")
runner = R.build_runner()
addr = dict(scenario_address=R.resolve_symbol(elf, R.SCENARIO_SYMBOL), param_address=R.resolve_symbol(elf, "gEcHeadlessFixtureParam"),
            trigger_address=R.resolve_symbol(elf, "gEcHeadlessFixtureTrigger"), setup_address=R.resolve_symbol(elf, "gEcHeadlessFixtureSetupResult"),
            observed_address=R.resolve_symbol(elf, "gEcHeadlessFixtureObservedResult"))
header = (ROOT / "include/emerald_champions_headless.h").read_text()
enum = [l.strip().rstrip(",") for l in header.split("enum EmeraldChampionsHeadlessScenario")[1].split("};")[0].splitlines() if l.strip().startswith("EC_HEADLESS_SCENARIO_")]
SWEEP_ID = enum.index("EC_HEADLESS_SCENARIO_MAP_SWEEP")
for i in range(start, end):
    spec = {"id": SWEEP_ID, "param": i, "frames": 800, "keys": []}
    try:
        R.render_one(f"{i:03d}-{names[i]}", spec, runner=runner, rom=rom, out=out, **addr)
    except BaseException as e:  # keep sweeping; report the failure
        print(f"{i:03d} {names[i]}: FAILED {type(e).__name__}: {str(e)[:300]}")
print("done", start, end)
