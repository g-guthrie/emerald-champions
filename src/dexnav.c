#include "global.h"
#include "caps.h"
#include "battle_main.h"
#include "battle_pike.h"
#include "battle_pyramid.h"
#include "battle_setup.h"
#include "bg.h"
#include "data.h"
#include "daycare.h"
#include "decompress.h"
#include "dexnav.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "event_scripts.h"
#include "field_effect.h"
#include "field_effect_helpers.h"
#include "field_message_box.h"
#include "field_player_avatar.h"
#include "field_screen_effect.h"
#include "fieldmap.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "item.h"
#include "international_string_util.h"
#include "list_menu.h"
#include "m4a.h"
#include "map_name_popup.h"
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "metatile_behavior.h"
#include "move.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokemon_summary_screen.h"
#include "random.h"
#include "region_map.h"
#include "roamer.h"
#include "rtc.h"
#include "safari_zone.h"
#include "scanline_effect.h"
#include "script.h"
#include "script_pokemon_util.h"
#include "sound.h"
#include "sprite.h"
#include "start_menu.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "wild_encounter.h"
#include "wild_roster.h"
#include "pokerus.h"
#include "guided_tutorial.h"
#include "config/pokerus.h"
#include "weather_anomaly.h"
#include "window.h"
#include "constants/species.h"
#include "constants/maps.h"
#include "constants/field_effects.h"
#include "constants/items.h"
#include "constants/songs.h"
#include "constants/abilities.h"
#include "constants/rgb.h"
#include "constants/region_map_sections.h"
#include "gba/m4a_internal.h"

#if DEXNAV_ENABLED
STATIC_ASSERT(DN_FLAG_SEARCHING != 0, DNFlagSearching_Must_Not_Be_Zero);
STATIC_ASSERT(DN_VAR_SPECIES != 0, DNVarSpecies_Must_Not_Be_Zero);
#endif

struct DexNavSearch
{
    enum Species species;
    u8 monLevel;
    u8 proximity;
    u8 environment;
    s16 tileX;
    s16 tileY;
    u8 fldEffSpriteId;
    u8 fldEffId;
    u8 movementCount;
    u8 windowId;
    u8 iconSpriteId;
    u8 ownedIconSpriteId;
    u32 startingTime;
};

// RAM

EWRAM_DATA static struct DexNavSearch *sDexNavSearchDataPtr = NULL;
EWRAM_DATA enum Species gDexNavSpecies = SPECIES_NONE;

//// Function Declarations
//GUI
static void Task_DexNavWaitFadeIn(u8 taskId);
static void Task_DexNavMain(u8 taskId);
// SEARCH
static bool8 TryStartHiddenMonFieldEffect(enum EncounterType environment, u8 xSize, u8 ySize, bool8 smallScan);
static u8 DexNavTryGenerateMonLevel(enum Species species, enum EncounterType environment);
static u8 GetEncounterLevelFromMapData(enum Species species, enum EncounterType environment);
static u8 GetPlayerDistance(s16 x, s16 y);
static u8 DexNavPickTile(enum EncounterType environment, u8 xSize, u8 ySize, bool8 smallScan);
static void DexNavProximityUpdate(void);
static void DexNavDrawIcons(void);
static void DexNavUpdateSearchWindow(void);
static bool32 IsDexNavSearchableEntry(const struct WildRosterEntry *entry);

//// Const Data
// gui image data
static const u32 sDexNavGuiTiles[] = INCGFX_U32("graphics/dexnav/gui_tiles.png", ".4bpp.smol");
static const u32 sDexNavGuiPal[] = INCGFX_U32("graphics/dexnav/gui.pal", ".gbapal");

static const u32 sSelectionCursorGfx[] = INCGFX_U32("graphics/dexnav/cursor.png", ".4bpp.smol");
static const u16 sSelectionCursorPal[] = INCGFX_U16("graphics/dexnav/cursor.png", ".gbapal");
static const u32 sCaughtMarkGfx[] = INCGFX_U32("graphics/dexnav/caught.png", ".4bpp.smol");  //uses selection cursor pal

// searching image data
static const u32 sOwnedIconGfx[] = INCGFX_U32("graphics/dexnav/owned_icon.png", ".4bpp.smol");

// strings

static const u8 sText_ArrowLeft[] = _("{LEFT_ARROW}");
static const u8 sText_ArrowRight[] = _("{RIGHT_ARROW}");
static const u8 sText_ArrowUp[] = _("{UP_ARROW}");
static const u8 sText_ArrowDown[] = _("{DOWN_ARROW}");

//search window font
static const u8 sSearchFontColor[3] = {0, 15, 13};

static const struct OamData sHeldItemOam =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .priority = 0,
    .paletteNum = 13,
};

static const struct OamData sSelectionCursorOam =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = 0,
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x32),
    .tileNum = 0,
    .priority = 0, // above BG layers
    .paletteNum = 12,
    .affineParam = 0
};

// gui sprite templates
static const struct SpriteTemplate sSelectionCursorSpriteTemplate =
{
    .tileTag = SELECTION_CURSOR_TAG,
    .paletteTag = 0xFFFF,
    .oam = &sSelectionCursorOam,
    .anims =  gDummySpriteAnimTable,
};

// search window sprite templates
static const struct SpriteTemplate sOwnedIconTemplate =
{
    .tileTag = OWNED_ICON_TAG,
    .paletteTag = 0xFFFF,   //held item pal
    .oam = &sHeldItemOam,
    .anims =  gDummySpriteAnimTable,
};

// search sprite sheets
static const struct CompressedSpriteSheet sOwnedIconSpriteSheet = {sOwnedIconGfx, (8 * 8) / 2, OWNED_ICON_TAG};

//// functions
///////////////////////
//// DEXNAV SEARCH ////
///////////////////////
static s16 GetSearchWindowY(void)
{
    return (GetWindowAttribute(sDexNavSearchDataPtr->windowId, WINDOW_TILEMAP_TOP) * 8);
}

#define SPECIES_ICON_X 28
static void DrawDexNavSearchMonIcon(enum Species species, u8 *dst, bool8 owned)
{
    u8 spriteId;

    LoadMonIconPalette(species);
    spriteId = CreateMonIcon(species, SpriteCB_MonIcon, SPECIES_ICON_X - 6, GetSearchWindowY() + 8, 0, 0xFFFFFFFF);
    gSprites[spriteId].oam.priority = 0;
    *dst = spriteId;

    if (owned)
        sDexNavSearchDataPtr->ownedIconSpriteId = CreateSprite(&sOwnedIconTemplate, SPECIES_ICON_X + 6, GetSearchWindowY() + 4, 0);
}

static void AddSearchWindow(u8 width)
{
    struct WindowTemplate template;
    u16 y = 16;

    if (sDexNavSearchDataPtr->tileY > (gSaveBlock1Ptr->pos.y + 7))
        y = 1;  //draw at top if chosen tile is below

    LoadDexNavWindowGfx(sDexNavSearchDataPtr->windowId, 0x1d5, 14 * 16);

    SetWindowTemplateFields(&template, 0, 1, y, width, 3, 14, 8);

    sDexNavSearchDataPtr->windowId = AddWindow(&template);
    FillWindowPixelBuffer(sDexNavSearchDataPtr->windowId, PIXEL_FILL(1));
    PutWindowTilemap(sDexNavSearchDataPtr->windowId);
    CopyWindowToVram(sDexNavSearchDataPtr->windowId, 3);

    DrawStdFrameWithCustomTileAndPalette(sDexNavSearchDataPtr->windowId, TRUE, 0x214, 14);
}

#define WINDOW_COL_0        (SPECIES_ICON_X + 4)
#define SEARCH_ARROW_X      208
#define SEARCH_ARROW_Y      0
#define SEARCH_WINDOW_WIDTH 28

static const u8 sText_SearchChain[] = _("Chain {STR_VAR_1}");
static const u8 sText_SearchSneak[] = _("Hold A to sneak");
static const u8 sText_TutorialSneak[] = _("Holding A to sneak");

static void AddSearchWindowText(enum Species species)
{
    u8 windowId = sDexNavSearchDataPtr->windowId;
    AddTextPrinterParameterized3(windowId, FONT_SMALL, WINDOW_COL_0, 0, sSearchFontColor, TEXT_SKIP_DRAW, GetSpeciesName(species));
    AddTextPrinterParameterized3(windowId, FONT_SMALL, WINDOW_COL_0, 12, sSearchFontColor, TEXT_SKIP_DRAW, IsRivalDexNavTutorialActive() ? sText_TutorialSneak : sText_SearchSneak);
    if (GetDexNavChain())
    {
        ConvertIntToDecimalStringN(gStringVar1, GetDexNavChain(), STR_CONV_MODE_LEFT_ALIGN, 3);
        StringExpandPlaceholders(gStringVar4, sText_SearchChain);
        AddTextPrinterParameterized3(windowId, FONT_SMALL, 154, 12, sSearchFontColor, TEXT_SKIP_DRAW, gStringVar4);
    }
    CopyWindowToVram(windowId, COPYWIN_GFX);
}

static void DrawSearchWindow(enum Species species)
{
    AddSearchWindow(SEARCH_WINDOW_WIDTH);
    AddSearchWindowText(species);
}

#undef SEARCH_WINDOW_WIDTH

static void RemoveDexNavWindowAndGfx(void)
{
    // try remove sprites
    if (sDexNavSearchDataPtr->iconSpriteId != MAX_SPRITES)
        DestroySprite(&gSprites[sDexNavSearchDataPtr->iconSpriteId]);
    if (sDexNavSearchDataPtr->ownedIconSpriteId != MAX_SPRITES)
        DestroySprite(&gSprites[sDexNavSearchDataPtr->ownedIconSpriteId]);

    FreeSpriteTilesByTag(OWNED_ICON_TAG);
    FreeSpritePaletteByTag(HELD_ITEM_TAG);
    SafeFreeMonIconPalette(sDexNavSearchDataPtr->species);

    // remove window
    ClearStdWindowAndFrameToTransparent(sDexNavSearchDataPtr->windowId, FALSE);
    CopyWindowToVram(sDexNavSearchDataPtr->windowId, 3);
    RemoveWindow(sDexNavSearchDataPtr->windowId);
}


//////////////////////
////DEXNAV SEARCH/////
//////////////////////
static u8 GetPlayerDistance(s16 x, s16 y)
{
    s16 originX = gSaveBlock1Ptr->pos.x + MAP_OFFSET, originY = gSaveBlock1Ptr->pos.y + MAP_OFFSET;
    RivalTutorialGetSearchOrigin(&originX, &originY);
    u16 deltaX = abs(x - originX);
    u16 deltaY = abs(y - originY);
    return deltaX + deltaY;
}

