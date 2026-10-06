#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "constants/trainers.h"
#include "script.h"
#include "caps.h"
#include "center_guide.h"
#include "constants/emerald_champions.h"
#include "event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/items.h"
#include "event_data.h"
#include "field_specials.h"
#include "constants/field_specials.h"
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

TEST("Mauville Genesect prize needs eight badges and records its one-time acquisition")
{
    ResetSignState();
    FlagClear(FLAG_RECEIVED_GAME_CORNER_GENESECT);
    for (u32 badge = 0; badge < 8; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    gSpecialVar_0x8004 = SPECIES_GENESECT;

    GiveEmeraldChampionsGameCornerPokemon();
    EXPECT_EQ(gSpecialVar_Result, EC_GAME_CORNER_PRIZE_ALREADY_CAUGHT);
    EXPECT(!FlagGet(FLAG_RECEIVED_GAME_CORNER_GENESECT));

    for (u32 badge = 0; badge < 8; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    GiveEmeraldChampionsGameCornerPokemon();
    EXPECT_EQ(gSpecialVar_Result, MON_GIVEN_TO_PARTY);
    EXPECT(FlagGet(FLAG_RECEIVED_GAME_CORNER_GENESECT));
    EXPECT(IsLegendarySignCaught(LEGENDARY_SIGN_GENESECT));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_GENESECT);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), GetCurrentLevelCap());

    GiveEmeraldChampionsGameCornerPokemon();
    EXPECT_EQ(gSpecialVar_Result, EC_GAME_CORNER_PRIZE_SET_FAILED);
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], 1);
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
            MarkLegendarySignCaughtBySpecies(gLegendaryGates[id].species);
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
    // Water slots are 40/30/20/10. Combining the first two yields
    // 70/20/10; reversal gives 10/20/70. Duplicate slots remain one species.
    for (u32 choice = 1; choice <= 2; choice++)
    {
        SET_RNG(RNG_WILD_MON_TARGET, choice);
        for (u32 roll = 0; roll < 100; roll++)
        {
            SET_RNG(RNG_NONE, roll);
            u32 index = ChooseSweetScentWildMonIndex(&(struct WildPokemonInfo){.wildPokemon = mons}, WILD_AREA_WATER);
            EXPECT_LT(index, NUM_WATER_MONS_ENCOUNTER_SLOTS);
            for (u32 i = 0; i < ARRAY_COUNT(species); i++)
                counts[i] += mons[index].species == species[i];
        }
    }
    EXPECT_EQ(counts[0], 20);
    EXPECT_EQ(counts[1], 40);
    EXPECT_EQ(counts[2], 140);
    EXPECT_EQ(counts[3], 0);

    // Land totals 28/28/44 become 36/36/28: identical rarity gets equal odds.
    // After sorting, tied species occupy indices 1 and 2.
    for (u32 i = 0; i < NUM_LAND_MONS_ENCOUNTER_SLOTS; i++)
        mons[i].species = i < 4 ? species[i % 2] : species[2];
    memset(counts, 0, sizeof(counts));
    for (u32 choice = 1; choice <= 2; choice++)
    {
        SET_RNG(RNG_WILD_MON_TARGET, choice);
        for (u32 roll = 0; roll < 100; roll++)
        {
            SET_RNG(RNG_NONE, roll);
            u32 index = ChooseSweetScentWildMonIndex(&(struct WildPokemonInfo){.wildPokemon = mons}, WILD_AREA_LAND);
            EXPECT_LT(index, NUM_LAND_MONS_ENCOUNTER_SLOTS);
            for (u32 i = 0; i < 3; i++)
                counts[i] += mons[index].species == species[i];
        }
    }
    EXPECT_EQ(counts[0], 72);
    EXPECT_EQ(counts[1], 72);
    EXPECT_EQ(counts[2], 56);

    // Legendary, Ultra Beast and Paradox slots under Sweet Scent are
    // covered in test/wild_slot_odds.c.
}

TEST("A caught gate row closes its wild slot")
{
    ResetSignState();
    for (enum LegendarySignId id = 0; id < LEGENDARY_SIGN_COUNT; id++)
    {
        MarkLegendarySignCaughtBySpecies(gLegendaryGates[id].species);
        EXPECT(IsLegendarySignCaught(id));
        EXPECT(!CanAcquireLegendarySignSpecies(gLegendaryGates[id].species));
        EXPECT(!IsWildSlotSpeciesAcquirable(gLegendaryGates[id].species));
    }
    ResetSignState();
}

TEST("Gates follow badges, milestone and family from the gate row")
{
    ResetSignState();
    u8 savedCaught[sizeof(gSaveBlock1Ptr->dexCaught)];
    memcpy(savedCaught, gSaveBlock1Ptr->dexCaught, sizeof(savedCaught));
    memset(gSaveBlock1Ptr->dexCaught, 0, sizeof(savedCaught));
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    FlagClear(FLAG_GOT_TM24_FROM_WATTSON);
    FlagClear(FLAG_RECEIVED_MAGMA_EMBLEM_MT_PYRE_SUMMIT);

    // Zeraora: five badges, then Wattson's New Mauville receipt.
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_ZERAORA));
    for (u32 badge = 0; badge < 5; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_ZERAORA));
    FlagSet(FLAG_GOT_TM24_FROM_WATTSON);
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_ZERAORA));

    // Cresselia: every badge and the milestone, then a Darkrai record.
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    FlagSet(FLAG_RECEIVED_MAGMA_EMBLEM_MT_PYRE_SUMMIT);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_CRESSELIA));
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_DARKRAI), FLAG_SET_CAUGHT);
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_CRESSELIA));

    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    FlagClear(FLAG_GOT_TM24_FROM_WATTSON);
    FlagClear(FLAG_RECEIVED_MAGMA_EMBLEM_MT_PYRE_SUMMIT);
    memcpy(gSaveBlock1Ptr->dexCaught, savedCaught, sizeof(savedCaught));
    ResetSignState();
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
    EXPECT(!IsLegendarySignUnlocked(LEGENDARY_SIGN_MELOETTA));
    FlagSet(FLAG_BADGE03_GET);
    FlagSet(FLAG_BADGE04_GET);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT(IsLegendarySignUnlocked(LEGENDARY_SIGN_MELOETTA));
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    ZeroPlayerPartyMons();
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_MELOETTA));
    TryUnlockLocalLegendaryDiscovery();
    EXPECT_EQ(gSpecialVar_Result, FALSE);

    // Landorus's quest opens with Groudon's awakening.
    for (u32 badge = 0; badge < 6; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    FlagSet(FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT);
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
    FlagClear(FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT);
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
    gSpecialVar_0x8005 = CENTER_GUIDE_TOPIC_LEGENDS;
    gSpecialVar_0x8004 = 0;
    BufferNextCenterLegendaryLead();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT(GuideTextContains(COMPOUND_STRING("Sing")));
    // The lead states its badge requirement in prose; nothing is appended.
    EXPECT(GuideTextContains(COMPOUND_STRING("After four")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("Gym Badges required")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("You're ready!")));
    EXPECT(!IsLegendarySignUnlocked(LEGENDARY_SIGN_MELOETTA));
    FlagSet(FLAG_BADGE01_GET);
    FlagSet(FLAG_BADGE02_GET);
    FlagSet(FLAG_BADGE03_GET);
    FlagSet(FLAG_BADGE04_GET);
    UnlockLegendarySign(LEGENDARY_SIGN_MELOETTA);
    gSpecialVar_0x8004 = 0;
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("You're ready!")));
    MarkLegendarySignCaughtBySpecies(SPECIES_MELOETTA);
    gSpecialVar_0x8004 = 0;
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("You've already found")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("Sing")));
    gMapHeader.regionMapSectionId = MAPSEC_OLDALE_TOWN;
    gSpecialVar_0x8004 = 0;
    BufferNextCenterLegendaryLead();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT(GuideTextContains(COMPOUND_STRING("Cosmog")));
    EXPECT(GuideTextContains(COMPOUND_STRING("Birch")));
    EXPECT(GuideTextContains(COMPOUND_STRING("Hall of")));
    EXPECT(!IsLegendarySignUnlocked(LEGENDARY_SIGN_COSMOG));
    BufferNextCenterLegendaryLead();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    // Shaymin's lead moved to Verdanturf with its Route 117 home.
    gMapHeader.regionMapSectionId = MAPSEC_VERDANTURF_TOWN;
    gSpecialVar_0x8004 = 0;
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("Shaymin")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("%")));
    MarkLegendarySignCaughtBySpecies(SPECIES_SHAYMIN);
    gSpecialVar_0x8004 = 0;
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("You've already found")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("find more")));
    // Regional Moltres shares the national Dex entry, but not the Ember Path scene.
    FlagClear(FLAG_EC_CAUGHT_MOLTRES);
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_MOLTRES_GALAR), FLAG_SET_CAUGHT);
    gMapHeader.regionMapSectionId = MAPSEC_LAVARIDGE_TOWN;
    gSpecialVar_0x8004 = 0;
    do { BufferNextCenterLegendaryLead(); } while (gSpecialVar_Result && !GuideTextContains(COMPOUND_STRING("Moltres")));
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT(GuideTextContains(COMPOUND_STRING("Ember Path")));
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
        MAPSEC_SOOTOPOLIS_CITY, MAPSEC_PACIFIDLOG_TOWN, MAPSEC_EVER_GRANDE_CITY};
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

