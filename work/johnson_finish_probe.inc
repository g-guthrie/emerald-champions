// Temporary E23 native calibration and four/six-player timing observations.
// Include after JohnsonPlayer and its native early set constants.
#line 54000 "work/johnson_finish_probe.inc"
#include "main.h"
#include "data.h"
#include "caps.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"

static void JohnsonFinishPrint(u32 context)
{
    const enum Species expected[] = {SPECIES_SHINX, SPECIES_STARLY, SPECIES_BUNNELBY, SPECIES_TAILLOW};
    for (u32 slot = 0; slot < ARRAY_COUNT(expected); slot++)
    {
        struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][slot];
        EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), expected[slot]);
        Test_MgbaPrintf("JOHNSON_FINISH_INPUT context=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
            context, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL), GetMonData(mon, MON_DATA_HP),
            GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF), GetMonData(mon, MON_DATA_SPATK),
            GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED), GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Johnson timing: four and six distinct native early player members")
{
    u32 playerCount;
    PARAMETRIZE { playerCount = 4; }
    PARAMETRIZE { playerCount = 6; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet johnsonTimingElectrike = {
            .moves = {MOVE_FLAMETHROWER, MOVE_THUNDERBOLT, MOVE_SNARL, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_LIGHTNING_ROD,
            .statPoints = {32, 0, 18, 16, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet johnsonTimingShellos = {
            .moves = {MOVE_EARTH_POWER, MOVE_SCALD, MOVE_RECOVER, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_STORM_DRAIN,
            .statPoints = {32, 0, 0, 32, 2, 0},
        };
        JohnsonPlayer(SPECIES_EEVEE, &sJohnsonEevee);
        JohnsonPlayer(SPECIES_TIMBURR, &sJohnsonTimburr);
        JohnsonPlayer(SPECIES_PACHIRISU, &sJohnsonPachirisu);
        JohnsonPlayer(SPECIES_LOTAD, &sJohnsonLotad);
        if (playerCount == 6)
        {
            // Both Route103 species and all these moves are native-legal.
            JohnsonPlayer(SPECIES_ELECTRIKE, &johnsonTimingElectrike);
            JohnsonPlayer(SPECIES_SHELLOS, &johnsonTimingShellos);
        }
        AuthoredOpponent(TRAINER_JOHNSON, 0, FALSE);
        JohnsonFinishPrint(playerCount);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        // No outer snapshot or synthetic latency ceiling: the evaluator owns
        // its normal allocations. Rebuilt includes native move-cache work.
        gAiLogicData->battlerMovesScored = 0;
        u32 start = gMain.vblankCounter1;
        bool32 warm = AI_ComputeDoublesDecisions(B_BATTLER_1);
        u32 warmFrames = gMain.vblankCounter1 - start;
        gAiLogicData->battlerMovesScored = 0;
        start = gMain.vblankCounter1;
        SetAiLogicDataForTurn(gAiLogicData);
        bool32 rebuilt = AI_ComputeDoublesDecisions(B_BATTLER_1);
        u32 rebuiltFrames = gMain.vblankCounter1 - start;
        EXPECT(warm && rebuilt);
        Test_MgbaPrintf("JOHNSON_TIMING playerCount=%d selected=%d/%d warmFrames=%d rebuiltFrames=%d damageBytes=%d active=%d/%d HP=%d/%d",
            playerCount, warm, rebuilt, warmFrames, rebuiltFrames, sizeof(struct SimulatedDamage),
            opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Johnson calibration: native Easy and Hard lookup fallback")
{
    enum DifficultyLevel difficulty;
    PARAMETRIZE { difficulty = DIFFICULTY_EASY; }
    PARAMETRIZE { difficulty = DIFFICULTY_HARD; }
    GIVEN {
        JohnsonPlayer(SPECIES_EEVEE, &sJohnsonEevee);
        JohnsonPlayer(SPECIES_TIMBURR, &sJohnsonTimburr);
        struct Pokemon *party = AllocZeroed(sizeof(struct Pokemon) * PARTY_SIZE);
        EXPECT(party != NULL);
        enum DifficultyLevel savedDifficulty = GetCurrentDifficultyLevel();
        u32 savedFlags = gBattleTypeFlags;
        bool8 savedBadges[8];
        for (u32 badge = 0; badge < ARRAY_COUNT(savedBadges); badge++)
        {
            savedBadges[badge] = FlagGet(FLAG_BADGE01_GET + badge);
            FlagClear(FLAG_BADGE01_GET + badge);
        }
        SetCurrentDifficultyLevel(difficulty);
        // This production accessor handles missing difficulty tables. Indexing
        // gTrainers[difficulty] directly would not reproduce actual generation.
        const struct Trainer *trainer = GetTrainerStructFromId(TRAINER_JOHNSON);
        EXPECT_EQ((u32)trainer->partySize, 4);
        u32 cap = GetCurrentLevelCap();
        u32 reduction = GetTrainerLevelReduction();
        gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
        CreateNPCTrainerPartyFromTrainer(party, trainer);
        ApplyTrainerLevelDifficulty(party);
        for (u32 slot = 0; slot < trainer->partySize; slot++)
        {
            u32 expected = max(trainer->party[slot].lvl, cap);
            expected = expected > reduction ? expected - reduction : 1;
            EXPECT_EQ(GetMonData(&party[slot], MON_DATA_LEVEL), expected);
        }
        gBattleTypeFlags = savedFlags;
        SetCurrentDifficultyLevel(savedDifficulty);
        for (u32 badge = 0; badge < ARRAY_COUNT(savedBadges); badge++)
            if (savedBadges[badge])
                FlagSet(FLAG_BADGE01_GET + badge);
            else
                FlagClear(FLAG_BADGE01_GET + badge);
        AI_FLAGS(trainer->aiFlags | AI_FLAG_DOUBLE_BATTLE);
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_JOHNSON;
        for (u32 slot = 0; slot < trainer->partySize; slot++)
            OPPONENT(GetMonData(&party[slot], MON_DATA_SPECIES)) {
                *gBattleTestRunnerState->data.currentMon = party[slot];
                Nature(GetNature(&party[slot])); Ability(GetMonAbility(&party[slot]));
                Speed(GetMonData(&party[slot], MON_DATA_SPEED));
                Moves(GetMonData(&party[slot], MON_DATA_MOVE1), GetMonData(&party[slot], MON_DATA_MOVE2),
                    GetMonData(&party[slot], MON_DATA_MOVE3), GetMonData(&party[slot], MON_DATA_MOVE4));
            }
        Free(party);
        JohnsonFinishPrint(100 + difficulty);
        Test_MgbaPrintf("JOHNSON_CALIBRATION_INPUT difficulty=%d cap=%d reduction=%d", difficulty, cap, reduction);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        Test_MgbaPrintf("JOHNSON_CALIBRATION difficulty=%d active=%d/%d levels=%d/%d HP=%d/%d LAST=%S/%S",
            difficulty, opponentLeft->species, opponentRight->species, opponentLeft->level, opponentRight->level,
            opponentLeft->hp, opponentRight->hp, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]));
        // A native calibration observation, not an Easy/Hard balance verdict.
    }
}



