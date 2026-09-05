#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "battle_gimmick.h"
#include "battle_util.h"
#include "caps.h"
#include "champions_circuit.h"
#include "difficulty.h"
#include "emerald_champions_battle_sets.h"
#include "event_data.h"
#include "field_move.h"
#include "field_player_avatar.h"
#include "field_specials.h"
#include "gym_leader_rematch.h"
#include "item.h"
#include "legendary_signs.h"
#include "load_save.h"
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
#if FREE_MATCH_CALL == FALSE
    gSaveBlock1Ptr->trainerRematches[REMATCH_ROSE] = 1;
#endif

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
    EXPECT(!IsEmeraldChampionsGimmickAllowed(GIMMICK_TERA));
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
    EXPECT_EQ(canonicalCount, 372);
    EXPECT(hasTailwind);
    EXPECT(hasWillOWisp);
    EXPECT(hasPreparationOnlyMove);
    EXPECT(!CanLearnTeachableMove(SPECIES_MEW, MOVE_TAILWIND));
    EXPECT(!CanLearnTeachableMove(SPECIES_MEW, MOVE_WILL_O_WISP));

    CreateMon(&mon, SPECIES_MEW, 50, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(
        GetEmeraldChampionsPreparationMovesToLearn(&mon.box, sEmeraldChampionsPreparationMoveBuffer),
        372);

    SetMonMoveSlot(&mon, MOVE_PSYCHIC, 0);
    SetMonMoveSlot(&mon, MOVE_TAILWIND, 1);
    SetMonMoveSlot(&mon, MOVE_WILL_O_WISP, 2);
    SetMonMoveSlot(&mon, MOVE_PROTECT, 3);
    EXPECT_EQ(
        GetEmeraldChampionsPreparationMovesToLearn(&mon.box, sEmeraldChampionsPreparationMoveBuffer),
        368);
    for (u32 i = 0; i < 368; i++)
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

TEST("Emerald Champions disables the Bag only in competitive trainer battles")
{
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    EXPECT(!IsAllowedToUseBag());
    gBattleTypeFlags = 0;
    EXPECT(IsAllowedToUseBag());
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_PYRAMID;
    EXPECT(IsAllowedToUseBag());
}

TEST("Emerald Champions forces instant text for legacy option values")
{
    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_SLOW;
    EXPECT_EQ(GetPlayerTextSpeed(), OPTIONS_TEXT_SPEED_INSTANT);
    EXPECT_EQ(GetPlayerTextSpeedDelay(), 1);

    gSaveBlock2Ptr->optionsTextSpeed = OPTIONS_TEXT_SPEED_FAST;
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

TEST("Emerald Champions live difficulty changes only trainer levels")
{
    struct Pokemon *party = gParties[B_TRAINER_OPPONENT_A];

    ZeroEnemyPartyMons();
    CreateMon(&party[0], SPECIES_PIKACHU, 20, 0, OTID_STRUCT_PLAYER_ID);

    SetCurrentDifficultyLevel(DIFFICULTY_HARD);
    ApplyTrainerLevelDifficulty(party);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_LEVEL), 20);

    SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
    ApplyTrainerLevelDifficulty(party);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_LEVEL), 18);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_HP), GetMonData(&party[0], MON_DATA_MAX_HP));

    CreateMon(&party[0], SPECIES_PIKACHU, 20, 0, OTID_STRUCT_PLAYER_ID);
    SetCurrentDifficultyLevel(DIFFICULTY_EASY);
    ApplyTrainerLevelDifficulty(party);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_LEVEL), 16);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PIKACHU);

    ZeroEnemyPartyMons();
    CreateMon(&party[0], SPECIES_EEVEE, 3, 0, OTID_STRUCT_PLAYER_ID);
    ApplyTrainerLevelDifficulty(party);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_LEVEL), 1);

    SetCurrentDifficultyLevel(DIFFICULTY_HARD);
    ZeroEnemyPartyMons();
}

TEST("Emerald Champions level caps follow every campaign milestone")
{
    static const u16 badges[] =
    {
        FLAG_BADGE01_GET,
        FLAG_BADGE02_GET,
        FLAG_BADGE03_GET,
        FLAG_BADGE04_GET,
        FLAG_BADGE05_GET,
        FLAG_BADGE06_GET,
        FLAG_BADGE07_GET,
        FLAG_BADGE08_GET,
    };
    static const u8 caps[] = {14, 20, 30, 40, 45, 55, 60, 70, 80};

    FlagClear(FLAG_IS_CHAMPION);
    for (u32 i = 0; i < ARRAY_COUNT(badges); i++)
        FlagClear(badges[i]);
    for (u32 i = 0; i < ARRAY_COUNT(caps); i++)
    {
        EXPECT_EQ(GetCurrentLevelCap(), caps[i]);
        if (i < ARRAY_COUNT(badges))
            FlagSet(badges[i]);
    }
    FlagSet(FLAG_IS_CHAMPION);
    EXPECT_EQ(GetCurrentLevelCap(), MAX_LEVEL);
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

TEST("Emerald Champions applies a complete authored battle set")
{
    struct Pokemon mon;
    u32 statPointTotal = 0;

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
        statPointTotal += GetMonData(&mon, MON_DATA_HP_EV + i);
    EXPECT_EQ(statPointTotal, 66);
}

TEST("Emerald Champions exposes named Doubles and Singles sets for every direct species")
{
    struct Pokemon mon;

    ClearBag();
    for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        if (gEmeraldChampionsDefaultBattleSets[species].moves[0] == MOVE_NONE)
            continue;
        enum Item formItem = gEmeraldChampionsDefaultBattleSets[species].item;

        CreateMon(&mon, species, 50, 0, OTID_STRUCT_PLAYER_ID);
        // Plate/relic forms only exist while holding the item that defines
        // them, so a bare mon of that form is not a state the player can
        // reach. Model the reachable one instead of hiding every set.
        if (IsEmeraldChampionsProtectedProgressionItem(formItem))
            SetMonData(&mon, MON_DATA_HELD_ITEM, &formItem);
        EXPECT_GE(GetEmeraldChampionsBattleSetCountForFormat(&mon, EC_BATTLE_FORMAT_DOUBLES), 2);
        EXPECT_GE(GetEmeraldChampionsBattleSetCountForFormat(&mon, EC_BATTLE_FORMAT_SINGLES), 1);
        for (u8 format = 0; format < EC_BATTLE_FORMAT_COUNT; format++)
        {
            u8 count = GetEmeraldChampionsBattleSetCountForFormat(&mon, format);
            for (u8 choice = 0; choice < count; choice++)
            {
                const u8 *name = GetEmeraldChampionsBattleSetNameForFormat(&mon, choice, format);
                EXPECT(StringCompare(name, COMPOUND_STRING("Recommended")) != 0);
                EXPECT_NE(
                    ApplyEmeraldChampionsBattleSetChoiceForFormat(&mon, choice, format),
                    EC_BATTLE_SET_FAILED
                );
            }
        }
    }
    ClearBag();
}

