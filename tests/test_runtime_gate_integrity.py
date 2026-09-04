"""Regression cases derived from Hydra and test_runner.c's output protocol."""

import contextlib
import io
from pathlib import Path
import subprocess
import unittest
from unittest.mock import patch

from scripts import run_emerald_champions_runtime_gates as runtime


PASS = "[00] Champions smoke: PASS\n- Tests PASSED: 1\n- Tests TOTAL: 1\n"
FAIL = "[00] existing debt 1/2: FAIL\n- Tests FAILED: 1\n- Tests TOTAL: 1\n"


class RuntimeResultIntegrityTests(unittest.TestCase):
    def validate(self, output, **kwargs):
        return runtime.validate_gate_output(runtime.RuntimeGate("*", 1, **kwargs), output)

    def test_pass_with_omitted_zero_counters(self):
        self.assertEqual(self.validate(PASS)["TOTAL"], 1)

    def test_real_hydra_ansi_and_failure_annotation(self):
        output = (
            "[00] existing debt 1/2: \x1b[31mFAIL\x1b[0m\n"
            "test/battle/ai/example.c:42: ASSERT failed\n"
            "\n  FAILED tests:\n  - test/battle/ai/example.c:42: existing debt: ASSERT failed\n"
            "- Tests \x1b[31mFAILED\x1b[0m:          1    Add TESTS='X' to run tests with the defined prefix.\n"
            "- Tests \x1b[34mTOTAL\x1b[0m:           1\n\n"
        )
        self.assertEqual(self.validate(output, allowed_failing=("existing debt 1/2",))["FAILED"], 1)

    def test_total_only_false_green_is_rejected(self):
        self.validate(PASS)
        with self.assertRaisesRegex(SystemExit, "named results"):
            self.validate("- Tests TOTAL: 109\n")

    def test_mismatched_accepted_failure_false_green_is_rejected(self):
        self.validate(FAIL, allowed_failing=("existing debt 1/2",))
        output = "[00] existing debt 1/2: FAIL\n- Tests FAILED: 99\n- Tests TOTAL: 100\n"
        with self.assertRaises(SystemExit):
            self.validate(output, allowed_failing=("existing debt 1/2",))

    def test_every_counter_is_reconciled(self):
        # Test accounting at the parser boundary. Gate debt policy must not
        # mask a missing counter check by independently rejecting the fixture.
        for status, counter in (
            ("PASS", "PASSED"), ("FAIL", "FAILED"),
            ("ASSUMPTION_FAIL", "ASSUMPTIONS_FAILED"),
            ("KNOWN_FAILING", "KNOWN_FAILING"), ("TO_DO", "TO_DO"),
            ("EXPECTED_FAIL", "EXPECT_FAILING"),
            ("KNOWN_FAILING_PASS", "KNOWN_FAILING_PASSING"),
            ("EXPECTED_FAIL_PASS", "EXPECTED_FAIL_PASSING"),
        ):
            valid = f"[00] example: {status}\n- Tests {counter}: 1\n- Tests TOTAL: 1\n"
            for invalid in (valid.replace(f"{counter}: 1", f"{counter}: 2"),
                            valid.replace(f"- Tests {counter}: 1\n", "")):
                with self.subTest(status=status, invalid=invalid):
                    runtime.parse_results(valid)
                    with self.assertRaises(SystemExit):
                        runtime.parse_results(invalid)
        runtime.parse_results(PASS)
        with self.assertRaises(SystemExit):
            runtime.parse_results(PASS.replace("TOTAL: 1", "TOTAL: 2"))

    def test_missing_or_duplicate_summary_rejected(self):
        for output in (
            "[00] Champions smoke: PASS\n",
            PASS + "- Tests TOTAL: 1\n",
            PASS.replace("- Tests TOTAL", "- Tests PASSED: 1\n- Tests TOTAL"),
            PASS + "- Tests FAILED: 0\n",
        ):
            with self.subTest(output=output):
                self.validate(PASS)
                with self.assertRaises(SystemExit):
                    self.validate(output)

    def test_duplicate_identity_rejected_even_with_matching_counts(self):
        for second in ("PASS", "FAIL"):
            counters = "- Tests PASSED: 2\n" if second == "PASS" else "- Tests PASSED: 1\n- Tests FAILED: 1\n"
            valid = f"[00] repeated: PASS\n[01] other: {second}\n{counters}- Tests TOTAL: 2\n"
            runtime.parse_results(valid)
            output = valid.replace("[01] other:", "[01] repeated:")
            with self.subTest(second=second), self.assertRaisesRegex(SystemExit, "duplicate runtime result"):
                runtime.parse_results(output)

    def test_unknown_malformed_and_out_of_order_records_rejected(self):
        for output in (
            PASS.replace(": PASS", ": SURPRISE"),
            PASS.replace(": PASS", ": PASS trailing garbage"),
            PASS.replace("Champions smoke", "WAITING..."),
            PASS.replace("Champions smoke", " "),
            PASS.replace("PASSED: 1", "PASSED: -1"),
            PASS.replace("PASSED: 1", "PASSED: 1x"),
            PASS.replace("TOTAL: 1", "TOTAL: abc"),
            PASS.replace("PASSED", "UNRECOGNIZED_COUNTER"),
            PASS + "[01] late: PASS\n",
        ):
            with self.subTest(output=output):
                self.validate(PASS)
                with self.assertRaises(SystemExit):
                    self.validate(output)

    def test_explicit_zero_counters_are_harmless(self):
        output = PASS.replace("- Tests TOTAL", "- Tests FAILED: 0\n- Tests TOTAL")
        self.validate(output)

    def test_expected_failure_is_a_supported_success(self):
        self.validate("[00] expected: EXPECTED_FAIL\n- Tests EXPECT_FAILING: 1\n- Tests TOTAL: 1\n")

    def test_accepted_known_failure_and_todo_still_require_exact_names(self):
        output = (
            "[00] known debt: KNOWN_FAILING\n[01] todo debt: TO_DO\n"
            "- Tests KNOWN_FAILING: 1\n- Tests TO_DO: 1\n- Tests TOTAL: 2\n"
        )
        allowances = dict(maximum_known_failing=1, allowed_known_failing=("known debt",),
                          maximum_todo=1, allowed_todo=("todo debt",))
        self.validate(output, **allowances)
        for old, new in (("known debt", "new known debt"), ("todo debt", "new todo debt")):
            with self.subTest(old=old), self.assertRaisesRegex(SystemExit, "unaccepted"):
                self.validate(output.replace(old, new), **allowances)

    def test_new_failures_and_assumption_failures_rejected(self):
        self.validate(PASS)
        with self.assertRaisesRegex(SystemExit, "new failing tests"):
            self.validate(FAIL)
        output = "[00] skipped: ASSUMPTION_FAIL\n- ASSUMPTIONS_FAILED: 1\n- Tests TOTAL: 1\n"
        with self.assertRaisesRegex(SystemExit, "new failing tests"):
            self.validate(output)
        self.validate(output, allowed_failing=("skipped",))

    def test_nonintermittent_debt_that_passes_requires_ledger_update(self):
        self.validate(FAIL.replace("existing debt 1/2", "Champions smoke"), allowed_failing=("Champions smoke",))
        with self.assertRaisesRegex(SystemExit, "no longer fail or are missing"):
            self.validate(PASS, allowed_failing=("Champions smoke",))

    def test_stochastic_debt_is_local_and_trial_index_only_is_normalized(self):
        allowances = dict(allowed_intermittent_failing=("random debt 1/2 (1/?)",))
        self.validate("[00] random debt: PASS\n- Tests PASSED: 1\n- Tests TOTAL: 1\n", **allowances)
        failure = "[00] random debt 1/2 (19/?): FAIL\n- Tests FAILED: 1\n- Tests TOTAL: 1\n"
        self.validate(failure, **allowances)
        with self.assertRaisesRegex(SystemExit, "new failing tests"):
            self.validate(failure)
        with self.assertRaisesRegex(SystemExit, "new failing tests"):
            self.validate(failure.replace("1/2", "2/2"), **allowances)
        with self.assertRaisesRegex(SystemExit, "did not execute"):
            self.validate(PASS, **allowances)

    def test_old_global_flaky_identity_has_no_implicit_exemption(self):
        output = FAIL.replace("existing debt 1/2", "AI thinking time doesn't explode (singles, smart)")
        self.validate(PASS)
        with self.assertRaisesRegex(SystemExit, "new failing tests"):
            self.validate(output)

    def test_crashes_errors_timeouts_and_flaky_status_never_accepted_as_debt(self):
        for status in ("CRASH", "ERROR", "TIMEOUT", "INVALID", "FLAKY", "UNKNOWN", "UNEXPECTED_FAIL_LINE"):
            self.validate(FAIL, allowed_failing=("existing debt 1/2",))
            with self.subTest(status=status), self.assertRaises(SystemExit):
                self.validate(FAIL.replace(": FAIL", f": {status}"), allowed_failing=("existing debt 1/2",))

    def test_unexpected_passes_rejected(self):
        for status, counter in (("KNOWN_FAILING_PASS", "KNOWN_FAILING_PASSING"),
                                ("EXPECTED_FAIL_PASS", "EXPECTED_FAIL_PASSING")):
            self.validate(PASS)
            output = f"[00] old debt: {status}\n- {counter}: 1\n- Tests TOTAL: 1\n"
            with self.subTest(status=status), self.assertRaisesRegex(SystemExit, "unexpected runtime"):
                self.validate(output)

    def test_minimum_selection_is_enforced(self):
        gate = runtime.RuntimeGate("*", 2)
        valid = "[00] first: PASS\n[01] second: PASS\n- Tests PASSED: 2\n- Tests TOTAL: 2\n"
        runtime.validate_gate_output(gate, valid)
        with self.assertRaisesRegex(SystemExit, "expected at least 2"):
            runtime.validate_gate_output(gate, PASS)


