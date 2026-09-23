#include "global.h"
#include "test/test.h"
#include "pokemon.h"
#include "pokedex.h"

extern bool32 Test_TradeEvolutionCleanup(bool32 canceled);
extern void Test_CommitEvolution(struct Pokemon *mon, enum Species before, enum Species after);

TEST("Evolution cleanup: trade scene destroys its task on completion and cancellation")
{
    bool32 canceled;
    PARAMETRIZE { canceled = FALSE; }
    PARAMETRIZE { canceled = TRUE; }
    EXPECT(Test_TradeEvolutionCleanup(canceled));
}

TEST("Evolution commit: updates species and Dex while preserving held item and moves")
{
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_EEVEE, 10, 0, OTID_STRUCT_PLAYER_ID);
    enum Item item = ITEM_LEFTOVERS;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    SetMonMoveSlot(&mon, MOVE_PROTECT, 0);
    u32 tracker = 5;
    SetMonData(&mon, MON_DATA_EVOLUTION_TRACKER, &tracker);
    Test_CommitEvolution(&mon, SPECIES_EEVEE, SPECIES_VAPOREON);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_VAPOREON);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_EVOLUTION_TRACKER), 0);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), ITEM_LEFTOVERS);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE1), MOVE_PROTECT);
    EXPECT(GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_VAPOREON), FLAG_GET_CAUGHT));
    EXPECT(GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_VAPOREON), FLAG_GET_SEEN));
}
