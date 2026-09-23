#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Freeze infliction: a new freeze starts with zero elapsed frozen actions")
{
    u32 generation;
    PARAMETRIZE { generation = GEN_9; }
    PARAMETRIZE { generation = GEN_CHAMPIONS; }
    GIVEN {
        WITH_CONFIG(B_FREEZE_TURNS, generation);
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); SpAttack(1); Moves(MOVE_ICE_BEAM, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ICE_BEAM, WITH_RNG(RNG_SECONDARY_EFFECT, TRUE)); MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_FROZEN, FALSE)); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_FROZEN, FALSE)); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_FROZEN, FALSE)); }
    } SCENE {
        MESSAGE("Wobbuffet was frozen solid!");
        MESSAGE("Wobbuffet is frozen solid!");
        MESSAGE("Wobbuffet is frozen solid!");
        if (generation == GEN_CHAMPIONS)
        {
            STATUS_ICON(player, none: TRUE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        }
        else
            MESSAGE("Wobbuffet is frozen solid!");
    } THEN {
        EXPECT_EQ(player->status1, generation == GEN_CHAMPIONS ? STATUS1_NONE : STATUS1_FREEZE);
    }
}

SINGLE_BATTLE_TEST("Freeze cure: Lum and Aspear clear elapsed frozen actions")
{
    enum Item berry;
    PARAMETRIZE { berry = ITEM_LUM_BERRY; }
    PARAMETRIZE { berry = ITEM_ASPEAR_BERRY; }
    GIVEN {
        WITH_CONFIG(B_FREEZE_TURNS, GEN_CHAMPIONS);
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Status1(STATUS1_FREEZE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); Item(berry); Moves(MOVE_CELEBRATE, MOVE_TRICK); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_FROZEN, FALSE)); }
        TURN { MOVE(opponent, MOVE_TRICK); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ((u32)GetBattlerPartyState(B_BATTLER_0)->freezeTurns, 0);
    }
}
