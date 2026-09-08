"""Exercise the real root Makefile using isolated fake ARM executables."""
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
FAKE = r'''#!/usr/bin/env python3
import json,os,sys
from pathlib import Path
name=Path(sys.argv[0]).name; args=sys.argv[1:]
with open(os.environ['FAKE_LOG'],'a') as f:f.write(json.dumps([name,args])+ '\n')
if '--print-prog-name=cc1' in args: print(Path(sys.argv[0]).parent/'cc1');sys.exit()
for arg in args:
 if arg.startswith('-print-file-name='): print(Path(sys.argv[0]).parent/arg.split('=',1)[1]);sys.exit()
if name=='scaninc':
 target=Path(args[args.index('-M')+1]);target.parent.mkdir(parents=True,exist_ok=True);target.write_text(str(target.with_suffix('.o'))+': '+args[-1]+'\n');sys.exit()
if name=='cc1' and Path(os.environ['FAKE_FAIL']).exists():sys.exit(9)
if name=='patchelf':
 target=Path(args[0]);target.write_text(target.read_text()+'FILTER='+args[-1]+'\\n');sys.exit()
if name in ('arm-none-eabi-as','arm-none-eabi-ld') or (name=='arm-none-eabi-gcc' and '-o' in args):
 assert not any(a.endswith('.json') for a in args)
 if name=='arm-none-eabi-as':sys.stdin.read()
 target=Path(args[args.index('-o')+1]);target.parent.mkdir(parents=True,exist_ok=True);target.write_text('artifact\n');sys.exit()
if name=='arm-none-eabi-cpp':
 sources=[Path(a) for a in args if a.endswith('.c')]
 print(sources[0].read_text() if sources else sys.stdin.read());sys.exit()
if name in ('preproc','cc1'):print(sys.stdin.read())
'''

