#include "global.h"
#include "test/battle.h"

// R(3)/L(3)/O(5)/Q(5): a side guard is worth what it saves on the whole side,
// not what it saves on its user. Four rooms reported the same shape - the
// spread attack was already on the record, the partner was the one about to
// die, and the guard was never chosen.
#define GUARD_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC side guard: Wide Guard is taken for the partner that dies without it")
{
    bool32 doomed = TRUE;
    GIVEN {
        AI_FLAGS(GUARD_FLAGS);
        // Toxapex's board. The spread attack has been used once already, so
        // it is public knowledge; the partner beside the guard user is the one
        // it kills. The guard user's own attack is single-target so it cannot
        // help that partner, and the partner cannot absorb it either. With a
        // healthy partner the AI guards as well, which is defensible against a
        // spread move it has already been shown, so there is no negative arm
        // here that means anything.
        PLAYER(SPECIES_GARDEVOIR) { Level(30); HP(300); MaxHP(300); SpAttack(140); Speed(150); Ability(ABILITY_PIXILATE); Moves(MOVE_HYPER_VOICE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_TOXAPEX) {
            Level(30); HP(300); MaxHP(300); SpAttack(80); Defense(180); SpDefense(180); Speed(30);
            Ability(ABILITY_REGENERATOR); Moves(MOVE_WIDE_GUARD, MOVE_SCALD);
        }
        OPPONENT(SPECIES_MANTINE) { Level(30); HP(doomed ? 200 : 400); MaxHP(400); Defense(120); SpDefense(120); Speed(20); Ability(ABILITY_SWIFT_SWIM); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_HYPER_VOICE);
            MOVE(playerRight, MOVE_SPLASH);
        }
        TURN {
            MOVE(playerLeft, MOVE_HYPER_VOICE);
            MOVE(playerRight, MOVE_SPLASH);
            EXPECT_MOVE(opponentLeft, MOVE_WIDE_GUARD);
        }
    } THEN {
        if (doomed)
            EXPECT(opponentRight->hp > 0);
    }
}
