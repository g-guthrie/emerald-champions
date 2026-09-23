#include "global.h"
#include "gpu_regs.h"
#include "battle.h"
#include "battle_interface.h"
#include "task.h"
#include "battle_controllers.h"
#include "battle_gfx_sfx_util.h"
#include "field_effect.h"
#include "pokemon.h"
#include "constants/trainers.h"
#include "malloc.h"
#include "sprite.h"
#include "test/test.h"

extern u32 gOamMatrixAllocBitmap;

extern void Test_CompleteMonAnimation(struct Sprite *sprite, bool32 tracked);

TEST("Mon animation: an ordinary completion cannot finish another battle animation")
{
    bool32 tracked = FALSE;
    PARAMETRIZE { tracked = FALSE; }
    PARAMETRIZE { tracked = TRUE; }
    struct BattleStruct *previous = gBattleStruct;
    gBattleStruct = AllocZeroed(sizeof(*gBattleStruct));
    gBattleStruct->battlerKOAnimsRunning = 1;
    struct Sprite sprite = {0};
    Test_CompleteMonAnimation(&sprite, tracked);
    u32 remaining = gBattleStruct->battlerKOAnimsRunning;
    Free(gBattleStruct);
    gBattleStruct = previous;
    EXPECT_EQ(remaining, 1);
}

TEST("Mon animation: ordinary completion is valid outside battle")
{
    struct BattleStruct *previous = gBattleStruct;
    gBattleStruct = NULL;
    struct Sprite sprite = {0};
    Test_CompleteMonAnimation(&sprite, FALSE);
    EXPECT(gBattleStruct == NULL);
    gBattleStruct = previous;
}

TEST("Trainer portrait: front-facing player portrait releases its unused affine matrix")
{
    bool32 otherMatrix = FALSE;
    PARAMETRIZE { otherMatrix = FALSE; }
    PARAMETRIZE { otherMatrix = TRUE; }
    struct BattleStruct *previous = gBattleStruct;
    struct MonSpritesGfx *previousGfx = gMonSpritesGfxPtr;
    u32 flags = gBattleTypeFlags;
    gBattleStruct = AllocZeroed(sizeof(*gBattleStruct));
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    gBattlerPositions[B_BATTLER_0] = B_POSITION_PLAYER_LEFT;
    ResetSpriteData();
    FreeAllSpritePalettes();
    gReservedSpritePaletteCount = 0;
    u32 expectedMatrices = 0;
    if (otherMatrix)
        expectedMatrices = 1u << AllocOamMatrix();
    AllocateMonSpritesGfx();
    BtlController_HandleDrawTrainerPic(B_BATTLER_0, TRAINER_PIC_BRENDAN, TRUE, 80, 80, 0);
    u8 spriteId = gBattleStruct->trainerSlideSpriteIds[B_BATTLER_0];
    u32 matrices = gOamMatrixAllocBitmap;
    u32 affine = gSprites[spriteId].oam.affineMode;
    bool32 flipped = gSprites[spriteId].hFlip;
    s16 yOffset = gSprites[spriteId].y2;
    u8 paletteNum = gSprites[spriteId].oam.paletteNum;
    DestroySprite(&gSprites[spriteId]);
    FieldEffectFreePaletteIfUnused(paletteNum);
    FreeMonSpritesGfx();
    Free(gBattleStruct);
    ResetSpriteData();
    gBattleStruct = previous;
    gMonSpritesGfxPtr = previousGfx;
    gBattleTypeFlags = flags;
    EXPECT_EQ(affine, ST_OAM_AFFINE_OFF);
    EXPECT(flipped);
    EXPECT_EQ(yOffset, 48);
    EXPECT_EQ(matrices, expectedMatrices);
}

extern void Test_FreeOpponentTrainerPortrait(u8 spriteId);

TEST("Trainer portrait: shared palette survives until the final portrait is destroyed")
{
    struct BattleStruct *previous = gBattleStruct;
    struct MonSpritesGfx *previousGfx = gMonSpritesGfxPtr;
    u32 flags = gBattleTypeFlags;
    gBattleStruct = AllocZeroed(sizeof(*gBattleStruct));
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
    gBattlerPositions[B_BATTLER_1] = B_POSITION_OPPONENT_LEFT;
    gBattlerPositions[B_BATTLER_3] = B_POSITION_OPPONENT_RIGHT;
    ResetSpriteData();
    FreeAllSpritePalettes();
    gReservedSpritePaletteCount = 0;
    AllocateMonSpritesGfx();
    BtlController_HandleDrawTrainerPic(B_BATTLER_1, TRAINER_PIC_BRENDAN, TRUE, 80, 80, 0);
    BtlController_HandleDrawTrainerPic(B_BATTLER_3, TRAINER_PIC_BRENDAN, TRUE, 120, 80, 0);
    u8 first = gBattleStruct->trainerSlideSpriteIds[B_BATTLER_1];
    u8 second = gBattleStruct->trainerSlideSpriteIds[B_BATTLER_3];
    u16 tag = GetSpritePaletteTagByPaletteNum(gSprites[first].oam.paletteNum);
    EXPECT_EQ(gSprites[first].oam.paletteNum + 0u, gSprites[second].oam.paletteNum + 0u);
    Test_FreeOpponentTrainerPortrait(first);
    bool32 sharedPreserved = IndexOfSpritePaletteTag(tag) != 0xFF && gSprites[second].inUse;
    Test_FreeOpponentTrainerPortrait(second);
    bool32 released = IndexOfSpritePaletteTag(tag) == 0xFF;
    FreeMonSpritesGfx();
    Free(gBattleStruct);
    ResetSpriteData();
    gBattleStruct = previous;
    gMonSpritesGfxPtr = previousGfx;
    gBattleTypeFlags = flags;
    EXPECT(sharedPreserved);
    EXPECT(released);
}