class RuntimeProcessIntegrityTests(unittest.TestCase):
    def run_process(self, code, output=PASS, **kwargs):
        with patch.object(runtime.subprocess, "run", return_value=subprocess.CompletedProcess([], code, output)):
            with contextlib.redirect_stdout(io.StringIO()):
                return runtime.run(["/tmp/mgba-rom-test-hydra"], **kwargs)

    def test_filename_does_not_grant_exit_code_exemption(self):
        self.run_process(0, FAIL)
        with self.assertRaisesRegex(SystemExit, "exited 1"):
            self.run_process(1, FAIL)

    def test_test_result_exit_one_accepted_only_with_coherent_results(self):
        self.run_process(1, FAIL, test_results=True)
        self.run_process(0, PASS, test_results=True)
        with self.assertRaises(SystemExit):
            self.run_process(1, "- Tests TOTAL: 109\n", test_results=True)

    def test_signals_and_tool_failures_are_never_test_debt(self):
        for code in (-9, -11, -15, 2, 127, 139):
            self.run_process(1, FAIL, test_results=True)
            with self.subTest(code=code), self.assertRaisesRegex(SystemExit, "exited"):
                self.run_process(code, FAIL, test_results=True)

    def test_exit_and_results_must_agree(self):
        self.run_process(1, FAIL, test_results=True)
        with self.assertRaisesRegex(SystemExit, "disagree"):
            self.run_process(0, FAIL, test_results=True)
        self.run_process(0, PASS, test_results=True)
        with self.assertRaisesRegex(SystemExit, "without a corresponding"):
            self.run_process(1, PASS, test_results=True)

    def test_assumptions_with_skip_is_fail_disabled_may_exit_zero(self):
        output = "[00] skipped: ASSUMPTION_FAIL\n- ASSUMPTIONS_FAILED: 1\n- Tests TOTAL: 1\n"
        self.run_process(0, output, test_results=True)

    def test_timeout_fails_even_after_complete_output(self):
        self.run_process(0, PASS, test_results=True)
        with patch.object(runtime.subprocess, "run", side_effect=subprocess.TimeoutExpired("hydra", 1, PASS.encode())):
            with contextlib.redirect_stdout(io.StringIO()), self.assertRaisesRegex(SystemExit, "timed out"):
                runtime.run(["hydra"], timeout=1, test_results=True)

    def test_verified_accepted_debt_is_reported_explicitly(self):
        captured = io.StringIO()
        gate = runtime.RuntimeGate("*", 1, allowed_failing=("existing debt 1/2",))
        with patch.object(runtime.shutil, "copyfile"), patch.object(runtime, "run", side_effect=[("", 0), (FAIL, 0.25)]):
            with contextlib.redirect_stdout(captured):
                runtime.verify_gate(gate, test_elf=Path("test.elf"), headless_elf=Path("headless.elf"),
                                    patchelf="patchelf", hydra="hydra", romtest="romtest", objcopy="objcopy",
                                    runtime_cwd=Path("."))
        self.assertIn("PASS WITH DEBT", captured.getvalue())
        self.assertIn("failures=1", captured.getvalue())


if __name__ == "__main__":
    unittest.main()
