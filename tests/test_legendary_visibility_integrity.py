"""Execute production Sign flag transitions with controlled save/item APIs."""
import re
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
OBJECT_FLAGS = {
    'ARTICUNO': 'EC_CAUGHT_ARTICUNO', 'CELEBI': 'EC_CAUGHT_CELEBI',
    'DARKRAI': 'HIDE_LEGENDARY_SIGN_DARKRAI',
    'CRESSELIA': 'HIDE_LEGENDARY_SIGN_CRESSELIA',
    'DIALGA': 'HIDE_LEGENDARY_SIGN_DIALGA', 'HOOPA': 'EC_CAUGHT_HOOPA',
    'MELOETTA': 'EC_CAUGHT_MELOETTA', 'MEWTWO': 'EC_CAUGHT_MEWTWO',
    'PALKIA': 'EC_CAUGHT_PALKIA', 'PECHARUNT': 'EC_CAUGHT_PECHARUNT',
    'REGIGIGAS': 'EC_CAUGHT_REGIGIGAS',
    'RESHIRAM': 'EC_CAUGHT_RESHIRAM', 'SHAYMIN': 'EC_CAUGHT_SHAYMIN',
    'TERAPAGOS': 'EC_CAUGHT_TERAPAGOS', 'ZAPDOS': 'EC_CAUGHT_ZAPDOS',
}


class LegendaryVisibilityIntegrity(unittest.TestCase):
    def test_unlock_capture_flags_and_order(self):
        source = (ROOT / 'src/legendary_signs.c').read_text()
        header = (ROOT / 'include/legendary_signs.h').read_text()
        sign_enum = re.search(r'enum LegendarySignId\s*\{.*?\};', header, re.S).group()
        unlock = source[source.index('void UnlockLegendarySign('):source.index('enum LegendarySignId GetLegendarySignIdBySpecies(')]
        caught = source[source.index('void MarkLegendarySignCaughtBySpecies('):source.index('bool32 PlayerPartyHasSpeciesFamily(')]
        helper = source[source.index('static u16 GetLegendarySignObjectFlag('):source.index('void UnlockLegendarySign(')]
        harness = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
typedef uint16_t u16;
typedef uint32_t u32;
enum Species { SPECIES_NONE, SPECIES_FIXTURE_START = 100 };
#define VAR_LEGENDARY_SIGNS_UNLOCKED_0 0
#define VAR_LEGENDARY_SIGNS_CAUGHT_0 1
'''
        harness += sign_enum
        harness += '\nenum { FLAG_UNUSED, ' + ', '.join('FLAG_' + value for value in OBJECT_FLAGS.values()) + ', FLAG_COUNT };\n'
        harness += 'static const u16 expected[LEGENDARY_SIGN_COUNT] = {\n' + ''.join(
            f'    [LEGENDARY_SIGN_{sign}] = FLAG_{flag},\n' for sign, flag in OBJECT_FLAGS.items()) + '};\n'
        harness += r'''
static unsigned unlocked[LEGENDARY_SIGN_COUNT], caught[LEGENDARY_SIGN_COUNT];
static unsigned lost[LEGENDARY_SIGN_COUNT], flags[FLAG_COUNT];
static struct { enum Species species; } gLegendarySignDefinitions[LEGENDARY_SIGN_COUNT];
static unsigned IsLegendarySignCaught(enum LegendarySignId sign) {
    assert((unsigned)sign < LEGENDARY_SIGN_COUNT);
    return caught[sign];
}
static unsigned IsLegendaryEncounterLost(enum Species species) {
    unsigned sign = (unsigned)species - SPECIES_FIXTURE_START;
    assert(sign < LEGENDARY_SIGN_COUNT);
    return lost[sign];
}
static unsigned events[8], eventCount;
static void Record(unsigned event) { assert(eventCount < 8); events[eventCount++] = event; }
static void SetLegendaryStateBit(u16 var, enum LegendarySignId sign) {
    if ((unsigned)sign >= LEGENDARY_SIGN_COUNT) return;
    if (var == VAR_LEGENDARY_SIGNS_UNLOCKED_0) { unlocked[sign] = 1; Record(1); }
    else { assert(var == VAR_LEGENDARY_SIGNS_CAUGHT_0); caught[sign] = 1; Record(3); }
}
static void FlagClear(u16 flag) { assert(flag && flag < FLAG_COUNT); flags[flag] = 0; Record(2); }
static void FlagSet(u16 flag) { assert(flag && flag < FLAG_COUNT); flags[flag] = 1; Record(4); }
static enum LegendarySignId GetLegendarySignIdBySpecies(enum Species species) {
    unsigned sign = (unsigned)species - SPECIES_FIXTURE_START;
    return sign < LEGENDARY_SIGN_COUNT ? sign : LEGENDARY_SIGN_COUNT;
}
static void GiveLegendaryRelicsForSpecies(enum Species species) { Record(0); }
'''
        harness += helper + unlock + caught
        harness += r'''
