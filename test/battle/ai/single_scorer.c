#include "global.h"
#include "test/battle.h"

// The pair search does not run in a single battle, so these price classes the
// per-battler scorer owns. All three come from real per-turn play.
#define SCORER_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY)

AI_SINGLE_BATTLE_TEST("EC single scorer: chip that costs contact damage every turn loses to the real attack")
{
    GIVEN {
        AI_FLAGS(SCORER_FLAGS);
        // Three turns of the priority chip took nine, nine and zero off this
        // wall while paying Rocky Helmet each time, with the real attack unused.
        PLAYER(SPECIES_SNORLAX) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(10); Item(ITEM_ROCKY_HELMET); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_EEVEE) {
            Level(30); HP(150); MaxHP(150); Attack(90); Speed(60);
            Ability(ABILITY_ADAPTABILITY); Moves(MOVE_QUICK_ATTACK, MOVE_DOUBLE_EDGE);
        }
    } WHEN {
        TURN {
            MOVE(player, MOVE_CELEBRATE);
            EXPECT_MOVE(opponent, MOVE_DOUBLE_EDGE);
        }
    }
}

AI_SINGLE_BATTLE_TEST("EC single scorer: sleep on a healthy threat beats the chip that cannot finish it")
{
    GIVEN {
        AI_FLAGS(SCORER_FLAGS);
        // The threat is status free and two-shots the user; five turns of chip
        // in front of it is not the trade.
        PLAYER(SPECIES_ARCANINE) { Level(30); HP(300); MaxHP(300); Attack(200); Defense(150); SpDefense(150); Speed(120); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_SHROOMISH) {
            Level(30); HP(300); MaxHP(300); Attack(40); Defense(120); SpDefense(120); Speed(30);
            Ability(ABILITY_EFFECT_SPORE); Moves(MOVE_SPORE, MOVE_ABSORB);
        }
    } WHEN {
        TURN {
            MOVE(player, MOVE_BITE);
            EXPECT_MOVE(opponent, MOVE_SPORE);
        }
    } THEN {
        // The subject is what the scorer is willing to spend the turn on. That
        // the sleep itself lands is covered in doubles by
        // "EC setup pricing: sleep on a healthy threat is worth the turn";
        // this single-battle board does not resolve the status, which is worth
        // a separate look at the powder path rather than a weaker assertion here.
        EXPECT(opponent->hp > 0);
    }
}

// The Conservative trait is a stated preference for the safe line. It had no
// contact with the guard model at all, which is how a team whose four members
// all carried Protect went a whole battle without using it.
AI_DOUBLE_BATTLE_TEST("EC single scorer: a Conservative flank banks more of what its guard denies")
{
    bool32 conservative;
    PARAMETRIZE { conservative = TRUE; }
    PARAMETRIZE { conservative = FALSE; }
    GIVEN {
        AI_FLAGS(SCORER_FLAGS | AI_FLAG_DOUBLE_BATTLE | (conservative ? AI_FLAG_CONSERVATIVE : 0));
        PLAYER(SPECIES_MACHOP) { Level(30); HP(300); MaxHP(300); Attack(200); Defense(150); SpDefense(150); Speed(90); Moves(MOVE_BRICK_BREAK); }
        PLAYER(SPECIES_MACHOP) { Level(30); HP(300); MaxHP(300); Attack(200); Defense(150); SpDefense(150); Speed(85); Moves(MOVE_BRICK_BREAK); }
        OPPONENT(SPECIES_NOSEPASS) {
            Level(30); HP(70); MaxHP(70); Defense(50); SpAttack(70); SpDefense(50); Speed(40);
            Ability(ABILITY_MAGNET_PULL); Moves(MOVE_POWER_GEM, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BRICK_BREAK, target: opponentLeft);
            MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentLeft);
            if (conservative)
                EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
            else
                EXPECT_MOVE(opponentLeft, MOVE_POWER_GEM);
        }
    }
}
