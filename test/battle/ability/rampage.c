#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Rampage skips the recharge turn when a recharge move knocks out its target")
{
    GIVEN {
        ASSUME(MoveHasAdditionalEffectSelf(MOVE_HYPER_BEAM, MOVE_EFFECT_RECHARGE));
        PLAYER(SPECIES_GYARADOS) { Ability(ABILITY_RAMPAGE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_BEAM); SEND_OUT(opponent, 1); }
        TURN { MOVE(player, MOVE_WATERFALL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_BEAM, player);
        MESSAGE("The opposing Wobbuffet fainted!");
        NOT MESSAGE("Gyarados must recharge!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATERFALL, player);
    }
}

SINGLE_BATTLE_TEST("Rampage still recharges when the target survives, and without the ability")
{
    enum Ability ability;
    u16 hp;

    PARAMETRIZE { ability = ABILITY_RAMPAGE; hp = 999; }
    PARAMETRIZE { ability = ABILITY_INTIMIDATE; hp = 1; }

    GIVEN {
        PLAYER(SPECIES_GYARADOS) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(hp); MaxHP(999); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (hp == 1)
            TURN { MOVE(player, MOVE_HYPER_BEAM); SEND_OUT(opponent, 1); }
        else
            TURN { MOVE(player, MOVE_HYPER_BEAM); }
        TURN { SKIP_TURN(player); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_BEAM, player);
        MESSAGE("Gyarados must recharge!");
    }
}

// Both moves knock out at equal accuracy, so the added-effect tie-break decides.
AI_SINGLE_BATTLE_TEST("AI only discounts a knockout recharge move with Rampage")
{
    enum Ability ability;
    enum Move other, expected;

    PARAMETRIZE { ability = ABILITY_INTIMIDATE; other = MOVE_AQUA_TAIL;  expected = MOVE_AQUA_TAIL; }
    PARAMETRIZE { ability = ABILITY_RAMPAGE;    other = MOVE_HAMMER_ARM; expected = MOVE_GIGA_IMPACT; }

    GIVEN {
        ASSUME(MoveHasAdditionalEffectSelf(MOVE_GIGA_IMPACT, MOVE_EFFECT_RECHARGE));
        ASSUME(GetMoveAccuracy(MOVE_GIGA_IMPACT) == GetMoveAccuracy(MOVE_AQUA_TAIL));
        ASSUME(GetMoveAccuracy(MOVE_GIGA_IMPACT) == GetMoveAccuracy(MOVE_HAMMER_ARM));
        ASSUME(GetMoveAdditionalEffectCount(MOVE_AQUA_TAIL) == 0);
        ASSUME(MoveHasAdditionalEffectSelf(MOVE_HAMMER_ARM, MOVE_EFFECT_STAT_MINUS));
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GYARADOS) { Ability(ability); Moves(MOVE_GIGA_IMPACT, other); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); EXPECT_MOVE(opponent, expected); SEND_OUT(player, 1); }
    }
}