TEST("Regigigas wakes for the three Regis' Pokedex records, not a three-Legendary party")
{
    // Route roster signs must resolve to real sign graphics.
    const struct ObjectEventGraphicsInfo *sign = GetObjectEventGraphicsInfo(OBJ_EVENT_GFX_SIGN);
    EXPECT(sign != NULL);
    EXPECT(sign->images != NULL);
    ResetSignState();
    u8 savedCaught[sizeof(gSaveBlock1Ptr->dexCaught)];
    memcpy(savedCaught, gSaveBlock1Ptr->dexCaught, sizeof(savedCaught));
    memset(gSaveBlock1Ptr->dexCaught, 0, sizeof(savedCaught));
    for (u32 badge = 0; badge < 8; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    const enum Species regis[] = {SPECIES_REGIROCK, SPECIES_REGICE, SPECIES_REGISTEEL};
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_REGIGIGAS));
    for (u32 i = 0; i < ARRAY_COUNT(regis); i++)
    {
        GetSetPokedexFlag(SpeciesToNationalPokedexNum(regis[i]), FLAG_SET_CAUGHT);
        EXPECT_EQ(CanAcquireLegendarySignSpecies(SPECIES_REGIGIGAS), i + 1 == ARRAY_COUNT(regis));
    }
    // The party holds none of them: the restricted-party rule allows only one.
    EXPECT(!PlayerPartyHasSpeciesFamily(SPECIES_REGIROCK));
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_REGIGIGAS));
    // An unlocked bit from an older save does not bypass the records.
    memset(gSaveBlock1Ptr->dexCaught, 0, sizeof(savedCaught));
    UnlockLegendarySign(LEGENDARY_SIGN_REGIGIGAS);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_REGIGIGAS));
    for (u32 badge = 0; badge < 8; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    memcpy(gSaveBlock1Ptr->dexCaught, savedCaught, sizeof(savedCaught));
    ResetSignState();
}

extern const u8 SealedChamber_InnerRoom_EventScript_CheckRegigigasResult[];
extern const u8 Common_EventScript_VisibleLegendaryCaught[];
extern const u8 ShoalCave_LowTideIceRoom_EventScript_CheckArticunoResult[];
extern const u8 NewMauville_Inside_EventScript_CheckZapdosResult[];
extern const u8 AlteringCave_B1F_EventScript_CheckMewtwoResult[];
extern const u8 Common_EventScript_LegendaryResting[];
extern const u8 Common_EventScript_LegendaryGone[];
extern ScrCmdFunc gScriptCmdTable[];
extern ScrCmdFunc gScriptCmdTableEnd[];

extern const u8 SealedChamber_InnerRoom_EventScript_Regigigas[];
extern const u8 ShoalCave_LowTideIceRoom_EventScript_Articuno[];
extern const u8 NewMauville_Inside_EventScript_Zapdos[];
extern const u8 AlteringCave_B1F_EventScript_Mewtwo[];

TEST("Visible legendary events: a knockout is lost, other uncaught outcomes retreat, capture uses the map hide flag")
{
    const struct {u16 map; enum Species species; const u8 *gate, *entry;} cases[] = {
        {MAP_SEALED_CHAMBER_INNER_ROOM, SPECIES_REGIGIGAS, SealedChamber_InnerRoom_EventScript_CheckRegigigasResult, SealedChamber_InnerRoom_EventScript_Regigigas},
        {MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM, SPECIES_ARTICUNO, ShoalCave_LowTideIceRoom_EventScript_CheckArticunoResult, ShoalCave_LowTideIceRoom_EventScript_Articuno},
        {MAP_NEW_MAUVILLE_INSIDE, SPECIES_ZAPDOS, NewMauville_Inside_EventScript_CheckZapdosResult, NewMauville_Inside_EventScript_Zapdos},
        {MAP_ALTERING_CAVE_B1F, SPECIES_MEWTWO, AlteringCave_B1F_EventScript_CheckMewtwoResult, AlteringCave_B1F_EventScript_Mewtwo},
    };
    const u8 outcomes[] = {B_OUTCOME_WON, B_OUTCOME_RAN, B_OUTCOME_PLAYER_TELEPORTED,
        B_OUTCOME_MON_FLED, B_OUTCOME_MON_TELEPORTED, B_OUTCOME_CAUGHT};
    for (u32 encounter = 0; encounter < ARRAY_COUNT(cases); encounter++)
    {
        for (u32 i = 0; i < ARRAY_COUNT(outcomes); i++)
        {
            struct ScriptContext ctx;
            InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
            SetupBytecodeScript(&ctx, cases[encounter].gate);
            gBattleOutcome = outcomes[i];
            FlagSet(FLAG_SYS_CTRL_OBJ_DELETE);
            for (u32 step = 0; step < 16
              && ctx.scriptPtr != Common_EventScript_LegendaryResting
              && ctx.scriptPtr != Common_EventScript_LegendaryGone
              && ctx.scriptPtr != Common_EventScript_VisibleLegendaryCaught; step++)
            {
                u8 command = *ctx.scriptPtr++;
                EXPECT(!ctx.cmdTable[command](&ctx));
            }
            EXPECT_EQ(ctx.scriptPtr, outcomes[i] == B_OUTCOME_CAUGHT ? Common_EventScript_VisibleLegendaryCaught
                : outcomes[i] == B_OUTCOME_WON ? Common_EventScript_LegendaryGone
                : Common_EventScript_LegendaryResting);
            EXPECT(!FlagGet(FLAG_SYS_CTRL_OBJ_DELETE));
            // The knockout path reports the species it loses.
            if (outcomes[i] == B_OUTCOME_WON)
                EXPECT_EQ(VarGet(VAR_0x8004), cases[encounter].species);
        }
        ResetSignState();
        const struct MapHeader *map = Overworld_GetMapHeaderByGroupAndId(
            MAP_GROUP(cases[encounter].map), MAP_NUM(cases[encounter].map));
        // Resolve the real encounter object through its script, independent of artwork.
        const struct ObjectEventTemplate *object = NULL;
        for (u32 i = 0; i < map->events->objectEventCount; i++)
            if (map->events->objectEvents[i].script == cases[encounter].entry)
                object = &map->events->objectEvents[i];
        EXPECT(object != NULL);
        FlagClear(object->flagId);
        MarkLegendarySignCaughtBySpecies(cases[encounter].species);
        EXPECT(FlagGet(object->flagId));
        EXPECT(IsLegendarySignCaught(GetLegendarySignIdBySpecies(cases[encounter].species)));
    }
}

