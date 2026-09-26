#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "battle_pike.h"
#include "battle_gimmick.h"
#include "battle_util.h"
#include "caps.h"
#include "champions_circuit.h"
#include "difficulty.h"
#include "data.h"
#include "emerald_champions_battle_sets.h"
#include "emerald_champions_opening.h"
#include "mega_stone_rewards.h"
#include "trade.h"
#include "tv.h"
#include "constants/party_menu.h"
#include "constants/battle_frontier.h"
#include "constants/emerald_champions.h"
#include "constants/battle_pike.h"
#include "event_data.h"
#include "field_move.h"
#include "field_player_avatar.h"
#include "field_specials.h"
#include "frontier_util.h"
#include "gym_leader_rematch.h"
#include "item.h"
#include "legendary_signs.h"
#include "load_save.h"
#include "money.h"
#include "move_relearner.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "script_menu.h"
#include "showdown_champions_circuit.h"
#include "string_util.h"
#include "test/battle.h"
#include "test/test.h"
#include "text.h"
#include "wild_encounter.h"
#include "constants/pokedex.h"
#include "constants/rematches.h"
#include "constants/script_menu.h"
#include "constants/cries.h"
#include "constants/field_specials.h"
#include "constants/flags.h"
#include "constants/maps.h"
#include "constants/layouts.h"
#include "constants/map_event_ids.h"
#include "constants/trainers.h"
#include "constants/vars.h"

// Large test fixtures must not consume the test linker's limited IWRAM stack
// headroom. Both transaction tests zero and use this fixture independently.
static EWRAM_DATA struct BattleStruct sEmeraldChampionsTestBattleStruct;
static EWRAM_DATA u16 sEmeraldChampionsPreparationMoveBuffer[MAX_RELEARNER_MOVES];

static const u16 sEmeraldChampionsTestSignStateVars[] =
{
    VAR_LEGENDARY_SIGNS_UNLOCKED_0,
    VAR_LEGENDARY_SIGNS_UNLOCKED_1,
    VAR_LEGENDARY_SIGNS_UNLOCKED_2,
    VAR_LEGENDARY_SIGNS_UNLOCKED_3,
    VAR_LEGENDARY_SIGNS_UNLOCKED_4,
    VAR_LEGENDARY_SIGNS_UNLOCKED_5,
    VAR_LEGENDARY_SIGNS_CAUGHT_0,
    VAR_LEGENDARY_SIGNS_CAUGHT_1,
    VAR_LEGENDARY_SIGNS_CAUGHT_2,
    VAR_LEGENDARY_SIGNS_CAUGHT_3,
    VAR_LEGENDARY_SIGNS_CAUGHT_4,
    VAR_LEGENDARY_SIGNS_CAUGHT_5,
};

static void ResetEmeraldChampionsGameCornerTestState(void)
{
    ZeroPlayerPartyMons();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
    VarSet(VAR_STARTER_GEN, 1);
    VarSet(VAR_STARTER_MON, 0);
    FlagClear(FLAG_EC_STARTER_ARCHIVE_BULBASAUR);
    FlagClear(FLAG_EC_STARTER_ARCHIVE_CHARMANDER);
    FlagClear(FLAG_EC_STARTER_ARCHIVE_SQUIRTLE);
    FlagClear(FLAG_EC_STARTER_ARCHIVE_QUAXLY);
}

static void ClearEmeraldChampionsLegendaryCaughtState(void)
{
    static const u16 caughtVars[] =
    {
        VAR_LEGENDARY_SIGNS_CAUGHT_0,
        VAR_LEGENDARY_SIGNS_CAUGHT_1,
        VAR_LEGENDARY_SIGNS_CAUGHT_2,
        VAR_LEGENDARY_SIGNS_CAUGHT_3,
        VAR_LEGENDARY_SIGNS_CAUGHT_4,
        VAR_LEGENDARY_SIGNS_CAUGHT_5,
    };

    memset(gSaveBlock2Ptr->pokedex.lostLegendaryEncounters, 0, sizeof(gSaveBlock2Ptr->pokedex.lostLegendaryEncounters));

    for (u32 i = 0; i < ARRAY_COUNT(caughtVars); i++)
        VarSet(caughtVars[i], 0);
}

static void FillEmeraldChampionsPokemonStorage(void)
{
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            CreateBoxMon(&gPokemonStoragePtr->boxes[box][slot], SPECIES_RATTATA, 5, 0, OTID_STRUCT_PLAYER_ID);
}

TEST("Emerald Champions battle-test runner fits its fixed workspace")
{
    EXPECT_LE(sizeof(struct BattleTestRunnerState), 0x5000);
}

static bool32 MonMatchesEmeraldChampionsNonMegaPreset(struct Pokemon *mon)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);

    for (u8 choice = 0; choice < GetEmeraldChampionsRawBattleSetCount(species); choice++)
    {
        const struct EmeraldChampionsBattleSet *preset = GetEmeraldChampionsRawBattleSet(species, choice);
        bool32 matches = TRUE;

        if (preset == NULL || preset->requiredItem != ITEM_NONE)
            continue;
        matches &= GetMonData(mon, MON_DATA_HIDDEN_NATURE) == preset->nature;
        matches &= GetMonAbility(mon) == preset->ability;
        matches &= GetMonData(mon, MON_DATA_HELD_ITEM) == preset->item;
        for (u32 move = 0; move < MAX_MON_MOVES; move++)
            matches &= GetMonData(mon, MON_DATA_MOVE1 + move) == preset->moves[move];
        if (matches)
            return TRUE;
    }
    return FALSE;
}

static bool32 BoxMonMatchesEmeraldChampionsNonMegaPreset(struct BoxPokemon *mon)
{
    enum Species species = GetBoxMonData(mon, MON_DATA_SPECIES);

    for (u8 choice = 0; choice < GetEmeraldChampionsRawBattleSetCount(species); choice++)
    {
        const struct EmeraldChampionsBattleSet *preset = GetEmeraldChampionsRawBattleSet(species, choice);
        bool32 matches = TRUE;

        if (preset == NULL || preset->requiredItem != ITEM_NONE)
            continue;
        matches &= GetBoxMonData(mon, MON_DATA_HIDDEN_NATURE) == preset->nature;
        matches &= GetAbilityBySpecies(
            species,
            GetBoxMonData(mon, MON_DATA_ABILITY_NUM)
        ) == preset->ability;
        matches &= GetBoxMonData(mon, MON_DATA_HELD_ITEM) == preset->item;
        for (u32 move = 0; move < MAX_MON_MOVES; move++)
            matches &= GetBoxMonData(mon, MON_DATA_MOVE1 + move) == preset->moves[move];
        if (matches)
            return TRUE;
    }
    return FALSE;
}

static bool32 SpeciesCanAccessEmeraldChampionsPresetMove(enum Species species, enum Move move)
{
    enum Species current = species;

    // Sketch is Smeargle's canonical access to its authored support moves.
    if (SpeciesToNationalPokedexNum(species) == NATIONAL_DEX_SMEARGLE)
        return TRUE;

    do
    {
        const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(current);

        for (u32 i = 0; learnset[i].move != LEVEL_UP_MOVE_END; i++)
        {
            if (learnset[i].move == move)
                return TRUE;
        }
        current = GetSpeciesPreEvolution(current);
    } while (current != SPECIES_NONE);

    if (CanLearnTeachableMove(species, move))
        return TRUE;

    current = species;
    while (GetSpeciesPreEvolution(current) != SPECIES_NONE)
        current = GetSpeciesPreEvolution(current);
    const u16 *eggMoves = GetSpeciesEggMoves(current);
    for (u32 i = 0; eggMoves[i] != MOVE_UNAVAILABLE; i++)
    {
        if (eggMoves[i] == move)
            return TRUE;
    }
    return FALSE;
}

struct EmeraldChampionsReviewedMoveAccess
{
    enum Species species;
    enum Move move;
};

static const struct EmeraldChampionsReviewedMoveAccess sReviewedMoveAccess[] =
{
#include "../src/data/pokemon/emerald_champions_move_access_review.h"
};

TEST("Emerald Champions disables Match Call and Gym rematches")
{

    EXPECT_EQ(ShouldTryRematchBattleForTrainerId(TRAINER_ROSE_1), FALSE);
    EXPECT_EQ(GetCurrentGymLeaderRematchLevel(), 0);

    SetTrainerFlag(TRAINER_BRAWLY_1);
    SetTrainerFlag(TRAINER_ARCHIE_SLATEPORT); // Reuses the disabled Brawly-2 slot.
    EXPECT_EQ(GetLastBeatenRematchTrainerId(TRAINER_BRAWLY_1), TRAINER_BRAWLY_1);
    EXPECT_EQ(CountBattledRematchTeams(REMATCH_BRAWLY), 1);

    gTrainerBattleParameter.params.opponentA = TRAINER_ROSE_1;
    FlagClear(TRAINER_REGISTERED_FLAGS_START + REMATCH_ROSE);
    EXPECT_EQ(IsTrainerReadyForRematch(), FALSE);
    FlagSet(TRAINER_REGISTERED_FLAGS_START + REMATCH_ROSE);
    EXPECT_EQ(IsTrainerReadyForRematch(), TRUE);
}

TEST("Emerald Champions exposes Mega as its only selectable gimmick")
{
    EXPECT(IsEmeraldChampionsGimmickAllowed(GIMMICK_NONE));
    EXPECT(IsEmeraldChampionsGimmickAllowed(GIMMICK_MEGA));
    EXPECT(!IsEmeraldChampionsGimmickAllowed(GIMMICK_Z_MOVE));
    EXPECT(!IsEmeraldChampionsGimmickAllowed(GIMMICK_ULTRA_BURST));
    EXPECT(!IsEmeraldChampionsGimmickAllowed(GIMMICK_DYNAMAX));
}

TEST("Emerald Champions Center preparation lists are complete and isolated")
{
    struct Pokemon mon;
    const u16 *canonical = GetEmeraldChampionsPreparationMoves(SPECIES_MEW);
    u32 canonicalCount = 0;
    bool32 hasTailwind = FALSE;
    bool32 hasWillOWisp = FALSE;
    bool32 hasPreparationOnlyMove = FALSE;

    while (canonical[canonicalCount] != MOVE_UNAVAILABLE)
    {
        enum Move move = canonical[canonicalCount];

        EXPECT(move > MOVE_NONE && move < MOVES_COUNT_ALL);
        for (u32 previous = 0; previous < canonicalCount; previous++)
            EXPECT_NE(canonical[previous], move);
        hasTailwind |= move == MOVE_TAILWIND;
        hasWillOWisp |= move == MOVE_WILL_O_WISP;
        hasPreparationOnlyMove |= !CanLearnTeachableMove(SPECIES_MEW, move);
        canonicalCount++;
    }
    EXPECT_EQ(canonicalCount, 371);
    EXPECT(hasTailwind);
    EXPECT(hasWillOWisp);
    EXPECT(hasPreparationOnlyMove);
    EXPECT(!CanLearnTeachableMove(SPECIES_MEW, MOVE_TAILWIND));
    EXPECT(!CanLearnTeachableMove(SPECIES_MEW, MOVE_WILL_O_WISP));

    CreateMon(&mon, SPECIES_MEW, 50, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(
        GetEmeraldChampionsPreparationMovesToLearn(&mon.box, sEmeraldChampionsPreparationMoveBuffer),
        371);

    SetMonMoveSlot(&mon, MOVE_PSYCHIC, 0);
    SetMonMoveSlot(&mon, MOVE_TAILWIND, 1);
    SetMonMoveSlot(&mon, MOVE_WILL_O_WISP, 2);
    SetMonMoveSlot(&mon, MOVE_PROTECT, 3);
    EXPECT_EQ(
        GetEmeraldChampionsPreparationMovesToLearn(&mon.box, sEmeraldChampionsPreparationMoveBuffer),
        367);
    for (u32 i = 0; i < 367; i++)
    {
        EXPECT_NE(sEmeraldChampionsPreparationMoveBuffer[i], MOVE_PSYCHIC);
        EXPECT_NE(sEmeraldChampionsPreparationMoveBuffer[i], MOVE_TAILWIND);
        EXPECT_NE(sEmeraldChampionsPreparationMoveBuffer[i], MOVE_WILL_O_WISP);
        EXPECT_NE(sEmeraldChampionsPreparationMoveBuffer[i], MOVE_PROTECT);
    }
}

TEST("Emerald Champions preparation table covers every enabled species")
{
    for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        if (!IsSpeciesEnabled(species))
            continue;

        const u16 *moves = GetEmeraldChampionsPreparationMoves(species);
        u32 count = 0;

        while (moves[count] != MOVE_UNAVAILABLE)
        {
            EXPECT(moves[count] > MOVE_NONE && moves[count] < MOVES_COUNT_ALL);
            EXPECT(count < MAX_RELEARNER_MOVES);
            count++;
        }
        EXPECT(count > 0);
    }

    // Historical form rows override their base form; custom forms absent from
    // the source corpus inherit the base species without changing TM data.
    EXPECT_NE(
        GetEmeraldChampionsPreparationMoves(SPECIES_ROTOM_WASH),
        GetEmeraldChampionsPreparationMoves(SPECIES_ROTOM));
    EXPECT_EQ(
        GetEmeraldChampionsPreparationMoves(SPECIES_GLIMMORA_MEGA),
        GetEmeraldChampionsPreparationMoves(SPECIES_GLIMMORA));
}

TEST("Emerald Champions move reordering preserves moves PP and upgrades")
{
    const u16 moves[] = {MOVE_TACKLE, MOVE_GROWL, MOVE_LEECH_SEED, MOVE_VINE_WHIP};
    const u8 pp[] = {1, 0, 3, 2};
    const u8 bonuses = 0xE4; // Distinct upgrade counts: 0, 1, 2, 3.
    struct Pokemon mon;

    for (u32 first = 0; first < MAX_MON_MOVES; first++)
    {
        for (u32 second = 0; second < MAX_MON_MOVES; second++)
        {
            CreateMon(&mon, SPECIES_BULBASAUR, 20, 0, OTID_STRUCT_PLAYER_ID);
            for (u32 i = 0; i < MAX_MON_MOVES; i++)
            {
                SetMonData(&mon, MON_DATA_MOVE1 + i, &moves[i]);
                SetMonData(&mon, MON_DATA_PP1 + i, &pp[i]);
            }
            SetMonData(&mon, MON_DATA_PP_BONUSES, &bonuses);
            SwapBoxMonMoves(&mon.box, first, second);
            for (u32 i = 0; i < MAX_MON_MOVES; i++)
            {
                u32 source = i == first ? second : i == second ? first : i;
                EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE1 + i), moves[source]);
                EXPECT_EQ(GetMonData(&mon, MON_DATA_PP1 + i), pp[source]);
                EXPECT_EQ((GetMonData(&mon, MON_DATA_PP_BONUSES) >> (2 * i)) & 3, source);
            }
            // Restore order, then exercise the deletion consumer at every slot.
            SwapBoxMonMoves(&mon.box, first, second);
            DeleteMove(&mon, moves[first]);
            for (u32 i = 0; i < MAX_MON_MOVES - 1; i++)
            {
                u32 source = i < first ? i : i + 1;
                EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE1 + i), moves[source]);
                EXPECT_EQ(GetMonData(&mon, MON_DATA_PP1 + i), pp[source]);
                EXPECT_EQ((GetMonData(&mon, MON_DATA_PP_BONUSES) >> (2 * i)) & 3, source);
            }
            EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE4), MOVE_NONE);
            EXPECT_EQ(GetMonData(&mon, MON_DATA_PP4), 0);
            EXPECT_EQ(GetMonData(&mon, MON_DATA_PP_BONUSES) >> 6, 0);
        }
    }
}

TEST("Emerald Champions disables the Bag only in competitive trainer battles")
{
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    EXPECT(!IsAllowedToUseBag());
    gBattleTypeFlags = 0;
    EXPECT(IsAllowedToUseBag());
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_PYRAMID;
    EXPECT(IsAllowedToUseBag());
}

// E3 hands Text Speed back to the player, so the saved value is now obeyed
// instead of being overridden to Instant; Instant is only the new-game default.
TEST("Emerald Champions Options owns the text speed setting")
{
    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_SLOW;
    EXPECT_EQ(GetPlayerTextSpeed(), OPTIONS_TEXT_SPEED_SLOW);
    EXPECT_EQ(GetPlayerTextSpeedDelay(), 8);
    EXPECT(!IsPlayerTextSpeedInstant());

    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_FAST;
    EXPECT_EQ(GetPlayerTextSpeed(), OPTIONS_TEXT_SPEED_FAST);
    EXPECT(!IsPlayerTextSpeedInstant());

    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_INSTANT;
    EXPECT_EQ(GetPlayerTextSpeed(), OPTIONS_TEXT_SPEED_INSTANT);
    EXPECT(IsPlayerTextSpeedInstant());
}

TEST("Emerald Champions catch transfers preserve both held-item loadouts")
{
    struct BattleStruct *savedBattleStruct = gBattleStruct;
    enum Item item;
    u32 outgoingItem;
    u32 caughtItem;

    memset(&sEmeraldChampionsTestBattleStruct, 0, sizeof(sEmeraldChampionsTestBattleStruct));
    gBattleStruct = &sEmeraldChampionsTestBattleStruct;
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_BULBASAUR, 14, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();

    // Catch-and-swap boxes the outgoing mon before the normal end-of-battle
    // restoration pass. Its battle-start item must be restored first.
    item = ITEM_EVIOLITE;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &item);
    RecordPlayerPartyMonHeldItemForRestoration(0);
    item = ITEM_NONE;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &item);
    RestorePlayerPartyMonHeldItem(0);
    outgoingItem = GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM);

    // A caught mon entering an empty/replaced slot needs a fresh restoration
    // baseline; otherwise the battle-start ITEM_NONE erases its authored item.
    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_CHARMANDER, 14, 0, OTID_STRUCT_PLAYER_ID);
    item = ITEM_LIFE_ORB;
    SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HELD_ITEM, &item);
    RecordPlayerPartyMonHeldItemForRestoration(1);
    item = ITEM_NONE;
    SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HELD_ITEM, &item);
    TryRestoreHeldItems();
    caughtItem = GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HELD_ITEM);
    gBattleStruct = savedBattleStruct;

    EXPECT_EQ(outgoingItem, ITEM_EVIOLITE);
    EXPECT_EQ(caughtItem, ITEM_LIFE_ORB);
}