TEST("Emerald Champions evolution applies the evolved Doubles recommendation")
{
    struct Pokemon mon;
    enum Species species = SPECIES_BEAUTIFLY;
    enum Item protectedItem = ITEM_LINKING_CORD;

    CreateMon(&mon, SPECIES_WURMPLE, 14, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(ApplyEmeraldChampionsBattleSetChoice(&mon, 1), EC_BATTLE_SET_SUCCESS);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE4), MOVE_STRING_SHOT);
    SetMonData(&mon, MON_DATA_HELD_ITEM, &protectedItem);
    SetMonData(&mon, MON_DATA_SPECIES, &species);

    EXPECT_EQ(ApplyEmeraldChampionsRecommendedEvolutionSet(&mon), EC_BATTLE_SET_SUCCESS);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE1), MOVE_QUIVER_DANCE);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE2), MOVE_BUG_BUZZ);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE3), MOVE_AIR_CUTTER);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE4), MOVE_PROTECT);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), ITEM_LINKING_CORD);
    for (u32 stat = 0; stat < NUM_STATS; stat++)
        EXPECT_EQ(GetMonData(&mon, MON_DATA_HP_IV + stat), MAX_PER_STAT_IVS);

    // Scovillain's raw slot zero is Mega-oriented. Evolution must skip it and
    // choose the first ordinary campaign role instead.
    CreateMon(&mon, SPECIES_SCOVILLAIN, 40, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(ApplyEmeraldChampionsRecommendedEvolutionSet(&mon), EC_BATTLE_SET_SUCCESS);
    EXPECT_NE(GetMonData(&mon, MON_DATA_HELD_ITEM), ITEM_SCOVILLAINITE);
}

TEST("Emerald Champions Stat Point editor clamps every spread to 32 and 66")
{
    u32 total = 0;

    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_BULBASAUR, 14, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    EXPECT_EQ(
        ApplyEmeraldChampionsBattleSetChoice(&gParties[B_TRAINER_PLAYER][0], 0),
        EC_BATTLE_SET_SUCCESS
    );
    gSpecialVar_0x800A = 0;
    gSpecialVar_0x8005 = STAT_HP;
    gSpecialVar_0x8006 = 2; // -1
    AdjustSelectedMonEmeraldChampionsStatPoints();
    EXPECT_EQ(gSpecialVar_Result, TRUE);

    gSpecialVar_0x8005 = STAT_ATK;
    gSpecialVar_0x8006 = 7; // Set Maximum, limited by the one free point.
    AdjustSelectedMonEmeraldChampionsStatPoints();
    for (u32 stat = 0; stat < NUM_STATS; stat++)
    {
        u32 value = GetMonData(&gParties[B_TRAINER_PLAYER][0], EC_STAT_POINT_DATA(stat));
        EXPECT_LE(value, EC_STAT_POINTS_PER_STAT);
        total += value;
    }
    EXPECT_EQ(total, EC_STAT_POINT_BUDGET);

    // A capped increase is rejected so the field script can play native
    // failure feedback instead of silently redrawing an unchanged value.
    gSpecialVar_0x8005 = STAT_ATK;
    gSpecialVar_0x8006 = 3; // +1 with no points remaining.
    AdjustSelectedMonEmeraldChampionsStatPoints();
    EXPECT_EQ(gSpecialVar_Result, FALSE);

    ResetSelectedMonEmeraldChampionsStatPoints();
    total = 0;
    for (u32 stat = 0; stat < NUM_STATS; stat++)
        total += GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP_EV + stat);
    EXPECT_EQ(total, 0);
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

TEST("Emerald Champions migrates Linking Cord into one reusable Key Item")
{
    ClearBag();
    BagPocket_SetSlotItemIdAndCount(&gBagPockets[POCKET_ITEMS], 0, ITEM_LINKING_CORD, 12);

    MigrateEmeraldChampionsLinkingCord();

    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_LINKING_CORD), 1);
    for (u32 i = 0; i < gBagPockets[POCKET_ITEMS].capacity; i++)
        EXPECT_NE(GetBagItemId(POCKET_ITEMS, i), ITEM_LINKING_CORD);

    // The migration is idempotent and cannot produce a second Key Item.
    MigrateEmeraldChampionsLinkingCord();
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_LINKING_CORD), 1);
}

