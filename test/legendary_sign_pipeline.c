#include "global.h"
#include "caps.h"
#include "event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/items.h"
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
#include "constants/maps.h"
#include "constants/region_map_sections.h"
#include "constants/moves.h"
#include "random.h"
#include "string_util.h"
#include "text.h"
#include "constants/characters.h"

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

// These exercise production selection and quest state. They do not simulate
// walking, script choreography, capture animations, or campaign traversal.
TEST("Sweet Scent reverses species totals with duplicates and ties while preserving legendary slots")
{
    struct WildPokemon mons[NUM_LAND_MONS_ENCOUNTER_SLOTS] = {
        {5, 5, SPECIES_MAGIKARP}, {5, 5, SPECIES_MAGIKARP},
        {5, 5, SPECIES_GOLDEEN}, {5, 5, SPECIES_TENTACOOL}, {5, 5, SPECIES_WINGULL},
    };
    const enum Species species[] = {SPECIES_MAGIKARP, SPECIES_GOLDEEN, SPECIES_TENTACOOL, SPECIES_WINGULL};
    u32 counts[ARRAY_COUNT(species)] = {0};
    ResetSignState();
    SET_RNG(RNG_WILD_MON_TARGET, 0);
    // Water totals 60/18/12/10 reverse to 10/12/18/60, despite duplicate slots.
    for (u32 roll = 0; roll < 100; roll++)
    {
        SET_RNG(RNG_NONE, roll);
        u32 index = ChooseSweetScentWildMonIndex(mons, WILD_AREA_WATER);
        EXPECT_LT(index, NUM_WATER_MONS_ENCOUNTER_SLOTS);
        for (u32 i = 0; i < ARRAY_COUNT(species); i++)
            counts[i] += mons[index].species == species[i];
    }
    EXPECT_EQ(counts[0], 10);
    EXPECT_EQ(counts[1], 12);
    EXPECT_EQ(counts[2], 18);
    EXPECT_EQ(counts[3], 60);

    // Land totals 36/36/28 become 32/32/36: identical rarity gets equal odds.
    for (u32 i = 0; i < NUM_LAND_MONS_ENCOUNTER_SLOTS; i++)
        mons[i].species = i < 3 ? species[0] : i < 7 ? species[1] : species[2];
    memset(counts, 0, sizeof(counts));
    for (u32 choice = 0; choice < 2; choice++)
    {
        SET_RNG(RNG_WILD_MON_TARGET, choice);
        for (u32 roll = 0; roll < 100; roll++)
        {
            SET_RNG(RNG_NONE, roll);
            u32 index = ChooseSweetScentWildMonIndex(mons, WILD_AREA_LAND);
            EXPECT_LT(index, NUM_LAND_MONS_ENCOUNTER_SLOTS);
            for (u32 i = 0; i < 3; i++)
                counts[i] += mons[index].species == species[i];
        }
    }
    EXPECT_EQ(counts[0], 64);
    EXPECT_EQ(counts[1], 64);
    EXPECT_EQ(counts[2], 72);

    // An ordinary-table Ultra Beast keeps its 35% slot, outside the reversal.
    mons[0].species = SPECIES_PHEROMOSA;
    for (u32 i = 1; i < NUM_WATER_MONS_ENCOUNTER_SLOTS; i++)
        mons[i].species = species[i - 1];
    SET_RNG(RNG_WILD_MON_TARGET, 0);
    u32 legendaryCount = 0;
    memset(counts, 0, sizeof(counts));
    for (u32 roll = 0; roll < 100; roll++)
    {
        SET_RNG(RNG_NONE, roll);
        u32 index = ChooseSweetScentWildMonIndex(mons, WILD_AREA_WATER);
        legendaryCount += index == 0;
        for (u32 i = 0; i < ARRAY_COUNT(species); i++)
            counts[i] += mons[index].species == species[i];
    }
    EXPECT_EQ(legendaryCount, 35);
    EXPECT_EQ(counts[0], 10);
    EXPECT_EQ(counts[1], 12);
    EXPECT_EQ(counts[2], 18);
    EXPECT_EQ(counts[3], 25);
    MarkLegendarySignCaughtBySpecies(SPECIES_PHEROMOSA);
    for (u32 roll = 0; roll < 100; roll++)
    {
        SET_RNG(RNG_NONE, roll);
        EXPECT_NE(ChooseSweetScentWildMonIndex(mons, WILD_AREA_WATER), 0);
    }
    // A habitat with a single ordinary species remains usable after capture.
    for (u32 i = 1; i < NUM_WATER_MONS_ENCOUNTER_SLOTS; i++)
        mons[i].species = SPECIES_MAGIKARP;
    EXPECT_EQ(mons[ChooseSweetScentWildMonIndex(mons, WILD_AREA_WATER)].species, SPECIES_MAGIKARP);
}