TEST("Emerald Champions captured prepared sets survive party PC and no-room transactions")
{
    struct Pokemon caughtMon;

    ZeroPlayerPartyMons();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
    SeedRng(19);
    CreateMon(&caughtMon, SPECIES_BULBASAUR, 14, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(ApplyEmeraldChampionsRandomWildSet(&caughtMon), EC_BATTLE_SET_SUCCESS);
    EXPECT_EQ(GiveCapturedMonToPlayer(&caughtMon), MON_GIVEN_TO_PARTY);
    EXPECT(MonMatchesEmeraldChampionsNonMegaPreset(&gParties[B_TRAINER_PLAYER][0]));

    for (u32 slot = 1; slot < PARTY_SIZE; slot++)
        CreateMon(&gParties[B_TRAINER_PLAYER][slot], SPECIES_RATTATA, 5, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    CreateMon(&caughtMon, SPECIES_CHARMANDER, 14, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(ApplyEmeraldChampionsRandomWildSet(&caughtMon), EC_BATTLE_SET_SUCCESS);
    EXPECT_EQ(GiveCapturedMonToPlayer(&caughtMon), MON_GIVEN_TO_PC);
    EXPECT(BoxMonMatchesEmeraldChampionsNonMegaPreset(&gPokemonStoragePtr->boxes[0][0]));

    FillEmeraldChampionsPokemonStorage();
    CreateMon(&caughtMon, SPECIES_SQUIRTLE, 14, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(ApplyEmeraldChampionsRandomWildSet(&caughtMon), EC_BATTLE_SET_SUCCESS);
    EXPECT_EQ(GiveCapturedMonToPlayer(&caughtMon), MON_CANT_GIVE);
    EXPECT(MonMatchesEmeraldChampionsNonMegaPreset(&caughtMon));
}

TEST("Emerald Champions custom Megas retain complete native assets")
{
    static const enum Species forms[] =
    {
        SPECIES_TATSUGIRI_CURLY_MEGA,
        SPECIES_TATSUGIRI_DROOPY_MEGA,
        SPECIES_TATSUGIRI_STRETCHY_MEGA,
        SPECIES_GLIMMORA_MEGA,
        SPECIES_BUTTERFREE_MEGA,
        SPECIES_MACHAMP_MEGA,
        SPECIES_KINGLER_MEGA,
        SPECIES_LAPRAS_MEGA,
        SPECIES_FLYGON_MEGA,
        SPECIES_MILOTIC_MEGA,
        SPECIES_KINGDRA_MEGA,
    };

    for (u32 i = 0; i < ARRAY_COUNT(forms); i++)
    {
        EXPECT(gSpeciesInfo[forms[i]].isMegaEvolution);
        EXPECT_NE(gSpeciesInfo[forms[i]].frontPic, NULL);
        EXPECT_NE(gSpeciesInfo[forms[i]].backPic, NULL);
        EXPECT_NE(gSpeciesInfo[forms[i]].iconSprite, NULL);
        EXPECT_NE((u16)gSpeciesInfo[forms[i]].cryId, (u16)CRY_NONE);
    }
    EXPECT_NE(gSpeciesInfo[SPECIES_TATSUGIRI_CURLY_MEGA].frontPic, gSpeciesInfo[SPECIES_TATSUGIRI_CURLY].frontPic);
    EXPECT_NE(gSpeciesInfo[SPECIES_GLIMMORA_MEGA].frontPic, gSpeciesInfo[SPECIES_GLIMMORA].frontPic);
    EXPECT_EQ(gItemsInfo[ITEM_TATSUGIRINITE].sortType, ITEM_TYPE_MEGA_STONE);
    EXPECT_EQ(gItemsInfo[ITEM_GLIMMORANITE].sortType, ITEM_TYPE_MEGA_STONE);
}

static const struct { u16 flag; u8 cap; } sCampaignCapExpectations[] =
{
    {FLAG_BADGE01_GET, 20},
    {FLAG_BADGE02_GET, 30},
    {FLAG_BADGE03_GET, 40},
    {FLAG_BADGE04_GET, 45},
    {FLAG_BADGE05_GET, 55},
    {FLAG_BADGE06_GET, 60},
    {FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT, 65},
    {FLAG_BADGE07_GET, 70},
    {FLAG_BADGE08_GET, 80},
    {FLAG_IS_CHAMPION, 100},
};

// Former milestone flags that no longer move the cap; cleared alongside the
// cap flags so each test starts from the same story state.
static const u16 sRetiredMilestoneFlags[] =
{
    FLAG_SYS_POKENAV_GET,
    FLAG_DELIVERED_DEVON_GOODS,
    FLAG_EC_REPORT_C14_COMPLETE,
    FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY,
    FLAG_EC_REPORT_C30_COMPLETE,
    FLAG_EC_REPORT_C36_COMPLETE,
    FLAG_EC_REPORT_C39_COMPLETE,
    FLAG_EC_REPORT_C42_COMPLETE,
    FLAG_DEFEATED_WALLY_VICTORY_ROAD,
    FLAG_EC_REPORT_C26_COMPLETE,
    FLAG_EC_REPORT_C28_COMPLETE,
    FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE,
    FLAG_EC_REPORT_C43_COMPLETE,
    FLAG_EC_REPORT_C44_COMPLETE,
    FLAG_EC_REPORT_C45_COMPLETE,
    FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE,
    FLAG_EC_REPORT_C48_COMPLETE,
};

static void ResetCampaignCapMilestones(void)
{
    for (u32 i = 0; i < ARRAY_COUNT(sCampaignCapExpectations); i++)
        FlagClear(sCampaignCapExpectations[i].flag);
    for (u32 i = 0; i < ARRAY_COUNT(sRetiredMilestoneFlags); i++)
        FlagClear(sRetiredMilestoneFlags[i]);
}

TEST("Emerald Champions native trainer creation applies live-cap role offsets once")
{
    static const struct TrainerMon mons[] = {
        {.species = SPECIES_PIKACHU, .lvl = 99, .useLevelOffset = TRUE, .levelOffset = 1,
         .nature = NATURE_TIMID, .gender = TRAINER_MON_RANDOM_GENDER,
         .heldItem = ITEM_LIGHT_BALL, .moves = {MOVE_THUNDERBOLT}},
        {.species = SPECIES_EEVEE, .lvl = 99, .useLevelOffset = TRUE, .levelOffset = -2,
         .gender = TRAINER_MON_RANDOM_GENDER},
    };
    static const struct Trainer trainer = {.party = mons, .partySize = 2};
    struct Pokemon *party = gParties[B_TRAINER_OPPONENT_A];

    ResetCampaignCapMilestones();
    SetCurrentDifficultyLevel(DIFFICULTY_HARD);
    CreateNPCTrainerPartyFromTrainer(party, &trainer);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_LEVEL), 15);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_LEVEL), 12);
    SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
    CreateNPCTrainerPartyFromTrainer(party, &trainer);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_LEVEL), 14);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_LEVEL), 11);
    SetCurrentDifficultyLevel(DIFFICULTY_EASY);
    CreateNPCTrainerPartyFromTrainer(party, &trainer);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_LEVEL), 11);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_LEVEL), 8);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_HP), GetMonData(&party[0], MON_DATA_MAX_HP));
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PIKACHU);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_HELD_ITEM), ITEM_LIGHT_BALL);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_MOVE1), MOVE_THUNDERBOLT);

    FlagSet(FLAG_IS_CHAMPION);
    EXPECT_EQ(GetCampaignTrainerLevel(3), 99);
    SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
    EXPECT_EQ(GetCampaignTrainerLevel(3), 102);
    SetCurrentDifficultyLevel(DIFFICULTY_HARD);
    EXPECT_EQ(GetCampaignTrainerLevel(3), 103);
    ResetCampaignCapMilestones();
    ZeroEnemyPartyMons();
}

TEST("Emerald Champions level caps follow every campaign milestone")
{
    ResetCampaignCapMilestones();
    EXPECT_EQ(GetCurrentLevelCap(), 14);
    for (u32 i = 0; i < ARRAY_COUNT(sCampaignCapExpectations); i++)
    {
        FlagSet(sCampaignCapExpectations[i].flag);
        EXPECT_EQ(GetCurrentLevelCap(), sCampaignCapExpectations[i].cap);
    }
    ResetCampaignCapMilestones();
}

TEST("Emerald Champions all species use the same campaign level cap")
{
    static const struct { enum Species species; u8 cap100; u8 cap50; } cases[] =
    {
        {SPECIES_GARCHOMP, 100, 50},
        {SPECIES_MEW, 100, 50},
        {SPECIES_KARTANA, 100, 50},
        {SPECIES_KYOGRE, 100, 50},
        {SPECIES_KYOGRE_PRIMAL, 100, 50},
        {SPECIES_MEWTWO, 100, 50},
        {SPECIES_MEWTWO_MEGA_X, 100, 50},
        {SPECIES_ARCEUS, 100, 50},
        {SPECIES_ARCEUS_FAIRY, 100, 50},
        {SPECIES_KYUREM_BLACK, 100, 50},
        {SPECIES_ZACIAN_HERO, 100, 50},
        {SPECIES_ZACIAN_CROWNED, 100, 50},
        {SPECIES_NECROZMA_DUSK_MANE, 100, 50},
    };
    for (u32 i = 0; i < ARRAY_COUNT(cases); i++)
    {
        EXPECT_EQ(GetLevelCapForSpecies(cases[i].species, 100), cases[i].cap100);
        EXPECT_EQ(GetLevelCapForSpecies(cases[i].species, 50), cases[i].cap50);
        EXPECT_EQ(GetLevelCapForSpecies(cases[i].species, 1), 1);
    }
    EXPECT_EQ(GetLevelCapForSpecies(SPECIES_NONE, 50), 50);
    EXPECT_EQ(GetLevelCapForSpecies(NUM_SPECIES, 50), 50);
}

TEST("Emerald Champions Mega items and Dragon Ascent add no player cap penalty")
{
    struct Pokemon mon;
    enum Item item;
    ResetCampaignCapMilestones();
    FlagSet(FLAG_BADGE03_GET);
    ClearBag();
    EXPECT(AddBagItem(ITEM_MEGA_RING, 1));
    CreateMon(&mon, SPECIES_VENUSAUR, 30, 12345, OTID_STRUCT_PLAYER_ID);
    CalculateMonStats(&mon);
    item = ITEM_VENUSAURITE;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    EXPECT(!ClampMonToPlayerLevelCap(&mon));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), 30);
    CreateMon(&mon, SPECIES_RAYQUAZA, 26, 12345, OTID_STRUCT_PLAYER_ID);
    CalculateMonStats(&mon);
    SetMonMoveSlot(&mon, MOVE_DRAGON_ASCENT, 0);
    EXPECT(!ClampMonToPlayerLevelCap(&mon));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), 26);
    CreateMon(&mon, SPECIES_KYOGRE, 26, 12345, OTID_STRUCT_PLAYER_ID);
    CalculateMonStats(&mon);
    item = ITEM_BLUE_ORB;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    EXPECT(!ClampMonToPlayerLevelCap(&mon));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), 26);
    ClearBag();
    ResetCampaignCapMilestones();
}

TEST("Emerald Champions acquisitions cap normally and box a second Legendary")
{
    struct Pokemon mon;
    u16 item = ITEM_FOCUS_SASH;
    u32 status = STATUS1_PARALYSIS;
    ResetCampaignCapMilestones();
    ZeroPlayerPartyMons();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
    CreateMon(&mon, SPECIES_MEWTWO, 80, 12345, OTID_STRUCT_PLAYER_ID);
    SetMonMoveSlot(&mon, MOVE_PSYSTRIKE, 0);
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    SetMonData(&mon, MON_DATA_STATUS, &status);
    EXPECT_EQ(GiveScriptedMonToPlayer(&mon, 0), MON_GIVEN_TO_PARTY);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 14);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE1), MOVE_PSYSTRIKE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), item);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_STATUS), status);
    CreateMon(&mon, SPECIES_MEWTWO, 80, 54321, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GiveScriptedMonToPlayer(&mon, 1), MON_GIVEN_TO_PC);
    EXPECT_EQ(GetLevelFromBoxMonExp(&gPokemonStoragePtr->boxes[0][0]), 14);
    EXPECT_EQ(GetPlayerLevelCapForSpecies(SPECIES_MEWTWO), 14);
    ZeroPlayerPartyMons();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
}

TEST("Emerald Champions cap changes preserve fainting through boxed storage")
{
    struct Pokemon mon, restored;
    struct BoxPokemon box;
    u16 hp = 0;
    ResetCampaignCapMilestones();
    CreateMon(&mon, SPECIES_KYUREM_BLACK, 100, 12345, OTID_STRUCT_PLAYER_ID);
    CalculateMonStats(&mon);
    EXPECT_GT(GetMonData(&mon, MON_DATA_MAX_HP), 0);
    SetMonData(&mon, MON_DATA_HP, &hp);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HP), 0);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HP_LOST), GetMonData(&mon, MON_DATA_MAX_HP));
    box = mon.box;
    BoxMonToMon(&box, &restored);
    EXPECT_EQ(GetMonData(&restored, MON_DATA_HP), 0);
    EXPECT(ClampBoxMonToPlayerLevelCap(&box));
    EXPECT_NE(GetBoxMonData(&box, MON_DATA_HP_LOST), 0);
    BoxMonToMon(&box, &restored);
    EXPECT_EQ(GetMonData(&restored, MON_DATA_LEVEL), 14);
    EXPECT_EQ(GetMonData(&restored, MON_DATA_HP), 0);
    EXPECT_EQ(GetMonData(&restored, MON_DATA_HP_LOST), GetMonData(&restored, MON_DATA_MAX_HP));
    EXPECT(!ClampMonToPlayerLevelCap(&restored));
    EXPECT_EQ(GetMonData(&restored, MON_DATA_HP), 0);
}

TEST("Emerald Champions candy and level increments stop at the campaign cap")
{
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    u32 experience;
    ResetCampaignCapMilestones();
    ZeroPlayerPartyMons();
    CreateMon(mon, SPECIES_MEWTWO, 13, 12345, OTID_STRUCT_PLAYER_ID);
    EXPECT(!ExecuteTableBasedItemEffect(mon, ITEM_RARE_CANDY, 0, 0));
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), 14);
    EXPECT(!IsMonEligibleForLeveler(mon));
    EXPECT(ExecuteTableBasedItemEffect(mon, ITEM_RARE_CANDY, 0, 0));
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), 14);
    experience = gExperienceTables[gSpeciesInfo[SPECIES_MEWTWO].growthRate][15];
    SetMonData(mon, MON_DATA_EXP, &experience);
    EXPECT(!TryIncrementMonLevel(mon));
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), 14);
    ZeroPlayerPartyMons();
}

TEST("Emerald Champions strict EXP cap blocks gains at the milestone")
{
    FlagClear(FLAG_IS_CHAMPION);
    for (u32 i = 0; i < NUM_BADGES; i++)
        FlagClear(FLAG_BADGE01_GET + i);

    EXPECT_EQ(GetCurrentLevelCap(), 14);
    EXPECT_EQ(GetSoftLevelCapExpValue(13, 100), 100);
    EXPECT_EQ(GetSoftLevelCapExpValue(14, 100), 0);
    EXPECT_EQ(GetSoftLevelCapExpValue(15, 100), 0);
}

TEST("Emerald Champions leveling never interrupts a competitive moveset")
{
    struct Pokemon mon;
    enum Move originalMoves[MAX_MON_MOVES];

    CreateMon(&mon, SPECIES_BULBASAUR, 7, 0, OTID_STRUCT_PLAYER_ID);
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        originalMoves[i] = GetMonData(&mon, MON_DATA_MOVE1 + i);

    EXPECT_EQ(MonTryLearningNewMoveAtLevel(&mon, TRUE, 7), MOVE_NONE);
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE1 + i), originalMoves[i]);
}

TEST("Emerald Champions can forget a former HM move to learn a new one")
{
    struct Pokemon mon;

    // CannotForgetMove is the one guard shared by the level-up learn path
    // (battle_script_commands.c), the evolution learn path and the Summary
    // replace flow. With licenses instead of HMs it must never refuse.
    EXPECT(IsMoveHM(MOVE_SURF));
    EXPECT(IsMoveHM(MOVE_FLY));
    EXPECT(IsMoveHM(MOVE_DIVE));
    EXPECT_EQ(CannotForgetMove(MOVE_SURF), FALSE);
    EXPECT_EQ(CannotForgetMove(MOVE_FLY), FALSE);
    EXPECT_EQ(CannotForgetMove(MOVE_DIVE), FALSE);

    CreateMon(&mon, SPECIES_MUDKIP, 20, 0, OTID_STRUCT_PLAYER_ID);
    SetMonMoveSlot(&mon, MOVE_SURF, 0);
    SetMonMoveSlot(&mon, MOVE_EARTHQUAKE, 1);
    SetMonMoveSlot(&mon, MOVE_ICE_BEAM, 2);
    SetMonMoveSlot(&mon, MOVE_PROTECT, 3);

    // A full moveset forces the replace prompt, exactly as a level-up would.
    EXPECT_EQ(GiveMoveToMon(&mon, MOVE_WATERFALL), MON_HAS_MAX_MOVES);

    // The level-up handler replaces the chosen slot only when the guard allows it.
    EXPECT_EQ(CannotForgetMove(GetMonData(&mon, MON_DATA_MOVE1)), FALSE);
    RemoveMonPPBonus(&mon, 0);
    SetMonMoveSlot(&mon, MOVE_WATERFALL, 0);

    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE1), MOVE_WATERFALL);
    EXPECT(!MonKnowsMove(&mon, MOVE_SURF));
    EXPECT(GetMonData(&mon, MON_DATA_PP1) > 0);
}

TEST("Emerald Champions applies a complete authored battle set")
{
    struct Pokemon mon;
    u32 evTotal = 0;

    CreateMon(&mon, SPECIES_BULBASAUR, 14, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_GE(GetEmeraldChampionsBattleSetCount(&mon), 1);
    EXPECT_EQ(ApplyEmeraldChampionsBattleSetChoice(&mon, 0), EC_BATTLE_SET_SUCCESS);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE1), MOVE_GROWTH);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE2), MOVE_SLEEP_POWDER);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE3), MOVE_GIGA_DRAIN);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE4), MOVE_SLUDGE_BOMB);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HIDDEN_NATURE), NATURE_TIMID);
    EXPECT_EQ(GetMonAbility(&mon), ABILITY_CHLOROPHYLL);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), ITEM_EVIOLITE);

    for (u32 i = 0; i < NUM_STATS; i++)
        evTotal += GetMonData(&mon, MON_DATA_HP_EV + i);
    EXPECT_LE(evTotal, MAX_TOTAL_EVS);
}

