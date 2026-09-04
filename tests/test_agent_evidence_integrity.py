"""Synthetic negative controls for report aggregation and harness checkpoints."""
import copy
import importlib.util
import json
import tempfile
import time
import unittest
from pathlib import Path
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[1]


def module(name):
    spec = importlib.util.spec_from_file_location(name, ROOT / "tools/agent_player" / f"{name}.py")
    result = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(result)
    return result


player = module("agent_player")
aggregate = module("aggregate_results")


class AggregateEvidence(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.runs = []
        for i in range(3):
            path = self.root / str(i)
            path.mkdir()
            session = {
                "schema_version": 1, "benchmark_mode": "battle_lab", "battle_id": "calvin",
                "rom_sha256": "a" * 64, "start_checkpoint_sha256": "b" * 64,
                "arsenal_manifest_sha256": "c" * 64, "runner_sha256": "d" * 64,
                "player_context_sha256": "e" * 64, "elf_sha256": "f" * 64,
                "config_sha256": str(i) * 64, "seed": i, "rng_delay_frames": i * 17,
                "rtc_epoch": 946684800, "level_cap": 14, "observation_mode": "vision_only",
                "model": {"provider": "manual", "name": "test-policy"},
                "metrics": {},
            }
            session["comparison_protocol"] = {
                key: session[key] for key in ("schema_version", "benchmark_mode", "battle_id", "observation_mode", "model", "rtc_epoch")
            }
            session["comparison_protocol"].update({
                "boot_frames": 1, "step": {"press_frames": 1, "release_frames": 15},
                "budgets": {"max_actions": 50}, "init_writes": [], "probes": [],
            })
            player.atomic_json(path / "session.json", session)
            player.append_jsonl(path / "events.jsonl", {
                "kind": "semantic", "event": "battle_attempt", "battle_id": "calvin", "timestamp_epoch": 0,
            })
            player.append_jsonl(path / "events.jsonl", {
                "kind": "semantic", "event": "battle_success", "battle_id": "calvin", "timestamp_epoch": 1,
                "detail": "The agent says this battle was won.",
            })
            self.runs.append(path)
        self.result()  # Every mutation below starts from a accepted cohort.

    def change(self, index, **fields):
        path = self.runs[index] / "session.json"
        session = player.load_json(path)
        session.update(fields)
        player.atomic_json(path, session)

    def result(self, minimum=3):
        return aggregate.aggregate_runs(self.runs, "calvin", minimum)

    def test_happy_path_preserves_numeric_results_but_labels_declarations(self):
        result = self.result()
        self.assertEqual(result["aggregate"]["wins"], 3)
        self.assertEqual(result["aggregate"]["first_plan_success_rate"], 1)
        self.assertEqual(result["protocol"]["independent_seed_count"], 3)
        self.assertIsNone(result["protocol"]["authored_hard"])
        self.assertEqual(result["outcome_basis"], "reported")
        self.assertEqual(result["rating"]["outcome_basis"], "reported")
        self.assertTrue(all(row["verified_win"] is None for row in result["raw_runs"]))
        self.assertTrue(result["protocol"]["comparison_protocol_recorded"])

    def test_mixed_roms_are_rejected(self):
        self.change(1, rom_sha256="1" * 64)
        with self.assertRaisesRegex(ValueError, "rom_sha256"):
            self.result()

    def test_null_checkpoint_hashes_cannot_claim_same_checkpoint(self):
        for i in range(3):
            self.change(i, start_checkpoint_sha256=None)
        with self.assertRaisesRegex(ValueError, "start_checkpoint_sha256"):
            self.result()

    def test_mixed_checkpoints_are_rejected(self):
        self.change(1, start_checkpoint_sha256="1" * 64)
        with self.assertRaisesRegex(ValueError, "checkpoint_sha256"):
            self.result()

    def test_mixed_arsenals_are_rejected(self):
        self.change(1, arsenal_manifest_sha256="1" * 64)
        with self.assertRaisesRegex(ValueError, "arsenal_manifest_sha256"):
            self.result()

    def test_missing_identity_is_not_an_equivalent_cohort(self):
        for field in ("rom_sha256", "arsenal_manifest_sha256", "runner_sha256", "player_context_sha256"):
            with self.subTest(field=field):
                original = player.load_json(self.runs[0] / "session.json")
                self.change(0, **{field: None})
                with self.assertRaisesRegex(ValueError, field):
                    self.result()
                player.atomic_json(self.runs[0] / "session.json", original)

    def test_mixed_protocol_fields_are_rejected(self):
        for field, value in (
            ("observation_mode", "instrumented"), ("rtc_epoch", 123), ("level_cap", 20),
            ("model", {"name": "other"}), ("elf_sha256", "1" * 64),
        ):
            with self.subTest(field=field):
                original = player.load_json(self.runs[0] / "session.json")
                self.result()
                changed = copy.deepcopy(original)
                changed[field] = value
                if field in changed["comparison_protocol"]:
                    changed["comparison_protocol"][field] = value
                player.atomic_json(self.runs[0] / "session.json", changed)
                aggregate.raw_run(self.runs[0])  # Valid individually; incompatible with this cohort.
                with self.assertRaisesRegex(ValueError, "cohort"):
                    self.result()
                player.atomic_json(self.runs[0] / "session.json", original)

    def test_mixed_budgets_are_rejected_even_when_artifacts_match(self):
        session = player.load_json(self.runs[0] / "session.json")
        session["comparison_protocol"]["budgets"]["max_actions"] = 100
        player.atomic_json(self.runs[0] / "session.json", session)
        with self.assertRaisesRegex(ValueError, "cohort"):
            self.result()

    def test_empty_protocol_is_not_reported_as_complete(self):
        for i in range(3):
            self.change(i, comparison_protocol={})
        with self.assertRaisesRegex(ValueError, "incomplete comparison_protocol"):
            self.result()

    def test_legacy_protocol_absence_is_disclosed_and_not_mixed_with_new(self):
        self.change(0, comparison_protocol=None)
        with self.assertRaisesRegex(ValueError, "cohort"):
            self.result()
        self.change(1, comparison_protocol=None)
        self.change(2, comparison_protocol=None)
        self.assertFalse(self.result()["protocol"]["comparison_protocol_recorded"])

    def test_campaign_automation_cannot_become_battle_evidence(self):
        self.change(0, benchmark_mode="campaign_play")
        with self.assertRaisesRegex(ValueError, "battle_lab"):
            self.result()

    def test_distinct_rng_delays_are_counted_instead_of_run_rows(self):
        self.change(2, rng_delay_frames=17)
        with self.assertRaisesRegex(ValueError, "distinct RNG-delay"):
            self.result()
        result = self.result(minimum=2)
        self.assertEqual(result["aggregate"]["runs"], 3)
        self.assertEqual(result["protocol"]["independent_seed_count"], 2)
        self.assertEqual(result["protocol"]["duplicate_delay_runs"], 1)

    def test_duplicate_directories_and_invalid_thresholds_are_rejected(self):
        with self.assertRaisesRegex(ValueError, "more than once"):
            aggregate.aggregate_runs(self.runs + [self.runs[0]], "calvin", 3)
        with self.assertRaisesRegex(ValueError, "positive"):
            self.result(minimum=0)

    def test_missing_or_boolean_delays_are_not_seeds(self):
        original = player.load_json(self.runs[0] / "session.json")
        for delay in (None, True, -1):
            with self.subTest(delay=delay):
                player.atomic_json(self.runs[0] / "session.json", original)
                self.result(minimum=1)
                self.change(0, rng_delay_frames=delay)
                with self.assertRaises(ValueError):
                    self.result(minimum=1)

    def test_missing_attempt_does_not_claim_first_plan_or_verified_success(self):
        self.assertTrue(self.result()["raw_runs"][0]["first_plan_success"])
        path = self.runs[0] / "events.jsonl"
        rows = [json.loads(line) for line in path.read_text().splitlines()]
        path.write_text("\n".join(json.dumps(row) for row in rows if row.get("event") != "battle_attempt") + "\n")
        row = self.result()["raw_runs"][0]
        self.assertFalse(row["first_plan_success"])
        self.assertEqual(row["outcome_basis"], "reported")
        self.assertIsNone(row["verified_win"])

    def test_unfinished_runs_without_reported_wins_do_not_claim_budget_exhaustion(self):
        for run in self.runs:
            session = player.load_json(run / "session.json")
            session["status"] = "active"
            player.atomic_json(run / "session.json", session)
            (run / "events.jsonl").write_text(json.dumps({
                "kind": "semantic", "event": "battle_attempt", "battle_id": "calvin", "timestamp_epoch": 0,
            }) + "\n")
        result = self.result()
        self.assertEqual(result["aggregate"]["wins"], 0)
        self.assertEqual(result["rating"]["label"], "no reported wins")
        self.assertIsNone(result["rating"]["unbeaten_within_budget"])

    def test_post_win_roster_is_not_credited_as_winning_team(self):
        rows = [
            {"kind": "prep", "status": "applied", "mutation": "roster", "value": "original",
             "battle_id": "calvin", "timestamp_epoch": 0},
            {"kind": "semantic", "event": "battle_attempt", "battle_id": "calvin", "timestamp_epoch": 1},
            {"kind": "semantic", "event": "battle_success", "battle_id": "calvin", "timestamp_epoch": 2},
        ]
        for run in self.runs:
            (run / "events.jsonl").write_text("\n".join(json.dumps(row) for row in rows) + "\n")
        before = self.result()
        for run in self.runs:
            for i in range(3):
                player.append_jsonl(run / "events.jsonl", {
                    "kind": "prep", "status": "applied", "mutation": "roster", "value": f"after-win-{i}",
                    "battle_id": "calvin", "timestamp_epoch": 3 + i,
                })
        after = self.result()
        self.assertEqual(after["rating"], before["rating"])
        for field in ("wins", "first_plan_success_rate", "median_prep_revisions", "distinct_winning_teams"):
            self.assertEqual(after["aggregate"][field], before["aggregate"][field])
        for row in after["raw_runs"]:
            self.assertTrue(row["first_plan_success"])
            self.assertEqual(row["prep_revisions"], 0)
            self.assertEqual(row["winning_team_hash"], aggregate.team_hash("original"))
            self.assertEqual(len(row["team_hashes"]), 4)  # Preserve later experiments in raw history.

    def test_unsolved_runs_keep_all_preparation_costs(self):
        for run in self.runs:
            rows = [{"kind": "semantic", "event": "battle_attempt", "battle_id": "calvin", "timestamp_epoch": 0}]
            (run / "events.jsonl").write_text(json.dumps(rows[0]) + "\n")
        before = self.result()
        self.assertTrue(all(row["prep_revisions"] == 0 for row in before["raw_runs"]))
        for run in self.runs:
            for i in range(4):
                player.append_jsonl(run / "events.jsonl", {
                    "kind": "prep", "status": "applied", "mutation": "roster", "value": f"attempt-{i}",
                    "battle_id": "calvin", "timestamp_epoch": i + 1,
                })
        after = self.result()
        self.assertEqual(after["aggregate"]["wins"], 0)
        self.assertEqual(after["aggregate"]["median_prep_revisions"], 3)
        self.assertTrue(all(row["prep_revisions"] == 3 for row in after["raw_runs"]))


class HarnessCheckpointEvidence(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        config = {"schema_version": 1, "observation_mode": "instrumented", "battle_id": "calvin"}
        for name in ("rom", "runner", "player_context"):
            path = self.root / name
            path.write_bytes(name.encode())
            config[name] = str(path)
        config["run_dir"] = str(self.root / "run")
        config["budgets"] = {"max_actions": 10, "max_frames": 100, "timeout_seconds": 100, "max_deaths": 3}
        path = self.root / "config.json"
        player.atomic_json(path, config)
        self.session = player.Session(path)
        self.session.run_dir.mkdir()
        self.session.state_path.write_bytes(b"old state")
        meta = self.session._base_meta()
        meta["last_probe_values"] = {"battles": 1}
        player.atomic_json(self.session.meta_path, meta)
        self.session.checkpoint("before")
        self.old_meta = copy.deepcopy(meta)
        meta["action_count"] = 7
        meta["frame_count"] = 80
        meta["metrics"]["deaths"] = 2
        meta["metrics"]["battle_attempts"] = 3
        meta["battle_attempts_by_id"] = {"calvin": 3}
        meta["last_probe_values"] = {"battles": 4}
        player.atomic_json(self.session.meta_path, meta)
        self.session.state_path.write_bytes(b"new state")

    def test_restore_rewinds_state_without_refunding_run_budgets(self):
        started = self.session.load_meta()["started_at_epoch"]
        self.session.restore("before")
        self.session.restore("before")
        meta = self.session.load_meta()
        self.assertEqual(self.session.state_path.read_bytes(), b"old state")
        self.assertEqual(meta["action_count"], 7)
        self.assertEqual(meta["frame_count"], 80)
        self.assertEqual(meta["started_at_epoch"], started)
        self.assertEqual(meta["metrics"]["attempts"], 3)
        self.assertEqual(meta["metrics"]["deaths"], 2)
        self.assertEqual(meta["metrics"]["battle_attempts"], 3)
        self.assertEqual(meta["battle_attempts_by_id"], {"calvin": 3})
        self.assertEqual(meta["last_probe_values"], {"battles": 1})
        self.session.record("battle_attempt", "calvin", None, None)
        meta = self.session.load_meta()
        self.assertEqual(meta["battle_attempts_by_id"]["calvin"], 4)
        self.assertEqual(meta["metrics"]["retries"], 1)

    def test_restore_does_not_reopen_exhausted_budgets(self):
        for field, value, message in (("action_count", 10, "action"), ("frame_count", 100, "frame"),
                                      ("started_at_epoch", time.time() - 200, "wall-clock")):
            with self.subTest(field=field):
                meta = self.session.load_meta()
                self.session._budget_check(meta, 1)
                meta[field] = value
                # The start epoch is the run identity, so update both fixtures
                # to model a checkpoint from the same now-expired run.
                checkpoint = self.session.run_dir / "checkpoints/before/metadata.json"
                snapshot = player.load_json(checkpoint)
                if field == "started_at_epoch":
                    snapshot[field] = value
                    player.atomic_json(checkpoint, snapshot)
                player.atomic_json(self.session.meta_path, meta)
                self.session.restore("before")
                with self.assertRaisesRegex(player.HarnessError, message + " budget"):
                    self.session._budget_check(self.session.load_meta(), 1)
                # Restore in-memory test budget values for the next case.
                meta = self.session.load_meta()
                meta["action_count"], meta["frame_count"] = 7, 80
                player.atomic_json(self.session.meta_path, meta)

    def test_foreign_checkpoint_identity_is_rejected_before_state_write(self):
        self.session.restore("before")
        self.session.state_path.write_bytes(b"new state")
        path = self.session.run_dir / "checkpoints/before/metadata.json"
        original = player.load_json(path)
        for field in ("config_sha256", "runner_sha256", "elf_sha256", "player_context_sha256",
                      "arsenal_manifest_sha256", "start_checkpoint_sha256", "started_at_epoch"):
            with self.subTest(field=field):
                snapshot = dict(original)
                snapshot[field] = "foreign"
                player.atomic_json(path, snapshot)
                with self.assertRaisesRegex(player.HarnessError, field):
                    self.session.restore("before")
                self.assertEqual(self.session.state_path.read_bytes(), b"new state")
        player.atomic_json(path, original)

    def test_restore_does_not_refund_death_budget(self):
        meta = self.session.load_meta()
        self.session._budget_check(meta, 1)
        meta["metrics"]["deaths"] = 3
        player.atomic_json(self.session.meta_path, meta)
        self.session.restore("before")
        with self.assertRaisesRegex(player.HarnessError, "death budget"):
            self.session._budget_check(self.session.load_meta(), 1)

    def test_changed_current_config_is_not_hidden_by_checkpoint_restore(self):
        self.session.restore("before")
        self.session.state_path.write_bytes(b"new state")
        self.session.config["seed"] = 50
        with self.assertRaisesRegex(player.HarnessError, "config hash"):
            self.session.restore("before")
        self.assertEqual(self.session.state_path.read_bytes(), b"new state")

    def test_invalid_restore_names_are_rejected(self):
        self.session.restore("before")
        with self.assertRaisesRegex(player.HarnessError, "checkpoint name"):
            self.session.restore("../before")

    def test_protocol_records_real_budget_and_step_settings(self):
        protocol = self.old_meta["comparison_protocol"]
        self.assertEqual(protocol["budgets"]["max_actions"], 10)
        self.assertEqual(protocol["step"], self.session.config["step"])
        self.assertNotIn("seed", protocol)
        self.assertNotIn("rng_delay_frames", protocol)

    def test_init_rejects_mismatched_pair_before_replacing_session_or_running_emulator(self):
        self.session.elf = self.root / "elf"
        self.session.elf.write_bytes(b"elf")
        with patch.object(player, "verify_rom_elf_pair"), patch.object(self.session, "_run_step", side_effect=self.fake_step):
            self.session.init(replace=True)
        previous = self.session.meta_path.read_bytes()
        previous_state = self.session.state_path.read_bytes()
        with patch.object(player, "verify_rom_elf_pair", side_effect=ValueError("mismatched bytes")) as verify, \
                patch.object(self.session, "_run_step") as step:
            with self.assertRaisesRegex(player.HarnessError, "ROM/ELF pair verification failed"):
                self.session.init(replace=True)
            step.assert_not_called()
        self.assertEqual(self.session.meta_path.read_bytes(), previous)
        self.assertEqual(self.session.state_path.read_bytes(), previous_state)

    def fake_step(self, frames, screenshot, *args):
        screenshot.parent.mkdir(parents=True, exist_ok=True)
        screenshot.write_bytes(b"png")
        self.session.state_path.write_bytes(b"initialized")
        return frames, {}

    def test_optional_pair_verification_precedes_emulator_step(self):
        events = []
        def fake_step(frames, screenshot, *args):
            if self.session.elf is not None:
                self.assertIn("verified", events)
            events.append("step")
            return self.fake_step(frames, screenshot, *args)

        for has_elf in (False, True):
            with self.subTest(has_elf=has_elf):
                events.clear()
                self.session.elf = self.root / "elf" if has_elf else None
                if has_elf:
                    self.session.elf.write_bytes(b"elf")
                with patch.object(player, "verify_rom_elf_pair", side_effect=lambda *args: events.append("verified")), \
                        patch.object(self.session, "_run_step", side_effect=fake_step) as step:
                    self.session.init(replace=True)
                    self.assertIn("step", events)
                    self.assertEqual(self.session.state_path.read_bytes(), b"initialized")


if __name__ == "__main__":
    unittest.main()
