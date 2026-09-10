// Temporary forced native mechanics, after AuthoredOpponent.
// This is not unrestricted AI-choice or pre-Lyle catch availability evidence.
#line 61000 "work/lyle_struggle_probe.inc"
#include "emerald_champions_battle_sets.h"

static void LyleStrugglePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species)
    {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon));
        Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
            GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

DOUBLE_BATTLE_TEST("EC live review Lyle Struggle Bug: native slow special denial and fast negative", s16 specialDamage)
{
    u32 mode;
    bool32 fast;
    PARAMETRIZE { fast = FALSE; mode = 0; }
    PARAMETRIZE { fast = FALSE; mode = 1; }
    PARAMETRIZE { fast = FALSE; mode = 2; }
    PARAMETRIZE { fast = TRUE; mode = 0; }
    PARAMETRIZE { fast = TRUE; mode = 1; }
    PARAMETRIZE { fast = TRUE; mode = 2; }
    GIVEN {
        struct EmeraldChampionsBattleSet special = {
            .moves = {MOVE_ICE_BEAM, MOVE_WATER_PULSE, MOVE_REST, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_QUIET, .ability = ABILITY_THICK_FAT,
            .statPoints = {18, 0, fast ? 0 : 32, 16, 0, fast ? 32 : 0},
        };
        struct EmeraldChampionsBattleSet guard = {
            .moves = {MOVE_ICE_BEAM, MOVE_WATER_PULSE, MOVE_REST, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
            .statPoints = {32, 0, 18, 16, 0, 0},
        };
        // Munchlax is a native Route 116 encounter, not an asserted pre-Lyle
        // catch. Both species natively teach all four listed moves. The two
        // Munchlax variants keep HP/SpA/nature identical, trading Def for Speed.
        LyleStrugglePlayer(SPECIES_MUNCHLAX, &special);
        LyleStrugglePlayer(SPECIES_LOTAD, &guard);
        AuthoredOpponent(TRAINER_LYLE, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_SEWADDLE);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_MOVE3), MOVE_STRUGGLE_BUG);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_DOTTLER);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_MOVE2), MOVE_STRUGGLE_BUG);
        struct Pokemon saved[4];
        memcpy(saved, party, sizeof(saved));
        party[0] = saved[1];
        party[1] = saved[3];
        party[2] = saved[0];
        party[3] = saved[2];
        u16 zero = 0;
        SetMonData(&party[2], MON_DATA_HP, &zero);
        SetMonData(&party[3], MON_DATA_HP, &zero);
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
        struct Pokemon *player = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][0];
        for (u32 slot = 0; slot < 2; slot++)
        {
            if (fast)
                EXPECT_GT(GetMonData(player, MON_DATA_SPEED), GetMonData(&party[slot], MON_DATA_SPEED));
            else
                EXPECT_LT(GetMonData(player, MON_DATA_SPEED), GetMonData(&party[slot], MON_DATA_SPEED));
        }
        Test_MgbaPrintf("LYLE_STRUGGLE_INPUT fast=%d mode=%d playerHP=%d SpA=%d Speed=%d SewSpeed=%d DottSpeed=%d DottHP=%d SpD=%d",
            fast, mode, GetMonData(player, MON_DATA_HP), GetMonData(player, MON_DATA_SPATK), GetMonData(player, MON_DATA_SPEED),
            GetMonData(&party[0], MON_DATA_SPEED), GetMonData(&party[1], MON_DATA_SPEED),
            GetMonData(&party[1], MON_DATA_HP), GetMonData(&party[1], MON_DATA_SPDEF));
    } WHEN {
        TURN {
            MOVE(playerRight, MOVE_PROTECT);
            if (mode == 1)
                MOVE(opponentLeft, MOVE_STRUGGLE_BUG, hit: TRUE, criticalHit: FALSE);
            else
                MOVE(opponentLeft, MOVE_PROTECT);
            if (mode == 2)
                MOVE(opponentRight, MOVE_STRUGGLE_BUG, hit: TRUE, criticalHit: FALSE);
            else
                MOVE(opponentRight, MOVE_REFLECT);
            MOVE(playerLeft, MOVE_ICE_BEAM, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (fast)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_BEAM, playerLeft);
            HP_BAR(opponentRight, captureDamage: &results[i].specialDamage);
        }
        if (mode > 0)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_STRUGGLE_BUG, mode == 1 ? opponentLeft : opponentRight);
            HP_BAR(playerLeft);
        }
        if (!fast)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_BEAM, playerLeft);
            HP_BAR(opponentRight, captureDamage: &results[i].specialDamage);
        }
    } THEN {
        EXPECT_GT(playerLeft->hp, 0);
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_GT(results[i].specialDamage, 0);
        EXPECT_EQ(playerLeft->statStages[STAT_SPATK], mode == 0 ? DEFAULT_STAT_STAGE : DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(gLastMoves[B_BATTLER_0], MOVE_ICE_BEAM);
        if (mode > 0)
            EXPECT_EQ(gLastMoves[mode == 1 ? B_BATTLER_1 : B_BATTLER_3], MOVE_STRUGGLE_BUG);
        Test_MgbaPrintf("LYLE_STRUGGLE_AFTER fast=%d mode=%d damage=%d playerHP=%d SpAStage=%d DottHP=%d SewLAST=%S DottLAST=%S",
            fast, mode, results[i].specialDamage, playerLeft->hp, playerLeft->statStages[STAT_SPATK], opponentRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]));
    } FINALLY {
        EXPECT_LT(results[1].specialDamage, results[0].specialDamage);
        EXPECT_LT(results[2].specialDamage, results[0].specialDamage);
        EXPECT_EQ(results[4].specialDamage, results[3].specialDamage);
        EXPECT_EQ(results[5].specialDamage, results[3].specialDamage);
        EXPECT_EQ(results[0].specialDamage, results[3].specialDamage);
    }
}
