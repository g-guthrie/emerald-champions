"""Counterexamples for source/artifact identity and prerequisite exit handling."""

import hashlib
import importlib.util
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location("stamp_release_inputs", ROOT / "scripts/stamp_release_inputs.py")
stamp = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(stamp)
RELEASE_SPEC = importlib.util.spec_from_file_location("verify_release", ROOT / "scripts/verify_emerald_champions_release.py")
release = importlib.util.module_from_spec(RELEASE_SPEC)
RELEASE_SPEC.loader.exec_module(release)

# Independent consumer requirements: do not expand the producer's own list here.
REQUIRED_GENERATOR_INPUTS = (
    "data/emerald_champions/emerald_champions_move_access_review.json",
    "data/emerald_champions/emerald_champions_preparation_form_learnsets.json",
    "scripts/stamp_release_inputs.py",
)


class StampIntegrityTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.artifacts = stamp.artifacts_for_stamp(self.root / "pokeemerald-release.inputs.json")
        for artifact in self.artifacts:
            artifact.write_bytes(artifact.suffix.encode())
        self.record = {
            "schema_version": stamp.SCHEMA_VERSION,
            "inputs_sha256": "a" * 64,
            "input_count": 1,
            "artifacts": {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in self.artifacts},
        }
        self.verify()  # Every artifact mutation below starts from accepted evidence.

    def verify(self):
        stamp.verify_stamp(self.record, "a" * 64, 1, self.artifacts)

    def test_valid_artifact_hash_bindings(self):
        self.verify()

    def test_unsupported_stamp_schema_is_rejected(self):
        self.record["schema_version"] = -1
        with self.assertRaises(ValueError):
            self.verify()

    def test_current_schema_without_artifact_hashes_is_rejected(self):
        del self.record["artifacts"]
        with self.assertRaises(ValueError):
            self.verify()

    def test_release_requires_both_artifact_hashes(self):
        del self.record["artifacts"][self.artifacts[1].name]
        with self.assertRaisesRegex(ValueError, "every expected artifact"):
            self.verify()

    def test_replaced_rom_is_rejected(self):
        self.artifacts[0].write_bytes(b"stale ROM")
        with self.assertRaisesRegex(ValueError, "bytes differ"):
            self.verify()

    def test_replaced_elf_is_rejected(self):
        self.artifacts[1].write_bytes(b"different ELF")
        with self.assertRaisesRegex(ValueError, "bytes differ"):
            self.verify()

    def test_missing_elf_is_rejected(self):
        self.artifacts[1].unlink()
        with self.assertRaisesRegex(ValueError, "missing stamped artifact"):
            self.verify()

    def test_different_supplied_source_digest_is_rejected(self):
        self.record["inputs_sha256"] = "b" * 64
        with self.assertRaisesRegex(ValueError, "source tree"):
            self.verify()

    def test_custom_test_stamp_binds_elf_beside_stamp(self):
        paths = stamp.artifacts_for_stamp(self.root / "pokeemerald-test-all-ai.inputs.json")
        self.assertEqual(paths, (self.root / "pokeemerald-test-all-ai.elf",))

    def test_build_inputs_cover_previous_omissions(self):
        inputs = {p.relative_to(ROOT).as_posix() for p in stamp.build_inputs(include_tests=True)}
        for relative in (
            *REQUIRED_GENERATOR_INPUTS,
            "tools/learnset_helpers/make_teachables.py",
            "tools/learnset_helpers/porymoves_files/rse.json",
            "tools/wild_encounters/wild_encounters_to_header.py",
            "tools/compresSmol/compressAlgo.cpp",
            "tools/mgba-rom-test-hydra/main.c",
            ".gitignore",
            "data/mb_berry_fix.gba",
            "ld_script_modern.ld", "ld_script_test.ld",
        ):
            with self.subTest(relative=relative):
                self.assertIn(relative, inputs)

    def source_fixture(self):
        for directory in ("src", "data", "include", "asm", "graphics", "sound", "libagbsyscall",
                          "test", "tools/stub", "tools/learnset_helpers", "tools/wild_encounters", "tools/misc"):
            (self.root / directory).mkdir(parents=True, exist_ok=True)
        files = {
            "src/main.c": "int game_state;\n",
            "include/game.h": "extern int game_state;\n",
            "ld_script_modern.ld": "SECTIONS { .ewram : { *(.ewram*) } }\n",
            "ld_script_test.ld": "SECTIONS { .text : { *(.text*) } }\n",
            "tools/stub/main.c": "int generator_state;\n",
            "make_tools.mk": "TOOL_NAMES := stub\nCHECK_TOOL_NAMES := stub\n",
            "Makefile": "all:\n\t@:\n",
            ".gitignore": "",
            "scripts/run_emerald_champions_runtime_gates.py": "# test selection\n",
            **{name: "{}\n" for name in REQUIRED_GENERATOR_INPUTS},
        }
        for name, content in files.items():
            path = self.root / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text(content)

    def test_actual_input_bytes_change_the_digest_and_invalidate_the_stamp(self):
        self.source_fixture()
        with patch.object(stamp, "ROOT", self.root):
            for relative in ("src/main.c", "include/game.h", "tools/stub/main.c", "ld_script_modern.ld",
                             "ld_script_test.ld", *REQUIRED_GENERATOR_INPUTS):
                with self.subTest(relative=relative):
                    digest, count = stamp.digest_tree()
                    self.record.update(inputs_sha256=digest, input_count=count)
                    stamp.verify_stamp(self.record, digest, count, self.artifacts)
                    path = self.root / relative
                    original = path.read_bytes()
                    path.write_bytes(original + b"changed\n")
                    changed_digest, changed_count = stamp.digest_tree()
                    self.assertNotEqual(changed_digest, digest)
                    self.assertEqual(changed_count, count)
                    with self.assertRaises(ValueError):
                        stamp.verify_stamp(self.record, changed_digest, changed_count, self.artifacts)
                    path.write_bytes(original)

    def test_missing_direct_generator_inputs_are_rejected(self):
        self.source_fixture()
        with patch.object(stamp, "ROOT", self.root):
            for relative in REQUIRED_GENERATOR_INPUTS:
                with self.subTest(relative=relative):
                    stamp.digest_tree()  # Same complete source tree passes first.
                    path = self.root / relative
                    original = path.read_bytes()
                    path.unlink()
                    with self.assertRaises(ValueError):
                        stamp.build_inputs()
                    path.write_bytes(original)

    def test_missing_required_source_directory_is_rejected(self):
        self.source_fixture()
        with patch.object(stamp, "ROOT", self.root):
            stamp.digest_tree()
            (self.root / "src").rename(self.root / "saved-src")
            with self.assertRaises(ValueError):
                stamp.build_inputs()


