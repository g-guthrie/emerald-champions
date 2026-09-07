"""Production CRC boundary coverage against an independent Python bit-stream oracle."""
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def reference(data):
    crc = 0x1121
    for byte in data:
        for bit in range(8):
            feedback = (crc ^ (byte >> bit)) & 1
            crc >>= 1
            if feedback:
                crc ^= 0x8408
    return crc ^ 0xffff


class UtilCRCIntegrity(unittest.TestCase):
    def test_crc_lengths_and_signed_empty_input(self):
        source = (ROOT / 'src/util.c').read_text()
        table = source[source.index('static const u16 sCrc16Table[]'):source.index('const u8 gMiscBlank_Gfx[]')]
        functions = source[source.index('u16 CalcCRC16('):source.index('u32 CalcByteArraySum(')]
        preamble = '#include <stdint.h>\ntypedef uint8_t u8; typedef uint16_t u16; typedef uint32_t u32; typedef int32_t s32;\n'
        data = bytes((i * 37 + (i >> 8) * 19) & 255 for i in range(131073))
        lengths = (0, 1, 2, 7, 255, 256, 999, 1024, 32768, 65534, 65535, 65536, 65537, 131072, 131073)
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)
            (path / 'crc.c').write_text(preamble + table + functions)
            subprocess.run(['cc', '-O2', '-std=c11', '-Wall', '-Wextra', '-Werror',
                            '-shared', '-fPIC', '-fsanitize=undefined', '-fno-sanitize-recover=undefined',
                            str(path / 'crc.c'), '-o', str(path / 'crc.so')], check=True, timeout=30)
            # Run in a child: restoring either u16 counter must fail by timeout,
            # rather than hanging the complete test runner.
            (path / 'data.bin').write_bytes(data)
            expected = [reference(data[:length]) for length in lengths]
            program = '''
import ctypes, pathlib, sys
path = pathlib.Path(sys.argv[1])
lib = ctypes.CDLL(str(path / 'crc.so'))
data = (path / 'data.bin').read_bytes()
for name, length_type in [('CalcCRC16', ctypes.c_int32), ('CalcCRC16WithTable', ctypes.c_uint32)]:
    function = getattr(lib, name)
    function.argtypes = [ctypes.c_char_p, length_type]
    function.restype = ctypes.c_uint16
    for length, expected in zip(LENGTHS, EXPECTED):
        actual = function(data, length)
        assert actual == expected, (name, length, actual, expected)
    assert function(None, 0) == 0xeede
assert lib.CalcCRC16(None, -1) == 0xeede
assert lib.CalcCRC16(None, -2147483648) == 0xeede
'''
            program = program.replace('LENGTHS', repr(lengths)).replace('EXPECTED', repr(expected))
            result = subprocess.run([sys.executable, '-c', program, str(path)], capture_output=True,
                                    text=True, timeout=10)
            self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == '__main__':
    unittest.main()
