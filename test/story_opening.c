#include "global.h"
#include "battle.h"
#include "emerald_champions_opening.h"
#include "event_data.h"
#include "pokemon.h"
#include "script.h"
#include "starter_choose.h"
#include "string_util.h"
#include "test/test.h"
#include "constants/emerald_champions.h"

extern const u8 RivalsHouse_EventScript_ApplyStarterRegion[];

TEST("Story opening: all region choices and cancel select the advertised region")
{
    static const u8 *const names[] = {
        COMPOUND_STRING("Kanto"), COMPOUND_STRING("Johto"), COMPOUND_STRING("Hoenn"),
        COMPOUND_STRING("Sinnoh"), COMPOUND_STRING("Unova"), COMPOUND_STRING("Kalos"),
        COMPOUND_STRING("Alola"), COMPOUND_STRING("Galar"), COMPOUND_STRING("Paldea"),
    };
    for (u32 choice = 0; choice <= ARRAY_COUNT(names); choice++)
    {
        u32 expected = choice == ARRAY_COUNT(names) ? 2 : choice;
        gSpecialVar_Result = choice == ARRAY_COUNT(names) ? 127 : choice;
        VarSet(VAR_STARTER_GEN, 0);
        gSpecialVar_0x8008 = 2; // Downstairs approach path must survive the menu.
        RunScriptImmediately(RivalsHouse_EventScript_ApplyStarterRegion);
        EXPECT_EQ(VarGet(VAR_STARTER_GEN), expected + 1);
        EXPECT_EQ(StringCompare(gStringVar2, names[expected]), 0);
        EXPECT_EQ(gSpecialVar_0x8008, 2);
    }
}

TEST("Story opening: every region grants each distinct starter pair atomically")
{
    for (u32 generation = 1; generation <= 9; generation++)
    {
        VarSet(VAR_STARTER_GEN, generation);
        for (u32 first = 0; first < 3; first++)
        {
            for (u32 second = 0; second < 3; second++)
            {
                ZeroPlayerPartyMons();
                VarSet(VAR_EC_OPENING_STATE, 0);
                if (first == second)
                {
                    EXPECT(!GiveEmeraldChampionsStarterPair(first, second));
                    EXPECT_EQ(CalculatePlayerPartyCount(), 0);
                    EXPECT_EQ(VarGet(VAR_EC_OPENING_STATE), 0);
                    continue;
                }
                EXPECT(GiveEmeraldChampionsStarterPair(first, second));
                EXPECT_EQ(CalculatePlayerPartyCount(), 2);
                EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), GetStarterPokemon(first));
                EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SPECIES), GetStarterPokemon(second));
                EXPECT_EQ(GetEmeraldChampionsRivalStarterIndex(), 3 - first - second);
                EXPECT_EQ(VarGet(VAR_EC_OPENING_STATE), EC_OPENING_PAIR_GRANTED);
                EXPECT(!GiveEmeraldChampionsStarterPair(first, second));
                EXPECT_EQ(CalculatePlayerPartyCount(), 2);
            }
        }
    }
    ZeroPlayerPartyMons();
}
