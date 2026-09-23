#include "global.h"
#include "pokemon.h"
#include "event_data.h"
#include "legendary_signs.h"
#include "wild_encounter.h"
#include "random.h"
#include "test/test.h"

TEST("Ultra Beast access: every native source has a table, one-percent slot and one-time capture gate")
{
    static const enum Species species[] = {SPECIES_BLACEPHALON, SPECIES_BUZZWOLE,
        SPECIES_CELESTEELA, SPECIES_GUZZLORD, SPECIES_KARTANA, SPECIES_NIHILEGO,
        SPECIES_PHEROMOSA, SPECIES_POIPOLE, SPECIES_STAKATAKA, SPECIES_XURKITREE};
    static const u16 caughtVars[] = {VAR_LEGENDARY_SIGNS_CAUGHT_0, VAR_LEGENDARY_SIGNS_CAUGHT_1,
        VAR_LEGENDARY_SIGNS_CAUGHT_2, VAR_LEGENDARY_SIGNS_CAUGHT_3,
        VAR_LEGENDARY_SIGNS_CAUGHT_4, VAR_LEGENDARY_SIGNS_CAUGHT_5};
    s8 group = gSaveBlock1Ptr->location.mapGroup, map = gSaveBlock1Ptr->location.mapNum;
    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        for (u32 v = 0; v < ARRAY_COUNT(caughtVars); v++)
            VarSet(caughtVars[v], 0);
        enum LegendarySignId id = GetLegendarySignIdBySpecies(species[i]);
        EXPECT(id < LEGENDARY_SIGN_COUNT);
        const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[id];
        gSaveBlock1Ptr->location.mapGroup = sign->mapId >> 8;
        gSaveBlock1Ptr->location.mapNum = sign->mapId;
        u32 header = GetCurrentMapWildMonHeaderId();
        EXPECT_NE(header, HEADER_NONE);
        EXPECT(gWildMonHeaders[header].encounterTypes[TIME_OF_DAY_DEFAULT].landMonsInfo != NULL);
        EXPECT(CanAcquireLegendarySignSpecies(species[i]));
        u32 appearances = 0;
        for (u32 roll = 0; roll < 100; roll++)
        {
            SET_RNG(RNG_NONE, roll);
            appearances += ChooseRareWildLegendarySpecies(WILD_AREA_LAND, FALSE) == species[i];
        }
        EXPECT_EQ(appearances, 1);
        MarkLegendarySignCaughtBySpecies(species[i]);
        EXPECT(!CanAcquireLegendarySignSpecies(species[i]));
        for (u32 roll = 0; roll < 100; roll++)
        {
            SET_RNG(RNG_NONE, roll);
            EXPECT_NE(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, FALSE), species[i]);
        }
    }
    for (u32 v = 0; v < ARRAY_COUNT(caughtVars); v++)
        VarSet(caughtVars[v], 0);
    gSaveBlock1Ptr->location.mapGroup = group;
    gSaveBlock1Ptr->location.mapNum = map;
}

TEST("Ultra Beast access: Poipole evolves only with its obtainable Dragon Pulse")
{
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_POIPOLE, 10, 0, OTID_STRUCT_PLAYER_ID);
    SetMonMoveSlot(&mon, MOVE_PECK, 0);
    for (u32 slot = 1; slot < MAX_MON_MOVES; slot++)
        SetMonMoveSlot(&mon, MOVE_NONE, slot);
    EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, NULL, CHECK_EVO), SPECIES_NONE);
    const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(SPECIES_POIPOLE);
    bool32 learnsPulse = FALSE;
    for (u32 i = 0; learnset[i].move != LEVEL_UP_MOVE_END; i++)
        learnsPulse |= learnset[i].move == MOVE_DRAGON_PULSE;
    EXPECT(learnsPulse);
    SetMonMoveSlot(&mon, MOVE_DRAGON_PULSE, 0);
    EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, NULL, CHECK_EVO), SPECIES_NAGANADEL);
}
