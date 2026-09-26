#include "global.h"
#include "event_data.h"
#include "field_specials.h"
#include "international_string_util.h"
#include "pokemon.h"
#include "string_util.h"
#include "strings.h"
#include "text.h"
#include "caps.h"
#include "constants/field_specials.h"
#include "constants/items.h"

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

void BufferChosenMonNature(void)
{
    struct Pokemon *mon = GetServiceMon(gSpecialVar_0x8004);
    gStringVar2[0] = EOS;
    if (mon != NULL)
        StringCopy(gStringVar2, gNaturesInfo[GetMonData(mon, MON_DATA_HIDDEN_NATURE)].name);
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

// One editor row: in VAR_0x8005 stat; out STR_VAR_2 its EVs now and
// STR_VAR_3 its planned EVs.
void BufferPlannedEVRow(void)
{
    struct Pokemon *mon = GetPlannedEVsMon();
    u32 stat = min(gSpecialVar_0x8005, NUM_STATS - 1);

    ConvertIntToDecimalStringN(gStringVar2, mon != NULL ? GetMonData(mon, sStatData[stat]) : 0, STR_CONV_MODE_LEFT_ALIGN, 3);
    ConvertIntToDecimalStringN(gStringVar3, sPlannedEVs[stat], STR_CONV_MODE_LEFT_ALIGN, 3);
}

// Out STR_VAR_3 the planned total.
void BufferPlannedEVTotal(void)
{
    ConvertIntToDecimalStringN(gStringVar3, PlannedEVTotal(), STR_CONV_MODE_LEFT_ALIGN, 3);
}

// The step menu's prompt for the stat in VAR_0x8005, in gStringVar4.
void BufferPlannedEVStatPrompt(void)
{
    struct Pokemon *mon = GetPlannedEVsMon();
    u32 stat = min(gSpecialVar_0x8005, NUM_STATS - 1);
    u8 *dst = StringCopy(gStringVar4, sPlannedEVStatNames[stat]);

    dst = StringCopy(dst, COMPOUND_STRING(": now "));
    dst = ConvertIntToDecimalStringN(dst, mon != NULL ? GetMonData(mon, sStatData[stat]) : 0, STR_CONV_MODE_LEFT_ALIGN, 3);
    dst = StringCopy(dst, COMPOUND_STRING(", planned "));
    dst = ConvertIntToDecimalStringN(dst, sPlannedEVs[stat], STR_CONV_MODE_LEFT_ALIGN, 3);
    dst = StringCopy(dst, COMPOUND_STRING(".\nPlanned total: "));
    dst = ConvertIntToDecimalStringN(dst, PlannedEVTotal(), STR_CONV_MODE_LEFT_ALIGN, 3);
    StringCopy(dst, COMPOUND_STRING(" of 510."));
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

// VAR_0x8004 is the party slot and VAR_0x8005 the Hidden Power type index.
// Minimize Attack, then retain as many maximum remaining IVs as possible.
void ChangeChosenMonHiddenPower(void)
{
    static const u8 spreads[16][NUM_STATS] = {
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
    u32 slot = gSpecialVar_0x8004;
    u32 type = gSpecialVar_0x8005;

    struct Pokemon *mon = GetServiceMon(slot);
    if (mon == NULL || type >= ARRAY_COUNT(spreads))
        return;
    for (u32 i = 0; i < NUM_STATS; i++)
        SetMonData(mon, sIvData[i], &spreads[type][i]);
    CalculateMonStats(mon);
}

void ChangePokemonNature(void)
{
    struct Pokemon *mon = GetServiceMon(gSpecialVar_0x8004);
    u32 raisedStat = gSpecialVar_0x8005;
    u32 loweredStat = gSpecialVar_0x8006;

    if (mon == NULL || raisedStat >= NUM_STATS - 1 || loweredStat >= NUM_STATS - 1)
        return;
    u32 nature = raisedStat * (NUM_STATS - 1) + loweredStat;
    // Preserve personality; this is the effective nature used for stats.
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
    CalculateMonStats(mon);
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
