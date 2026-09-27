#include "global.h"
#include "test/test.h"
#include "battle.h"
#include "battle_setup.h"
#include "caps.h"
#include "data.h"
#include "difficulty.h"
#include "event_data.h"
#include "pokemon.h"
#include "constants/flags.h"
#include "constants/opponents.h"
#include "trainer_util.h"

TEST("EC trainer levels: wide offsets survive creation, Mega stats and both opponent owners")
{
    static const struct TrainerMon mons[] = {
        {.species = SPECIES_GARCHOMP, .lvl = 1, .useLevelOffset = TRUE, .levelOffset = 50,
         .gender = TRAINER_MON_RANDOM_GENDER, .nature = NATURE_JOLLY,
         .heldItem = ITEM_GARCHOMPITE, .moves = {MOVE_EARTHQUAKE}},
        {.species = SPECIES_MAGBY, .lvl = 1, .useLevelOffset = TRUE, .levelOffset = 150,
         .gender = TRAINER_MON_RANDOM_GENDER, .moves = {MOVE_SEISMIC_TOSS}},
        {.species = SPECIES_EEVEE, .lvl = 1, .useLevelOffset = TRUE, .levelOffset = -20,
         .gender = TRAINER_MON_RANDOM_GENDER},
    };
    static const struct Trainer trainer = {.party = mons, .partySize = 3};
    u32 oldFlags = gBattleTypeFlags;
    bool32 wasChampion = FlagGet(FLAG_IS_CHAMPION);
    enum DifficultyLevel oldDifficulty = GetCurrentDifficultyLevel();
    enum Species mega = SPECIES_GARCHOMP_MEGA;

    FlagSet(FLAG_IS_CHAMPION);
    SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
    static const u8 owners[] = {B_TRAINER_OPPONENT_A, B_TRAINER_OPPONENT_B};
    for (u32 i = 0; i < ARRAY_COUNT(owners); i++)
    {
        struct Pokemon *party = gParties[owners[i]];
        CreateNPCTrainerPartyFromTrainer(party, &trainer);
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_LEVEL), 134);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_LEVEL), 234);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_LEVEL), 64);
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_HP), GetMonData(&party[0], MON_DATA_MAX_HP));
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_MOVE1), MOVE_SEISMIC_TOSS);
        u32 exp = GetMonData(&party[0], MON_DATA_EXP);
        u32 attack = GetMonData(&party[0], MON_DATA_ATK);
        SetMonData(&party[0], MON_DATA_SPECIES, &mega);
        CalculateMonStats(&party[0]);
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_LEVEL), 134);
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_EXP), exp);
        EXPECT_GT(GetMonData(&party[0], MON_DATA_ATK), attack);
    }
    EXPECT_EQ(GetCampaignTrainerLevel(254), 255);
    EXPECT_EQ(GetCampaignTrainerLevel(-254), 1);
    SetCurrentDifficultyLevel(DIFFICULTY_HARD);
    EXPECT_EQ(GetCampaignTrainerLevel(50), 135);
    SetCurrentDifficultyLevel(DIFFICULTY_EASY);
    EXPECT_EQ(GetCampaignTrainerLevel(50), 129);
    // A transient opponent level must not leak into saved player progression.
    gParties[B_TRAINER_PLAYER][0] = gParties[B_TRAINER_OPPONENT_A][0];
    CalculateMonStats(&gParties[B_TRAINER_PLAYER][0]);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 100);
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    if (!wasChampion)
        FlagClear(FLAG_IS_CHAMPION);
    SetCurrentDifficultyLevel(oldDifficulty);
    gBattleTypeFlags = oldFlags;
}

