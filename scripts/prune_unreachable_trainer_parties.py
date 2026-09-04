#!/usr/bin/env python3
"""Remove trainer party rows that cannot occur in the campaign.

Trainer IDs remain append-only constants for save/script compatibility.  Only
their materialized party blocks are pruned.  The campaign master is already
verified against every physical and scripted opponent; TRAINER_NONE is the
single structural row retained outside that executable roster.
"""

from __future__ import annotations

import argparse
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PARTIES = ROOT / "src/data/trainers.party"
MASTER = ROOT / "data/emerald_champions/emerald_champions_master_battle_design.txt"
MARKER = re.compile(r"(?m)^=== (TRAINER_[A-Z0-9_]+) ===$")


def split_blocks(source: str) -> tuple[str, list[tuple[str, str]]]:
    markers = list(MARKER.finditer(source))
    if not markers:
        raise SystemExit("trainers.party contains no trainer blocks")
    prefix = source[: markers[0].start()]
    blocks = []
    for index, marker in enumerate(markers):
        end = markers[index + 1].start() if index + 1 < len(markers) else len(source)
        blocks.append((marker.group(1), source[marker.start():end]))
    return prefix, blocks


def expected_ids() -> set[str]:
    master_ids = set(re.findall(r"\bTRAINER_[A-Z0-9_]+\b", MASTER.read_text()))
    return master_ids | {"TRAINER_NONE"}


def materialize(source: str) -> tuple[str, list[str]]:
    prefix, blocks = split_blocks(source)
    keep = expected_ids()
    present = {trainer for trainer, _ in blocks}
    missing = sorted(keep - present)
    if missing:
        raise SystemExit(f"campaign trainers missing from trainers.party: {missing}")
    removed = [trainer for trainer, _ in blocks if trainer not in keep]
    output = prefix + "".join(block for trainer, block in blocks if trainer in keep)
    return output, removed


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--write", action="store_true")
    args = parser.parse_args()

    current = PARTIES.read_text()
    expected, removed = materialize(current)
    if args.write:
        PARTIES.write_text(expected)
        print(f"pruned_trainer_party_rows={len(removed)}")
        return

    if current != expected:
        raise SystemExit(
            f"trainers.party contains {len(removed)} unreachable rows; "
            "run scripts/prune_unreachable_trainer_parties.py --write"
        )
    _, blocks = split_blocks(current)
    if len(blocks) != len(expected_ids()):
        raise SystemExit("materialized trainer row count does not equal campaign roster plus TRAINER_NONE")
    print(f"PASS: all {len(blocks) - 1} materialized opponent rows are reachable campaign branches")


if __name__ == "__main__":
    main()