int main(void) {
    for (unsigned sign = 0; sign < LEGENDARY_SIGN_COUNT; sign++)
        gLegendarySignDefinitions[sign].species = (enum Species)(SPECIES_FIXTURE_START + sign);
    for (unsigned sign = 0; sign < LEGENDARY_SIGN_COUNT; sign++) {
        enum Species species = gLegendarySignDefinitions[sign].species;
        assert(GetLegendarySignObjectFlag(sign) == expected[sign]);
        // Repeated discovery may reveal an encounter that is still available.
        for (unsigned repeat = 0; repeat < 2; repeat++) {
            for (unsigned i = 0; i < FLAG_COUNT; i++) flags[i] = 1;
            eventCount = 0;
            UnlockLegendarySign(sign);
            assert(unlocked[sign] && !caught[sign] && !lost[sign]);
            assert(eventCount == (expected[sign] ? 2 : 1));
            assert(events[0] == 1);
            if (expected[sign]) assert(events[1] == 2);
            for (unsigned flag = 0; flag < FLAG_COUNT; flag++) assert(flags[flag] == (flag != expected[sign] || !flag));
        }
        // First capture keeps relic -> unlock -> caught -> physical hide order.
        eventCount = 0;
        MarkLegendarySignCaughtBySpecies(species);
        assert(unlocked[sign] && caught[sign] && !lost[sign]);
        assert(eventCount == (expected[sign] ? 5 : 3));
        assert(events[0] == 0 && events[1] == 1);
        if (expected[sign]) assert(events[2] == 2 && events[3] == 3 && events[4] == 4);
        else assert(events[2] == 3);
        for (unsigned flag = 0; flag < FLAG_COUNT; flag++) assert(flags[flag] == 1);
        for (unsigned repeat = 0; repeat < 2; repeat++) {
            eventCount = 0;
            UnlockLegendarySign(sign);
            assert(eventCount == 1 && events[0] == 1);
            assert(unlocked[sign] && caught[sign] && !lost[sign]);
            for (unsigned flag = 0; flag < FLAG_COUNT; flag++) assert(flags[flag] == 1);
        }
        eventCount = 0;
        MarkLegendarySignCaughtBySpecies(species);
        assert(eventCount == (expected[sign] ? 4 : 3));
        assert(events[0] == 0 && events[1] == 1 && events[2] == 3);
        if (expected[sign]) assert(events[3] == 4);
        for (unsigned flag = 0; flag < FLAG_COUNT; flag++) assert(flags[flag] == 1);
        // Independent failed-attempt fixture: loss is never capture ownership.
        caught[sign] = 0;
        lost[sign] = 1;
        unlocked[sign] = 0;
        for (unsigned repeat = 0; repeat < 2; repeat++) {
            eventCount = 0;
            UnlockLegendarySign(sign);
            assert(eventCount == 1 && events[0] == 1);
            assert(unlocked[sign] && !caught[sign] && lost[sign]);
            for (unsigned flag = 0; flag < FLAG_COUNT; flag++) assert(flags[flag] == 1);
        }
    }
    const unsigned invalid[] = {LEGENDARY_SIGN_COUNT, LEGENDARY_SIGN_COUNT + 1, 65535, UINT32_MAX};
    for (unsigned i = 0; i < sizeof invalid / sizeof *invalid; i++) {
        eventCount = 0;
        assert(GetLegendarySignObjectFlag(invalid[i]) == 0);
        UnlockLegendarySign(invalid[i]);
        assert(eventCount == 0);
    }
    const unsigned invalidSpecies[] = {SPECIES_NONE, SPECIES_FIXTURE_START + LEGENDARY_SIGN_COUNT, 65535, UINT32_MAX};
    for (unsigned i = 0; i < sizeof invalidSpecies / sizeof *invalidSpecies; i++) {
        eventCount = 0;
        MarkLegendarySignCaughtBySpecies((enum Species)invalidSpecies[i]);
        assert(eventCount == 1 && events[0] == 0);
    }
    return 0;
}
'''
        compiler = shutil.which('cc')
        self.assertIsNotNone(compiler, 'host C compiler required')
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp)
            (path / 'test.c').write_text(harness)
            subprocess.run([compiler, '-std=c11', '-fsanitize=undefined',
                            '-fno-sanitize-recover=undefined',
                            str(path / 'test.c'), '-o', str(path / 'test')], check=True, timeout=30)
            subprocess.run([str(path / 'test')], check=True, timeout=30)


if __name__ == '__main__':
    unittest.main()
