"""Execute production tutor lookup with synthetic tables and inventory gates.

The fixture controls data and inventory, not the lookup algorithm. It does not
exercise actual generated presets or the native tutor UI.
"""
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


class BattleSetVisibilityIntegrity(unittest.TestCase):
    def test_count_names_and_presets_share_filtered_order(self):
        source = (ROOT / 'src/emerald_champions_battle_sets.c').read_text()
        lookup = source[source.index('static bool32 IsValidBattleFormat('):
                        source.index('static bool32 DoesMonMatchPresetMoves(')]
        harness = r'''
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int bool32;
#define TRUE 1
#define FALSE 0
#define EC_BATTLE_FORMAT_SINGLES 0
#define EC_BATTLE_FORMAT_DOUBLES 1
#define EC_BATTLE_FORMAT_COUNT 2
#define MON_DATA_SPECIES 0
#define FORM_SPECIES_END NUM_SPECIES
#define MOVE_NONE 0
#define ITEM_NONE 0
enum Species { SPECIES_NONE, DIRECT, FORM, EMPTY, SPECIES_EGG, NUM_SPECIES };
enum Item { ITEM_TEST = 1 };
struct Pokemon { enum Species species; unsigned access, owned; };
struct EmeraldChampionsBattleSet { int moves[4]; enum Item item, requiredItem; unsigned transformation, protectedItem; };
struct EmeraldChampionsBattleSetRange { unsigned offset; u8 count; };
struct EmeraldChampionsBattleSetChoice { struct EmeraldChampionsBattleSet preset; const u8 *name; };
static const u8 sRecommendedSetName[] = "Recommended";
static const u16 forms[] = {FORM, DIRECT, NUM_SPECIES};
static struct { const u16 *formSpeciesIdTable; } gSpeciesInfo[NUM_SPECIES] = {[FORM] = {forms}};
static struct EmeraldChampionsBattleSetRange gEmeraldChampionsBattleSetRanges[EC_BATTLE_FORMAT_COUNT][NUM_SPECIES];
static struct EmeraldChampionsBattleSetChoice gEmeraldChampionsBattleSets[8];
static unsigned GetMonData(struct Pokemon *mon, unsigned key) { return mon->species; }
static bool32 PresetRequiresTransformation(const struct EmeraldChampionsBattleSet *p) { return p->transformation; }
static bool32 HasTransformationAccess(struct Pokemon *mon, const struct EmeraldChampionsBattleSet *p) { return mon->access; }
static bool32 PresetRequiresOwnedHeldItem(struct Pokemon *mon, const struct EmeraldChampionsBattleSet *p) { return p->protectedItem && !mon->owned; }
'''
        harness += lookup
        harness += r'''
int main(void) {
    for (unsigned format = 0; format < EC_BATTLE_FORMAT_COUNT; format++) {
        struct EmeraldChampionsBattleSetChoice *entries = &gEmeraldChampionsBattleSets[format * 4];
        struct EmeraldChampionsBattleSetRange *ranges = gEmeraldChampionsBattleSetRanges[format];
        entries[0].preset.moves[0] = 1;
        entries[0].name = format == 0 ? (const u8 *)"Singles" : sRecommendedSetName;
        ranges[DIRECT] = (struct EmeraldChampionsBattleSetRange){format * 4, 4};
        entries[1] = (struct EmeraldChampionsBattleSetChoice){{{2}, 1, 1, 1, 0}, (const u8 *)"Mega"};
        entries[2] = (struct EmeraldChampionsBattleSetChoice){{{3}, 1, 0, 0, 1}, (const u8 *)"Relic"};
        entries[3] = (struct EmeraldChampionsBattleSetChoice){{{4}, 0, 0, 0, 0}, (const u8 *)"Ordinary"};
        for (unsigned hiddenDefault = 0; hiddenDefault < 2; hiddenDefault++) {
            entries[0].preset.transformation = hiddenDefault;
            for (unsigned access = 0; access < 2; access++) for (unsigned owned = 0; owned < 2; owned++) {
                const struct EmeraldChampionsBattleSet *expected[4];
                const u8 *expectedNames[4];
                unsigned count = 0;
                if (!hiddenDefault || access) { expected[count] = &entries[0].preset; expectedNames[count++] = entries[0].name; }
                if (access) { expected[count] = &entries[1].preset; expectedNames[count++] = entries[1].name; }
                if (owned) { expected[count] = &entries[2].preset; expectedNames[count++] = entries[2].name; }
                expected[count] = &entries[3].preset; expectedNames[count++] = entries[3].name;
                for (unsigned species = DIRECT; species <= FORM; species++) {
                    struct Pokemon mon = {species, access, owned};
                    assert(GetEmeraldChampionsBattleSetCountForFormat(&mon, format) == count);
                    for (unsigned choice = 0; choice <= 255; choice++) {
                        const struct EmeraldChampionsBattleSet *p = GetEmeraldChampionsBattleSetPresetForFormat(&mon, choice, format);
                        assert(p == (choice < count ? expected[choice] : NULL));
                        assert(GetEmeraldChampionsBattleSetNameForFormat(&mon, choice, format) == (choice < count ? expectedNames[choice] : sRecommendedSetName));
                        assert(GetEmeraldChampionsBattleSetItemForFormat(&mon, choice, format) == (p ? (p->requiredItem ? p->requiredItem : p->item) : ITEM_NONE));
                        assert(GetEmeraldChampionsBattleSetRequiredItemForFormat(&mon, choice, format) == (p ? p->requiredItem : ITEM_NONE));
                    }
                }
            }
        }
    }
    for (unsigned species = 0; species <= NUM_SPECIES + 1; species++) for (unsigned format = 0; format <= 255; format++) {
        if ((species == DIRECT || species == FORM) && format < EC_BATTLE_FORMAT_COUNT) continue;
        struct Pokemon mon = {species, 1, 1};
        assert(GetEmeraldChampionsBattleSetCountForFormat(&mon, format) == 0);
        assert(GetEmeraldChampionsBattleSetPresetForFormat(&mon, 0, format) == NULL);
        assert(GetEmeraldChampionsBattleSetNameForFormat(&mon, 0, format) == sRecommendedSetName);
    }
    gEmeraldChampionsBattleSetRanges[EC_BATTLE_FORMAT_SINGLES][DIRECT].count = 1;
    struct Pokemon mon = {DIRECT, 0, 0};
    assert(GetEmeraldChampionsBattleSetCountForFormat(&mon, EC_BATTLE_FORMAT_SINGLES) == 0);
    assert(GetEmeraldChampionsBattleSetPresetForFormat(&mon, 0, EC_BATTLE_FORMAT_SINGLES) == NULL);
    assert(GetEmeraldChampionsBattleSetNameForFormat(&mon, 0, EC_BATTLE_FORMAT_SINGLES) == sRecommendedSetName);
    mon.access = 1;
    assert(GetEmeraldChampionsBattleSetCountForFormat(&mon, EC_BATTLE_FORMAT_SINGLES) == 1);
    assert(GetEmeraldChampionsBattleSetPresetForFormat(&mon, 0, EC_BATTLE_FORMAT_SINGLES) == &gEmeraldChampionsBattleSets[gEmeraldChampionsBattleSetRanges[EC_BATTLE_FORMAT_SINGLES][DIRECT].offset].preset);
    assert(GetEmeraldChampionsBattleSetPresetForFormat(&mon, 1, EC_BATTLE_FORMAT_SINGLES) == NULL);
    return 0;
}
'''
        compiler = shutil.which('cc')
        self.assertIsNotNone(compiler, 'host C compiler required')
        with tempfile.TemporaryDirectory() as temp:
            path = Path(temp)
            (path / 'test.c').write_text(harness)
            subprocess.run([compiler, '-std=c11', '-fsanitize=undefined',
                            str(path / 'test.c'), '-o', str(path / 'test')], check=True, timeout=30)
            subprocess.run([str(path / 'test')], check=True, timeout=30)


if __name__ == '__main__':
    unittest.main()
