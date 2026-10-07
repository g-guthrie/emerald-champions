#include "global.h"
#include "story.h"
#include "battle.h"
#include "battle_setup.h"
#include "data.h"
#include "difficulty.h"
#include "emerald_champions_opening.h"
#include "event_data.h"
#include "pokemon.h"
#include "starter_choose.h"
#include "test/test.h"
#include "constants/emerald_champions.h"
#include "constants/opponents.h"
#include "constants/vars.h"

TEST("Campaign factory: regional rivals keep their authored evolution stage and other teammates")
{
    static const struct { u16 generation; enum Species species[3]; } regions[] = {
        {0, {SPECIES_MUDKIP, SPECIES_MARSHTOMP, SPECIES_SWAMPERT}},
        {1, {SPECIES_SQUIRTLE, SPECIES_WARTORTLE, SPECIES_BLASTOISE}},
        {2, {SPECIES_TOTODILE, SPECIES_CROCONAW, SPECIES_FERALIGATR}},
        {3, {SPECIES_MUDKIP, SPECIES_MARSHTOMP, SPECIES_SWAMPERT}},
        {4, {SPECIES_PIPLUP, SPECIES_PRINPLUP, SPECIES_EMPOLEON}},
        {5, {SPECIES_OSHAWOTT, SPECIES_DEWOTT, SPECIES_SAMUROTT}},
        {6, {SPECIES_FROAKIE, SPECIES_FROGADIER, SPECIES_GRENINJA}},
        {7, {SPECIES_POPPLIO, SPECIES_BRIONNE, SPECIES_PRIMARINA}},
        {8, {SPECIES_SOBBLE, SPECIES_DRIZZILE, SPECIES_INTELEON}},
        {9, {SPECIES_QUAXLY, SPECIES_QUAXWELL, SPECIES_QUAQUAVAL}},
        {10, {SPECIES_MUDKIP, SPECIES_MARSHTOMP, SPECIES_SWAMPERT}},
    };
    static const struct { u16 trainer; u8 slot; } fights[] = {
        {TRAINER_MAY_ROUTE_103_TORCHIC, 0},
        {TRAINER_MAY_ROUTE_110_TORCHIC, 4},
        {TRAINER_MAY_LILYCOVE_TORCHIC, 5},
    };
    static EWRAM_DATA struct Pokemon reference[PARTY_SIZE];
    u32 savedFlags = gBattleTypeFlags;
    TrainerBattleParameter savedParameters = gTrainerBattleParameter;
    enum DifficultyLevel savedDifficulty = GetCurrentDifficultyLevel();
    u16 savedGeneration = VarGet(VAR_STARTER_GEN);
    u16 savedFirst = VarGet(VAR_STARTER_MON);
    u16 savedSecond = VarGet(VAR_EC_SECOND_STARTER);
    u16 savedOpening = GetStoryStep();
    SetCurrentDifficultyLevel(DIFFICULTY_HARD);
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
    VarSet(VAR_STARTER_MON, 0);
    VarSet(VAR_EC_SECOND_STARTER, 2); // Grass/Fire pair leaves the regional Water starter.
    StoryStageAt(STORY_STEP_RESCUED_BIRCH);
    EXPECT_EQ(GetEmeraldChampionsRivalStarterIndex(), 2);
    for (u32 stage = 0; stage < ARRAY_COUNT(fights); stage++)
    {
        const struct Trainer *trainer = GetTrainerStructFromId(fights[stage].trainer);
        CreateNPCTrainerPartyFromTrainer(reference, trainer);
        EXPECT_GT((u32)trainer->partySize, fights[stage].slot);
        EXPECT_EQ(GetMonData(&reference[fights[stage].slot], MON_DATA_SPECIES), regions[0].species[stage]);
        for (u32 region = 0; region < ARRAY_COUNT(regions); region++)
        {
            VarSet(VAR_STARTER_GEN, regions[region].generation);
            TRAINER_BATTLE_PARAM.opponentA = fights[stage].trainer;
            TRAINER_BATTLE_PARAM.opponentB = 0xFFFF;
            EmeraldChampions_RebuildTrainerBattleParties();
            for (u32 slot = 0; slot < trainer->partySize; slot++)
            {
                struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][slot];
                struct Pokemon *original = &reference[slot];
                enum Species expected = slot == fights[stage].slot ? regions[region].species[stage]
                    : GetMonData(original, MON_DATA_SPECIES);
                EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), expected);
                EXPECT_EQ(GetMonData(mon, MON_DATA_LEVEL), GetMonData(original, MON_DATA_LEVEL));
                EXPECT_EQ(GetMonData(mon, MON_DATA_FRIENDSHIP), GetMonData(original, MON_DATA_FRIENDSHIP));
                if (slot == fights[stage].slot && stage == 0 && regions[region].generation == 4)
                {
                    // The regional special set keeps the seed personality;
                    // its effective nature is the one used for stats/auditing.
                    EXPECT_EQ(GetNature(mon), NATURE_ADAMANT);
                    EXPECT_EQ(GetMonData(mon, MON_DATA_HIDDEN_NATURE), NATURE_BOLD);
                }
                if (slot != fights[stage].slot || regions[region].generation == 0
                 || regions[region].generation == 3 || regions[region].generation == 10)
                {
                    EXPECT_EQ(GetMonData(mon, MON_DATA_HELD_ITEM), GetMonData(original, MON_DATA_HELD_ITEM));
                    EXPECT_EQ(GetMonAbility(mon), GetMonAbility(original));
                    EXPECT_EQ(GetNature(mon), GetNature(original));
                    for (u32 move = 0; move < MAX_MON_MOVES; move++)
                        EXPECT_EQ(GetMonData(mon, MON_DATA_MOVE1 + move), GetMonData(original, MON_DATA_MOVE1 + move));
                }
            }
        }
    }
    ZeroEnemyPartyMons();
    gBattleTypeFlags = savedFlags;
    gTrainerBattleParameter = savedParameters;
    SetCurrentDifficultyLevel(savedDifficulty);
    VarSet(VAR_STARTER_GEN, savedGeneration);
    VarSet(VAR_STARTER_MON, savedFirst);
    VarSet(VAR_EC_SECOND_STARTER, savedSecond);
    StoryStageAt(savedOpening);
}

