#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_interface.h"
#include "battle_gimmick.h"
#include "battle_z_move.h"
#include "battle_setup.h"
#include "emerald_champions_battle_plan.h"
#include "battle_util.h"
#include "item.h"
#include "mega_stone_rewards.h"
#include "palette.h"
#include "pokemon.h"
#include "sprite.h"
#include "util.h"
#include "test_runner.h"
#include "constants/emerald_champions.h"

#include "data/gimmicks.h"

// Populates gBattleStruct->gimmick.usableGimmick for each battler.
void AssignUsableGimmicks(void)
{
    for (enum BattlerId battler = 0; battler < gBattlersCount; ++battler)
    {
        gBattleStruct->gimmick.usableGimmick[battler] = GIMMICK_NONE;
        for (enum Gimmick gimmick = 0; gimmick < GIMMICKS_COUNT; ++gimmick)
        {
            if (CanActivateGimmick(battler, gimmick))
            {
                gBattleStruct->gimmick.usableGimmick[battler] = gimmick;
                break;
            }
        }
    }
}

// Returns whether a battler is able to use a gimmick. Checks consumption and gimmick specific functions.
bool32 CanActivateGimmick(enum BattlerId battler, enum Gimmick gimmick)
{
#if !TESTING
    if (!IsEmeraldChampionsGimmickAllowed(gimmick))
        return FALSE;
#endif
    return gGimmicksInfo[gimmick].CanActivate != NULL && gGimmicksInfo[gimmick].CanActivate(battler);
}

bool32 IsEmeraldChampionsGimmickAllowed(enum Gimmick gimmick)
{
    if (!EMERALD_CHAMPIONS_MEGA_ONLY)
        return TRUE;
    return gimmick == GIMMICK_NONE || gimmick == GIMMICK_MEGA;
}

// Returns whether the player has a gimmick selected while in the move selection menu.
bool32 IsGimmickSelected(enum BattlerId battler, enum Gimmick gimmick)
{
    // There's no player select in tests, but some gimmicks need to test choice before they are fully activated.
    #if TESTING
    return (gBattleStruct->gimmick.toActivate & (1u << battler)) && gBattleStruct->gimmick.usableGimmick[battler] == gimmick;
    #else
    return gBattleStruct->gimmick.usableGimmick[battler] == gimmick && gBattleStruct->gimmick.playerSelect;
    #endif
}

// Sets a battler as having a gimmick active using their party index.
void SetActiveGimmick(enum BattlerId battler, enum Gimmick gimmick)
{
    gBattleStruct->gimmick.activeGimmick[GetBattlerTrainer(battler)][gBattlerPartyIndexes[battler]] = gimmick;
}

// Returns a battler's active gimmick, if any.
enum Gimmick GetActiveGimmick(enum BattlerId battler)
{
    return gBattleStruct->gimmick.activeGimmick[GetBattlerTrainer(battler)][gBattlerPartyIndexes[battler]];
}

// Returns whether a trainer mon is intended to use an unrestrictive gimmick via .useGimmick.
bool32 ShouldTrainerBattlerUseGimmick(enum BattlerId battler, enum Gimmick gimmick)
{
    // There are no trainer party settings in battles, but the AI needs to know which gimmick to use.
    #if TESTING
    return gimmick == TestRunner_Battle_GetChosenGimmick(GetBattlerTrainer(battler), gBattlerPartyIndexes[battler]);
    #else
    // The player can bypass these checks because they can choose through the controller.
    if (IsOnPlayerSide(battler) && !((gBattleTypeFlags & BATTLE_TYPE_MULTI) && GetBattlerPosition(battler) == B_POSITION_PLAYER_RIGHT))
        return TRUE;

    // When reading trainer party data, we load invalid values in struct Pokemon to indicate the gimmick should not be used
    struct Pokemon *mon = GetBattlerMon(battler);
    if (gimmick == GIMMICK_DYNAMAX && GetMonData(mon, MON_DATA_DYNAMAX_LEVEL) != BLOCK_AI_DYNAMAX)
        return TRUE;
    #endif

    return FALSE;
}

