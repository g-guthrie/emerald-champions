#include "global.h"
#include "test/battle.h"

AI_DOUBLE_BATTLE_TEST("EC Prankster burn: a timely burn preserves the attack that threatens the partner")
{
    bool32 burn, priority;
    enum Ability targetAbility = ABILITY_NO_GUARD;
    enum Item targetItem = ITEM_CHOICE_BAND;
    enum Move attack = MOVE_DYNAMIC_PUNCH;
    u32 playerHP = 45;
    bool32 reduced = TRUE;
    bool32 exhaustedFacade = FALSE;
    PARAMETRIZE { burn = TRUE; priority = TRUE; }
    PARAMETRIZE { burn = FALSE; priority = TRUE; }
    PARAMETRIZE { burn = TRUE; priority = FALSE; }
    // These abilities remove No Guard; use accurate damage so Protect is not
    // legitimately needed to cover a Dynamic Punch miss.
    PARAMETRIZE { burn = TRUE; priority = TRUE; targetAbility = ABILITY_GUTS; attack = MOVE_CLOSE_COMBAT; reduced = FALSE; }
    PARAMETRIZE { burn = TRUE; priority = TRUE; targetAbility = ABILITY_WATER_VEIL; attack = MOVE_CLOSE_COMBAT; reduced = FALSE; }
    PARAMETRIZE { burn = TRUE; priority = TRUE; targetItem = ITEM_LUM_BERRY; playerHP = 30; reduced = FALSE; }
    PARAMETRIZE { burn = TRUE; priority = TRUE; attack = MOVE_FACADE; playerHP = 25; reduced = FALSE; }
    PARAMETRIZE { burn = TRUE; priority = TRUE; exhaustedFacade = TRUE; }
    PARAMETRIZE { burn = TRUE; priority = TRUE; attack = MOVE_FACADE; playerHP = 45; reduced = FALSE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_MARSHTOMP) {
            Level(20); HP(playerHP); MaxHP(76); Attack(45); Defense(39);
            SpAttack(51); SpDefense(39); Speed(27);
            Ability(ABILITY_DAMP); Item(ITEM_EVIOLITE); Moves(MOVE_MUDDY_WATER);
        }
        PLAYER(SPECIES_SABLEYE) {
            Level(20); HP(76); MaxHP(76); Attack(41); Defense(43);
            SpAttack(33); SpDefense(56); Speed(31);
            Ability(priority ? ABILITY_PRANKSTER : ABILITY_KEEN_EYE); Moves(MOVE_WILL_O_WISP, MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_MACHAMP) {
            Level(20); HP(76); MaxHP(84); Attack(82); Defense(43);
            SpAttack(33); SpDefense(45); Speed(33);
            Ability(targetAbility); Item(targetItem);
            if (exhaustedFacade)
                MovesWithPP({attack, 20}, {MOVE_FACADE, 0});
            else
                Moves(attack);
        }
        OPPONENT(SPECIES_ANNIHILAPE) {
            Level(21); HP(8); MaxHP(96); Defense(45); SpAttack(28); SpDefense(49); Speed(49);
            Ability(ABILITY_VITAL_SPIRIT); Moves(MOVE_PROTECT, MOVE_SHADOW_BALL);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_MUDDY_WATER, hit: TRUE);
            MOVE(playerRight, burn ? MOVE_WILL_O_WISP : MOVE_CELEBRATE, target: opponentLeft, hit: TRUE);
            EXPECT_MOVE(opponentLeft, attack, hit: TRUE, criticalHit: FALSE);
            // The partner cannot see whether the burn is coming this turn, so
            // its own choice no longer depends on it. The burn mechanics below
            // are what this fixture tests.
            EXPECT_MOVE(opponentRight, MOVE_SHADOW_BALL);
        }
    } THEN {
        if (burn && priority && reduced)
            EXPECT_GT(playerLeft->hp, 0);
        else
            EXPECT_EQ(playerLeft->hp, 0);
        EXPECT_GT(opponentRight->hp, 0);
    }
}
