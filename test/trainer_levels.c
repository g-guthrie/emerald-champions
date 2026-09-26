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
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_LEVEL), 149);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_LEVEL), 249);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_LEVEL), 79);
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_HP), GetMonData(&party[0], MON_DATA_MAX_HP));
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_MOVE1), MOVE_SEISMIC_TOSS);
        u32 exp = GetMonData(&party[0], MON_DATA_EXP);
        u32 attack = GetMonData(&party[0], MON_DATA_ATK);
        SetMonData(&party[0], MON_DATA_SPECIES, &mega);
        CalculateMonStats(&party[0]);
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_LEVEL), 149);
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_EXP), exp);
        EXPECT_GT(GetMonData(&party[0], MON_DATA_ATK), attack);
    }
    EXPECT_EQ(GetCampaignTrainerLevel(254), 255);
    EXPECT_EQ(GetCampaignTrainerLevel(-254), 1);
    SetCurrentDifficultyLevel(DIFFICULTY_HARD);
    EXPECT_EQ(GetCampaignTrainerLevel(50), 150);
    SetCurrentDifficultyLevel(DIFFICULTY_EASY);
    EXPECT_EQ(GetCampaignTrainerLevel(50), 146);
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
