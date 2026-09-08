#include "global.h"
#include "test/battle.h"

AI_DOUBLE_BATTLE_TEST("EC expert pair: two eligible partners commit only one Mega for their owner")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PREDICTION | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO
            | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(400); MaxHP(400); Defense(200); SpDefense(200); Attack(1); Speed(60); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(400); MaxHP(400); Defense(200); SpDefense(200); Attack(1); Speed(50); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_MAWILE) { Level(50); HP(300); MaxHP(300); Attack(150); Speed(130); Item(ITEM_MAWILITE); Moves(MOVE_IRON_HEAD); }
        OPPONENT(SPECIES_VENUSAUR) { Level(50); HP(300); MaxHP(300); SpAttack(100); Speed(120); Item(ITEM_VENUSAURITE); Moves(MOVE_SLUDGE_BOMB); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_IRON_HEAD, gimmick: GIMMICK_MEGA);
            EXPECT_MOVE(opponentRight, MOVE_SLUDGE_BOMB, gimmick: GIMMICK_NONE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponentLeft);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponentRight);
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_MAWILE_MEGA);
        EXPECT_EQ(opponentRight->species, SPECIES_VENUSAUR);
    }
}
