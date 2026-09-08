#!/usr/bin/env python3
"""Build the test ELF once and prove the curated Emerald Champions runtime gates.

This is intentionally a focused release suite, not a claim that every upstream
pokeemerald-expansion test passes.  Each entry selects current behavior and records explicitly accepted debt.
Every filter must execute named results; historical test-count quotas are
not game requirements. Result identities, counters and exit status still agree.
"""

from __future__ import annotations

import argparse
import hashlib
import os
import platform
import re
import shutil
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
ANSI = re.compile(r"\x1b\[[0-9;]*m")
SUMMARY = re.compile(
    r"^- (?:Tests )?(FAILED|EXPECTED_FAIL_PASSING|KNOWN_FAILING|"
    r"ASSUMPTIONS_FAILED|TO_DO|KNOWN_FAILING_PASSING|EXPECT_FAILING|"
    r"PASSED|TOTAL):[ \t]+(\d+)(?:[ \t]+(.*))?$",
)
SUMMARY_ANNOTATIONS = {
    "FAILED": "Add TESTS='X' to run tests with the defined prefix.",
    "KNOWN_FAILING_PASSING": "Please remove KNOWN_FAILING if these tests intentionally PASS",
}
RESULT = re.compile(r"^\[\d+\] (.+): ([A-Z_]+)$")
# The ROM emits one result per test, not per parameter/trial. Successful
# tests reset their printed identity to the declaration's base name.
TRIAL_SUFFIX = re.compile(r" \(\d+/\?\)$")
EXECUTION_SUFFIX = re.compile(r"(?: \d+/\d+)?(?: \(\d+/(?:\d+|\?)\))?$")
RESULT_COUNTER = {
    "PASS": "PASSED",
    "EXPECTED_FAIL": "EXPECT_FAILING",
    "KNOWN_FAILING": "KNOWN_FAILING",
    "KNOWN_FAILING_PASS": "KNOWN_FAILING_PASSING",
    "EXPECTED_FAIL_PASS": "EXPECTED_FAIL_PASSING",
    "TO_DO": "TO_DO",
    "ASSUMPTION_FAIL": "ASSUMPTIONS_FAILED",
    "FAIL": "FAILED",
    "UNEXPECTED_FAIL_LINE": "FAILED",
    "INVALID": "FAILED",
    "ERROR": "FAILED",
    "TIMEOUT": "FAILED",
    "FLAKY": "FAILED",
    "UNKNOWN": "FAILED",
}
TEST_DECLARATION = re.compile(
    r'(?m)^\s*(?:TEST|[A-Z_]+BATTLE_TEST)\s*\(\s*"((?:[^"\\]|\\.)*)"'
)
TEST_SUPPORT_SOURCES = {
    "test/test_runner.c",
    "test/test_runner_args.c",
    "test/test_runner_battle.c",
}


@dataclass(frozen=True)
class RuntimeGate:
    filter: str
    maximum_known_failing: int = 0
    maximum_todo: int = 0
    allowed_known_failing: tuple[str, ...] = ()
    allowed_todo: tuple[str, ...] = ()
    # Exact result identities (with their "k/n" parametrize suffix) that are
    # accepted as FAIL / ASSUMPTION_FAIL today. This is tracked debt, not
    # tolerance: a failure not named here still breaks the gate, and a named
    # one that starts passing breaks it too so the list gets trimmed.
    allowed_failing: tuple[str, ...] = ()
    # Existing intermittent debt is scoped to this filter and may pass or
    # fail. Only a named '(k/?)' trial index is normalized; other parameters
    # remain exact. PASSES_RANDOMLY alone never grants an exemption.
    allowed_intermittent_failing: tuple[str, ...] = ()
    timeout_seconds: int = 180


