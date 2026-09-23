#include "global.h"
#include "item_icon.h"
#include "mon_markings.h"
#include "berry.h"
#include "pokemon_icon.h"
#include "event_object_movement.h"
#include "decoration.h"
#include "constants/decorations.h"
#include "item_menu.h"
#include "item_menu_icons.h"
#include "malloc.h"
#include "sprite.h"
#include "test/test.h"

extern u8 Test_ShowOverworldItemIcon(enum Item item, bool32 flash);
extern void Test_DestroyOverworldItemIcon(void);

TEST("Overworld item icons: exhaustion leaves the sentinel alone and repeated cleanup preserves reused slots")
{
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeSpriteTileRanges();
    gReservedSpritePaletteCount = 0;
    for (u32 i = 0; i < MAX_SPRITES; i++)
        gSprites[i].inUse = TRUE;
    struct Sprite sentinel = gSprites[MAX_SPRITES];
    EXPECT_EQ(Test_ShowOverworldItemIcon(ITEM_POTION, TRUE), MAX_SPRITES);
    Test_DestroyOverworldItemIcon();
    EXPECT_EQ(memcmp(&sentinel, &gSprites[MAX_SPRITES], sizeof(sentinel)), 0);
    for (u32 i = 0; i < MAX_SPRITES; i++)
        EXPECT(gSprites[i].inUse);
    ResetSpriteData();
    u8 id = Test_ShowOverworldItemIcon(ITEM_POTION, TRUE);
    EXPECT(id < MAX_SPRITES);
    EXPECT(gSprites[id].copyToObjWin);
    Test_DestroyOverworldItemIcon();
    u8 replacement = CreateInvisibleSprite(SpriteCallbackDummy);
    EXPECT_EQ(replacement, id);
    Test_DestroyOverworldItemIcon();
    EXPECT(gSprites[replacement].inUse);
    DestroySprite(&gSprites[replacement]);
}

TEST("Item icons: scratch failure cleans up and bounded decompression headroom can create an icon")
{
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeSpriteTileRanges();
    gReservedSpritePaletteCount = 0;
    u32 available;
    bool32 custom;
    for (u32 size = 0; size <= 0x500; size += 0x500)
        for (u32 variant = 0; variant < 2; variant++)
            PARAMETRIZE { available = size; custom = variant; }
    void *blocks[64];
    u32 sizes[64];
    u32 count = 0, largest = 0;
    const struct MemBlock *head = HeapHead(), *block = head;
    do
    {
        if (!block->allocated)
        {
            ASSUME(count < ARRAY_COUNT(blocks));
            sizes[count] = block->size;
            blocks[count] = AllocUnchecked(block->size);
            if (sizes[count] > sizes[largest])
                largest = count;
            count++;
        }
        block = block->next;
    } while (block != head);
    if (available)
    {
        ASSUME(sizes[largest] > available + sizeof(struct MemBlock));
        Free(blocks[largest]);
        blocks[largest] = AllocUnchecked(sizes[largest] - available - sizeof(struct MemBlock));
    }
    u8 result = custom
        ? AddCustomItemIconSprite(&gItemIconSpriteTemplate, 0x7100, 0x7100, ITEM_POTION)
        : AddItemIconSprite(0x7100, 0x7100, ITEM_POTION);
    bool32 cleaned = gItemIconDecompressionBuffer == NULL && gItemIcon4x4Buffer == NULL;
    FreeItemIconTemporaryBuffers();
    for (u32 i = 0; i < count; i++)
        Free(blocks[i]);
    if (available)
    {
        EXPECT(result < MAX_SPRITES);
        DestroySpriteAndFreeResources(&gSprites[result]);
    }
    else
        EXPECT_EQ(result, MAX_SPRITES);
    EXPECT(cleaned);
    EXPECT(AllocItemIconTemporaryBuffers());
    for (u32 i = 0; i < 0x200; i++)
        EXPECT_EQ(gItemIcon4x4Buffer[i], 0);
    FreeItemIconTemporaryBuffers();
}

