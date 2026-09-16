#include "global.h"
#include "test/battle.h"

// The L(1) family re-run on the depth fix, plus two vetoes from group P that
// belong to the same question: what a turn spent on something other than
// damage is worth, and when it is worth nothing at all.
#define STARVE_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC starvation: Tailwind is taken on the board that was built for it")
{
    GIVEN {
        AI_FLAGS(STARVE_FLAGS);
        // Wendy's board: both of hers are outsped, both live through the turn,
        // and the wind flips the order for the rest of the battle.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(20); Defense(200); SpDefense(200); Speed(150); Ability(ABILITY_TELEPATHY); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(20); Defense(200); SpDefense(200); Speed(140); Ability(ABILITY_TELEPATHY); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_SWANNA) {
            Level(30); HP(300); MaxHP(300); SpAttack(90); Defense(90); Speed(100);
            Ability(ABILITY_KEEN_EYE); Moves(MOVE_TAILWIND, MOVE_AIR_SLASH);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Defense(120); SpDefense(120); Speed(90); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_TAILWIND);
        }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TAILWIND);
    }
}

AI_DOUBLE_BATTLE_TEST("EC starvation: Dragon Dance is taken on a turn nothing can punish")
{
    GIVEN {
        AI_FLAGS(STARVE_FLAGS);
        // Flint's board: the setter is faster than both, takes almost nothing
        // from either, and the dance pays on every turn after this one.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(10); SpAttack(10); Defense(200); SpDefense(200); Speed(20); Ability(ABILITY_TELEPATHY); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(10); SpAttack(10); Defense(200); SpDefense(200); Speed(15); Ability(ABILITY_TELEPATHY); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_DRAGONITE) {
            Level(30); HP(300); MaxHP(300); Attack(110); Defense(110); SpDefense(110); Speed(90);
            Ability(ABILITY_INNER_FOCUS); Moves(MOVE_DRAGON_DANCE, MOVE_DRAGON_CLAW);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentLeft);
            EXPECT_MOVE(opponentLeft, MOVE_DRAGON_DANCE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC starvation: Spore goes in when nothing opposite is immune to it")
{
    GIVEN {
        AI_FLAGS(STARVE_FLAGS);
        // Breloom's board: six turns went by without this in the receipt.
        // Neither body is Grass, holds Goggles or has Overcoat.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Attack(150); Defense(120); SpDefense(120); Speed(150); Ability(ABILITY_TELEPATHY); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Attack(5); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_BRELOOM) {
            Level(30); HP(200); MaxHP(200); Attack(120); Defense(60); Speed(70);
            Ability(ABILITY_EFFECT_SPORE); Moves(MOVE_SPORE, MOVE_MACH_PUNCH, MOVE_SEED_BOMB);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            // Either body is a fair target; the receipt's complaint was that
            // the sleep never happened at all.
            EXPECT_MOVE(opponentLeft, MOVE_SPORE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC starvation: a single-target Water move is not fired into the partner's Storm Drain")
{
    bool32 drain;
    PARAMETRIZE { drain = TRUE; }
    PARAMETRIZE { drain = FALSE; }
    GIVEN {
        AI_FLAGS(STARVE_FLAGS);
        // Huntail's board: the Water move is the knockout, and the partner
        // beside it takes every Water move on the field. With an ordinary
        // partner the same attack is simply correct.
        // A body the Water move is doubled against and the Ice move halved,
        // so only the absorbing partner can change the answer.
        PLAYER(SPECIES_TORKOAL) { Level(30); HP(200); MaxHP(300); Defense(60); SpDefense(60); Speed(10); Ability(ABILITY_WHITE_SMOKE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_HUNTAIL) {
            Level(30); HP(300); MaxHP(300); SpAttack(120); Speed(60);
            Ability(ABILITY_SWIFT_SWIM); Moves(MOVE_HYDRO_PUMP, MOVE_ICE_BEAM);
        }
        OPPONENT(SPECIES_GASTRODON) { Level(30); HP(300); MaxHP(300); Speed(20); Ability(drain ? ABILITY_STORM_DRAIN : ABILITY_SAND_FORCE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_SPLASH);
            if (drain)
                NOT_EXPECT_MOVE(opponentLeft, MOVE_HYDRO_PUMP);
            else
                EXPECT_MOVE(opponentLeft, MOVE_HYDRO_PUMP, target: playerLeft);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC starvation: Prankster status is not aimed at a Dark body")
{
    GIVEN {
        AI_FLAGS(STARVE_FLAGS);
        // Liepard's board: Prankster gives the status move priority and a Dark
        // target is immune to it because of that priority. The attack beside
        // it is doubled into the same body, so nothing else is close.
        PLAYER(SPECIES_SABLEYE) { Level(30); HP(200); MaxHP(200); Defense(90); SpDefense(90); Speed(20); Ability(ABILITY_KEEN_EYE); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Defense(120); SpDefense(120); Speed(5); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_LIEPARD) {
            Level(30); HP(200); MaxHP(200); Attack(90); Speed(110);
            Ability(ABILITY_PRANKSTER); Moves(MOVE_ENCORE, MOVE_PLAY_ROUGH);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentLeft);
        }
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentLeft);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_ENCORE);
        }
    }
}