// Returns whether a trainer has used a gimmick during a battle.
bool32 HasTrainerUsedGimmick(enum BattlerId battler, enum Gimmick gimmick)
{
    if (IsDoubleBattle() && (IsPartnerMonFromSameTrainer(battler) || (gimmick == GIMMICK_DYNAMAX)))
    {
        enum BattlerId partner = GetPartnerBattler(battler);
        if (gBattleStruct->gimmick.activated[partner][gimmick]
         || ((gBattleStruct->gimmick.toActivate & (1u << partner)) && gBattleStruct->gimmick.usableGimmick[partner] == gimmick))
            return TRUE;
    }

    return gBattleStruct->gimmick.activated[battler][gimmick];
}

u32 GetRemainingMegaEvolutions(enum BattlerId battler)
{
    u32 limit = EmeraldChampions_GetMegaEvolutionLimit(battler);
    u32 used = gBattleStruct->gimmick.megaEvolutionsUsed[GetBattlerTrainer(battler)];
    return used < limit ? limit - used : 0;
}

// Whether this battler may still activate `gimmick` right now. Mega asks the
// trainer's authored allowance instead of the one-per-side default, so a team
// licensed for two stones can still evolve after its partner already has.
bool32 CanTrainerStillActivateGimmick(enum BattlerId battler, enum Gimmick gimmick)
{
    u32 reserved = 0;
    enum BattlerId partner;

    if (gimmick != GIMMICK_MEGA || EmeraldChampions_GetMegaEvolutionLimit(battler) == 1)
        return !HasTrainerUsedGimmick(battler, gimmick);

    if (GetActiveGimmick(battler) == GIMMICK_MEGA)
        return FALSE;

    // A partner that has already committed to Mega this turn holds one charge.
    partner = GetPartnerBattler(battler);
    if (IsDoubleBattle() && IsPartnerMonFromSameTrainer(battler)
     && (gBattleStruct->gimmick.toActivate & (1u << partner))
     && gBattleStruct->gimmick.usableGimmick[partner] == GIMMICK_MEGA
     && GetActiveGimmick(partner) != GIMMICK_MEGA)
        reserved = 1;

    return GetRemainingMegaEvolutions(battler) > reserved;
}

// Sets a gimmick as used by a trainer with checks for Multi Battles.
void SetGimmickAsActivated(enum BattlerId battler, enum Gimmick gimmick)
{
    if (gimmick == GIMMICK_MEGA)
    {
        gBattleStruct->gimmick.megaEvolutionsUsed[GetBattlerTrainer(battler)]++;
        // The Trainer Card records every Mega the player watches, either side.
        EmeraldChampions_RecordMegaWitnessed(gBattleMons[battler].item);
    }
    gBattleStruct->gimmick.activated[battler][gimmick] = TRUE;
    if (IsDoubleBattle() && (IsPartnerMonFromSameTrainer(battler) || (gimmick == GIMMICK_DYNAMAX)))
        gBattleStruct->gimmick.activated[GetPartnerBattler(battler)][gimmick] = TRUE;
}

#define SINGLES_GIMMICK_TRIGGER_POS_X_OPTIMAL (30)
#define SINGLES_GIMMICK_TRIGGER_POS_X_PRIORITY (31)
#define SINGLES_GIMMICK_TRIGGER_POS_X_SLIDE (15)
#define SINGLES_GIMMICK_TRIGGER_POS_Y_DIFF (-5)

#define DOUBLES_GIMMICK_TRIGGER_POS_X_OPTIMAL (30)
#define DOUBLES_GIMMICK_TRIGGER_POS_X_PRIORITY (31)
#define DOUBLES_GIMMICK_TRIGGER_POS_X_SLIDE (15)
#define DOUBLES_GIMMICK_TRIGGER_POS_Y_DIFF (-2)

#define tBattler    data[0]
#define tHide       data[1]