// A knockout saves the object's hide flag (removeobject) or the shrine flag
// (setflag) without a capture: the Pokémon is lost for good.
TEST("Static legendary knockout: lost for good, never re-armed, and every lead says so")
{
    static const struct {enum LegendarySignId id; u16 flag;} visible[] = {
        {LEGENDARY_SIGN_REGIGIGAS, FLAG_EC_CAUGHT_REGIGIGAS},
        {LEGENDARY_SIGN_ARTICUNO, FLAG_EC_CAUGHT_ARTICUNO},
        {LEGENDARY_SIGN_ZAPDOS, FLAG_EC_CAUGHT_ZAPDOS},
        {LEGENDARY_SIGN_MEWTWO, FLAG_EC_CAUGHT_MEWTWO},
        {LEGENDARY_SIGN_PECHARUNT, FLAG_EC_CAUGHT_PECHARUNT},
        {LEGENDARY_SIGN_REGIROCK, FLAG_DEFEATED_REGIROCK},
        {LEGENDARY_SIGN_RAYQUAZA, FLAG_DEFEATED_RAYQUAZA},
        {LEGENDARY_SIGN_REGICE, FLAG_DEFEATED_REGICE}, // Kyogre/Groudon would grant their Orbs here.
        {LEGENDARY_SIGN_JIRACHI, FLAG_DEFEATED_JIRACHI},
        {LEGENDARY_SIGN_MOLTRES, FLAG_DEFEATED_MOLTRES},
    };
    u8 savedCaught[sizeof(gSaveBlock1Ptr->dexCaught)];
    memcpy(savedCaught, gSaveBlock1Ptr->dexCaught, sizeof(savedCaught));
    memset(gSaveBlock1Ptr->dexCaught, 0, sizeof(savedCaught));
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    FlagSet(FLAG_SYS_GAME_CLEAR);
    FlagSet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
    for (u32 i = 0; i < ARRAY_COUNT(visible); i++)
    {
        ResetSignState();
        FlagClear(visible[i].flag);
        gSpecialVar_0x8004 = visible[i].id;
        GetSelectedLegendarySignState();
        EXPECT_NE(gSpecialVar_Result, 4);
        FlagSet(visible[i].flag); // Knocked out: the map hides it for good.
        gSpecialVar_0x8004 = visible[i].id;
        EXPECT_EQ(GetSelectedLegendarySignState(), 4);
        TryUnlockSelectedLegendarySign();
        EXPECT_EQ(gSpecialVar_Result, 6);
        EXPECT(!CanAcquireLegendarySignSpecies(gLegendaryGates[visible[i].id].species));
        UnlockLegendarySign(visible[i].id); // Never re-arms the object.
        EXPECT(FlagGet(visible[i].flag));
        // A capture sets the same flag; the caught bit wins.
        MarkLegendarySignCaughtBySpecies(gLegendaryGates[visible[i].id].species);
        gSpecialVar_0x8004 = visible[i].id;
        EXPECT_EQ(GetSelectedLegendarySignState(), 2);
        FlagClear(visible[i].flag);
    }

    // Darkrai's shrine flag starts set: only an unlocked, uncaught shrine is lost.
    ResetSignState();
    FlagSet(FLAG_HIDE_LEGENDARY_SIGN_DARKRAI);
    gSpecialVar_0x8004 = LEGENDARY_SIGN_DARKRAI;
    EXPECT_NE(GetSelectedLegendarySignState(), 4);
    UnlockLegendarySign(LEGENDARY_SIGN_DARKRAI);
    EXPECT(!FlagGet(FLAG_HIDE_LEGENDARY_SIGN_DARKRAI));
    FlagSet(FLAG_HIDE_LEGENDARY_SIGN_DARKRAI);
    gSpecialVar_0x8004 = LEGENDARY_SIGN_DARKRAI;
    EXPECT_EQ(GetSelectedLegendarySignState(), 4);
    TryUnlockSelectedLegendarySign();
    EXPECT_EQ(gSpecialVar_Result, 6);
    EXPECT(FlagGet(FLAG_HIDE_LEGENDARY_SIGN_DARKRAI));

    // Heatran's room flag starts set until the Magma Stone is set down.
    ResetSignState();
    FlagSet(FLAG_DEFEATED_HEATRAN);
    FlagClear(FLAG_ITEM_MAGMA_HIDEOUT_2F_2R_MAGMA_STONE);
    gSpecialVar_0x8004 = LEGENDARY_SIGN_HEATRAN;
    EXPECT_NE(GetSelectedLegendarySignState(), 4);
    FlagSet(FLAG_ITEM_MAGMA_HIDEOUT_2F_2R_MAGMA_STONE);
    EXPECT(AddBagItem(ITEM_MAGMA_STONE, 1));
    EXPECT_NE(GetSelectedLegendarySignState(), 4);
    EXPECT(RemoveBagItem(ITEM_MAGMA_STONE, 1));
    EXPECT_EQ(GetSelectedLegendarySignState(), 4);

    // Center leads: a lost classic legend closes its own lead, and a lost
    // Regirock closes the Regigigas lead that depends on it.
    ResetSignState();
    FlagClear(FLAG_DEFEATED_HEATRAN);
    FlagClear(FLAG_ITEM_MAGMA_HIDEOUT_2F_2R_MAGMA_STONE);
    FlagSet(FLAG_DEFEATED_REGIROCK);
    gMapHeader.regionMapSectionId = MAPSEC_PACIFIDLOG_TOWN;
    bool32 sawRegirock = FALSE, sawRegigigas = FALSE;
    gSpecialVar_0x8005 = CENTER_GUIDE_TOPIC_LEGENDS;
    gSpecialVar_0x8004 = 0;
    while (TRUE)
    {
        BufferNextCenterLegendaryLead();
        if (!gSpecialVar_Result)
            break;
        if (GuideTextContains(COMPOUND_STRING("Regirock fainted")))
        {
            sawRegirock = TRUE;
            EXPECT(GuideTextContains(COMPOUND_STRING("never returns")));
        }
        if (GuideTextContains(COMPOUND_STRING("wakes Regigigas")))
        {
            sawRegigigas = TRUE;
            EXPECT(GuideTextContains(COMPOUND_STRING("It won't come.")));
        }
    }
    EXPECT(sawRegirock);
    EXPECT(sawRegigigas);
    // A later Regirock record (e.g. from a trade) reopens the lead.
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_REGIROCK), FLAG_SET_CAUGHT);
    sawRegigigas = FALSE;
    gSpecialVar_0x8004 = 0;
    while (TRUE)
    {
        BufferNextCenterLegendaryLead();
        if (!gSpecialVar_Result)
            break;
        if (GuideTextContains(COMPOUND_STRING("wakes Regigigas")))
        {
            sawRegigigas = TRUE;
            EXPECT(!GuideTextContains(COMPOUND_STRING("It won't come.")));
        }
    }
    EXPECT(sawRegigigas);

    FlagClear(FLAG_DEFEATED_REGIROCK);
    FlagClear(FLAG_HIDE_LEGENDARY_SIGN_DARKRAI);
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    FlagClear(FLAG_SYS_GAME_CLEAR);
    FlagClear(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
    memcpy(gSaveBlock1Ptr->dexCaught, savedCaught, sizeof(savedCaught));
    ResetSignState();
}

extern void Test_CollectAsh(void);

TEST("Soot collection: spendable ash and lifetime discovery progress remain independent")
{
    ResetSignState();
    VarSet(VAR_ASH_GATHER_COUNT, 0);
    VarSet(VAR_EC_SOOT_PROGRESS, EC_SOOT_CORD_RECEIVED);
    Test_CollectAsh();
    EXPECT_EQ(VarGet(VAR_ASH_GATHER_COUNT), 0);
    EXPECT_EQ(VarGet(VAR_EC_SOOT_PROGRESS), EC_SOOT_CORD_RECEIVED);
    EXPECT(AddBagItem(ITEM_SOOT_SACK, 1));
    for (u32 step = 0; step < EC_SOOT_MARSHADOW_TARGET; step++)
        Test_CollectAsh();
    EXPECT_EQ(VarGet(VAR_ASH_GATHER_COUNT), 250);
    EXPECT_EQ(VarGet(VAR_EC_SOOT_PROGRESS), EC_SOOT_CORD_RECEIVED | 250);
    // Buying a Blue Flute spends the old balance, never the lifetime total.
    VarSet(VAR_ASH_GATHER_COUNT, VarGet(VAR_ASH_GATHER_COUNT) - 250);
    Test_CollectAsh();
    EXPECT_EQ(VarGet(VAR_ASH_GATHER_COUNT), 1);
    EXPECT_EQ(VarGet(VAR_EC_SOOT_PROGRESS), EC_SOOT_CORD_RECEIVED | 251);
    VarSet(VAR_ASH_GATHER_COUNT, 9998);
    VarSet(VAR_EC_SOOT_PROGRESS, EC_SOOT_CORD_RECEIVED | 9998);
    Test_CollectAsh();
    Test_CollectAsh();
    EXPECT_EQ(VarGet(VAR_ASH_GATHER_COUNT), 9999);
    EXPECT_EQ(VarGet(VAR_EC_SOOT_PROGRESS), EC_SOOT_CORD_RECEIVED | 9999);
    VarSet(VAR_ASH_GATHER_COUNT, 0);
    Test_CollectAsh();
    EXPECT_EQ(VarGet(VAR_ASH_GATHER_COUNT), 1);
    EXPECT_EQ(VarGet(VAR_EC_SOOT_PROGRESS), EC_SOOT_CORD_RECEIVED | 9999);
}