TEST("Emerald Champions exposes named Doubles and Singles sets for every direct species")
{
    struct Pokemon mon;

    for (u8 format = 0; format < EC_BATTLE_FORMAT_COUNT; format++)
    {
        const struct EmeraldChampionsBattleSetRange *ranges = gEmeraldChampionsBattleSetRanges[format];

        for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
        {
            if (ranges[species].count == 0)
                continue;
            PARAMETRIZE_LABEL("format=%d species=%d", format, species)
            {
                // Enumerate the compiled tables independently of menu visibility:
                // a hidden or missing role must fail rather than escape the test.
                for (u32 raw = 0; raw < ranges[species].count; raw++)
                {
                    const struct EmeraldChampionsBattleSet *preset = &gEmeraldChampionsBattleSets[ranges[species].offset + raw].preset;
                    enum Item held = preset->requiredItem != ITEM_NONE ? preset->requiredItem : preset->item;
                    u32 applied = 0;
                    u32 total = 0;

                    ClearBag();
                    EXPECT(AddBagItem(ITEM_MEGA_RING, 1));
                    CreateMon(&mon, species, 50, 0, OTID_STRUCT_PLAYER_ID);
                    // Model ownership without depending on bag capacity. This also
                    // makes item-defined forms reachable before menu enumeration.
                    SetMonData(&mon, MON_DATA_HELD_ITEM, &held);
                    u32 count = GetEmeraldChampionsBattleSetCountForFormat(&mon, format);
                    for (u32 choice = 0; choice < count; choice++)
                    {
                        if (GetEmeraldChampionsBattleSetPresetForFormat(&mon, choice, format) != preset)
                            continue;
                        const u8 *name = GetEmeraldChampionsBattleSetNameForFormat(&mon, choice, format);
                        EXPECT(StringCompare(name, COMPOUND_STRING("Recommended")) != 0);
                        u8 result = ApplyEmeraldChampionsBattleSetChoiceForFormat(&mon, choice, format);
                        EXPECT(result == EC_BATTLE_SET_SUCCESS || result == EC_BATTLE_SET_MEGA
                            || result == EC_BATTLE_SET_MEGA_STONE_HELD);
                        applied++;
                        break;
                    }
                    if (applied != 1)
                        Test_MgbaPrintf("missing preset format=%d species=%d raw=%d", format, species, raw);
                    EXPECT_EQ(applied, 1);
                    for (u32 move = 0; move < MAX_MON_MOVES; move++)
                        EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE1 + move), preset->moves[move]);
                    EXPECT_EQ(GetMonData(&mon, MON_DATA_HIDDEN_NATURE), preset->nature);
                    EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), held);
                    EXPECT_NE(GetMonAbility(&mon), ABILITY_NONE);
                    if (preset->requiredItem == ITEM_NONE && preset->requiredMove == MOVE_NONE)
                        EXPECT_EQ(GetMonAbility(&mon), preset->ability);
                    for (u32 stat = 0; stat < NUM_STATS; stat++)
                    {
                        u32 points = GetMonData(&mon, EC_EV_DATA(stat));
                        total += points;
                        EXPECT_EQ(points, preset->evs[stat]);
                    }
                    EXPECT_LE(total, MAX_TOTAL_EVS);
                }
            }
        }
    }
    ClearBag();
}

TEST("Emerald Champions protects progression items from preparation services")
{
    static const enum Item evolutionItems[] =
    {
#include "../src/data/emerald_champions_evolution_items.h"
    };
    struct Pokemon mon;
    enum Item item = ITEM_DEEP_SEA_TOOTH;

    EXPECT(GetItemImportance(ITEM_LINKING_CORD));
    EXPECT(IsEmeraldChampionsProtectedProgressionItem(ITEM_VENUSAURITE));
    EXPECT(IsEmeraldChampionsProtectedProgressionItem(ITEM_RED_ORB));
    EXPECT(IsEmeraldChampionsProtectedProgressionItem(ITEM_WELLSPRING_MASK));
    EXPECT(IsEmeraldChampionsProtectedProgressionItem(ITEM_DOUSE_DRIVE));
    EXPECT(IsEmeraldChampionsProtectedProgressionItem(ITEM_FLAME_PLATE));
    for (u32 i = 0; i < ARRAY_COUNT(evolutionItems); i++)
        EXPECT(IsEmeraldChampionsProtectedProgressionItem(evolutionItems[i]));
    EXPECT(!IsEmeraldChampionsProtectedProgressionItem(ITEM_LIFE_ORB));

    CreateMon(&mon, SPECIES_CLAMPERL, 30, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    EXPECT_EQ(
        ApplyEmeraldChampionsBattleSetChoice(&mon, 0),
        EC_BATTLE_SET_SPECIAL_ITEM_EQUIPPED
    );
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), ITEM_DEEP_SEA_TOOTH);
}

TEST("Emerald Champions battle-ready wild presets exclude special encounters")
{
    EXPECT(IsEmeraldChampionsOrdinaryWildSpecies(SPECIES_BULBASAUR));
    EXPECT(IsEmeraldChampionsOrdinaryWildSpecies(SPECIES_NIHILEGO));
    EXPECT(IsEmeraldChampionsOrdinaryWildSpecies(SPECIES_GREAT_TUSK));
    EXPECT(!IsEmeraldChampionsOrdinaryWildSpecies(SPECIES_MEW));
    EXPECT(!IsEmeraldChampionsOrdinaryWildSpecies(SPECIES_VENUSAUR_MEGA));
}

TEST("Emerald Champions ordinary wild creation uses natural moves within the current cap")
{
    SeedRng(17);
    CreateWildMon(SPECIES_CHARIZARD, 35);
    struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][0];
    EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), SPECIES_CHARIZARD);
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), min(35, GetCurrentLevelCap()));
    struct Pokemon expected = *mon;
    GiveMonInitialMoveset(&expected);
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
        EXPECT_EQ(GetMonData(mon, MON_DATA_MOVE1 + slot), GetMonData(&expected, MON_DATA_MOVE1 + slot));
    EXPECT_EQ(GetMonData(mon, MON_DATA_HP_EV), 0);
}

TEST("Emerald Champions mandatory legendary scenes apply their authored set")
{
    gSpecialVar_0x8004 = SPECIES_HEATRAN;
    gSpecialVar_0x8005 = 0;
    CreateEmeraldChampionsStaticLegendaryEncounter();

    struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][0];
    EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), SPECIES_HEATRAN);
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), GetCurrentLevelCap());
    EXPECT_EQ(GetMonData(mon, MON_DATA_HELD_ITEM), ITEM_AIR_BALLOON);
    EXPECT_EQ(GetMonAbility(mon), ABILITY_FLASH_FIRE);
    EXPECT_EQ(GetMonData(mon, MON_DATA_HIDDEN_NATURE), NATURE_TIMID);
    EXPECT_EQ(GetMonData(mon, EC_EV_DATA(3)), 252); // Display-order Sp. Atk.
    bool32 hasMagmaStorm = FALSE;
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
    {
        if (GetMonData(mon, MON_DATA_MOVE1 + slot) == MOVE_MAGMA_STORM)
            hasMagmaStorm = TRUE;
    }
    EXPECT(hasMagmaStorm);
}

TEST("Emerald Champions unauthored legendary scenes still receive a random set")
{
    gSpecialVar_0x8004 = SPECIES_COBALION;
    gSpecialVar_0x8005 = 0;
    CreateEmeraldChampionsStaticLegendaryEncounter();

    struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][0];
    EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), SPECIES_COBALION);
    EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), GetCurrentLevelCap());
    u32 nonzeroMoves = 0;
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
    {
        if (GetMonData(mon, MON_DATA_MOVE1 + slot) != MOVE_NONE)
            nonzeroMoves++;
    }
    EXPECT_EQ(nonzeroMoves, MAX_MON_MOVES);
}

TEST("Emerald Champions manor Jigglypuff retain Sing without altering unrelated natural moves")
{
    u16 oldLayout = gMapHeader.mapLayoutId;
    struct Pokemon ordinary;
    for (u32 sample = 0; sample < 16; sample++)
    {
        gMapHeader.mapLayoutId = LAYOUT_ROUTE104;
        SeedRng(sample);
        CreateWildMon(SPECIES_JIGGLYPUFF, 20);
        ordinary = gParties[B_TRAINER_OPPONENT_A][0];
        bool32 knowsSing = FALSE;
        for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            knowsSing |= GetMonData(&ordinary, MON_DATA_MOVE1 + slot) == MOVE_SING;

        gMapHeader.mapLayoutId = LAYOUT_DEWFORD_MANOR_1F;
        SeedRng(sample);
        CreateWildMon(SPECIES_JIGGLYPUFF, 20);
        struct Pokemon *singer = &gParties[B_TRAINER_OPPONENT_A][0];
        bool32 retainsSing = FALSE;
        for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
        {
            u32 move = GetMonData(singer, MON_DATA_MOVE1 + slot);
            retainsSing |= move == MOVE_SING;
            EXPECT_EQ(move, !knowsSing && slot == MAX_MON_MOVES - 1
                ? MOVE_SING : GetMonData(&ordinary, MON_DATA_MOVE1 + slot));
            if (move == MOVE_SING)
                EXPECT_EQ(GetMonData(singer, MON_DATA_PP1 + slot), GetMoveMaxPP(MOVE_SING));
        }
        EXPECT(retainsSing);
        EXPECT_EQ(GetMonData(singer, MON_DATA_PERSONALITY), GetMonData(&ordinary, MON_DATA_PERSONALITY));
        EXPECT_EQ(GetMonData(singer, MON_DATA_HELD_ITEM), GetMonData(&ordinary, MON_DATA_HELD_ITEM));
        EXPECT_EQ(GetMonData(singer, MON_DATA_LEVEL), min(20, GetCurrentLevelCap()));
    }
    CreateWildMon(SPECIES_GASTLY, 20);
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
        EXPECT_NE(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_MOVE1 + slot), MOVE_SING);
    gMapHeader.mapLayoutId = oldLayout;
}

TEST("Emerald Champions wild presets never include Mega roles after Mega access")
{
    struct Pokemon mon;

    ClearBag();
    EXPECT(AddBagItem(ITEM_MEGA_RING, 1));
    // Charizard has two ordinary roles and both Mega X and Mega Y roles.
    // Repeated applications prove Mega access cannot alter the wild pool. The
    // static gate separately proves unbiased reservoir sampling across the two
    // eligible ordinary roles.
    for (u32 sample = 0; sample < 16; sample++)
    {
        CreateMon(&mon, SPECIES_CHARIZARD, 50, 0, OTID_STRUCT_PLAYER_ID);
        EXPECT_EQ(ApplyEmeraldChampionsRandomWildSet(&mon), EC_BATTLE_SET_SUCCESS);
        EXPECT(MonMatchesEmeraldChampionsNonMegaPreset(&mon));
        EXPECT_NE(GetMonData(&mon, MON_DATA_HELD_ITEM), ITEM_CHARIZARDITE_X);
        EXPECT_NE(GetMonData(&mon, MON_DATA_HELD_ITEM), ITEM_CHARIZARDITE_Y);
    }
    ClearBag();
}

TEST("Emerald Champions tutor gates Mega roles and never grants their stones")
{
    struct Pokemon mon;
    u8 megaChoices = 0;

    ClearBag();
    CreateMon(&mon, SPECIES_CHARIZARD, 50, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GetEmeraldChampionsBattleSetCount(&mon), 2);
    for (u8 choice = 0; choice < GetEmeraldChampionsBattleSetCount(&mon); choice++)
        EXPECT_EQ(GetEmeraldChampionsBattleSetRequiredItem(&mon, choice), ITEM_NONE);

    EXPECT(AddBagItem(ITEM_MEGA_RING, 1));
    EXPECT_EQ(GetEmeraldChampionsBattleSetCount(&mon), 6);
    for (u8 choice = 0; choice < GetEmeraldChampionsBattleSetCount(&mon); choice++)
    {
        enum Item requiredItem = GetEmeraldChampionsBattleSetRequiredItem(&mon, choice);

        if (requiredItem == ITEM_NONE)
            continue;
        megaChoices++;
        EXPECT_EQ(ApplyEmeraldChampionsBattleSetChoice(&mon, choice), EC_BATTLE_SET_MEGA);
        EXPECT_NE(GetMonData(&mon, MON_DATA_HELD_ITEM), requiredItem);
        EXPECT_EQ(CountTotalItemQuantityInBag(requiredItem), 0);
    }
    EXPECT_EQ(megaChoices, 4);
    ClearBag();
}

TEST("Emerald Champions restored Inclement Mega builds obey Ring and held-stone gates")
{
    static const struct { enum Species species; enum Item item; } forms[] =
    {
        {SPECIES_BUTTERFREE, ITEM_BUTTERFRENITE},
        {SPECIES_MACHAMP, ITEM_MACHAMPITE},
        {SPECIES_KINGLER, ITEM_KINGLERITE},
        {SPECIES_LAPRAS, ITEM_LAPRASITE},
        {SPECIES_FLYGON, ITEM_FLYGONITE},
        {SPECIES_MILOTIC, ITEM_MILOTICITE},
        {SPECIES_KINGDRA, ITEM_KINGDRANITE},
    };

    for (u32 f = 0; f < ARRAY_COUNT(forms); f++)
    {
        for (u32 format = 0; format < EC_BATTLE_FORMAT_COUNT; format++)
        {
            struct Pokemon mon;
            u32 megaChoices = 0;
            ClearBag();
            CreateMon(&mon, forms[f].species, 50, 0, OTID_STRUCT_PLAYER_ID);
            u32 ordinaryCount = GetEmeraldChampionsBattleSetCountForFormat(&mon, format);
            for (u32 choice = 0; choice < ordinaryCount; choice++)
                EXPECT_EQ(GetEmeraldChampionsBattleSetRequiredItemForFormat(&mon, choice, format), ITEM_NONE);
            EXPECT(AddBagItem(ITEM_MEGA_RING, 1));
            EXPECT_EQ(GetEmeraldChampionsBattleSetCountForFormat(&mon, format), ordinaryCount + 2);
            for (u32 choice = 0; choice < ordinaryCount + 2; choice++)
            {
                if (GetEmeraldChampionsBattleSetRequiredItemForFormat(&mon, choice, format) != forms[f].item)
                    continue;
                CreateMon(&mon, forms[f].species, 50, 0, OTID_STRUCT_PLAYER_ID);
                EXPECT_EQ(ApplyEmeraldChampionsBattleSetChoiceForFormat(&mon, choice, format), EC_BATTLE_SET_MEGA);
                EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), ITEM_NONE);
                EXPECT_EQ(CountTotalItemQuantityInBag(forms[f].item), 0);
                SetMonData(&mon, MON_DATA_HELD_ITEM, &forms[f].item);
                EXPECT_EQ(ApplyEmeraldChampionsBattleSetChoiceForFormat(&mon, choice, format), EC_BATTLE_SET_MEGA_STONE_HELD);
                EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), forms[f].species);
                EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), forms[f].item);
                EXPECT_EQ(GetEmeraldChampionsCurrentBattleSetChoiceForFormat(&mon, format), choice);
                megaChoices++;
            }
            EXPECT_EQ(megaChoices, 2);
        }
    }
    ClearBag();
}

TEST("Emerald Champions Primal tutor roles require their own orb and preserve owned relics")
{
    static const enum Species species[] = {SPECIES_KYOGRE, SPECIES_GROUDON};
    static const enum Item orbs[] = {ITEM_BLUE_ORB, ITEM_RED_ORB};

    for (u32 s = 0; s < ARRAY_COUNT(species); s++)
    {
        struct Pokemon mon;
        u32 ordinaryCount;
        u32 primalCount = 0;

        ClearBag();
        CreateMon(&mon, species[s], 50, 0, OTID_STRUCT_PLAYER_ID);
        ordinaryCount = GetEmeraldChampionsBattleSetCount(&mon);
        EXPECT(AddBagItem(ITEM_MEGA_RING, 1));
        EXPECT(AddBagItem(orbs[1 - s], 1));
        EXPECT_EQ(GetEmeraldChampionsBattleSetCount(&mon), ordinaryCount);
        EXPECT(AddBagItem(orbs[s], 1));
        EXPECT_GT(GetEmeraldChampionsBattleSetCount(&mon), ordinaryCount);
        for (u32 choice = 0; choice < GetEmeraldChampionsBattleSetCount(&mon); choice++)
        {
            if (GetEmeraldChampionsBattleSetRequiredItem(&mon, choice) != orbs[s])
                continue;
            primalCount++;
            EXPECT_EQ(ApplyEmeraldChampionsBattleSetChoice(&mon, choice), EC_BATTLE_SET_MEGA);
            EXPECT_NE(GetMonData(&mon, MON_DATA_HELD_ITEM), orbs[s]);
            EXPECT_EQ(CountTotalItemQuantityInBag(orbs[s]), 1);
            SetMonData(&mon, MON_DATA_HELD_ITEM, &orbs[s]);
            EXPECT(RemoveBagItem(orbs[s], 1));
            EXPECT_EQ(ApplyEmeraldChampionsBattleSetChoice(&mon, choice), EC_BATTLE_SET_MEGA_STONE_HELD);
            EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), orbs[s]);
            EXPECT_EQ(CountTotalItemQuantityInBag(orbs[s]), 0);
            EXPECT_EQ(GetEmeraldChampionsCurrentBattleSetChoice(&mon), choice);
            EXPECT(AddBagItem(orbs[s], 1));
            enum Item noItem = ITEM_NONE;
            SetMonData(&mon, MON_DATA_HELD_ITEM, &noItem);
        }
        EXPECT_GT(primalCount, 0);
        EXPECT_EQ(GetEmeraldChampionsBattleSetCount(&mon), ordinaryCount + primalCount);
    }
    ClearBag();
}