static void DexNavProximityUpdate(void)
{
    sDexNavSearchDataPtr->proximity = GetPlayerDistance(sDexNavSearchDataPtr->tileX, sDexNavSearchDataPtr->tileY);
}

//Pick a specific tile based on environment
static bool8 DexNavPickTile(enum EncounterType environment, u8 areaX, u8 areaY, bool8 smallScan)
{
    // area of map to cover starting from camera position {-7, -7}
    s16 topX = gSaveBlock1Ptr->pos.x - SCANSTART_X + (smallScan * 5);
    s16 topY = gSaveBlock1Ptr->pos.y - SCANSTART_Y + (smallScan * 5);
    s16 botX = topX + areaX;
    s16 botY = topY + areaY;
    u8 i;
    bool8 nextIter;
    u8 scale = 0;
    u8 weight = 0;
    enum MapType currMapType = GetCurrentMapType();
    u8 tileBehaviour;
    u8 tileBuffer = 2;
    u8 *xPos = AllocZeroed((botX - topX) * (botY - topY) * sizeof(u8));
    u8 *yPos = AllocZeroed((botX - topX) * (botY - topY) * sizeof(u8));
    u32 iter = 0;
    bool32 ret = FALSE;

    // loop through every tile in area and evaluate
    while (topY < botY)
    {
        while (topX < botX)
        {
            tileBehaviour = MapGridGetMetatileBehaviorAt(topX, topY);
            //Check for objects
            nextIter = FALSE;
            if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_BIKE))
                tileBuffer = SNEAKING_PROXIMITY + 3;
            else if (TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_DASH))
                tileBuffer = SNEAKING_PROXIMITY + 1;

            if (GetPlayerDistance(topX, topY) <= tileBuffer)
            {
                // tile too close to player
                topX++;
                continue;
            }

            for (i = 0; i < OBJECT_EVENTS_COUNT; i++)
            {
                if (gObjectEvents[i].currentCoords.x == topX && gObjectEvents[i].currentCoords.y == topY)
                {
                    // cannot be on a tile where an object exists
                    nextIter = TRUE;
                    break;
                }
            }

            if (nextIter)
            {
                topX++;
                continue;
            }

            weight = 0; // initiliaze weight
            switch (environment)
            {
            case ENCOUNTER_TYPE_LAND:
                if (MetatileBehavior_IsLandWildEncounter(tileBehaviour))
                {
                    if (currMapType == MAP_TYPE_UNDERGROUND)
                    {
                        // inside (cave)
                        if (IsElevationMismatchAt(gObjectEvents[gPlayerAvatar.objectEventId].currentElevation, topX, topY))
                            break; //occurs at same z coord

                        scale = 440 - (smallScan * 200) - (GetPlayerDistance(topX, topY) / 2)  - (2 * (topX + topY));
                        weight = ((Random() % scale) < 1) && !MapGridGetCollisionAt(topX, topY);
                    }
                    else
                    {
                        // outdoors: grass
                        scale = 100 - (GetPlayerDistance(topX, topY) * 2);
                        weight = (Random() % scale <= 5) && !MapGridGetCollisionAt(topX, topY);
                    }
                }
                break;
            case ENCOUNTER_TYPE_WATER:
                if (MetatileBehavior_IsSurfableWaterOrUnderwater(tileBehaviour))
                {
                    u8 scale = 320 - (smallScan * 200) - (GetPlayerDistance(topX, topY) / 2);
                    if (IsElevationMismatchAt(gObjectEvents[gPlayerAvatar.objectEventId].currentElevation, topX, topY))
                        break;

                    weight = (Random() % scale <= 1) && !MapGridGetCollisionAt(topX, topY);
                }
                break;
            default:
                break;
            }

            if (weight > 0)
            {
                xPos[iter] = topX;
                yPos[iter] = topY;
                iter++;
            }

            topX++;
        }

        topY++;
        topX = gSaveBlock1Ptr->pos.x - SCANSTART_X + (smallScan * 5);
    }

    if (iter > 0)
    {
        i = Random() % iter;
        sDexNavSearchDataPtr->tileX = xPos[i];
        sDexNavSearchDataPtr->tileY = yPos[i];
        ret = TRUE;
    }

    Free(xPos);
    Free(yPos);

    return ret;
}


static bool8 TryStartHiddenMonFieldEffect(enum EncounterType environment, u8 xSize, u8 ySize, bool8 smallScan)
{
    enum MapType currMapType = GetCurrentMapType();
    u8 fldEffId = 0;
    bool32 placed;
    if (IsRivalDexNavTutorialActive())
    {
        sDexNavSearchDataPtr->tileX = RIVAL_TUTORIAL_TARGET_X + MAP_OFFSET;
        sDexNavSearchDataPtr->tileY = RIVAL_TUTORIAL_TARGET_Y + MAP_OFFSET;
        placed = MetatileBehavior_IsLandWildEncounter(MapGridGetMetatileBehaviorAt(sDexNavSearchDataPtr->tileX, sDexNavSearchDataPtr->tileY))
            && !MapGridGetCollisionAt(sDexNavSearchDataPtr->tileX, sDexNavSearchDataPtr->tileY);
    }
    else
        placed = DexNavPickTile(environment, xSize, ySize, smallScan);

    if (placed)
    {
        u8 metatileBehaviour = MapGridGetMetatileBehaviorAt(sDexNavSearchDataPtr->tileX, sDexNavSearchDataPtr->tileY);

        switch (environment)
        {
        case ENCOUNTER_TYPE_LAND:
            if (currMapType == MAP_TYPE_UNDERGROUND)
            {
                fldEffId = FLDEFF_CAVE_DUST;
            }
            else if (IsMapTypeIndoors(currMapType))
            {
                if (MetatileBehavior_IsTallGrass(metatileBehaviour)) //Grass in cave
                    fldEffId = FLDEFF_SHAKING_GRASS;
                else if (MetatileBehavior_IsLongGrass(metatileBehaviour)) //Really tall grass
                    fldEffId = FLDEFF_SHAKING_LONG_GRASS;
                else if (MetatileBehavior_IsSandOrDeepSand(metatileBehaviour))
                    fldEffId = FLDEFF_SAND_HOLE;
                else
                    fldEffId = FLDEFF_CAVE_DUST;
            }
            else //outdoor, underwater
            {
                if (MetatileBehavior_IsTallGrass(metatileBehaviour)) //Regular grass
                    fldEffId = FLDEFF_SHAKING_GRASS;
                else if (MetatileBehavior_IsLongGrass(metatileBehaviour)) //Really tall grass
                    fldEffId = FLDEFF_SHAKING_LONG_GRASS;
                else if (MetatileBehavior_IsSandOrDeepSand(metatileBehaviour)) //Desert Sand
                    fldEffId = FLDEFF_SAND_HOLE;
                else if (MetatileBehavior_IsMountain(metatileBehaviour)) //Rough Terrain
                    fldEffId = FLDEFF_CAVE_DUST;
                else
                    fldEffId = FLDEFF_BERRY_TREE_GROWTH_SPARKLE; //default
            }
            break;
        case ENCOUNTER_TYPE_WATER:
            fldEffId = FLDEFF_WATER_SURFACING;
            break;
        default:
            return FALSE;
        }

        if (fldEffId != 0)
        {
            gFieldEffectArguments[0] = sDexNavSearchDataPtr->tileX;
            gFieldEffectArguments[1] = sDexNavSearchDataPtr->tileY;
            gFieldEffectArguments[2] = 0xFF; // subpriority
            gFieldEffectArguments[3] = 2;   //priority
            sDexNavSearchDataPtr->fldEffSpriteId = FieldEffectStart(fldEffId);
            if (sDexNavSearchDataPtr->fldEffSpriteId == MAX_SPRITES)
                return FALSE;

            sDexNavSearchDataPtr->fldEffId = fldEffId;
            return TRUE;
        }
    }

    return FALSE;
}

static void LoadSearchIconData(void)
{
    // palettes clash with mon icon, so must load manually
    LoadSpriteSheet(&gSpriteSheet_HeldItem);
    LoadPalette(gHeldItemPalette, OBJ_PLTT_ID(sHeldItemOam.paletteNum), PLTT_SIZE_4BPP);
    LoadCompressedSpriteSheetUsingHeap(&sOwnedIconSpriteSheet);
}

// Chains affect only the two advertised encounter rolls. Species, moves,
// levels, items and abilities retain their normal wild-slot behavior.
u32 GetDexNavChain(void)
{
    return min(gSaveBlock3Ptr->dexNavChain, DEXNAV_CHAIN_MAX);
}

void ApplyDexNavChainRewards(struct Pokemon *mon)
{
    u32 chain = GetDexNavChain();
    bool32 shiny = chain ? RandomUniform(RNG_DEXNAV_SHINY, 0, 99) < chain
                        : RandomUniform(RNG_DEXNAV_SHINY, 0, MAX_u16) < SHINY_ODDS;
    bool32 pokerus = chain ? RandomUniform(RNG_DEXNAV_POKERUS, 0, 199) < chain
                          : RandomUniform(RNG_DEXNAV_POKERUS, 0, MAX_u16) < P_POKERUS_INFECTION_ODDS;
    SetMonData(mon, MON_DATA_IS_SHINY, &shiny);
    if (pokerus)
        GiveMonPokerus(mon, TRUE);
}

static void CreateDexNavSearchMon(void)
{
    enum Species species = sDexNavSearchDataPtr->species;
    CreateWildSlotMon(species, sDexNavSearchDataPtr->monLevel);
    ApplyDexNavChainRewards(&gParties[B_TRAINER_OPPONENT_A][0]);
    gDexNavSpecies = species;
    if (IsRivalDexNavTutorialActive())
        PrepareRivalTutorialCatch(&gParties[B_TRAINER_OPPONENT_A][0]);
}

static void SetUpDexNavSearch(void)
{

    // init sprites
    sDexNavSearchDataPtr->iconSpriteId = MAX_SPRITES;
    sDexNavSearchDataPtr->ownedIconSpriteId = MAX_SPRITES;

    DexNavProximityUpdate();

    LoadSearchIconData();
    DexNavDrawIcons();
    DexNavUpdateSearchWindow();

    gPlayerAvatar.creeping = TRUE;  //initialize as true in case mon appears beside you
    sDexNavSearchDataPtr->proximity = gSprites[gPlayerAvatar.spriteId].x;
    sDexNavSearchDataPtr->startingTime = gMain.vblankCounter1;
    IncrementGameStat(GAME_STAT_DEXNAV_SCANNED);
}