TEST("Marshadow discovery: glassmaker requires lifetime soot and unlocks once without spending it")
{
    ResetSignState();
    for (u32 badge = 0; badge < 3; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    VarSet(VAR_ASH_GATHER_COUNT, 0);
    VarSet(VAR_EC_SOOT_PROGRESS, EC_SOOT_CORD_RECEIVED | 250);
    gSpecialVar_0x8004 = LEGENDARY_SIGN_MARSHADOW;
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE113);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE113);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_MARSHADOW));
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE113_GLASS_WORKSHOP);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE113_GLASS_WORKSHOP);
    VarSet(VAR_EC_SOOT_PROGRESS, EC_SOOT_CORD_RECEIVED | 249);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    VarSet(VAR_EC_SOOT_PROGRESS, EC_SOOT_CORD_RECEIVED | 250);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT(IsLegendarySignUnlocked(LEGENDARY_SIGN_MARSHADOW));
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_MARSHADOW));
    EXPECT_EQ(VarGet(VAR_ASH_GATHER_COUNT), 0);
    EXPECT_EQ(VarGet(VAR_EC_SOOT_PROGRESS), EC_SOOT_CORD_RECEIVED | 250);
    TryUnlockLocalLegendaryDiscovery();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    MarkLegendarySignCaughtBySpecies(SPECIES_MARSHADOW);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_MARSHADOW));
    for (u32 badge = 0; badge < 3; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
}

extern bool32 Test_PokedexAreaHasSection(enum Species species, u16 section);
extern u8 gAreaTimeOfDay;

TEST("Rare habitat: the Pokedex reflects unlocked and uncaught wild residents")
{
    ResetSignState();
    u16 oldSection = gMapHeader.regionMapSectionId;
    u8 oldTime = gAreaTimeOfDay;
    struct WarpData oldLocation = gSaveBlock1Ptr->location;
    gMapHeader.regionMapSectionId = MAPSEC_ROUTE_113;
    gAreaTimeOfDay = TIME_OF_DAY_DEFAULT;
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE113);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE113);
    for (u32 badge = 0; badge < 3; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_MARSHADOW));
    EXPECT(!Test_PokedexAreaHasSection(SPECIES_MARSHADOW, MAPSEC_ROUTE_113));
    UnlockLegendarySign(LEGENDARY_SIGN_MARSHADOW);
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_MARSHADOW));
    EXPECT(Test_PokedexAreaHasSection(SPECIES_MARSHADOW, MAPSEC_ROUTE_113));
    MarkLegendarySignCaughtBySpecies(SPECIES_MARSHADOW);
    EXPECT(!Test_PokedexAreaHasSection(SPECIES_MARSHADOW, MAPSEC_ROUTE_113));
    for (u32 badge = 0; badge < 3; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    gMapHeader.regionMapSectionId = oldSection;
    gAreaTimeOfDay = oldTime;
    gSaveBlock1Ptr->location = oldLocation;
    ResetSignState();
}

TEST("Rare habitat: native meadow residents disappear independently after capture")
{
    ResetSignState();
    u16 oldSection = gMapHeader.regionMapSectionId;
    u8 oldTime = gAreaTimeOfDay;
    gMapHeader.regionMapSectionId = MAPSEC_VERDANTURF_MEADOW;
    gAreaTimeOfDay = TIME_OF_DAY_DEFAULT;
    // Gated residents stay off the map until their milestones.
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    FlagClear(FLAG_VISITED_FORTREE_CITY);
    EXPECT(!Test_PokedexAreaHasSection(SPECIES_ENAMORUS, MAPSEC_VERDANTURF_MEADOW));
    EXPECT(!Test_PokedexAreaHasSection(SPECIES_FEZANDIPITI, MAPSEC_VERDANTURF_MEADOW));
    for (u32 badge = 0; badge < 6; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    FlagSet(FLAG_VISITED_FORTREE_CITY);
    EXPECT(Test_PokedexAreaHasSection(SPECIES_ENAMORUS, MAPSEC_VERDANTURF_MEADOW));
    EXPECT(Test_PokedexAreaHasSection(SPECIES_FEZANDIPITI, MAPSEC_VERDANTURF_MEADOW));
    MarkLegendarySignCaughtBySpecies(SPECIES_ENAMORUS);
    EXPECT(!Test_PokedexAreaHasSection(SPECIES_ENAMORUS, MAPSEC_VERDANTURF_MEADOW));
    EXPECT(Test_PokedexAreaHasSection(SPECIES_FEZANDIPITI, MAPSEC_VERDANTURF_MEADOW));
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    FlagClear(FLAG_VISITED_FORTREE_CITY);
    gMapHeader.regionMapSectionId = oldSection;
    gAreaTimeOfDay = oldTime;
    ResetSignState();
}

static bool32 WildTableHasSpecies(const struct WildPokemonInfo *info, u32 slots, enum Species species)
{
    for (u32 slot = 0; info != NULL && slot < slots; slot++)
        if (info->wildPokemon[slot].species == species)
            return TRUE;
    return FALSE;
}

TEST("Gate table: every wild or quest row has a compiled slot and every official wild legend has a row")
{
    u32 missing = 0;
    for (enum LegendarySignId id = 0; id < LEGENDARY_SIGN_COUNT; id++)
    {
        const struct LegendaryGate *gate = &gLegendaryGates[id];
        EXPECT(IsLegendaryEncounterSpecies(gate->species));
        EXPECT_EQ(GetLegendarySignIdBySpecies(gate->species), id);
        if (gate->kind != LEGENDARY_KIND_WILD && gate->kind != LEGENDARY_KIND_QUEST)
            continue;
        bool32 found = FALSE;
        for (u32 header = 0; gWildMonHeaders[header].mapGroup != MAP_GROUP(MAP_UNDEFINED) && !found; header++)
        {
            const struct WildEncounterTypes *types = &gWildMonHeaders[header].encounterTypes[TIME_OF_DAY_DEFAULT];
            found = WildTableHasSpecies(types->landMonsInfo, NUM_LAND_MONS_ENCOUNTER_SLOTS, gate->species)
                 || WildTableHasSpecies(types->waterMonsInfo, NUM_WATER_MONS_ENCOUNTER_SLOTS, gate->species);
        }
        if (!found)
        {
            Test_MgbaPrintf("Wild gate row without a compiled slot: sign %d species %d", id, gate->species);
            missing++;
        }
    }
    // A Legendary or Ultra Beast in a wild table without a row would be
    // ungated and uncatchable-once only by accident. Paradox slots intentionally
    // have no Sign rows; their Pokedex records retire them after capture.
    for (u32 header = 0; gWildMonHeaders[header].mapGroup != MAP_GROUP(MAP_UNDEFINED); header++)
    {
        const struct WildEncounterTypes *types = &gWildMonHeaders[header].encounterTypes[TIME_OF_DAY_DEFAULT];
        const struct WildPokemonInfo *infos[] = {types->landMonsInfo, types->waterMonsInfo};
        const u32 slots[] = {NUM_LAND_MONS_ENCOUNTER_SLOTS, NUM_WATER_MONS_ENCOUNTER_SLOTS};
        for (u32 method = 0; method < ARRAY_COUNT(infos); method++)
        for (u32 slot = 0; infos[method] != NULL && slot < slots[method]; slot++)
        {
            enum Species species = infos[method]->wildPokemon[slot].species;
            const struct SpeciesInfo *metadata = &gSpeciesInfo[GET_BASE_SPECIES_ID(species)];
            // Named power-tier additions are intentionally ungated; Bloodmoon
            // has its separate caught flag and Gholdengo uses its Pokedex record.
            if ((metadata->isRestrictedLegendary || metadata->isSubLegendary
              || metadata->isMythical || metadata->isUltraBeast)
             && GetLegendarySignIdBySpecies(species) >= LEGENDARY_SIGN_COUNT)
            {
                Test_MgbaPrintf("Wild legend without a gate row: map %d.%d species %d",
                    gWildMonHeaders[header].mapGroup, gWildMonHeaders[header].mapNum, species);
                missing++;
            }
        }
    }
    EXPECT_EQ(missing, 0);
}

// Every guide page: each line within 200px, and it fits gStringVar4.
static void ExpectCenterGuidePageFits(void)
{
    u8 line[sizeof(gStringVar4)];
    u32 length = 0;

    EXPECT_LT(StringLength(gStringVar4), sizeof(gStringVar4));
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
        {
            line[length++] = c;
        }
    }
}

