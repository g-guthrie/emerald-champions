"""Actual isolated syscall make runs with fake assembler/archive executables."""
import json
import os
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SYSCALLS = '''IntrWait RegisterRamReset Sqrt MusicPlayerOpen SoundBiasReset
SoundDriverVSyncOn Mod VBlankIntrWait MusicPlayerStart SoundDriverVSyncOff
HuffUnComp SoftResetExram MusicPlayerFadeOut LZ77UnCompWram SoundDriverMain
SoundBiasChange LZ77UnCompVram ArcTan2 MusicPlayerStop DivArm ModArm
SoundDriverVSync SoundDriverInit BgAffineSet Diff8bitUnFilterWram MultiBoot
MidiKey2Freq Div Diff8bitUnFilterVram ArcTan ObjAffineSet SoftResetRom
SoundDriverMode RLUnCompWram BitUnPack SoundChannelClear CpuFastSet CpuSet
Diff16bitUnFilter SoundBiasSet MusicPlayerContinue SoftReset RLUnCompVram'''.split()

FAKE_TOOL = '''#!/usr/bin/env python3
import json, os, sys
from pathlib import Path
kind = 'as' if Path(sys.argv[0]).name.endswith('as') else 'ar'
args = sys.argv[1:]
with open(os.environ['TOOL_LOG'], 'a') as log:
    log.write(json.dumps([kind, args]) + '\\n')
marker = Path(os.environ['FAIL_MARKER'])
if marker.exists() and marker.read_text() == kind:
    marker.unlink()
    output = args[args.index('-o') + 1] if kind == 'as' else args[1]
    Path(output).write_text('partial failed output')
    sys.exit(1)
if kind == 'as':
    symbol = args[args.index('--defsym') + 1]
    output = args[args.index('-o') + 1]
    assert symbol == 'L_' + Path(output).stem + '=1'
    assert args[-1] == 'libagbsyscall.s'
    Path(output).write_text(json.dumps(args))
else:
    assert args[1] == 'libagbsyscall.a'
    assert all(Path(p).is_file() for p in args[2:])
    previous = json.loads(Path(args[1]).read_text())[2:] if Path(args[1]).exists() else []
    members = list(dict.fromkeys(previous + args[2:]))
    Path(args[1]).write_text(json.dumps(args[:2] + members))
'''


