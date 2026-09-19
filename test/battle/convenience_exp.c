#include "global.h"
#include "test/battle.h"
#include "caps.h"
#include "event_data.h"

static void ResetConvenienceCap(void)
{
    for (u32 flag = FLAG_BADGE01_GET; flag <= FLAG_BADGE08_GET; flag++)
        FlagClear(flag);
    FlagClear(FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT);
    FlagClear(FLAG_IS_CHAMPION);
}

WILD_BATTLE_TEST("Convenience XP: normal modern experience below the cap")
{
    ResetConvenienceCap();
    GIVEN {
        PLAYER(SPECIES_ZIGZAGOON) { Level(13); Speed(200); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_MAGIKARP) { Level(13); HP(1); Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        EXPERIENCE_BAR(player);
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_EXP),
            gExperienceTables[gSpeciesInfo[SPECIES_ZIGZAGOON].growthRate][13] + 105);
    }
}

WILD_BATTLE_TEST("Convenience XP: capped party members have no experience animation")
{
    ResetConvenienceCap();
    GIVEN {
        PLAYER(SPECIES_ZIGZAGOON) { Level(14); Speed(200); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_MAGIKARP) { Level(13); HP(1); Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        NONE_OF {
            EXPERIENCE_BAR(player);
            MESSAGE("Zigzagoon gained 0 Exp. Points!");
        }
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 14);
    }
}

WILD_BATTLE_TEST("Convenience XP: an uncapped teammate earns the normal half share beside a capped lead")
{
    ResetConvenienceCap();
    GIVEN {
        PLAYER(SPECIES_ZIGZAGOON) { Level(14); Speed(200); Moves(MOVE_DRAGON_RAGE); }
        PLAYER(SPECIES_LINOONE) { Level(13); Speed(100); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_MAGIKARP) { Level(13); HP(1); Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        NOT MESSAGE("Zigzagoon gained 0 Exp. Points!");
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_EXP),
            gExperienceTables[gSpeciesInfo[SPECIES_ZIGZAGOON].growthRate][14]);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_EXP),
            gExperienceTables[gSpeciesInfo[SPECIES_LINOONE].growthRate][13] + 53);
    }
}

WILD_BATTLE_TEST("Convenience XP: a large award levels up without passing the cap")
{
    ResetConvenienceCap();
    GIVEN {
        PLAYER(SPECIES_LINOONE) { Level(13); Speed(200); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_BLISSEY) { Level(50); HP(1); Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        EXPERIENCE_BAR(player);
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 14);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_EXP),
            gExperienceTables[gSpeciesInfo[SPECIES_LINOONE].growthRate][14]);
    }
}

WILD_BATTLE_TEST("Convenience XP: a legendary at its individual cap stays silent")
{
    ResetConvenienceCap();
    GIVEN {
        PLAYER(SPECIES_MEWTWO) { Level(12); Speed(200); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_MAGIKARP) { Level(13); HP(1); Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        NONE_OF {
            EXPERIENCE_BAR(player);
            MESSAGE("Mewtwo gained 0 Exp. Points!");
        }
    } THEN {
        EXPECT_EQ(GetPlayerLevelCapForSpecies(SPECIES_MEWTWO), 12);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_EXP),
            gExperienceTables[gSpeciesInfo[SPECIES_MEWTWO].growthRate][12]);
    }
}
