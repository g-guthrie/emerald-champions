"""Host execution of production preset ability selection and recognition.

The whole of src/emerald_champions_battle_sets.c runs against the real
generated preset tables and real item data. The harness controls, through
other units' symbols only, each species' Ability slots and the Pokemon's
Ability API (recording call order). Wild application is checked up to the
Ability slot it writes; move/stat mutation is outside this fixture.
"""
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tests'))
import host_c

# pokemon.c boundary: writable Ability slots over the configured form tables.
BOUNDARY = r'''
void HostSetSpeciesAbilities(enum Species species, const enum Ability abilities[NUM_ABILITY_SLOTS])
{
    for (u32 slot = 0; slot < NUM_ABILITY_SLOTS; slot++)
        gSpeciesInfo[species].abilities[slot] = abilities[slot];
}
'''

HARNESS = r'''
#include <stdio.h>
void HostSetSpeciesAbilities(enum Species species, const enum Ability abilities[NUM_ABILITY_SLOTS]);
struct HostMon { struct Pokemon mon; enum Species species; enum Ability actual; u32 abilityNum; bool8 abilityNumSet; };
static unsigned calls;
static enum Ability lastAbility;
u32 GetMonData2(struct Pokemon *mon, s32 field)
{
    if (field == MON_DATA_SPECIES) return ((struct HostMon *)mon)->species;
    assert(field == MON_DATA_HELD_ITEM);
    return ITEM_NONE;
}
enum Ability GetMonAbility(struct Pokemon *mon) { calls = calls * 10 + 1; return lastAbility = ((struct HostMon *)mon)->actual; }
enum Ability GetAbilityBySpecies(enum Species species, u8 slot)
{
    calls = calls * 10 + 2;
    enum Ability ability = gSpeciesInfo[species].abilities[slot];
    for (unsigned i = 0; !ability && i < NUM_ABILITY_SLOTS; i++) ability = gSpeciesInfo[species].abilities[i];
    return lastAbility = ability;
}
void SetMonData(struct Pokemon *mon, s32 field, const void *data)
{
    if (field == MON_DATA_ABILITY_NUM) {
        ((struct HostMon *)mon)->abilityNum = *(const u8 *)data;
        ((struct HostMon *)mon)->abilityNumSet = TRUE;
    }
}
void SetMonMoveSlot(struct Pokemon *mon, enum Move move, u8 slot) {}
bool32 TryFormChangeOnMove(struct Pokemon *mon, enum Move changedMove, enum BattleTrainer trainer) { return FALSE; }
void CalculateMonStats(struct Pokemon *mon) {}
bool32 ClampMonToPlayerLevelCap(struct Pokemon *mon) { assert(!"wild application never clamps"); return FALSE; }
bool32 CheckBagHasItem(enum Item itemId, u16 count) { assert(!"wild application never checks the bag"); return FALSE; }

// Symbolic Abilities of the original cases; each run maps them onto distinct
// real Ability ids, with the case's fallback symbol mapped onto the real
// doubles-default Ability of the species under test.
enum { A_NONE, A, B, C, MEGA, SYMBOLS };
#define NO_SLOT 255 // expected: application fails, no slot is written
static enum Ability sConcrete[SYMBOLS];
static void MapSymbols(unsigned fallbackSymbol, enum Ability fallback)
{
    enum Ability next = 1;
    sConcrete[A_NONE] = ABILITY_NONE;
    for (unsigned symbol = A; symbol < SYMBOLS; symbol++) {
        if (symbol == fallbackSymbol) { sConcrete[symbol] = fallback; continue; }
        while (next == fallback) next++;
        sConcrete[symbol] = next++;
    }
    assert(fallbackSymbol == A_NONE ? fallback == ABILITY_NONE : sConcrete[fallbackSymbol] == fallback);
}

static bool32 IsValidSpecies(u32 species) { return species > SPECIES_NONE && species < NUM_SPECIES && species != SPECIES_EGG; }

int main(void) {
    const struct { unsigned slots[3], authored, fallback, expected; } cases[] = {
        {{A, B, C}, A, B, 0}, // authored wins over default/hidden
        {{A, B, C}, B, A, 1},
        {{A, B, C}, MEGA, B, 1}, // legal doubles-default wins over hidden
        {{A, B, C}, MEGA, MEGA, 2}, // then hidden
        {{A, B, A_NONE}, MEGA, MEGA, 1}, // then second ordinary
        {{A, A_NONE, A_NONE}, MEGA, MEGA, 0}, // then first ordinary
        {{A_NONE, A_NONE, A_NONE}, MEGA, MEGA, NO_SLOT},
        {{A, A_NONE, C}, A_NONE, MEGA, 1}, // existing empty-slot semantics
        {{A, A_NONE, C}, MEGA, A_NONE, 1},
        {{A, A, C}, A, B, 0}, // first matching slot is canonical
    };
    const struct EmeraldChampionsBattleSetRange *doubles = gEmeraldChampionsBattleSetRanges[EC_BATTLE_FORMAT_DOUBLES];
    // Real species: a direct doubles set; a form resolving to another species'
    // set; and one with no set at all (the fallback-less, ABILITY_NONE path).
    u32 direct = 0, form = 0, owner = 0, unset = 0;
    for (u32 species = 1; species < NUM_SPECIES; species++) {
        if (!IsValidSpecies(species)) continue;
        u32 raw = GetEmeraldChampionsRawBattleSetCount(species);
        if (!direct && doubles[species].count) direct = species;
        if (!unset && !raw) unset = species;
        if (!form && !doubles[species].count && raw && gSpeciesInfo[species].formSpeciesIdTable) {
            const u16 *forms = gSpeciesInfo[species].formSpeciesIdTable;
            for (u32 i = 0; !owner && forms[i] != FORM_SPECIES_END; i++)
                if (doubles[forms[i]].count) owner = forms[i];
            form = species;
        }
    }
    assert(direct && form && owner && unset && owner != form);
    unsigned runs = 0;
    for (unsigned variant = 0; variant < 2; variant++) {
        for (unsigned i = 0; i < sizeof cases / sizeof *cases; i++) {
            u32 species = cases[i].fallback == A_NONE ? unset : variant ? form : direct;
            const struct EmeraldChampionsBattleSet *fallbackSet = GetEmeraldChampionsRawBattleSet(species, 0);
            assert((fallbackSet == NULL) == (cases[i].fallback == A_NONE));
            MapSymbols(cases[i].fallback, fallbackSet ? fallbackSet->ability : ABILITY_NONE);
            enum Ability slots[NUM_ABILITY_SLOTS], mega[NUM_ABILITY_SLOTS];
            for (unsigned slot = 0; slot < NUM_ABILITY_SLOTS; slot++) {
                slots[slot] = sConcrete[cases[i].slots[slot]];
                mega[slot] = sConcrete[MEGA];
            }
            HostSetSpeciesAbilities(species, slots);
            if (species == form) HostSetSpeciesAbilities(owner, mega);
            struct EmeraldChampionsBattleSet preset = {.ability = sConcrete[cases[i].authored]};
            for (unsigned actualSymbol = A_NONE; actualSymbol <= MEGA; actualSymbol++) {
                enum Ability actual = sConcrete[actualSymbol];
                struct HostMon mon = {.species = species, .actual = actual};
                calls = 0;
                u8 result = ApplyPreset(&mon.mon, &preset, PRESET_WILD);
                if (cases[i].expected == NO_SLOT) {
                    assert(result == EC_BATTLE_SET_FAILED && !mon.abilityNumSet);
                } else {
                    assert(result == EC_BATTLE_SET_SUCCESS && mon.abilityNumSet);
                    assert(mon.abilityNum == cases[i].expected);
                }
                assert(calls == 0);
                unsigned legalAuthored = 0;
                for (unsigned slot = 0; slot < NUM_ABILITY_SLOTS; slot++) legalAuthored |= cases[i].slots[slot] == cases[i].authored;
                unsigned expectedCalls = actualSymbol == cases[i].authored || legalAuthored || cases[i].expected == NO_SLOT ? 1 : 12;
                enum Ability fallbackAbility = ABILITY_NONE;
                if (cases[i].expected != NO_SLOT) {
                    fallbackAbility = slots[cases[i].expected];
                    for (unsigned slot = 0; !fallbackAbility && slot < NUM_ABILITY_SLOTS; slot++) fallbackAbility = slots[slot];
                }
                unsigned expectedMatch = actual == preset.ability || (!legalAuthored && cases[i].expected != NO_SLOT && actual == fallbackAbility);
                assert(DoesMonMatchPresetAbility(&mon.mon, &preset) == expectedMatch);
                assert(calls == expectedCalls);
                assert(lastAbility == (expectedCalls == 12 ? fallbackAbility : actual));
                runs++;
            }
        }
    }
    struct HostMon invalid = {.species = SPECIES_NONE, .actual = sConcrete[A]};
    struct EmeraldChampionsBattleSet preset = {.ability = sConcrete[A]};
    assert(ApplyPreset(&invalid.mon, &preset, PRESET_WILD) == EC_BATTLE_SET_FAILED);
    invalid.species = direct;
    assert(ApplyPreset(&invalid.mon, NULL, PRESET_WILD) == EC_BATTLE_SET_FAILED);
    assert(!invalid.abilityNumSet);
    printf("PASS: %u ability cases over species %u, form %u->%u, unset %u\n", runs, direct, form, owner, unset);
    return 0;
}
'''


class BattleSetAbilityIntegrity(unittest.TestCase):
    def test_application_and_recognition_use_same_base_fallback(self):
        with tempfile.TemporaryDirectory() as temp:
            executable = host_c.build(Path(temp), {
                'battle_sets.c': host_c.production('src/emerald_champions_battle_sets.c') + HARNESS,
                'item.c': host_c.item_data_unit('CheckBagHasItem'),
                'pokemon_boundary.c': host_c.species_form_boundary(writable=True) + BOUNDARY,
            }, inert=host_c.ITEM_DATA_INERT)
            result = host_c.run(executable)
            self.assertIn('PASS:', result.stdout)


if __name__ == '__main__':
    unittest.main()
