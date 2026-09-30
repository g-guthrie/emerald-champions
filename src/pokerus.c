#include "global.h"
#include "battle_message.h"
#include "event_data.h"
#include "config_changes.h"
#include "pokemon.h"
#include "data.h"
#include "pokerus.h"
#include "random.h"
#include "config/pokerus.h"
#include "script.h"
#include "string_util.h"
#include "strings.h"
#include "constants/characters.h"

// Reuse the native saved byte. FB is hot-spring recovery;
// FC/FD/FE mean zero/one/two transmissions;
// these low-nibble values cannot occur in a normal 1-4 day infection.
// Older infections retain their benefit but receive no new transmissions.
#define LIMITED_POKERUS 0xFC
#define HOT_SPRING_POKERUS 0xFB
#define SOAKED_POKERUS 0xF8 // F8/F9/FA preserve zero/one/two transmissions.

u32 GetPokerusSpreadsLeft(struct Pokemon *mon)
{
    u32 value = GetMonData(mon, MON_DATA_POKERUS);
    if (!GetConfig(POKERUS_ENABLED)
     || ((value & 0xFC) != LIMITED_POKERUS && !IsMonReadyForHotSpringTreatment(mon)))
        return 0;
    return min(value & 3, 2);
}

bool32 CheckMonHasHadPokerus(struct Pokemon *mon)
{
    return GetConfig(POKERUS_ENABLED) && GetMonData(mon, MON_DATA_POKERUS) != 0;
}

bool32 HasHotSpringPokerus(struct Pokemon *mon)
{
    return GetConfig(POKERUS_ENABLED) && GetMonData(mon, MON_DATA_POKERUS) == HOT_SPRING_POKERUS;
}

bool32 IsMonReadyForHotSpringTreatment(struct Pokemon *mon)
{
    u32 value = GetMonData(mon, MON_DATA_POKERUS);
    return CheckMonHasHadPokerus(mon) && value >= SOAKED_POKERUS && value < HOT_SPRING_POKERUS;
}

bool32 PreparePartyPokerusInHotSpring(void)
{
    bool32 changed = FALSE;
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        if (!GetMonData(mon, MON_DATA_SPECIES) || GetMonData(mon, MON_DATA_IS_EGG)
         || IsMonTrainerOwned(mon) || !CheckMonHasHadPokerus(mon)
         || HasHotSpringPokerus(mon) || IsMonReadyForHotSpringTreatment(mon))
            continue;
        u32 value = SOAKED_POKERUS | GetPokerusSpreadsLeft(mon);
        SetMonData(mon, MON_DATA_POKERUS, &value);
        changed = TRUE;
    }
    return changed;
}

bool32 RecoverMonPokerusInHotSpring(struct Pokemon *mon)
{
    if (!GetMonData(mon, MON_DATA_SPECIES) || GetMonData(mon, MON_DATA_IS_EGG)
     || IsMonTrainerOwned(mon) || !IsMonReadyForHotSpringTreatment(mon))
        return FALSE;
    u32 value = HOT_SPRING_POKERUS;
    SetMonData(mon, MON_DATA_POKERUS, &value);
    CalculateMonStats(mon);
    return TRUE;
}

void BufferHotSpringPokerusPreview(void)
{
    gSpecialVar_Result = FALSE;
    gStringVar4[0] = EOS;
    if (gSpecialVar_0x8004 >= gPartiesCount[B_TRAINER_PLAYER])
        return;
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][gSpecialVar_0x8004];
    struct Pokemon preview = *mon;
    if (!RecoverMonPokerusInHotSpring(&preview))
        return;
    u32 nature = GetMonData(mon, MON_DATA_HIDDEN_NATURE);
    u8 *dst = gStringVar4;
    if (gNaturesInfo[nature].statUp == gNaturesInfo[nature].statDown)
        dst = StringCopy(dst, COMPOUND_STRING("Neutral Nature: stats stay the same.\p"));
    else
    {
        for (u32 i = 0; i < 2; i++)
        {
            u32 stat = i == 0 ? gNaturesInfo[nature].statUp : gNaturesInfo[nature].statDown;
            dst = StringCopy(dst, gStatNamesTable[stat]);
            dst = StringCopy(dst, COMPOUND_STRING(": "));
            dst = ConvertIntToDecimalStringN(dst, GetMonData(mon, MON_DATA_MAX_HP + stat), STR_CONV_MODE_LEFT_ALIGN, 3);
            dst = StringCopy(dst, COMPOUND_STRING(" > "));
            dst = ConvertIntToDecimalStringN(dst, GetMonData(&preview, MON_DATA_MAX_HP + stat), STR_CONV_MODE_LEFT_ALIGN, 3);
            *dst++ = i == 0 ? CHAR_NEWLINE : CHAR_PROMPT_CLEAR;
            *dst = EOS;
        }
    }
    StringCopy(dst, COMPOUND_STRING("Treatment ends spreading. Its bonus\nfollows future Nature changes.\pApply this permanent treatment?"));
    gSpecialVar_Result = TRUE;
}