TEST("Emerald Champions migrates the exact 81e Sign Circuit and difficulty layout")
{
    static const u16 signVars[] =
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

    VarSet(VAR_EMERALD_CHAMPIONS_SAVE_VERSION, 0);
    FlagSet(FLAG_UNUSED_0x91E); // 81e gym-reward migration marker.
    FlagSet(FLAG_UNUSED_0x91F); // 81e item-ball migration marker.
    FlagSet(FLAG_EC_CAUGHT_SHAYMIN); // 81e difficulty migration marker at 0x4F9.
    FlagClear(FLAG_EC_BESPOKE_TRAINER_FLAGS_MIGRATED); // Raw 81e save without the colliding defeated-Zygarde bit.
    for (u32 i = 0; i < ARRAY_COUNT(signVars); i++)
        VarSet(signVars[i], 0xFFFF);
    // 81e used 0x40F7-0x40FA for unlocked bits, 0x40FB-0x40FE
    // for caught bits, and 0x40FF for lifetime Circuit wins.
    VarSet(0x40F7, 1u << LEGENDARY_SIGN_CELEBI);
    VarSet(0x40F8, 0);
    VarSet(0x40F9, 1u << (LEGENDARY_SIGN_SHAYMIN - 32));
    VarSet(0x40FA, 1u << (LEGENDARY_SIGN_ZYGARDE - 48));
    VarSet(0x40FB, 1u << LEGENDARY_SIGN_CELEBI);
    VarSet(0x40FC, 0);
    VarSet(0x40FD, 1u << (LEGENDARY_SIGN_SHAYMIN - 32));
    VarSet(0x40FE, 1u << (LEGENDARY_SIGN_ZYGARDE - 48));
    VarSet(0x40FF, 37);
    gSaveBlock2Ptr->optionsTextSpeed = 2; // 81e Easy.
    VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, 9);
    VarSet(VAR_CHAMPIONS_CIRCUIT_ACTIVE, 1);

    // Every row models an unrelated live 81e bit that collides with current
    // content and therefore must not suppress that content after migration.
    FlagSet(FLAG_EC_STARTER_ARCHIVE_BULBASAUR);
    FlagSet(FLAG_RECEIVED_GAME_CORNER_GENESECT);
    FlagSet(FLAG_HIDE_ROUTE111_VIAL_CHANSEY);
    FlagSet(FLAG_EC_CAUGHT_ARTICUNO);
    FlagSet(FLAG_RECEIVED_BRAWLY_LUCARIONITE);
    FlagSet(FLAG_EC_ITEM_PRISON_BOTTLE);
    FlagSet(FLAG_HIDDEN_ITEM_ROUTE_113_ULTRA_BALL);
    FlagSet(FLAG_ITEM_ROUTE_116_LUCARIONITE_Z);

    MigrateEmeraldChampionsCoreState();

    EXPECT_EQ(VarGet(VAR_EMERALD_CHAMPIONS_SAVE_VERSION), EMERALD_CHAMPIONS_SAVE_VERSION_CURRENT);
    EXPECT_EQ(GetCurrentDifficultyLevel(), DIFFICULTY_EASY);
    EXPECT(FlagGet(FLAG_EC_BESPOKE_TRAINER_FLAGS_MIGRATED));
    EXPECT(!FlagGet(FLAG_UNUSED_0x91E));
    EXPECT(!FlagGet(FLAG_UNUSED_0x91F));
    EXPECT(IsLegendarySignCaught(LEGENDARY_SIGN_CELEBI));
    EXPECT(IsLegendarySignCaught(LEGENDARY_SIGN_SHAYMIN));
    EXPECT(IsLegendarySignCaught(LEGENDARY_SIGN_ZYGARDE));
    EXPECT(!IsLegendarySignCaught(LEGENDARY_SIGN_ARTICUNO));
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_UNLOCKED_4), 0);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_UNLOCKED_5), 0);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_CAUGHT_4), 0);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_CAUGHT_5), 0);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS), 0);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS), 37);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_ACTIVE), 0);
    EXPECT(FlagGet(FLAG_EC_CAUGHT_CELEBI));
    EXPECT(FlagGet(FLAG_EC_CAUGHT_SHAYMIN));
    EXPECT(FlagGet(FLAG_EC_CAUGHT_ZYGARDE));
    EXPECT(!FlagGet(FLAG_EC_CAUGHT_ARTICUNO));
    EXPECT(!FlagGet(FLAG_EC_STARTER_ARCHIVE_BULBASAUR));
    EXPECT(!FlagGet(FLAG_RECEIVED_GAME_CORNER_GENESECT));
    EXPECT(!FlagGet(FLAG_HIDE_ROUTE111_VIAL_CHANSEY));
    EXPECT(!FlagGet(FLAG_RECEIVED_BRAWLY_LUCARIONITE));
    EXPECT(!FlagGet(FLAG_EC_ITEM_PRISON_BOTTLE));
    EXPECT(!FlagGet(FLAG_HIDDEN_ITEM_ROUTE_113_ULTRA_BALL));
    EXPECT(!FlagGet(FLAG_ITEM_ROUTE_116_LUCARIONITE_Z));

    // The version, not a repurposed flag, makes the migration idempotent.
    SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
    VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 41);
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_4, 0xA55A);
    FlagSet(FLAG_EC_STARTER_ARCHIVE_BULBASAUR);
    MigrateEmeraldChampionsCoreState();
    EXPECT_EQ(GetCurrentDifficultyLevel(), DIFFICULTY_NORMAL);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS), 41);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_CAUGHT_4), 0xA55A);
    EXPECT(FlagGet(FLAG_EC_STARTER_ARCHIVE_BULBASAUR));
}

TEST("Emerald Champions ambiguous unversioned saves fail safe")
{
    VarSet(VAR_EMERALD_CHAMPIONS_SAVE_VERSION, 0);
    FlagSet(FLAG_UNUSED_0x91E);
    FlagClear(FLAG_UNUSED_0x91F); // Partial legacy signature is deliberately ambiguous.
    FlagSet(FLAG_EC_BESPOKE_TRAINER_FLAGS_MIGRATED);
    FlagSet(FLAG_EC_CAUGHT_SHAYMIN);
    FlagSet(FLAG_EC_STARTER_ARCHIVE_CHARMANDER);
    FlagSet(FLAG_RECEIVED_GAME_CORNER_POIPOLE);
    FlagSet(FLAG_EC_ITEM_MASTER_BALL);
    FlagSet(FLAG_HIDDEN_ITEM_ROUTE_113_ULTRA_BALL);
    FlagSet(FLAG_ITEM_ROUTE_116_LUCARIONITE_Z);
    for (u32 i = 0; i < ARRAY_COUNT(sEmeraldChampionsTestSignStateVars); i++)
        VarSet(sEmeraldChampionsTestSignStateVars[i], 0xFFFF);
    SetCurrentDifficultyLevel(DIFFICULTY_EASY);
    VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, 12);
    VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 34);
    VarSet(VAR_CHAMPIONS_CIRCUIT_ACTIVE, 1);

    MigrateEmeraldChampionsCoreState();

    EXPECT_EQ(VarGet(VAR_EMERALD_CHAMPIONS_SAVE_VERSION), EMERALD_CHAMPIONS_SAVE_VERSION_CURRENT);
    EXPECT_EQ(GetCurrentDifficultyLevel(), DIFFICULTY_HARD);
    EXPECT(FlagGet(FLAG_EC_BESPOKE_TRAINER_FLAGS_MIGRATED));
    EXPECT(!FlagGet(FLAG_UNUSED_0x91E));
    EXPECT(!FlagGet(FLAG_UNUSED_0x91F));
    EXPECT(!FlagGet(FLAG_EC_CAUGHT_SHAYMIN));
    EXPECT(!FlagGet(FLAG_EC_STARTER_ARCHIVE_CHARMANDER));
    EXPECT(!FlagGet(FLAG_RECEIVED_GAME_CORNER_POIPOLE));
    EXPECT(!FlagGet(FLAG_EC_ITEM_MASTER_BALL));
    EXPECT(!FlagGet(FLAG_HIDDEN_ITEM_ROUTE_113_ULTRA_BALL));
    EXPECT(!FlagGet(FLAG_ITEM_ROUTE_116_LUCARIONITE_Z));
    for (u32 i = 0; i < ARRAY_COUNT(sEmeraldChampionsTestSignStateVars); i++)
        EXPECT_EQ(VarGet(sEmeraldChampionsTestSignStateVars[i]), 0);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS), 0);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS), 0);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_ACTIVE), 0);
    EXPECT(FlagGet(FLAG_HIDE_LEGENDARY_SIGN_DARKRAI));
}

