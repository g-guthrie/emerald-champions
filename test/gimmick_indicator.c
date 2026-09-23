#include "global.h"
#include "battle.h"
#include "battle_gimmick.h"
#include "sprite.h"
#include "test/test.h"

TEST("Gimmick indicator: missing indicators do not alter another sprite")
{
    static EWRAM_DATA struct BattleStruct state;
    struct BattleStruct *saved = gBattleStruct;
    gBattleStruct = &state;
    memset(&state, 0, sizeof(state));
    // Healthbox1 belongs to battler0; its absent indicator has the Safari sentinel0.
    gSprites[1].data[6] = 0;
    memset(&gSprites[0], 0, sizeof(gSprites[0]));
    struct Sprite before = gSprites[0];
    UpdateIndicatorOamPriority(1, 2);
    UpdateIndicatorLevelData(1, 100);
    EXPECT_EQ(memcmp(&gSprites[0], &before, sizeof(before)), 0);
    state.gimmick.indicatorSpriteId[0] = MAX_SPRITES;
    struct Sprite dummy = gSprites[MAX_SPRITES];
    UpdateIndicatorOamPriority(1, 2);
    UpdateIndicatorLevelData(1, 5);
    EXPECT_EQ(memcmp(&gSprites[MAX_SPRITES], &dummy, sizeof(dummy)), 0);
    // Valid indicators still receive their intended priority and level offset.
    state.gimmick.indicatorSpriteId[0] = 2;
    UpdateIndicatorOamPriority(1, 2);
    UpdateIndicatorLevelData(1, 100);
    EXPECT_EQ((u32)gSprites[2].oam.priority, 2);
    EXPECT_EQ(gSprites[2].data[3], -4);
    UpdateIndicatorLevelData(1, 5);
    EXPECT_EQ(gSprites[2].data[3], 5);
    UpdateIndicatorLevelData(1, 50);
    EXPECT_EQ(gSprites[2].data[3], 0);
    gBattleStruct = saved;
}