static bool32 sSummaryControllerCompleted;
static void SummaryControllerComplete(enum BattlerId battler)
{
    sSummaryControllerCompleted = battler == B_BATTLER_0;
}
static void SummaryAlreadyHiding(u8 taskId) {}

TEST("Party summary: hide scheduling respects ownership and cleanup state")
{
    u32 state = 0;
    for (u32 i = 0; i < 6; i++)
        PARAMETRIZE { state = i; }
    struct BattleSpriteData data = {0};
    struct BattleHealthboxInfo boxes[MAX_BATTLERS_COUNT] = {0};
    struct BattleSpriteData *previous = gBattleSpritesDataPtr;
    void (*previousEnd)(enum BattlerId) = gBattlerControllerEndFuncs[B_BATTLER_0];
    u8 previousId = gBattlerStatusSummaryTaskId[B_BATTLER_0];
    data.healthBoxesData = boxes;
    gBattleSpritesDataPtr = &data;
    gBattlerControllerEndFuncs[B_BATTLER_0] = SummaryControllerComplete;
    u8 task = CreateTask(state == 0 ? SummaryAlreadyHiding : TaskDummy, 5);
    gTasks[task].data[0] = state == 2 ? B_BATTLER_1 : B_BATTLER_0;
    gBattlerStatusSummaryTaskId[B_BATTLER_0] = state == 4 ? TASK_NONE : task;
    boxes[0].partyStatusSummaryShown = state != 5;
    if (state == 3)
        DestroyTask(task);
    TaskFunc original = gTasks[task].func;
    sSummaryControllerCompleted = FALSE;
    BtlController_HandleHidePartyStatusSummary(B_BATTLER_0);
    bool32 preserved = state == 1 ? gTasks[task].func != original : gTasks[task].func == original;
    TaskFunc after = gTasks[task].func;
    BtlController_HandleHidePartyStatusSummary(B_BATTLER_0);
    preserved = preserved && gTasks[task].func == after;
    if (gTasks[task].isActive)
        DestroyTask(task);
    gBattleSpritesDataPtr = previous;
    gBattlerControllerEndFuncs[B_BATTLER_0] = previousEnd;
    gBattlerStatusSummaryTaskId[B_BATTLER_0] = previousId;
    EXPECT(preserved);
    EXPECT(sSummaryControllerCompleted);
}

extern bool32 Test_CompletePartySummaryHide(enum BattlerId battler, bool32 intro);

TEST("Party summary: shared blending survives until the final tray finishes")
{
    bool32 intro = FALSE, other = FALSE;
    for (u32 i = 0; i < 2; i++)
        for (u32 o = 0; o < 2; o++)
            PARAMETRIZE { intro = i; other = o; }
    struct BattleSpriteData data = {0};
    struct BattleHealthboxInfo boxes[MAX_BATTLERS_COUNT] = {0};
    struct BattleSpriteData *previous = gBattleSpritesDataPtr;
    u16 oldBlend = GetGpuReg(REG_OFFSET_BLDCNT), oldAlpha = GetGpuReg(REG_OFFSET_BLDALPHA);
    data.healthBoxesData = boxes;
    gBattleSpritesDataPtr = &data;
    boxes[0].partyStatusSummaryShown = TRUE;
    boxes[1].partyStatusSummaryShown = other;
    u16 blend = BLDCNT_TGT2_ALL | BLDCNT_EFFECT_BLEND;
    u16 alpha = BLDALPHA_BLEND(8, 8);
    SetGpuReg(REG_OFFSET_BLDCNT, blend);
    SetGpuReg(REG_OFFSET_BLDALPHA, alpha);
    bool32 completed = Test_CompletePartySummaryHide(B_BATTLER_0, intro);
    u16 afterBlend = GetGpuReg(REG_OFFSET_BLDCNT), afterAlpha = GetGpuReg(REG_OFFSET_BLDALPHA);
    bool32 cleared = !boxes[0].partyStatusSummaryShown;
    if (other)
        completed &= Test_CompletePartySummaryHide(B_BATTLER_1, !intro);
    u16 finalBlend = GetGpuReg(REG_OFFSET_BLDCNT), finalAlpha = GetGpuReg(REG_OFFSET_BLDALPHA);
    gBattleSpritesDataPtr = previous;
    SetGpuReg(REG_OFFSET_BLDCNT, oldBlend);
    SetGpuReg(REG_OFFSET_BLDALPHA, oldAlpha);
    EXPECT(completed && cleared);
    EXPECT_EQ(afterBlend, other ? blend : 0);
    EXPECT_EQ(afterAlpha, other ? alpha : 0);
    EXPECT_EQ(finalBlend, 0);
    EXPECT_EQ(finalAlpha, 0);
}

