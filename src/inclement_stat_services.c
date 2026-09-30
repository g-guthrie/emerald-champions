#include "global.h"
#include "battle_message.h"
#include "battle_main.h"
#include "data.h"
#include "event_data.h"
#include "field_specials.h"
#include "international_string_util.h"
#include "pokemon.h"
#include "money.h"
#include "string_util.h"
#include "strings.h"
#include "text.h"
#include "caps.h"
#include "clock.h"
#include "item.h"
#include "menu.h"
#include "money.h"
#include "window.h"
#include "random.h"
#include "constants/field_specials.h"
#include "constants/items.h"
#include "constants/emerald_champions.h"
#include "pokerus.h"
#include "field_specials.h"

// Inclement Emerald's Super Training, Hyper Training and nature services,
// written against this engine rather than lifted from its 2021 source. The
// scripts drive them through the same variables Inclement used:
//   VAR_0x8004 party slot, VAR_0x8005 stat index, VAR_0x8006 amount.

static const u8 sStatData[NUM_STATS] =
{
    MON_DATA_HP_EV, MON_DATA_ATK_EV, MON_DATA_DEF_EV,
    MON_DATA_SPEED_EV, MON_DATA_SPATK_EV, MON_DATA_SPDEF_EV,
};

static const u8 sIvData[NUM_STATS] =
{
    MON_DATA_HP_IV, MON_DATA_ATK_IV, MON_DATA_DEF_IV,
    MON_DATA_SPEED_IV, MON_DATA_SPATK_IV, MON_DATA_SPDEF_IV,
};

static struct Pokemon *GetServiceMon(u32 slot)
{
    if (slot >= PARTY_SIZE)
        return NULL;
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][slot];
    if (!GetMonData(mon, MON_DATA_SPECIES) || GetMonData(mon, MON_DATA_IS_EGG)
        || GetMonData(mon, MON_DATA_SANITY_IS_BAD_EGG))
        return NULL;
    return mon;
}

static u32 TotalEVs(struct Pokemon *mon)
{
    u32 total = 0;

    if (mon == NULL)
        return 0;
    for (u32 i = 0; i < NUM_STATS; i++)
        total += GetMonData(mon, sStatData[i]);
    return total;
}

static void BufferAllStats(const u8 *fields, u32 digits)
{
    struct Pokemon *mon = GetServiceMon(gSpecialVar_0x8004);
    u8 *dst = gStringVar4;

    *dst = EOS;
    if (mon == NULL)
        return;
    for (u32 i = 0; i < NUM_STATS; i++)
    {
        dst = ConvertIntToDecimalStringN(dst, GetMonData(mon, fields[i]), STR_CONV_MODE_LEFT_ALIGN, digits);
        *dst++ = (i == NUM_STATS - 1) ? EOS : CHAR_SLASH;
    }
    *dst = EOS;
}

void BufferChosenMonAllEVs(void)
{
    BufferAllStats(sStatData, 3);
}

void BufferChosenMonAllIVs(void)
{
    BufferAllStats(sIvData, 2);
}

static void BufferStat(const u8 *fields, u32 digits)
{
    struct Pokemon *mon = GetServiceMon(gSpecialVar_0x8004);
    u32 stat = gSpecialVar_0x8005;

    gSpecialVar_0x8006 = 0;
    gStringVar2[0] = EOS;
    if (mon == NULL || stat >= NUM_STATS)
        return;
    gSpecialVar_0x8006 = GetMonData(mon, fields[stat]);
    ConvertIntToDecimalStringN(gStringVar2, gSpecialVar_0x8006, STR_CONV_MODE_LEFT_ALIGN, digits);
}

void BufferChosenMonEV(void)
{
    BufferStat(sStatData, 3);
}

void BufferChosenMonIV(void)
{
    BufferStat(sIvData, 2);
}

