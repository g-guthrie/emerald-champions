// Temporary E0061 Alyssa native review. Edward is a different battle.
#include "isabel_gen9_probe.inc"
#line 4 "work/alyssa_gen9_probe.inc"

static void AlyssaOpponent(u32 pair)
{
    AuthoredOpponent(TRAINER_ALYSSA, 2, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    for (u32 i = 0; i < 3; i++)
        Test_MgbaPrintf("ALYSSA_INPUT slot=%d species=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d", i,
            GetMonData(&party[i], MON_DATA_SPECIES), GetMonData(&party[i], MON_DATA_LEVEL), GetMonData(&party[i], MON_DATA_HP), GetMonData(&party[i], MON_DATA_ATK),
            GetMonData(&party[i], MON_DATA_DEF), GetMonData(&party[i], MON_DATA_SPATK), GetMonData(&party[i], MON_DATA_SPDEF),
            GetMonData(&party[i], MON_DATA_SPEED), GetMonData(&party[i], MON_DATA_HELD_ITEM), GetMonAbility(&party[i]));
    if (pair) {
        const enum Species orders[][3] = {
            {SPECIES_GLIGAR, SPECIES_CYCLIZAR, SPECIES_FLETCHINDER},
            {SPECIES_GLIGAR, SPECIES_CYCLIZAR, SPECIES_FLETCHINDER},
            {SPECIES_CYCLIZAR, SPECIES_FLETCHINDER, SPECIES_GLIGAR},
            {SPECIES_CYCLIZAR, SPECIES_FLETCHINDER, SPECIES_GLIGAR},
            {SPECIES_GLIGAR, SPECIES_FLETCHINDER, SPECIES_CYCLIZAR},
        };
        struct Pokemon saved[3];
        for (u32 i = 0; i < 3; i++) saved[i] = party[i];
        for (u32 i = 0; i < 3; i++) for (u32 j = 0; j < 3; j++)
            if (GetMonData(&saved[j], MON_DATA_SPECIES) == orders[pair][i]) party[i] = saved[j];
        if (pair < 3) { u16 zero = 0; SetMonData(&party[2], MON_DATA_HP, &zero); }
    }
}

static void AlyssaResult(u32 board, u32 turns, u32 pair)
{
    Test_MgbaPrintf("ALYSSA_AFTER board=%d turns=%d pair=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speed=%d/%d foeSpeed=%d/%d items=%d/%d PP=%d/%d secondPP=%d/%d thirdPP=%d/%d fourthPP=%d/%d TW=%d sub=%d/%d faints=%d/%d partyHP=%d/%d/%d foeStatus=%d/%d",
        board, turns, pair, gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1] < MOVES_COUNT ? gLastMoves[1] : MOVE_NONE), GetMoveName(gLastMoves[3] < MOVES_COUNT ? gLastMoves[3] : MOVE_NONE),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].item, gBattleMons[3].item,
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleMons[1].pp[1], gBattleMons[3].pp[1],
        gBattleMons[1].pp[2], gBattleMons[3].pp[2], gBattleMons[1].pp[3], gBattleMons[3].pp[3],
        gSideTimers[1].tailwindTimer, gBattleMons[1].volatiles.substituteHP, gBattleMons[3].volatiles.substituteHP,
        gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter,
        GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP), GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
        GetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_HP), gBattleMons[0].status1, gBattleMons[2].status1);
}

