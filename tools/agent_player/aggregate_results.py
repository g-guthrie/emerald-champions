#!/usr/bin/env python3
"""Aggregate comparable battle-run reports; declarations are not verified wins."""

from __future__ import annotations

import argparse
import hashlib
import json
import re
import statistics
from pathlib import Path
from typing import Any


def load(path: Path) -> Any:
    return json.loads(path.read_text())


def team_hash(value: str) -> str:
    return hashlib.sha256(value.encode()).hexdigest()


def raw_run(directory: Path) -> dict[str, Any]:
    session = load(directory / "session.json")
    if session.get("schema_version") != 1 or session.get("benchmark_mode") != "battle_lab":
        raise ValueError(f"{directory}: expected a battle_lab session, not a campaign automation run")
    for field in ("rom_sha256", "start_checkpoint_sha256", "arsenal_manifest_sha256",
                  "runner_sha256", "player_context_sha256", "config_sha256"):
        if not isinstance(session.get(field), str) or not re.fullmatch(r"[0-9a-f]{64}", session[field]):
            raise ValueError(f"{directory}: missing or invalid {field}")
    if session.get("elf_sha256") is not None and not re.fullmatch(r"[0-9a-f]{64}", str(session["elf_sha256"])):
        raise ValueError(f"{directory}: invalid elf_sha256")
    for field in ("seed", "rng_delay_frames", "rtc_epoch", "level_cap"):
        if type(session.get(field)) is not int:
            raise ValueError(f"{directory}: missing or invalid {field}")
    if session["rng_delay_frames"] < 0 or session["level_cap"] <= 0:
        raise ValueError(f"{directory}: invalid RNG delay or level cap")
    if session.get("observation_mode") not in {"vision_only", "instrumented"} or not isinstance(session.get("model"), dict):
        raise ValueError(f"{directory}: missing observation mode or model identity")
    protocol = session.get("comparison_protocol")
    if protocol is not None:
        mirrored = ("schema_version", "benchmark_mode", "battle_id", "observation_mode", "model", "rtc_epoch")
        settings = ("boot_frames", "step", "budgets", "init_writes", "probes")
        if not isinstance(protocol, dict) or any(key not in protocol for key in mirrored + settings):
            raise ValueError(f"{directory}: incomplete comparison_protocol")
        if any(protocol[key] != session.get(key) for key in mirrored):
            raise ValueError(f"{directory}: comparison_protocol disagrees with session identity")
        if not isinstance(protocol["step"], dict) or not isinstance(protocol["budgets"], dict):
            raise ValueError(f"{directory}: invalid comparison_protocol step or budgets")
        if not isinstance(protocol["init_writes"], list) or not isinstance(protocol["probes"], list):
            raise ValueError(f"{directory}: invalid comparison_protocol init_writes or probes")
    events = [json.loads(line) for line in (directory / "events.jsonl").read_text().splitlines()]
    battle_id = session.get("battle_id")
    attempts = [row for row in events if row.get("kind") == "semantic" and row.get("event") == "battle_attempt" and row.get("battle_id") == battle_id]
    successes = [row for row in events if row.get("kind") == "semantic" and row.get("event") == "battle_success" and row.get("battle_id") == battle_id]
    turns = [row for row in events if row.get("kind") == "semantic" and row.get("event") == "battle_turn" and row.get("battle_id") == battle_id]
    whiteouts = [row for row in events if row.get("kind") == "semantic" and row.get("event") == "whiteout" and row.get("battle_id") == battle_id]
    prep = [row for row in events if row.get("kind") == "prep" and row.get("status") == "applied" and row.get("battle_id") == battle_id]
    teams = [team_hash(str(row.get("value"))) for row in prep if row.get("mutation") == "roster"]
    first_success_time = successes[0]["timestamp_epoch"] if successes else None
    attempts_to_win = sum(row["timestamp_epoch"] <= first_success_time for row in attempts) if first_success_time is not None else None
    turns_to_win = sum(row["timestamp_epoch"] <= first_success_time for row in turns) if first_success_time is not None else None
    prep_before_win = [row for row in prep if row["timestamp_epoch"] <= first_success_time] if first_success_time is not None else []
    # Cost to solve stops at the first reported win. Keep all raw preparation
    # history below, but later experiments must not make that victory harder.
    scored_prep = prep_before_win if first_success_time is not None else prep
    winning_rosters = [team_hash(str(row.get("value"))) for row in prep_before_win if row.get("mutation") == "roster"]
    success_detail = successes[0].get("detail") if successes else None
    return {
        "run_dir": str(directory), "battle_id": battle_id, "seed": session["seed"], "rng_delay_frames": session["rng_delay_frames"],
        "checkpoint_sha256": session.get("start_checkpoint_sha256"), "rom_sha256": session.get("rom_sha256"),
        "arsenal_manifest_sha256": session["arsenal_manifest_sha256"],
        "cohort": {key: session.get(key) for key in (
            "schema_version", "benchmark_mode", "observation_mode", "model", "rtc_epoch", "level_cap",
            "runner_sha256", "elf_sha256", "player_context_sha256", "comparison_protocol",
        )},
        "outcome_basis": "reported", "verified_win": None,
        "won": bool(successes), "first_plan_success": bool(successes) and attempts_to_win == 1 and len(prep_before_win) <= 1,
        "attempts_to_first_win": attempts_to_win, "turns_to_first_win": turns_to_win,
        "whiteouts": len(whiteouts), "prep_revisions": max(0, len(scored_prep) - 1),
        "prep_types": sorted({row.get("mutation") for row in prep}), "team_hashes": teams,
        "winning_team_hash": winning_rosters[-1] if winning_rosters else None,
        "surviving_party_hp": success_detail,
        "budgets": session.get("metrics", {}),
    }


