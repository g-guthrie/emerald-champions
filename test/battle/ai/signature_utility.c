#include "global.h"
#include "test/battle.h"

// Group H, from the Route 111 to Mt. Chimney receipts: signature utility moves
// that never fired, and three damage-choice reports from the same block.
#define UTIL_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC signature utility: Octolock is the plan on a board that cannot punish it")
{
    GIVEN {
        AI_FLAGS(UTIL_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(5); Defense(200); SpDefense(200); Speed(20); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Attack(5); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_GRAPPLOCT) {
            Level(30); HP(300); MaxHP(300); Attack(90); Defense(90); Speed(40);
            Ability(ABILITY_LIMBER); Moves(MOVE_OCTOLOCK, MOVE_ROCK_SMASH);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_OCTOLOCK, target: playerLeft);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC signature utility: Destiny Bond at one HP takes the trade it is holding")
{
    GIVEN {
        AI_FLAGS(UTIL_FLAGS);
        // Fixed damage, seen once, against a body at exactly one HP: the next
        // one is certain and lands first.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(200); Moves(MOVE_NIGHT_SHADE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_FROSLASS) {
            Level(30); HP(31); MaxHP(31); SpAttack(90); Speed(60);
            Ability(ABILITY_SNOW_CLOAK); Moves(MOVE_DESTINY_BOND, MOVE_ICE_BEAM);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_NIGHT_SHADE, target: opponentLeft);
            MOVE(playerRight, MOVE_SPLASH);
        }
        TURN {
            MOVE(playerLeft, MOVE_NIGHT_SHADE, target: opponentLeft);
            MOVE(playerRight, MOVE_SPLASH);
            EXPECT_MOVE(opponentLeft, MOVE_DESTINY_BOND);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC signature utility: a support body spends its turn on Thunder Wave")
{
    GIVEN {
        AI_FLAGS(UTIL_FLAGS);
        // The support member's own attack is nothing; the paralysis is the
        // contribution, and it lands on the fast threat.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Attack(150); Defense(120); SpDefense(120); Speed(200); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Attack(5); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_DRAGONAIR) {
            Level(30); HP(300); MaxHP(300); Attack(40); SpAttack(40); Defense(90); Speed(50);
            Ability(ABILITY_SHED_SKIN); Moves(MOVE_THUNDER_WAVE, MOVE_DRAGON_TAIL);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_THUNDER_WAVE, target: playerLeft);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC signature utility: Aurora Veil is taken while the snow is up")
{
    GIVEN {
        AI_FLAGS(UTIL_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Attack(150); Defense(120); SpDefense(120); Speed(200); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_MACHAMP) { Level(30); HP(300); MaxHP(300); Attack(150); Defense(120); SpDefense(120); Speed(150); Ability(ABILITY_GUTS); Moves(MOVE_KARATE_CHOP); }
        OPPONENT(SPECIES_NINETALES_ALOLA) {
            Level(30); HP(300); MaxHP(300); SpAttack(90); Defense(90); Speed(100);
            Ability(ABILITY_SNOW_WARNING); Moves(MOVE_AURORA_VEIL, MOVE_ICE_BEAM);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_KARATE_CHOP, target: opponentLeft);
            EXPECT_MOVE(opponentLeft, MOVE_AURORA_VEIL);
        }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_AURORA_VEIL);
    }
}

AI_DOUBLE_BATTLE_TEST("EC signature utility: Wide Guard answers the spread that is four times on it")
{
    GIVEN {
        AI_FLAGS(UTIL_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Attack(150); Defense(120); SpDefense(120); Speed(200); Moves(MOVE_ROCK_SLIDE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Attack(5); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_FROSMOTH) {
            Level(30); HP(200); MaxHP(200); SpAttack(90); Defense(60); Speed(80);
            Ability(ABILITY_ICE_SCALES); Moves(MOVE_WIDE_GUARD, MOVE_ICE_BEAM);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ROCK_SLIDE);
            MOVE(playerRight, MOVE_TACKLE, target: opponentLeft);
        }
        TURN {
            MOVE(playerLeft, MOVE_ROCK_SLIDE);
            MOVE(playerRight, MOVE_TACKLE, target: opponentLeft);
            EXPECT_MOVE(opponentLeft, MOVE_WIDE_GUARD);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC signature utility: the super-effective attack beats the recoil one")
{
    GIVEN {
        AI_FLAGS(UTIL_FLAGS);
        // Double-Edge and High Horsepower are the same power on paper; the
        // ground move is doubled here and costs the user nothing.
        PLAYER(SPECIES_MAGNEMITE) { Level(30); HP(300); MaxHP(300); Attack(5); Defense(120); SpDefense(120); Speed(20); Ability(ABILITY_STURDY); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Attack(5); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_SAWSBUCK) {
            Level(30); HP(300); MaxHP(300); Attack(120); Speed(90);
            Ability(ABILITY_SAP_SIPPER); Moves(MOVE_DOUBLE_EDGE, MOVE_HIGH_HORSEPOWER);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_HIGH_HORSEPOWER, target: playerLeft);
        }
    }
}
