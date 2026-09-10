// Temporary E0014 healthy reserves, native party-size timing and level
// calibration. Include after GinaMiaLeadPlayer; no trainer choice is forced.
#line 61000 "work/gina_mia_finish_probe.inc"
#include "main.h"
#include "battle_ai_switch.h"

static void GinaFinishPrint(u32 context)
{
    for (u32 slot = 0; slot < 4; slot++)
    {
        struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][slot];
        Test_MgbaPrintf("GINA_FINISH_INPUT context=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
            context, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
            GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
            GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
            GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Gina finish: healthy insect reserves against physical and special bulk")
{
    bool32 special;
    u32 turns;
    PARAMETRIZE { special = FALSE; turns = 1; }
    PARAMETRIZE { special = FALSE; turns = 2; }
    PARAMETRIZE { special = TRUE; turns = 1; }
    PARAMETRIZE { special = TRUE; turns = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet pachi = {
            .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet timburr = {
            .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet lotad = {
            .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_CALM, .ability = ABILITY_RAIN_DISH,
            .statPoints = {32, 0, 2, 0, 32, 0},
        };
        GinaMiaLeadPlayer(SPECIES_PACHIRISU, &pachi);
        GinaMiaLeadPlayer(special ? SPECIES_LOTAD : SPECIES_TIMBURR, special ? &lotad : &timburr);
        AuthoredOpponent(TRAINER_GINA_AND_MIA_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PLUSLE);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_MINUN);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_VOLBEAT);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_ILLUMISE);
        u16 zero = 0;
        SetMonData(&party[0], MON_DATA_HP, &zero);
        SetMonData(&party[1], MON_DATA_HP, &zero);
        for (u32 slot = 0; slot < 2; slot++)
        {
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + 2];
            party[slot + 2] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
        GinaFinishPrint(special);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, special ? MOVE_GIGA_DRAIN : MOVE_ICE_PUNCH, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("GINA_FINISH_RESERVES special=%d turns=%d actualTurns=%d species=%d/%d ownHP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S ownSpA=%d/%d ownStatus=%d/%d faints=%d/%d",
            special, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            opponentLeft->status1, opponentRight->status1,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Gina timing: four and six native player members")
{
    u32 playerCount;
    PARAMETRIZE { playerCount = 4; }
    PARAMETRIZE { playerCount = 6; }
    GIVEN {
        // Native early prepared reserve diversity; no repeated dummy mons.
        GinaMiaLeadPlayer(SPECIES_EEVEE, &sGinaMiaEevee);
        GinaMiaLeadPlayer(SPECIES_TIMBURR, &sGinaMiaTimburr);
        GinaMiaLeadPlayer(SPECIES_ELECTRIKE, &sGinaMiaElectrike);
        GinaMiaLeadPlayer(SPECIES_SHELLOS, &sGinaMiaShellos);
        if (playerCount == 6)
        {
            GinaMiaLeadPlayer(SPECIES_MIENFOO, &sGinaMiaMienfoo);
            GinaMiaLeadPlayer(SPECIES_PACHIRISU, &sGinaMiaPachirisu);
        }
        AuthoredOpponent(TRAINER_GINA_AND_MIA_1, 0, FALSE);
        GinaFinishPrint(playerCount);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        // No extra snapshot around the evaluator: it owns/restores its own
        // allocation. Warm excludes the explicitly rebuilt native move cache.
        gAiLogicData->battlerMovesScored = 0;
        u32 start = gMain.vblankCounter1;
        bool32 warm = AI_ComputeDoublesDecisions(B_BATTLER_1);
        u32 warmFrames = gMain.vblankCounter1 - start;
        gAiLogicData->battlerMovesScored = 0;
        start = gMain.vblankCounter1;
        SetAiLogicDataForTurn(gAiLogicData);
        bool32 cold = AI_ComputeDoublesDecisions(B_BATTLER_1);
        u32 coldFrames = gMain.vblankCounter1 - start;
        Test_MgbaPrintf("GINA_TIMING playerCount=%d selected=%d/%d warmFrames=%d rebuiltFrames=%d active=%d/%d HP=%d/%d",
            playerCount, warm, cold, warmFrames, coldFrames, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Gina calibration: native Easy and Hard party generation")
{
    enum DifficultyLevel difficulty;
    PARAMETRIZE { difficulty = DIFFICULTY_EASY; }
    PARAMETRIZE { difficulty = DIFFICULTY_HARD; }
    GIVEN {
        GinaMiaLeadPlayer(SPECIES_EEVEE, &sGinaMiaEevee);
        GinaMiaLeadPlayer(SPECIES_TIMBURR, &sGinaMiaTimburr);
        struct Pokemon *party = AllocZeroed(sizeof(struct Pokemon) * PARTY_SIZE);
        enum DifficultyLevel savedDifficulty = GetCurrentDifficultyLevel();
        u32 savedFlags = gBattleTypeFlags;
        bool8 savedBadges[8];
        for (u32 badge = 0; badge < ARRAY_COUNT(savedBadges); badge++)
        {
            savedBadges[badge] = FlagGet(FLAG_BADGE01_GET + badge);
            FlagClear(FLAG_BADGE01_GET + badge);
        }
        SetCurrentDifficultyLevel(difficulty);
        const struct Trainer *trainer = GetTrainerStructFromId(TRAINER_GINA_AND_MIA_1);
        gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
        CreateNPCTrainerPartyFromTrainer(party, trainer);
        ApplyTrainerLevelDifficulty(party);
        gBattleTypeFlags = savedFlags;
        SetCurrentDifficultyLevel(savedDifficulty);
        for (u32 badge = 0; badge < ARRAY_COUNT(savedBadges); badge++)
            if (savedBadges[badge])
                FlagSet(FLAG_BADGE01_GET + badge);
        AI_FLAGS(trainer->aiFlags | AI_FLAG_DOUBLE_BATTLE);
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_GINA_AND_MIA_1;
        for (u32 slot = 0; slot < trainer->partySize; slot++)
            OPPONENT(GetMonData(&party[slot], MON_DATA_SPECIES)) {
                *gBattleTestRunnerState->data.currentMon = party[slot];
                Nature(GetNature(&party[slot])); Ability(GetMonAbility(&party[slot]));
                Speed(GetMonData(&party[slot], MON_DATA_SPEED));
                Moves(GetMonData(&party[slot], MON_DATA_MOVE1), GetMonData(&party[slot], MON_DATA_MOVE2),
                    GetMonData(&party[slot], MON_DATA_MOVE3), GetMonData(&party[slot], MON_DATA_MOVE4));
            }
        Free(party);
        GinaFinishPrint(100 + difficulty);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        Test_MgbaPrintf("GINA_CALIBRATION difficulty=%d active=%d/%d levels=%d/%d HP=%d/%d Speed=%d/%d LAST=%S/%S",
            difficulty, opponentLeft->species, opponentRight->species, opponentLeft->level, opponentRight->level,
            opponentLeft->hp, opponentRight->hp, opponentLeft->speed, opponentRight->speed,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]));
        // Level calibration observation, not evidence of Easy/Hard balance.
    }
}
