#!/usr/bin/env python3
"""Run certified independent native battle puzzles with a frozen player policy.

This is evaluation transport, not an expert solver. No result labels a battle
impossible or assigns a human difficulty from an automated policy's win rate.
A legal winning witness requires complete native evidence and a fresh replay.
"""
from __future__ import annotations

import argparse
from collections import Counter
import copy
import json
import os
import signal
import shutil
import subprocess
import sys
import time
import tempfile
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))
sys.path.insert(0, str(ROOT / "scripts"))
import generate_battle_suite as suite_tools
import build_provenance
import stamp_release_inputs
from doubles_policy import PolicyUnresolved, matches

CATEGORIES = {"won", "lost", "timeout", "invalid", "unresolved"}


def load(path: Path) -> Any:
    return json.loads(path.read_text())


def save(path: Path, data: Any) -> None:
    path.write_text(json.dumps(data, indent=2, sort_keys=True) + "\n")


def public_observation(state: dict) -> dict:
    """Supply the board and loadouts, never native commands or future RNG.

    Player policies in this first adapter have a scouting report of the authored
    opponent. That assumption is recorded; they are not novice-player models.
    """
    result = copy.deepcopy(state)
    for battler in result.get("actives", []):
        battler.pop("choosing", None)
    for key in ("seed", "rng", "selection_state", "message_serial", "log_head",
                "ai_decision_frames", "ai_setup_frames", "ai_delay_frames"):
        result.pop(key, None)
    return result


class ScriptedPolicy:
    def __init__(self, data: dict):
        if data.get("schema_version") != 1 or data.get("kind") != "scripted":
            raise ValueError("policy must be schema 1, kind=scripted")
        self.decisions = data.get("decisions", [])
        self.rules = data.get("rules", [])
        if not isinstance(self.decisions, list) or not isinstance(self.rules, list):
            raise ValueError("policy decisions and rules must be lists")
        if not self.decisions and not self.rules:
            raise ValueError("policy must contain at least one decision or rule")
        for entry in self.decisions + self.rules:
            if not isinstance(entry, dict) or not isinstance(entry.get("commands"), list) or not entry["commands"]:
                raise ValueError("policy entries require nonempty commands lists")
            if any(not isinstance(command, str) for command in entry["commands"]):
                raise ValueError("policy commands must be strings")
        for rule in self.rules:
            if not isinstance(rule.get("when"), dict):
                raise ValueError("policy rules need explicit observation predicates")

    def choose(self, observation: dict, decision: int) -> list[str]:
        if decision < len(self.decisions):
            entry = self.decisions[decision]
            if not matches(entry.get("expected", {}), observation):
                raise PolicyUnresolved(f"scripted decision {decision} guard does not match this board")
            return entry["commands"]
        for rule in self.rules:
            if matches(rule["when"], observation):
                return rule["commands"]
        raise PolicyUnresolved("frozen policy has no decision for this board")


def make_policy(data: dict):
    if data.get('kind') == 'tactical':
        from doubles_policy import TacticalPolicy
        return TacticalPolicy(data)
    return ScriptedPolicy(data)


class DriverFailure(Exception):
    pass


class NativeDriver:
    def __init__(self, directory: Path, step_timeout: int):
        self.directory = directory
        self.timeout = step_timeout
        self.calls = 0
        self.directory.mkdir(parents=True)

    def call(self, command: str, *arguments: str) -> dict:
        self.calls += 1
        argv = [sys.executable, str(ROOT / "scripts/playthrough/battle_driver.py"), command,
                "--run-dir", str(self.directory), *arguments]
        process = subprocess.Popen(argv, cwd=ROOT, text=True, stdout=subprocess.PIPE,
                                   stderr=subprocess.PIPE, start_new_session=True)
        timed_out = False
        try:
            stdout, stderr = process.communicate(timeout=self.timeout)
        except subprocess.TimeoutExpired:
            timed_out = True
            # The Python driver owns an mGBA child. Terminating only Python
            # leaves the emulator running and loses its failure diagnostics.
            os.killpg(process.pid, signal.SIGKILL)
            stdout, stderr = process.communicate()
        # Preserve refused actions, assertions and driver diagnostics as evidence.
        save(self.directory / f"driver-call-{self.calls:04d}.json", {
            "argv": argv, "returncode": process.returncode, "timed_out": timed_out,
            "stdout": stdout, "stderr": stderr,
        })
        if timed_out:
            raise subprocess.TimeoutExpired(argv, self.timeout, output=stdout, stderr=stderr)
        if process.returncode:
            raise DriverFailure(stderr.strip() or stdout.strip() or "native driver failed")
        try:
            return json.loads(stdout)
        except ValueError as error:
            raise DriverFailure("native driver returned non-JSON output") from error


