#include "global.h"
#include "data.h"
#include "pokemon.h"
#include "verdant_battle_sets.h"
#include "constants/abilities.h"
#include "constants/moves.h"
#include "constants/species.h"

#include "data/pokemon/verdant_battle_sets.h"

bool8 ApplyVerdantBattleSetPreset(struct Pokemon *mon)
{
    const struct VerdantBattleSetPreset *preset;
    u16 species;
    u16 move;
    u8 ppBonuses = 0;
    u8 i;
    u8 j;
    bool8 sawEmptyMove = FALSE;

    species = GetMonData(mon, MON_DATA_SPECIES2, NULL);
    if (species == SPECIES_NONE || species == SPECIES_EGG || species >= NUM_SPECIES)
        return FALSE;

    preset = &gVerdantBattleSetPresets[species];
    if (preset->nature >= NUM_NATURES
     || preset->abilitySlot >= NUM_ABILITY_SLOTS
     || gBaseStats[species].abilities[preset->abilitySlot] == ABILITY_NONE)
        return FALSE;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        move = preset->moves[i];
        if (move == MOVE_NONE)
        {
            sawEmptyMove = TRUE;
            continue;
        }
        if (sawEmptyMove || move >= MOVES_COUNT)
            return FALSE;
        for (j = 0; j < i; j++)
        {
            if (preset->moves[j] == move)
                return FALSE;
        }
    }
    if (preset->moves[0] == MOVE_NONE)
        return FALSE;

    SetMonData(mon, MON_DATA_PP_BONUSES, &ppBonuses);
    for (i = 0; i < MAX_MON_MOVES; i++)
        SetMonMoveSlot(mon, preset->moves[i], i);
    SetMonData(mon, MON_DATA_NATURE, &preset->nature);
    SetMonData(mon, MON_DATA_ABILITY_NUM, &preset->abilitySlot);
    CalculateMonStats(mon);
    return TRUE;
}

bool8 IsVerdantLegendarySpecies(u16 species)
{
    switch (species)
    {
#include "data/pokemon/verdant_legendary_species.h"
        return TRUE;
    default:
        return FALSE;
    }
}
