#include "global.h"
#include "emerald_champions_studio.h"
#include "reload_save.h"
#include "money.h"

#if EC_HEADLESS_FIXTURES

#include "battle.h"
#include "battle_anim.h"
#include "battle_gimmick.h"
#include "battle_interface.h"
#include "battle_main.h"
#include "battle_setup.h"
#include "caps.h"
#include "champions_circuit.h"
#include "emerald_champions_headless.h"
#include "emerald_champions_battle_sets.h"
#include "emerald_champions_agent_prep.h"
#include "emerald_champions_agent_battle.h"
#include "coins.h"
#include "event_data.h"
#include "item_use.h"
#include "berry.h"
#include "mega_stone_rewards.h"
#include "constants/quest_states.h"
#include "constants/berry.h"
#include "load_save.h"
#include "event_object_movement.h"
#include "field_effect.h"
#include "field_player_avatar.h"
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
#include "mauville_old_man.h"
#include "daycare.h"
#include "move_relearner.h"
#include "naming_screen.h"
#include "new_game.h"
#include "option_menu.h"
#include "overworld.h"
#include "party_menu.h"
#include "play_time.h"
#include "pokedex.h"
#include "save.h"
#include "pokedex_common.h"
#include "pokemon.h"
#include "pokemon_summary_screen.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "rtc.h"
#include "safari_zone.h"
#include "script.h"
#include "script_pokemon_util.h"
#include "slot_machine.h"
#include "string_util.h"
#include "wild_encounter.h"
#include "title_screen.h"
#include "trainer_card.h"
#include "constants/battle.h"
#include "constants/battle_frontier.h"
#include "constants/game_stat.h"
#include "constants/items.h"
#include "constants/lilycove_lady.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/flags.h"
#include "constants/event_objects.h"
#include "constants/emerald_champions.h"
#include "constants/field_effects.h"
#include "constants/field_specials.h"
#include "constants/script_menu.h"
#include "constants/heal_locations.h"
#include "constants/maps.h"
#include "constants/opponents.h"
#include "constants/decorations.h"
#include "constants/metatile_labels.h"
#include "constants/secret_bases.h"
#include "constants/pokedex.h"
#include "constants/region_map_sections.h"
#include "constants/species.h"

EWRAM_DATA volatile u32 gEcHeadlessFixtureScenario = EC_HEADLESS_SCENARIO_NONE;
EWRAM_DATA volatile u32 gEcHeadlessFixtureActiveScenario = EC_HEADLESS_SCENARIO_NONE;
EWRAM_DATA volatile u32 gEcHeadlessFixtureSetupResult = FALSE;
EWRAM_DATA volatile u32 gEcHeadlessFixtureObservedResult = FALSE;
EWRAM_DATA volatile u32 gEcHeadlessFixtureParam = MOVE_NONE;
EWRAM_DATA volatile u32 gEcHeadlessFixtureTrigger = FALSE;
EWRAM_DATA volatile u32 gEcHeadlessCampaignBattleSerial = 0;
EWRAM_DATA volatile u32 gEcHeadlessCampaignLastBattleType = 0;
EWRAM_DATA volatile u32 gEcHeadlessCampaignLastOpponentA = TRAINER_NONE;
EWRAM_DATA volatile u32 gEcHeadlessCampaignLastOpponentB = TRAINER_NONE;
EWRAM_DATA volatile u32 gEcHeadlessCampaignMapId = 0;
EWRAM_DATA volatile u32 gEcHeadlessCampaignMapGroup = 0;
EWRAM_DATA volatile u32 gEcHeadlessCampaignMapNum = 0;
EWRAM_DATA volatile u32 gEcHeadlessCampaignPlayerX = 0;
EWRAM_DATA volatile u32 gEcHeadlessCampaignPlayerY = 0;
EWRAM_DATA volatile u32 gEcHeadlessCampaignPlayerFacing = DIR_NONE;
EWRAM_DATA volatile u32 gEcHeadlessCampaignControlsLocked = 0;
EWRAM_DATA volatile u32 gEcHeadlessCampaignScriptEnabled = 0;
EWRAM_DATA volatile u32 gEcHeadlessCampaignInBattle = FALSE;
EWRAM_DATA volatile u32 gEcHeadlessCampaignQueryKind = EC_HEADLESS_CAMPAIGN_QUERY_NONE;
EWRAM_DATA volatile u32 gEcHeadlessCampaignQueryId = 0;
EWRAM_DATA volatile u32 gEcHeadlessCampaignQueryValue = 0;
EWRAM_DATA volatile u32 gEcHeadlessCampaignQueryObjectActive = FALSE;
EWRAM_DATA volatile u32 gEcHeadlessCampaignQueryObjectX = 0;
EWRAM_DATA volatile u32 gEcHeadlessCampaignQueryObjectY = 0;
EWRAM_DATA volatile u32 gEcHeadlessCampaignCaptureSerial = 0;
EWRAM_DATA volatile u32 gEcHeadlessCampaignLastCapturedSpecies = SPECIES_NONE;
EWRAM_DATA volatile u32 gEcHeadlessCampaignLastCaptureResult = 0;
EWRAM_DATA volatile u32 gEcHeadlessCampaignCaptureBookkeepingValid = FALSE;
EWRAM_DATA volatile u32 gEcHeadlessCampaignLastResolution = EC_HEADLESS_BATTLE_NATIVE;
EWRAM_DATA volatile u32 gEcHeadlessCampaignForceLoss = FALSE;
EWRAM_DATA volatile u32 gEcHeadlessLeafRewardOwned = FALSE;
EWRAM_DATA volatile u32 gEcHeadlessLeafCompleted = FALSE;
EWRAM_DATA volatile u32 gEcHeadlessLeafTrainerDefeated = FALSE;
EWRAM_DATA volatile u32 gEcHeadlessFixtureFlashLevel = 0;
static EWRAM_DATA u8 sEcHeadlessName[POKEMON_NAME_LENGTH + 1] = {0};
static EWRAM_DATA u16 sEcHeadlessObservedDelay = 0;
static EWRAM_DATA bool8 sEcHeadlessFurfrouMenuOpened = FALSE;
static EWRAM_DATA bool8 sEcHeadlessAutoCaptureInProgress = FALSE;
static const u8 sEcHeadlessPlayerName[] = _("Brendan");
extern void gInitialMainCB2(void);
extern const u8 RivalsHouse_EventScript_ChooseStarterRegion[];
extern const u8 BattleFrontier_BattleTowerLobby_EventScript_CircuitNextMatch[];

bool32 EmeraldChampionsHeadlessBattleAutomationActive(void)
{
    return gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CAMPAIGN_AUTOWIN
        || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CAMPAIGN_NATIVE
        || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CAPTURE_TO_PARTY
        || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CAPTURE_TO_PC
        || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CAPTURE_QUEST_DIANCIE
        || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CAPTURE_QUEST_REGISTEEL
        || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CAPTURE_QUEST_LATIOS
        || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CAPTURE_ORDINARY_FIRST
        || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_ROXANNE_VICTORY
        || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_FIRST_CENTER_ACQUISITION
        || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_LEAF_SCENE
        || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_SECRET_BASE_ESTABLISHED
        || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_NEW_MAUVILLE_BUTTONS
        || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_RUSTBORO_GUIDE_RETRY
        || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_STORY_HANDOFF
        || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_RYDEL_RETRY
        || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_ROUTE110_RETRY
        || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_BOOK_RESEARCH;
}

bool32 EmeraldChampionsHeadlessAutoCaptureActive(void)
{
    return EmeraldChampionsHeadlessBattleAutomationActive()
        && sEcHeadlessAutoCaptureInProgress;
}

enum EmeraldChampionsHeadlessBattleResolution EmeraldChampionsHeadlessGetBattleResolution(void)
{
    // Campaign observation must not choose actions or force battle outcomes.
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CAMPAIGN_NATIVE)
        return EC_HEADLESS_BATTLE_NATIVE;
    // Scoped synthetic reveal fixture: play native turns before explicitly
    // requesting the existing victory shortcut to inspect the reward scene.
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_ROXANNE_VICTORY
     && gEcHeadlessFixtureParam == 1 && !gEcHeadlessFixtureTrigger)
        return EC_HEADLESS_BATTLE_NATIVE;
    if (gBattleTypeFlags & (BATTLE_TYPE_LINK
                          | BATTLE_TYPE_RECORDED
                          | BATTLE_TYPE_RECORDED_LINK
                          | BATTLE_TYPE_CATCH_TUTORIAL
                          | BATTLE_TYPE_POKEDUDE))
        return EC_HEADLESS_BATTLE_NATIVE;
    if (gEcHeadlessCampaignForceLoss)
        return EC_HEADLESS_BATTLE_LOSS;
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_BOOK_RESEARCH)
        return (gEcHeadlessFixtureParam & 0x800) ? EC_HEADLESS_BATTLE_CAPTURE : EC_HEADLESS_BATTLE_WIN;
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_STORY_HANDOFF
     && (gEcHeadlessFixtureParam == 116 || gEcHeadlessFixtureParam == 249 || gEcHeadlessFixtureParam == 250)
     && gEcHeadlessCampaignCaptureSerial == 0
     && gEcHeadlessCampaignBattleSerial == 0)
        return EC_HEADLESS_BATTLE_WIN;
    if (gBattleTypeFlags & (BATTLE_TYPE_LEGENDARY | BATTLE_TYPE_ROAMER))
        return EC_HEADLESS_BATTLE_CAPTURE;
    if ((gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CAMPAIGN_AUTOWIN
      || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CAPTURE_ORDINARY_FIRST)
     && !(gBattleTypeFlags & (BATTLE_TYPE_TRAINER
                           | BATTLE_TYPE_SAFARI
                           | BATTLE_TYPE_GHOST
                           | BATTLE_TYPE_FIRST_BATTLE))
     && !(gBattleTypeFlags & (BATTLE_TYPE_MULTI | BATTLE_TYPE_INGAME_PARTNER))
     && CalculatePlayerPartyCount() < 2)
        return EC_HEADLESS_BATTLE_CAPTURE;
    return EC_HEADLESS_BATTLE_WIN;
}

void EmeraldChampionsHeadlessBeginAutoCapture(void)
{
    sEcHeadlessAutoCaptureInProgress = TRUE;
}

void EmeraldChampionsHeadlessRecordCapture(enum Species species, u32 result)
{
    bool32 delivered = FALSE;

    if (result == MON_GIVEN_TO_PARTY)
    {
        for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        {
            if (GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_SPECIES) == species)
                delivered = TRUE;
        }
    }
    else if (result == MON_GIVEN_TO_PC)
    {
        for (u32 box = 0; box < TOTAL_BOXES_COUNT && !delivered; box++)
        {
            for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            {
                if (GetBoxMonData(&gPokemonStoragePtr->boxes[box][slot], MON_DATA_SPECIES) == species)
                {
                    delivered = TRUE;
                    break;
                }
            }
        }
    }
    gEcHeadlessCampaignLastCapturedSpecies = species;
    gEcHeadlessCampaignLastCaptureResult = result;
    gEcHeadlessCampaignCaptureBookkeepingValid = delivered
        && GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_GET_CAUGHT);
    gEcHeadlessCampaignCaptureSerial++;
    sEcHeadlessAutoCaptureInProgress = FALSE;
}

struct EcHeadlessOverworldFixture
{
    u16 map;
    u16 species;
    s16 playerX;
    s16 playerY;
};

#define EC_HEADLESS_OVERWORLD_FIXTURE(index, map, species, playerX, playerY) \
    [index - 1] = {map, species, playerX, playerY},
static const struct EcHeadlessOverworldFixture sEcHeadlessOverworldFixtures[] =
{
#include "emerald_champions_headless_overworld_fixtures.h"
};
#undef EC_HEADLESS_OVERWORLD_FIXTURE

static const struct { u16 map; s16 x; s16 y; } sEcHeadlessMapSweep[] =
{
#define EC_HEADLESS_MAP_SWEEP(map, x, y) {map, x, y},
#include "emerald_champions_headless_map_sweep.h"
#undef EC_HEADLESS_MAP_SWEEP
};

STATIC_ASSERT(ARRAY_COUNT(sEcHeadlessOverworldFixtures) == 8, HeadlessOverworldFixtureCount);

static u16 GetHeadlessOverworldFixtureGraphicsId(enum Species species)
{
    switch (species)
    {
    case SPECIES_ARTICUNO: return OBJ_EVENT_GFX_INCLEMENT_ARTICUNO;
    case SPECIES_ZAPDOS: return OBJ_EVENT_GFX_INCLEMENT_ZAPDOS;
    case SPECIES_MOLTRES: return OBJ_EVENT_GFX_SPECIES(MOLTRES);
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

static void PrepareHeadlessFieldMoveParty(enum Species species, u16 badgeFlag, u16 hmFlag)
{
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], species, 14, 0, OTID_STRUCT_PLAYER_ID);
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

// Synthetic capacity boundary only: unique valid keys, with owed gifts absent.
static void FillHeadlessKeyPocket(void)
{
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_DOWSING_MACHINE)];
    u32 slot = 0;

    for (u32 item = 1; item < ITEMS_COUNT && slot < pocket->capacity; item++)
    {
        if (GetItemPocket(item) == GetItemPocket(ITEM_DOWSING_MACHINE)
         && item != ITEM_DOWSING_MACHINE && item != ITEM_MACH_BIKE && item != ITEM_ACRO_BIKE
         && item != ITEM_GO_GOGGLES && item != ITEM_DEVON_SCOPE && item != ITEM_LINKING_CORD
         && item != ITEM_MAGMA_EMBLEM)
            BagPocket_SetSlotItemIdAndCount(pocket, slot++, item, 1);
    }
    fatal_assertf(slot == pocket->capacity, "Headless key pocket could not be filled");
}

static void LoadHeadlessMap(u16 map, s16 x, s16 y)
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

// Synthetic prerequisite states only. NPC interaction, map travel, reports,
// inventory transactions and battle exits still execute the real game scripts.
// Low byte: scene; 0x100: full reward pocket; 0x200: missing prerequisite;
// 0x400: already completed; 0x800: capture instead of the default battle win.
static void PrepareBookResearchScene(void)
{
    u32 scene = gEcHeadlessFixtureParam & 0xFF;
    bool32 missing = gEcHeadlessFixtureParam & 0x200;
    bool32 completed = gEcHeadlessFixtureParam & 0x400;
    u32 badges = scene <= 4 ? 5 : scene == 6 ? 6 : 8;
    if (scene == 0 || scene == 4)
        badges = 4;
    ZeroPlayerPartyMons();
    CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ZIGZAGOON, 80, OTID_STRUCT_PLAYER_ID);
    if (!missing)
        CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_CASTFORM_NORMAL, 50, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    ClearBag();
    SetMoney(&gSaveBlock1Ptr->money, 6000);
    for (u32 badge = 0; badge < badges; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    FlagSet(FLAG_SYS_POKEMON_GET);
    FlagSet(FLAG_SYS_POKEDEX_GET);
    FlagSet(FLAG_RECEIVED_DEXNAV);
    FlagSet(FLAG_HIDE_ROUTE_119_TEAM_AQUA);
    VarSet(VAR_WEATHER_INSTITUTE_STATE, 1);
    VarSet(VAR_REPEL_STEP_COUNT, 250);
    if (gEcHeadlessFixtureParam & 0x100)
    {
        if (scene == 6)
            FillHeadlessKeyPocket();
        else
        {
            struct BagPocket *pocket = &gBagPockets[GetItemPocket(scene == 14 || scene == 15 || scene == 17 || scene == 26 ? ITEM_VENUSAURITE : ITEM_DEEP_SEA_TOOTH)];
            for (u32 slot = 0; slot < pocket->capacity; slot++)
                BagPocket_SetSlotItemIdAndCount(pocket, slot,
                    pocket->id == POCKET_MEGA_STONES ? ITEM_ABOMASITE : ITEM_FIRE_STONE, MAX_BAG_ITEM_CAPACITY);
        }
    }
    switch (scene)
    {
    case 0:
        FlagSet(FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY);
        VarSet(VAR_CHANSEY_NURSE_STATE, CHANSEY_NURSE_RETIRED);
        FlagClear(FLAG_EC_CAUGHT_MOLTRES);
        LoadHeadlessMap(MAP_EMBER_PATH, 21, 15);
        break;
    case 1:
        if (completed)
            FlagSet(FLAG_EC_REPORT_C30_COMPLETE);
        LoadHeadlessMap(MAP_ROUTE111_RUINS_EXTERIOR, 15, 13);
        break;
    case 2:
        if (completed)
            FlagSet(FLAG_EC_REPORT_C30_COMPLETE);
        LoadHeadlessMap(MAP_ROUTE119_WEATHER_INSTITUTE_2F, 5, 6);
        break;
    case 3:
        FlagSet(FLAG_BADGE07_GET);
        FlagClear(FLAG_HIDE_SLATEPORT_CITY_HARBOR_CAPTAIN_STERN);
        VarSet(VAR_SLATEPORT_HARBOR_STATE, 2);
        if (completed)
        {
            FlagSet(FLAG_EC_REPORT_C44_COMPLETE);
            FlagSet(FLAG_EXCHANGED_SCANNER);
        }
        else if (!missing)
            AddBagItem(ITEM_SCANNER, 1);
        LoadHeadlessMap(MAP_SLATEPORT_CITY_HARBOR, 6, 14);
        break;
    case 4:
        VarSet(VAR_CHANSEY_NURSE_STATE, CHANSEY_NURSE_RETIRED);
        FlagSet(FLAG_EC_RESOLVED_MOLTRES);
        if (!missing)
            FlagSet(FLAG_EC_SURVEYED_DESERT_DEPTHS);
        if (completed)
            FlagSet(FLAG_EC_REPORT_C26_COMPLETE);
        LoadHeadlessMap(MAP_SANDSTREWN_RUINS, 9, 131);
        break;
    case 5:
        FlagSet(FLAG_EC_REPORT_C26_COMPLETE);
        FlagSet(FLAG_EC_REPORT_C30_COMPLETE);
        if (!missing)
            FlagSet(FLAG_EC_REPORT_C44_COMPLETE);
        if (completed)
            FlagSet(FLAG_REGI_DOORS_OPENED);
        LoadHeadlessMap(MAP_SEALED_CHAMBER_INNER_ROOM, 10, 5);
        break;
    case 6:
        FlagSet(FLAG_EC_REPORT_C30_COMPLETE);
        FlagSet(FLAG_RECEIVED_DEVON_SCOPE);
        if (!missing)
            SetTrainerFlag(TRAINER_BRENDAN_LILYCOVE_TREECKO);
        FlagClear(FLAG_HIDE_LILYCOVE_HARBOR_FERRY_ATTENDANT);
        if (completed)
            FlagSet(FLAG_EC_RESOLVED_MEW);
        LoadHeadlessMap(MAP_LILYCOVE_CITY_HARBOR, 8, 11);
        break;
    case 7:
        FlagSet(FLAG_EC_REPORT_C42_COMPLETE);
        FlagSet(FLAG_RECEIVED_HM_DIVE);
        FlagClear(FLAG_HIDE_MOSSDEEP_CITY_STEVENS_HOUSE_STEVEN);
        VarSet(VAR_STEVENS_HOUSE_STATE, 2);
        FlagSet(FLAG_EC_SURVEYED_ORIGIN_CHAMBER);
        if (!missing)
            FlagSet(FLAG_EC_SURVEYED_METEOR_CHAMBER);
        LoadHeadlessMap(MAP_MOSSDEEP_CITY_STEVENS_HOUSE, 6, 6);
        break;
    case 8:
        FlagSet(FLAG_RECEIVED_PIDGEOTITE_FROM_DEVON);
        FlagSet(FLAG_DELIVERED_STEVEN_LETTER);
        FlagSet(FLAG_SYS_POKENAV_GET);
        VarSet(VAR_DEVON_CORP_3F_STATE, 1);
        if (!missing)
            FlagSet(FLAG_EC_STEVEN_RESEARCH_CONCLUSION);
        if (completed)
            FlagSet(FLAG_EC_REPORT_C48_COMPLETE);
        LoadHeadlessMap(MAP_RUSTBORO_CITY_DEVON_CORP_3F, 17, 6);
        break;
    case 9:
        LoadHeadlessMap(MAP_DEWFORD_TOWN, 8, 18);
        break;
    case 10:
        AddBagItem(ITEM_MAGMA_STONE, 1);
        FlagSet(FLAG_EC_CAUGHT_HEATRAN);
        LoadHeadlessMap(MAP_SCORCHED_SLAB_HEATRANS_ROOM, 10, 15);
        break;
    case 11:
        LoadHeadlessMap(MAP_ROUTE104, 31, 25);
        break;
    case 12:
        LoadHeadlessMap(MAP_ROUTE104, 27, 16);
        break;
    case 13:
        AddBagItem(ITEM_SOOT_SACK, 1);
        VarSet(VAR_EC_SOOT_PROGRESS, 100 | (completed ? EC_SOOT_CORD_RECEIVED : 0));
        if (!missing)
            AddPCItem(ITEM_LINKING_CORD, 1);
        LoadHeadlessMap(MAP_ROUTE113_GLASS_WORKSHOP, 2, 4);
        break;
    case 14:
        FlagClear(FLAG_ITEM_ROUTE_120_GENGARITE);
        if (!missing)
            AddPCItem(ITEM_GENGARITE, 1);
        LoadHeadlessMap(MAP_ROUTE120, 20, 56);
        break;
    case 15:
        VarSet(VAR_STARTER_GEN, 1);
        VarSet(VAR_STARTER_MON, 0);
        VarSet(VAR_EC_SECOND_STARTER, 0);
        FlagSet(FLAG_DELIVERED_STEVEN_LETTER);
        FlagClear(FLAG_HIDE_GRANITE_CAVE_STEVEN);
        AddPCItem(ITEM_MEGA_RING, 1);
        if (!missing)
            AddPCItem(ITEM_VENUSAURITE, 1);
        LoadHeadlessMap(MAP_GRANITE_CAVE_STEVENS_ROOM, 7, 9);
        break;
    case 16:
        gSaveBlock2Ptr->frontier.battlePoints = 100;
        if (!missing)
            AddPCItem(ITEM_LINKING_CORD, 1);
        LoadHeadlessMap(MAP_BATTLE_FRONTIER_EXCHANGE_SERVICE_CORNER, 12, 4);
        break;
    case 17:
    {
        enum Item item = ITEM_VENUSAURITE;
        SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &item);
        LoadHeadlessMap(MAP_RUSTBORO_CITY_HOUSE1, 5, 4);
        break;
    }
    case 18:
        AddBagItem(ITEM_POKE_BALL, 2);
        AddBagItem(ITEM_POKE_VIAL, 1);
        AddBagItem(ITEM_LEVELER, 1);
        AddBagItem(ITEM_REGENERATOR, 1);
        AddBagItem(ITEM_REPEL_SPRAY, 1);
        AddBagItem(ITEM_FLIGHT_BEACON, 1);
        VarSet(VAR_POKE_VIAL_MAX_CHARGES, POKE_VIAL_CAPACITY_BASE);
        LoadHeadlessMap(MAP_OLDALE_TOWN_POKEMON_CENTER_1F, 8, 4);
        break;
    case 19:
        AddBagItem(ITEM_SHOAL_SALT, 4);
        AddBagItem(ITEM_SHOAL_SHELL, 4);
        if (!missing)
            AddPCItem(ITEM_GLALITITE, 1);
        FlagClear(FLAG_ITEM_ABANDONED_SHIP_ROOMS_B1F_GLALITITE);
        LoadHeadlessMap(MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM, 17, 15);
        break;
    case 20:
        gSaveBlock2Ptr->frontier.battlePoints = 100;
        LoadHeadlessMap(MAP_BATTLE_FRONTIER_EXCHANGE_SERVICE_CORNER, 9, 7);
        break;
    case 21: // Origin actor; missing means the chapter's Rain Badge is missing.
    case 22: // Meteor actor; the two observations are intentionally independent.
    case 23: // Origin entrance, with an unavailable actor and no observation.
    case 24: // Meteor entrance, with an unavailable actor and no observation.
        FlagClear(FLAG_EC_SURVEYED_ORIGIN_CHAMBER);
        FlagClear(FLAG_EC_SURVEYED_METEOR_CHAMBER);
        FlagClear(FLAG_EC_CAUGHT_DIANCIE);
        FlagClear(FLAG_EC_CAUGHT_JIRACHI);
        if (missing)
            FlagClear(FLAG_BADGE08_GET);
        if (gEcHeadlessFixtureParam & 0x1000)
            FlagSet(scene & 1 ? FLAG_EC_SURVEYED_METEOR_CHAMBER : FLAG_EC_SURVEYED_ORIGIN_CHAMBER);
        if (gEcHeadlessFixtureParam & 0x2000)
            FlagSet(FLAG_EC_STEVEN_RESEARCH_CONCLUSION);
        if (gEcHeadlessFixtureParam & 0x4000)
            FlagSet(FLAG_EC_REPORT_C48_COMPLETE);
        if (gEcHeadlessFixtureParam & 0x8000)
            FlagSet(FLAG_SYS_GAME_CLEAR);
        if (completed)
            FlagSet(scene & 1 ? FLAG_EC_SURVEYED_ORIGIN_CHAMBER : FLAG_EC_SURVEYED_METEOR_CHAMBER);
        if (gEcHeadlessFixtureParam & 0x100)
        {
            // Real full-storage boundary: no party/PC slot can receive a catch.
            for (u32 slot = 0; slot < PARTY_SIZE; slot++)
                CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][slot], SPECIES_ZIGZAGOON, 80, OTID_STRUCT_PLAYER_ID);
            for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
                for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
                    CreateBoxMon(&gPokemonStoragePtr->boxes[box][slot], SPECIES_ZIGZAGOON, 14, 0, OTID_STRUCT_PLAYER_ID);
            CalculatePlayerPartyCount();
        }
        AddBagItem(ITEM_POKE_BALL, 1);
        if (gEcHeadlessFixtureParam & 0x10000)
        {
            // A real Smoke Ball makes native Run deterministic for exit QA.
            enum Item item = ITEM_SMOKE_BALL;
            SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &item);
        }
        SetLastHealLocationWarp(HEAL_LOCATION_SOOTOPOLIS_CITY);
        if (scene == 23)
            FlagSet(FLAG_EC_CAUGHT_DIANCIE);
        if (scene == 24)
            FlagSet(FLAG_EC_CAUGHT_JIRACHI);
        if (scene & 1)
            LoadHeadlessMap(MAP_CAVE_OF_ORIGIN_DIANCIES_ROOM, 9, scene == 23 ? 8 : 10);
        else
            LoadHeadlessMap(MAP_METEOR_FALLS_JIRACHIS_ROOM, 7, scene == 24 ? 8 : 7);
        break;
    case 25: // The obsolete pickup no longer blocks the required Jirachi door.
        if (gEcHeadlessFixtureParam & 0x100)
        {
            struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_BEAST_BALL)];
            for (u32 slot = 0; slot < pocket->capacity; slot++)
                BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_POKE_BALL, MAX_BAG_ITEM_CAPACITY);
        }
        LoadHeadlessMap(MAP_METEOR_FALLS_B1F_2R, 5, 4);
        break;
    case 26: // Diancite reuses Shoal's original visible TM70 pickup corner.
        if (missing)
            AddPCItem(ITEM_DIANCITE, 1);
        if (completed)
            FlagSet(FLAG_EC_MEGA_REWARD_DIANCITE);
        LoadHeadlessMap(MAP_SHOAL_CAVE_LOW_TIDE_STAIRS_ROOM, 12, 12);
        break;
    case 28: // Finite travel-paper delivery without blocking Center healing.
    {
        const enum Item tools[] = {ITEM_POKE_VIAL, ITEM_LEVELER, ITEM_REGENERATOR, ITEM_REPEL_SPRAY, ITEM_FLIGHT_BEACON};
        ClearBag();
        if (!missing)
            for (u32 i = 0; i < ARRAY_COUNT(tools); i++)
                AddBagItem(tools[i], 1);
        VarSet(VAR_POKE_VIAL_MAX_CHARGES, POKE_VIAL_CAPACITY_BASE);
        FlagSet(FLAG_EC_EARNED_SS_TICKET);
        FlagSet(FLAG_EC_EARNED_EON_TICKET);
        FlagSet(FLAG_EC_EARNED_OLD_SEA_MAP);
        FlagSet(FLAG_EC_EARNED_AURORA_TICKET);
        FlagSet(FLAG_ENABLE_SHIP_NAVEL_ROCK);
        if (gEcHeadlessFixtureParam & 0x100)
        {
            struct BagPocket *pocket = &gBagPockets[POCKET_KEY_ITEMS];
            for (u32 slot = missing ? 0 : ARRAY_COUNT(tools); slot < pocket->capacity; slot++)
                BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_BASEMENT_KEY, 1);
        }
        // An observable healing outcome, not merely the offer text.
        u16 hp = 1;
        SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP, &hp);
        LoadHeadlessMap(MAP_OLDALE_TOWN_POKEMON_CENTER_1F, 8, 4);
        break;
    }
    case 27:
        FlagSet(FLAG_SYS_USE_FLASH);
        LoadHeadlessMap(MAP_GRANITE_CAVE_B2F, 12, 11);
        break;
    }
    gEcHeadlessFixtureSetupResult = TRUE;
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

