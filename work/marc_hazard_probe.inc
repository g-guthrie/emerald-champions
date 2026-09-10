// Temporary actual E0029 native observations; independent of prior encounters.
#line 51000 "work/marc_gen9_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void MarcPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    for (u32 index = 0; index < MAX_MON_MOVES; index++)
        if (set->moves[index] != MOVE_NONE)
            EXPECT(CanSpeciesUseEmeraldChampionsPreparationMove(species, set->moves[index]));
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
            GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}


AI_DOUBLE_BATTLE_TEST("EC playtest Marc hazard repeat diagnostic")
{
    u32 turns = 0;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    PARAMETRIZE { turns = 3; }
    GIVEN {
        MarcPlayer(SPECIES_EEVEE, &gEmeraldChampionsDefaultBattleSets[SPECIES_EEVEE]);
        MarcPlayer(SPECIES_TIMBURR, &gEmeraldChampionsDefaultBattleSets[SPECIES_TIMBURR]);
        MarcPlayer(SPECIES_PACHIRISU, &gEmeraldChampionsDefaultBattleSets[SPECIES_PACHIRISU]);
        MarcPlayer(SPECIES_MIENFOO, &gEmeraldChampionsDefaultBattleSets[SPECIES_MIENFOO]);
        MarcPlayer(SPECIES_LOTAD, &gEmeraldChampionsDefaultBattleSets[SPECIES_LOTAD]);
        MarcPlayer(SPECIES_TREECKO, &gEmeraldChampionsDefaultBattleSets[SPECIES_TREECKO]);
        AuthoredOpponent(TRAINER_MARC, 0, FALSE);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT(IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_STEALTH_ROCK));
        EXPECT(!IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_STEALTH_ROCK));
        EXPECT_EQ(opponentRight->pp[3], GetMovePP(MOVE_STEALTH_ROCK) - 1);
        Test_MgbaPrintf("MARC_HAZARD turns=%d ownRocks=%d playerRocks=%d last=%S/%S pp=%d target=%d hazardScore=%d",
            turns, IsHazardOnSide(B_SIDE_OPPONENT, HAZARDS_STEALTH_ROCK),
            IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_STEALTH_ROCK),
            GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]), opponentRight->pp[3],
            gAiBattleData->chosenTarget[3], AI_ScoreMoveAgainstTarget(3, 2, 3));
    }
}
