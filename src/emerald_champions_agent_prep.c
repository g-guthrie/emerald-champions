#include "global.h"

#if EC_HEADLESS_FIXTURES

#include "caps.h"
#include "emerald_champions_agent_prep.h"
#include "emerald_champions_battle_sets.h"
#include "main.h"
#include "load_save.h"
#include "legendary_signs.h"
#include "pokemon.h"
#include "pokedex.h"
#include "save.h"
#include "constants/abilities.h"
#include "constants/form_change_types.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/species.h"

EWRAM_DATA volatile u32 gEcAgentPrepCommand = 0;
EWRAM_DATA volatile u32 gEcAgentPrepResult = EC_AGENT_PREP_PENDING;
EWRAM_DATA volatile u32 gEcAgentPrepErrorSlot = 0;
EWRAM_DATA volatile u32 gEcAgentPrepPartyCount = 0;
EWRAM_DATA volatile u32 gEcAgentPrepExtended = 0;
EWRAM_DATA volatile u32 gEcAgentPrepSpecies[EC_AGENT_PREP_PARTY_SIZE] = {0};
EWRAM_DATA volatile u32 gEcAgentPrepPreset[EC_AGENT_PREP_PARTY_SIZE] = {0};
EWRAM_DATA volatile u32 gEcAgentPrepFormat[EC_AGENT_PREP_PARTY_SIZE] = {0};
EWRAM_DATA volatile u32 gEcAgentPrepLevel[EC_AGENT_PREP_PARTY_SIZE] = {0};
EWRAM_DATA volatile u32 gEcAgentPrepMoves[EC_AGENT_PREP_PARTY_SIZE][EC_AGENT_PREP_MOVE_COUNT] = {{0}};
EWRAM_DATA volatile u32 gEcAgentPrepNature[EC_AGENT_PREP_PARTY_SIZE] = {0};
EWRAM_DATA volatile u32 gEcAgentPrepAbility[EC_AGENT_PREP_PARTY_SIZE] = {0};
EWRAM_DATA volatile u32 gEcAgentPrepItem[EC_AGENT_PREP_PARTY_SIZE] = {0};
EWRAM_DATA volatile u32 gEcAgentPrepEvs[EC_AGENT_PREP_PARTY_SIZE][EC_AGENT_PREP_STAT_COUNT] = {{0}};
EWRAM_DATA volatile u32 gEcAgentPrepIvs[EC_AGENT_PREP_PARTY_SIZE][EC_AGENT_PREP_STAT_COUNT] = {{0}};
EWRAM_DATA volatile u32 gEcAgentPrepFriendship[EC_AGENT_PREP_PARTY_SIZE] = {0};
EWRAM_DATA volatile u32 gEcAgentPrepPokerus[EC_AGENT_PREP_PARTY_SIZE] = {0};
EWRAM_DATA volatile u32 gEcAgentPrepPpBonuses[EC_AGENT_PREP_PARTY_SIZE] = {0};

static EWRAM_DATA struct Pokemon sEcAgentPreparedParty[PARTY_SIZE] = {0};

static void Fail(enum EmeraldChampionsAgentPrepResult result, u32 slot)
{
    gEcAgentPrepResult = result;
    gEcAgentPrepErrorSlot = slot;
    gEcAgentPrepCommand = 0;
    gEcAgentPrepExtended = 0;
}

// The player's Pokemon may hold any slot the party menu offers, the
// Inclement added slots included.
static bool32 FindAbility(enum Species species, enum Ability ability, u8 *slotOut)
{
    u32 slot;
    if (!FindSpeciesAbilitySlotForOwner(species, ability, FALSE, &slot))
        return FALSE;
    *slotOut = slot;
    return TRUE;
}

static enum EmeraldChampionsAgentPrepResult ApplyOverrides(struct Pokemon *mon, u32 slot)
{
    u8 ppBonuses = 0;
    u32 total = 0;

