#!/usr/bin/env python3
"""Render canonical map fixtures: OUT_DIR [start] [end] --rom ROM --elf ELF --stamp STAMP.

This checks fixture state and rendering, not campaign traversal or visual fidelity.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "scripts"))
import render_emerald_champions_ui as R
from rom_artifacts import verify_rom_elf_pair
from stamp_release_inputs import digest_tree, verify_stamp


def load_census(root=ROOT):
    names = json.loads((root / "scripts/audit/map_sweep_names.json").read_text())
    if (not isinstance(names, list) or not names
            or any(not isinstance(n, str) or not re.fullmatch(r"[A-Za-z0-9_]+", n) for n in names)
            or len(set(names)) != len(names)):
        raise ValueError("map sweep names must be nonempty, unique map names")
    groups = json.loads((root / "data/maps/map_groups.json").read_text())
    maps = {name: json.loads((root / "data/maps" / name / "map.json").read_text())
            for group in groups["group_order"] for name in groups[group]}
    canonical = [name for name, data in maps.items() if data["region"] == "REGION_HOENN"]
    if names != canonical:
        raise ValueError("map sweep names differ from canonical map group order")
    header = (root / "include/emerald_champions_headless_map_sweep.h").read_text()
    rows = re.findall(r"^EC_HEADLESS_MAP_SWEEP\((MAP_\w+),\s*(-?\d+),\s*(-?\d+)\)\s*$", header, re.M)
    if len(rows) != len(names):
        raise ValueError("map sweep fixture count differs from canonical census")
    layouts = {row["id"]: row for row in json.loads(
        (root / "data/layouts/layouts.json").read_text())["layouts"]}
    for name, (map_id, x, y) in zip(names, rows):
        data = maps[name]
        if data["id"] != map_id:
            raise ValueError(f"map sweep fixture order mismatch: {name} versus {map_id}")
        if not all(0 <= int(v) <= 32767 for v in (x, y)):
            raise ValueError(f"map sweep coordinates outside signed 16-bit range: {name}")
        layout = layouts[data["layout"]]
        if not (int(x) < layout["width"] and int(y) < layout["height"]):
            raise ValueError(f"map sweep coordinates outside layout bounds: {name}")
    return names, rows


def scenario_id():
    header = (ROOT / "include/emerald_champions_headless.h").read_text()
    body = header.split("enum EmeraldChampionsHeadlessScenario")[1].split("};")[0]
    entries = [line.strip().rstrip(",") for line in body.splitlines()
               if line.strip().startswith("EC_HEADLESS_SCENARIO_")]
    return entries.index("EC_HEADLESS_SCENARIO_MAP_SWEEP")


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("out", type=Path)
    parser.add_argument("start", type=int, nargs="?", default=0)
    parser.add_argument("end", type=int, nargs="?")
    parser.add_argument("--rom", type=Path, required=True)
    parser.add_argument("--elf", type=Path, required=True)
    parser.add_argument("--stamp", type=Path, required=True,
                        help="input stamp created in the same tree as the fixture build")
    args = parser.parse_args(argv)
    report = {"status": "failed", "artifacts": {}, "outcomes": [],
              "scope": "fixture rendering and native state; not campaign traversal or visual fidelity"}
    args.out.mkdir(parents=True, exist_ok=True)
    try:
        names, rows = load_census()
        end = len(names) if args.end is None else args.end
        if not 0 <= args.start < end <= len(names):
            raise ValueError(f"selection must satisfy 0 <= start < end <= {len(names)}")
        report["selection"] = {"start": args.start, "end": end, "census_count": len(names)}
        rom = R.require_resident_file(args.rom.resolve(), "headless fixture ROM")
        elf = R.require_resident_file(args.elf.resolve(), "headless fixture ELF")
        for label, path in (("rom", rom), ("elf", elf)):
            report["artifacts"][label] = {"path": str(path), "sha256": hashlib.sha256(path.read_bytes()).hexdigest()}
        stamp_path = R.require_resident_file(args.stamp.resolve(), "fixture input stamp")
        stamp_bytes = stamp_path.read_bytes()
        stamp = json.loads(stamp_bytes)
        digest, count = digest_tree()
        verify_stamp(stamp, digest, count, (rom, elf))
        report["source_stamp"] = {"path": str(stamp_path),
                                  "sha256": hashlib.sha256(stamp_bytes).hexdigest(),
                                  "inputs_sha256": digest, "input_count": count}
        verify_rom_elf_pair(rom, elf)
        report["artifacts"]["pair_verified"] = True
        runner = R.build_runner()
        addresses = {key: R.resolve_symbol(elf, symbol) for key, symbol in (
            ("scenario_address", R.SCENARIO_SYMBOL),
            ("param_address", "gEcHeadlessFixtureParam"),
            ("trigger_address", "gEcHeadlessFixtureTrigger"),
            ("setup_address", "gEcHeadlessFixtureSetupResult"),
            ("observed_address", "gEcHeadlessFixtureObservedResult"))}
        sweep_id = scenario_id()
        for index in range(args.start, end):
            name = names[index]
            map_id, x, y = rows[index]
            outcome = {"index": index, "map": name, "expected_map": map_id,
                       "expected_position": [int(x), int(y)], "status": "failed"}
            report["outcomes"].append(outcome)
            try:
                spec = {"id": sweep_id, "param": index, "frames": 800,
                        "keys": [], "verify": True}
                outcome["render"] = R.render_one(f"{index:03d}-{name}", spec,
                    runner=runner, rom=rom, out=args.out, **addresses)
                if not outcome["render"].get("verified_runtime_state"):
                    raise RuntimeError("renderer did not verify native fixture state")
                outcome["status"] = "passed"
            except Exception as error:
                outcome["error"] = f"{type(error).__name__}: {error}"
                print(f"{index:03d} {name}: FAILED {outcome['error']}", file=sys.stderr)
        # Recheck against the original stamp to reject source/artifact changes mid-run.
        digest, count = digest_tree()
        verify_stamp(stamp, digest, count, (rom, elf))
        report["artifacts"]["unchanged_after_sweep"] = True
        report["failed_count"] = sum(row["status"] != "passed" for row in report["outcomes"])
        report["status"] = "failed" if report["failed_count"] else "passed"
    except Exception as error:
        report["error"] = f"{type(error).__name__}: {error}"
        print(report["error"], file=sys.stderr)
    finally:
        # Interrupts still propagate, leaving evidence that this run did not pass.
        (args.out / "map_sweep_report.json").write_text(json.dumps(report, indent=2) + "\n")
    return 0 if report["status"] == "passed" else 1


if __name__ == "__main__":
    sys.exit(main())