static void PrepareMoveReplacement(void)
{
    CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_MEW, 30, OTID_STRUCT_PLAYER_ID);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_SURF, 0);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_PSYCHIC, 1);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_ICE_BEAM, 2);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_PROTECT, 3);
    CalculatePlayerPartyCount();
    gMoveRelearnerState = MOVE_RELEARNER_ALL_MOVES;
    gRelearnMode = RELEARN_MODE_SCRIPT;
    ShowSelectMovePokemonSummaryScreen(
        gParties[B_TRAINER_PLAYER],
        0,
        gInitialMainCB2,
        MOVE_THUNDERBOLT
    );
    gEcHeadlessFixtureSetupResult = TRUE;
}

// Params 20-22 seed a single species that lives only in one restored Inclement area, so
// the Area page proves the region-map marker/glow for the expansion map group.
#define EC_HEADLESS_POKEDEX_AREA_ASHEN_WOODS       20
#define EC_HEADLESS_POKEDEX_AREA_DEWFORD_MEADOW    21
#define EC_HEADLESS_POKEDEX_AREA_VERDANTURF_MEADOW 22

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
    static const enum Species areaSpecies[] =
    {
        [EC_HEADLESS_POKEDEX_AREA_ASHEN_WOODS - 20] = SPECIES_BUZZWOLE,
        [EC_HEADLESS_POKEDEX_AREA_DEWFORD_MEADOW - 20] = SPECIES_PHEROMOSA,
        [EC_HEADLESS_POKEDEX_AREA_VERDANTURF_MEADOW - 20] = SPECIES_ALCREMIE,
    };
    const enum Species *list = species;
    u32 count = ARRAY_COUNT(species);

    if (gEcHeadlessFixtureParam >= 20 && gEcHeadlessFixtureParam < 20 + ARRAY_COUNT(areaSpecies))
    {
        list = &areaSpecies[gEcHeadlessFixtureParam - 20];
        count = 1;
    }

    FlagSet(FLAG_SYS_POKEDEX_GET);
    EnableNationalPokedex();
    for (u32 i = 0; i < count; i++)
    {
        enum NationalDexOrder dex = SpeciesToNationalPokedexNum(list[i]);

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
    enum Species species = (gEcHeadlessFixtureParam >> 16) != 0
        ? SanitizeSpeciesId(gEcHeadlessFixtureParam >> 16) : SPECIES_GARCHOMP;
    if (gSpeciesInfo[species].isMegaEvolution && gSpeciesInfo[species].formChangeTable != NULL)
    {
        const struct FormChange *form = gSpeciesInfo[species].formChangeTable;
        for (; form->method != FORM_CHANGE_TERMINATOR; form++)
            if (form->method == FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM && form->targetSpecies == species)
                item = form->param1;
    }

    CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], species, 67, OTID_STRUCT_PLAYER_ID);
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
    // Move details share the Ball shortcut, so this fixture needs an empty Bag.
    if (gEcHeadlessFixtureActiveScenario != EC_HEADLESS_SCENARIO_MOVE_DETAILS)
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

static enum Species GetHeadlessCaptureSpecies(void)
{
    enum Species species = gEcHeadlessFixtureParam >> 16;
    return species > SPECIES_NONE && species < NUM_SPECIES ? species : SPECIES_DIANCIE;
}

static bool32 HeadlessCapturedSignIsComplete(void)
{
    enum LegendarySignId signId = GetLegendarySignIdBySpecies(GetHeadlessCaptureSpecies());
    return signId == LEGENDARY_SIGN_COUNT
        || (IsLegendarySignUnlocked(signId) && IsLegendarySignCaught(signId));
}

static void PrepareHeadlessCaptureBattle(bool32 fullParty)
{
    SetWarpDestination(MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), WARP_ID_NONE, 8, 12);
    WarpIntoMap();
    InitMap();
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    for (u32 i = 0; i < (fullParty ? PARTY_SIZE : 1); i++)
    {
        CreateHealthyHeadlessMon(
            &gParties[B_TRAINER_PLAYER][i], SPECIES_GEODUDE, 30,
            OTID_STRUCT_PLAYER_ID
        );
    }
    CalculatePlayerPartyCount();
    CreateWildMon(GetHeadlessCaptureSpecies(), 50);
    gBattleTypeFlags = BATTLE_TYPE_LEGENDARY;
    gMain.savedCallback = gInitialMainCB2;
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

// Low bits preserve the original ready/active scenarios; high bits select a form.
static enum Species GetHeadlessMegaTarget(void)
{
    return (gEcHeadlessFixtureParam >> 16) != 0
        ? gEcHeadlessFixtureParam >> 16 : SPECIES_CHARIZARD_MEGA_X;
}

static void PrepareHeadlessMegaBattle(void)
{
    u16 stone = ITEM_CHARIZARDITE_X;
    enum Species base = SPECIES_CHARIZARD;
    enum Move requiredMove = MOVE_AIR_SLASH;
    bool32 found = FALSE;

    for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES && !found; species++)
    {
        const struct FormChange *form = gSpeciesInfo[species].formChangeTable;
        if (form == NULL || gSpeciesInfo[species].isMegaEvolution)
            continue;
        for (; form->method != FORM_CHANGE_TERMINATOR; form++)
        {
            if (form->targetSpecies != GetHeadlessMegaTarget()
             || (form->method != FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM
              && form->method != FORM_CHANGE_BATTLE_MEGA_EVOLUTION_MOVE))
                continue;
            base = species;
            stone = form->method == FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM ? form->param1 : ITEM_NONE;
            if (form->method == FORM_CHANGE_BATTLE_MEGA_EVOLUTION_MOVE)
                requiredMove = form->param1;
            found = TRUE;
            break;
        }
    }
    if (!found)
    {
        SetMainCallback2(gInitialMainCB2);
        return;
    }

    SetWarpDestination(MAP_GROUP(MAP_ROUTE101), MAP_NUM(MAP_ROUTE101), WARP_ID_NONE, 8, 12);
    WarpIntoMap();
    InitMap();
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], base, 50, OTID_STRUCT_PLAYER_ID);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_PROTECT, 0);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_DRAGON_CLAW, 1);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_FLAMETHROWER, 2);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], requiredMove, 3);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &stone);
    CreateHealthyHeadlessMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_VENUSAUR, 50, OTID_STRUCT_RANDOM_NO_SHINY);
    if (gEcHeadlessFixtureParam >> 16)
    {
        SetMonMoveSlot(&gParties[B_TRAINER_OPPONENT_A][0], MOVE_SPLASH, 0);
        for (u32 slot = 1; slot < MAX_MON_MOVES; slot++)
            SetMonMoveSlot(&gParties[B_TRAINER_OPPONENT_A][0], MOVE_NONE, slot);
    }
    CalculatePlayerPartyCount();
    ClearBag();
    // Level-50 UI subjects need earned obedience, including the no-Ring case.
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    if (!(gEcHeadlessFixtureParam & 2))
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

static void PrepareHeadlessGoldTrainerCard(void)
{
    SetGameStat(GAME_STAT_ENTERED_HOF, 1);
    EnableNationalPokedex();
    for (enum NationalDexOrder dex = 1; dex < NATIONAL_DEX_COUNT; dex++)
        GetSetPokedexFlag(dex, FLAG_SET_CAUGHT);
    for (u32 i = 0; i < NUM_FRONTIER_FACILITIES; i++)
    {
        FlagSet(FLAG_SYS_TOWER_SILVER + 2 * i);
        FlagSet(FLAG_SYS_TOWER_GOLD + 2 * i);
    }
    ShowPlayerTrainerCard(gInitialMainCB2);
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
    switch (gEcHeadlessFixtureParam & 0xFFFF)
    {
    case EC_HEADLESS_SUMMARY_INFO:
        return IsPokemonSummaryHeadlessOnPage(PSS_PAGE_INFO, FALSE);
    case EC_HEADLESS_SUMMARY_SKILLS:
        return IsPokemonSummaryHeadlessOnPage(PSS_PAGE_SKILLS, FALSE);
    case EC_HEADLESS_SUMMARY_BATTLE_MOVES:
        return IsPokemonSummaryHeadlessOnPage(PSS_PAGE_BATTLE_MOVES, FALSE);
    case EC_HEADLESS_SUMMARY_MOVE_DETAILS:
        return IsPokemonSummaryHeadlessOnPage(PSS_PAGE_BATTLE_MOVES, TRUE);
    case EC_HEADLESS_SUMMARY_PARTY_ROUNDTRIP:
        return IsPartyMenuHeadlessAwaitingSelection();
    }
    return FALSE;
}