class SyscallBuildConfigIntegrity(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.cwd = self.root / 'libagbsyscall'
        self.cwd.mkdir()
        (self.root / 'scripts').mkdir()
        shutil.copy(ROOT / 'libagbsyscall/Makefile', self.cwd)
        shutil.copy(ROOT / 'libagbsyscall/libagbsyscall.s', self.cwd)
        for include in ('constants/gba_constants.inc', 'asm/macros/function.inc'):
            target = self.root / include
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy(ROOT / include, target)
        shutil.copy(ROOT / 'scripts/update_build_config.py', self.root / 'scripts')
        self.assembler = self.root / 'fake-as'
        self.archiver = self.root / 'fake-ar'
        for tool in (self.assembler, self.archiver):
            tool.write_text(FAKE_TOOL)
            tool.chmod(0o755)
        self.log = self.root / 'tools.jsonl'
        self.marker = self.root / 'fail'
        self.env = dict(os.environ, TOOL_LOG=str(self.log), FAIL_MARKER=str(self.marker), DEVKITARM='')
        self.options = {'AS': str(self.assembler), 'AR': str(self.archiver)}

    def make(self, *targets, success=True, **options):
        args = [shutil.which('make'), '-j4', *targets]
        args += [f'{key}={value}' for key, value in (self.options | options).items()]
        result = subprocess.run(args, cwd=self.cwd, env=self.env, capture_output=True, text=True, timeout=30)
        if success:
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        else:
            self.assertNotEqual(result.returncode, 0)
        events = [json.loads(line) for line in self.log.read_text().splitlines()] if self.log.exists() else []
        self.log.unlink(missing_ok=True)
        return events

    def assert_full_build(self, events):
        assembly = [args for kind, args in events if kind == 'as']
        archives = [args for kind, args in events if kind == 'ar']
        self.assertEqual(len(assembly), len(SYSCALLS))
        self.assertEqual({args[args.index('-o') + 1] for args in assembly}, {name + '.o' for name in SYSCALLS})
        self.assertEqual(len(archives), 1)
        self.assertEqual(archives[0][2:], [name + '.o' for name in SYSCALLS])
        self.assertTrue((self.cwd / 'libagbsyscall.a').is_file())

    def test_effective_config_and_tool_content_invalidate_separately(self):
        self.assert_full_build(self.make())  # no explicit goal: archive remains default
        receipts = list(self.cwd.glob('.*config.json'))
        self.assertEqual(len(receipts), 2)
        mtimes = {p.name: p.stat().st_mtime_ns for p in receipts}
        self.assertEqual(self.make(), [])
        self.assertEqual(mtimes, {p.name: p.stat().st_mtime_ns for p in receipts})
        self.options['ASFLAGS'] = '-mcpu=arm7tdmi --alternate'
        self.assert_full_build(self.make())
        self.assertEqual(self.make(), [])
        self.options['ARFLAGS'] = 'rcs'
        events = self.make()
        self.assertEqual([kind for kind, _ in events], ['ar'])
        self.assertEqual(events[0][1][0], 'rcs')
        self.assertEqual(self.make(), [])
        self.assembler.write_text(FAKE_TOOL + '\n# changed assembler implementation\n')
        self.assert_full_build(self.make())
        self.archiver.write_text(FAKE_TOOL + '\n# changed archiver implementation\n')
        self.assertEqual([kind for kind, _ in self.make()], ['ar'])
        self.assertEqual(self.make(), [])

    def test_recipe_edits_track_only_the_affected_stage(self):
        self.assert_full_build(self.make())
        makefile = self.cwd / 'Makefile'
        original = makefile.read_text()
        makefile.write_text(original.replace('ARCHIVE = rm -f $@ &&', 'ARCHIVE = rm -f $@ && '))
        self.assertEqual([kind for kind, _ in self.make()], ['ar'])
        self.assertEqual(self.make(), [])
        makefile.write_text(makefile.read_text().replace('ASSEMBLE = $(AS) $(ASFLAGS)', 'ASSEMBLE = $(AS) $(ASFLAGS) '))
        self.assert_full_build(self.make())
        self.assertEqual(self.make(), [])

    def test_failed_tools_retry_without_configuration_change(self):
        self.marker.write_text('as')
        self.make(success=False)
        events = self.make()
        self.assertTrue(any(kind == 'as' for kind, _ in events))
        self.assertEqual(sum(kind == 'ar' for kind, _ in events), 1)
        self.options['ARFLAGS'] = 'rcs'
        self.marker.write_text('ar')
        self.assertEqual([kind for kind, _ in self.make(success=False)], ['ar'])
        self.assertEqual([kind for kind, _ in self.make()], ['ar'])
        self.assertEqual(self.make(), [])

    def test_removed_archive_member_is_not_retained(self):
        self.assert_full_build(self.make())
        retained = SYSCALLS[:-1]
        events = self.make(SYSCALLS=' '.join(retained))
        self.assertEqual([kind for kind, _ in events], ['ar'])
        archive = json.loads((self.cwd / 'libagbsyscall.a').read_text())
        self.assertEqual(archive[2:], [name + '.o' for name in retained])

    def test_assembly_include_content_changes_rebuild_every_member(self):
        self.assert_full_build(self.make())
        for name in ('constants/gba_constants.inc', 'asm/macros/function.inc'):
            with self.subTest(include=name):
                include = self.root / name
                timestamp = include.stat().st_mtime_ns
                include.write_text(include.read_text() + '\n@ changed included assembly\n')
                os.utime(include, ns=(timestamp, timestamp))
                self.assert_full_build(self.make())
                self.assertEqual(self.make(), [])

    def test_clean_needs_no_tool_and_removes_receipts(self):
        self.assert_full_build(self.make())
        self.assembler.unlink()
        self.archiver.unlink()
        self.assertEqual(self.make('clean'), [])
        self.assertFalse(list(self.cwd.glob('*.o')))
        self.assertFalse(list(self.cwd.glob('*.a')))
        self.assertFalse(list(self.cwd.glob('.*config.json')))


if __name__ == '__main__':
    unittest.main()
