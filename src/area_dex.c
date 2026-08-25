#include "global.h"
#include "area_dex.h"
#include "bg.h"
#include "data.h"
#include "gpu_regs.h"
#include "main.h"
#include "menu.h"
#include "overworld.h"
#include "palette.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "region_map.h"
#include "scanline_effect.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "task.h"
#include "text.h"
#include "text_window.h"
#include "wild_encounter.h"
#include "window.h"
#include "constants/maps.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#define AREA_DEX_METHOD_COUNT 8
#define AREA_DEX_PAGE_SIZE 6
#define AREA_DEX_MAX_SPECIES LAND_WILD_COUNT

enum
{
    WIN_TITLE,
    WIN_LIST,
    WIN_HELP,
};

enum
{
    METHOD_GRASS,
    METHOD_SURF,
    METHOD_OLD_ROD,
    METHOD_GOOD_ROD,
    METHOD_SUPER_ROD,
    METHOD_ROCK_SMASH,
    METHOD_HONEY,
    METHOD_SPECIAL,
};

struct AreaDexMethod
{
    const u8 *name;
    const struct WildPokemonInfo *info;
    u8 slotType;
    u8 firstSlot;
    u8 slotCount;
    bool8 isSpecial;
};

struct AreaDexEntry
{
    u16 species;
    u8 chance;
};

struct AreaDexScreen
{
    struct AreaDexMethod methods[AREA_DEX_METHOD_COUNT];
    struct AreaDexEntry entries[AREA_DEX_MAX_SPECIES];
    u8 iconSpriteIds[AREA_DEX_PAGE_SIZE];
    u8 methodCount;
    u8 methodIndex;
    u8 page;
    u8 entryCount;
};

static void MainCB2(void);
static void VBlankCB(void);
static void Task_AreaDexFadeIn(u8 taskId);
static void Task_AreaDexProcessInput(u8 taskId);
static void Task_AreaDexFadeOut(u8 taskId);
static void BuildMethodList(void);
static void AddMethod(const u8 *name, const struct WildPokemonInfo *info, u8 slotType, u8 firstSlot, u8 slotCount, bool8 isSpecial);
static void CollectEntries(void);
static void RenderAreaDex(void);
static void DestroyEntryIcons(void);
static void PrintCentered(u8 windowId, const u8 *text, u8 y);

static EWRAM_DATA struct AreaDexScreen sAreaDex = {0};

static const u8 sText_Grass[] = _("Grass");
static const u8 sText_Surf[] = _("Surf");
static const u8 sText_OldRod[] = _("Old Rod");
static const u8 sText_GoodRod[] = _("Good Rod");
static const u8 sText_SuperRod[] = _("Super Rod");
static const u8 sText_RockSmash[] = _("Rock Smash");
static const u8 sText_Honey[] = _("Honey");
static const u8 sText_Special[] = _("Special");
static const u8 sText_Encounters[] = _("Encounters");
static const u8 sText_NoWildPokemon[] = _("No wild Pokémon are found here.");
static const u8 sText_Help[] = _("L/R Method  Up/Down Page  B Back");
static const u8 sText_Percent[] = _("%");
static const u8 sText_Caught[] = _("  Caught");
static const u8 sText_UnderBridge[] = _("Under bridge");
static const u8 sText_PageSeparator[] = _("  ");
static const u8 sText_Slash[] = _("/");

static const u8 *const sMethodNames[AREA_DEX_METHOD_COUNT] =
{
    [METHOD_GRASS] = sText_Grass,
    [METHOD_SURF] = sText_Surf,
    [METHOD_OLD_ROD] = sText_OldRod,
    [METHOD_GOOD_ROD] = sText_GoodRod,
    [METHOD_SUPER_ROD] = sText_SuperRod,
    [METHOD_ROCK_SMASH] = sText_RockSmash,
    [METHOD_HONEY] = sText_Honey,
    [METHOD_SPECIAL] = sText_Special,
};

static const struct BgTemplate sBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 2,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0,
    },
};

