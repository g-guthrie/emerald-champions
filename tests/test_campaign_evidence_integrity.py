"""Campaign evidence regressions; no emulator or game build is required.

Policy tests compile the actual production decision function on the host.
Other tests use temporary evidence artifacts and in-memory corruptions.
"""

import copy
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import run_emerald_champions_campaign as campaign
import verify_emerald_champions_campaign_battle_policy as policy


def artifact(digest, finalized=True):
    result = {
        "snapshot_sha256": digest, "snapshot_size": 12,
        "source_sha256_before": digest, "source_sha256_after_copy": digest,
    }
    if finalized:
        result.update(snapshot_sha256_after_run=digest, source_sha256_after_run=digest,
                      verified_immutable=True)
    return result


class CheckpointEvidenceTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix="ec-checkpoint-evidence-")
        self.addCleanup(self.temporary.cleanup)
        self.directory = Path(self.temporary.name)
        self.state = self.directory / "parent.ss1"
        self.state.write_bytes(b"actual checkpoint bytes")
        self.artifacts = {label: artifact(char * 64, finalized=False) for label, char in (("rom", "a"), ("elf", "b"), ("manifest", "c"))}
        self.artifacts["manifest"] = self.manifest_artifact("manifest.json", [{"id": "parent", "parent": None, "frames": 10}])
        self.metadata = {
            "schema_version": 1, "segment": "parent", "parent": None,
            "rom_sha256": "a" * 64,
            "state_sha256": campaign.sha256(self.state),
            "artifact_evidence": copy.deepcopy(self.artifacts),
        }
        self.persist()

    def persist(self):
        campaign.state_metadata_path(self.state).write_text(json.dumps(self.metadata))

    def validate(self):
        campaign.validate_state_input(self.state, "a" * 64, expected_segment="parent", artifact_evidence=self.artifacts)

    def manifest_artifact(self, name, segments):
        path = self.directory / name
        path.write_text(json.dumps({"schema_version": 1, "segments": segments}))
        evidence = artifact(campaign.sha256(path), finalized=False)
        evidence["snapshot"] = str(path)
        return evidence

    def test_valid_in_progress_and_finalized_metadata(self):
        self.validate()
        self.metadata["artifact_evidence"] = {label: {**row, **artifact(row["snapshot_sha256"])} for label, row in self.artifacts.items()}
        self.persist()
        self.validate()

    def test_same_manifest_wrong_parent_lineage_is_rejected(self):
        self.metadata["parent"] = "unrelated-earlier-segment"
        self.persist()
        with self.assertRaisesRegex(RuntimeError, "recorded parent disagrees"):
            self.validate()

    def test_missing_parent_lineage_is_rejected(self):
        del self.metadata["parent"]
        self.persist()
        with self.assertRaisesRegex(RuntimeError, "lacks recorded parent"):
            self.validate()

    def test_modified_checkpoint_bytes_rejected(self):
        self.state.write_bytes(b"replaced by a same-ROM checkpoint from later in the story")
        with self.assertRaisesRegex(RuntimeError, "recorded state hash"):
            self.validate()

    def test_missing_or_malformed_hash_and_wrong_segment_rejected(self):
        original = copy.deepcopy(self.metadata)
        for key, value in (("state_sha256", None), ("state_sha256", "g" * 64),
                           ("segment", "unrelated"), ("rom_sha256", "f" * 64)):
            with self.subTest(key=key, value=value):
                self.metadata = copy.deepcopy(original)
                self.metadata[key] = value
                self.persist()
                with self.assertRaises(RuntimeError):
                    self.validate()

    def test_missing_or_inconsistent_artifact_provenance_rejected(self):
        original = copy.deepcopy(self.metadata)
        cases = [None, {}, {"rom": artifact("a" * 64)}]
        for label in ("rom", "elf", "manifest"):
            invalid = copy.deepcopy(self.artifacts)
            invalid[label]["source_sha256_after_copy"] = "9" * 64
            cases.append(invalid)
        for evidence in cases:
            with self.subTest(evidence=evidence):
                self.metadata = copy.deepcopy(original)
                self.metadata["artifact_evidence"] = evidence
                self.persist()
                with self.assertRaises(RuntimeError):
                    self.validate()

    def test_different_elf_rejected_even_with_same_rom(self):
        self.artifacts["elf"] = artifact("8" * 64)
        with self.assertRaisesRegex(RuntimeError, "different elf"):
            self.validate()

    def test_changed_manifest_with_identical_ancestry_is_compatible(self):
        chain = [{"id": "root", "parent": None, "frames": 10}, {"id": "parent", "parent": "root", "frames": 20}]
        original = self.manifest_artifact("old.json", chain)
        extended = self.manifest_artifact("new.json", chain + [{"id": "future", "parent": "parent", "frames": 50}])
        self.metadata["artifact_evidence"]["manifest"] = original
        self.metadata["parent"] = "root"
        self.artifacts["manifest"] = extended
        self.persist()
        self.validate()

    def test_changed_ancestor_action_is_not_compatible(self):
        chain = [{"id": "root", "parent": None, "frames": 10}, {"id": "parent", "parent": "root", "frames": 20}]
        original = self.manifest_artifact("old.json", chain)
        chain[0]["actions"] = [{"at": 1, "duration": 2, "keys": ["A"]}]
        self.metadata["artifact_evidence"]["manifest"] = original
        self.artifacts["manifest"] = self.manifest_artifact("new.json", chain)
        self.persist()
        with self.assertRaisesRegex(RuntimeError, "ancestry changed"):
            self.validate()

    def test_missing_or_tampered_original_manifest_cannot_prove_ancestry(self):
        self.artifacts["manifest"] = artifact("8" * 64)
        with self.assertRaisesRegex(RuntimeError, "cannot verify ancestry"):
            self.validate()
        original = self.manifest_artifact("old.json", [{"id": "parent", "frames": 10}])
        self.metadata["artifact_evidence"]["manifest"] = original
        self.persist()
        Path(original["snapshot"]).write_text("tampered")
        with self.assertRaisesRegex(RuntimeError, "missing or changed"):
            self.validate()

    def test_missing_checkpoint_and_sidecar_fail_closed(self):
        campaign.state_metadata_path(self.state).unlink()
        with self.assertRaisesRegex(RuntimeError, "metadata is missing"):
            self.validate()
        self.state.unlink()
        with self.assertRaisesRegex(RuntimeError, "checkpoint is missing"):
            self.validate()

    def test_explicit_parent_run_overrides_old_local_file_without_fallback(self):
        out = self.directory
        run_out = out / "runs" / "destination"
        local = run_out / "checkpoints" / "parent.ss1"
        local.parent.mkdir(parents=True)
        local.write_bytes(b"old local checkpoint")
        selected = campaign.select_parent_checkpoint("parent", completed_segments=set(), run_out=run_out, out=out, parent_run_id="requested")
        self.assertEqual(selected, out / "runs/requested/checkpoints/parent.ss1")
        self.assertFalse(selected.exists())
        with self.assertRaisesRegex(RuntimeError, "checkpoint is missing"):
            campaign.validate_state_input(selected, "a" * 64)

    def test_freshly_completed_parent_takes_precedence_for_chain_continuity(self):
        selected = campaign.select_parent_checkpoint("parent", completed_segments={"parent"}, run_out=self.directory, out=self.directory, parent_run_id="requested")
        self.assertEqual(selected, self.directory / "checkpoints/parent.ss1")