    for (u32 moveSlot = 0; moveSlot < MAX_MON_MOVES; moveSlot++)
    {
        u32 move = gEcAgentPrepMoves[slot][moveSlot];
        if (move == EC_AGENT_PREP_KEEP)
            continue;
        if (move >= MOVES_COUNT || !CanSpeciesUseEmeraldChampionsPreparationMove(GetMonData(mon, MON_DATA_SPECIES), move))
            return EC_AGENT_PREP_BAD_MOVE;
        SetMonMoveSlot(mon, move, moveSlot);
    }
    if (gEcAgentPrepNature[slot] != EC_AGENT_PREP_KEEP)
    {
        u8 nature;
        if (gEcAgentPrepNature[slot] >= NUM_NATURES)
            return EC_AGENT_PREP_BAD_NATURE;
        nature = gEcAgentPrepNature[slot];
        SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
    }
    if (gEcAgentPrepAbility[slot] != EC_AGENT_PREP_KEEP)
    {
        u8 abilitySlot;
        if (!FindAbility(GetMonData(mon, MON_DATA_SPECIES), gEcAgentPrepAbility[slot], &abilitySlot))
            return EC_AGENT_PREP_BAD_ABILITY;
        SetMonData(mon, MON_DATA_ABILITY_NUM, &abilitySlot);
    }
    if (gEcAgentPrepItem[slot] != EC_AGENT_PREP_KEEP)
    {
        u16 item;
        if (gEcAgentPrepItem[slot] >= ITEMS_COUNT)
            return EC_AGENT_PREP_BAD_ITEM;
        item = gEcAgentPrepItem[slot];
        SetMonData(mon, MON_DATA_HELD_ITEM, &item);
    }
    for (u32 stat = 0; stat < NUM_STATS; stat++)
    {
        u32 points = gEcAgentPrepEvs[slot][stat];
        if (points == EC_AGENT_PREP_KEEP)
        {
            // An override is one complete spread or six KEEP sentinels.
            if (gEcAgentPrepEvs[slot][0] != EC_AGENT_PREP_KEEP)
                return EC_AGENT_PREP_BAD_EVS;
            continue;
        }
        if (gEcAgentPrepEvs[slot][0] == EC_AGENT_PREP_KEEP)
            return EC_AGENT_PREP_BAD_EVS;
        if (points > MAX_PER_STAT_EVS)
            return EC_AGENT_PREP_BAD_EVS;
        total += points;
    }
    if (gEcAgentPrepEvs[slot][0] != EC_AGENT_PREP_KEEP)
    {
        if (total > MAX_TOTAL_EVS)
            return EC_AGENT_PREP_BAD_EVS;
        for (u32 stat = 0; stat < NUM_STATS; stat++)
        {
            u8 points = gEcAgentPrepEvs[slot][stat];
            SetMonData(mon, EC_EV_DATA(stat), &points);
        }
    }
    if (gEcAgentPrepExtended && gEcAgentPrepPpBonuses[slot] != EC_AGENT_PREP_KEEP)
    {
        if (gEcAgentPrepPpBonuses[slot] > UINT8_MAX)
            return EC_AGENT_PREP_BAD_PP_BONUSES;
        ppBonuses = gEcAgentPrepPpBonuses[slot];
    }
    SetMonData(mon, MON_DATA_PP_BONUSES, &ppBonuses);
    for (u32 stat = 0; stat < NUM_STATS; stat++)
    {
        u32 requested = gEcAgentPrepExtended ? gEcAgentPrepIvs[slot][stat] : EC_AGENT_PREP_KEEP;
        u8 iv = requested == EC_AGENT_PREP_KEEP ? MAX_PER_STAT_IVS : requested;
        if (requested != EC_AGENT_PREP_KEEP && requested > MAX_PER_STAT_IVS)
            return EC_AGENT_PREP_BAD_IVS;
        SetMonData(mon, EC_IV_DATA(stat), &iv);
    }
    if (gEcAgentPrepExtended && gEcAgentPrepFriendship[slot] != EC_AGENT_PREP_KEEP)
    {
        u8 friendship = gEcAgentPrepFriendship[slot];
        if (gEcAgentPrepFriendship[slot] > MAX_FRIENDSHIP)
            return EC_AGENT_PREP_BAD_FRIENDSHIP;
        SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);
    }
    if (gEcAgentPrepExtended && gEcAgentPrepPokerus[slot] != EC_AGENT_PREP_KEEP)
    {
        u8 pokerus = gEcAgentPrepPokerus[slot];
        if (gEcAgentPrepPokerus[slot] > UINT8_MAX)
            return EC_AGENT_PREP_BAD_POKERUS;
        SetMonData(mon, MON_DATA_POKERUS, &pokerus);
    }
    CalculateMonStats(mon);
    MonRestorePP(mon);
    return EC_AGENT_PREP_SUCCESS;
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
        enum EmeraldChampionsAgentPrepResult overrideResult;
        if (species == SPECIES_NONE || species >= NUM_SPECIES)
        {
            Fail(EC_AGENT_PREP_BAD_SPECIES, slot);
            return;
        }
        // The PC, gifts and League door share the one restricted member limit.
        enum RestrictedPartyClass kind = GetRestrictedPartyClass(species);
        for (u32 earlier = 0; kind != RESTRICTED_PARTY_NONE && earlier < slot; earlier++)
        {
            if (GetRestrictedPartyClass(gEcAgentPrepSpecies[earlier]) != RESTRICTED_PARTY_NONE)
            {
                Fail(EC_AGENT_PREP_RESTRICTED_PARTY, slot);
                return;
            }
        }
        if (level == EC_AGENT_PREP_KEEP)
            level = GetPlayerLevelCapForSpecies(species);
        if (level != GetPlayerLevelCapForSpecies(species) || level > MAX_LEVEL)
        {
            Fail(EC_AGENT_PREP_BAD_LEVEL, slot);
            return;
        }
        CreateRandomMonWithIVs(&sEcAgentPreparedParty[slot], species, level, MAX_PER_STAT_IVS);
        if (gEcAgentPrepPreset[slot] != EC_AGENT_PREP_KEEP
         && ApplyEmeraldChampionsBattleSetChoiceForFormat(
                &sEcAgentPreparedParty[slot],
                gEcAgentPrepPreset[slot],
                gEcAgentPrepFormat[slot]) == EC_BATTLE_SET_FAILED)
        {
            Fail(EC_AGENT_PREP_BAD_PRESET, slot);
            return;
        }
        overrideResult = ApplyOverrides(&sEcAgentPreparedParty[slot], slot);
        if (overrideResult != EC_AGENT_PREP_SUCCESS)
        {
            Fail(overrideResult, slot);
            return;
        }
        ClampMonToPlayerLevelCap(&sEcAgentPreparedParty[slot]);
    }

    // Several Mega Stone holders are legal (one Mega per battle); a holder
    // beside another restricted member could never use its stone.
    for (u32 slot = 0; slot < gEcAgentPrepPartyCount; slot++)
    {
        struct Pokemon *mon = &sEcAgentPreparedParty[slot];
        enum Species species = GetMonData(mon, MON_DATA_SPECIES);
        struct FormChangeContext ctx = {
            .method = FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM,
            .currentSpecies = species,
            .heldItem = GetMonData(mon, MON_DATA_HELD_ITEM),
            .ability = GetMonAbility(mon),
        };
        for (u32 move = 0; move < MAX_MON_MOVES; move++)
            ctx.moves[move] = GetMonData(mon, MON_DATA_MOVE1 + move);
        bool32 mega = GetFormChangeTargetSpecies_Internal(ctx) != species;
        ctx.method = FORM_CHANGE_BATTLE_MEGA_EVOLUTION_MOVE;
        mega |= GetFormChangeTargetSpecies_Internal(ctx) != species;
        if (!mega)
            continue;
        for (u32 other = 0; other < gEcAgentPrepPartyCount; other++)
            if (other != slot && GetRestrictedPartyClass(gEcAgentPrepSpecies[other]) != RESTRICTED_PARTY_NONE)
            {
                Fail(EC_AGENT_PREP_RESTRICTED_PARTY, slot);
                return;
            }
    }

    memset(gParties[B_TRAINER_PLAYER], 0, sizeof(gParties[B_TRAINER_PLAYER]));
    memcpy(gParties[B_TRAINER_PLAYER], sEcAgentPreparedParty, sizeof(sEcAgentPreparedParty));
    CalculatePlayerPartyCount();
    for (u32 slot = 0; slot < gEcAgentPrepPartyCount; slot++)
    {
        gEcAgentPrepLevel[slot] = GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_LEVEL);
        HandleSetPokedexFlagFromMon(&gParties[B_TRAINER_PLAYER][slot], FLAG_SET_SEEN);
        HandleSetPokedexFlagFromMon(&gParties[B_TRAINER_PLAYER][slot], FLAG_SET_CAUGHT);
        MarkLegendarySignCaughtBySpecies(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_SPECIES));
    }
    SavePlayerParty();
    gEcAgentPrepResult = EC_AGENT_PREP_SUCCESS;
    gEcAgentPrepErrorSlot = EC_AGENT_PREP_KEEP;
    gEcAgentPrepCommand = 0;
    gEcAgentPrepExtended = 0;
}

#endif
