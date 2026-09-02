#!/usr/bin/env python3
"""Build the test ELF once and prove the curated Emerald Champions runtime gates.

This is intentionally a focused release suite, not a claim that every upstream
pokeemerald-expansion test passes.  Each entry records the minimum live test
coverage and the maximum accepted debt.  Adding passing coverage is harmless;
losing coverage or adding a known failure/TODO is a release failure.
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
    r"PASSED|TOTAL):\s+(\d+)",
    re.MULTILINE,
)
DEBT_RESULT = re.compile(r"^\[\d+\] (.*): (KNOWN_FAILING|TO_DO)$", re.MULTILINE)
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
    minimum_total: int
    maximum_known_failing: int = 0
    maximum_todo: int = 0
    allowed_known_failing: tuple[str, ...] = ()
    allowed_todo: tuple[str, ...] = ()
    timeout_seconds: int = 180


RUNTIME_GATES = (
    RuntimeGate("*Champions", 99),
    RuntimeGate("Blitz Boxer", 1),
    RuntimeGate("*preparation", 3),
    RuntimeGate("*Item descriptions fit on Bag and Shop Screen", 1),
    RuntimeGate("*Eggs safely inherit", 1),
    RuntimeGate("test/upstream_critical_fixes.c", 4),
    RuntimeGate("Commander", 42),
    RuntimeGate("test/battle/ability/forecast.c", 18),
    RuntimeGate("test/battle/ability/flower_gift.c", 12),
    RuntimeGate("*returns its base Form upon battle end after Mega Evolving", 2),
    RuntimeGate("*Simultaneous manual switches", 4),
    RuntimeGate("*Switch-in abilities trigger in Speed Order after post-KO switch", 5),
    RuntimeGate("*Spread Moves: Earthquake fails", 2),
    RuntimeGate(
        "AI_FLAG_SMART_MON_CHOICES: Move data does not spill over between switch-in candidates",
        1,
    ),
    RuntimeGate(
        "AI_FLAG_SMART_MON_CHOICES: Switchin move data is reset before recalculation",
        1,
    ),
    RuntimeGate(
        "Imposter uses a copied move slot against its selected opponent, not itself",
        1,
    ),
    RuntimeGate(
        "Imposter AI targets a foe with copied Spore instead of itself",
        1,
    ),
    RuntimeGate(
        "Sleep Clause: Sleep clause is deactivated when a sleeping mon is sent out and transforms into a mon with Insomnia / Vital spirit",
        1,
    ),
    RuntimeGate(
        "Billy's Imposter lead targets a vulnerable foe after copying its moves",
        1,
    ),
    RuntimeGate("test/save.c", 4),
    RuntimeGate(
        "test/battle/ai/ai_doubles.c",
        67,
    ),
)


def fail(message: str) -> None:
    raise SystemExit(message)


def run(
    command: list[str],
    *,
    timeout: int | None = None,
    cwd: Path = ROOT,
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
    if result.returncode != 0:
        fail(f"runtime gate exited {result.returncode}: {' '.join(command)}")
    return result.stdout, elapsed


def parse_summary(output: str) -> dict[str, int]:
    clean = ANSI.sub("", output)
    return {name: int(value) for name, value in SUMMARY.findall(clean)}


def parse_debt_identities(output: str) -> tuple[set[str], set[str]]:
    clean = ANSI.sub("", output)
    known: set[str] = set()
    todo: set[str] = set()
    for name, result in DEBT_RESULT.findall(clean):
        (known if result == "KNOWN_FAILING" else todo).add(name)
    return known, todo


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
    )

    summary = parse_summary(output)
    known_identities, todo_identities = parse_debt_identities(output)
    total = summary.get("TOTAL", 0)
    known = summary.get("KNOWN_FAILING", 0)
    todo = summary.get("TO_DO", 0)
    failures = summary.get("FAILED", 0)
    assumptions = summary.get("ASSUMPTIONS_FAILED", 0)
    newly_passing = summary.get("KNOWN_FAILING_PASSING", 0)
    expected_passing = summary.get("EXPECTED_FAIL_PASSING", 0)

    if total < gate.minimum_total:
        fail(
            f"{gate.filter!r} selected {total} tests; expected at least "
            f"{gate.minimum_total}"
        )
    if failures or assumptions or newly_passing or expected_passing:
        fail(f"{gate.filter!r} has an unexpected runtime result: {summary}")
    if known > gate.maximum_known_failing:
        fail(
            f"{gate.filter!r} has {known} known failures; maximum accepted is "
            f"{gate.maximum_known_failing}"
        )
    if todo > gate.maximum_todo:
        fail(
            f"{gate.filter!r} has {todo} TODO tests; maximum accepted is "
            f"{gate.maximum_todo}"
        )
    unexpected_known = known_identities.difference(gate.allowed_known_failing)
    unexpected_todo = todo_identities.difference(gate.allowed_todo)
    if unexpected_known or unexpected_todo:
        fail(
            f"{gate.filter!r} replaced an accepted debt item with a new one: "
            f"known={sorted(unexpected_known)} todo={sorted(unexpected_todo)}"
        )
    if len(known_identities) != known or len(todo_identities) != todo:
        fail(
            f"{gate.filter!r} summary debt counts do not match named results: "
            f"summary={summary} known={sorted(known_identities)} "
            f"todo={sorted(todo_identities)}"
        )

    print(
        f"PASS: {gate.filter!r}: total={total} known={known} todo={todo} "
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
        # Force one exact relink when the derived source allowlist changes while
        # retaining all expensive compiled objects and generated assets.
        test_elf.unlink(missing_ok=True)
        test_elf.with_name(test_elf.stem + "-headless.elf").unlink(missing_ok=True)
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
        "\nEMERALD CHAMPIONS CURATED RUNTIME GATES: PASS\n"
        f"build_seconds={build_elapsed:.2f} filter_seconds={test_elapsed:.2f} "
        f"wall_seconds={elapsed:.2f}",
        flush=True,
    )


if __name__ == "__main__":
    main()