TEST("Item icons: normal and custom icons coexist with independent graphics tags")
{
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeSpriteTileRanges();
    gReservedSpritePaletteCount = 0;
    u8 normal = AddItemIconSprite(0x7100, 0x7100, ITEM_POTION);
    u8 custom = AddCustomItemIconSprite(&gItemIconSpriteTemplate, 0x7101, 0x7101, ITEM_POKE_BALL);
    EXPECT(normal < MAX_SPRITES);
    EXPECT(custom < MAX_SPRITES);
    EXPECT(normal != custom);
    EXPECT(GetSpriteTileStartByTag(0x7100) != TAG_NONE);
    EXPECT(GetSpriteTileStartByTag(0x7101) != TAG_NONE);
    EXPECT(gSprites[normal].oam.paletteNum != gSprites[custom].oam.paletteNum);
    EXPECT(gItemIconDecompressionBuffer == NULL);
    EXPECT(gItemIcon4x4Buffer == NULL);
    EXPECT_EQ(gSprites[normal].template->tileTag, 0x7100);
    EXPECT_EQ(gSprites[custom].template->tileTag, 0x7101);
    DestroySpriteAndFreeResources(&gSprites[normal]);
    EXPECT_EQ(GetSpriteTileStartByTag(0x7100), TAG_NONE);
    EXPECT(GetSpriteTileStartByTag(0x7101) != TAG_NONE);
    DestroySpriteAndFreeResources(&gSprites[custom]);
}

TEST("Bag icons: cleanup uses owned tags even when a temporary template is reused")
{
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeSpriteTileRanges();
    gReservedSpritePaletteCount = 0;
    struct BagMenu *previous = gBagMenu;
    gBagMenu = AllocZeroed(sizeof(*gBagMenu));
    memset(gBagMenu->spriteIds, SPRITE_NONE, sizeof(gBagMenu->spriteIds));
    AddBagItemIconSprite(ITEM_POTION, 0);
    AddBagItemIconSprite(ITEM_POKE_BALL, 1);
    u8 first = gBagMenu->spriteIds[ITEMMENUSPRITE_ITEM];
    u8 second = gBagMenu->spriteIds[ITEMMENUSPRITE_ITEM + 1];
    EXPECT(first < MAX_SPRITES && second < MAX_SPRITES);
    u16 firstTag = GetSpriteTileTagByTileStart(gSprites[first].sheetTileStart);
    u16 secondTag = GetSpriteTileTagByTileStart(gSprites[second].sheetTileStart);
    // Model temporary-template storage being reused for the other icon.
    struct SpriteTemplate reused = gItemIconSpriteTemplate;
    reused.tileTag = secondTag;
    reused.paletteTag = GetSpritePaletteTagByPaletteNum(gSprites[second].oam.paletteNum);
    gSprites[first].template = &reused;
    RemoveBagItemIconSprite(0);
    bool32 correct = GetSpriteTileStartByTag(firstTag) == TAG_NONE
        && GetSpriteTileStartByTag(secondTag) != TAG_NONE;
    RemoveBagItemIconSprite(1);
    Free(gBagMenu);
    gBagMenu = previous;
    EXPECT(correct);
}

TEST("Item icons: exhausted sprite slots roll back new graphics without touching shared tags")
{
    bool32 shared;
    PARAMETRIZE { shared = FALSE; }
    PARAMETRIZE { shared = TRUE; }
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeSpriteTileRanges();
    gReservedSpritePaletteCount = 0;
    if (shared)
        EXPECT(AddItemIconSprite(0x7100, 0x7100, ITEM_POTION) < MAX_SPRITES);
    for (u32 i = 0; i < MAX_SPRITES; i++)
        gSprites[i].inUse = TRUE;
    EXPECT_EQ(AddItemIconSprite(0x7100, 0x7100, ITEM_POTION), MAX_SPRITES);
    EXPECT_EQ((bool32)(GetSpriteTileStartByTag(0x7100) != TAG_NONE), shared);
    EXPECT_EQ((bool32)(IndexOfSpritePaletteTag(0x7100) != 0xFF), shared);
    EXPECT(gItemIconDecompressionBuffer == NULL);
    ResetSpriteData();
    FreeSpriteTilesByTag(0x7100);
    FreeSpritePaletteByTag(0x7100);
}

