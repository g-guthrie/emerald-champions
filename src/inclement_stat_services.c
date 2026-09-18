#include "global.h"
#include "event_data.h"
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

static struct Pokemon *ChosenMon(void)
{
    u32 slot = gSpecialVar_0x8004;

    if (slot >= PARTY_SIZE)
        slot = 0;
    return &gParties[B_TRAINER_PLAYER][slot];
}

static u32 TotalEVs(struct Pokemon *mon)
{
    u32 total = 0;

    for (u32 i = 0; i < NUM_STATS; i++)
        total += GetMonData(mon, sStatData[i]);
    return total;
}

void BufferChosenMonAllEVs(void)
{
    struct Pokemon *mon = ChosenMon();
    u8 *dst = gStringVar4;

    for (u32 i = 0; i < NUM_STATS; i++)
    {
        dst = ConvertIntToDecimalStringN(dst, GetMonData(mon, sStatData[i]), STR_CONV_MODE_LEFT_ALIGN, 3);
        *dst++ = (i == NUM_STATS - 1) ? EOS : CHAR_SLASH;
    }
    *dst = EOS;
}

void BufferChosenMonAllIVs(void)
{
    struct Pokemon *mon = ChosenMon();
    u8 *dst = gStringVar4;

    for (u32 i = 0; i < NUM_STATS; i++)
    {
        dst = ConvertIntToDecimalStringN(dst, GetMonData(mon, sIvData[i]), STR_CONV_MODE_LEFT_ALIGN, 2);
        *dst++ = (i == NUM_STATS - 1) ? EOS : CHAR_SLASH;
    }
    *dst = EOS;
}

void BufferChosenMonEV(void)
{
    u32 stat = gSpecialVar_0x8005;

    if (stat >= NUM_STATS)
        stat = 0;
    ConvertIntToDecimalStringN(gStringVar2, GetMonData(ChosenMon(), sStatData[stat]), STR_CONV_MODE_LEFT_ALIGN, 3);
}

void BufferChosenMonIV(void)
{
    u32 stat = gSpecialVar_0x8005;

    if (stat >= NUM_STATS)
        stat = 0;
    ConvertIntToDecimalStringN(gStringVar2, GetMonData(ChosenMon(), sIvData[stat]), STR_CONV_MODE_LEFT_ALIGN, 2);
}

void BufferChosenMonNature(void)
{
    StringCopy(gStringVar2, gNaturesInfo[GetNature(ChosenMon())].name);
}

// TRUE while the spread still has room for the requested amount.
void CheckChosenMonCanGainEVs(void)
{
    struct Pokemon *mon = ChosenMon();
    u32 stat = gSpecialVar_0x8005 < NUM_STATS ? gSpecialVar_0x8005 : 0;
    u32 want = gSpecialVar_0x8006;
    u32 current = GetMonData(mon, sStatData[stat]);

    gSpecialVar_Result = (current + want <= MAX_PER_STAT_EVS
                       && TotalEVs(mon) + want <= MAX_TOTAL_EVS);
}

void AreChosenMonEVsMaxedOut(void)
{
    gSpecialVar_Result = (TotalEVs(ChosenMon()) >= MAX_TOTAL_EVS);
}

void Special_AreLeadMonEVsMaxedOut(void)
{
    gSpecialVar_0x8004 = 0;
    AreChosenMonEVsMaxedOut();
}

void IncreaseChosenMonEVs(void)
{
    struct Pokemon *mon = ChosenMon();
    u32 stat = gSpecialVar_0x8005 < NUM_STATS ? gSpecialVar_0x8005 : 0;
    u32 want = gSpecialVar_0x8006;
    u32 current = GetMonData(mon, sStatData[stat]);
    u32 room = MAX_TOTAL_EVS - TotalEVs(mon);
    u32 gained;

    if (want > MAX_PER_STAT_EVS - current)
        want = MAX_PER_STAT_EVS - current;
    if (want > room)
        want = room;
    gained = current + want;
    SetMonData(mon, sStatData[stat], &gained);
    CalculateMonStats(mon);
    gSpecialVar_0x8007 = want;
    gSpecialVar_Result = (want != 0);
}

void ResetChosenMonEVs(void)
{
    struct Pokemon *mon = ChosenMon();
    u32 zero = 0;

    for (u32 i = 0; i < NUM_STATS; i++)
        SetMonData(mon, sStatData[i], &zero);
    CalculateMonStats(mon);
}

// Hyper Training: one stat, or every stat, taken to its ceiling.
void ChangeChosenMonIVs(void)
{
    struct Pokemon *mon = ChosenMon();
    u32 stat = gSpecialVar_0x8005;
    u32 best = MAX_PER_STAT_IVS;

    if (stat >= NUM_STATS)
    {
        for (u32 i = 0; i < NUM_STATS; i++)
            SetMonData(mon, sIvData[i], &best);
    }
    else
    {
        SetMonData(mon, sIvData[stat], &best);
    }
    CalculateMonStats(mon);
}

// Hidden Power's type follows from the IV parities, so the chosen type is
// reached by nudging each IV's low bit rather than by storing a type.
void ChangeChosenMonHiddenPower(void)
{
    static const u8 sOrder[NUM_STATS] = {0, 1, 2, 3, 4, 5};
    struct Pokemon *mon = ChosenMon();
    u32 wanted = gSpecialVar_0x8005;
    u32 bits = (wanted * 63) / (NUMBER_OF_MON_TYPES - 3);

    for (u32 i = 0; i < NUM_STATS; i++)
    {
        u32 iv = GetMonData(mon, sIvData[sOrder[i]]);

        iv = (iv & ~1u) | ((bits >> i) & 1u);
        SetMonData(mon, sIvData[sOrder[i]], &iv);
    }
    CalculateMonStats(mon);
}

void ChangePokemonNature(void)
{
    struct Pokemon *mon = ChosenMon();
    u32 nature = gSpecialVar_0x8006;

    if (nature >= NUM_NATURES)
        nature = 0;
    // This engine keeps the visible nature separate from personality, the same
    // field the Champions tutor writes, so the change survives without
    // rerolling the Pokemon.
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
    CalculateMonStats(mon);
}

void BufferVarsForIVRater(void)
{
    struct Pokemon *mon = ChosenMon();
    u32 best = 0, bestStat = 0, total = 0;

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
    gSpecialVar_0x8005 = bestStat;
    gSpecialVar_0x8006 = best;
    gSpecialVar_0x8007 = total;
}
