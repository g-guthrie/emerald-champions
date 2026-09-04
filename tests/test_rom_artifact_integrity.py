"""Reject mismatched ROM/ELF pairs independently of matching metadata hashes."""

import importlib.util
from pathlib import Path
import subprocess
import tempfile
import unittest
from unittest.mock import patch


ROOT = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location("rom_artifacts", ROOT / "scripts/rom_artifacts.py")
artifacts = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(artifacts)


class RomArtifactIntegrity(unittest.TestCase):
    def test_unpadded_power_of_two_binary(self):
        artifacts.verify_rom_bytes(b"ABCD", b"ABCD")

    def test_actual_gbafix_padding_rule(self):
        artifacts.verify_rom_bytes(b"ABC\xff", b"ABC")

    def test_different_elf_even_with_valid_padding(self):
        with self.assertRaisesRegex(ValueError, "do not match"):
            artifacts.verify_rom_bytes(b"ABC\xff", b"ABD")

    def test_padding_mutation(self):
        with self.assertRaisesRegex(ValueError, "padding"):
            artifacts.verify_rom_bytes(b"ABC\x00", b"ABC")

    def test_extra_or_missing_bytes(self):
        for rom in (b"ABC", b"ABC\xff\xff"):
            with self.subTest(rom=rom), self.assertRaisesRegex(ValueError, "size"):
                artifacts.verify_rom_bytes(rom, b"ABC")

    def test_empty_derived_binary(self):
        with self.assertRaisesRegex(ValueError, "invalid"):
            artifacts.verify_rom_bytes(b"", b"")

    def test_invokes_objcopy_and_checks_its_actual_output(self):
        with tempfile.TemporaryDirectory() as directory:
            rom = Path(directory) / "rom.gba"
            elf = Path(directory) / "rom.elf"
            rom.write_bytes(b"ABC\xff")
            elf.write_bytes(b"elf fixture")
            def objcopy(command, **kwargs):
                self.assertEqual(command[:4], ["objcopy", "-O", "binary", str(elf)])
                Path(command[4]).write_bytes(b"ABC")
                return subprocess.CompletedProcess(command, 0, "", "")
            with patch.object(artifacts.subprocess, "run", side_effect=objcopy):
                artifacts.verify_rom_elf_pair(rom, elf, objcopy="objcopy")
                rom.write_bytes(b"ABD\xff")
                with self.assertRaisesRegex(ValueError, "do not match"):
                    artifacts.verify_rom_elf_pair(rom, elf, objcopy="objcopy")

    def test_failed_objcopy_is_not_valid_evidence(self):
        with tempfile.TemporaryDirectory() as directory:
            rom, elf = (Path(directory) / name for name in ("rom.gba", "rom.elf"))
            rom.write_bytes(b"ABCD")
            elf.write_bytes(b"elf fixture")
            with patch.object(artifacts.subprocess, "run", return_value=subprocess.CompletedProcess([], 2, "", "invalid ELF")):
                with self.assertRaisesRegex(ValueError, "cannot derive"):
                    artifacts.verify_rom_elf_pair(rom, elf, objcopy="objcopy")


if __name__ == "__main__":
    unittest.main()