static const struct WindowTemplate sWindowTemplates[] =
{
    [WIN_TITLE] =
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 1,
        .width = 28,
        .height = 3,
        .paletteNum = 14,
        .baseBlock = 20,
    },
    [WIN_LIST] =
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 5,
        .width = 28,
        .height = 12,
        .paletteNum = 14,
        .baseBlock = 104,
    },
    [WIN_HELP] =
    {
        .bg = 0,
        .tilemapLeft = 1,
        .tilemapTop = 18,
        .width = 28,
        .height = 1,
        .paletteNum = 14,
        .baseBlock = 440,
    },
    DUMMY_WIN_TEMPLATE,
};

static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    RunTextPrinters();
    UpdatePaletteFade();
}

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void CB2_InitAreaDex(void)
{
    switch (gMain.state)
    {
    case 0:
        SetVBlankCallback(NULL);
        DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000);
        DmaClear32(3, OAM, OAM_SIZE);
        DmaClear16(3, PLTT, PLTT_SIZE);
        ResetBgsAndClearDma3BusyFlags(0);
        InitBgsFromTemplates(0, sBgTemplates, ARRAY_COUNT(sBgTemplates));
        InitWindows(sWindowTemplates);
        DeactivateAllTextPrinters();
        SetGpuReg(REG_OFFSET_DISPCNT, 0);
        gMain.state++;
        break;
    case 1:
        ResetPaletteFade();
        ScanlineEffect_Stop();
        ResetTasks();
        ResetSpriteData();
        FreeAllSpritePalettes();
        memset(&sAreaDex, 0, sizeof(sAreaDex));
        memset(sAreaDex.iconSpriteIds, MAX_SPRITES, sizeof(sAreaDex.iconSpriteIds));
        LoadUserWindowBorderGfx(WIN_TITLE, 1, 0xD0);
        Menu_LoadStdPalAt(0xE0);
        LoadMonIconPalettes();
        FillBgTilemapBufferRect_Palette0(0, 0, 0, 0, 30, 20);
        PutWindowTilemap(WIN_TITLE);
        PutWindowTilemap(WIN_LIST);
        PutWindowTilemap(WIN_HELP);
        DrawStdFrameWithCustomTileAndPalette(WIN_TITLE, FALSE, 1, 0xD);
        DrawStdFrameWithCustomTileAndPalette(WIN_LIST, FALSE, 1, 0xD);
        BuildMethodList();
        RenderAreaDex();
        ShowBg(0);
        gMain.state++;
        break;
    case 2:
        CreateTask(Task_AreaDexFadeIn, 0);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0x10, 0, RGB_BLACK);
        SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_OBJ_ON);
        SetGpuReg(REG_OFFSET_BLDCNT, 0);
        SetVBlankCallback(VBlankCB);
        SetMainCallback2(MainCB2);
        break;
    }
}

static void Task_AreaDexFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_AreaDexProcessInput;
}

static void Task_AreaDexProcessInput(u8 taskId)
{
    u8 pageCount;

    if (JOY_NEW(B_BUTTON))
    {
        PlaySE(SE_SELECT);
        BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 0x10, RGB_BLACK);
        gTasks[taskId].func = Task_AreaDexFadeOut;
    }
    else if (sAreaDex.methodCount != 0 && JOY_NEW(L_BUTTON))
    {
        PlaySE(SE_SELECT);
        sAreaDex.methodIndex = (sAreaDex.methodIndex + sAreaDex.methodCount - 1) % sAreaDex.methodCount;
        sAreaDex.page = 0;
        RenderAreaDex();
    }
    else if (sAreaDex.methodCount != 0 && JOY_NEW(R_BUTTON))
    {
        PlaySE(SE_SELECT);
        sAreaDex.methodIndex = (sAreaDex.methodIndex + 1) % sAreaDex.methodCount;
        sAreaDex.page = 0;
        RenderAreaDex();
    }
    else if (sAreaDex.methodCount != 0 && JOY_NEW(DPAD_UP | DPAD_DOWN))
    {
        CollectEntries();
        pageCount = (sAreaDex.entryCount + AREA_DEX_PAGE_SIZE - 1) / AREA_DEX_PAGE_SIZE;
        if (pageCount > 1)
        {
            PlaySE(SE_SELECT);
            if (JOY_NEW(DPAD_UP))
                sAreaDex.page = (sAreaDex.page + pageCount - 1) % pageCount;
            else
                sAreaDex.page = (sAreaDex.page + 1) % pageCount;
            RenderAreaDex();
        }
    }
}

