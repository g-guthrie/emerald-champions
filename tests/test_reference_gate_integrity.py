"""Small synthetic references exercise failure handling, not campaign correctness."""
from __future__ import annotations

import contextlib
import importlib.util
import io
import json
import struct
import subprocess
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[1]


def load_gate(name):
    spec = importlib.util.spec_from_file_location(name, ROOT / "scripts" / f"{name}.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


story = load_gate("verify_inclement_story_parity")
maps = load_gate("verify_map_reachability")


class ReferenceGateIntegrity(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.map_path = "data/maps/Test/map.json"
        self.script_path = "data/maps/Test/scripts.inc"
        self.block_path = "data/layouts/Test/map.bin"
        self.layout_path = "data/layouts/layouts.json"
        self.reference = {
            self.map_path: json.dumps({
                "layout": "LAYOUT_TEST", "object_events": [],
                "warp_events": [{"x": 1, "y": 1}],
            }).encode(),
            self.script_path: b"map_script MAP_SCRIPT_ON_LOAD, Test_Load\nsetflag FLAG_TEST\n",
            self.layout_path: json.dumps({"layouts": [{
                "id": "LAYOUT_TEST", "width": 3, "height": 3,
                "blockdata_filepath": self.block_path,
            }]}).encode(),
            self.block_path: struct.pack("<9H", *([0] * 9)),
        }
        for path, raw in self.reference.items():
            self.write(path, raw)
        # Every negative starts with the same complete, accepted fixture.
        self.run_gate(story)
        self.run_gate(maps)

    def write(self, path, raw):
        target = self.root / path
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(raw)

    def git(self, argv, **kwargs):
        if argv[1] == "rev-parse":
            result = story.BASELINE.encode() + b"\n"
        elif argv[1] == "ls-tree":
            result = "\0".join(self.reference).encode() + b"\0"
        elif argv[1] == "show":
            result = self.reference[argv[2].split(":", 1)[1]]
        else:
            self.fail(f"unexpected git command: {argv}")
        return result.decode() if kwargs.get("text") else result

    def run_gate(self, gate, *, git=None, expected=0):
        output = io.StringIO()
        with patch.object(gate, "ROOT", self.root), \
                patch.object(gate.subprocess, "check_output", side_effect=git or self.git), \
                contextlib.redirect_stdout(output):
            if expected:
                with self.assertRaises(SystemExit) as raised:
                    gate.main()
                self.assertEqual(raised.exception.code, expected)
            else:
                gate.main()
        if expected:
            self.assertIn("FAIL", output.getvalue())
            self.assertNotIn(": PASS", output.getvalue())
        return output.getvalue()

    def test_missing_reference_cannot_pass(self):
        def missing(argv, **kwargs):
            raise subprocess.CalledProcessError(128, argv, stderr=b"bad revision")
        for gate in (story, maps):
            with self.subTest(gate=gate.__name__):
                self.run_gate(gate, git=missing, expected=1)

    def test_missing_git_cannot_pass(self):
        def missing(argv, **kwargs):
            raise FileNotFoundError("git is unavailable")
        for gate in (story, maps):
            with self.subTest(gate=gate.__name__):
                self.run_gate(gate, git=missing, expected=1)

    def test_tree_enumeration_failure_cannot_pass(self):
        def fail_tree(argv, **kwargs):
            if argv[1] == "ls-tree":
                raise subprocess.CalledProcessError(128, argv)
            return self.git(argv, **kwargs)
        for gate in (story, maps):
            with self.subTest(gate=gate.__name__):
                self.run_gate(gate, git=fail_tree, expected=1)

    def test_existing_reference_file_read_failure_cannot_be_skipped(self):
        def fail_blob(argv, **kwargs):
            if argv[1] == "show" and argv[2].endswith(":" + self.map_path):
                raise subprocess.CalledProcessError(128, argv)
            return self.git(argv, **kwargs)
        for gate in (story, maps):
            with self.subTest(gate=gate.__name__):
                self.run_gate(gate, git=fail_blob, expected=1)

    def test_absent_path_is_distinct_from_unreadable_path(self):
        with patch.object(subprocess, "check_output", side_effect=self.git):
            self.assertIsNotNone(story.show(self.map_path, set(self.reference)))
            self.assertIsNotNone(maps.show(maps.BASELINE, self.map_path, set(self.reference)))
            self.assertIsNone(story.show("new/path", set(self.reference)))
            self.assertIsNone(maps.show(maps.BASELINE, "new/path", set(self.reference)))

    def test_story_counts_observed_comparisons_and_exceptions(self):
        output = self.run_gate(story)
        self.assertIn("1 map object comparisons; 1 callback comparisons", output)
        self.assertIn("0 observed reviewed divergences", output)
        self.assertNotIn("22 reviewed", output)

    def test_story_includes_deleted_reference_script_setters(self):
        self.reference["data/scripts/removed.inc"] = b"setflag FLAG_REMOVED_SETTER\n"
        self.write(self.script_path, self.reference[self.script_path] + b"checkflag FLAG_REMOVED_SETTER\n")
        output = self.run_gate(story, expected=1)
        self.assertIn("FLAG_REMOVED_SETTER remains referenced", output)

    def test_story_refuses_empty_current_map_inventory(self):
        (self.root / self.map_path).unlink()
        self.run_gate(story, expected=1)

    def test_story_refuses_reference_without_scripts(self):
        del self.reference[self.script_path]
        self.run_gate(story, expected=1)

    def test_map_counts_real_pairs_and_states_scope(self):
        output = self.run_gate(maps)
        self.assertIn("1 map pairs compared; 0 new maps", output)
        self.assertIn("1 current and 1 reference warps inspected", output)
        self.assertIn("campaign reachability not verified", output)

    def test_map_missing_reference_layout_index_cannot_pass(self):
        del self.reference[self.layout_path]
        self.run_gate(maps, expected=1)

    def test_map_missing_reference_layout_id_cannot_pass(self):
        self.reference[self.map_path] = b'{"layout": "LAYOUT_MISSING", "warp_events": []}'
        output = self.run_gate(maps, expected=1)
        self.assertIn("references missing layout LAYOUT_MISSING", output)

    def test_map_missing_reference_collision_cannot_pass_when_current_has_no_defects(self):
        del self.reference[self.block_path]
        output = self.run_gate(maps, expected=1)
        self.assertIn("missing blockdata", output)

    def test_map_invalid_blockdata_size_cannot_pass(self):
        for raw in (b"\0\0", self.reference[self.block_path] + b"\0\0"):
            with self.subTest(size=len(raw)):
                self.write(self.block_path, self.reference[self.block_path])
                self.run_gate(maps)
                self.write(self.block_path, raw)
                self.assertIn("blockdata size mismatch", self.run_gate(maps, expected=1))

    def test_map_unmodeled_warps_are_counted(self):
        self.write(self.block_path, struct.pack("<9H", *([1 << 10] * 9)))
        output = self.run_gate(maps)
        self.assertIn("1 current and 0 reference warps unmodeled", output)

    def test_recognized_border_warps_are_reported_as_unmodeled(self):
        # This checks honest coverage for the heuristic's recognized examples,
        # not that width+1 is a universal bound enforced by the game engine.
        for x, y in ((3, 1), (4, 1), (1, 3), (1, 4)):
            with self.subTest(x=x, y=y):
                self.write(self.map_path, self.reference[self.map_path])
                self.run_gate(maps)
                payload = json.loads(self.reference[self.map_path])
                payload["warp_events"] = [{"x": x, "y": y}]
                self.write(self.map_path, json.dumps(payload).encode())
                output = self.run_gate(maps)
                self.assertIn("1 current and 0 reference warps unmodeled", output)

    def blocking_objects(self):
        return [{"x": x, "y": y, "flag": "0"} for x, y in ((0, 1), (1, 0), (1, 2), (2, 1))]

    def test_newly_blocked_warp_is_rejected(self):
        payload = json.loads(self.reference[self.map_path])
        payload["object_events"] = self.blocking_objects()
        self.write(self.map_path, json.dumps(payload).encode())
        self.run_gate(maps, expected=1)

    def test_preexisting_blockage_is_not_a_new_regression(self):
        payload = json.loads(self.reference[self.map_path])
        payload["object_events"] = self.blocking_objects()
        self.reference[self.map_path] = json.dumps(payload).encode()
        self.write(self.map_path, self.reference[self.map_path])
        self.run_gate(maps)  # Same blocked warp is present in both revisions.
        payload["warp_events"].append({"x": 0, "y": 0})
        self.write(self.map_path, json.dumps(payload).encode())
        self.run_gate(maps, expected=1)  # Newly blocked second warp must still fail.

    def test_new_map_collision_findings_are_not_skipped(self):
        path = "data/maps/New/map.json"
        payload = json.loads(self.reference[self.map_path])
        self.write(path, json.dumps(payload).encode())
        self.run_gate(maps)
        payload["object_events"] = self.blocking_objects()
        self.write(path, json.dumps(payload).encode())
        self.run_gate(maps, expected=1)

    def test_new_single_tile_approach_pocket_is_rejected(self):
        blocks = [1 << 10] * 9
        blocks[7] = 0  # Only the tile south of the warp is collision-free.
        self.write(self.block_path, struct.pack("<9H", *blocks))
        self.run_gate(maps, expected=1)

    def test_flagged_objects_are_not_treated_as_permanent_blockers(self):
        payload = json.loads(self.reference[self.map_path])
        objects = self.blocking_objects()
        for obj in objects:
            obj["flag"] = "FLAG_STORY"
        payload["object_events"] = objects
        self.write(self.map_path, json.dumps(payload).encode())
        self.run_gate(maps)
        for obj in objects:
            obj["flag"] = "0"
        self.write(self.map_path, json.dumps(payload).encode())
        self.run_gate(maps, expected=1)

    def rename_map(self, name):
        (self.root / "data/maps/Test").rename(self.root / "data/maps" / name)
        self.reference = {key.replace("data/maps/Test/", f"data/maps/{name}/"): value
                          for key, value in self.reference.items()}
        self.map_path = f"data/maps/{name}/map.json"
        self.script_path = f"data/maps/{name}/scripts.inc"

    def story_object(self, graphics="OBJ_EVENT_GFX_TEST"):
        return {"graphics_id": graphics, "script": "Test_Actor", "flag": "FLAG_STORY", "x": 0, "y": 0}

    def test_newly_permanent_story_object_is_rejected(self):
        payload = json.loads(self.reference[self.map_path])
        payload["object_events"] = [self.story_object()]
        self.reference[self.map_path] = json.dumps(payload).encode()
        self.write(self.map_path, self.reference[self.map_path])
        self.run_gate(story)
        payload["object_events"][0]["flag"] = "0"
        self.write(self.map_path, json.dumps(payload).encode())
        self.run_gate(story, expected=1)

    def test_reviewed_permanent_object_is_allowed_but_not_a_different_actor(self):
        self.rename_map("FortreeCity_Mart")
        payload = json.loads(self.reference[self.map_path])
        payload["object_events"] = [self.story_object("OBJ_EVENT_GFX_SPENSER"), self.story_object()]
        self.reference[self.map_path] = json.dumps(payload).encode()
        self.write(self.map_path, self.reference[self.map_path])
        self.run_gate(story)
        payload["object_events"][0]["flag"] = "0"
        self.write(self.map_path, json.dumps(payload).encode())
        self.run_gate(story)  # The existing Spenser exception applies.
        payload["object_events"][1]["flag"] = "0"
        self.write(self.map_path, json.dumps(payload).encode())
        self.run_gate(story, expected=1)  # It cannot excuse the other actor.

    def test_removed_callback_is_rejected(self):
        self.write(self.script_path, b"setflag FLAG_TEST\n")
        self.run_gate(story, expected=1)

    def test_reviewed_callback_removal_is_allowed_without_hiding_other_maps(self):
        self.rename_map("MossdeepCity_House1")
        self.run_gate(story)
        self.write(self.script_path, b"setflag FLAG_TEST\n")
        self.run_gate(story)
        other = "data/maps/Other/scripts.inc"
        self.reference[other] = b"map_script MAP_SCRIPT_ON_LOAD, Other_Load\n"
        self.write(other, self.reference[other])
        self.run_gate(story)
        self.write(other, b"Other_Load:\nreturn\n")
        self.run_gate(story, expected=1)

    def test_reviewed_dead_gate_is_allowed_without_hiding_an_unreviewed_flag(self):
        path = "data/scripts/reviewed.inc"
        self.reference[path] = b"setflag FLAG_DEFEATED_DEOXYS\nsetflag FLAG_OTHER_STORY\n"
        self.write(path, self.reference[path])
        self.run_gate(story)
        self.write(path, b"checkflag FLAG_DEFEATED_DEOXYS\nsetflag FLAG_OTHER_STORY\n")
        self.run_gate(story)
        self.write(path, b"checkflag FLAG_DEFEATED_DEOXYS\ncheckflag FLAG_OTHER_STORY\n")
        self.run_gate(story, expected=1)

    def test_new_maps_are_not_reported_as_historical_comparisons(self):
        self.write("data/maps/New/map.json", self.reference[self.map_path])
        output = self.run_gate(maps)
        self.assertIn("1 map pairs compared; 1 new maps checked", output)
        output = self.run_gate(story)
        self.assertIn("1 map object comparisons", output)
        self.assertIn("1 new maps", output)

    def test_map_refuses_zero_historical_comparisons(self):
        del self.reference[self.map_path]
        self.run_gate(maps, expected=1)


if __name__ == "__main__":
    unittest.main()
