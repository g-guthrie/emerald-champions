#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Confusion berries wait until quarter HP without Gluttony after Belly Drum")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_FIGY_BERRY; }
    PARAMETRIZE { item = ITEM_WIKI_BERRY; }
    PARAMETRIZE { item = ITEM_MAGO_BERRY; }
    PARAMETRIZE { item = ITEM_AGUAV_BERRY; }
    PARAMETRIZE { item = ITEM_IAPAPA_BERRY; }
    GIVEN {
        ASSUME(B_CONFUSE_BERRIES_HEAL >= GEN_8);
        PLAYER(SPECIES_ZIGZAGOON) { Ability(ABILITY_PICKUP); Nature(NATURE_HARDY); MaxHP(96); HP(96); Speed(100); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(24); Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_BELLY_DRUM); MOVE(opponent, MOVE_NIGHT_SHADE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BELLY_DRUM, player);
        HP_BAR(player, hp: 48);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_NIGHT_SHADE, opponent);
        HP_BAR(player, hp: 24);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_BERRY, player);
        HP_BAR(player, hp: 56);
    } THEN {
        enum BattlerId battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(gBattleMons[battler].hp, 56);
        EXPECT_EQ(gBattleMons[battler].item, ITEM_NONE);
    }
}

TO_DO_BATTLE_TEST("Figy Berry confuses a Pokemon whose nature dislikes spicy food")