extern void Test_DestroyPartySummary(u8 taskId, bool32 intro);

TEST("Party summary: overlapping trays retain live graphics and release all final tags")
{
    bool32 firstIntro = FALSE, secondIntro = FALSE;
    for (u32 first = 0; first < 2; first++)
        for (u32 second = 0; second < 2; second++)
            PARAMETRIZE { firstIntro = first; secondIntro = second; }
    struct BattleSpriteData data = {0};
    struct BattleHealthboxInfo boxes[MAX_BATTLERS_COUNT] = {0};
    struct BattleAnimationInfo animation = {0};
    struct BattleSpriteData *previous = gBattleSpritesDataPtr;
    u32 flags = gBattleTypeFlags;
    data.healthBoxesData = boxes;
    data.animationData = &animation;
    gBattleSpritesDataPtr = &data;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    gBattlerPositions[0] = B_POSITION_PLAYER_LEFT;
    gBattlerPositions[1] = B_POSITION_OPPONENT_LEFT;
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeSpriteTileRanges();
    struct HpAndStatus party[PARTY_SIZE] = {0};
    for (u32 i = 0; i < PARTY_SIZE; i++)
        party[i].hp = 1;
    u8 first = CreatePartyStatusSummarySprites(B_BATTLER_0, party, FALSE, firstIntro);
    u8 second = CreatePartyStatusSummarySprites(B_BATTLER_1, party, FALSE, secondIntro);
    boxes[0].partyStatusSummaryShown = boxes[1].partyStatusSummaryShown = TRUE;
    u8 remainingBar = gTasks[second].data[1];
    u16 liveTileStart = gSprites[remainingBar].sheetTileStart;
    Test_DestroyPartySummary(first, firstIntro);
    bool32 retained = gSprites[remainingBar].inUse
        && GetSpriteTileTagByTileStart(liveTileStart) == TAG_STATUS_SUMMARY_BAR_TILE
        && IndexOfSpritePaletteTag(TAG_STATUS_SUMMARY_BAR_PAL) != 0xFF;
    Test_DestroyPartySummary(second, secondIntro);
    bool32 freed = GetSpriteTileStartByTag(TAG_STATUS_SUMMARY_BAR_TILE) == TAG_NONE
        && GetSpriteTileStartByTag(TAG_STATUS_SUMMARY_BALLS_TILE) == TAG_NONE
        && IndexOfSpritePaletteTag(TAG_STATUS_SUMMARY_BAR_PAL) == 0xFF
        && IndexOfSpritePaletteTag(TAG_STATUS_SUMMARY_BALLS_PAL) == 0xFF;
    gBattleSpritesDataPtr = previous;
    gBattleTypeFlags = flags;
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeSpriteTileRanges();
    EXPECT(retained);
    EXPECT(freed);
}

extern void Test_CreateBattlePopUpSprites(enum BattlerId battler, bool32 isDoubleBattle);

TEST("Battle popups: shared factory preserves per-battler templates and two-half tiles")
{
    struct BattleStruct *saved = gBattleStruct;
    u8 savedCount = gBattlersCount;
    gBattleStruct = AllocZeroed(sizeof(*gBattleStruct));
    gBattlersCount = 4;
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeSpriteTileRanges();
    gReservedSpritePaletteCount = 0;
    for (u32 battler = 0; battler < 4; battler++)
    {
        gBattlerPositions[battler] = battler;
        Test_CreateBattlePopUpSprites(battler, TRUE);
    }
    for (u32 battler = 0; battler < 4; battler++)
    {
        const u8 *ids = gBattleStruct->abilityPopUpSpriteIds[battler];
        EXPECT(gSprites[ids[0]].template != gSprites[ids[1]].template);
        EXPECT_EQ(gSprites[ids[0]].template->tileTag, gSprites[ids[1]].template->tileTag);
        EXPECT_EQ((u32)gSprites[ids[1]].oam.tileNum, gSprites[ids[0]].oam.tileNum + 32);
        if (battler)
            EXPECT_NE(gSprites[ids[0]].template->tileTag,
                gSprites[gBattleStruct->abilityPopUpSpriteIds[0][0]].template->tileTag);
    }
    FreeAbilityPopUpGfx();
    for (u32 battler = 0; battler < 4; battler++)
        for (u32 half = 0; half < 2; half++)
            FieldEffectFreeGraphicsResources(&gSprites[gBattleStruct->abilityPopUpSpriteIds[battler][half]]);
    Free(gBattleStruct);
    gBattleStruct = saved;
    gBattlersCount = savedCount;
}
