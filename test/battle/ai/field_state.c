#include "global.h"
#include "test/battle.h"

// Group M: choices that depend on state the AI has to read off the field -
// the weather a move's type resolves under, the stat a move borrows, and
// whether a denial move has anything to deny.
#define FIELD_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC field state: Weather Ball is the type the field makes it")
{
    bool32 sun;
    PARAMETRIZE { sun = TRUE; }
    PARAMETRIZE { sun = FALSE; }
    GIVEN {
        AI_FLAGS(FIELD_FLAGS);
        // Weather Ball is doubled in power by any weather; only its type
        // changes. Under the player's sun it is Fire into a Fire body and
        // halved, so the STAB attack has to win; under the player's rain it
        // is Water, neutral, and the strongest move on the set.
        PLAYER(sun ? SPECIES_TORKOAL : SPECIES_PELIPPER) { Level(30); HP(300); MaxHP(300); Defense(90); SpDefense(90); Speed(20); Ability(sun ? ABILITY_DROUGHT : ABILITY_DRIZZLE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_PELIPPER) {
            Level(30); HP(300); MaxHP(300); SpAttack(110); Speed(40);
            Ability(ABILITY_KEEN_EYE); Moves(MOVE_WEATHER_BALL, MOVE_AIR_SLASH);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_SPLASH);
            if (sun)
                NOT_EXPECT_MOVE(opponentLeft, MOVE_WEATHER_BALL);
            else
                EXPECT_MOVE(opponentLeft, MOVE_WEATHER_BALL, target: playerLeft);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC field state: Foul Play is worth the target's Attack, not the user's")
{
    bool32 strong;
    PARAMETRIZE { strong = TRUE; }
    PARAMETRIZE { strong = FALSE; }
    GIVEN {
        AI_FLAGS(FIELD_FLAGS);
        // Foul Play borrows the target's Attack. Against a special body that
        // barely has one it is the weaker move on the set.
        // A body that neither resists nor is weak to either move, so only the
        // borrowed Attack can decide between them.
        PLAYER(SPECIES_LANTURN) { Level(30); HP(300); MaxHP(300); Attack(strong ? 300 : 5); Defense(90); SpDefense(90); Speed(20); Ability(ABILITY_ILLUMINATE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_MANDIBUZZ) {
            Level(30); HP(300); MaxHP(300); Attack(60); SpAttack(90); Speed(40);
            Ability(ABILITY_OVERCOAT); Moves(MOVE_FOUL_PLAY, MOVE_AIR_SLASH);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_SPLASH);
            if (strong)
                EXPECT_MOVE(opponentLeft, MOVE_FOUL_PLAY, target: playerLeft);
            else
                NOT_EXPECT_MOVE(opponentLeft, MOVE_FOUL_PLAY);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC field state: a shield is not something Taunt is for")
{
    bool32 real;
    PARAMETRIZE { real = TRUE; }
    PARAMETRIZE { real = FALSE; }
    GIVEN {
        AI_FLAGS(FIELD_FLAGS);
        // Golbat's board: the only status move opposite is Protect, and there
        // is a real attack on the set. A body holding an actual status move
        // is still worth the turn.
        PLAYER(SPECIES_MAGNEZONE) { Level(30); HP(200); MaxHP(200); Defense(60); SpDefense(60); SpAttack(120); Speed(20); Ability(ABILITY_STURDY); Moves(real ? MOVE_THUNDER_WAVE : MOVE_PROTECT, MOVE_THUNDERBOLT); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Attack(5); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_GOLBAT) {
            Level(30); HP(200); MaxHP(200); Attack(90); Speed(90);
            Ability(ABILITY_INNER_FOCUS); Moves(MOVE_TAUNT, MOVE_WING_ATTACK);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            if (real)
                EXPECT_MOVE(opponentLeft, MOVE_TAUNT, target: playerLeft);
            else
                NOT_EXPECT_MOVE(opponentLeft, MOVE_TAUNT);
        }
    }
}