AI_DOUBLE_BATTLE_TEST("EC live review Alyssa core: native leads reserves and Cyclizar opening control")
{
    u32 board = 0, turns = 0, pair = 0;
    for (u32 p = 0; p < 5; p++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (p == 0 ? (b == 0 ? 4 : 3) : (p == 4 && b == 0 ? 2 : 3)); t++) PARAMETRIZE { pair = p; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        AlyssaOpponent(pair);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } SCENE {
        if (pair == 0 && board == 1 && turns == 3) ANIMATION(ANIM_TYPE_MOVE, MOVE_SHED_TAIL, opponentRight);
    } THEN {
        AlyssaResult(board, turns, pair);
        if (pair == 0 && board == 1 && turns == 3) {
            EXPECT_EQ(opponentRight->species, SPECIES_FLETCHINDER);
            EXPECT_EQ((u32)opponentRight->volatiles.substituteHP, 26);
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP), 84);
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HELD_ITEM), ITEM_NONE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Alyssa disruption: Fake Out and Ice pressure")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 2; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        IsabelPlayer(SPECIES_MIENFOO, &sMuseumOneMienfoo);
        IsabelPlayer(SPECIES_LOMBRE, &sMuseumOneLotad);
        AlyssaOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            else MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_ICE_BEAM, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { AlyssaResult(10, turns, 0); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Alyssa low pressure: status and Shed Tail opportunity")
{
    u32 turns = 0, pair = 0;
    for (u32 p = 0; p < 3; p++) for (u32 t = 1; t <= (p == 0 ? 7 : 4); t++) PARAMETRIZE { turns = t; pair = p == 0 ? 0 : p + 2; }
    GIVEN {
        const struct EmeraldChampionsBattleSet sableye = {
            .moves = {MOVE_FOUL_PLAY, MOVE_WILL_O_WISP, MOVE_RECOVER, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        IsabelPlayer(SPECIES_SABLEYE, &sableye);
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        AlyssaOpponent(pair);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn == 0) MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentLeft);
            else MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } THEN { AlyssaResult(20, turns, pair); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Alyssa fast pressure: Crobat speed control")
{
    u32 turns = 0, pair = 0;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= 4; t++) PARAMETRIZE { turns = t; pair = p == 0 ? 0 : 4; }
    GIVEN {
        const struct EmeraldChampionsBattleSet crobat = {
            .moves = {MOVE_AIR_SLASH, MOVE_SLUDGE_BOMB, MOVE_TAUNT, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_TIMID, .ability = ABILITY_INNER_FOCUS,
            .evs = {4, 0, 0, 252, 0, 252},
        };
        IsabelPlayer(SPECIES_CROBAT, &crobat);
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        AlyssaOpponent(pair);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_AIR_SLASH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (pair == 4) ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, opponentRight);
    } THEN {
        AlyssaResult(30, turns, pair);
        if (pair == 4 && turns == 1) {
            EXPECT_EQ(opponentRight->pp[2], 14);
            EXPECT_EQ(gSideTimers[1].tailwindTimer, 3);
            EXPECT(gAiLogicData->speedStats[1] > gAiLogicData->speedStats[0]);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Alyssa timing: complete cold six-player party")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) IsabelPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]]);
        AlyssaOpponent(0);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        u32 start = gMain.vblankCounter1;
        AssignUsableGimmicks(); SetShellSideArmCategory(); SetAiLogicDataForTurn(gAiLogicData);
        gAiLogicData->reverseBattlerLogicOrder = RandomPercentage(RNG_AI_REVERSE_BATTLER_LOGIC_ORDER,
            GetConfig(AI_REVERSE_BATTLER_LOGIC_ORDER_CHANCE)) && IsDoubleBattle();
        u32 setup = gMain.vblankCounter1 - start;
        enum BattlerId first = gAiLogicData->reverseBattlerLogicOrder ? B_BATTLER_3 : B_BATTLER_1;
        ComputeAiBattlerDecisions(first); ComputeAiBattlerDecisions(GetPartnerBattler(first));
        u32 complete = gMain.vblankCounter1 - start;
        gAiLogicData->battlerMovesScored = 0;
        u32 warmStart = gMain.vblankCounter1;
        ComputeAiBattlerDecisions(first); ComputeAiBattlerDecisions(GetPartnerBattler(first));
        Test_MgbaPrintf("ALYSSA_TIMING setup=%d bothActors=%d complete=%d warm=%d", setup, complete - setup, complete, gMain.vblankCounter1 - warmStart);
        EXPECT(complete <= 72);
    }
}