class ProductionPolicyTests(unittest.TestCase):
    def test_actual_c_classifier_passes_declared_policy_cases(self):
        result = policy.audit()
        self.assertEqual(result["failures"], [])
        self.assertEqual(len(result["flows"]), 29)
        self.assertEqual(result["evidence"]["mode"], "actual-production-C-host-execution")
        self.assertRegex(result["evidence"]["classifier_sha256"], r"^[0-9a-f]{64}$")

    def test_return_value_mutation_fails_even_when_every_old_token_remains(self):
        original = Path.read_text
        source_path = ROOT / "src/emerald_champions_headless.c"
        def changed_read(path, *args, **kwargs):
            value = original(path, *args, **kwargs)
            if path == source_path:
                value = value.replace("return EC_HEADLESS_BATTLE_WIN;", "return EC_HEADLESS_BATTLE_NATIVE;")
            return value
        with patch.object(Path, "read_text", new=changed_read):
            result = policy.audit()
        self.assertTrue(any("trainer-single" in error for error in result["failures"]))
        self.assertTrue(any("ordinary-established-party" in error for error in result["failures"]))

class CampaignPairBoundaryTests(unittest.TestCase):
    def test_init_rejects_mismatched_pair_before_building_runner_or_playing(self):
        with tempfile.TemporaryDirectory(prefix="ec-campaign-pair-boundary-") as raw:
            directory = Path(raw)
            manifest = directory / "manifest.json"
            manifest.write_text(json.dumps({"schema_version": 1, "segments": [{"id": "root", "frames": 10}]}))
            rom, elf = directory / "game.gba", directory / "game.elf"
            rom.write_bytes(b"ROM build one")
            elf.write_bytes(b"ELF build two")
            with patch.object(sys, "argv", ["campaign", "--manifest", str(manifest), "--rom", str(rom), "--elf", str(elf), "--out", str(directory / "out")]), \
                 patch.object(campaign.ui, "require_resident_file", side_effect=lambda path, label: path), \
                 patch.object(campaign.capture_paths, "audit", return_value={"failures": []}), \
                 patch.object(campaign.prerequisites, "audit", return_value={"failures": []}), \
                 patch.object(campaign, "parse_map_ids", return_value={}), \
                 patch.object(campaign, "parse_numeric_constants", return_value={}), \
                 patch.object(campaign, "verify_rom_elf_pair", side_effect=ValueError("different build")) as verify, \
                 patch.object(campaign.ui, "build_runner") as build:
                with self.assertRaisesRegex(RuntimeError, "ROM/ELF correspondence failed"):
                    campaign.main()
                verify.assert_called_once_with(
                    directory / "out/artifacts" / f"rom-{campaign.sha256(rom)}.gba",
                    directory / "out/artifacts" / f"elf-{campaign.sha256(elf)}.elf",
                )
                build.assert_not_called()


if __name__ == "__main__":
    unittest.main()