TEST("Wild residents leave encounter pools after capture and signs record completion")
{
    const enum Species expected[] = {SPECIES_RAIKOU, SPECIES_TAPU_KOKO, SPECIES_THUNDURUS};
    ResetSignState();
    for (u32 badge = 0; badge < 8; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    FlagClear(FLAG_HIDE_ROUTE_119_TEAM_AQUA);
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE110);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE110);
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_RAIKOU));
    for (u32 unlocked = 0; unlocked < 2; unlocked++)
    {
        if (unlocked)
        {
            for (u32 badge = 0; badge < 5; badge++)
                FlagSet(FLAG_BADGE01_GET + badge);
            FlagSet(FLAG_HIDE_ROUTE_119_TEAM_AQUA);
        }
        for (u32 roll = 0; roll < 100; roll++)
        {
            SET_RNG(RNG_NONE, roll);
            EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, FALSE),
                roll < 3 ? SPECIES_RAIKOU : roll < 6 ? SPECIES_TAPU_KOKO
                : unlocked && roll == 6 ? SPECIES_THUNDURUS : SPECIES_NONE);
            for (u32 choice = 0; choice < 2 + unlocked; choice++)
            {
                SET_RNG(RNG_WILD_MON_TARGET, choice);
                EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, TRUE),
                    roll < 25 ? expected[choice] : SPECIES_NONE);
            }
        }
    }
    EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_WATER, TRUE), SPECIES_NONE);
    MarkLegendarySignCaughtBySpecies(SPECIES_RAIKOU);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_RAIKOU));
    // The remaining native keeps its 3% slot; the quest keeps its 1% slot.
    // Sweet Scent redistributes its total 25% over the two remaining species.
    for (u32 roll = 0; roll < 100; roll++)
    {
        SET_RNG(RNG_NONE, roll);
        EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, FALSE),
            roll < 3 ? SPECIES_TAPU_KOKO : roll == 3 ? SPECIES_THUNDURUS : SPECIES_NONE);
        for (u32 choice = 0; choice < 2; choice++)
        {
            SET_RNG(RNG_WILD_MON_TARGET, choice);
            EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, TRUE),
                roll < 25 ? expected[choice + 1] : SPECIES_NONE);
        }
    }
    MarkLegendarySignCaughtBySpecies(SPECIES_THUNDURUS);
    SET_RNG(RNG_NONE, 0);
    SET_RNG(RNG_WILD_MON_TARGET, 0);
    EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, TRUE), SPECIES_TAPU_KOKO);
    MarkLegendarySignCaughtBySpecies(SPECIES_TAPU_KOKO);
    EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, FALSE), SPECIES_NONE);
    EXPECT_EQ(ChooseRareWildLegendarySpecies(WILD_AREA_LAND, TRUE), SPECIES_NONE);
    ResetSignState();
    for (enum LegendarySignId id = 0; id < LEGENDARY_SIGN_COUNT; id++)
    {
        const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[id];
        if (sign->source != LEGENDARY_SOURCE_NATIVE_WILD)
            continue;
        EXPECT(CanAcquireLegendarySignSpecies(sign->species));
        MarkLegendarySignCaughtBySpecies(sign->species);
        EXPECT(!CanAcquireLegendarySignSpecies(sign->species));
        gSaveBlock1Ptr->location.mapGroup = sign->mapId >> 8;
        gSaveBlock1Ptr->location.mapNum = sign->mapId & 255;
        BufferCurrentMapRouteSignSpecies();
        u32 length = StringLength(gStringVar4);
        u8 name[64];
        StringCopy(name, GetLegendaryDisplayName(sign->species));
        StringAppend(name, COMPOUND_STRING(" (Caught)"));
        u32 nameLength = StringLength(name);
        bool32 found = FALSE;
        EXPECT_LT(length, sizeof(gStringVar4));
        for (u32 i = 0; i + nameLength <= length; i++)
            found |= StringCompareN(gStringVar4 + i, name, nameLength) == 0;
        EXPECT(found);
        // Long regional names plus the marker must still fit the sign window.
        u8 line[sizeof(gStringVar4)];
        u32 lineLength = 0;
        for (u32 i = 0; ; i++)
        {
            u8 c = gStringVar4[i];
            if (c == EOS || c == CHAR_NEWLINE || c == CHAR_PROMPT_SCROLL || c == CHAR_PROMPT_CLEAR)
            {
                line[lineLength] = EOS;
                EXPECT_LE(GetStringWidth(FONT_NORMAL, line, 0), 200);
                lineLength = 0;
                if (c == EOS)
                    break;
            }
            else
                line[lineLength++] = c;
        }
        gSpecialVar_0x8004 = id;
        ResearchSelectedLegendarySign();
        EXPECT_EQ(gSpecialVar_Result, 4);
    }
}

