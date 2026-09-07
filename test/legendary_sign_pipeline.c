#include "global.h"
#include "battle.h"
#include "caps.h"
#include "daycare.h"
#include "champions_circuit.h"
#include "malloc.h"
#include "script_menu.h"
#include "field_specials.h"
#include "event_data.h"
#include "item.h"
#include "legendary_signs.h"
#include "pokemon.h"
#include "random.h"
#include "save.h"
#include "string_util.h"
#include "test/test.h"
#include "constants/flags.h"
#include "constants/field_specials.h"
#include "constants/maps.h"
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
    memset(gSaveBlock2Ptr->pokedex.lostLegendaryEncounters, 0, sizeof(gSaveBlock2Ptr->pokedex.lostLegendaryEncounters));
    for (u32 i = 0; i < ARRAY_COUNT(sUnlockedVars); i++)
    {
        VarSet(sUnlockedVars[i], 0);
        VarSet(sCaughtVars[i], 0);
    }
    ZeroPlayerPartyMons();
    ClearBag();
}

static void SetSignPrerequisites(const struct LegendarySignDefinition *sign, bool32 enabled)
{
    for (u32 i = 0; i < NUM_BADGES; i++)
    {
        if (enabled)
            FlagSet(FLAG_BADGE01_GET + i);
        else
            FlagClear(FLAG_BADGE01_GET + i);
    }
    if (sign->requiredFlag != 0)
    {
        if (enabled)
            FlagSet(sign->requiredFlag);
        else
            FlagClear(sign->requiredFlag);
    }
}

static void BringRequiredPartner(const struct LegendarySignDefinition *sign)
{
    ZeroPlayerPartyMons();
    if (sign->requiredSpecies != SPECIES_NONE)
        CreateMon(&gParties[B_TRAINER_PLAYER][0], sign->requiredSpecies, 50, 0, OTID_STRUCT_PLAYER_ID);
}

TEST("Every gated Sign distinguishes locked missing-partner active and caught states")
{
    u32 checked = 0;
    for (enum LegendarySignId id = 0; id < LEGENDARY_SIGN_COUNT; id++)
    {
        const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[id];
        if (sign->source != LEGENDARY_SOURCE_CONDITIONAL_WILD
         && !(sign->source == LEGENDARY_SOURCE_VISIBLE && sign->mapId != 0xFFFF))
            continue;
        ResetSignState();
        SetSignPrerequisites(sign, FALSE);
        BringRequiredPartner(sign);
        gSpecialVar_0x8004 = id;
        EXPECT_EQ(GetSelectedLegendarySignState(), 0);
        TryUnlockSelectedLegendarySign();
        EXPECT_EQ(gSpecialVar_Result, 0);
        EXPECT(!IsLegendarySignUnlocked(id));

        SetSignPrerequisites(sign, TRUE);
        if (sign->requiredFlag != 0)
        {
            FlagClear(sign->requiredFlag);
            TryUnlockSelectedLegendarySign();
            EXPECT_EQ(gSpecialVar_Result, 0);
            EXPECT(!IsLegendarySignUnlocked(id));
            FlagSet(sign->requiredFlag);
        }
        SetSignPrerequisites(sign, FALSE);
        if (sign->requiredFlag != 0)
            FlagSet(sign->requiredFlag);
        // A required badge alone can itself satisfy a one-badge threshold.
        if (sign->minimumBadges > 1
         || (sign->minimumBadges > 0 && (sign->requiredFlag < FLAG_BADGE01_GET || sign->requiredFlag >= FLAG_BADGE01_GET + NUM_BADGES)))
        {
            TryUnlockSelectedLegendarySign();
            EXPECT_EQ(gSpecialVar_Result, 0);
            EXPECT(!IsLegendarySignUnlocked(id));
        }
        SetSignPrerequisites(sign, TRUE);
        ZeroPlayerPartyMons();
        if (sign->requiredSpecies != SPECIES_NONE)
        {
            TryUnlockSelectedLegendarySign();
            EXPECT_EQ(gSpecialVar_Result, 1);
            EXPECT(!IsLegendarySignUnlocked(id));
        }
        BringRequiredPartner(sign);
        TryUnlockSelectedLegendarySign();
        EXPECT_EQ(gSpecialVar_Result, 6);
        EXPECT(!IsLegendarySignUnlocked(id));
        ResearchSelectedLegendarySign();
        EXPECT_EQ(gSpecialVar_Result, 2);
        EXPECT_EQ(GetSelectedLegendarySignState(), 1);
        TryUnlockSelectedLegendarySign();
        EXPECT_EQ(gSpecialVar_Result, 3);
        MarkLegendarySignCaughtBySpecies(sign->species);
        EXPECT_EQ(GetSelectedLegendarySignState(), 2);
        TryUnlockSelectedLegendarySign();
        EXPECT_EQ(gSpecialVar_Result, 4);
        EXPECT_EQ(ShouldShowSelectedLegendarySignObject(), FALSE);
        checked++;
    }
    EXPECT_GT(checked, 0);
}

