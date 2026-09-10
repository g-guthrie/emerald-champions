// Temporary E0051 review. Reuse the preceding native player-set helpers;
// its EC live review Museum One groups are excluded by the selected prefix.
#include "museum_one_gen9_probe.inc"
#include "battle_ai_main.h"
#include "battle_gimmick.h"
#include "main.h"
#include "random.h"
#include "config/ai.h"
#line 52000 "work/museum_two_gen9_probe.inc"

static void MuseumTwoOpponent(u32 reserves)
{
    AuthoredOpponent(TRAINER_GRUNT_MUSEUM_2, 1, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species species[] = {SPECIES_GRIMER_ALOLA, SPECIES_CLAUNCHER, SPECIES_CROCONAW, SPECIES_CORPHISH};
    for (u32 slot = 0; slot < ARRAY_COUNT(species); slot++) {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), species[slot]);
        Test_MgbaPrintf("MUSEUM_TWO_INPUT slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
            slot, GetMonData(&party[slot], MON_DATA_LEVEL), GetMonData(&party[slot], MON_DATA_HP),
            GetMonData(&party[slot], MON_DATA_ATK), GetMonData(&party[slot], MON_DATA_DEF),
            GetMonData(&party[slot], MON_DATA_SPATK), GetMonData(&party[slot], MON_DATA_SPDEF),
            GetMonData(&party[slot], MON_DATA_SPEED), GetMonData(&party[slot], MON_DATA_HELD_ITEM), GetMonAbility(&party[slot]));
    }
    if (reserves == 1) {
        u16 zero = 0;
        for (u32 slot = 0; slot < 2; slot++) {
            SetMonData(&party[slot], MON_DATA_HP, &zero);
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + 2];
            party[slot + 2] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
    }
    else if (reserves == 2) {
        // Explicit mixed survivor pair, not a forced native switch.
        struct Pokemon saved = party[1];
        party[1] = party[2];
        party[2] = saved;
        u16 zero = 0;
        SetMonData(&party[2], MON_DATA_HP, &zero);
        SetMonData(&party[3], MON_DATA_HP, &zero);
    }
}

static void MuseumTwoResult(u32 board, u32 turns, u32 reserves)
{
    Test_MgbaPrintf("MUSEUM_TWO_AFTER board=%d turns=%d pair=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speed=%d/%d foeSpeed=%d/%d items=%d/%d foeItems=%d/%d status=%d/%d foeStatus=%d/%d PP=%d/%d guardPP=%d/%d faints=%d/%d absent=%d",
        board, turns, reserves, gBattleMons[1].species, gBattleMons[3].species,
        gBattleMons[1].hp, gBattleMons[3].hp, gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].item, gBattleMons[3].item, gBattleMons[0].item, gBattleMons[2].item,
        gBattleMons[1].status1, gBattleMons[3].status1, gBattleMons[0].status1, gBattleMons[2].status1,
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleMons[1].pp[3], gBattleMons[3].pp[3],
        gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter, gAbsentBattlerFlags);
}

AI_DOUBLE_BATTLE_TEST("EC live review Museum Two core: physical special leads and reserves")
{
    u32 board = 0, turns = 0, reserves = 0;
    for (u32 p = 0; p < 2; p++)
        for (u32 b = 0; b < 2; b++)
            for (u32 t = 1; t <= 3; t++) PARAMETRIZE { board = b; turns = t; reserves = p; }
    GIVEN {
        MuseumOnePlayer(board == 0 ? SPECIES_EEVEE : SPECIES_NATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        MuseumOnePlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        MuseumTwoOpponent(reserves);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN { MuseumTwoResult(board, turns, reserves); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Museum Two disruption: Fake Out Taunt and redirection")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= 4; t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        MuseumOnePlayer(SPECIES_MIENFOO, &sMuseumOneMienfoo);
        MuseumOnePlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        MuseumTwoOpponent(board);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else if (turn == 1) MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
            else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            if (board == 0) MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } THEN { MuseumTwoResult(10 + board, turns, board); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Museum Two attrition: one injured usable player Pokemon")
{
    u32 board = 0;
    PARAMETRIZE { board = 0; }
    PARAMETRIZE { board = 1; }
    GIVEN {
        MuseumOnePlayer(board == 0 ? SPECIES_EEVEE : SPECIES_TIMBURR, board == 0 ? &sMuseumOneEevee : &sMuseumOneTimburr);
        MuseumOnePlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
        u16 hp = GetMonData(&party[0], MON_DATA_MAX_HP) * 3 / 4;
        SetMonData(&party[0], MON_DATA_HP, &hp);
        hp = 0;
        SetMonData(&party[1], MON_DATA_HP, &hp);
        MuseumTwoOpponent(FALSE);
    } WHEN {
        for (u32 turn = 0; turn < (board == 0 ? 1 : 2); turn++) TURN {
            if (board == 0) MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            // The recorder requires a known placeholder action even though
            // native battle startup marks this fainted slot absent.
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        MuseumTwoResult(20 + board, board == 0 ? 1 : 2, 0);
        EXPECT(gAbsentBattlerFlags & (1 << B_BATTLER_2));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Museum Two support: mixed item removal and Sheer Force")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        MuseumOnePlayer(SPECIES_TIMBURR, &sMuseumOneTimburr);
        MuseumOnePlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        MuseumTwoOpponent(2);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_BULK_UP);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } THEN {
        MuseumTwoResult(30, turns, 2);
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
        EXPECT_EQ(opponentRight->item, ITEM_LIFE_ORB);
        if (turns >= 2) EXPECT_LT(opponentRight->pp[0], 15);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Museum Two timing: complete cold decision six-player party")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_NATU, SPECIES_MUDKIP, SPECIES_TREECKO, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++)
            MuseumOnePlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]]);
        AuthoredOpponent(TRAINER_GRUNT_MUSEUM_2, 1, FALSE);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        u32 start = gMain.vblankCounter1;
        AssignUsableGimmicks();
        SetShellSideArmCategory();
        SetAiLogicDataForTurn(gAiLogicData);
        gAiLogicData->reverseBattlerLogicOrder = RandomPercentage(RNG_AI_REVERSE_BATTLER_LOGIC_ORDER,
            GetConfig(AI_REVERSE_BATTLER_LOGIC_ORDER_CHANCE)) && IsDoubleBattle();
        u32 setup = gMain.vblankCounter1 - start;
        enum BattlerId first = gAiLogicData->reverseBattlerLogicOrder ? B_BATTLER_3 : B_BATTLER_1;
        ComputeAiBattlerDecisions(first);
        ComputeAiBattlerDecisions(GetPartnerBattler(first));
        u32 complete = gMain.vblankCounter1 - start;
        gAiLogicData->battlerMovesScored = 0;
        u32 warmStart = gMain.vblankCounter1;
        ComputeAiBattlerDecisions(first);
        ComputeAiBattlerDecisions(GetPartnerBattler(first));
        Test_MgbaPrintf("MUSEUM_TWO_TIMING setup=%d bothActors=%d complete=%d warm=%d", setup, complete - setup, complete, gMain.vblankCounter1 - warmStart);
    }
}
