"""Native build cache, failure preservation, and ELF parser regressions."""
import os
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'scripts'))
import native_tools as native


class NativeToolsIntegrity(unittest.TestCase):
    def test_cache_binds_source_builder_command_and_executable(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            source, builder, output = root / 'runner.c', root / 'builder.py', root / 'runner'
            source.write_text('source v1')
            builder.write_text('builder v1')
            calls = []

            def compile(command):
                calls.append(command)
                Path(command[-1]).write_bytes(b'compiled runner')
                return subprocess.CompletedProcess(command, 0, '', '')

            with patch.object(native, '__file__', str(builder)), patch.object(native, 'mgba_flags', return_value=['-lmgba']), patch.object(native, 'run', side_effect=compile):
                native.build_runner(source, output)
                native.build_runner(source, output)
                self.assertEqual(len(calls), 1)
                for path in (source, builder):
                    timestamp = path.stat().st_mtime_ns
                    path.write_text(path.read_text() + ' changed')
                    os.utime(path, ns=(timestamp, timestamp))
                    native.build_runner(source, output)
                self.assertEqual(len(calls), 3)
                with patch.dict(os.environ, {'CC': 'alternate-cc'}):
                    native.build_runner(source, output)
                    self.assertEqual(calls[-1][0], 'alternate-cc')
                    output.write_bytes(b'corrupted executable')
                    native.build_runner(source, output)
                self.assertEqual(len(calls), 5)
                stamp = output.with_name(output.name + '.inputs.json')
                before = output.read_bytes(), stamp.read_bytes()
                with patch.object(native, 'run', side_effect=native.NativeToolError('compiler failed')):
                    with self.assertRaisesRegex(native.NativeToolError, 'compiler failed'):
                        native.build_runner(source, output)
                self.assertEqual(before, (output.read_bytes(), stamp.read_bytes()))

    def test_missing_and_failed_tools_are_reported(self):
        with patch.object(native.subprocess, 'run', side_effect=FileNotFoundError('missing compiler')):
            with self.assertRaisesRegex(native.NativeToolError, 'missing compiler'):
                native.run(['missing'])
        with patch.object(native.subprocess, 'run', return_value=subprocess.CompletedProcess(['cc'], 1, 'out', 'failure')):
            with self.assertRaisesRegex(native.NativeToolError, 'failure'):
                native.run(['cc'])
        with patch.object(native.shutil, 'which', return_value=None), patch.object(Path, 'is_file', return_value=False):
            with self.assertRaisesRegex(native.NativeToolError, 'arm-none-eabi-nm'):
                native.find_nm(Path('/missing'))

    def test_sized_unsized_undefined_and_malformed_symbols(self):
        records = '''
08000101 00000018 T function
03000000 B global
00000000 A zero
         U undefined
zzzz 00000004 T invalid
08000100 garbage T malformed_size
08000120 00000004 malformed_type bad_type
08000200 00000008 T function
'''
        self.assertEqual(native.parse_symbols(records), {'function': 0x08000200, 'global': 0x03000000, 'zero': 0})
        self.assertEqual(native.parse_symbols(records, first=True)['function'], 0x08000101)

    def test_prefix_requires_headers_and_library(self):
        with tempfile.TemporaryDirectory() as temp:
            prefix = Path(temp)
            header = prefix / 'include/mgba/core/core.h'
            header.parent.mkdir(parents=True)
            header.write_text('header')
            library = prefix / 'lib/aarch64-linux-gnu/libmgba.so'
            library.parent.mkdir(parents=True)
            library.write_text('library')
            with patch.dict(os.environ, {'MGBA_PREFIX': str(prefix)}), patch.object(native.shutil, 'which', return_value=None):
                self.assertEqual(native.find_mgba_prefix(), prefix.resolve())
                with patch.object(Path, 'glob', return_value=[]):
                    with self.assertRaisesRegex(native.NativeToolError, 'MGBA_PREFIX'):
                        native.find_mgba_prefix()


if __name__ == '__main__':
    unittest.main()
