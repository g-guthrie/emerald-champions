#include "global.h"
#include "test/battle.h"
#include "constants/opponents.h"

AI_DOUBLE_BATTLE_TEST("EC Quash: Tailwind cannot rescue an attacker moved behind its knockout")
{
    bool32 quash, priority;
    bool32 dark = FALSE;
    PARAMETRIZE { quash = TRUE; priority = TRUE; }
    PARAMETRIZE { quash = FALSE; priority = TRUE; }
    PARAMETRIZE { quash = TRUE; priority = FALSE; }
    PARAMETRIZE { quash = TRUE; priority = TRUE; dark = TRUE; }
    GIVEN {
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_JOCELYN;
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        // Jocelyn's observed turn-two interaction, narrowed to a guaranteed
        // damage race so a damage roll cannot justify the losing attack.
        PLAYER(SPECIES_AERODACTYL) {
            Level(20); HP(28); MaxHP(68); Attack(65); Defense(37);
            SpAttack(31); SpDefense(41); Speed(82);
            Ability(ABILITY_UNNERVE); Moves(MOVE_DUAL_WINGBEAT);
        }
        PLAYER(SPECIES_SABLEYE) {
            Level(20); HP(76); MaxHP(76); Defense(43); Speed(31);
            Ability(priority ? ABILITY_PRANKSTER : ABILITY_KEEN_EYE);
            Moves(MOVE_QUASH, MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WHIMSICOTT) {
            Level(20); HP(72); MaxHP(72); Speed(77);
            Ability(ABILITY_PRANKSTER); Moves(MOVE_TAILWIND);
        }
        OPPONENT(dark ? SPECIES_SCRAGGY : SPECIES_COMBUSKEN) {
            Level(20); HP(55); MaxHP(63); Attack(57); Defense(35);
            SpDefense(35); Speed(46); Ability(ABILITY_SPEED_BOOST);
            Item(ITEM_EVIOLITE); Moves(MOVE_DRAIN_PUNCH, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DUAL_WINGBEAT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, quash ? MOVE_QUASH : MOVE_CELEBRATE, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_TAILWIND);
            // A pending Quash is not knowable, so the same board must produce
            // the same choice; Quash's effect on the race is asserted below.
            EXPECT_MOVE(opponentRight, MOVE_DRAIN_PUNCH, criticalHit: FALSE);
        }
    } THEN {
        if (quash && priority && !dark)
        {
            // Quash still moves the attacker behind its knockout; without the
            // read the target no longer answers with a perfect shield.
            EXPECT_EQ(playerLeft->hp, 28);
        }
        else
        {
            EXPECT_EQ(playerLeft->hp, 0);
            EXPECT_GT(opponentRight->hp, 0);
        }
    }
}
