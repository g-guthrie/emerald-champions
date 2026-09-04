#!/usr/bin/env python3
"""Compare a campaign simulator run with an explicitly approved baseline.

The campaign runner owns execution and evidence capture.  This script deliberately
does not launch the game or update a baseline implicitly: it reduces a completed
``latest-run.json`` to stable, reviewable facts, then either writes that snapshot
when asked or compares it with an existing approved snapshot.
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import re
import sys


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_RUN = ROOT / "work/campaign-playthrough/current/latest-run.json"
DEFAULT_BASELINE = ROOT / "tests/campaign/runtime_baseline.json"

TELEMETRY_KEYS = (
    "gEcHeadlessCampaignBattleSerial",
    "gEcHeadlessCampaignLastBattleType",
    "gEcHeadlessCampaignLastOpponentA",
    "gEcHeadlessCampaignLastOpponentB",
    "gEcHeadlessCampaignCaptureSerial",
    "gEcHeadlessCampaignLastCapturedSpecies",
    "gEcHeadlessCampaignLastCaptureResult",
    "gEcHeadlessCampaignMapId",
    "gEcHeadlessCampaignMapGroup",
    "gEcHeadlessCampaignMapNum",
    "gEcHeadlessCampaignPlayerX",
    "gEcHeadlessCampaignPlayerY",
    "gEcHeadlessCampaignPlayerFacing",
    "gEcHeadlessCampaignControlsLocked",
    "gEcHeadlessCampaignScriptEnabled",
    "gEcHeadlessCampaignInBattle",
)


def require(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def load_json(path: Path) -> dict:
    require(path.is_file(), f"missing JSON file: {path}")
    try:
        value = json.loads(path.read_text())
    except (OSError, json.JSONDecodeError) as error:
        raise RuntimeError(f"cannot read {path}: {error}") from error
    require(isinstance(value, dict), f"top-level JSON must be an object: {path}")
    return value


def screenshot_name(path: str, segment: str) -> str:
    parts = Path(path).parts
    if segment in parts:
        index = len(parts) - 1 - tuple(reversed(parts)).index(segment)
        return "/".join(parts[index + 1 :])
    return Path(path).name


def normalize_run(run: dict) -> dict:
    require(run.get("schema_version") == 1, "campaign run schema_version must be 1")
    require(run.get("run_kind") in ("full", "targeted"), "campaign run lacks a valid run_kind")
    rows = run.get("segments")
    require(isinstance(rows, list) and rows, "campaign run contains no segments")
    selection = run.get("selection")
    require(isinstance(selection, dict), "campaign run lacks selection provenance")
    require(
        selection.get("complete_manifest") is (run["run_kind"] == "full"),
        "campaign run kind disagrees with complete_manifest",
    )
    artifact_evidence = run.get("artifact_evidence")
    require(isinstance(artifact_evidence, dict), "campaign run lacks artifact evidence")
    normalized_artifacts = {}
    for label in ("rom", "elf", "manifest"):
        evidence = artifact_evidence.get(label)
        require(isinstance(evidence, dict), f"campaign run lacks {label} artifact evidence")
        require(evidence.get("verified_immutable") is True, f"{label} snapshot was not verified immutable")
        expected = evidence.get("snapshot_sha256")
        require(valid_sha256(expected), f"campaign run lacks a valid {label} snapshot hash")
        require(
            all(evidence.get(field) == expected for field in (
                "snapshot_sha256_after_run", "source_sha256_before",
                "source_sha256_after_copy", "source_sha256_after_run",
            )),
            f"{label} artifact hashes do not close",
        )
        require(run.get(f"{label}_sha256") == expected, f"run {label} identity disagrees with artifact evidence")
        size = evidence.get("snapshot_size")
        require(type(size) is int and size > 0, f"{label} snapshot size must be a positive integer")
        normalized_artifacts[label] = {"sha256": expected, "size": size}
    segments: dict[str, dict] = {}
    for row in rows:
        require(isinstance(row, dict), "campaign run segment must be an object")
        segment = row.get("segment")
        require(isinstance(segment, str) and segment, "campaign run segment lacks an ID")
        require(segment not in segments, f"duplicate campaign run segment: {segment}")
        parent = row.get("parent")
        require(parent is None or isinstance(parent, str) and parent, f"{segment}: invalid parent identity")
        require(parent != segment, f"{segment}: segment cannot be its own parent")
        if run["run_kind"] == "full" and parent is not None:
            require(parent in segments, f"{segment}: parent was not completed earlier in this full run")
        require(row.get("rom_sha256") == run["rom_sha256"], f"{segment}: ROM identity differs from run")
        row_artifacts = row.get("artifact_evidence")
        require(isinstance(row_artifacts, dict), f"{segment}: artifact provenance is missing")
        for label, evidence in artifact_evidence.items():
            require(row_artifacts.get(label) == evidence, f"{segment}: {label} provenance differs from finalized run evidence")
        for kind in ("state", "save"):
            require(isinstance(row.get(kind), str) and row[kind], f"{segment}: {kind} path is missing")
            require(valid_sha256(row.get(f"{kind}_sha256")), f"{segment}: {kind} hash is invalid")
        telemetry = row.get("telemetry")
        require(isinstance(telemetry, dict), f"{segment}: telemetry is missing")
        missing = [key for key in TELEMETRY_KEYS if key not in telemetry]
        require(not missing, f"{segment}: telemetry keys are missing: {', '.join(missing)}")
        require(all(type(telemetry[key]) is int and 0 <= telemetry[key] <= 0xFFFFFFFF for key in TELEMETRY_KEYS),
                f"{segment}: telemetry values must be unsigned 32-bit integers")
        shots = row.get("screenshots")
        require(isinstance(shots, list) and shots, f"{segment}: screenshots are missing")
        normalized_shots: dict[str, str] = {}
        for shot in shots:
            require(isinstance(shot, dict), f"{segment}: invalid screenshot record")
            require(isinstance(shot.get("path"), str) and shot["path"], f"{segment}: screenshot path is missing")
            name = screenshot_name(shot["path"], segment)
            pixel_hash = shot.get("pixel_sha256")
            require(name and name not in normalized_shots, f"{segment}: duplicate screenshot {name}")
            require(
                valid_sha256(pixel_hash),
                f"{segment}/{name}: invalid decoded-pixel hash",
            )
            require(valid_sha256(shot.get("png_sha256")), f"{segment}/{name}: invalid PNG hash")
            normalized_shots[name] = pixel_hash
        assertions = row.get("assertions", {"flags": {}, "vars": {}})
        require(isinstance(assertions, dict), f"{segment}: assertions are invalid")
        normalized_assertions: dict[str, dict] = {}
        for kind in ("flags", "vars"):
            values = assertions.get(kind, {})
            require(isinstance(values, dict), f"{segment}: {kind} assertions are invalid")
            for name, assertion in values.items():
                require(isinstance(assertion, dict), f"{segment}: invalid assertion {name}")
                require(assertion.get("passed") is True, f"{segment}: failed assertion persisted for {name}")
                require(type(assertion.get("id")) is int and assertion["id"] >= 0, f"{segment}: assertion {name} lacks an ID")
                require(type(assertion.get("actual")) is int and type(assertion.get("expected")) is int,
                        f"{segment}: assertion {name} lacks integer expected/actual values")
                require(
                    assertion.get("actual") == assertion.get("expected"),
                    f"{segment}: assertion mismatch persisted for {name}",
                )
            normalized_assertions[kind] = values
        segments[segment] = {
            "parent": row.get("parent"),
            "telemetry": {key: telemetry[key] for key in TELEMETRY_KEYS},
            "screenshots": dict(sorted(normalized_shots.items())),
            "assertions": normalized_assertions,
        }
    require(selection.get("segment_ids") == list(segments), "completed segment identities/order disagree with run selection")
    return {
        "schema_version": 1,
        "kind": "emerald-champions-campaign-runtime-baseline",
        "segment_count": len(segments),
        "artifacts": normalized_artifacts,
        "segments": segments,
    }


def valid_sha256(value: object) -> bool:
    return isinstance(value, str) and re.fullmatch(r"[0-9a-f]{64}", value) is not None


def compare(expected: dict, actual: dict) -> list[str]:
    errors: list[str] = []
    if expected.get("schema_version") != 1 or expected.get("kind") != actual["kind"]:
        return ["baseline schema or kind is incompatible"]
    expected_segments = expected.get("segments")
    if not isinstance(expected_segments, dict):
        return ["baseline has no segment table"]
    actual_segments = actual["segments"]
    expected_ids = list(expected_segments)
    actual_ids = list(actual_segments)
    if expected_ids != actual_ids:
        missing = [item for item in expected_ids if item not in actual_segments]
        added = [item for item in actual_ids if item not in expected_segments]
        if missing:
            errors.append("missing segments: " + ", ".join(missing))
        if added:
            errors.append("new unreviewed segments: " + ", ".join(added))
        if not missing and not added:
            errors.append("segment order changed")
    for segment in expected_ids:
        if segment not in actual_segments:
            continue
        before = expected_segments[segment]
        after = actual_segments[segment]
        if before.get("parent") != after.get("parent"):
            errors.append(
                f"{segment}: parent changed from {before.get('parent')!r} to {after.get('parent')!r}"
            )
        before_telemetry = before.get("telemetry", {})
        after_telemetry = after["telemetry"]
        for key in TELEMETRY_KEYS:
            if before_telemetry.get(key) != after_telemetry.get(key):
                errors.append(
                    f"{segment}: {key} changed from {before_telemetry.get(key)!r} "
                    f"to {after_telemetry.get(key)!r}"
                )
        before_shots = before.get("screenshots", {})
        after_shots = after["screenshots"]
        if set(before_shots) != set(after_shots):
            errors.append(f"{segment}: screenshot inventory changed")
        for name in sorted(set(before_shots) & set(after_shots)):
            if before_shots[name] != after_shots[name]:
                errors.append(f"{segment}/{name}: decoded pixels changed")
        if before.get("assertions", {"flags": {}, "vars": {}}) != after["assertions"]:
            errors.append(f"{segment}: named flag/variable assertions changed")
    if expected.get("artifacts") != actual.get("artifacts"):
        errors.append("immutable ROM/ELF artifact identity changed")
    return errors


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--run", type=Path, default=DEFAULT_RUN)
    parser.add_argument("--baseline", type=Path, default=DEFAULT_BASELINE)
    parser.add_argument("--write-baseline", action="store_true")
    args = parser.parse_args()

    actual = normalize_run(load_json(args.run))
    serialized = json.dumps(actual, indent=2, sort_keys=False) + "\n"
    if args.write_baseline:
        args.baseline.parent.mkdir(parents=True, exist_ok=True)
        args.baseline.write_text(serialized)
        print(f"WROTE: {args.baseline} ({actual['segment_count']} reviewed segments)")
        return 0

    expected = load_json(args.baseline)
    errors = compare(expected, actual)
    if errors:
        print(f"CAMPAIGN RUNTIME BASELINE: FAIL ({len(errors)} differences)")
        for error in errors:
            print(f"  - {error}")
        print("Inspect the run evidence before accepting intentional changes with --write-baseline.")
        return 1
    print(f"CAMPAIGN RUNTIME BASELINE: PASS ({actual['segment_count']} segments)")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError) as error:
        print(f"campaign runtime baseline: FAIL: {error}", file=sys.stderr)
        raise SystemExit(1)