// The chosen Pokemon's Nature in STR_VAR_2 and what it does in STR_VAR_3
// ("raises Attack and lowers Sp. Atk"), for the Slateport nature chef.
void BufferChosenMonNature(void)
{
    struct Pokemon *mon = GetServiceMon(gSpecialVar_0x8004);
    gStringVar2[0] = EOS;
    gStringVar3[0] = EOS;
    if (mon == NULL)
        return;
    const struct NatureInfo *info = &gNaturesInfo[GetMonData(mon, MON_DATA_HIDDEN_NATURE)];
    StringCopy(gStringVar2, info->name);
    if (info->statUp == info->statDown)
    {
        StringCopy(gStringVar3, COMPOUND_STRING("leaves every stat as it is"));
        return;
    }
    u8 *end = StringCopy(gStringVar3, COMPOUND_STRING("raises "));
    end = StringCopy(end, gStatNamesTable[info->statUp]);
    end = StringCopy(end, HasHotSpringPokerus(mon) ? COMPOUND_STRING(" and raises ") : COMPOUND_STRING(" and lowers "));
    StringCopy(end, gStatNamesTable[info->statDown]);
}

// TRUE while the spread still has room for the requested amount. The
// spread's current total comes back in VAR_0x8007 for the refusal message.
void CheckChosenMonCanGainEVs(void)
{
    struct Pokemon *mon = GetServiceMon(gSpecialVar_0x8004);
    u32 stat = gSpecialVar_0x8005;
    u32 want = gSpecialVar_0x8006;
    gSpecialVar_0x8007 = 0;
    gSpecialVar_Result = FALSE;
    if (mon == NULL || stat >= NUM_STATS)
        return;
    u32 current = GetMonData(mon, sStatData[stat]);
    gSpecialVar_0x8007 = TotalEVs(mon);
    gSpecialVar_Result = (current + want <= MAX_PER_STAT_EVS
                       && gSpecialVar_0x8007 + want <= MAX_TOTAL_EVS);
}

bool8 Special_AreLeadMonEVsMaxedOut(void)
{
    return TotalEVs(GetServiceMon(GetLeadMonIndex())) >= MAX_TOTAL_EVS;
}

void IncreaseChosenMonEVs(void)
{
    struct Pokemon *mon = GetServiceMon(gSpecialVar_0x8004);
    u32 stat = gSpecialVar_0x8005;
    u32 want = gSpecialVar_0x8006;
    gSpecialVar_0x8007 = 0;
    gSpecialVar_Result = FALSE;
    if (mon == NULL || stat >= NUM_STATS)
        return;
    u32 current = GetMonData(mon, sStatData[stat]);
    u32 total = TotalEVs(mon);
    gSpecialVar_0x8007 = current;
    // Existing over-cap records must not wrap either remaining-room subtraction.
    if (current >= MAX_PER_STAT_EVS || total >= MAX_TOTAL_EVS)
        return;
    want = min(want, min(MAX_PER_STAT_EVS - current, MAX_TOTAL_EVS - total));
    u32 gained = current + want;
    SetMonData(mon, sStatData[stat], &gained);
    CalculateMonStats(mon);
    gSpecialVar_0x8007 = gained;
    gSpecialVar_Result = (want != 0);
}

void ResetChosenMonEVs(void)
{
    struct Pokemon *mon = GetServiceMon(gSpecialVar_0x8004);
    u32 zero = 0;

    if (mon == NULL)
        return;
    for (u32 i = 0; i < NUM_STATS; i++)
        SetMonData(mon, sStatData[i], &zero);
    CalculateMonStats(mon);
}

// The Center move tutor's EV editor plans a whole spread and applies it at
// once on confirmation, so backing out never touches the Pokémon. Its scripts
// pass the stat in VAR_0x8005 and an EV_PLAN_STEP_* in VAR_0x8006; the Pokémon
// is fixed when planning starts.
static EWRAM_DATA u8 sPlannedEVs[NUM_STATS] = {0};
static EWRAM_DATA bool8 sEvieEVPricing = FALSE;
static EWRAM_DATA u32 sPlannedEVsPersonality = 0;
static EWRAM_DATA u8 sPlannedEVsSlot = 0; // party slot + 1; 0 before planning

