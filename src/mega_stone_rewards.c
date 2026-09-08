#include "global.h"
#include "event_data.h"
#include "item.h"
#include "mega_stone_rewards.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "constants/emerald_champions.h"

bool32 OwnsEmeraldChampionsMegaStone(enum Item item)
{
    if (CheckBagHasItem(item, 1) || CheckPCHasItem(item, 1))
        return TRUE;
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        if (GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_SPECIES) != SPECIES_NONE
            && GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HELD_ITEM) == item)
            return TRUE;
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            if (GetBoxMonData(&gPokemonStoragePtr->boxes[box][slot], MON_DATA_SPECIES) != SPECIES_NONE
                && GetBoxMonData(&gPokemonStoragePtr->boxes[box][slot], MON_DATA_HELD_ITEM) == item)
                return TRUE;
    return FALSE;
}

// These garden berries are not supplied by competitive presets or free item
// menus. Existing berry trees and planting remain the source of the currency.
static const enum Item sGardenBerries[] =
{
    ITEM_RAZZ_BERRY, ITEM_BLUK_BERRY, ITEM_NANAB_BERRY,
    ITEM_WEPEAR_BERRY, ITEM_PINAP_BERRY, ITEM_POMEG_BERRY,
    ITEM_KELPSY_BERRY, ITEM_QUALOT_BERRY, ITEM_HONDEW_BERRY,
    ITEM_GREPA_BERRY, ITEM_TAMATO_BERRY,
};

static const struct
{
    enum Item item;
    u16 flag;
} sBerryStoneTrades[] =
{
    {ITEM_BAXCALIBRITE, FLAG_EC_BERRY_TRADE_BAXCALIBRITE},
    {ITEM_DRAGONINITE, FLAG_EC_BERRY_TRADE_DRAGONINITE},
    {ITEM_TYRANITARITE, FLAG_EC_BERRY_TRADE_TYRANITARITE},
};

static u16 CountGardenBerries(void)
{
    u16 total = 0;
    for (u32 i = 0; i < ARRAY_COUNT(sGardenBerries); i++)
        total += CountTotalItemQuantityInBag(sGardenBerries[i]);
    return total;
}

void CountEmeraldChampionsGardenBerries(void)
{
    gSpecialVar_Result = CountGardenBerries();
    gSpecialVar_0x8005 = EC_MEGA_BERRY_TRADE_COST;
}

void TradeEmeraldChampionsGardenBerries(void)
{
    enum Item item = gSpecialVar_0x8004;
    gSpecialVar_Result = EC_MEGA_BERRY_TRADE_INVALID;
    for (u32 i = 0; i < ARRAY_COUNT(sBerryStoneTrades); i++)
    {
        if (item != sBerryStoneTrades[i].item)
            continue;
        if (FlagGet(sBerryStoneTrades[i].flag))
        {
            gSpecialVar_Result = EC_MEGA_BERRY_TRADE_ALREADY_DONE;
            return;
        }
        if (CountGardenBerries() < EC_MEGA_BERRY_TRADE_COST)
        {
            gSpecialVar_Result = EC_MEGA_BERRY_TRADE_NOT_ENOUGH;
            return;
        }
        // Check and deliver the reward before charging. A full Items pocket
        // must never cost berries or mark the exchange as completed.
        if (!AddBagItem(item, 1))
        {
            gSpecialVar_Result = EC_MEGA_BERRY_TRADE_BAG_FULL;
            return;
        }
        u16 remaining = EC_MEGA_BERRY_TRADE_COST;
        for (u32 berry = 0; berry < ARRAY_COUNT(sGardenBerries) && remaining; berry++)
        {
            u16 quantity = min(remaining, CountTotalItemQuantityInBag(sGardenBerries[berry]));
            if (quantity)
            {
                RemoveBagItem(sGardenBerries[berry], quantity);
                remaining -= quantity;
            }
        }
        FlagSet(sBerryStoneTrades[i].flag);
        gSpecialVar_Result = EC_MEGA_BERRY_TRADE_SUCCESS;
        return;
    }
}

