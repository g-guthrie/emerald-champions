#include "global.h"

#if EC_HEADLESS_FIXTURES

#include "battle.h"
#include "battle_anim.h"
#include "battle_dome.h"
#include "battle_gimmick.h"
#include "battle_interface.h"
#include "battle_main.h"
#include "battle_setup.h"
#include "champions_circuit.h"
#include "emerald_champions_headless.h"
#include "emerald_champions_battle_sets.h"
#include "coins.h"
#include "contest.h"
#include "contest_util.h"
#include "event_data.h"
#include "item_use.h"
#include "load_save.h"
#include "event_object_movement.h"
#include "field_effect.h"
#include "field_specials.h"
#include "field_door.h"
#include "field_screen_effect.h"
#include "fieldmap.h"
#include "frontier_pass.h"
#include "heal_location.h"
#include "item.h"
#include "item_menu.h"
#include "legendary_signs.h"
#include "load_save.h"
#include "main_menu.h"
#include "move_relearner.h"
#include "naming_screen.h"
#include "new_game.h"
#include "option_menu.h"
#include "overworld.h"
#include "party_menu.h"
#include "play_time.h"
#include "pokeblock.h"
#include "pokedex.h"
#include "pokedex_common.h"
#include "pokemon.h"
#include "pokemon_summary_screen.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "safari_zone.h"
#include "script.h"
#include "slot_machine.h"
#include "string_util.h"
#include "wild_encounter.h"
#include "title_screen.h"
#include "trainer_card.h"
#include "constants/battle.h"
#include "constants/battle_frontier.h"
#include "constants/contest.h"
#include "constants/game_stat.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/flags.h"
#include "constants/event_objects.h"
#include "constants/field_effects.h"
#include "constants/field_specials.h"
#include "constants/script_menu.h"
#include "constants/heal_locations.h"
#include "constants/maps.h"
#include "constants/pokedex.h"
#include "constants/region_map_sections.h"
#include "constants/species.h"

EWRAM_DATA volatile u32 gEcHeadlessFixtureScenario = EC_HEADLESS_SCENARIO_NONE;
EWRAM_DATA volatile u32 gEcHeadlessFixtureActiveScenario = EC_HEADLESS_SCENARIO_NONE;
EWRAM_DATA volatile u32 gEcHeadlessFixtureSetupResult = FALSE;
EWRAM_DATA volatile u32 gEcHeadlessFixtureObservedResult = FALSE;
EWRAM_DATA volatile u32 gEcHeadlessFixtureParam = MOVE_NONE;
EWRAM_DATA volatile u32 gEcHeadlessFixtureTrigger = FALSE;
static EWRAM_DATA u8 sEcHeadlessName[POKEMON_NAME_LENGTH + 1] = {0};
static EWRAM_DATA u16 sEcHeadlessObservedDelay = 0;
static EWRAM_DATA bool8 sEcHeadlessFurfrouMenuOpened = FALSE;
static const u8 sEcHeadlessPlayerName[] = _("BRENDAN");
extern void gInitialMainCB2(void);
extern const u8 Common_EventScript_ChooseStarterRegion[];
extern void CallBattleDomeFunction(void);

struct EcHeadlessOverworldFixture
{
    u16 map;
    u16 species;
    s8 playerX;
    s8 playerY;
};

#define EC_HEADLESS_OVERWORLD_FIXTURE(index, map, species, playerX, playerY) \
    [index - 1] = {map, species, playerX, playerY},
static const struct EcHeadlessOverworldFixture sEcHeadlessOverworldFixtures[] =
{
#include "emerald_champions_headless_overworld_fixtures.h"
};
#undef EC_HEADLESS_OVERWORLD_FIXTURE

STATIC_ASSERT(ARRAY_COUNT(sEcHeadlessOverworldFixtures) == 8, HeadlessOverworldFixtureCount);

static u16 GetHeadlessOverworldFixtureGraphicsId(enum Species species)
{
    switch (species)
    {
    case SPECIES_ARTICUNO: return OBJ_EVENT_GFX_INCLEMENT_ARTICUNO;
    case SPECIES_ZAPDOS: return OBJ_EVENT_GFX_INCLEMENT_ZAPDOS;
    case SPECIES_MOLTRES: return OBJ_EVENT_GFX_INCLEMENT_MOLTRES;
    case SPECIES_MEWTWO: return OBJ_EVENT_GFX_INCLEMENT_MEWTWO;
    case SPECIES_JIRACHI: return OBJ_EVENT_GFX_INCLEMENT_JIRACHI;
    case SPECIES_HEATRAN: return OBJ_EVENT_GFX_INCLEMENT_HEATRAN;
    case SPECIES_REGIGIGAS: return OBJ_EVENT_GFX_REGIGIGAS_STATUE;
    case SPECIES_DIANCIE: return OBJ_EVENT_GFX_INCLEMENT_DIANCIE;
    default: return OBJ_EVENT_MON + species;
    }
}

static void PrepareHeadlessOverworldFixtureState(enum Species species)
{
    FlagClear(FLAG_EC_CAUGHT_ARTICUNO);
    FlagClear(FLAG_EC_CAUGHT_DIANCIE);
    FlagClear(FLAG_EC_CAUGHT_HEATRAN);
    FlagClear(FLAG_EC_CAUGHT_JIRACHI);
    FlagClear(FLAG_EC_CAUGHT_MOLTRES);
    FlagClear(FLAG_EC_CAUGHT_MEWTWO);
    FlagClear(FLAG_EC_CAUGHT_REGIGIGAS);
    FlagClear(FLAG_EC_CAUGHT_ZAPDOS);
    UnlockLegendarySign(GetLegendarySignIdBySpecies(species));
}

static EWRAM_DATA enum Species sEcHeadlessFlightRider = SPECIES_NONE;

static void PrepareHeadlessFieldMoveParty(u16 badgeFlag, u16 hmFlag)
{
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ZIGZAGOON, 14, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    FlagSet(badgeFlag);
    FlagSet(hmFlag);
}

static void Task_HeadlessOpenFlightBeaconMap(u8 taskId)
{
    CleanupOverworldWindowsAndTilemaps();
    OpenFlyMapForFlightBeacon(CB2_ReturnToField);
    sEcHeadlessFlightRider = gFieldMoveShowMonSpeciesOverride;
    gEcHeadlessFixtureSetupResult = TRUE;
    DestroyTask(taskId);
}

static void PrepareHeadlessNewGame(void)
{
    SetSaveBlocksPointers(0);
    NewGameInitData();
    StringCopy(gSaveBlock2Ptr->playerName, sEcHeadlessPlayerName);
    gSaveBlock2Ptr->playerGender = MALE;
    gSaveBlock2Ptr->playerTrainerId[0] = 0x34;
    gSaveBlock2Ptr->playerTrainerId[1] = 0x12;
    gSaveBlock2Ptr->playerTrainerId[2] = 0x78;
    gSaveBlock2Ptr->playerTrainerId[3] = 0x56;
    ResetInitialPlayerAvatarState();
    PlayTimeCounter_Start();
    ScriptContext_Init();
    UnlockPlayerFieldControls();
}