void ApplyHotSpringPokerusTreatment(void)
{
    gSpecialVar_Result = gSpecialVar_0x8004 < gPartiesCount[B_TRAINER_PLAYER]
        && RecoverMonPokerusInHotSpring(&gParties[B_TRAINER_PLAYER][gSpecialVar_0x8004]);
}

bool32 IsPokerusNatureBoosted(struct Pokemon *mon, u32 stat)
{
    u32 nature = GetMonData(mon, MON_DATA_HIDDEN_NATURE);
    return stat > STAT_HP && stat < NUM_STATS && !IsMonTrainerOwned(mon)
        && CheckMonHasHadPokerus(mon)
        && gNaturesInfo[nature].statUp != gNaturesInfo[nature].statDown
        && (stat == gNaturesInfo[nature].statUp
         || (HasHotSpringPokerus(mon) && stat == gNaturesInfo[nature].statDown));
}

u32 GetPokerusNatureModifier(struct Pokemon *mon, u32 stat)
{
    if (!IsPokerusNatureBoosted(mon, stat))
        return 100;
    u32 nature = GetMonData(mon, MON_DATA_HIDDEN_NATURE);
    // +5 percentage points on the favored stat; spring recovery adds
    // 15 points to the usual -10%, leaving the other stat at +5%.
    return stat == gNaturesInfo[nature].statUp ? 115 : 105;
}

void GiveMonPokerus(struct Pokemon *mon, bool32 canSpread)
{
    if (!GetConfig(POKERUS_ENABLED) || !GetMonData(mon, MON_DATA_SPECIES)
     || GetMonData(mon, MON_DATA_IS_EGG) || CheckMonHasHadPokerus(mon))
        return;
    u32 value = LIMITED_POKERUS | (canSpread ? 2 : 0);
    SetMonData(mon, MON_DATA_POKERUS, &value);
    CalculateMonStats(mon);
}

void RandomlyGivePartyPokerus(void)
{
    if (!GetConfig(POKERUS_ENABLED) || IsPokerusInParty())
        return;
    if (P_POKERUS_FLAG_INFECTION && !FlagGet(P_POKERUS_FLAG_INFECTION))
        return;
    if (RandomUniform(RNG_POKERUS_INFECTION, 0, MAX_u16) >= P_POKERUS_INFECTION_ODDS)
        return;
    u8 targets[PARTY_SIZE];
    u32 count = 0;
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        if (GetMonData(mon, MON_DATA_SPECIES) && !GetMonData(mon, MON_DATA_IS_EGG)
         && !CheckMonHasHadPokerus(mon))
            targets[count++] = i;
    }
    if (count)
        GiveMonPokerus(&gParties[B_TRAINER_PLAYER][targets[RandomUniform(RNG_POKERUS_PARTY_MEMBER, 0, count - 1)]], TRUE);
}

bool32 CheckMonPokerus(struct Pokemon *mon)
{
    return GetPokerusSpreadsLeft(mon) != 0;
}

bool32 IsPokerusInParty(void)
{
    for (u32 i = 0; i < PARTY_SIZE; i++)
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES)
         && CheckMonPokerus(&gParties[B_TRAINER_PLAYER][i]))
            return TRUE;
    return FALSE;
}

bool32 ShouldPokemonShowActivePokerus(struct Pokemon *mon)
{
    return !GetMonData(mon, MON_DATA_IS_EGG) && CheckMonPokerus(mon);
}

bool32 ShouldPokemonShowCuredPokerus(struct Pokemon *mon)
{
    return !GetMonData(mon, MON_DATA_IS_EGG) && CheckMonHasHadPokerus(mon) && !CheckMonPokerus(mon);
}

static bool32 CanReceivePokerus(struct Pokemon *mon)
{
    return GetMonData(mon, MON_DATA_SPECIES) && !GetMonData(mon, MON_DATA_IS_EGG)
        && !CheckMonHasHadPokerus(mon);
}

void PartySpreadPokerus(void)
{
    if (!GetConfig(POKERUS_ENABLED)
     || RandomUniform(RNG_POKERUS_SPREAD, 0, MAX_u16) >= P_POKERUS_SPREAD_ODDS)
        return;
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        struct Pokemon *source = &gParties[B_TRAINER_PLAYER][i];
        u32 left = GetPokerusSpreadsLeft(source);
        if (!GetMonData(source, MON_DATA_SPECIES) || GetMonData(source, MON_DATA_IS_EGG) || !left)
            continue;
        u8 targets[2];
        u32 count = 0;
        if (i > 0 && CanReceivePokerus(&gParties[B_TRAINER_PLAYER][i - 1]))
            targets[count++] = i - 1;
        if (i + 1 < PARTY_SIZE && CanReceivePokerus(&gParties[B_TRAINER_PLAYER][i + 1]))
            targets[count++] = i + 1;
        if (count)
        {
            u32 target = targets[RandomUniform(RNG_POKERUS_SPREAD_SIDE, 0, count - 1)];
            GiveMonPokerus(&gParties[B_TRAINER_PLAYER][target], FALSE);
            u32 value = (IsMonReadyForHotSpringTreatment(source) ? SOAKED_POKERUS : LIMITED_POKERUS) | (left - 1);
            SetMonData(source, MON_DATA_POKERUS, &value);
            return; // At most one recipient per battle; recipients never spread.
        }
    }
}
