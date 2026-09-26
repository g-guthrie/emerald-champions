"""Exercise production halfword storage, including the original signed-shift bug.

The whole of src/util.c is compiled on the host against the real headers.
"""
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tests'))
import host_c


def run_c(body):
    with tempfile.TemporaryDirectory() as temp:
        executable = host_c.build(Path(temp), {'util.c': host_c.production('src/util.c') + '#include <string.h>\n' + body})
        return host_c.run(executable)


class UtilWordIntegrity(unittest.TestCase):

    def test_roundtrip_layout_and_defined_domain(self):
        run_c(r'''
int main(void) {
    const unsigned lows[] = {0, 1, 0x7fff, 0x8000, 0xffff};
    s16 data[8]; // Sprite data: signed halfwords may alias their unsigned type.
    for (unsigned high = 0; high <= 0xffff; high++) {
        for (unsigned low = 0; low < sizeof lows / sizeof *lows; low++) {
            u32 expected = ((u32)high << 16) | lows[low];
            for (unsigned offset = 0; offset < 7; offset++) {
                struct {u32 before, word, after;} output = {0x12345678, 0, 0x9abcdef0};
                memset(data, 0xa5, sizeof data);
                StoreWordInTwoHalfwords((u16 *)&data[offset], expected);
                LoadWordFromTwoHalfwords((u16 *)&data[offset], &output.word);
                assert(output.word == expected);
                assert(output.before == 0x12345678 && output.after == 0x9abcdef0);
                for (unsigned slot = 0; slot < 8; slot++) {
                    u16 value = 0xa5a5;
                    if (slot == offset) value = expected;
                    if (slot == offset + 1) value = expected >> 16;
                    assert((u16)data[slot] == value);
                }
                // Compare the former expression only where its shift is defined.
                if (high < 0x8000) {
                    const u16 *h = (u16 *)&data[offset];
                    u32 original = h[0] | (s16)h[1] << 16;
                    assert(output.word == original);
                }
            }
        }
    }
    // Exercise every low half too, with complementary high bits.
    for (unsigned low = 0; low <= 0xffff; low++) {
        u32 expected = ((u32)(0xffff - low) << 16) | low, actual;
        StoreWordInTwoHalfwords((u16 *)data, expected);
        LoadWordFromTwoHalfwords((u16 *)data, &actual);
        assert(actual == expected);
    }
    return 0;
}
''')


if __name__ == '__main__':
    unittest.main()
