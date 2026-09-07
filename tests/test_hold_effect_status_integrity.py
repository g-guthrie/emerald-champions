"""Status-cure masks must preserve unrelated state and toxic-counter gating."""
import re
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def harness(source):
    cases = source[source.index('    case HOLD_EFFECT_CURE_PAR:'):source.index('    case HOLD_EFFECT_CURE_FRZ:')]
    names = set(re.findall(r'effect = (\w+)\(', cases))
    functions = '\n'.join(re.search(r'static enum ItemEffect ' + name + r'\([^;]+?\)\n\{.*?\n\}', source, re.S)[0] for name in sorted(names))
    constants = (ROOT / 'include/constants/battle.h').read_text()
    constants = '\n'.join(line for line in constants.splitlines() if re.match(r'#define STATUS1_(POISON|BURN|PARALYSIS|TOXIC_POISON|TOXIC_COUNTER|PSN_ANY)\s', line))
    return r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
typedef uint32_t u32;
enum BattlerId { B0, B1, B2, B3 };
enum ItemEffect { ITEM_NO_EFFECT, ITEM_STATUS_CHANGE };
enum { HOLD_EFFECT_CURE_PAR, HOLD_EFFECT_CURE_PSN, HOLD_EFFECT_CURE_BRN };
enum { B_MSG_CURED_PARALYSIS, B_MSG_CURED_POISON, B_MSG_CURED_BURN };
enum { MULTISTRING_CHOOSER = 2, BattleScript_BerryCureStatusRet = 123 };
static struct { u32 status1; } gBattleMons[4];
static unsigned char gBattleCommunication[8];
static unsigned calls, target, expectedMessage;
static u32 expectedStatus;
static void BattleScriptCall(unsigned script) {
    assert(script == BattleScript_BerryCureStatusRet);
    assert(gBattleMons[target].status1 == expectedStatus);
    assert(gBattleCommunication[MULTISTRING_CHOOSER] == expectedMessage);
    calls++;
}
''' + constants + '\n' + functions + r'''
static enum ItemEffect Dispatch(unsigned holdEffect, enum BattlerId itemBattler) {
    enum ItemEffect effect = ITEM_NO_EFFECT;
    switch (holdEffect) {
''' + cases + r'''
    }
    return effect;
}
int main(void) {
    const u32 statuses[] = { STATUS1_PARALYSIS, STATUS1_PSN_ANY, STATUS1_BURN };
    const u32 clears[] = { STATUS1_PARALYSIS, STATUS1_PSN_ANY | STATUS1_TOXIC_COUNTER, STATUS1_BURN };
    const unsigned messages[] = { B_MSG_CURED_PARALYSIS, B_MSG_CURED_POISON, B_MSG_CURED_BURN };
    unsigned scenarios = 0;
    for (target = 0; target < 4; target++)
    for (unsigned item = 0; item < 3; item++)
    for (u32 low = 0; low < 65536; low++) {
        u32 status = low | 0xa55a0000u;
        unsigned active = !!(status & statuses[item]);
        expectedStatus = active ? status & ~clears[item] : status;
        expectedMessage = active ? messages[item] : 0xa5;
        for (unsigned i = 0; i < 4; i++) gBattleMons[i].status1 = status;
        for (unsigned i = 0; i < 8; i++) gBattleCommunication[i] = 0xa5;
        calls = 0;
        assert(Dispatch(item, target) == (active ? ITEM_STATUS_CHANGE : ITEM_NO_EFFECT));
        assert(calls == active);
        for (unsigned i = 0; i < 4; i++) assert(gBattleMons[i].status1 == (i == target ? expectedStatus : status));
        for (unsigned i = 0; i < 8; i++) assert(gBattleCommunication[i] == (i == MULTISTRING_CHOOSER ? expectedMessage : 0xa5));
        scenarios++;
    }
    printf("%u scenarios passed\n", scenarios);
}
'''


class HoldEffectStatusIntegrity(unittest.TestCase):
    def test_status_masks_and_ordered_writes(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)
            (path / 'test.c').write_text(harness((ROOT / 'src/battle_hold_effects.c').read_text()))
            subprocess.run(['cc', '-O2', '-std=c11', '-Wall', '-Wextra', '-Werror',
                            '-fsanitize=undefined', '-fno-sanitize-recover=undefined',
                            str(path / 'test.c'), '-o', str(path / 'test')], check=True, timeout=30)
            result = subprocess.run([str(path / 'test')], capture_output=True, text=True, timeout=10)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn('786432 scenarios passed', result.stdout)


if __name__ == '__main__':
    unittest.main()
