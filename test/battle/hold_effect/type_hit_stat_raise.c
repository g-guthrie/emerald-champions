#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Type-hit items preserve their stat mapping, type requirement, and substitute exclusion")
{
    enum Item item = ITEM_NONE;
    enum Move move = MOVE_NONE;
    enum Stat stat = STAT_ATK;
    u32 scenario = 0;

    for (u32 mode = 0; mode < 3; mode++)
    {
        PARAMETRIZE { item = ITEM_SNOWBALL; move = MOVE_ICE_SHARD; stat = STAT_ATK; scenario = mode; }
        PARAMETRIZE { item = ITEM_LUMINOUS_MOSS; move = MOVE_WATER_GUN; stat = STAT_SPDEF; scenario = mode; }
        PARAMETRIZE { item = ITEM_CELL_BATTERY; move = MOVE_THUNDER_SHOCK; stat = STAT_ATK; scenario = mode; }
        PARAMETRIZE { item = ITEM_ABSORB_BULB; move = MOVE_WATER_GUN; stat = STAT_SPATK; scenario = mode; }
    }
    GIVEN {
        ASSUME(GetMoveType(MOVE_ICE_SHARD) == TYPE_ICE);
        ASSUME(GetMoveType(MOVE_WATER_GUN) == TYPE_WATER);
        ASSUME(GetMoveType(MOVE_THUNDER_SHOCK) == TYPE_ELECTRIC);
        ASSUME(GetMoveType(MOVE_TACKLE) == TYPE_NORMAL);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); }
    } WHEN {
        if (scenario == 2)
            TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, scenario == 1 ? MOVE_TACKLE : move); }
    } THEN {
        EXPECT_EQ(opponent->item, scenario == 0 ? ITEM_NONE : item);
        for (u32 checkStat = STAT_ATK; checkStat < NUM_BATTLE_STATS; checkStat++)
            EXPECT_EQ(opponent->statStages[checkStat], DEFAULT_STAT_STAGE + (scenario == 0 && checkStat == stat));
    }
}
