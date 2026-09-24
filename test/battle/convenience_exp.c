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

WILD_BATTLE_TEST("Convenience XP: a legendary at the normal cap stays silent")
{
    ResetConvenienceCap();
    GIVEN {
        PLAYER(SPECIES_MEWTWO) { Level(14); Speed(200); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_MAGIKARP) { Level(13); HP(1); Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        NONE_OF {
            EXPERIENCE_BAR(player);
            MESSAGE("Mewtwo gained 0 Exp. Points!");
        }
    } THEN {
        EXPECT_EQ(GetPlayerLevelCapForSpecies(SPECIES_MEWTWO), 14);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_EXP),
            gExperienceTables[gSpeciesInfo[SPECIES_MEWTWO].growthRate][14]);
    }
}

WILD_BATTLE_TEST("Convenience XP: level-up preserves Power Trick on normal and transformed battlers")
{
    bool32 transformed;
    PARAMETRIZE { transformed = FALSE; }
    PARAMETRIZE { transformed = TRUE; }
    ResetConvenienceCap();
    GIVEN {
        PLAYER(SPECIES_MEW) { Level(13); Speed(200); Moves(MOVE_TRANSFORM, MOVE_POWER_TRICK, MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_SHUCKLE) { Level(50); HP(1); Attack(100); Defense(400); Speed(1); Moves(MOVE_POWER_TRICK, MOVE_DRAGON_RAGE, MOVE_SPLASH); }
    } WHEN {
        if (transformed)
        {
            TURN { MOVE(player, MOVE_TRANSFORM); MOVE(opponent, MOVE_SPLASH); }
            TURN { MOVE(player, moveSlot: 0); MOVE(opponent, MOVE_SPLASH); }
            TURN { MOVE(player, moveSlot: 1); MOVE(opponent, MOVE_SPLASH); }
        }
        else
        {
            TURN { MOVE(player, MOVE_POWER_TRICK); MOVE(opponent, MOVE_SPLASH); }
            TURN { MOVE(player, MOVE_DRAGON_RAGE); MOVE(opponent, MOVE_SPLASH); }
        }
    } SCENE {
        EXPERIENCE_BAR(player);
    } THEN {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 14);
        EXPECT(gBattleMons[B_BATTLER_0].volatiles.powerTrick);
        EXPECT_EQ(gBattleMons[B_BATTLER_0].maxHP, GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MAX_HP));
        EXPECT_EQ(gBattleMons[B_BATTLER_0].attack, transformed ? 400 : GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_DEF));
        EXPECT_EQ(gBattleMons[B_BATTLER_0].defense, transformed ? 100 : GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK));
    }
}

WILD_BATTLE_TEST("Convenience XP: level-up preserves Speed Swap while normal Speed updates")
{
    bool32 swapped;
    PARAMETRIZE { swapped = FALSE; }
    PARAMETRIZE { swapped = TRUE; }
    ResetConvenienceCap();
    GIVEN {
        PLAYER(SPECIES_MEW) { Level(13); Speed(200); Moves(MOVE_SPEED_SWAP, MOVE_DRAGON_RAGE, MOVE_SPLASH); }
        OPPONENT(SPECIES_SHUCKLE) { Level(50); HP(1); Speed(1); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, swapped ? MOVE_SPEED_SWAP : MOVE_SPLASH); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_DRAGON_RAGE); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        EXPERIENCE_BAR(player);
    } THEN {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
        EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), 14);
        EXPECT_EQ((bool32)gBattleMons[B_BATTLER_0].volatiles.speedSwapped, swapped);
        EXPECT_EQ(gBattleMons[B_BATTLER_0].speed, swapped ? 1 : GetMonData(mon, MON_DATA_SPEED));
        EXPECT_GT(GetMonData(mon, MON_DATA_SPEED), 1);
        EXPECT_EQ(gBattleMons[B_BATTLER_0].attack, GetMonData(mon, MON_DATA_ATK));
        EXPECT_EQ(gBattleMons[B_BATTLER_0].maxHP, GetMonData(mon, MON_DATA_MAX_HP));
    }
}
