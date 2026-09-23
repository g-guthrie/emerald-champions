#include "global.h"
#include "berry.h"
#include "event_data.h"
#include "item.h"
#include "legendary_signs.h"
#include "mega_stone_rewards.h"
#include "malloc.h"
#include "script_menu.h"
#include "string_util.h"
#include "constants/emerald_champions.h"

struct HarvestIngredient { u8 berry; u8 count; };
static const struct
{
    enum Item item;
    u16 flag;
    struct HarvestIngredient recipe[6];
} sBerryStoneTrades[] =
{
    {ITEM_BAXCALIBRITE, FLAG_EC_BERRY_TRADE_BAXCALIBRITE,
        {{BERRY_ID_RAZZ, 6}, {BERRY_ID_BLUK, 6}, {BERRY_ID_NANAB, 4}, {BERRY_ID_WEPEAR, 4}}},
    {ITEM_DRAGONINITE, FLAG_EC_BERRY_TRADE_DRAGONINITE,
        {{BERRY_ID_CHERI, 6}, {BERRY_ID_CHESTO, 6}, {BERRY_ID_ORAN, 6}, {BERRY_ID_PECHA, 6}}},
    {ITEM_TYRANITARITE, FLAG_EC_BERRY_TRADE_TYRANITARITE,
        {{BERRY_ID_POMEG, 4}, {BERRY_ID_KELPSY, 4}, {BERRY_ID_QUALOT, 4},
         {BERRY_ID_HONDEW, 4}, {BERRY_ID_GREPA, 4}, {BERRY_ID_TAMATO, 4}}},
    // A permanent optional encounter invitation; retries never charge again.
    {ITEM_NONE, 0,
        {{BERRY_ID_PINAP, 8}, {BERRY_ID_SITRUS, 8}, {BERRY_ID_LUM, 4}, {BERRY_ID_LEPPA, 8}}},
};
STATIC_ASSERT(ARRAY_COUNT(sBerryStoneTrades) == EC_HARVEST_REWARD_COUNT, HarvestRewardCount);
STATIC_ASSERT(NUM_BERRIES + 1 <= 0x58, HarvestSaveCapacity);

u16 GetHarvestedBerryCount(u8 berry)
{
    return berry && berry <= NUM_BERRIES ? gSaveBlock2Ptr->pokedex.harvestedBerries[berry - 1] : 0;
}

// The pouch count saturates: picking is never refused just because a type's
// record is full, since most berry types are never spent by a trade.
void AddHarvestedBerries(u8 berry, u16 count)
{
    if (berry && berry <= NUM_BERRIES)
        gSaveBlock2Ptr->pokedex.harvestedBerries[berry - 1] = min(EC_HARVEST_LIMIT, GetHarvestedBerryCount(berry) + count);
}

static bool32 RewardClaimed(u32 choice)
{
    if (choice == 3)
        return gSaveBlock2Ptr->pokedex.gardenCelebiUnlocked || IsLegendarySignCaught(LEGENDARY_SIGN_CELEBI);
    return FlagGet(sBerryStoneTrades[choice].flag) || PlayerOwnsItem(sBerryStoneTrades[choice].item);
}

void BuildEmeraldChampionsHarvestChoices(void)
{
    for (u32 berry = 1; berry < NUM_BERRIES; berry++)
    {
        u8 *label = Alloc(32);
        u8 *end = CopyItemName(BerryTypeToItemId(berry), label);
        end = StringAppend(end, COMPOUND_STRING("  x"));
        ConvertIntToDecimalStringN(end, GetHarvestedBerryCount(berry), STR_CONV_MODE_LEFT_ALIGN, 3);
        MultichoiceDynamic_PushElement((struct ListMenuItem){label, berry});
    }
}