static const u8 *const sPlannedEVStatNames[NUM_STATS] =
{
    COMPOUND_STRING("HP"), COMPOUND_STRING("Attack"), COMPOUND_STRING("Defense"),
    COMPOUND_STRING("Speed"), COMPOUND_STRING("Sp. Atk"), COMPOUND_STRING("Sp. Def"),
};

static const u8 *const sPlannedEVShortNames[NUM_STATS] =
{
    COMPOUND_STRING("HP"), COMPOUND_STRING("Atk"), COMPOUND_STRING("Def"),
    COMPOUND_STRING("Spe"), COMPOUND_STRING("SpA"), COMPOUND_STRING("SpD"),
};

static u32 PlannedEVTotal(void)
{
    u32 total = 0;
    for (u32 i = 0; i < NUM_STATS; i++)
        total += sPlannedEVs[i];
    return total;
}

static struct Pokemon *GetPlannedEVsMon(void)
{
    struct Pokemon *mon = sPlannedEVsSlot != 0 ? GetServiceMon(sPlannedEVsSlot - 1) : NULL;
    if (mon == NULL || GetMonData(mon, MON_DATA_PERSONALITY) != sPlannedEVsPersonality)
        return NULL;
    return mon;
}

// In VAR_0x8004 party slot; out VAR_RESULT FALSE for an Egg or empty slot.
void StartPlannedEVSpread(void)
{
    struct Pokemon *mon = GetServiceMon(gSpecialVar_0x8004);

    sPlannedEVsSlot = 0;
    gSpecialVar_Result = FALSE;
    if (mon == NULL)
        return;
    sPlannedEVsSlot = gSpecialVar_0x8004 + 1;
    sPlannedEVsPersonality = GetMonData(mon, MON_DATA_PERSONALITY);
    for (u32 i = 0; i < NUM_STATS; i++)
        sPlannedEVs[i] = min(GetMonData(mon, sStatData[i]), MAX_PER_STAT_EVS);
    gSpecialVar_Result = TRUE;
}

// Editor rows keep four fixed columns whatever the name or digit count:
// the name, the current EVs right-aligned, the arrow, the planned EVs
// right-aligned (pixel positions within the list window).
#define EV_ROW_NOW_RIGHT     70
#define EV_ROW_ARROW_LEFT    76
#define EV_ROW_PLAN_RIGHT   112

static u8 *AppendClearTo(u8 *dst, u32 x)
{
    *dst++ = EXT_CTRL_CODE_BEGIN;
    *dst++ = EXT_CTRL_CODE_CLEAR_TO;
    *dst++ = x;
    *dst = EOS;
    return dst;
}

static u8 *AppendRightAligned(u8 *dst, u32 value, u32 rightEdge)
{
    u8 digits[4];
    ConvertIntToDecimalStringN(digits, value, STR_CONV_MODE_LEFT_ALIGN, 3);
    dst = AppendClearTo(dst, rightEdge - GetStringWidth(FONT_NORMAL, digits, 0));
    return StringCopy(dst, digits);
}

// One editor row for the stat in VAR_0x8005, in STR_VAR_2.
void BufferPlannedEVRow(void)
{
    struct Pokemon *mon = GetPlannedEVsMon();
    u32 stat = min(gSpecialVar_0x8005, NUM_STATS - 1);
    u8 *dst = StringCopy(gStringVar2, sPlannedEVStatNames[stat]);

    dst = AppendRightAligned(dst, mon != NULL ? GetMonData(mon, sStatData[stat]) : 0, EV_ROW_NOW_RIGHT);
    dst = AppendClearTo(dst, EV_ROW_ARROW_LEFT);
    *dst++ = CHAR_RIGHT_ARROW;
    AppendRightAligned(dst, sPlannedEVs[stat], EV_ROW_PLAN_RIGHT);
}

// Planned value of the stat in VAR_0x8005, in VAR_RESULT.
void GetPlannedEV(void)
{
    gSpecialVar_Result = gSpecialVar_0x8005 < NUM_STATS ? sPlannedEVs[gSpecialVar_0x8005] : 0;
}

