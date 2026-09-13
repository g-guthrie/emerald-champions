#include "global.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("Substitute damage messages require an affected target")
{
    bool32 hitSubstitute;
    PARAMETRIZE { hitSubstitute = FALSE; }
    PARAMETRIZE { hitSubstitute = TRUE; }
    GIVEN {
        PLAYER(SPECIES_PACHIRISU) { HP(300); MaxHP(300); SpAttack(50); SpDefense(50); Speed(100); }
        PLAYER(SPECIES_MIENFOO) { HP(300); MaxHP(300); Speed(1); }
        OPPONENT(SPECIES_GIMMIGHOUL) { HP(300); MaxHP(300); SpAttack(50); SpDefense(50); Speed(30); }
        OPPONENT(SPECIES_PORYGON) { HP(300); MaxHP(300); Speed(40); }
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_SUBSTITUTE); }
        TURN {
            if (hitSubstitute)
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft);
            else
                MOVE(opponentLeft, MOVE_SHADOW_BALL, target: playerLeft);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, opponentLeft);
        MESSAGE("Mienfoo used Celebrate!");
        if (hitSubstitute) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERBOLT, playerLeft);
            MESSAGE("The substitute took damage for the opposing Gimmighoul!");
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SHADOW_BALL, opponentLeft);
            NOT MESSAGE("The substitute took damage for Pachirisu!");
        }
        MESSAGE("Mienfoo used Celebrate!");
    } THEN {
        EXPECT_EQ(opponentLeft->hp, 225);
        if (hitSubstitute)
            EXPECT_EQ(playerLeft->hp, 300);
        else
            EXPECT_LT(playerLeft->hp, 300);
    }
}
