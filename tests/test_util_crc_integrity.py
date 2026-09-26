"""Production CRC boundary coverage against an independent Python bit-stream oracle.

The whole of src/util.c is compiled on the host into a library that exports
only the two CRC entry points.
"""
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tests'))
import host_c

EXPORTS = r'''
HOST_EXPORT u16 Host_CalcCRC16(const u8 *data, s32 length) { return CalcCRC16(data, length); }
HOST_EXPORT u16 Host_CalcCRC16WithTable(const u8 *data, u32 length) { return CalcCRC16WithTable(data, length); }
'''


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
        data = bytes((i * 37 + (i >> 8) * 19) & 255 for i in range(131073))
        lengths = (0, 1, 2, 7, 255, 256, 999, 1024, 32768, 65534, 65535, 65536, 65537, 131072, 131073)
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)
            host_c.build(path, {'util.c': host_c.production('src/util.c') + EXPORTS}, name='crc', shared=True, optimize='-O2')
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
    function = getattr(lib, 'Host_' + name)
    function.argtypes = [ctypes.c_char_p, length_type]
    function.restype = ctypes.c_uint16
    for length, expected in zip(LENGTHS, EXPECTED):
        actual = function(data, length)
        assert actual == expected, (name, length, actual, expected)
    assert function(None, 0) == 0xeede
assert lib.Host_CalcCRC16(None, -1) == 0xeede
assert lib.Host_CalcCRC16(None, -2147483648) == 0xeede
'''
            program = program.replace('LENGTHS', repr(lengths)).replace('EXPECTED', repr(expected))
            result = subprocess.run([sys.executable, '-c', program, str(path)], capture_output=True,
                                    text=True, timeout=10)
            self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == '__main__':
    unittest.main()
