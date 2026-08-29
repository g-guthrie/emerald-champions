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
#include "gym_leader_rematch.h"
#include "item.h"
#include "legendary_signs.h"
#include "load_save.h"
#include "overworld.h"
#include "pokemon.h"
#include "random.h"
#include "showdown_champions_circuit.h"
#include "test/test.h"
#include "constants/pokedex.h"
#include "constants/rematches.h"
#include "constants/cries.h"
#include "constants/flags.h"
#include "constants/trainers.h"
#include "constants/vars.h"

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

TEST("Emerald Champions disables the Bag only in competitive trainer battles")
{
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    EXPECT(!IsAllowedToUseBag());
    gBattleTypeFlags = 0;
    EXPECT(IsAllowedToUseBag());
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_PYRAMID;
    EXPECT(IsAllowedToUseBag());
}

TEST("Emerald Champions catch transfers preserve both held-item loadouts")
{
    static struct BattleStruct sCaptureTransferBattleStruct;
    struct BattleStruct *savedBattleStruct = gBattleStruct;
    enum Item item;
    u32 outgoingItem;
    u32 caughtItem;

    memset(&sCaptureTransferBattleStruct, 0, sizeof(sCaptureTransferBattleStruct));
    gBattleStruct = &sCaptureTransferBattleStruct;
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_BULBASAUR, 14, 0, OTID_STRUCT_PLAYER_ID);

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
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE1), MOVE_GIGA_DRAIN);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE2), MOVE_SLUDGE_BOMB);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE3), MOVE_SLEEP_POWDER);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MOVE4), MOVE_PROTECT);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HIDDEN_NATURE), NATURE_MODEST);
    EXPECT_EQ(GetMonAbility(&mon), ABILITY_CHLOROPHYLL);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), ITEM_EVIOLITE);

    for (u32 i = 0; i < NUM_STATS; i++)
        statPointTotal += GetMonData(&mon, MON_DATA_HP_EV + i);
    EXPECT_EQ(statPointTotal, 66);
}

TEST("Emerald Champions protects progression items from preparation services")
{
    EXPECT(GetItemImportance(ITEM_LINKING_CORD));
    EXPECT(IsEmeraldChampionsProtectedProgressionItem(ITEM_VENUSAURITE));
    EXPECT(IsEmeraldChampionsProtectedProgressionItem(ITEM_RED_ORB));
    EXPECT(IsEmeraldChampionsProtectedProgressionItem(ITEM_WELLSPRING_MASK));
    EXPECT(IsEmeraldChampionsProtectedProgressionItem(ITEM_DOUSE_DRIVE));
    EXPECT(IsEmeraldChampionsProtectedProgressionItem(ITEM_FLAME_PLATE));
    EXPECT(!IsEmeraldChampionsProtectedProgressionItem(ITEM_LIFE_ORB));
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

TEST("Emerald Champions battle-ready wild presets exclude special encounters")
{
    EXPECT(IsEmeraldChampionsOrdinaryWildSpecies(SPECIES_BULBASAUR));
    EXPECT(IsEmeraldChampionsOrdinaryWildSpecies(SPECIES_NIHILEGO));
    EXPECT(IsEmeraldChampionsOrdinaryWildSpecies(SPECIES_GREAT_TUSK));
    EXPECT(!IsEmeraldChampionsOrdinaryWildSpecies(SPECIES_MEW));
    EXPECT(!IsEmeraldChampionsOrdinaryWildSpecies(SPECIES_VENUSAUR_MEGA));
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
             && preset->requiredItem == ITEM_NONE)
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

TEST("Emerald Champions covers every ordinary species and form")
{
    u32 missing = 0;

    for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        if (IsEmeraldChampionsOrdinaryWildSpecies(species)
         && GetEmeraldChampionsRawBattleSetCount(species) == 0)
            missing++;
    }
    EXPECT_EQ(missing, 0);
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