static bool32 CenterGuideTipsMention(u16 city, const u8 *needle)
{
    bool32 found = FALSE;

    gMapHeader.regionMapSectionId = city;
    gSpecialVar_0x8005 = CENTER_GUIDE_TOPIC_TIPS;
    gSpecialVar_0x8004 = 0;
    while (TRUE)
    {
        BufferNextCenterLegendaryLead();
        if (!gSpecialVar_Result)
            break;
        ExpectCenterGuidePageFits();
        found |= GuideTextContains(needle);
    }
    gSpecialVar_0x8005 = CENTER_GUIDE_TOPIC_LEGENDS;
    return found;
}

TEST("Center guide: story directions follow the campaign, one step at a time")
{
    // The story flags in campaign order, as the guide reads them.
    static const u16 sStory[] = {
        FLAG_DEFEATED_RIVAL_ROUTE103, FLAG_ADVENTURE_STARTED, FLAG_BADGE01_GET,
        FLAG_RECOVERED_DEVON_GOODS, FLAG_RETURNED_DEVON_GOODS, FLAG_RECEIVED_POKENAV,
        FLAG_DELIVERED_STEVEN_LETTER, FLAG_DELIVERED_DEVON_GOODS, FLAG_HIDE_SLATEPORT_CITY_BRAWLY,
        FLAG_BADGE02_GET, FLAG_BADGE03_GET, FLAG_RECEIVED_HM06, FLAG_MET_ARCHIE_METEOR_FALLS,
        FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY, FLAG_BADGE04_GET, FLAG_BADGE05_GET,
        FLAG_RECEIVED_HM03, FLAG_HIDE_ROUTE_119_TEAM_AQUA, FLAG_RECEIVED_DEVON_SCOPE, FLAG_KECLEON_FLED_FORTREE,
        FLAG_BADGE06_GET, FLAG_RECEIVED_MAGMA_EMBLEM_MT_PYRE_SUMMIT, FLAG_RECEIVED_HM04, FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT,
        FLAG_MET_TEAM_AQUA_HARBOR, FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE, FLAG_BADGE07_GET,
        FLAG_DEFEATED_MAGMA_SPACE_CENTER, FLAG_RECEIVED_HM08, FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN,
        FLAG_WALLACE_GOES_TO_SKY_PILLAR,
    };
    u8 last[sizeof(gStringVar4)];

    // Test the rival destination with the actual required Leveler owned.
    AddBagItem(ITEM_LEVELER, 1);
    ZeroPlayerPartyMons();
    ClearTrainerFlag(TRAINER_GRUNT_RUSTURF_TUNNEL);
    ClearTrainerFlag(TRAINER_WALLY_VR_2);
    FlagClear(FLAG_SYS_RECEIVED_KEYSTONE);
    FlagClear(FLAG_DEFEATED_WALLY_VICTORY_ROAD);
    FlagClear(FLAG_MET_MAXIE_SOOTOPOLIS);
    FlagClear(FLAG_MET_ARCHIE_SOOTOPOLIS);
    VarSet(VAR_PETALBURG_WOODS_STATE, 0);

    for (u32 i = 0; i < ARRAY_COUNT(sStory); i++)
        FlagClear(sStory[i]);
    FlagClear(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
    FlagClear(FLAG_RECEIVED_HM07);
    FlagClear(FLAG_BADGE08_GET);
    FlagClear(FLAG_SYS_GAME_CLEAR);
    FlagClear(FLAG_DEFEATED_WALLY_MAUVILLE);
    VarSet(VAR_PETALBURG_GYM_STATE, 0);
    VarSet(VAR_ROUTE110_STATE, 0);
    VarSet(VAR_ROUTE119_STATE, 0);
    VarSet(VAR_MT_PYRE_STATE, 0);
    VarSet(VAR_SOOTOPOLIS_CITY_STATE, 0);
    gMapHeader.regionMapSectionId = MAPSEC_OLDALE_TOWN;
    gSpecialVar_0x8005 = CENTER_GUIDE_TOPIC_STORY;
    BufferNextCenterLegendaryLead();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT(GuideTextContains(COMPOUND_STRING("Route 103")));
    // Each completed step moves the guide on to a new destination.
    for (u32 i = 0; i < ARRAY_COUNT(sStory); i++)
    {
        StringCopy(last, gStringVar4);
        FlagSet(sStory[i]);
        BufferNextCenterLegendaryLead();
        EXPECT_EQ(gSpecialVar_Result, TRUE);
        ExpectCenterGuidePageFits();
        EXPECT_NE(StringCompare(last, gStringVar4), 0);
        if (sStory[i] == FLAG_ADVENTURE_STARTED)
        {
            EXPECT(GuideTextContains(COMPOUND_STRING("Norman")));
            VarSet(VAR_PETALBURG_GYM_STATE, 2);
            BufferNextCenterLegendaryLead();
            EXPECT(GuideTextContains(COMPOUND_STRING("researcher")));
            VarSet(VAR_PETALBURG_WOODS_STATE, 1);
            BufferNextCenterLegendaryLead();
            EXPECT(GuideTextContains(COMPOUND_STRING("Roxanne")));
        }
        else if (sStory[i] == FLAG_BADGE01_GET)
        {
            EXPECT(GuideTextContains(COMPOUND_STRING("thief")));
            SetTrainerFlag(TRAINER_GRUNT_RUSTURF_TUNNEL);
            BufferNextCenterLegendaryLead();
            EXPECT(GuideTextContains(COMPOUND_STRING("Make room")));
            EXPECT(GuideTextContains(COMPOUND_STRING("Peeko")));
        }
        else if (sStory[i] == FLAG_BADGE04_GET)
        {
            EXPECT(GuideTextContains(COMPOUND_STRING("Mega")));
            FlagSet(FLAG_SYS_RECEIVED_KEYSTONE);
            BufferNextCenterLegendaryLead();
            EXPECT(GuideTextContains(COMPOUND_STRING("Norman")));
            EXPECT(!GuideTextContains(COMPOUND_STRING("Mega")));
        }
        else if (sStory[i] == FLAG_BADGE02_GET)
        {
            EXPECT(GuideTextContains(COMPOUND_STRING("rival")));
            VarSet(VAR_ROUTE110_STATE, 1);
            BufferNextCenterLegendaryLead();
            EXPECT(GuideTextContains(COMPOUND_STRING("Wally")));
            FlagSet(FLAG_DEFEATED_WALLY_MAUVILLE);
            BufferNextCenterLegendaryLead();
            EXPECT(GuideTextContains(COMPOUND_STRING("Wattson")));
        }
        else if (sStory[i] == FLAG_BADGE03_GET)
        {
            EXPECT(GuideTextContains(COMPOUND_STRING("Rock Smash license")));
        }
        else if (sStory[i] == FLAG_BADGE05_GET)
        {
            EXPECT(GuideTextContains(COMPOUND_STRING("Surf license")));
        }
        else if (sStory[i] == FLAG_HIDE_ROUTE_119_TEAM_AQUA)
        {
            EXPECT(GuideTextContains(COMPOUND_STRING("rival")));
            VarSet(VAR_ROUTE119_STATE, 1);
            BufferNextCenterLegendaryLead();
            EXPECT(GuideTextContains(COMPOUND_STRING("Steven")));
        }
        else if (sStory[i] == FLAG_BADGE06_GET)
        {
            EXPECT(GuideTextContains(COMPOUND_STRING("summit")));
            VarSet(VAR_MT_PYRE_STATE, 1);
            BufferNextCenterLegendaryLead();
            EXPECT(GuideTextContains(COMPOUND_STRING("Bag was full")));
        }
        else if (sStory[i] == FLAG_RECEIVED_MAGMA_EMBLEM_MT_PYRE_SUMMIT)
        {
            EXPECT(GuideTextContains(COMPOUND_STRING("Strength license")));
        }
        ExpectCenterGuidePageFits();
    }
    EXPECT(GuideTextContains(COMPOUND_STRING("Sky")));
    VarSet(VAR_SOOTOPOLIS_CITY_STATE, 5);
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("both")));
    FlagSet(FLAG_MET_MAXIE_SOOTOPOLIS);
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("Speak to Archie")));
    FlagClear(FLAG_MET_MAXIE_SOOTOPOLIS);
    FlagSet(FLAG_MET_ARCHIE_SOOTOPOLIS);
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("Speak to Maxie")));
    FlagSet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("Wallace")));
    FlagSet(FLAG_RECEIVED_HM07);
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("Juan")));
    FlagSet(FLAG_BADGE08_GET);
    BufferNextCenterLegendaryLead();
    EXPECT(GuideTextContains(COMPOUND_STRING("Victory Road")));
    ExpectCenterGuidePageFits();
    EXPECT(GuideTextContains(COMPOUND_STRING("Wally")));
    FlagSet(FLAG_DEFEATED_WALLY_VICTORY_ROAD);
    BufferNextCenterLegendaryLead();
    EXPECT(!GuideTextContains(COMPOUND_STRING("Wally")));
    // Shared finale callers and the Center use exactly this same tree.
    FlagSet(FLAG_SYS_GAME_CLEAR);
    BufferNextCenterLegendaryLead();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT(GuideTextContains(COMPOUND_STRING("Wally")));
    ExpectCenterGuidePageFits();

    for (u32 i = 0; i < ARRAY_COUNT(sStory); i++)
        FlagClear(sStory[i]);
    FlagClear(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
    FlagClear(FLAG_RECEIVED_HM07);
    FlagClear(FLAG_BADGE08_GET);
    FlagClear(FLAG_SYS_GAME_CLEAR);
    FlagClear(FLAG_DEFEATED_WALLY_MAUVILLE);
    VarSet(VAR_PETALBURG_GYM_STATE, 0);
    VarSet(VAR_ROUTE110_STATE, 0);
    VarSet(VAR_ROUTE119_STATE, 0);
    VarSet(VAR_MT_PYRE_STATE, 0);
    VarSet(VAR_SOOTOPOLIS_CITY_STATE, 0);
    VarSet(VAR_PETALBURG_WOODS_STATE, 0);
    FlagClear(FLAG_SYS_RECEIVED_KEYSTONE);
    FlagClear(FLAG_DEFEATED_WALLY_VICTORY_ROAD);
    FlagClear(FLAG_MET_MAXIE_SOOTOPOLIS);
    FlagClear(FLAG_MET_ARCHIE_SOOTOPOLIS);
    ClearTrainerFlag(TRAINER_GRUNT_RUSTURF_TUNNEL);
    RemoveBagItem(ITEM_LEVELER, 1);
    gSpecialVar_0x8005 = CENTER_GUIDE_TOPIC_LEGENDS;
}

