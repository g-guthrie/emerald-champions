"""Host execution of production preset ability selection and recognition.

Application stops after selecting the slot; inventory/move mutation is outside
this fixture. Actual Pokemon ability APIs are controlled for call-order checks.
"""
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


class BattleSetAbilityIntegrity(unittest.TestCase):
    def test_application_and_recognition_use_same_base_fallback(self):
        source = (ROOT / 'src/emerald_champions_battle_sets.c').read_text()
        find = source[source.index('static bool32 FindAbilitySlot('):source.index('static bool32 IsValidBattleFormat(')]
        start = source.index('static bool32 FindPresetAbilitySlot(') if 'static bool32 FindPresetAbilitySlot(' in source else source.index('static bool32 DoesMonMatchPresetAbility(')
        match = source[start:source.index('// A set built around Belly Drum')]
        apply = source[source.index('static u8 ApplyPreset('):]
        apply = apply[:apply.index('    if (IsEmeraldChampionsProtectedProgressionItem(preset->item)')]
        apply += '    return abilitySlot;\n}\n'
        harness = r'''
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
typedef uint8_t u8;
typedef uint32_t u32;
typedef int32_t s32;
typedef int bool32;
#define TRUE 1
#define FALSE 0
#define NUM_NORMAL_ABILITY_SLOTS 2
#define NUM_ABILITY_SLOTS 3
#define EC_BATTLE_FORMAT_DOUBLES 1
#define MON_DATA_SPECIES 0
#define MON_DATA_HELD_ITEM 1
#define MAX_PER_STAT_IVS 31
#define EC_BATTLE_SET_FAILED 255
enum Species { SPECIES_NONE, BASE, FORM, SPECIES_EGG, NUM_SPECIES };
enum Ability { ABILITY_NONE, A, B, C, MEGA };
enum Item { ITEM_NONE };
struct EmeraldChampionsBattleSet { enum Ability ability; enum Item item, requiredItem; };
struct Pokemon { enum Species species; enum Ability actual; };
static struct { enum Ability abilities[NUM_ABILITY_SLOTS]; } gSpeciesInfo[NUM_SPECIES];
static struct EmeraldChampionsBattleSet gEmeraldChampionsDefaultBattleSets[NUM_SPECIES];
static unsigned calls, lastAbility;
static unsigned GetMonData(struct Pokemon *mon, unsigned key) { return key == MON_DATA_SPECIES ? mon->species : ITEM_NONE; }
static enum Species ResolveBattleSetSpecies(enum Species species, unsigned format) { assert(format == EC_BATTLE_FORMAT_DOUBLES); return species == FORM ? BASE : species; }
static bool32 IsEmeraldChampionsProtectedProgressionItem(enum Item item) { return 0; }
static enum Ability GetMonAbility(struct Pokemon *mon) { calls = calls * 10 + 1; return lastAbility = mon->actual; }
static enum Ability GetAbilityBySpecies(enum Species species, unsigned slot) {
    calls = calls * 10 + 2;
    enum Ability ability = gSpeciesInfo[species].abilities[slot];
    for (unsigned i = 0; !ability && i < NUM_ABILITY_SLOTS; i++) ability = gSpeciesInfo[species].abilities[i];
    return lastAbility = ability;
}
'''
        harness += find + match + apply
        harness += r'''
int main(void) {
    const struct { enum Ability slots[3], authored, fallback; unsigned expected; } cases[] = {
        {{A, B, C}, A, B, 0}, // authored wins over default/hidden
        {{A, B, C}, B, A, 1},
        {{A, B, C}, MEGA, B, 1}, // legal doubles-default wins over hidden
        {{A, B, C}, MEGA, MEGA, 2}, // then hidden
        {{A, B, ABILITY_NONE}, MEGA, MEGA, 1}, // then second ordinary
        {{A, ABILITY_NONE, ABILITY_NONE}, MEGA, MEGA, 0}, // then first ordinary
        {{ABILITY_NONE, ABILITY_NONE, ABILITY_NONE}, MEGA, MEGA, EC_BATTLE_SET_FAILED},
        {{A, ABILITY_NONE, C}, ABILITY_NONE, MEGA, 1}, // existing empty-slot semantics
        {{A, ABILITY_NONE, C}, MEGA, ABILITY_NONE, 1},
        {{A, A, C}, A, B, 0}, // first matching slot is canonical
    };
    for (unsigned species = BASE; species <= FORM; species++) {
        for (unsigned i = 0; i < sizeof cases / sizeof *cases; i++) {
            for (unsigned slot = 0; slot < NUM_ABILITY_SLOTS; slot++) {
                gSpeciesInfo[species].abilities[slot] = cases[i].slots[slot];
                if (species == FORM) gSpeciesInfo[BASE].abilities[slot] = MEGA;
            }
            gEmeraldChampionsDefaultBattleSets[BASE].ability = cases[i].fallback;
            struct EmeraldChampionsBattleSet preset = {cases[i].authored};
            for (unsigned actual = ABILITY_NONE; actual <= MEGA; actual++) {
                struct Pokemon mon = {species, actual};
                calls = 0;
                assert(ApplyPreset(&mon, &preset, 0, 0, 0, 0) == cases[i].expected);
                assert(calls == 0);
                unsigned legalAuthored = 0;
                for (unsigned slot = 0; slot < NUM_ABILITY_SLOTS; slot++) legalAuthored |= cases[i].slots[slot] == cases[i].authored;
                unsigned expectedCalls = actual == preset.ability || legalAuthored || cases[i].expected == EC_BATTLE_SET_FAILED ? 1 : 12;
                enum Ability fallbackAbility = ABILITY_NONE;
                if (cases[i].expected != EC_BATTLE_SET_FAILED) {
                    fallbackAbility = cases[i].slots[cases[i].expected];
                    for (unsigned slot = 0; !fallbackAbility && slot < NUM_ABILITY_SLOTS; slot++) fallbackAbility = cases[i].slots[slot];
                }
                unsigned expectedMatch = actual == preset.ability || (!legalAuthored && cases[i].expected != EC_BATTLE_SET_FAILED && actual == fallbackAbility);
                assert(DoesMonMatchPresetAbility(&mon, &preset) == expectedMatch);
                assert(calls == expectedCalls);
                assert(lastAbility == (expectedCalls == 12 ? fallbackAbility : actual));
            }
        }
    }
    struct Pokemon invalid = {SPECIES_NONE, A};
    struct EmeraldChampionsBattleSet preset = {A};
    assert(ApplyPreset(&invalid, &preset, 0, 0, 0, 0) == EC_BATTLE_SET_FAILED);
    invalid.species = BASE;
    assert(ApplyPreset(&invalid, NULL, 0, 0, 0, 0) == EC_BATTLE_SET_FAILED);
    return 0;
}
'''
        compiler = shutil.which('cc')
        self.assertIsNotNone(compiler)
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp)
            (path / 'test.c').write_text(harness)
            subprocess.run([compiler, '-std=c11', '-fsanitize=undefined', str(path / 'test.c'), '-o', str(path / 'test')], check=True, timeout=30)
            subprocess.run([str(path / 'test')], check=True, timeout=30)


if __name__ == '__main__':
    unittest.main()
