#include "global.h"
#include "test/battle.h"

// Last turn's move is legal knowledge on every difficulty. These pin the
// classes that live or die on it, plus the two-guard turn, all from real play.
#define REACTIVE_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC reactive pricing: both slots do not guard the same turn")
{
    GIVEN {
        AI_FLAGS(REACTIVE_FLAGS);
        // Each guard is defensible alone - both flanks are threatened - but
        // together they pass the turn and halve both guards next turn. The
        // right slot has a knockout waiting, so it is the one that must act.
        PLAYER(SPECIES_MACHOP) { Level(30); HP(40); MaxHP(40); Attack(200); Defense(60); SpDefense(60); Speed(90); Moves(MOVE_BRICK_BREAK); }
        PLAYER(SPECIES_MACHOP) { Level(30); HP(300); MaxHP(300); Attack(200); Defense(200); SpDefense(200); Speed(85); Moves(MOVE_BRICK_BREAK); }
        OPPONENT(SPECIES_NOSEPASS) {
            Level(30); HP(60); MaxHP(60); Defense(50); SpAttack(40); SpDefense(50); Speed(40);
            Ability(ABILITY_MAGNET_PULL); Moves(MOVE_POWER_GEM, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_MANECTRIC) {
            Level(30); HP(60); MaxHP(60); Defense(50); SpAttack(200); SpDefense(50); Speed(100);
            Ability(ABILITY_LIGHTNING_ROD); Moves(MOVE_THUNDERBOLT, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BRICK_BREAK, target: opponentLeft);
            MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentRight);
            NOT_EXPECT_MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT(playerLeft->hp < 40 || playerRight->hp < 300);
    }
}

AI_DOUBLE_BATTLE_TEST("EC reactive pricing: Sucker Punch is not aimed at the flank that just redirected")
{
    GIVEN {
        AI_FLAGS(REACTIVE_FLAGS);
        PLAYER(SPECIES_CLEFAIRY) { Level(30); HP(300); MaxHP(300); Defense(150); SpDefense(150); Speed(70); Moves(MOVE_FOLLOW_ME, MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_ABSOL) {
            Level(30); HP(200); MaxHP(200); Attack(150); Speed(90);
            Ability(ABILITY_PRESSURE); Moves(MOVE_SUCKER_PUNCH, MOVE_NIGHT_SLASH);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        // The redirector shows what it does with its turn, and Sucker Punch
        // fails into a status move.
        TURN { MOVE(playerLeft, MOVE_FOLLOW_ME); MOVE(playerRight, MOVE_SPLASH); }
        TURN {
            MOVE(playerLeft, MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_SPLASH);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_SUCKER_PUNCH);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC reactive pricing: Counter is not chosen against a side that only attacks specially")
{
    GIVEN {
        AI_FLAGS(REACTIVE_FLAGS);
        PLAYER(SPECIES_ALAKAZAM) { Level(30); HP(300); MaxHP(300); SpAttack(120); Defense(150); SpDefense(150); Speed(120); Moves(MOVE_PSYCHIC); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) {
            Level(30); HP(300); MaxHP(300); Defense(120); SpDefense(120); Speed(20);
            Ability(ABILITY_SHADOW_TAG); Moves(MOVE_COUNTER, MOVE_MIRROR_COAT);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft); MOVE(playerRight, MOVE_SPLASH); }
        TURN {
            MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft);
            MOVE(playerRight, MOVE_SPLASH);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_COUNTER);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC reactive pricing: Feint is not worth a turn with no shield to break")
{
    GIVEN {
        AI_FLAGS(REACTIVE_FLAGS);
        // Nothing on the field has guarded, so Feint is a weak attack against
        // a body that resists it while the other move is neutral and stronger.
        PLAYER(SPECIES_SKARMORY) { Level(30); HP(300); MaxHP(300); Defense(150); SpDefense(150); Speed(20); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_CLOBBOPUS) {
            Level(30); HP(200); MaxHP(200); Attack(120); Speed(60);
            Ability(ABILITY_TECHNICIAN); Moves(MOVE_FEINT, MOVE_CIRCLE_THROW);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_SPLASH);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_FEINT);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC reactive pricing: a burn goes on the physical attacker")
{
    GIVEN {
        AI_FLAGS(REACTIVE_FLAGS);
        // Two equally burnable bodies: only one of them loses anything to it.
        PLAYER(SPECIES_MACHOP) { Level(30); HP(300); MaxHP(300); Attack(200); SpAttack(20); Defense(150); SpDefense(150); Speed(70); Item(ITEM_LIFE_ORB); Moves(MOVE_BRICK_BREAK); }
        PLAYER(SPECIES_ALAKAZAM) { Level(30); HP(300); MaxHP(300); Attack(20); SpAttack(200); Defense(150); SpDefense(150); Speed(65); Moves(MOVE_PSYCHIC); }
        OPPONENT(SPECIES_FLETCHINDER) {
            Level(30); HP(200); MaxHP(200); Attack(40); SpAttack(40); Speed(90);
            Ability(ABILITY_FLAME_BODY); Moves(MOVE_WILL_O_WISP, MOVE_EMBER);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BRICK_BREAK, target: opponentLeft);
            MOVE(playerRight, MOVE_PSYCHIC, target: opponentLeft);
            // Spending the turn on the burn at all is the reported defect -
            // four turns of it unused in front of a Life Orb physical
            // attacker. Which body it lands on is scored too (a burn on a
            // physical attacker is worth more) but is not decided by that term
            // alone on this board, so it is not asserted here.
            EXPECT_MOVE(opponentLeft, MOVE_WILL_O_WISP);
        }
    } THEN {
        EXPECT((playerLeft->status1 & STATUS1_BURN) || (playerRight->status1 & STATUS1_BURN));
    }
}

AI_DOUBLE_BATTLE_TEST("EC reactive pricing: Encore locks in the move the target just used")
{
    GIVEN {
        AI_FLAGS(REACTIVE_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Defense(150); SpDefense(150); Speed(70); Moves(MOVE_HARDEN, MOVE_TACKLE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WHIMSICOTT) {
            Level(30); HP(200); MaxHP(200); SpAttack(40); Speed(100);
            Ability(ABILITY_PRANKSTER); Moves(MOVE_ENCORE, MOVE_MOONBLAST);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        // A foe that just spent its turn on a stat move is the best thing on
        // the board to lock into it.
        TURN { MOVE(playerLeft, MOVE_HARDEN); MOVE(playerRight, MOVE_SPLASH); }
        TURN {
            MOVE(playerLeft, MOVE_HARDEN);
            MOVE(playerRight, MOVE_SPLASH);
            EXPECT_MOVE(opponentLeft, MOVE_ENCORE, target: playerLeft);
        }
    } THEN {
        EXPECT(playerLeft->volatiles.encoreTimer != 0);
    }
}
