#include "global.h"
#include "test/test.h"
#include "emerald_champions_battle_sets.h"
#include "pokedex_plus_hgss.h"
#include "pokemon.h"

// The Pokedex's moves page and the Center's All Legal Moves service must list
// the same moves: the page reads GetPokedexTutorMoves, the tutor collects for a
// Pokemon that knows no moves yet.
TEST("Pokédex moves page lists exactly the Center tutor's moves for every species")
{
    static EWRAM_DATA u16 dexMoves[MOVES_COUNT_ALL];
    static EWRAM_DATA u16 centerMoves[MOVES_COUNT_ALL];
    static EWRAM_DATA bool8 offered[MOVES_COUNT_ALL];
    struct BoxPokemon mon;

    for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        u32 dexCount, centerCount;

        if (!IsSpeciesEnabled(species)
         || gSpeciesInfo[species].isMegaEvolution
         || gSpeciesInfo[species].isGigantamax)
            continue;
        CreateBoxMon(&mon, species, 5, 0, OTID_STRUCT_PLAYER_ID);
        for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            SetBoxMonMoveSlot(&mon, MOVE_NONE, slot);

        centerCount = GetEmeraldChampionsPreparationMovesToLearn(&mon, centerMoves);
        dexCount = GetPokedexTutorMoves(species, dexMoves);
        EXPECT_EQ(dexCount, centerCount);
        memset(offered, FALSE, sizeof(offered));
        for (u32 i = 0; i < centerCount; i++)
            offered[centerMoves[i]] = TRUE;
        for (u32 i = 0; i < dexCount; i++)
            EXPECT(offered[dexMoves[i]]);
    }
}
