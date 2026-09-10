// Temporary E0065 native review; no permanent trainer-specific suite.
#include "isabel_gen9_probe.inc"
#line 4 "work/abigail_gen9_probe.inc"

static void AbigailOpponent(bool32 reserves)
{
    AuthoredOpponent(TRAINER_ABIGAIL_1, 2, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species species[] = {SPECIES_SCOLIPEDE, SPECIES_PONYTA, SPECIES_RABOOT, SPECIES_TOXTRICITY};
    for (u32 i = 0; i < 4; i++) {
        EXPECT_EQ(GetMonData(&party[i], MON_DATA_SPECIES), species[i]);
        Test_MgbaPrintf("ABIGAIL_INPUT slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Spe=%d item=%d ability=%d", i,
            GetMonData(&party[i], MON_DATA_LEVEL), GetMonData(&party[i], MON_DATA_HP), GetMonData(&party[i], MON_DATA_ATK),
            GetMonData(&party[i], MON_DATA_DEF), GetMonData(&party[i], MON_DATA_SPATK), GetMonData(&party[i], MON_DATA_SPDEF),
            GetMonData(&party[i], MON_DATA_SPEED), GetMonData(&party[i], MON_DATA_HELD_ITEM), GetMonAbility(&party[i]));
    }
    if (reserves) for (u32 i = 0; i < 2; i++) {
        struct Pokemon mon = party[i]; party[i] = party[i + 2]; party[i + 2] = mon;
        u16 zero = 0; SetMonData(&party[i + 2], MON_DATA_HP, &zero);
    }
}

static void AbigailResult(u32 board, u32 turns, bool32 reserves)
{
    Test_MgbaPrintf("ABIGAIL_AFTER board=%d turns=%d reserves=%d species=%d/%d HP=%d/%d foes=%d/%d Atk=%d/%d SpA=%d/%d Spe=%d/%d item=%d/%d faints=%d/%d", board, turns, reserves,
        gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp, gBattleMons[0].hp, gBattleMons[2].hp,
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK], gBattleMons[1].statStages[STAT_SPATK], gBattleMons[3].statStages[STAT_SPATK],
        gBattleMons[1].statStages[STAT_SPEED], gBattleMons[3].statStages[STAT_SPEED], gBattleMons[1].item, gBattleMons[3].item,
        gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
    Test_MgbaPrintf("ABIGAIL_STATE moves=%S/%S PP=%d/%d/%d/%d otherPP=%d/%d/%d/%d partyHP=%d/%d/%d/%d type=%d/%d status=%d/%d",
        GetMoveName(gLastMoves[1] < MOVES_COUNT ? gLastMoves[1] : MOVE_NONE), GetMoveName(gLastMoves[3] < MOVES_COUNT ? gLastMoves[3] : MOVE_NONE),
        gBattleMons[1].pp[0], gBattleMons[1].pp[1], gBattleMons[1].pp[2], gBattleMons[1].pp[3],
        gBattleMons[3].pp[0], gBattleMons[3].pp[1], gBattleMons[3].pp[2], gBattleMons[3].pp[3],
        GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP), GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
        GetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_HP), GetMonData(&gParties[B_TRAINER_OPPONENT_A][3], MON_DATA_HP),
        gBattleMons[1].types[0], gBattleMons[3].types[0], gBattleMons[1].status1, gBattleMons[3].status1);
    Test_MgbaPrintf("ABIGAIL_FOES status=%d/%d SpA=%d/%d libero=%d/%d", gBattleMons[0].status1, gBattleMons[2].status1,
        gBattleMons[0].statStages[STAT_SPATK], gBattleMons[2].statStages[STAT_SPATK],
        gBattleMons[1].volatiles.usedProteanLibero, gBattleMons[3].volatiles.usedProteanLibero);
}

AI_DOUBLE_BATTLE_TEST("EC live review Abigail core: pressure and promoted reserves")
{
    u32 board = 0, turns = 0; bool32 reserves = FALSE;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= (!p && b == 0 ? 4 : 3); t++)
        PARAMETRIZE { reserves = p; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        AbigailOpponent(reserves);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        AbigailResult(board, turns, reserves);
        if (reserves && (board == 0 || turns >= 2)) {
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
            EXPECT_EQ((u32)opponentLeft->volatiles.usedProteanLibero, TRUE);
            EXPECT_EQ(opponentLeft->types[0], board == 0 ? TYPE_FIGHTING : TYPE_DARK);
        }
        if (!reserves && board == 1) {
            EXPECT_EQ(opponentLeft->species, SPECIES_SCOLIPEDE);
            EXPECT(opponentLeft->statStages[STAT_SPEED] >= DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(opponentLeft->pp[0], 4);
            if (turns == 3) {
                EXPECT_EQ(opponentRight->pp[3], 19);
                EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
            }
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Abigail disruption: flinch and Water pressure")
{
    u32 turns = 0; bool32 reserves = FALSE;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= 2; t++) PARAMETRIZE { reserves = p; turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet mienfoo = sMuseumOneMienfoo; mienfoo.item = ITEM_FOCUS_SASH;
        struct EmeraldChampionsBattleSet lombre = sMuseumOneLotad; lombre.moves[1] = MOVE_SCALD;
        IsabelPlayer(SPECIES_MIENFOO, &mienfoo); IsabelPlayer(SPECIES_LOMBRE, &lombre);
        AbigailOpponent(reserves);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (t == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { AbigailResult(10, turns, reserves); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Abigail counterplay: burn and redirection")
{
    u32 turns = 0; bool32 reserves = FALSE;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= (p ? 4 : 6); t++) PARAMETRIZE { reserves = p; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet sableye = {
            .moves = {MOVE_FOUL_PLAY, MOVE_WILL_O_WISP, MOVE_TAUNT, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER, .evs = {252, 0, 252, 0, 4, 0},
        };
        IsabelPlayer(SPECIES_SABLEYE, &sableye); IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        AbigailOpponent(reserves);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (t == 0) MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentLeft, hit: TRUE);
            else MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } THEN { AbigailResult(20, turns, reserves); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Abigail Ground: four-party transitions")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 5; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet mudbray = {
            .moves = {MOVE_EARTHQUAKE, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_STAMINA, .evs = {252, 252, 4, 0, 0, 0},
        };
        IsabelPlayer(SPECIES_MUDBRAY, &mudbray); IsabelPlayer(SPECIES_XATU, &sMuseumOneNatu);
        AbigailOpponent(FALSE);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { AbigailResult(30, turns, FALSE); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Abigail timing: complete six-player party")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) IsabelPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]]);
        AbigailOpponent(FALSE);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
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
        Test_MgbaPrintf("ABIGAIL_TIMING setup=%d bothActors=%d complete=%d warm=%d", setup, complete - setup, complete, gMain.vblankCounter1 - warmStart);
        EXPECT(complete <= 72);
    }
}
