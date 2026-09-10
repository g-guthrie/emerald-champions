// Temporary source-backed routing/party diagnostic, not tactical sign-off.
#line 42000 "work/rival_dispatch_probe.inc"
#include "emerald_champions_opening.h"
#include "starter_choose.h"
#include "string_util.h"
void E1ReviewCreateFieldOpponentParties(void);

DOUBLE_BATTLE_TEST("EC live review E0001 dispatch: ordered choices regions aliases and difficulty")
{
    GIVEN {
        PLAYER(SPECIES_MAGIKARP) { Moves(MOVE_SPLASH); }
        PLAYER(SPECIES_MAGIKARP) { Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_MAGIKARP) { Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_MAGIKARP) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SPLASH); MOVE(playerRight, MOVE_SPLASH);
               MOVE(opponentLeft, MOVE_SPLASH); MOVE(opponentRight, MOVE_SPLASH); }
    } THEN {
        // Indices are the actual Route103 script's selector order, not the
        // misleading legacy trainer suffix's apparent starter species.
        static const u16 routes[2][3] = {
            {TRAINER_MAY_ROUTE_103_TREECKO, TRAINER_MAY_ROUTE_103_TORCHIC, TRAINER_MAY_ROUTE_103_MUDKIP},
            {TRAINER_BRENDAN_ROUTE_103_TREECKO, TRAINER_BRENDAN_ROUTE_103_TORCHIC, TRAINER_BRENDAN_ROUTE_103_MUDKIP},
        };
        static const enum Species reserves[] = {SPECIES_PIKACHU, SPECIES_SHROOMISH, SPECIES_TAILLOW};
        struct Pokemon savedParty[PARTY_SIZE], hoenn[PARTY_SIZE];
        memcpy(savedParty, gParties[B_TRAINER_OPPONENT_A], sizeof(savedParty));
        u16 savedFirst = VarGet(VAR_STARTER_MON), savedSecond = VarGet(VAR_EC_SECOND_STARTER);
        u16 savedGen = VarGet(VAR_STARTER_GEN), savedResult = gSpecialVar_Result;
        u16 savedTrainer = TRAINER_BATTLE_PARAM.opponentA;
        u32 savedFlags = gBattleTypeFlags;
        enum DifficultyLevel savedDifficulty = GetCurrentDifficultyLevel();
        bool8 badges[8];
        for (u32 i = 0; i < 8; i++) { badges[i] = FlagGet(FLAG_BADGE01_GET + i); FlagClear(FLAG_BADGE01_GET + i); }
        gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
        u32 cases = 0;
        for (u32 gen = 1; gen <= 9; gen++)
        for (u32 first = 0; first < 3; first++)
        for (u32 second = 0; second < 3; second++)
        {
            if (first == second) continue;
            VarSet(VAR_STARTER_GEN, gen);
            VarSet(VAR_STARTER_MON, first);
            VarSet(VAR_EC_SECOND_STARTER, second + 1);
            u32 remaining = 0;
            while (remaining == first || remaining == second) remaining++;
            EXPECT_EQ(GetEmeraldChampionsRivalStarterIndex(), remaining);
            BufferEmeraldChampionsRivalBranch();
            EXPECT(gSpecialVar_Result < 3);
            enum Species expected = GetStarterPokemonForGeneration(remaining, gen);
            for (u32 gender = 0; gender < 2; gender++)
            for (u32 difficulty = DIFFICULTY_EASY; difficulty <= DIFFICULTY_HARD; difficulty++)
            {
                SetCurrentDifficultyLevel(difficulty);
                TRAINER_BATTLE_PARAM.opponentA = routes[gender][gSpecialVar_Result];
                E1ReviewCreateFieldOpponentParties();
                struct Pokemon *party = gParties[B_TRAINER_OPPONENT_A];
                EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), expected);
                for (u32 slot = 0; slot < 4; slot++)
                {
                    if (slot) EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), reserves[slot - 1]);
                    EXPECT_EQ(GetMonData(&party[slot], MON_DATA_LEVEL), 10 + 2 * difficulty);
                    for (u32 stat = MON_DATA_HP_IV; stat <= MON_DATA_SPDEF_IV; stat++)
                        EXPECT_EQ(GetMonData(&party[slot], stat), 31);
                }
                EXPECT_EQ(GetMonData(&party[4], MON_DATA_SPECIES), SPECIES_NONE);
                EXPECT_EQ(GetMonData(&party[5], MON_DATA_SPECIES), SPECIES_NONE);
                if (gen == 3)
                {
                    // The private regional resolver must preserve bespoke
                    // Hoenn moves/items/EVs rather than reapply a shared preset.
                    CreateNPCTrainerPartyFromTrainer(hoenn, GetTrainerStructFromId(TRAINER_BATTLE_PARAM.opponentA));
                    ApplyTrainerLevelDifficulty(hoenn);
                    EXPECT_EQ(GetMonData(&party[0], MON_DATA_HELD_ITEM), GetMonData(&hoenn[0], MON_DATA_HELD_ITEM));
                    EXPECT_EQ(GetMonAbility(&party[0]), GetMonAbility(&hoenn[0]));
                    for (u32 move = MON_DATA_MOVE1; move <= MON_DATA_MOVE4; move++)
                        EXPECT_EQ(GetMonData(&party[0], move), GetMonData(&hoenn[0], move));
                    for (u32 stat = MON_DATA_HP_EV; stat <= MON_DATA_SPDEF_EV; stat++)
                        EXPECT_EQ(GetMonData(&party[0], stat), GetMonData(&hoenn[0], stat));
                }
                else
                {
                    u8 nickname[POKEMON_NAME_LENGTH + 1];
                    GetMonData(&party[0], MON_DATA_NICKNAME, nickname);
                    EXPECT_EQ(StringCompare(nickname, GetSpeciesName(expected)), 0);
                }
                cases++;
            }
        }
        EXPECT_EQ(cases, 324);
        Test_MgbaPrintf("E1_DISPATCH generated=%d regions=9 orderedChoices=6 aliases=2 difficulties=3 allLevels=10/12/14", cases);
        memcpy(gParties[B_TRAINER_OPPONENT_A], savedParty, sizeof(savedParty));
        VarSet(VAR_STARTER_MON, savedFirst); VarSet(VAR_EC_SECOND_STARTER, savedSecond); VarSet(VAR_STARTER_GEN, savedGen);
        gSpecialVar_Result = savedResult; TRAINER_BATTLE_PARAM.opponentA = savedTrainer;
        gBattleTypeFlags = savedFlags; SetCurrentDifficultyLevel(savedDifficulty);
        for (u32 i = 0; i < 8; i++) { if (badges[i]) FlagSet(FLAG_BADGE01_GET + i); else FlagClear(FLAG_BADGE01_GET + i); }
    }
}