static void DexNavSearchBail(const u8 *script)
{
    TRY_FREE_AND_SET_NULL(sDexNavSearchDataPtr);
    FlagClear(DN_FLAG_SEARCHING);
    FreeMonIconPalettes();
    extern const u8 EC_RivalDexNavTutorial_Abort[];
    bool32 tutorial = IsRivalDexNavTutorialActive();
    if (tutorial)
        FinishRivalDexNavTutorial();
    ScriptContext_SetupScript(tutorial ? EC_RivalDexNavTutorial_Abort : script);
}

static bool8 InitDexNavSearch(enum Species species, u32 environment)
{
    sDexNavSearchDataPtr = AllocZeroed(sizeof(struct DexNavSearch));
    if (sDexNavSearchDataPtr == NULL)
    {
        DexNavSearchBail(EventScript_NotFoundNearby);
        return TRUE;
    }
    FlagSet(DN_FLAG_SEARCHING);

    // assign non-objects to struct
    sDexNavSearchDataPtr->species = species;
    sDexNavSearchDataPtr->environment = environment;
    sDexNavSearchDataPtr->monLevel = DexNavTryGenerateMonLevel(species, environment);

    if (GetFlashLevel() > 0)
    {
        DexNavSearchBail(EventScript_TooDark);
        return TRUE;
    }

    if (sDexNavSearchDataPtr->monLevel == MON_LEVEL_NONEXISTENT || !TryStartHiddenMonFieldEffect(sDexNavSearchDataPtr->environment, 12, 12, FALSE))
    {
        DexNavSearchBail(EventScript_NotFoundNearby);
        return TRUE;
    }

    SetUpDexNavSearch();
    if (IsRivalDexNavTutorialActive())
        RivalTutorialSearchStarted();
    return FALSE;
}

static void DexNavUpdateDirectionArrow(void)
{
    u16 tileX = sDexNavSearchDataPtr->tileX;
    u16 tileY = sDexNavSearchDataPtr->tileY;
    s16 playerX = gSaveBlock1Ptr->pos.x + MAP_OFFSET;
    s16 playerY = gSaveBlock1Ptr->pos.y + MAP_OFFSET;
    RivalTutorialGetSearchOrigin(&playerX, &playerY);
    u16 deltaX = abs(tileX - playerX);
    u16 deltaY = abs(tileY - playerY);
    const u8 *str;
    u8 windowId = sDexNavSearchDataPtr->windowId;

    FillWindowPixelRect(windowId, PIXEL_FILL(1), SEARCH_ARROW_X, SEARCH_ARROW_Y, 12, 12);
    if (deltaX <= 1 && deltaY <= 1)
    {
        str = gText_EmptyString2;
    }
    else if (deltaX > deltaY)
    {
        if (playerX > tileX)
            str = sText_ArrowLeft;  //player to right
        else
            str = sText_ArrowRight; //player to left
    }
    else //greater Y diff
    {
        if (playerY > tileY)
            str = sText_ArrowUp;    //player below
        else
            str = sText_ArrowDown;  //player above
    }

    AddTextPrinterParameterized3(windowId, FONT_NORMAL, SEARCH_ARROW_X, SEARCH_ARROW_Y, sSearchFontColor, TEXT_SKIP_DRAW, str);
    CopyWindowToVram(windowId, 2);
}

static void DexNavDrawIcons(void)
{
    enum Species species = sDexNavSearchDataPtr->species;

    DrawSearchWindow(species);
    DrawDexNavSearchMonIcon(species, &sDexNavSearchDataPtr->iconSpriteId, GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_GET_CAUGHT));
    DexNavUpdateDirectionArrow();
}

/////////////////////
//// SEARCH TASK ////
/////////////////////
// DexNav works where its start menu entry does: in hand, outside the Safari
// Zone and the Battle Pike and Pyramid, which keep their own encounter rules.
static bool32 IsDexNavUsableHere(void)
{
    return FlagGet(DN_FLAG_DEXNAV_GET)
        && !GetSafariZoneFlag()
        && !InBattlePike()
        && CurrentBattlePyramidLocation() == PYRAMID_LOCATION_NONE;
}

bool32 TryStartDexNavSearch(void)
{
    u16 val = VarGet(DN_VAR_SPECIES);

    if (!IsDexNavUsableHere())
        return FALSE;

    if (FlagGet(DN_FLAG_SEARCHING) || (val & DEXNAV_MASK_SPECIES) == SPECIES_NONE)
        return FALSE;

    HideMapNamePopUpWindow();
    ChangeBgY_ScreenOff(0, 0, 0);
    PlaySE(SE_DEX_SEARCH);
    return InitDexNavSearch(val & DEXNAV_MASK_SPECIES, val >> 14);
}

void EndDexNavSearch(void)
{
    if (!FlagGet(DN_FLAG_SEARCHING) || sDexNavSearchDataPtr == NULL)
        return;
    RemoveDexNavWindowAndGfx();
    FieldEffectStop(&gSprites[sDexNavSearchDataPtr->fldEffSpriteId], sDexNavSearchDataPtr->fldEffId);
    FREE_AND_SET_NULL(sDexNavSearchDataPtr);
    FlagClear(DN_FLAG_SEARCHING);
}

static void EndDexNavSearchSetupScript(const u8 *script)
{
    gSaveBlock3Ptr->dexNavChain = 0;   //reset chain
    EndDexNavSearch();
    extern const u8 EC_RivalDexNavTutorial_Abort[];
    bool32 tutorial = IsRivalDexNavTutorialActive();
    if (tutorial)
        FinishRivalDexNavTutorial();
    ScriptContext_SetupScript(tutorial ? EC_RivalDexNavTutorial_Abort : script);
}

bool32 OnStep_DexNavSearch(void)
{
    if (!FlagGet(DN_FLAG_SEARCHING) || sDexNavSearchDataPtr == NULL)
        return FALSE;

    u32 frameCount = gMain.vblankCounter1 - sDexNavSearchDataPtr->startingTime;
    DexNavProximityUpdate();
    DexNavUpdateSearchWindow();

    if (sDexNavSearchDataPtr->proximity > MAX_PROXIMITY)
    { // out of range
        EndDexNavSearchSetupScript(EventScript_LostSignal);
        return TRUE;
    }

    if (sDexNavSearchDataPtr->proximity <= CREEPING_PROXIMITY && !gPlayerAvatar.creeping && frameCount > 60)
    { //should be creeping but player walks normally
        EndDexNavSearchSetupScript(EventScript_MovedTooFast);
        return TRUE;
    }

    if (!IsRivalDexNavTutorialActive() && sDexNavSearchDataPtr->proximity <= SNEAKING_PROXIMITY && TestPlayerAvatarFlags(PLAYER_AVATAR_FLAG_DASH | PLAYER_AVATAR_FLAG_BIKE))
    { // running/biking too close
        EndDexNavSearchSetupScript(EventScript_MovedTooFast);
        return TRUE;
    }

    if (sDexNavSearchDataPtr->proximity < 1)
    {
        CreateDexNavSearchMon();
        extern const u8 EC_RivalDexNavTutorial_Capture[];
        ScriptContext_SetupScript(IsRivalDexNavTutorialActive() ? EC_RivalDexNavTutorial_Capture : EventScript_StartDexNavBattle);
        FREE_AND_SET_NULL(sDexNavSearchDataPtr);
        FlagClear(DN_FLAG_SEARCHING);
        return TRUE;
    }

    //Caves and water the Pokémon moves around
    if ((sDexNavSearchDataPtr->environment == ENCOUNTER_TYPE_WATER || GetCurrentMapType() == MAP_TYPE_UNDERGROUND)
        && sDexNavSearchDataPtr->proximity < 2 && sDexNavSearchDataPtr->movementCount < 2)
    {
        FieldEffectStop(&gSprites[sDexNavSearchDataPtr->fldEffSpriteId], sDexNavSearchDataPtr->fldEffId);

        if (!TryStartHiddenMonFieldEffect(sDexNavSearchDataPtr->environment, 10, 10, TRUE))
        {
            EndDexNavSearchSetupScript(EventScript_PokemonGotAway);
            return TRUE;
        }

        sDexNavSearchDataPtr->movementCount++;
    }
    return FALSE;
}

static void DexNavUpdateSearchWindow(void)
{
    FillWindowPixelBuffer(sDexNavSearchDataPtr->windowId, PIXEL_FILL(1));
    AddSearchWindowText(sDexNavSearchDataPtr->species);
    DexNavUpdateDirectionArrow();
}

static u8 DexNavTryGenerateMonLevel(enum Species species, enum EncounterType environment)
{
    return GetEncounterLevelFromMapData(species, environment);
}

// Searches draw from the map's live roster (src/wild_roster.c), so they find
// only what the encounter engine could produce here right now, at its
// levels. Searchability separately excludes storm guests and special methods.
static u8 GetEncounterLevelFromMapData(enum Species species, enum EncounterType environment)
{
    struct WildRosterEntry roster[WILD_ROSTER_MAX_ENTRIES];
    u32 count = GetWildRosterForCurrentMap(roster, ARRAY_COUNT(roster));
    enum WildRosterMethod method;

    switch (environment)
    {
    case ENCOUNTER_TYPE_LAND:
        method = WILD_ROSTER_LAND;
        break;
    case ENCOUNTER_TYPE_WATER:
        method = WILD_ROSTER_SURFING;
        break;
    default:
        return MON_LEVEL_NONEXISTENT;
    }

    for (u32 i = 0; i < count && i < ARRAY_COUNT(roster); i++)
    {
        if (roster[i].species == species && roster[i].method == method && IsDexNavSearchableEntry(&roster[i]))
            return RandomUniform(RNG_DEXNAV_ENCOUNTER_LEVEL, roster[i].minLevel, roster[i].maxLevel);
    }
    return MON_LEVEL_NONEXISTENT;
}


///////////
/// GUI ///
///////////
// The screen lists the wild roster of the map the player stands on
// (src/wild_roster.c): every Pokémon that can appear here right now, by
// every method, most common first, each with its real icon. Each method is a
// section of icon rows; the sections scroll, and the panel on the right
// describes the highlighted Pokémon.

enum DexNavWindows
{
    WIN_TITLE,
    WIN_LIST,
    WIN_INFO,
};

#define DEXNAV_TEXT_PAL     15

// panel.pal
#define COLOR_WHITE         1
#define COLOR_TEXT          2
#define COLOR_SHADOW        3

