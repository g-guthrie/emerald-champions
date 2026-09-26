#include "global.h"
#include "test/battle.h"
#include "caps.h"
#include "event_data.h"

// Battles never grant experience: levels come only from the Leveler and Rare
// Candies, so a knockout leaves the winner's Exp. and level exactly as they were.

static void ResetBadgeCap(void)
{
    for (u32 flag = FLAG_BADGE01_GET; flag <= FLAG_BADGE08_GET; flag++)
        FlagClear(flag);
    FlagClear(FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT);
    FlagClear(FLAG_IS_CHAMPION);
}

WILD_BATTLE_TEST("No battle experience: a wild knockout grants no Exp. and no level")
{
    ResetBadgeCap();
    GIVEN {
        PLAYER(SPECIES_ZIGZAGOON) { Level(5); Speed(200); Moves(MOVE_DRAGON_RAGE); }
        PLAYER(SPECIES_LINOONE) { Level(5); Speed(100); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_MAGIKARP) { Level(13); HP(1); Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        NONE_OF {
            EXPERIENCE_BAR(player);
            MESSAGE("Zigzagoon gained 0 Exp. Points!");
        }
    } THEN {
        for (u32 i = 0; i < 2; i++)
        {
            enum Species species = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES);
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_LEVEL), 5);
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_EXP),
                gExperienceTables[gSpeciesInfo[species].growthRate][5]);
        }
    }
}

SINGLE_BATTLE_TEST("No battle experience: a trainer knockout grants no Exp. and no level")
{
    ResetBadgeCap();
    GIVEN {
        PLAYER(SPECIES_ZIGZAGOON) { Level(5); Speed(200); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_MAGIKARP) { Level(13); HP(1); Speed(1); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_MAGIKARP) { Level(13); Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); SEND_OUT(opponent, 1); }
    } SCENE {
        NOT EXPERIENCE_BAR(player);
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 5);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_EXP),
            gExperienceTables[gSpeciesInfo[SPECIES_ZIGZAGOON].growthRate][5]);
    }
}