TEST("Decoration icons: temporary templates survive until generic sprite cleanup")
{
    u32 decor;
    PARAMETRIZE { decor = DECOR_HEAVY_DESK; }
    PARAMETRIZE { decor = DECOR_SMALL_DESK; }
    PARAMETRIZE { decor = DECOR_PIKACHU_DOLL; }
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeSpriteTileRanges();
    gReservedSpritePaletteCount = 0;
    u8 id = AddDecorationIconObject(decor, 0, 0, 0, 0x7100, 0x7100);
    EXPECT(id < MAX_SPRITES);
    u16 tileTag = gSprites[id].template->tileTag;
    u16 paletteTag = gSprites[id].template->paletteTag;
    if (decor != DECOR_PIKACHU_DOLL)
    {
        EXPECT_EQ(tileTag, 0x7100);
        EXPECT_EQ(paletteTag, 0x7100);
    }
    EXPECT(paletteTag != TAG_NONE);
    DestroySpriteAndFreeResources(&gSprites[id]);
    if (tileTag != TAG_NONE) // Untagged object graphics own raw tiles instead.
        EXPECT_EQ(GetSpriteTileStartByTag(tileTag), TAG_NONE);
    EXPECT_EQ((u32)gSprites[id].inUse, FALSE);
    EXPECT_EQ(IndexOfSpritePaletteTag(paletteTag), 0xFF);
}

TEST("Sprite copies: template ownership survives original slot reuse")
{
    bool32 reverse, dynamic;
    for (u32 direction = 0; direction < 2; direction++)
        for (u32 owned = 0; owned < 2; owned++)
            PARAMETRIZE { reverse = direction; dynamic = owned; }
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeSpriteTileRanges();
    gReservedSpritePaletteCount = 0;
    u8 original = AddItemIconSprite(0x7100, 0x7100, ITEM_POTION);
    EXPECT(original < MAX_SPRITES);
    struct SpriteTemplate staticTemplate = *gSprites[original].template;
    if (!dynamic)
        gSprites[original].template = &staticTemplate;
    u8 clone = reverse ? CreateCopySpriteAt(&gSprites[original], 10, 11, 12)
        : CopySprite(&gSprites[original], 10, 11, 12);
    EXPECT(clone < MAX_SPRITES);
    EXPECT_EQ(gSprites[clone].x, 10);
    EXPECT_EQ(gSprites[clone].y, 11);
    EXPECT_EQ(gSprites[clone].subpriority, 12);
    if (!dynamic)
        EXPECT(gSprites[clone].template == &staticTemplate);
    DestroySprite(&gSprites[original]); // The clone still owns the loaded resources.
    u8 replacement = AddItemIconSprite(0x7101, 0x7101, ITEM_POKE_BALL);
    EXPECT_EQ(replacement, original);
    EXPECT_EQ(gSprites[clone].template->tileTag, 0x7100);
    DestroySpriteAndFreeResources(&gSprites[clone]);
    EXPECT_EQ(GetSpriteTileStartByTag(0x7100), TAG_NONE);
    EXPECT(GetSpriteTileStartByTag(0x7101) != TAG_NONE);
    DestroySpriteAndFreeResources(&gSprites[replacement]);
}

