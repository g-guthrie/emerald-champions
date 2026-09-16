#!/usr/bin/env python3
"""Drive one headless trainer battle with a seeded random legal-move policy.

A regression exerciser for scripts/playthrough/battle_driver.py, not a play
benchmark: random legal choices say nothing about a trainer's difficulty. Every
command goes through the driver's own validation, so an illegal choice fails
loudly instead of being silently clamped.
"""
import argparse
import json
import random
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DRIVER = [sys.executable, str(ROOT / 'scripts/playthrough/battle_driver.py')]


def driver(*args):
    result = subprocess.run(DRIVER + list(args), cwd=ROOT, text=True, capture_output=True)
    if result.returncode != 0:
        raise SystemExit(f'{" ".join(args)} failed:\n{result.stdout}\n{result.stderr}')
    return json.loads(result.stdout)


def choose(rng, entry, phase, taken):
    """`taken` collects reserve slots already spent in this act call, so a double
    faint never sends the same Pokemon out twice."""
    legal_moves = [m for m in entry['moves'] if m['legal']]
    if entry.get('replacing') or phase == 'await_switch' or not legal_moves:
        slots = [s for s in entry['switch_slots'] if s not in taken]
        if not slots:
            raise SystemExit(f'battler {entry["battler"]} has no legal choice at all')
        slot = rng.choice(slots)
        taken.add(slot)
        return f'{entry["battler"]}:switch{slot}'
    move = rng.choice(legal_moves)
    return f'{entry["battler"]}:move{move["index"]}@{rng.choice(move["targets"])}'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--trainer', required=True)
    parser.add_argument('--trainer2')
    parser.add_argument('--partner')
    parser.add_argument('--party', required=True)
    parser.add_argument('--seed', type=lambda v: int(v, 0), required=True)
    parser.add_argument('--cap', type=int, required=True)
    parser.add_argument('--difficulty', default='medium')
    parser.add_argument('--run-dir', required=True)
    parser.add_argument('--build-dir')
    parser.add_argument('--max-decisions', type=int, default=200)
    args = parser.parse_args()

    start = ['start', '--trainer', args.trainer, '--party', args.party,
             '--seed', str(args.seed), '--cap', str(args.cap),
             '--difficulty', args.difficulty, '--run-dir', args.run_dir]
    if args.trainer2:
        start += ['--trainer2', args.trainer2]
    if args.partner:
        start += ['--partner', args.partner]
    if args.build_dir:
        start += ['--build-dir', args.build_dir]
    state = driver(*start)

    rng = random.Random(args.seed)
    for _ in range(args.max_decisions):
        if state['phase'] == 'ended' or not state['pending_decision']:
            break
        # Answer only what the engine is actually asking. Commanding a battler
        # that merely owes a replacement is legal, but it claims a reserve slot
        # before the engine has resolved a partner's switch that would free
        # another one, which can leave the mandatory replacement with nothing.
        taken = set()
        asked = [e for e in state['pending_decision'] if e.get('awaiting_now', True)]
        commands = [choose(rng, entry, state['phase'], taken) for entry in asked]
        if not commands:
            break
        events = driver('act', *commands, '--run-dir', args.run_dir)
        if events['phase'] == 'ended':
            break
        if not events['halted']:
            raise SystemExit(f'no decision point within {events["frames"]} frames after '
                             f'{commands}; the battle did not advance')
        state = driver('state', '--run-dir', args.run_dir)
    result = driver('result', '--run-dir', args.run_dir)
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