TEST("Emerald Champions unversioned e7 saves preserve Shaymin and current state")
{
    VarSet(VAR_EMERALD_CHAMPIONS_SAVE_VERSION, 0);
    FlagClear(FLAG_UNUSED_0x91E);
    FlagClear(FLAG_UNUSED_0x91F);
    FlagSet(FLAG_EC_BESPOKE_TRAINER_FLAGS_MIGRATED);
    FlagSet(FLAG_EC_CAUGHT_SHAYMIN);
    FlagSet(FLAG_EC_STARTER_ARCHIVE_BULBASAUR);
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0xA55A);
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_2, 1u << (LEGENDARY_SIGN_SHAYMIN - 32));
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_4, 0x5AA5);
    VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 23);
    VarSet(VAR_CHANSEY_NURSE_STATE, 6);
    SetCurrentDifficultyLevel(DIFFICULTY_EASY);

    MigrateEmeraldChampionsCoreState();

    EXPECT_EQ(VarGet(VAR_EMERALD_CHAMPIONS_SAVE_VERSION), EMERALD_CHAMPIONS_SAVE_VERSION_CURRENT);
    EXPECT_EQ(GetCurrentDifficultyLevel(), DIFFICULTY_EASY);
    EXPECT(FlagGet(FLAG_EC_BESPOKE_TRAINER_FLAGS_MIGRATED));
    EXPECT(FlagGet(FLAG_EC_CAUGHT_SHAYMIN));
    EXPECT(FlagGet(FLAG_EC_STARTER_ARCHIVE_BULBASAUR));
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_CAUGHT_0), 0xA55A);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_CAUGHT_2), 1u << (LEGENDARY_SIGN_SHAYMIN - 32));
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_CAUGHT_4), 0x5AA5);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS), 23);
    EXPECT_EQ(VarGet(VAR_CHANSEY_NURSE_STATE), 6);
}

TEST("Emerald Champions overlapping 81e Zygarde and e7 lineage fails safe")
{
    VarSet(VAR_EMERALD_CHAMPIONS_SAVE_VERSION, 0);
    FlagSet(FLAG_UNUSED_0x91E);
    FlagSet(FLAG_UNUSED_0x91F);
    FlagSet(FLAG_EC_BESPOKE_TRAINER_FLAGS_MIGRATED);
    FlagSet(FLAG_EC_CAUGHT_SHAYMIN);
    FlagSet(FLAG_EC_STARTER_ARCHIVE_BULBASAUR);
    SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
    VarSet(VAR_LEGENDARY_SIGNS_UNLOCKED_0, 0x1357);
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0x2468);
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_2, 1u << (LEGENDARY_SIGN_SHAYMIN - 32));
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_4, 0x5AA5);
    VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, 7);
    VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 23);
    VarSet(VAR_CHAMPIONS_CIRCUIT_ACTIVE, 1);

    MigrateEmeraldChampionsCoreState();

    EXPECT_EQ(VarGet(VAR_EMERALD_CHAMPIONS_SAVE_VERSION), EMERALD_CHAMPIONS_SAVE_VERSION_CURRENT);
    EXPECT_EQ(GetCurrentDifficultyLevel(), DIFFICULTY_HARD);
    EXPECT(!FlagGet(FLAG_UNUSED_0x91E));
    EXPECT(!FlagGet(FLAG_UNUSED_0x91F));
    EXPECT(FlagGet(FLAG_EC_BESPOKE_TRAINER_FLAGS_MIGRATED));
    EXPECT(!FlagGet(FLAG_EC_CAUGHT_SHAYMIN));
    EXPECT(!FlagGet(FLAG_EC_STARTER_ARCHIVE_BULBASAUR));
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_UNLOCKED_0), 0);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_CAUGHT_0), 0);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_CAUGHT_2), 0);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_SIGNS_CAUGHT_4), 0);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS), 0);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS), 0);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_ACTIVE), 0);
}

TEST("Emerald Champions battle-ready wild presets exclude special encounters")
{
    EXPECT(IsEmeraldChampionsOrdinaryWildSpecies(SPECIES_BULBASAUR));
    EXPECT(IsEmeraldChampionsOrdinaryWildSpecies(SPECIES_NIHILEGO));
    EXPECT(IsEmeraldChampionsOrdinaryWildSpecies(SPECIES_GREAT_TUSK));
    EXPECT(!IsEmeraldChampionsOrdinaryWildSpecies(SPECIES_MEW));
    EXPECT(!IsEmeraldChampionsOrdinaryWildSpecies(SPECIES_VENUSAUR_MEGA));
}

TEST("Emerald Champions ordinary wild creation applies a prepared non-Mega set")
{
    ClearBag();
    EXPECT(AddBagItem(ITEM_MEGA_RING, 1));
    SeedRng(17);

    CreateWildMon(SPECIES_CHARIZARD, 35);

    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES), SPECIES_CHARIZARD);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL), 35);
    EXPECT(MonMatchesEmeraldChampionsNonMegaPreset(&gParties[B_TRAINER_OPPONENT_A][0]));
    EXPECT_NE(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HELD_ITEM), ITEM_CHARIZARDITE_X);
    EXPECT_NE(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HELD_ITEM), ITEM_CHARIZARDITE_Y);
    ClearBag();
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

TEST("Emerald Champions Game Corner delivers a prepared alternate starter transactionally")
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
    EXPECT_NE(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);
    EXPECT(MonMatchesEmeraldChampionsNonMegaPreset(&gParties[B_TRAINER_PLAYER][0]));
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

