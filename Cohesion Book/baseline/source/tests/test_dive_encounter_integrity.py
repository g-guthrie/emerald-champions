"""Exercise every land and Dive slot roll against the currently authored weights."""
import json
import re
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

class DiveEncounterIntegrity(unittest.TestCase):
    def test_all_slots_use_the_non_grindy_distribution(self):
        source = (ROOT / "src/wild_encounter.c").read_text()
        start = source.index("u32 ChooseWildMonIndex_Land(void)")
        end = source.index("\n}", start) + 2
        selector = source[start:end]
        fields = json.loads((ROOT / "src/data/wild_encounters.json").read_text())["wild_encounter_groups"][0]["fields"]
        rates = next(row["encounter_rates"] for row in fields if row["type"] == "land_mons")
        defines = "\n".join(f"#define ENCOUNTER_CHANCE_LAND_MONS_SLOT_{i} {sum(rates[:i+1])}" for i in range(12))
        harness = r'''
#include <assert.h>
#include <stdint.h>
typedef uint8_t u8;
typedef uint32_t u32;
typedef uint8_t bool8;
#define TRUE 1
#define FALSE 0
#define MAP_TYPE_UNDERWATER 5
#define NUM_LAND_MONS_ENCOUNTER_SLOTS 12
#define ENCOUNTER_CHANCE_LAND_MONS_TOTAL 100
#define LURE_STEP_COUNT 0
static struct { unsigned mapType; } gMapHeader;
static unsigned nextRoll;
static bool8 sSweetScentInverted;
static unsigned Random(void) { return nextRoll; }
''' + defines + "\n" + selector + r'''
int main(void) {
    const unsigned expected[12] = {CURRENT_RATES};
    for (unsigned dive = 0; dive < 2; dive++) {
        gMapHeader.mapType = dive ? MAP_TYPE_UNDERWATER : 1;
        for (unsigned reversed = 0; reversed < 2; reversed++) {
            unsigned counts[12] = {0};
            sSweetScentInverted = reversed;
            for (nextRoll = 0; nextRoll < 100; nextRoll++) {
                unsigned slot = ChooseWildMonIndex_Land();
                assert(slot < 12);
                counts[slot]++;
            }
            for (unsigned slot = 0; slot < 12; slot++)
                assert(counts[slot] == expected[reversed ? 11-slot : slot]);
        }
    }
}
'''
        harness = harness.replace("CURRENT_RATES", ",".join(map(str, rates)))
        with tempfile.TemporaryDirectory() as folder:
            path = Path(folder)
            (path / "check.c").write_text(harness)
            subprocess.run([shutil.which("cc") or "cc", "-std=c11", "-fsanitize=undefined", "-fno-sanitize-recover=all", str(path / "check.c"), "-o", str(path / "check")], check=True)
            subprocess.run([str(path / "check")], check=True)