TEST("Marking sprites: separate dynamic templates retain their own resource tags")
{
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeSpriteTileRanges();
    gReservedSpritePaletteCount = 0;
    struct Sprite *first = CreateMonMarkingAllCombosSprite(0x7100, 0x7100, NULL);
    EXPECT(first != NULL);
    struct Sprite *second = CreateMonMarkingComboSprite(0x7101, 0x7101, NULL);
    EXPECT(second != NULL);
    EXPECT(first->template != second->template);
    EXPECT_EQ(first->template->tileTag, 0x7100);
    EXPECT_EQ(first->template->paletteTag, 0x7100);
    EXPECT_EQ(second->template->tileTag, 0x7101);
    DestroySpriteAndFreeResources(first);
    EXPECT_EQ(GetSpriteTileStartByTag(0x7100), TAG_NONE);
    EXPECT_NE(GetSpriteTileStartByTag(0x7101), TAG_NONE);
    DestroySpriteAndFreeResources(second);
    EXPECT_EQ(GetSpriteTileStartByTag(0x7101), TAG_NONE);
    EXPECT_EQ(IndexOfSpritePaletteTag(0x7100), 0xFF);
    EXPECT_EQ(IndexOfSpritePaletteTag(0x7101), 0xFF);
}

TEST("Berry icons: templates remain distinct and cleanup releases raw sprite tiles")
{
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeSpriteTileRanges();
    gReservedSpritePaletteCount = 0;
    u32 firstBerry = ItemIdToBerryType(ITEM_CHERI_BERRY);
    u32 secondBerry = ItemIdToBerryType(ITEM_CHESTO_BERRY);
    u32 first = CreateBerryTagSprite(firstBerry, 0, 0);
    u32 second = CreateBerryTagSprite(secondBerry, 64, 0);
    EXPECT_LT(first, MAX_SPRITES);
    EXPECT_LT(second, MAX_SPRITES);
    EXPECT(gSprites[first].template != gSprites[second].template);
    EXPECT(gSprites[first].template->images == gSprites[first].images);
    EXPECT(gSprites[second].template->images == gSprites[second].images);
    u32 tile = gSprites[first].oam.tileNum;
    DestroyBerryIconSprite(first, firstBerry, TRUE);
    EXPECT(gSprites[second].inUse);
    first = CreateBerryTagSprite(firstBerry, 0, 0);
    EXPECT_EQ((u32)gSprites[first].oam.tileNum, tile);
    DestroyBerryIconSprite(first, firstBerry, TRUE);
    DestroyBerryIconSprite(second, secondBerry, TRUE);
}

TEST("Pokemon icons: tagged and raw icon templates survive later factory calls")
{
    ResetSpriteData();
    FreeAllSpritePalettes();
    FreeSpriteTileRanges();
    gReservedSpritePaletteCount = 0;
    u32 tagged = CreateTaggedMonIcon(0x7100, 0x7100, SPECIES_EEVEE);
    LoadMonIconPalette(SPECIES_PIKACHU);
    u32 raw = CreateMonIcon(SPECIES_PIKACHU, SpriteCB_MonIcon, 0, 0, 0, 0);
    EXPECT(gSprites[tagged].template != gSprites[raw].template);
    EXPECT_EQ(gSprites[tagged].template->tileTag, 0x7100);
    EXPECT_EQ(gSprites[raw].template->tileTag, TAG_NONE);
    EXPECT_EQ(gSprites[raw].template->images->size, 32 * 32 / 2);
    EXPECT_EQ(gSprites[raw].images, (const struct SpriteFrameImage *)GetMonIconTiles(SPECIES_PIKACHU, 0));
    u32 rawTile = gSprites[raw].oam.tileNum;
    FreeAndDestroyMonIconSprite(&gSprites[raw]);
    raw = CreateMonIcon(SPECIES_PIKACHU, SpriteCB_MonIcon, 0, 0, 0, 0);
    EXPECT_EQ((u32)gSprites[raw].oam.tileNum, rawTile);
    FreeAndDestroyMonIconSprite(&gSprites[raw]);
    FreeMonIconPalette(SPECIES_PIKACHU);
    DestroySpriteAndFreeResources(&gSprites[tagged]);
    EXPECT_EQ(GetSpriteTileStartByTag(0x7100), TAG_NONE);
}
