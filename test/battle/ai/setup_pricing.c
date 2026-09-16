#include "global.h"
#include "test/battle.h"

// A single turn cannot see what speed control, a boost, a sleep or a low-HP
// levelling move is worth, because their value arrives afterwards. These pin
// the bounded next-turn values that keep those classes competitive with a
// direct attack without ever making them automatic.
#define SETUP_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC setup pricing: Tailwind is worth the turn when the side is outsped")
{
    bool32 outsped;
    PARAMETRIZE { outsped = TRUE; }
    PARAMETRIZE { outsped = FALSE; }
    GIVEN {
        AI_FLAGS(SETUP_FLAGS);
        // The crossings the wind actually buys are the value: with the player
        // already slower there are none, and the attack keeps the turn.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(outsped ? 120 : 10); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(outsped ? 110 : 8); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WHIMSICOTT) {
            Level(30); HP(200); MaxHP(200); SpAttack(80); Speed(70);
            Ability(ABILITY_INFILTRATOR); Moves(MOVE_TAILWIND, MOVE_MOONBLAST);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(60); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            if (outsped)
                EXPECT_MOVE(opponentLeft, MOVE_TAILWIND);
            else
                EXPECT_MOVE(opponentLeft, MOVE_MOONBLAST);
        }
    } THEN {
        if (outsped)
            EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TAILWIND);
    }
}

AI_DOUBLE_BATTLE_TEST("EC setup pricing: a boost is worth a safe turn and not a dangerous one")
{
    bool32 safe;
    PARAMETRIZE { safe = TRUE; }
    PARAMETRIZE { safe = FALSE; }
    GIVEN {
        AI_FLAGS(SETUP_FLAGS);
        // Safe: neither flank can meaningfully hurt the setter, so the turn is
        // free. Dangerous: the same board with attackers that end it.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(safe ? 5 : 500); Defense(200); SpDefense(200); Speed(20); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(safe ? 5 : 500); Defense(200); SpDefense(200); Speed(15); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_BEAUTIFLY) {
            Level(30); HP(120); MaxHP(120); Defense(40); SpAttack(90); SpDefense(40); Speed(80);
            Ability(ABILITY_SWARM); Moves(MOVE_QUIVER_DANCE, MOVE_BUG_BUZZ);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentLeft);
            if (safe)
                EXPECT_MOVE(opponentLeft, MOVE_QUIVER_DANCE);
            else
                EXPECT_MOVE(opponentLeft, MOVE_BUG_BUZZ);
        }
    } THEN {
        if (safe)
            EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

AI_DOUBLE_BATTLE_TEST("EC setup pricing: sleep on a healthy threat is worth the turn")
{
    GIVEN {
        AI_FLAGS(SETUP_FLAGS);
        PLAYER(SPECIES_MACHOP) { Level(30); HP(300); MaxHP(300); Attack(200); Defense(120); SpDefense(120); Speed(60); Moves(MOVE_BRICK_BREAK); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_VENONAT) {
            Level(30); HP(200); MaxHP(200); SpAttack(60); Speed(90);
            Ability(ABILITY_COMPOUND_EYES); Moves(MOVE_SLEEP_POWDER, MOVE_CONFUSION);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BRICK_BREAK, target: opponentLeft);
            MOVE(playerRight, MOVE_SPLASH);
            EXPECT_MOVE(opponentLeft, MOVE_SLEEP_POWDER, target: playerLeft);
        }
    } THEN {
        EXPECT(playerLeft->status1 & STATUS1_SLEEP);
    }
}

AI_DOUBLE_BATTLE_TEST("EC setup pricing: Endeavor is the low-HP attack when it can land first")
{
    GIVEN {
        AI_FLAGS(SETUP_FLAGS);
        // Two HP left and faster than everything on the board: levelling the
        // healthy target is worth far more than the priority chip, and the one
        // turn it has is enough to do it.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Attack(150); Defense(150); SpDefense(150); Speed(10); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_DODUO) {
            Level(30); HP(2); MaxHP(120); Attack(70); Speed(100);
            Ability(ABILITY_RUN_AWAY); Moves(MOVE_ENDEAVOR, MOVE_QUICK_ATTACK);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_SPLASH);
            EXPECT_MOVE(opponentLeft, MOVE_ENDEAVOR, target: playerLeft);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 2);
    }
}

AI_DOUBLE_BATTLE_TEST("EC setup pricing: a healthy lead does not reset the board on turn one")
{
    GIVEN {
        AI_FLAGS(SETUP_FLAGS);
        // The reserve is a cleaner matchup on paper, but the lead is at full
        // health with a real attack and nothing has been revealed: spending
        // the turn to swap gives the free hit away for a guess.
        // Neither flank threatens the lead: a healthy, unpressured board is
        // exactly where a turn-one withdrawal costs a free hit for nothing.
        PLAYER(SPECIES_MACHOP) { Level(30); HP(300); MaxHP(300); Attack(12); Defense(120); SpDefense(120); Speed(60); Moves(MOVE_BRICK_BREAK); }
        PLAYER(SPECIES_MACHOP) { Level(30); HP(300); MaxHP(300); Attack(12); Defense(120); SpDefense(120); Speed(55); Moves(MOVE_BRICK_BREAK); }
        OPPONENT(SPECIES_SCRAGGY) {
            Level(30); HP(150); MaxHP(150); Attack(110); Defense(70); SpDefense(70); Speed(50);
            Ability(ABILITY_MOXIE); Moves(MOVE_BRICK_BREAK, MOVE_CRUNCH);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(10); Moves(MOVE_SPLASH); }
        // A mildly better matchup on paper, not a hard counter: not worth the
        // free turn a withdrawal hands over with nothing revealed yet.
        OPPONENT(SPECIES_XATU) { Level(30); HP(150); MaxHP(150); SpAttack(60); SpDefense(60); Speed(70); Ability(ABILITY_SYNCHRONIZE); Moves(MOVE_PSYCHIC, MOVE_AIR_SLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BRICK_BREAK, target: opponentLeft);
            MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentLeft);
            // Staying in and attacking is the assertion; the species check
            // below is what a withdrawal would break.
            EXPECT_MOVE(opponentLeft, MOVE_BRICK_BREAK);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_SCRAGGY);
    }
}

AI_DOUBLE_BATTLE_TEST("EC setup pricing: recovery beats a shield that banks less than it heals")
{
    GIVEN {
        AI_FLAGS(SETUP_FLAGS);
        // Half the maximum back is permanent; what a shield saves is only the
        // part of one turn's damage a payoff makes permanent.
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(60); Moves(MOVE_SPLASH); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_NACLSTACK) {
            Level(30); HP(10); MaxHP(50); Defense(150); SpDefense(150); Speed(30);
            Ability(ABILITY_STURDY); Moves(MOVE_RECOVER, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SPLASH);
            MOVE(playerRight, MOVE_SPLASH);
            EXPECT_MOVE(opponentLeft, MOVE_RECOVER);
        }
    } THEN {
        EXPECT(opponentLeft->hp > 10);
    }
}
