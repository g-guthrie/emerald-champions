"""Execute production task-word access with UBSan and check GBA byte layout.

The whole of src/task.c is compiled on the host against the real struct Task.
"""
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tests'))
import host_c

HARNESS = r'''
#include <string.h>
static struct Task sExpected[NUM_TASKS];
int main(void) {
    const unsigned lows[] = {0, 1, 0x7fff, 0x8000, 0xffff};
    for (unsigned high = 0; high <= 0xffff; high++) {
        for (unsigned low = 0; low < sizeof lows / sizeof *lows; low++) {
            u32 value = ((u32)high << 16) | lows[low];
            for (unsigned element = 0; element < NUM_TASK_DATA - 1; element++) {
                memset(gTasks, 0xa5, sizeof(struct Task) * NUM_TASKS);
                memcpy(sExpected, gTasks, sizeof sExpected);
                SetWordTaskArg(1, element, value);
                assert(GetWordTaskArg(1, element) == value);
                for (unsigned task = 0; task < NUM_TASKS; task++) for (unsigned slot = 0; slot < NUM_TASK_DATA; slot++) {
                    u16 expected = 0xa5a5;
                    if (task == 1 && slot == element) expected = value;
                    if (task == 1 && slot == element + 1) expected = value >> 16;
                    assert((u16)gTasks[task].data[slot] == expected);
                }
                // The prior debug setters stored these exact four bytes.
                const u8 *bytes = (const u8 *)&gTasks[1].data[element];
                for (unsigned byte = 0; byte < 4; byte++) assert(bytes[byte] == (u8)(value >> (byte * 8)));
                // Nothing else in any task (callbacks, links, priority) moves.
                memcpy(&sExpected[1].data[element], bytes, 4);
                assert(memcmp(sExpected, gTasks, sizeof sExpected) == 0);
            }
        }
    }
    for (unsigned element = NUM_TASK_DATA - 1; element <= 255; element++) {
        memset(gTasks, 0xa5, sizeof(struct Task) * NUM_TASKS);
        memcpy(sExpected, gTasks, sizeof sExpected);
        SetWordTaskArg(1, element, 0xffffffff);
        assert(GetWordTaskArg(1, element) == 0);
        assert(memcmp(sExpected, gTasks, sizeof sExpected) == 0);
    }
    return 0;
}
'''


class TaskWordIntegrity(unittest.TestCase):
    def test_all_high_halves_roundtrip_without_undefined_behavior(self):
        with tempfile.TemporaryDirectory() as temp:
            executable = host_c.build(Path(temp), {'task.c': host_c.production('src/task.c') + HARNESS})
            host_c.run(executable, timeout=120)


if __name__ == '__main__':
    unittest.main()