def terminal_category(result: dict) -> str:
    if result.get("phase") != "ended":
        return "unresolved"
    return result.get("outcome") if result.get("outcome") in {"won", "lost"} else "unresolved"


def trace_digest(trace):
    semantic = [{"observation": entry["observation"], "commands": entry["commands"],
                 "event": {key: value for key, value in entry["event"].items()
                           if key not in {"ai_decision_frames", "ai_setup_frames", "ai_delay_frames", "frames"}}}
                for entry in trace]
    return suite_tools.digest_bytes(suite_tools.canonical(semantic))


def run_seed(driver: Any, scenario: dict, party_path: Path, scenario_path: Path,
             build_path: Path, policy: ScriptedPolicy, seed: int,
             max_decisions: int = 300, timeout_seconds: int = 3600) -> dict:
    trace = []
    deadline = time.monotonic() + timeout_seconds
    category, reason, native = "unresolved", "", None
    try:
        start_arguments = ["--trainer", scenario["trainer_id"],
                            "--difficulty", scenario["difficulty"], "--cap", str(scenario["level_cap"]),
                            "--scenario", str(scenario_path),
                            "--seed", str(seed), "--build-dir", str(build_path)]
        if scenario.get('battle_kind') == 'birch_rescue':
            start_arguments += ['--battle-kind', 'birch_rescue']
        else:
            start_arguments += ['--party', str(party_path)]
        state = driver.call('start', *start_arguments)
        for decision in range(max_decisions):
            if state.get("phase") == "ended":
                break
            if time.monotonic() >= deadline:
                category, reason = "timeout", "experiment wall-clock budget exhausted"
                break
            if state.get("phase") not in {"await_action", "await_switch"}:
                category, reason = "timeout", "native battle did not halt at a decision"
                break
            observation = public_observation(state)
            commands = policy.choose(observation, decision)
            event = driver.call("act", *commands)
            trace.append({"observation": observation, "commands": commands, "event": event})
            state = driver.call("state")
        else:
            if state.get("phase") != "ended":
                category, reason = "timeout", "decision budget exhausted"
        if state.get("phase") == "ended":
            native = driver.call("result")
            category = terminal_category(native)
            if category == "unresolved":
                reason = f"unsupported native outcome: {native.get('outcome')}"
    except PolicyUnresolved as error:
        category, reason = "unresolved", str(error)
    except subprocess.TimeoutExpired:
        category, reason = "timeout", "native driver call exceeded step budget"
    except (DriverFailure, ValueError, KeyError) as error:
        category, reason = "invalid", str(error)
    events_path = driver.directory / "events.jsonl"
    events = [json.loads(line) for line in events_path.read_text().splitlines()] if events_path.exists() else []
    logs = [record for record in events if record.get("event") in {"start", "act"}]
    partial_logs_complete = bool(logs) and logs[0].get("event") == "start" and all(
        record.get("log_complete") is True for record in logs)
    complete = partial_logs_complete and native is not None and native.get('phase') == 'ended' \
        and category in {'won', 'lost'}
    session_path = driver.directory / "session.json"
    field_exact = False
    opponent_identity_verified = False
    player_factory_verified = scenario.get('battle_kind') != 'birch_rescue'
    factory_sha256 = None
    if session_path.exists():
        session = load(session_path)
        opponent_identity_verified = session.get('opponent_identity', {}).get('status') == 'verified'
        if scenario.get('battle_kind') == 'birch_rescue':
            factory = session.get('player_factory', {})
            player_factory_verified = factory.get('status') == 'verified'
            factory_sha256 = factory.get('roster_sha256')
        field = session.get("map_field", {})
        explicit = scenario.get("battle_field", {})
        if explicit.get("weather") and explicit.get("environment"):
            field_exact = (field.get("weather") == explicit["weather"] and
                           field.get("environment") == explicit["environment"])
        else:
            field_exact = bool(field.get("map")) and not any(key in field for key in
                ("weather_cycle", "story_conditional_weather", "position_dependent_weather", "possible_anomaly_weather")) \
                and "not reproduced" not in field.get("weather_basis", "")
    result = {
        "seed": seed, "category": category, "reason": reason, "native_result": native,
        "decisions": len(trace), "evidence_complete": complete,
        "partial_logs_complete": partial_logs_complete,
        "battle_field_exact": field_exact,
        "opponent_identity_verified": opponent_identity_verified,
        "player_factory_verified": player_factory_verified,
        "player_factory_sha256": factory_sha256,
        "trace_sha256": trace_digest(trace),
        "witness_verified": False,
    }
    save(driver.directory / "policy-trace.json", trace)
    save(driver.directory / "evaluation.json", result)
    return result


