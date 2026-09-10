#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Belly Drum activates half-HP healing items at either HP parity")
{
    u32 maxHP = 96;
    enum Item item = ITEM_SITRUS_BERRY;
    static const enum Item items[] = {ITEM_SITRUS_BERRY, ITEM_FIGY_BERRY, ITEM_BERRY_JUICE, ITEM_ORAN_BERRY};
    for (u32 i = 0; i < ARRAY_COUNT(items); i++)
    {
        PARAMETRIZE { maxHP = 96; item = items[i]; }
        PARAMETRIZE { maxHP = 97; item = items[i]; }
    }
    u32 afterDrum = maxHP - maxHP / 2;
    u32 healed = afterDrum + (item == ITEM_SITRUS_BERRY ? 24 : item == ITEM_FIGY_BERRY ? 32
        : item == ITEM_BERRY_JUICE ? 20 : 10);
    GIVEN {
        PLAYER(SPECIES_ZIGZAGOON) { Ability(ABILITY_GLUTTONY); Nature(NATURE_HARDY); MaxHP(maxHP); HP(maxHP); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BELLY_DRUM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BELLY_DRUM, player);
        HP_BAR(player, hp: afterDrum);
        HP_BAR(player, hp: healed);
    } THEN {
        enum BattlerId battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(gBattleMons[battler].hp, healed);
        EXPECT_EQ(gBattleMons[battler].item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("HP-threshold items activate one HP above their normal cutoff but never at full HP")
{
    static const enum Item items[] = {
        ITEM_ORAN_BERRY, ITEM_BERRY_JUICE, ITEM_SITRUS_BERRY,
        ITEM_FIGY_BERRY, ITEM_WIKI_BERRY, ITEM_MAGO_BERRY, ITEM_AGUAV_BERRY, ITEM_IAPAPA_BERRY,
        ITEM_LIECHI_BERRY, ITEM_GANLON_BERRY, ITEM_SALAC_BERRY, ITEM_PETAYA_BERRY, ITEM_APICOT_BERRY,
        ITEM_LANSAT_BERRY, ITEM_STARF_BERRY, ITEM_MICLE_BERRY, ITEM_CUSTAP_BERRY,
    };
    enum Item item = ITEM_SITRUS_BERRY;
    enum Ability ability = ABILITY_PICKUP;
    u32 maxHP = 100, hp = 51;
    bool32 activates = TRUE;
    for (u32 i = 0; i < ARRAY_COUNT(items); i++)
    {
        for (u32 odd = 0; odd < 2; odd++)
        {
            for (u32 gluttony = 0; gluttony < 2; gluttony++)
            {
                for (u32 above = 0; above < 2; above++)
                PARAMETRIZE_LABEL("item=%d odd=%d gluttony=%d above=%d", items[i], odd, gluttony, above)
                {
                    item = items[i];
                    maxHP = odd ? 103 : 100;
                    ability = gluttony ? ABILITY_GLUTTONY : ABILITY_PICKUP;
                    // Literal boundaries, independent of the production helper:
                    // half=51/52; quarter=26 at either maximum.
                    hp = (i < 3 || gluttony ? (odd ? 52 : 51) : 26) + above;
                    activates = !above;
                }
            }
        }
        PARAMETRIZE_LABEL("item=%d fullHP=1", items[i])
        {
            item = items[i]; maxHP = hp = 1; ability = ABILITY_GLUTTONY; activates = FALSE;
        }
    }
    GIVEN {
        PLAYER(SPECIES_ZIGZAGOON) { Ability(ability); Nature(NATURE_HARDY); MaxHP(maxHP); HP(hp); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        enum BattlerId battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        u32 heal = item == ITEM_ORAN_BERRY ? 10 : item == ITEM_BERRY_JUICE ? 20
            : item == ITEM_SITRUS_BERRY ? maxHP / 4
            : item >= ITEM_FIGY_BERRY && item <= ITEM_IAPAPA_BERRY ? maxHP / 3 : 0;
        EXPECT_EQ(gBattleMons[battler].hp, activates ? min(maxHP, hp + heal) : hp);
        EXPECT_EQ(gBattleMons[battler].item, activates ? ITEM_NONE : item);
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