static void Task_AreaDexFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyEntryIcons();
        FreeMonIconPalettes();
        DestroyTask(taskId);
        FreeAllWindowBuffers();
        SetMainCallback2(gMain.savedCallback);
    }
}

static void AddMethod(const u8 *name, const struct WildPokemonInfo *info, u8 slotType, u8 firstSlot, u8 slotCount, bool8 isSpecial)
{
    struct AreaDexMethod *method = &sAreaDex.methods[sAreaDex.methodCount++];

    method->name = name;
    method->info = info;
    method->slotType = slotType;
    method->firstSlot = firstSlot;
    method->slotCount = slotCount;
    method->isSpecial = isSpecial;
}

static void BuildMethodList(void)
{
    u16 headerId = GetCurrentMapWildMonHeaderId();

    if (headerId != 0xFFFF)
    {
        const struct WildPokemonHeader *header = &gWildMonHeaders[headerId];

        if (header->landMonsInfo != NULL)
            AddMethod(sMethodNames[METHOD_GRASS], header->landMonsInfo, WILD_SLOT_LAND, 0, LAND_WILD_COUNT, FALSE);
        if (header->waterMonsInfo != NULL)
            AddMethod(sMethodNames[METHOD_SURF], header->waterMonsInfo, WILD_SLOT_WATER, 0, WATER_WILD_COUNT, FALSE);
        if (header->fishingMonsInfo != NULL)
        {
            AddMethod(sMethodNames[METHOD_OLD_ROD], header->fishingMonsInfo, WILD_SLOT_OLD_ROD, 0, 2, FALSE);
            AddMethod(sMethodNames[METHOD_GOOD_ROD], header->fishingMonsInfo, WILD_SLOT_GOOD_ROD, 2, 3, FALSE);
            AddMethod(sMethodNames[METHOD_SUPER_ROD], header->fishingMonsInfo, WILD_SLOT_SUPER_ROD, 5, 5, FALSE);
        }
        if (header->rockSmashMonsInfo != NULL)
            AddMethod(sMethodNames[METHOD_ROCK_SMASH], header->rockSmashMonsInfo, WILD_SLOT_ROCK_SMASH, 0, ROCK_WILD_COUNT, FALSE);
        if (header->honeyMonsInfo != NULL)
            AddMethod(sMethodNames[METHOD_HONEY], header->honeyMonsInfo, WILD_SLOT_HONEY, 0, HONEY_WILD_COUNT, FALSE);
    }

    if (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(ROUTE119)
     && gSaveBlock1Ptr->location.mapNum == MAP_NUM(ROUTE119))
        AddMethod(sMethodNames[METHOD_SPECIAL], NULL, WILD_SLOT_LAND, 0, 1, TRUE);
}

static void CollectEntries(void)
{
    const struct AreaDexMethod *method;
    u8 i;

    sAreaDex.entryCount = 0;
    if (sAreaDex.methodCount == 0)
        return;

    method = &sAreaDex.methods[sAreaDex.methodIndex];
    if (method->isSpecial)
    {
        sAreaDex.entries[0].species = SPECIES_FEEBAS;
        sAreaDex.entries[0].chance = 0;
        sAreaDex.entryCount = 1;
        return;
    }

    for (i = 0; i < method->slotCount; i++)
    {
        u8 j;
        u16 species = method->info->wildPokemon[method->firstSlot + i].species;
        u8 chance = GetWildEncounterSlotChance(method->slotType, i);

        for (j = 0; j < sAreaDex.entryCount; j++)
        {
            if (sAreaDex.entries[j].species == species)
            {
                sAreaDex.entries[j].chance += chance;
                break;
            }
        }

        if (j == sAreaDex.entryCount && sAreaDex.entryCount < AREA_DEX_MAX_SPECIES)
        {
            sAreaDex.entries[j].species = species;
            sAreaDex.entries[j].chance = chance;
            sAreaDex.entryCount++;
        }
    }
}