RUNTIME_GATES = (
    RuntimeGate("test/legendary_sign_pipeline.c"),
    RuntimeGate("test/mega_stone_rewards.c"),
    RuntimeGate("test/emerald_champions_daycare.c"),
    RuntimeGate("test/battle/ai/party_knowledge.c"),
    RuntimeGate("*Champions"),
    RuntimeGate("Blitz Boxer"),
    RuntimeGate("*preparation"),
    RuntimeGate("*Item descriptions fit on Bag and Shop Screen"),
    RuntimeGate("*Eggs safely inherit"),
    RuntimeGate("test/upstream_critical_fixes.c"),
    RuntimeGate("Commander"),
    RuntimeGate("test/battle/ability/forecast.c"),
    RuntimeGate("test/battle/ability/flower_gift.c"),
    RuntimeGate("*returns its base Form upon battle end after Mega Evolving"),
    RuntimeGate("*Simultaneous manual switches"),
    RuntimeGate("*Switch-in abilities trigger in Speed Order after post-KO switch"),
    RuntimeGate("*Spread Moves: Earthquake fails"),
    RuntimeGate(
        "AI_FLAG_SMART_MON_CHOICES: Move data does not spill over between switch-in candidates",
    ),
    RuntimeGate(
        "AI_FLAG_SMART_MON_CHOICES: Switchin move data is reset before recalculation",
    ),
    RuntimeGate(
        "Imposter uses a copied move slot against its selected opponent, not itself",
    ),
    RuntimeGate(
        "Imposter AI targets a foe with copied Spore instead of itself",
    ),
    RuntimeGate(
        "Sleep Clause: Sleep clause is deactivated when a sleeping mon is sent out and transforms into a mon with Insomnia / Vital spirit",
    ),
    RuntimeGate(
        "Billy's Imposter lead targets a vulnerable foe after copying its moves",
    ),
    RuntimeGate("test/save.c"),
    RuntimeGate(
        "test/battle/ai/ai_doubles.c",
    ),
    RuntimeGate(
        "test/battle/ai/ai.c",
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/ai_assume_stab.c",
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/ai_assume_status_moves.c",
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/ai_calc_best_move_score.c",
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/ai_check_viability.c",
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/ai_choice.c",
        allowed_intermittent_failing=(
            "Choiced Pokémon won't switch out if they can still affect one opposing Pokémon in doubles (reversed) 1/2 (1/?)",
        ),
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/ai_combo_attack.c",
        allowed_failing=(
            'Combo Attack: Fusion moves are only incentivised when partners are adjacent in turn order 2/2',
        ),
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/ai_double_ace.c",
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/ai_flag_attacks_partner.c",
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/ai_flag_predict_ability.c",
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/ai_flag_predict_move.c",
        allowed_intermittent_failing=(
            "AI won't use Sucker Punch if it expects a move of the same priority bracket and the opponent is faster (1/?)",
            "AI_FLAG_PREDICT_MOVE: AI will still attack you when it should",
        ),
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/ai_flag_predict_switch.c",
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/ai_flag_risky.c",
        allowed_failing=(
            'AI_FLAG_RISKY: Mid-battle switches prioritize offensive options 1/2',
        ),
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/ai_flag_sequence_switching.c",
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/ai_multi.c",
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/ai_powerful_status.c",
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/ai_pp_stall_prevention.c",
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/ai_switching.c",
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/ai_trytofaint.c",
        timeout_seconds=600,
    ),
    RuntimeGate(
        "test/battle/ai/check_bad_move.c",
        timeout_seconds=600,
    ),
    RuntimeGate("test/battle/ai/gimmick_mega.c", timeout_seconds=600),
    RuntimeGate("test/battle/ability/telepathy.c"),
    RuntimeGate("test/chansey_overworld.c"),
    RuntimeGate("test/battle/ai/values_moves_over_splash.c", timeout_seconds=600),
)


def fail(message: str) -> None:
    raise SystemExit(message)


def run(
    command: list[str],
    *,
    timeout: int | None = None,
    cwd: Path = ROOT,
    test_results: bool = False,
) -> tuple[str, float]:
    started = time.monotonic()
    try:
        result = subprocess.run(
            command,
            cwd=cwd,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            timeout=timeout,
            check=False,
        )
    except subprocess.TimeoutExpired as error:
        output = error.stdout or ""
        if isinstance(output, bytes):
            output = output.decode(errors="replace")
        print(output, end="")
        fail(f"runtime gate timed out after {timeout}s: {' '.join(command)}")
    elapsed = time.monotonic() - started
    print(result.stdout, end="")
    # Hydra uses 1 for test failures and 2 for tooling errors. Signals and
    # other process failures must never be mistaken for accepted test debt.
    # Opt in at the call site, not by trusting an executable's filename.
    if result.returncode not in ((0, 1) if test_results else (0,)):
        fail(f"runtime gate exited {result.returncode}: {' '.join(command)}")
    if test_results:
        summary, _ = parse_results(result.stdout)
        # SkipIsFail is disabled. Assumption failures alone need not exit 1;
        # TODO tests may. Ordinary failures and unexpected passes must do so.
        must_fail = any(summary.get(key, 0) for key in (
            "FAILED", "KNOWN_FAILING_PASSING", "EXPECTED_FAIL_PASSING",
        ))
        if must_fail and result.returncode != 1:
            fail("runtime result failures disagree with Hydra exit status")
        if result.returncode == 1 and not (must_fail or summary.get("TO_DO", 0)):
            fail("Hydra exited 1 without a corresponding failing result")
    return result.stdout, elapsed


