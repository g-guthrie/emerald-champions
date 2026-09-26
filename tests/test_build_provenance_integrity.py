"""Build provenance: the ROM's embedded id, its stamp and the compiled save layout."""
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import build_provenance as bp  # noqa: E402
import stamp_release_inputs  # noqa: E402

ARM_GCC = shutil.which("arm-none-eabi-gcc")
ARM_OBJDUMP = shutil.which("arm-none-eabi-objdump")


def fake_build(directory: Path, *, description=None, source_stable=True, embed=True):
    """Write a ROM/ELF pair plus a stamp shaped exactly like finalize() writes it."""
    description = description or {"schema": 1, "source": {"kind": "git", "commit": "a" * 40, "dirty_inputs": {}},
                                   "config": {"ROM": "fake.gba"}, "toolchain": []}
    identifier = bp.build_id(description)
    rom = directory / "fake.gba"
    elf = directory / "fake.elf"
    rom.write_bytes(b"\x00" * 64 + ((bp.MAGIC + identifier.encode() + b"\0") if embed else b"") + b"\xff" * 64)
    elf.write_bytes(b"\x7fELF fake")
    stamp = {"schema": 1, "build_id": identifier, "description": description, "source_stable": source_stable,
             "save_layout": {"id": "layout1:" + "b" * 64},
             "artifacts": {"rom": {"sha256": bp.sha256_file(rom)}, "elf": {"sha256": bp.sha256_file(elf)}}}
    bp.stamp_path(rom).write_text(json.dumps(stamp))
    return rom, elf, stamp


class ProvenanceVerification(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="ec-provenance-")
        self.addCleanup(self.temp.cleanup)
        self.dir = Path(self.temp.name)

    def test_matching_rom_elf_and_stamp_verify(self):
        rom, elf, stamp = fake_build(self.dir)
        summary = bp.verify(rom, elf)
        self.assertEqual(summary["status"], "verified", summary.get("reason"))
        self.assertEqual(summary["build_id"], stamp["build_id"])
        self.assertEqual(summary["save_layout"], "layout1:" + "b" * 64)
        self.assertTrue(bp.label(summary).startswith("verified build "))

    def test_every_disagreement_is_unverified_with_a_reason(self):
        cases = {
            "ROM bytes differ": lambda rom, elf, stamp: rom.write_bytes(rom.read_bytes()[:-1] + b"\x00"),
            "ELF bytes differ": lambda rom, elf, stamp: elf.write_bytes(b"other"),
            "does not hash": lambda rom, elf, stamp: bp.stamp_path(rom).write_text(json.dumps(
                dict(stamp, description=dict(stamp["description"], config={"ROM": "edited"})))),
            "ROM embeds": lambda rom, elf, stamp: bp.stamp_path(rom).write_text(json.dumps(dict(stamp, build_id="c" * 64))),
            "changed while": lambda rom, elf, stamp: bp.stamp_path(rom).write_text(json.dumps(dict(stamp, source_stable=False))),
            "unreadable stamp": lambda rom, elf, stamp: bp.stamp_path(rom).write_text("{"),
        }
        for reason, tamper in cases.items():
            with self.subTest(reason), tempfile.TemporaryDirectory() as directory:
                rom, elf, stamp = fake_build(Path(directory))
                tamper(rom, elf, stamp)
                summary = bp.verify(rom, elf)
                self.assertEqual(summary["status"], "unverified")
                self.assertIn(reason, summary["reason"])
                self.assertTrue(bp.label(summary).startswith("UNVERIFIED build: "))

    def test_unknown_source_is_not_verified(self):
        rom, elf, _ = fake_build(self.dir, description={"schema": 1, "source": {"kind": "unknown", "reason": "x"},
                                                         "config": {}, "toolchain": []})
        self.assertEqual(bp.verify(rom, elf)["status"], "unverified")

    def test_roms_without_embedded_id_or_stamp_are_legacy(self):
        rom, elf, _ = fake_build(self.dir, embed=False)
        bp.stamp_path(rom).unlink()
        summary = bp.verify(rom, elf)
        self.assertEqual(summary["status"], "legacy")
        self.assertIn("legacy provenance", bp.label(summary))
        self.assertIn("legacy provenance", bp.label(None))  # records made before provenance existed
        self.assertEqual(bp.short_label(None), "legacy provenance")

    def test_embedded_id_without_stamp_is_unverified_not_legacy(self):
        rom, elf, _ = fake_build(self.dir)
        bp.stamp_path(rom).unlink()
        self.assertEqual(bp.verify(rom, elf)["status"], "unverified")

    def test_carry_attaches_only_a_stamp_that_proves_the_copy(self):
        rom, elf, _ = fake_build(self.dir)
        copies = self.dir / "copies"
        copies.mkdir()
        same_rom, same_elf = copies / "scene.gba", copies / "scene.elf"
        shutil.copy2(rom, same_rom)
        shutil.copy2(elf, same_elf)
        self.assertEqual(bp.carry(rom, same_rom, same_elf)["status"], "verified")
        self.assertTrue(bp.stamp_path(same_rom).is_file())
        # A snapshot of different bytes never inherits the source's claim.
        other_rom = copies / "other.gba"
        other_rom.write_bytes(b"\xff" * 128)
        self.assertEqual(bp.carry(rom, other_rom, same_elf)["status"], "legacy")
        self.assertFalse(bp.stamp_path(other_rom).exists())