TEST("Emerald Champions tutor recognizes and reopens on the current battle set")
{
    struct Pokemon mon;
    enum Move firstMove;
    enum Move secondMove;
    u8 changedNature;

    ClearBag();
    CreateMon(&mon, SPECIES_GEODUDE, 30, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_GE(GetEmeraldChampionsBattleSetCount(&mon), 2);

    for (u8 choice = 0; choice < GetEmeraldChampionsBattleSetCount(&mon); choice++)
    {
        EXPECT_EQ(ApplyEmeraldChampionsBattleSetChoice(&mon, choice), EC_BATTLE_SET_SUCCESS);
        EXPECT_EQ(GetEmeraldChampionsCurrentBattleSetChoice(&mon), choice);

        // Reordering moves is presentation-only and must not make the tutor
        // forget which authored orientation the Pokémon is using.
        firstMove = GetMonData(&mon, MON_DATA_MOVE1);
        secondMove = GetMonData(&mon, MON_DATA_MOVE2);
        SetMonMoveSlot(&mon, secondMove, 0);
        SetMonMoveSlot(&mon, firstMove, 1);
        EXPECT_EQ(GetEmeraldChampionsCurrentBattleSetChoice(&mon), choice);

        changedNature = (GetMonData(&mon, MON_DATA_HIDDEN_NATURE) + 1) % NUM_NATURES;
        SetMonData(&mon, MON_DATA_HIDDEN_NATURE, &changedNature);
        EXPECT_EQ(GetEmeraldChampionsCurrentBattleSetChoice(&mon), -1);
    }
}

TEST("Emerald Champions role and Ability chooser names fit their bounded windows")
{
    ClearBag();
    EXPECT(AddBagItem(ITEM_MEGA_RING, 1));

    for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        struct Pokemon mon;

        if (gSpeciesInfo[species].baseHP == 0)
            continue;
        CreateMon(&mon, species, 50, 0, OTID_STRUCT_PLAYER_ID);
        for (u32 choice = 0; choice < GetEmeraldChampionsBattleSetCount(&mon); choice++)
        {
            const u8 *name = GetEmeraldChampionsBattleSetName(&mon, choice);
            u32 width = GetStringWidth(FONT_NORMAL, name, 0);

            EXPECT_LE(ConvertPixelWidthToTileWidth(width), MAX_MULTICHOICE_WIDTH);
            EXPECT_LE(width, (MAX_MULTICHOICE_WIDTH - 2) * TILE_WIDTH);
        }
        for (u32 abilitySlot = 0; abilitySlot < NUM_ABILITY_SLOTS; abilitySlot++)
        {
            enum Ability ability = GetAbilityBySpecies(species, abilitySlot);

            if (ability != ABILITY_NONE)
                EXPECT_LE(GetStringWidth(FONT_NORMAL, gAbilitiesInfo[ability].name, 0), 14 * TILE_WIDTH);
        }
    }
    ClearBag();
}

TEST("Emerald Champions Zygardite preset preserves the staged Power Construct path")
{
    struct Pokemon mon;
    bool32 foundMegaRole = FALSE;

    ClearBag();
    EXPECT(AddBagItem(ITEM_MEGA_RING, 1));
    CreateMon(&mon, SPECIES_ZYGARDE_50_POWER_CONSTRUCT, 70, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GetMonAbility(&mon), ABILITY_POWER_CONSTRUCT);

    for (u8 choice = 0; choice < GetEmeraldChampionsBattleSetCount(&mon); choice++)
    {
        if (GetEmeraldChampionsBattleSetRequiredItem(&mon, choice) != ITEM_ZYGARDITE)
            continue;
        foundMegaRole = TRUE;
        EXPECT_EQ(ApplyEmeraldChampionsBattleSetChoice(&mon, choice), EC_BATTLE_SET_MEGA);
        // Power Construct must remain active so this form can become Complete
        // below half HP; only Complete Zygarde can use Zygardite afterward.
        EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_ZYGARDE_50_POWER_CONSTRUCT);
        EXPECT_EQ(GetMonAbility(&mon), ABILITY_POWER_CONSTRUCT);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), ITEM_NONE);
    }
    EXPECT(foundMegaRole);
    ClearBag();
}

TEST("Emerald Champions Game Corner rejects the initially chosen starter")
{
    ResetEmeraldChampionsGameCornerTestState();
    gSpecialVar_0x8004 = SPECIES_BULBASAUR;

    IsEmeraldChampionsGameCornerPokemonClaimed();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    GiveEmeraldChampionsGameCornerPokemon();
    EXPECT_EQ(gSpecialVar_Result, EC_GAME_CORNER_PRIZE_SET_FAILED);
    EXPECT(!FlagGet(FLAG_EC_STARTER_ARCHIVE_BULBASAUR));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_NONE);
}

TEST("Emerald Champions Game Corner delivers a natural alternate starter transactionally")
{
    ResetEmeraldChampionsGameCornerTestState();
    SeedRng(7);
    gSpecialVar_0x8004 = SPECIES_CHARMANDER;

    IsEmeraldChampionsGameCornerPokemonClaimed();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    GiveEmeraldChampionsGameCornerPokemon();
    EXPECT_EQ(gSpecialVar_Result, MON_GIVEN_TO_PARTY);
    EXPECT(FlagGet(FLAG_EC_STARTER_ARCHIVE_CHARMANDER));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_CHARMANDER);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), GetCurrentLevelCap());
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP_EV), 0);
}

TEST("Emerald Champions Game Corner rejects a repeated archive claim")
{
    ResetEmeraldChampionsGameCornerTestState();
    gSpecialVar_0x8004 = SPECIES_CHARMANDER;
    GiveEmeraldChampionsGameCornerPokemon();
    EXPECT_EQ(gSpecialVar_Result, MON_GIVEN_TO_PARTY);
    EXPECT(FlagGet(FLAG_EC_STARTER_ARCHIVE_CHARMANDER));

    GiveEmeraldChampionsGameCornerPokemon();
    EXPECT_EQ(gSpecialVar_Result, EC_GAME_CORNER_PRIZE_SET_FAILED);
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], 1);
}

TEST("Emerald Champions Game Corner keeps a full-storage claim retryable")
{
    ResetEmeraldChampionsGameCornerTestState();
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        CreateMon(&gParties[B_TRAINER_PLAYER][slot], SPECIES_RATTATA, 5, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
    {
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            CreateBoxMon(&gPokemonStoragePtr->boxes[box][slot], SPECIES_RATTATA, 5, 0, OTID_STRUCT_PLAYER_ID);
    }
    gSpecialVar_0x8004 = SPECIES_SQUIRTLE;

    GiveEmeraldChampionsGameCornerPokemon();
    EXPECT_EQ(gSpecialVar_Result, MON_CANT_GIVE);
    EXPECT(!FlagGet(FLAG_EC_STARTER_ARCHIVE_SQUIRTLE));

    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
    ZeroPlayerPartyMons();
}

TEST("Emerald Champions Game Corner rejects invalid or presetless prizes")
{
    enum Species presetless = SPECIES_NONE;

    ResetEmeraldChampionsGameCornerTestState();
    for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        if (gSpeciesInfo[species].baseHP != 0 && GetEmeraldChampionsRawBattleSetCount(species) == 0)
        {
            presetless = species;
            break;
        }
    }
    EXPECT_EQ(
        GiveEmeraldChampionsGameCornerPokemonForTesting(presetless, FLAG_EC_STARTER_ARCHIVE_QUAXLY),
        EC_GAME_CORNER_PRIZE_SET_FAILED);
    EXPECT(!FlagGet(FLAG_EC_STARTER_ARCHIVE_QUAXLY));
    EXPECT_EQ(
        GiveEmeraldChampionsGameCornerPokemonForTesting(SPECIES_NONE, FLAG_EC_STARTER_ARCHIVE_QUAXLY),
        EC_GAME_CORNER_PRIZE_SET_FAILED);
    EXPECT(!FlagGet(FLAG_EC_STARTER_ARCHIVE_QUAXLY));
}

TEST("Emerald Champions story gifts preserve natural builds and empty held items")
{
    static const enum Species species[] =
    {
        SPECIES_CASTFORM_NORMAL,
        SPECIES_BELDUM,
        SPECIES_LILEEP,
    };
    static const u8 levels[] = {25, 5, 20};
    struct BattleStruct *savedBattleStruct = gBattleStruct;

    ZeroPlayerPartyMons();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
    memset(&sEmeraldChampionsTestBattleStruct, 0, sizeof(sEmeraldChampionsTestBattleStruct));
    gBattleStruct = &sEmeraldChampionsTestBattleStruct;
    SeedRng(11);
    for (u32 slot = 0; slot < ARRAY_COUNT(species); slot++)
    {
        enum Item item;
        enum Item restorationItem;

        EXPECT_EQ(
            GiveEmeraldChampionsPreparedPokemonForTesting(species[slot], levels[slot]),
            MON_GIVEN_TO_PARTY
        );
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_SPECIES), species[slot]);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_LEVEL),
            min(levels[slot], GetPlayerLevelCapForSpecies(species[slot])));
        EXPECT_EQ(GetMonEVCount(&gParties[B_TRAINER_PLAYER][slot]), 0);
        item = GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HELD_ITEM);
        EXPECT_EQ(item, ITEM_NONE);
        restorationItem = gBattleStruct->itemLost[B_TRAINER_PLAYER][slot].originalItem;
        EXPECT_EQ(restorationItem, item);
    }
    gBattleStruct = savedBattleStruct;
}

TEST("Emerald Champions prepared story gifts preserve PC delivery and no-room retries")
{
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        CreateMon(&gParties[B_TRAINER_PLAYER][slot], SPECIES_RATTATA, 5, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
    {
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            CreateBoxMon(&gPokemonStoragePtr->boxes[box][slot], SPECIES_RATTATA, 5, 0, OTID_STRUCT_PLAYER_ID);
    }

    EXPECT_EQ(
        GiveEmeraldChampionsPreparedPokemonForTesting(SPECIES_BELDUM, 5),
        MON_CANT_GIVE
    );

    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
    SeedRng(13);
    EXPECT_EQ(
        GiveEmeraldChampionsPreparedPokemonForTesting(SPECIES_BELDUM, 5),
        MON_GIVEN_TO_PC
    );
    EXPECT_EQ(
        GetBoxMonData(&gPokemonStoragePtr->boxes[0][0], MON_DATA_SPECIES),
        SPECIES_BELDUM
    );
    EXPECT_EQ(GetBoxMonData(&gPokemonStoragePtr->boxes[0][0], MON_DATA_HELD_ITEM), ITEM_NONE);
    for (u32 stat = 0; stat < NUM_STATS; stat++)
        EXPECT_EQ(GetBoxMonData(&gPokemonStoragePtr->boxes[0][0], MON_DATA_HP_EV + stat), 0);

    ZeroPlayerPartyMons();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
}

TEST("Emerald Champions legendary requirements accept the whole evolution family")
{
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_MUNNA, 20, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT(PlayerPartyHasSpeciesFamily(SPECIES_MUSHARNA));
    EXPECT(!PlayerPartyHasSpeciesFamily(SPECIES_LUCARIO));

    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_RIOLU, 20, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT(PlayerPartyHasSpeciesFamily(SPECIES_LUCARIO));

    CreateMon(&gParties[B_TRAINER_PLAYER][2], SPECIES_TAUROS_PALDEA_BLAZE, 20, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT(PlayerPartyHasSpeciesFamily(SPECIES_TAUROS));
}



TEST("Emerald Champions persists appended legendary sign bits")
{
    static const u16 vars[] =
    {
        VAR_LEGENDARY_SIGNS_UNLOCKED_0,
        VAR_LEGENDARY_SIGNS_UNLOCKED_1,
        VAR_LEGENDARY_SIGNS_UNLOCKED_2,
        VAR_LEGENDARY_SIGNS_UNLOCKED_3,
        VAR_LEGENDARY_SIGNS_UNLOCKED_4,
        VAR_LEGENDARY_SIGNS_UNLOCKED_5,
        VAR_LEGENDARY_SIGNS_CAUGHT_0,
        VAR_LEGENDARY_SIGNS_CAUGHT_1,
        VAR_LEGENDARY_SIGNS_CAUGHT_2,
        VAR_LEGENDARY_SIGNS_CAUGHT_3,
        VAR_LEGENDARY_SIGNS_CAUGHT_4,
        VAR_LEGENDARY_SIGNS_CAUGHT_5,
    };

    for (u32 i = 0; i < ARRAY_COUNT(vars); i++)
        VarSet(vars[i], 0);
    UnlockLegendarySign(LEGENDARY_SIGN_KELDEO);
    EXPECT(IsLegendarySignUnlocked(LEGENDARY_SIGN_KELDEO));
    EXPECT(!IsLegendarySignCaught(LEGENDARY_SIGN_KELDEO));
    MarkLegendarySignCaughtBySpecies(SPECIES_KELDEO);
    EXPECT(IsLegendarySignCaught(LEGENDARY_SIGN_KELDEO));
    EXPECT(!IsLegendarySignCaught(LEGENDARY_SIGN_ARCEUS));
}

TEST("Emerald Champions Arceus gift waits only for the Hall of Fame")
{
    ZeroPlayerPartyMons();
    for (u32 i = 0; i < ARRAY_COUNT(sEmeraldChampionsTestSignStateVars); i++)
        VarSet(sEmeraldChampionsTestSignStateVars[i], 0);
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    FlagClear(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
    FlagClear(FLAG_IS_CHAMPION);

    TryGiveArceusLegendarySignMasteryReward();
    EXPECT_EQ(gSpecialVar_Result, 0);
    // The former badge-and-crisis gate no longer opens it.
    FlagSet(FLAG_BADGE08_GET);
    FlagSet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
    TryGiveArceusLegendarySignMasteryReward();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_NONE);
    FlagClear(FLAG_BADGE08_GET);
    FlagClear(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);

    FlagSet(FLAG_IS_CHAMPION);
    TryGiveArceusLegendarySignMasteryReward();
    EXPECT_EQ(gSpecialVar_Result, 1);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_ARCEUS);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), GetCurrentLevelCap());
    TryGiveArceusLegendarySignMasteryReward();
    EXPECT_EQ(gSpecialVar_Result, 4);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SPECIES), SPECIES_NONE);
    FlagClear(FLAG_IS_CHAMPION);
    ZeroPlayerPartyMons();
}

TEST("Emerald Champions Ogerpon mask roles require owned masks without a Mega Ring")
{
    static const enum Item masks[] = {ITEM_WELLSPRING_MASK, ITEM_HEARTHFLAME_MASK, ITEM_CORNERSTONE_MASK};
    static const enum Species forms[] = {SPECIES_OGERPON_WELLSPRING, SPECIES_OGERPON_HEARTHFLAME, SPECIES_OGERPON_CORNERSTONE};
    u32 exercised = 0;

    for (u32 format = 0; format < EC_BATTLE_FORMAT_COUNT; format++)
    {
        for (u32 m = 0; m < ARRAY_COUNT(masks); m++)
        {
            struct Pokemon mon;
            u32 matches = 0;
            ClearBag();
            CreateMon(&mon, SPECIES_OGERPON, 50, 0, OTID_STRUCT_PLAYER_ID);
            EXPECT(IsEmeraldChampionsProtectedProgressionItem(masks[m]));
            EXPECT(AddBagItem(ITEM_MEGA_RING, 1));
            for (u32 choice = 0; choice < GetEmeraldChampionsBattleSetCountForFormat(&mon, format); choice++)
                EXPECT_EQ(GetEmeraldChampionsBattleSetRequiredItemForFormat(&mon, choice, format), ITEM_NONE);
            ClearBag();
            EXPECT(AddBagItem(masks[m], 1));
            u32 count = GetEmeraldChampionsBattleSetCountForFormat(&mon, format);
            for (u32 choice = 0; choice < count; choice++)
            {
                CreateMon(&mon, SPECIES_OGERPON, 50, 0, OTID_STRUCT_PLAYER_ID);
                if (GetEmeraldChampionsBattleSetRequiredItemForFormat(&mon, choice, format) != masks[m])
                    continue;
                const struct EmeraldChampionsBattleSet *preset = GetEmeraldChampionsBattleSetPresetForFormat(&mon, choice, format);
                EXPECT_EQ(ApplyEmeraldChampionsBattleSetChoiceForFormat(&mon, choice, format), EC_BATTLE_SET_MEGA);
                EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), ITEM_NONE);
                EXPECT_EQ(CountTotalItemQuantityInBag(masks[m]), 1);
                SetMonData(&mon, MON_DATA_HELD_ITEM, &masks[m]);
                EXPECT(RemoveBagItem(masks[m], 1));
                TryFormChange(&mon, FORM_CHANGE_ITEM_HOLD, B_TRAINER_PLAYER);
                EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), forms[m]);
                EXPECT_EQ(ApplyEmeraldChampionsBattleSetChoiceForFormat(&mon, choice, format), EC_BATTLE_SET_MEGA_STONE_HELD);
                EXPECT_EQ(GetMonAbility(&mon), preset->ability);
                EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), masks[m]);
                EXPECT_EQ(CountTotalItemQuantityInBag(masks[m]), 0);
                EXPECT_EQ(GetEmeraldChampionsCurrentBattleSetChoiceForFormat(&mon, format), choice);
                EXPECT(AddBagItem(masks[m], 1));
                matches++;
                exercised++;
            }
            EXPECT_EQ(matches, 2);
        }
    }
    EXPECT_EQ(exercised, 12);
    ClearBag();
}

TEST("Emerald Champions free Memories remain protected and require an equipped item")
{
    u32 exercised = 0;
    for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        const struct EmeraldChampionsBattleSetRange *range = &gEmeraldChampionsBattleSetRanges[EC_BATTLE_FORMAT_DOUBLES][species];
        enum Item item = range->count == 0 ? ITEM_NONE : gEmeraldChampionsBattleSets[range->offset].preset.item;
        if (item == ITEM_NONE || gItemsInfo[item].sortType != ITEM_TYPE_MEMORY)
            continue;
        struct Pokemon mon;
        ClearBag();
        CreateMon(&mon, species, 50, 0, OTID_STRUCT_PLAYER_ID);
        EXPECT(IsEmeraldChampionsProtectedProgressionItem(item));
        EXPECT_EQ(GetEmeraldChampionsBattleSetCount(&mon), 0);
        EXPECT_EQ(ApplyEmeraldChampionsBattleSetChoice(&mon, 0), EC_BATTLE_SET_FAILED);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), ITEM_NONE);
        SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
        EXPECT_GT(GetEmeraldChampionsBattleSetCount(&mon), 0);
        EXPECT_EQ(ApplyEmeraldChampionsBattleSetChoice(&mon, 0), EC_BATTLE_SET_SUCCESS);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), item);
        EXPECT_EQ(CountTotalItemQuantityInBag(item), 0);
        exercised++;
    }
    EXPECT_EQ(exercised, 17);
    ClearBag();
}

