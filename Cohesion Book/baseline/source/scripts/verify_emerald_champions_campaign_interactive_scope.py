#!/usr/bin/env python3
"""Validate native-interaction coverage declared by the campaign simulator."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import sys


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_SCOPE = ROOT / "tests/campaign/interactive_flow_scope.json"
DEFAULT_MANIFEST = ROOT / "tests/campaign/playthrough.json"


class ScopeError(RuntimeError):
    pass


def require(condition: bool, message: str) -> None:
    if not condition:
        raise ScopeError(message)


def load(path: Path) -> dict:
    require(path.is_file(), f"missing JSON file: {path}")
    value = json.loads(path.read_text())
    require(isinstance(value, dict), f"top-level JSON must be an object: {path}")
    return value


def validate_scope(scope: dict) -> dict[str, dict]:
    require(scope.get("schema_version") == 1, "interactive scope schema_version must be 1")
    allowed_modes = scope.get("allowed_modes")
    flows = scope.get("flows")
    require(isinstance(allowed_modes, list) and allowed_modes, "interactive scope has no modes")
    require(len(allowed_modes) == len(set(allowed_modes)), "interactive scope modes are duplicated")
    require(isinstance(flows, list) and flows, "interactive scope has no flows")
    by_id: dict[str, dict] = {}
    for flow in flows:
        require(isinstance(flow, dict), "interactive flow must be an object")
        flow_id = flow.get("id")
        require(isinstance(flow_id, str) and flow_id, "interactive flow lacks an ID")
        require(flow_id not in by_id, f"duplicate interactive flow: {flow_id}")
        require(flow.get("classification") in ("main_story", "optional"), f"{flow_id}: invalid classification")
        require(flow.get("mode") in allowed_modes, f"{flow_id}: invalid evidence mode")
        require(isinstance(flow.get("generic_autowin"), bool), f"{flow_id}: generic_autowin must be boolean")
        evidence = flow.get("evidence")
        require(isinstance(evidence, list) and evidence, f"{flow_id}: source evidence is empty")
        for item in evidence:
            require(isinstance(item, dict), f"{flow_id}: evidence row must be an object")
            path = ROOT / str(item.get("path", ""))
            needle = item.get("contains")
            require(path.is_file(), f"{flow_id}: evidence file is missing: {path}")
            require(isinstance(needle, str) and needle, f"{flow_id}: evidence token is empty")
            require(needle in path.read_text(errors="replace"), f"{flow_id}: lost evidence {needle!r} in {path}")
        by_id[flow_id] = flow
    return by_id


def manifest_claims(manifest: dict, flows: dict[str, dict]) -> tuple[dict[str, list[str]], list[str]]:
    require(manifest.get("schema_version") == 1, "playthrough manifest schema_version must be 1")
    segments = manifest.get("segments")
    require(isinstance(segments, list), "playthrough manifest has no segment list")
    claims: dict[str, list[str]] = {flow_id: [] for flow_id in flows}
    errors: list[str] = []
    for segment in segments:
        segment_id = segment.get("id", "<unknown>")
        coverage = segment.get("coverage", {})
        if not isinstance(coverage, dict):
            errors.append(f"{segment_id}: coverage must be an object")
            continue
        declared = coverage.get("interactive_flows", [])
        if not isinstance(declared, list) or not all(isinstance(item, str) for item in declared):
            errors.append(f"{segment_id}: coverage.interactive_flows must be a string array")
            continue
        if len(declared) != len(set(declared)):
            errors.append(f"{segment_id}: duplicate interactive flow claims")
        for flow_id in declared:
            if flow_id not in flows:
                errors.append(f"{segment_id}: unknown interactive flow {flow_id}")
                continue
            claims[flow_id].append(str(segment_id))
        if not declared:
            continue
        modes = {flows[flow_id]["mode"] for flow_id in declared if flow_id in flows}
        declared_mode = segment.get("interactive_mode")
        if len(modes) != 1 or declared_mode not in modes:
            errors.append(
                f"{segment_id}: interactive_mode {declared_mode!r} does not match claimed modes {sorted(modes)}"
            )
        if modes == {"capture"}:
            if segment.get("battle_automation") != "auto-capture":
                errors.append(f"{segment_id}: capture flow must use native-bookkeeping auto-capture")
        elif any(not flows[flow_id]["generic_autowin"] for flow_id in declared if flow_id in flows):
            if segment.get("battle_automation") != "native":
                errors.append(f"{segment_id}: native-only flow may not use generic battle automation")
        if not segment.get("screenshots") and not any(
            action.get("type") in ("screenshot", "advance_until_battle")
            for action in segment.get("semantic_actions", [])
            if isinstance(action, dict)
        ):
            errors.append(f"{segment_id}: interactive flow has no screenshot evidence point")
    return claims, errors


def report(scope: dict, claims: dict[str, list[str]], errors: list[str]) -> dict:
    flows = {flow["id"]: flow for flow in scope["flows"]}
    missing = [flow_id for flow_id, segments in claims.items() if not segments]
    covered = [flow_id for flow_id, segments in claims.items() if segments]
    by_mode = {}
    for mode in scope["allowed_modes"]:
        ids = [flow_id for flow_id, flow in flows.items() if flow["mode"] == mode]
        by_mode[mode] = {
            "total": len(ids),
            "covered": sum(bool(claims[flow_id]) for flow_id in ids),
            "missing": [flow_id for flow_id in ids if not claims[flow_id]],
        }
    return {
        "schema_version": 1,
        "flow_count": len(flows),
        "covered_count": len(covered),
        "missing_count": len(missing),
        "covered": {flow_id: claims[flow_id] for flow_id in covered},
        "missing": missing,
        "by_mode": by_mode,
        "errors": errors,
        "complete": not missing and not errors,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--scope", type=Path, default=DEFAULT_SCOPE)
    parser.add_argument("--manifest", type=Path, default=DEFAULT_MANIFEST)
    parser.add_argument("--json-out", type=Path)
    parser.add_argument("--require-complete", action="store_true")
    args = parser.parse_args()

    scope = load(args.scope)
    flows = validate_scope(scope)
    claims, errors = manifest_claims(load(args.manifest), flows)
    result = report(scope, claims, errors)
    if args.json_out is not None:
        args.json_out.parent.mkdir(parents=True, exist_ok=True)
        args.json_out.write_text(json.dumps(result, indent=2, sort_keys=True) + "\n")
    if errors:
        raise ScopeError("invalid interactive coverage claims:\n  " + "\n  ".join(errors))
    print(
        "CAMPAIGN INTERACTIVE SCOPE: "
        f"{result['covered_count']}/{result['flow_count']} declared; "
        f"{result['missing_count']} gaps"
    )
    for mode, row in result["by_mode"].items():
        print(f"  {mode}: {row['covered']}/{row['total']}")
    if args.require_complete and result["missing"]:
        raise ScopeError("interactive pipeline is incomplete:\n  " + "\n  ".join(result["missing"]))
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ScopeError, json.JSONDecodeError) as error:
        print(f"campaign interactive scope: FAIL: {error}", file=sys.stderr)
        raise SystemExit(1)