TEST("Center guide: finale names the next unbeaten cabin and pending tickets")
{
    static const u16 trainers[] = {TRAINER_WALLY_VR_2, TRAINER_COLTON, TRAINER_MICAH,
        TRAINER_THOMAS, TRAINER_LEA_AND_JED, TRAINER_NAOMI, TRAINER_STEVEN, TRAINER_BUFFEL};
    static const u8 *const names[] = {COMPOUND_STRING("Colton"), COMPOUND_STRING("Micah"),
        COMPOUND_STRING("Thomas"), COMPOUND_STRING("Lea"), COMPOUND_STRING("Naomi")};
    for (u32 i = 0; i < ARRAY_COUNT(trainers); i++)
        ClearTrainerFlag(trainers[i]);
    FlagSet(FLAG_SYS_GAME_CLEAR);
    FlagClear(FLAG_RECEIVED_SS_TICKET);
    FlagClear(FLAG_EC_EARNED_SS_TICKET);
    FlagClear(FLAG_RECEIVED_AURORA_TICKET);
    FlagClear(FLAG_EC_FINALE_DEOXYS_RESOLVED);
    FlagClear(FLAG_BATTLED_DEOXYS);
    FlagClear(FLAG_DEFEATED_DEOXYS);
    BufferCenterGuideDirections();
    EXPECT(GuideTextContains(COMPOUND_STRING("Wally")));
    SetTrainerFlag(TRAINER_WALLY_VR_2);
    BufferCenterGuideDirections();
    EXPECT(GuideTextContains(COMPOUND_STRING("your own house")));
    FlagSet(FLAG_EC_EARNED_SS_TICKET);
    BufferCenterGuideDirections();
    EXPECT(GuideTextContains(COMPOUND_STRING("Center nurse")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("Colton")));
    ExpectCenterGuidePageFits();
    // A delivered ticket in PC storage is usable even before a nurse has
    // repaired an older save's receipt flag. Advice must not mutate that flag.
    AddPCItem(ITEM_SS_TICKET, 1);
    BufferCenterGuideDirections();
    EXPECT(GuideTextContains(names[0]));
    EXPECT_EQ(FlagGet(FLAG_RECEIVED_SS_TICKET), FALSE);
    for (u32 i = 0; i < PC_ITEMS_COUNT; i++)
        if (gSaveBlock1Ptr->pcItems[i].itemId == ITEM_SS_TICKET)
            RemovePCItem(i, 1);
    FlagSet(FLAG_RECEIVED_SS_TICKET);
    // Completion can happen in any order: an unfinished earlier cabin stays.
    SetTrainerFlag(TRAINER_NAOMI);
    BufferCenterGuideDirections();
    EXPECT(GuideTextContains(names[0]));
    ClearTrainerFlag(TRAINER_NAOMI);
    for (u32 i = 0; i < ARRAY_COUNT(names); i++)
    {
        BufferCenterGuideDirections();
        EXPECT(GuideTextContains(names[i]));
        EXPECT(GuideTextContains(COMPOUND_STRING("door")));
        EXPECT(GuideTextContains(COMPOUND_STRING("Cabin 2")));
        EXPECT(!GuideTextContains(COMPOUND_STRING("Steven")));
        ExpectCenterGuidePageFits();
        SetTrainerFlag(trainers[i + 1]);
    }
    BufferCenterGuideDirections();
    EXPECT(GuideTextContains(COMPOUND_STRING("Steven")));
    EXPECT(GuideTextContains(COMPOUND_STRING("far-west ladder")));
    SetTrainerFlag(TRAINER_STEVEN);
    BufferCenterGuideDirections();
    EXPECT(GuideTextContains(COMPOUND_STRING("still has your Aurora")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("choose Birth Island")));
    ExpectCenterGuidePageFits();
    FlagSet(FLAG_RECEIVED_AURORA_TICKET);
    BufferCenterGuideDirections();
    EXPECT(GuideTextContains(COMPOUND_STRING("choose Birth Island")));
    EXPECT(GuideTextContains(COMPOUND_STRING("Left 3, down 1")));
    EXPECT(GuideTextContains(COMPOUND_STRING("knockout loses it forever")));
    ExpectCenterGuidePageFits();
    BufferCenterGuideObjective();
    EXPECT(GuideTextContains(COMPOUND_STRING("Birth")));
    EXPECT(GuideTextContains(COMPOUND_STRING("triangle")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("Left 3")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("Right 6")));
    EXPECT(GuideTextContains(COMPOUND_STRING("knockout loses it forever")));
    ExpectCenterGuidePageFits();
    // Both capture and permanent knockout retire the same mandatory encounter.
    FlagSet(FLAG_BATTLED_DEOXYS);
    BufferCenterGuideDirections();
    EXPECT(GuideTextContains(COMPOUND_STRING("Buffel")));
    FlagClear(FLAG_BATTLED_DEOXYS);
    FlagSet(FLAG_DEFEATED_DEOXYS);
    BufferCenterGuideDirections();
    EXPECT(GuideTextContains(COMPOUND_STRING("Buffel")));
    ExpectCenterGuidePageFits();
    SetTrainerFlag(TRAINER_BUFFEL);
    BufferCenterGuideDirections();
    EXPECT(GuideTextContains(COMPOUND_STRING("completed")));
    ExpectCenterGuidePageFits();
    for (u32 i = 0; i < ARRAY_COUNT(trainers); i++)
        ClearTrainerFlag(trainers[i]);
    FlagClear(FLAG_SYS_GAME_CLEAR);
    FlagClear(FLAG_RECEIVED_SS_TICKET);
    FlagClear(FLAG_EC_EARNED_SS_TICKET);
    FlagClear(FLAG_RECEIVED_AURORA_TICKET);
    FlagClear(FLAG_DEFEATED_DEOXYS);
}