TEST("Emerald Champions story gifts arrive battle-ready with restoration baselines")
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
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_LEVEL), levels[slot]);
        EXPECT(MonMatchesEmeraldChampionsNonMegaPreset(&gParties[B_TRAINER_PLAYER][slot]));
        item = GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HELD_ITEM);
        EXPECT_NE(item, ITEM_NONE);
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
    EXPECT(BoxMonMatchesEmeraldChampionsNonMegaPreset(&gPokemonStoragePtr->boxes[0][0]));

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

TEST("Emerald Champions conditional Signs awaken at their marked place")
{
    static const u16 signStateVars[] =
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
    enum Species species = SPECIES_NONE;
    u8 level = 0;

    for (u32 i = 0; i < ARRAY_COUNT(signStateVars); i++)
        VarSet(signStateVars[i], 0);
    for (u32 i = 0; i < NUM_BADGES; i++)
        FlagClear(FLAG_BADGE01_GET + i);
    FlagSet(FLAG_BADGE01_GET);

    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_RIOLU, 20, 0, OTID_STRUCT_PLAYER_ID);
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_GRANITE_CAVE_B2F);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_GRANITE_CAVE_B2F);

    EXPECT(!IsLegendarySignUnlocked(LEGENDARY_SIGN_COBALION));
    TryGetLegendarySignWildOverride(WILD_AREA_LAND, &species, &level);
    EXPECT(IsLegendarySignUnlocked(LEGENDARY_SIGN_COBALION));
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

TEST("Emerald Champions Arceus mastery requires every finite Sign source")
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

    ZeroPlayerPartyMons();
    for (u32 i = 0; i < ARRAY_COUNT(caughtVars); i++)
        VarSet(caughtVars[i], 0);
    for (enum LegendarySignId signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
    {
        if (signId == LEGENDARY_SIGN_ARCEUS || signId == LEGENDARY_SIGN_GENESECT)
            continue;
        MarkLegendarySignCaughtBySpecies(gLegendarySignDefinitions[signId].species);
    }

    TryGiveArceusLegendarySignMasteryReward();
    EXPECT_EQ(gSpecialVar_Result, 0);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_NONE);

    MarkLegendarySignCaughtBySpecies(SPECIES_GENESECT);
    TryGiveArceusLegendarySignMasteryReward();
    EXPECT_EQ(gSpecialVar_Result, 1);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_ARCEUS);
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
            u32 statPointTotal = 0;
            u8 result;

            CreateMon(&mon, species, 50, 0, OTID_STRUCT_PLAYER_ID);
            result = ApplyEmeraldChampionsOpponentSet(&mon, choice);
            if (result != EC_BATTLE_SET_SUCCESS && result != EC_BATTLE_SET_MEGA)
                Test_MgbaPrintf("illegal imported set species=%d choice=%d ability=%d item=%d required=%d", species, choice, preset->ability, preset->item, preset->requiredItem);
            EXPECT(result == EC_BATTLE_SET_SUCCESS || result == EC_BATTLE_SET_MEGA);
            EXPECT_NE(GetMonAbility(&mon), ABILITY_NONE);
            if (gEmeraldChampionsDefaultBattleSets[species].moves[0] != MOVE_NONE
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
                statPointTotal += preset->statPoints[stat];
            EXPECT_EQ(statPointTotal, 66);
            if (preset->requiredItem != ITEM_NONE)
                EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), preset->requiredItem);
        }
    }
    EXPECT_EQ(abilityFallbacks, 0);
}

TEST("Emerald Champions reviewed move-access exceptions are natively tutor-accessible")
{
    EXPECT_EQ(ARRAY_COUNT(sReviewedMoveAccess), 55);
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

        if (gEmeraldChampionsDefaultBattleSets[species].moves[0] == MOVE_NONE)
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

        CreateMon(&gParties[B_TRAINER_PLAYER][slot], SPECIES_BULBASAUR, 20 + slot, 0, OTID_STRUCT_PLAYER_ID);
        SetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HELD_ITEM, &item);
        originalLevels[slot] = GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_LEVEL);
    }
    ChampionsCircuitBegin();
    VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, 5);
    VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 9);

    gBattleOutcome = B_OUTCOME_WON;
    ChampionsCircuitHandleBattleResult();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS), 6);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS), 10);
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HP), GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_MAX_HP));

    gBattleOutcome = B_OUTCOME_LOST;
    ChampionsCircuitHandleBattleResult();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_ACTIVE), FALSE);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS), 0);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS), 10);
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
    {
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_LEVEL), originalLevels[slot]);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HELD_ITEM), items[slot]);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_HP), GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_MAX_HP));
    }
}

TEST("Champions Circuit sends earned rewards to the PC")
{
    ZeroPlayerPartyMons();
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        CreateMon(&gParties[B_TRAINER_PLAYER][slot], SPECIES_RATTATA, 20, 0, OTID_STRUCT_PLAYER_ID);
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
    ClearEmeraldChampionsLegendaryCaughtState();
    VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, 0);
    VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 2);

    ChampionsCircuitTryGiveReward();

    EXPECT_EQ(gSpecialVar_Result, 2);
    EXPECT_EQ(GetBoxMonData(&gPokemonStoragePtr->boxes[0][0], MON_DATA_SPECIES), SPECIES_CALYREX);
    EXPECT(IsLegendarySignCaught(LEGENDARY_SIGN_CALYREX));
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS), 0);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS), 2);
}

TEST("Champions Circuit full-PC rewards remain claimable without another win")
{
    ZeroPlayerPartyMons();
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        CreateMon(&gParties[B_TRAINER_PLAYER][slot], SPECIES_RATTATA, 20, 0, OTID_STRUCT_PLAYER_ID);
    FillEmeraldChampionsPokemonStorage();
    ClearEmeraldChampionsLegendaryCaughtState();
    VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, 0);
    VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 2);

    ChampionsCircuitTryGiveReward();
    EXPECT_EQ(gSpecialVar_Result, 3);
    EXPECT(!IsLegendarySignCaught(LEGENDARY_SIGN_CALYREX));

    memset(&gPokemonStoragePtr->boxes[0][0], 0, sizeof(gPokemonStoragePtr->boxes[0][0]));
    ChampionsCircuitTryGiveReward();
    EXPECT_EQ(gSpecialVar_Result, 2);
    EXPECT_EQ(GetBoxMonData(&gPokemonStoragePtr->boxes[0][0], MON_DATA_SPECIES), SPECIES_CALYREX);
    EXPECT(IsLegendarySignCaught(LEGENDARY_SIGN_CALYREX));
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS), 0);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS), 2);
}

