#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Flinch: Inner Focus prevents Fake Out unless Mold Breaker bypasses it")
{
    enum Ability attackerAbility, defenderAbility;
    PARAMETRIZE { attackerAbility = ABILITY_IRON_FIST; defenderAbility = ABILITY_NONE; }
    PARAMETRIZE { attackerAbility = ABILITY_IRON_FIST; defenderAbility = ABILITY_INNER_FOCUS; }
    PARAMETRIZE { attackerAbility = ABILITY_MOLD_BREAKER; defenderAbility = ABILITY_NONE; }
    PARAMETRIZE { attackerAbility = ABILITY_MOLD_BREAKER; defenderAbility = ABILITY_INNER_FOCUS; }
    GIVEN {
        PLAYER(SPECIES_PANGORO) { Ability(attackerAbility); Moves(MOVE_FAKE_OUT); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(defenderAbility); HP(500); MaxHP(500); Moves(MOVE_SWORDS_DANCE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FAKE_OUT); MOVE(opponent, MOVE_SWORDS_DANCE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], defenderAbility == ABILITY_INNER_FOCUS
                  && attackerAbility != ABILITY_MOLD_BREAKER ? DEFAULT_STAT_STAGE + 2 : DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Flinch: two Fake Outs deny one action and trigger Steadfast once")
{
    GIVEN {
        PLAYER(SPECIES_PANGORO) { Moves(MOVE_FAKE_OUT, MOVE_CELEBRATE); }
        PLAYER(SPECIES_PANGORO) { Moves(MOVE_FAKE_OUT, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_STEADFAST); HP(500); MaxHP(500); Moves(MOVE_SWORDS_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft); MOVE(playerRight, MOVE_FAKE_OUT, target: opponentLeft); MOVE(opponentLeft, MOVE_SWORDS_DANCE); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_SWORDS_DANCE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}