void ChangeGimmickTriggerSprite(u32 spriteId, u32 animId)
{
    StartSpriteAnim(&gSprites[spriteId], animId);
}

void CreateGimmickTriggerSprite(enum BattlerId battler)
{
    const struct GimmickInfo * gimmick = &gGimmicksInfo[gBattleStruct->gimmick.usableGimmick[battler]];

    // Exit if there shouldn't be a sprite produced.
    if (!IsOnPlayerSide(battler)
     || gBattleStruct->gimmick.usableGimmick[battler] == GIMMICK_NONE
     || gimmick->triggerSheet == NULL
     || HasTrainerUsedGimmick(battler, gBattleStruct->gimmick.usableGimmick[battler]))
    {
        return;
    }

    LoadSpritePalette(gimmick->triggerPal);
    if (GetSpriteTileStartByTag(TAG_GIMMICK_TRIGGER_TILE) == 0xFFFF)
        LoadSpriteSheet(gimmick->triggerSheet);

    if (gBattleStruct->gimmick.triggerSpriteId == 0xFF)
    {
        if (GetBattlerCoordsIndex(battler) == BATTLE_COORDS_DOUBLES)
            gBattleStruct->gimmick.triggerSpriteId = CreateSprite(gimmick->triggerTemplate,
                                                                  gSprites[gHealthboxSpriteIds[battler]].x - DOUBLES_GIMMICK_TRIGGER_POS_X_SLIDE,
                                                                  gSprites[gHealthboxSpriteIds[battler]].y - DOUBLES_GIMMICK_TRIGGER_POS_Y_DIFF, 0);
        else
            gBattleStruct->gimmick.triggerSpriteId = CreateSprite(gimmick->triggerTemplate,
                                                                  gSprites[gHealthboxSpriteIds[battler]].x - SINGLES_GIMMICK_TRIGGER_POS_X_SLIDE,
                                                                  gSprites[gHealthboxSpriteIds[battler]].y - SINGLES_GIMMICK_TRIGGER_POS_Y_DIFF, 0);
    }

    gSprites[gBattleStruct->gimmick.triggerSpriteId].tBattler = battler;
    gSprites[gBattleStruct->gimmick.triggerSpriteId].tHide = FALSE;

    ChangeGimmickTriggerSprite(gBattleStruct->gimmick.triggerSpriteId, 0);
}

bool32 IsGimmickTriggerSpriteActive(void)
{
    if (GetSpriteTileStartByTag(TAG_GIMMICK_TRIGGER_TILE) == 0xFFFF)
        return FALSE;

    if (IndexOfSpritePaletteTag(TAG_GIMMICK_TRIGGER_PAL) != 0xFF)
        return TRUE;

    return FALSE;
}

bool32 IsGimmickTriggerSpriteMatchingBattler(enum BattlerId battler)
{
    if (battler == gSprites[gBattleStruct->gimmick.triggerSpriteId].tBattler)
        return TRUE;

    return FALSE;
}

void HideGimmickTriggerSprite(void)
{
    if (gBattleStruct->gimmick.triggerSpriteId != 0xFF)
    {
        ChangeGimmickTriggerSprite(gBattleStruct->gimmick.triggerSpriteId, 0);
        gSprites[gBattleStruct->gimmick.triggerSpriteId].tHide = TRUE;
    }
}

void DestroyGimmickTriggerSprite(void)
{
    FreeSpritePaletteByTag(TAG_GIMMICK_TRIGGER_PAL);
    FreeSpriteTilesByTag(TAG_GIMMICK_TRIGGER_TILE);
    if (gBattleStruct->gimmick.triggerSpriteId != 0xFF)
        DestroySprite(&gSprites[gBattleStruct->gimmick.triggerSpriteId]);
    gBattleStruct->gimmick.triggerSpriteId = 0xFF;
}

