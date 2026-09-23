#include "global.h"
#include "lottery_corner.h"
#include "event_data.h"
#include "pokemon.h"
#include "constants/items.h"
#include "random.h"
#include "string_util.h"
#include "text.h"
#include "pokemon_storage_system.h"

// Inclement's four prize tiers; the live clerk delivers VAR_0x8005.
static const enum Item sLotteryPrizes[] =
{
    ITEM_PP_UP,
    ITEM_BOTTLE_CAP,
    ITEM_MAX_REVIVE,
    ITEM_MASTER_BALL,
};

static u8 GetMatchingDigits(u16 winNumber, u32 otId);

#define LOTTERY_CORNER_SALT 0x03007173 // Adress of the lottery corner var in vanilla Emerald

void ResetLotteryCorner(void)
{
    SetLotteryNumber(Random32());
    VarSet(VAR_POKELOT_PRIZE_ITEM, ITEM_NONE);
}

void SetRandomLotteryNumber(u16 i)
{
    u32 var = Random();

    while (--i != 0xFFFF)
        var = ISO_RANDOMIZE2(var);

    SetLotteryNumber(var);
}

void RetrieveLotteryNumber(void)
{
    u16 lottoNumber = GetLotteryNumber();
    gSpecialVar_Result = lottoNumber;
}

void PickLotteryCornerTicket(void)
{
    u16 i;
    u16 j;
    u32 box;
    u32 slot;

    gSpecialVar_0x8004 = 0;
    gSpecialVar_0x8005 = ITEM_NONE;
    gSpecialVar_0x8006 = 0;
    slot = 0;
    box = 0;
    for (i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];

        if (GetMonData(mon, MON_DATA_SPECIES) != SPECIES_NONE)
        {
            // do not calculate ticket values for eggs.
            if (!GetMonData(mon, MON_DATA_IS_EGG))
            {
                u32 otId = GetMonData(mon, MON_DATA_OT_ID);
                u8 numMatchingDigits = GetMatchingDigits(gSpecialVar_Result, otId);

                if (numMatchingDigits > gSpecialVar_0x8004 && numMatchingDigits > 1)
                {
                    gSpecialVar_0x8004 = numMatchingDigits - 1;
                    box = TOTAL_BOXES_COUNT;
                    slot = i;
                }
            }
        }
        else // Pokémon are always arranged from populated spots first to unpopulated, so the moment a NONE species is found, that's the end of the list.
        {
            break;
        }
    }

    for (i = 0; i < TOTAL_BOXES_COUNT; i++)
    {
        for (j = 0; j < IN_BOX_COUNT; j++)
        {
            if (GetBoxMonData(&gPokemonStoragePtr->boxes[i][j], MON_DATA_SPECIES) != SPECIES_NONE &&
            !GetBoxMonData(&gPokemonStoragePtr->boxes[i][j], MON_DATA_IS_EGG))
            {
                u32 otId = GetBoxMonData(&gPokemonStoragePtr->boxes[i][j], MON_DATA_OT_ID);
                u8 numMatchingDigits = GetMatchingDigits(gSpecialVar_Result, otId);

                if (numMatchingDigits > gSpecialVar_0x8004 && numMatchingDigits > 1)
                {
                    gSpecialVar_0x8004 = numMatchingDigits - 1;
                    box = i;
                    slot = j;
                }
            }
        }
    }

    if (gSpecialVar_0x8004 != 0)
    {
        gSpecialVar_0x8005 = sLotteryPrizes[gSpecialVar_0x8004 - 1];
        if (box == TOTAL_BOXES_COUNT)
        {
            gSpecialVar_0x8006 = 0;
            GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_NICKNAME, gStringVar1);
        }
        else
        {
            gSpecialVar_0x8006 = 1;
            GetBoxMonData(&gPokemonStoragePtr->boxes[box][slot], MON_DATA_NICKNAME, gStringVar1);
        }
        StringGet_Nickname(gStringVar1);
    }
}

static u8 GetMatchingDigits(u16 winNumber, u32 otId)
{
    u8 matchingDigits = 0;
    otId &= 0xFFFF;
    for (; matchingDigits < 5; matchingDigits++)
    {
        if (winNumber % 10 != otId % 10)
            break;
        winNumber /= 10;
        otId /= 10;
    }
    return matchingDigits;
}

// lottery numbers go from 0 to 99999, not 65535 (0xFFFF). Interestingly enough, the function that calls GetLotteryNumber shifts to u16, so it can't be anything above 65535 anyway.
void SetLotteryNumber(u32 lotteryNum)
{
    #if OW_USE_DAILY_SEED_FOR_VANILLA_VARIABLES == FALSE
    u16 lowNum = lotteryNum >> 16;
    u16 highNum = lotteryNum;

    VarSet(VAR_POKELOT_RND1, highNum);
    VarSet(VAR_POKELOT_RND2, lowNum);
    #endif
}

u32 GetLotteryNumber(void)
{
    #if OW_USE_DAILY_SEED_FOR_VANILLA_VARIABLES == FALSE
    u16 highNum = VarGet(VAR_POKELOT_RND1);
    u16 lowNum = VarGet(VAR_POKELOT_RND2);
    return (lowNum << 16) | highNum;
    #else
    rng_value_t localRngState = LocalRandomSeed(gSaveBlock1Ptr->dailySeed ^ LOTTERY_CORNER_SALT);
    return LocalRandom32(&localRngState);
    #endif
}
