"""Exercise production reaction-item dispatch and ordered effects on every type."""
import re
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def harness(source):
    cases = source[source.index('    case HOLD_EFFECT_SNOWBALL:'):source.index('    case HOLD_EFFECT_JABOCA_BERRY:')]
    names = set(re.findall(r'effect = (\w+)\(', cases))
    helpers = '\n'.join(re.search(r'static enum ItemEffect ' + name + r'\([^;]+?\)\n\{.*?\n\}', source, re.S)[0] for name in sorted(names))
    types = (ROOT / 'include/constants/pokemon.h').read_text()
    types = re.search(r'enum __attribute__\(\(packed\)\) Type\n\{.*?\n\};', types, re.S)[0]
    return r'''
#include <assert.h>
#include <stdio.h>
enum BattlerId { BATTLER_0, BATTLER_1, BATTLER_2, BATTLER_3 };
enum Stat { STAT_ATK, STAT_SPATK, STAT_SPDEF };
enum ItemEffect { ITEM_NO_EFFECT, ITEM_STATS_CHANGE };
enum { HOLD_EFFECT_SNOWBALL, HOLD_EFFECT_LUMINOUS_MOSS, HOLD_EFFECT_CELL_BATTERY, HOLD_EFFECT_ABSORB_BULB };
enum { EXCLUDING_SUBSTITUTES, BattleScript_ItemStatChange };
static unsigned gCurrentMove = 123, count, damaged, substitute, expectedBattler, expectedStat;
static unsigned trace[4];
''' + types + r'''
static enum Type moveType;
static void Record(unsigned event) { assert(count < 4); trace[count++] = event; }
static unsigned IsBattlerTurnDamaged(enum BattlerId battler, unsigned mode) {
    assert(battler == expectedBattler && mode == EXCLUDING_SUBSTITUTES);
    Record(1); return damaged && !substitute;
}
static enum Type GetBattleMoveType(unsigned move) { assert(move == gCurrentMove); Record(2); return moveType; }
static void SetStatChange(enum BattlerId battler, enum Stat stat, int amount) {
    assert(battler == expectedBattler && stat == expectedStat && amount == 1); Record(3);
}
static void BattleScriptCall(unsigned script) { assert(script == BattleScript_ItemStatChange); Record(4); }
''' + helpers + r'''
static enum ItemEffect Dispatch(unsigned holdEffect, enum BattlerId itemBattler) {
    enum ItemEffect effect = ITEM_NO_EFFECT;
    switch (holdEffect) {
''' + cases + r'''
    }
    return effect;
}
int main(void) {
    const enum Type expectedTypes[] = { TYPE_ICE, TYPE_WATER, TYPE_ELECTRIC, TYPE_WATER };
    const enum Stat expectedStats[] = { STAT_ATK, STAT_SPDEF, STAT_ATK, STAT_SPATK };
    unsigned scenarios = 0;
    for (unsigned item = 0; item < 4; item++)
    for (expectedBattler = 0; expectedBattler < 4; expectedBattler++)
    for (damaged = 0; damaged < 2; damaged++)
    for (substitute = 0; substitute < 2; substitute++)
    for (moveType = TYPE_NONE; moveType < NUMBER_OF_MON_TYPES; moveType++) {
        expectedStat = expectedStats[item]; count = 0;
        unsigned hit = damaged && !substitute;
        unsigned activates = hit && moveType == expectedTypes[item];
        assert(Dispatch(item, expectedBattler) == (activates ? ITEM_STATS_CHANGE : ITEM_NO_EFFECT));
        assert(count == (activates ? 4 : hit ? 2 : 1));
        for (unsigned i = 0; i < count; i++) assert(trace[i] == i + 1);
        scenarios++;
    }
    printf("%u scenarios passed\n", scenarios);
}
'''


class HoldEffectTypeHitIntegrity(unittest.TestCase):
    def test_item_mapping_and_side_effect_order(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)
            (path / 'test.c').write_text(harness((ROOT / 'src/battle_hold_effects.c').read_text()))
            subprocess.run(['cc', '-O2', '-std=c11', '-Wall', '-Wextra', '-Werror',
                            '-fsanitize=undefined', '-fno-sanitize-recover=undefined',
                            str(path / 'test.c'), '-o', str(path / 'test')], check=True, timeout=30)
            result = subprocess.run([str(path / 'test')], capture_output=True, text=True, timeout=10)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn('1344 scenarios passed', result.stdout)


if __name__ == '__main__':
    unittest.main()
