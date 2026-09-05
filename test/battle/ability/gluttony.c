#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Gluttony consumes confusion berries after Belly Drum only at half HP or below")
{
    enum Item item;
    u32 maxHP;
    PARAMETRIZE { item = ITEM_FIGY_BERRY; maxHP = 96; }
    PARAMETRIZE { item = ITEM_FIGY_BERRY; maxHP = 97; }
    PARAMETRIZE { item = ITEM_WIKI_BERRY; maxHP = 96; }
    PARAMETRIZE { item = ITEM_WIKI_BERRY; maxHP = 97; }
    PARAMETRIZE { item = ITEM_MAGO_BERRY; maxHP = 96; }
    PARAMETRIZE { item = ITEM_MAGO_BERRY; maxHP = 97; }
    PARAMETRIZE { item = ITEM_AGUAV_BERRY; maxHP = 96; }
    PARAMETRIZE { item = ITEM_AGUAV_BERRY; maxHP = 97; }
    PARAMETRIZE { item = ITEM_IAPAPA_BERRY; maxHP = 96; }
    PARAMETRIZE { item = ITEM_IAPAPA_BERRY; maxHP = 97; }
    GIVEN {
        ASSUME(B_CONFUSE_BERRIES_HEAL >= GEN_8);
        PLAYER(SPECIES_ZIGZAGOON) { Ability(ABILITY_GLUTTONY); Nature(NATURE_HARDY); MaxHP(maxHP); HP(maxHP); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BELLY_DRUM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BELLY_DRUM, player);
        HP_BAR(player, hp: maxHP == 96 ? 48 : 49);
        if (maxHP == 96) {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player);
            HP_BAR(player, hp: 80);
        } else {
            NONE_OF { ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player); }
        }
    } THEN {
        enum BattlerId battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(gBattleMons[battler].hp, maxHP == 96 ? 80 : 49);
        EXPECT_EQ(gBattleMons[battler].item, maxHP == 96 ? ITEM_NONE : item);
    }
}