// Out STR_VAR_3 the planned total.
void BufferPlannedEVTotal(void)
{
    ConvertIntToDecimalStringN(gStringVar3, PlannedEVTotal(), STR_CONV_MODE_LEFT_ALIGN, 3);
}

// Preview the actual stat on a copy; planning never changes the partner.
void BufferPlannedEVStatPrompt(void)
{
    struct Pokemon *mon = GetPlannedEVsMon();
    u32 stat = min(gSpecialVar_0x8005, NUM_STATS - 1);
    u8 *dst = StringCopy(gStringVar4, sPlannedEVStatNames[stat]);
    dst = StringCopy(dst, COMPOUND_STRING(" EVs: "));
    dst = ConvertIntToDecimalStringN(dst, mon != NULL ? GetMonData(mon, sStatData[stat]) : 0, STR_CONV_MODE_LEFT_ALIGN, 3);
    dst = StringCopy(dst, COMPOUND_STRING(" > "));
    dst = ConvertIntToDecimalStringN(dst, sPlannedEVs[stat], STR_CONV_MODE_LEFT_ALIGN, 3);
    dst = StringCopy(dst, COMPOUND_STRING("\nStat: "));
    if (mon != NULL)
    {
        struct Pokemon preview = *mon;
        for (u32 i = 0; i < NUM_STATS; i++)
        {
            u32 value = sPlannedEVs[i];
            SetMonData(&preview, sStatData[i], &value);
        }
        CalculateMonStats(&preview);
        dst = ConvertIntToDecimalStringN(dst, GetMonData(mon, MON_DATA_MAX_HP + stat), STR_CONV_MODE_LEFT_ALIGN, 3);
        dst = StringCopy(dst, COMPOUND_STRING(" > "));
        dst = ConvertIntToDecimalStringN(dst, GetMonData(&preview, MON_DATA_MAX_HP + stat), STR_CONV_MODE_LEFT_ALIGN, 3);
    }
    dst = StringCopy(dst, COMPOUND_STRING("  Total: "));
    dst = ConvertIntToDecimalStringN(dst, PlannedEVTotal(), STR_CONV_MODE_LEFT_ALIGN, 3);
    StringCopy(dst, COMPOUND_STRING("/510"));
}

void SetPlannedEVPricing(void)
{
    sEvieEVPricing = gSpecialVar_0x8004 != 0;
}

void IsEvieEVPlan(void)
{
    gSpecialVar_Result = sEvieEVPricing;
}

static u32 PlannedEVFee(void)
{
    struct Pokemon *mon = GetPlannedEVsMon();
    u32 gained = 0;
    if (mon == NULL)
        return 0;
    if (!sEvieEVPricing)
        return EV_PLAN_FEE;
    for (u32 i = 0; i < NUM_STATS; i++)
    {
        u32 original = GetMonData(mon, sStatData[i]);
        if (sPlannedEVs[i] > original)
            gained += sPlannedEVs[i] - original;
    }
    return gained * EC_EVIE_PRICE_PER_EV;
}

void BufferPlannedEVConfirmation(void)
{
    BufferPlannedEVSummary();
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("{STR_VAR_2}\n{STR_VAR_3}\p"));
    u8 *dst = gStringVar4 + StringLength(gStringVar4);
    dst = StringCopy(dst, COMPOUND_STRING("Apply this spread for ¥"));
    dst = ConvertIntToDecimalStringN(dst, PlannedEVFee(), STR_CONV_MODE_LEFT_ALIGN, 4);
    StringCopy(dst, COMPOUND_STRING("?"));
}

void PayForPlannedEVSpread(void)
{
    CheckPlannedEVSpreadChanged();
    u32 fee = gSpecialVar_Result ? PlannedEVFee() : 0;
    ConvertIntToDecimalStringN(gStringVar1, fee, STR_CONV_MODE_LEFT_ALIGN, 4);
    if (GetMoney(&gSaveBlock1Ptr->money) < fee)
    {
        gSpecialVar_Result = FALSE;
        return;
    }
    ApplyPlannedEVSpread();
    if (gSpecialVar_Result)
        RemoveMoney(&gSaveBlock1Ptr->money, fee);
}

