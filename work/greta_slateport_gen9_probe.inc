// Temporary Greta review. Native player helpers/timing dependencies only;
// older Museum test groups are not selected by the Greta prefix.
#include "museum_two_gen9_probe.inc"
#line 54000 "work/greta_slateport_gen9_probe.inc"

static void GretaSlateportResult(u32 board, u32 turns, u32 pair);

static void GretaSlateportOpponent(u32 reserves)
{
    AuthoredOpponent(TRAINER_GRETA_SLATEPORT, 1, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species species[] = {SPECIES_HERACROSS, SPECIES_UMBREON, SPECIES_SHEDINJA, SPECIES_GASTLY};
    for (u32 slot = 0; slot < ARRAY_COUNT(species); slot++) {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), species[slot]);
        Test_MgbaPrintf("GRETA_INPUT slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
            slot, GetMonData(&party[slot], MON_DATA_LEVEL), GetMonData(&party[slot], MON_DATA_HP),
            GetMonData(&party[slot], MON_DATA_ATK), GetMonData(&party[slot], MON_DATA_DEF),
            GetMonData(&party[slot], MON_DATA_SPATK), GetMonData(&party[slot], MON_DATA_SPDEF),
            GetMonData(&party[slot], MON_DATA_SPEED), GetMonData(&party[slot], MON_DATA_HELD_ITEM), GetMonAbility(&party[slot]));
    }
    if (reserves == 2) {
        struct Pokemon saved = party[1];
        party[1] = party[3]; party[3] = saved;
        u16 zero = 0;
        SetMonData(&party[2], MON_DATA_HP, &zero);
        SetMonData(&party[3], MON_DATA_HP, &zero);
    }
    else if (reserves == 3) {
        u16 hp = 32;
        SetMonData(&party[1], MON_DATA_HP, &hp);
    }
    else if (reserves == 1) {
        u16 zero = 0;
        for (u32 slot = 0; slot < 2; slot++) {
            SetMonData(&party[slot], MON_DATA_HP, &zero);
            struct Pokemon saved = party[slot]; party[slot] = party[slot + 2]; party[slot + 2] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Greta sustain: injured Umbreon under special pressure")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        MuseumOnePlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        MuseumOnePlayer(SPECIES_LOTAD, &sMuseumOneLotad);
        GretaSlateportOpponent(3);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentRight, hit: TRUE, criticalHit: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, opponentRight);
        if (turns >= 2) ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_WISH_HEAL, opponentRight);
    } THEN {
        GretaSlateportResult(30, turns, 3);
        Test_MgbaPrintf("GRETA_WISH count=%d HP=%d", gBattleStruct->wish[3].counter, opponentRight->hp);
        EXPECT_EQ(opponentRight->pp[2], 9);
        if (turns >= 2) EXPECT_EQ(opponentRight->hp, 65);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Greta spread safety: mixed nonimmune ally")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 2; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        MuseumOnePlayer(SPECIES_EEVEE, &sMuseumOneEevee);
        MuseumOnePlayer(SPECIES_TIMBURR, &sMuseumOneTimburr);
        GretaSlateportOpponent(2);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
        }
    } THEN { GretaSlateportResult(31, turns, 2); }
}

static void GretaSlateportResult(u32 board, u32 turns, u32 pair)
{
    Test_MgbaPrintf("GRETA_AFTER board=%d turns=%d pair=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speed=%d/%d foeSpeed=%d/%d items=%d/%d stageAtk=%d/%d foeSpA=%d/%d PP=%d/%d thirdPP=%d/%d fourthPP=%d/%d status=%d/%d faints=%d/%d",
        board, turns, pair, gBattleMons[1].species, gBattleMons[3].species,
        gBattleMons[1].hp, gBattleMons[3].hp, gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].item, gBattleMons[3].item, gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK],
        gBattleMons[0].statStages[STAT_SPATK], gBattleMons[2].statStages[STAT_SPATK],
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleMons[1].pp[2], gBattleMons[3].pp[2],
        gBattleMons[1].pp[3], gBattleMons[3].pp[3], gBattleMons[1].status1, gBattleMons[3].status1,
        gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}

AI_DOUBLE_BATTLE_TEST("EC live review Greta core: physical special leads and reserves")
{
    u32 pair = 0, board = 0, turns = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (p == 0 && b == 0 ? 2 : 3); t++) PARAMETRIZE { pair = p; board = b; turns = t; }
    GIVEN {
        MuseumOnePlayer(board == 0 ? SPECIES_EEVEE : SPECIES_NATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        MuseumOnePlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        GretaSlateportOpponent(pair);
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
    } SCENE {
        if (pair == 1 && board == 0 && turns >= 2)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SLUDGE_WAVE, opponentRight);
    } THEN {
        GretaSlateportResult(board, turns, pair);
        if (pair == 0 && board == 0 && turns == 1)
            EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        if (pair == 1 && board == 0 && turns >= 2) {
            EXPECT_EQ(opponentLeft->hp, 1);
            EXPECT_EQ(opponentLeft->item, ITEM_FOCUS_SASH);
            EXPECT_EQ(opponentLeft->status1, 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Greta disruption: Fake Out Taunt and Flying coverage")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= (b == 0 ? 4 : 3); t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        if (board == 0) MuseumOnePlayer(SPECIES_MIENFOO, &sMuseumOneMienfoo);
        else {
            struct EmeraldChampionsBattleSet set = sMuseumOneNatu;
            set.item = ITEM_FOCUS_SASH;
            MuseumOnePlayer(SPECIES_NATU, &set);
        }
        MuseumOnePlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        GretaSlateportOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                if (turn == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else if (turn == 1) MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else MOVE(playerLeft, MOVE_AIR_SLASH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { GretaSlateportResult(10 + board, turns, 0); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Greta counterplay: burn bypasses Wonder Guard")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 2; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet set = {
            .moves = {MOVE_WILL_O_WISP, MOVE_FOUL_PLAY, MOVE_RECOVER, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        MuseumOnePlayer(SPECIES_SABLEYE, &set);
        MuseumOnePlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        GretaSlateportOpponent(1);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentLeft, hit: TRUE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { GretaSlateportResult(21, turns, 1); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Greta counterplay: Ghost coverage into Wonder Guard")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 2; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet set = {
            .moves = {MOVE_SHADOW_BALL, MOVE_SLUDGE_BOMB, MOVE_ICY_WIND, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .nature = NATURE_TIMID, .ability = ABILITY_LEVITATE,
            .evs = {4, 0, 0, 252, 0, 252},
        };
        MuseumOnePlayer(SPECIES_GASTLY, &set);
        MuseumOnePlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        GretaSlateportOpponent(1);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_SHADOW_BALL, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { GretaSlateportResult(20, turns, 1); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Greta timing: complete cold six-player party")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_NATU, SPECIES_MUDKIP, SPECIES_TREECKO, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++)
            MuseumOnePlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]]);
        AuthoredOpponent(TRAINER_GRETA_SLATEPORT, 1, FALSE);
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
        Test_MgbaPrintf("GRETA_TIMING setup=%d bothActors=%d complete=%d warm=%d", setup, complete - setup, complete, gMain.vblankCounter1 - warmStart);
    }
}