void EmeraldChampionsHeadlessObserve(void)
{
    EmeraldChampionsStudioPoll();
    EmeraldChampionsAgentPrepPoll();
    EmeraldChampionsAgentBattlePoll();
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_STORY_HANDOFF
     && (gEcHeadlessFixtureParam == 275 || gEcHeadlessFixtureParam == 276)
     && gMain.callback2 == CB2_Overworld && !gEcHeadlessFixtureObservedResult)
    {
        FlagSet(FLAG_TEMP_1); // Synthetic prior tour, after map temp-flag reset.
        gEcHeadlessFixtureObservedResult = 1;
    }
    if ((gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_RYDEL_RETRY
      || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_ROUTE110_RETRY)
     && gEcHeadlessFixtureTrigger && gMain.callback2 == CB2_Overworld
     && !ArePlayerFieldControlsLocked() && !ScriptContext_IsEnabled())
    {
        // Diagnostic-only recovery actions; never used by the earned campaign.
        if (gEcHeadlessFixtureTrigger == 2)
        {
            struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_DOWSING_MACHINE)];
            BagPocket_SetSlotItemIdAndCount(pocket, 0, ITEM_NONE, 0);
        }
        else if (gEcHeadlessFixtureTrigger == 3)
            HealPlayerParty();
        gEcHeadlessFixtureTrigger = 0;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_STORY_HANDOFF
        && gEcHeadlessFixtureTrigger && gMain.callback2 == CB2_Overworld
        && !ArePlayerFieldControlsLocked() && !ScriptContext_IsEnabled())
    {
        // Fixture storage edits only; reload exercises native persistent state.
        if (gEcHeadlessFixtureTrigger == 2 || gEcHeadlessFixtureTrigger == 3)
        {
            u32 slot = gEcHeadlessFixtureTrigger - 2;
            gSaveBlock1Ptr->pcItems[slot] = (struct ItemSlot){ITEM_NONE, 0};
        }
        gEcHeadlessFixtureTrigger = 0;
        if (gEcHeadlessFixtureParam == 115 || gEcHeadlessFixtureParam == 116)
            LoadHeadlessMap(MAP_SCORCHED_SLAB_HEATRANS_ROOM, 10, 15);
        else if (gEcHeadlessFixtureParam == 151)
            LoadHeadlessMap(MAP_MT_PYRE_SUMMIT, 23, 6);
        else if (gEcHeadlessFixtureParam == 154)
            LoadHeadlessMap(MAP_MT_PYRE_SUMMIT, 24, 5);
        else if (gEcHeadlessFixtureParam == 245 || gEcHeadlessFixtureParam == 249)
            LoadHeadlessMap(MAP_CAVE_OF_ORIGIN_DIANCIES_ROOM, 9, 10);
        else if (gEcHeadlessFixtureParam == 246 || gEcHeadlessFixtureParam == 250)
            LoadHeadlessMap(MAP_SKY_PILLAR_TOP, 14, 7);
        else if (gEcHeadlessFixtureParam == 175)
            LoadHeadlessMap(MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM, 8, 9);
        else if (gEcHeadlessFixtureParam == 106)
            LoadHeadlessMap(MAP_ROUTE113_GLASS_WORKSHOP, 2, 4);
        else if (gEcHeadlessFixtureParam == 94 || gEcHeadlessFixtureParam == 95)
            LoadHeadlessMap(MAP_ROUTE120, 14, 15);
        else if (gEcHeadlessFixtureParam == 91)
            VarSet(VAR_ABNORMAL_WEATHER_STEP_COUNTER, 999);
        else if (gEcHeadlessFixtureParam == 43)
            LoadHeadlessMap(MAP_MAUVILLE_CITY_GYM, gSaveBlock1Ptr->pos.x, gSaveBlock1Ptr->pos.y);
        else if (gEcHeadlessFixtureParam == 68)
            LoadHeadlessMap(MAP_LAVARIDGE_TOWN, 11, 16);
        else if (gEcHeadlessFixtureParam == 89 || gEcHeadlessFixtureParam == 90)
            LoadHeadlessMap(MAP_ROUTE119, 25, 33);
        else if (gEcHeadlessFixtureParam == 77)
        {
            FlagClear(FLAG_HIDE_MAUVILLE_CITY_WATTSON);
            LoadHeadlessMap(MAP_MAUVILLE_CITY, 29, 10);
        }
        else if (gEcHeadlessFixtureParam >= 4 && gEcHeadlessFixtureParam <= 9)
            LoadHeadlessMap(MAP_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB, 6, 5);
        else
            LoadHeadlessMap(MAP_GRANITE_CAVE_STEVENS_ROOM, 7, 9);
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_RUSTBORO_GUIDE_RETRY
        && gEcHeadlessFixtureTrigger == 2 && gMain.callback2 == CB2_Overworld
        && !ArePlayerFieldControlsLocked() && !ScriptContext_IsEnabled())
    {
        // Fixture-only room-making; the guide state and position stay untouched.
        ClearBag();
        gEcHeadlessFixtureTrigger = 0;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_BOOK_RESEARCH
     && gEcHeadlessFixtureTrigger == 2 && gMain.callback2 == CB2_Overworld
     && !ArePlayerFieldControlsLocked() && !ScriptContext_IsEnabled())
    {
        ClearBag();
        gEcHeadlessFixtureTrigger = 0;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_LEAF_SCENE)
    {
        gEcHeadlessFixtureFlashLevel = GetFlashLevel();
        gEcHeadlessLeafRewardOwned = CheckBagHasItem(ITEM_BOTTLE_CAP, 30);
        gEcHeadlessLeafCompleted = VarGet(VAR_LEAF_STATE) != 0;
        gEcHeadlessLeafTrainerDefeated = HasTrainerBeenFought(TRAINER_LEAF_ALTERING_CAVE);
        // Host setup requests only: reload preserves persistent encounter state.
        if (gEcHeadlessFixtureTrigger && gMain.callback2 == CB2_Overworld
            && !ArePlayerFieldControlsLocked() && !ScriptContext_IsEnabled())
        {
            if (gEcHeadlessFixtureTrigger == 2)
                ClearBag();
            gEcHeadlessFixtureTrigger = 0;
            LoadHeadlessMap(MAP_ALTERING_CAVE_B1F, 21, 20);
        }
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_SECRET_BASE_ESTABLISHED)
    {
        gEcHeadlessFixtureObservedResult = gMain.callback2 == CB2_Overworld
            && gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_SECRET_BASE_RED_CAVE1)
            && gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_SECRET_BASE_RED_CAVE1)
            && gSaveBlock1Ptr->pos.x == 6 && gSaveBlock1Ptr->pos.y == 5
            && VarGet(VAR_CURRENT_SECRET_BASE) == 0
            && VarGet(VAR_INIT_SECRET_BASE) == 1
            && VarGet(VAR_SECRET_BASE_INITIALIZED) == 1
            && FlagGet(FLAG_HIDE_SECRET_BASE_TRAINER)
            && MapGridGetMetatileIdAt(5 + MAP_OFFSET, 5 + MAP_OFFSET) == METATILE_SecretBase_SmallChair
            && !ArePlayerFieldControlsLocked() && !ScriptContext_IsEnabled();
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_NEW_MAUVILLE_BUTTONS)
    {
        bool32 blue = VarGet(VAR_TEMP_1) == 1 && VarGet(VAR_TEMP_2) == 0;
        bool32 green = VarGet(VAR_TEMP_1) == 0 && VarGet(VAR_TEMP_2) == 1;
        gEcHeadlessFixtureObservedResult = 0;
        if (gMain.callback2 == CB2_Overworld
            && gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_NEW_MAUVILLE_INSIDE)
            && gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_NEW_MAUVILLE_INSIDE)
            && !ArePlayerFieldControlsLocked() && !ScriptContext_IsEnabled())
        {
            // Sample a passage tile from each color, including collision.
            if (blue
                && MapGridGetMetatileIdAt(10 + MAP_OFFSET, 18 + MAP_OFFSET) == METATILE_BikeShop_Floor_Shadow_Top
                && MapGridGetCollisionAt(10 + MAP_OFFSET, 18 + MAP_OFFSET) == 0
                && MapGridGetMetatileIdAt(21 + MAP_OFFSET, 4 + MAP_OFFSET) == METATILE_BikeShop_Barrier_Green_BottomMid
                && MapGridGetCollisionAt(21 + MAP_OFFSET, 4 + MAP_OFFSET) != 0)
                gEcHeadlessFixtureObservedResult = 1;
            if (green
                && MapGridGetMetatileIdAt(10 + MAP_OFFSET, 18 + MAP_OFFSET) == METATILE_BikeShop_Barrier_Blue_BottomMid
                && MapGridGetCollisionAt(10 + MAP_OFFSET, 18 + MAP_OFFSET) != 0
                && MapGridGetMetatileIdAt(21 + MAP_OFFSET, 4 + MAP_OFFSET) == METATILE_BikeShop_Floor_Shadow_Top
                && MapGridGetCollisionAt(21 + MAP_OFFSET, 4 + MAP_OFFSET) == 0)
                gEcHeadlessFixtureObservedResult = 2;
        }
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_MAP_SWEEP)
    {
        u32 index = gEcHeadlessFixtureParam;
        gEcHeadlessFixtureObservedResult = index < ARRAY_COUNT(sEcHeadlessMapSweep)
            && gEcHeadlessFixtureSetupResult
            && gMain.callback2 == CB2_Overworld
            && gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(sEcHeadlessMapSweep[index].map)
            && gSaveBlock1Ptr->location.mapNum == MAP_NUM(sEcHeadlessMapSweep[index].map)
            && gSaveBlock1Ptr->pos.x == sEcHeadlessMapSweep[index].x
            && gSaveBlock1Ptr->pos.y == sEcHeadlessMapSweep[index].y;
        return;
    }
    if (EmeraldChampionsHeadlessBattleAutomationActive()
     || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_BERRY_ECONOMY
     || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_ECONOMY_SHOPS
     || gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_C14_SONG)
    {
        if (!gMain.inBattle)
            sEcHeadlessAutoCaptureInProgress = FALSE;
        gEcHeadlessFixtureSetupResult = TRUE;
        gEcHeadlessCampaignInBattle = gMain.inBattle;
        if (gEcHeadlessCampaignQueryKind == EC_HEADLESS_CAMPAIGN_QUERY_FLAG)
            gEcHeadlessCampaignQueryValue = FlagGet(gEcHeadlessCampaignQueryId);
        else if (gEcHeadlessCampaignQueryKind == EC_HEADLESS_CAMPAIGN_QUERY_VAR)
            gEcHeadlessCampaignQueryValue = VarGet(gEcHeadlessCampaignQueryId);
        else if (gEcHeadlessCampaignQueryKind == EC_HEADLESS_CAMPAIGN_QUERY_SELL_PRICE)
            gEcHeadlessCampaignQueryValue = GetItemSellPrice(gEcHeadlessCampaignQueryId);
        else if (gEcHeadlessCampaignQueryKind == EC_HEADLESS_CAMPAIGN_QUERY_ITEM_PRICE)
            gEcHeadlessCampaignQueryValue = GetItemPrice(gEcHeadlessCampaignQueryId);
        else if (gEcHeadlessCampaignQueryKind == EC_HEADLESS_CAMPAIGN_QUERY_MONEY)
            gEcHeadlessCampaignQueryValue = GetMoney(&gSaveBlock1Ptr->money);
        else if (gEcHeadlessCampaignQueryKind == EC_HEADLESS_CAMPAIGN_QUERY_BP)
            gEcHeadlessCampaignQueryValue = gSaveBlock2Ptr->frontier.battlePoints;
        else if (gEcHeadlessCampaignQueryKind == EC_HEADLESS_CAMPAIGN_QUERY_PARTY_SPECIES)
            gEcHeadlessCampaignQueryValue = gEcHeadlessCampaignQueryId < PARTY_SIZE
                ? GetMonData(&gParties[B_TRAINER_PLAYER][gEcHeadlessCampaignQueryId], MON_DATA_SPECIES) : SPECIES_NONE;
        else if (gEcHeadlessCampaignQueryKind == EC_HEADLESS_CAMPAIGN_QUERY_PLAYER_LEVEL_CAP)
            gEcHeadlessCampaignQueryValue = GetPlayerLevelCapForSpecies(gEcHeadlessCampaignQueryId);
        else if (gEcHeadlessCampaignQueryKind == EC_HEADLESS_CAMPAIGN_QUERY_PP_BONUSES)
            gEcHeadlessCampaignQueryValue = GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP_BONUSES);
        else if (gEcHeadlessCampaignQueryKind == EC_HEADLESS_CAMPAIGN_QUERY_LEGENDARY_ELIGIBLE)
            gEcHeadlessCampaignQueryValue = CanAcquireLegendarySignSpecies(gEcHeadlessCampaignQueryId);
        else if (gEcHeadlessCampaignQueryKind == EC_HEADLESS_CAMPAIGN_QUERY_HARVEST)
            gEcHeadlessCampaignQueryValue = GetHarvestedBerryCount(gEcHeadlessCampaignQueryId);
        else if (gEcHeadlessCampaignQueryKind == EC_HEADLESS_CAMPAIGN_QUERY_ITEM)
            gEcHeadlessCampaignQueryValue = CountTotalItemQuantityInBag(gEcHeadlessCampaignQueryId);
        else if (gEcHeadlessCampaignQueryKind == EC_HEADLESS_CAMPAIGN_QUERY_PC_ITEM)
        {
            gEcHeadlessCampaignQueryValue = 0;
            for (u32 slot = 0; slot < PC_ITEMS_COUNT; slot++)
                if (gSaveBlock1Ptr->pcItems[slot].itemId == gEcHeadlessCampaignQueryId)
                    gEcHeadlessCampaignQueryValue += gSaveBlock1Ptr->pcItems[slot].quantity;
        }
        else if (gEcHeadlessCampaignQueryKind == EC_HEADLESS_CAMPAIGN_QUERY_OBJECT
              && gMain.callback2 == CB2_Overworld)
        {
            u8 objectEventId = GetObjectEventIdByLocalIdAndMap(
                gEcHeadlessCampaignQueryId,
                gSaveBlock1Ptr->location.mapNum,
                gSaveBlock1Ptr->location.mapGroup
            );

            gEcHeadlessCampaignQueryObjectActive = objectEventId < OBJECT_EVENTS_COUNT
                && gObjectEvents[objectEventId].active;
            if (gEcHeadlessCampaignQueryObjectActive)
            {
                gEcHeadlessCampaignQueryObjectX =
                    gObjectEvents[objectEventId].currentCoords.x - MAP_OFFSET;
                gEcHeadlessCampaignQueryObjectY =
                    gObjectEvents[objectEventId].currentCoords.y - MAP_OFFSET;
            }
            else
            {
                gEcHeadlessCampaignQueryObjectX = 0;
                gEcHeadlessCampaignQueryObjectY = 0;
            }
            gEcHeadlessCampaignQueryValue = gEcHeadlessCampaignQueryObjectActive;
        }
        else
        {
            gEcHeadlessCampaignQueryValue = 0;
            gEcHeadlessCampaignQueryObjectActive = FALSE;
            gEcHeadlessCampaignQueryObjectX = 0;
            gEcHeadlessCampaignQueryObjectY = 0;
        }

        if (gMain.callback2 == CB2_Overworld)
        {
            gEcHeadlessCampaignMapId = (gSaveBlock1Ptr->location.mapGroup << 8)
                | gSaveBlock1Ptr->location.mapNum;
            gEcHeadlessCampaignMapGroup = gSaveBlock1Ptr->location.mapGroup;
            gEcHeadlessCampaignMapNum = gSaveBlock1Ptr->location.mapNum;
            gEcHeadlessCampaignPlayerX = gSaveBlock1Ptr->pos.x;
            gEcHeadlessCampaignPlayerY = gSaveBlock1Ptr->pos.y;
            gEcHeadlessCampaignPlayerFacing = GetPlayerFacingDirection();
            gEcHeadlessCampaignControlsLocked = ArePlayerFieldControlsLocked();
            gEcHeadlessCampaignScriptEnabled = ScriptContext_IsEnabled();
        }
        else
        {
            gEcHeadlessCampaignControlsLocked = TRUE;
            gEcHeadlessCampaignScriptEnabled = TRUE;
        }
        if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CAPTURE_TO_PARTY)
        {
            gEcHeadlessFixtureObservedResult =
                gEcHeadlessCampaignBattleSerial == 1
                && gEcHeadlessCampaignCaptureSerial == 1
                && gEcHeadlessCampaignLastCapturedSpecies == GetHeadlessCaptureSpecies()
                && gEcHeadlessCampaignLastCaptureResult == MON_GIVEN_TO_PARTY
                && gEcHeadlessCampaignCaptureBookkeepingValid
                && HeadlessCapturedSignIsComplete();
        }
        else if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CAPTURE_TO_PC)
        {
            gEcHeadlessFixtureObservedResult =
                gEcHeadlessCampaignBattleSerial == 1
                && gEcHeadlessCampaignCaptureSerial == 1
                && gEcHeadlessCampaignLastCapturedSpecies == GetHeadlessCaptureSpecies()
                && gEcHeadlessCampaignLastCaptureResult == MON_GIVEN_TO_PC
                && gEcHeadlessCampaignCaptureBookkeepingValid
                && HeadlessCapturedSignIsComplete();
        }
        else if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CAPTURE_QUEST_DIANCIE)
        {
            gEcHeadlessFixtureObservedResult =
                gEcHeadlessCampaignBattleSerial == 1
                && gEcHeadlessCampaignCaptureSerial == 1
                && gEcHeadlessCampaignLastCapturedSpecies == SPECIES_DIANCIE
                && gEcHeadlessCampaignLastCaptureResult == MON_GIVEN_TO_PARTY
                && gEcHeadlessCampaignCaptureBookkeepingValid
                && FlagGet(FLAG_EC_CAUGHT_DIANCIE);
        }
        else if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CAPTURE_QUEST_REGISTEEL)
        {
            gEcHeadlessFixtureObservedResult =
                gEcHeadlessCampaignBattleSerial == 1
                && gEcHeadlessCampaignCaptureSerial == 1
                && gEcHeadlessCampaignLastCapturedSpecies == SPECIES_REGISTEEL
                && gEcHeadlessCampaignCaptureBookkeepingValid
                && FlagGet(FLAG_DEFEATED_REGISTEEL);
        }
        else if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CAPTURE_QUEST_LATIOS)
        {
            gEcHeadlessFixtureObservedResult =
                gEcHeadlessCampaignBattleSerial == 1
                && gEcHeadlessCampaignCaptureSerial == 1
                && gEcHeadlessCampaignLastCapturedSpecies == SPECIES_LATIOS
                && gEcHeadlessCampaignCaptureBookkeepingValid
                && FlagGet(FLAG_CAUGHT_LATIAS_OR_LATIOS);
        }
        else if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CAPTURE_ORDINARY_FIRST)
        {
            gEcHeadlessFixtureObservedResult =
                gEcHeadlessCampaignBattleSerial == 1
                && gEcHeadlessCampaignCaptureSerial == 1
                && !(gEcHeadlessCampaignLastBattleType
                    & (BATTLE_TYPE_TRAINER | BATTLE_TYPE_LEGENDARY | BATTLE_TYPE_CATCH_TUTORIAL))
                && gEcHeadlessCampaignLastCapturedSpecies == SPECIES_POOCHYENA
                && gEcHeadlessCampaignLastCaptureResult == MON_GIVEN_TO_PARTY
                && gEcHeadlessCampaignCaptureBookkeepingValid;
        }
        else if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_ROXANNE_VICTORY)
        {
            gEcHeadlessFixtureObservedResult =
                gEcHeadlessCampaignBattleSerial == 1
                && gEcHeadlessCampaignLastResolution == EC_HEADLESS_BATTLE_WIN
                && FlagGet(FLAG_BADGE01_GET)
                && FlagGet(FLAG_DEFEATED_RUSTBORO_GYM)
                && FlagGet(FLAG_RECEIVED_TM39) // Roxanne's Delphoxite receipt
                && HasTrainerBeenFought(TRAINER_ROXANNE_1)
                && GetCurrentLevelCap() == 20
                && CheckBagHasItem(ITEM_DELPHOXITE, 1)
                && VarGet(VAR_RUSTBORO_CITY_STATE) == 2
                && !gMain.inBattle
                && gMain.callback2 == CB2_Overworld
                && !ArePlayerFieldControlsLocked()
                && !ScriptContext_IsEnabled();
        }
        else if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_FIRST_CENTER_ACQUISITION)
        {
            u32 checks = 0;

#define EC_CENTER_CHECK(bit, condition) if (condition) checks |= 1u << (bit)
            EC_CENTER_CHECK(0, gEcHeadlessCampaignBattleSerial == 0);
            EC_CENTER_CHECK(1, gEcHeadlessCampaignCaptureSerial == 0);
            EC_CENTER_CHECK(2, !FlagGet(FLAG_SYS_POKEDEX_GET)
                           && !FlagGet(FLAG_RECEIVED_POKEDEX_FROM_BIRCH)
                           && !FlagGet(FLAG_ADVENTURE_STARTED));
            EC_CENTER_CHECK(3, FlagGet(FLAG_SYS_POKEMON_GET));
            EC_CENTER_CHECK(4, !FlagGet(FLAG_BADGE01_GET));
            EC_CENTER_CHECK(5, CheckBagHasItem(ITEM_POKE_VIAL, 1));
            EC_CENTER_CHECK(6, CheckBagHasItem(ITEM_LEVELER, 1) && CheckBagHasItem(ITEM_REGENERATOR, 1));
            EC_CENTER_CHECK(7, CheckBagHasItem(ITEM_REPEL_SPRAY, 1));
            EC_CENTER_CHECK(8, CheckBagHasItem(ITEM_FLIGHT_BEACON, 1));
            EC_CENTER_CHECK(9, VarGet(VAR_POKE_VIAL_MAX_CHARGES) == 1);
            EC_CENTER_CHECK(10, VarGet(VAR_POKE_VIAL_CHARGES) == 1);
            EC_CENTER_CHECK(11, VarGet(VAR_RUSTBORO_CITY_STATE) == 0);
            EC_CENTER_CHECK(12, VarGet(VAR_PETALBURG_GYM_STATE) == 0);
            EC_CENTER_CHECK(13, GetGameStat(GAME_STAT_USED_POKECENTER) == 1);
            EC_CENTER_CHECK(14, GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP)
                == GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MAX_HP));
            EC_CENTER_CHECK(15, GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_STATUS) == 0);
            EC_CENTER_CHECK(16, !gMain.inBattle && gMain.callback2 == CB2_Overworld);
            EC_CENTER_CHECK(17, !ArePlayerFieldControlsLocked() && !ScriptContext_IsEnabled());