TEST("Emerald Champions imported battle sets remain legal against current data")
{
    struct Pokemon mon;
    u32 abilityFallbacks = 0;

    for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        u8 count = GetEmeraldChampionsRawBattleSetCount(species);
        for (u8 choice = 0; choice < count; choice++)
        {
            const struct EmeraldChampionsBattleSet *preset = GetEmeraldChampionsRawBattleSet(species, choice);
            u32 evTotal = 0;
            u8 result;

            CreateMon(&mon, species, 50, 0, OTID_STRUCT_PLAYER_ID);
            result = ApplyEmeraldChampionsOpponentSet(&mon, choice);
            if (result != EC_BATTLE_SET_SUCCESS && result != EC_BATTLE_SET_MEGA)
                Test_MgbaPrintf("illegal imported set species=%d choice=%d ability=%d item=%d required=%d", species, choice, preset->ability, preset->item, preset->requiredItem);
            EXPECT(result == EC_BATTLE_SET_SUCCESS || result == EC_BATTLE_SET_MEGA);
            EXPECT_NE(GetMonAbility(&mon), ABILITY_NONE);
            if (gEmeraldChampionsBattleSetRanges[EC_BATTLE_FORMAT_DOUBLES][species].count != 0
             && preset->requiredItem == ITEM_NONE
             && preset->requiredMove == MOVE_NONE)
            {
                if (GetMonAbility(&mon) != preset->ability)
                {
                    Test_MgbaPrintf("preset ability fallback species=%d choice=%d expected=%d actual=%d",
                                    species, choice, preset->ability, GetMonAbility(&mon));
                    abilityFallbacks++;
                }
            }
            EXPECT_EQ(GetMonData(&mon, MON_DATA_HIDDEN_NATURE), preset->nature);
            for (u32 stat = 0; stat < NUM_STATS; stat++)
                evTotal += preset->evs[stat];
            EXPECT_LE(evTotal, MAX_TOTAL_EVS);
            if (preset->requiredItem != ITEM_NONE)
                EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), preset->requiredItem);
        }
    }
    EXPECT_EQ(abilityFallbacks, 0);
}

TEST("Emerald Champions reviewed move-access exceptions are natively tutor-accessible")
{
    EXPECT_EQ(ARRAY_COUNT(sReviewedMoveAccess), EC_REVIEWED_MOVE_ACCESS_COUNT);
    for (u32 i = 0; i < ARRAY_COUNT(sReviewedMoveAccess); i++)
    {
        bool32 accessible = SpeciesCanAccessEmeraldChampionsPresetMove(
            sReviewedMoveAccess[i].species,
            sReviewedMoveAccess[i].move
        );
        if (!accessible)
        {
            Test_MgbaPrintf(
                "reviewed move inaccessible index=%d species=%d move=%d",
                i,
                sReviewedMoveAccess[i].species,
                sReviewedMoveAccess[i].move
            );
        }
        EXPECT(accessible);
    }
}

TEST("Emerald Champions covers every ordinary species and form")
{
    u32 missing = 0;
    u32 missingSecondNonMega = 0;

    for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        if (IsEmeraldChampionsOrdinaryWildSpecies(species)
         && GetEmeraldChampionsRawBattleSetCount(species) == 0)
            missing++;
        if (IsEmeraldChampionsOrdinaryWildSpecies(species))
        {
            u32 nonMegaCount = 0;

            for (u8 choice = 0; choice < GetEmeraldChampionsRawBattleSetCount(species); choice++)
            {
                const struct EmeraldChampionsBattleSet *preset =
                    GetEmeraldChampionsRawBattleSet(species, choice);
                nonMegaCount += preset != NULL && preset->requiredItem == ITEM_NONE;
            }
            if (nonMegaCount < 2)
                missingSecondNonMega++;
        }
    }
    EXPECT_EQ(missing, 0);
    EXPECT_EQ(missingSecondNonMega, 0);
}

TEST("Emerald Champions exposes two pre-Mega roles for every direct set row")
{
    u32 rowsBelowMinimum = 0;

    for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        u32 nonMegaCount = 0;

        if (gEmeraldChampionsBattleSetRanges[EC_BATTLE_FORMAT_DOUBLES][species].count == 0)
            continue;
        for (u8 choice = 0; choice < GetEmeraldChampionsRawBattleSetCount(species); choice++)
        {
            const struct EmeraldChampionsBattleSet *preset =
                GetEmeraldChampionsRawBattleSet(species, choice);

            nonMegaCount += preset != NULL && preset->requiredItem == ITEM_NONE;
        }
        if (nonMegaCount < 2)
            rowsBelowMinimum++;
    }
    EXPECT_EQ(rowsBelowMinimum, 0);
}

TEST("Champions Circuit entry requires six healthy non-Egg Pokemon")
{
    bool8 isEgg = TRUE;
    u16 hp = 0;

    ZeroPlayerPartyMons();
    ChampionsCircuitCanEnter();
    EXPECT_EQ(gSpecialVar_Result, FALSE);

    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][slot], SPECIES_BULBASAUR, 20, 0, OTID_STRUCT_PLAYER_ID, MAX_PER_STAT_IVS);
    ChampionsCircuitCanEnter();
    EXPECT_EQ(gSpecialVar_Result, TRUE);

    SetMonData(&gParties[B_TRAINER_PLAYER][PARTY_SIZE - 1], MON_DATA_HP, &hp);
    ChampionsCircuitCanEnter();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    HealPokemon(&gParties[B_TRAINER_PLAYER][PARTY_SIZE - 1]);

    SetMonData(&gParties[B_TRAINER_PLAYER][PARTY_SIZE - 1], MON_DATA_IS_EGG, &isEgg);
    ChampionsCircuitCanEnter();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
}

TEST("Champions Circuit win and loss transitions preserve counters and restore the party")
{
    u16 damagedHp = 1;
    ZeroPlayerPartyMons();
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][slot], SPECIES_BULBASAUR, 20, 0, OTID_STRUCT_PLAYER_ID, MAX_PER_STAT_IVS);
    gPartiesCount[B_TRAINER_PLAYER] = PARTY_SIZE;
    ChampionsCircuitBegin();
    VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, 5);
    VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 9);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP, &damagedHp);

    gBattleOutcome = B_OUTCOME_WON;
    ChampionsCircuitHandleBattleResult();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS), 6);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS), 10);
    EXPECT_GT(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), damagedHp);
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HP), GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_MAX_HP));

    gBattleOutcome = B_OUTCOME_LOST;
    ChampionsCircuitHandleBattleResult();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_ACTIVE), FALSE);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS), 0);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS), 10);
    // Full prepared-party byte restoration is checked by its dedicated test.
}

TEST("Champions Circuit no longer grants legendary rewards")
{
    ZeroPlayerPartyMons();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
    ClearEmeraldChampionsLegendaryCaughtState();
    VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, 0);
    VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 40);

    gSpecialVar_Result = 0xFFFF;
    ChampionsCircuitTryGiveReward();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT_EQ(GetBoxMonData(&gPokemonStoragePtr->boxes[0][0], MON_DATA_SPECIES), SPECIES_NONE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_NONE);
    EXPECT(!IsLegendarySignCaught(LEGENDARY_SIGN_CALYREX));
    EXPECT(!IsLegendarySignCaught(LEGENDARY_SIGN_ETERNATUS));
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS), 40);
    VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 0);
}

TEST("Champions Circuit honors the live difficulty level reduction")
{
    static const enum DifficultyLevel difficulties[] =
    {
        DIFFICULTY_HARD,
        DIFFICULTY_NORMAL,
        DIFFICULTY_EASY,
    };
    static const u8 expectedLevels[] = {100, 99, 96};

    VarSet(VAR_CHAMPIONS_CIRCUIT_ACTIVE, TRUE);
    VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, 0);
    for (u32 i = 0; i < ARRAY_COUNT(difficulties); i++)
    {
        SetCurrentDifficultyLevel(difficulties[i]);
        SeedRng(7);
        ChampionsCircuitGenerateOpponent();
        EXPECT_EQ(gSpecialVar_Result, PARTY_SIZE);
        for (u32 slot = 0; slot < PARTY_SIZE; slot++)
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_LEVEL), expectedLevels[i]);
    }
    SetCurrentDifficultyLevel(DIFFICULTY_HARD);
    VarSet(VAR_CHAMPIONS_CIRCUIT_ACTIVE, FALSE);
}

TEST("Champions Circuit assembles complete competitive sets across 2048 seeds")
{
    for (u32 seed = 1; seed <= 2048; seed++)
    PARAMETRIZE_LABEL("seed=%d", seed)
    {
        u32 speedControl = 0, physical = 0, special = 0, megaStones = 0;
        VarSet(VAR_CHAMPIONS_CIRCUIT_ACTIVE, TRUE);
        VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, seed);
        SetCurrentDifficultyLevel(DIFFICULTY_HARD);
        SeedRng(seed);
        ChampionsCircuitGenerateOpponent();
        EXPECT_EQ(gSpecialVar_Result, PARTY_SIZE);
        EXPECT_EQ(gPartiesCount[B_TRAINER_OPPONENT_A], PARTY_SIZE);
        for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        {
            struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][slot];
            enum Species species = GetMonData(mon, MON_DATA_SPECIES);
            enum Item item = GetMonData(mon, MON_DATA_HELD_ITEM);
            u32 moves = 0, status = 0, points = 0;
            bool32 reachedEmptyMove = FALSE;
            EXPECT_NE(species, SPECIES_NONE);
            EXPECT_NE(GetMonAbility(mon), ABILITY_NONE);
            megaStones += gItemsInfo[item].sortType == ITEM_TYPE_MEGA_STONE;
            EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), GetChampionsCircuitOpponentLevel(seed, slot));
            EXPECT_GT(GetMonData(mon, MON_DATA_HP), 0);
            if (slot == PARTY_SIZE - 1)
                EXPECT_NE(GetMonAbility(mon), ABILITY_ILLUSION);
            for (u32 stat = 0; stat < NUM_STATS; stat++)
            {
                u32 investment = GetMonData(mon, MON_DATA_HP_EV + stat);
                EXPECT_LE(investment, MAX_PER_STAT_EVS);
                points += investment;
            }
            EXPECT_LE(points, MAX_TOTAL_EVS);
            for (u32 m = 0; m < MAX_MON_MOVES; m++)
            {
                enum Move move = GetMonData(mon, MON_DATA_MOVE1 + m);
                if (move == MOVE_NONE)
                {
                    reachedEmptyMove = TRUE;
                    continue;
                }
                EXPECT(!reachedEmptyMove);
                moves++;
                EXPECT_GT(GetMonData(mon, MON_DATA_PP1 + m), 0);
                for (u32 earlier = 0; earlier < m; earlier++)
                    EXPECT_NE(move, GetMonData(mon, MON_DATA_MOVE1 + earlier));
                status += GetMoveCategory(move) == DAMAGE_CATEGORY_STATUS;
                physical += GetMoveCategory(move) == DAMAGE_CATEGORY_PHYSICAL;
                special += GetMoveCategory(move) == DAMAGE_CATEGORY_SPECIAL;
                switch (move)
                {
                case MOVE_ELECTROWEB: case MOVE_GLARE: case MOVE_ICY_WIND:
                case MOVE_NUZZLE: case MOVE_QUASH: case MOVE_TAILWIND:
                case MOVE_THUNDER_WAVE: case MOVE_TRICK_ROOM:
                    speedControl++;
                    break;
                default:
                    break;
                }
                if (item == ITEM_CHOICE_SCARF || item == ITEM_CHOICE_BAND || item == ITEM_CHOICE_SPECS)
                {
                    if (move == MOVE_PROTECT)
                        Test_MgbaPrintf("Circuit choice conflict: species=%d item=%d moves=%d/%d/%d/%d", species, item,
                            GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                            GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
                    EXPECT_NE(move, MOVE_PROTECT);
                    EXPECT_NE(move, MOVE_DETECT);
                    EXPECT_NE(move, MOVE_SWORDS_DANCE);
                    EXPECT_NE(move, MOVE_NASTY_PLOT);
                    EXPECT_NE(move, MOVE_CALM_MIND);
                }
            }
            EXPECT_GE(moves, 1);
            if (item == ITEM_ASSAULT_VEST)
                EXPECT_EQ(status, 0);
            for (u32 earlier = 0; earlier < slot; earlier++)
                EXPECT_NE(SpeciesToNationalPokedexNum(species), SpeciesToNationalPokedexNum(
                    GetMonData(&gParties[B_TRAINER_OPPONENT_A][earlier], MON_DATA_SPECIES)));
        }
        EXPECT_GT(speedControl, 0);
        EXPECT_GT(physical, 0);
        EXPECT_GT(special, 0);
        EXPECT_LE(megaStones, 1);
        VarSet(VAR_CHAMPIONS_CIRCUIT_ACTIVE, FALSE);
    }
}

TEST("Champions Circuit levels add one point per win and saturate without wrapping")
{
    static const u16 streaks[] = {0, 1, 5, 6, 7, 929, 930, 931, 65535};
    SetCurrentDifficultyLevel(DIFFICULTY_HARD);
    for (u32 index = 0; index < ARRAY_COUNT(streaks); index++)
    {
        u32 total = 0;
        for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        {
            u32 level = GetChampionsCircuitOpponentLevel(streaks[index], slot);
            EXPECT_GE(level, 100);
            EXPECT_LE(level, 255);
            total += level;
        }
        EXPECT_EQ(total, min(600 + streaks[index], 1530));
    }
}

TEST("Champions Circuit overlevel stats survive form recalculation without changing EXP")
{
    struct Pokemon *opponent = &gParties[B_TRAINER_OPPONENT_A][0];
    static const u8 levels[] = {101, 150, 255};
    enum Species mega = SPECIES_GARCHOMP_MEGA;
    u32 exp, previousStats[NUM_STATS];

    CreateMonWithIVs(opponent, SPECIES_GARCHOMP, 100, 0, OTID_STRUCT_PLAYER_ID, MAX_PER_STAT_IVS);
    exp = GetMonData(opponent, MON_DATA_EXP);
    for (u32 field = MON_DATA_MAX_HP; field <= MON_DATA_SPDEF; field++)
        previousStats[field - MON_DATA_MAX_HP] = GetMonData(opponent, field);
    VarSet(VAR_CHAMPIONS_CIRCUIT_ACTIVE, TRUE);
    for (u32 i = 0; i < ARRAY_COUNT(levels); i++)
    {
        SetMonData(opponent, MON_DATA_LEVEL, &levels[i]);
        CalculateMonStats(opponent);
        EXPECT_EQ(GetMonData(opponent, MON_DATA_LEVEL), levels[i]);
        EXPECT_EQ(GetMonData(opponent, MON_DATA_EXP), exp);
        for (u32 field = MON_DATA_MAX_HP; field <= MON_DATA_SPDEF; field++)
        {
            u32 stat = GetMonData(opponent, field);
            EXPECT_GT(stat, previousStats[field - MON_DATA_MAX_HP]);
            previousStats[field - MON_DATA_MAX_HP] = stat;
        }
        CalculateMonStats(opponent);
        for (u32 field = MON_DATA_MAX_HP; field <= MON_DATA_SPDEF; field++)
            EXPECT_EQ(GetMonData(opponent, field), previousStats[field - MON_DATA_MAX_HP]);
    }
    SetMonData(opponent, MON_DATA_SPECIES, &mega);
    CalculateMonStats(opponent);
    EXPECT_EQ(GetMonData(opponent, MON_DATA_LEVEL), 255);
    EXPECT_EQ(GetMonData(opponent, MON_DATA_EXP), exp);

    // The same cached level in the player's party cannot bypass progression.
    gParties[B_TRAINER_PLAYER][0] = *opponent;
    CalculateMonStats(&gParties[B_TRAINER_PLAYER][0]);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 100);
    VarSet(VAR_CHAMPIONS_CIRCUIT_ACTIVE, FALSE);
    CalculateMonStats(opponent);
    EXPECT_EQ(GetMonData(opponent, MON_DATA_LEVEL), 100);
}

TEST("Champions Circuit variant families are contiguous and retain a base form")
{
    static EWRAM_DATA bool8 closedFamilies[NATIONAL_DEX_COUNT + 1];
    enum NationalDexOrder current = NATIONAL_DEX_NONE;
    bool32 currentHasOrdinary = FALSE;

    memset(closedFamilies, 0, sizeof(closedFamilies));
    for (u32 i = 0; i < SHOWDOWN_CIRCUIT_VARIANT_COUNT; i++)
    {
        enum NationalDexOrder dex = SpeciesToNationalPokedexNum(gShowdownCircuitVariants[i].partySpecies);

        if (dex != current)
        {
            if (current != NATIONAL_DEX_NONE)
            {
                EXPECT(currentHasOrdinary);
                closedFamilies[current] = TRUE;
            }
            EXPECT(!closedFamilies[dex]);
            current = dex;
            currentHasOrdinary = FALSE;
        }
        if (gShowdownCircuitVariants[i].requiredItem == ITEM_NONE)
            currentHasOrdinary = TRUE;
    }
    EXPECT(currentHasOrdinary);
}

TEST("Champions Circuit templates use configured legal Abilities")
{
    for (u32 variantIndex = 0; variantIndex < SHOWDOWN_CIRCUIT_VARIANT_COUNT; variantIndex++)
    {
        const struct ShowdownCircuitVariant *variant = &gShowdownCircuitVariants[variantIndex];
        EXPECT_GT(variant->templateCount, 0);

        for (u32 templateIndex = variant->templateOffset;
             templateIndex < variant->templateOffset + variant->templateCount;
             templateIndex++)
        {
            const struct ShowdownCircuitTemplate *template = &gShowdownCircuitTemplates[templateIndex];
            EXPECT_GT(template->abilityCount, 0);

            for (u32 abilityIndex = 0; abilityIndex < template->abilityCount; abilityIndex++)
            {
                enum Ability ability = template->abilities[abilityIndex];
                bool32 found = FALSE;
                EXPECT_NE(ability, ABILITY_NONE);

                for (u32 slot = 0; slot < NUM_ABILITY_SLOTS; slot++)
                    if (gSpeciesInfo[variant->partySpecies].abilities[slot] == ability)
                        found = TRUE;
                EXPECT(found);
            }
        }
    }
}

TEST("Champions Circuit restores the exact prepared party after a run")
{
    // One Legendary only: the desk enforces the party rule.
    static const enum Species species[PARTY_SIZE] =
    {
        SPECIES_MEWTWO,
        SPECIES_MACHAMP,
        SPECIES_SQUIRTLE,
        SPECIES_PIKACHU,
        SPECIES_EEVEE,
        SPECIES_RIOLU,
    };
    static const enum Item items[PARTY_SIZE] =
    {
        ITEM_EVIOLITE,
        ITEM_LIFE_ORB,
        ITEM_SITRUS_BERRY,
        ITEM_LIGHT_BALL,
        ITEM_CHOICE_SCARF,
        ITEM_FOCUS_SASH,
    };
    u8 originalLevels[PARTY_SIZE];

    ZeroPlayerPartyMons();
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
    {
        enum Item item = items[slot];
        u16 hp = 1;

        CreateMon(&gParties[B_TRAINER_PLAYER][slot], species[slot], 20 + slot, 0, OTID_STRUCT_PLAYER_ID);
        SetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HELD_ITEM, &item);
        SetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HP, &hp);
        originalLevels[slot] = GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_LEVEL);
    }
    ChampionsCircuitCanEnter();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    ChampionsCircuitBegin();
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_ACTIVE), TRUE);
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_LEVEL),
            GetLevelCapForSpecies(species[slot], CHAMPIONS_CIRCUIT_BASE_LEVEL));

    ChampionsCircuitEnd();
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_ACTIVE), FALSE);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS), 0);
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
    {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_LEVEL), originalLevels[slot]);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HELD_ITEM), items[slot]);
        EXPECT_EQ(
            GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_MAX_HP)
        );
    }
}