def parse_results(output: str) -> tuple[dict[str, int], dict[str, str]]:
    """Reconcile every final result with Hydra's optional nonzero counters.

    Hydra does not expose its command letter for CRASH (which can mean an
    expected crash or an actual failure). Curated gates fail closed on that
    ambiguous status until a machine-readable protocol distinguishes it.
    """
    summary: dict[str, int] = {}
    results: dict[str, str] = {}
    counts = dict.fromkeys(set(RESULT_COUNTER.values()), 0)
    if not output.endswith("\n"):
        fail("runtime output is truncated (missing final newline)")
    for line in ANSI.sub("", output).splitlines():
        match = SUMMARY.fullmatch(line)
        if match:
            name, value, annotation = match.groups()
            if annotation and annotation != SUMMARY_ANNOTATIONS.get(name):
                fail(f"malformed runtime summary annotation: {line}")
            if name in summary:
                fail(f"duplicate runtime summary: {name}")
            if "TOTAL" in summary:
                fail("runtime summary continues after TOTAL")
            summary[name] = int(value)
        elif line.startswith("- "):
            fail(f"malformed or unknown runtime summary: {line}")
        elif line.startswith("["):
            match = RESULT.fullmatch(line)
            if match is None:
                fail(f"malformed runtime result: {line}")
            name, status = match.groups()
            if summary:
                fail("runtime results appear after the summary started")
            if status not in RESULT_COUNTER:
                fail(f"unsupported runtime status {status!r} for {name!r}")
            if name in results:
                fail(f"duplicate runtime result identity: {name!r}")
            if not name.strip() or name == "WAITING...":
                fail("runtime result has no test identity")
            results[name] = status
            counts[RESULT_COUNTER[status]] += 1
    if "TOTAL" not in summary:
        fail("runtime output has no TOTAL summary")
    if summary["TOTAL"] != len(results) or not results:
        fail(f"runtime TOTAL {summary['TOTAL']} disagrees with {len(results)} named results")
    for name, count in counts.items():
        if summary.get(name, 0) != count:
            fail(f"runtime {name} summary {summary.get(name, 0)} disagrees with {count} named results")
    return summary, results


def validate_gate_output(gate: RuntimeGate, output: str) -> dict[str, int]:
    summary, results = parse_results(output)
    total = summary["TOTAL"]
    invalid = {name: status for name, status in results.items() if status not in (
        "PASS", "EXPECTED_FAIL", "KNOWN_FAILING", "TO_DO", "FAIL", "ASSUMPTION_FAIL",
    )}
    if invalid:
        fail(f"{gate.filter!r} has unexpected runtime results: {invalid}")

    failing = {name for name, status in results.items() if status in ("FAIL", "ASSUMPTION_FAIL")}
    allowed = set(gate.allowed_failing)
    intermittent = {TRIAL_SUFFIX.sub(" (*/?)", name) for name in gate.allowed_intermittent_failing}
    unexpected = {name for name in failing - allowed if TRIAL_SUFFIX.sub(" (*/?)", name) not in intermittent}
    if unexpected:
        fail(f"{gate.filter!r} has new failing tests: {sorted(unexpected)}")
    if allowed - failing:
        fail(f"{gate.filter!r} debt items no longer fail or are missing; update allowed_failing: {sorted(allowed - failing)}")

    # Intermittent debt must still execute, even on a passing run. The ROM
    # strips parameter/trial suffixes from successful result identities.
    executed = {EXECUTION_SUFFIX.sub("", name) for name in results}
    missing = {name for name in gate.allowed_intermittent_failing if EXECUTION_SUFFIX.sub("", name) not in executed}
    if missing:
        fail(f"{gate.filter!r} intermittent debt tests did not execute: {sorted(missing)}")
    for status, maximum, accepted in (
        ("KNOWN_FAILING", gate.maximum_known_failing, gate.allowed_known_failing),
        ("TO_DO", gate.maximum_todo, gate.allowed_todo),
    ):
        identities = {name for name, result in results.items() if result == status}
        if len(identities) > maximum or identities - set(accepted):
            fail(f"{gate.filter!r} has unaccepted {status} debt: {sorted(identities)} (maximum {maximum})")
    return summary