enum DexNavTheme
{
    THEME_GRASS,
    THEME_WATER,
    THEME_EARTH,
    THEME_HONEY,
};

// Band color, then the darker line under the band and around the box.
static const u8 sThemeColors[][2] =
{
    [THEME_GRASS] = {4, 5},
    [THEME_WATER] = {6, 7},
    [THEME_EARTH] = {10, 11},
    [THEME_HONEY] = {12, 13},
};

// The list and info windows sit under the title bar. Their layout is
// window-relative, with room above and below for the scroll arrows. Every
// icon owns a whole 32-pixel cell, the size of an icon frame, so no two
// icons (or an icon and a band) can ever touch: five to a row.
#define LIST_WINDOW_Y       16
#define LIST_TOP            8
#define LIST_BOTTOM         136
#define LIST_BOX_X          2
#define LIST_BOX_WIDTH      164
#define LIST_HEADER_HEIGHT  16
#define LIST_ROW_HEIGHT     32
#define LIST_BOX_BOTTOM     3   // margin and border under a section's last row
#define LIST_SECTION_GAP    4
#define LIST_ICONS_PER_ROW  5
#define LIST_ICON_PITCH     32
#define LIST_ICON_X         (LIST_BOX_X + 2 + LIST_ICON_PITCH / 2)   // first icon's center
#define LIST_ICON_Y         (LIST_ROW_HEIGHT / 2)                    // icon center below its row's top
#define LIST_MAX_ROWS       (WILD_ROSTER_MAX_ENTRIES / LIST_ICONS_PER_ROW + WILD_ROSTER_METHOD_COUNT)
#define LIST_ARROW_X        (LIST_BOX_X + LIST_BOX_WIDTH / 2)
#define LIST_ARROW_UP_Y     (LIST_WINDOW_Y + 4)
#define LIST_ARROW_DOWN_Y   (LIST_WINDOW_Y + LIST_BOTTOM + 4)

#define INFO_WINDOW_X       168
#define INFO_BOX_X          2
#define INFO_BOX_WIDTH      68
#define INFO_TOP            LIST_TOP
#define INFO_BOTTOM         LIST_BOTTOM
#define INFO_TEXT_X         (INFO_BOX_X + 4)
#define INFO_TEXT_WIDTH     (INFO_BOX_WIDTH - 8)
#define INFO_NAME_WIDTH     (INFO_BOX_WIDTH - 18)   // leaves room for the caught mark
#define INFO_TYPES_Y        (INFO_TOP + LIST_HEADER_HEIGHT + 3)
#define INFO_RARITY_Y       (INFO_TYPES_Y + 11)
#define INFO_LEVEL_Y        (INFO_RARITY_Y + 12)
#define INFO_SHINY_Y        (INFO_LEVEL_Y + 14)
#define INFO_POKERUS_Y      (INFO_SHINY_Y + 20)
#define INFO_HINT_Y         (INFO_BOTTOM - 28)
#define INFO_RULE_Y         (INFO_HINT_Y - 4)

#define DEXNAV_ARROWS_TAG   0x4012

struct DexNavRow
{
    u8 first;   // roster index of the row's first Pokémon
    u8 count;
    bool8 startsSection;
    bool8 endsSection;
};

struct DexNavGUI
{
    MainCallback savedCallback;
    u8 state;
    u8 cursorSpriteId;
    u8 typeIconSpriteIds[2];
    u8 caughtSpriteId;
    u8 arrowTaskId;
    u8 rosterCount;
    u8 rowCount;
    u8 cursorRow;
    u8 cursorCol;
    u8 topRow;
    u8 lastTopRow;
    u16 scrollOffset;   // topRow, for the scroll arrows
    s16 rowY[LIST_MAX_ROWS];   // window y of each row on screen, or -1
    u8 iconSpriteIds[WILD_ROSTER_MAX_ENTRIES];
    u8 markSpriteIds[WILD_ROSTER_MAX_ENTRIES];
    struct DexNavRow rows[LIST_MAX_ROWS];
    struct WildRosterEntry roster[WILD_ROSTER_MAX_ENTRIES];
};

EWRAM_DATA static struct DexNavGUI *sDexNavUiDataPtr = NULL;
EWRAM_DATA static u8 *sBg1TilemapBuffer = NULL;
// Reopening the DexNav on the same map puts the cursor back on the last
// Pokémon, when it is still there.
EWRAM_DATA static u16 sDexNavCursorMap = 0; // map group/num + 1; 0 = none yet
EWRAM_DATA static u16 sDexNavCursorSpecies = SPECIES_NONE;
EWRAM_DATA static u8 sDexNavCursorMethod = 0;

static const u16 sDexNavPanelPal[] = INCGFX_U16("graphics/dexnav/panel.pal", ".gbapal");

static const u8 sText_DexNavRegistered[] = _("{R_BUTTON} {STR_VAR_1}");
static const u8 sText_DexNavNoWildPokemon[] = _("No wild Pokémon live here.");
static const u8 sText_DexNavLevel[] = _("Lv. {STR_VAR_1}");
static const u8 sText_DexNavLevelRange[] = _("Lv. {STR_VAR_1}-{STR_VAR_2}");
static const u8 sText_DexNavHintSearch[] = _("{A_BUTTON} Search\n{R_BUTTON} Register");
static const u8 sText_DexNavStormGuest[] = _("Storm guest.\nNo search.");
static const u8 sText_DexNavHowTallGrass[] = _("Walk in the\ntall grass.");
static const u8 sText_DexNavHowCave[] = _("Walk around\nthis cave.");
static const u8 sText_DexNavHowSeaweed[] = _("Swim through\nthe seaweed.");
static const u8 sText_DexNavHowWalking[] = _("Walk around\nhere.");
static const u8 sText_DexNavHowSurfing[] = _("Surf on the\nwater here.");
static const u8 sText_DexNavHowOldRod[] = _("Fish with\nan Old Rod.");
static const u8 sText_DexNavHowGoodRod[] = _("Fish with\na Good Rod.");
static const u8 sText_DexNavHowSuperRod[] = _("Fish with\na Super Rod.");
static const u8 sText_DexNavHowRockSmash[] = _("Smash rocks\nhere.");
static const u8 sText_DexNavHowHoney[] = _("Spread Honey\nhere.");
static const u8 sText_DexNavHowCutTrees[] = _("Cut down\ntrees here.");
static const u8 sText_DexNavHowHiddenSpots[] = _("Bites at only\nsix spots.");

static const u8 sTitleTextColors[3] = {TEXT_COLOR_TRANSPARENT, COLOR_WHITE, COLOR_TEXT};
static const u8 sBodyTextColors[3] = {TEXT_COLOR_TRANSPARENT, COLOR_TEXT, COLOR_SHADOW};

static const struct WindowTemplate sDexNavWindowTemplates[] =
{
    [WIN_TITLE] =
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 0,
        .width = 30,
        .height = 2,
        .paletteNum = DEXNAV_TEXT_PAL,
        .baseBlock = 1,
    },
    [WIN_LIST] =
    {
        .bg = 0,
        .tilemapLeft = 0,
        .tilemapTop = 2,
        .width = 21,
        .height = 18,
        .paletteNum = DEXNAV_TEXT_PAL,
        .baseBlock = 1 + 30 * 2,
    },
    [WIN_INFO] =
    {
        .bg = 0,
        .tilemapLeft = 21,
        .tilemapTop = 2,
        .width = 9,
        .height = 18,
        .paletteNum = DEXNAV_TEXT_PAL,
        .baseBlock = 1 + 30 * 2 + 21 * 18,
    },
    DUMMY_WIN_TEMPLATE
};

static const struct OamData sCaughtMarkOam =
{
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .shape = SPRITE_SHAPE(8x8),
    .size = SPRITE_SIZE(8x8),
    .priority = 0,
    .paletteNum = 12,   // the selection cursor's palette
};

static const struct SpriteTemplate sCaughtMarkTemplate =
{
    .tileTag = CAUGHT_MARK_TAG,
    .paletteTag = TAG_NONE,
    .oam = &sCaughtMarkOam,
    .anims = gDummySpriteAnimTable,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy,
};

static const struct CompressedSpriteSheet sCaughtMarkSpriteSheet = {sCaughtMarkGfx, (8 * 8) / 2, CAUGHT_MARK_TAG};

static const struct BgTemplate sDexNavMenuBgTemplates[2] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .priority = 0
    },
    {
        .bg = 1,
        .charBaseIndex = 3,
        .mapBaseIndex = 30,
        .priority = 1
    }
};

// gui_tiles.png
enum
{
    GUI_TILE_BAR,
    GUI_TILE_BAR_EDGE,
    GUI_TILE_STRIPES,
    GUI_TILE_STRIPES_EDGE,
};

static void DexNav_VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void DexNav_MainCB(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static bool8 DexNav_InitBgs(void)
{
    ResetVramOamAndBgCntRegs();
    ResetAllBgsCoordinates();
    sBg1TilemapBuffer = Alloc(0x800);
    if (sBg1TilemapBuffer == NULL)
        return FALSE;

    memset(sBg1TilemapBuffer, 0, 0x800);
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sDexNavMenuBgTemplates, NELEMS(sDexNavMenuBgTemplates));
    SetBgTilemapBuffer(1, sBg1TilemapBuffer);
    ScheduleBgCopyTilemapToVram(1);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_1D_MAP | DISPCNT_OBJ_ON);
    SetGpuReg(REG_OFFSET_BLDCNT , 0);
    ShowBg(0);
    ShowBg(1);
    return TRUE;
}

// The DexNav's striped backdrop under a black title bar.
static void DrawDexNavBackground(void)
{
    FillBgTilemapBufferRect_Palette0(1, GUI_TILE_BAR, 0, 0, 32, 1);
    FillBgTilemapBufferRect_Palette0(1, GUI_TILE_BAR_EDGE, 0, 1, 32, 1);
    FillBgTilemapBufferRect_Palette0(1, GUI_TILE_STRIPES, 0, 2, 32, 17);
    FillBgTilemapBufferRect_Palette0(1, GUI_TILE_STRIPES_EDGE, 0, 19, 32, 1);
    ScheduleBgCopyTilemapToVram(1);
}

