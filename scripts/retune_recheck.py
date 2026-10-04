#!/usr/bin/env python3
"""Which finished Hard benchmarks does a player-pool change invalidate?

A benchmark proves only that its recorded party won at its delta. After a
wild-slot, item, tutor or evolution change (and a fresh manifest), re-check
every receipt's party against the current progression: a party that still
passes keeps its result; one that fails used something that moved and must be
rerun. Pool additions never fail a party; earlier results then stand as
conservative lower bounds.

  python3 scripts/retune_recheck.py              # every receipt under work/retune/NN-*/
  python3 scripts/retune_recheck.py --cap 14     # one cap window
"""
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))

RETUNE = ROOT / "work/retune"


def receipts(retune: Path = RETUNE, cap: int | None = None) -> list[tuple[Path, dict]]:
    found = []
    for path in sorted(retune.glob("[0-9][0-9]*-TRAINER_*/receipt.json")):
        try:
            receipt = json.loads(path.read_text())
        except ValueError:
            continue
        if cap is None or receipt.get("cap") == cap:
            found.append((path.parent, receipt))
    return found


def party_path(directory: Path, receipt: dict) -> Path:
    named = receipt.get("party")
    if named:
        candidate = Path(named)
        return candidate if candidate.is_absolute() else ROOT / candidate
    return directory / "party.json"


def make_checker(progression: Path):
    """Return check(trainer, cap, party) -> list of problems, sharing one pool builder."""
    from reference_pool import Builder, Encounters, encounter_pool
    from tuning_pool_check import check, narrow_to_progression, species_aliases
    builder = Builder()
    encounters = Encounters(builder)
    aliases = species_aliases()

    def run(trainer: str, cap: int, party: dict) -> list[str]:
        pool = encounter_pool(trainer, str(cap), builder=builder, encounters=encounters)
        pool, _ = narrow_to_progression(pool, trainer, progression)
        return check(party, pool, aliases)
    return run


def recheck(items: list[tuple[Path, dict]], checker) -> list[dict]:
    rows = []
    for directory, receipt in items:
        trainer, cap = receipt.get("trainer"), receipt.get("cap")
        path = party_path(directory, receipt)
        if not path.exists():
            rows.append(dict(battle=directory.name, status="no party", problems=[str(path)]))
            continue
        try:
            problems = checker(trainer, cap, json.loads(path.read_text()))
        except ValueError as exc:
            problems = [f"cannot check: {exc}"]
        rows.append(dict(battle=directory.name, status="rerun" if problems else "stands", problems=problems))
    return rows


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--cap", type=int)
    parser.add_argument("--progression", type=Path, default=RETUNE / "progression.pickle")
    args = parser.parse_args(argv)
    items = receipts(cap=args.cap)
    if not items:
        print("no receipts to check")
        return 0
    rows = recheck(items, make_checker(args.progression))
    for row in rows:
        print(f"{row['status']:<9} {row['battle']}")
        for problem in row["problems"]:
            print(f"          - {problem}")
    rerun = sum(r["status"] != "stands" for r in rows)
    print(f"{len(rows) - rerun} stand, {rerun} need a rerun")
    return 1 if rerun else 0


if __name__ == "__main__":
    raise SystemExit(main())
