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

static void ExpectUntrainedOpeningMon(struct Pokemon *mon, u32 level, bool32 playerOwned)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(species);
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), level);
    EXPECT_EQ(GetMonData(mon, MON_DATA_HELD_ITEM), ITEM_NONE);
    // No authored set: the player's get the baseline spread, wild ones none.
    EXPECT_EQ(GetMonData(mon, MON_DATA_HP_EV), playerOwned ? MAX_PER_STAT_EVS : 0);
    EXPECT_EQ(GetMonEVCount(mon), playerOwned ? MAX_TOTAL_EVS : 0);
    EXPECT_NE(GetMonData(mon, MON_DATA_MOVE1), MOVE_NONE);
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
    {
        enum Move move = GetMonData(mon, MON_DATA_MOVE1 + slot);
        bool32 found = move == MOVE_NONE;
        for (u32 i = 0; learnset[i].move != LEVEL_UP_MOVE_END; i++)
            if (learnset[i].level > 0 && learnset[i].level <= level && learnset[i].move == move)
                found = TRUE;
        EXPECT(found);
    }
}

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
                ExpectUntrainedOpeningMon(&gParties[B_TRAINER_PLAYER][0], 5, TRUE);
                ExpectUntrainedOpeningMon(&gParties[B_TRAINER_PLAYER][1], 5, TRUE);
                EXPECT_EQ(GetEmeraldChampionsRivalStarterIndex(), 3 - first - second);
                EXPECT_EQ(VarGet(VAR_EC_OPENING_STATE), EC_OPENING_PAIR_GRANTED);
                EXPECT(!GiveEmeraldChampionsStarterPair(first, second));
                EXPECT_EQ(CalculatePlayerPartyCount(), 2);
            }
        }
    }
    ZeroPlayerPartyMons();
}

TEST("Story opening: Birch is chased by ordinary level-2 Poochyena and Zigzagoon")
{
    CreateEmeraldChampionsBirchRescueParty();
    EXPECT_EQ(gPartiesCount[B_TRAINER_OPPONENT_A], 2);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES), SPECIES_POOCHYENA);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_SPECIES), SPECIES_ZIGZAGOON);
    for (u32 slot = 0; slot < 2; slot++)
        ExpectUntrainedOpeningMon(&gParties[B_TRAINER_OPPONENT_A][slot], 2, FALSE);
    ZeroEnemyPartyMons();
}
