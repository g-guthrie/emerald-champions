#include "global.h"
#include "test/battle.h"

// Group F, from the Electric Gym block: an attack the other side's ability
// takes for free, a disruption move aimed at a body with nothing to disrupt,
// and a pivot with nowhere to pivot to.
#define ELECTRIC_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC electric: an Electric attack is not fed to a Lightning Rod")
{
    bool32 rod;
    PARAMETRIZE { rod = TRUE; }
    PARAMETRIZE { rod = FALSE; }
    GIVEN {
        AI_FLAGS(ELECTRIC_FLAGS);
        // With the Rod up, the Electric attack is intercepted, absorbed and
        // pays the holder a Special Attack stage. The neutral attack is worth
        // less on paper and more in fact.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(200); MaxHP(200); Defense(60); SpDefense(60); Speed(20); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MANECTRIC) { Level(30); HP(200); MaxHP(200); SpDefense(60); Speed(30); Ability(rod ? ABILITY_LIGHTNING_ROD : ABILITY_STATIC); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_THUNDURUS) {
            Level(30); HP(200); MaxHP(200); SpAttack(140); Speed(120);
            Ability(ABILITY_PRANKSTER); Moves(MOVE_THUNDERBOLT, MOVE_SWIFT);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(5); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            if (rod)
                EXPECT_MOVE(opponentLeft, MOVE_SWIFT);
            else
                EXPECT_MOVE(opponentLeft, MOVE_THUNDERBOLT, target: playerLeft);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC electric: Taunt needs a status move to take away")
{
    bool32 status;
    PARAMETRIZE { status = TRUE; }
    PARAMETRIZE { status = FALSE; }
    GIVEN {
        AI_FLAGS(ELECTRIC_FLAGS);
        // Taunt costs the whole turn. Against a body holding a real status
        // move it takes that move away; against one that only attacks it takes
        // nothing and the turn is simply gone.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(200); MaxHP(200); Defense(60); SpDefense(60); Speed(20); Moves(status ? MOVE_TOXIC : MOVE_TACKLE, MOVE_TACKLE); }
        // The partner holds no status move either: Taunt must have nothing on
        // the board to take, not merely nothing on the nearer body.
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Attack(5); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_ELECTRODE) {
            Level(30); HP(200); MaxHP(200); SpAttack(90); Speed(140);
            Ability(ABILITY_STATIC); Moves(MOVE_TAUNT, MOVE_THUNDERBOLT);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(5); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            if (status)
                EXPECT_MOVE(opponentLeft, MOVE_TAUNT, target: playerLeft);
            else
                NOT_EXPECT_MOVE(opponentLeft, MOVE_TAUNT);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC electric: a pivot with an empty bench is only an attack")
{
    bool32 bench;
    PARAMETRIZE { bench = TRUE; }
    PARAMETRIZE { bench = FALSE; }
    GIVEN {
        AI_FLAGS(ELECTRIC_FLAGS);
        // Volt Switch is chosen for the exit it buys. With nobody to bring in
        // there is no exit, and the stronger attack has to win the comparison.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(20); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_VIKAVOLT) {
            Level(30); HP(200); MaxHP(200); SpAttack(120); Speed(40);
            Ability(ABILITY_LEVITATE); Moves(MOVE_VOLT_SWITCH, MOVE_BUG_BUZZ);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(200); MaxHP(200); Speed(5); Moves(MOVE_SPLASH); }
        if (bench)
            OPPONENT(SPECIES_ELECTIVIRE) { Level(30); HP(200); MaxHP(200); Speed(60); Moves(MOVE_THUNDER_PUNCH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_SPLASH);
            if (!bench)
                NOT_EXPECT_MOVE(opponentLeft, MOVE_VOLT_SWITCH);
        }
    }
}
