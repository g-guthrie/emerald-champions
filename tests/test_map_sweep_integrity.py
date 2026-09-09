"""Map-sweep evidence must fail closed without swallowing interrupts."""
import contextlib
import importlib.util
import io
import hashlib
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location("map_sweep_render", ROOT / "scripts/audit/map_sweep_render.py")
sweep = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(sweep)


class MapSweepIntegrity(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.out = Path(self.temp.name)
        self.rom = self.out / "fixture.gba"
        self.elf = self.out / "fixture.elf"
        self.rom.write_bytes(b"rom")
        self.elf.write_bytes(b"elf")
        self.stamp = self.out / "fixture.inputs.json"
        self.stamp.write_text(json.dumps({
            "schema_version": 2, "inputs_sha256": "current-source", "input_count": 12,
            "artifacts": {p.name: hashlib.sha256(p.read_bytes()).hexdigest()
                          for p in (self.rom, self.elf)}}))
        self.stack = contextlib.ExitStack()
        self.addCleanup(self.stack.close)
        self.stack.enter_context(contextlib.redirect_stderr(io.StringIO()))
        self.stack.enter_context(patch.object(sweep, "load_census", return_value=(
            ["First", "Second"], [("MAP_FIRST", "1", "2"), ("MAP_SECOND", "3", "140")])) )
        self.digest = self.stack.enter_context(patch.object(sweep, "digest_tree", return_value=("current-source", 12)))
        self.pair = self.stack.enter_context(patch.object(sweep, "verify_rom_elf_pair"))
        self.stack.enter_context(patch.object(sweep.R, "build_runner", return_value=Path("runner")))
        self.stack.enter_context(patch.object(sweep.R, "resolve_symbol", return_value=123))
        self.render = self.stack.enter_context(patch.object(sweep.R, "render_one", return_value={"verified_runtime_state": True}))

    def run_sweep(self, *selection):
        return sweep.main([str(self.out), *map(str, selection), "--rom", str(self.rom), "--elf", str(self.elf), "--stamp", str(self.stamp)])

    def report(self):
        return json.loads((self.out / "map_sweep_report.json").read_text())

    def test_success_has_bound_artifacts_and_all_outcomes(self):
        self.assertEqual(self.run_sweep(), 0)
        report = self.report()
        self.assertEqual(report["status"], "passed")
        self.assertEqual(len(report["artifacts"]["rom"]["sha256"]), 64)
        self.assertTrue(report["artifacts"]["pair_verified"])
        self.assertEqual(len(report["outcomes"]), 2)
        self.assertEqual(report["outcomes"][1]["expected_position"], [3, 140])
        self.assertTrue(self.render.call_args.args[1]["verify"])

    def test_render_failure_continues_but_returns_failure(self):
        self.render.side_effect = [RuntimeError("bad image"), {"verified_runtime_state": True}]
        self.assertEqual(self.run_sweep(), 1)
        self.assertEqual(self.render.call_count, 2)
        self.assertEqual(self.report()["failed_count"], 1)
        self.assertIn("bad image", self.report()["outcomes"][0]["error"])

    def test_unverified_result_is_failure(self):
        self.render.return_value = {"verified_runtime_state": False}
        self.assertEqual(self.run_sweep(0, 1), 1)

    def test_interrupt_propagates_and_report_does_not_pass(self):
        self.render.side_effect = KeyboardInterrupt()
        with self.assertRaises(KeyboardInterrupt):
            self.run_sweep()
        self.assertEqual(self.render.call_count, 1)
        self.assertEqual(self.report()["status"], "failed")
        self.assertEqual(self.report()["outcomes"][0]["status"], "failed")

    def test_bad_selection_never_renders(self):
        for selection in ((-1,), (0, 0), (1, 0), (0, 3), (2,)):
            with self.subTest(selection=selection):
                self.assertEqual(self.run_sweep(*selection), 1)
        self.render.assert_not_called()
        self.pair.assert_not_called()

    def test_stale_source_stamp_never_renders(self):
        self.digest.return_value = ("changed-census-or-source", 12)
        self.assertEqual(self.run_sweep(), 1)
        self.render.assert_not_called()
        self.assertIn("does not match this source tree", self.report()["error"])

    def test_artifact_mutation_during_sweep_fails(self):
        def render(*args, **kwargs):
            self.rom.write_bytes(b"changed ROM")
            return {"verified_runtime_state": True}
        self.render.side_effect = render
        self.assertEqual(self.run_sweep(), 1)
        self.assertIn("artifact bytes differ", self.report()["error"])
        self.assertEqual(self.report()["status"], "failed")

    def test_source_mutation_during_sweep_fails(self):
        self.digest.side_effect = [("current-source", 12), ("changed-source", 12)]
        self.assertEqual(self.run_sweep(), 1)
        self.assertIn("does not match this source tree", self.report()["error"])

    def test_mismatched_pair_never_renders(self):
        self.pair.side_effect = ValueError("ROM bytes do not match the supplied ELF")
        self.assertEqual(self.run_sweep(), 1)
        self.render.assert_not_called()
        self.assertIn("do not match", self.report()["error"])


if __name__ == "__main__":
    unittest.main()