static void SpriteCb_GimmickTrigger(struct Sprite *sprite)
{
    s32 xSlide, xPriority, xOptimal;
    s32 yDiff;
    s32 xHealthbox = gSprites[gHealthboxSpriteIds[sprite->tBattler]].x;

    if (GetBattlerCoordsIndex(sprite->tBattler) == BATTLE_COORDS_DOUBLES)
    {
        xSlide = DOUBLES_GIMMICK_TRIGGER_POS_X_SLIDE;
        xPriority = DOUBLES_GIMMICK_TRIGGER_POS_X_PRIORITY;
        xOptimal = DOUBLES_GIMMICK_TRIGGER_POS_X_OPTIMAL;
        yDiff = DOUBLES_GIMMICK_TRIGGER_POS_Y_DIFF;
    }
    else
    {
        xSlide = SINGLES_GIMMICK_TRIGGER_POS_X_SLIDE;
        xPriority = SINGLES_GIMMICK_TRIGGER_POS_X_PRIORITY;
        xOptimal = SINGLES_GIMMICK_TRIGGER_POS_X_OPTIMAL;
        yDiff = SINGLES_GIMMICK_TRIGGER_POS_Y_DIFF;
    }

    if (sprite->tHide)
    {
        if (sprite->x < xHealthbox - xSlide)
            sprite->x++;

        if (sprite->x >= xHealthbox - xPriority)
            sprite->oam.priority = 2;
        else
            sprite->oam.priority = 1;

        sprite->y = gSprites[gHealthboxSpriteIds[sprite->tBattler]].y - yDiff;
        sprite->y2 = gSprites[gHealthboxSpriteIds[sprite->tBattler]].y2 - yDiff;
        if (sprite->x == xHealthbox - xSlide)
            DestroyGimmickTriggerSprite();
    }
    else
    {
        // Edge case: in doubles, if selecting move and next mon's action too fast, the second battler's gimmick icon uses the x from the first battler's gimmick icon
        if (sprite->y != gSprites[gHealthboxSpriteIds[sprite->tBattler]].y - yDiff)
            sprite->x = xHealthbox - xSlide;

        if (sprite->x > xHealthbox - xOptimal)
            sprite->x--;

        if (sprite->x >= xHealthbox - xPriority)
            sprite->oam.priority = 2;
        else
            sprite->oam.priority = 1;

        sprite->y = gSprites[gHealthboxSpriteIds[sprite->tBattler]].y - yDiff;
        sprite->y2 = gSprites[gHealthboxSpriteIds[sprite->tBattler]].y2 - yDiff;
    }
}

#undef tBattler
#undef tHide

// for sprite data fields
#define tBattler        data[0]
#define tPosX           data[2]
#define tLevelXDelta    data[3] // X position depends whether level has 3, 2 or 1 digit

// data fields for healthboxMain
// oam.affineParam holds healthboxRight spriteId
#define hMain_Battler               data[6]

void LoadIndicatorSpritesGfx(void)
{
    LoadSpritePalette(&sSpritePalette_MiscIndicator);
    LoadSpritePalette(&sSpritePalette_MegaIndicator);
}

static void SpriteCb_GimmickIndicator(struct Sprite *sprite)
{
    enum BattlerId battler = sprite->tBattler;

    sprite->x = gSprites[gHealthboxSpriteIds[battler]].x + sprite->tPosX + sprite->tLevelXDelta;
    sprite->x2 = gSprites[gHealthboxSpriteIds[battler]].x2;
    sprite->y2 = gSprites[gHealthboxSpriteIds[battler]].y2;
}

static struct Sprite *GetIndicatorSprite(u32 healthboxId)
{
    u32 id = gBattleStruct->gimmick.indicatorSpriteId[gSprites[healthboxId].hMain_Battler];
    // Safari's player healthbox has no indicator (zero). Failed allocations
    // return MAX_SPRITES, which is not a live indicator either.
    return id != 0 && id < MAX_SPRITES ? &gSprites[id] : NULL;
}

