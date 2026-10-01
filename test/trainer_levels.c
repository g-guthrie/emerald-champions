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

TEST("EC trainer levels: modes scale the lead over the shared cap and Hard remains capped at 100")
{
    enum DifficultyLevel oldDifficulty = GetCurrentDifficultyLevel();
    bool32 wasChampion = FlagGet(FLAG_IS_CHAMPION);
    FlagSet(FLAG_IS_CHAMPION);
    // Offset 0 is a two-level lead: Hard keeps it, Medium and Easy keep one,
    // and Easy then drops 15% of the cap (13 levels at 85).
    SetCurrentDifficultyLevel(DIFFICULTY_EASY);
    EXPECT_EQ(GetCampaignTrainerLevel(0), 73);
    SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
    EXPECT_EQ(GetCampaignTrainerLevel(0), 86);
    SetCurrentDifficultyLevel(DIFFICULTY_HARD);
    EXPECT_EQ(GetCampaignTrainerLevel(0), 87);
    EXPECT_EQ(GetCampaignTrainerLevel(20), 100);
    EXPECT_EQ(GetCurrentLevelCap(), 85);
    if (!wasChampion)
        FlagClear(FLAG_IS_CHAMPION);
    SetCurrentDifficultyLevel(oldDifficulty);
}

TEST("EC trainer levels: creation and cached level writes cannot exceed 100")
{
    struct Pokemon mon;
    u8 invalidLevel = 255;
    CreateMon(&mon, SPECIES_GARCHOMP, invalidLevel, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), MAX_LEVEL);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_EXP),
        gExperienceTables[gSpeciesInfo[SPECIES_GARCHOMP].growthRate][MAX_LEVEL]);
    SetMonData(&mon, MON_DATA_LEVEL, &invalidLevel);
    EXPECT_EQ(mon.level, MAX_LEVEL);
    // A legacy or corrupt cache is normalized by the ordinary EXP/stat path.
    mon.level = invalidLevel;
    EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), MAX_LEVEL);
    CalculateMonStats(&mon);
    EXPECT_EQ(mon.level, MAX_LEVEL);
}

TEST("EC trainer levels: offsets cap at 100 through creation and Mega stats for both opponents")
{
    static const struct TrainerMon mons[] = {
        {.species = SPECIES_GARCHOMP, .lvl = 1, .useLevelOffset = TRUE, .levelOffset = 50,
         .gender = TRAINER_MON_RANDOM_GENDER, .nature = NATURE_JOLLY,
         .heldItem = ITEM_GARCHOMPITE, .moves = {MOVE_EARTHQUAKE}},
        {.species = SPECIES_MAGBY, .lvl = 1, .useLevelOffset = TRUE, .levelOffset = 150,
         .gender = TRAINER_MON_RANDOM_GENDER, .moves = {MOVE_SEISMIC_TOSS}},
        {.species = SPECIES_EEVEE, .lvl = 1, .useLevelOffset = TRUE, .levelOffset = -20,
         .gender = TRAINER_MON_RANDOM_GENDER},
        {.species = SPECIES_EEVEE, .lvl = 255, .gender = TRAINER_MON_RANDOM_GENDER},
    };
    static const struct Trainer trainer = {.party = mons, .partySize = 4};
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
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_LEVEL), 100);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_LEVEL), 100);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_LEVEL), 67);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_LEVEL), 100);
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_HP), GetMonData(&party[0], MON_DATA_MAX_HP));
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_MOVE1), MOVE_SEISMIC_TOSS);
        u32 exp = GetMonData(&party[0], MON_DATA_EXP);
        u32 attack = GetMonData(&party[0], MON_DATA_ATK);
        SetMonData(&party[0], MON_DATA_SPECIES, &mega);
        CalculateMonStats(&party[0]);
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_LEVEL), 100);
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_EXP), exp);
        EXPECT_GT(GetMonData(&party[0], MON_DATA_ATK), attack);
    }
    EXPECT_EQ(GetCampaignTrainerLevel(254), 100);
    EXPECT_EQ(GetCampaignTrainerLevel(-254), 1);
    SetCurrentDifficultyLevel(DIFFICULTY_HARD);
    EXPECT_EQ(GetCampaignTrainerLevel(50), 100);
    SetCurrentDifficultyLevel(DIFFICULTY_EASY);
    EXPECT_EQ(GetCampaignTrainerLevel(50), 85);
    ZeroEnemyPartyMons();
    if (!wasChampion)
        FlagClear(FLAG_IS_CHAMPION);
    SetCurrentDifficultyLevel(oldDifficulty);
    gBattleTypeFlags = oldFlags;
}

TEST("EC trainer levels: every difficulty lowers Casual and Regular party members only")
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
                    if (cases[i].reduced)
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

TEST("EC trainer levels: routine reduction floors at one and does not affect partners")
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
    for (u32 difficulty = DIFFICULTY_EASY; difficulty <= DIFFICULTY_HARD; difficulty++)
    {
        SetCurrentDifficultyLevel(difficulty);
        MakeTrainerGenerator(&generator, &trainer);
        GenerateMonFromTrainerMon(&gParties[B_TRAINER_OPPONENT_A][0], &mons[0], &generator);
        GenerateMonFromTrainerMon(&gParties[B_TRAINER_OPPONENT_A][1], &mons[1], &generator);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL), 1);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_LEVEL), 4);
        MakePartnerGenerator(&generator, &trainer);
        GenerateMonFromTrainerMon(&gParties[B_TRAINER_OPPONENT_A][1], &mons[1], &generator);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_LEVEL), 5);
    }
    ZeroEnemyPartyMons();
    SetCurrentDifficultyLevel(oldDifficulty);
}

TEST("EC caps: every difficulty shares the player cap while opponents scale their lead")
{
    bool32 wasChampion = FlagGet(FLAG_IS_CHAMPION);
    enum DifficultyLevel oldDifficulty = GetCurrentDifficultyLevel();
    const enum DifficultyLevel modes[] = {DIFFICULTY_EASY, DIFFICULTY_NORMAL, DIFFICULTY_HARD};
    // Offset 3 is a five-level lead: Easy keeps 25% and drops 15% of the
    // cap, Medium keeps 60%, Hard all.
    const u32 opponents[] = {73, 88, 90};
    FlagSet(FLAG_IS_CHAMPION);
    for (u32 i = 0; i < ARRAY_COUNT(modes); i++)
    {
        SetCurrentDifficultyLevel(modes[i]);
        EXPECT_EQ(GetCurrentLevelCap(), 85);
        EXPECT_EQ(GetPlayerLevelCapForSpecies(SPECIES_MEWTWO), 85);
        EXPECT_EQ(GetCampaignTrainerLevel(3), opponents[i]);
    }
    if (!wasChampion)
        FlagClear(FLAG_IS_CHAMPION);
    SetCurrentDifficultyLevel(oldDifficulty);
}