static bool8 DexNav_LoadGraphics(void)
{
    switch (sDexNavUiDataPtr->state)
    {
    case 0:
        ResetTempTileDataBuffers();
        DecompressAndCopyTileDataToVram(1, sDexNavGuiTiles, 0, 0, 0);
        sDexNavUiDataPtr->state++;
        break;
    case 1:
        if (FreeTempTileDataBuffersIfPossible() != TRUE)
        {
            DrawDexNavBackground();
            sDexNavUiDataPtr->state++;
        }
        break;
    case 2:
        LoadPalette(sDexNavGuiPal, BG_PLTT_ID(0), PLTT_SIZE_4BPP);
        LoadPalette(sDexNavPanelPal, BG_PLTT_ID(DEXNAV_TEXT_PAL), PLTT_SIZE_4BPP);
        sDexNavUiDataPtr->state++;
        break;
    default:
        sDexNavUiDataPtr->state = 0;
        return TRUE;
    }

    return FALSE;
}

static u16 CurrentDexNavCursorMap(void)
{
    return ((gSaveBlock1Ptr->location.mapGroup << 8) | gSaveBlock1Ptr->location.mapNum) + 1;
}

// A roamer is met only as itself, with the HP and status it carries.
static bool32 IsRoamingHere(enum Species species)
{
    for (u32 i = 0; i < ROAMER_COUNT; i++)
    {
        if (IsRoamerAt(i, gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum)
         && gSaveBlock1Ptr->roamer[i].species == species)
            return TRUE;
    }
    return FALSE;
}

// A storm guest stays visible in the roster but must be met through the
// storm's ordinary encounter roll. Once it becomes a resident after the
// storm window, it follows the same search rules as other residents.
static bool32 IsStormVisitorHere(enum Species species)
{
    enum LegendarySignId sign = GetLiveWeatherAnomalyOnMap(gSaveBlock1Ptr->location.mapGroup,
                                                          gSaveBlock1Ptr->location.mapNum);
    return sign < LEGENDARY_SIGN_COUNT && gLegendaryGates[sign].species == species;
}

// The search engine hides its Pokémon in tall grass, on cave floors or in
// water the player can Surf (DexNavPickTile); seaweed is neither. Whatever
// it finds is made as its wild slot would make it (CreateDexNavSearchMon).
static bool32 IsDexNavSearchableEntry(const struct WildRosterEntry *entry)
{
    if (IsRoamingHere(entry->species) || IsStormVisitorHere(entry->species))
        return FALSE;
    if (entry->method == WILD_ROSTER_SURFING)
        return TRUE;
    return entry->method == WILD_ROSTER_LAND && GetCurrentMapType() != MAP_TYPE_UNDERWATER;
}

// The Safari Zone keeps its own encounter rules: its DexNav only shows.
static bool32 CanDexNavSearchFor(const struct WildRosterEntry *entry)
{
    return IsDexNavSearchableEntry(entry) && IsDexNavUsableHere();
}

static enum EncounterType GetDexNavEntryEnvironment(const struct WildRosterEntry *entry)
{
    return entry->method == WILD_ROSTER_SURFING ? ENCOUNTER_TYPE_WATER : ENCOUNTER_TYPE_LAND;
}

static enum DexNavTheme GetDexNavMethodTheme(enum WildRosterMethod method)
{
    switch (method)
    {
    case WILD_ROSTER_LAND:
        switch (GetCurrentMapType())
        {
        case MAP_TYPE_UNDERGROUND:
        case MAP_TYPE_INDOOR:
            return THEME_EARTH;
        default:
            return THEME_GRASS;
        }
    case WILD_ROSTER_ROCK_SMASH:
        return THEME_EARTH;
    case WILD_ROSTER_HONEY:
        return THEME_HONEY;
    case WILD_ROSTER_CUT_TREES:
        return THEME_GRASS;
    default:
        return THEME_WATER;
    }
}

static const u8 *GetDexNavMethodName(enum WildRosterMethod method)
{
    return GetWildRosterMethodName(method, gSaveBlock1Ptr->location.mapGroup, gSaveBlock1Ptr->location.mapNum);
}

// What the panel says under a Pokémon: the buttons when the DexNav can
// search for it, otherwise how to find it here.
static const u8 *GetDexNavEntryHint(const struct WildRosterEntry *entry)
{
    if (IsStormVisitorHere(entry->species))
        return sText_DexNavStormGuest;
    if (CanDexNavSearchFor(entry))
        return sText_DexNavHintSearch;

    switch (entry->method)
    {
    case WILD_ROSTER_LAND:
        switch (GetCurrentMapType())
        {
        case MAP_TYPE_UNDERGROUND: return sText_DexNavHowCave;
        case MAP_TYPE_UNDERWATER:  return sText_DexNavHowSeaweed;
        case MAP_TYPE_INDOOR:      return sText_DexNavHowWalking;
        default:                   return sText_DexNavHowTallGrass;
        }
    case WILD_ROSTER_SURFING:    return sText_DexNavHowSurfing;
    case WILD_ROSTER_OLD_ROD:    return sText_DexNavHowOldRod;
    case WILD_ROSTER_GOOD_ROD:   return sText_DexNavHowGoodRod;
    case WILD_ROSTER_SUPER_ROD:  return sText_DexNavHowSuperRod;
    case WILD_ROSTER_ROCK_SMASH: return sText_DexNavHowRockSmash;
    case WILD_ROSTER_HONEY:      return sText_DexNavHowHoney;
    case WILD_ROSTER_CUT_TREES:  return sText_DexNavHowCutTrees;
    default:                     return sText_DexNavHowHiddenSpots;
    }
}

// Reads the roster and splits each method into rows of six.
static void DexNavLoadRoster(void)
{
    struct DexNavGUI *ui = sDexNavUiDataPtr;
    u32 i = 0, count = GetWildRosterForCurrentMap(ui->roster, WILD_ROSTER_MAX_ENTRIES);

    ui->rosterCount = min(count, WILD_ROSTER_MAX_ENTRIES);
    ui->rowCount = 0;
    while (i < ui->rosterCount && ui->rowCount < LIST_MAX_ROWS)
    {
        struct DexNavRow *row = &ui->rows[ui->rowCount++];
        u8 method = ui->roster[i].method;

        row->first = i;
        row->count = 0;
        row->startsSection = (i == 0 || ui->roster[i - 1].method != method);
        while (i < ui->rosterCount && row->count < LIST_ICONS_PER_ROW && ui->roster[i].method == method)
        {
            row->count++;
            i++;
        }
        row->endsSection = (i == ui->rosterCount || ui->roster[i].method != method);
    }
}

// Places rows from topRow down. A section's band sits over its first row,
// and the top row always shows its own section's band. Returns the last row
// that fits.
static u32 LayoutDexNavRows(u32 topRow, s16 *rowY)
{
    struct DexNavGUI *ui = sDexNavUiDataPtr;
    u32 y = LIST_TOP, last = topRow;

    for (u32 row = 0; row < LIST_MAX_ROWS; row++)
        rowY[row] = -1;
    for (u32 row = topRow; row < ui->rowCount; row++)
    {
        u32 top = y;

        if (row != topRow && ui->rows[row].startsSection)
            top += LIST_SECTION_GAP;
        if (row == topRow || ui->rows[row].startsSection)
            top += LIST_HEADER_HEIGHT;
        if (top + LIST_ROW_HEIGHT + LIST_BOX_BOTTOM > LIST_BOTTOM)
            break;
        rowY[row] = top;
        y = top + LIST_ROW_HEIGHT;
        if (ui->rows[row].endsSection)
            y += LIST_BOX_BOTTOM;
        last = row;
    }
    return last;
}

static void DrawDexNavBox(u32 windowId, enum DexNavTheme theme, u32 x, u32 top, u32 width, u32 bottom, bool32 band)
{
    const u8 *colors = sThemeColors[theme];
    u32 bodyTop = band ? top + LIST_HEADER_HEIGHT : top;

    if (band)
    {
        FillWindowPixelRect(windowId, PIXEL_FILL(colors[0]), x, top, width, LIST_HEADER_HEIGHT - 1);
        FillWindowPixelRect(windowId, PIXEL_FILL(colors[1]), x, top + LIST_HEADER_HEIGHT - 1, width, 1);
    }
    FillWindowPixelRect(windowId, PIXEL_FILL(colors[1]), x, bodyTop, width, bottom - bodyTop);
    FillWindowPixelRect(windowId, PIXEL_FILL(COLOR_WHITE), x + 1, bodyTop + (band ? 0 : 1), width - 2, bottom - bodyTop - (band ? 1 : 2));
    // Round the corners.
    FillWindowPixelRect(windowId, PIXEL_FILL(0), x, top, 1, 1);
    FillWindowPixelRect(windowId, PIXEL_FILL(0), x + width - 1, top, 1, 1);
    FillWindowPixelRect(windowId, PIXEL_FILL(0), x, bottom - 1, 1, 1);
    FillWindowPixelRect(windowId, PIXEL_FILL(0), x + width - 1, bottom - 1, 1, 1);
}

static void PrintDexNavBandText(u32 windowId, enum DexNavTheme theme, u32 fontId, u32 x, u32 y, const u8 *str)
{
    u8 colors[3] = {TEXT_COLOR_TRANSPARENT, COLOR_WHITE, sThemeColors[theme][1]};
    AddTextPrinterParameterized3(windowId, fontId, x, y, colors, TEXT_SKIP_DRAW, str);
}

static void DrawDexNavList(void)
{
    struct DexNavGUI *ui = sDexNavUiDataPtr;
    u32 last = LayoutDexNavRows(ui->topRow, ui->rowY);

    FillWindowPixelBuffer(WIN_LIST, PIXEL_FILL(0));
    for (u32 row = ui->topRow; row <= last;)
    {
        u32 end = row;
        u8 method = ui->roster[ui->rows[row].first].method;
        enum DexNavTheme theme = GetDexNavMethodTheme(method);
        u32 top = ui->rowY[row] - LIST_HEADER_HEIGHT;

        while (end < last && !ui->rows[end + 1].startsSection)
            end++;
        DrawDexNavBox(WIN_LIST, theme, LIST_BOX_X, top, LIST_BOX_WIDTH, ui->rowY[end] + LIST_ROW_HEIGHT + LIST_BOX_BOTTOM, TRUE);
        PrintDexNavBandText(WIN_LIST, theme, FONT_NORMAL, LIST_BOX_X + 6, top, GetDexNavMethodName(method));
        row = end + 1;
    }
    PutWindowTilemap(WIN_LIST);
    CopyWindowToVram(WIN_LIST, COPYWIN_FULL);
}