static void LoadHeadlessMap(u16 map, s8 x, s8 y)
{
    SetWarpDestination(MAP_GROUP(map), MAP_NUM(map), WARP_ID_NONE, x, y);
    WarpIntoMap();
    gFieldCallback = FieldCB_WarpExitFadeFromBlack;
    gFieldCallback2 = NULL;
    SetMainCallback2(CB2_LoadMap);
}

static void FieldCB_HeadlessSuppressOnFrame(void)
{
    VarSet(VAR_TEMP_1, 1);
    FieldCB_WarpExitFadeFromBlack();
}

static void CreateHealthyHeadlessMon(
    struct Pokemon *mon,
    enum Species species,
    u8 level,
    struct OriginalTrainerId trainerId)
{
    CreateMon(mon, species, level, 0, trainerId);
    CalculateMonStats(mon);
}

static void PrepareHeadlessHallParty(u32 count)
{
    static const enum Species species[PARTY_SIZE] =
    {
        SPECIES_PIKACHU, SPECIES_CHARIZARD, SPECIES_BLASTOISE,
        SPECIES_VENUSAUR, SPECIES_GENGAR, SPECIES_DRAGONITE,
    };

    for (u32 i = 0; i < min(count, PARTY_SIZE); i++)
        CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][i], species[i], 80, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
}

static void PrepareAbilityMenu(void)
{
    CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_GEODUDE, 30, OTID_STRUCT_PLAYER_ID);
    CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_GARDEVOIR, 30, OTID_STRUCT_PLAYER_ID);
    CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][2], SPECIES_PIKACHU, 30, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    SetMainCallback2(CB2_PartyMenuFromStartMenu);
}

static void GiveHeadlessGeodude(u8 level)
{
    CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_GEODUDE, level, OTID_STRUCT_PLAYER_ID);
    ApplyEmeraldChampionsBattleSetChoice(&gParties[B_TRAINER_PLAYER][0], 0);
    CalculatePlayerPartyCount();
}

static void PrepareCircuitParty(void)
{
    static const enum Species species[PARTY_SIZE] =
    {
        SPECIES_PIKACHU,
        SPECIES_CHARIZARD,
        SPECIES_BLASTOISE,
        SPECIES_VENUSAUR,
        SPECIES_GENGAR,
        SPECIES_DRAGONITE,
    };

    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][slot], species[slot], 80, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
}

static void PrepareAllLegalMoves(enum Species species)
{
    CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], species, 30, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    gSpecialVar_0x8004 = 0;
    gMoveRelearnerState = MOVE_RELEARNER_ALL_MOVES;
    gRelearnMode = RELEARN_MODE_SCRIPT;
    SetMainCallback2(CB2_InitLearnMove);
}

static void PrepareHeadlessPokedex(void)
{
    static const enum Species species[] =
    {
        SPECIES_BULBASAUR,
        SPECIES_IVYSAUR,
        SPECIES_VENUSAUR,
        SPECIES_CHARMANDER,
        SPECIES_CHARMELEON,
        SPECIES_CHARIZARD,
    };

    FlagSet(FLAG_SYS_POKEDEX_GET);
    EnableNationalPokedex();
    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        enum NationalDexOrder dex = SpeciesToNationalPokedexNum(species[i]);

        GetSetPokedexFlag(dex, FLAG_SET_SEEN);
        if (i != 2)
            GetSetPokedexFlag(dex, FLAG_SET_CAUGHT);
    }
    gSaveBlock2Ptr->pokedex.mode = DEX_MODE_NATIONAL;
    SetMainCallback2(CB2_OpenPokedex);
}

static void PrepareHeadlessSummary(void)
{
    metloc_u8_t metLocation = MAPSEC_ROUTE_101;
    u16 item = ITEM_LEFTOVERS;

    CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_GARCHOMP, 67, OTID_STRUCT_PLAYER_ID);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_EARTHQUAKE, 0);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_DRAGON_CLAW, 1);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_ROCK_SLIDE, 2);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_PROTECT, 3);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MET_LOCATION, &metLocation);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &item);
    CalculatePlayerPartyCount();
    ShowPokemonSummaryScreen(
        SUMMARY_MODE_NORMAL,
        gParties[B_TRAINER_PLAYER],
        0,
        0,
        CB2_PartyMenuFromStartMenu);
}

static void PrepareHeadlessBag(void)
{
    ClearBag();
    AddBagItem(ITEM_LEFTOVERS, 6);
    AddBagItem(ITEM_ROCKY_HELMET, 4);
    GoToBagMenu(ITEMMENULOCATION_FIELD, POCKET_BATTLE, gInitialMainCB2);
}

static void PrepareHeadlessFrontierPass(void)
{
    SetWarpDestination(
        MAP_GROUP(MAP_BATTLE_FRONTIER_BATTLE_TOWER_LOBBY),
        MAP_NUM(MAP_BATTLE_FRONTIER_BATTLE_TOWER_LOBBY),
        WARP_ID_NONE,
        23,
        6);
    WarpIntoMap();
    InitMap();
    FlagSet(FLAG_SYS_FRONTIER_PASS);
    gSaveBlock2Ptr->frontier.battlePoints = 987;
    ShowFrontierPass(gInitialMainCB2);
}

static void PrepareHeadlessWildBattle(bool32 isDouble)
{
    SetWarpDestination(MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), WARP_ID_NONE, 8, 12);
    WarpIntoMap();
    InitMap();
    SeedRng(0x1234);
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();

    CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_GEODUDE, 14, OTID_STRUCT_PLAYER_ID);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_EARTHQUAKE, 0);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_ROCK_SLIDE, 1);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_PROTECT, 2);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_WIDE_GUARD, 3);
    if (isDouble)
    {
        CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_GARDEVOIR, 14, OTID_STRUCT_PLAYER_ID);
        SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][1], MOVE_DAZZLING_GLEAM, 0);
        SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][1], MOVE_PSYCHIC, 1);
        SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][1], MOVE_HELPING_HAND, 2);
        SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][1], MOVE_PROTECT, 3);
    }
    CalculatePlayerPartyCount();
    CreateWildMon(SPECIES_POOCHYENA, 5);
    if (isDouble)
        CreateHealthyHeadlessMon(&gParties[B_TRAINER_OPPONENT_A][1], SPECIES_PIKACHU, 5, OTID_STRUCT_RANDOM_NO_SHINY);

    ClearBag();
    AddBagItem(ITEM_QUICK_BALL, 10);
    gLastThrownBall = ITEM_QUICK_BALL;
    gBallToDisplay = ITEM_QUICK_BALL;
    gSaveBlock2Ptr->optionsButtonMode = OPTIONS_BUTTON_MODE_NORMAL;
    gActionSelectionCursor[0] = 0;
    gMoveSelectionCursor[0] = 0;
    gBattleTypeFlags = isDouble ? BATTLE_TYPE_DOUBLE : 0;
    gMain.savedCallback = gInitialMainCB2;
    gEcHeadlessFixtureSetupResult = TRUE;
    SetMainCallback2(CB2_InitBattle);
}

static void SetHeadlessStatus(struct Pokemon *mon, u32 status)
{
    SetMonData(mon, MON_DATA_STATUS, &status);
}

