#include "global.h"

#if EC_HEADLESS_FIXTURES

#include "caps.h"
#include "emerald_champions_agent_prep.h"
#include "emerald_champions_battle_sets.h"
#include "main.h"
#include "load_save.h"
#include "pokemon.h"
#include "save.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/species.h"

EWRAM_DATA volatile u32 gEcAgentPrepCommand = 0;
EWRAM_DATA volatile u32 gEcAgentPrepResult = EC_AGENT_PREP_PENDING;
EWRAM_DATA volatile u32 gEcAgentPrepErrorSlot = 0;
EWRAM_DATA volatile u32 gEcAgentPrepPartyCount = 0;
EWRAM_DATA volatile u32 gEcAgentPrepSpecies[EC_AGENT_PREP_PARTY_SIZE] = {0};
EWRAM_DATA volatile u32 gEcAgentPrepPreset[EC_AGENT_PREP_PARTY_SIZE] = {0};
EWRAM_DATA volatile u32 gEcAgentPrepFormat[EC_AGENT_PREP_PARTY_SIZE] = {0};
EWRAM_DATA volatile u32 gEcAgentPrepLevel[EC_AGENT_PREP_PARTY_SIZE] = {0};
EWRAM_DATA volatile u32 gEcAgentPrepMoves[EC_AGENT_PREP_PARTY_SIZE][EC_AGENT_PREP_MOVE_COUNT] = {{0}};
EWRAM_DATA volatile u32 gEcAgentPrepNature[EC_AGENT_PREP_PARTY_SIZE] = {0};
EWRAM_DATA volatile u32 gEcAgentPrepAbility[EC_AGENT_PREP_PARTY_SIZE] = {0};
EWRAM_DATA volatile u32 gEcAgentPrepItem[EC_AGENT_PREP_PARTY_SIZE] = {0};
EWRAM_DATA volatile u32 gEcAgentPrepStatPoints[EC_AGENT_PREP_PARTY_SIZE][EC_AGENT_PREP_STAT_COUNT] = {{0}};

static EWRAM_DATA struct Pokemon sEcAgentPreparedParty[PARTY_SIZE] = {0};

static void Fail(enum EmeraldChampionsAgentPrepResult result, u32 slot)
{
    gEcAgentPrepResult = result;
    gEcAgentPrepErrorSlot = slot;
    gEcAgentPrepCommand = 0;
}

static bool32 FindAbility(enum Species species, enum Ability ability, u8 *slotOut)
{
    for (u32 slot = 0; slot < NUM_ABILITY_SLOTS; slot++)
    {
        if (GetSpeciesAbility(species, slot) == ability)
        {
            *slotOut = slot;
            return TRUE;
        }
    }
    return FALSE;
}

