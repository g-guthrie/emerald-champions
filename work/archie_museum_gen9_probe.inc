// Temporary E0052 diagnostics; reuse native player sets and complete timing
// dependencies from the Museum Two scratch fixture, not its outcome evidence.
#include "museum_two_gen9_probe.inc"
#line 53000 "work/archie_museum_gen9_probe.inc"

static void ArchieMuseumOpponent(u32 pair)
{
    AuthoredOpponent(TRAINER_ARCHIE_SLATEPORT, 1, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species species[] = {SPECIES_WINGULL, SPECIES_LOMBRE, SPECIES_CORPHISH, SPECIES_TENTACOOL, SPECIES_GOLBAT, SPECIES_CARVANHA};
    for (u32 slot = 0; slot < ARRAY_COUNT(species); slot++) {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), species[slot]);
        Test_MgbaPrintf("ARCHIE_MUSEUM_INPUT slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
            slot, GetMonData(&party[slot], MON_DATA_LEVEL), GetMonData(&party[slot], MON_DATA_HP),
            GetMonData(&party[slot], MON_DATA_ATK), GetMonData(&party[slot], MON_DATA_DEF),
            GetMonData(&party[slot], MON_DATA_SPATK), GetMonData(&party[slot], MON_DATA_SPDEF),
            GetMonData(&party[slot], MON_DATA_SPEED), GetMonData(&party[slot], MON_DATA_HELD_ITEM), GetMonAbility(&party[slot]));
    }
    if (pair >= 3) {
        u32 left = pair == 3 ? 1 : 3;
        u32 right = pair == 3 ? 2 : 1;
        struct Pokemon selectedLeft = party[left], selectedRight = party[right];
        party[0] = selectedLeft; party[1] = selectedRight;
        u16 zero = 0;
        for (u32 slot = 2; slot < PARTY_SIZE; slot++) SetMonData(&party[slot], MON_DATA_HP, &zero);
    }
    else if (pair) {
        u16 zero = 0;
        for (u32 slot = 0; slot < 2 * pair; slot++) SetMonData(&party[slot], MON_DATA_HP, &zero);
        for (u32 slot = 0; slot < 2; slot++) {
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + 2 * pair];
            party[slot + 2 * pair] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
    }
}

static void ArchieMuseumResult(u32 board, u32 turns, u32 pair)
{
    Test_MgbaPrintf("ARCHIE_MUSEUM_AFTER board=%d turns=%d pair=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speed=%d/%d foeSpeed=%d/%d items=%d/%d stageAtk=%d/%d stageSpe=%d/%d PP=%d/%d fourthPP=%d/%d rain=%d weatherTimer=%d faints=%d/%d absent=%d",
        board, turns, pair, gBattleMons[1].species, gBattleMons[3].species,
        gBattleMons[1].hp, gBattleMons[3].hp, gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].item, gBattleMons[3].item, gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK],
        gBattleMons[1].statStages[STAT_SPEED], gBattleMons[3].statStages[STAT_SPEED],
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleMons[1].pp[3], gBattleMons[3].pp[3],
        !!(gBattleWeather & B_WEATHER_RAIN), gBattleStruct->weatherDuration,
        gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter, gAbsentBattlerFlags);
}

AI_DOUBLE_BATTLE_TEST("EC live review Archie Museum core: physical special and both reserve pairs")
{
    u32 pair = 0, board = 0, turns = 0;
    for (u32 p = 0; p < 3; p++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (p == 0 ? 4 : (p == 1 && b == 0 ? 2 : 3)); t++) PARAMETRIZE { pair = p; board = b; turns = t; }
    GIVEN {
        MuseumOnePlayer(board == 0 ? SPECIES_EEVEE : SPECIES_NATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        MuseumOnePlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        ArchieMuseumOpponent(pair);
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
    } THEN {
        ArchieMuseumResult(board, turns, pair);
        if (pair == 0 && turns == 1) {
            EXPECT(gBattleWeather & B_WEATHER_RAIN);
            EXPECT_EQ(gAiLogicData->speedStats[3], 86);
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
        }
        if (pair == 1 && board == 0 && turns == 1) {
            EXPECT_EQ(gLastMoves[3], MOVE_RAIN_DANCE);
            EXPECT_EQ(playerLeft->hp, 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Archie Museum disruption: Fake Out Taunt and opposing Tailwind")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= 3; t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        MuseumOnePlayer(board == 0 ? SPECIES_MIENFOO : SPECIES_NATU, board == 0 ? &sMuseumOneMienfoo : &sMuseumOneNatu);
        MuseumOnePlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        ArchieMuseumOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                if (turn == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else if (turn == 1) MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                if (turn == 0) MOVE(playerLeft, MOVE_TAILWIND);
                else MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { ArchieMuseumResult(10 + board, turns, 0); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Archie Museum attrition: one injured usable player Pokemon")
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
        hp = 0; SetMonData(&party[1], MON_DATA_HP, &hp);
        ArchieMuseumOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < 2; turn++) TURN {
            if (board == 0) MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PROTECT); // Absent slot's recorder placeholder.
        }
    } THEN {
        ArchieMuseumResult(20 + board, 2, 0);
        EXPECT(gAbsentBattlerFlags & (1 << B_BATTLER_2));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Archie Museum weather: natural sun overwrite and backup setter")
{
    u32 pair = 0, turns = 0;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= 3; t++) PARAMETRIZE { pair = p == 0 ? 0 : 4; turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet set = {
            .moves = {MOVE_SUNNY_DAY, MOVE_FOUL_PLAY, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        MuseumOnePlayer(SPECIES_SABLEYE, &set);
        MuseumOnePlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        ArchieMuseumOpponent(pair);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn == 1) MOVE(playerLeft, MOVE_SUNNY_DAY);
            else MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } THEN {
        ArchieMuseumResult(30, turns, pair);
        if (pair == 0 && turns == 2) {
            EXPECT(gBattleWeather & B_WEATHER_RAIN);
            EXPECT_EQ(opponentLeft->pp[0], 3);
        }
        if (pair == 4 && turns == 3) {
            EXPECT(gBattleWeather & B_WEATHER_RAIN);
            EXPECT_EQ(opponentLeft->pp[2], 3);
            EXPECT_EQ(gAiLogicData->speedStats[3], 86);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Archie Museum setup: fast foes with redirected bulk")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        MuseumOnePlayer(SPECIES_MIENFOO, &sMuseumOneMienfoo);
        MuseumOnePlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        ArchieMuseumOpponent(1);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } THEN { ArchieMuseumResult(41, turns, 1); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Archie Museum setup: mixed Fake Out and Dragon Dance opportunity")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        MuseumOnePlayer(SPECIES_EEVEE, &sMuseumOneEevee);
        MuseumOnePlayer(SPECIES_TIMBURR, &sMuseumOneTimburr);
        ArchieMuseumOpponent(3);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
        }
    } THEN { ArchieMuseumResult(40, turns, 3); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Archie Museum timing: complete cold six-player party")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_NATU, SPECIES_MUDKIP, SPECIES_TREECKO, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++)
            MuseumOnePlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]]);
        AuthoredOpponent(TRAINER_ARCHIE_SLATEPORT, 1, FALSE);
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
        Test_MgbaPrintf("ARCHIE_MUSEUM_TIMING setup=%d bothActors=%d complete=%d warm=%d", setup, complete - setup, complete, gMain.vblankCounter1 - warmStart);
    }
}