static void PrepareHeadlessDoubleStatusAbilityBattle(void)
{
    u32 abilitySlot = 0;

    SetWarpDestination(MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), WARP_ID_NONE, 8, 12);
    WarpIntoMap();
    InitMap();
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ARCANINE, 50, OTID_STRUCT_PLAYER_ID);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ABILITY_NUM, &abilitySlot);
    CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_GARDEVOIR, 50, OTID_STRUCT_PLAYER_ID);
    CreateHealthyHeadlessMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_VENUSAUR, 50, OTID_STRUCT_RANDOM_NO_SHINY);
    CreateHealthyHeadlessMon(&gParties[B_TRAINER_OPPONENT_A][1], SPECIES_PIKACHU, 50, OTID_STRUCT_RANDOM_NO_SHINY);
    SetHeadlessStatus(&gParties[B_TRAINER_PLAYER][0], STATUS1_BURN);
    SetHeadlessStatus(&gParties[B_TRAINER_PLAYER][1], STATUS1_PARALYSIS);
    SetHeadlessStatus(&gParties[B_TRAINER_OPPONENT_A][0], STATUS1_POISON);
    SetHeadlessStatus(&gParties[B_TRAINER_OPPONENT_A][1], STATUS1_SLEEP_TURN(2));
    CalculatePlayerPartyCount();
    gBattleTypeFlags = BATTLE_TYPE_DOUBLE;
    gMain.savedCallback = gInitialMainCB2;
    SetMainCallback2(CB2_InitBattle);
}

static void PrepareHeadlessMegaBattle(void)
{
    u16 stone = ITEM_CHARIZARDITE_X;

    SetWarpDestination(MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), WARP_ID_NONE, 8, 12);
    WarpIntoMap();
    InitMap();
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_CHARIZARD, 50, OTID_STRUCT_PLAYER_ID);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_PROTECT, 0);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_DRAGON_CLAW, 1);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_FLAMETHROWER, 2);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_AIR_SLASH, 3);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &stone);
    CreateHealthyHeadlessMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_VENUSAUR, 50, OTID_STRUCT_RANDOM_NO_SHINY);
    CalculatePlayerPartyCount();
    ClearBag();
    AddBagItem(ITEM_MEGA_RING, 1);
    gBattleTypeFlags = 0;
    gMain.savedCallback = gInitialMainCB2;
    SetMainCallback2(CB2_InitBattle);
}

static void PrepareHeadlessPrimalBattle(void)
{
    u16 blueOrb = ITEM_BLUE_ORB;
    u16 redOrb = ITEM_RED_ORB;

    SetWarpDestination(MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), WARP_ID_NONE, 8, 12);
    WarpIntoMap();
    InitMap();
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_KYOGRE_PRIMAL, 70, OTID_STRUCT_PLAYER_ID);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &blueOrb);
    CreateHealthyHeadlessMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_GROUDON_PRIMAL, 70, OTID_STRUCT_RANDOM_NO_SHINY);
    SetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HELD_ITEM, &redOrb);
    CalculatePlayerPartyCount();
    gBattleTypeFlags = 0;
    gMain.savedCallback = gInitialMainCB2;
    SetMainCallback2(CB2_InitBattle);
}

static void PrepareHeadlessSafariBattle(void)
{
    SetWarpDestination(MAP_GROUP(MAP_SAFARI_ZONE_NORTH), MAP_NUM(MAP_SAFARI_ZONE_NORTH), WARP_ID_NONE, 8, 8);
    WarpIntoMap();
    InitMap();
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_PIKACHU, 30, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    CreateWildMon(SPECIES_CHANSEY, 30);
    gNumSafariBalls = 30;
    gBattleTypeFlags = BATTLE_TYPE_SAFARI;
    gMain.savedCallback = gInitialMainCB2;
    SetMainCallback2(CB2_InitBattle);
}

static void PrepareHeadlessPokeblock(void)
{
    struct Pokeblock *pokeblock = &gSaveBlock1Ptr->pokeblocks[0];

    CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_MILOTIC, 40, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    *pokeblock = (struct Pokeblock){PBLOCK_CLR_GOLD, 40, 35, 30, 25, 20, 10};
    ChooseMonToGivePokeblock(pokeblock, gInitialMainCB2);
}

static void PrepareHeadlessGoldTrainerCard(void)
{
    SetGameStat(GAME_STAT_ENTERED_HOF, 1);
    EnableNationalPokedex();
    for (enum NationalDexOrder dex = 1; dex < NATIONAL_DEX_COUNT; dex++)
        GetSetPokedexFlag(dex, FLAG_SET_CAUGHT);
    for (u32 i = 0; i < CONTEST_CATEGORIES_COUNT; i++)
        gSaveBlock1Ptr->contestWinners[MUSEUM_CONTEST_WINNERS_START + i].species = SPECIES_MILOTIC;
    for (u32 i = 0; i < NUM_FRONTIER_FACILITIES; i++)
    {
        FlagSet(FLAG_SYS_TOWER_SILVER + 2 * i);
        FlagSet(FLAG_SYS_TOWER_GOLD + 2 * i);
    }
    ShowPlayerTrainerCard(gInitialMainCB2);
}

static void PrepareHeadlessDomeInfo(void)
{
    VarSet(VAR_FRONTIER_BATTLE_MODE, FRONTIER_MODE_SINGLES);
    gSaveBlock2Ptr->frontier.lvlMode = FRONTIER_LVL_50;
    gSpecialVar_0x8004 = BATTLE_DOME_FUNC_INIT;
    CallBattleDomeFunction();
    gSpecialVar_0x8004 = BATTLE_DOME_FUNC_INIT_TRAINERS;
    CallBattleDomeFunction();
    gSpecialVar_0x8004 = BATTLE_DOME_FUNC_SHOW_OPPONENT_INFO;
    CallBattleDomeFunction();
}

static void PrepareHeadlessContestResults(void)
{
    static const enum Species species[CONTESTANT_COUNT] =
    {
        SPECIES_MILOTIC,
        SPECIES_PIKACHU,
        SPECIES_ALTARIA,
        SPECIES_ROSERADE,
    };
    static const u8 *const names[CONTESTANT_COUNT] =
    {
        COMPOUND_STRING("AQUA"),
        COMPOUND_STRING("SPARK"),
        COMPOUND_STRING("ARIA"),
        COMPOUND_STRING("ROSE"),
    };

    gContestPlayerMonIndex = 0;
    gSpecialVar_ContestCategory = CONTEST_CATEGORY_BEAUTY;
    gSpecialVar_ContestRank = CONTEST_RANK_MASTER;
    for (u32 i = 0; i < CONTESTANT_COUNT; i++)
    {
        memset(&gContestMons[i], 0, sizeof(gContestMons[i]));
        gContestMons[i].species = species[i];
        StringCopy(gContestMons[i].nickname, names[i]);
        StringCopy(gContestMons[i].trainerName, i == 0 ? sEcHeadlessPlayerName : names[i]);
        gContestMons[i].personality = 0x12340000 + i;
        gContestMons[i].trainerGfxId = OBJ_EVENT_GFX_BRENDAN_NORMAL;
        gContestFinalStandings[i] = i;
        gContestMonRound1Points[i] = 400 - i * 50;
        gContestMonRound2Points[i] = 400 - i * 40;
        gContestMonTotalPoints[i] = gContestMonRound1Points[i] + gContestMonRound2Points[i];
    }
}