// In VAR_0x8005 stat, VAR_0x8006 an EV_PLAN_STEP_*. Adds as much as the
// 252-per-stat and 510-total limits allow; out VAR_RESULT an EV_PLAN_*.
void AdjustPlannedEV(void)
{
    u32 stat = gSpecialVar_0x8005;
    u32 current, room;

    gSpecialVar_Result = EV_PLAN_STAT_EMPTY;
    if (stat >= NUM_STATS)
        return;
    current = sPlannedEVs[stat];
    switch (gSpecialVar_0x8006)
    {
    case EV_PLAN_STEP_ADD_4:
    case EV_PLAN_STEP_ADD_64:
    case EV_PLAN_STEP_ADD_MAX:
        if (current >= MAX_PER_STAT_EVS)
        {
            gSpecialVar_Result = EV_PLAN_STAT_FULL;
            return;
        }
        room = min(MAX_PER_STAT_EVS - current, MAX_TOTAL_EVS - min(PlannedEVTotal(), MAX_TOTAL_EVS));
        if (room == 0)
        {
            gSpecialVar_Result = EV_PLAN_TOTAL_FULL;
            return;
        }
        if (gSpecialVar_0x8006 == EV_PLAN_STEP_ADD_4)
            room = min(room, 4);
        else if (gSpecialVar_0x8006 == EV_PLAN_STEP_ADD_64)
            room = min(room, 64);
        sPlannedEVs[stat] = current + room;
        break;
    case EV_PLAN_STEP_SUB_4:
    case EV_PLAN_STEP_SUB_64:
    case EV_PLAN_STEP_CLEAR:
        if (current == 0)
            return;
        if (gSpecialVar_0x8006 == EV_PLAN_STEP_SUB_4)
            sPlannedEVs[stat] = current - min(current, 4);
        else if (gSpecialVar_0x8006 == EV_PLAN_STEP_SUB_64)
            sPlannedEVs[stat] = current - min(current, 64);
        else
            sPlannedEVs[stat] = 0;
        break;
    default:
        return;
    }
    gSpecialVar_Result = EV_PLAN_CHANGED;
}

// Clears every stat in the plan.
void ClearPlannedEVs(void)
{
    for (u32 i = 0; i < NUM_STATS; i++)
        sPlannedEVs[i] = 0;
}

// Undo Changes: the plan goes back to the Pokémon's spread, which nothing
// has touched since the editor opened.
void UndoPlannedEVs(void)
{
    struct Pokemon *mon = GetPlannedEVsMon();

    for (u32 i = 0; i < NUM_STATS; i++)
        sPlannedEVs[i] = mon != NULL ? min(GetMonData(mon, sStatData[i]), MAX_PER_STAT_EVS) : 0;
}

// Out VAR_RESULT TRUE when the plan differs from the Pokémon's spread.
void CheckPlannedEVSpreadChanged(void)
{
    struct Pokemon *mon = GetPlannedEVsMon();

    gSpecialVar_Result = FALSE;
    if (mon == NULL)
        return;
    for (u32 i = 0; i < NUM_STATS; i++)
    {
        if (GetMonData(mon, sStatData[i]) != sPlannedEVs[i])
            gSpecialVar_Result = TRUE;
    }
}

// The confirmation summary: out STR_VAR_2 and STR_VAR_3, three stats each.
void BufferPlannedEVSummary(void)
{
    for (u32 line = 0; line < 2; line++)
    {
        u8 *dst = line == 0 ? gStringVar2 : gStringVar3;
        for (u32 i = line * 3; i < line * 3 + 3; i++)
        {
            dst = StringCopy(dst, sPlannedEVShortNames[i]);
            *dst++ = CHAR_SPACE;
            dst = ConvertIntToDecimalStringN(dst, sPlannedEVs[i], STR_CONV_MODE_LEFT_ALIGN, 3);
            if (i != line * 3 + 2)
                dst = StringCopy(dst, COMPOUND_STRING("   "));
        }
        *dst = EOS;
    }
}