class PrerequisiteIntegrityTests(unittest.TestCase):
    @staticmethod
    def conditional_block(source, start):
        depth = 0
        lines = []
        for line in source[start:].splitlines():
            directive = line.strip().split(maxsplit=1)[0] if line.strip() else ""
            if directive in {"ifeq", "ifneq", "ifdef", "ifndef"}:
                depth += 1
            elif directive == "endif":
                depth -= 1
            lines.append(line)
            if depth == 0:
                return "\n".join(lines) + "\n"
        raise AssertionError("unterminated Makefile conditional")

    def run_prerequisite(self, tools_status=0, generated_status=0):
        # Exercise the actual two Makefile blocks under the installed make,
        # including macOS GNU Make 3.81, without running project generation.
        makefile = (ROOT / "Makefile").read_text()
        start = makefile.index("ifeq ($(SETUP_PREREQS),1)")
        block = self.conditional_block(makefile, start)
        for command, label, status in (("$(MAKE) -f make_tools.mk", "tools", tools_status),
                                       ("$(MAKE) MAP_VERSION=$(MAP_VERSION) generated", "generated", generated_status)):
            self.assertIn(command, block)
            block = block.replace(command, f"sh -c 'echo {label}-ran; exit {status}'")
        # Import the actual shell configuration; never silently provide pipefail
        # on behalf of a Makefile that no longer enables it.
        shell = "\n".join(re.findall(r"(?m)^SHELL\s*[:?+]?=.*$", makefile))
        script = shell + "\nSETUP_PREREQS := 1\n" + block + "\nall:\n\t@:\n"
        return subprocess.run(["make", "-f", "-"], input=script, text=True, capture_output=True, timeout=10)

    def test_failed_submake_cannot_hide_behind_successful_sed(self):
        baseline = self.run_prerequisite()
        self.assertEqual(baseline.returncode, 0, baseline.stderr)
        result = self.run_prerequisite(tools_status=1)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("tools-ran", result.stdout)

    def test_failed_generation_cannot_hide_after_successful_tools(self):
        baseline = self.run_prerequisite()
        self.assertEqual(baseline.returncode, 0, baseline.stderr)
        result = self.run_prerequisite(generated_status=1)
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("tools-ran", result.stdout)
        self.assertIn("generated-ran", result.stdout)

    def test_successful_prerequisites_remain_accepted(self):
        result = self.run_prerequisite()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("tools-ran", result.stdout)
        self.assertIn("generated-ran", result.stdout)

    def test_only_zero_fixture_setting_can_build_release(self):
        source = (ROOT / "Makefile").read_text()
        marker = "$(error EC_HEADLESS_FIXTURES must remain disabled for release builds)"
        start = source.rfind("ifeq ($(RELEASE),1)", 0, source.index(marker))
        guard = self.conditional_block(source, start)
        def evaluate(release_mode, value):
            script = f"RELEASE := {release_mode}\nEC_HEADLESS_FIXTURES := {value}\n" + guard + "all:\n\t@:\n"
            return subprocess.run(["make", "-n", "-f", "-"], input=script,
                                  text=True, capture_output=True, timeout=10)
        baseline = evaluate(1, "0")
        self.assertEqual(baseline.returncode, 0, baseline.stderr)
        for value in ("1", "2", "-1", "TRUE", ""):
            with self.subTest(value=value):
                baseline = evaluate(0, value)
                self.assertEqual(baseline.returncode, 0, baseline.stderr)
                result = evaluate(1, value)
                self.assertNotEqual(result.returncode, 0)