void BufferEmeraldChampionsHarvestRecipe(void)
{
    u32 choice = gSpecialVar_0x8004;
    if (choice >= ARRAY_COUNT(sBerryStoneTrades))
        return;
    if (choice == 3)
        StringCopy(gStringVar1, COMPOUND_STRING("Celebi's invitation"));
    else
        CopyItemName(sBerryStoneTrades[choice].item, gStringVar1);
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("{STR_VAR_1}\nHarvested berries: have / need"));
    for (u32 i = 0; i < ARRAY_COUNT(sBerryStoneTrades[choice].recipe); i++)
    {
        const struct HarvestIngredient *part = &sBerryStoneTrades[choice].recipe[i];
        if (!part->count)
            break;
        StringAppend(gStringVar4, i % 2 == 0 ? COMPOUND_STRING("\p") : COMPOUND_STRING("\n"));
        StringAppend(gStringVar4, GetBerryInfo(part->berry)->name);
        StringAppend(gStringVar4, COMPOUND_STRING(": "));
        ConvertIntToDecimalStringN(gStringVar2, GetHarvestedBerryCount(part->berry), STR_CONV_MODE_LEFT_ALIGN, 3);
        StringAppend(gStringVar4, gStringVar2);
        StringAppend(gStringVar4, COMPOUND_STRING(" / "));
        ConvertIntToDecimalStringN(gStringVar2, part->count, STR_CONV_MODE_LEFT_ALIGN, 2);
        StringAppend(gStringVar4, gStringVar2);
    }
    gSpecialVar_0x800B = RewardClaimed(choice);
    StringAppend(gStringVar4, gSpecialVar_0x800B
        ? COMPOUND_STRING("\pAlready claimed. No more berries due.")
        : COMPOUND_STRING("\pBring these to the BERRY MASTER\non ROUTE 123. One of each reward!"));
}

void TradeEmeraldChampionsGardenBerries(void)
{
    u32 choice = gSpecialVar_0x8004;
    gSpecialVar_Result = EC_MEGA_BERRY_TRADE_INVALID;
    if (choice >= ARRAY_COUNT(sBerryStoneTrades))
        return;
    if (RewardClaimed(choice))
    {
        gSpecialVar_Result = EC_MEGA_BERRY_TRADE_ALREADY_DONE;
        return;
    }
    for (u32 i = 0; i < ARRAY_COUNT(sBerryStoneTrades[choice].recipe); i++)
    {
        const struct HarvestIngredient *part = &sBerryStoneTrades[choice].recipe[i];
        if (GetHarvestedBerryCount(part->berry) < part->count)
        {
            gSpecialVar_Result = EC_MEGA_BERRY_TRADE_NOT_ENOUGH;
            return;
        }
    }
    // Delivery before debit/receipt: failed storage must preserve the entire recipe.
    if (choice != 3 && !AddBagItem(sBerryStoneTrades[choice].item, 1))
    {
        gSpecialVar_Result = EC_MEGA_BERRY_TRADE_BAG_FULL;
        return;
    }
    for (u32 i = 0; i < ARRAY_COUNT(sBerryStoneTrades[choice].recipe); i++)
    {
        const struct HarvestIngredient *part = &sBerryStoneTrades[choice].recipe[i];
        if (part->count)
            gSaveBlock2Ptr->pokedex.harvestedBerries[part->berry - 1] -= part->count;
    }
    if (choice == 3)
        gSaveBlock2Ptr->pokedex.gardenCelebiUnlocked = TRUE;
    else
        FlagSet(sBerryStoneTrades[choice].flag);
    gSpecialVar_Result = EC_MEGA_BERRY_TRADE_SUCCESS;
}

void CheckEmeraldChampionsGardenCelebi(void)
{
    gSpecialVar_Result = IsLegendarySignCaught(LEGENDARY_SIGN_CELEBI) ? 2
        : gSaveBlock2Ptr->pokedex.gardenCelebiUnlocked ? 1 : 0;
}

// The Champions archive is exactly the Mega Stones the campaign distributes,
// so one bit per entry is also the "every Mega seen" test.
static const u16 sMegaStoneArchive[] =
{
#include "data/emerald_champions_mega_stones.h"
};

STATIC_ASSERT(ARRAY_COUNT(sMegaStoneArchive) == 99, MegaArchiveIsNinetyNine);
STATIC_ASSERT(sizeof(((struct SaveBlock1 *)0)->megasWitnessed) * 8 >= ARRAY_COUNT(sMegaStoneArchive), MegasWitnessedFitsSaveBlock);

u32 EmeraldChampions_GetMegaArchiveCount(void)
{
    return ARRAY_COUNT(sMegaStoneArchive);
}

void EmeraldChampions_RecordMegaWitnessed(u32 item)
{
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sMegaStoneArchive); i++)
    {
        if (sMegaStoneArchive[i] != item)
            continue;
        gSaveBlock1Ptr->megasWitnessed[i / 8] |= 1u << (i % 8);
        return;
    }
}

u32 EmeraldChampions_CountMegasWitnessed(void)
{
    u32 count = 0;
    u32 i;

    for (i = 0; i < ARRAY_COUNT(sMegaStoneArchive); i++)
    {
        if (gSaveBlock1Ptr->megasWitnessed[i / 8] & (1u << (i % 8)))
            count++;
    }
    return count;
}