def validate_seeds(config: dict) -> tuple[list[int], list[int]]:
    discovery = config.get("discovery_seeds", [])
    evaluation = config.get("evaluation_seeds", [])
    for name, seeds in (("discovery", discovery), ("evaluation", evaluation)):
        if not isinstance(seeds, list) or any(type(seed) is not int or not 0 <= seed < 2**32 for seed in seeds):
            raise ValueError(f"{name} seeds must be unsigned 32-bit integers")
        if len(seeds) != len(set(seeds)):
            raise ValueError(f"duplicate {name} seeds would overcount evidence")
    if not evaluation:
        raise ValueError("supply at least one held-out evaluation seed")
    if set(discovery) & set(evaluation):
        raise ValueError("evaluation seeds must be held out from policy discovery")
    return discovery, evaluation


def snapshot_build(build: Path, target: Path) -> dict:
    digest, count = stamp_release_inputs.digest_tree()
    stamp = load(build / "pokeemerald-headless.inputs.json")
    artifacts = tuple(build / name for name in ("pokeemerald-headless.gba", "pokeemerald-headless.elf"))
    stamp_release_inputs.verify_stamp(stamp, digest, count, artifacts)
    provenance = build_provenance.verify(*artifacts)
    if provenance.get("status") != "verified":
        raise ValueError(f"headless build provenance is unverified: {provenance.get('reason')}")
    target.mkdir()
    for artifact in (*artifacts, build / "pokeemerald-headless.inputs.json"):
        shutil.copy2(artifact, target / artifact.name)
    copied = build_provenance.carry(artifacts[0], target / artifacts[0].name, target / artifacts[1].name)
    stamp_release_inputs.verify_stamp(stamp, digest, count, tuple(target / p.name for p in artifacts))
    if copied.get("status") != "verified":
        raise ValueError("artifact snapshot provenance failed verification")
    return copied


def validate_puzzle(suite: dict, puzzle_id: str, party: dict) -> dict:
    if suite.get("schema_version") != 2 or suite.get("source_fingerprint") != suite_tools.source_fingerprint():
        raise ValueError("battle suite is unsupported or stale for current sources")
    found = [puzzle for puzzle in suite["puzzles"] if puzzle["puzzle_id"] == puzzle_id]
    if len(found) != 1:
        raise ValueError("puzzle_id must identify exactly one source scenario")
    puzzle = found[0]
    content = {key: value for key, value in puzzle.items() if key != "content_sha256"}
    if puzzle.get("content_sha256") != suite_tools.digest_bytes(suite_tools.canonical(content)):
        raise ValueError("puzzle content digest does not match")
    scenario = puzzle["scenario"]
    if scenario.get('battle_kind') == 'birch_rescue':
        if party is not None:
            raise ValueError('Rescue calibration cannot accept a prepared-party manifest')
        import battle_scripted_wild_arsenal as wild
        wild.certify_scenario(scenario)
        return puzzle
    if scenario.get('battle_kind', 'trainer') != 'trainer':
        raise ValueError('scripted-wild native fixtures require a dedicated source stage producer; trainer suite cannot certify them')
    if party is None:
        raise ValueError('Trainer calibration requires a validated prepared-party manifest')
    if scenario.get("legality_status") != "proven":
        raise ValueError("scenario legality is unresolved; candidate pools cannot certify a winning team")
    if not scenario.get('expected_opponent'):
        raise ValueError('scenario requires source-derived expected opponent identity before native calibration')
    # The separate producer validates its fixedpoint acquisition certificate and
    # joint party resources. source_generated=true alone is never a legal proof.
    import battle_arsenal
    battle_arsenal.validate_party(scenario, party)
    # Exercise the same native preparation encoder before any build snapshot
    # or clean boot. Source legality alone does not guarantee a usable manifest.
    from prepare_party import protocol as prepare_protocol
    with tempfile.TemporaryDirectory(prefix='ec-calibration-preflight-') as scratch:
        manifest_path = Path(scratch) / 'party.json'
        save(manifest_path, party)
        try:
            prepare_protocol(manifest_path, ROOT)
        except subprocess.CalledProcessError as error:
            raise ValueError('native preparation protocol could not resolve source constants') from error
    return puzzle


