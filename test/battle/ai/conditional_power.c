#include "global.h"
#include "test/battle.h"

// Group G: a move whose power depends on the target's current state has to be
// priced at the power it will actually have, and a guard that denies nothing
// has to lose to a move with a next-turn value.
#define COND_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC conditional power: Hex is worth its doubled power only against a status")
{
    bool32 statused;
    PARAMETRIZE { statused = TRUE; }
    PARAMETRIZE { statused = FALSE; }
    GIVEN {
        AI_FLAGS(COND_FLAGS);
        // Same user, same STAB, same target. Against a clean body Hex is the
        // weaker move and the reliable one has to win.
        // The two moves share type, accuracy and target, so only the
        // conditional power can decide between them.
        PLAYER(SPECIES_WAILMER) { Level(30); HP(300); MaxHP(300); Defense(120); SpDefense(120); Speed(20); Status1(statused ? STATUS1_POISON : STATUS1_NONE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_DRIFBLIM) {
            Level(30); HP(200); MaxHP(200); SpAttack(100); Speed(80);
            Ability(ABILITY_AFTERMATH); Moves(MOVE_HEX, MOVE_SHADOW_BALL);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(5); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_SPLASH);
            if (statused)
                EXPECT_MOVE(opponentLeft, MOVE_HEX, target: playerLeft);
            else
                EXPECT_MOVE(opponentLeft, MOVE_SHADOW_BALL, target: playerLeft);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC conditional power: a free turn at full health is spent on Spore, not a shield")
{
    GIVEN {
        AI_FLAGS(COND_FLAGS);
        // Parasect's board: full health, nothing aimed at it this turn, and a
        // hundred-accuracy sleep available. A guard denies nothing here and
        // the sleep is worth every turn the target now loses.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(200); Defense(200); SpDefense(200); Speed(200); Ability(ABILITY_INSOMNIA); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_MACHAMP) { Level(30); HP(400); MaxHP(400); Attack(150); Defense(200); SpDefense(200); Speed(150); Ability(ABILITY_GUTS); Moves(MOVE_KARATE_CHOP); }
        OPPONENT(SPECIES_PARASECT) {
            Level(30); HP(200); MaxHP(200); Defense(90); Speed(30);
            Ability(ABILITY_EFFECT_SPORE); Moves(MOVE_SPORE, MOVE_PROTECT, MOVE_FURY_CUTTER);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_KARATE_CHOP, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_SPORE, target: playerRight);
        }
    }
}
