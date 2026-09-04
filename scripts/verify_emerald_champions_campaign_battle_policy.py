#!/usr/bin/env python3
"""Emit and validate the explicit campaign-fixture battle resolution policy."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
from pathlib import Path
import shutil
import subprocess
import tempfile


ROOT = Path(__file__).resolve().parents[1]
NATIVE = "native-flow-required"
WIN = "auto-win"
CAPTURE = "auto-capture"

CASES = (
    ("ordinary-first-party-slot", (), 1, CAPTURE, "first catch supplies the second party member before Route 102 doubles"),
    ("ordinary-established-party", (), 2, WIN, "later random encounters are traversal noise"),
    ("scripted-nonlegendary-wild", (), 2, WIN, "script receives the normal defeated outcome"),
    ("trainer-single", ("BATTLE_TYPE_TRAINER",), 2, WIN, "normal victory reward and trainer-flag callbacks must run"),
    ("trainer-double", ("BATTLE_TYPE_TRAINER", "BATTLE_TYPE_DOUBLE"), 2, WIN, "normal doubles victory cleanup must run"),
    ("birch-first-battle", ("BATTLE_TYPE_FIRST_BATTLE",), 1, WIN, "the rescue battle is defeated, never captured"),
    ("wally-catch-tutorial", ("BATTLE_TYPE_CATCH_TUTORIAL",), 1, NATIVE, "the autonomous tutorial controller must perform its scripted capture"),
    ("pokedude-tutorial", ("BATTLE_TYPE_POKEDUDE",), 1, NATIVE, "the tutorial controller owns the demonstration"),
    ("safari", ("BATTLE_TYPE_SAFARI",), 1, WIN, "campaign traversal exits the encounter without fabricating a Safari capture"),
    ("ghost", ("BATTLE_TYPE_GHOST",), 2, WIN, "ghost and Marowak callbacks consume the defeated outcome"),
    ("roamer", ("BATTLE_TYPE_ROAMER",), 2, CAPTURE, "a traversal win would permanently deactivate the uncaught roamer"),
    ("legendary", ("BATTLE_TYPE_LEGENDARY",), 2, CAPTURE, "capture-gated quest branches require CAUGHT"),
    ("raid", ("BATTLE_TYPE_RAID",), 2, WIN, "no live campaign capture gate uses this engine facility"),
    ("frontier-tower", ("BATTLE_TYPE_TRAINER", "BATTLE_TYPE_BATTLE_TOWER"), 2, WIN, "facility streak callback requires WON"),
    ("frontier-dome", ("BATTLE_TYPE_TRAINER", "BATTLE_TYPE_DOME"), 2, WIN, "facility bracket callback requires WON"),
    ("frontier-palace", ("BATTLE_TYPE_TRAINER", "BATTLE_TYPE_PALACE"), 2, WIN, "facility callback requires WON"),
    ("frontier-arena", ("BATTLE_TYPE_TRAINER", "BATTLE_TYPE_ARENA"), 2, WIN, "facility judgment callback requires WON"),
    ("frontier-factory", ("BATTLE_TYPE_TRAINER", "BATTLE_TYPE_FACTORY"), 2, WIN, "rental-party callback requires WON"),
    ("frontier-pike-wild", ("BATTLE_TYPE_PIKE",), 2, WIN, "Pike room callback requires a normal completed battle"),
    ("frontier-pyramid", ("BATTLE_TYPE_TRAINER", "BATTLE_TYPE_PYRAMID"), 2, WIN, "Pyramid floor callback requires WON"),
    ("trainer-hill", ("BATTLE_TYPE_TRAINER", "BATTLE_TYPE_TRAINER_HILL"), 2, WIN, "Hill timer and result callback require WON"),
    ("ereader-trainer", ("BATTLE_TYPE_TRAINER", "BATTLE_TYPE_EREADER_TRAINER"), 2, WIN, "special trainer callback requires WON"),
    ("secret-base", ("BATTLE_TYPE_TRAINER", "BATTLE_TYPE_SECRET_BASE"), 2, WIN, "secret-base callback requires WON"),
    ("partner-multi-trainer", ("BATTLE_TYPE_TRAINER", "BATTLE_TYPE_DOUBLE", "BATTLE_TYPE_MULTI", "BATTLE_TYPE_INGAME_PARTNER"), 1, WIN, "partner restoration must use normal victory cleanup"),
    ("partner-multi-wild", ("BATTLE_TYPE_DOUBLE", "BATTLE_TYPE_MULTI", "BATTLE_TYPE_INGAME_PARTNER"), 1, WIN, "the partner battle is not the opening ordinary catch"),
    ("link", ("BATTLE_TYPE_LINK",), 2, NATIVE, "remote synchronization owns the outcome"),
    ("link-multi", ("BATTLE_TYPE_LINK", "BATTLE_TYPE_MULTI", "BATTLE_TYPE_TOWER_LINK_MULTI"), 2, NATIVE, "remote multi synchronization owns the outcome"),
    ("recorded", ("BATTLE_TYPE_RECORDED",), 2, NATIVE, "playback must consume recorded actions and outcome"),
    ("recorded-link", ("BATTLE_TYPE_RECORDED_LINK",), 2, NATIVE, "recorded link playback owns the outcome"),
)


def flag_values() -> dict[str, int]:
    text = (ROOT / "include/constants/battle.h").read_text()
    return {
        name: 1 << int(bit)
        for name, bit in re.findall(r"#define\s+(BATTLE_TYPE_[A-Z0-9_]+)\s+\(1\s*<<\s*(\d+)\)", text)
    }


def c_block(source: str, signature: str) -> str:
    start = source.index(signature + "\n{")
    end = source.index("\n}", start) + 2
    return source[start:end] + (";" if source[end:end + 1] == ";" else "") + "\n"


def classify_cases(cases: list[tuple[int, int]]) -> tuple[list[str], str]:
    """Execute the production C decision function with explicit host inputs.

    Only party-count retrieval and the two input globals are supplied by the
    host fixture. This proves the classifier's decisions, not battle callbacks
    or traversal. A compiler failure is a failed audit, never a Python fallback.
    """
    compiler = shutil.which("cc")
    if compiler is None:
        raise RuntimeError("host C compiler required to verify the actual campaign battle policy")
    source = (ROOT / "src/emerald_champions_headless.c").read_text()
    header = (ROOT / "include/emerald_champions_headless.h").read_text()
    function = c_block(source, "enum EmeraldChampionsHeadlessBattleResolution EmeraldChampionsHeadlessGetBattleResolution(void)")
    declarations = c_block(header, "enum EmeraldChampionsHeadlessScenario")
    declarations += c_block(header, "enum EmeraldChampionsHeadlessBattleResolution")
    inputs = ",\n".join(f"{{{flags}u, {count}u}}" for flags, count in cases)
    program = '''#include <stdint.h>
#include <stdio.h>
#include "constants/pokemon.h"
#include "constants/battle.h"
''' + declarations + '''
static uint32_t gBattleTypeFlags;
static enum EmeraldChampionsHeadlessScenario gEcHeadlessFixtureActiveScenario;
static unsigned partyCount;
static unsigned CalculatePlayerPartyCount(void) { return partyCount; }
''' + function + '''
int main(void) {
    const unsigned inputs[][2] = {
''' + inputs + '''
    };
    gEcHeadlessFixtureActiveScenario = EC_HEADLESS_SCENARIO_CAMPAIGN_AUTOWIN;
    for (unsigned i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++) {
        gBattleTypeFlags = inputs[i][0];
        partyCount = inputs[i][1];
        switch (EmeraldChampionsHeadlessGetBattleResolution()) {
        case EC_HEADLESS_BATTLE_NATIVE: puts("native-flow-required"); break;
        case EC_HEADLESS_BATTLE_WIN: puts("auto-win"); break;
        case EC_HEADLESS_BATTLE_CAPTURE: puts("auto-capture"); break;
        default: return 2;
        }
    }
    return 0;
}
'''
    with tempfile.TemporaryDirectory(prefix="ec-campaign-policy-") as directory:
        path = Path(directory)
        fixture = path / "policy.c"
        executable = path / "policy"
        fixture.write_text(program)
        compiled = subprocess.run(
            [compiler, "-std=c11", "-Wall", "-Wextra", "-Werror", "-I", str(ROOT / "include"), str(fixture), "-o", str(executable)],
            capture_output=True, text=True, timeout=30, check=False,
        )
        if compiled.returncode:
            raise RuntimeError("actual C campaign policy failed to compile:\n" + compiled.stdout + compiled.stderr)
        result = subprocess.run([str(executable)], capture_output=True, text=True, timeout=10, check=False)
        decisions = result.stdout.splitlines()
        if result.returncode or len(decisions) != len(cases) or any(value not in (NATIVE, WIN, CAPTURE) for value in decisions):
            raise RuntimeError(f"actual C campaign policy produced invalid results (exit {result.returncode}): {result.stdout}{result.stderr}")
    return decisions, hashlib.sha256(function.encode()).hexdigest()


def classify(flags: int, party_count: int, values: dict[str, int]) -> str:
    # Retain the callable interface; decisions now use the actual C and headers.
    return classify_cases([(flags, party_count)])[0][0]


def audit() -> dict[str, object]:
    values = flag_values()
    rows = []
    failures = []
    missing = sorted({flag for _, names, *_ in CASES for flag in names if flag not in values})
    if missing:
        return {"schema_version": 1, "flows": rows, "failures": [f"unknown battle flags: {missing}"]}
    try:
        decisions, source_hash = classify_cases([(sum(values[flag] for flag in names), count) for _, names, count, _, _ in CASES])
    except (OSError, RuntimeError, ValueError, subprocess.TimeoutExpired) as error:
        return {"schema_version": 1, "flows": rows, "failures": [str(error)]}
    for (name, names, party_count, expected, reason), actual in zip(CASES, decisions):
        if actual != expected:
            failures.append(f"{name}: expected {expected}, policy resolves {actual}")
        rows.append({
            "flow": name,
            "flags": list(names),
            "party_count": party_count,
            "resolution": actual,
            "reason": reason,
        })

    return {
        "schema_version": 1, "flows": rows, "failures": failures,
        "evidence": {"mode": "actual-production-C-host-execution", "classifier_sha256": source_hash,
                     "scope": "campaign scenario classifier; supplied party count; no battle callbacks"},
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--json", action="store_true")
    args = parser.parse_args()
    result = audit()
    if args.json:
        print(json.dumps(result, indent=2, sort_keys=True))
    elif result["failures"]:
        print("\n".join(result["failures"]))
    else:
        counts = {kind: sum(row["resolution"] == kind for row in result["flows"]) for kind in (WIN, CAPTURE, NATIVE)}
        print(f"PASS: explicit campaign battle policy covers {len(result['flows'])} flows {counts}")
    return bool(result["failures"])


if __name__ == "__main__":
    raise SystemExit(main())
