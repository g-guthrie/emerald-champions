#include "global.h"
#include "test/battle.h"

// Additional timing coverage lives in restore_hp.c.
SINGLE_BATTLE_TEST("Sitrus Berry respects Belly Drum HP parity and heals one quarter")
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
        if (maxHP == 96) {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player);
            HP_BAR(player, hp: 72);
        } else {
            NONE_OF { ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player); }
        }
    } THEN {
        enum BattlerId battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(gBattleMons[battler].hp, maxHP == 96 ? 72 : 49);
        EXPECT_EQ(gBattleMons[battler].item, maxHP == 96 ? ITEM_NONE : ITEM_SITRUS_BERRY);
    }
}
