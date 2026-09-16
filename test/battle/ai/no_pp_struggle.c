#include "global.h"
#include "test/battle.h"

#define NO_PP_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

// A human answers a refused selection by picking something else. An AI battler
// answers it with the same move, so a refusal that sends it back to the menu
// has to change its options or the turn never completes.
AI_DOUBLE_BATTLE_TEST("EC no PP: an AI flank with every move spent struggles instead of reselecting")
{
    u32 moves;
    PARAMETRIZE { moves = 2; }
    PARAMETRIZE { moves = 4; }
    GIVEN {
        AI_FLAGS(NO_PP_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(500); MaxHP(500); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(500); MaxHP(500); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_NOSEPASS) {
            Level(30); HP(200); MaxHP(200); Speed(40); Ability(ABILITY_MAGNET_PULL);
            // A partly filled move array is the shape the campaign's authored
            // sets actually take, so cover both it and a full one.
            if (moves == 2)
                MovesWithPP({MOVE_POWER_GEM, 0}, {MOVE_PROTECT, 0});
            else
                MovesWithPP({MOVE_POWER_GEM, 0}, {MOVE_PROTECT, 0}, {MOVE_ROCK_SLIDE, 0}, {MOVE_THUNDER_WAVE, 0});
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(20); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRUGGLE, opponentLeft);
    } THEN {
        EXPECT(playerLeft->hp < 500 || playerRight->hp < 500);
        EXPECT(opponentLeft->hp < 200);
    }
}

// Only the spent slots are refused: the AI must move its choice to a move it
// can still select rather than struggling or stalling on the empty one.
AI_DOUBLE_BATTLE_TEST("EC no PP: an AI flank with one move left selects that move")
{
    GIVEN {
        AI_FLAGS(NO_PP_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(500); MaxHP(500); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(500); MaxHP(500); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_NOSEPASS) {
            Level(30); HP(200); MaxHP(200); SpAttack(120); Speed(40); Ability(ABILITY_MAGNET_PULL);
            MovesWithPP({MOVE_POWER_GEM, 0}, {MOVE_PROTECT, 0}, {MOVE_THUNDERBOLT, 5});
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(20); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_THUNDERBOLT);
        }
    } THEN {
        EXPECT(playerLeft->hp < 500 || playerRight->hp < 500);
    }
}
