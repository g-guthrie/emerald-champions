#include "global.h"
#include "event_data.h"
#include "item.h"
#include "legendary_signs.h"
#include "pokemon.h"
#include "overworld.h"
#include "pokedex.h"
#include "constants/flags.h"
#include "save.h"
#include "test/test.h"
#include "constants/vars.h"

static const u16 sUnlockedVars[] = {
    VAR_LEGENDARY_SIGNS_UNLOCKED_0, VAR_LEGENDARY_SIGNS_UNLOCKED_1,
    VAR_LEGENDARY_SIGNS_UNLOCKED_2, VAR_LEGENDARY_SIGNS_UNLOCKED_3,
    VAR_LEGENDARY_SIGNS_UNLOCKED_4, VAR_LEGENDARY_SIGNS_UNLOCKED_5,
};
static const u16 sCaughtVars[] = {
    VAR_LEGENDARY_SIGNS_CAUGHT_0, VAR_LEGENDARY_SIGNS_CAUGHT_1,
    VAR_LEGENDARY_SIGNS_CAUGHT_2, VAR_LEGENDARY_SIGNS_CAUGHT_3,
    VAR_LEGENDARY_SIGNS_CAUGHT_4, VAR_LEGENDARY_SIGNS_CAUGHT_5,
};

static void ResetSignState(void)
{
    for (u32 i = 0; i < ARRAY_COUNT(sUnlockedVars); i++)
    {
        VarSet(sUnlockedVars[i], 0);
        VarSet(sCaughtVars[i], 0);
    }
    ZeroPlayerPartyMons();
    ClearBag();
}

TEST("Sign state survives a native flash save and load across all six storage groups")
{
    u16 expectedUnlocked[ARRAY_COUNT(sUnlockedVars)];
    u16 expectedCaught[ARRAY_COUNT(sCaughtVars)];

    ResetSignState();
    for (enum LegendarySignId id = 0; id < LEGENDARY_SIGN_COUNT; id++)
    {
        // Retain both active and caught entries in each populated group.
        UnlockLegendarySign(id);
        if (id % 2 == 0)
            MarkLegendarySignCaughtBySpecies(gLegendarySignDefinitions[id].species);
    }
    for (u32 i = 0; i < ARRAY_COUNT(sUnlockedVars); i++)
    {
        expectedUnlocked[i] = VarGet(sUnlockedVars[i]);
        expectedCaught[i] = VarGet(sCaughtVars[i]);
    }
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    for (u32 i = 0; i < ARRAY_COUNT(sUnlockedVars); i++)
    {
        VarSet(sUnlockedVars[i], 0);
        VarSet(sCaughtVars[i], 0);
    }
    EXPECT(!IsLegendarySignUnlocked(LEGENDARY_SIGN_COBALION));
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    for (u32 i = 0; i < ARRAY_COUNT(sUnlockedVars); i++)
    {
        EXPECT_EQ(VarGet(sUnlockedVars[i]), expectedUnlocked[i]);
        EXPECT_EQ(VarGet(sCaughtVars[i]), expectedCaught[i]);
    }
    for (enum LegendarySignId id = 0; id < LEGENDARY_SIGN_COUNT; id++)
    {
        EXPECT(IsLegendarySignUnlocked(id));
        EXPECT_EQ(IsLegendarySignCaught(id), id % 2 == 0);
    }
}

// A historical-save compatibility check, not a constraint on current
// encounter difficulty, unlock rules, team choices or reward ordering.
TEST("Legendary v2 save migration preserves captures and pending reward data")
{
    // v2: undelivered Rusted Sword and Flame Plate, with both grants earned.
    const u16 pendingRelics = (1u << 2) | (1u << 7);
    const u16 earnedRelics = (1u << 10) | (1u << 13);
    ResetSignState();
    memset(gSaveBlock2Ptr->pokedex.lostLegendaryEncounters, 0, sizeof(gSaveBlock2Ptr->pokedex.lostLegendaryEncounters));
    MarkLegendarySignCaughtBySpecies(SPECIES_ARTICUNO);
    FlagSet(FLAG_EC_CAUGHT_MEWTWO);
    FlagSet(FLAG_DEFEATED_REGIROCK);
    FlagSet(FLAG_HIDE_REGIROCK);
    HandleSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_RAYQUAZA), FLAG_SET_CAUGHT, 0);
    FlagSet(FLAG_DEFEATED_RAYQUAZA);
    FlagSet(FLAG_HIDE_SKY_PILLAR_TOP_RAYQUAZA_STILL);
    gSaveBlock2Ptr->pokedex.lostLegendaryEncounters[LEGENDARY_SIGN_MEWTWO / 8] |= 1u << (LEGENDARY_SIGN_MEWTWO % 8);
    gSaveBlock2Ptr->pokedex.lostLegendaryEncounters[LEGENDARY_SIGN_ARTICUNO / 8] |= 1u << (LEGENDARY_SIGN_ARTICUNO % 8);
    gSaveBlock2Ptr->pokedex.lostLegendaryEncounters[12] = (1u << 2) | (1u << 3);
    VarSet(VAR_EMERALD_CHAMPIONS_SAVE_VERSION, 2);
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_0, pendingRelics);
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_1, earnedRelics);
    MigrateEmeraldChampionsCoreState();
    EXPECT_EQ(VarGet(VAR_EMERALD_CHAMPIONS_SAVE_VERSION), EMERALD_CHAMPIONS_SAVE_VERSION_CURRENT);
    EXPECT(!FlagGet(FLAG_EC_CAUGHT_MEWTWO));
    EXPECT(!FlagGet(FLAG_DEFEATED_REGIROCK));
    EXPECT(!FlagGet(FLAG_HIDE_REGIROCK));
    EXPECT(FlagGet(FLAG_DEFEATED_RAYQUAZA));
    EXPECT(FlagGet(FLAG_HIDE_SKY_PILLAR_TOP_RAYQUAZA_STILL));
    EXPECT(IsLegendarySignCaught(LEGENDARY_SIGN_ARTICUNO));
    EXPECT(FlagGet(FLAG_EC_CAUGHT_ARTICUNO));
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0), pendingRelics);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_1), earnedRelics);
    for (u32 i = 0; i < sizeof(gSaveBlock2Ptr->pokedex.lostLegendaryEncounters); i++)
        EXPECT_EQ(gSaveBlock2Ptr->pokedex.lostLegendaryEncounters[i], 0);
    MigrateEmeraldChampionsCoreState();
    EXPECT(FlagGet(FLAG_DEFEATED_RAYQUAZA));
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0), pendingRelics);
}
