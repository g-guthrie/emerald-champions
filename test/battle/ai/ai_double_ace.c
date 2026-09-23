#include "global.h"
#include "test/battle.h"

ASSUMPTIONS {
    ASSUME(GetMoveEffect(MOVE_U_TURN) == EFFECT_HIT_ESCAPE);
    ASSUME(GetMoveType(MOVE_CRUNCH) == TYPE_DARK);
    ASSUME(GetSpeciesType(SPECIES_WOBBUFFET, 0) == TYPE_PSYCHIC);
    ASSUME(GetSpeciesType(SPECIES_WOBBUFFET, 1) == TYPE_PSYCHIC);
}

// U-Turn into the Aces: with SMART_MON_CHOICES the pair planner owns doubles replacements; "last Ace first by level" no longer applies.

AI_DOUBLE_BATTLE_TEST("AI_FLAG_DOUBLE_ACE_POKEMON: sends out Ace mons when no other options remain mid-battle")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_SMART_SWITCHING | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_DOUBLE_ACE_POKEMON);

        PLAYER(SPECIES_WOBBUFFET) { Level(50); Speed(200); Moves(MOVE_THUNDERBOLT, MOVE_CELEBRATE); SpAttack(200); }
        PLAYER(SPECIES_WOBBUFFET) { Level(50); Speed(150); Moves(MOVE_THUNDERBOLT, MOVE_CELEBRATE); SpAttack(200); }

        OPPONENT(SPECIES_ZIGZAGOON) { Level(5); HP(1); Speed(1); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_POOCHYENA) { Level(5); HP(1); Speed(1); Moves(MOVE_SPLASH); }

        // Aces
        OPPONENT(SPECIES_MIGHTYENA) { Level(50); Speed(10); Moves(MOVE_CRUNCH); }
        OPPONENT(SPECIES_GENGAR) { Level(50); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_SPLASH);
            EXPECT_MOVE(opponentRight, MOVE_SPLASH);
            // The pair planner picks the Ace by matchup, not party order:
            // Mightyena's Crunch threatens the Wobbuffets, Gengar only Splashes.
            EXPECT_SEND_OUT(opponentLeft, 2);
        }
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_CRUNCH);
            EXPECT_MOVE(opponentRight, MOVE_SPLASH);
            EXPECT_SEND_OUT(opponentRight, 3);
        }
    }
}
