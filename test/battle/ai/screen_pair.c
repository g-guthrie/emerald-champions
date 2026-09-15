#include "global.h"
#include "test/battle.h"

AI_DOUBLE_BATTLE_TEST("EC new screens: a timely screen preserves the attack threatening the partner")
{
    enum Move screen = MOVE_LIGHT_SCREEN;
    bool32 timely = TRUE;
    bool32 cast = TRUE;
    bool32 snow = TRUE;
    bool32 bypass = FALSE;
    bool32 protected = TRUE;
    PARAMETRIZE { screen = MOVE_LIGHT_SCREEN; }
    PARAMETRIZE { screen = MOVE_LIGHT_SCREEN; timely = FALSE; }
    PARAMETRIZE { screen = MOVE_LIGHT_SCREEN; cast = FALSE; }
    PARAMETRIZE { screen = MOVE_AURORA_VEIL; }
    PARAMETRIZE { screen = MOVE_AURORA_VEIL; snow = FALSE; protected = FALSE; }
    PARAMETRIZE { screen = MOVE_LIGHT_SCREEN; bypass = TRUE; protected = FALSE; }
    PARAMETRIZE { screen = MOVE_AURORA_VEIL; bypass = TRUE; protected = FALSE; }
    PARAMETRIZE { screen = MOVE_REFLECT; protected = FALSE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_MARSHTOMP) {
            Level(20); HP(48); MaxHP(76); Attack(45); Defense(39);
            SpAttack(51); SpDefense(39); Speed(27);
            Ability(ABILITY_DAMP); Item(ITEM_EVIOLITE); Moves(MOVE_MUDDY_WATER);
        }
        PLAYER(SPECIES_NINETALES_ALOLA) {
            Level(20); HP(76); MaxHP(76); Defense(100); SpDefense(100);
            Speed(timely ? 70 : 10); Ability(snow ? ABILITY_SNOW_WARNING : ABILITY_SNOW_CLOAK);
            Moves(screen, MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_MACHAMP) {
            Level(20); HP(76); MaxHP(84); Attack(82); Defense(43);
            SpAttack(82); SpDefense(45); Speed(33);
            Ability(bypass ? ABILITY_INFILTRATOR : ABILITY_NO_GUARD);
            Item(ITEM_CHOICE_SPECS); Moves(MOVE_AURA_SPHERE);
        }
        OPPONENT(SPECIES_ANNIHILAPE) {
            Level(21); HP(8); MaxHP(96); Defense(45); SpAttack(28); SpDefense(49); Speed(49);
            Ability(ABILITY_VITAL_SPIRIT); Moves(MOVE_PROTECT, MOVE_SHADOW_BALL);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_MUDDY_WATER, hit: TRUE);
            MOVE(playerRight, cast ? screen : MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_AURA_SPHERE, criticalHit: FALSE);
            // Whether the screen is actually cast this turn is not knowable
            // at decision time; the guard follows the board, not the command.
            EXPECT_MOVE(opponentRight, timely && protected ? MOVE_PROTECT : MOVE_SHADOW_BALL);
        }
    } THEN {
        if (timely && protected)
            EXPECT_GT(playerLeft->hp, 0);
        else
            EXPECT_EQ(playerLeft->hp, 0);
        EXPECT_GT(opponentRight->hp, 0);
    }
}
