#include "global.h"
#include "battle_setup.h"
#include "battle_gimmick.h"
#include "champions_circuit.h"
#include "difficulty.h"
#include "emerald_champions_battle_sets.h"
#include "event_data.h"
#include "gym_leader_rematch.h"
#include "item.h"
#include "pokemon.h"
#include "random.h"
#include "test/test.h"
#include "constants/rematches.h"
#include "constants/cries.h"
#include "constants/trainers.h"
#include "constants/vars.h"

TEST("Emerald Champions disables Match Call and Gym rematches")
{
#if FREE_MATCH_CALL == FALSE
    gSaveBlock1Ptr->trainerRematches[REMATCH_ROSE] = 1;
#endif

    EXPECT_EQ(ShouldTryRematchBattleForTrainerId(TRAINER_ROSE_1), FALSE);
    EXPECT_EQ(GetCurrentGymLeaderRematchLevel(), 0);
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
        EXPECT_NE(gSpeciesInfo[forms[i]].cryId, CRY_NONE);
    }
    EXPECT_NE(gSpeciesInfo[SPECIES_TATSUGIRI_CURLY_MEGA].frontPic, gSpeciesInfo[SPECIES_TATSUGIRI_CURLY].frontPic);
    EXPECT_NE(gSpeciesInfo[SPECIES_GLIMMORA_MEGA].frontPic, gSpeciesInfo[SPECIES_GLIMMORA].frontPic);
    EXPECT_EQ(gItemsInfo[ITEM_TATSUGIRINITE].sortType, ITEM_TYPE_MEGA_STONE);
    EXPECT_EQ(gItemsInfo[ITEM_GLIMMORANITE].sortType, ITEM_TYPE_MEGA_STONE);
}

TEST("Emerald Champions live difficulty changes only trainer levels")
{
    struct Pokemon party[PARTY_SIZE] = {0};
    struct Pokemon lowParty[PARTY_SIZE] = {0};

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

    CreateMon(&lowParty[0], SPECIES_EEVEE, 3, 0, OTID_STRUCT_PLAYER_ID);
    ApplyTrainerLevelDifficulty(lowParty);
    EXPECT_EQ(GetMonData(&lowParty[0], MON_DATA_LEVEL), 1);

    SetCurrentDifficultyLevel(DIFFICULTY_HARD);
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
    EXPECT(IsEmeraldChampionsProtectedProgressionItem(ITEM_VENUSAURITE));
    EXPECT(IsEmeraldChampionsProtectedProgressionItem(ITEM_RED_ORB));
    EXPECT(IsEmeraldChampionsProtectedProgressionItem(ITEM_WELLSPRING_MASK));
    EXPECT(IsEmeraldChampionsProtectedProgressionItem(ITEM_DOUSE_DRIVE));
    EXPECT(IsEmeraldChampionsProtectedProgressionItem(ITEM_FLAME_PLATE));
    EXPECT(!IsEmeraldChampionsProtectedProgressionItem(ITEM_LIFE_ORB));
}

TEST("Emerald Champions battle-ready wild presets exclude special encounters")
{
    EXPECT(IsEmeraldChampionsOrdinaryWildSpecies(SPECIES_BULBASAUR));
    EXPECT(!IsEmeraldChampionsOrdinaryWildSpecies(SPECIES_MEW));
    EXPECT(!IsEmeraldChampionsOrdinaryWildSpecies(SPECIES_VENUSAUR_MEGA));
}

TEST("Emerald Champions imported battle sets remain legal against current data")
{
    struct Pokemon mon;

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
            EXPECT_EQ(GetMonData(&mon, MON_DATA_HIDDEN_NATURE), preset->nature);
            for (u32 stat = 0; stat < NUM_STATS; stat++)
                statPointTotal += preset->statPoints[stat];
            EXPECT_EQ(statPointTotal, 66);
            if (preset->requiredItem != ITEM_NONE)
                EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), preset->requiredItem);
        }
    }
}

TEST("Emerald Champions reports remaining ordinary preset coverage")
{
    u32 missing = 0;

    for (enum Species species = SPECIES_BULBASAUR; species < NUM_SPECIES; species++)
    {
        if (IsEmeraldChampionsOrdinaryWildSpecies(species)
         && GetEmeraldChampionsRawBattleSetCount(species) == 0)
            missing++;
    }
    Test_MgbaPrintf("missing ordinary preset count=%d", missing);
}

TEST("Champions Circuit generates live Showdown doubles teams")
{
    static bool8 seenSpecies[NUM_SPECIES];
    u32 diversity = 0;

    memset(seenSpecies, 0, sizeof(seenSpecies));
    for (u32 seed = 1; seed <= 64; seed++)
    {
        SeedRng(seed);
        VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, seed % 48);
        ChampionsCircuitGenerateOpponent();
        EXPECT_EQ(gSpecialVar_Result, PARTY_SIZE);
        EXPECT_EQ(gPartiesCount[B_TRAINER_OPPONENT_A], PARTY_SIZE);

        for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        {
            enum Species species = GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES);
            EXPECT_NE(species, SPECIES_NONE);
            EXPECT_NE(GetMonAbility(&gParties[B_TRAINER_OPPONENT_A][slot]), ABILITY_NONE);
            EXPECT_NE(GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MOVE1), MOVE_NONE);
            for (u32 move = 0; move < MAX_MON_MOVES; move++)
            {
                enum Move selectedMove = GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MOVE1 + move);
                if (selectedMove == MOVE_NONE)
                    continue;
                for (u32 otherMove = 0; otherMove < move; otherMove++)
                    EXPECT_NE(selectedMove, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MOVE1 + otherMove));
            }
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
    EXPECT_GE(diversity, 100);
}