TEST("Every conditional Sign requires individual Devon research and stops after capture")
{
    u32 checked = 0;
    for (enum LegendarySignId id = 0; id < LEGENDARY_SIGN_COUNT; id++)
    {
        const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[id];
        enum Species species = SPECIES_NONE;
        u8 level = 0;
        bool32 encountered = FALSE;
        s32 expectedLevel;
        if (sign->source != LEGENDARY_SOURCE_CONDITIONAL_WILD)
            continue;
        ResetSignState();
        // Other completed Signs cannot win an encounter roll for this test.
        for (u32 i = 0; i < ARRAY_COUNT(sCaughtVars); i++)
            VarSet(sCaughtVars[i], 0xFFFF);
        VarSet(sCaughtVars[id / 16], 0xFFFF ^ (1u << (id % 16)));
        SetSignPrerequisites(sign, TRUE);
        BringRequiredPartner(sign);
        gSaveBlock1Ptr->location.mapGroup = sign->mapId >> 8;
        gSaveBlock1Ptr->location.mapNum = sign->mapId & 0xFF;

        SetSignPrerequisites(sign, FALSE);
        EXPECT(!TryGetLegendarySignWildOverride(sign->area, &species, &level));
        EXPECT(!IsLegendarySignUnlocked(id));
        SetSignPrerequisites(sign, TRUE);
        gSaveBlock1Ptr->location.mapGroup = 0xFF;
        gSaveBlock1Ptr->location.mapNum = 0xFF;
        EXPECT(!TryGetLegendarySignWildOverride(sign->area, &species, &level));
        EXPECT(!IsLegendarySignUnlocked(id));
        gSaveBlock1Ptr->location.mapGroup = sign->mapId >> 8;
        gSaveBlock1Ptr->location.mapNum = sign->mapId & 0xFF;

        // Wrong terrain and a missing partner must not awaken it.
        TryGetLegendarySignWildOverride(sign->area == WILD_AREA_LAND ? WILD_AREA_WATER : WILD_AREA_LAND, &species, &level);
        EXPECT(!IsLegendarySignUnlocked(id));
        ZeroPlayerPartyMons();
        TryGetLegendarySignWildOverride(sign->area, &species, &level);
        EXPECT(!IsLegendarySignUnlocked(id));
        BringRequiredPartner(sign);
        // All local requirements are met, but this Sign has not visited Devon.
        EXPECT(!TryGetLegendarySignWildOverride(sign->area, &species, &level));
        // The retired local path set this bit before its random encounter roll.
        EXPECT(!IsLegendarySignUnlocked(id));
        gSpecialVar_0x8004 = id;
        ResearchSelectedLegendarySign();
        EXPECT_EQ(gSpecialVar_Result, 2);
        SeedRng(17);
        for (u32 attempt = 0; attempt < 512 && !encountered; attempt++)
            encountered = TryGetLegendarySignWildOverride(sign->area, &species, &level);
        EXPECT(encountered);
        EXPECT(IsLegendarySignUnlocked(id));
        EXPECT_EQ(species, sign->species);
        expectedLevel = GetCurrentLevelCap() + sign->levelOffset;
        if (expectedLevel < 1)
            expectedLevel = 1;
        if (expectedLevel > MAX_LEVEL)
            expectedLevel = MAX_LEVEL;
        EXPECT_EQ(level, expectedLevel);

        // Once awakened, leaving the partner behind does not relock it.
        ZeroPlayerPartyMons();
        encountered = FALSE;
        for (u32 attempt = 0; attempt < 512 && !encountered; attempt++)
            encountered = TryGetLegendarySignWildOverride(sign->area, &species, &level);
        EXPECT(encountered);
        EXPECT_EQ(species, sign->species);
        MarkLegendarySignCaughtBySpecies(sign->species);
        EXPECT(!TryGetLegendarySignWildOverride(sign->area, &species, &level));
        checked++;
    }
    EXPECT_GT(checked, 0);
}