static void PrepareHeadlessFairySummary(void)
{
    metloc_u8_t metLocation = MAPSEC_ROUTE_101;

    CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_SYLVEON, 50, OTID_STRUCT_PLAYER_ID);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_MOONBLAST, 0);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_HYPER_VOICE, 1);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_PROTECT, 2);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_HELPING_HAND, 3);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MET_LOCATION, &metLocation);
    CalculatePlayerPartyCount();
    ShowPokemonSummaryScreen(
        SUMMARY_MODE_NORMAL,
        gParties[B_TRAINER_PLAYER],
        0,
        0,
        CB2_PartyMenuFromStartMenu);
}

static bool32 IsHeadlessPokedexStateObserved(void)
{
    switch (gEcHeadlessFixtureParam)
    {
    case EC_HEADLESS_POKEDEX_LIST:
        return IsPokedexHeadlessOnScreen(PAGE_MAIN, AREA_SCREEN, FALSE);
    case EC_HEADLESS_POKEDEX_INFO:
        return IsPokedexHeadlessOnScreen(PAGE_INFO, AREA_SCREEN, FALSE);
    case EC_HEADLESS_POKEDEX_AREA:
        return IsPokedexHeadlessOnScreen(PAGE_AREA, AREA_SCREEN, FALSE);
    case EC_HEADLESS_POKEDEX_STATS:
        return IsPokedexHeadlessOnScreen(STATS_SCREEN, AREA_SCREEN, FALSE);
    case EC_HEADLESS_POKEDEX_EVOLUTIONS:
        return IsPokedexHeadlessOnScreen(EVO_SCREEN, EVO_SCREEN, FALSE);
    case EC_HEADLESS_POKEDEX_FORMS:
        return IsPokedexHeadlessOnScreen(FORMS_SCREEN, FORMS_SCREEN, FALSE);
    case EC_HEADLESS_POKEDEX_CRY:
        return IsPokedexHeadlessOnScreen(PAGE_CRY, CRY_SCREEN, FALSE);
    case EC_HEADLESS_POKEDEX_SIZE:
        return IsPokedexHeadlessOnScreen(PAGE_SIZE, SIZE_SCREEN, FALSE);
    case EC_HEADLESS_POKEDEX_SEARCH:
        return IsPokedexHeadlessOnScreen(PAGE_SEARCH, AREA_SCREEN, FALSE);
    case EC_HEADLESS_POKEDEX_SEARCH_RESULTS:
        return IsPokedexHeadlessOnScreen(PAGE_SEARCH_RESULTS, AREA_SCREEN, TRUE);
    }
    return FALSE;
}

static bool32 IsHeadlessSummaryStateObserved(void)
{
    switch (gEcHeadlessFixtureParam)
    {
    case EC_HEADLESS_SUMMARY_INFO:
        return IsPokemonSummaryHeadlessOnPage(PSS_PAGE_INFO, FALSE);
    case EC_HEADLESS_SUMMARY_SKILLS:
        return IsPokemonSummaryHeadlessOnPage(PSS_PAGE_SKILLS, FALSE);
    case EC_HEADLESS_SUMMARY_BATTLE_MOVES:
        return IsPokemonSummaryHeadlessOnPage(PSS_PAGE_BATTLE_MOVES, FALSE);
    case EC_HEADLESS_SUMMARY_CONTEST_MOVES:
        return IsPokemonSummaryHeadlessOnPage(PSS_PAGE_CONTEST_MOVES, FALSE);
    case EC_HEADLESS_SUMMARY_MOVE_DETAILS:
        return IsPokemonSummaryHeadlessOnPage(PSS_PAGE_BATTLE_MOVES, TRUE);
    case EC_HEADLESS_SUMMARY_PARTY_ROUNDTRIP:
        return IsPartyMenuHeadlessAwaitingSelection();
    }
    return FALSE;
}