TEST("Rare wild NPC discovery requires local help and survives depositing the partner")
{
    ResetSignState();
    for (u32 badge = 0; badge < 8; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_DEWFORD_MEADOW);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_DEWFORD_MEADOW);
    gSpecialVar_0x8004 = LEGENDARY_SIGN_MELOETTA;
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_JIGGLYPUFF, 10, 0, OTID_STRUCT_PLAYER_ID);
    for (u32 move = 0; move < MAX_MON_MOVES; move++)
        SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_NONE, move);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_SING, 0);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT(!IsLegendarySignUnlocked(LEGENDARY_SIGN_MELOETTA));
    FlagSet(FLAG_BADGE01_GET);
    FlagSet(FLAG_BADGE02_GET);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_POUND, 0);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT(!IsLegendarySignUnlocked(LEGENDARY_SIGN_MELOETTA));
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_SING, 0);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT(IsLegendarySignUnlocked(LEGENDARY_SIGN_MELOETTA));
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    ZeroPlayerPartyMons();
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_MELOETTA));
    TryUnlockLocalLegendaryDiscovery();
    EXPECT_EQ(gSpecialVar_Result, FALSE);

    for (u32 badge = 0; badge < 5; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    gSpecialVar_0x8004 = LEGENDARY_SIGN_LANDORUS;
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_CASTFORM, 25, 0, OTID_STRUCT_PLAYER_ID);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT(!IsLegendarySignUnlocked(LEGENDARY_SIGN_LANDORUS));
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE111_RUINS_EXTERIOR);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE111_RUINS_EXTERIOR);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT(IsLegendarySignUnlocked(LEGENDARY_SIGN_LANDORUS));
    ZeroPlayerPartyMons();
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_LANDORUS));
}

static bool32 GuideTextContains(const u8 *needle)
{
    u32 length = StringLength(gStringVar4);
    u32 needleLength = StringLength(needle);
    for (u32 i = 0; i + needleLength <= length; i++)
        if (StringCompareN(gStringVar4 + i, needle, needleLength) == 0)
            return TRUE;
    return FALSE;
}