TEST("Devon directs Palkia and Manaphy to their underwater encounter area")
{
    static const enum LegendarySignId ids[] = {LEGENDARY_SIGN_PALKIA, LEGENDARY_SIGN_MANAPHY};
    for (u32 index = 0; index < ARRAY_COUNT(ids); index++)
    {
        enum LegendarySignId id = ids[index];
        const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[id];
        EXPECT_EQ(sign->mapId, MAP_UNDERWATER_SEAFLOOR_CAVERN);
        ResetSignState();
        for (u32 i = 0; i < ARRAY_COUNT(sCaughtVars); i++)
            VarSet(sCaughtVars[i], 0xFFFF);
        VarSet(sCaughtVars[id / 16], 0xFFFF ^ (1u << (id % 16)));
        SetSignPrerequisites(sign, TRUE);
        BringRequiredPartner(sign);
        gSpecialVar_0x8004 = id;
        ResearchSelectedLegendarySign();
        EXPECT_EQ(gSpecialVar_Result, 2);
        EXPECT(IsLegendarySignUnlocked(id));
        EXPECT_EQ(StringCompare(gStringVar3, COMPOUND_STRING("the Seafloor Cavern seabed")), 0);
    }
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

TEST("Lost legendary encounters remain distinct from capture and survive native flash save")
{
    static const enum Species nativeSpecies[] = {
        SPECIES_GROUDON, SPECIES_KYOGRE, SPECIES_RAYQUAZA,
        SPECIES_REGIROCK, SPECIES_REGICE, SPECIES_REGISTEEL,
        SPECIES_LATIAS, SPECIES_LATIOS, SPECIES_LUGIA, SPECIES_HO_OH,
        SPECIES_MEW, SPECIES_DEOXYS, SPECIES_JIRACHI, SPECIES_DIANCIE,
        SPECIES_HEATRAN, SPECIES_MOLTRES,
    };
    ResetSignState();
    for (enum LegendarySignId id = 0; id < LEGENDARY_SIGN_COUNT; id++)
    {
        enum Species species = gLegendarySignDefinitions[id].species;
        EXPECT(!IsLegendaryEncounterLost(species));
        MarkLegendaryEncounterLost(species);
        EXPECT(IsLegendaryEncounterLost(species));
        EXPECT(!IsLegendarySignCaught(id));
        EXPECT(!CanAcquireLegendarySignSpecies(species));
        UnlockLegendarySign(id);
        gSpecialVar_0x8004 = id;
        EXPECT(!ShouldShowSelectedLegendarySignObject());
    }
    for (u32 i = 0; i < ARRAY_COUNT(nativeSpecies); i++)
    {
        EXPECT(!IsLegendaryEncounterLost(nativeSpecies[i]));
        MarkLegendaryEncounterLost(nativeSpecies[i]);
        EXPECT(IsLegendaryEncounterLost(nativeSpecies[i]));
    }
    MarkLegendaryEncounterLost(SPECIES_ZIGZAGOON);
    EXPECT(!IsLegendaryEncounterLost(SPECIES_ZIGZAGOON));
    EXPECT(!IsLegendaryEncounterLost(SPECIES_NONE));
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    memset(gSaveBlock2Ptr->pokedex.lostLegendaryEncounters, 0, sizeof(gSaveBlock2Ptr->pokedex.lostLegendaryEncounters));
    EXPECT(!IsLegendaryEncounterLost(SPECIES_REGIROCK));
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    for (enum LegendarySignId id = 0; id < LEGENDARY_SIGN_COUNT; id++)
    {
        EXPECT(IsLegendaryEncounterLost(gLegendarySignDefinitions[id].species));
        EXPECT(!IsLegendarySignCaught(id));
    }
    for (u32 i = 0; i < ARRAY_COUNT(nativeSpecies); i++)
        EXPECT(IsLegendaryEncounterLost(nativeSpecies[i]));
    EXPECT(IsLegendaryEncounterLost(SPECIES_GROUDON_PRIMAL));
    EXPECT(IsLegendaryEncounterLost(SPECIES_URSHIFU_SINGLE_STRIKE));
    EXPECT(IsLegendaryEncounterLost(SPECIES_LUNALA));
}

TEST("Individual Devon research gates every Sign acquisition independently")
{
    for (enum LegendarySignId id = 0; id < LEGENDARY_SIGN_COUNT; id++)
    {
        const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[id];
        ResetSignState();
        SetSignPrerequisites(sign, TRUE);
        BringRequiredPartner(sign);
        EXPECT(!CanAcquireLegendarySignSpecies(sign->species));
        gSpecialVar_0x8004 = id;
        ResearchSelectedLegendarySign();
        EXPECT_EQ(gSpecialVar_Result, 2);
        EXPECT(CanAcquireLegendarySignSpecies(sign->species));
        EXPECT_EQ(StringCompare(gStringVar3, COMPOUND_STRING("an unknown place")) == 0, FALSE);
        for (enum LegendarySignId other = 0; other < LEGENDARY_SIGN_COUNT; other++)
            if (other != id)
                EXPECT(!IsLegendarySignUnlocked(other));
        MarkLegendarySignCaughtBySpecies(sign->species);
        EXPECT(!CanAcquireLegendarySignSpecies(sign->species));
    }
}

TEST("Phione breeding waits for its own Devon research")
{
    ResetSignState();
    memset(&gSaveBlock1Ptr->daycare, 0, sizeof(gSaveBlock1Ptr->daycare));
    CreateBoxMon(&gSaveBlock1Ptr->daycare.mons[0].mon, SPECIES_MANAPHY, 30, 0, OTID_STRUCT_PLAYER_ID);
    CreateBoxMon(&gSaveBlock1Ptr->daycare.mons[1].mon, SPECIES_DITTO, 30, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GetDaycareCompatibilityScore(&gSaveBlock1Ptr->daycare), PARENTS_INCOMPATIBLE);
    gSpecialVar_0x8004 = LEGENDARY_SIGN_PHIONE;
    ResearchSelectedLegendarySign();
    EXPECT_EQ(gSpecialVar_Result, 2);
    EXPECT_GT(GetDaycareCompatibilityScore(&gSaveBlock1Ptr->daycare), PARENTS_INCOMPATIBLE);
}

TEST("Game Corner legendary prizes explain missing research before giving a Pokemon")
{
    ResetSignState();
    FlagClear(FLAG_RECEIVED_GAME_CORNER_POIPOLE);
    gSpecialVar_0x8004 = SPECIES_POIPOLE;
    GiveEmeraldChampionsGameCornerPokemon();
    EXPECT_EQ(gSpecialVar_Result, EC_GAME_CORNER_PRIZE_NEEDS_RESEARCH);
    EXPECT(!FlagGet(FLAG_RECEIVED_GAME_CORNER_POIPOLE));
    EXPECT(!IsLegendarySignCaught(LEGENDARY_SIGN_POIPOLE));
    gSpecialVar_0x8004 = LEGENDARY_SIGN_POIPOLE;
    ResearchSelectedLegendarySign();
    EXPECT_EQ(gSpecialVar_Result, 2);
    gSpecialVar_0x8004 = SPECIES_POIPOLE;
    GiveEmeraldChampionsGameCornerPokemon();
    EXPECT_EQ(gSpecialVar_Result, MON_GIVEN_TO_PARTY);
    EXPECT(FlagGet(FLAG_RECEIVED_GAME_CORNER_POIPOLE));
    EXPECT(IsLegendarySignCaught(LEGENDARY_SIGN_POIPOLE));
}

TEST("Devon's research menu includes translated Signs for later review")
{
    bool32 foundTranslated = FALSE;
    bool32 foundUntranslated = FALSE;
    ResetSignState();
    for (enum LegendarySignId id = 0; id < LEGENDARY_SIGN_COUNT; id++)
        SetSignPrerequisites(&gLegendarySignDefinitions[id], TRUE);
    UnlockLegendarySign(LEGENDARY_SIGN_COBALION);
    BuildLegendarySignResearchMenu();
    EXPECT_GT(gSpecialVar_Result, 0);
    EXPECT_EQ(MultichoiceDynamic_StackSize(), gSpecialVar_Result);
    for (u32 i = 0; i < MultichoiceDynamic_StackSize(); i++)
    {
        struct ListMenuItem *item = MultichoiceDynamic_PeekElementAt(i);
        if (item->id == LEGENDARY_SIGN_COBALION)
        {
            foundTranslated = TRUE;
            EXPECT_EQ(StringCompare(item->name, COMPOUND_STRING("Cobalion (R)")), 0);
        }
        if (item->id == LEGENDARY_SIGN_PHIONE)
            foundUntranslated = TRUE;
        Free((void *)item->name);
    }
    MultichoiceDynamic_DestroyStack();
    EXPECT(foundTranslated);
    EXPECT(foundUntranslated);
}

TEST("An earned Circuit reward waits for its own Devon research without another win")
{
    ResetSignState();
    VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 2);
    ChampionsCircuitTryGiveReward();
    EXPECT_EQ(gSpecialVar_Result, 4);
    EXPECT(!IsLegendarySignCaught(LEGENDARY_SIGN_CALYREX));
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS), 2);
    gSpecialVar_0x8004 = LEGENDARY_SIGN_CALYREX;
    ResearchSelectedLegendarySign();
    EXPECT_EQ(gSpecialVar_Result, 2);
    ChampionsCircuitTryGiveReward();
    EXPECT_EQ(gSpecialVar_Result, 1);
    EXPECT(IsLegendarySignCaught(LEGENDARY_SIGN_CALYREX));
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS), 2);
}

TEST("Reloading a save from before a legendary attempt restores its availability")
{
    ResetSignState();
    UnlockLegendarySign(LEGENDARY_SIGN_MEWTWO);
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_MEWTWO));
    EXPECT_EQ(TrySavingData(SAVE_NORMAL), SAVE_STATUS_OK);
    MarkLegendaryEncounterLost(SPECIES_MEWTWO);
    EXPECT(IsLegendaryEncounterLost(SPECIES_MEWTWO));
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_MEWTWO));
    EXPECT_EQ(LoadGameSave(SAVE_NORMAL), SAVE_STATUS_OK);
    EXPECT(!IsLegendaryEncounterLost(SPECIES_MEWTWO));
    EXPECT(!IsLegendarySignCaught(LEGENDARY_SIGN_MEWTWO));
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_MEWTWO));
    gSpecialVar_0x8004 = LEGENDARY_SIGN_MEWTWO;
    EXPECT(ShouldShowSelectedLegendarySignObject());
    EXPECT(!FlagGet(FLAG_EC_CAUGHT_MEWTWO));
}