static void DrawDexNavEmptyList(void)
{
    u32 top = (LIST_TOP + LIST_BOTTOM) / 2 - 16;

    FillWindowPixelBuffer(WIN_LIST, PIXEL_FILL(0));
    DrawDexNavBox(WIN_LIST, THEME_WATER, LIST_BOX_X, top, LIST_BOX_WIDTH, top + 32, FALSE);
    AddTextPrinterParameterized3(WIN_LIST, FONT_NORMAL,
                                 LIST_BOX_X + (LIST_BOX_WIDTH - GetStringWidth(FONT_NORMAL, sText_DexNavNoWildPokemon, 0)) / 2,
                                 top + 8, sBodyTextColors, TEXT_SKIP_DRAW, sText_DexNavNoWildPokemon);
    PutWindowTilemap(WIN_LIST);
    CopyWindowToVram(WIN_LIST, COPYWIN_FULL);
}

static bool32 IsSpeciesCaught(enum Species species)
{
    return GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_GET_CAUGHT);
}

static void DestroyDexNavListSprites(void)
{
    struct DexNavGUI *ui = sDexNavUiDataPtr;

    for (u32 i = 0; i < WILD_ROSTER_MAX_ENTRIES; i++)
    {
        if (ui->iconSpriteIds[i] != MAX_SPRITES)
            FreeAndDestroyMonIconSprite(&gSprites[ui->iconSpriteIds[i]]);
        if (ui->markSpriteIds[i] != MAX_SPRITES)
            DestroySprite(&gSprites[ui->markSpriteIds[i]]);
        ui->iconSpriteIds[i] = MAX_SPRITES;
        ui->markSpriteIds[i] = MAX_SPRITES;
    }
}

static void CreateDexNavListSprites(void)
{
    struct DexNavGUI *ui = sDexNavUiDataPtr;

    for (u32 row = ui->topRow; row < ui->rowCount && ui->rowY[row] >= 0; row++)
    {
        for (u32 col = 0; col < ui->rows[row].count; col++)
        {
            u32 i = ui->rows[row].first + col;
            s16 x = LIST_ICON_X + col * LIST_ICON_PITCH;
            s16 y = LIST_WINDOW_Y + ui->rowY[row] + LIST_ICON_Y;
            u8 spriteId = CreateMonIcon(ui->roster[i].species, SpriteCallbackDummy, x, y, 1, 0xFFFFFFFF);

            if (spriteId == MAX_SPRITES)
                continue;
            gSprites[spriteId].oam.priority = 0;
            ui->iconSpriteIds[i] = spriteId;
            if (IsSpeciesCaught(ui->roster[i].species))
                ui->markSpriteIds[i] = CreateSprite(&sCaughtMarkTemplate, x + LIST_ICON_PITCH / 2 - 5, y + LIST_ROW_HEIGHT / 2 - 5, 0);
        }
    }
}

static const struct WildRosterEntry *GetSelectedDexNavEntry(void)
{
    struct DexNavGUI *ui = sDexNavUiDataPtr;

    if (ui->rowCount == 0)
        return NULL;
    return &ui->roster[ui->rows[ui->cursorRow].first + ui->cursorCol];
}

// Keeps the highlighted row on screen; TRUE when the list moved.
static bool32 ScrollDexNavToCursor(void)
{
    struct DexNavGUI *ui = sDexNavUiDataPtr;
    s16 rowY[LIST_MAX_ROWS];
    u32 oldTop = ui->topRow;

    if (ui->cursorRow < ui->topRow)
        ui->topRow = ui->cursorRow;
    while (ui->topRow < ui->cursorRow && LayoutDexNavRows(ui->topRow, rowY) < ui->cursorRow)
        ui->topRow++;
    ui->scrollOffset = ui->topRow;
    return ui->topRow != oldTop;
}

static void SetTypeIconPosAndPal(u8 typeId, u8 x, u8 y, u8 spriteArrayId);

static const u8 sText_ShinyChance[] = _("Shiny");
static const u8 sText_PokerusChance[] = _("Pokérus");
static const u8 sText_ChancePercent[] = _("{STR_VAR_1}%");
static const u8 sText_ChanceHalfPercent[] = _("{STR_VAR_1}.5%");
static const u8 sText_ChanceFraction[] = _("{STR_VAR_2}/{STR_VAR_1}");
static const u8 sText_NoSearchOdds[] = _("--");

static void PrintSearchChance(bool32 pokerus, bool32 searchable)
{
    u32 chain = GetDexNavChain();
    u32 y = pokerus ? INFO_POKERUS_Y : INFO_SHINY_Y;
    const u8 *label = pokerus ? sText_PokerusChance : sText_ShinyChance;
    AddTextPrinterParameterized3(WIN_INFO, FONT_SMALL_NARROW, INFO_TEXT_X, y, sBodyTextColors, TEXT_SKIP_DRAW, label);
    if (!searchable)
        StringCopy(gStringVar4, sText_NoSearchOdds);
    else if (!chain)
    {
        ConvertIntToDecimalStringN(gStringVar1, pokerus ? 65536 : 65536 / SHINY_ODDS, STR_CONV_MODE_LEFT_ALIGN, 5);
        ConvertIntToDecimalStringN(gStringVar2, pokerus ? P_POKERUS_INFECTION_ODDS : 1, STR_CONV_MODE_LEFT_ALIGN, 2);
        StringExpandPlaceholders(gStringVar4, sText_ChanceFraction);
    }
    else
    {
        ConvertIntToDecimalStringN(gStringVar1, pokerus ? chain / 2 : chain, STR_CONV_MODE_LEFT_ALIGN, 3);
        StringExpandPlaceholders(gStringVar4, pokerus && (chain & 1) ? sText_ChanceHalfPercent : sText_ChancePercent);
    }
    AddTextPrinterParameterized3(WIN_INFO, FONT_SMALL_NARROW, INFO_TEXT_X, y + 10, sBodyTextColors, TEXT_SKIP_DRAW, gStringVar4);
}

static void PrintDexNavInfo(void)
{
    struct DexNavGUI *ui = sDexNavUiDataPtr;
    const struct WildRosterEntry *entry = GetSelectedDexNavEntry();
    enum DexNavTheme theme;
    enum Type type1, type2;
    const u8 *name;

    FillWindowPixelBuffer(WIN_INFO, PIXEL_FILL(0));
    gSprites[ui->typeIconSpriteIds[0]].invisible = TRUE;
    gSprites[ui->typeIconSpriteIds[1]].invisible = TRUE;
    gSprites[ui->caughtSpriteId].invisible = TRUE;
    if (entry == NULL)
    {
        PutWindowTilemap(WIN_INFO);
        CopyWindowToVram(WIN_INFO, COPYWIN_FULL);
        return;
    }

    theme = GetDexNavMethodTheme(entry->method);
    DrawDexNavBox(WIN_INFO, theme, INFO_BOX_X, INFO_TOP, INFO_BOX_WIDTH, INFO_BOTTOM, TRUE);

    name = GetSpeciesName(entry->species);
    PrintDexNavBandText(WIN_INFO, theme, GetFontIdToFit(name, FONT_NORMAL, 0, INFO_NAME_WIDTH), INFO_TEXT_X, INFO_TOP, name);
    gSprites[ui->caughtSpriteId].invisible = !IsSpeciesCaught(entry->species);

    type1 = GetSpeciesType(entry->species, 0);
    type2 = GetSpeciesType(entry->species, 1);
    SetTypeIconPosAndPal(type1, INFO_WINDOW_X + INFO_BOX_X + 2, LIST_WINDOW_Y + INFO_TYPES_Y, 0);
    if (type2 != type1)
        SetTypeIconPosAndPal(type2, INFO_WINDOW_X + INFO_BOX_X + 2 + 32, LIST_WINDOW_Y + INFO_TYPES_Y, 1);

    AddTextPrinterParameterized3(WIN_INFO, FONT_SMALL, INFO_TEXT_X, INFO_RARITY_Y, sBodyTextColors, TEXT_SKIP_DRAW,
                                 GetWildRosterRarityName(entry->rarity));
    ConvertIntToDecimalStringN(gStringVar1, entry->minLevel, STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertIntToDecimalStringN(gStringVar2, entry->maxLevel, STR_CONV_MODE_LEFT_ALIGN, 3);
    StringExpandPlaceholders(gStringVar4, entry->minLevel == entry->maxLevel ? sText_DexNavLevel : sText_DexNavLevelRange);
    AddTextPrinterParameterized3(WIN_INFO, FONT_SMALL, INFO_TEXT_X, INFO_LEVEL_Y, sBodyTextColors, TEXT_SKIP_DRAW, gStringVar4);

    PrintSearchChance(FALSE, CanDexNavSearchFor(entry));
    PrintSearchChance(TRUE, CanDexNavSearchFor(entry));

    FillWindowPixelRect(WIN_INFO, PIXEL_FILL(COLOR_SHADOW), INFO_TEXT_X, INFO_RULE_Y, INFO_TEXT_WIDTH, 1);
    AddTextPrinterParameterized3(WIN_INFO, FONT_SMALL, INFO_TEXT_X, INFO_HINT_Y, sBodyTextColors, TEXT_SKIP_DRAW,
                                 GetDexNavEntryHint(entry));

    PutWindowTilemap(WIN_INFO);
    CopyWindowToVram(WIN_INFO, COPYWIN_FULL);
}

// Redraws what the cursor changed: the list when it scrolled, the moving
// icon (only the highlighted Pokémon animates), the cursor and the panel.
static void UpdateDexNavSelection(bool32 redrawList)
{
    struct DexNavGUI *ui = sDexNavUiDataPtr;
    const struct WildRosterEntry *entry = GetSelectedDexNavEntry();
    u32 selected = ui->rows[ui->cursorRow].first + ui->cursorCol;

    if (ScrollDexNavToCursor() || redrawList)
    {
        DestroyDexNavListSprites();
        DrawDexNavList();
        CreateDexNavListSprites();
    }

    for (u32 i = 0; i < ui->rosterCount; i++)
    {
        struct Sprite *icon;

        if (ui->iconSpriteIds[i] == MAX_SPRITES)
            continue;
        icon = &gSprites[ui->iconSpriteIds[i]];
        if (i == selected)
        {
            icon->callback = SpriteCB_MonIcon;
        }
        else if (icon->callback != SpriteCallbackDummy)
        {
            icon->callback = SpriteCallbackDummy;
            icon->animCmdIndex = 0;
            icon->animDelayCounter = 0;
            UpdateMonIconFrame(icon);
        }
    }

    gSprites[ui->cursorSpriteId].x = LIST_ICON_X + ui->cursorCol * LIST_ICON_PITCH;
    gSprites[ui->cursorSpriteId].y = LIST_WINDOW_Y + ui->rowY[ui->cursorRow] + LIST_ICON_Y;
    PrintDexNavInfo();

    sDexNavCursorMap = CurrentDexNavCursorMap();
    sDexNavCursorSpecies = entry->species;
    sDexNavCursorMethod = entry->method;
}

// Back on the Pokémon highlighted last time on this map, if it is still here.
static void RestoreDexNavCursor(void)
{
    struct DexNavGUI *ui = sDexNavUiDataPtr;

    ui->cursorRow = 0;
    ui->cursorCol = 0;
    if (!IsRivalDexNavTutorialActive() && sDexNavCursorMap != CurrentDexNavCursorMap())
        return;
    for (u32 row = 0; row < ui->rowCount; row++)
    {
        for (u32 col = 0; col < ui->rows[row].count; col++)
        {
            const struct WildRosterEntry *entry = &ui->roster[ui->rows[row].first + col];
            if (entry->species == (IsRivalDexNavTutorialActive() ? SPECIES_ZIGZAGOON : sDexNavCursorSpecies)
             && entry->method == (IsRivalDexNavTutorialActive() ? WILD_ROSTER_LAND : sDexNavCursorMethod))
            {
                ui->cursorRow = row;
                ui->cursorCol = col;
                return;
            }
        }
    }
}

static void CreateSelectionCursor(void)
{
    struct CompressedSpriteSheet spriteSheet;

    spriteSheet.data = sSelectionCursorGfx;
    spriteSheet.size = 0x200;
    spriteSheet.tag = SELECTION_CURSOR_TAG;
    LoadCompressedSpriteSheet(&spriteSheet);
    LoadPalette(sSelectionCursorPal, OBJ_PLTT_ID(sSelectionCursorOam.paletteNum), PLTT_SIZE_4BPP);
    sDexNavUiDataPtr->cursorSpriteId = CreateSprite(&sSelectionCursorSpriteTemplate, 0, 0, 0);
}

static void CreateDexNavScrollArrows(void)
{
    struct DexNavGUI *ui = sDexNavUiDataPtr;
    s16 rowY[LIST_MAX_ROWS];

    ui->lastTopRow = 0;
    while (ui->lastTopRow + 1u < ui->rowCount && LayoutDexNavRows(ui->lastTopRow, rowY) + 1u < ui->rowCount)
        ui->lastTopRow++;
    if (ui->lastTopRow != 0)
        ui->arrowTaskId = AddScrollIndicatorArrowPairParameterized(SCROLL_ARROW_UP, LIST_ARROW_X, LIST_ARROW_UP_Y, LIST_ARROW_DOWN_Y,
                                                                   ui->lastTopRow, DEXNAV_ARROWS_TAG, DEXNAV_ARROWS_TAG, &ui->scrollOffset);
}

static void DexNav_InitWindows(void)
{
    InitWindows(sDexNavWindowTemplates);
    DeactivateAllTextPrinters();
    ScheduleBgCopyTilemapToVram(0);
}

static void DexNavGuiFreeResources(void)
{
    if (sDexNavUiDataPtr->arrowTaskId != TASK_NONE)
        RemoveScrollIndicatorArrowPair(sDexNavUiDataPtr->arrowTaskId);
    FREE_AND_SET_NULL(sDexNavUiDataPtr);
    FREE_AND_SET_NULL(sBg1TilemapBuffer);
    FreeAllWindowBuffers();
}

static void CB1_InitDexNavSearch(void)
{
    if (!gPaletteFade.active && !ArePlayerFieldControlsLocked() && gMain.callback2 == CB2_Overworld)
    {
        SetMainCallback1(CB1_Overworld);
        InitDexNavSearch(gSpecialVar_0x8000, gSpecialVar_0x8001);
    }
}

static void CB1_DexNavSearchCallback(void)
{
    CB1_InitDexNavSearch();
}

static void Task_DexNavExitAndSearch(u8 taskId)
{
    DespawnAllOverworldWildEncounters(OWE_GENERATED, 0);
    DexNavGuiFreeResources();
    DestroyTask(taskId);
    SetMainCallback1(CB1_DexNavSearchCallback);
    SetMainCallback2(CB2_ReturnToField);
}

static void Task_DexNavFadeAndExit(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        SetMainCallback2(sDexNavUiDataPtr->savedCallback);
        DexNavGuiFreeResources();
        DestroyTask(taskId);
    }
}