class BuildConfigIntegrity(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix='ec-build-config-')
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        for p in [ROOT / 'Makefile', *ROOT.glob('*.mk')]:
            shutil.copy2(p, self.root / p.name)
        for directory in ('scripts', 'src', 'test', 'include/config', 'data/maps', 'sound/songs/midi', 'bin', 'libagbsyscall'):
            (self.root / directory).mkdir(parents=True, exist_ok=True)
        for p in [*(ROOT / 'scripts').glob('*build_config*.py'), ROOT / 'scripts/export_test_elf.py']:
            shutil.copy2(p, self.root / 'scripts' / p.name)
        for path in ('src/main.c', 'src/librfu_intr.c', 'test/example.c', 'test/test_runner.c', 'test/test_runner_args.c', 'test/test_runner_battle.c'):
            (self.root / path).write_text('int fixture;\n')
        for path in ('charmap.txt', 'sound/songs/midi/midi.cfg', 'include/config/battle.h', 'ld_script_modern.ld', 'ld_script_test.ld'):
            (self.root / path).write_text('')
        (self.root / 'libagbsyscall/Makefile').write_text('all:\n\t@:\nclean:\n\t@:\n')
        (self.root / 'libagbsyscall/libagbsyscall.a').write_bytes(b'library')
        for name in ('arm-none-eabi-gcc', 'arm-none-eabi-cpp', 'arm-none-eabi-as', 'arm-none-eabi-ld', 'cc1', 'preproc', 'gbafix', 'patchelf', 'scaninc'):
            p = self.root / 'bin' / name
            p.write_text(FAKE)
            p.chmod(0o755)
        for name in ('libgcc.a', 'libnosys.a', 'libc.a'):
            (self.root / 'bin' / name).write_bytes(b'library')
        self.log = self.root / 'calls.jsonl'
        self.env = dict(os.environ, PATH=str(self.root / 'bin') + os.pathsep + os.environ['PATH'], FAKE_LOG=str(self.log), FAKE_FAIL=str(self.root / 'fail'))
        self.env.pop('DEVKITARM', None)

    def make(self, *args, success=True):
        result = subprocess.run(['make', 'SETUP_PREREQS=0', 'NODEP=1', 'TOOL_NAMES=', 'CHECK_TOOL_NAMES=', 'USE_LTO_ON_RELEASE=0', 'PREPROC=' + str(self.root / 'bin/preproc'), 'SCANINC=' + str(self.root / 'bin/scaninc'), 'FIX=' + str(self.root / 'bin/gbafix'), 'PATCHELF=' + str(self.root / 'bin/patchelf'), *args], cwd=self.root, env=self.env, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=30)
        if success:
            self.assertEqual(result.returncode, 0, result.stdout)
        else:
            self.assertNotEqual(result.returncode, 0, result.stdout)
        return result

    def calls(self, name):
        return [a for n, a in map(json.loads, self.log.read_text().splitlines()) if n == name] if self.log.exists() else []

    def test_unchanged_elf_is_noop_and_link_inputs_do_not_compile(self):
        self.make('pokeemerald.elf')
        compiles, links = len(self.calls('cc1')), len(self.calls('arm-none-eabi-ld'))
        self.make('pokeemerald.elf')
        self.assertEqual((len(self.calls('cc1')), len(self.calls('arm-none-eabi-ld'))), (compiles, links))
        self.make('pokeemerald.elf', 'LDFLAGS=--changed')
        self.assertEqual(len(self.calls('cc1')), compiles)
        self.assertEqual(len(self.calls('arm-none-eabi-ld')), links + 1)
        (self.root / 'bin/libc.a').write_bytes(b'changed library')
        self.make('pokeemerald.elf', 'LDFLAGS=--changed')
        self.assertEqual(len(self.calls('cc1')), compiles)
        self.assertEqual(len(self.calls('arm-none-eabi-ld')), links + 2)

    def test_test_elf_link_flags_are_separate(self):
        self.make('pokeemerald-test.elf', 'TEST=1')
        compiles, links = len(self.calls('cc1')), len(self.calls('arm-none-eabi-ld'))
        self.make('pokeemerald-test.elf', 'TEST=1')
        self.assertEqual((len(self.calls('cc1')), len(self.calls('arm-none-eabi-ld'))), (compiles, links))
        self.make('pokeemerald-test.elf', 'TEST=1', 'TESTLDFLAGS=--changed-test')
        self.assertEqual(len(self.calls('cc1')), compiles)
        self.assertEqual(len(self.calls('arm-none-eabi-ld')), links + 1)

    def test_filter_roundtrip_does_not_relink_and_repairs_external_mutation(self):
        self.make('pokeemerald-test.elf', 'TEST=1', 'TESTS=original')
        public = self.root / 'pokeemerald-test.elf'
        first = public.read_bytes()
        links = len(self.calls('arm-none-eabi-ld'))
        self.make('patch-test-filter', 'TEST=1', 'TESTS=other')
        self.assertNotEqual(public.read_bytes(), first)
        self.make('pokeemerald-test.elf', 'TEST=1', 'TESTS=original')
        self.assertEqual(public.read_bytes(), first)
        self.assertEqual(len(self.calls('arm-none-eabi-ld')), links)
        public.write_bytes(b'external patched state')
        self.make('pokeemerald-test.elf', 'TEST=1', 'TESTS=original')
        self.assertEqual(public.read_bytes(), first)
        self.assertEqual(len(self.calls('arm-none-eabi-ld')), links)

    def test_selected_test_membership_is_a_link_input(self):
        self.make('pokeemerald-test.elf', 'TEST=1', 'TEST_SOURCE_ALLOWLIST=test/example.c')
        compiles = len(self.calls('cc1'))
        links = len(self.calls('arm-none-eabi-ld'))
        self.make('pokeemerald-test.elf', 'TEST=1', 'TEST_SOURCE_ALLOWLIST=test/test_runner.c')
        self.assertEqual(len(self.calls('cc1')), compiles)
        self.assertEqual(len(self.calls('arm-none-eabi-ld')), links + 1)
        self.assertNotIn('test/example.o', self.calls('arm-none-eabi-ld')[-1])

    def test_compile_flags_and_tool_bytes_invalidate_same_source(self):
        target = 'build/emerald/src/main.o'
        self.make(target)
        for extra in (['CPPFLAGS=-DCHANGED'], ['CPPFLAGS=-DCHANGED', 'CFLAGS=-DOTHER']):
            before = len(self.calls('cc1'))
            self.make(target, *extra)
            self.assertEqual(len(self.calls('cc1')), before + 1)
        compiler = self.root / 'bin/cc1'
        previous_time = compiler.stat().st_mtime_ns
        compiler.write_text(compiler.read_text() + '\n# replacement compiler\n')
        os.utime(compiler, ns=(previous_time, previous_time))
        before = len(self.calls('cc1'))
        self.make(target, 'CPPFLAGS=-DCHANGED', 'CFLAGS=-DOTHER')
        self.assertEqual(len(self.calls('cc1')), before + 1)

    def test_target_specific_flags_do_not_poison_global_receipt(self):
        special, normal = 'build/emerald/src/librfu_intr.o', 'build/emerald/src/main.o'
        self.make(special, normal)
        calls = self.calls('cc1')
        self.assertIn('-fno-toplevel-reorder', calls[0])
        self.assertNotIn('-fno-toplevel-reorder', calls[1])
        self.make(normal, special)
        self.assertEqual(len(self.calls('cc1')), 2)

    def test_target_override_recipe_change_invalidates_objects(self):
        special, normal = 'build/emerald/src/librfu_intr.o', 'build/emerald/src/main.o'
        self.make(special, normal)
        rules = self.root / 'compile_rules.mk'
        original = rules.read_text()
        rules.write_text(original.replace('-fno-toplevel-reorder -Wno-pointer-to-int-cast', '-fno-toplevel-reorder -DRECEIPT_SPECIAL -Wno-pointer-to-int-cast', 1))
        self.make(special, normal)
        self.assertEqual(len(self.calls('cc1')), 4)
        self.assertIn('-DRECEIPT_SPECIAL', self.calls('cc1')[2])
        self.assertNotIn('-DRECEIPT_SPECIAL', self.calls('cc1')[3])

    def test_dependency_receipt_restarts_once_and_scanner_identity_is_tracked(self):
        target = 'build/emerald/src/main.o'
        self.make(target, 'NODEP=0')
        self.assertEqual(len(self.calls('cc1')), 1)
        scans = len(self.calls('scaninc'))
        self.make(target, 'NODEP=0')
        self.assertEqual(len(self.calls('scaninc')), scans)
        scanner = self.root / 'bin/scaninc'
        scanner.write_text(scanner.read_text() + '\n# scanner update\n')
        self.make(target, 'NODEP=0')
        self.assertGreater(len(self.calls('scaninc')), scans)
        self.assertEqual(len(self.calls('cc1')), 2)

    def test_modes_have_independent_object_state(self):
        normal = 'build/emerald/src/main.o'
        self.make(normal)
        self.make('build/emerald-debug/src/main.o', 'DEBUG=1')
        self.make('build/emerald-test/src/main.o', 'TEST=1')
        self.make('build/emerald-release/src/main.o', 'RELEASE=1')
        self.make(normal)
        self.assertEqual(len(self.calls('cc1')), 4)

    def test_failed_compile_retries(self):
        target = 'build/emerald/src/main.o'
        (self.root / 'fail').touch()
        self.make(target, success=False)
        (self.root / 'fail').unlink()
        self.make(target)
        self.assertEqual(len(self.calls('cc1')), 2)
        self.assertTrue((self.root / target).exists())

    def test_clean_and_tools_do_not_probe_arm(self):
        self.env.update(CC1='inherited', LIBPATH='inherited', LIB='inherited')
        self.make('clean')
        self.make('tools')
        self.assertFalse(self.calls('arm-none-eabi-gcc'))

if __name__ == '__main__':
    unittest.main()
