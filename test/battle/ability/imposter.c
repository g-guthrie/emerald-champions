#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Imposter uses a copied move slot against its selected opponent, not itself")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SPORE) == EFFECT_NON_VOLATILE_STATUS);
        ASSUME(GetMoveNonVolatileStatus(MOVE_SPORE) == MOVE_EFFECT_SLEEP);
        PLAYER(SPECIES_ZIGZAGOON) { Moves(MOVE_CELEBRATE, MOVE_SPORE); }
        // Slot 1 only needs to exist in the recorded party. Imposter replaces
        // it with the target's slot 1 before the cartridge executes this turn.
        OPPONENT(SPECIES_DITTO) { Ability(ABILITY_IMPOSTER); Moves(MOVE_TRANSFORM, MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(player, MOVE_CELEBRATE);
            MOVE(opponent, moveSlot: 1, target: player);
        }
    } SCENE {
        MESSAGE("The opposing Ditto transformed into Zigzagoon!");
        MESSAGE("The opposing Ditto used Spore!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPORE, opponent);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, player);
        NOT MESSAGE("The opposing Ditto fell asleep!");
    } THEN {
        EXPECT_NE(player->status1, STATUS1_NONE);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

AI_SINGLE_BATTLE_TEST("Imposter AI targets a foe with copied Spore instead of itself")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SPORE) == EFFECT_NON_VOLATILE_STATUS);
        ASSUME(GetMoveNonVolatileStatus(MOVE_SPORE) == MOVE_EFFECT_SLEEP);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_ZIGZAGOON) { Moves(MOVE_CELEBRATE, MOVE_SPORE); }
        OPPONENT(SPECIES_DITTO) { Ability(ABILITY_IMPOSTER); Moves(MOVE_TRANSFORM, MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(player, MOVE_CELEBRATE);
            EXPECT_MOVE(opponent, moveSlot: 1, target: player);
        }
    } THEN {
        EXPECT_NE(player->status1, STATUS1_NONE);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

AI_DOUBLE_BATTLE_TEST("Imposter doubles AI targets a vulnerable foe after copying its moves")
{
    u64 aiFlags;
    bool32 transformed;
    // Retain the original reduced-information reproduction alongside the
    // current campaign profile. This isolates flags without changing teams.
    for (u32 i = 0; i < 2; i++)
    {
        PARAMETRIZE { transformed = i; aiFlags = AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT
            | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION
            | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS
            | AI_FLAG_KNOW_OPPONENT_PARTY; }
        PARAMETRIZE { transformed = i; aiFlags = AI_FLAG_BASIC_TRAINER | AI_FLAG_HP_AWARE
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_ASSUME_STAB | AI_FLAG_ASSUME_STATUS_MOVES; }
    }
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SPORE) == EFFECT_NON_VOLATILE_STATUS);
        ASSUME(GetMoveNonVolatileStatus(MOVE_SPORE) == MOVE_EFFECT_SLEEP);
        AI_FLAGS(aiFlags);
        PLAYER(SPECIES_AMOONGUSS) { Moves(MOVE_CELEBRATE); }
        // Opponent-left Imposter copies the diagonal player-right battler.
        PLAYER(SPECIES_SMEARGLE) { Moves(MOVE_CELEBRATE, MOVE_SPORE); }
        OPPONENT(transformed ? SPECIES_DITTO : SPECIES_SMEARGLE) {
            Ability(transformed ? ABILITY_IMPOSTER : ABILITY_OWN_TEMPO);
            Item(ITEM_CHOICE_SCARF);
            Moves(transformed ? MOVE_TRANSFORM : MOVE_CELEBRATE, transformed ? MOVE_CELEBRATE : MOVE_SPORE);
        }
        OPPONENT(SPECIES_WIMPOD) {
            Ability(ABILITY_WIMP_OUT);
            Item(ITEM_FOCUS_SASH);
            Moves(MOVE_TAUNT, MOVE_SPIKES, MOVE_AQUA_JET, MOVE_LEECH_LIFE);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, moveSlot: 1, target: playerRight);
        }
    } THEN {
        EXPECT_EQ(playerLeft->status1, STATUS1_NONE);
        EXPECT_NE(playerRight->status1, STATUS1_NONE);
        EXPECT_EQ(opponentLeft->status1, STATUS1_NONE);
        EXPECT_EQ(opponentRight->status1, STATUS1_NONE);
    }
}