static bool32 ApplyOverrides(struct Pokemon *mon, u32 slot)
{
    u8 perfectIv = MAX_PER_STAT_IVS;
    u8 ppBonuses = 0;
    u32 total = 0;

    for (u32 moveSlot = 0; moveSlot < MAX_MON_MOVES; moveSlot++)
    {
        u32 move = gEcAgentPrepMoves[slot][moveSlot];
        if (move == EC_AGENT_PREP_KEEP)
            continue;
        if (move >= MOVES_COUNT)
            return FALSE;
        SetMonMoveSlot(mon, move, moveSlot);
    }
    if (gEcAgentPrepNature[slot] != EC_AGENT_PREP_KEEP)
    {
        u8 nature = gEcAgentPrepNature[slot];
        if (nature >= NUM_NATURES)
            return FALSE;
        SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
    }
    if (gEcAgentPrepAbility[slot] != EC_AGENT_PREP_KEEP)
    {
        u8 abilitySlot;
        if (!FindAbility(GetMonData(mon, MON_DATA_SPECIES), gEcAgentPrepAbility[slot], &abilitySlot))
            return FALSE;
        SetMonData(mon, MON_DATA_ABILITY_NUM, &abilitySlot);
    }
    if (gEcAgentPrepItem[slot] != EC_AGENT_PREP_KEEP)
    {
        u16 item = gEcAgentPrepItem[slot];
        if (item >= ITEMS_COUNT)
            return FALSE;
        SetMonData(mon, MON_DATA_HELD_ITEM, &item);
    }
    for (u32 stat = 0; stat < NUM_STATS; stat++)
    {
        u32 points = gEcAgentPrepStatPoints[slot][stat];
        if (points == EC_AGENT_PREP_KEEP)
            continue;
        if (points > EC_STAT_POINTS_PER_STAT)
            return FALSE;
        total += points;
    }
    if (gEcAgentPrepStatPoints[slot][0] != EC_AGENT_PREP_KEEP)
    {
        if (total != EC_STAT_POINT_BUDGET)
            return FALSE;
        for (u32 stat = 0; stat < NUM_STATS; stat++)
        {
            u8 points = gEcAgentPrepStatPoints[slot][stat];
            SetMonData(mon, EC_STAT_POINT_DATA(stat), &points);
        }
    }
    SetMonData(mon, MON_DATA_PP_BONUSES, &ppBonuses);
    for (u32 stat = 0; stat < NUM_STATS; stat++)
        SetMonData(mon, MON_DATA_HP_IV + stat, &perfectIv);
    CalculateMonStats(mon);
    return TRUE;
}

void EmeraldChampionsAgentPrepPoll(void)
{
    if (gEcAgentPrepCommand == 0)
        return;
    if (gEcAgentPrepCommand != 1)
    {
        Fail(EC_AGENT_PREP_BAD_COMMAND, 0);
        return;
    }
    if (gMain.inBattle)
    {
        Fail(EC_AGENT_PREP_IN_BATTLE, 0);
        return;
    }
    if (gEcAgentPrepPartyCount == 0 || gEcAgentPrepPartyCount > PARTY_SIZE)
    {
        Fail(EC_AGENT_PREP_BAD_PARTY, 0);
        return;
    }

    memset(sEcAgentPreparedParty, 0, sizeof(sEcAgentPreparedParty));
    for (u32 slot = 0; slot < gEcAgentPrepPartyCount; slot++)
    {
        enum Species species = gEcAgentPrepSpecies[slot];
        u32 level = gEcAgentPrepLevel[slot];
        if (species == SPECIES_NONE || species >= NUM_SPECIES)
        {
            Fail(EC_AGENT_PREP_BAD_SPECIES, slot);
            return;
        }
        if (level != GetCurrentLevelCap() || level > MAX_LEVEL)
        {
            Fail(EC_AGENT_PREP_BAD_LEVEL, slot);
            return;
        }
        CreateMon(&sEcAgentPreparedParty[slot], species, level, MAX_PER_STAT_IVS, OTID_STRUCT_PLAYER_ID);
        if (gEcAgentPrepPreset[slot] != EC_AGENT_PREP_KEEP
         && ApplyEmeraldChampionsBattleSetChoiceForFormat(
                &sEcAgentPreparedParty[slot],
                gEcAgentPrepPreset[slot],
                gEcAgentPrepFormat[slot]) == EC_BATTLE_SET_FAILED)
        {
            Fail(EC_AGENT_PREP_BAD_PRESET, slot);
            return;
        }
        if (!ApplyOverrides(&sEcAgentPreparedParty[slot], slot))
        {
            Fail(EC_AGENT_PREP_BAD_STAT_POINTS, slot);
            return;
        }
    }

    memset(gParties[B_TRAINER_PLAYER], 0, sizeof(gParties[B_TRAINER_PLAYER]));
    memcpy(gParties[B_TRAINER_PLAYER], sEcAgentPreparedParty, sizeof(sEcAgentPreparedParty));
    CalculatePlayerPartyCount();
    SavePlayerParty();
    gEcAgentPrepResult = EC_AGENT_PREP_SUCCESS;
    gEcAgentPrepErrorSlot = EC_AGENT_PREP_KEEP;
    gEcAgentPrepCommand = 0;
}

#endif