TEST("Emerald Champions reload refreshes cached stats without reviving fainted Pokemon")
{
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    u32 hp;
    u32 staleAttack = 1;
    u32 expectedAttack;
    u32 expectedMaxHp;
    PARAMETRIZE_LABEL("hp=%d", 0) { hp = 0; }
    PARAMETRIZE_LABEL("hp=%d", 10) { hp = 10; }
    ZeroPlayerPartyMons();
    CreateMon(mon, SPECIES_ZIGZAGOON, 20, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    CalculateMonStats(mon);
    expectedAttack = GetMonData(mon, MON_DATA_ATK);
    expectedMaxHp = GetMonData(mon, MON_DATA_MAX_HP);
    SetMonData(mon, MON_DATA_HP, &hp);
    SetMonData(mon, MON_DATA_ATK, &staleAttack);
    SavePlayerParty();
    LoadPlayerParty();
    EXPECT_EQ(GetMonData(mon, MON_DATA_ATK), expectedAttack);
    EXPECT_EQ(GetMonData(mon, MON_DATA_MAX_HP), expectedMaxHp);
    EXPECT_EQ(GetMonData(mon, MON_DATA_HP), hp);
}

TEST("Emerald Champions field moves need badge, license, and species compatibility without a moveslot")
{
    struct Pokemon *party = gParties[B_TRAINER_PLAYER];

    ZeroPlayerPartyMons();
    CreateMon(&party[0], SPECIES_MAGIKARP, 14, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&party[1], SPECIES_MARILL, 14, 0, OTID_STRUCT_PLAYER_ID);
    SetMonMoveSlot(&party[1], MOVE_SURF, 0);
    CalculatePlayerPartyCount();

    FlagClear(FLAG_BADGE01_GET);
    FlagClear(FLAG_RECEIVED_HM_CUT);
    FlagClear(FLAG_BADGE05_GET);
    FlagClear(FLAG_RECEIVED_HM_SURF);
    // Locked: nobody, even though Marill knows Surf.
    EXPECT_EQ(FieldMove_GetUserSlot(FIELD_MOVE_SURF, TRUE), PARTY_SIZE);
    EXPECT_EQ(PartyHasMonWithSurf(), FALSE);

    FlagSet(FLAG_BADGE05_GET);
    FlagSet(FLAG_RECEIVED_HM_SURF);
    // The compatible member supplies the animation; its moveslot is irrelevant.
    EXPECT_EQ(FieldMove_GetUserSlot(FIELD_MOVE_SURF, TRUE), 1);
    EXPECT_EQ(PartyHasMonWithSurf(), TRUE);
    SetMonMoveSlot(&party[1], MOVE_SPLASH, 0);
    EXPECT_EQ(FieldMove_GetUserSlot(FIELD_MOVE_SURF, TRUE), 1);

    FlagSet(FLAG_BADGE01_GET);
    FlagSet(FLAG_RECEIVED_HM_CUT);
    // An unlocked license alone is insufficient without a capable party member.
    EXPECT_EQ(FieldMove_GetUserSlot(FIELD_MOVE_CUT, TRUE), PARTY_SIZE);
    // A party member that could learn it (without knowing it) does it.
    CreateMon(&party[0], SPECIES_ZIGZAGOON, 14, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(FieldMove_GetUserSlot(FIELD_MOVE_CUT, TRUE), 0);
    // Cut alone retains Inclement's optional grass-clearing party action.
    EXPECT_EQ(FieldMove_IsVisible(FIELD_MOVE_CUT), TRUE);
    EXPECT_EQ(FieldMove_IsCapabilityBased(FIELD_MOVE_CUT), TRUE);
    EXPECT_EQ(FieldMove_IsVisible(FIELD_MOVE_SURF), FALSE);
    EXPECT_EQ(FieldMove_IsVisible(FIELD_MOVE_WATERFALL), FALSE);
    EXPECT_EQ(FieldMove_IsVisible(FIELD_MOVE_SWEET_SCENT), TRUE);

    FlagClear(FLAG_BADGE01_GET);
    FlagClear(FLAG_RECEIVED_HM_CUT);
    FlagClear(FLAG_BADGE05_GET);
    FlagClear(FLAG_RECEIVED_HM_SURF);
}

TEST("Emerald Champions removes only the requested quantity across item stacks")
{
    ClearBag();
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_POTION)];
    BagPocket_SetSlotItemIdAndCount(pocket, 0, ITEM_POTION, 3);
    BagPocket_SetSlotItemIdAndCount(pocket, 2, ITEM_POTION, 5);
    EXPECT(AddBagItem(ITEM_ANTIDOTE, 2));

    EXPECT(RemoveBagItem(ITEM_POTION, 4));
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_POTION), 4);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_ANTIDOTE), 2);
}

TEST("Emerald Champions compacts an emptied stack that is not the last one drained")
{
    ClearBag();
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_POTION)];
    BagPocket_SetSlotItemIdAndCount(pocket, 0, ITEM_POTION, 3);
    BagPocket_SetSlotItemIdAndCount(pocket, 2, ITEM_POTION, 5);

    // Drains slot 0 completely and slot 2 partially. The emptied slot must not
    // survive as a hole in front of the stack that is still carrying items.
    EXPECT(RemoveBagItem(ITEM_POTION, 4));
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_POTION), 4);
    EXPECT_EQ(BagPocket_GetSlotData(pocket, 0).itemId, ITEM_POTION);
    EXPECT_EQ(BagPocket_GetSlotData(pocket, 0).quantity, 4);
}

TEST("Emerald Champions leaves inventory unchanged when an item removal is insufficient")
{
    ClearBag();
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_POTION)];
    BagPocket_SetSlotItemIdAndCount(pocket, 0, ITEM_POTION, 3);
    BagPocket_SetSlotItemIdAndCount(pocket, 2, ITEM_POTION, 5);
    EXPECT(AddBagItem(ITEM_ANTIDOTE, 2));
    struct ItemSlot before[pocket->capacity];
    for (u32 i = 0; i < pocket->capacity; i++)
        before[i] = BagPocket_GetSlotData(pocket, i);

    EXPECT(!RemoveBagItem(ITEM_POTION, 9));
    for (u32 i = 0; i < pocket->capacity; i++)
    {
        struct ItemSlot after = BagPocket_GetSlotData(pocket, i);
        EXPECT_EQ(after.itemId, before[i].itemId);
        EXPECT_EQ(after.quantity, before[i].quantity);
    }
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_ANTIDOTE), 2);
}

TEST("Emerald Champions rejects invalid preset requests without changing held items or Pokemon")
{
    static const enum Item items[] = {ITEM_NONE, ITEM_RED_ORB, ITEM_CHARIZARDITE_X};
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_CHARIZARD, 50, 0, OTID_STRUCT_PLAYER_ID);
    u8 invalidChoice = GetEmeraldChampionsRawBattleSetCount(SPECIES_CHARIZARD);
    EXPECT(GetEmeraldChampionsRawBattleSet(SPECIES_CHARIZARD, invalidChoice) == NULL);

    for (u32 i = 0; i < ARRAY_COUNT(items); i++)
    {
        SetMonData(&mon, MON_DATA_HELD_ITEM, &items[i]);
        struct Pokemon before = mon;
        EXPECT_EQ(ApplyEmeraldChampionsBattleSetChoiceForFormat(&mon, 0, EC_BATTLE_FORMAT_COUNT), EC_BATTLE_SET_FAILED);
        EXPECT_EQ(memcmp(&mon, &before, sizeof(mon)), 0);
        // The raw opponent API forwards a missing preset to ApplyPreset;
        // invalid visible formats above already fail before that boundary.
        EXPECT_EQ(ApplyEmeraldChampionsOpponentSet(&mon, invalidChoice), EC_BATTLE_SET_FAILED);
        EXPECT_EQ(memcmp(&mon, &before, sizeof(mon)), 0);
    }
}

// Catching sign-row legends sets caught bits and hide flags; tests that do
// so restore every event flag and var afterwards.
static EWRAM_DATA u8 sSavedRelicTestFlags[NUM_FLAG_BYTES];
static EWRAM_DATA u16 sSavedRelicTestVars[VARS_COUNT];

static void SaveRelicTestEventState(void)
{
    memcpy(sSavedRelicTestFlags, gSaveBlock1Ptr->flags, sizeof(sSavedRelicTestFlags));
    memcpy(sSavedRelicTestVars, gSaveBlock1Ptr->vars, sizeof(sSavedRelicTestVars));
}

static void RestoreRelicTestEventState(void)
{
    memcpy(gSaveBlock1Ptr->flags, sSavedRelicTestFlags, sizeof(sSavedRelicTestFlags));
    memcpy(gSaveBlock1Ptr->vars, sSavedRelicTestVars, sizeof(sSavedRelicTestVars));
}

static void ResetLegendaryRelicState(void)
{
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_0, 0);
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_1, 0);
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_2, 0);
}

// Runs the nurse's hand-over the way data/scripts/pkmn_center_nurse.inc does:
// one named giveitem per relic the Bag has room for, until none fits.
static u32 HandOverLegendaryRelics(enum Item *given, u32 capacity)
{
    u32 count = 0;
    for (;;)
    {
        BufferNextLegendaryRelic();
        enum Item item = gSpecialVar_Result;
        if (item == ITEM_NONE || !AddBagItem(item, 1))
            return count;
        if (count < capacity)
            given[count] = item;
        count++;
    }
}

TEST("Emerald Champions: a legend's relic waits for the Hall of Fame, then the next Center heal")
{
    u8 message[64];
    enum Item given[4];
    SaveRelicTestEventState();
    FlagClear(FLAG_IS_CHAMPION);
    ClearBag();
    ResetLegendaryRelicState();

    MarkLegendarySignCaughtBySpecies(SPECIES_KYOGRE);
    EXPECT(!CheckBagHasItem(ITEM_BLUE_ORB, 1));
    // The catch names the relic it put aside.
    EXPECT(BufferLegendaryRelicsHeldOnCatch(SPECIES_KYOGRE, message));
    EXPECT_EQ(StringCompare(message, COMPOUND_STRING("The Blue Orb")), 0);
    EXPECT(!BufferLegendaryRelicsHeldOnCatch(SPECIES_KYOGRE, message));
    CountDeliverableLegendaryRelics();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT_EQ(HandOverLegendaryRelics(given, ARRAY_COUNT(given)), 0);
    EXPECT(!CheckBagHasItem(ITEM_BLUE_ORB, 1));

    FlagSet(FLAG_IS_CHAMPION);
    CountDeliverableLegendaryRelics();
    EXPECT_EQ(gSpecialVar_Result, 1);
    EXPECT_EQ(HandOverLegendaryRelics(given, ARRAY_COUNT(given)), 1);
    EXPECT_EQ(given[0], ITEM_BLUE_ORB);
    EXPECT(CheckBagHasItem(ITEM_BLUE_ORB, 1));
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0), 0);
    CountDeliverableLegendaryRelics();
    EXPECT_EQ(gSpecialVar_Result, 0);

    // A catch after the Hall of Fame hands its relic over at once, silently.
    MarkLegendarySignCaughtBySpecies(SPECIES_GROUDON);
    EXPECT(CheckBagHasItem(ITEM_RED_ORB, 1));
    EXPECT(!BufferLegendaryRelicsHeldOnCatch(SPECIES_GROUDON, message));

    ClearBag();
    RestoreRelicTestEventState();
}

TEST("Emerald Champions: fusion and unbinding tools are relics that wait for the Hall of Fame")
{
    static const enum Item formCounterStock[] =
    {
#include "../src/data/emerald_champions_form_items.h"
    };
    static const struct { enum Species caught; enum Item tools[2]; const u8 *message; } legends[] =
    {
        {SPECIES_KYUREM, {ITEM_DNA_SPLICERS}, COMPOUND_STRING("The DNA Splicers")},
        {SPECIES_CALYREX, {ITEM_REINS_OF_UNITY}, COMPOUND_STRING("The Reins of Unity")},
        {SPECIES_NECROZMA, {ITEM_N_SOLARIZER, ITEM_N_LUNARIZER}, COMPOUND_STRING("The N-Solarizer and the N-Lunarizer")},
        {SPECIES_HOOPA, {ITEM_PRISON_BOTTLE}, COMPOUND_STRING("The Prison Bottle")},
        {SPECIES_ZYGARDE, {ITEM_ZYGARDE_CUBE}, COMPOUND_STRING("The Zygarde Cube")},
    };
    u8 message[64];
    enum Item given[8];
    u32 toolCount = 0;

    // No shop hands them out for free before the legend is caught.
    for (u32 l = 0; l < ARRAY_COUNT(legends); l++)
    {
        for (u32 t = 0; t < ARRAY_COUNT(legends[l].tools) && legends[l].tools[t] != ITEM_NONE; t++)
        {
            for (u32 i = 0; i < ARRAY_COUNT(formCounterStock); i++)
                EXPECT_NE(formCounterStock[i], legends[l].tools[t]);
            EXPECT(!IsEmeraldChampionsFreeCatalogueItem(legends[l].tools[t]));
            EXPECT(IsEmeraldChampionsProtectedProgressionItem(legends[l].tools[t]));
        }
    }

    SaveRelicTestEventState();
    FlagClear(FLAG_IS_CHAMPION);
    ClearBag();
    ResetLegendaryRelicState();
    for (u32 l = 0; l < ARRAY_COUNT(legends); l++)
    {
        // Caught in any form, the legend earns its base form's tools.
        MarkLegendarySignCaughtBySpecies(legends[l].caught);
        EXPECT(BufferLegendaryRelicsHeldOnCatch(legends[l].caught, message));
        EXPECT_EQ(StringCompare(message, legends[l].message), 0);
        for (u32 t = 0; t < ARRAY_COUNT(legends[l].tools) && legends[l].tools[t] != ITEM_NONE; t++, toolCount++)
        {
            EXPECT(!CheckBagHasItem(legends[l].tools[t], 1));
            EXPECT(!CheckPCHasItem(legends[l].tools[t], 1));
        }
    }
    // Pending relics 24-29 and earned grants 6-10 live in the third var;
    // the Orb, Plate and Mask bits in the first two are untouched.
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0), 0);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_1), 0);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_2), 0x1F3F);
    CountDeliverableLegendaryRelics();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT_EQ(HandOverLegendaryRelics(given, ARRAY_COUNT(given)), 0);

    FlagSet(FLAG_IS_CHAMPION);
    CountDeliverableLegendaryRelics();
    EXPECT_EQ(gSpecialVar_Result, toolCount);
    EXPECT_EQ(HandOverLegendaryRelics(given, ARRAY_COUNT(given)), toolCount);
    EXPECT_EQ(given[0], ITEM_DNA_SPLICERS);
    EXPECT_EQ(given[1], ITEM_REINS_OF_UNITY);
    EXPECT_EQ(given[2], ITEM_N_SOLARIZER);
    EXPECT_EQ(given[3], ITEM_N_LUNARIZER);
    EXPECT_EQ(given[4], ITEM_PRISON_BOTTLE);
    EXPECT_EQ(given[5], ITEM_ZYGARDE_CUBE);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_2), 0x1F00);

    // Each grant is earned once: another catch never repeats a tool.
    ClearBag();
    MarkLegendarySignCaughtBySpecies(SPECIES_KYUREM_BLACK);
    MarkLegendarySignCaughtBySpecies(SPECIES_NECROZMA_DAWN_WINGS);
    EXPECT(!BufferLegendaryRelicsHeldOnCatch(SPECIES_NECROZMA, message));
    CountDeliverableLegendaryRelics();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT(!CheckBagHasItem(ITEM_DNA_SPLICERS, 1));
    EXPECT(!CheckBagHasItem(ITEM_N_SOLARIZER, 1));

    ClearBag();
    RestoreRelicTestEventState();
}

TEST("Emerald Champions pending relics survive a full Bag and never replay discarded rewards")
{
    // Delivery mechanics after the Hall of Fame, when the relics are released.
    enum Item given[2];
    SaveRelicTestEventState();
    FlagSet(FLAG_IS_CHAMPION);
    ClearBag();
    ResetLegendaryRelicState();
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_RED_ORB)];
    for (u32 slot = 0; slot < pocket->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_SOFT_SAND, 1);
    for (u32 slot = 0; slot < PC_ITEMS_COUNT; slot++)
        gSaveBlock1Ptr->pcItems[slot] = (struct ItemSlot){ITEM_SOFT_SAND, 1};

    MarkLegendarySignCaughtBySpecies(SPECIES_GROUDON);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0), 1);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_1), 0x100);
    EXPECT(!CheckBagHasItem(ITEM_RED_ORB, 1));
    EXPECT(!CheckPCHasItem(ITEM_RED_ORB, 1));

    // The nurse names it, but a full Bag keeps it pending for the next heal.
    CountDeliverableLegendaryRelics();
    EXPECT_EQ(gSpecialVar_Result, 1);
    EXPECT_EQ(HandOverLegendaryRelics(given, ARRAY_COUNT(given)), 0);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0), 1);

    BagPocket_SetSlotItemIdAndCount(pocket, 0, ITEM_NONE, 0);
    EXPECT_EQ(HandOverLegendaryRelics(given, ARRAY_COUNT(given)), 1);
    EXPECT_EQ(given[0], ITEM_RED_ORB);
    EXPECT(CheckBagHasItem(ITEM_RED_ORB, 1));
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0), 0);

    // Discarded, it is not handed over again, even after another catch.
    EXPECT(RemoveBagItem(ITEM_RED_ORB, 1));
    MarkLegendarySignCaughtBySpecies(SPECIES_GROUDON);
    CountDeliverableLegendaryRelics();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT(!CheckBagHasItem(ITEM_RED_ORB, 1));
    EXPECT(!CheckPCHasItem(ITEM_RED_ORB, 1));
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0), 0);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_1), 0x100);

    // A pending relic the player already holds settles without a second copy.
    ResetLegendaryRelicState();
    FlagClear(FLAG_IS_CHAMPION);
    MarkLegendarySignCaughtBySpecies(SPECIES_GROUDON);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0), 1);
    EXPECT(AddBagItem(ITEM_RED_ORB, 1));
    FlagSet(FLAG_IS_CHAMPION);
    CountDeliverableLegendaryRelics();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0), 0);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_RED_ORB), 1);

    ClearBag();
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
    RestoreRelicTestEventState();
}

