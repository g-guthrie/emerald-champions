// Temporary native mechanics controls; include after allen_ai_probe.inc.
// Full current Allen party, native level-14/IV31 player preparation. Forced
// commands isolate immediate sleep timing, NOT unrestricted AI selection.
#line 20000 "work/allen_spore_controls.inc"
#include "battle_main.h"
#include "battle_util.h"

static const struct EmeraldChampionsBattleSet sAllenSporeLumTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_KNOCK_OFF, MOVE_PROTECT},
    .item = ITEM_LUM_BERRY, .nature = NATURE_BRAVE, .ability = ABILITY_IRON_FIST,
    .statPoints = {32, 32, 2, 0, 0, 0},
};
static const struct EmeraldChampionsBattleSet sAllenSporeEarlyNatu = {
    .moves = {MOVE_NIGHT_SHADE, MOVE_REFLECT, MOVE_ROOST, MOVE_PROTECT},
    .item = ITEM_IRON_BALL, .nature = NATURE_RELAXED, .ability = ABILITY_EARLY_BIRD,
    .statPoints = {32, 0, 28, 0, 0, 6},
};
static const struct EmeraldChampionsBattleSet sAllenSporeTalkTimburr = {
    // Low-power special damage or a status call avoids a replacement confound.
    // This legal diagnostic moveset is not a recommended campaign player set.
    .moves = {MOVE_ROUND, MOVE_BULK_UP, MOVE_PROTECT, MOVE_SLEEP_TALK},
    .item = ITEM_EVIOLITE, .nature = NATURE_BRAVE, .ability = ABILITY_IRON_FIST,
    .statPoints = {32, 32, 2, 0, 0, 0},
};