class ReleaseVariantIntegrityTests(unittest.TestCase):
    def test_test_runner_stub_symbol_is_not_a_forbidden_interface(self):
        release.verify_release_symbols("08000100 V gTestRunnerEnabled\n08001000 T __rom_end\n")

    def test_fixture_and_test_interfaces_rejected(self):
        for name in ("CB2_EmeraldChampionsHeadlessFixture", "gEcHeadlessFixtureScenario",
                     "EmeraldChampionsHeadlessObserve",
                     "EmeraldChampionsAgentPrepPoll", "gEcAgentPrepCommand",
                     "CB2_TestRunner", "gTestRunnerState", "gTestRunnerHeadless"):
            with self.subTest(name=name):
                release.verify_release_symbols("08001000 T __rom_end\n")
                with self.assertRaisesRegex(SystemExit, "test/fixture interfaces"):
                    release.verify_release_symbols("08000100 T " + name + "\n")


class HydraProcessIntegrityTests(unittest.TestCase):
    def test_child_status_decoder_preserves_exit_and_signal_failures(self):
        # Unit coverage of the decoder, not Hydra's parent aggregation loop.
        compiler = shutil.which("cc")
        self.assertIsNotNone(compiler, "native C compiler required for Hydra process regression")
        source_path = ROOT / "tools/mgba-rom-test-hydra/main.c"
        program = '#define main hydra_main\n#include "' + str(source_path) + '"\n#undef main\n' + r'''
int main(void)
{
    const int expected[] = {0, 1, 2, 128 + SIGTERM};
    for (int i = 0; i < 4; i++)
    {
        pid_t child = fork();
        if (child == -1) return 10;
        if (child == 0)
        {
            if (i == 3) raise(SIGTERM);
            _exit(i);
        }
        int status;
        if (waitpid(child, &status, 0) != child) return 11;
        if (RunnerExitCode(status) != expected[i]) return 12 + i;
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            binary = str(Path(directory) / "hydra-process-test")
            build = subprocess.run([compiler, "-x", "c", "-", "-lm", "-o", binary],
                                   input=program, text=True, capture_output=True)
            self.assertEqual(build.returncode, 0, build.stderr)
            run = subprocess.run([binary], capture_output=True, text=True)
            self.assertEqual(run.returncode, 0, run.stderr)


if __name__ == "__main__":
    unittest.main()
