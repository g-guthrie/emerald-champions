// trigger data
static const u8 ALIGNED(4) sMegaTriggerGfx[] = INCGFX_U8("graphics/battle_interface/mega_trigger.png", ".4bpp");
static const u8 ALIGNED(4) sZMoveTriggerGfx[] = INCGFX_U8("graphics/battle_interface/z_move_trigger.png", ".4bpp");
static const u8 ALIGNED(4) sBurstTriggerGfx[] = INCGFX_U8("graphics/battle_interface/burst_trigger.png", ".4bpp");
static const u8 ALIGNED(4) sDynamaxTriggerGfx[] = INCGFX_U8("graphics/battle_interface/dynamax_trigger.png", ".4bpp");

static const u16 sMegaTriggerPal[] = INCGFX_U16("graphics/battle_interface/mega_trigger.png", ".gbapal");
static const u16 sZMoveTriggerPal[] = INCGFX_U16("graphics/battle_interface/z_move_trigger.png", ".gbapal");
static const u16 sBurstTriggerPal[] = INCGFX_U16("graphics/battle_interface/burst_trigger.png", ".gbapal");
static const u16 sDynamaxTriggerPal[] = INCGFX_U16("graphics/battle_interface/dynamax_trigger.png", ".gbapal");

static const struct SpriteSheet sSpriteSheet_MegaTrigger = {sMegaTriggerGfx, sizeof(sMegaTriggerGfx), TAG_GIMMICK_TRIGGER_TILE};
static const struct SpriteSheet sSpriteSheet_ZMoveTrigger = {sZMoveTriggerGfx, sizeof(sZMoveTriggerGfx), TAG_GIMMICK_TRIGGER_TILE};
static const struct SpriteSheet sSpriteSheet_BurstTrigger = {sBurstTriggerGfx, sizeof(sBurstTriggerGfx), TAG_GIMMICK_TRIGGER_TILE};
static const struct SpriteSheet sSpriteSheet_DynamaxTrigger = {sDynamaxTriggerGfx, sizeof(sDynamaxTriggerGfx), TAG_GIMMICK_TRIGGER_TILE};

static const struct SpritePalette sSpritePalette_MegaTrigger = {sMegaTriggerPal, TAG_GIMMICK_TRIGGER_TILE};
static const struct SpritePalette sSpritePalette_ZMoveTrigger = {sZMoveTriggerPal, TAG_GIMMICK_TRIGGER_PAL};
static const struct SpritePalette sSpritePalette_BurstTrigger = {sBurstTriggerPal, TAG_GIMMICK_TRIGGER_TILE};
static const struct SpritePalette sSpritePalette_DynamaxTrigger = {sDynamaxTriggerPal, TAG_GIMMICK_TRIGGER_PAL};

static const struct OamData sOamData_GimmickTrigger =
{
    .y = 0,
    .affineMode = 0,
    .objMode = 0,
    .mosaic = 0,
    .bpp = 0,
    .shape = ST_OAM_SQUARE,
    .x = 0,
    .matrixNum = 0,
    .size = 2,
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

static const union AnimCmd sSpriteAnim_GimmickTriggerOff[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_GimmickTriggerOn[] =
{
    ANIMCMD_FRAME(16, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_GimmickTrigger[] =
{
    sSpriteAnim_GimmickTriggerOff,
    sSpriteAnim_GimmickTriggerOn,
};

static void SpriteCb_GimmickTrigger(struct Sprite *sprite);
static const struct SpriteTemplate sSpriteTemplate_GimmickTrigger =
{
    .tileTag = TAG_GIMMICK_TRIGGER_TILE,
    .paletteTag = TAG_GIMMICK_TRIGGER_PAL,
    .oam = &sOamData_GimmickTrigger,
    .anims = sSpriteAnimTable_GimmickTrigger,
    .callback = SpriteCb_GimmickTrigger,
};

// indicator data
static const u8 ALIGNED(4) sMegaIndicatorGfx[] = INCGFX_U8("graphics/battle_interface/mega_indicator.png", ".4bpp");
static const u8 ALIGNED(4) sAlphaIndicatorGfx[] = INCGFX_U8("graphics/battle_interface/alpha_indicator.png", ".4bpp");
static const u8 ALIGNED(4) sOmegaIndicatorGfx[] = INCGFX_U8("graphics/battle_interface/omega_indicator.png", ".4bpp");
static const u8 ALIGNED(4) sDynamaxIndicatorGfx[] = INCGFX_U8("graphics/battle_interface/dynamax_indicator.png", ".4bpp");
static const u8 ALIGNED(4) sNormalIndicatorGfx[] = INCGFX_U8("graphics/battle_interface/normal_indicator.png", ".4bpp");

static const u16 sMiscIndicatorPal[] = INCGFX_U16("graphics/battle_interface/misc_indicator.pal", ".gbapal"); // has room for more colors
static const u16 sMegaIndicatorPal[] = INCGFX_U16("graphics/battle_interface/mega_indicator.png", ".gbapal");

static const struct SpritePalette sSpritePalette_MiscIndicator = {sMiscIndicatorPal, TAG_MISC_INDICATOR_PAL};
static const struct SpritePalette sSpritePalette_MegaIndicator = {sMegaIndicatorPal, TAG_MEGA_INDICATOR_PAL};

static const struct OamData sOamData_GimmickIndicator =
{
    .shape = SPRITE_SHAPE(8x16),
    .size = SPRITE_SIZE(8x16),
    .priority = 1,
};

static void SpriteCb_GimmickIndicator(struct Sprite *sprite);

#define BATTLER_INDICATOR_TAG 0xDEDE
static const struct SpriteSheet sBattler_GimmickSpritesheets[] =
{
    {sNormalIndicatorGfx, sizeof(sNormalIndicatorGfx), BATTLER_INDICATOR_TAG},
    {sNormalIndicatorGfx, sizeof(sNormalIndicatorGfx), BATTLER_INDICATOR_TAG+1},
    {sNormalIndicatorGfx, sizeof(sNormalIndicatorGfx), BATTLER_INDICATOR_TAG+2},
    {sNormalIndicatorGfx, sizeof(sNormalIndicatorGfx), BATTLER_INDICATOR_TAG+3},
};

static const struct SpriteTemplate sSpriteTemplate_BattlerIndicators[] =
{
    [0] =
    {
        .tileTag = BATTLER_INDICATOR_TAG,   // updated dynamically
        .paletteTag = TAG_MISC_INDICATOR_PAL,   // updated dynamically
        .oam = &sOamData_GimmickIndicator,
        .callback = SpriteCb_GimmickIndicator,
    },
    [1] =
    {
        .tileTag = BATTLER_INDICATOR_TAG + 1,   // updated dynamically
        .paletteTag = TAG_MISC_INDICATOR_PAL,   // updated dynamically
        .oam = &sOamData_GimmickIndicator,
        .callback = SpriteCb_GimmickIndicator,
    },
    [2] =
    {
        .tileTag = BATTLER_INDICATOR_TAG + 2,   // updated dynamically
        .paletteTag = TAG_MISC_INDICATOR_PAL,   // updated dynamically
        .oam = &sOamData_GimmickIndicator,
        .callback = SpriteCb_GimmickIndicator,
    },
    [3] =
    {
        .tileTag = BATTLER_INDICATOR_TAG + 3,   // updated dynamically
        .paletteTag = TAG_MISC_INDICATOR_PAL,   // updated dynamically
        .oam = &sOamData_GimmickIndicator,
        .callback = SpriteCb_GimmickIndicator,
    },
};