// Writes the whole plan at once; out VAR_RESULT FALSE if the Pokémon is gone.
void ApplyPlannedEVSpread(void)
{
    struct Pokemon *mon = GetPlannedEVsMon();

    gSpecialVar_Result = FALSE;
    if (mon == NULL || PlannedEVTotal() > MAX_TOTAL_EVS)
        return;
    for (u32 i = 0; i < NUM_STATS; i++)
    {
        u32 value = sPlannedEVs[i];
        SetMonData(mon, sStatData[i], &value);
    }
    CalculateMonStats(mon);
    gSpecialVar_Result = TRUE;
}

// The native menu permits low IVs as well as maximum IVs.
void ChangeChosenMonIVs(void)
{
    struct Pokemon *mon = GetServiceMon(gSpecialVar_0x8004);
    u32 stat = gSpecialVar_0x8005;
    u32 value = gSpecialVar_0x8006;

    if (mon == NULL || stat >= NUM_STATS || value > MAX_PER_STAT_IVS)
        return;
    SetMonData(mon, sIvData[stat], &value);
    CalculateMonStats(mon);
}

// Shared by the preview and transaction: minimize Attack, then retain as
// many maximum remaining IVs as possible, including Speed.
static const u8 sHiddenPowerSpreads[16][NUM_STATS] = {
        {31, 0, 30, 30, 30, 30}, // Fighting
        {31, 0, 31, 30, 30, 30}, // Flying
        {31, 0, 30, 31, 30, 30}, // Poison
        {31, 0, 31, 31, 30, 30}, // Ground
        {31, 0, 30, 30, 31, 30}, // Rock
        {31, 0, 30, 31, 31, 30}, // Bug
        {31, 0, 31, 31, 31, 30}, // Ghost
        {31, 0, 30, 30, 30, 31}, // Steel
        {31, 0, 31, 30, 30, 31}, // Fire
        {31, 0, 30, 31, 30, 31}, // Water
        {31, 0, 31, 31, 30, 31}, // Grass
        {31, 0, 30, 30, 31, 31}, // Electric
        {31, 0, 31, 30, 31, 31}, // Psychic
        {31, 0, 30, 31, 31, 31}, // Ice
        {31, 0, 31, 31, 31, 31}, // Dragon
        {31, 1, 31, 31, 31, 31}, // Dark
};

static EWRAM_DATA u8 sHiddenPowerPreviewWindow = 0; // Window id + 1; zero means closed.

// VAR_0x8004 party slot, VAR_0x8005 type index. Buffers only; no payment or
// mutation, even for a proposed spread that is already the current spread.
void BufferChosenMonHiddenPowerPreview(void)
{
    struct Pokemon *mon = GetServiceMon(gSpecialVar_0x8004);
    u32 type = gSpecialVar_0x8005;
    u8 *dst = gStringVar4;

    gSpecialVar_Result = FALSE;
    gStringVar1[0] = gStringVar4[0] = EOS;
    if (mon == NULL || type >= ARRAY_COUNT(sHiddenPowerSpreads))
        return;
    StringCopy(gStringVar1, gTypesInfo[type < 8 ? type + TYPE_FIGHTING : type + TYPE_FIRE - 8].name);
    for (u32 i = 0; i < NUM_STATS; i++)
    {
        dst = StringCopy(dst, sPlannedEVStatNames[i]);
        *dst++ = EXT_CTRL_CODE_BEGIN;
        *dst++ = EXT_CTRL_CODE_CLEAR_TO;
        *dst++ = 58;
        dst = ConvertIntToDecimalStringN(dst, GetMonData(mon, sIvData[i]), STR_CONV_MODE_LEFT_ALIGN, 2);
        *dst++ = EXT_CTRL_CODE_BEGIN;
        *dst++ = EXT_CTRL_CODE_CLEAR_TO;
        *dst++ = 76;
        *dst++ = CHAR_RIGHT_ARROW;
        *dst++ = EXT_CTRL_CODE_BEGIN;
        *dst++ = EXT_CTRL_CODE_CLEAR_TO;
        *dst++ = 94;
        dst = ConvertIntToDecimalStringN(dst, sHiddenPowerSpreads[type][i], STR_CONV_MODE_LEFT_ALIGN, 2);
        *dst++ = i == NUM_STATS - 1 ? EOS : CHAR_NEWLINE;
    }
    gSpecialVar_Result = TRUE;
}