TEST("Emerald Champions relics bound for other pockets arrive past one whose pocket is full")
{
    enum Item given[4];
    SaveRelicTestEventState();
    FlagSet(FLAG_IS_CHAMPION);
    ClearBag();
    ResetLegendaryRelicState();
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_RUSTED_SWORD)];
    EXPECT_NE(GetItemPocket(ITEM_RED_ORB), pocket->id);
    EXPECT_NE(GetItemPocket(ITEM_DNA_SPLICERS), pocket->id);
    for (u32 slot = 0; slot < pocket->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_SOFT_SAND, 1);
    for (u32 slot = 0; slot < PC_ITEMS_COUNT; slot++)
        gSaveBlock1Ptr->pcItems[slot] = (struct ItemSlot){ITEM_SOFT_SAND, 1};

    // Pending: Red Orb (0), Rusted Sword (2) and DNA Splicers (24).
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_0, (1u << 0) | (1u << 2));
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_1, 0x500);
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_2, 0x101);
    CountDeliverableLegendaryRelics();
    EXPECT_EQ(gSpecialVar_Result, 3);
    EXPECT_EQ(HandOverLegendaryRelics(given, ARRAY_COUNT(given)), 2);
    EXPECT_EQ(given[0], ITEM_RED_ORB);
    EXPECT_EQ(given[1], ITEM_DNA_SPLICERS);
    EXPECT(!CheckBagHasItem(ITEM_RUSTED_SWORD, 1));
    EXPECT(!CheckPCHasItem(ITEM_RUSTED_SWORD, 1));
    // Only the Rusted Sword is left for the nurse's "keep it safe" line.
    CountDeliverableLegendaryRelics();
    EXPECT_EQ(gSpecialVar_Result, 1);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0), 1u << 2);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_1), 0x500);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_2), 0x100);

    // Freed space delivers it at the next heal.
    BagPocket_SetSlotItemIdAndCount(pocket, 0, ITEM_NONE, 0);
    EXPECT_EQ(HandOverLegendaryRelics(given, ARRAY_COUNT(given)), 1);
    EXPECT_EQ(given[0], ITEM_RUSTED_SWORD);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0), 0);
    CountDeliverableLegendaryRelics();
    EXPECT_EQ(gSpecialVar_Result, 0);

    ClearBag();
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
    RestoreRelicTestEventState();
}

TEST("Emerald Champions partial mask grants retry only their saved undelivered items")
{
    enum Item given[4];
    ClearBag();
    ResetLegendaryRelicState();
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_WELLSPRING_MASK)];
    EXPECT_EQ(GetItemPocket(ITEM_HEARTHFLAME_MASK), pocket->id);
    EXPECT_EQ(GetItemPocket(ITEM_CORNERSTONE_MASK), pocket->id);
    for (u32 slot = 1; slot < pocket->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_SOFT_SAND, 1);
    for (u32 slot = 0; slot < PC_ITEMS_COUNT; slot++)
        gSaveBlock1Ptr->pcItems[slot] = (struct ItemSlot){ITEM_SOFT_SAND, 1};

    MarkLegendarySignCaughtBySpecies(SPECIES_OGERPON_TEAL);
    EXPECT(CheckBagHasItem(ITEM_WELLSPRING_MASK, 1));
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0), (1u << 5) | (1u << 6));
    // Masks are not held for the Hall of Fame, so the catch says nothing.
    EXPECT(!BufferLegendaryRelicsHeldOnCatch(SPECIES_OGERPON_TEAL, (u8[64]){0}));
    EXPECT(RemoveBagItem(ITEM_WELLSPRING_MASK, 1));
    BagPocket_SetSlotItemIdAndCount(pocket, 1, ITEM_NONE, 0);
    CountDeliverableLegendaryRelics();
    EXPECT_EQ(gSpecialVar_Result, 2);
    EXPECT_EQ(HandOverLegendaryRelics(given, ARRAY_COUNT(given)), 2);
    EXPECT_EQ(given[0], ITEM_HEARTHFLAME_MASK);
    EXPECT_EQ(given[1], ITEM_CORNERSTONE_MASK);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0), 0);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_1), 0x1000);
    MarkLegendarySignCaughtBySpecies(SPECIES_OGERPON_TEAL);
    CountDeliverableLegendaryRelics();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT(!CheckBagHasItem(ITEM_WELLSPRING_MASK, 1));
    EXPECT(!CheckPCHasItem(ITEM_WELLSPRING_MASK, 1));
    ClearBag();
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
    ResetLegendaryRelicState();
}

TEST("Emerald Champions no-repetition evolutions work at cap and honor Everstone")
{
    static const struct { enum Species from, to; enum Move move; u32 personality; } cases[] =
    {
        {SPECIES_PRIMEAPE, SPECIES_ANNIHILAPE, MOVE_RAGE_FIST, 0},
        {SPECIES_STANTLER, SPECIES_WYRDEER, MOVE_PSYSHIELD_BASH, 0},
        {SPECIES_PAWMO, SPECIES_PAWMOT, MOVE_NONE, 0},
        {SPECIES_BRAMBLIN, SPECIES_BRAMBLEGHAST, MOVE_NONE, 0},
        {SPECIES_RELLOR, SPECIES_RABSCA, MOVE_NONE, 0},
        {SPECIES_BASCULIN_WHITE_STRIPED, SPECIES_BASCULEGION_M, MOVE_NONE, 255},
        {SPECIES_BASCULIN_WHITE_STRIPED, SPECIES_BASCULEGION_F, MOVE_NONE, 0},
        {SPECIES_FARFETCHD_GALAR, SPECIES_SIRFETCHD, MOVE_NONE, 0},
    };
    struct Pokemon mon;
    u16 everstone = ITEM_EVERSTONE;
    ResetCampaignCapMilestones();
    for (u32 i = 0; i < ARRAY_COUNT(cases); i++)
    {
        bool32 canStop = TRUE;
        CreateMon(&mon, cases[i].from, 14, cases[i].personality, OTID_STRUCT_PLAYER_ID);
        if (cases[i].move != MOVE_NONE)
        {
            EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, &canStop, CHECK_EVO), SPECIES_NONE);
            SetMonMoveSlot(&mon, cases[i].move, 0);
        }
        EXPECT(IsMonEligibleForLeveler(&mon));
        EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, &canStop, CHECK_EVO), cases[i].to);
        EXPECT(canStop);
        EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), 14);
        SetMonData(&mon, MON_DATA_HELD_ITEM, &everstone);
        EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, &canStop, CHECK_EVO), SPECIES_NONE);
        EXPECT(!IsMonEligibleForLeveler(&mon));
    }
}

TEST("Emerald Champions authored Coalossal keeps its deliberate zero Attack IV")
{
    // Authored IVs must reach the native party exactly: Tabitha's Magma
    // Hideout Coalossal is a special attacker built with a 0 Attack IV.
    struct Pokemon *party = gParties[B_TRAINER_OPPONENT_A];
    ResetCampaignCapMilestones();
    CreateNPCTrainerPartyFromTrainer(party, &gTrainers[DIFFICULTY_NORMAL][TRAINER_TABITHA_MAGMA_HIDEOUT]);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_COALOSSAL);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_ATK_IV), 0);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPATK_IV), 31);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_FRIENDSHIP), 255);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_MOVE1), MOVE_HEAT_WAVE);
    ZeroEnemyPartyMons();
}

extern void Test_SetBattledTrainersFlags(void);

TEST("Frontier gambler skips closed challenges and refunds old wagers once")
{
    u16 savedChallenge = VarGet(VAR_FRONTIER_GAMBLER_CHALLENGE);
    u16 savedSetChallenge = VarGet(VAR_FRONTIER_GAMBLER_SET_CHALLENGE);
    u16 savedState = VarGet(VAR_FRONTIER_GAMBLER_STATE);
    u16 savedBet = VarGet(VAR_FRONTIER_GAMBLER_AMOUNT_BET);
    u16 savedResult = gSpecialVar_Result;
    u16 savedPoints = gSaveBlock2Ptr->frontier.battlePoints;
    const bool8 open[FRONTIER_GAMBLER_CHALLENGE_COUNT] = {
        FALSE, TRUE, TRUE, FALSE, TRUE, FALSE,
        TRUE, FALSE, TRUE, FALSE, TRUE, TRUE,
    };
    for (u32 challenge = 0; challenge < ARRAY_COUNT(open); challenge++)
    {
        VarSet(VAR_FRONTIER_GAMBLER_CHALLENGE, challenge);
        UpdateFrontierGambler(0);
        EXPECT(open[VarGet(VAR_FRONTIER_GAMBLER_CHALLENGE)]);
        VarSet(VAR_FRONTIER_GAMBLER_SET_CHALLENGE, challenge);
        VarSet(VAR_FRONTIER_GAMBLER_STATE, FRONTIER_GAMBLER_PLACED_BET);
        VarSet(VAR_FRONTIER_GAMBLER_AMOUNT_BET, FRONTIER_GAMBLER_BET_5);
        gSaveBlock2Ptr->frontier.battlePoints = 20;
        RefundRetiredFrontierGamblerBet();
        EXPECT_EQ(gSpecialVar_Result, !open[challenge]);
        EXPECT_EQ(gSaveBlock2Ptr->frontier.battlePoints, open[challenge] ? 20 : 25);
        EXPECT_EQ(VarGet(VAR_FRONTIER_GAMBLER_STATE),
            open[challenge] ? FRONTIER_GAMBLER_PLACED_BET : FRONTIER_GAMBLER_WAITING);
        RefundRetiredFrontierGamblerBet();
        EXPECT_EQ(gSpecialVar_Result, FALSE);
        EXPECT_EQ(gSaveBlock2Ptr->frontier.battlePoints, open[challenge] ? 20 : 25);
    }
    VarSet(VAR_FRONTIER_GAMBLER_CHALLENGE, 11);
    UpdateFrontierGambler(1);
    EXPECT_EQ(VarGet(VAR_FRONTIER_GAMBLER_CHALLENGE), 1);
    VarSet(VAR_FRONTIER_GAMBLER_SET_CHALLENGE, 9);
    VarSet(VAR_FRONTIER_GAMBLER_STATE, FRONTIER_GAMBLER_PLACED_BET);
    VarSet(VAR_FRONTIER_GAMBLER_AMOUNT_BET, FRONTIER_GAMBLER_BET_15);
    gSaveBlock2Ptr->frontier.battlePoints = MAX_BATTLE_FRONTIER_POINTS - 1;
    RefundRetiredFrontierGamblerBet();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT_EQ(gSaveBlock2Ptr->frontier.battlePoints, MAX_BATTLE_FRONTIER_POINTS);
    VarSet(VAR_FRONTIER_GAMBLER_CHALLENGE, savedChallenge);
    VarSet(VAR_FRONTIER_GAMBLER_SET_CHALLENGE, savedSetChallenge);
    VarSet(VAR_FRONTIER_GAMBLER_STATE, savedState);
    VarSet(VAR_FRONTIER_GAMBLER_AMOUNT_BET, savedBet);
    gSaveBlock2Ptr->frontier.battlePoints = savedPoints;
    gSpecialVar_Result = savedResult;
}

TEST("Frontier gambler settles Pike Doubles and an old Pike-mode wager")
{
    u16 savedFacility = VarGet(VAR_FRONTIER_FACILITY);
    u16 savedMode = VarGet(VAR_FRONTIER_BATTLE_MODE);
    u16 savedChallenge = VarGet(VAR_FRONTIER_GAMBLER_SET_CHALLENGE);
    u16 savedState = VarGet(VAR_FRONTIER_GAMBLER_STATE);

    VarSet(VAR_FRONTIER_FACILITY, FRONTIER_FACILITY_PIKE);
    VarSet(VAR_FRONTIER_GAMBLER_SET_CHALLENGE, 10);
    for (u32 mode = FRONTIER_MODE_SINGLES; mode <= FRONTIER_MODE_DOUBLES; mode++)
    {
        VarSet(VAR_FRONTIER_BATTLE_MODE, mode);
        VarSet(VAR_FRONTIER_GAMBLER_STATE, FRONTIER_GAMBLER_PLACED_BET);
        FrontierGamblerSetWonOrLost(TRUE);
        EXPECT_EQ(VarGet(VAR_FRONTIER_GAMBLER_STATE), FRONTIER_GAMBLER_WON);
        VarSet(VAR_FRONTIER_GAMBLER_STATE, FRONTIER_GAMBLER_PLACED_BET);
        FrontierGamblerSetWonOrLost(FALSE);
        EXPECT_EQ(VarGet(VAR_FRONTIER_GAMBLER_STATE), FRONTIER_GAMBLER_LOST);
    }

    VarSet(VAR_FRONTIER_FACILITY, savedFacility);
    VarSet(VAR_FRONTIER_BATTLE_MODE, savedMode);
    VarSet(VAR_FRONTIER_GAMBLER_SET_CHALLENGE, savedChallenge);
    VarSet(VAR_FRONTIER_GAMBLER_STATE, savedState);
}

TEST("Pike Doubles retains the old mode's saved streak")
{
    u16 savedFacility = VarGet(VAR_FRONTIER_FACILITY);
    u16 savedMode = VarGet(VAR_FRONTIER_BATTLE_MODE);
    u8 savedLevel = gSaveBlock2Ptr->frontier.lvlMode;
    u16 savedStreak = gSaveBlock2Ptr->frontier.pikeWinStreaks[FRONTIER_LVL_OPEN];

    VarSet(VAR_FRONTIER_FACILITY, FRONTIER_FACILITY_PIKE);
    gSaveBlock2Ptr->frontier.lvlMode = FRONTIER_LVL_OPEN;
    gSaveBlock2Ptr->frontier.pikeWinStreaks[FRONTIER_LVL_OPEN] = 11;
    VarSet(VAR_FRONTIER_BATTLE_MODE, FRONTIER_MODE_SINGLES);
    EXPECT_EQ(GetCurrentFacilityWinStreak(), 11);
    VarSet(VAR_FRONTIER_BATTLE_MODE, FRONTIER_MODE_DOUBLES);
    EXPECT_EQ(GetCurrentFacilityWinStreak(), 11);

    gSaveBlock2Ptr->frontier.pikeWinStreaks[FRONTIER_LVL_OPEN] = savedStreak;
    gSaveBlock2Ptr->frontier.lvlMode = savedLevel;
    VarSet(VAR_FRONTIER_FACILITY, savedFacility);
    VarSet(VAR_FRONTIER_BATTLE_MODE, savedMode);
}

TEST("Pike rooms with one usable Pokemon do not offer optional doubles trainer rooms")
{
    struct Pokemon savedParty[FRONTIER_PARTY_SIZE];
    u8 savedCount = gPartiesCount[B_TRAINER_PLAYER];
    u8 savedHint = gSaveBlock2Ptr->frontier.pikeHintedRoomType;
    u8 savedHintIndex = gSaveBlock2Ptr->frontier.pikeHintedRoomIndex;
    bool8 savedHealingDisabled = gSaveBlock2Ptr->frontier.pikeHealingRoomsDisabled;
    rng_value_t savedRng = gRngValue;
    u16 savedChoice = gSpecialVar_0x8007;
    u16 savedFunction = gSpecialVar_0x8004;
    memcpy(savedParty, gParties[B_TRAINER_PLAYER], sizeof(savedParty));
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_RATTATA, 30, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    const u8 battleRooms[] = {PIKE_ROOM_ONE_TRAINER_BATTLE, PIKE_ROOM_HARD_BATTLE, PIKE_ROOM_TWO_TRAINER_BATTLE};
    for (u32 hint = 0; hint < ARRAY_COUNT(battleRooms); hint++)
    {
        gSaveBlock2Ptr->frontier.pikeHintedRoomType = battleRooms[hint];
        gSaveBlock2Ptr->frontier.pikeHintedRoomIndex = 1;
        gSpecialVar_0x8007 = 1;
        gSpecialVar_0x8004 = BATTLE_PIKE_FUNC_SET_ROOM_TYPE;
        CallBattlePikeFunction();
        gSpecialVar_0x8004 = BATTLE_PIKE_FUNC_GET_ROOM_TYPE;
        CallBattlePikeFunction();
        EXPECT_EQ(gSpecialVar_Result, PIKE_ROOM_NPC);

        gSpecialVar_0x8007 = 2;
        for (u32 disableHealing = 0; disableHealing < 2; disableHealing++)
        {
            gSaveBlock2Ptr->frontier.pikeHealingRoomsDisabled = disableHealing;
            for (u32 seed = 1; seed <= 24; seed++)
            {
                SeedRng(seed);
                gSpecialVar_0x8004 = BATTLE_PIKE_FUNC_SET_ROOM_TYPE;
                CallBattlePikeFunction();
                gSpecialVar_0x8004 = BATTLE_PIKE_FUNC_GET_ROOM_TYPE;
                CallBattlePikeFunction();
                for (u32 room = 0; room < ARRAY_COUNT(battleRooms); room++)
                    EXPECT_NE(gSpecialVar_Result, battleRooms[room]);
            }
        }
    }
    memcpy(gParties[B_TRAINER_PLAYER], savedParty, sizeof(savedParty));
    gPartiesCount[B_TRAINER_PLAYER] = savedCount;
    gSaveBlock2Ptr->frontier.pikeHintedRoomType = savedHint;
    gSaveBlock2Ptr->frontier.pikeHintedRoomIndex = savedHintIndex;
    gSaveBlock2Ptr->frontier.pikeHealingRoomsDisabled = savedHealingDisabled;
    gRngValue = savedRng;
    gSpecialVar_0x8007 = savedChoice;
    gSpecialVar_0x8004 = savedFunction;
}

