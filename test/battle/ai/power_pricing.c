#include "global.h"
#include "test/battle.h"

// A set's signature move is usually its best turn. These are the four shapes
// where something cheaper was winning instead, all from real per-turn play.
#define POWER_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC power pricing: Water Spout at full health beats the small reliable attack")
{
    GIVEN {
        AI_FLAGS(POWER_FLAGS);
        PLAYER(SPECIES_MACHOP) { Level(30); HP(300); MaxHP(300); Attack(60); Defense(120); SpDefense(120); Speed(20); Moves(MOVE_BRICK_BREAK); }
        PLAYER(SPECIES_MACHOP) { Level(30); HP(300); MaxHP(300); Attack(60); Defense(120); SpDefense(120); Speed(15); Moves(MOVE_BRICK_BREAK); }
        OPPONENT(SPECIES_WAILMER) {
            Level(30); HP(250); MaxHP(250); SpAttack(120); Defense(80); SpDefense(80); Speed(60);
            Ability(ABILITY_WATER_VEIL); Moves(MOVE_WATER_SPOUT, MOVE_SCALD);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BRICK_BREAK, target: opponentLeft);
            MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentLeft);
            EXPECT_MOVE(opponentLeft, MOVE_WATER_SPOUT);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC power pricing: a max-power Reversal at low health beats waiting behind a shield")
{
    GIVEN {
        AI_FLAGS(POWER_FLAGS);
        // Five HP left is exactly where this move is at its strongest, and the
        // target is weak to it: the shield only postpones the same end.
        PLAYER(SPECIES_MACHOP) { Level(30); HP(60); MaxHP(60); Attack(60); Defense(40); SpDefense(40); Speed(20); Moves(MOVE_BRICK_BREAK); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOOLOO) {
            Level(30); HP(5); MaxHP(57); Attack(140); Defense(40); SpDefense(40); Speed(60);
            Ability(ABILITY_FLUFFY); Moves(MOVE_REVERSAL, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BRICK_BREAK, target: opponentLeft);
            MOVE(playerRight, MOVE_SPLASH);
            EXPECT_MOVE(opponentLeft, MOVE_REVERSAL, target: playerLeft);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC power pricing: priority buys nothing when the user already moves first")
{
    GIVEN {
        AI_FLAGS(POWER_FLAGS);
        // Already faster than the target, and the big move is the one that
        // ends it: striking first is not worth two thirds of the damage.
        PLAYER(SPECIES_MACHOP) { Level(30); HP(120); MaxHP(120); Attack(60); Defense(60); SpDefense(60); Speed(20); Moves(MOVE_BRICK_BREAK); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_BUNNELBY) {
            Level(30); HP(200); MaxHP(200); Attack(160); Speed(120);
            Ability(ABILITY_HUGE_POWER); Moves(MOVE_QUICK_ATTACK, MOVE_RETURN);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BRICK_BREAK, target: opponentLeft);
            MOVE(playerRight, MOVE_SPLASH);
            EXPECT_MOVE(opponentLeft, MOVE_RETURN, target: playerLeft);
        }
    } THEN {
        // The bigger move is the one that takes the most off; striking first
        // was already free.
        EXPECT(playerLeft->hp < 90);
    }
}

AI_DOUBLE_BATTLE_TEST("EC power pricing: recoil is not paid into a body that resists it")
{
    GIVEN {
        AI_FLAGS(POWER_FLAGS);
        // One target resists it and pays the user back in recoil for nothing;
        // the other is neutral and on the same board.
        PLAYER(SPECIES_MAWILE) { Level(30); HP(200); MaxHP(200); Defense(120); SpDefense(120); Speed(20); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MACHOP) { Level(30); HP(200); MaxHP(200); Defense(60); SpDefense(60); Speed(15); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ZUBAT) {
            Level(30); HP(28); MaxHP(65); Attack(120); Speed(80);
            Ability(ABILITY_INNER_FOCUS); Moves(MOVE_BRAVE_BIRD);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_BRAVE_BIRD, target: playerRight);
        }
    }
}