void EmeraldChampionsHeadlessObserve(void)
{
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_HEAL_LOCATION_WHITEOUT)
    {
        bool32 stateMatches = IsWhiteoutRespawnHeadlessState(gEcHeadlessFixtureParam);

        if (IsLastHealLocationPlayerHouse())
            gEcHeadlessFixtureObservedResult = stateMatches && ArePlayerFieldControlsLocked();
        else
            gEcHeadlessFixtureObservedResult = stateMatches
                && FieldEffectActiveListContains(FLDEFF_POKECENTER_HEAL);
        return;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_FIELD_MOVE_CUT
     || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_FIELD_MOVE_ROCK_SMASH
     || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_FIELD_MOVE_STRENGTH)
    {
        gEcHeadlessFixtureSetupResult = TRUE;
        if (FieldEffectActiveListContains(FLDEFF_FIELD_MOVE_SHOW_MON))
            gEcHeadlessFixtureObservedResult = TRUE;
        return;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_FLIGHT_BEACON)
    {
        if (gEcHeadlessFixtureTrigger)
        {
            gEcHeadlessFixtureTrigger = FALSE;
            CreateTask(Task_HeadlessOpenFlightBeaconMap, 0);
        }
        if (sEcHeadlessFlightRider == SPECIES_WINGULL
         && gFieldMoveShowMonSpeciesOverride == SPECIES_NONE
         && FieldEffectActiveListContains(FLDEFF_USE_FLY)
         && FieldEffectActiveListContains(FLDEFF_FIELD_MOVE_SHOW_MON))
            gEcHeadlessFixtureObservedResult = TRUE;
        return;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_HALL_OF_FAME_RECORD)
    {
        if (gEcHeadlessFixtureTrigger && !gEcHeadlessFixtureSetupResult
         && gMain.callback2 == CB2_Overworld)
        {
            FieldEffectStart(FLDEFF_HALL_OF_FAME_RECORD);
            gEcHeadlessFixtureSetupResult = TRUE;
        }
        gEcHeadlessFixtureObservedResult = gEcHeadlessFixtureSetupResult
            && IsHallOfFameRecordHeadlessVisible(gEcHeadlessFixtureParam);
        return;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_MULTI_CORRIDOR_DOOR)
    {
        if (gEcHeadlessFixtureTrigger && !gEcHeadlessFixtureSetupResult
         && gMain.callback2 == CB2_Overworld)
        {
            u32 x = ((gEcHeadlessFixtureParam & 1) ? 8 : 7) + MAP_OFFSET;
            u32 y = 1 + MAP_OFFSET;

            if (gEcHeadlessFixtureParam >= 2)
            {
                FieldSetDoorOpened(x, y);
                gEcHeadlessFixtureSetupResult = FieldAnimateDoorClose(x, y) >= 0;
            }
            else
            {
                gEcHeadlessFixtureSetupResult = FieldAnimateDoorOpen(x, y) >= 0;
            }
        }
        gEcHeadlessFixtureObservedResult = gEcHeadlessFixtureSetupResult
            && FieldIsDoorAnimationRunning();
        return;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_FURFROU_TRIMS)
    {
        bool32 menuActive;

        if (!gEcHeadlessFixtureSetupResult && gMain.callback2 == CB2_Overworld)
        {
            gSpecialVar_0x8004 = SCROLL_MULTI_FURFROU_TRIMS;
            gSpecialVar_0x8005 = 0;
            ShowScrollableMultichoice();
            gEcHeadlessFixtureSetupResult = TRUE;
        }
        menuActive = IsScrollableMultichoiceHeadlessActive(SCROLL_MULTI_FURFROU_TRIMS);
        if (menuActive)
            sEcHeadlessFurfrouMenuOpened = TRUE;
        switch (gEcHeadlessFixtureParam)
        {
        case EC_HEADLESS_FURFROU_TRIMS_OPEN:
            gEcHeadlessFixtureObservedResult = menuActive
                && gScrollableMultichoice_ScrollOffset == 0;
            break;
        case EC_HEADLESS_FURFROU_TRIMS_SCROLLED:
            gEcHeadlessFixtureObservedResult = menuActive
                && gScrollableMultichoice_ScrollOffset == 6;
            break;
        case EC_HEADLESS_FURFROU_TRIMS_B_CANCELLED:
            gEcHeadlessFixtureObservedResult = sEcHeadlessFurfrouMenuOpened
                && !menuActive
                && gSpecialVar_Result == MULTI_B_PRESSED;
            break;
        case EC_HEADLESS_FURFROU_TRIMS_BACK_SELECTED:
            gEcHeadlessFixtureObservedResult = sEcHeadlessFurfrouMenuOpened
                && !menuActive
                && gSpecialVar_Result == 10;
            break;
        }
        return;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_POKEDEX)
    {
        gEcHeadlessFixtureObservedResult = IsHeadlessPokedexStateObserved();
        return;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_SUMMARY
     || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_FAIRY_SUMMARY)
    {
        gEcHeadlessFixtureObservedResult = IsHeadlessSummaryStateObserved();
        return;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_BAG)
    {
        gEcHeadlessFixtureObservedResult = gEcHeadlessFixtureParam < POCKETS_COUNT
            && IsBagHeadlessOnPocket(gEcHeadlessFixtureParam);
        return;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_MAGMA_SPARKLE
     && gMain.callback2 == CB2_Overworld)
    {
        gEcHeadlessFixtureSetupResult = TRUE;
        if (gEcHeadlessFixtureTrigger && !gEcHeadlessFixtureObservedResult)
        {
            gFieldEffectArguments[0] = 16;
            gFieldEffectArguments[1] = 21;
            gFieldEffectArguments[2] = 0;
            FieldEffectStart(FLDEFF_SPARKLE);
            gEcHeadlessFixtureObservedResult = TRUE;
        }
        return;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CENTER_OLDALE
     && gEcHeadlessFixtureParam != 0
     && gMain.callback2 == CB2_Overworld)
    {
        gEcHeadlessFixtureSetupResult = TRUE;
        if (FieldEffectActiveListContains(FLDEFF_POKECENTER_HEAL))
            gEcHeadlessFixtureObservedResult = TRUE;
        return;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CONTEST_RESULTS
     && !gEcHeadlessFixtureSetupResult
     && gMain.callback2 == CB2_Overworld)
    {
        ShowContestResults();
        gEcHeadlessFixtureSetupResult = TRUE;
        return;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_SLOT_MACHINE
     && !gEcHeadlessFixtureSetupResult
     && gMain.callback2 == CB2_Overworld)
    {
        PlaySlotMachine(0, gInitialMainCB2);
        gEcHeadlessFixtureSetupResult = TRUE;
        return;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_SPECIES_OVERWORLD
     && !gEcHeadlessFixtureSetupResult
     && gMain.callback2 == CB2_Overworld)
    {
        const struct EcHeadlessOverworldFixture *fixture;

        if (gEcHeadlessFixtureParam >= ARRAY_COUNT(sEcHeadlessOverworldFixtures))
            return;
        fixture = &sEcHeadlessOverworldFixtures[gEcHeadlessFixtureParam];
        for (u32 objectEventId = 0; objectEventId < OBJECT_EVENTS_COUNT; objectEventId++)
        {
            struct ObjectEvent *objectEvent = &gObjectEvents[objectEventId];
            struct Sprite *sprite;

            if (!objectEvent->active
             || objectEvent->mapGroup != MAP_GROUP(fixture->map)
             || objectEvent->mapNum != MAP_NUM(fixture->map)
             || objectEvent->graphicsId != GetHeadlessOverworldFixtureGraphicsId(fixture->species))
                continue;

            gEcHeadlessFixtureSetupResult = TRUE;
            if (objectEvent->invisible
             || objectEvent->offScreen
             || objectEvent->spriteId >= MAX_SPRITES)
                return;
            sprite = &gSprites[objectEvent->spriteId];
            if (sprite->inUse
             && sprite->x + sprite->x2 >= -64
             && sprite->x + sprite->x2 < DISPLAY_WIDTH + 64
             && sprite->y + sprite->y2 >= -64
             && sprite->y + sprite->y2 < DISPLAY_HEIGHT + 64)
                gEcHeadlessFixtureObservedResult = TRUE;
            return;
        }
        return;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_SPECIES_OVERWORLD_BACKGROUND
     && gMain.callback2 == CB2_Overworld)
    {
        const struct EcHeadlessOverworldFixture *fixture;

        if (gEcHeadlessFixtureParam >= ARRAY_COUNT(sEcHeadlessOverworldFixtures))
            return;
        fixture = &sEcHeadlessOverworldFixtures[gEcHeadlessFixtureParam];
        for (u32 objectEventId = 0; objectEventId < OBJECT_EVENTS_COUNT; objectEventId++)
        {
            struct ObjectEvent *objectEvent = &gObjectEvents[objectEventId];

            if (!objectEvent->active
             || objectEvent->mapGroup != MAP_GROUP(fixture->map)
             || objectEvent->mapNum != MAP_NUM(fixture->map)
             || objectEvent->graphicsId != GetHeadlessOverworldFixtureGraphicsId(fixture->species))
                continue;

            RemoveObjectEvent(objectEvent);
            gEcHeadlessFixtureSetupResult = TRUE;
            return;
        }
        if (gEcHeadlessFixtureSetupResult)
            gEcHeadlessFixtureObservedResult = TRUE;
        return;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_STORAGE
     && !gEcHeadlessFixtureSetupResult
     && gMain.callback2 == CB2_Overworld)
    {
        ShowPokemonStorageSystemPC();
        gEcHeadlessFixtureSetupResult = TRUE;
        return;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_STARTER_REGIONS
     && !gEcHeadlessFixtureSetupResult
     && gMain.callback2 == CB2_Overworld)
    {
        ScriptContext_SetupScript(Common_EventScript_ChooseStarterRegion);
        gEcHeadlessFixtureSetupResult = TRUE;
        return;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_MOVE_ANIMATION
     && gMain.inBattle
     && gBattleStruct != NULL)
    {
        if (gEcHeadlessFixtureTrigger)
        {
            gEcHeadlessFixtureTrigger = FALSE;
            gBattlerAttacker = B_BATTLER_0;
            gBattlerTarget = B_BATTLER_1;
            gCurrentMove = gEcHeadlessFixtureParam;
            DoMoveAnim(gCurrentMove);
            gEcHeadlessFixtureSetupResult = TRUE;
        }
        if (gEcHeadlessFixtureSetupResult && gAnimScriptActive)
        {
            gAnimScriptCallback();
            gEcHeadlessFixtureObservedResult = TRUE;
        }
        return;
    }

    if (gEcHeadlessFixtureObservedResult || !gMain.inBattle || gBattleStruct == NULL)
        return;

    switch (gEcHeadlessFixtureActiveScenario)
    {
    case EC_HEADLESS_SCENARIO_DOUBLE_STATUS_ABILITY:
        if (gHealthboxSpriteIds[B_BATTLER_0] != MAX_SPRITES
         && gHealthboxSpriteIds[B_BATTLER_1] != MAX_SPRITES
         && gHealthboxSpriteIds[B_BATTLER_2] != MAX_SPRITES
         && gHealthboxSpriteIds[B_BATTLER_3] != MAX_SPRITES)
        {
            gEcHeadlessFixtureSetupResult = TRUE;
            if (gEcHeadlessFixtureTrigger)
            {
                gEcHeadlessFixtureTrigger = FALSE;
                CreateAbilityPopUp(B_BATTLER_0, ABILITY_INTIMIDATE, TRUE);
            }
        }
        if (gEcHeadlessFixtureSetupResult && IsAnyAbilityPopUpActive())
        {
            u8 left = gBattleStruct->abilityPopUpSpriteIds[B_BATTLER_0][0];
            u8 right = gBattleStruct->abilityPopUpSpriteIds[B_BATTLER_0][1];

            if (left < MAX_SPRITES && right < MAX_SPRITES
             && gSprites[left].inUse && gSprites[right].inUse
             && gSprites[left].x + gSprites[left].x2 == 24)
                gEcHeadlessFixtureObservedResult = TRUE;
        }
        break;
    case EC_HEADLESS_SCENARIO_MEGA:
        if (gEcHeadlessFixtureParam == 0)
        {
            if (gBattleStruct->gimmick.triggerSpriteId != MAX_SPRITES)
            {
                gEcHeadlessFixtureSetupResult = TRUE;
                if (gSprites[gBattleStruct->gimmick.triggerSpriteId].inUse)
                    gEcHeadlessFixtureObservedResult = TRUE;
            }
        }
        else if (gBattleMons[B_BATTLER_0].species == SPECIES_CHARIZARD_MEGA_X)
        {
            u8 indicator = gBattleStruct->gimmick.indicatorSpriteId[B_BATTLER_0];

            gEcHeadlessFixtureSetupResult = TRUE;
            if (indicator < MAX_SPRITES && gSprites[indicator].inUse
             && ++sEcHeadlessObservedDelay >= 120)
                gEcHeadlessFixtureObservedResult = TRUE;
        }
        break;
    case EC_HEADLESS_SCENARIO_PRIMALS:
        if (gBattleMons[B_BATTLER_0].species == SPECIES_KYOGRE_PRIMAL
         && gBattleMons[B_BATTLER_1].species == SPECIES_GROUDON_PRIMAL)
        {
            u8 alpha = gBattleStruct->gimmick.indicatorSpriteId[B_BATTLER_0];
            u8 omega = gBattleStruct->gimmick.indicatorSpriteId[B_BATTLER_1];

            gEcHeadlessFixtureSetupResult = TRUE;
            if (alpha < MAX_SPRITES && omega < MAX_SPRITES
             && gSprites[alpha].inUse && gSprites[omega].inUse
             && ++sEcHeadlessObservedDelay >= 60)
                gEcHeadlessFixtureObservedResult = TRUE;
        }
        break;
    case EC_HEADLESS_SCENARIO_SAFARI:
        if ((gBattleTypeFlags & BATTLE_TYPE_SAFARI)
         && gHealthboxSpriteIds[B_BATTLER_0] < MAX_SPRITES
         && gSprites[gHealthboxSpriteIds[B_BATTLER_0]].inUse)
        {
            gEcHeadlessFixtureSetupResult = TRUE;
            gEcHeadlessFixtureObservedResult = TRUE;
        }
        break;
    case EC_HEADLESS_SCENARIO_WILD_ACTION_MENU:
        if (gLastUsedBallMenuPresent
         && gBallToDisplay == ITEM_QUICK_BALL
         && gBattleStruct->ballSpriteIds[0] != MAX_SPRITES
         && gBattleStruct->ballSpriteIds[1] != MAX_SPRITES)
            gEcHeadlessFixtureObservedResult = TRUE;
        break;
    case EC_HEADLESS_SCENARIO_MOVE_DETAILS:
        if (gBattle_BG0_Y == DISPLAY_HEIGHT * 2
         && gBattleStruct->descriptionSubmenu
         && gCategoryIconSpriteId != 0xFF
         && gMoveSelectionCursor[0] == 0)
            gEcHeadlessFixtureObservedResult = TRUE;
        break;
    }
}