TEST("Champions Circuit mastery waits for every finite Circuit reward")
{
    ZeroPlayerPartyMons();
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        CreateMon(&gParties[B_TRAINER_PLAYER][slot], SPECIES_RATTATA, 20, 0, OTID_STRUCT_PLAYER_ID);
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
    ClearEmeraldChampionsLegendaryCaughtState();
    VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 40);

    ChampionsCircuitTryGiveReward();
    EXPECT(IsLegendarySignCaught(LEGENDARY_SIGN_CALYREX));
    EXPECT(!IsLegendarySignCaught(LEGENDARY_SIGN_ETERNATUS));

    for (enum LegendarySignId signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
        if (gLegendarySignDefinitions[signId].source == LEGENDARY_SOURCE_CIRCUIT)
            MarkLegendarySignCaughtBySpecies(gLegendarySignDefinitions[signId].species);
    ChampionsCircuitTryGiveReward();
    EXPECT_EQ(gSpecialVar_Result, 2);
    EXPECT(IsLegendarySignCaught(LEGENDARY_SIGN_ETERNATUS));
}

TEST("Champions Circuit generates live Showdown doubles teams")
{
    static EWRAM_DATA bool8 seenSpecies[NUM_SPECIES];
    u32 diversity = 0;

    SetCurrentDifficultyLevel(DIFFICULTY_HARD);
    memset(seenSpecies, 0, sizeof(seenSpecies));
    // Sixteen live generations exercise 96 complete sets while staying below
    // the GBA test runner's per-test cycle budget.
    for (u32 seed = 1; seed <= 16; seed++)
    {
        u32 megaStoneCount = 0;

        SeedRng(seed);
        VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, seed % 48);
        ChampionsCircuitGenerateOpponent();
        EXPECT_EQ(gSpecialVar_Result, PARTY_SIZE);
        EXPECT_EQ(gPartiesCount[B_TRAINER_OPPONENT_A], PARTY_SIZE);

        for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        {
            enum Species species = GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES);
            u8 expectedLevel = min(MAX_LEVEL, 80 + (seed % 48) / PARTY_SIZE);
            u32 statPointTotal = 0;
            u32 moveCount = 0;
            bool32 reachedEmptyMove = FALSE;

            if (slot < (seed % 48) % PARTY_SIZE && expectedLevel < MAX_LEVEL)
                expectedLevel++;
            EXPECT_NE(species, SPECIES_NONE);
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_LEVEL), expectedLevel);
            EXPECT_NE(GetMonAbility(&gParties[B_TRAINER_OPPONENT_A][slot]), ABILITY_NONE);
            if (gItemsInfo[GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HELD_ITEM)].sortType
             == ITEM_TYPE_MEGA_STONE)
                megaStoneCount++;
            for (u32 stat = 0; stat < NUM_STATS; stat++)
                statPointTotal += GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP_EV + stat);
            EXPECT_EQ(statPointTotal, 66);
            for (u32 move = 0; move < MAX_MON_MOVES; move++)
            {
                enum Move selectedMove = GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MOVE1 + move);
                if (selectedMove == MOVE_NONE)
                {
                    reachedEmptyMove = TRUE;
                    continue;
                }
                EXPECT(!reachedEmptyMove);
                moveCount++;
                for (u32 otherMove = 0; otherMove < move; otherMove++)
                    EXPECT_NE(selectedMove, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MOVE1 + otherMove));
            }
            // Showdown deliberately uses Transform-only Ditto and the
            // two-move Fake Out + Last Resort set; both are complete sets.
            EXPECT_GE(moveCount, 1);
            for (u32 other = 0; other < slot; other++)
            {
                enum Species otherSpecies = GetMonData(&gParties[B_TRAINER_OPPONENT_A][other], MON_DATA_SPECIES);
                EXPECT_NE(SpeciesToNationalPokedexNum(species), SpeciesToNationalPokedexNum(otherSpecies));
            }
            if (!seenSpecies[species])
            {
                seenSpecies[species] = TRUE;
                diversity++;
            }
        }
        EXPECT_LE(megaStoneCount, 1);
    }
    EXPECT_GE(diversity, 50);
}