static void DexNavFadeAndExit(void)
{
    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
    CreateTask(Task_DexNavFadeAndExit, 0);
    SetVBlankCallback(DexNav_VBlankCB);
    SetMainCallback2(DexNav_MainCB);
}

static void SetSpriteInvisibility(u8 spriteArrayId, bool8 invisible)
{
    gSprites[sDexNavUiDataPtr->typeIconSpriteIds[spriteArrayId]].invisible = invisible;
}

// different from pokemon_summary_screen
#define TYPE_ICON_PAL_NUM_0     13
#define TYPE_ICON_PAL_NUM_1     14
#define TYPE_ICON_PAL_NUM_2     15
static const u8 sMoveTypeToOamPaletteNum[NUMBER_OF_MON_TYPES] =
{
    [TYPE_NORMAL] = TYPE_ICON_PAL_NUM_0,
    [TYPE_FIGHTING] = TYPE_ICON_PAL_NUM_0,
    [TYPE_FLYING] = TYPE_ICON_PAL_NUM_1,
    [TYPE_POISON] = TYPE_ICON_PAL_NUM_1,
    [TYPE_GROUND] = TYPE_ICON_PAL_NUM_0,
    [TYPE_ROCK] = TYPE_ICON_PAL_NUM_0,
    [TYPE_BUG] = TYPE_ICON_PAL_NUM_2,
    [TYPE_GHOST] = TYPE_ICON_PAL_NUM_1,
    [TYPE_STEEL] = TYPE_ICON_PAL_NUM_0,
    [TYPE_MYSTERY] = TYPE_ICON_PAL_NUM_2,
    [TYPE_FIRE] = TYPE_ICON_PAL_NUM_0,
    [TYPE_WATER] = TYPE_ICON_PAL_NUM_1,
    [TYPE_GRASS] = TYPE_ICON_PAL_NUM_2,
    [TYPE_ELECTRIC] = TYPE_ICON_PAL_NUM_0,
    [TYPE_PSYCHIC] = TYPE_ICON_PAL_NUM_1,
    [TYPE_ICE] = TYPE_ICON_PAL_NUM_1,
    [TYPE_DRAGON] = TYPE_ICON_PAL_NUM_2,
    [TYPE_DARK] = TYPE_ICON_PAL_NUM_0,
    [TYPE_FAIRY] = TYPE_ICON_PAL_NUM_1,
};
// x and y are the icon's top left corner.
static void SetTypeIconPosAndPal(u8 typeId, u8 x, u8 y, u8 spriteArrayId)
{
    struct Sprite *sprite;

    sprite = &gSprites[sDexNavUiDataPtr->typeIconSpriteIds[spriteArrayId]];
    StartSpriteAnim(sprite, typeId);
    sprite->oam.paletteNum = sMoveTypeToOamPaletteNum[typeId];
    sprite->x = x + 16;
    sprite->y = y + 8;
    SetSpriteInvisibility(spriteArrayId, FALSE);
}

// The map on the left; on the right, the Pokémon R searches for in the field.
static void PrintDexNavTitle(void)
{
    enum Species registered = VarGet(DN_VAR_SPECIES) & DEXNAV_MASK_SPECIES;

    FillWindowPixelBuffer(WIN_TITLE, PIXEL_FILL(0));
    GetMapName(gStringVar3, GetCurrentRegionMapSectionId(), 0);
    AddTextPrinterParameterized3(WIN_TITLE, FONT_NORMAL, 4, 0, sTitleTextColors, TEXT_SKIP_DRAW, gStringVar3);
    if (registered != SPECIES_NONE && registered < NUM_SPECIES)
    {
        StringCopy(gStringVar1, GetSpeciesName(registered));
        StringExpandPlaceholders(gStringVar4, sText_DexNavRegistered);
        AddTextPrinterParameterized3(WIN_TITLE, FONT_NORMAL, DISPLAY_WIDTH - 4 - GetStringWidth(FONT_NORMAL, gStringVar4, 0), 0,
                                     sTitleTextColors, TEXT_SKIP_DRAW, gStringVar4);
    }
    PutWindowTilemap(WIN_TITLE);
    CopyWindowToVram(WIN_TITLE, COPYWIN_FULL);
}

static void CreateInfoSprites(void)
{
    struct DexNavGUI *ui = sDexNavUiDataPtr;

    LoadCompressedSpriteSheet(&gSpriteSheet_MoveTypes);
    LoadPalette(gMoveTypes_Pal, OBJ_PLTT_ID(13), 3 * PLTT_SIZE_4BPP);
    for (u32 i = 0; i < 2; i++)
    {
        ui->typeIconSpriteIds[i] = CreateSprite(&gSpriteTemplate_MoveTypes, 10, 10, 2);
        gSprites[ui->typeIconSpriteIds[i]].oam.priority = 0;
        SetSpriteInvisibility(i, TRUE);
    }
    LoadCompressedSpriteSheet(&sCaughtMarkSpriteSheet);
    ui->caughtSpriteId = CreateSprite(&sCaughtMarkTemplate, INFO_WINDOW_X + INFO_BOX_X + INFO_BOX_WIDTH - 8,
                                      LIST_WINDOW_Y + INFO_TOP + LIST_HEADER_HEIGHT / 2 - 1, 0);
    gSprites[ui->caughtSpriteId].invisible = TRUE;
}

