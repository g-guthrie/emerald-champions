#!/usr/bin/env python3
"""Run every registered engine verifier and reject unclassified verifier drift."""

from __future__ import annotations

from pathlib import Path
import subprocess
import sys


ROOT = Path(__file__).resolve().parents[1]
SCRIPTS = ROOT / "scripts"

# Order is deliberate: validate the test harness first, then narrow mechanics,
# broad engine inventories, and finally presentation/save contracts.
ENGINE_VERIFIERS = (
    "verify_verifier_integrity.py",
    "verify_core_engine_integrity.py",
    "verify_allocation_safety.py",
    "verify_legacy_bugfix_canonicalization.py",
    "verify_graphics_integrity.py",
    "verify_sprite_resource_sidecar.py",
    "verify_task_exhaustion.py",
    "verify_presentation_save_deep.py",
    "verify_leveler.py",
    "verify_capture_ready_wilds.py",
    "verify_multi_battle_sets.py",
    "verify_no_z_items.py",
    "verify_no_z_engine.py",
    "verify_primal_reversion.py",
    "verify_fling_backport.py",
    "verify_magician_symbiosis.py",
    "verify_placeholder_moves.py",
    "verify_pledge_mechanics.py",
    "verify_battle_edge_semantics.py",
    "verify_upstream_mechanics_backports.py",
    "verify_battle_issue_batch_2.py",
    "verify_upstream_engine_fixes.py",
    "verify_verdant_gen9_battle_mechanics.py",
    "verify_mechanics_completeness.py",
    "verify_frontier_runtime.py",
    "verify_overworld_ui_hardening.py",
    "verify_verdant_presentation_safety.py",
    "verify_verdant_save_ui.py",
)

# These are deliberately outside the engine-only suite. Each exclusion is
# explicit so a new verify_*.py file can never be silently skipped.
EXCLUDED_VERIFIERS = {
    "verify_competitive_references.py": "trainer-design research corpus",
    "verify_verdant.py": "full campaign and moving trainer-design umbrella",
}


def validate_manifest() -> None:
    discovered = {path.name for path in SCRIPTS.glob("verify_*.py")}
    classified = set(ENGINE_VERIFIERS) | set(EXCLUDED_VERIFIERS) | {Path(__file__).name}
    unknown = sorted(discovered - classified)
    missing = sorted(classified - discovered)
    overlap = sorted(set(ENGINE_VERIFIERS) & set(EXCLUDED_VERIFIERS))

    problems = []
    if unknown:
        problems.append(f"unclassified verifier scripts: {unknown}")
    if missing:
        problems.append(f"registered verifier scripts are missing: {missing}")
    if overlap:
        problems.append(f"verifiers are both included and excluded: {overlap}")
    if len(ENGINE_VERIFIERS) != len(set(ENGINE_VERIFIERS)):
        problems.append("engine verifier manifest contains duplicates")
    if problems:
        raise SystemExit("Verifier manifest failure:\n- " + "\n- ".join(problems))


def main() -> int:
    validate_manifest()
    failures: list[str] = []

    for script_name in ENGINE_VERIFIERS:
        print(f"\n=== {script_name} ===", flush=True)
        result = subprocess.run(
            [sys.executable, str(SCRIPTS / script_name)],
            cwd=ROOT,
            check=False,
        )
        if result.returncode != 0:
            failures.append(f"{script_name} (exit {result.returncode})")

    if failures:
        print("\nEngine verification failed:", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1

    print(f"\nAll {len(ENGINE_VERIFIERS)} registered engine verifiers passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