TEST("Champions Circuit honors the live difficulty level reduction")
{
    static const enum DifficultyLevel difficulties[] =
    {
        DIFFICULTY_HARD,
        DIFFICULTY_NORMAL,
        DIFFICULTY_EASY,
    };
    static const u8 expectedLevels[] = {80, 78, 76};

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

        for (u32 templateIndex = variant->templateOffset;
             templateIndex < variant->templateOffset + variant->templateCount;
             templateIndex++)
        {
            const struct ShowdownCircuitTemplate *template = &gShowdownCircuitTemplates[templateIndex];

            for (u32 abilityIndex = 0; abilityIndex < template->abilityCount; abilityIndex++)
            {
                enum Ability ability = template->abilities[abilityIndex];
                bool32 found = FALSE;

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
    static const enum Species species[PARTY_SIZE] =
    {
        SPECIES_BULBASAUR,
        SPECIES_CHARMANDER,
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
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_LEVEL), 80);

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

static u32 EmeraldChampionsExpectedStat(enum Species species, u32 stat, u32 level, u32 points)
{
    u32 investment = min(2 * points, 63);
    u32 n = 2 * GetSpeciesBaseStat(species, stat) + MAX_PER_STAT_IVS + investment;

    if (stat == STAT_HP)
        return (n * level) / 100 + level + 10;
    return (n * level) / 100 + 5; // Hardy nature, no friendship boost.
}

static const u8 sEmeraldChampionsLevelCaps[] = {14, 20, 30, 40, 45, 55, 60, 70, 80};

TEST("Emerald Champions Stat Points change every stat at every level cap")
{
    static const u8 stats[] = {STAT_HP, STAT_ATK, STAT_DEF, STAT_SPEED, STAT_SPATK, STAT_SPDEF};
    u32 nature = NATURE_HARDY;
    u32 friendship = 0;

    for (u32 c = 0; c < ARRAY_COUNT(sEmeraldChampionsLevelCaps); c++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
        u32 level = sEmeraldChampionsLevelCaps[c];

        ZeroPlayerPartyMons();
        CreateMon(mon, SPECIES_ZIGZAGOON, level, 0, OTID_STRUCT_PLAYER_ID);
        SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
        SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);
        for (u32 s = 0; s < ARRAY_COUNT(stats); s++)
        {
            u8 zero = 0;
            u8 full = EC_STAT_POINTS_PER_STAT;
            u32 atZero;
            u32 atFull;

            SetMonData(mon, MON_DATA_HP_EV + stats[s], &zero);
            CalculateMonStats(mon);
            atZero = GetMonData(mon, MON_DATA_MAX_HP + stats[s]);
            EXPECT_EQ(atZero, EmeraldChampionsExpectedStat(SPECIES_ZIGZAGOON, stats[s], level, 0));

            SetMonData(mon, MON_DATA_HP_EV + stats[s], &full);
            CalculateMonStats(mon);
            atFull = GetMonData(mon, MON_DATA_MAX_HP + stats[s]);
            EXPECT_EQ(atFull, EmeraldChampionsExpectedStat(SPECIES_ZIGZAGOON, stats[s], level, EC_STAT_POINTS_PER_STAT));
            EXPECT_GT(atFull, atZero);

            SetMonData(mon, MON_DATA_HP_EV + stats[s], &zero);
        }
    }
}

TEST("Emerald Champions Belly Drum berry sets land on even HP at every level cap")
{
    static const enum Species species[] = {SPECIES_ZIGZAGOON, SPECIES_AZURILL, SPECIES_AZUMARILL};

    for (u32 s = 0; s < ARRAY_COUNT(species); s++)
    {
        for (u32 c = 0; c < ARRAY_COUNT(sEmeraldChampionsLevelCaps); c++)
        {
            struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
            s32 choice = -1;
            u32 total = 0;
            u8 value_restore;

            ZeroPlayerPartyMons();
            CreateMon(mon, species[s], sEmeraldChampionsLevelCaps[c], 0, OTID_STRUCT_PLAYER_ID);
            CalculatePlayerPartyCount();
            for (u32 i = 0; i < GetEmeraldChampionsBattleSetCount(mon); i++)
            {
                const struct EmeraldChampionsBattleSet *preset =
                    GetEmeraldChampionsBattleSetPresetForFormat(mon, i, EC_BATTLE_FORMAT_DOUBLES);
                bool32 drum = FALSE;

                for (u32 m = 0; preset != NULL && m < MAX_MON_MOVES; m++)
                    drum |= preset->moves[m] == MOVE_BELLY_DRUM;
                bool32 halfHpBerry = preset != NULL
                                  && (preset->item == ITEM_SITRUS_BERRY
                                   || (preset->ability == ABILITY_GLUTTONY
                                    && gItemsInfo[preset->item].holdEffect == HOLD_EFFECT_CONFUSE_FLAVOR));

                if (drum && halfHpBerry)
                {
                    choice = i;
                    break;
                }
            }
            EXPECT_GE(choice, 0);
            EXPECT_EQ(ApplyEmeraldChampionsBattleSetChoice(mon, choice), EC_BATTLE_SET_SUCCESS);
            // Belly Drum leaves ceil(maxHP / 2); a half-HP berry fires at
            // floor(maxHP / 2), so the two meet only on even HP. The
            // normalizer re-lands the authored spread within the point
            // budget, so parity is required exactly where some legal HP
            // value can reach it: a spread already spending the full budget
            // on other stats has no point to move.
            {
                u32 authoredHp = GetMonData(mon, MON_DATA_HP_EV);
                u32 spent = 0;
                bool32 parityReachable = FALSE;

                for (u32 stat = 0; stat < NUM_STATS; stat++)
                    spent += GetMonData(mon, MON_DATA_HP_EV + stat);
                // The normalizer may only deviate by the amount
                // GetEmeraldChampionsCurrentBattleSetChoice still recognizes,
                // so parity is required exactly where that window reaches it.
                for (u32 points = authoredHp > 2 ? authoredHp - 2 : 0;
                     points <= authoredHp + 2 && points <= EC_STAT_POINTS_PER_STAT;
                     points++)
                {
                    u8 value = points;

                    if (spent - authoredHp + points > EC_STAT_POINT_BUDGET)
                        continue;
                    SetMonData(mon, MON_DATA_HP_EV, &value);
                    CalculateMonStats(mon);
                    if ((GetMonData(mon, MON_DATA_MAX_HP) % 2) == 0)
                        parityReachable = TRUE;
                }
                value_restore = authoredHp;
                SetMonData(mon, MON_DATA_HP_EV, &value_restore);
                CalculateMonStats(mon);
                if (parityReachable)
                    EXPECT_EQ(GetMonData(mon, MON_DATA_MAX_HP) % 2, 0);
            }
            EXPECT_EQ(GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_MAX_HP));
            for (u32 stat = 0; stat < NUM_STATS; stat++)
                total += GetMonData(mon, MON_DATA_HP_EV + stat);
            EXPECT_LE(total, EC_STAT_POINT_BUDGET);
            // The re-landed spread still reads as the authored set.
            EXPECT_EQ(GetEmeraldChampionsCurrentBattleSetChoice(mon), choice);
        }
    }
}

TEST("Emerald Champions Belly Drum normalization preserves authored sets across successive level caps")
{
    static const enum Species species[] = {SPECIES_ZIGZAGOON, SPECIES_AZUMARILL};

    ClearBag();
    for (u32 s = 0; s < ARRAY_COUNT(species); s++)
    {
        struct Pokemon mon;
        s32 choice = -1;

        CreateMon(&mon, species[s], sEmeraldChampionsLevelCaps[0], 0, OTID_STRUCT_PLAYER_ID);
        for (u32 i = 0; i < GetEmeraldChampionsBattleSetCount(&mon); i++)
        {
            const struct EmeraldChampionsBattleSet *preset =
                GetEmeraldChampionsBattleSetPresetForFormat(&mon, i, EC_BATTLE_FORMAT_DOUBLES);
            for (u32 m = 0; m < MAX_MON_MOVES; m++)
                if (preset->moves[m] == MOVE_BELLY_DRUM)
                    choice = i;
        }
        EXPECT_GE(choice, 0);
        EXPECT_EQ(ApplyEmeraldChampionsBattleSetChoice(&mon, choice), EC_BATTLE_SET_SUCCESS);
        for (u32 c = 0; c < ARRAY_COUNT(sEmeraldChampionsLevelCaps); c++)
        {
            u32 experience = gExperienceTables[gSpeciesInfo[species[s]].growthRate][sEmeraldChampionsLevelCaps[c]];
            u32 hp;

            SetMonData(&mon, MON_DATA_EXP, &experience);
            CalculateMonStats(&mon);
            hp = GetMonData(&mon, MON_DATA_MAX_HP);
            SetMonData(&mon, MON_DATA_HP, &hp);
            TryNormalizeEmeraldChampionsBellyDrumHpParity(&mon);
            EXPECT_EQ(GetEmeraldChampionsCurrentBattleSetChoice(&mon), choice);
            EXPECT_EQ(GetMonData(&mon, MON_DATA_HP), GetMonData(&mon, MON_DATA_MAX_HP));
        }
    }
}