def evaluate(config_path: Path, run_dir: Path) -> dict:
    config = load(config_path)
    if config.get("schema_version") != 1:
        raise ValueError("calibration config must be schema 1")
    discovery, seeds = validate_seeds(config)
    resolve = lambda name: (config_path.parent / config[name]).resolve()
    party = load(resolve('party')) if config.get('party') is not None else None
    policy_data, suite = load(resolve("policy")), load(resolve("suite"))
    policy = make_policy(policy_data)
    puzzle = validate_puzzle(suite, config["puzzle_id"], party)
    budgets = config.get("budgets", {})
    budget_values = {key: budgets.get(key, default) for key, default in
                     (("max_decisions", 300), ("timeout_seconds", 3600), ("step_timeout_seconds", 900))}
    if any(type(value) is not int or value <= 0 for value in budget_values.values()):
        raise ValueError("budgets must be positive integer limits")
    run_dir.mkdir(parents=True, exist_ok=False)
    build = run_dir / "build"
    provenance = snapshot_build(resolve("build_dir"), build)
    save(run_dir / "config.json", config)
    save(run_dir / "puzzle.json", puzzle)
    if party is not None:
        save(run_dir / "party.json", party)
    save(run_dir / "policy.json", policy_data)
    save(run_dir / "scenario.json", puzzle["scenario"])
    identities = {name: suite_tools.digest_file(run_dir / name) for name in
                  ("config.json", "puzzle.json", "policy.json", "scenario.json")}
    if party is not None:
        identities['party.json'] = suite_tools.digest_file(run_dir / 'party.json')
    for path in sorted(build.iterdir()):
        if path.is_file():
            identities[str(path.relative_to(run_dir))] = suite_tools.digest_file(path)
    results = []
    for seed in seeds:
        # Player memory (for example entry-turn Fake Out availability) belongs
        # to one fight, never the preceding seed in the evaluation suite.
        policy = make_policy(policy_data)
        driver = NativeDriver(run_dir / f"seed-{seed:08x}", budget_values["step_timeout_seconds"])
        party_path = run_dir / 'party.json' if party is not None else None
        result = run_seed(driver, puzzle["scenario"], party_path, run_dir / "scenario.json",
                          build, policy, seed, budget_values["max_decisions"], budget_values["timeout_seconds"])
        if result["category"] == "won" and result["evidence_complete"] and result["battle_field_exact"] and result['opponent_identity_verified'] and result['player_factory_verified']:
            replay = run_seed(NativeDriver(run_dir / f"replay-{seed:08x}", budget_values["step_timeout_seconds"]),
                              puzzle["scenario"], party_path, run_dir / "scenario.json", build,
                              policy, seed, budget_values["max_decisions"], budget_values["timeout_seconds"])
            result["witness_verified"] = replay["category"] == "won" and replay["evidence_complete"] and replay["battle_field_exact"] and replay['opponent_identity_verified'] and \
                replay["trace_sha256"] == result["trace_sha256"] and replay['player_factory_verified'] and \
                replay['player_factory_sha256'] == result['player_factory_sha256']
            result["replay"] = replay
        results.append(result)
        save(driver.directory / "evaluation.json", result)
        save(run_dir / "results.json", results)
        print(f"seed {seed}: {result['category']}, decisions={result['decisions']}, "
              f"replayed_witness={result['witness_verified']}", flush=True)
    frozen_inputs = all(suite_tools.digest_file(run_dir / name) == digest for name, digest in identities.items())
    source_current = suite_tools.source_fingerprint() == suite["source_fingerprint"]
    artifact_current = build_provenance.verify(build / "pokeemerald-headless.gba", build / "pokeemerald-headless.elf").get("status") == "verified"
    valid = frozen_inputs and source_current and artifact_current
    counts = dict(Counter(result["category"] for result in results))
    summary = {
        "schema_version": 1, "kind": "native_independent_battle_policy_evaluation",
        "puzzle_id": puzzle["puzzle_id"], "source_fingerprint": suite["source_fingerprint"],
        "build_provenance": provenance, "input_sha256": identities,
        "discovery_seeds": discovery, "evaluation_seeds": seeds,
        "categories": counts, "results": results, "evidence_valid": valid,
        "verified_legal_winning_witnesses": sum(result["witness_verified"] for result in results) if valid else 0,
        "held_out_policy_win_fraction": counts.get("won", 0) / len(seeds) if valid else None,
        "limitations": ["Frozen observable player policy with opponent scouting; no human difficulty grade.",
                        "An unresolved policy or failed search never proves a fight impossible.",
                        "Independent fights with retries; no campaign completion or permadeath claim."],
    }
    if not valid:
        summary["invalid_reason"] = "source, immutable inputs or artifact snapshot changed during evaluation"
    save(run_dir / "summary.json", summary)
    return summary


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", type=Path, required=True)
    parser.add_argument("--run-dir", type=Path, required=True)
    args = parser.parse_args()
    try:
        result = evaluate(args.config.resolve(), args.run_dir.resolve())
    except (ValueError, OSError, KeyError) as error:
        parser.exit(1, f"calibration refused: {error}\n")
    print(json.dumps({key: result[key] for key in ("puzzle_id", "categories", "evidence_valid", "verified_legal_winning_witnesses")}, indent=2))
    return 0 if result["evidence_valid"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