void CB2_EmeraldChampionsHeadlessFixture(void)
{
    u32 scenario = gEcHeadlessFixtureScenario;

    // The host writes the selected scenario only after CRT startup has cleared
    // EWRAM. Until then the fixture deliberately renders a blank boot frame.
    if (scenario == EC_HEADLESS_SCENARIO_NONE)
        return;

    gEcHeadlessFixtureScenario = EC_HEADLESS_SCENARIO_NONE;
    gEcHeadlessFixtureActiveScenario = scenario;
    gEcHeadlessFixtureSetupResult = FALSE;
    gEcHeadlessFixtureObservedResult = FALSE;
    gEcHeadlessFixtureTrigger = FALSE;
    sEcHeadlessObservedDelay = 0;
    sEcHeadlessFurfrouMenuOpened = FALSE;
    PrepareHeadlessNewGame();

    switch (scenario)
    {
    case EC_HEADLESS_SCENARIO_CENTER_OLDALE:
        if (gEcHeadlessFixtureParam != 0)
        {
            // An established save holds every Center tool; without the spray
            // the nurse's back-fill dialog would shift the scenario timeline.
            AddBagItem(ITEM_POKE_VIAL, 1);
            AddBagItem(ITEM_LEVELER, 1);
            AddBagItem(ITEM_REPEL_SPRAY, 1);
            AddBagItem(ITEM_FLIGHT_BEACON, 1);
            VarSet(VAR_POKE_VIAL_MAX_CHARGES, 1);
            VarSet(VAR_POKE_VIAL_CHARGES, 1);
            PrepareCircuitParty();
        }
        if (gEcHeadlessFixtureParam == 2
         || gEcHeadlessFixtureParam == 4
         || gEcHeadlessFixtureParam == 6)
        {
            SetLastHealLocationWarp(
                gEcHeadlessFixtureParam == 4
                    ? HEAL_LOCATION_EVER_GRANDE_CITY_POKEMON_LEAGUE
                    : gEcHeadlessFixtureParam == 6
                        ? HEAL_LOCATION_LAVARIDGE_TOWN
                        : HEAL_LOCATION_OLDALE_TOWN
            );
            DoWhiteOut();
            gFieldCallback = FieldCB_RushInjuredPokemonToCenter;
            gFieldCallback2 = NULL;
            SetMainCallback2(CB2_LoadMap);
        }
        else if (gEcHeadlessFixtureParam == 5)
        {
            LoadHeadlessMap(MAP_TRAINER_HILL_ENTRANCE, 4, 10);
        }
        else
        {
            LoadHeadlessMap(MAP_OLDALE_TOWN_POKEMON_CENTER_1F, 8, 7);
        }
        break;
    case EC_HEADLESS_SCENARIO_CENTER_LAVARIDGE:
        LoadHeadlessMap(MAP_LAVARIDGE_TOWN_POKEMON_CENTER_1F, 8, 7);
        break;
    case EC_HEADLESS_SCENARIO_ABILITY_MENU:
        PrepareAbilityMenu();
        break;
    case EC_HEADLESS_SCENARIO_OPTIONS:
        SetMainCallback2(CB2_InitOptionMenu);
        break;
    case EC_HEADLESS_SCENARIO_BATTLE_VENDOR:
        if (gEcHeadlessFixtureParam != 0)
            FlagSet(FLAG_BADGE08_GET);
        LoadHeadlessMap(MAP_OLDALE_TOWN_POKEMON_CENTER_1F, 2, 3);
        break;
    case EC_HEADLESS_SCENARIO_MOVE_SPECIALIST:
        GiveHeadlessGeodude(30);
        if (gEcHeadlessFixtureParam != 0)
        {
            bool8 isEgg = TRUE;

            CreateHealthyHeadlessMon(
                &gParties[B_TRAINER_PLAYER][1],
                SPECIES_TOGEPI,
                1,
                OTID_STRUCT_PLAYER_ID
            );
            SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_IS_EGG, &isEgg);
            CalculatePlayerPartyCount();
        }
        if (gEcHeadlessFixtureParam == 2)
            LoadHeadlessMap(MAP_FALLARBOR_TOWN_MOVE_RELEARNERS_HOUSE, 7, 5);
        else
            LoadHeadlessMap(MAP_OLDALE_TOWN_POKEMON_CENTER_1F, 13, 3);
        break;
    case EC_HEADLESS_SCENARIO_MOVE_SPECIALIST_CURRENT_SET:
        GiveHeadlessGeodude(30);
        ApplyEmeraldChampionsBattleSetChoice(&gParties[B_TRAINER_PLAYER][0], 1);
        LoadHeadlessMap(MAP_OLDALE_TOWN_POKEMON_CENTER_1F, 13, 3);
        break;
    case EC_HEADLESS_SCENARIO_THUNDURUS:
        LoadHeadlessMap(MAP_ROUTE110, 6, 16);
        break;
    case EC_HEADLESS_SCENARIO_TORNADUS:
        LoadHeadlessMap(MAP_ROUTE119, 29, 11);
        break;
    case EC_HEADLESS_SCENARIO_LANDORUS:
        LoadHeadlessMap(MAP_ROUTE111_RUINS_EXTERIOR, 9, 13);
        break;
    case EC_HEADLESS_SCENARIO_GAME_CORNER:
        AddBagItem(ITEM_COIN_CASE, 1);
        SetCoins(9999);
        LoadHeadlessMap(MAP_MAUVILLE_CITY_GAME_CORNER, 13, 3);
        break;
    case EC_HEADLESS_SCENARIO_CIRCUIT_LOBBY:
        FlagSet(FLAG_SYS_GAME_CLEAR);
        PrepareCircuitParty();
        LoadHeadlessMap(MAP_BATTLE_FRONTIER_BATTLE_TOWER_LOBBY, 23, 6);
        break;
    case EC_HEADLESS_SCENARIO_LEVELER:
        GiveHeadlessGeodude(1);
        gSpecialVar_ItemId = ITEM_LEVELER;
        StartLevelerPartySequence(CB2_PartyMenuFromStartMenu);
        break;
    case EC_HEADLESS_SCENARIO_ALL_LEGAL_MOVES:
        PrepareAllLegalMoves(SPECIES_GEODUDE);
        break;
    case EC_HEADLESS_SCENARIO_ALL_LEGAL_MOVES_MEW:
        PrepareAllLegalMoves(SPECIES_MEW);
        break;
    case EC_HEADLESS_SCENARIO_WILD_ACTION_MENU:
    case EC_HEADLESS_SCENARIO_MOVE_DETAILS:
        PrepareHeadlessWildBattle(FALSE);
        break;
    case EC_HEADLESS_SCENARIO_MOVE_ANIMATION:
        PrepareHeadlessWildBattle(TRUE);
        // This scenario uses setupResult to prove the requested animation was
        // launched, rather than merely proving that the battle was created.
        gEcHeadlessFixtureSetupResult = FALSE;
        break;
    case EC_HEADLESS_SCENARIO_NAMING:
        DoNamingScreen(
            NAMING_SCREEN_NICKNAME,
            sEcHeadlessName,
            SPECIES_GEODUDE,
            MON_MALE,
            0,
            gInitialMainCB2);
        break;
    case EC_HEADLESS_SCENARIO_STORAGE:
    {
        u16 item = ITEM_LEFTOVERS;

        CreateBoxMon(
            &gPokemonStoragePtr->boxes[0][0],
            SPECIES_GEODUDE,
            30,
            0,
            OTID_STRUCT_PLAYER_ID);
        SetBoxMonData(&gPokemonStoragePtr->boxes[0][0], MON_DATA_HELD_ITEM, &item);
        LoadHeadlessMap(MAP_OLDALE_TOWN_POKEMON_CENTER_1F, 8, 7);
        break;
    }
    case EC_HEADLESS_SCENARIO_STARTER_REGIONS:
        LoadHeadlessMap(MAP_OLDALE_TOWN_POKEMON_CENTER_1F, 8, 7);
        break;
    case EC_HEADLESS_SCENARIO_CIRCUIT_ROOM:
        PrepareCircuitParty();
        ChampionsCircuitBegin();
        LoadHeadlessMap(MAP_BATTLE_FRONTIER_BATTLE_TOWER_BATTLE_ROOM, 5, 8);
        break;
    case EC_HEADLESS_SCENARIO_POKEDEX:
        PrepareHeadlessPokedex();
        gEcHeadlessFixtureSetupResult = TRUE;
        break;
    case EC_HEADLESS_SCENARIO_SUMMARY:
        PrepareHeadlessSummary();
        gEcHeadlessFixtureSetupResult = TRUE;
        break;
    case EC_HEADLESS_SCENARIO_BAG:
        PrepareHeadlessBag();
        gEcHeadlessFixtureSetupResult = TRUE;
        break;
    case EC_HEADLESS_SCENARIO_FRONTIER_PASS:
        PrepareHeadlessFrontierPass();
        break;
    case EC_HEADLESS_SCENARIO_EMBER_PATH_WARDEN:
        LoadHeadlessMap(MAP_EMBER_PATH, 8, 39);
        break;
    case EC_HEADLESS_SCENARIO_SPECIES_OVERWORLD:
        if (gEcHeadlessFixtureParam < ARRAY_COUNT(sEcHeadlessOverworldFixtures))
        {
            const struct EcHeadlessOverworldFixture *fixture =
                &sEcHeadlessOverworldFixtures[gEcHeadlessFixtureParam];

            PrepareHeadlessOverworldFixtureState(fixture->species);
            FlagSet(FLAG_SYS_USE_FLASH);
            LoadHeadlessMap(fixture->map, fixture->playerX, fixture->playerY);
        }
        else
        {
            SetMainCallback2(gInitialMainCB2);
        }
        break;
    case EC_HEADLESS_SCENARIO_SPECIES_OVERWORLD_BACKGROUND:
        if (gEcHeadlessFixtureParam < ARRAY_COUNT(sEcHeadlessOverworldFixtures))
        {
            const struct EcHeadlessOverworldFixture *fixture =
                &sEcHeadlessOverworldFixtures[gEcHeadlessFixtureParam];

            PrepareHeadlessOverworldFixtureState(fixture->species);
            FlagSet(FLAG_SYS_USE_FLASH);
            LoadHeadlessMap(fixture->map, fixture->playerX, fixture->playerY);
        }
        else
        {
            SetMainCallback2(gInitialMainCB2);
        }
        break;
    case EC_HEADLESS_SCENARIO_DOUBLE_STATUS_ABILITY:
        PrepareHeadlessDoubleStatusAbilityBattle();
        break;
    case EC_HEADLESS_SCENARIO_MEGA:
        PrepareHeadlessMegaBattle();
        break;
    case EC_HEADLESS_SCENARIO_PRIMALS:
        PrepareHeadlessPrimalBattle();
        break;
    case EC_HEADLESS_SCENARIO_SAFARI:
        PrepareHeadlessSafariBattle();
        break;
    case EC_HEADLESS_SCENARIO_TITLE:
        SetMainCallback2(CB2_InitTitleScreen);
        break;
    case EC_HEADLESS_SCENARIO_BIRCH:
        SetMainCallback2(CB2_InitMainMenu);
        break;
    case EC_HEADLESS_SCENARIO_POKEBLOCK:
        PrepareHeadlessPokeblock();
        break;
    case EC_HEADLESS_SCENARIO_TRAINER_CARD:
        PrepareHeadlessGoldTrainerCard();
        break;
    case EC_HEADLESS_SCENARIO_DOME_INFO:
        PrepareHeadlessDomeInfo();
        break;
    case EC_HEADLESS_SCENARIO_CONTEST_RESULTS:
        PrepareHeadlessContestResults();
        LoadHeadlessMap(MAP_CONTEST_HALL, 7, 10);
        break;
    case EC_HEADLESS_SCENARIO_SLOT_MACHINE:
        SetCoins(5000);
        LoadHeadlessMap(MAP_MAUVILLE_CITY_GAME_CORNER, 8, 8);
        break;
    case EC_HEADLESS_SCENARIO_FAIRY_SUMMARY:
        PrepareHeadlessFairySummary();
        gEcHeadlessFixtureSetupResult = TRUE;
        break;
    case EC_HEADLESS_SCENARIO_MAGMA_SPARKLE:
        LoadHeadlessMap(MAP_MAGMA_HIDEOUT_4F, 16, 22);
        break;
    case EC_HEADLESS_SCENARIO_FURFROU_TRIMS:
        LoadHeadlessMap(MAP_SLATEPORT_CITY_POKEMON_FAN_CLUB, 12, 11);
        break;
    case EC_HEADLESS_SCENARIO_HEAL_LOCATION_WHITEOUT:
        PrepareCircuitParty();
        SetLastHealLocationWarp(gEcHeadlessFixtureParam);
        DoWhiteOut();
        gFieldCallback = FieldCB_RushInjuredPokemonToCenter;
        gFieldCallback2 = NULL;
        SetMainCallback2(CB2_LoadMap);
        gEcHeadlessFixtureSetupResult = TRUE;
        break;
    // Field moves without HM carriers: the party is one Zigzagoon that could
    // learn the move but does not know it, the badge and HM flags are set, and
    // the player stands facing the obstacle. The scenario taps UP, A, then A on
    // the Yes/No, and the observer latches the "used <move>!" showcase.
    case EC_HEADLESS_SCENARIO_FIELD_MOVE_CUT:
        PrepareHeadlessFieldMoveParty(FLAG_BADGE01_GET, FLAG_RECEIVED_HM_CUT);
        LoadHeadlessMap(MAP_ROUTE104, 35, 23);
        break;
    case EC_HEADLESS_SCENARIO_FIELD_MOVE_ROCK_SMASH:
        PrepareHeadlessFieldMoveParty(FLAG_BADGE03_GET, FLAG_RECEIVED_HM_ROCK_SMASH);
        LoadHeadlessMap(MAP_ROUTE111, 18, 102);
        break;
    case EC_HEADLESS_SCENARIO_FIELD_MOVE_STRENGTH:
        PrepareHeadlessFieldMoveParty(FLAG_BADGE04_GET, FLAG_RECEIVED_HM_STRENGTH);
        LoadHeadlessMap(MAP_FIERY_PATH, 10, 16);
        break;
    // The Flight Beacon: nobody in the party can fly, but a boxed Wingull knows
    // Fly. The trigger opens the fly map the way the item does, A picks the
    // current town, and the observer latches the Fly showcase carrying the
    // boxed rider with the override consumed.
    case EC_HEADLESS_SCENARIO_FLIGHT_BEACON:
    {
        struct Pokemon rider;

        PrepareHeadlessFieldMoveParty(FLAG_BADGE06_GET, FLAG_RECEIVED_HM_FLY);
        AddBagItem(ITEM_FLIGHT_BEACON, 1);
        FlagSet(FLAG_VISITED_LITTLEROOT_TOWN);
        CreateMon(&rider, SPECIES_WINGULL, 20, 0, OTID_STRUCT_PLAYER_ID);
        SetMonMoveSlot(&rider, MOVE_FLY, 0);
        gPokemonStoragePtr->boxes[0][0] = rider.box;
        sEcHeadlessFlightRider = SPECIES_NONE;
        LoadHeadlessMap(MAP_LITTLEROOT_TOWN, 8, 10);
        break;
    }
    case EC_HEADLESS_SCENARIO_HALL_OF_FAME_RECORD:
        PrepareHeadlessHallParty(gEcHeadlessFixtureParam);
        LoadHeadlessMap(MAP_EVER_GRANDE_CITY_HALL_OF_FAME, 7, 11);
        gFieldCallback = FieldCB_HeadlessSuppressOnFrame;
        break;
    case EC_HEADLESS_SCENARIO_MULTI_CORRIDOR_DOOR:
        LoadHeadlessMap(MAP_BATTLE_FRONTIER_BATTLE_TOWER_MULTI_CORRIDOR, 8, 3);
        gFieldCallback = FieldCB_HeadlessSuppressOnFrame;
        break;
    default:
        SetMainCallback2(gInitialMainCB2);
        break;
    }
}

#endif // EC_HEADLESS_FIXTURES