TEST("Center guide: Route 103 directs refused beginners to tools and leveling")
{
    FlagClear(FLAG_DEFEATED_RIVAL_ROUTE103);
    FlagClear(FLAG_SYS_GAME_CLEAR);
    RemoveBagItem(ITEM_LEVELER, 1);
    // Existing tests leave no Leveler in the PC; a real under-cap partner
    // distinguishes the level refusal from simply repeating the destination.
    ZeroPlayerPartyMons();
    BufferCenterGuideDirections();
    EXPECT(GuideTextContains(COMPOUND_STRING("Poké Mart")));
    AddBagItem(ITEM_LEVELER, 1);
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_TREECKO, 1, 0, OTID_STRUCT_PLAYER_ID);
    BufferCenterGuideDirections();
    EXPECT(GuideTextContains(COMPOUND_STRING("USE")));
    ExpectCenterGuidePageFits();
    RemoveBagItem(ITEM_LEVELER, 1);
    AddPCItem(ITEM_LEVELER, 1);
    BufferCenterGuideDirections();
    EXPECT(GuideTextContains(COMPOUND_STRING("Withdraw Item")));
    ExpectCenterGuidePageFits();
    for (u32 i = 0; i < PC_ITEMS_COUNT; i++)
        if (gSaveBlock1Ptr->pcItems[i].itemId == ITEM_LEVELER)
            RemovePCItem(i, 1);
    AddBagItem(ITEM_LEVELER, 1);
    ZeroPlayerPartyMons();
    BufferCenterGuideDirections();
    EXPECT(GuideTextContains(COMPOUND_STRING("Route 103")));
    EXPECT(!GuideTextContains(COMPOUND_STRING("USE")));
    RemoveBagItem(ITEM_LEVELER, 1);
}