static void DestroyEntryIcons(void)
{
    u8 i;

    for (i = 0; i < AREA_DEX_PAGE_SIZE; i++)
    {
        if (sAreaDex.iconSpriteIds[i] != MAX_SPRITES)
        {
            FreeAndDestroyMonIconSprite(&gSprites[sAreaDex.iconSpriteIds[i]]);
            sAreaDex.iconSpriteIds[i] = MAX_SPRITES;
        }
    }
}

static void PrintCentered(u8 windowId, const u8 *text, u8 y)
{
    u16 width = GetStringWidth(1, text, 0);
    u16 windowWidth = GetWindowAttribute(windowId, WINDOW_WIDTH) * 8;
    u8 x = width < windowWidth ? (windowWidth - width) / 2 : 0;

    AddTextPrinterParameterized(windowId, 1, text, x, y, TEXT_SPEED_FF, NULL);
}

static void RenderAreaDex(void)
{
    u8 i;
    u8 pageCount;
    u8 firstEntry;
    u8 text[32];

    DestroyEntryIcons();
    FillWindowPixelBuffer(WIN_TITLE, PIXEL_FILL(1));
    FillWindowPixelBuffer(WIN_LIST, PIXEL_FILL(1));
    FillWindowPixelBuffer(WIN_HELP, PIXEL_FILL(1));

    GetMapName(gStringVar1, gMapHeader.regionMapSectionId, 0);
    PrintCentered(WIN_TITLE, gStringVar1, 0);

    if (sAreaDex.methodCount == 0)
    {
        PrintCentered(WIN_TITLE, sText_Encounters, 16);
        PrintCentered(WIN_LIST, sText_NoWildPokemon, 40);
    }
    else
    {
        const struct AreaDexMethod *method = &sAreaDex.methods[sAreaDex.methodIndex];

        CollectEntries();
        pageCount = (sAreaDex.entryCount + AREA_DEX_PAGE_SIZE - 1) / AREA_DEX_PAGE_SIZE;
        if (sAreaDex.page >= pageCount)
            sAreaDex.page = 0;

        StringCopy(text, method->name);
        if (pageCount > 1)
        {
            StringAppend(text, sText_PageSeparator);
            ConvertIntToDecimalStringN(text + StringLength(text), sAreaDex.page + 1, STR_CONV_MODE_LEFT_ALIGN, 1);
            StringAppend(text, sText_Slash);
            ConvertIntToDecimalStringN(text + StringLength(text), pageCount, STR_CONV_MODE_LEFT_ALIGN, 1);
        }
        PrintCentered(WIN_TITLE, text, 16);

        firstEntry = sAreaDex.page * AREA_DEX_PAGE_SIZE;
        for (i = 0; i < AREA_DEX_PAGE_SIZE && firstEntry + i < sAreaDex.entryCount; i++)
        {
            const struct AreaDexEntry *entry = &sAreaDex.entries[firstEntry + i];
            u8 column = i % 2;
            u8 row = i / 2;
            u8 x = column * 112;
            u8 y = row * 32;
            bool8 caught = GetSetPokedexFlag(SpeciesToNationalPokedexNum(entry->species), FLAG_GET_CAUGHT);

            sAreaDex.iconSpriteIds[i] = CreateMonIconNoPersonality(entry->species, SpriteCB_MonIcon, 28 + column * 112, 56 + row * 32, 0);
            AddTextPrinterParameterized(WIN_LIST, 1, gSpeciesNames[entry->species], x + 40, y, TEXT_SPEED_FF, NULL);

            if (method->isSpecial)
                StringCopy(text, sText_UnderBridge);
            else
            {
                ConvertIntToDecimalStringN(text, entry->chance, STR_CONV_MODE_LEFT_ALIGN, 3);
                StringAppend(text, sText_Percent);
            }
            if (caught)
                StringAppend(text, sText_Caught);
            AddTextPrinterParameterized(WIN_LIST, 1, text, x + 40, y + 16, TEXT_SPEED_FF, NULL);
        }
    }

    PrintCentered(WIN_HELP, sText_Help, 0);
    CopyWindowToVram(WIN_TITLE, 3);
    CopyWindowToVram(WIN_LIST, 3);
    CopyWindowToVram(WIN_HELP, 3);
}
