"""Shared stat adjustment must preserve live/AI weather and recording boundaries."""
import re
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def function(source, name):
    return re.search(r'(?:static )?(?:s32|void|u32) ' + name + r'\([^;]+?\)\n\{.*?\n\}', source, re.S)[0]


def harness(stat_source, ai_source):
    source = r'''
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
typedef int32_t s32;
typedef uint32_t u32;
typedef int8_t s8;
typedef unsigned bool32;
enum Ability { ABILITY_NONE, ABILITY_CONTRARY, ABILITY_SIMPLE, ABILITY_MEGA_SOL, ABILITY_OTHER };
enum HoldEffect { HOLD_EFFECT_NONE, HOLD_EFFECT_UTILITY_UMBRELLA };
enum BattlerId { B0, B1, B2, B3 };
enum Move { MOVE_ORDINARY, MOVE_GROWTH };
enum { EFFECT_OTHER, EFFECT_GROWTH, B_WEATHER_SUN = 1, B_WEATHER_RAIN = 2 };
#define STAT_CHANGE_FORCE_MAX 7
struct BattleCalcValues { enum BattlerId battlerDef; enum Ability abilities[4]; enum HoldEffect holdEffects[4]; unsigned moveEffect; };
struct StatChange { s8 stage; unsigned onlyChecking; };
static struct { enum Ability abilities[4]; enum HoldEffect holdEffects[4]; } ai, *gAiLogicData = &ai;
static unsigned liveWeather, aiWeather, liveQueries, aiQueries, weatherQueries, records, target;
static enum Ability expectedAbility;
static unsigned GetWeather(void) { liveQueries++; return liveWeather; }
static unsigned AI_GetWeather(void) { aiQueries++; return aiWeather; }
static unsigned GetMoveEffect(enum Move move) { return move == MOVE_GROWTH ? EFFECT_GROWTH : EFFECT_OTHER; }
static void RecordAbilityBattle(enum BattlerId battler, enum Ability ability) { assert(battler == target && ability == expectedAbility); records++; }
'''
    weather = function((ROOT / 'src/battle_util.c').read_text(), 'GetAttackerWeather')
    source += weather.replace('{', '{\n    weatherQueries++;', 1) + '\n'
    if 's32 GetAdjustedStatStage(' in stat_source:
        source += function(stat_source, 'GetAdjustedStatStage') + '\n'
    source += function(stat_source, 'AdjustStatStage') + '\n' + function(ai_source, 'AI_GetAdjustedStatStage')
    return source + r'''
static int Expected(int stage, enum Ability ability, unsigned umbrella, unsigned weather, unsigned growth) {
    unsigned sunny = ability == ABILITY_MEGA_SOL || (!umbrella && (weather & B_WEATHER_SUN));
    if (growth && sunny) stage = 2;
    if (stage == 7) stage = 12;
    if (ability == ABILITY_CONTRARY) return -stage;
    if (ability == ABILITY_SIMPLE) return 2 * stage;
    return stage;
}
int main(void) {
    unsigned scenarios = 0;
    for (target = 0; target < 4; target++)
    for (int stage = -128; stage < 128; stage++)
    for (unsigned liveAbility = 0; liveAbility < 5; liveAbility++)
    for (unsigned aiAbility = 0; aiAbility < 5; aiAbility++)
    for (unsigned growth = 0; growth < 2; growth++)
    for (unsigned umbrella = 0; umbrella < 2; umbrella++)
    for (liveWeather = 0; liveWeather < 4; liveWeather++)
    for (unsigned checking = 0; checking < 2; checking++) {
        aiWeather = liveWeather ^ 3; // Prediction and actual weather intentionally disagree.
        struct BattleCalcValues cv = { .battlerDef = target, .moveEffect = growth ? EFFECT_GROWTH : EFFECT_OTHER };
        struct StatChange st = { .stage = stage, .onlyChecking = checking };
        cv.abilities[target] = liveAbility; cv.holdEffects[target] = umbrella;
        ai.abilities[target] = aiAbility; ai.holdEffects[target] = !umbrella;
        expectedAbility = liveAbility; liveQueries = aiQueries = weatherQueries = records = 0;
        AdjustStatStage(&cv, &st);
        assert(st.stage == (s8)Expected(stage, liveAbility, umbrella, liveWeather, growth));
        assert(records == (!checking && (liveAbility == ABILITY_CONTRARY || liveAbility == ABILITY_SIMPLE)));
        assert(liveQueries == growth && aiQueries == 0 && weatherQueries == growth);
        records = liveQueries = aiQueries = weatherQueries = 0;
        int prediction = AI_GetAdjustedStatStage(target, growth ? MOVE_GROWTH : MOVE_ORDINARY, stage);
        assert(prediction == Expected(stage, aiAbility, !umbrella, aiWeather, growth));
        assert(records == 0 && liveQueries == 0 && aiQueries == growth && weatherQueries == growth);
        scenarios++;
    }
    printf("%u scenarios passed\n", scenarios);
}
'''


class StatStageOwnerIntegrity(unittest.TestCase):
    def test_engine_and_ai_keep_separate_context(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)
            (path / 'test.c').write_text(harness((ROOT / 'src/battle_stat_change.c').read_text(),
                                                (ROOT / 'src/battle_ai_util.c').read_text()))
            subprocess.run(['cc', '-O2', '-std=c11', '-Wall', '-Wextra', '-Werror',
                            '-fsanitize=undefined', '-fno-sanitize-recover=undefined',
                            str(path / 'test.c'), '-o', str(path / 'test')], check=True, timeout=30)
            result = subprocess.run([str(path / 'test')], capture_output=True, text=True, timeout=15)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn('819200 scenarios passed', result.stdout)


if __name__ == '__main__':
    unittest.main()