DOUBLE_BATTLE_TEST("EC live review Allen Spore controls: Lum, Early Bird and Sleep Talk")
{
    u32 control;
    PARAMETRIZE { control = 0; } // Lum: sleep is cured before the pending action.
    PARAMETRIZE { control = 1; } // Early Bird + native counter 2: immediate wake.
    PARAMETRIZE { control = 2; } // Early Bird + native counter 3: action denied.
    PARAMETRIZE { control = 3; } // Sleep Talk: asleep does not mean actionless.
    GIVEN {
        if (control == 0)
            AllenProbePlayer(SPECIES_TIMBURR, &sAllenSporeLumTimburr);
        else if (control == 3)
            AllenProbePlayer(SPECIES_TIMBURR, &sAllenSporeTalkTimburr);
        else
            AllenProbePlayer(SPECIES_NATU, &sAllenSporeEarlyNatu);
        AllenProbePlayer(SPECIES_PACHIRISU, &sAllenProbePachirisu);
        AuthoredOpponent(TRAINER_ALLEN, 0, FALSE);
        struct Pokemon *players = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
        struct Pokemon *enemies = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        u32 nativeSpeed = GetMonData(&players[0], MON_DATA_SPEED);
        u32 parasSpeed = GetMonData(&enemies[1], MON_DATA_SPEED);
        u32 divisor = (control == 1 || control == 2) ? 2 : 1;
        EXPECT_EQ(GetMonData(&enemies[0], MON_DATA_SPECIES), SPECIES_PIKACHU);
        EXPECT_EQ(GetMonData(&enemies[1], MON_DATA_SPECIES), SPECIES_PARAS);
        EXPECT_EQ(GetMonData(&enemies[1], MON_DATA_MOVE2), MOVE_WIDE_GUARD);
        // Native speed stages are applied before the Iron Ball division.
        // Derive only this item's/stage's comparison from generated stats;
        // no injected Speed, ability, or HP overrides are used.
        EXPECT_GT(nativeSpeed / divisor, parasSpeed);
        EXPECT_LT(nativeSpeed * 2 / 3 / divisor, parasSpeed);
        EXPECT_LT(nativeSpeed / divisor, GetMonData(&enemies[0], MON_DATA_SPEED));
        EXPECT_EQ(GetMonAbility(&players[0]), divisor == 2 ? ABILITY_EARLY_BIRD : ABILITY_IRON_FIST);
        Test_MgbaPrintf("Allen Spore input control=%d species=%d HP=%d rawSpeed=%d beforeWeb=%d afterWeb=%d ParasSpeed=%d ability=%d item=%d",
            control, GetMonData(&players[0], MON_DATA_SPECIES), GetMonData(&players[0], MON_DATA_MAX_HP),
            nativeSpeed, nativeSpeed / divisor, nativeSpeed * 2 / 3 / divisor, parasSpeed,
            GetMonAbility(&players[0]), GetMonData(&players[0], MON_DATA_HELD_ITEM));
    } WHEN {
        TURN {
            MOVE(playerRight, MOVE_PROTECT);
            if (control == 0)
                MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            else if (control == 3)
                MOVE(playerLeft, MOVE_SLEEP_TALK);
            else
                MOVE(playerLeft, MOVE_NIGHT_SHADE, target: opponentRight);
            MOVE(opponentLeft, MOVE_ELECTROWEB, hit: TRUE, criticalHit: FALSE);
            // Current Champions chooses counter 2 with weight 1 and counter 3
            // with weight 2. Cover both Early Bird outcomes deterministically;
            // do not infer a win rate or guaranteed denial from either sample.
            MOVE(opponentRight, MOVE_SPORE, target: playerLeft,
                WITH_RNG(RNG_SLEEP_TURNS, control == 1 ? 2 : 3));
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ELECTROWEB, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPORE, opponentRight);
        if (control == 0)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAIN_PUNCH, playerLeft);
        else if (control == 1)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_NIGHT_SHADE, playerLeft);
        else if (control == 2)
        {
            NONE_OF { ANIMATION(ANIM_TYPE_MOVE, MOVE_NIGHT_SHADE, playerLeft); }
        }
        else
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SLEEP_TALK, playerLeft);
    } THEN {
        EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_ELECTROWEB);
        EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_SPORE);
        EXPECT_EQ(playerLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        EXPECT_LT(GetBattlerTotalSpeedStat(B_BATTLER_0, GetBattlerAbility(B_BATTLER_0), GetBattlerHoldEffect(B_BATTLER_0)),
            GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)));
        EXPECT_GT(playerLeft->hp, 0);
        EXPECT_GT(opponentLeft->hp, 0);
        EXPECT_GT(opponentRight->hp, 0);
        if (control == 0)
        {
            EXPECT_EQ(playerLeft->status1 & STATUS1_SLEEP, 0);
            EXPECT_EQ(playerLeft->item, ITEM_NONE);
            EXPECT_EQ(gLastMoves[B_BATTLER_0], MOVE_DRAIN_PUNCH);
            EXPECT_LT(opponentRight->hp, opponentRight->maxHP);
        }
        else if (control == 1)
        {
            EXPECT_EQ(playerLeft->status1 & STATUS1_SLEEP, 0);
            EXPECT_EQ(gLastMoves[B_BATTLER_0], MOVE_NIGHT_SHADE);
            EXPECT_EQ(opponentRight->maxHP - opponentRight->hp, 14);
        }
        else if (control == 2)
        {
            EXPECT_EQ(playerLeft->status1 & STATUS1_SLEEP, STATUS1_SLEEP_TURN(1));
            EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
        }
        else
        {
            EXPECT_EQ(playerLeft->status1 & STATUS1_SLEEP, STATUS1_SLEEP_TURN(2));
            // SCENE proves Sleep Talk executed. Its called move is deliberately
            // unrigged: no assertion that it always attacks or chooses one move.
        }
        Test_MgbaPrintf("Allen Spore result control=%d playerLAST=%S status=%d item=%d HP=%d ParasHP=%d effectiveSpeed=%d",
            control, GetMoveName(gLastMoves[B_BATTLER_0]), playerLeft->status1, playerLeft->item,
            playerLeft->hp, opponentRight->hp,
            GetBattlerTotalSpeedStat(B_BATTLER_0, GetBattlerAbility(B_BATTLER_0), GetBattlerHoldEffect(B_BATTLER_0)));
    }
}