void HideChosenMonHiddenPowerPreview(void)
{
    if (sHiddenPowerPreviewWindow == 0)
        return;
    ClearStdWindowAndFrameToTransparent(sHiddenPowerPreviewWindow - 1, TRUE);
    RemoveWindow(sHiddenPowerPreviewWindow - 1);
    sHiddenPowerPreviewWindow = 0;
}

void ShowChosenMonHiddenPowerPreview(void)
{
    // Tiles 100..286 leave the Yes/No window (0x125) and dialogue (0x194)
    // free. All six rows remain visible while the player confirms below.
    struct WindowTemplate template = CreateWindowTemplate(0, 1, 1, 17, 11, 15, 100);
    u8 title[40];
    u8 window;

    HideChosenMonHiddenPowerPreview();
    BufferChosenMonHiddenPowerPreview();
    if (!gSpecialVar_Result)
        return;
    window = AddWindow(&template);
    if (window == WINDOW_NONE)
    {
        gSpecialVar_Result = FALSE;
        return;
    }
    sHiddenPowerPreviewWindow = window + 1;
    StringCopy(title, COMPOUND_STRING("Hidden Power: "));
    StringAppend(title, gStringVar1);
    SetStandardWindowBorderStyle(window, FALSE);
    AddTextPrinterParameterized(window, FONT_SMALL, title, 0, 0, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(window, FONT_SMALL, gStringVar4, 0, 14, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(window, COPYWIN_FULL);
}

// VAR_0x8004 is the party slot and VAR_0x8005 the Hidden Power type index.
void ChangeChosenMonHiddenPower(void)
{
    u32 slot = gSpecialVar_0x8004;
    u32 type = gSpecialVar_0x8005;

    struct Pokemon *mon = GetServiceMon(slot);
    if (mon == NULL || type >= ARRAY_COUNT(sHiddenPowerSpreads))
        return;
    for (u32 i = 0; i < NUM_STATS; i++)
        SetMonData(mon, sIvData[i], &sHiddenPowerSpreads[type][i]);
    CalculateMonStats(mon);
}

// Reuse the saved daily seed so visits and reloads keep the same request.
// All five ingredients grow on ripe trees on Routes 102, 103, 104 or 116
// before Slateport. The flower shop's random gift alone is not enough to
// guarantee every basic Berry has been available on the first visit.
u16 GetNatureChangerBerry(void)
{
    static const enum Item ingredients[] = {
        ITEM_CHERI_BERRY, ITEM_CHESTO_BERRY, ITEM_PECHA_BERRY,
        ITEM_LEPPA_BERRY, ITEM_ORAN_BERRY,
    };
    DoTimeBasedEvents();
    rng_value_t rng = LocalRandomSeed(gSaveBlock1Ptr->dailySeed ^ 0x4E415455);
    return ingredients[LocalRandom32(&rng) % ARRAY_COUNT(ingredients)];
}

// VAR_0x8005 is the raised stat and VAR_0x8006 the lowered one, in Nature
// order (Attack, Defense, Speed, Sp. Atk, Sp. Def). VAR_RESULT is FALSE when
// nothing would change (the same Nature, or one neutral Nature for another),
// so the player keeps the Berries.
void ChangePokemonNature(void)
{
    struct Pokemon *mon = GetServiceMon(gSpecialVar_0x8004);
    u32 raisedStat = gSpecialVar_0x8005;
    u32 loweredStat = gSpecialVar_0x8006;

    gSpecialVar_Result = FALSE;
    if (mon == NULL || raisedStat >= NUM_STATS - 1 || loweredStat >= NUM_STATS - 1)
        return;
    u32 nature = raisedStat * (NUM_STATS - 1) + loweredStat;
    u32 current = GetMonData(mon, MON_DATA_HIDDEN_NATURE);
    if (nature == current
     || (raisedStat == loweredStat && gNaturesInfo[current].statUp == gNaturesInfo[current].statDown))
        return;
    // Preserve personality; this is the effective nature used for stats.
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
    CalculateMonStats(mon);
    gSpecialVar_Result = TRUE;
}

void BufferVarsForIVRater(void)
{
    struct Pokemon *mon = GetServiceMon(gSpecialVar_0x8004);
    u32 best = 0, bestStat = 0, total = 0;

    gSpecialVar_0x8005 = gSpecialVar_0x8006 = gSpecialVar_0x8007 = 0;
    if (mon == NULL)
        return;
    for (u32 i = 0; i < NUM_STATS; i++)
    {
        u32 iv = GetMonData(mon, sIvData[i]);

        total += iv;
        if (iv > best)
        {
            best = iv;
            bestStat = i;
        }
    }
    gSpecialVar_0x8005 = total;
    gSpecialVar_0x8006 = bestStat;
    gSpecialVar_0x8007 = best;
}

// VAR_0x8004: one cap, EC_SOOT_CAP_BATCH, or EC_SOOT_EXCHANGE_ALL.
// No debit until delivery succeeds.
void ExchangeSootForCaps(void)
{
    u32 soot = VarGet(VAR_ASH_GATHER_COUNT);
    u32 count = gSpecialVar_0x8004;
    gSpecialVar_Result = EC_SOOT_EXCHANGE_REFUSED;
    if (count == EC_SOOT_EXCHANGE_ALL)
        count = soot / EC_SOOT_PER_CAP;
    else if (count != 1 && count != EC_SOOT_CAP_BATCH)
        return;
    if (count == 0 || count * EC_SOOT_PER_CAP > soot)
        return;
    if (!AddBagItem(ITEM_BOTTLE_CAP, count))
    {
        gSpecialVar_Result = EC_SOOT_EXCHANGE_BAG_FULL;
        return;
    }
    VarSet(VAR_ASH_GATHER_COUNT, soot - count * EC_SOOT_PER_CAP);
    ConvertIntToDecimalStringN(gStringVar1, count, STR_CONV_MODE_LEFT_ALIGN, 2);
    gSpecialVar_Result = EC_SOOT_EXCHANGE_DELIVERED;
}

// These script transactions charge only for changed IVs. Failed delivery
// restores the complete mon, including calculated stats and current HP.
static void PayForIvyService(bool32 hiddenPower)
{
    struct Pokemon *mon = GetServiceMon(gSpecialVar_0x8004);
    gSpecialVar_Result = EC_IV_SERVICE_UNCHANGED;
    if (mon == NULL)
        return;
    struct Pokemon before = *mon;
    if (hiddenPower)
        ChangeChosenMonHiddenPower();
    else
    {
        if (gSpecialVar_0x8005 != STAT_ATK && gSpecialVar_0x8005 != STAT_SPEED)
            return;
        ChangeChosenMonIVs();
    }
    bool32 changed = FALSE;
    for (u32 i = 0; i < NUM_STATS; i++)
        changed |= GetMonData(mon, sIvData[i]) != GetMonData(&before, sIvData[i]);
    if (!changed)
        return;
    if (!RemoveBagItem(ITEM_BOTTLE_CAP, hiddenPower ? EC_HIDDEN_POWER_CAP_COST : EC_IV_CHANGE_CAP_COST))
    {
        *mon = before;
        gSpecialVar_Result = EC_IV_SERVICE_NOT_ENOUGH_CAPS;
        return;
    }
    gSpecialVar_Result = EC_IV_SERVICE_CHANGED;
}

void PayForChosenMonIVChange(void)
{
    PayForIvyService(FALSE);
}

void PayForChosenMonHiddenPower(void)
{
    PayForIvyService(TRUE);
}
