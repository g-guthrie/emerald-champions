#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(GetMoveEffect(MOVE_HYPNOSIS) == EFFECT_NON_VOLATILE_STATUS);
    ASSUME(GetMoveNonVolatileStatus(MOVE_HYPNOSIS) == MOVE_EFFECT_SLEEP);
}


SINGLE_BATTLE_TEST("Hypnosis inflicts 1-2 turns of sleep (Champions)")
{
    u32 turns, count;
    ASSUME(B_SLEEP_TURNS >= GEN_5);
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    GIVEN {
        WITH_CONFIG(B_SLEEP_TURNS, GEN_CHAMPIONS);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_HYPNOSIS); MOVE(opponent, MOVE_CELEBRATE); }
        for (count = 0; count < turns; ++count)
            TURN {}
    } SCENE {
        if (turns == 1)
        {
            PASSES_RANDOMLY(1, 3, RNG_SLEEP_TURNS);
        } else {
            PASSES_RANDOMLY(2, 3, RNG_SLEEP_TURNS);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, player);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
        MESSAGE("The opposing Wobbuffet fell asleep!");
        STATUS_ICON(opponent, sleep: TRUE);
        for (count = 0; count < turns; ++count)
        {
            if (count < turns - 1)
                MESSAGE("The opposing Wobbuffet is fast asleep.");
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
        }
        MESSAGE("The opposing Wobbuffet woke up!");
        STATUS_ICON(opponent, none: TRUE);
    }
}

DOUBLE_BATTLE_TEST("Wake-Up Slap: curing sleep removes Nightmare before a same-turn new sleep")
{
    GIVEN {
        WITH_CONFIG(B_SLEEP_TURNS, GEN_9);
        PLAYER(SPECIES_WOBBUFFET) { Attack(1); Speed(200); Moves(MOVE_HYPNOSIS, MOVE_WAKE_UP_SLAP); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_NIGHTMARE, MOVE_HYPNOSIS); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(200); Speed(20); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_HYPNOSIS, target: opponentLeft, WITH_RNG(RNG_SLEEP_TURNS, 3)); MOVE(playerRight, MOVE_NIGHTMARE, target: opponentLeft); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_WAKE_UP_SLAP, target: opponentLeft); MOVE(playerRight, MOVE_HYPNOSIS, target: opponentLeft, WITH_RNG(RNG_SLEEP_TURNS, 3)); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponentLeft->status1 & STATUS1_SLEEP);
        EXPECT(!(bool32)opponentLeft->volatiles.nightmare);
    }
}

DOUBLE_BATTLE_TEST("Jungle Healing: curing an ally removes Nightmare before same-turn new sleep")
{
    GIVEN {
        WITH_CONFIG(B_SLEEP_TURNS, GEN_9);
        PLAYER(SPECIES_WOBBUFFET) { Speed(200); Moves(MOVE_HYPNOSIS, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_NIGHTMARE, MOVE_HYPNOSIS); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(20); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ZARUDE) { Speed(150); Moves(MOVE_CELEBRATE, MOVE_JUNGLE_HEALING); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_HYPNOSIS, target: opponentLeft, WITH_RNG(RNG_SLEEP_TURNS, 3)); MOVE(playerRight, MOVE_NIGHTMARE, target: opponentLeft); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_HYPNOSIS, target: opponentLeft, WITH_RNG(RNG_SLEEP_TURNS, 3)); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_JUNGLE_HEALING); }
    } THEN {
        EXPECT(opponentLeft->status1 & STATUS1_SLEEP);
        EXPECT(!(bool32)opponentLeft->volatiles.nightmare);
    }
}