def filter_matches(filter_text: str, test_name: str) -> bool:
    if filter_text.startswith("*"):
        return filter_text[1:] in test_name
    return test_name.startswith(filter_text)


def curated_test_sources() -> tuple[str, ...]:
    selected = set(TEST_SUPPORT_SOURCES)
    declarations: dict[str, tuple[str, ...]] = {}
    for path in sorted((ROOT / "test").rglob("*.c")):
        relative = str(path.relative_to(ROOT))
        declarations[relative] = tuple(TEST_DECLARATION.findall(path.read_text(errors="ignore")))

    missing: list[str] = []
    for gate in RUNTIME_GATES:
        if gate.filter.endswith(".c"):
            path = ROOT / gate.filter
            if not path.is_file():
                missing.append(gate.filter)
            else:
                selected.add(gate.filter)
            continue
        matches = {
            relative
            for relative, names in declarations.items()
            if any(filter_matches(gate.filter, name) for name in names)
        }
        if not matches:
            missing.append(gate.filter)
        selected.update(matches)
    if missing:
        fail(f"curated runtime filters have no source declarations: {missing}")
    return tuple(sorted(selected))


def resolve_tool(explicit: str | None, commands: tuple[str, ...], fallback: Path) -> str:
    if explicit:
        path = Path(explicit).expanduser().resolve()
        if not path.is_file():
            fail(f"tool does not exist: {path}")
        return str(path)
    for command in commands:
        found = shutil.which(command)
        if found:
            return found
    if fallback.is_file():
        return str(fallback)
    fail(f"missing tool: tried {', '.join(commands)} and {fallback}")


