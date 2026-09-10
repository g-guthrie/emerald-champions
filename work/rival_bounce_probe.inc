// Temporary investigation only. Include after AuthoredOpponent in plans.c.
#line 5000 "work/rival_bounce_probe.inc"
// Native execution and same-state scorer comparisons are separate evidence.
#include "battle_ai_main.h"

static void RivalBouncePlayer(enum Species species, enum Ability natuAbility)
{
    struct Pokemon mon;
    struct EmeraldChampionsBattleSet preset = gEmeraldChampionsDefaultBattleSets[species];
    if (species == SPECIES_NATU)
        preset.ability = natuAbility;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    ApplyEmeraldChampionsScriptedSet(&mon, &preset);
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon));
        Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
              GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

static void RivalBounceReserves(void)
{
    u16 zero = 0;
    AuthoredOpponent(TRAINER_BRENDAN_ROUTE_103_TORCHIC, 0, FALSE);
    SetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP, &zero);
    SetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP, &zero);
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 2;
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 3;
}

DOUBLE_BATTLE_TEST("EC live review E0001 Bounce: selected status reaches non-Bounce partner")
{
    enum Ability ability;
    enum Move move;
    PARAMETRIZE { ability = ABILITY_MAGIC_BOUNCE; move = MOVE_SPORE; }
    PARAMETRIZE { ability = ABILITY_EARLY_BIRD; move = MOVE_SPORE; }
    PARAMETRIZE { ability = ABILITY_MAGIC_BOUNCE; move = MOVE_LEECH_SEED; }
    PARAMETRIZE { ability = ABILITY_EARLY_BIRD; move = MOVE_LEECH_SEED; }
    GIVEN {
        RivalBouncePlayer(SPECIES_MIENFOO, ability);
        RivalBouncePlayer(SPECIES_NATU, ability);
        RivalBounceReserves();
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight);
            MOVE(playerRight, MOVE_TAILWIND);
            MOVE(opponentLeft, move, target: playerLeft, hit: TRUE);
            MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_1], 2);
        EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_3], 3);
        EXPECT_EQ(opponentLeft->species, SPECIES_SHROOMISH);
        EXPECT_EQ(opponentRight->species, SPECIES_TAILLOW);
        EXPECT_EQ(gLastMoves[B_BATTLER_1], move);
        if (move == MOVE_SPORE)
            EXPECT(playerLeft->status1 & STATUS1_SLEEP);
        else
            EXPECT_EQ((u32)playerLeft->volatiles.leechSeed, LEECHSEEDED_BY(B_BATTLER_1));
        Test_MgbaPrintf("E1 Bounce native ability=%d executed=%S targetStatus=%d seed=%d",
            ability, GetMoveName(gLastMoves[B_BATTLER_1]), playerLeft->status1,
            playerLeft->volatiles.leechSeed);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review E0001 Bounce: actual reserves same-state target scores")
{
    GIVEN {
        RivalBouncePlayer(SPECIES_MIENFOO, ABILITY_MAGIC_BOUNCE);
        RivalBouncePlayer(SPECIES_NATU, ABILITY_MAGIC_BOUNCE);
        RivalBounceReserves();
    } WHEN {
        TURN {
            // Preserve both targets for the diagnostic; no exact AI move is
            // prescribed. The normal decision-time logger records its choice.
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_SHROOMISH);
        EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_1], 2);
        EXPECT(playerLeft->hp && playerRight->hp && opponentLeft->hp);
        struct AiThinkingStruct thinking = *gAiThinkingStruct;
        enum Ability savedAbility = gAiLogicData->abilities[B_BATTLER_2];
        enum Ability savedNativeAbility = playerRight->ability;
        enum Move savedPartnerMove = gAiLogicData->partnerMove;
        gAiLogicData->partnerMove = MOVE_NONE;
        for (u32 i = 0; i < 2; i++)
        {
            enum Move move = i ? MOVE_LEECH_SEED : MOVE_SPORE;
            u32 index = GetMoveIndex(B_BATTLER_1, move);
            EXPECT(index < MAX_MON_MOVES);
            gAiLogicData->abilities[B_BATTLER_2] = playerRight->ability = ABILITY_EARLY_BIRD;
            s32 allyControl = AI_ScoreMoveAgainstTarget(B_BATTLER_1, B_BATTLER_0, index);
            s32 directControl = AI_ScoreMoveAgainstTarget(B_BATTLER_1, B_BATTLER_2, index);
            gAiLogicData->abilities[B_BATTLER_2] = playerRight->ability = ABILITY_MAGIC_BOUNCE;
            s32 allyBounce = AI_ScoreMoveAgainstTarget(B_BATTLER_1, B_BATTLER_0, index);
            s32 directBounce = AI_ScoreMoveAgainstTarget(B_BATTLER_1, B_BATTLER_2, index);
            Test_MgbaPrintf("E1 Bounce score %S allyControl=%d allyBounce=%d directControl=%d directBounce=%d executed=%S",
                GetMoveName(move), allyControl, allyBounce, directControl, directBounce,
                GetMoveName(gLastMoves[B_BATTLER_1]));
            EXPECT(allyControl > 0);
            EXPECT_EQ(allyBounce, allyControl);
            EXPECT(directBounce < directControl);
        }
        gAiLogicData->abilities[B_BATTLER_2] = savedAbility;
        playerRight->ability = savedNativeAbility;
        gAiLogicData->partnerMove = savedPartnerMove;
        *gAiThinkingStruct = thinking;
        // These are counterfactual scores on the post-turn native board, not
        // the scores or target that produced turn one's executed command.
    }
}
