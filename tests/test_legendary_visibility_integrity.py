"""Production capture/unlock state transitions with controlled save/item APIs.

The current object-flag table supplies addresses; this checks state isolation,
not authored map associations, research policy or encounter-loss policy.
"""
import re
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


class LegendaryVisibilityIntegrity(unittest.TestCase):
    def test_unlock_capture_flags_preserve_ownership_and_other_signs(self):
        source = (ROOT / 'src/legendary_signs.c').read_text()
        header = (ROOT / 'include/legendary_signs.h').read_text()
        sign_enum = re.search(r'enum LegendarySignId\s*\{.*?\};', header, re.S).group()
        names = ('GetLegendarySignObjectFlag', 'UnlockLegendarySign',
                 'GetLegendarySignIdBySpecies', 'MarkLegendarySignCaughtBySpecies')
        functions = [re.search(r'^[^\n;{}]+\b' + name + r'\([^;{}]*\)\n\{.*?^\}',
                               source, re.M | re.S).group() for name in names]
        flags = sorted(set(re.findall(r'\bFLAG_\w+', functions[0])))
        harness = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
typedef uint16_t u16;
typedef uint32_t u32;
enum Species { SPECIES_NONE, SPECIES_FIXTURE_START = 100 };
// Form resolution is owned by pokemon.c; this fixture uses synthetic base IDs.
#define SanitizeSpeciesId(species) (species)
#define GET_BASE_SPECIES_ID(species) (species)
#define VAR_LEGENDARY_SIGNS_UNLOCKED_0 0
#define VAR_LEGENDARY_SIGNS_CAUGHT_0 1
''' + sign_enum + '\nenum { FLAG_UNUSED, ' + ', '.join(flags) + ', FLAG_COUNT };\n'
        harness += r'''
static unsigned unlocked[LEGENDARY_SIGN_COUNT], caught[LEGENDARY_SIGN_COUNT];
static unsigned flags[FLAG_COUNT], relicCalls;
static enum Species relicSpecies;
static struct { enum Species species; } gLegendarySignDefinitions[LEGENDARY_SIGN_COUNT];
static unsigned IsLegendarySignCaught(enum LegendarySignId sign) {
    assert((unsigned)sign < LEGENDARY_SIGN_COUNT);
    return caught[sign];
}
static void SetLegendaryStateBit(u16 var, enum LegendarySignId sign) {
    if ((unsigned)sign >= LEGENDARY_SIGN_COUNT) return;
    if (var == VAR_LEGENDARY_SIGNS_UNLOCKED_0) unlocked[sign] = 1;
    else { assert(var == VAR_LEGENDARY_SIGNS_CAUGHT_0); caught[sign] = 1; }
}
static void FlagClear(u16 flag) { assert(flag && flag < FLAG_COUNT); flags[flag] = 0; }
static void FlagSet(u16 flag) { assert(flag && flag < FLAG_COUNT); flags[flag] = 1; }
static void GiveLegendaryRelicsForSpecies(enum Species species) {
    relicCalls++; relicSpecies = species;
}
static void Reset(void) {
    memset(unlocked, 0, sizeof unlocked);
    memset(caught, 0, sizeof caught);
    for (unsigned flag = 0; flag < FLAG_COUNT; flag++) flags[flag] = 1;
    relicCalls = 0;
    relicSpecies = SPECIES_NONE;
}
''' + '\n'.join(functions) + r'''
int main(void) {
    for (unsigned sign = 0; sign < LEGENDARY_SIGN_COUNT; sign++)
        gLegendarySignDefinitions[sign].species = (enum Species)(SPECIES_FIXTURE_START + sign);
    for (unsigned sign = 0; sign < LEGENDARY_SIGN_COUNT; sign++) {
        Reset();
        enum Species species = gLegendarySignDefinitions[sign].species;
        u16 objectFlag = GetLegendarySignObjectFlag(sign);
        for (unsigned repeat = 0; repeat < 2; repeat++) {
            UnlockLegendarySign(sign);
            for (unsigned other = 0; other < LEGENDARY_SIGN_COUNT; other++) {
                assert(unlocked[other] == (other == sign));
                assert(!caught[other]);
            }
            for (unsigned flag = 0; flag < FLAG_COUNT; flag++)
                assert(flags[flag] == (flag != objectFlag || !flag));
        }
        MarkLegendarySignCaughtBySpecies(species);
        assert(relicCalls > 0 && relicSpecies == species);
        // Check after each call so recapturing cannot hide an accidental reveal.
        for (unsigned repeat = 0; repeat < 4; repeat++) {
            if (repeat % 2 == 0) UnlockLegendarySign(sign);
            else MarkLegendarySignCaughtBySpecies(species);
            for (unsigned other = 0; other < LEGENDARY_SIGN_COUNT; other++) {
                assert(unlocked[other] == (other == sign));
                assert(caught[other] == (other == sign));
            }
            for (unsigned flag = 0; flag < FLAG_COUNT; flag++) assert(flags[flag] == 1);
        }
    }
    Reset();
    const unsigned invalid[] = {LEGENDARY_SIGN_COUNT, LEGENDARY_SIGN_COUNT + 1, 65535, UINT32_MAX};
    for (unsigned i = 0; i < sizeof invalid / sizeof *invalid; i++) {
        assert(GetLegendarySignObjectFlag(invalid[i]) == 0);
        UnlockLegendarySign(invalid[i]);
    }
    const unsigned invalidSpecies[] = {SPECIES_NONE, SPECIES_FIXTURE_START + LEGENDARY_SIGN_COUNT, 65535, UINT32_MAX};
    for (unsigned i = 0; i < sizeof invalidSpecies / sizeof *invalidSpecies; i++)
        MarkLegendarySignCaughtBySpecies((enum Species)invalidSpecies[i]);
    for (unsigned sign = 0; sign < LEGENDARY_SIGN_COUNT; sign++)
        assert(!unlocked[sign] && !caught[sign]);
    for (unsigned flag = 0; flag < FLAG_COUNT; flag++) assert(flags[flag] == 1);
    return 0;
}
'''
        compiler = shutil.which('cc')
        self.assertIsNotNone(compiler, 'host C compiler required')
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp)
            (path / 'test.c').write_text(harness)
            subprocess.run([compiler, '-std=c11', '-fsanitize=undefined',
                            '-fno-sanitize-recover=undefined', str(path / 'test.c'),
                            '-o', str(path / 'test')], check=True, timeout=30)
            subprocess.run([str(path / 'test')], check=True, timeout=30)


if __name__ == '__main__':
    unittest.main()
