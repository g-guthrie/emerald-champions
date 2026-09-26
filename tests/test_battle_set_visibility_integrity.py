"""Execute production tutor lookup over the real generated preset tables.

The whole of src/emerald_champions_battle_sets.c is compiled on the host with
the real item data from src/item.c. The harness controls only the world the
lookup reads through other units: the Pokemon's species and held item, the
bag (Mega Ring / every other item) and the configured species form tables.
It does not exercise the native tutor UI.
"""
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tests'))
import host_c

HARNESS = r'''
#include <stdio.h>
struct HostMon { struct Pokemon mon; enum Species species; enum Item held; };
static struct { bool8 megaRing, everyItem; } sBag;
bool32 CheckBagHasItem(enum Item itemId, u16 count)
{
    assert(count == 1);
    return itemId == ITEM_MEGA_RING ? sBag.megaRing : sBag.everyItem;
}
u32 GetMonData2(struct Pokemon *mon, s32 field)
{
    const struct HostMon *host = (const struct HostMon *)mon;
    if (field == MON_DATA_SPECIES) return host->species;
    assert(field == MON_DATA_HELD_ITEM);
    return host->held;
}

// Independent of the production resolver: direct range, else the first form
// in the species' form table that owns one.
static const struct EmeraldChampionsBattleSetRange *ExpectedRange(u32 species, u32 format)
{
    if (format >= EC_BATTLE_FORMAT_COUNT || species == SPECIES_NONE || species == SPECIES_EGG || species >= NUM_SPECIES)
        return NULL;
    const struct EmeraldChampionsBattleSetRange *ranges = gEmeraldChampionsBattleSetRanges[format];
    if (ranges[species].count == 0 && gSpeciesInfo[species].formSpeciesIdTable) {
        const u16 *forms = gSpeciesInfo[species].formSpeciesIdTable;
        for (u32 i = 0; forms[i] != FORM_SPECIES_END; i++)
            if (ranges[forms[i]].count != 0) return &ranges[forms[i]];
    }
    return &ranges[species];
}

static u32 sViaForm, sHiddenTransformation, sHiddenOwned, sHiddenDefault, sShownTransformation, sShownOwned, sChecks;

static void CheckWorld(struct HostMon *host, u32 format, const struct EmeraldChampionsBattleSetRange *range)
{
    const struct EmeraldChampionsBattleSet *expected[256];
    const u8 *expectedNames[256];
    u32 count = 0;
    for (u32 raw = 0; range != NULL && raw < range->count; raw++) {
        const struct EmeraldChampionsBattleSetChoice *entry = &gEmeraldChampionsBattleSets[range->offset + raw];
        bool32 transformation = PresetRequiresTransformation(&entry->preset);
        bool32 access = HasTransformationAccess(&host->mon, &entry->preset);
        bool32 unowned = PresetRequiresOwnedHeldItem(&host->mon, &entry->preset);
        if (transformation) { if (access) sShownTransformation++; else sHiddenTransformation++; }
        if (IsEmeraldChampionsProtectedProgressionItem(entry->preset.item)) { if (unowned) sHiddenOwned++; else sShownOwned++; }
        if ((transformation && !access) || unowned) {
            if (raw == 0 && range->count > 1) sHiddenDefault++;
            continue;
        }
        expected[count] = &entry->preset;
        expectedNames[count++] = entry->name;
    }
    struct Pokemon *mon = &host->mon;
    assert(GetEmeraldChampionsBattleSetCountForFormat(mon, format) == count);
    if (format == EC_BATTLE_FORMAT_DOUBLES) assert(GetEmeraldChampionsBattleSetCount(mon) == count);
    for (u32 choice = 0; choice <= 255; choice++) {
        if (choice > count + 1 && choice != 255) continue;
        const struct EmeraldChampionsBattleSet *p = GetEmeraldChampionsBattleSetPresetForFormat(mon, choice, format);
        assert(p == (choice < count ? expected[choice] : NULL));
        assert(GetEmeraldChampionsBattleSetNameForFormat(mon, choice, format) == (choice < count ? expectedNames[choice] : sRecommendedSetName));
        assert(GetEmeraldChampionsBattleSetItemForFormat(mon, choice, format) == (p ? (p->requiredItem ? p->requiredItem : p->item) : ITEM_NONE));
        assert(GetEmeraldChampionsBattleSetRequiredItemForFormat(mon, choice, format) == (p ? p->requiredItem : ITEM_NONE));
        if (format == EC_BATTLE_FORMAT_DOUBLES) {
            assert(GetEmeraldChampionsBattleSetName(mon, choice) == GetEmeraldChampionsBattleSetNameForFormat(mon, choice, format));
            assert(GetEmeraldChampionsBattleSetRequiredItem(mon, choice) == (p ? p->requiredItem : ITEM_NONE));
        }
        sChecks++;
    }
}

int main(void)
{
    for (u32 species = 0; species <= NUM_SPECIES + 1; species++) {
        for (u32 format = 0; format <= 255; format++) {
            const struct EmeraldChampionsBattleSetRange *range = ExpectedRange(species, format);
            struct HostMon host = {.species = species};
            if (range == NULL || range->count == 0) {
                // Invalid species/format, or nothing authored: every query is empty.
                sBag.megaRing = sBag.everyItem = TRUE;
                assert(GetEmeraldChampionsBattleSetCountForFormat(&host.mon, format) == 0);
                assert(GetEmeraldChampionsBattleSetPresetForFormat(&host.mon, 0, format) == NULL);
                assert(GetEmeraldChampionsBattleSetNameForFormat(&host.mon, 0, format) == sRecommendedSetName);
                continue;
            }
            if (range != &gEmeraldChampionsBattleSetRanges[format][species]) sViaForm++;
            // Held-item candidates: nothing, or each item a raw entry names.
            enum Item held[1 + 2 * 255] = {ITEM_NONE};
            u32 heldCount = 1;
            for (u32 raw = 0; raw < range->count; raw++) {
                const struct EmeraldChampionsBattleSet *preset = &gEmeraldChampionsBattleSets[range->offset + raw].preset;
                held[heldCount++] = preset->item;
                held[heldCount++] = preset->requiredItem;
            }
            for (u32 bag = 0; bag < 4; bag++) {
                sBag.megaRing = bag & 1;
                sBag.everyItem = bag >> 1;
                for (u32 i = 0; i < heldCount; i++) {
                    host.held = held[i];
                    CheckWorld(&host, format, range);
                }
            }
        }
    }
    // The real tables must exercise every filter branch, not only the easy path.
    assert(sViaForm && sHiddenTransformation && sShownTransformation);
    assert(sHiddenOwned && sShownOwned && sHiddenDefault);
    printf("PASS: %u lookups; form-resolved %u, transformation hidden/shown %u/%u, protected hidden/shown %u/%u, hidden default %u\n",
           sChecks, sViaForm, sHiddenTransformation, sShownTransformation, sHiddenOwned, sShownOwned, sHiddenDefault);
    return 0;
}
'''


class BattleSetVisibilityIntegrity(unittest.TestCase):
    def test_count_names_and_presets_share_filtered_order(self):
        with tempfile.TemporaryDirectory() as temp:
            executable = host_c.build(Path(temp), {
                'battle_sets.c': host_c.production('src/emerald_champions_battle_sets.c') + HARNESS,
                'item.c': host_c.item_data_unit('CheckBagHasItem'),
                'pokemon_boundary.c': host_c.species_form_boundary(),
            }, inert=host_c.ITEM_DATA_INERT)
            result = host_c.run(executable, timeout=120)
            self.assertIn('PASS:', result.stdout)


if __name__ == '__main__':
    unittest.main()
