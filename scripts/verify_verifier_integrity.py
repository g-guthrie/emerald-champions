#!/usr/bin/env python3
"""Meta-check that engine verification cannot silently disappear or be skipped."""

from __future__ import annotations

import ast
import os
from pathlib import Path
import subprocess
import sys
from types import ModuleType


ROOT = Path(__file__).resolve().parents[1]
SCRIPTS = ROOT / "scripts"


def fail(message: str) -> None:
    raise AssertionError(message)


def check_no_bare_asserts() -> int:
    count = 0
    for path in sorted(SCRIPTS.glob("verify_*.py")):
        tree = ast.parse(path.read_text(), filename=str(path))
        for node in ast.walk(tree):
            if isinstance(node, ast.Assert):
                fail(f"{path.relative_to(ROOT)}:{node.lineno}: bare assert disappears under python -O")
        count += 1
    return count


def check_optimized_gen9_gate() -> None:
    env = os.environ.copy()
    env["PYTHONOPTIMIZE"] = "1"
    result = subprocess.run(
        [sys.executable, str(SCRIPTS / "verify_verdant_gen9_battle_mechanics.py")],
        cwd=ROOT,
        env=env,
        capture_output=True,
        text=True,
        check=False,
    )
    if result.returncode != 0 or "PASS" not in result.stdout:
        fail(
            "Gen 9 verifier does not execute successfully under PYTHONOPTIMIZE=1:\n"
            + result.stdout
            + result.stderr
        )


def check_ci_entrypoints() -> None:
    workflow = (ROOT / ".github/workflows/build.yml").read_text()
    makefile = (ROOT / "Makefile").read_text()
    if "      - main" not in workflow:
        fail("CI push branches do not include main")
    if "make verify-static" not in workflow:
        fail("CI does not invoke the canonical static verifier")
    if "python3 scripts/verify_verdant.py" not in workflow:
        fail("CI does not invoke the campaign/generated-data verifier")
    if 'make -j"$(nproc)"' not in workflow:
        fail("CI does not use the actual nproc worker count")
    if workflow.count("scripts/check_rom_layout.py") != 2:
        fail("CI does not validate both linked ROM memory layouts")
    for target in ("verify:", "verify-static:", "verify-campaign:", "verify-build:"):
        if target not in makefile:
            fail(f"Makefile is missing {target}")
    if makefile.count("scripts/check_rom_layout.py") != 2:
        fail("verify-build does not validate both linked ROM memory layouts")


def check_manifest_rejects_an_omission(verify_engine_module: ModuleType) -> None:
    original = verify_engine_module.ENGINE_VERIFIERS
    verify_engine_module.ENGINE_VERIFIERS = original[:-1]
    try:
        try:
            verify_engine_module.validate_manifest()
        except SystemExit:
            return
        fail("verifier manifest accepted an intentionally omitted verifier")
    finally:
        verify_engine_module.ENGINE_VERIFIERS = original


def main() -> None:
    # Importing does not execute the suite; it validates the same manifest the
    # runner uses and makes unregistered verify_*.py files a hard failure.
    sys.path.insert(0, str(SCRIPTS))
    import verify_engine

    verify_engine.validate_manifest()
    check_manifest_rejects_an_omission(verify_engine)
    verifier_count = check_no_bare_asserts()
    check_optimized_gen9_gate()
    check_ci_entrypoints()
    print(f"Verifier integrity: PASS ({verifier_count} verifier scripts classified)")


if __name__ == "__main__":
    main()
