#include "global.h"
#include "event_data.h"
#include "config_changes.h"
#include "pokemon.h"
#include "data.h"
#include "pokerus.h"
#include "random.h"
#include "config/pokerus.h"

// Reuse the native saved byte. FC/FD/FE mean zero/one/two transmissions;
// these low-nibble values cannot occur in a normal 1-4 day infection.
// Older infections retain their benefit but receive no new transmissions.
#define LIMITED_POKERUS 0xFC

u32 GetPokerusSpreadsLeft(struct Pokemon *mon)
{
    u32 value = GetMonData(mon, MON_DATA_POKERUS);
    if (!GetConfig(POKERUS_ENABLED) || (value & 0xFC) != LIMITED_POKERUS)
        return 0;
    return min(value & 3, 2);
}

bool32 CheckMonHasHadPokerus(struct Pokemon *mon)
{
    return GetConfig(POKERUS_ENABLED) && GetMonData(mon, MON_DATA_POKERUS) != 0;
}

bool32 IsPokerusNatureBoosted(struct Pokemon *mon, u32 stat)
{
    u32 nature = GetMonData(mon, MON_DATA_HIDDEN_NATURE);
    return stat > STAT_HP && stat < NUM_STATS && !IsMonTrainerOwned(mon)
        && CheckMonHasHadPokerus(mon)
        && gNaturesInfo[nature].statUp != gNaturesInfo[nature].statDown
        && stat == gNaturesInfo[nature].statUp;
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
            u32 value = LIMITED_POKERUS | (left - 1);
            SetMonData(source, MON_DATA_POKERUS, &value);
            return; // At most one recipient per battle; recipients never spread.
        }
    }
}
