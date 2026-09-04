#!/usr/bin/env python3
"""Prove campaign automation cannot defeat a capture-gated battle as WON."""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import verify_emerald_champions_campaign_battle_policy as battle_policy

ROOT = Path(__file__).resolve().parents[1]
LABEL = re.compile(r"(?m)^([A-Za-z_]\w*)::?\s*$")
BATTLE_START = re.compile(
    r"(?m)^\s*(?:special\s+)?(BattleSetup_Start[A-Za-z0-9_]*Battle|Start[A-Za-z0-9_]*Battle|dowildbattle)\b"
)


def blocks(path: Path) -> list[tuple[str, int, str]]:
    text = path.read_text(errors="ignore")
    markers = list(LABEL.finditer(text))
    return [
        (
            marker.group(1),
            text.count("\n", 0, marker.start()) + 1,
            text[marker.end() : markers[index + 1].start() if index + 1 < len(markers) else len(text)],
        )
        for index, marker in enumerate(markers)
    ]


def function_body(source: str, name: str) -> str:
    match = re.search(rf"(?m)^void\s+{re.escape(name)}\s*\([^)]*\)\s*\{{", source)
    if match is None:
        return ""
    depth = 1
    cursor = match.end()
    while cursor < len(source) and depth:
        if source[cursor] == "{":
            depth += 1
        elif source[cursor] == "}":
            depth -= 1
        cursor += 1
    return source[match.end() : cursor - 1]


def audit() -> dict[str, object]:
    setup_source = (ROOT / "src/battle_setup.c").read_text()
    automation = (ROOT / "src/battle_main.c").read_text()
    paths = sorted((ROOT / "data/maps").glob("*/scripts.inc"))
    paths += sorted((ROOT / "data/scripts").glob("*.inc"))
    rows: list[dict[str, object]] = []
    callback_consumers: list[dict[str, object]] = []
    failures: list[str] = []
    parsed = {path: blocks(path) for path in paths if "_Frlg" not in str(path)}

    for path, source_blocks in parsed.items():
        for label, line, body in source_blocks:
            if "GetBattleOutcome" not in body or "B_OUTCOME_CAUGHT" not in body:
                continue
            starters = BATTLE_START.findall(body)
            relative = path.relative_to(ROOT).as_posix()
            if not starters:
                callback_consumers.append({"path": relative, "line": line, "label": label})
                continue
            won_branch = "B_OUTCOME_WON" in body
            for starter in starters:
                if starter == "dowildbattle":
                    classification = "ordinary_static_won_safe" if won_branch else "unsafe_ordinary_capture_gate"
                    legendary = False
                    if not won_branch:
                        failures.append(f"{relative}:{line}: {label} capture-gates dowildbattle without a WON branch")
                else:
                    starter_body = function_body(setup_source, starter)
                    legendary = "gBattleTypeFlags = BATTLE_TYPE_LEGENDARY" in starter_body
                    classification = "legendary_auto_capture" if legendary else "unsafe_nonlegendary_capture_gate"
                    if not legendary:
                        failures.append(
                            f"{relative}:{line}: {label} uses {starter}, which does not set BATTLE_TYPE_LEGENDARY"
                        )
                rows.append(
                    {
                        "path": relative,
                        "line": line,
                        "label": label,
                        "starter": starter,
                        "has_won_branch": won_branch,
                        "sets_legendary_battle_type": legendary,
                        "pipeline_classification": classification,
                        "consumer_kind": "direct_result_branch",
                    }
                )

    # ON_RESUME cleanup hooks often consume CAUGHT separately from the script
    # that started the battle. Prove every battle protected by OBJ_DELETE in
    # the same map is either Legendary (capture) or explicitly handles WON.
    callback_paths = {
        ROOT / row["path"] for row in callback_consumers
    }
    existing = {(row["path"], row["label"], row["starter"]) for row in rows}
    for path in sorted(callback_paths):
        relative = path.relative_to(ROOT).as_posix()
        for label, line, body in parsed[path]:
            if "setflag FLAG_SYS_CTRL_OBJ_DELETE" not in body:
                continue
            for starter in BATTLE_START.findall(body):
                key = (relative, label, starter)
                if key in existing:
                    continue
                won_branch = "B_OUTCOME_WON" in body
                if starter == "dowildbattle":
                    legendary = False
                    classification = "ordinary_static_won_safe" if won_branch else "unsafe_ordinary_capture_gate"
                    if not won_branch:
                        failures.append(
                            f"{relative}:{line}: {label} protects an ordinary static battle but has no WON branch"
                        )
                else:
                    starter_body = function_body(setup_source, starter)
                    legendary = "gBattleTypeFlags = BATTLE_TYPE_LEGENDARY" in starter_body
                    classification = "legendary_auto_capture" if legendary else "unsafe_nonlegendary_capture_gate"
                    if not legendary:
                        failures.append(
                            f"{relative}:{line}: {label} uses {starter}, which does not set BATTLE_TYPE_LEGENDARY"
                        )
                rows.append(
                    {
                        "path": relative,
                        "line": line,
                        "label": label,
                        "starter": starter,
                        "has_won_branch": won_branch,
                        "sets_legendary_battle_type": legendary,
                        "pipeline_classification": classification,
                        "consumer_kind": "resume_cleanup_producer",
                    }
                )
                existing.add(key)

    required_resolver_tokens = (
        "EmeraldChampionsHeadlessGetBattleResolution()",
        "headlessResolution != EC_HEADLESS_BATTLE_NATIVE",
        "headlessResolution == EC_HEADLESS_BATTLE_CAPTURE",
        "BattleDebug_CaptureBattle();",
        "else\n            BattleDebug_WonBattle();",
    )
    for token in required_resolver_tokens:
        if token not in automation:
            failures.append(f"src/battle_main.c: campaign resolver lacks {token!r}")
    policy_result = battle_policy.audit()
    failures.extend(f"battle policy: {failure}" for failure in policy_result["failures"])

    return {
        "schema_version": 1,
        "battle_result_consumers": rows,
        "resume_cleanup_consumers": callback_consumers,
        "summary": {
            "battle_result_consumers": len(rows),
            "legendary_auto_capture": sum(row["pipeline_classification"] == "legendary_auto_capture" for row in rows),
            "ordinary_static_won_safe": sum(row["pipeline_classification"] == "ordinary_static_won_safe" for row in rows),
            "resume_cleanup_consumers": len(callback_consumers),
            "failures": len(failures),
        },
        "failures": failures,
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--json", action="store_true", help="emit the complete machine-readable audit")
    args = parser.parse_args()
    result = audit()
    if args.json:
        print(json.dumps(result, indent=2, sort_keys=True))
    if result["failures"]:
        if not args.json:
            print("\n".join(result["failures"]))
        return 1
    if not args.json:
        summary = result["summary"]
        print(
            "PASS: campaign capture audit covers "
            f"{summary['legendary_auto_capture']} legendary capture paths, "
            f"{summary['ordinary_static_won_safe']} ordinary WON-safe paths, and "
            f"{summary['resume_cleanup_consumers']} resume cleanup consumers"
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