TEST("Center guide: side quests appear with their gates and retire when done")
{
    const u16 cities[] = {MAPSEC_OLDALE_TOWN, MAPSEC_PETALBURG_CITY, MAPSEC_DEWFORD_TOWN,
        MAPSEC_RUSTBORO_CITY, MAPSEC_SLATEPORT_CITY, MAPSEC_MAUVILLE_CITY,
        MAPSEC_VERDANTURF_TOWN, MAPSEC_LAVARIDGE_TOWN, MAPSEC_FALLARBOR_TOWN,
        MAPSEC_FORTREE_CITY, MAPSEC_LILYCOVE_CITY, MAPSEC_MOSSDEEP_CITY,
        MAPSEC_SOOTOPOLIS_CITY, MAPSEC_PACIFIDLOG_TOWN, MAPSEC_EVER_GRANDE_CITY,
        MAPSEC_BATTLE_FRONTIER};

    ResetSignState();
    for (u32 badge = 0; badge < 8; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    FlagSet(FLAG_SYS_GAME_CLEAR);
    FlagSet(FLAG_VISITED_FORTREE_CITY);
    for (u32 city = 0; city < ARRAY_COUNT(cities); city++)
        CenterGuideTipsMention(cities[city], COMPOUND_STRING("")); // Width checks only.

    // The Chansey chase: one tip per stage, then the Route 133 upgrade.
    VarSet(VAR_POKE_VIAL_MAX_CHARGES, 1);
    VarSet(VAR_CHANSEY_NURSE_STATE, 0);
    EXPECT(CenterGuideTipsMention(MAPSEC_MAUVILLE_CITY, COMPOUND_STRING("has lost her")));
    VarSet(VAR_CHANSEY_NURSE_STATE, 3);
    EXPECT(CenterGuideTipsMention(MAPSEC_MAUVILLE_CITY, COMPOUND_STRING("Heal Ball")));
    EXPECT(CenterGuideTipsMention(MAPSEC_LAVARIDGE_TOWN, COMPOUND_STRING("Heal Ball")));
    EXPECT(!CenterGuideTipsMention(MAPSEC_PACIFIDLOG_TOWN, COMPOUND_STRING("Route 133")));
    VarSet(VAR_CHANSEY_NURSE_STATE, 6);
    EXPECT(CenterGuideTipsMention(MAPSEC_MAUVILLE_CITY, COMPOUND_STRING("You caught Blob")));
    VarSet(VAR_POKE_VIAL_MAX_CHARGES, 2);
    EXPECT(!CenterGuideTipsMention(MAPSEC_MAUVILLE_CITY, COMPOUND_STRING("Blob")));
    EXPECT(CenterGuideTipsMention(MAPSEC_PACIFIDLOG_TOWN, COMPOUND_STRING("Route 133")));
    VarSet(VAR_POKE_VIAL_MAX_CHARGES, 3);
    EXPECT(!CenterGuideTipsMention(MAPSEC_PACIFIDLOG_TOWN, COMPOUND_STRING("Route 133")));

    // Spiritomb: shown until the Odd Keystone is spent.
    FlagClear(FLAG_SANDSTREWN_RUINS_ODD_KEYSTONE);
    EXPECT(CenterGuideTipsMention(MAPSEC_SLATEPORT_CITY, COMPOUND_STRING("Odd Keystone")));
    FlagSet(FLAG_SANDSTREWN_RUINS_ODD_KEYSTONE);
    AddBagItem(ITEM_ODD_KEYSTONE, 1);
    EXPECT(CenterGuideTipsMention(MAPSEC_SLATEPORT_CITY, COMPOUND_STRING("Odd Keystone")));
    RemoveBagItem(ITEM_ODD_KEYSTONE, 1);
    EXPECT(!CenterGuideTipsMention(MAPSEC_SLATEPORT_CITY, COMPOUND_STRING("Odd Keystone")));

    // Badge gates and receipts.
    for (u32 badge = 4; badge < 8; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    FlagClear(FLAG_GOT_TM24_FROM_WATTSON);
    EXPECT(!CenterGuideTipsMention(MAPSEC_MAUVILLE_CITY, COMPOUND_STRING("Wattson")));
    FlagSet(FLAG_BADGE05_GET);
    EXPECT(CenterGuideTipsMention(MAPSEC_MAUVILLE_CITY, COMPOUND_STRING("Wattson")));
    FlagSet(FLAG_GOT_TM24_FROM_WATTSON);
    EXPECT(!CenterGuideTipsMention(MAPSEC_MAUVILLE_CITY, COMPOUND_STRING("Wattson")));
    FlagClear(FLAG_RECEIVED_MELTAN);
    EXPECT(CenterGuideTipsMention(MAPSEC_MOSSDEEP_CITY, COMPOUND_STRING("Meltan")));
    FlagClear(FLAG_SYS_GAME_CLEAR);
    EXPECT(!CenterGuideTipsMention(MAPSEC_MOSSDEEP_CITY, COMPOUND_STRING("Meltan")));
    // Local stone leads retire independently when their pickups are claimed.
    FlagClear(FLAG_ITEM_ROUTE119_BUTTERFRENITE);
    FlagClear(FLAG_ITEM_ROUTE_106_KINGLERITE);
    EXPECT(CenterGuideTipsMention(MAPSEC_DEWFORD_TOWN, COMPOUND_STRING("Butterfree")));
    EXPECT(CenterGuideTipsMention(MAPSEC_DEWFORD_TOWN, COMPOUND_STRING("Kingler")));
    FlagSet(FLAG_ITEM_DEWFORD_MEADOW_HEAT_ROCK);
    EXPECT(CenterGuideTipsMention(MAPSEC_DEWFORD_TOWN, COMPOUND_STRING("Butterfree")));
    FlagClear(FLAG_ITEM_DEWFORD_MEADOW_HEAT_ROCK);
    FlagSet(FLAG_ITEM_ROUTE119_BUTTERFRENITE);
    EXPECT(!CenterGuideTipsMention(MAPSEC_DEWFORD_TOWN, COMPOUND_STRING("Butterfree")));
    EXPECT(CenterGuideTipsMention(MAPSEC_DEWFORD_TOWN, COMPOUND_STRING("Kingler")));
    FlagSet(FLAG_ITEM_ROUTE_106_KINGLERITE);
    EXPECT(!CenterGuideTipsMention(MAPSEC_DEWFORD_TOWN, COMPOUND_STRING("")));
    FlagClear(FLAG_ITEM_ROUTE119_BUTTERFRENITE);
    FlagClear(FLAG_ITEM_ROUTE_106_KINGLERITE);

    for (u32 badge = 0; badge < 8; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    FlagClear(FLAG_VISITED_FORTREE_CITY);
    FlagClear(FLAG_GOT_TM24_FROM_WATTSON);
    FlagClear(FLAG_SANDSTREWN_RUINS_ODD_KEYSTONE);
    VarSet(VAR_POKE_VIAL_MAX_CHARGES, 1);
    VarSet(VAR_CHANSEY_NURSE_STATE, 0);
    gSpecialVar_0x8005 = CENTER_GUIDE_TOPIC_LEGENDS;
}

// The lead's text with its line and page breaks read as spaces.
static void NormalizeLeadText(u8 *dest, const u8 *lead)
{
    u32 i;
    for (i = 0; lead[i] != EOS && i < sizeof(gStringVar4) - 1; i++)
    {
        u8 c = lead[i];
        dest[i] = (c == CHAR_NEWLINE || c == CHAR_PROMPT_SCROLL || c == CHAR_PROMPT_CLEAR) ? CHAR_SPACE : c;
    }
    dest[i] = EOS;
}

static bool32 LeadTextContains(const u8 *text, const u8 *needle)
{
    u32 length = StringLength(text);
    u32 needleLength = StringLength(needle);
    for (u32 i = 0; i + needleLength <= length; i++)
        if (StringCompareN(text + i, needle, needleLength) == 0)
            return TRUE;
    return FALSE;
}

// The Badge count a lead states in prose ("five Badges", "all eight Gym
// Badges"), or 0 when it states none.
static u32 GetLeadStatedBadges(const u8 *text)
{
    static const u8 *const numbers[][2] = {
        {COMPOUND_STRING("one "), COMPOUND_STRING("One ")},
        {COMPOUND_STRING("two "), COMPOUND_STRING("Two ")},
        {COMPOUND_STRING("three "), COMPOUND_STRING("Three ")},
        {COMPOUND_STRING("four "), COMPOUND_STRING("Four ")},
        {COMPOUND_STRING("five "), COMPOUND_STRING("Five ")},
        {COMPOUND_STRING("six "), COMPOUND_STRING("Six ")},
        {COMPOUND_STRING("seven "), COMPOUND_STRING("Seven ")},
        {COMPOUND_STRING("eight "), COMPOUND_STRING("Eight ")},
    };
    const u8 *gym = COMPOUND_STRING("Gym ");
    const u8 *badge = COMPOUND_STRING("Badge");

    for (u32 i = 0; text[i] != EOS; i++)
    {
        if (i > 0 && text[i - 1] != CHAR_SPACE)
            continue;
        for (u32 n = 0; n < ARRAY_COUNT(numbers); n++)
        {
            for (u32 form = 0; form < 2; form++)
            {
                const u8 *word = numbers[n][form];
                u32 at = i + StringLength(word);
                if (StringCompareN(text + i, word, StringLength(word)) != 0)
                    continue;
                if (StringCompareN(text + at, gym, StringLength(gym)) == 0)
                    at += StringLength(gym);
                if (StringCompareN(text + at, badge, StringLength(badge)) == 0)
                    return n + 1;
            }
        }
    }
    return 0;
}

static bool32 IsBadgeFlag(u16 flag)
{
    return flag >= FLAG_BADGE01_GET && flag <= FLAG_BADGE08_GET;
}

// Each Rare Pokémon lead tells the player its gate in prose. The gate row in
// src/data/pokemon/legendary_signs.h is the truth: a stated Badge count must
// match it, a Badge-only gate must be stated, a story gate must be named and
// a required catch must be named. Edit a gate row and its lead must follow.
TEST("Center guide: every legendary lead states the gate its row enforces")
{
    u8 text[sizeof(gStringVar4)];
    u32 failures = 0;

    for (u32 i = 0; i < GetCenterLegendaryLeadCountForTesting(); i++)
    {
        enum LegendarySignId id;
        u16 city;
        const u8 *lead = GetCenterLegendaryLeadForTesting(i, &id, &city);
        const struct LegendaryGate *gate;
        bool32 namesHallOfFame;
        bool32 ok = TRUE;
        u32 stated;

        NormalizeLeadText(text, lead);
        stated = GetLeadStatedBadges(text);
        // Classic residents (Jirachi, the Regis, the weather trio...) follow
        // their own scripted gates rather than a gate row.
        if (id >= LEGENDARY_SIGN_COUNT)
            continue;
        gate = &gLegendaryGates[id];
        namesHallOfFame = LeadTextContains(text, COMPOUND_STRING("Hall of Fame"))
                       || LeadTextContains(text, COMPOUND_STRING("Champion"));

        if (stated != 0)
        {
            ok &= stated == gate->minimumBadges;
        }
        else if (gate->minimumBadges > 0)
        {
            // An unstated count is fine only when a named milestone implies it.
            bool32 implied = (gate->unlockFlag != 0 && !IsBadgeFlag(gate->unlockFlag))
                          || namesHallOfFame;
            if (gate->requiredSpecies != SPECIES_NONE)
            {
                enum LegendarySignId required = GetLegendarySignIdBySpecies(gate->requiredSpecies);
                if (required < LEGENDARY_SIGN_COUNT
                 && gLegendaryGates[required].minimumBadges >= gate->minimumBadges)
                    implied = TRUE;
            }
            ok &= implied;
        }

        if (IsBadgeFlag(gate->unlockFlag))
            ok &= stated == gate->unlockFlag - FLAG_BADGE01_GET + 1;
        else if (gate->requiredSpecies != SPECIES_NONE
              && GetLegendarySignIdBySpecies(gate->requiredSpecies) < LEGENDARY_SIGN_COUNT
              && gLegendaryGates[GetLegendarySignIdBySpecies(gate->requiredSpecies)].unlockFlag == gate->unlockFlag)
            ; // The required catch already waits for the same milestone.
        else if (gate->unlockFlag == FLAG_IS_CHAMPION || gate->unlockFlag == FLAG_SYS_GAME_CLEAR)
            ok &= namesHallOfFame;
        else if (gate->unlockFlag == FLAG_RECEIVED_MAGMA_EMBLEM_MT_PYRE_SUMMIT)
            ok &= LeadTextContains(text, COMPOUND_STRING("Magma Emblem"))
               || LeadTextContains(text, COMPOUND_STRING("Mt. Pyre"));
        else if (gate->unlockFlag == FLAG_VISITED_FORTREE_CITY)
            ok &= LeadTextContains(text, COMPOUND_STRING("first Fortree visit"));
        else if (gate->unlockFlag == FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN)
            ok &= LeadTextContains(text, COMPOUND_STRING("Kyogre"));
        else if (gate->unlockFlag == FLAG_GOT_TM24_FROM_WATTSON)
            ok &= LeadTextContains(text, COMPOUND_STRING("Wattson"));
        else if (gate->unlockFlag == FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE)
            ok &= LeadTextContains(text, COMPOUND_STRING("crisis"))
               || LeadTextContains(text, COMPOUND_STRING("skies calm"));
        else if (gate->unlockFlag == FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT)
            ok &= LeadTextContains(text, COMPOUND_STRING("Maxie"));
        else if (gate->unlockFlag != 0)
            ok = FALSE; // A new story gate: teach this test the words its lead uses.

        if (gate->requiredSpecies != SPECIES_NONE)
            ok &= LeadTextContains(text, GetSpeciesName(gate->requiredSpecies));

        if (!ok)
        {
            Test_MgbaPrintf("Lead %d (sign %d) disagrees with its gate: states %d, gate %d Badges, flag 0x%x",
                i, id, stated, gate->minimumBadges, gate->unlockFlag);
            failures++;
        }
    }
    EXPECT_EQ(failures, 0);
}
