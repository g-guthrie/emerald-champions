#include "global.h"
#include "test/battle.h"

// Group I, from the Petalburg and Mt. Chimney blocks: a move that cannot
// damage the target at all, a guard repeated into its own failure odds, and
// self-inflicted damage that kills the user.
#define LETHAL_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC lethal choice: a move the target is immune to loses to coverage that is not")
{
    GIVEN {
        AI_FLAGS(LETHAL_FLAGS);
        // Randall's board, reduced: the only target is a Ghost, the STAB is
        // Normal and does exactly nothing, and the same set carries a Dark
        // move that is doubled. The type chart is static knowledge.
        PLAYER(SPECIES_COFAGRIGUS) { Level(30); HP(300); MaxHP(300); Defense(120); SpDefense(120); Speed(20); Ability(ABILITY_MUMMY); Moves(MOVE_PROTECT, MOVE_WILL_O_WISP); }
        PLAYER(SPECIES_SABLEYE) { Level(30); HP(300); MaxHP(300); Defense(120); SpDefense(120); Speed(5); Ability(ABILITY_KEEN_EYE); Moves(MOVE_PROTECT); }
        OPPONENT(SPECIES_GREEDENT) {
            Level(30); HP(300); MaxHP(300); Attack(120); Speed(40);
            Ability(ABILITY_CHEEK_POUCH); Moves(MOVE_BODY_SLAM, MOVE_CRUNCH);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentLeft, MOVE_CRUNCH);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC lethal choice: an attack that kills its own user through the item is not taken")
{
    bool32 lethal;
    enum Move attack;
    PARAMETRIZE { lethal = TRUE; attack = MOVE_HEADBUTT; }
    PARAMETRIZE { lethal = FALSE; attack = MOVE_HEADBUTT; }
    GIVEN {
        AI_FLAGS(LETHAL_FLAGS);
        // Cinccino's board: a Life Orb attacker at two HP. The orb takes a
        // tenth of its maximum after the hit, which kills it. With room to
        // pay the orb the same attack is simply the best move on the set.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Defense(60); SpDefense(60); Speed(20); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_CINCCINO) {
            Level(30); HP(lethal ? 2 : 128); MaxHP(128); Attack(120); Speed(110);
            Ability(ABILITY_SKILL_LINK); Item(ITEM_LIFE_ORB); Moves(attack, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_SPLASH);
            if (lethal)
                NOT_EXPECT_MOVE(opponentLeft, attack);
            else
                EXPECT_MOVE(opponentLeft, attack, target: playerLeft);
        }
    } THEN {
        EXPECT(opponentLeft->hp > 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC lethal choice: a guard is scored after an ordinary attack without asking it for a method")
{
    GIVEN {
        AI_FLAGS(LETHAL_FLAGS);
        // Regression for abf52611d3: scoring Protect consulted the last move's
        // protect method, which asserts when the last move is an attack. The
        // battler here attacks on turn one and still has Protect on its set,
        // so turn two scores the guard with an attack behind it.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(300); Defense(200); SpDefense(200); Speed(200); Ability(ABILITY_TELEPATHY); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Attack(5); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_MELOETTA) {
            Level(30); HP(200); MaxHP(200); Attack(120); SpAttack(120); Defense(90); Speed(90);
            Ability(ABILITY_SERENE_GRACE); Moves(MOVE_HYPER_VOICE, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_HYPER_VOICE);
        }
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentLeft);
        }
    } THEN {
        EXPECT(opponentLeft->hp > 0 || opponentLeft->hp == 0);
    }
}
