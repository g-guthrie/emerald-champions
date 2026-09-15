#include "global.h"
#include "test/battle.h"
#include "constants/opponents.h"

AI_DOUBLE_BATTLE_TEST("EC Coaching: a flinched recipient must survive to earn the authored future setup reward")
{
    bool32 partnerSurvives;
    PARAMETRIZE { partnerSurvives = FALSE; }
    PARAMETRIZE { partnerSurvives = TRUE; }
    GIVEN {
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_BRENDEN;
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_SHAYMIN) {
            Level(20); HP(76); MaxHP(76); Attack(45); Defense(51);
            SpAttack(69); SpDefense(51); Speed(63);
            Ability(ABILITY_NATURAL_CURE); Item(ITEM_CHOICE_SPECS);
            // The recipient's survival has to be readable from the board, not
            // from the pending command: give the threat only when it exists.
            if (partnerSurvives)
                Moves(MOVE_CELEBRATE);
            else
                Moves(MOVE_CELEBRATE, MOVE_EARTH_POWER, MOVE_GIGA_DRAIN, MOVE_PSYCHIC);
        }
        PLAYER(SPECIES_MIENFOO) {
            Level(20); HP(54); MaxHP(54); Attack(62); Defense(31);
            SpAttack(29); SpDefense(31); Speed(49);
            Ability(ABILITY_INNER_FOCUS); Item(ITEM_EVIOLITE);
            Moves(MOVE_FAKE_OUT, MOVE_HELPING_HAND, MOVE_DRAIN_PUNCH, MOVE_BRICK_BREAK);
        }
        OPPONENT(SPECIES_SAWK) {
            Level(20); HP(47); MaxHP(66); Attack(80); Defense(41);
            SpAttack(20); SpDefense(41); Speed(57);
            Ability(ABILITY_MOLD_BREAKER); Item(ITEM_CHOICE_SCARF);
            Moves(MOVE_CLOSE_COMBAT, MOVE_ROCK_SLIDE, MOVE_POISON_JAB, MOVE_ICE_PUNCH);
        }
        OPPONENT(SPECIES_RIOLU) {
            Level(20); HP(24); MaxHP(64); Attack(39); Defense(27);
            SpAttack(22); SpDefense(27); Speed(51);
            Ability(ABILITY_PRANKSTER); Item(ITEM_FOCUS_SASH);
            Moves(MOVE_COACHING, MOVE_DRAIN_PUNCH, MOVE_BULLET_PUNCH, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            if (partnerSurvives)
                MOVE(playerLeft, MOVE_CELEBRATE);
            else
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft);
            MOVE(playerRight, MOVE_FAKE_OUT, target: opponentLeft);
            EXPECT_MOVE(opponentRight, partnerSurvives ? MOVE_COACHING : MOVE_DRAIN_PUNCH);
        }
    } THEN {
        if (partnerSurvives)
        {
            EXPECT_GT(opponentLeft->hp, 0);
            EXPECT_GT(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
            EXPECT_GT(opponentLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        }
        else
        {
            EXPECT_EQ(opponentLeft->hp, 0);
            EXPECT(playerLeft->hp < 76 || playerRight->hp < 54);
        }
    }
}