const u32 *GetIndicatorSpriteSrc(enum BattlerId battler)
{
    enum Gimmick gimmick = GetActiveGimmick(battler);

    if (IsBattlerPrimalReverted(battler))
    {
        if (gBattleMons[battler].species == SPECIES_GROUDON_PRIMAL)
            return (u32 *)&sOmegaIndicatorGfx;
        else
            return (u32 *)&sAlphaIndicatorGfx;
    }

    if (gGimmicksInfo[gimmick].indicatorData != NULL)
        return (u32 *)gGimmicksInfo[gimmick].indicatorData;

    return NULL;
}

u32 GetIndicatorPalTag(enum BattlerId battler)
{
    enum Gimmick gimmick = GetActiveGimmick(battler);
    if (IsBattlerPrimalReverted(battler))
        return TAG_MISC_INDICATOR_PAL;
    else if (gGimmicksInfo[gimmick].indicatorPalTag != 0)
        return gGimmicksInfo[gimmick].indicatorPalTag;
    else
        return TAG_NONE;
}

#define INDICATOR_SIZE (8 * 16 / 2)

void UpdateIndicatorVisibilityAndType(u32 healthboxId, bool32 invisible)
{
    enum BattlerId battler = gSprites[healthboxId].hMain_Battler;
    u32 palTag = GetIndicatorPalTag(battler);
    struct Sprite *sprite = GetIndicatorSprite(healthboxId);

    if (sprite == NULL)
        return;

    if (palTag != TAG_NONE)
    {
        sprite->oam.paletteNum = IndexOfSpritePaletteTag(palTag);
        sprite->invisible = invisible;

        u32 *dst = (u32 *)(OBJ_VRAM0 + TILE_SIZE_4BPP * GetSpriteTileStartByTag(BATTLER_INDICATOR_TAG + battler));

        const u32 *src = GetIndicatorSpriteSrc(battler);

        for (u32 i = 0; i < INDICATOR_SIZE / 4; i++)
            dst[i] = src[i];
    }
    else // in case of error
    {
        sprite->invisible = TRUE;
    }
}

#undef INDICATOR_SIZE

void UpdateIndicatorOamPriority(u32 healthboxId, u32 oamPriority)
{
    struct Sprite *sprite = GetIndicatorSprite(healthboxId);
    if (sprite != NULL)
        sprite->oam.priority = oamPriority;
}

void UpdateIndicatorLevelData(u32 healthboxId, u32 level)
{
    s32 xDelta = 0;
    struct Sprite *sprite = GetIndicatorSprite(healthboxId);
    if (sprite == NULL)
        return;

    if (level >= 100)
        xDelta -= 4;
    else if (level < 10)
        xDelta += 5;

    sprite->tLevelXDelta = xDelta;
}

static const s8 sIndicatorPositions[][2] =
{
    [B_POSITION_PLAYER_LEFT] = {49, -9},
    [B_POSITION_OPPONENT_LEFT] = {40, -9},
    [B_POSITION_PLAYER_RIGHT] = {48, -9},
    [B_POSITION_OPPONENT_RIGHT] = {40, -9},
};

void CreateIndicatorSprite(enum BattlerId battler)
{
    enum BattlerPosition position;
    u32 spriteId;
    s16 xHealthbox = 0, x = 0, y = 0;

    position = GetBattlerPosition(battler);
    GetBattlerHealthboxCoords(battler, &xHealthbox, &y);

    x = sIndicatorPositions[position][0];
    y += sIndicatorPositions[position][1];

    LoadSpriteSheet(&sBattler_GimmickSpritesheets[battler]);
    spriteId = CreateSprite(&(sSpriteTemplate_BattlerIndicators[battler]), 0, y, 0);
    gBattleStruct->gimmick.indicatorSpriteId[battler] = spriteId;
    gSprites[spriteId].tBattler = battler;
    gSprites[spriteId].tPosX = x;
    gSprites[spriteId].invisible = FALSE;
}

#undef tBattler
#undef tPosX
#undef tLevelXDelta

#undef hMain_Battler
