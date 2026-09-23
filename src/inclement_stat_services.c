#include "global.h"
#include "event_data.h"
#include "field_specials.h"
#include "international_string_util.h"
#include "pokemon.h"
#include "string_util.h"
#include "strings.h"
#include "text.h"
#include "caps.h"
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

// TRUE while the spread still has room for the requested amount.
void CheckChosenMonCanGainEVs(void)
{
    struct Pokemon *mon = GetServiceMon(gSpecialVar_0x8004);
    u32 stat = gSpecialVar_0x8005;
    u32 want = gSpecialVar_0x8006;
    gSpecialVar_0x8008 = 0;
    gSpecialVar_Result = FALSE;
    if (mon == NULL || stat >= NUM_STATS)
        return;
    u32 current = GetMonData(mon, sStatData[stat]);
    gSpecialVar_0x8008 = TotalEVs(mon);
    gSpecialVar_Result = (current + want <= MAX_PER_STAT_EVS
                       && gSpecialVar_0x8008 + want <= MAX_TOTAL_EVS);
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

// The native scroll menu saves the party slot in 800A and the type in 8007.
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
    u32 slot = gSpecialVar_0x800A;
    u32 type = gSpecialVar_0x8007;

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
