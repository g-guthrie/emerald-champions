// Temporary candidate for one shared Wish regression, not a trainer catalogue.
// Explicit stats reproduce the executed HaleyWishBoard inputs; no trainer ID,
// preparation helper, authored tactic, or forced opponent command is used.
#line 60000 "work/haley_wish_shared_probe.inc"
#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_util.h"
#include "random.h"

AI_DOUBLE_BATTLE_TEST("EC expert pair: Wish survives modest pressure and pays at the next end turn", s16 healing)
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO
            | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(77); MaxHP(77); Attack(18); Defense(66);
            SpAttack(21); SpDefense(36); Speed(35); Nature(NATURE_BOLD);
            Ability(ABILITY_VOLT_ABSORB); Item(ITEM_EVIOLITE);
            Moves(MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT);
        }
        PLAYER(SPECIES_LOTAD) {
            Level(14); HP(71); MaxHP(71); Attack(15); Defense(53);
            SpAttack(20); SpDefense(25); Speed(17); Nature(NATURE_BOLD);
            Ability(ABILITY_RAIN_DISH); Item(ITEM_EVIOLITE);
            Moves(MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_REST, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_EEVEE) {
            Level(12); HP(35); MaxHP(70); Attack(58); Defense(22);
            SpAttack(17); SpDefense(24); Speed(21); Nature(NATURE_ADAMANT);
            Ability(ABILITY_ADAPTABILITY); Item(ITEM_EVIOLITE);
            Moves(MOVE_QUICK_ATTACK, MOVE_DOUBLE_EDGE, MOVE_WISH, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_RALTS) {
            Level(12); HP(34); MaxHP(34); Attack(12); Defense(14);
            SpAttack(51); SpDefense(17); Speed(55); Nature(NATURE_TIMID);
            Ability(ABILITY_TRACE); Item(ITEM_FOCUS_SASH);
            Moves(MOVE_PSYCHIC, MOVE_DAZZLING_GLEAM, MOVE_THUNDER_WAVE, MOVE_HELPING_HAND);
        }
        OPPONENT(SPECIES_MARILL) {
            Level(12); HP(74); MaxHP(74); Attack(49); Defense(22);
            SpAttack(11); SpDefense(20); Speed(18); Nature(NATURE_ADAMANT);
            Ability(ABILITY_HUGE_POWER); Item(ITEM_SITRUS_BERRY);
            Moves(MOVE_AQUA_JET, MOVE_PLAY_ROUGH, MOVE_BRICK_BREAK, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_SNUBBULL) {
            Level(12); HP(72); MaxHP(72); Attack(64); Defense(22);
            SpAttack(16); SpDefense(18); Speed(15); Nature(NATURE_ADAMANT);
            Ability(ABILITY_INTIMIDATE); Item(ITEM_LUM_BERRY);
            Moves(MOVE_PLAY_ROUGH, MOVE_FIRE_PUNCH, MOVE_THUNDER_WAVE, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentLeft, MOVE_WISH);
        }
        if (turns == 2)
        {
            TURN {
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_PROTECT);
                // Partner switching and the user's second action stay free;
                // this protects a healing payoff, not a prescribed script.
            }
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, opponentLeft);
        if (turns == 2)
        {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_WISH_HEAL, opponentLeft);
            HP_BAR(opponentLeft, captureDamage: &results[i].healing);
        }
    } THEN {
        EXPECT_EQ(gBattleStruct->wish[B_BATTLER_1].counter, turns == 1 ? 1 : 0);
        if (turns == 2)
        {
            EXPECT_EQ(results[i].healing, -35);
            EXPECT_GT(opponentLeft->hp, 35);
        }
        else
        {
            EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_WISH);
            // Same genuine post-Wish board, with only the pending counter
            // removed for the second query. No outer heap snapshot: both
            // evaluators restore their trial positions internally.
            rng_value_t rng = gRngValue, rng2 = gRng2Value;
            u16 counter = gBattleStruct->wish[B_BATTLER_1].counter;
            SetAiLogicDataForTurn(gAiLogicData);
            s32 dueValue = AI_EvaluateDoublesPosition(B_BATTLER_1, 0);
            gBattleStruct->wish[B_BATTLER_1].counter = 0;
            gRngValue = rng;
            gRng2Value = rng2;
            SetAiLogicDataForTurn(gAiLogicData);
            s32 absentValue = AI_EvaluateDoublesPosition(B_BATTLER_1, 0);
            gBattleStruct->wish[B_BATTLER_1].counter = counter;
            gRngValue = rng;
            gRng2Value = rng2;
            SetAiLogicDataForTurn(gAiLogicData);
            gRngValue = rng;
            gRng2Value = rng2;
            Test_MgbaPrintf("WISH_SHARED_PENDING dueValue=%d absentValue=%d HP=%d/%d counter=%d",
                dueValue, absentValue, opponentLeft->hp, opponentRight->hp, counter);
            EXPECT_GT(dueValue, absentValue);
        }
        Test_MgbaPrintf("WISH_SHARED_AFTER turns=%d healing=%d species=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S faints=%d/%d",
            turns, results[i].healing, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
    }
}
