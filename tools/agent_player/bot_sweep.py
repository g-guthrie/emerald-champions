#!/usr/bin/env python3
"""Bot benchmark: play one trainer battle with the tactical policy across
level deltas and seeds, and report a win-rate curve.

No language model plays: TacticalPolicy (doubles_policy.py) answers every
decision through the headless battle driver. Each (delta, seed) is an
independent native run, so runs execute in parallel worker processes.

  python3 tools/agent_player/bot_sweep.py --trainer TRAINER_CALVIN_1 --cap 14 \
      --party work/retune/02-TRAINER_CALVIN_1/party.json --deltas 0,2,3,4 --seeds 1-8 \
      --out work/retune/bot/02-calvin --workers 3
"""
from __future__ import annotations

import argparse
import json
import subprocess
import sys
import time
from concurrent.futures import ProcessPoolExecutor, as_completed
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))


def play(trainer: str, party: str, cap: int, delta: int, seed: int, directory: str,
         owners: str, max_decisions: int, step_timeout: int) -> dict:
    from battle_calibration import NativeDriver, DriverFailure, public_observation
    from doubles_policy import TacticalPolicy, PolicyUnresolved
    started = time.monotonic()
    driver = NativeDriver(Path(directory), step_timeout)
    deltas = ",".join(f"{owner}*={delta:+d}" for owner in owners)
    policy = TacticalPolicy()
    outcome, reason, turns = "unresolved", "", 0
    try:
        state = driver.call("start", "--trainer", trainer, "--party", party, "--difficulty", "hard",
                            "--cap", str(cap), "--seed", str(seed), "--level-delta", deltas)
        for decision in range(max_decisions):
            if state.get("phase") == "ended":
                break
            if state.get("phase") not in {"await_action", "await_switch"}:
                reason = f"unexpected phase {state.get('phase')}"
                break
            observation = public_observation(state)
            policy.observe(observation)
            driver.call("act", *policy.choose(observation, decision))
            turns = decision + 1
            state = driver.call("state")
        if state.get("phase") == "ended":
            result = driver.call("result")
            outcome = str(result.get("outcome", "unresolved"))
        elif not reason:
            reason = "decision budget exhausted"
    except (DriverFailure, PolicyUnresolved, ValueError, KeyError, subprocess.TimeoutExpired) as error:
        reason = f"{type(error).__name__}: {error}"[:300]
    return dict(delta=delta, seed=seed, outcome=outcome, decisions=turns, reason=reason,
                seconds=round(time.monotonic() - started), run_dir=directory)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--trainer", required=True)
    parser.add_argument("--party", required=True)
    parser.add_argument("--cap", type=int, required=True)
    parser.add_argument("--deltas", default="0,2,4", help="comma list, e.g. -2,0,2,3,4")
    parser.add_argument("--seeds", default="1-6", help="e.g. 1-8 or 1,3,5")
    parser.add_argument("--owners", default="A", help="opposing owners to shift: A, or AB for two-owner battles")
    parser.add_argument("--out", required=True)
    parser.add_argument("--workers", type=int, default=3)
    parser.add_argument("--max-decisions", type=int, default=200)
    parser.add_argument("--step-timeout", type=int, default=600)
    args = parser.parse_args(argv)

    deltas = [int(d) for d in args.deltas.split(",")]
    seeds = _seeds(args.seeds)
    out = Path(args.out)
    out.mkdir(parents=True, exist_ok=True)
    party = str(Path(args.party).resolve())
    jobs = []
    with ProcessPoolExecutor(max_workers=args.workers) as pool:
        for delta in deltas:
            for seed in seeds:
                directory = out / f"d{delta:+d}-s{seed}"
                if directory.exists():
                    continue
                jobs.append(pool.submit(play, args.trainer, party, args.cap, delta, seed, str(directory),
                                        args.owners, args.max_decisions, args.step_timeout))
        results = []
        for job in as_completed(jobs):
            row = job.result()
            results.append(row)
            print(f"  d{row['delta']:+d} s{row['seed']}: {row['outcome']} ({row['decisions']} decisions, "
                  f"{row['seconds']}s){' ' + row['reason'] if row['reason'] else ''}", flush=True)
    path = out / "results.json"
    previous = json.loads(path.read_text()) if path.exists() else []
    merged = {(r["delta"], r["seed"]): r for r in previous + results}
    rows = sorted(merged.values(), key=lambda r: (r["delta"], r["seed"]))
    path.write_text(json.dumps(rows, indent=1) + "\n")
    print(f"{args.trainer} (cap {args.cap}) bot win rate by Hard delta:")
    for delta in sorted({r["delta"] for r in rows}):
        group = [r for r in rows if r["delta"] == delta]
        wins = sum(r["outcome"] == "won" for r in group)
        bad = sum(r["outcome"] not in ("won", "lost") for r in group)
        print(f"  {delta:+d}: {wins}/{len(group) - bad} won" + (f"  ({bad} unresolved)" if bad else ""))
    return 0


def _seeds(text: str) -> list[int]:
    values = []
    for part in text.split(","):
        if "-" in part:
            low, high = part.split("-")
            values.extend(range(int(low), int(high) + 1))
        else:
            values.append(int(part))
    return values


if __name__ == "__main__":
    raise SystemExit(main())