class SourceIdentity(unittest.TestCase):
    def test_dirty_path_filter_mirrors_release_input_digest(self):
        tool_dirs = bp._tool_dirs(ROOT)
        missing = [p.relative_to(ROOT).as_posix() for p in stamp_release_inputs.build_inputs()
                   if not bp.is_build_input(p.relative_to(ROOT).as_posix(), tool_dirs)]
        self.assertEqual(missing, [])
        for outside in ("tests/test_x.py", "tools/studio/server.py", "work/studio/x.json",
                        "scripts/run_emerald_champions_campaign.py", "pokeemerald-headless.gba"):
            self.assertFalse(bp.is_build_input(outside, tool_dirs), outside)
        self.assertTrue(bp.is_build_input("scripts/build_provenance.py", tool_dirs))

    def test_git_checkout_identity_names_commit_and_only_build_inputs(self):
        identity = bp.source_identity(ROOT)
        if identity["kind"] != "git":
            self.skipTest("not a Git checkout")
        self.assertRegex(identity["commit"], r"^[0-9a-f]{40}$")
        tool_dirs = bp._tool_dirs(ROOT)
        self.assertTrue(all(bp.is_build_input(path, tool_dirs) for path in identity["dirty_inputs"]))


@unittest.skipUnless(ARM_GCC and ARM_OBJDUMP, "ARM toolchain required")
class CompiledLayout(unittest.TestCase):
    def describe(self, source: str, name: str = "Probe"):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)
            (path / "probe.c").write_text(source + f"\nstruct {name} gProbe;\n")
            subprocess.run([ARM_GCC, "-mthumb", "-mabi=apcs-gnu", "-g", "-c", str(path / "probe.c"),
                            "-o", str(path / "probe.o")], check=True, capture_output=True)
            text = subprocess.run([ARM_OBJDUMP, "--dwarf=info", str(path / "probe.o")],
                                  check=True, capture_output=True, text=True).stdout
        dies = bp._parse_dwarf(text)
        variable = next(d for d in dies.values() if d["tag"] == "DW_TAG_variable"
                        and bp._name(d["attrs"].get("DW_AT_name")) == "gProbe")
        return bp.Layout(dies).describe(bp._ref(variable["attrs"]["DW_AT_type"]))

    BASE = """typedef unsigned char u8; typedef unsigned short u16; typedef unsigned int u32;
struct Inner { u16 a; u8 b[3]; };
struct Probe { u8 x; struct Inner inner[2]; u32 flags:5, rest:27; union { u16 h; u8 c[2]; } u; };"""

    def test_text_only_changes_keep_the_layout(self):
        base = self.describe(self.BASE)
        edited = "/* commentary */\nvoid Unrelated(void);\n#define UNUSED 1\n" + self.BASE.replace(
            "typedef unsigned short u16;", "typedef unsigned short   u16; /* spacing */")
        self.assertEqual(base, self.describe(edited))
        self.assertEqual(base[1], 28)  # APCS pads every struct to 4 bytes; a host compiler says 24

    def test_layout_changes_change_the_fingerprint(self):
        base = self.describe(self.BASE)
        for label, old, new in (
            ("member order", "u16 a; u8 b[3];", "u8 b[3]; u16 a;"),
            ("array length", "u8 b[3]", "u8 b[4]"),
            ("bitfield width", "flags:5, rest:27", "flags:6, rest:26"),
            ("member type", "u16 h;", "u32 h;"),
            ("member name", "u8 x;", "u8 y;"),
        ):
            with self.subTest(label):
                self.assertNotEqual(base, self.describe(self.BASE.replace(old, new)))

    def test_real_save_structures_fingerprint(self):
        flags = "-iquote include -DMODERN=1 -DTESTING=0 -DEMERALD -std=gnu17 -mthumb -mabi=apcs-gnu -march=armv4t -O2 -flto=auto"
        with tempfile.TemporaryDirectory() as directory:
            layout = bp.save_layout(ARM_GCC, ARM_OBJDUMP, flags, Path(directory))
            again = bp.save_layout(ARM_GCC, ARM_OBJDUMP, flags, Path(directory))
        self.assertEqual(layout, again)
        self.assertRegex(layout["id"], r"^layout1:[0-9a-f]{64}$")
        self.assertEqual(set(layout["roots"]), set(bp.LAYOUT_ROOTS))
        self.assertEqual(layout["roots"]["SaveSector"]["size"], 4096)
        self.assertGreater(layout["ids"]["flags"], 0)
        self.assertGreater(layout["ids"]["vars"], 0)


if __name__ == "__main__":
    unittest.main()
