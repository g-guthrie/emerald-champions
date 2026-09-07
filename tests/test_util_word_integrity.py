"""Exercise production halfword storage, including the original signed-shift bug."""
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PREAMBLE = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
typedef uint16_t u16;
typedef uint32_t u32;
typedef int16_t s16;
'''


def production_words():
    source = (ROOT / 'src/util.c').read_text()
    return source[source.index('void StoreWordInTwoHalfwords('):source.index('void SetBgAffineStruct(')]


def run_c(body):
    with tempfile.TemporaryDirectory() as temp:
        path = Path(temp)
        (path / 'test.c').write_text(PREAMBLE + body)
        subprocess.run(['cc', '-O1', '-std=c11', '-Wall', '-Wextra', '-Werror',
                        '-fsanitize=undefined', '-fno-sanitize-recover=undefined',
                        str(path / 'test.c'), '-o', str(path / 'test')],
                       check=True, timeout=30)
        return subprocess.run([str(path / 'test')], capture_output=True, text=True, timeout=30)


class UtilWordIntegrity(unittest.TestCase):
    def test_original_high_word_shift_is_undefined(self):
        result = run_c(r'''
int main(void) {
    volatile u16 h[] = {0x1234, 0x8000};
    volatile u32 w = h[0] | (s16)h[1] << 16;
    (void)w;
    return 0;
}
''')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('left shift of negative value', result.stderr)

    def test_roundtrip_layout_and_defined_domain(self):
        result = run_c(production_words() + r'''
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
        self.assertEqual(result.returncode, 0, result.stderr)


if __name__ == '__main__':
    unittest.main()