TEST("Emerald Champions Stat Point editor reports the next stat breakpoint")
{
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    u32 nature = NATURE_HARDY;
    u32 friendship = 0;
    u32 delta;
    u32 value;
    u32 base;
    u8 points = 2;

    ZeroPlayerPartyMons();
    CreateMon(mon, SPECIES_ZIGZAGOON, 20, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
    SetMonData(mon, MON_DATA_FRIENDSHIP, &friendship);
    SetMonData(mon, MON_DATA_HP_EV, &points);
    CalculateMonStats(mon);
    base = GetMonData(mon, MON_DATA_MAX_HP);

    EXPECT_EQ(GetEmeraldChampionsStatPointBreakpoint(mon, 0, &delta, &value), EC_STAT_BREAKPOINT_FOUND);
    EXPECT_GE(delta, 1);
    EXPECT_GT(value, base);
    // The probe restores the spread, the stat and the current HP.
    EXPECT_EQ(GetMonData(mon, MON_DATA_HP_EV), 2);
    EXPECT_EQ(GetMonData(mon, MON_DATA_MAX_HP), base);
    EXPECT_EQ(GetMonData(mon, MON_DATA_HP), base);

    // Every smaller step is dead, and the reported step lands the reported value.
    for (u32 step = 1; step < delta; step++)
    {
        points = 2 + step;
        SetMonData(mon, MON_DATA_HP_EV, &points);
        CalculateMonStats(mon);
        EXPECT_EQ(GetMonData(mon, MON_DATA_MAX_HP), base);
    }
    points = 2 + delta;
    SetMonData(mon, MON_DATA_HP_EV, &points);
    CalculateMonStats(mon);
    EXPECT_EQ(GetMonData(mon, MON_DATA_MAX_HP), value);

    points = EC_STAT_POINTS_PER_STAT;
    SetMonData(mon, MON_DATA_HP_EV, &points);
    CalculateMonStats(mon);
    EXPECT_EQ(GetEmeraldChampionsStatPointBreakpoint(mon, 0, &delta, &value), EC_STAT_BREAKPOINT_STAT_MAXED);
}

TEST("Emerald Champions field moves need the badge and a party member that could learn them")
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
    // Unlocked: the Pokémon that knows the move is preferred.
    EXPECT_EQ(FieldMove_GetUserSlot(FIELD_MOVE_SURF, TRUE), 1);
    EXPECT_EQ(PartyHasMonWithSurf(), TRUE);

    FlagSet(FLAG_BADGE01_GET);
    FlagSet(FLAG_RECEIVED_HM_CUT);
    // Unlocked, but neither Magikarp nor Marill could ever learn Cut.
    EXPECT_EQ(FieldMove_GetUserSlot(FIELD_MOVE_CUT, TRUE), PARTY_SIZE);
    // A party member that could learn it (without knowing it) does it.
    CreateMon(&party[0], SPECIES_ZIGZAGOON, 14, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(FieldMove_GetUserSlot(FIELD_MOVE_CUT, TRUE), 0);
    // Obstacle moves never show in the party menu, even unlocked; the
    // non-obstacle ones still do.
    EXPECT_EQ(FieldMove_IsVisible(FIELD_MOVE_CUT), FALSE);
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

TEST("Emerald Champions pending relics survive full stores and never replay discarded rewards")
{
    ClearBag();
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_0, 0);
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_1, 0);
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
    RetryPendingLegendaryRelics();
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0), 1);

    RemovePCItem(0, 1);
    RetryPendingLegendaryRelics();
    EXPECT(CheckPCHasItem(ITEM_RED_ORB, 1));
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0), 0);
    for (u32 slot = 0; slot < PC_ITEMS_COUNT; slot++)
    {
        if (gSaveBlock1Ptr->pcItems[slot].itemId == ITEM_RED_ORB)
        {
            RemovePCItem(slot, 1);
            break;
        }
    }
    MarkLegendarySignCaughtBySpecies(SPECIES_GROUDON);
    RetryPendingLegendaryRelics();
    EXPECT(!CheckBagHasItem(ITEM_RED_ORB, 1));
    EXPECT(!CheckPCHasItem(ITEM_RED_ORB, 1));
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0), 0);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_1), 0x100);
    ClearBag();
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
}

TEST("Emerald Champions partial mask grants retry only their saved undelivered items")
{
    ClearBag();
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_0, 0);
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_1, 0);
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
    u16 savedLow = VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0);
    u16 savedHigh = VarGet(VAR_LEGENDARY_RELIC_DELIVERY_1);
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_0, 0);
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_1, 0);
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_0, savedLow);
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_1, savedHigh);
    EXPECT(RemoveBagItem(ITEM_WELLSPRING_MASK, 1));
    RemovePCItem(0, 1);
    RetryPendingLegendaryRelics();
    EXPECT(CheckBagHasItem(ITEM_HEARTHFLAME_MASK, 1));
    EXPECT(CheckPCHasItem(ITEM_CORNERSTONE_MASK, 1));
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0), 0);
    EXPECT_EQ(VarGet(VAR_LEGENDARY_RELIC_DELIVERY_1), 0x1000);
    MarkLegendarySignCaughtBySpecies(SPECIES_OGERPON_TEAL);
    RetryPendingLegendaryRelics();
    EXPECT(!CheckBagHasItem(ITEM_WELLSPRING_MASK, 1));
    EXPECT(!CheckPCHasItem(ITEM_WELLSPRING_MASK, 1));
    ClearBag();
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
}
