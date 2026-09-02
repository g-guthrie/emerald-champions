#!/usr/bin/env python3
"""Static contracts for critical inherited Expansion bug mitigations."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def require(path: str, needle: str, reason: str) -> None:
    text = (ROOT / path).read_text()
    if needle not in text:
        raise SystemExit(f"FAIL: {reason}: {path} lacks {needle!r}")


def main() -> None:
    require(
        "src/trainer_see.c",
        "if (task->tFuncId < funcCount)",
        "buried-trainer direct interaction must bounds-check its local dispatch table",
    )
    require(
        "src/trainer_see.c",
        "if (task->tFuncId >= funcCount",
        "buried-trainer completion must consume the main-table successor state",
    )
    require(
        "src/pokemon.c",
        "ctx.learnedMove != formChanges[i].param1",
        "FORM_CHANGE_MOVE must compare the changed move, not the action argument",
    )
    # Move-driven form changes, recorded-battle lockout, and the PC held-item
    # text are proven at runtime by test/upstream_critical_fixes.c; only the
    # buried-trainer and FORM_CHANGE_MOVE contracts lack a runtime test.
    print("PASS: critical upstream crash and form-change contracts without runtime coverage")


if __name__ == "__main__":
    main()