def verify_gate(
    gate: RuntimeGate,
    *,
    test_elf: Path,
    headless_elf: Path,
    patchelf: str,
    hydra: str,
    romtest: str,
    objcopy: str,
    runtime_cwd: Path,
) -> float:
    shutil.copyfile(test_elf, headless_elf)
    run(
        [
            patchelf,
            str(headless_elf),
            "gTestRunnerArgv",
            gate.filter + r"\0",
            "gTestRunnerHeadless",
            r"\x01",
            "gTestRunnerSkipIsFail",
            r"\x00",
        ]
    )
    output, elapsed = run(
        [hydra, romtest, objcopy, str(headless_elf)],
        timeout=gate.timeout_seconds,
        cwd=runtime_cwd,
        test_results=True,
    )

    summary = validate_gate_output(gate, output)
    total = summary["TOTAL"]
    known = summary.get("KNOWN_FAILING", 0)
    todo = summary.get("TO_DO", 0)
    failures = summary.get("FAILED", 0)
    assumptions = summary.get("ASSUMPTIONS_FAILED", 0)
    label = "PASS WITH DEBT" if known or todo or failures or assumptions or gate.allowed_intermittent_failing else "PASS"
    print(
        f"{label}: {gate.filter!r}: total={total} known={known} todo={todo} "
        f"failures={failures} assumptions={assumptions} "
        f"intermittent_debt={len(gate.allowed_intermittent_failing)} "
        f"elapsed={elapsed:.2f}s",
        flush=True,
    )
    return elapsed


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--jobs", type=int, default=os.cpu_count() or 1)
    parser.add_argument("--build-only", action="store_true")
    parser.add_argument("--run-only", action="store_true")
    parser.add_argument("--test-elf", type=Path, default=ROOT / "pokeemerald-test.elf")
    parser.add_argument("--patchelf")
    parser.add_argument("--hydra")
    parser.add_argument("--romtest")
    parser.add_argument("--objcopy")
    parser.add_argument(
        "--runtime-cwd",
        type=Path,
        default=ROOT,
        help="directory whose tools/patchelf/patchelf Hydra should use",
    )
    args = parser.parse_args()
    if args.jobs < 1:
        fail("--jobs must be positive")
    if args.build_only and args.run_only:
        fail("--build-only and --run-only are mutually exclusive")

    started = time.monotonic()
    build_elapsed = 0.0
    test_elf = args.test_elf.resolve()
    if not args.run_only:
        expected_build_elf = (ROOT / "pokeemerald-test.elf").resolve()
        if test_elf != expected_build_elf:
            fail(f"build mode produces {expected_build_elf}; --test-elf requested {test_elf}")
        # Make's link receipt includes the exact selected object list; its
        # separate filter export preserves reusable compiled and linked inputs.
        print("== Build shared runtime-test ELF ==", flush=True)
        test_sources = curated_test_sources()
        print(
            f"curated_test_sources={len(test_sources)} "
            f"full_test_sources={len(tuple((ROOT / 'test').rglob('*.c')))}",
            flush=True,
        )
        _, build_elapsed = run(
            [
                "make",
                f"-j{args.jobs}",
                "pokeemerald-test.elf",
                "TESTS=",
                "TEST_SOURCE_ALLOWLIST=" + " ".join(test_sources),
            ],
        )
    if not test_elf.is_file():
        fail(f"shared test ELF is missing: {test_elf}")
    test_stamp = test_elf.with_name(test_elf.stem + ".inputs.json")
    if args.run_only:
        # The ELF may have been built in a container copy of the tree; require
        # the content stamp written beside it so the suite proves this tree.
        run([sys.executable, str(ROOT / "scripts" / "stamp_release_inputs.py"), "--check", "--stamp", str(test_stamp)])
    else:
        run([sys.executable, str(ROOT / "scripts" / "stamp_release_inputs.py"), "--stamp", str(test_stamp)])
    if args.build_only:
        print(f"runtime_test_elf={test_elf} build_seconds={build_elapsed:.2f}")
        return

    darwin = platform.system() == "Darwin"
    # This repository's patchelf patches symbols inside the GBA test ELF; the
    # unrelated system utility with the same name cannot perform that job.
    patchelf = resolve_tool(
        args.patchelf,
        (),
        ROOT / "tools/patchelf/patchelf",
    )
    hydra = resolve_tool(
        args.hydra,
        ("mgba-rom-test-hydra",),
        ROOT / "tools/mgba-rom-test-hydra/mgba-rom-test-hydra",
    )
    romtest = resolve_tool(
        args.romtest,
        (("mgba-rom-test-mac",) if darwin else ("mgba-rom-test",)),
        ROOT / "tools/mgba" / ("mgba-rom-test-mac" if darwin else "mgba-rom-test"),
    )
    objcopy = resolve_tool(
        args.objcopy,
        ("arm-none-eabi-objcopy",),
        ROOT / "tools/agbcc/bin/arm-none-eabi-objcopy",
    )
    headless_elf = test_elf.with_name(test_elf.stem + "-headless.elf")
    runtime_cwd = args.runtime_cwd.resolve()
    if not (runtime_cwd / "tools/patchelf/patchelf").is_file():
        fail(f"Hydra runtime cwd lacks tools/patchelf/patchelf: {runtime_cwd}")
    base_digest = hashlib.sha256(test_elf.read_bytes()).hexdigest()

    test_elapsed = 0.0
    for gate in RUNTIME_GATES:
        print(f"\n== Runtime filter: {gate.filter} ==", flush=True)
        test_elapsed += verify_gate(
            gate,
            test_elf=test_elf,
            headless_elf=headless_elf,
            patchelf=patchelf,
            hydra=hydra,
            romtest=romtest,
            objcopy=objcopy,
            runtime_cwd=runtime_cwd,
        )

    if hashlib.sha256(test_elf.read_bytes()).hexdigest() != base_digest:
        fail("shared test ELF changed while running filters")

    elapsed = time.monotonic() - started
    print(
        "\nEMERALD CHAMPIONS CURATED RUNTIME GATES: PASS WITH IDENTITY-PINNED DEBT\n"
        "See docs/VERIFICATION.md; no unlisted runtime regression was accepted.\n"
        f"build_seconds={build_elapsed:.2f} filter_seconds={test_elapsed:.2f} "
        f"wall_seconds={elapsed:.2f}",
        flush=True,
    )


if __name__ == "__main__":
    main()