static bool8 DexNav_DoGfxSetup(void)
{
    struct DexNavGUI *ui = sDexNavUiDataPtr;

    switch (gMain.state)
    {
    case 0:
        SetVBlankHBlankCallbacksToNull();
        ClearScheduledBgCopiesToVram();
        gMain.state++;
        break;
    case 1:
        ScanlineEffect_Stop();
        gMain.state++;
        break;
    case 2:
        FreeAllSpritePalettes();
        gMain.state++;
        break;
    case 3:
        ResetPaletteFade();
        ResetSpriteData();
        ResetTasks();
        gMain.state++;
        break;
    case 4:
        if (DexNav_InitBgs())
        {
            ui->state = 0;
            gMain.state++;
        }
        else
        {
            DexNavFadeAndExit();
            return TRUE;
        }
        break;
    case 5:
        if (DexNav_LoadGraphics() == TRUE)
            gMain.state++;
        break;
    case 6:
        DexNav_InitWindows();
        DexNavLoadRoster();
        RestoreDexNavCursor();
        gMain.state++;
        break;
    case 7:
        PrintDexNavTitle();
        gMain.state++;
        break;
    case 8:
        CreateTask(Task_DexNavWaitFadeIn, 0);
        gMain.state++;
        break;
    case 9:
        LoadMonIconPalettes();
        CreateInfoSprites();
        CreateSelectionCursor();
        gMain.state++;
        break;
    case 10:
        if (ui->rowCount == 0)
        {
            DrawDexNavEmptyList();
            PrintDexNavInfo();
            gSprites[ui->cursorSpriteId].invisible = TRUE;
        }
        else
        {
            CreateDexNavScrollArrows();
            UpdateDexNavSelection(TRUE);
        }
        gMain.state++;
        break;
    case 11:
        BlendPalettes(0xFFFFFFFF, 16, RGB_BLACK);
        gMain.state++;
        break;
    case 12:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB_BLACK);
        gMain.state++;
        break;
    default:
        SetVBlankCallback(DexNav_VBlankCB);
        SetMainCallback2(DexNav_MainCB);
        return TRUE;
    }

    return FALSE;
}

static void DexNav_RunSetup(void)
{
    while (!DexNav_DoGfxSetup()) {}
}

// Entry point for the dexnav GUI
static void DexNavGuiInit(MainCallback callback)
{
    assertf(DEXNAV_ENABLED, "DexNav was opened when DEXNAV_ENABLED config was disabled.\nCheck include/config/dexnav.h")
    {
        SetMainCallback2(callback);
        return;
    }

    if ((sDexNavUiDataPtr = AllocZeroed(sizeof(struct DexNavGUI))) == NULL)
    {
        SetMainCallback2(callback);
        return;
    }

    sDexNavUiDataPtr->state = 0;
    sDexNavUiDataPtr->savedCallback = callback;
    sDexNavUiDataPtr->arrowTaskId = TASK_NONE;
    for (u32 i = 0; i < WILD_ROSTER_MAX_ENTRIES; i++)
    {
        sDexNavUiDataPtr->iconSpriteIds[i] = MAX_SPRITES;
        sDexNavUiDataPtr->markSpriteIds[i] = MAX_SPRITES;
    }
    SetMainCallback2(DexNav_RunSetup);
}

static void Task_OpenRivalTutorialDexNav(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        DexNavGuiInit(CB2_ReturnToFieldContinueScript);
        DestroyTask(taskId);
    }
}

void OpenRivalTutorialDexNav(void)
{
    FadeScreen(FADE_TO_BLACK, 0);
    CreateTask(Task_OpenRivalTutorialDexNav, 0);
}

void Task_OpenDexNavFromStartMenu(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        CleanupOverworldWindowsAndTilemaps();
        DexNavGuiInit(CB2_ReturnToFieldWithOpenMenu);
        DestroyTask(taskId);
    }
}

static void Task_DexNavWaitFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_DexNavMain;
}

static void MoveDexNavCursor(s32 rowDelta, s32 colDelta)
{
    struct DexNavGUI *ui = sDexNavUiDataPtr;

    // Up and Down stop at the ends of the list; Left and Right go round a row.
    if (rowDelta != 0)
    {
        if ((rowDelta < 0 && ui->cursorRow == 0) || (rowDelta > 0 && ui->cursorRow + 1 >= ui->rowCount))
            return;
        ui->cursorRow += rowDelta;
        ui->cursorCol = min(ui->cursorCol, ui->rows[ui->cursorRow].count - 1);
    }
    else
    {
        u32 count = ui->rows[ui->cursorRow].count;
        if (count == 1)
            return;
        ui->cursorCol = (ui->cursorCol + count + colDelta) % count;
    }
    PlaySE(SE_RG_BAG_CURSOR);
    UpdateDexNavSelection(FALSE);
}

static void Task_DexNavMain(u8 taskId)
{
    struct Task *task = &gTasks[taskId];
    const struct WildRosterEntry *entry;

    if (IsSEPlaying())
        return;

    u16 pressed = IsRivalDexNavTutorialActive() ? RivalTutorialDexNavKeys() : gMain.newKeys;
    u16 repeated = IsRivalDexNavTutorialActive() ? 0 : gMain.newAndRepeatedKeys;

    if (pressed & B_BUTTON)
    {
        PlaySE(SE_POKENAV_OFF);
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
        task->func = Task_DexNavFadeAndExit;
        return;
    }

    entry = GetSelectedDexNavEntry();
    if (IsRivalDexNavTutorialActive() && (entry == NULL || !CanDexNavSearchFor(entry)))
    {
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
        task->func = Task_DexNavFadeAndExit;
        return;
    }
    if (entry == NULL)
    {
        if ((pressed & (A_BUTTON | R_BUTTON)))
            PlaySE(SE_FAILURE);
        return;
    }

    if ((repeated & (DPAD_UP)))
    {
        MoveDexNavCursor(-1, 0);
    }
    else if ((repeated & (DPAD_DOWN)))
    {
        MoveDexNavCursor(1, 0);
    }
    else if ((repeated & (DPAD_LEFT)))
    {
        MoveDexNavCursor(0, -1);
    }
    else if ((repeated & (DPAD_RIGHT)))
    {
        MoveDexNavCursor(0, 1);
    }
    else if ((pressed & (R_BUTTON)))
    {
        // Register the Pokémon for R in the field.
        if (CanDexNavSearchFor(entry))
        {
            VarSet(DN_VAR_SPECIES, (GetDexNavEntryEnvironment(entry) << 14) | entry->species);
            PrintDexNavTitle();
            PlayCry_Script(entry->species, 0);
        }
        else
        {
            PlaySE(SE_FAILURE);
        }
    }
    else if ((pressed & (A_BUTTON)))
    {
        // The panel already says how to find the ones it cannot search for.
        if (CanDexNavSearchFor(entry))
        {
            gSpecialVar_0x8000 = entry->species;
            gSpecialVar_0x8001 = GetDexNavEntryEnvironment(entry);
            PlaySE(SE_DEX_SEARCH);
            BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB_BLACK);
            task->func = Task_DexNavExitAndSearch;
        }
        else
        {
            PlaySE(SE_FAILURE);
        }
    }
}

/////////////////////////
//// GENERAL UTILITY ////
/////////////////////////
void ResetDexNavSearch(void)
{
    gSaveBlock3Ptr->dexNavChain = 0;    //reset dex nav chaining on new map
    if (FlagGet(DN_FLAG_SEARCHING))
        EndDexNavSearch();   //moving to new map ends dexnav search
}

void IncrementDexNavChain(void)
{
    if (gSaveBlock3Ptr->dexNavChain < DEXNAV_CHAIN_MAX)
        gSaveBlock3Ptr->dexNavChain++;
}

// Birch hands DexNav over with the Pokedex. Saves made before DexNav existed
// already hold the Pokedex, so they get it once on load, with a clean
// registration (its var once held other state).
void GiveDexNavIfNeeded(void)
{
    if (!FlagGet(FLAG_SYS_POKEDEX_GET) || FlagGet(DN_FLAG_DEXNAV_GET))
        return;
    FlagSet(DN_FLAG_DEXNAV_GET);
    VarSet(DN_VAR_SPECIES, SPECIES_NONE);
}

#if TESTING
void Test_DexNavTutorialSearchFailure(void)
{
    DexNavSearchBail(EventScript_NotFoundNearby);
}

// The level a search would use on the current map, or MON_LEVEL_NONEXISTENT.
u8 Test_DexNavGenerateMonLevel(enum Species species, enum EncounterType environment)
{
    return DexNavTryGenerateMonLevel(species, environment);
}

// The DexNav screen's list on the current map, in display order.
u32 Test_DexNavGetList(struct WildRosterEntry *entries, u32 max)
{
    typeof(sDexNavUiDataPtr) saved = sDexNavUiDataPtr;
    u32 count;

    sDexNavUiDataPtr = AllocZeroed(sizeof(*sDexNavUiDataPtr));
    if (sDexNavUiDataPtr == NULL)
    {
        sDexNavUiDataPtr = saved;
        return 0;
    }
    DexNavLoadRoster();
    count = 0;
    for (u32 row = 0; row < sDexNavUiDataPtr->rowCount; row++)
    {
        for (u32 col = 0; col < sDexNavUiDataPtr->rows[row].count; col++)
        {
            if (count < max)
                entries[count] = sDexNavUiDataPtr->roster[sDexNavUiDataPtr->rows[row].first + col];
            count++;
        }
    }
    Free(sDexNavUiDataPtr);
    sDexNavUiDataPtr = saved;
    return count;
}

// Runs a search on the current map up to the encounter: TRUE, with the
// Pokémon made in the opponent's first slot, when the DexNav finds one.
bool32 Test_DexNavCreateSearchMon(enum Species species, enum EncounterType environment)
{
    typeof(sDexNavSearchDataPtr) saved = sDexNavSearchDataPtr;
    bool32 found;

    sDexNavSearchDataPtr = AllocZeroed(sizeof(*sDexNavSearchDataPtr));
    if (sDexNavSearchDataPtr == NULL)
    {
        sDexNavSearchDataPtr = saved;
        return FALSE;
    }
    sDexNavSearchDataPtr->species = species;
    sDexNavSearchDataPtr->environment = environment;
    sDexNavSearchDataPtr->monLevel = DexNavTryGenerateMonLevel(species, environment);
    found = sDexNavSearchDataPtr->monLevel != MON_LEVEL_NONEXISTENT;
    if (found)
    {
            CreateDexNavSearchMon();
    }
    gDexNavSpecies = SPECIES_NONE;
    Free(sDexNavSearchDataPtr);
    sDexNavSearchDataPtr = saved;
    return found;
}

// A and R on this entry search and register (TRUE) or only buzz.
bool32 Test_DexNavCanSearchFor(const struct WildRosterEntry *entry)
{
    return CanDexNavSearchFor(entry);
}

// Where the list puts its icons: the cell pitch, row height and each
// sprite's top below its row's top.
void Test_DexNavGetIconLayout(u32 *pitch, u32 *rowHeight, s32 *spriteTop)
{
    *pitch = LIST_ICON_PITCH;
    *rowHeight = LIST_ROW_HEIGHT;
    *spriteTop = LIST_ICON_Y - 16;
}

// The panel's line under this entry.
const u8 *Test_DexNavGetEntryHint(const struct WildRosterEntry *entry)
{
    return GetDexNavEntryHint(entry);
}

bool32 Test_DexNavIsUsableHere(void)
{
    return IsDexNavUsableHere();
}
#endif
