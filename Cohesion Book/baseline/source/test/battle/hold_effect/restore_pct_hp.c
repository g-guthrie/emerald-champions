#include "global.h"
#include "test/battle.h"

// Additional timing coverage lives in restore_hp.c.
SINGLE_BATTLE_TEST("Sitrus Berry heals one quarter after Belly Drum at either HP parity")
{
    u32 maxHP;
    PARAMETRIZE { maxHP = 96; }
    PARAMETRIZE { maxHP = 97; }
    GIVEN {
        ASSUME(gItemsInfo[ITEM_SITRUS_BERRY].holdEffect == HOLD_EFFECT_RESTORE_PCT_HP);
        PLAYER(SPECIES_ZIGZAGOON) { Ability(ABILITY_PICKUP); MaxHP(maxHP); HP(maxHP); Item(ITEM_SITRUS_BERRY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BELLY_DRUM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BELLY_DRUM, player);
        HP_BAR(player, hp: maxHP == 96 ? 48 : 49);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player);
        HP_BAR(player, hp: maxHP == 96 ? 72 : 73);
    } THEN {
        enum BattlerId battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(gBattleMons[battler].hp, maxHP == 96 ? 72 : 73);
        EXPECT_EQ(gBattleMons[battler].item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Sitrus Berry activates only at or below half HP rounded up")
{
    u32 maxHP, hp;
    PARAMETRIZE { maxHP = 96; hp = 48; }
    PARAMETRIZE { maxHP = 96; hp = 49; }
    PARAMETRIZE { maxHP = 97; hp = 49; }
    PARAMETRIZE { maxHP = 97; hp = 50; }
    GIVEN {
        PLAYER(SPECIES_ZIGZAGOON) { Ability(ABILITY_PICKUP); MaxHP(maxHP); HP(hp); Item(ITEM_SITRUS_BERRY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        enum BattlerId battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        bool32 activates = hp == 48 || (maxHP == 97 && hp == 49);
        EXPECT_EQ(gBattleMons[battler].hp, activates ? hp + 24 : hp);
        EXPECT_EQ(gBattleMons[battler].item, activates ? ITEM_NONE : ITEM_SITRUS_BERRY);
        EXPECT_EQ(gBattleMons[battler].maxHP, maxHP);
    }
}

SINGLE_BATTLE_TEST("Sitrus Berry is not consumed at full HP on Shedinja")
{
    GIVEN {
        PLAYER(SPECIES_SHEDINJA) { Item(ITEM_SITRUS_BERRY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        enum BattlerId battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(gBattleMons[battler].hp, 1);
        EXPECT_EQ(gBattleMons[battler].maxHP, 1);
        EXPECT_EQ(gBattleMons[battler].item, ITEM_SITRUS_BERRY);
    }
}