TEST("Center local guide reflects quest progress without unlocking discoveries")
{
    ResetSignState();
    for (u32 badge = 0; badge < 8; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    gMapHeader.regionMapSectionId = MAPSEC_DEWFORD_TOWN;
    gSpecialVar_0x8004 = 0;
    BufferNextCenterLegendaryLead();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT(GuideTextContains(COMPOUND_STRING("SING")));
    EXPECT(GuideTextContains(COMPOUND_STRING("Gym Badges required:\n2.")));
    EXPECT(!IsLegendarySignUnlocked(LEGENDARY_SIGN_MELOETTA));
    FlagSet(FLAG_BADGE01_GET);
    FlagSet(FLAG_BADGE02_GET);
    UnlockLegendarySign(LEGENDARY_SIGN_MELOETTA);
    gSpecialVar_0x8004 = 0;
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("You've met its requirements!")));
    MarkLegendarySignCaughtBySpecies(SPECIES_MELOETTA);
    gSpecialVar_0x8004 = 0;
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("You've already found")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("SING")));
    gMapHeader.regionMapSectionId = MAPSEC_OLDALE_TOWN;
    gSpecialVar_0x8004 = 0;
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("SHAYMIN")));
    EXPECT(GuideTextContains(COMPOUND_STRING("3%")));
    BufferNextCenterLegendaryLead();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    MarkLegendarySignCaughtBySpecies(SPECIES_SHAYMIN);
    gSpecialVar_0x8004 = 0;
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("You've already found")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("3%")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("find more")));
    // Regional Moltres shares the national Dex entry, but not the Ember Path scene.
    FlagClear(FLAG_EC_CAUGHT_MOLTRES);
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_MOLTRES_GALAR), FLAG_SET_CAUGHT);
    gMapHeader.regionMapSectionId = MAPSEC_LAVARIDGE_TOWN;
    gSpecialVar_0x8004 = 0;
    do { BufferNextCenterLegendaryLead(); } while (gSpecialVar_Result && !GuideTextContains(COMPOUND_STRING("MOLTRES")));
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT(GuideTextContains(COMPOUND_STRING("EMBER PATH")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("You've already found")));
    FlagSet(FLAG_EC_CAUGHT_MOLTRES);
    gSpecialVar_0x8004--; // Revisit the same lead after the original encounter is caught.
    BufferNextCenterLegendaryLead();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT(GuideTextContains(COMPOUND_STRING("You've already found")));
    const u16 cities[] = {MAPSEC_OLDALE_TOWN, MAPSEC_PETALBURG_CITY, MAPSEC_DEWFORD_TOWN,
        MAPSEC_RUSTBORO_CITY, MAPSEC_SLATEPORT_CITY, MAPSEC_MAUVILLE_CITY,
        MAPSEC_VERDANTURF_TOWN, MAPSEC_LAVARIDGE_TOWN, MAPSEC_FALLARBOR_TOWN,
        MAPSEC_FORTREE_CITY, MAPSEC_LILYCOVE_CITY, MAPSEC_MOSSDEEP_CITY,
        MAPSEC_SOOTOPOLIS_CITY, MAPSEC_PACIFIDLOG_TOWN, MAPSEC_EVER_GRANDE_CITY,
        MAPSEC_BATTLE_FRONTIER};
    for (u32 city = 0; city < ARRAY_COUNT(cities); city++)
    {
        u32 pages = 0;
        gMapHeader.regionMapSectionId = cities[city];
        gSpecialVar_0x8004 = 0;
        while (TRUE)
        {
            BufferNextCenterLegendaryLead();
            if (!gSpecialVar_Result)
                break;
            EXPECT_LT(++pages, 20);
            EXPECT_LT(StringLength(gStringVar4), sizeof(gStringVar4));
            u8 line[sizeof(gStringVar4)];
            u32 length = 0;
            for (u32 i = 0; ; i++)
            {
                u8 c = gStringVar4[i];
                if (c == EOS || c == CHAR_NEWLINE || c == CHAR_PROMPT_SCROLL || c == CHAR_PROMPT_CLEAR)
                {
                    line[length] = EOS;
                    EXPECT_LE(GetStringWidth(FONT_NORMAL, line, 0), 200);
                    length = 0;
                    if (c == EOS)
                        break;
                }
                else
                    line[length++] = c;
            }
        }
        EXPECT_GT(pages, 0);
    }
}

TEST("Legendary scene retries require the Regis and island Latis use the live cap")
{
    // Route roster signs must resolve in Emerald, not only in FireRed/LeafGreen.
    const struct ObjectEventGraphicsInfo *sign = GetObjectEventGraphicsInfo(OBJ_EVENT_GFX_SIGN);
    EXPECT(sign != NULL);
    EXPECT(sign->images != NULL);
    ResetSignState();
    for (u32 badge = 0; badge < 8; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    UnlockLegendarySign(LEGENDARY_SIGN_REGIGIGAS);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_REGIGIGAS));
    const enum Species regis[] = {SPECIES_REGIROCK, SPECIES_REGICE, SPECIES_REGISTEEL};
    for (u32 slot = 0; slot < ARRAY_COUNT(regis); slot++)
        CreateMon(&gParties[B_TRAINER_PLAYER][slot], regis[slot], 50, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_REGIGIGAS));
    ZeroMonData(&gParties[B_TRAINER_PLAYER][2]);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_REGIGIGAS));
    for (u32 badge = 0; badge < 8; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    const enum Species latis[] = {SPECIES_LATIAS, SPECIES_LATIOS};
    for (u32 i = 0; i < ARRAY_COUNT(latis); i++)
    {
        gSpecialVar_0x8004 = latis[i];
        gSpecialVar_0x8005 = 0;
        CreateEmeraldChampionsStaticLegendaryEncounter();
        struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][0];
        EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), latis[i]);
        EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), GetCurrentLevelCap());
        EXPECT_EQ(GetMonData(mon, MON_DATA_HELD_ITEM), ITEM_SOUL_DEW);
        EXPECT_EQ(GetMonData(mon, MON_DATA_MODERN_FATEFUL_ENCOUNTER), TRUE);
    }
}
