"""Execute production task-word access with UBSan and check GBA byte layout."""
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def word_harness(source):
    functions = source[source.index('void SetWordTaskArg('):]
    return r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int16_t s16;
#define NUM_TASK_DATA 16
static struct {s16 data[NUM_TASK_DATA];} gTasks[3];
''' + functions + r'''
int main(void) {
    const unsigned lows[] = {0, 1, 0x7fff, 0x8000, 0xffff};
    for (unsigned high = 0; high <= 0xffff; high++) {
        for (unsigned low = 0; low < sizeof lows / sizeof *lows; low++) {
            u32 value = ((u32)high << 16) | lows[low];
            for (unsigned element = 0; element < NUM_TASK_DATA - 1; element++) {
                memset(gTasks, 0xa5, sizeof gTasks);
                SetWordTaskArg(1, element, value);
                assert(GetWordTaskArg(1, element) == value);
                for (unsigned task = 0; task < 3; task++) for (unsigned slot = 0; slot < NUM_TASK_DATA; slot++) {
                    u16 expected = 0xa5a5;
                    if (task == 1 && slot == element) expected = value;
                    if (task == 1 && slot == element + 1) expected = value >> 16;
                    assert((u16)gTasks[task].data[slot] == expected);
                }
                // The prior debug setters stored these exact four bytes.
                const u8 *bytes = (const u8 *)&gTasks[1].data[element];
                for (unsigned byte = 0; byte < 4; byte++) assert(bytes[byte] == (u8)(value >> (byte * 8)));
            }
        }
    }
    for (unsigned element = NUM_TASK_DATA - 1; element <= 255; element++) {
        memset(gTasks, 0xa5, sizeof gTasks);
        SetWordTaskArg(1, element, 0xffffffff);
        assert(GetWordTaskArg(1, element) == 0);
        for (unsigned task = 0; task < 3; task++) for (unsigned slot = 0; slot < NUM_TASK_DATA; slot++)
            assert((u16)gTasks[task].data[slot] == 0xa5a5);
    }
    return 0;
}
'''


class TaskWordIntegrity(unittest.TestCase):
    def test_all_high_halves_roundtrip_without_undefined_behavior(self):
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp)
            (path / 'test.c').write_text(word_harness((ROOT / 'src/task.c').read_text()))
            subprocess.run(['cc', '-O1', '-std=c11', '-fsanitize=undefined', '-fno-sanitize-recover=undefined', str(path / 'test.c'), '-o', str(path / 'test')], check=True, timeout=30)
            subprocess.run([str(path / 'test')], check=True, timeout=30)


if __name__ == '__main__':
    unittest.main()
