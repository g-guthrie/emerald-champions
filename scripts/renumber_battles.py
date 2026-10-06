#!/usr/bin/env python3
"""Renumber the campaign's battles (E0001...) to the order play reaches them.

The route manifest (artifacts/progression-manifest/route-manifest.txt) is the verified play order:
its steps' Battles lines in order, then the battles each step defers, then every battle not yet
verified in its current order. Live battles take the existing live numbers in that order, so
retired numbers (data/emerald_champions/retired_battles.json) keep their slots and nothing collides.
Every tracked text file that names a live battle number is rewritten in one pass.

  python3 scripts/renumber_battles.py --plan    print the moves
  python3 scripts/renumber_battles.py --write   apply them
"""
from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tests'))
import test_manifest_battle_order as order   # the manifest parser the order test uses

TEXT = {'.txt', '.json', '.py', '.c', '.h', '.inc', '.md', '.party'}


def plan() -> dict[int, int]:
    live, _ = order.live_battles()
    current = sorted(live)
    wanted, seen = [], set()
    steps = order.manifest_steps()
    for _, battles, _ in steps:
        for n in battles:
            if n not in seen: wanted.append(n); seen.add(n)
    for _, _, deferred in steps:
        for n in deferred:
            if n not in seen: wanted.append(n); seen.add(n)
    wanted += [n for n in current if n not in seen]
    return {old: new for old, new in zip(wanted, current) if old != new}


def write(moves: dict[int, int]) -> int:
    files = subprocess.run(['git', 'ls-files'], cwd=ROOT, capture_output=True, text=True).stdout.split()
    pat = re.compile(r'\bE(\d{4})\b')
    changed = 0
    for f in files:
        p = ROOT / f
        if p.suffix not in TEXT: continue
        try: s = p.read_text()
        except (UnicodeDecodeError, FileNotFoundError): continue
        t = pat.sub(lambda m: f'E{moves.get(int(m.group(1)), int(m.group(1))):04d}', s)
        if t != s:
            p.write_text(t); changed += 1
    return changed


def main():
    ap = argparse.ArgumentParser()
    g = ap.add_mutually_exclusive_group(required=True)
    g.add_argument('--plan', action='store_true'); g.add_argument('--write', action='store_true')
    a = ap.parse_args()
    moves = plan()
    if a.plan:
        for old, new in sorted(moves.items(), key=lambda kv: kv[1]): print(f'E{old:04d} -> E{new:04d}')
        print(f'{len(moves)} battles move'); return
    print(f'{write(moves)} files changed, {len(moves)} battles renumbered')


if __name__ == '__main__':
    main()