#undef EC_CENTER_CHECK
            gEcHeadlessCampaignQueryValue = checks;
            gEcHeadlessFixtureObservedResult = checks == (1u << 18) - 1;
        }
        return;
    }

    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_DEWFORD_GYM_ENTRY)
    {
        gEcHeadlessFixtureSetupResult = TRUE;
        if (gSaveBlock1Ptr->location.mapGroup == MAP_GROUP(MAP_DEWFORD_TOWN_GYM)
         && gSaveBlock1Ptr->location.mapNum == MAP_NUM(MAP_DEWFORD_TOWN_GYM))
            gEcHeadlessFixtureObservedResult = TRUE;
        return;
    }
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_MOVE_REPLACEMENT)
    {
        if (gSpecialVar_Result == TRUE && gSpecialVar_0x8005 == 0)
            gEcHeadlessFixtureObservedResult = TRUE;
        return;
    }
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
        ScriptContext_SetupScript(RivalsHouse_EventScript_ChooseStarterRegion);
        gEcHeadlessFixtureSetupResult = TRUE;
        return;
    }
    // A Circuit run in progress: enter the desk's challenge loop at its next
    // match once the lobby is idle, as if the player had just chosen to continue.
    if (gEcHeadlessFixtureActiveScenario == EC_HEADLESS_SCENARIO_CIRCUIT_ROOM
     && !gEcHeadlessFixtureSetupResult
     && gMain.callback2 == CB2_Overworld
     && !ArePlayerFieldControlsLocked() && !ScriptContext_IsEnabled())
    {
        ScriptContext_SetupScript(BattleFrontier_BattleTowerLobby_EventScript_CircuitNextMatch);
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
        if (!(gEcHeadlessFixtureParam & 1))
        {
            if (gBattleStruct->gimmick.triggerSpriteId != MAX_SPRITES)
            {
                gEcHeadlessFixtureSetupResult = TRUE;
                if (gSprites[gBattleStruct->gimmick.triggerSpriteId].inUse)
                    gEcHeadlessFixtureObservedResult = TRUE;
            }
        }
        else if (gBattleMons[B_BATTLER_0].species == GetHeadlessMegaTarget())
        {
            u8 indicator = gBattleStruct->gimmick.indicatorSpriteId[B_BATTLER_0];

            if (gBattleSpritesDataPtr->healthBoxesData[B_BATTLER_0].animFromTableActive)
                gEcHeadlessFixtureSetupResult = TRUE;
            if (gEcHeadlessFixtureSetupResult
             && indicator < MAX_SPRITES && gSprites[indicator].inUse
             && !gBattleSpritesDataPtr->healthBoxesData[B_BATTLER_0].animFromTableActive
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
    case EC_HEADLESS_SCENARIO_WILD_FOE_TYPES:
        if (gBattle_BG0_Y == DISPLAY_HEIGHT
         && GetWindowAttribute(B_WIN_MOVE_DESCRIPTION, WINDOW_TILEMAP_TOP)
            + GetWindowAttribute(B_WIN_MOVE_DESCRIPTION, WINDOW_HEIGHT) == 33
         && gBattleStruct->descriptionSubmenu
         && gBattleStruct->foeTypesSubmenu)
            gEcHeadlessFixtureObservedResult = TRUE;
        break;
    case EC_HEADLESS_SCENARIO_MOVE_FOE_TYPES:
        if (gBattle_BG0_Y == DISPLAY_HEIGHT * 2
         && GetWindowAttribute(B_WIN_MOVE_DESCRIPTION, WINDOW_TILEMAP_TOP)
            + GetWindowAttribute(B_WIN_MOVE_DESCRIPTION, WINDOW_HEIGHT) == 53
         && gBattleStruct->descriptionSubmenu
         && gBattleStruct->foeTypesSubmenu)
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
    gEcHeadlessCampaignForceLoss = FALSE;
    gEcHeadlessFixtureSetupResult = FALSE;
    gEcHeadlessFixtureObservedResult = FALSE;
    gEcHeadlessFixtureTrigger = FALSE;
    sEcHeadlessObservedDelay = 0;
    sEcHeadlessFurfrouMenuOpened = FALSE;

    if (scenario == EC_HEADLESS_SCENARIO_CAMPAIGN_AUTOWIN
     || scenario == EC_HEADLESS_SCENARIO_CAMPAIGN_NATIVE)
    {
        gEcHeadlessCampaignBattleSerial = 0;
        gEcHeadlessCampaignLastBattleType = 0;
        gEcHeadlessCampaignLastOpponentA = TRAINER_NONE;
        gEcHeadlessCampaignLastOpponentB = TRAINER_NONE;
        gEcHeadlessCampaignMapId = 0;
        gEcHeadlessCampaignMapGroup = 0;
        gEcHeadlessCampaignMapNum = 0;
        gEcHeadlessCampaignPlayerX = 0;
        gEcHeadlessCampaignPlayerY = 0;
        gEcHeadlessCampaignPlayerFacing = DIR_NONE;
        gEcHeadlessCampaignControlsLocked = TRUE;
        gEcHeadlessCampaignScriptEnabled = TRUE;
        gEcHeadlessCampaignInBattle = FALSE;
        gEcHeadlessCampaignQueryKind = EC_HEADLESS_CAMPAIGN_QUERY_NONE;
        gEcHeadlessCampaignQueryId = 0;
        gEcHeadlessCampaignQueryValue = 0;
        gEcHeadlessCampaignQueryObjectActive = FALSE;
        gEcHeadlessCampaignQueryObjectX = 0;
        gEcHeadlessCampaignQueryObjectY = 0;
        gEcHeadlessCampaignCaptureSerial = 0;
        gEcHeadlessCampaignLastCapturedSpecies = SPECIES_NONE;
        gEcHeadlessCampaignLastCaptureResult = MON_CANT_GIVE;
        gEcHeadlessCampaignCaptureBookkeepingValid = FALSE;
        gEcHeadlessCampaignLastResolution = EC_HEADLESS_BATTLE_NATIVE;
        sEcHeadlessAutoCaptureInProgress = FALSE;
        gEcHeadlessFixtureSetupResult = TRUE;
        SetMainCallback2(gInitialMainCB2);
        return;
    }

    if (scenario == EC_HEADLESS_SCENARIO_STUDIO_RESUME)
    {
        ReloadSave();
        gEcHeadlessFixtureActiveScenario = EC_HEADLESS_SCENARIO_CAMPAIGN_NATIVE;
        return;
    }
    PrepareHeadlessNewGame();
    if (scenario == EC_HEADLESS_SCENARIO_AGENT_BATTLE)
    {
        // Headless per-turn battle driver. The param packs the requested
        // campaign level cap (low byte) and difficulty (next byte). The host
        // then prepares a stage-legal party and asks for one trainer battle.
        gEcHeadlessFixtureActiveScenario = EC_HEADLESS_SCENARIO_AGENT_BATTLE;
        CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_TREECKO, 5, OTID_STRUCT_PLAYER_ID);
        CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_MUDKIP, 5, OTID_STRUCT_PLAYER_ID);
        CalculatePlayerPartyCount();
        FlagSet(FLAG_SYS_POKEMON_GET);
        VarSet(VAR_EC_OPENING_STATE, EC_OPENING_RESCUE_WON);
        EmeraldChampionsAgentBattleBegin(gEcHeadlessFixtureParam & 0xFF,
                                         (gEcHeadlessFixtureParam >> 8) & 0xFF);
        // Norman grants the Mega Ring before his own battle, the cap-45 stage.
        // Only boards at that cap or later start with the bracelet; earlier
        // fights must be played without Mega Evolution, as the real player does.
        if ((gEcHeadlessFixtureParam & 0xFF) >= 45)
            AddBagItem(ITEM_MEGA_RING, 1);
        gEcHeadlessFixtureSetupResult = TRUE;
        LoadHeadlessMap(MAP_OLDALE_TOWN_POKEMON_CENTER_1F, 8, 6);
        return;
    }
    if (scenario == EC_HEADLESS_SCENARIO_STUDIO_NEW)
    {
        // An explicitly synthetic playground. Battles always resolve natively.
        gEcHeadlessFixtureActiveScenario = EC_HEADLESS_SCENARIO_CAMPAIGN_NATIVE;
        if (gEcHeadlessFixtureParam == 101)
        {
            // The real opening: empty party at Birch's bag, before the pair is chosen.
            VarSet(VAR_ROUTE101_STATE, 2);
            VarSet(VAR_EC_OPENING_STATE, EC_OPENING_UNSELECTED);
            FlagClear(FLAG_HIDE_ROUTE_101_ZIGZAGOON);
            FlagClear(FLAG_HIDE_ROUTE_101_BIRCH_ZIGZAGOON_BATTLE);
            FlagClear(FLAG_HIDE_ROUTE_101_BIRCH_STARTERS_BAG);
            FlagClear(FLAG_RESCUED_BIRCH);
            LoadHeadlessMap(MAP_ROUTE101, 7, 15);
            return;
        }
        CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_TREECKO, 14, OTID_STRUCT_PLAYER_ID);
        CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_MUDKIP, 14, OTID_STRUCT_PLAYER_ID);
        // Real partners arrive knowing their level-up moves; so do these.
        GiveMonInitialMoveset(&gParties[B_TRAINER_PLAYER][0]);
        GiveMonInitialMoveset(&gParties[B_TRAINER_PLAYER][1]);
        CalculatePlayerPartyCount();
        FlagSet(FLAG_SYS_POKEMON_GET);
        VarSet(VAR_EC_OPENING_STATE, EC_OPENING_RESCUE_WON);
        if (gEcHeadlessFixtureParam == 4)
        {
            VarSet(VAR_PETALBURG_CITY_STATE, 1);
            VarSet(VAR_PETALBURG_GYM_STATE, 0);
            LoadHeadlessMap(MAP_PETALBURG_CITY_GYM, 4, 108);
        }
        else if (gEcHeadlessFixtureParam == 7)
            LoadHeadlessMap(MAP_RUSTBORO_CITY_GYM, 5, 4);
        else
            LoadHeadlessMap(MAP_OLDALE_TOWN_POKEMON_CENTER_1F, 8, 6);
        return;
    }

    if (scenario == EC_HEADLESS_SCENARIO_CAPTURE_TO_PARTY
     || scenario == EC_HEADLESS_SCENARIO_CAPTURE_TO_PC)
    {
        gEcHeadlessCampaignBattleSerial = 0;
        gEcHeadlessCampaignCaptureSerial = 0;
        gEcHeadlessCampaignLastCapturedSpecies = SPECIES_NONE;
        gEcHeadlessCampaignLastCaptureResult = MON_CANT_GIVE;
        gEcHeadlessCampaignCaptureBookkeepingValid = FALSE;
        sEcHeadlessAutoCaptureInProgress = FALSE;
        PrepareHeadlessCaptureBattle(scenario == EC_HEADLESS_SCENARIO_CAPTURE_TO_PC);
        return;
    }
    if (scenario == EC_HEADLESS_SCENARIO_CAPTURE_QUEST_DIANCIE)
    {
        gEcHeadlessCampaignBattleSerial = 0;
        gEcHeadlessCampaignCaptureSerial = 0;
        gEcHeadlessCampaignLastCapturedSpecies = SPECIES_NONE;
        gEcHeadlessCampaignLastCaptureResult = MON_CANT_GIVE;
        gEcHeadlessCampaignCaptureBookkeepingValid = FALSE;
        sEcHeadlessAutoCaptureInProgress = FALSE;
        PrepareHeadlessOverworldFixtureState(SPECIES_DIANCIE);
        CreateHealthyHeadlessMon(
            &gParties[B_TRAINER_PLAYER][0], SPECIES_GEODUDE, 70,
            OTID_STRUCT_PLAYER_ID
        );
        CalculatePlayerPartyCount();
        LoadHeadlessMap(MAP_CAVE_OF_ORIGIN_DIANCIES_ROOM, 9, 11);
        return;
    }
    if (scenario == EC_HEADLESS_SCENARIO_CAPTURE_QUEST_REGISTEEL
     || scenario == EC_HEADLESS_SCENARIO_CAPTURE_QUEST_LATIOS)
    {
        gEcHeadlessCampaignBattleSerial = 0;
        gEcHeadlessCampaignCaptureSerial = 0;
        gEcHeadlessCampaignLastCapturedSpecies = SPECIES_NONE;
        gEcHeadlessCampaignLastCaptureResult = MON_CANT_GIVE;
        gEcHeadlessCampaignCaptureBookkeepingValid = FALSE;
        sEcHeadlessAutoCaptureInProgress = FALSE;
        CreateHealthyHeadlessMon(
            &gParties[B_TRAINER_PLAYER][0], SPECIES_GEODUDE, 70,
            OTID_STRUCT_PLAYER_ID
        );
        CalculatePlayerPartyCount();
        if (scenario == EC_HEADLESS_SCENARIO_CAPTURE_QUEST_REGISTEEL)
            LoadHeadlessMap(MAP_ANCIENT_TOMB, 8, 8);
        else
        {
            FlagSet(FLAG_ENABLE_SHIP_SOUTHERN_ISLAND);
            LoadHeadlessMap(MAP_SOUTHERN_ISLAND_INTERIOR, 13, 12);
        }
        return;
    }
    if (scenario == EC_HEADLESS_SCENARIO_CAPTURE_ORDINARY_FIRST)
    {
        gEcHeadlessCampaignBattleSerial = 0;
        gEcHeadlessCampaignCaptureSerial = 0;
        gEcHeadlessCampaignLastCapturedSpecies = SPECIES_NONE;
        gEcHeadlessCampaignLastCaptureResult = MON_CANT_GIVE;
        gEcHeadlessCampaignCaptureBookkeepingValid = FALSE;
        PrepareHeadlessWildBattle(FALSE);
        gBattleTypeFlags = 0;
        return;
    }
    if (scenario == EC_HEADLESS_SCENARIO_ROXANNE_VICTORY)
    {
        gEcHeadlessCampaignBattleSerial = 0;
        gEcHeadlessCampaignLastResolution = EC_HEADLESS_BATTLE_NATIVE;
        CreateHealthyHeadlessMon(
            &gParties[B_TRAINER_PLAYER][0], SPECIES_GEODUDE, 14,
            OTID_STRUCT_PLAYER_ID
        );
        CreateHealthyHeadlessMon(
            &gParties[B_TRAINER_PLAYER][1], SPECIES_TREECKO, 14,
            OTID_STRUCT_PLAYER_ID
        );
        CalculatePlayerPartyCount();
        LoadHeadlessMap(MAP_RUSTBORO_CITY_GYM, 5, 3);
        return;
    }
    if (scenario == EC_HEADLESS_SCENARIO_FIRST_CENTER_ACQUISITION)
    {
        u32 hp = 1;
        u32 status = STATUS1_POISON;

        gEcHeadlessCampaignBattleSerial = 0;
        gEcHeadlessCampaignCaptureSerial = 0;
        FlagClear(FLAG_SYS_POKEDEX_GET);
        FlagClear(FLAG_RECEIVED_DEXNAV);
        FlagClear(FLAG_SYS_NATIONAL_DEX);
        FlagClear(FLAG_RECEIVED_POKEDEX_FROM_BIRCH);
        FlagClear(FLAG_ADVENTURE_STARTED);
        FlagSet(FLAG_SYS_POKEMON_GET);
        CreateHealthyHeadlessMon(
            &gParties[B_TRAINER_PLAYER][0], SPECIES_TREECKO, 10,
            OTID_STRUCT_PLAYER_ID
        );
        CreateHealthyHeadlessMon(
            &gParties[B_TRAINER_PLAYER][1], SPECIES_POOCHYENA, 8,
            OTID_STRUCT_PLAYER_ID
        );
        SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP, &hp);
        SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_STATUS, &status);
        CalculatePlayerPartyCount();
        LoadHeadlessMap(MAP_OLDALE_TOWN_POKEMON_CENTER_1F, 8, 3);
        return;
    }

    switch (scenario)
    {
    case EC_HEADLESS_SCENARIO_C14_SONG:
    {
        u32 param = gEcHeadlessFixtureParam;
        PrepareCircuitParty();
        FlagSet(FLAG_BADGE02_GET);
        FlagSet(FLAG_EC_MANOR_NOTES_READ);
        if (param != 3)
            AddBagItem(ITEM_MEGA_RING, 1);
        if (param != 1)
        {
            CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_JIGGLYPUFF, 20, OTID_STRUCT_PLAYER_ID);
            SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_SING, 0);
        }
        LoadHeadlessMap(MAP_DEWFORD_MEADOW, 27, 13);
        return;
    }
    case EC_HEADLESS_SCENARIO_BERRY_ECONOMY:
    {
        u32 param = gEcHeadlessFixtureParam;
        PrepareCircuitParty();
        AddBagItem(ITEM_BERRY_POUCH, 1);
        AddBagItem(ITEM_WAILMER_PAIL, 1);
        AddBagItem(ITEM_LUM_BERRY, 6);
        if (param == 1 || param == 2 || param == 7)
            memset(gSaveBlock2Ptr->pokedex.harvestedBerries, 30, NUM_BERRIES);
        if (param == 8)
            memset(gSaveBlock2Ptr->pokedex.harvestedBerries, 255, NUM_BERRIES);
        if (param == 2)
        {
            struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_BAXCALIBRITE)];
            for (u32 slot = 0; slot < pocket->capacity; slot++)
                BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_DRAGONINITE, 1);
        }
        if (param >= 3 && param <= 5)
        {
            PlantBerryTree(BERRY_TREE_ROUTE_102_ORAN, BERRY_ID_ORAN, BERRY_STAGE_BERRIES, TRUE);
            if (param == 4)
                AddHarvestedBerries(BERRY_ID_ORAN, 255);
            if (param == 5)
            {
                struct BagPocket *pocket = &gBagPockets[POCKET_BERRIES];
                for (u32 slot = 0; slot < pocket->capacity; slot++)
                    BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_CHERI_BERRY, MAX_BAG_ITEM_CAPACITY);
            }
            LoadHeadlessMap(MAP_ROUTE102, 24, 3);
        }
        else if (param == 0 || param == 8)
            LoadHeadlessMap(MAP_OLDALE_TOWN_POKEMON_CENTER_1F, 2, 3);
        else
        {
            if (param == 7)
                FlagSet(FLAG_EC_BERRY_TRADE_BAXCALIBRITE);
            LoadHeadlessMap(MAP_ROUTE123_BERRY_MASTERS_HOUSE, 4, 5);
        }
        return;
    }
    case EC_HEADLESS_SCENARIO_CENTER_OLDALE:
        if (gEcHeadlessFixtureParam != 0)
        {
            // An established save holds every Center tool; without the spray
            // the nurse's back-fill dialog would shift the scenario timeline.
            AddBagItem(ITEM_POKE_VIAL, 1);
            AddBagItem(ITEM_LEVELER, 1);
            AddBagItem(ITEM_REGENERATOR, 1);
            AddBagItem(ITEM_REPEL_SPRAY, 1);
            AddBagItem(ITEM_FLIGHT_BEACON, 1);
            VarSet(VAR_POKE_VIAL_MAX_CHARGES, POKE_VIAL_CAPACITY_BASE);
            VarSet(VAR_POKE_VIAL_CHARGES, 1);
            PrepareCircuitParty();
        }
        if (gEcHeadlessFixtureParam == 7)
        {
            // Cap/UI fixture: real item, Leveler, summary and storage flows.
            FlagSet(FLAG_DELIVERED_DEVON_GOODS); // Chapter cap 30.
            FlagSet(FLAG_SYS_POKEMON_GET);
            AddBagItem(ITEM_MEGA_RING, 1);
            AddBagItem(ITEM_VENUSAURITE, 1);
            AddBagItem(ITEM_CHARIZARDITE_X, 1);
            AddBagItem(ITEM_MEWTWONITE_X, 1);
            ZeroPlayerPartyMons();
            CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_VENUSAUR, 30, OTID_STRUCT_PLAYER_ID);
            CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_CHARIZARD, 30, OTID_STRUCT_PLAYER_ID);
            CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][2], SPECIES_MEWTWO, 26, OTID_STRUCT_PLAYER_ID);
            CalculatePlayerPartyCount();
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
    case EC_HEADLESS_SCENARIO_ECONOMY_SHOPS:
        // Synthetic shop prerequisites; all purchases use native NPC/menu input.
        ClearBag();
        memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
        SetMoney(&gSaveBlock1Ptr->money, gEcHeadlessFixtureParam == 0 ? 6000 : 20000);
        if (gEcHeadlessFixtureParam == 0)
            LoadHeadlessMap(MAP_RUSTBORO_CITY_MART, 5, 3);
        else
        {
            if (gEcHeadlessFixtureParam != 2)
                AddBagItem(ITEM_MEGA_RING, 1);
            if (gEcHeadlessFixtureParam == 3)
                AddPCItem(ITEM_PIDGEOTITE, 1);
            if (gEcHeadlessFixtureParam == 4)
            {
                u16 item = ITEM_PIDGEOTITE;
                CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_PIDGEOT, 36, OTID_STRUCT_PLAYER_ID);
                SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &item);
                CalculatePlayerPartyCount();
            }
            if (gEcHeadlessFixtureParam == 5)
            {
                struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_PIDGEOTITE)];
                for (u32 slot = 0; slot < pocket->capacity; slot++)
                    BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_POTION, MAX_BAG_ITEM_CAPACITY);
            }
            if (gEcHeadlessFixtureParam == 6)
                SetMoney(&gSaveBlock1Ptr->money, 19999);
            if (gEcHeadlessFixtureParam == 7)
                LoadHeadlessMap(MAP_LILYCOVE_CITY_DEPARTMENT_STORE_4F, 9, 4);
            else if (gEcHeadlessFixtureParam == 8)
                LoadHeadlessMap(MAP_LILYCOVE_CITY_DEPARTMENT_STORE_4F, 7, 4);
            else
                LoadHeadlessMap(MAP_LILYCOVE_CITY_DEPARTMENT_STORE_3F, 10, 4);
        }
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
        // Param 3: too little money for the tutor's EV training fee.
        if (gEcHeadlessFixtureParam == 3)
            SetMoney(&gSaveBlock1Ptr->money, 100);
        if (gEcHeadlessFixtureParam == 2)
            LoadHeadlessMap(MAP_FALLARBOR_TOWN_MOVE_RELEARNERS_HOUSE, 7, 5);
        else
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
        // The Starter Archive prices its hatchlings and dolls in money.
        SetMoney(&gSaveBlock1Ptr->money, 999999);
        LoadHeadlessMap(MAP_MAUVILLE_CITY_GAME_CORNER, 13, 3);
        break;
    case EC_HEADLESS_SCENARIO_CIRCUIT_LOBBY:
        // The player stands at the Circuit desk (the Tower's Single counter) with
        // a legal party of six. Param 0: Frontier open. 1: before the Hall of Fame
        // (the desk refuses). 2: two Legendary Pokemon (party rule refusal).
        // 3: only five Pokemon. 4: Frontier open with a prior record (best 12,
        // 40 lifetime wins, rules already heard) for the record board.
        PrepareCircuitParty();
        if (gEcHeadlessFixtureParam != 1)
        {
            FlagSet(FLAG_SYS_GAME_CLEAR);
            FlagSet(FLAG_IS_CHAMPION);
        }
        if (gEcHeadlessFixtureParam == 2)
        {
            CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_MEWTWO, 80, OTID_STRUCT_PLAYER_ID);
            CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_LUGIA, 80, OTID_STRUCT_PLAYER_ID);
        }
        else if (gEcHeadlessFixtureParam == 3)
        {
            ZeroMonData(&gParties[B_TRAINER_PLAYER][PARTY_SIZE - 1]);
            CalculatePlayerPartyCount();
        }
        else if (gEcHeadlessFixtureParam == 4)
        {
            FlagSet(FLAG_EC_CHAMPIONS_CIRCUIT_EXPLAINED);
            VarSet(VAR_EC_CIRCUIT_BEST_WINS, 12);
            VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 40);
        }
        LoadHeadlessMap(MAP_BATTLE_FRONTIER_BATTLE_TOWER_LOBBY, 6, 6);
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
    case EC_HEADLESS_SCENARIO_MOVE_REPLACEMENT:
        PrepareMoveReplacement();
        break;
    case EC_HEADLESS_SCENARIO_DEWFORD_GYM_ENTRY:
        LoadHeadlessMap(MAP_DEWFORD_TOWN, 8, 18);
        break;
    case EC_HEADLESS_SCENARIO_MAP_SWEEP:
        {
            u32 index = gEcHeadlessFixtureParam;
            if (index >= ARRAY_COUNT(sEcHeadlessMapSweep))
                break;
            // Established-save state so story objects are in their normal positions.
            FlagSet(FLAG_SYS_POKEDEX_GET);
            FlagSet(FLAG_RECEIVED_DEXNAV);
            FlagSet(FLAG_SYS_POKEMON_GET);
            FlagSet(FLAG_SYS_POKENAV_GET);
            // Ground Mega Stone actors only spawn once the Ring is carried.
            AddBagItem(ITEM_MEGA_RING, 1);
            LoadHeadlessMap(sEcHeadlessMapSweep[index].map, sEcHeadlessMapSweep[index].x, sEcHeadlessMapSweep[index].y);
            gEcHeadlessFixtureSetupResult = TRUE;
        }
        break;
    case EC_HEADLESS_SCENARIO_LEAF_SCENE:
        {
            u32 slot;
            // Established Flash unlock uses normal cave-entry illumination.
            FlagSet(FLAG_BADGE02_GET);
            FlagSet(FLAG_RECEIVED_HM_FLASH);
            ZeroPlayerPartyMons();
            CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ZIGZAGOON, 80, OTID_STRUCT_PLAYER_ID);
            if (gEcHeadlessFixtureParam == EC_HEADLESS_LEAF_FIRST_BATTLE)
                CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_GEODUDE, 80, OTID_STRUCT_PLAYER_ID);
            CalculatePlayerPartyCount();
            FlagSet(FLAG_SYS_POKEMON_GET);
            VarSet(VAR_LEAF_STATE, 0);
            FlagClear(FLAG_DEFEATED_LEAF);
            ClearTrainerFlag(TRAINER_LEAF_ALTERING_CAVE);
            ClearBag();
            if (gEcHeadlessFixtureParam == EC_HEADLESS_LEAF_COMPLETED)
            {
                SetTrainerFlag(TRAINER_LEAF_ALTERING_CAVE);
                VarSet(VAR_LEAF_STATE, 1);
                FlagSet(FLAG_DEFEATED_LEAF);
            }
            else if (gEcHeadlessFixtureParam == EC_HEADLESS_LEAF_PENDING_FULL_BAG)
            {
                // The reward is Bottle Caps; fill their pocket with something else.
                struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_BOTTLE_CAP)];
                SetTrainerFlag(TRAINER_LEAF_ALTERING_CAVE);
                for (slot = 0; slot < pocket->capacity; slot++)
                    BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_NUGGET, 1);
            }
            LoadHeadlessMap(MAP_ALTERING_CAVE_B1F, 21, 20);
        }
        break;
    case EC_HEADLESS_SCENARIO_SECRET_BASE_ESTABLISHED:
        {
            struct SecretBase *base = &gSaveBlock1Ptr->secretBases[0];
            base->secretBaseId = SECRET_BASE_RED_CAVE1_1;
            StringCopyN(base->trainerName, gSaveBlock2Ptr->playerName, PLAYER_NAME_LENGTH);
            memcpy(base->trainerId, gSaveBlock2Ptr->playerTrainerId, TRAINER_ID_LENGTH);
            base->gender = gSaveBlock2Ptr->playerGender;
            base->language = GAME_LANGUAGE;
            base->numTimesEntered = 1;
            base->decorations[0] = DECOR_SMALL_CHAIR;
            base->decorationPositions[0] = (5 << 4) | 5;
            VarSet(VAR_CURRENT_SECRET_BASE, 0);
            VarSet(VAR_SECRET_BASE_MAP, MAPSEC_ROUTE_118);
            VarSet(VAR_INIT_SECRET_BASE, 1);
            VarSet(VAR_SECRET_BASE_INITIALIZED, 0);
            FlagSet(FLAG_HIDE_SECRET_BASE_TRAINER);
            LoadHeadlessMap(MAP_SECRET_BASE_RED_CAVE1, 6, 5);
        }
        break;
    case EC_HEADLESS_SCENARIO_RYDEL_RETRY:
        ClearBag();
        memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
        FlagClear(FLAG_RECEIVED_BIKE);
        FlagClear(FLAG_DECLINED_BIKE);
        FillHeadlessKeyPocket();
        LoadHeadlessMap(MAP_MAUVILLE_CITY_BIKE_SHOP, 1, 5);
        break;
    case EC_HEADLESS_SCENARIO_ROUTE110_RETRY:
        {
            u32 lane = gEcHeadlessFixtureParam % 3;
            bool32 oneUsable = gEcHeadlessFixtureParam >= 3 && gEcHeadlessFixtureParam < 6;
            CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ZIGZAGOON, 30, OTID_STRUCT_PLAYER_ID);
            CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_GEODUDE, 30, OTID_STRUCT_PLAYER_ID);
            CalculatePlayerPartyCount();
            FlagSet(FLAG_SYS_POKEMON_GET);
            ClearBag();
            memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
            VarSet(VAR_STARTER_MON, 1);
            VarSet(VAR_ROUTE110_STATE, 0);
            VarSet(VAR_REPEL_STEP_COUNT, 250);
            FlagClear(FLAG_HIDE_ROUTE_110_RIVAL);
            FlagSet(FLAG_HIDE_ROUTE_110_RIVAL_ON_BIKE);
            if (oneUsable)
            {
                u16 hp = 0;
                SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HP, &hp);
            }
            else if (gEcHeadlessFixtureParam < 3)
            {
                FillHeadlessKeyPocket();
                for (u32 slot = 0; slot < PC_ITEMS_COUNT; slot++)
                    gSaveBlock1Ptr->pcItems[slot] = (struct ItemSlot){ITEM_POTION, 1};
            }
            LoadHeadlessMap(MAP_ROUTE110, 33 + lane, 57);
        }
        break;
    case EC_HEADLESS_SCENARIO_BOOK_RESEARCH:
        PrepareBookResearchScene();
        break;
    case EC_HEADLESS_SCENARIO_STORY_HANDOFF:
        {
            u32 slot;
            CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ZIGZAGOON, 20, OTID_STRUCT_PLAYER_ID);
            CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_GEODUDE, 20, OTID_STRUCT_PLAYER_ID);
            CalculatePlayerPartyCount();
            FlagSet(FLAG_SYS_POKEMON_GET);
            ClearBag();
            memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
            // Opening reward/bracelet audit: synthetic prerequisites, native scripts.
            if (gEcHeadlessFixtureParam >= 252 && gEcHeadlessFixtureParam <= 283)
            {
                u32 scene = gEcHeadlessFixtureParam;
                FlagSet(FLAG_ADVENTURE_STARTED);
                FlagSet(FLAG_RESCUED_BIRCH);
                FlagSet(FLAG_RECEIVED_POKEDEX_FROM_BIRCH);
                FlagClear(FLAG_BADGE01_GET);
                FlagClear(FLAG_BADGE02_GET);
                VarSet(VAR_PETALBURG_CITY_STATE, 3);
                VarSet(VAR_PETALBURG_GYM_STATE, 2);
                FlagClear(FLAG_EC_WOODS_GREAT_BALL_PENDING);
                FlagClear(FLAG_EC_RUSTBORO_GREAT_BALL_PENDING);
                AddBagItem(ITEM_POKE_VIAL, 1);
                AddBagItem(ITEM_LEVELER, 1);
                AddBagItem(ITEM_REGENERATOR, 1);
                AddBagItem(ITEM_REPEL_SPRAY, 1);
                AddBagItem(ITEM_FLIGHT_BEACON, 1);
                VarSet(VAR_POKE_VIAL_MAX_CHARGES, POKE_VIAL_CAPACITY_BASE);
                if (scene == 254 || scene == 255 || scene == 256 || scene == 270)
                {
                    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_GREAT_BALL)];
                    for (slot = 0; slot < pocket->capacity; slot++)
                        BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_POKE_BALL, MAX_BAG_ITEM_CAPACITY);
                }
                if (scene <= 255)
                {
                    VarSet(VAR_PETALBURG_WOODS_STATE, 0);
                    FlagClear(FLAG_HIDE_PETALBURG_WOODS_DEVON_EMPLOYEE);
                    FlagClear(FLAG_HIDE_PETALBURG_WOODS_AQUA_GRUNT);
                    ClearTrainerFlag(TRAINER_GRUNT_PETALBURG_WOODS);
                    LoadHeadlessMap(MAP_PETALBURG_WOODS, scene & 1 ? 27 : 26, 24);
                }
                else if (scene == 256 || scene == 257)
                {
                    FlagSet(FLAG_EC_WOODS_GREAT_BALL_PENDING);
                    FlagSet(FLAG_EC_RUSTBORO_GREAT_BALL_PENDING);
                    LoadHeadlessMap(MAP_PETALBURG_CITY_POKEMON_CENTER_1F, 8, 4);
                }
                else if (scene == 258 || scene == 259)
                {
                    if (scene == 259) AddBagItem(ITEM_MEGA_RING, 1);
                    FlagClear(FLAG_EC_MEGA_REWARD_BUTTERFRENITE);
                    SetTrainerFlag(TRAINER_LYLE);
                    SetTrainerFlag(TRAINER_JAMES_1);
                    LoadHeadlessMap(MAP_PETALBURG_WOODS, 4, 9);
                }
                else if (scene == 260 || scene == 261)
                {
                    if (scene == 261) AddBagItem(ITEM_MEGA_RING, 1);
                    FlagClear(FLAG_EC_MEGA_GIFT_VENUSAURITE);
                    FlagClear(FLAG_DAILY_FLOWER_SHOP_RECEIVED_BERRY);
                    LoadHeadlessMap(MAP_ROUTE104_PRETTY_PETAL_FLOWER_SHOP, 11, 7);
                }
                else if (scene == 262)
                {
                    FlagClear(FLAG_ITEM_ROUTE_102_POTION);
                    SetTrainerFlag(TRAINER_CALVIN_1);
                    SetTrainerFlag(TRAINER_RICK);
                    SetTrainerFlag(TRAINER_ALLEN);
                    SetTrainerFlag(TRAINER_TIANA);
                    LoadHeadlessMap(MAP_ROUTE102, 11, 16);
                }
                else if (scene == 263 || scene == 264)
                {
                    FlagClear(FLAG_RECEIVED_ROUTE104_LEAF_STONE);
                    FlagClear(FLAG_RECEIVED_ROUTE104_FLORIST_LEAF_STONE);
                    LoadHeadlessMap(MAP_ROUTE104, scene == 263 ? 5 : 8, scene == 263 ? 27 : 20);
                }
                else if (scene == 265 || scene == 266)
                {
                    FlagSet(FLAG_BADGE01_GET);
                    SetTrainerFlag(TRAINER_ROXANNE_1);
                    FlagClear(FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE);
                    if (scene == 266)
                    {
                        AddBagItem(ITEM_MEGA_RING, 1);
                        FlagSet(FLAG_RECEIVED_ROXANNE_OLD_AMBER);
                    }
                    else FlagClear(FLAG_RECEIVED_ROXANNE_OLD_AMBER);
                    LoadHeadlessMap(MAP_RUSTBORO_CITY_GYM, 5, 3);
                }
                else if (scene == 267 || scene == 268)
                {
                    FlagSet(FLAG_BADGE02_GET);
                    SetTrainerFlag(TRAINER_BRAWLY_1);
                    FlagClear(FLAG_RECEIVED_BRAWLY_LUCARIONITE);
                    if (scene == 268) AddBagItem(ITEM_MEGA_RING, 1);
                    LoadHeadlessMap(MAP_DEWFORD_TOWN_GYM, 4, 4);
                }
                else if (scene == 269)
                {
                    VarSet(VAR_PETALBURG_WOODS_STATE, 1);
                    FlagSet(FLAG_HIDE_PETALBURG_WOODS_DEVON_EMPLOYEE);
                    FlagSet(FLAG_HIDE_PETALBURG_WOODS_AQUA_GRUNT);
                    FlagSet(FLAG_EC_WOODS_GREAT_BALL_PENDING);
                    LoadHeadlessMap(MAP_PETALBURG_WOODS, 26, 24);
                }
                else if (scene == 272 || scene == 279 || scene == 280 || scene == 283)
                {
                    FlagSet(FLAG_BADGE02_GET);
                    FlagSet(FLAG_DELIVERED_STEVEN_LETTER);
                    FlagClear(FLAG_HIDE_GRANITE_CAVE_STEVEN);
                    FlagClear(FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE);
                    AddBagItem(ITEM_OLD_AMBER, 1);
                    if (scene == 280) AddBagItem(ITEM_AERODACTYLITE, 1);
                    if (scene == 279 || scene == 283)
                    {
                        struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_AERODACTYLITE)];
                        for (slot = 0; slot < pocket->capacity; slot++)
                            BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_VENUSAURITE, MAX_BAG_ITEM_CAPACITY);
                        if (scene == 279)
                            for (slot = 0; slot < PC_ITEMS_COUNT; slot++)
                                gSaveBlock1Ptr->pcItems[slot] = (struct ItemSlot){ITEM_POTION, MAX_BAG_ITEM_CAPACITY};
                    }
                    LoadHeadlessMap(MAP_GRANITE_CAVE_STEVENS_ROOM, 7, 9);
                }
                else if (scene == 281)
                {
                    FlagClear(FLAG_EC_GARDEN_BUNDLE_ROUTE115_PINAP_BERRY);
                    LoadHeadlessMap(MAP_ROUTE115, 20, 61);
                }
                else if (scene == 282)
                {
                    FlagSet(FLAG_EC_WOODS_GREAT_BALL_PENDING);
                    LoadHeadlessMap(MAP_PETALBURG_CITY_POKEMON_CENTER_1F, 8, 4);
                }
                else if (scene == 273)
                {
                    FlagClear(FLAG_RECEIVED_PETALBURG_WOODS_TART_APPLE);
                    LoadHeadlessMap(MAP_PETALBURG_WOODS, 33, 7);
                }
                else if (scene == 274)
                {
                    FlagSet(FLAG_EC_BIRCH_GREAT_BALLS_PENDING);
                    LoadHeadlessMap(MAP_PETALBURG_CITY_POKEMON_CENTER_1F, 8, 4);
                }
                else if (scene == 277 || scene == 278)
                {
                    ClearTrainerFlag(TRAINER_GINA_AND_MIA_1);
                    LoadHeadlessMap(MAP_ROUTE104, scene == 277 ? 27 : 28, 16);
                }
                else if (scene == 275 || scene == 276)
                {
                    FlagClear(FLAG_RECEIVED_POTION_OLDALE);
                    // The guide's first tour already ran; retry without restaging it.
                    AddBagItem(ITEM_POKE_BALL, 10);
                    if (scene == 275)
                    {
                        struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_HEAL_BALL)];
                        for (slot = 0; slot < pocket->capacity; slot++)
                            BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_POKE_BALL, MAX_BAG_ITEM_CAPACITY);
                    }
                    LoadHeadlessMap(MAP_OLDALE_TOWN, 13, 15);
                }
                else
                {
                    FlagSet(FLAG_RECOVERED_DEVON_GOODS);
                    FlagClear(FLAG_RETURNED_DEVON_GOODS);
                    FlagClear(FLAG_HIDE_RUSTBORO_CITY_DEVON_EMPLOYEE_1);
                    VarSet(VAR_RUSTBORO_CITY_STATE, 4);
                    LoadHeadlessMap(MAP_RUSTBORO_CITY, 30, 9);
                }
                break;
            }
            if (gEcHeadlessFixtureParam == 251)
            {
                // Synthetic dialogue-only review; never an earned campaign warp.
                FlagClear(FLAG_MET_PRETTY_PETAL_SHOP_OWNER);
                FlagClear(FLAG_BADGE03_GET);
                FlagSet(FLAG_RECEIVED_WAILMER_PAIL);
                LoadHeadlessMap(MAP_ROUTE104_PRETTY_PETAL_FLOWER_SHOP, 4, 7);
                break;
            }
            if (gEcHeadlessFixtureParam == 60 || gEcHeadlessFixtureParam == 61)
            {
                gSaveBlock2Ptr->playerGender = gEcHeadlessFixtureParam == 60 ? MALE : FEMALE;
                VarSet(VAR_METEOR_FALLS_STATE, 0);
                FlagClear(FLAG_HIDE_METEOR_FALLS_TEAM_MAGMA);
                FlagClear(FLAG_HIDE_METEOR_FALLS_1F_1R_COZMO);
                FlagSet(FLAG_HIDE_METEOR_FALLS_TEAM_AQUA);
                FlagClear(FLAG_HIDE_ROUTE_112_TEAM_MAGMA);
                ClearTrainerFlag(TRAINER_COURTNEY_METEOR_FALLS);
                ClearTrainerFlag(TRAINER_GRUNT_METEOR_FALLS);
                LoadHeadlessMap(MAP_METEOR_FALLS_1F_1R, 15, 18);
                break;
            }
            if (gEcHeadlessFixtureParam >= 64 && gEcHeadlessFixtureParam <= 68)
            {
                gSaveBlock2Ptr->playerGender = (gEcHeadlessFixtureParam & 1) ? FEMALE : MALE;
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_BADGE02_GET);
                FlagSet(FLAG_BADGE03_GET);
                FlagSet(FLAG_BADGE04_GET);
                FlagSet(FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY);
                FlagClear(FLAG_RECEIVED_GO_GOGGLES);
                FlagSet(FLAG_HIDE_LAVARIDGE_TOWN_RIVAL);
                FlagSet(FLAG_HIDE_LAVARIDGE_TOWN_RIVAL_ON_BIKE);
                VarSet(VAR_LAVARIDGE_TOWN_STATE, 1);
                if (gEcHeadlessFixtureParam == 68)
                {
                    FillHeadlessKeyPocket();
                    for (slot = 0; slot < PC_ITEMS_COUNT; slot++)
                        gSaveBlock1Ptr->pcItems[slot] = (struct ItemSlot){ITEM_POTION, 1};
                }
                LoadHeadlessMap(MAP_LAVARIDGE_TOWN,
                    gEcHeadlessFixtureParam == 66 || gEcHeadlessFixtureParam == 67 ? 9 : 5,
                    gEcHeadlessFixtureParam == 66 || gEcHeadlessFixtureParam == 67 ? 7 : 16);
                break;
            }
            if (gEcHeadlessFixtureParam == 69 || gEcHeadlessFixtureParam == 70)
            {
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_BADGE02_GET);
                FlagSet(FLAG_BADGE03_GET);
                FlagClear(FLAG_BADGE04_GET);
                FlagClear(FLAG_DEFEATED_LAVARIDGE_GYM);
                FlagClear(FLAG_RECEIVED_FLANNERY_CAMERUPTITE);
                ClearTrainerFlag(TRAINER_FLANNERY_1);
                SetTrainerFlag(TRAINER_COLE);
                SetTrainerFlag(TRAINER_GERALD);
                VarSet(VAR_PETALBURG_GYM_STATE, 5);
                VarSet(VAR_LAVARIDGE_TOWN_STATE, 0);
                LoadHeadlessMap(MAP_LAVARIDGE_TOWN_GYM_1F,
                    gEcHeadlessFixtureParam == 69 ? 13 : 3,
                    gEcHeadlessFixtureParam == 69 ? 10 : 13);
                break;
            }
            if (gEcHeadlessFixtureParam == 71 || gEcHeadlessFixtureParam == 72)
            {
                const u16 trainers[] = {TRAINER_COLE, TRAINER_GERALD, TRAINER_AXLE,
                    TRAINER_DANIELLE, TRAINER_KEEGAN, TRAINER_JACE, TRAINER_JEFF, TRAINER_ELI};
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_BADGE02_GET);
                FlagSet(FLAG_BADGE03_GET);
                FlagClear(FLAG_BADGE04_GET);
                FlagClear(FLAG_DEFEATED_LAVARIDGE_GYM);
                ClearTrainerFlag(TRAINER_FLANNERY_1);
                for (slot = 0; slot < ARRAY_COUNT(trainers); slot++)
                    SetTrainerFlag(trainers[slot]);
                if (gEcHeadlessFixtureParam == 72)
                    ClearTrainerFlag(TRAINER_COLE);
                LoadHeadlessMap(MAP_LAVARIDGE_TOWN_GYM_1F,
                    gEcHeadlessFixtureParam == 71 ? 13 : 3,
                    gEcHeadlessFixtureParam == 71 ? 17 : 13);
                break;
            }
            if (gEcHeadlessFixtureParam >= 73 && gEcHeadlessFixtureParam <= 76)
            {
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_BADGE02_GET);
                FlagSet(FLAG_BADGE03_GET);
                FlagSet(FLAG_BADGE04_GET);
                FlagClear(FLAG_BADGE05_GET);
                FlagClear(FLAG_DEFEATED_PETALBURG_GYM);
                FlagClear(FLAG_RECEIVED_HM_SURF);
                FlagClear(FLAG_RECEIVED_NORMAN_LOPUNNITE);
                FlagSet(FLAG_HIDE_PETALBURG_GYM_WALLY);
                FlagSet(FLAG_HIDE_PETALBURG_GYM_WALLYS_DAD);
                ClearTrainerFlag(TRAINER_NORMAN_1);
                VarSet(VAR_PETALBURG_GYM_STATE, 6);
                VarSet(VAR_PETALBURG_CITY_STATE, 3);
                if (gEcHeadlessFixtureParam == 76)
                {
                    const u16 trainers[] = {TRAINER_RANDALL, TRAINER_MARY, TRAINER_PARKER,
                        TRAINER_ALEXIA, TRAINER_GEORGE, TRAINER_JODY, TRAINER_BERKE};
                    for (slot = 0; slot < ARRAY_COUNT(trainers); slot++)
                        ClearTrainerFlag(trainers[slot]);
                    FlagClear(FLAG_HIDE_PETALBURG_GYM_GREETER);
                    LoadHeadlessMap(MAP_PETALBURG_CITY_GYM, 4, 110);
                    break;
                }
                LoadHeadlessMap(MAP_PETALBURG_CITY_GYM,
                    gEcHeadlessFixtureParam == 73 ? 4 : gEcHeadlessFixtureParam == 74 ? 3 : 5,
                    gEcHeadlessFixtureParam == 73 ? 3 : 2);
                break;
            }
            if (gEcHeadlessFixtureParam >= 241 && gEcHeadlessFixtureParam <= 250)
            {
                u32 param = gEcHeadlessFixtureParam;
                const u16 badges[] = {FLAG_BADGE01_GET, FLAG_BADGE02_GET, FLAG_BADGE03_GET,
                    FLAG_BADGE04_GET, FLAG_BADGE05_GET, FLAG_BADGE06_GET, FLAG_BADGE07_GET};
                for (slot = 0; slot < ARRAY_COUNT(badges); slot++)
                    FlagSet(badges[slot]);
                FlagClear(FLAG_BADGE08_GET);
                FlagClear(FLAG_SYS_GAME_CLEAR);
                FlagClear(FLAG_SYS_WEATHER_CTRL);
                FlagSet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
                FlagSet(FLAG_HIDE_SOOTOPOLIS_CITY_STEVEN);
                FlagSet(FLAG_HIDE_SOOTOPOLIS_CITY_WALLACE);
                FlagSet(FLAG_HIDE_SOOTOPOLIS_CITY_ARCHIE);
                FlagSet(FLAG_HIDE_SOOTOPOLIS_CITY_MAXIE);
                FlagSet(FLAG_HIDE_SOOTOPOLIS_CITY_RESIDENTS);
                FlagSet(FLAG_HIDE_CAVE_OF_ORIGIN_B1F_WALLACE);
                VarSet(VAR_SOOTOPOLIS_CITY_STATE, SOOTOPOLIS_STATE_GYM_BEATEN);
                VarSet(VAR_SKY_PILLAR_STATE, 3);
                VarSet(VAR_SKY_PILLAR_RAYQUAZA_CRY_DONE, 1);
                if (param <= 242)
                    LoadHeadlessMap(MAP_SOOTOPOLIS_CITY_GYM_1F, 8, 22);
                else if (param <= 244)
                {
                    if (param == 244)
                        FlagSet(FLAG_BADGE08_GET);
                    LoadHeadlessMap(MAP_CAVE_OF_ORIGIN_1F, 5, 9);
                }
                else if (param == 245 || param == 249)
                {
                    FlagSet(FLAG_BADGE08_GET);
                    FlagClear(FLAG_EC_CAUGHT_DIANCIE);
                    LoadHeadlessMap(MAP_CAVE_OF_ORIGIN_DIANCIES_ROOM, 9, 10);
                }
                else if (param == 246 || param == 250)
                {
                    FlagClear(FLAG_DEFEATED_RAYQUAZA);
                    FlagSet(FLAG_HIDE_SKY_PILLAR_TOP_RAYQUAZA);
                    FlagClear(FLAG_HIDE_SKY_PILLAR_TOP_RAYQUAZA_STILL);
                    LoadHeadlessMap(MAP_SKY_PILLAR_TOP, 14, 7);
                }
                else
                {
                    if (param == 247)
                        FlagClear(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
                    LoadHeadlessMap(MAP_CAVE_OF_ORIGIN_B1F, 8, 4);
                }
                break;
            }
            if (gEcHeadlessFixtureParam >= 213 && gEcHeadlessFixtureParam <= 230)
            {
                u32 param = gEcHeadlessFixtureParam;
                const u16 badges[] = {FLAG_BADGE01_GET, FLAG_BADGE02_GET, FLAG_BADGE03_GET,
                    FLAG_BADGE04_GET, FLAG_BADGE05_GET, FLAG_BADGE06_GET, FLAG_BADGE07_GET};
                for (slot = 0; slot < ARRAY_COUNT(badges); slot++)
                    FlagSet(badges[slot]);
                FlagClear(FLAG_BADGE08_GET);
                FlagSet(FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN);
                FlagSet(FLAG_SYS_WEATHER_CTRL);
                FlagSet(FLAG_LEGENDARIES_IN_SOOTOPOLIS);
                FlagClear(FLAG_HIDE_SOOTOPOLIS_CITY_STEVEN);
                FlagClear(FLAG_HIDE_SOOTOPOLIS_CITY_ARCHIE);
                FlagClear(FLAG_HIDE_SOOTOPOLIS_CITY_MAXIE);
                FlagClear(FLAG_HIDE_SOOTOPOLIS_CITY_RESIDENTS);
                FlagClear(FLAG_HIDE_SOOTOPOLIS_CITY_GROUDON);
                FlagClear(FLAG_HIDE_SOOTOPOLIS_CITY_KYOGRE);
                FlagSet(FLAG_HIDE_SOOTOPOLIS_CITY_RAYQUAZA);
                FlagSet(FLAG_HIDE_SOOTOPOLIS_CITY_WALLACE);
                FlagClear(FLAG_STEVEN_GUIDES_TO_CAVE_OF_ORIGIN);
                FlagClear(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
                FlagClear(FLAG_MET_ARCHIE_SOOTOPOLIS);
                FlagClear(FLAG_MET_MAXIE_SOOTOPOLIS);
                FlagClear(FLAG_RECEIVED_HM_WATERFALL);
                VarSet(VAR_SOOTOPOLIS_CITY_STATE, SOOTOPOLIS_STATE_LEGENDS_CLASH);
                VarSet(VAR_SKY_PILLAR_STATE, 0);
                VarSet(VAR_SOOTOPOLIS_WALLACE_STATE, 0);
                if (param <= 214)
                    LoadHeadlessMap(MAP_SOOTOPOLIS_CITY, param == 213 ? 29 : 43, param == 213 ? 53 : 32);
                else if (param <= 216)
                {
                    VarSet(VAR_SOOTOPOLIS_CITY_STATE, SOOTOPOLIS_STATE_LEGENDS_SEEN);
                    LoadHeadlessMap(MAP_SOOTOPOLIS_CITY, param == 215 ? 21 : 20, param == 215 ? 36 : 37);
                }
                else if (param == 217)
                {
                    VarSet(VAR_SOOTOPOLIS_CITY_STATE, SOOTOPOLIS_STATE_LEGENDS_SEEN);
                    FlagClear(FLAG_HIDE_CAVE_OF_ORIGIN_B1F_WALLACE);
                    LoadHeadlessMap(MAP_CAVE_OF_ORIGIN_B1F, 9, 14);
                }
                else if (param == 218)
                {
                    VarSet(VAR_SOOTOPOLIS_CITY_STATE, SOOTOPOLIS_STATE_TO_SKY_PILLAR);
                    FlagSet(FLAG_WALLACE_GOES_TO_SKY_PILLAR);
                    FlagClear(FLAG_HIDE_SKY_PILLAR_WALLACE);
                    LoadHeadlessMap(MAP_SKY_PILLAR_OUTSIDE, 17, 14);
                }
                else if (param == 219)
                {
                    VarSet(VAR_SOOTOPOLIS_CITY_STATE, SOOTOPOLIS_STATE_SKY_PILLAR_OPEN);
                    VarSet(VAR_SKY_PILLAR_RAYQUAZA_CRY_DONE, 0);
                    FlagClear(FLAG_HIDE_SKY_PILLAR_TOP_RAYQUAZA);
                    FlagSet(FLAG_HIDE_SKY_PILLAR_TOP_RAYQUAZA_STILL);
                    LoadHeadlessMap(MAP_SKY_PILLAR_TOP, 14, 10);
                }
                else if (param <= 223)
                {
                    VarSet(VAR_SOOTOPOLIS_CITY_STATE, SOOTOPOLIS_STATE_RAYQUAZA_AWAKE);
                    VarSet(VAR_SKY_PILLAR_STATE, param <= 221 ? 1 : 3);
                    FlagSet(FLAG_STEVEN_GUIDES_TO_CAVE_OF_ORIGIN);
                    FlagClear(FLAG_HIDE_SOOTOPOLIS_CITY_WALLACE);
                    if (param >= 222)
                    {
                        FlagClear(FLAG_SYS_WEATHER_CTRL);
                        FlagClear(FLAG_LEGENDARIES_IN_SOOTOPOLIS);
                        FlagSet(FLAG_HIDE_SOOTOPOLIS_CITY_GROUDON);
                        FlagSet(FLAG_HIDE_SOOTOPOLIS_CITY_KYOGRE);
                    }
                    if (param == 223)
                    {
                        FlagSet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
                        FlagSet(FLAG_HIDE_SOOTOPOLIS_CITY_ARCHIE);
                        FlagSet(FLAG_HIDE_SOOTOPOLIS_CITY_MAXIE);
                    }
                    LoadHeadlessMap(MAP_SOOTOPOLIS_CITY,
                        param == 220 ? 29 : param == 221 ? 43 : param == 222 ? 33 : 31,
                        param == 220 ? 53 : param == 221 ? 32 : param == 222 ? 36 : 34);
                }
                else if (param == 224)
                {
                    FlagClear(FLAG_DEFEATED_SOOTOPOLIS_GYM);
                    FlagClear(FLAG_RECEIVED_JUAN_GYARADOSITE);
                    ClearTrainerFlag(TRAINER_JUAN_1);
                    LoadHeadlessMap(MAP_SOOTOPOLIS_CITY_GYM_1F, 8, 3);
                }
                else if (param >= 229)
                {
                    FlagClear(FLAG_SYS_CLOCK_SET);
                    FlagClear(FLAG_DAILY_SOOTOPOLIS_RECEIVED_BERRY);
                    VarSet(VAR_SOOTOPOLIS_CITY_STATE, SOOTOPOLIS_STATE_GYM_BEATEN);
                    if (param == 230)
                    {
                        struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_CHERI_BERRY)];
                        for (slot = 0; slot < pocket->capacity; slot++)
                            BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_CHERI_BERRY, MAX_BAG_ITEM_CAPACITY);
                        // Only the first berry can fit: the second must undo the first.
                        BagPocket_SetSlotItemIdAndCount(pocket, 0, ITEM_NONE, 0);
                    }
                    LoadHeadlessMap(MAP_SOOTOPOLIS_CITY, 9, 44);
                }
                else
                {
                    CreateMon(&gParties[B_TRAINER_PLAYER][0], param <= 226 ? SPECIES_SEEDOT : SPECIES_LOTAD, 30, 0xFFFFFFFF, OTID_STRUCT_PLAYER_ID);
                    CalculateMonStats(&gParties[B_TRAINER_PLAYER][0]);
                    VarSet(VAR_SEEDOT_SIZE_RECORD, 0x8000);
                    VarSet(VAR_LOTAD_SIZE_RECORD, 0x8000);
                    RemoveBagItem(ITEM_ELIXIR, CountTotalItemQuantityInBag(ITEM_ELIXIR));
                    if (param == 226 || param == 228)
                    {
                        struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_ELIXIR)];
                        for (slot = 0; slot < pocket->capacity; slot++)
                            BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_POTION, MAX_BAG_ITEM_CAPACITY);
                    }
                    LoadHeadlessMap(MAP_SOOTOPOLIS_CITY_LOTAD_AND_SEEDOT_HOUSE, param <= 226 ? 5 : 2, 5);
                }
                break;
            }
            if (gEcHeadlessFixtureParam >= 190 && gEcHeadlessFixtureParam <= 212)
            {
                const struct { u16 map; s16 x, y; } signs[] = {
                    {MAP_SEAFLOOR_CAVERN_ENTRANCE, 11, 4},
                    {MAP_SEAFLOOR_CAVERN_ROOM1, 6, 18},
                    {MAP_SEAFLOOR_CAVERN_ROOM2, 11, 6},
                    {MAP_SEAFLOOR_CAVERN_ROOM3, 10, 12},
                    {MAP_SEAFLOOR_CAVERN_ROOM4, 12, 3},
                    {MAP_SEAFLOOR_CAVERN_ROOM5, 5, 3},
                    {MAP_SEAFLOOR_CAVERN_ROOM6, 10, 21},
                    {MAP_SEAFLOOR_CAVERN_ROOM7, 4, 23},
                    {MAP_SEAFLOOR_CAVERN_ROOM8, 3, 12},
                    {MAP_ROUTE126, 13, 1},
                    {MAP_ROUTE127, 13, 23},
                    {MAP_ROUTE128, 64, 30},
                    {MAP_UNDERWATER_ROUTE126, 46, 67},
                };
                CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_WAILMER, 40, OTID_STRUCT_PLAYER_ID);
                for (slot = 0; slot < MAX_MON_MOVES; slot++)
                    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], slot == 0 ? MOVE_WATER_GUN : MOVE_NONE, slot);
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_BADGE02_GET);
                FlagSet(FLAG_BADGE03_GET);
                FlagSet(FLAG_BADGE04_GET);
                FlagSet(FLAG_BADGE05_GET);
                FlagSet(FLAG_BADGE06_GET);
                FlagSet(FLAG_BADGE07_GET);
                FlagClear(FLAG_BADGE08_GET);
                FlagSet(FLAG_RECEIVED_HM_DIVE);
                FlagSet(FLAG_RECEIVED_HM_SURF);
                FlagSet(FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE);
                FlagClear(FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN);
                FlagClear(FLAG_SYS_WEATHER_CTRL);
                FlagClear(FLAG_LEGENDARIES_IN_SOOTOPOLIS);
                FlagClear(FLAG_HIDE_UNDERWATER_SEA_FLOOR_CAVERN_STOLEN_SUBMARINE);
                FlagClear(FLAG_HIDE_SEAFLOOR_CAVERN_ENTRANCE_AQUA_GRUNT);
                FlagClear(FLAG_HIDE_SEAFLOOR_CAVERN_AQUA_GRUNTS);
                FlagClear(FLAG_HIDE_SEAFLOOR_CAVERN_ROOM_9_KYOGRE_ASLEEP);
                FlagSet(FLAG_HIDE_SEAFLOOR_CAVERN_ROOM_9_KYOGRE);
                FlagSet(FLAG_HIDE_SEAFLOOR_CAVERN_ROOM_9_ARCHIE);
                FlagSet(FLAG_HIDE_SEAFLOOR_CAVERN_ROOM_9_MAXIE);
                FlagSet(FLAG_HIDE_SEAFLOOR_CAVERN_ROOM_9_MAGMA_GRUNTS);
                FlagSet(FLAG_HIDE_ROUTE_128_STEVEN);
                FlagSet(FLAG_HIDE_ROUTE_128_ARCHIE);
                FlagSet(FLAG_HIDE_ROUTE_128_MAXIE);
                VarSet(VAR_SEAFLOOR_CAVERN_STATE, 0);
                VarSet(VAR_ROUTE128_STATE, 0);
                VarSet(VAR_SOOTOPOLIS_CITY_STATE, SOOTOPOLIS_STATE_CALM);
                VarSet(VAR_HAS_TALKED_TO_SEAFLOOR_CAVERN_ENTRANCE_GRUNT, 0);
                ClearTrainerFlag(TRAINER_ARCHIE);
                if (gEcHeadlessFixtureParam >= 200)
                {
                    slot = gEcHeadlessFixtureParam - 200;
                    LoadHeadlessMap(signs[slot].map, signs[slot].x, signs[slot].y);
                }
                else if (gEcHeadlessFixtureParam <= 192)
                {
                    if (gEcHeadlessFixtureParam == 191)
                    {
                        u16 hp = 0;
                        SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HP, &hp);
                    }
                    if (gEcHeadlessFixtureParam == 192)
                    {
                        FlagSet(FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN);
                        FlagSet(FLAG_HIDE_SEAFLOOR_CAVERN_ROOM_9_KYOGRE_ASLEEP);
                        FlagSet(FLAG_HIDE_SEAFLOOR_CAVERN_AQUA_GRUNTS);
                        VarSet(VAR_SEAFLOOR_CAVERN_STATE, 1);
                    }
                    LoadHeadlessMap(MAP_SEAFLOOR_CAVERN_ROOM9, 17, 43);
                }
                else if (gEcHeadlessFixtureParam <= 194)
                {
                    if (gEcHeadlessFixtureParam == 194)
                        FlagSet(FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN);
                    LoadHeadlessMap(MAP_UNDERWATER_SEAFLOOR_CAVERN, 6, 5);
                }
                else if (gEcHeadlessFixtureParam == 195)
                    LoadHeadlessMap(MAP_ROUTE128, 38, 27);
                else if (gEcHeadlessFixtureParam == 196)
                    LoadHeadlessMap(MAP_UNDERWATER_ROUTE126, 45, 66);
                else
                    LoadHeadlessMap(MAP_SEAFLOOR_CAVERN_ENTRANCE, 10, 3);
                break;
            }
            if (gEcHeadlessFixtureParam >= 174 && gEcHeadlessFixtureParam <= 189)
            {
                const u16 collectionFlags[] = {
                    FLAG_RECEIVED_SHOAL_SALT_1, FLAG_RECEIVED_SHOAL_SALT_2,
                    FLAG_RECEIVED_SHOAL_SALT_3, FLAG_RECEIVED_SHOAL_SALT_4,
                    FLAG_RECEIVED_SHOAL_SHELL_1, FLAG_RECEIVED_SHOAL_SHELL_2,
                    FLAG_RECEIVED_SHOAL_SHELL_3, FLAG_RECEIVED_SHOAL_SHELL_4,
                };
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_BADGE02_GET);
                FlagSet(FLAG_BADGE03_GET);
                FlagSet(FLAG_BADGE04_GET);
                FlagSet(FLAG_BADGE05_GET);
                FlagSet(FLAG_BADGE06_GET);
                FlagSet(FLAG_BADGE07_GET);
                FlagClear(FLAG_BADGE08_GET);
                FlagClear(FLAG_SYS_CLOCK_SET); // Daily-reset cases seed its earned pending flag below.
                FlagClear(FLAG_SYS_SHOAL_TIDE);
                FlagClear(FLAG_EC_CAUGHT_ARTICUNO);
                FlagClear(FLAG_RECEIVED_SHOAL_DEEP_SEA_SCALE);
                for (slot = 0; slot < ARRAY_COUNT(collectionFlags); slot++)
                    FlagClear(collectionFlags[slot]);
                if (gEcHeadlessFixtureParam == 174 || gEcHeadlessFixtureParam == 184)
                {
                    // The tide is save state now; 184 approaches at high tide.
                    if (gEcHeadlessFixtureParam == 184)
                        FlagSet(FLAG_SYS_SHOAL_TIDE);
                    LoadHeadlessMap(MAP_ROUTE125, 22, 20);
                }
                else if (gEcHeadlessFixtureParam >= 185)
                {
                    if (gEcHeadlessFixtureParam == 185)
                        LoadHeadlessMap(MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM, 20, 16);
                    else if (gEcHeadlessFixtureParam <= 187)
                    {
                        if (gEcHeadlessFixtureParam == 187)
                            FlagSet(FLAG_SYS_SHOAL_TIDE);
                        LoadHeadlessMap(MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM, 41, 23);
                    }
                    else if (gEcHeadlessFixtureParam == 188)
                        LoadHeadlessMap(MAP_SHOAL_CAVE_LOW_TIDE_STAIRS_ROOM, 10, 13);
                    else
                        LoadHeadlessMap(MAP_SHOAL_CAVE_LOW_TIDE_LOWER_ROOM, 12, 6);
                }
                else if (gEcHeadlessFixtureParam <= 176)
                {
                    FlagClear(FLAG_BADGE07_GET);
                    if (gEcHeadlessFixtureParam == 176)
                        FlagClear(FLAG_BADGE06_GET);
                    LoadHeadlessMap(MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM, 8, 9);
                }
                else if (gEcHeadlessFixtureParam <= 178)
                {
                    if (gEcHeadlessFixtureParam == 178)
                        GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_ZEKROM), FLAG_SET_CAUGHT);
                    LoadHeadlessMap(MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM, 12, 11);
                }
                else if (gEcHeadlessFixtureParam <= 180)
                {
                    for (slot = 0; slot < ARRAY_COUNT(collectionFlags); slot++)
                        FlagSet(collectionFlags[slot]);
                    // 180 arrives with the deposits already taken and the tide
                    // waiting to be turned, which is what restocks them now.
                    LoadHeadlessMap(MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM, 17, 15);
                }
                else if (gEcHeadlessFixtureParam == 181)
                    LoadHeadlessMap(MAP_SHOAL_CAVE_LOW_TIDE_STAIRS_ROOM, 11, 12);
                else if (gEcHeadlessFixtureParam == 182)
                {
                    FlagSet(FLAG_SYS_SHOAL_TIDE);
                    LoadHeadlessMap(MAP_SHOAL_CAVE_LOW_TIDE_INNER_ROOM, 41, 21);
                }
                else
                    LoadHeadlessMap(MAP_SHOAL_CAVE_LOW_TIDE_LOWER_ROOM, 11, 5);
                break;
            }
            if (gEcHeadlessFixtureParam >= 164 && gEcHeadlessFixtureParam <= 173)
            {
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_BADGE02_GET);
                FlagSet(FLAG_BADGE03_GET);
                FlagSet(FLAG_BADGE04_GET);
                FlagSet(FLAG_BADGE05_GET);
                FlagSet(FLAG_BADGE06_GET);
                FlagClear(FLAG_BADGE08_GET);
                FlagSet(FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE);
                if (gEcHeadlessFixtureParam == 164 || gEcHeadlessFixtureParam == 172)
                {
                    FlagClear(FLAG_BADGE07_GET);
                    FlagClear(FLAG_DEFEATED_MOSSDEEP_GYM);
                    FlagClear(FLAG_RECEIVED_TATE_LIZA_METAGROSSITE);
                    ClearTrainerFlag(TRAINER_TATE_AND_LIZA_1);
                    LoadHeadlessMap(MAP_MOSSDEEP_CITY_GYM,
                        gEcHeadlessFixtureParam == 164 ? 23 : 2,
                        gEcHeadlessFixtureParam == 164 ? 8 : 22);
                }
                else
                {
                    FlagSet(FLAG_BADGE07_GET);
                    VarSet(VAR_MOSSDEEP_CITY_STATE, 2);
                    FlagClear(FLAG_HIDE_MOSSDEEP_CITY_SPACE_CENTER_1F_TEAM_MAGMA);
                    FlagClear(FLAG_HIDE_MOSSDEEP_CITY_SPACE_CENTER_2F_TEAM_MAGMA);
                    FlagClear(FLAG_HIDE_MOSSDEEP_CITY_SPACE_CENTER_2F_STEVEN);
                    FlagSet(FLAG_HIDE_MOSSDEEP_CITY_SPACE_CENTER_1F_STEVEN);
                    VarSet(VAR_MOSSDEEP_SPACE_CENTER_STATE, 2);
                    if (gEcHeadlessFixtureParam == 165 || gEcHeadlessFixtureParam == 166)
                    {
                        VarSet(VAR_MOSSDEEP_SPACE_CENTER_STAIR_GUARD_STATE,
                            gEcHeadlessFixtureParam == 165 ? 0 : 2);
                        FlagClear(FLAG_DEFEATED_GRUNT_SPACE_CENTER_1F);
                        ClearTrainerFlag(TRAINER_GRUNT_SPACE_CENTER_2);
                        if (gEcHeadlessFixtureParam == 166)
                        {
                            FlagSet(FLAG_DEFEATED_GRUNT_SPACE_CENTER_1F);
                            SetTrainerFlag(TRAINER_GRUNT_SPACE_CENTER_2);
                        }
                        LoadHeadlessMap(MAP_MOSSDEEP_CITY_SPACE_CENTER_1F, 13, 3);
                    }
                    else if (gEcHeadlessFixtureParam <= 168)
                    {
                        VarSet(VAR_MOSSDEEP_SPACE_CENTER_STATE, 1);
                        ClearTrainerFlag(TRAINER_GRUNT_SPACE_CENTER_5);
                        ClearTrainerFlag(TRAINER_GRUNT_SPACE_CENTER_6);
                        ClearTrainerFlag(TRAINER_GRUNT_SPACE_CENTER_7);
                        LoadHeadlessMap(MAP_MOSSDEEP_CITY_SPACE_CENTER_2F, 13, 2);
                    }
                    else if (gEcHeadlessFixtureParam <= 170)
                    {
                        // Full roster for real three-of-six selection and restoration.
                        FlagSet(FLAG_ADVENTURE_STARTED);
                        SetLastHealLocationWarp(HEAL_LOCATION_MOSSDEEP_CITY);
                        PrepareCircuitParty();
                        for (u32 partySlot = 0; partySlot < PARTY_SIZE; partySlot++)
                        {
                            u16 hp = 1;
                            ClampMonToPlayerLevelCap(&gParties[B_TRAINER_PLAYER][partySlot]);
                            SetMonData(&gParties[B_TRAINER_PLAYER][partySlot], MON_DATA_HP, &hp);
                        }
                        AddBagItem(ITEM_POKE_VIAL, 1);
                        VarSet(VAR_POKE_VIAL_MAX_CHARGES, POKE_VIAL_CAPACITY_BLOB);
                        VarSet(VAR_POKE_VIAL_CHARGES, 0);
                        SetTrainerFlag(TRAINER_GRUNT_SPACE_CENTER_5);
                        SetTrainerFlag(TRAINER_GRUNT_SPACE_CENTER_7);
                        if (gEcHeadlessFixtureParam == 169)
                            SetTrainerFlag(TRAINER_TABITHA_MOSSDEEP);
                        else // 170: the last commander's fight is still outstanding.
                            ClearTrainerFlag(TRAINER_TABITHA_MOSSDEEP);
                        FlagSet(FLAG_INTERACTED_WITH_STEVEN_SPACE_CENTER);
                        VarSet(VAR_MOSSDEEP_CITY_STATE, 1); // Preserve the already-heard prompt on entry.
                        LoadHeadlessMap(MAP_MOSSDEEP_CITY_SPACE_CENTER_2F, 2, 8);
                    }
                    else if (gEcHeadlessFixtureParam == 171)
                    {
                        FlagClear(FLAG_HIDE_MOSSDEEP_CITY_STEVENS_HOUSE_STEVEN);
                        FlagSet(FLAG_RECEIVED_HM_DIVE);
                        FlagSet(FLAG_EC_REPORT_C42_COMPLETE);
                        VarSet(VAR_STEVENS_HOUSE_STATE, 2);
                        LoadHeadlessMap(MAP_MOSSDEEP_CITY_STEVENS_HOUSE, 3, 7);
                    }
                    else
                    {
                        VarSet(VAR_MOSSDEEP_CITY_STATE, 1);
                        FlagClear(FLAG_HIDE_MOSSDEEP_CITY_TEAM_MAGMA);
                        LoadHeadlessMap(MAP_MOSSDEEP_CITY, 40, 24);
                    }
                }
                break;
            }
            if (gEcHeadlessFixtureParam >= 157 && gEcHeadlessFixtureParam <= 163)
            {
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_BADGE02_GET);
                FlagSet(FLAG_BADGE03_GET);
                FlagSet(FLAG_BADGE04_GET);
                FlagSet(FLAG_BADGE05_GET);
                FlagSet(FLAG_BADGE06_GET);
                FlagClear(FLAG_BADGE07_GET);
                FlagClear(FLAG_BADGE08_GET);
                FlagSet(FLAG_RECEIVED_SS_TICKET);
                FlagSet(FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT);
                FlagClear(FLAG_MET_TEAM_AQUA_HARBOR);
                FlagClear(FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE);
                FlagClear(FLAG_HIDE_SLATEPORT_CITY_HARBOR_CAPTAIN_STERN);
                FlagClear(FLAG_HIDE_SLATEPORT_CITY_HARBOR_SUBMARINE_SHADOW);
                FlagClear(FLAG_HIDE_SLATEPORT_CITY_HARBOR_AQUA_GRUNT);
                FlagClear(FLAG_HIDE_SLATEPORT_CITY_HARBOR_ARCHIE);
                VarSet(VAR_SLATEPORT_CITY_STATE, 1);
                VarSet(VAR_SLATEPORT_HARBOR_STATE, 1);
                if (gEcHeadlessFixtureParam == 157)
                {
                    FlagClear(FLAG_HIDE_SLATEPORT_CITY_CAPTAIN_STERN);
                    FlagClear(FLAG_HIDE_SLATEPORT_CITY_GABBY_AND_TY);
                    LoadHeadlessMap(MAP_SLATEPORT_CITY, 27, 13);
                }
                else if (gEcHeadlessFixtureParam <= 161)
                    LoadHeadlessMap(MAP_SLATEPORT_CITY_HARBOR, 9, 11 + gEcHeadlessFixtureParam - 158);
                else if (gEcHeadlessFixtureParam == 162)
                {
                    FlagClear(FLAG_HIDE_AQUA_HIDEOUT_GRUNTS);
                    FlagClear(FLAG_HIDE_AQUA_HIDEOUT_B2F_SUBMARINE_SHADOW);
                    ClearTrainerFlag(TRAINER_MATT);
                    LoadHeadlessMap(MAP_AQUA_HIDEOUT_B2F, 24, 19);
                }
                else
                {
                    VarSet(VAR_SLATEPORT_HARBOR_STATE, 2);
                    FlagSet(FLAG_MET_TEAM_AQUA_HARBOR);
                    FlagSet(FLAG_HIDE_SLATEPORT_CITY_HARBOR_PATRONS);
                    FlagSet(FLAG_HIDE_SLATEPORT_CITY_HARBOR_SUBMARINE_SHADOW);
                    FlagSet(FLAG_HIDE_SLATEPORT_CITY_HARBOR_AQUA_GRUNT);
                    FlagSet(FLAG_HIDE_SLATEPORT_CITY_HARBOR_ARCHIE);
                    LoadHeadlessMap(MAP_SLATEPORT_CITY_HARBOR, 8, 11);
                }
                break;
            }
            if (gEcHeadlessFixtureParam >= 147 && gEcHeadlessFixtureParam <= 156)
            {
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_BADGE02_GET);
                FlagSet(FLAG_BADGE03_GET);
                FlagSet(FLAG_BADGE04_GET);
                FlagSet(FLAG_BADGE05_GET);
                FlagSet(FLAG_BADGE06_GET);
                FlagClear(FLAG_BADGE07_GET);
                FlagClear(FLAG_BADGE08_GET);
                FlagClear(FLAG_RECEIVED_RED_OR_BLUE_ORB);
                if (gEcHeadlessFixtureParam <= 151)
                {
                    VarSet(VAR_MT_PYRE_STATE, 0);
                    FlagClear(FLAG_HIDE_MT_PYRE_SUMMIT_ARCHIE);
                    FlagClear(FLAG_HIDE_MT_PYRE_SUMMIT_TEAM_AQUA);
                    FlagSet(FLAG_HIDE_MT_PYRE_SUMMIT_MAXIE);
                    if (gEcHeadlessFixtureParam >= 150)
                        FillHeadlessKeyPocket();
                    if (gEcHeadlessFixtureParam == 151)
                        for (slot = 0; slot < PC_ITEMS_COUNT; slot++)
                            gSaveBlock1Ptr->pcItems[slot] = (struct ItemSlot){ITEM_POTION, 1};
                    LoadHeadlessMap(MAP_MT_PYRE_SUMMIT,
                        gEcHeadlessFixtureParam <= 149 ? 22 + gEcHeadlessFixtureParam - 147 : 23, 8);
                }
                else if (gEcHeadlessFixtureParam <= 153 || gEcHeadlessFixtureParam == 156)
                {
                    FlagClear(FLAG_HIDE_MAGMA_HIDEOUT_GRUNTS);
                    FlagClear(FLAG_HIDE_MAGMA_HIDEOUT_4F_GROUDON_ASLEEP);
                    if (gEcHeadlessFixtureParam == 156)
                        FlagSet(FLAG_HIDE_MAGMA_HIDEOUT_4F_GROUDON_ASLEEP);
                    FlagSet(FLAG_HIDE_MAGMA_HIDEOUT_4F_GROUDON);
                    FlagClear(FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT);
                    SetTrainerFlag(TRAINER_TABITHA_MAGMA_HIDEOUT);
                    if (gEcHeadlessFixtureParam != 153)
                        SetTrainerFlag(TRAINER_COURTNEY_MAGMA_HIDEOUT);
                    else
                        ClearTrainerFlag(TRAINER_COURTNEY_MAGMA_HIDEOUT);
                    ClearTrainerFlag(TRAINER_MAXIE_MAGMA_HIDEOUT);
                    LoadHeadlessMap(MAP_MAGMA_HIDEOUT_4F, 15, 21);
                }
                else
                {
                    VarSet(VAR_MT_PYRE_STATE, 1);
                    FlagSet(FLAG_RECEIVED_RED_OR_BLUE_ORB);
                    FlagSet(FLAG_HIDE_MT_PYRE_SUMMIT_ARCHIE);
                    FlagSet(FLAG_HIDE_MT_PYRE_SUMMIT_MAXIE);
                    FlagSet(FLAG_HIDE_MT_PYRE_SUMMIT_TEAM_AQUA);
                    if (gEcHeadlessFixtureParam == 154)
                        GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_MUNNA), FLAG_SET_CAUGHT);
                    // Darkrai's altar is read from beside the elders.
                    LoadHeadlessMap(MAP_MT_PYRE_SUMMIT, 24, 5);
                }
                break;
            }
            if (gEcHeadlessFixtureParam >= 141 && gEcHeadlessFixtureParam <= 146)
            {
                if (gEcHeadlessFixtureParam == 141)
                {
                    struct LilycoveLadyFavor *favor = &gSaveBlock1Ptr->lilycoveLady.favor;
                    memset(favor, 0, sizeof(*favor));
                    favor->id = LILYCOVE_LADY_FAVOR;
                    favor->itemId = ITEM_UNREMARKABLE_TEACUP;
                    favor->language = gGameLanguage;
                    StringCopy(favor->playerName, COMPOUND_STRING("WWWWWWW"));
                    LoadHeadlessMap(MAP_LILYCOVE_CITY_POKEMON_CENTER_1F, 4, 5);
                }
                else if (gEcHeadlessFixtureParam == 142 || gEcHeadlessFixtureParam == 143)
                {
                    VarSet(VAR_LILYCOVE_MUSEUM_2F_STATE, gEcHeadlessFixtureParam == 143);
                    LoadHeadlessMap(MAP_LILYCOVE_CITY_LILYCOVE_MUSEUM_2F, 11,
                        gEcHeadlessFixtureParam == 142 ? 8 : 7);
                }
                else if (gEcHeadlessFixtureParam == 146)
                {
                    struct LilycoveLadyQuiz *quiz = &gSaveBlock1Ptr->lilycoveLady.quiz;
                    memset(quiz, 0, sizeof(*quiz));
                    quiz->id = LILYCOVE_LADY_QUIZ;
                    quiz->state = LILYCOVE_LADY_STATE_PRIZE;
                    quiz->prize = ITEM_MAX_ETHER;
                    StringCopy(quiz->playerName, COMPOUND_STRING(""));
                    quiz->language = gGameLanguage;
                    LoadHeadlessMap(MAP_LILYCOVE_CITY_POKEMON_CENTER_1F, 4, 5);
                }
                else
                {
                    VarSet(VAR_LILYCOVE_CONTEST_LOBBY_STATE, 0);
                    LoadHeadlessMap(MAP_LILYCOVE_CITY_CONTEST_LOBBY, 14, 4);
                }
                break;
            }
            if (gEcHeadlessFixtureParam >= 129 && gEcHeadlessFixtureParam <= 140)
            {
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_BADGE02_GET);
                FlagSet(FLAG_BADGE03_GET);
                FlagSet(FLAG_BADGE04_GET);
                FlagSet(FLAG_BADGE05_GET);
                FlagSet(FLAG_BADGE06_GET);
                FlagClear(FLAG_BADGE07_GET);
                FlagClear(FLAG_BADGE08_GET);
                FlagClear(FLAG_SYS_GAME_CLEAR);
                if (gEcHeadlessFixtureParam <= 134)
                {
                    u32 first = (gEcHeadlessFixtureParam - 129) % 3;
                    gSaveBlock2Ptr->playerGender = gEcHeadlessFixtureParam <= 131 ? MALE : FEMALE;
                    VarSet(VAR_STARTER_MON, first);
                    VarSet(VAR_EC_SECOND_STARTER, (first + 1) % 3 + 1);
                    FlagClear(FLAG_HIDE_LILYCOVE_CITY_RIVAL);
                    FlagClear(FLAG_DECLINED_RIVAL_BATTLE_LILYCOVE);
                    FlagClear(FLAG_MET_RIVAL_LILYCOVE);
                    LoadHeadlessMap(MAP_LILYCOVE_CITY, 27, 8);
                }
                else if (gEcHeadlessFixtureParam <= 139)
                {
                    FlagClear(FLAG_HIDE_LILYCOVE_HARBOR_FERRY_ATTENDANT);
                    FlagClear(FLAG_HIDE_LILYCOVE_HARBOR_SSTIDAL);
                    FlagSet(FLAG_HIDE_LILYCOVE_HARBOR_FERRY_SAILOR);
                    FlagSet(FLAG_HIDE_LILYCOVE_HARBOR_EVENT_TICKET_TAKER);
                    FlagClear(FLAG_RECEIVED_SS_TICKET);
                    FlagClear(FLAG_LATIOS_OR_LATIAS_ROAMING);
                    if (gEcHeadlessFixtureParam == 136)
                        FillHeadlessKeyPocket();
                    if (gEcHeadlessFixtureParam == 137 || gEcHeadlessFixtureParam == 138)
                    {
                        FlagSet(FLAG_RECEIVED_SS_TICKET);
                        AddBagItem(ITEM_OLD_SEA_MAP, 1);
                        FlagSet(FLAG_ENABLE_SHIP_FARAWAY_ISLAND);
                        FlagClear(FLAG_SHOWN_OLD_SEA_MAP);
                        LoadHeadlessMap(MAP_LILYCOVE_CITY_HARBOR,
                            gEcHeadlessFixtureParam == 137 ? 8 : 7,
                            gEcHeadlessFixtureParam == 137 ? 11 : 10);
                    }
                    else
                    {
                        if (gEcHeadlessFixtureParam == 139)
                            FlagClear(FLAG_BADGE06_GET);
                        LoadHeadlessMap(MAP_LILYCOVE_CITY_HARBOR, 3, 14);
                    }
                }
                else
                {
                    FlagSet(FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE);
                    LoadHeadlessMap(MAP_LILYCOVE_CITY_COVE_LILY_MOTEL_1F, 9, 3);
                }
                break;
            }
            if (gEcHeadlessFixtureParam >= 125 && gEcHeadlessFixtureParam <= 128)
            {
                SetMoney(&gSaveBlock1Ptr->money, gEcHeadlessFixtureParam == 127 ? 499 : 3000);
                if (gEcHeadlessFixtureParam == 128)
                {
                    for (slot = 2; slot < PARTY_SIZE; slot++)
                        CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][slot], SPECIES_ZIGZAGOON, 20, OTID_STRUCT_PLAYER_ID);
                    CalculatePlayerPartyCount();
                    CreateBoxMon(&gPokemonStoragePtr->boxes[0][0], SPECIES_ZIGZAGOON, 20, 0, OTID_STRUCT_PLAYER_ID);
                    for (slot = 1; slot < TOTAL_BOXES_COUNT * IN_BOX_COUNT; slot++)
                        gPokemonStoragePtr->boxes[slot / IN_BOX_COUNT][slot % IN_BOX_COUNT] = gPokemonStoragePtr->boxes[0][0];
                }
                ResetSafariZoneFlag();
                VarSet(VAR_SAFARI_ZONE_STATE, 0);
                LoadHeadlessMap(MAP_ROUTE121_SAFARI_ZONE_ENTRANCE, 9, 4);
                break;
            }
            if (gEcHeadlessFixtureParam >= 115 && gEcHeadlessFixtureParam <= 124)
            {
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_BADGE02_GET);
                FlagSet(FLAG_BADGE03_GET);
                FlagSet(FLAG_BADGE04_GET);
                FlagSet(FLAG_BADGE05_GET);
                FlagSet(FLAG_BADGE06_GET);
                FlagSet(FLAG_RECEIVED_HM_FLASH);
                FlagSet(FLAG_RECEIVED_HM_SURF);
                FlagSet(FLAG_RECEIVED_HM_STRENGTH);
                if (gEcHeadlessFixtureParam <= 116)
                {
                    FlagSet(FLAG_EC_CAUGHT_HEATRAN);
                    AddBagItem(ITEM_MAGMA_STONE, 1);
                    LoadHeadlessMap(MAP_SCORCHED_SLAB_HEATRANS_ROOM, 10, 15);
                }
                else if (gEcHeadlessFixtureParam <= 119)
                {
                    FlagSet(FLAG_EC_CAUGHT_HEATRAN);
                    if (gEcHeadlessFixtureParam >= 118)
                    {
                        MarkLegendarySignCaughtBySpecies(SPECIES_RESHIRAM);
                        FlagClear(FLAG_EC_CAUGHT_HEATRAN);
                    }
                    if (gEcHeadlessFixtureParam == 119)
                    {
                        GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_HEATRAN), FLAG_SET_CAUGHT);
                        FlagSet(FLAG_EC_CAUGHT_HEATRAN);
                    }
                    LoadHeadlessMap(MAP_SCORCHED_SLAB_B2F, 19, 17);
                }
                else if (gEcHeadlessFixtureParam == 120)
                {
                    SetTrainerFlag(TRAINER_LEONEL);
                    MarkLegendarySignCaughtBySpecies(SPECIES_OGERPON);
                    LoadHeadlessMap(MAP_ROUTE120, 14, 35);
                }
                else if (gEcHeadlessFixtureParam <= 122)
                {
                    FlagSet(FLAG_WINGULL_SENT_ON_ERRAND);
                    FlagSet(FLAG_WINGULL_DELIVERED_MAIL);
                    FlagClear(FLAG_HIDE_FORTREE_CITY_HOUSE_4_WINGULL);
                    FlagClear(FLAG_RECEIVED_FORTREE_SACHET);
                    if (gEcHeadlessFixtureParam == 122)
                        FlagSet(FLAG_RECEIVED_FORTREE_SACHET);
                    LoadHeadlessMap(MAP_FORTREE_CITY_HOUSE4, 1, 4);
                }
                else
                {
                    VarSet(VAR_ROUTE121_STATE, 0);
                    FlagClear(FLAG_HIDE_ROUTE_121_TEAM_AQUA_GRUNTS);
                    LoadHeadlessMap(MAP_ROUTE121, 24, gEcHeadlessFixtureParam == 123 ? 5 : 8);
                }
                break;
            }
            if (gEcHeadlessFixtureParam >= 101 && gEcHeadlessFixtureParam <= 114)
            {
                if (gEcHeadlessFixtureParam <= 106)
                {
                    u16 total = gEcHeadlessFixtureParam == 101 ? 99 :
                        (gEcHeadlessFixtureParam == 102 || gEcHeadlessFixtureParam == 105) ? 100 :
                        gEcHeadlessFixtureParam == 103 ? 250 : 500;
                    VarSet(VAR_EC_SOOT_PROGRESS, total);
                    FlagClear(FLAG_ITEM_FIERY_PATH_HOUNDOOMINITE);
                    AddBagItem(ITEM_SOOT_SACK, 1);
                    if (gEcHeadlessFixtureParam >= 105)
                    {
                        FillHeadlessKeyPocket();
                        if (gEcHeadlessFixtureParam == 106)
                        {
                            struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_HOUNDOOMINITE)];
                            for (slot = 0; slot < pocket->capacity; slot++)
                                BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_LEFTOVERS, 1);
                            for (slot = 0; slot < PC_ITEMS_COUNT; slot++)
                                gSaveBlock1Ptr->pcItems[slot] = (struct ItemSlot){ITEM_POTION, 1};
                        }
                    }
                    if (!CheckBagHasItem(ITEM_SOOT_SACK, 1))
                        BagPocket_SetSlotItemIdAndCount(&gBagPockets[GetItemPocket(ITEM_SOOT_SACK)], 0, ITEM_SOOT_SACK, 1);
                    LoadHeadlessMap(MAP_ROUTE113_GLASS_WORKSHOP, 2, 4);
                }
                else if (gEcHeadlessFixtureParam <= 108)
                {
                    SetMoney(&gSaveBlock1Ptr->money, 50000);
                    LoadHeadlessMap(MAP_LILYCOVE_CITY_DEPARTMENT_STORE_4F,
                        gEcHeadlessFixtureParam == 107 ? 7 : 9, 4);
                }
                else if (gEcHeadlessFixtureParam <= 111)
                {
                    FlagClear(FLAG_ITEM_ABANDONED_SHIP_ROOMS_B1F_GLALITITE);
                    if (gEcHeadlessFixtureParam == 110)
                        FlagSet(FLAG_ITEM_ABANDONED_SHIP_ROOMS_B1F_GLALITITE);
                    AddBagItem(ITEM_SHOAL_SALT, 4);
                    AddBagItem(ITEM_SHOAL_SHELL, 4);
                    if (gEcHeadlessFixtureParam == 111)
                    {
                        struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_GLALITITE)];
                        // Mega Stones have their own pocket; salt/shells are
                        // in Items and do not occupy its first two slots.
                        for (slot = 0; slot < pocket->capacity; slot++)
                            BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_ABOMASITE, 1);
                        for (slot = 0; slot < PC_ITEMS_COUNT; slot++)
                            gSaveBlock1Ptr->pcItems[slot] = (struct ItemSlot){ITEM_POTION, 1};
                    }
                    LoadHeadlessMap(MAP_SHOAL_CAVE_LOW_TIDE_ENTRANCE_ROOM, 17, 15);
                }
                else if (gEcHeadlessFixtureParam == 112)
                {
                    u8 bonuses = 0xE4;
                    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP_BONUSES, &bonuses);
                    ApplyEmeraldChampionsBattleSetChoice(&gParties[B_TRAINER_PLAYER][0], 0);
                    LoadHeadlessMap(MAP_ROUTE113_GLASS_WORKSHOP, 2, 4);
                }
                else
                {
                    gSaveBlock2Ptr->frontier.battlePoints = 100;
                    if (gEcHeadlessFixtureParam == 114)
                        AddPCItem(ITEM_LINKING_CORD, 1);
                    LoadHeadlessMap(MAP_BATTLE_FRONTIER_EXCHANGE_SERVICE_CORNER, 12, 4);
                }
                break;
            }
            if (gEcHeadlessFixtureParam >= 92 && gEcHeadlessFixtureParam <= 100)
            {
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_BADGE02_GET);
                FlagSet(FLAG_BADGE03_GET);
                FlagSet(FLAG_BADGE04_GET);
                FlagSet(FLAG_BADGE05_GET);
                FlagClear(FLAG_BADGE06_GET);
                FlagSet(FLAG_RECEIVED_HM_SURF);
                FlagSet(FLAG_RECEIVED_HM_FLY);
                FlagClear(FLAG_RECEIVED_DEVON_SCOPE);
                FlagClear(FLAG_HIDE_ROUTE_120_STEVEN);
                FlagClear(FLAG_HIDE_ROUTE_120_KECLEON_BRIDGE);
                FlagClear(FLAG_HIDE_ROUTE_120_KECLEON_BRIDGE_SHADOW);
                FlagClear(FLAG_NOT_READY_FOR_BATTLE_ROUTE_120);
                if (gEcHeadlessFixtureParam <= 95)
                {
                    if (gEcHeadlessFixtureParam >= 94)
                    {
                        FillHeadlessKeyPocket();
                        for (slot = 0; slot < PC_ITEMS_COUNT; slot++)
                            gSaveBlock1Ptr->pcItems[slot] = (struct ItemSlot){ITEM_POTION, 1};
                    }
                    LoadHeadlessMap(MAP_ROUTE120, (gEcHeadlessFixtureParam & 1) ? 14 : 13,
                        (gEcHeadlessFixtureParam & 1) ? 15 : 16);
                }
                else if (gEcHeadlessFixtureParam <= 97)
                {
                    const u16 trainers[] = {TRAINER_JARED, TRAINER_EDWARDO, TRAINER_FLINT,
                        TRAINER_ASHLEY, TRAINER_HUMBERTO, TRAINER_DARIUS};
                    for (slot = 0; slot < ARRAY_COUNT(trainers); slot++)
                        SetTrainerFlag(trainers[slot]);
                    ClearTrainerFlag(TRAINER_WINONA_1);
                    FlagClear(FLAG_DEFEATED_FORTREE_GYM);
                    FlagClear(FLAG_RECEIVED_WINONA_ALTARIANITE);
                    FlagClear(FLAG_RECEIVED_RED_OR_BLUE_ORB);
                    LoadHeadlessMap(MAP_FORTREE_CITY_GYM, gEcHeadlessFixtureParam == 96 ? 15 : 16,
                        gEcHeadlessFixtureParam == 96 ? 3 : 23);
                }
                else if (gEcHeadlessFixtureParam == 98)
                {
                    FlagSet(FLAG_RECEIVED_DEVON_SCOPE);
                    FlagClear(FLAG_KECLEON_FLED_FORTREE);
                    AddBagItem(ITEM_DEVON_SCOPE, 1);
                    LoadHeadlessMap(MAP_FORTREE_CITY, 25, 9);
                }
                else if (gEcHeadlessFixtureParam == 99)
                {
                    FlagClear(FLAG_WINGULL_SENT_ON_ERRAND);
                    FlagClear(FLAG_WINGULL_DELIVERED_MAIL);
                    FlagClear(FLAG_RECEIVED_FORTREE_SACHET);
                    FlagClear(FLAG_HIDE_FORTREE_CITY_HOUSE_4_WINGULL);
                    LoadHeadlessMap(MAP_FORTREE_CITY_HOUSE4, 1, 4);
                }
                else
                {
                    FlagSet(FLAG_BADGE06_GET);
                    FlagSet(FLAG_RECEIVED_DEVON_SCOPE);
                    LoadHeadlessMap(MAP_ROUTE120, 14, 15);
                }
                break;
            }
            if (gEcHeadlessFixtureParam == 91)
            {
                VarSet(VAR_WEATHER_INSTITUTE_STATE, 2);
                VarSet(VAR_ABNORMAL_WEATHER_LOCATION, 0);
                FlagSet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
                FlagSet(FLAG_RECEIVED_CASTFORM);
                FlagSet(FLAG_HIDE_ROUTE_119_TEAM_AQUA);
                FlagSet(FLAG_HIDE_WEATHER_INSTITUTE_2F_WORKERS);
                FlagSet(FLAG_HIDE_WEATHER_INSTITUTE_2F_AQUA_GRUNT_M);
                FlagClear(FLAG_DEFEATED_KYOGRE);
                FlagClear(FLAG_DEFEATED_GROUDON);
                AddBagItem(ITEM_REVEAL_GLASS, 1);
                LoadHeadlessMap(MAP_ROUTE119_WEATHER_INSTITUTE_2F, 2, 3);
                break;
            }
            if (gEcHeadlessFixtureParam >= 82 && gEcHeadlessFixtureParam <= 90)
            {
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_BADGE02_GET);
                FlagSet(FLAG_BADGE03_GET);
                FlagSet(FLAG_BADGE04_GET);
                FlagSet(FLAG_BADGE05_GET);
                FlagSet(FLAG_RECEIVED_HM_SURF);
                FlagClear(FLAG_RECEIVED_HM_FLY);
                FlagSet(FLAG_EC_REPEL_SPRAY_ACTIVE);
                VarSet(VAR_EC_REPEL_SPRAY_STEPS, 500);
                if (gEcHeadlessFixtureParam <= 84)
                {
                    const u16 trainers[] = {TRAINER_GRUNT_WEATHER_INST_1, TRAINER_GRUNT_WEATHER_INST_2,
                        TRAINER_GRUNT_WEATHER_INST_3, TRAINER_GRUNT_WEATHER_INST_4,
                        TRAINER_GRUNT_WEATHER_INST_5, TRAINER_SHELLY_WEATHER_INSTITUTE};
                    for (slot = 0; slot < ARRAY_COUNT(trainers); slot++)
                        ClearTrainerFlag(trainers[slot]);
                    VarSet(VAR_WEATHER_INSTITUTE_STATE, 0);
                    FlagClear(FLAG_RECEIVED_CASTFORM);
                    FlagClear(FLAG_HIDE_ROUTE_119_TEAM_AQUA);
                    FlagClear(FLAG_HIDE_WEATHER_INSTITUTE_2F_WORKERS);
                    FlagSet(FLAG_HIDE_WEATHER_INSTITUTE_1F_WORKERS);
                    FlagSet(FLAG_HIDE_WEATHER_INSTITUTE_2F_AQUA_GRUNT_M);
                    if (gEcHeadlessFixtureParam == 83)
                    {
                        for (slot = 2; slot < PARTY_SIZE; slot++)
                            CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][slot], SPECIES_ZIGZAGOON, 20, OTID_STRUCT_PLAYER_ID);
                        CalculatePlayerPartyCount();
                    }
                    if (gEcHeadlessFixtureParam == 84)
                        LoadHeadlessMap(MAP_ROUTE119_WEATHER_INSTITUTE_1F, 5, 11);
                    else
                        LoadHeadlessMap(MAP_ROUTE119_WEATHER_INSTITUTE_2F, 5, 6);
                }
                else
                {
                    gSaveBlock2Ptr->playerGender = (gEcHeadlessFixtureParam & 1) ? MALE : FEMALE;
                    VarSet(VAR_STARTER_GEN, 1);
                    VarSet(VAR_STARTER_MON, 1);
                    VarSet(VAR_WEATHER_INSTITUTE_STATE, 2);
                    VarSet(VAR_ROUTE119_STATE, 0);
                    FlagSet(FLAG_HIDE_ROUTE_119_TEAM_AQUA);
                    FlagSet(FLAG_HIDE_ROUTE_119_RIVAL);
                    FlagSet(FLAG_HIDE_ROUTE_119_RIVAL_ON_BIKE);
                    FlagSet(FLAG_HIDE_ROUTE_119_SCOTT);
                    if (gEcHeadlessFixtureParam >= 89)
                    {
                        // Full storage must not affect direct field-license registration.
                        FillHeadlessKeyPocket();
                        for (slot = 0; slot < PC_ITEMS_COUNT; slot++)
                            gSaveBlock1Ptr->pcItems[slot] = (struct ItemSlot){ITEM_POTION, 1};
                    }
                    LoadHeadlessMap(MAP_ROUTE119,
                        gEcHeadlessFixtureParam == 87 || gEcHeadlessFixtureParam == 88 || gEcHeadlessFixtureParam == 90 ? 26 : 25, 32);
                }
                break;
            }
            if (gEcHeadlessFixtureParam >= 77 && gEcHeadlessFixtureParam <= 81)
            {
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_BADGE02_GET);
                FlagSet(FLAG_BADGE03_GET);
                FlagSet(FLAG_BADGE04_GET);
                FlagSet(FLAG_BADGE05_GET);
                FlagSet(FLAG_RECEIVED_HM_SURF);
                FlagSet(FLAG_EC_REPEL_SPRAY_ACTIVE);
                VarSet(VAR_EC_REPEL_SPRAY_STEPS, 500);
                if (gEcHeadlessFixtureParam == 77)
                {
                    // Traversal assumes the removable objects have been cleared.
                    FlagSet(FLAG_ITEM_NEW_MAUVILLE_ESCAPE_ROPE);
                    FlagSet(FLAG_ITEM_NEW_MAUVILLE_ROTOM_CATALOG);
                    FlagSet(FLAG_ITEM_NEW_MAUVILLE_UPGRADE);
                    FlagSet(FLAG_DEFEATED_VOLTORB_1_NEW_MAUVILLE);
                    FlagSet(FLAG_DEFEATED_VOLTORB_2_NEW_MAUVILLE);
                    FlagSet(FLAG_DEFEATED_VOLTORB_3_NEW_MAUVILLE);
                    FlagSet(FLAG_HIDE_NEW_MAUVILLE_VOLTORB_1);
                    FlagSet(FLAG_HIDE_NEW_MAUVILLE_VOLTORB_2);
                    FlagSet(FLAG_HIDE_NEW_MAUVILLE_VOLTORB_3);
                    AddBagItem(ITEM_BASEMENT_KEY, 1);
                    AddBagItem(ITEM_ROTOM_CATALOG, 1);
                    VarSet(VAR_NEW_MAUVILLE_STATE, 0);
                    LoadHeadlessMap(MAP_NEW_MAUVILLE_ENTRANCE, 4, 3);
                }
                else if (gEcHeadlessFixtureParam == 78)
                {
                    VarSet(VAR_NEW_MAUVILLE_STATE, 1);
                    LoadHeadlessMap(MAP_NEW_MAUVILLE_INSIDE, 32, 6);
                }
                else
                {
                    VarSet(VAR_ROUTE118_STATE, 0);
                    FlagClear(FLAG_HIDE_ROUTE_118_STEVEN);
                    SetTrainerFlag(TRAINER_DALTON_1);
                    LoadHeadlessMap(MAP_ROUTE118,
                        gEcHeadlessFixtureParam == 79 ? 17 : gEcHeadlessFixtureParam == 80 ? 43 : 45,
                        gEcHeadlessFixtureParam == 79 ? 9 : 12);
                }
                break;
            }
            if (gEcHeadlessFixtureParam == 62 || gEcHeadlessFixtureParam == 63)
            {
                FlagClear(FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY);
                FlagClear(FLAG_HIDE_MT_CHIMNEY_TEAM_MAGMA);
                FlagClear(FLAG_HIDE_MT_CHIMNEY_TEAM_AQUA);
                FlagSet(FLAG_HIDE_MT_CHIMNEY_TRAINERS);
                FlagSet(FLAG_HIDE_MT_CHIMNEY_LAVA_COOKIE_LADY);
                ClearTrainerFlag(TRAINER_MAXIE_MT_CHIMNEY);
                LoadHeadlessMap(MAP_MT_CHIMNEY, gEcHeadlessFixtureParam == 62 ? 12 : 13,
                    gEcHeadlessFixtureParam == 62 ? 6 : 7);
                break;
            }
            if (gEcHeadlessFixtureParam == 48)
            {
                ClearTrainerFlag(TRAINER_VICTOR);
                ClearTrainerFlag(TRAINER_VICTORIA);
                ClearTrainerFlag(TRAINER_VIVI);
                ClearTrainerFlag(TRAINER_VICKY);
                LoadHeadlessMap(MAP_ROUTE111, 13, 115);
                break;
            }
            if (gEcHeadlessFixtureParam >= 49 && gEcHeadlessFixtureParam <= 59)
            {
                AddBagItem(ITEM_HEAL_BALL, 1);
                switch (gEcHeadlessFixtureParam)
                {
                case 49:
                    VarSet(VAR_CHANSEY_NURSE_STATE, CHANSEY_NURSE_NEEDS_HELP);
                    LoadHeadlessMap(MAP_ROUTE111, 19, 103);
                    break;
                case 50:
                    VarSet(VAR_CHANSEY_NURSE_STATE, CHANSEY_NURSE_BLOB_ON_ROUTE112);
                    LoadHeadlessMap(MAP_ROUTE112, 25, 32);
                    break;
                case 51:
                case 54:
                    VarSet(VAR_CHANSEY_NURSE_STATE, CHANSEY_NURSE_BLOB_ON_JAGGED_PASS);
                    LoadHeadlessMap(MAP_JAGGED_PASS, gEcHeadlessFixtureParam == 51 ? 11 : 12,
                        gEcHeadlessFixtureParam == 51 ? 29 : 28);
                    break;
                case 52:
                    VarSet(VAR_CHANSEY_NURSE_STATE, CHANSEY_NURSE_BLOB_IN_ASHEN_WOODS);
                    LoadHeadlessMap(MAP_ASHEN_WOODS, 14, 30);
                    break;
                case 53:
                    VarSet(VAR_CHANSEY_NURSE_STATE, CHANSEY_NURSE_BLOB_ASHEN_WOODS_WEST);
                    LoadHeadlessMap(MAP_ASHEN_WOODS, 7, 39);
                    break;
                case 55:
                    VarSet(VAR_CHANSEY_NURSE_STATE, CHANSEY_NURSE_BLOB_IN_ASHEN_WOODS);
                    LoadHeadlessMap(MAP_ASHEN_WOODS, 17, 29);
                    break;
                case 56:
                    VarSet(VAR_CHANSEY_NURSE_STATE, CHANSEY_NURSE_BLOB_ASHEN_WOODS_WEST);
                    LoadHeadlessMap(MAP_ASHEN_WOODS, 6, 35);
                    break;
                case 57:
                case 58:
                    VarSet(VAR_CHANSEY_NURSE_STATE, CHANSEY_NURSE_BLOB_ASHEN_WOODS_EAST);
                    if (gEcHeadlessFixtureParam == 58)
                        RemoveBagItem(ITEM_HEAL_BALL, 1);
                    LoadHeadlessMap(MAP_ASHEN_WOODS, 27, 44);
                    break;
                case 59:
                    VarSet(VAR_CHANSEY_NURSE_STATE, CHANSEY_NURSE_BLOB_CAUGHT);
                    VarSet(VAR_POKE_VIAL_MAX_CHARGES, POKE_VIAL_CAPACITY_BASE);
                    VarSet(VAR_POKE_VIAL_CHARGES, 0);
                    LoadHeadlessMap(MAP_ROUTE111, 19, 102);
                    break;
                }
                break;
            }
            if (gEcHeadlessFixtureParam == 46 || gEcHeadlessFixtureParam == 47)
            {
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_RECEIVED_HM_CUT);
                VarSet(VAR_TRICK_HOUSE_LEVEL, gEcHeadlessFixtureParam - 46);
                VarSet(VAR_TRICK_HOUSE_PUZZLE_1_STATE, 0);
                VarSet(VAR_TRICK_HOUSE_PUZZLE_2_STATE, 0);
                SetTrainerFlag(TRAINER_SALLY);
                SetTrainerFlag(TRAINER_EDDIE);
                SetTrainerFlag(TRAINER_ROBIN);
                SetTrainerFlag(TRAINER_TED);
                SetTrainerFlag(TRAINER_PAUL);
                SetTrainerFlag(TRAINER_GEORGIA);
                LoadHeadlessMap(gEcHeadlessFixtureParam == 46
                    ? MAP_ROUTE110_TRICK_HOUSE_PUZZLE1 : MAP_ROUTE110_TRICK_HOUSE_PUZZLE2, 0, 20);
                break;
            }
            if (gEcHeadlessFixtureParam == 43)
            {
                FlagClear(FLAG_DEFEATED_MAUVILLE_GYM);
                FlagClear(FLAG_BADGE03_GET);
                FlagClear(FLAG_MAUVILLE_GYM_BARRIERS_STATE);
                VarSet(VAR_MAUVILLE_GYM_STATE, 0);
                SetTrainerFlag(TRAINER_KIRK);
                SetTrainerFlag(TRAINER_SHAWN);
                SetTrainerFlag(TRAINER_BEN);
                SetTrainerFlag(TRAINER_VIVIAN);
                SetTrainerFlag(TRAINER_ANGELO);
                LoadHeadlessMap(MAP_MAUVILLE_CITY_GYM, 4, 19);
                break;
            }
            if (gEcHeadlessFixtureParam == 44 || gEcHeadlessFixtureParam == 45)
            {
                FlagSet(FLAG_BADGE03_GET);
                FlagSet(FLAG_RECEIVED_HM_ROCK_SMASH);
                FlagClear(FLAG_RUSTURF_TUNNEL_OPENED);
                FlagClear(FLAG_RECEIVED_HM_STRENGTH);
                FlagClear(FLAG_HIDE_RUSTURF_TUNNEL_ROCK_1);
                FlagClear(FLAG_HIDE_RUSTURF_TUNNEL_ROCK_2);
                FlagClear(FLAG_HIDE_RUSTURF_TUNNEL_WANDA);
                FlagClear(FLAG_HIDE_RUSTURF_TUNNEL_WANDAS_BOYFRIEND);
                VarSet(VAR_RUSTURF_TUNNEL_STATE, 3);
                LoadHeadlessMap(MAP_RUSTURF_TUNNEL,
                    gEcHeadlessFixtureParam == 44 ? 22 : 26,
                    gEcHeadlessFixtureParam == 44 ? 4 : 5);
                break;
            }
            if (gEcHeadlessFixtureParam >= 40 && gEcHeadlessFixtureParam <= 42)
            {
                FlagClear(FLAG_HIDE_FALLARBOR_POKEMON_CENTER_LANETTE);
                FlagSet(FLAG_HIDE_LANETTES_HOUSE_LANETTE);
                LoadHeadlessMap(MAP_FALLARBOR_TOWN_POKEMON_CENTER_1F,
                    gEcHeadlessFixtureParam == 40 ? 11 : gEcHeadlessFixtureParam == 41 ? 12 : 10,
                    gEcHeadlessFixtureParam == 40 ? 5 : 4);
                break;
            }
            if (gEcHeadlessFixtureParam == 38 || gEcHeadlessFixtureParam == 39)
            {
                gSaveBlock2Ptr->playerGender = gEcHeadlessFixtureParam == 38 ? MALE : FEMALE;
                VarSet(VAR_FALLARBOR_TOWN_STATE, 0);
                VarSet(VAR_METEOR_FALLS_STATE, 0);
                FlagClear(FLAG_HIDE_FALLARBOR_RIVAL);
                LoadHeadlessMap(MAP_FALLARBOR_TOWN, 14, gEcHeadlessFixtureParam == 38 ? 10 : 8);
                break;
            }
            if (gEcHeadlessFixtureParam == 37)
            {
                memset(&gSaveBlock1Ptr->daycare, 0, sizeof(gSaveBlock1Ptr->daycare));
                CreateBoxMon(&gSaveBlock1Ptr->daycare.mons[0].mon, SPECIES_MANAPHY, 20, 0, OTID_STRUCT_PLAYER_ID);
                CreateBoxMon(&gSaveBlock1Ptr->daycare.mons[1].mon, SPECIES_DITTO, 20, 0, OTID_STRUCT_PLAYER_ID);
                LoadHeadlessMap(MAP_ROUTE117, 47, 6);
                break;
            }
            if (gEcHeadlessFixtureParam == 35 || gEcHeadlessFixtureParam == 36)
            {
                memset(&gSaveBlock1Ptr->daycare, 0, sizeof(gSaveBlock1Ptr->daycare));
                CreateBoxMon(&gSaveBlock1Ptr->daycare.mons[0].mon, SPECIES_DITTO,
                    gEcHeadlessFixtureParam == 35 ? 10 : 20, 0, OTID_STRUCT_PLAYER_ID);
                gSaveBlock1Ptr->daycare.mons[0].steps = 100000;
                gSpecialVar_0x8004 = 0;
                gSpecialVar_0x8006 = GetNumLevelsGainedFromDaycare();
                GetDaycareCostAndPrepareString();
                LoadHeadlessMap(MAP_ROUTE117_POKEMON_DAY_CARE, 2, 3);
                break;
            }
            if (gEcHeadlessFixtureParam == 34)
            {
                memset(&gSaveBlock1Ptr->daycare, 0, sizeof(gSaveBlock1Ptr->daycare));
                CreateBoxMon(&gPokemonStoragePtr->boxes[0][0], SPECIES_DITTO, 20, 0, OTID_STRUCT_PLAYER_ID);
                LoadHeadlessMap(MAP_ROUTE117_POKEMON_DAY_CARE, 2, 3);
                break;
            }
            if (gEcHeadlessFixtureParam == 32 || gEcHeadlessFixtureParam == 33)
            {
                memset(gSaveBlock2Ptr->playerTrainerId, 0, TRAINER_ID_LENGTH);
                gSaveBlock2Ptr->playerTrainerId[0] = gEcHeadlessFixtureParam == 32 ? 6 : 8;
                SetMauvilleOldMan();
                SetGameStat(GAME_STAT_SAVED_GAME, 100);
                LoadHeadlessMap(MAP_MAUVILLE_CITY_POKEMON_CENTER_1F, 4, 4);
                break;
            }
            if (gEcHeadlessFixtureParam >= 29 && gEcHeadlessFixtureParam <= 31)
            {
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_BADGE02_GET);
                FlagSet(FLAG_ADVENTURE_STARTED);
                FlagSet(FLAG_RECEIVED_POKENAV);
                FlagSet(FLAG_SYS_POKENAV_GET);
                FlagSet(FLAG_DELIVERED_DEVON_GOODS);
                if (gEcHeadlessFixtureParam == 29)
                {
                    FlagClear(FLAG_HIDE_MAUVILLE_CITY_WALLY);
                    FlagClear(FLAG_HIDE_MAUVILLE_CITY_WALLYS_UNCLE);
                    LoadHeadlessMap(MAP_MAUVILLE_CITY, 8, 7);
                }
                else if (gEcHeadlessFixtureParam == 30)
                {
                    FlagClear(FLAG_HIDE_MAUVILLE_GYM_WATTSON);
                    LoadHeadlessMap(MAP_MAUVILLE_CITY_GYM, 5, 3);
                }
                else
                {
                    VarSet(VAR_REGISTER_BIRCH_STATE, 1);
                    LoadHeadlessMap(MAP_ROUTE110, 9, 86);
                }
                break;
            }
            if (gEcHeadlessFixtureParam == 28)
            {
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_ADVENTURE_STARTED);
                FlagSet(FLAG_RECEIVED_POKENAV);
                FlagSet(FLAG_SYS_POKENAV_GET);
                FlagSet(FLAG_DELIVERED_DEVON_GOODS);
                FlagSet(FLAG_HIDE_SLATEPORT_CITY_TEAM_AQUA);
                VarSet(VAR_SLATEPORT_OUTSIDE_MUSEUM_STATE, 1);
                LoadHeadlessMap(MAP_SLATEPORT_CITY, 30, 27);
                break;
            }
            if (gEcHeadlessFixtureParam == 27)
            {
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_ADVENTURE_STARTED);
                FlagSet(FLAG_DELIVERED_STEVEN_LETTER);
                FlagSet(FLAG_RECEIVED_POKENAV);
                FlagClear(FLAG_DELIVERED_DEVON_GOODS);
                FlagClear(FLAG_HIDE_SLATEPORT_CITY_OCEANIC_MUSEUM_2F_CAPTAIN_STERN);
                FlagSet(FLAG_HIDE_SLATEPORT_CITY_OCEANIC_MUSEUM_2F_AQUA_GRUNT_1);
                FlagSet(FLAG_HIDE_SLATEPORT_CITY_OCEANIC_MUSEUM_2F_AQUA_GRUNT_2);
                FlagSet(FLAG_HIDE_SLATEPORT_CITY_OCEANIC_MUSEUM_2F_ARCHIE);
                AddBagItem(ITEM_DEVON_PARTS, 1);
                LoadHeadlessMap(MAP_SLATEPORT_CITY_OCEANIC_MUSEUM_2F, 12, 6);
                break;
            }
            if (gEcHeadlessFixtureParam == 25 || gEcHeadlessFixtureParam == 26)
            {
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_ADVENTURE_STARTED);
                FlagSet(FLAG_RETURNED_DEVON_GOODS);
                if (gEcHeadlessFixtureParam == 25)
                {
                    VarSet(VAR_DEVON_CORP_3F_STATE, 0);
                    FlagClear(FLAG_RECEIVED_POKENAV);
                    AddBagItem(ITEM_DEVON_PARTS, 1);
                    LoadHeadlessMap(MAP_RUSTBORO_CITY_DEVON_CORP_3F, 2, 2);
                }
                else
                {
                    VarSet(VAR_FOSSIL_RESURRECTION_STATE, 1);
                    VarSet(VAR_WHICH_FOSSIL_REVIVED, ITEM_OLD_AMBER); // Old Amber already handed in (the var holds an item id).
                    LoadHeadlessMap(MAP_RUSTBORO_CITY_DEVON_CORP_2F, 14, 9);
                }
                break;
            }
            // Rescue/reward/sailing handoffs use native map scripts and movement.
            if (gEcHeadlessFixtureParam >= 22 && gEcHeadlessFixtureParam <= 24)
            {
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_ADVENTURE_STARTED);
                FlagSet(FLAG_RECEIVED_POKENAV);
                FlagSet(FLAG_SYS_POKENAV_GET);
                if (gEcHeadlessFixtureParam == 22)
                {
                    FlagClear(FLAG_HIDE_RUSTURF_TUNNEL_AQUA_GRUNT);
                    FlagClear(FLAG_HIDE_RUSTURF_TUNNEL_PEEKO);
                    FlagSet(FLAG_HIDE_RUSTURF_TUNNEL_BRINEY);
                    FlagSet(FLAG_DEVON_GOODS_STOLEN);
                    VarSet(VAR_RUSTURF_TUNNEL_STATE, 3);
                    LoadHeadlessMap(MAP_RUSTURF_TUNNEL, 13, 5);
                }
                else if (gEcHeadlessFixtureParam == 23)
                {
                    FlagClear(FLAG_BADGE02_GET);
                    FlagClear(FLAG_HIDE_GRANITE_CAVE_STEVEN);
                    FlagSet(FLAG_HIDE_SLATEPORT_CITY_BRAWLY);
                    VarSet(VAR_PETALBURG_GYM_STATE, 3);
                    LoadHeadlessMap(MAP_DEWFORD_TOWN_GYM, 4, 4);
                }
                else
                {
                    FlagClear(FLAG_HIDE_BRINEYS_HOUSE_MR_BRINEY);
                    FlagClear(FLAG_HIDE_BRINEYS_HOUSE_PEEKO);
                    FlagClear(FLAG_MR_BRINEY_SAILING_INTRO);
                    FlagClear(FLAG_DELIVERED_STEVEN_LETTER);
                    VarSet(VAR_BRINEY_HOUSE_STATE, 0);
                    VarSet(VAR_BRINEY_LOCATION, 1);
                    AddBagItem(ITEM_LETTER, 1);
                    AddBagItem(ITEM_DEVON_PARTS, 1);
                    LoadHeadlessMap(MAP_ROUTE104_MR_BRINEYS_HOUSE, 5, 4);
                }
                break;
            }
            // Scoped dialogue checks for completed/available local discoveries.
            if (gEcHeadlessFixtureParam >= 17 && gEcHeadlessFixtureParam <= 21)
            {
                FlagSet(FLAG_BADGE01_GET);
                FlagSet(FLAG_BADGE02_GET);
                if (gEcHeadlessFixtureParam <= 18)
                {
                    FlagClear(FLAG_HIDE_GRANITE_CAVE_STEVEN);
                    FlagSet(FLAG_DELIVERED_STEVEN_LETTER);
                    FlagSet(FLAG_DELIVERED_DEVON_GOODS);
                    AddBagItem(ITEM_MEGA_RING, 1);
                    if (gEcHeadlessFixtureParam == 18)
                        MarkLegendarySignCaughtBySpecies(SPECIES_COBALION);
                    LoadHeadlessMap(MAP_GRANITE_CAVE_STEVENS_ROOM, 7, 9);
                }
                else
                {
                    if (gEcHeadlessFixtureParam == 20)
                        MarkLegendarySignCaughtBySpecies(SPECIES_MELOETTA);
                    else if (gEcHeadlessFixtureParam == 21)
                        SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_SING, 0);
                    LoadHeadlessMap(MAP_DEWFORD_MEADOW, 27, 13);
                }
                break;
            }
            // Opening chapter dialogue: Scott's four approach rows, Wally's
            // parents before/after his catch, and Norman's first meeting.
            if (gEcHeadlessFixtureParam >= 10 && gEcHeadlessFixtureParam <= 16)
            {
                StringCopy(gSaveBlock2Ptr->playerName, COMPOUND_STRING("WWWWWWW"));
                FlagSet(FLAG_ADVENTURE_STARTED);
                FlagSet(FLAG_RESCUED_BIRCH);
                FlagSet(FLAG_RECEIVED_POKEDEX_FROM_BIRCH);
                VarSet(VAR_PETALBURG_CITY_STATE, 3);
                VarSet(VAR_PETALBURG_GYM_STATE, 2);
                if (gEcHeadlessFixtureParam <= 13)
                {
                    VarSet(VAR_SCOTT_PETALBURG_ENCOUNTER, 0);
                    LoadHeadlessMap(MAP_PETALBURG_CITY, 5, gEcHeadlessFixtureParam);
                }
                else if (gEcHeadlessFixtureParam <= 15)
                {
                    VarSet(VAR_PETALBURG_GYM_STATE, gEcHeadlessFixtureParam == 14 ? 0 : 2);
                    LoadHeadlessMap(MAP_PETALBURG_CITY_WALLYS_HOUSE, 4, 4);
                }
                else
                {
                    VarSet(VAR_PETALBURG_CITY_STATE, 1);
                    VarSet(VAR_PETALBURG_GYM_STATE, 0);
                    LoadHeadlessMap(MAP_PETALBURG_CITY_GYM, 4, 108);
                }
                break;
            }
            // 4-6: opening send-off, Bag / PC / both full; 7-9: female branch.
            if (gEcHeadlessFixtureParam >= 4 && gEcHeadlessFixtureParam <= 9)
            {
                u32 storage = (gEcHeadlessFixtureParam - 4) % 3;
                gSaveBlock2Ptr->playerGender = gEcHeadlessFixtureParam >= 7 ? FEMALE : MALE;
                StringCopy(gSaveBlock2Ptr->playerName, COMPOUND_STRING("WWWWWWW"));
                FlagSet(FLAG_RESCUED_BIRCH);
                FlagSet(FLAG_DEFEATED_RIVAL_ROUTE103);
                FlagSet(FLAG_RECEIVED_POKEDEX_FROM_BIRCH);
                FlagSet(FLAG_SYS_POKEDEX_GET);
                FlagSet(FLAG_RECEIVED_DEXNAV);
                FlagClear(FLAG_ADVENTURE_STARTED);
                FlagClear(FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_BIRCH);
                FlagClear(FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_RIVAL);
                VarSet(VAR_EC_OPENING_STATE, EC_OPENING_RESCUE_WON);
                VarSet(VAR_BIRCH_LAB_STATE, 4);
                if (storage != 0)
                {
                    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_GREAT_BALL)];
                    for (slot = 0; slot < pocket->capacity; slot++)
                        BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_POKE_BALL, 1);
                }
                if (storage == 2)
                    for (slot = 0; slot < PC_ITEMS_COUNT; slot++)
                        gSaveBlock1Ptr->pcItems[slot] = (struct ItemSlot){ITEM_POTION, 1};
                LoadHeadlessMap(MAP_LITTLEROOT_TOWN_PROFESSOR_BIRCHS_LAB, 6, 12);
                break;
            }
            FlagClear(FLAG_HIDE_GRANITE_CAVE_STEVEN);
            FlagClear(FLAG_DELIVERED_STEVEN_LETTER);
            FlagClear(FLAG_BADGE02_GET);
            if (gEcHeadlessFixtureParam == 1)
                AddPCItem(ITEM_LETTER, 1);
            else if (gEcHeadlessFixtureParam == 2)
            {
                struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_AERODACTYLITE)];
                FlagSet(FLAG_DELIVERED_STEVEN_LETTER);
                FlagSet(FLAG_BADGE02_GET);
                VarSet(VAR_STARTER_GEN, 1);
                VarSet(VAR_STARTER_MON, 1);
                // Ring already delivered; the one Aerodactylite reward still needs space.
                FlagClear(FLAG_EC_RECEIVED_ROXANNE_AERODACTYLITE);
                AddBagItem(ITEM_MEGA_RING, 1);
                for (slot = 0; slot < pocket->capacity; slot++)
                    BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_VENUSAURITE, 1);
                for (slot = 0; slot < PC_ITEMS_COUNT; slot++)
                    gSaveBlock1Ptr->pcItems[slot] = (struct ItemSlot){ITEM_POTION, 1};
            }
            if (gEcHeadlessFixtureParam == 3)
            {
                FlagClear(FLAG_HIDE_SLATEPORT_CITY_OCEANIC_MUSEUM_2F_CAPTAIN_STERN);
                FlagSet(FLAG_HIDE_SLATEPORT_CITY_OCEANIC_MUSEUM_2F_AQUA_GRUNT_1);
                FlagSet(FLAG_HIDE_SLATEPORT_CITY_OCEANIC_MUSEUM_2F_AQUA_GRUNT_2);
                FlagSet(FLAG_HIDE_SLATEPORT_CITY_OCEANIC_MUSEUM_2F_ARCHIE);
                LoadHeadlessMap(MAP_SLATEPORT_CITY_OCEANIC_MUSEUM_2F, 12, 6);
            }
            else
                LoadHeadlessMap(MAP_GRANITE_CAVE_STEVENS_ROOM, 7, 9);
        }
        break;
    case EC_HEADLESS_SCENARIO_RUSTBORO_GUIDE_RETRY:
        {
            static const s16 starts[][2] = {{5, 19}, {5, 19}, {6, 19}, {6, 19}};
            struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_FRESH_WATER)];
            u32 slot;
            if (gEcHeadlessFixtureParam >= ARRAY_COUNT(starts))
                break;
            CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ZIGZAGOON, 5, OTID_STRUCT_PLAYER_ID);
            CalculatePlayerPartyCount();
            FlagSet(FLAG_SYS_POKEMON_GET);
            ClearBag();
            for (slot = 0; slot < pocket->capacity; slot++)
                BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_POTION, 1);
            VarSet(VAR_RUSTBORO_CITY_STATE, 0);
            LoadHeadlessMap(MAP_RUSTBORO_CITY_GYM, starts[gEcHeadlessFixtureParam][0], starts[gEcHeadlessFixtureParam][1]);
        }
        break;
    case EC_HEADLESS_SCENARIO_NEW_MAUVILLE_BUTTONS:
        // Established visit: the Voltorb and item covering these two switches
        // have been cleared. Keep a real party and native Repel protection.
        CreateHealthyHeadlessMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ZIGZAGOON, 100, OTID_STRUCT_PLAYER_ID);
        CalculatePlayerPartyCount();
        FlagSet(FLAG_SYS_POKEMON_GET);
        FlagSet(FLAG_DEFEATED_VOLTORB_2_NEW_MAUVILLE);
        FlagSet(FLAG_HIDE_NEW_MAUVILLE_VOLTORB_2);
        FlagSet(FLAG_ITEM_NEW_MAUVILLE_UPGRADE);
        VarSet(VAR_REPEL_STEP_COUNT, 250);
        LoadHeadlessMap(MAP_NEW_MAUVILLE_INSIDE, 6, 12);
        break;
    case EC_HEADLESS_SCENARIO_START_MENU_FULL:
        // Every Start menu row an established save can show: Pokedex, DexNav, Pokemon,
        // Bag, PokeNav, Player, Save, Reload, Option, Exit (ten rows, two more than fit).
        FlagSet(FLAG_SYS_POKEDEX_GET);
        FlagSet(FLAG_RECEIVED_DEXNAV);
        FlagSet(FLAG_SYS_POKEMON_GET);
        FlagSet(FLAG_SYS_POKENAV_GET);
        gSaveFileStatus = SAVE_STATUS_OK; // makes the Reload row appear, as on a real save
        LoadHeadlessMap(MAP_OLDALE_TOWN_POKEMON_CENTER_1F, 7, 6);
        break;
    case EC_HEADLESS_SCENARIO_WILD_ACTION_MENU:
    case EC_HEADLESS_SCENARIO_MOVE_DETAILS:
    case EC_HEADLESS_SCENARIO_WILD_FOE_TYPES:
    case EC_HEADLESS_SCENARIO_MOVE_FOE_TYPES:
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
    {
        // A Circuit run already in progress at the desk with `param` wins (also
        // the best and lifetime totals). The Observe hook starts the next match.
        // Param 9 makes the next win the tenth lifetime win (milestone bonus).
        u16 wins = gEcHeadlessFixtureParam;

        PrepareCircuitParty();
        FlagSet(FLAG_SYS_GAME_CLEAR);
        FlagSet(FLAG_IS_CHAMPION);
        FlagSet(FLAG_EC_CHAMPIONS_CIRCUIT_EXPLAINED);
        ChampionsCircuitBegin();
        VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, wins);
        VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, wins);
        VarSet(VAR_EC_CIRCUIT_BEST_WINS, wins);
        LoadHeadlessMap(MAP_BATTLE_FRONTIER_BATTLE_TOWER_LOBBY, 6, 6);
        break;
    }
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
    case EC_HEADLESS_SCENARIO_TRAINER_CARD:
        PrepareHeadlessGoldTrainerCard();
        break;
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
    // Field moves without a taught HM: the party member could learn the move
    // but does not know it, the badge and HM flags are set, and
    // the player stands facing the obstacle. The scenario taps UP, A, then A on
    // the Yes/No, and the observer latches the "used <move>!" showcase.
    case EC_HEADLESS_SCENARIO_FIELD_MOVE_CUT:
        PrepareHeadlessFieldMoveParty(SPECIES_ZIGZAGOON, FLAG_BADGE01_GET, FLAG_RECEIVED_HM_CUT);
        LoadHeadlessMap(MAP_ROUTE104, 35, 23);
        break;
    case EC_HEADLESS_SCENARIO_FIELD_MOVE_ROCK_SMASH:
        PrepareHeadlessFieldMoveParty(SPECIES_ZIGZAGOON, FLAG_BADGE03_GET, FLAG_RECEIVED_HM_ROCK_SMASH);
        // Beside the nurse, below the east rock; her own tile is occupied.
        LoadHeadlessMap(MAP_ROUTE111, 19, 101);
        break;
    case EC_HEADLESS_SCENARIO_FIELD_MOVE_STRENGTH:
        PrepareHeadlessFieldMoveParty(SPECIES_LINOONE, FLAG_BADGE04_GET, FLAG_RECEIVED_HM_STRENGTH);
        LoadHeadlessMap(MAP_FIERY_PATH, 10, 16);
        break;
    // The Flight Beacon: nobody in the party can fly, but a boxed Wingull can
    // learn Fly without knowing it. The trigger opens the fly map, A picks the
    // current town, and the observer latches the Fly showcase carrying the
    // boxed rider with the override consumed.
    case EC_HEADLESS_SCENARIO_FLIGHT_BEACON:
    {
        struct Pokemon rider;

        PrepareHeadlessFieldMoveParty(SPECIES_ZIGZAGOON, FLAG_BADGE06_GET, FLAG_RECEIVED_HM_FLY);
        AddBagItem(ITEM_FLIGHT_BEACON, 1);
        FlagSet(FLAG_VISITED_LITTLEROOT_TOWN);
        CreateMon(&rider, SPECIES_WINGULL, 20, 0, OTID_STRUCT_PLAYER_ID);
        gPokemonStoragePtr->boxes[0][0] = rider.box;
        sEcHeadlessFlightRider = SPECIES_NONE;
        LoadHeadlessMap(MAP_LITTLEROOT_TOWN, 8, 10);
        break;
    }
    case EC_HEADLESS_SCENARIO_HALL_OF_FAME_RECORD:
        PrepareHeadlessHallParty(gEcHeadlessFixtureParam);
        // Where Wallace's walk leaves the player, so the balls land on the machine.
        LoadHeadlessMap(MAP_EVER_GRANDE_CITY_HALL_OF_FAME, 7, 5);
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