def median(values: list[int | None]) -> float | None:
    present = [value for value in values if value is not None]
    return statistics.median(present) if present else None


def rating(rows: list[dict[str, Any]]) -> dict[str, Any]:
    wins = [row for row in rows if row["won"]]
    win_rate = len(wins) / len(rows)
    first_rate = sum(row["first_plan_success"] for row in rows) / len(rows)
    attempts = median([row["attempts_to_first_win"] for row in wins])
    revisions = median([row["prep_revisions"] for row in wins])
    if not wins:
        label, rule = "no reported wins", "zero reported wins; budget completion and impossibility are not verified"
    elif win_rate < 0.5 or (attempts is not None and attempts >= 4):
        label, rule = "very hard", "win_rate < 0.5 or median attempts_to_first_win >= 4"
    elif first_rate < 0.5 or (attempts is not None and attempts >= 3) or (revisions is not None and revisions >= 2):
        label, rule = "hard", "first_plan_success_rate < 0.5, median attempts >= 3, or median prep revisions >= 2"
    elif first_rate < 0.8 or (attempts is not None and attempts >= 2):
        label, rule = "moderate", "first_plan_success_rate < 0.8 or median attempts >= 2"
    else:
        label, rule = "consistently solved", "first_plan_success_rate >= 0.8 and median attempts < 2"
    return {"label": label, "rule": rule, "unbeaten_within_budget": None, "outcome_basis": "reported"}


def aggregate_runs(runs: list[Path], battle_id: str, minimum_seeds: int = 5) -> dict[str, Any]:
    if minimum_seeds < 1:
        raise ValueError("minimum-seeds must be positive")
    if not battle_id:
        raise ValueError("battle_id must be nonempty")
    paths = [path.resolve() for path in runs]
    if len(set(paths)) != len(paths):
        raise ValueError("the same run directory was supplied more than once")
    rows = [raw_run(path) for path in paths]
    distinct_delays = len({row["rng_delay_frames"] for row in rows})
    if distinct_delays < minimum_seeds:
        raise ValueError(f"need at least {minimum_seeds} distinct RNG-delay seeds")
    if any(row["battle_id"] != battle_id for row in rows):
        raise ValueError("run battle_id mismatch")
    for field in ("checkpoint_sha256", "rom_sha256", "arsenal_manifest_sha256", "cohort"):
        if any(row[field] != rows[0][field] for row in rows):
            raise ValueError(f"runs do not share one {field}")
    wins = [row for row in rows if row["won"]]
    winning_teams = {row["winning_team_hash"] for row in wins if row["winning_team_hash"] is not None}
    return {
        "schema_version": 1, "battle_id": battle_id, "outcome_basis": "reported",
        "protocol": {
            "independent_seed_count": distinct_delays, "same_checkpoint": True, "authored_hard": None,
            "seed_basis": "distinct rng_delay_frames; statistical independence is not verified",
            "duplicate_delay_runs": len(rows) - distinct_delays,
            "same_rom": True, "same_arsenal": True,
            "comparison_protocol_recorded": rows[0]["cohort"]["comparison_protocol"] is not None,
            "difficulty_basis": "unverified: sessions do not record observed difficulty",
            "outcome_basis": "reported",
        },
        "raw_runs": rows,
        "aggregate": {
            "runs": len(rows), "wins": len(wins), "win_rate": len(wins) / len(rows),
            "first_plan_success_rate": sum(row["first_plan_success"] for row in rows) / len(rows),
            "median_attempts_to_first_win": median([row["attempts_to_first_win"] for row in wins]),
            "median_turns_to_first_win": median([row["turns_to_first_win"] for row in wins]),
            "total_whiteouts": sum(row["whiteouts"] for row in rows),
            "median_prep_revisions": median([row["prep_revisions"] for row in rows]),
            "roster_diversity": len({team for row in rows for team in row["team_hashes"]}),
            "distinct_winning_teams": len(winning_teams),
            "outcome_basis": "reported",
        },
        "rating": rating(rows),
    }


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--battle-id", required=True)
    parser.add_argument("--minimum-seeds", type=int, default=5)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("runs", nargs="+", type=Path)
    args = parser.parse_args()
    try:
        payload = aggregate_runs(args.runs, args.battle_id, args.minimum_seeds)
    except (OSError, ValueError, KeyError, TypeError) as error:
        raise SystemExit(str(error)) from error
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(payload, indent=2, sort_keys=True) + "\n")
    print(args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
