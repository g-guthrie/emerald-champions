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

TEST("Double Team, Minimize and one-hit KO moves are removed from every source a player uses")
{
    static const u16 removed[] = {MOVE_DOUBLE_TEAM, MOVE_MINIMIZE, MOVE_FISSURE,
                                  MOVE_SHEER_COLD, MOVE_GUILLOTINE, MOVE_HORN_DRILL};
    static EWRAM_DATA u16 moves[MOVES_COUNT_ALL];
    // Each species learns at least one removed move by level-up or tutor.
    static const u16 species[] = {SPECIES_WALREIN, SPECIES_CHANSEY, SPECIES_KINGLER,
                                  SPECIES_SEAKING, SPECIES_DUGTRIO, SPECIES_GARCHOMP};
    struct BoxPokemon mon;

    for (u32 i = 0; i < ARRAY_COUNT(removed); i++)
        EXPECT(IsMoveRemovedFromGame(removed[i]));
    EXPECT(!IsMoveRemovedFromGame(MOVE_SPORE));
    for (u32 s = 0; s < ARRAY_COUNT(species); s++)
    {
        CreateBoxMon(&mon, species[s], MAX_LEVEL, 0, OTID_STRUCT_PLAYER_ID);
        for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            for (u32 i = 0; i < ARRAY_COUNT(removed); i++)
                EXPECT_NE(GetBoxMonData(&mon, MON_DATA_MOVE1 + slot), removed[i]);
        u32 count = GetPokedexTutorMoves(species[s], moves);
        for (u32 m = 0; m < count; m++)
            for (u32 i = 0; i < ARRAY_COUNT(removed); i++)
                EXPECT_NE(moves[m], removed[i]);
    }
}
