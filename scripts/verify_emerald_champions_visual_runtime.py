#!/usr/bin/env python3
"""Render and pixel-verify the complete Inclement migration seam matrix."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
import tempfile
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
RENDERER = ROOT / "scripts" / "render_emerald_champions_ui.py"
BASELINE = ROOT / "tests/headless/inclement_visual_runtime_baseline.json"


def require(condition: bool, message: str) -> None:
    if not condition:
        raise SystemExit(message)


def baseline_payload(manifest: dict) -> dict:
    rendered = manifest.get("rendered", [])
    require(rendered, "Inclement seam renderer produced no scenarios")
    scenarios = {}
    for row in rendered:
        name = row["name"]
        require(name not in scenarios, f"duplicate rendered scenario: {name}")
        # Scenarios that declare a headless "verify" observer carry semantic
        # runtime proof; every other scenario is pinned by decoded pixels only.
        verified = row.get("verified_runtime_state") is True
        pixel_hash = row.get("pixel_sha256")
        require(
            isinstance(pixel_hash, str) and len(pixel_hash) == 64,
            f"scenario lacks decoded-pixel hash: {name}",
        )
        scenarios[name] = {
            "pixel_sha256": pixel_hash,
            "frames": row["frames"],
            "keys": row["keys"],
            "verified_runtime_state": verified,
        }
    verified_count = sum(1 for row in scenarios.values() if row["verified_runtime_state"])
    require(verified_count > 0, "no seam scenario carries semantic runtime proof")
    return {
        "schema_version": 2,
        "scenario_group": "inclement-seams",
        "scenario_count": len(scenarios),
        "verified_scenario_count": verified_count,
        "scenarios": dict(sorted(scenarios.items())),
    }


def render(*, rom: Path, elf: Path, out: Path) -> dict:
    command = [
        sys.executable,
        str(RENDERER),
        "inclement-seams",
        "--rom",
        str(rom),
        "--elf",
        str(elf),
        "--out",
        str(out),
    ]
    result = subprocess.run(
        command,
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        check=False,
    )
    if result.returncode != 0:
        raise SystemExit(
            f"Inclement seam render failed ({result.returncode}):\n{result.stdout[-12000:]}"
        )
    manifest_path = out / "manifest.inclement-seams.json"
    require(manifest_path.is_file(), f"renderer omitted {manifest_path}")
    return json.loads(manifest_path.read_text())


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--rom", type=Path, default=ROOT / "pokeemerald-headless.gba")
    parser.add_argument("--elf", type=Path, default=ROOT / "pokeemerald-headless.elf")
    parser.add_argument("--out", type=Path)
    parser.add_argument("--write-baseline", action="store_true")
    args = parser.parse_args()

    require(args.rom.is_file(), f"headless ROM is missing: {args.rom}")
    require(args.elf.is_file(), f"headless ELF is missing: {args.elf}")

    if args.out is None:
        with tempfile.TemporaryDirectory(prefix="emerald-champions-visual-runtime-") as temp:
            payload = baseline_payload(render(rom=args.rom, elf=args.elf, out=Path(temp)))
    else:
        args.out.mkdir(parents=True, exist_ok=True)
        payload = baseline_payload(render(rom=args.rom, elf=args.elf, out=args.out))

    serialized = json.dumps(payload, indent=2, sort_keys=True) + "\n"
    if args.write_baseline:
        BASELINE.write_text(serialized)
        print(f"wrote {BASELINE} ({payload['scenario_count']} scenarios)")
        return

    require(BASELINE.is_file(), f"visual runtime baseline is missing: {BASELINE}")
    expected = json.loads(BASELINE.read_text())
    require(
        payload["verified_scenario_count"] >= expected.get("verified_scenario_count", 0),
        "seam scenarios lost semantic runtime proof; restore their headless verify observers",
    )
    require(
        payload == expected,
        "Inclement visual runtime baseline drifted; inspect the rendered matrix, then run "
        "python3 scripts/verify_emerald_champions_visual_runtime.py --write-baseline",
    )
    print(
        "EMERALD CHAMPIONS VISUAL RUNTIME: PASS "
        f"({payload['scenario_count']} pixel fixtures, "
        f"{payload['verified_scenario_count']} with semantic runtime proof)"
    )


if __name__ == "__main__":
    main()