TEST("EC trainer levels: Easy lowers every Casual and Regular party member only")
{
    static const struct { u16 id; bool8 reduced; } cases[] = {
        {TRAINER_TIANA, TRUE}, // Casual
        {TRAINER_CALVIN_1, TRUE}, // Early Regular
        {TRAINER_NAOMI, TRUE}, // Late Regular
        {TRAINER_CAROLINE, FALSE}, // Ace
        {TRAINER_JOSH, FALSE}, // Gym trainer
        {TRAINER_GRUNT_PETALBURG_WOODS, FALSE},
        {TRAINER_BRENDAN_ROUTE_103_MUDKIP, FALSE}, // Rival
        {TRAINER_ROXANNE_1, FALSE},
        {TRAINER_STEVEN, FALSE},
    };
    u32 oldFlags = gBattleTypeFlags;
    enum DifficultyLevel oldDifficulty = GetCurrentDifficultyLevel();
    bool32 wasChampion = FlagGet(FLAG_IS_CHAMPION);
    FlagSet(FLAG_IS_CHAMPION); // Avoid the level-one floor hiding reductions.
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
    for (u32 difficulty = DIFFICULTY_EASY; difficulty <= DIFFICULTY_HARD; difficulty++)
    {
        SetCurrentDifficultyLevel(difficulty);
        for (u32 i = 0; i < ARRAY_COUNT(cases); i++)
        {
            const struct Trainer *trainer = GetTrainerStructFromId(cases[i].id);
            EXPECT_EQ((u32)trainer->easyLevelReduction, cases[i].reduced);
            static const u8 owners[] = {B_TRAINER_OPPONENT_A, B_TRAINER_OPPONENT_B};
            for (u32 owner = 0; owner < ARRAY_COUNT(owners); owner++)
            {
                struct Pokemon *party = gParties[owners[owner]];
                CreateNPCTrainerPartyFromTrainer(party, trainer);
                for (u32 slot = 0; slot < trainer->partySize; slot++)
                {
                    const struct TrainerMon *mon = &trainer->party[slot];
                    s32 expected = mon->useLevelOffset ? GetCampaignTrainerLevel(mon->levelOffset) : mon->lvl;
                    if (difficulty == DIFFICULTY_EASY && cases[i].reduced)
                        expected = max(1, expected - 1);
                    EXPECT_EQ(GetMonData(&party[slot], MON_DATA_LEVEL), expected);
                    EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), mon->species);
                    EXPECT_EQ(GetMonData(&party[slot], MON_DATA_HELD_ITEM), mon->heldItem);
                    for (u32 move = 0; move < MAX_MON_MOVES; move++)
                        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_MOVE1 + move), mon->moves[move]);
                }
            }
        }
    }
    ZeroEnemyPartyMons();
    if (!wasChampion)
        FlagClear(FLAG_IS_CHAMPION);
    SetCurrentDifficultyLevel(oldDifficulty);
    gBattleTypeFlags = oldFlags;
}

TEST("EC trainer levels: Easy reduction floors at one and does not affect partners")
{
    static const struct TrainerMon mons[] = {
        {.species = SPECIES_EEVEE, .lvl = 1, .gender = TRAINER_MON_RANDOM_GENDER},
        {.species = SPECIES_EEVEE, .lvl = 5, .gender = TRAINER_MON_RANDOM_GENDER},
    };
    static const struct Trainer trainer = {.party = mons, .partySize = 2, .easyLevelReduction = TRUE};
    enum DifficultyLevel oldDifficulty = GetCurrentDifficultyLevel();
    struct TrainerGenerator generator = {0};
    struct Trainer withoutReduction = trainer;
    withoutReduction.easyLevelReduction = FALSE;
    rng_value_t originalSeed = GeneratePartySeed(&withoutReduction);
    rng_value_t reducedSeed = GeneratePartySeed(&trainer);
    EXPECT_EQ(memcmp(&originalSeed, &reducedSeed, sizeof(originalSeed)), 0);
    SetCurrentDifficultyLevel(DIFFICULTY_EASY);
    MakeTrainerGenerator(&generator, &trainer);
    GenerateMonFromTrainerMon(&gParties[B_TRAINER_OPPONENT_A][0], &mons[0], &generator);
    GenerateMonFromTrainerMon(&gParties[B_TRAINER_OPPONENT_A][1], &mons[1], &generator);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL), 1);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_LEVEL), 4);
    MakePartnerGenerator(&generator, &trainer);
    GenerateMonFromTrainerMon(&gParties[B_TRAINER_OPPONENT_A][1], &mons[1], &generator);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_LEVEL), 5);
    ZeroEnemyPartyMons();
    SetCurrentDifficultyLevel(oldDifficulty);
}
