#include "global.h"
#include "test/battle.h"

// Group E: what a voluntary switch costs. A position that cannot be held is
// worth the turn it takes to leave; a healthy one under fire is not, and the
// difference must not be "the foe hits hard", which is true of every board.
#define SWITCH_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC switch pricing: a healthy wall under fire holds the turn it would give away")
{
    GIVEN {
        AI_FLAGS(SWITCH_FLAGS);
        // The lead takes well over half from the attacker opposite, which is
        // what the reserve search looks for - but it is at full health on turn
        // one, it has a real attack, and nothing about the bench is known to
        // be better. Leaving is a guess paid for with the whole turn.
        PLAYER(SPECIES_MACHAMP) { Level(30); HP(300); MaxHP(300); Attack(300); Speed(200); Ability(ABILITY_GUTS); Moves(MOVE_CROSS_CHOP); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_NOSEPASS) { Level(30); HP(120); MaxHP(120); Defense(90); Speed(20); Ability(ABILITY_STURDY); Moves(MOVE_ROCK_SLIDE, MOVE_THUNDER_WAVE); }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_NACLSTACK) { Level(30); HP(300); MaxHP(300); Defense(120); Speed(30); Moves(MOVE_SALT_CURE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CROSS_CHOP, target: opponentLeft);
            MOVE(playerRight, MOVE_SPLASH);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_NOSEPASS);
    }
}

AI_DOUBLE_BATTLE_TEST("EC switch pricing: a Truant body leaves when it is walled, not because it loafs")
{
    bool32 walled;
    PARAMETRIZE { walled = TRUE; }
    PARAMETRIZE { walled = FALSE; }
    GIVEN {
        AI_FLAGS(SWITCH_FLAGS);
        // Truant waives the toll on the exit, but the exit still has to be
        // worth taking. Against a Ghost the Normal attack does nothing at all
        // and the bench is strictly better; against a body it can hit, the
        // loafing turn is not a reason to hand the matchup away.
        PLAYER(walled ? SPECIES_GENGAR : SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(60); Defense(200); SpDefense(200); Speed(200); Moves(MOVE_TACKLE); }
        PLAYER(walled ? SPECIES_HAUNTER : SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_SLAKOTH) { Level(30); HP(300); MaxHP(300); Attack(150); Defense(100); Speed(60); Ability(ABILITY_TRUANT); Moves(MOVE_SLASH); }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_HAUNTER) { Level(30); HP(300); MaxHP(300); SpAttack(150); Speed(90); Moves(MOVE_SHADOW_BALL); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_SPLASH);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, walled ? SPECIES_HAUNTER : SPECIES_SLAKOTH);
    }
}