TEST("Battle setup marks only participating trainer owners and preserves the Fortree gift")
{
    u32 flags;
    u16 opponentB;
    bool32 markB;
    PARAMETRIZE_LABEL("%s", "partner one owner") { flags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_INGAME_PARTNER; opponentB = 0xFFFF; markB = FALSE; }
    PARAMETRIZE_LABEL("%s", "single") { flags = BATTLE_TYPE_TRAINER; opponentB = 0; markB = FALSE; }
    PARAMETRIZE_LABEL("%s", "stale second owner") { flags = BATTLE_TYPE_TRAINER; opponentB = TRAINER_COURTNEY_MOSSDEEP; markB = FALSE; }
    PARAMETRIZE_LABEL("%s", "partner sentinel") { flags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_INGAME_PARTNER | BATTLE_TYPE_TWO_OPPONENTS; opponentB = 0xFFFF; markB = FALSE; }
    PARAMETRIZE_LABEL("%s", "two owners") { flags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_TWO_OPPONENTS; opponentB = TRAINER_COURTNEY_MOSSDEEP; markB = TRUE; }

    u32 savedFlags = gBattleTypeFlags;
    TrainerBattleParameter savedParams = gTrainerBattleParameter;
    bool32 savedGift = FlagGet(FLAG_EC_GIFT_FORTREE_CITY_HOUSE2);
    InitTrainerBattleParameter();
    gBattleTypeFlags = flags;
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_MAXIE_MOSSDEEP;
    TRAINER_BATTLE_PARAM.opponentB = opponentB;
    ClearTrainerFlag(TRAINER_MAXIE_MOSSDEEP);
    ClearTrainerFlag(TRAINER_COURTNEY_MOSSDEEP);
    FlagClear(FLAG_EC_GIFT_FORTREE_CITY_HOUSE2);
    Test_SetBattledTrainersFlags();
    EXPECT(HasTrainerBeenFought(TRAINER_MAXIE_MOSSDEEP));
    EXPECT_EQ(HasTrainerBeenFought(TRAINER_COURTNEY_MOSSDEEP), markB);
    EXPECT(!FlagGet(FLAG_EC_GIFT_FORTREE_CITY_HOUSE2));
    if (savedGift)
        FlagSet(FLAG_EC_GIFT_FORTREE_CITY_HOUSE2);
    gBattleTypeFlags = savedFlags;
    gTrainerBattleParameter = savedParams;
}

TEST("Evolution conditions commit item costs once after all conditions pass")
{
    static const struct EvolutionParam conditions[] = {
        {IF_HOLD_ITEM, ITEM_METAL_COAT},
        {IF_BAG_ITEM_COUNT, ITEM_POKE_BALL, 1},
        {IF_MIN_FRIENDSHIP, 255},
        {CONDITIONS_END},
    };
    bool32 eligible = FALSE;
    enum EvoState mode = CHECK_EVO;
    PARAMETRIZE_LABEL("%s", "failed preview") { eligible = FALSE; mode = CHECK_EVO; }
    PARAMETRIZE_LABEL("%s", "failed commit") { eligible = FALSE; mode = DO_EVO; }
    PARAMETRIZE_LABEL("%s", "successful preview") { eligible = TRUE; mode = CHECK_EVO; }
    PARAMETRIZE_LABEL("%s", "successful commit") { eligible = TRUE; mode = DO_EVO; }
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_ONIX, 20, 0, OTID_STRUCT_PLAYER_ID);
    u16 heldItem = ITEM_METAL_COAT;
    u8 friendship = eligible ? 255 : 0;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &heldItem);
    SetMonData(&mon, MON_DATA_FRIENDSHIP, &friendship);
    ClearBag();
    EXPECT(AddBagItem(ITEM_POKE_BALL, 5));
    bool32 canStop = TRUE;
    EXPECT_EQ(DoesMonMeetAdditionalConditions(&mon, conditions, NULL, PARTY_SIZE, &canStop, mode), eligible);
    bool32 consumed = eligible && mode == DO_EVO;
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), consumed ? ITEM_NONE : ITEM_METAL_COAT);
    EXPECT(CheckBagHasItem(ITEM_POKE_BALL, consumed ? 4 : 5));
    EXPECT(!CheckBagHasItem(ITEM_POKE_BALL, consumed ? 5 : 6));
    EXPECT_EQ(canStop, !eligible);
}

TEST("A held-item evolution can't be cancelled once its item is spent")
{
    // Happiny's Oval Stone is taken when the evolution starts; cancelling
    // afterwards would leave an unevolved Happiny with no stone.
    static const struct EvolutionParam conditions[] = {
        {IF_HOLD_ITEM, ITEM_OVAL_STONE},
        {CONDITIONS_END},
    };
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_HAPPINY, 20, 0, OTID_STRUCT_PLAYER_ID);
    u16 heldItem = ITEM_OVAL_STONE;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &heldItem);
    bool32 canStop = TRUE;
    EXPECT(DoesMonMeetAdditionalConditions(&mon, conditions, NULL, PARTY_SIZE, &canStop, CHECK_EVO));
    EXPECT(!canStop);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), ITEM_OVAL_STONE);
}

TEST("Emerald Champions paired prizes use incoming cap once and exclude replays")
{
    struct BattleStruct *savedStruct = gBattleStruct;
    u32 savedFlags = gBattleTypeFlags;
    TrainerBattleParameter savedParams = gTrainerBattleParameter;
    ResetCampaignCapMilestones();
    FlagSet(FLAG_BADGE07_GET); // cap 70 at the Space Center
    memset(&sEmeraldChampionsTestBattleStruct, 0, sizeof(sEmeraldChampionsTestBattleStruct));
    gBattleStruct = &sEmeraldChampionsTestBattleStruct;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_TWO_OPPONENTS;
    InitTrainerBattleParameter();
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_MAXIE_MOSSDEEP;
    TRAINER_BATTLE_PARAM.opponentB = TRAINER_COURTNEY_MOSSDEEP;
    ClearTrainerFlag(TRAINER_BATTLE_PARAM.opponentA);
    ClearTrainerFlag(TRAINER_BATTLE_PARAM.opponentB);
    InitCampaignBattleReward();
    // The authored tier 25 (leader/admin) pays 25 per point of the live cap.
    EXPECT_EQ(GetCampaignBattleMoneyReward(), 1750);
    FlagSet(FLAG_EC_REPORT_C42_COMPLETE); // retired: must not move the reward
    SetCurrentDifficultyLevel(DIFFICULTY_HARD);
    EXPECT_EQ(GetCampaignBattleMoneyReward(), 1750);
    SetCurrentDifficultyLevel(DIFFICULTY_EASY);
    EXPECT_EQ(GetCampaignBattleMoneyReward(), 1750);
    SetTrainerFlag(TRAINER_BATTLE_PARAM.opponentA);
    SetTrainerFlag(TRAINER_BATTLE_PARAM.opponentB);
    InitCampaignBattleReward();
    EXPECT_EQ(GetCampaignBattleMoneyReward(), 0);
    ClearTrainerFlag(TRAINER_BATTLE_PARAM.opponentA);
    ClearTrainerFlag(TRAINER_BATTLE_PARAM.opponentB);
    TRAINER_BATTLE_PARAM.isRematch = TRUE;
    InitCampaignBattleReward();
    EXPECT_EQ(GetCampaignBattleMoneyReward(), 0);
    gTrainerBattleParameter = savedParams;
    gBattleStruct = savedStruct;
    gBattleTypeFlags = savedFlags;
    SetCurrentDifficultyLevel(DIFFICULTY_HARD);
    ResetCampaignCapMilestones();
}

TEST("Emerald Champions v4 reporter stages skip retired teams and end after party 6")
{
    // Parties 1, 2, 5 and 6 are fought once each (Route 111, 118, 118, 120);
    // after party 6 the counter stays at 6 and the pair stays on Route 120.
    static const u8 nextStates[] = {1, 4, 5, 6, 6};
    static const u8 gabbyIds[] = {LOCALID_ROUTE111_GABBY_1, LOCALID_ROUTE118_GABBY_1,
        LOCALID_ROUTE118_GABBY_2, LOCALID_ROUTE120_GABBY_2, LOCALID_ROUTE120_GABBY_2};
    static const u8 tyIds[] = {LOCALID_ROUTE111_TY_1, LOCALID_ROUTE118_TY_1,
        LOCALID_ROUTE118_TY_2, LOCALID_ROUTE120_TY_2, LOCALID_ROUTE120_TY_2};
    ResetGabbyAndTy();
    EXPECT_EQ(GabbyAndTyGetBattleNum(), 0);
    gBattleResults.lastUsedMovePlayer = MOVE_TACKLE;
    for (u32 i = 0; i < ARRAY_COUNT(nextStates); i++)
    {
        GabbyAndTyBeforeInterview();
        EXPECT_EQ(GabbyAndTyGetBattleNum(), nextStates[i]);
        GetGabbyAndTyLocalIds();
        EXPECT_EQ(gSpecialVar_0x8004, gabbyIds[i]);
        EXPECT_EQ(gSpecialVar_0x8005, tyIds[i]);
    }
    for (u32 i = 0; i < 300; i++)
    {
        GabbyAndTyBeforeInterview();
        EXPECT_EQ(GabbyAndTyGetBattleNum(), 6);
    }
    // Saves from the old endless party-6 cycle (7, 8) and the retired
    // parties 3/4 (2, 3) land on a live state.
    for (u32 legacy = 2; legacy <= 8; legacy++)
    {
        gSaveBlock1Ptr->gabbyAndTyData.battleNum = legacy;
        EXPECT_EQ(GabbyAndTyGetBattleNum(), legacy <= 3 ? 4 : min(legacy, 6));
    }
    ResetGabbyAndTy();
    FlagClear(FLAG_TEMP_SKIP_GABBY_INTERVIEW);
}

TEST("Emerald Champions v4 final reporter battle pays first-clear money once")
{
    struct BattleStruct *savedStruct = gBattleStruct;
    u32 savedFlags = gBattleTypeFlags;
    TrainerBattleParameter savedParams = gTrainerBattleParameter;
    memset(&sEmeraldChampionsTestBattleStruct, 0, sizeof(sEmeraldChampionsTestBattleStruct));
    gBattleStruct = &sEmeraldChampionsTestBattleStruct;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
    InitTrainerBattleParameter();
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_GABBY_AND_TY_6;
    ClearTrainerFlag(TRAINER_GABBY_AND_TY_6);
    gSaveBlock1Ptr->gabbyAndTyData.battleNum = 5;
    InitCampaignBattleReward();
    EXPECT(GetCampaignBattleMoneyReward() > 0);
    // The retired pair never battles again (the map keeps the trainer flag
    // set); even a legacy save with the flag cleared earns no second receipt.
    gSaveBlock1Ptr->gabbyAndTyData.battleNum = 6;
    ClearTrainerFlag(TRAINER_GABBY_AND_TY_6);
    InitCampaignBattleReward();
    EXPECT_EQ(GetCampaignBattleMoneyReward(), 0);
    gTrainerBattleParameter = savedParams;
    gBattleStruct = savedStruct;
    gBattleTypeFlags = savedFlags;
    ResetGabbyAndTy();
}

static void ResetBookItemOwnership(void)
{
    ClearBag();
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
    ZeroPlayerPartyMons();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
    memset(&gSaveBlock1Ptr->daycare, 0, sizeof(gSaveBlock1Ptr->daycare));
}

TEST("Emerald Champions permanent item ownership includes Day Care and protects Mega value")
{
    enum Item stone = ITEM_VENUSAURITE;
    enum Item none = ITEM_NONE;
    ResetBookItemOwnership();
    EXPECT(!PlayerOwnsItem(ITEM_NONE));
    EXPECT(!PlayerOwnsItem(stone));
    EXPECT_EQ(GetFiniteDuplicateRewardValue(stone), 0);
    CreateBoxMon(&gSaveBlock1Ptr->daycare.mons[0].mon, SPECIES_BULBASAUR, 20, 0, OTID_STRUCT_PLAYER_ID);
    SetBoxMonData(&gSaveBlock1Ptr->daycare.mons[0].mon, MON_DATA_HELD_ITEM, &stone);
    EXPECT(PlayerOwnsItem(stone));
    EXPECT_EQ(GetFiniteDuplicateRewardValue(stone), 3000);
    EXPECT(IsItemProtectedFromLoss(stone));
    EXPECT_EQ(GetItemImportance(stone), 0); // Remains holdable.
    EXPECT_EQ(GetItemSellPrice(stone), 0);
    SetBoxMonData(&gSaveBlock1Ptr->daycare.mons[0].mon, MON_DATA_HELD_ITEM, &none);
    EXPECT(!PlayerOwnsItem(stone));
    EXPECT(AddPCItem(ITEM_LINKING_CORD, 1));
    EXPECT_EQ(GetFiniteDuplicateRewardValue(ITEM_LINKING_CORD), 3000);
    EXPECT(IsItemProtectedFromLoss(ITEM_LINKING_CORD));
    EXPECT_EQ(GetItemSellPrice(ITEM_LINKING_CORD), 0);
    ResetBookItemOwnership();
}

TEST("Emerald Champions Ball prices and sale policy")
{
    ResetBookItemOwnership();
    EXPECT_EQ(GetItemPrice(ITEM_POKE_BALL), 200);
    EXPECT_EQ(GetItemPrice(ITEM_GREAT_BALL), 600);
    EXPECT_EQ(GetItemPrice(ITEM_ULTRA_BALL), 800);
    EXPECT_EQ(GetItemPrice(ITEM_QUICK_BALL), 1000);
    EXPECT_EQ(GetItemPrice(ITEM_ETHER), 1200);
    EXPECT_EQ(GetItemSellPrice(ITEM_POKE_BALL), 50);
    EXPECT_EQ(GetItemSellPrice(ITEM_ULTRA_BALL), 200);
    EXPECT_EQ(GetItemSellPrice(ITEM_HEART_SCALE), 25); // Paid items use the configured modern quarter-price resale.
    ResetBookItemOwnership();
}

TEST("Emerald Champions garden refuses owned stones before charging")
{
    ResetBookItemOwnership();
    FlagClear(FLAG_EC_BERRY_TRADE_BAXCALIBRITE);
    FlagClear(FLAG_ITEM_ABANDONED_SHIP_ROOMS_B1F_GLALITITE);
    EXPECT(AddBagItem(ITEM_RAZZ_BERRY, 20));
    EXPECT(AddPCItem(ITEM_BAXCALIBRITE, 1));
    gSpecialVar_0x8004 = 0;
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_ALREADY_DONE);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_RAZZ_BERRY), 20);
    EXPECT(!FlagGet(FLAG_EC_BERRY_TRADE_BAXCALIBRITE));
    ResetBookItemOwnership();
}

TEST("Emerald Champions NPC trades recover held items from party or PC and reject full stores")
{
    enum Item stone = ITEM_VENUSAURITE;
    ResetBookItemOwnership();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ZIGZAGOON, 20, 0, OTID_STRUCT_PLAYER_ID);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &stone);
    gSpecialVar_0x8004 = 0;
    ReturnInGameTradeHeldItem();
    EXPECT_EQ(gSpecialVar_Result, 2);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);
    EXPECT_EQ(CountTotalItemQuantityInBag(stone), 1);
    ReturnInGameTradeHeldItem();
    EXPECT_EQ(gSpecialVar_Result, 1);
    EXPECT_EQ(CountTotalItemQuantityInBag(stone), 1);
    ClearBag();
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(stone)];
    for (u32 slot = 0; slot < pocket->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_ABOMASITE, MAX_BAG_ITEM_CAPACITY);
    CreateBoxMon(GetBoxedMonPtr(3, 7), SPECIES_ZIGZAGOON, 20, 0, OTID_STRUCT_PLAYER_ID);
    SetBoxMonData(GetBoxedMonPtr(3, 7), MON_DATA_HELD_ITEM, &stone);
    gSpecialVar_0x8004 = PC_MON_CHOSEN;
    gSpecialVar_MonBoxId = 3;
    gSpecialVar_MonBoxPos = 7;
    ReturnInGameTradeHeldItem();
    EXPECT_EQ(gSpecialVar_Result, 3);
    EXPECT_EQ(GetBoxMonData(GetBoxedMonPtr(3, 7), MON_DATA_HELD_ITEM), ITEM_NONE);
    EXPECT(CheckPCHasItem(stone, 1));
    for (u32 slot = 0; slot < PC_ITEMS_COUNT; slot++)
        gSaveBlock1Ptr->pcItems[slot] = (struct ItemSlot){ITEM_POTION, MAX_PC_ITEM_CAPACITY};
    SetBoxMonData(GetBoxedMonPtr(3, 7), MON_DATA_HELD_ITEM, &stone);
    ReturnInGameTradeHeldItem();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT_EQ(GetBoxMonData(GetBoxedMonPtr(3, 7), MON_DATA_SPECIES), SPECIES_ZIGZAGOON);
    EXPECT_EQ(GetBoxMonData(GetBoxedMonPtr(3, 7), MON_DATA_HELD_ITEM), stone);
    ResetBookItemOwnership();
}

TEST("Emerald Champions party restore preserves fainting status Eggs items and move PP")
{
    static const enum Species species[] = {SPECIES_EEVEE, SPECIES_PIKACHU, SPECIES_TOGEPI};
    static const enum Item items[] = {ITEM_SITRUS_BERRY, ITEM_LEFTOVERS, ITEM_NONE};
    static const u32 statuses[] = {STATUS1_PARALYSIS, STATUS1_TOXIC_POISON, 0};
    u32 personalities[3], hp[3];
    ZeroPlayerPartyMons();
    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        CreateMon(mon, species[i], 20, 1234 + i, OTID_STRUCT_PLAYER_ID);
        CalculateMonStats(mon);
        hp[i] = i == 0 ? 1 : i == 1 ? 0 : GetMonData(mon, MON_DATA_MAX_HP);
        SetMonData(mon, MON_DATA_HP, &hp[i]);
        SetMonData(mon, MON_DATA_STATUS, &statuses[i]);
        SetMonData(mon, MON_DATA_HELD_ITEM, &items[i]);
        u32 egg = i == 2;
        SetMonData(mon, MON_DATA_IS_EGG, &egg);
        SetMonMoveSlot(mon, MOVE_PROTECT, 0);
        u32 pp = 3;
        SetMonData(mon, MON_DATA_PP1, &pp);
        personalities[i] = GetMonData(mon, MON_DATA_PERSONALITY);
    }
    CalculatePlayerPartyCount();
    SavePlayerParty();
    ZeroPlayerPartyMons();
    LoadPlayerParty();
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], 3);
    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), species[i]);
        EXPECT_EQ(GetMonData(mon, MON_DATA_PERSONALITY), personalities[i]);
        EXPECT_EQ(GetMonData(mon, MON_DATA_HP), hp[i]);
        EXPECT_EQ(GetMonData(mon, MON_DATA_STATUS), statuses[i]);
        EXPECT_EQ(GetMonData(mon, MON_DATA_HELD_ITEM), items[i]);
        EXPECT_EQ(GetMonData(mon, MON_DATA_IS_EGG), i == 2);
        EXPECT_EQ(GetMonData(mon, MON_DATA_MOVE1), MOVE_PROTECT);
        EXPECT_EQ(GetMonData(mon, MON_DATA_PP1), 3);
    }
    for (u32 i = 3; i < PARTY_SIZE; i++)
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES), SPECIES_NONE);
    ZeroPlayerPartyMons();
    CalculatePlayerPartyCount();
}