TEST("Campaign factory: legacy unpaired saves use the native rival-index fallback")
{
    u32 savedFlags = gBattleTypeFlags;
    TrainerBattleParameter savedParameters = gTrainerBattleParameter;
    u16 savedGeneration = VarGet(VAR_STARTER_GEN);
    u16 savedFirst = VarGet(VAR_STARTER_MON);
    u16 savedSecond = VarGet(VAR_EC_SECOND_STARTER);
    u16 savedOpening = GetStoryStep();
    static const u16 aliases[] = {TRAINER_BRENDAN_ROUTE_103_MUDKIP, TRAINER_MAY_ROUTE_103_TORCHIC};
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
    VarSet(VAR_STARTER_GEN, 4);
    VarSet(VAR_STARTER_MON, 7); // Native first%3=Fire, so legacy rival takes Water.
    VarSet(VAR_EC_SECOND_STARTER, 0);
    StoryStageAt(STORY_STEP_NEW_GAME); // before the pair: the legacy single starter
    EXPECT_EQ(GetEmeraldChampionsRivalStarterIndex(), 2);
    for (u32 i = 0; i < ARRAY_COUNT(aliases); i++)
    {
        TRAINER_BATTLE_PARAM.opponentA = aliases[i];
        TRAINER_BATTLE_PARAM.opponentB = 0xFFFF;
        EmeraldChampions_RebuildTrainerBattleParties();
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_SPECIES), SPECIES_PIPLUP);
    }
    ZeroEnemyPartyMons();
    gBattleTypeFlags = savedFlags;
    gTrainerBattleParameter = savedParameters;
    VarSet(VAR_STARTER_GEN, savedGeneration);
    VarSet(VAR_STARTER_MON, savedFirst);
    VarSet(VAR_EC_SECOND_STARTER, savedSecond);
    StoryStageAt(savedOpening);
}
