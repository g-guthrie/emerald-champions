// Temporary E0071 native review; no permanent trainer-specific suite.
#include "isabel_gen9_probe.inc"
#line 60000 "work/isaac_gen9_probe.inc"

static void IsaacOpponent(u32 reserves)
{
    AuthoredOpponent(TRAINER_ISAAC_1, 2, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species species[] = {SPECIES_MUNCHLAX, SPECIES_TOGEPI, SPECIES_ABRA, SPECIES_DRATINI, SPECIES_STARYU, SPECIES_KANGASKHAN};
    for (u32 i = 0; i < 6; i++) {
        EXPECT_EQ(GetMonData(&party[i], MON_DATA_SPECIES), species[i]);
        Test_MgbaPrintf("ISAAC_1_INPUT slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Spe=%d item=%d ability=%d", i,
            GetMonData(&party[i], MON_DATA_LEVEL), GetMonData(&party[i], MON_DATA_HP), GetMonData(&party[i], MON_DATA_ATK),
            GetMonData(&party[i], MON_DATA_DEF), GetMonData(&party[i], MON_DATA_SPATK), GetMonData(&party[i], MON_DATA_SPDEF),
            GetMonData(&party[i], MON_DATA_SPEED), GetMonData(&party[i], MON_DATA_HELD_ITEM), GetMonAbility(&party[i]));
    }
    if (reserves) {
        u32 pair = reserves >= 3 ? 2 : reserves;
        for (u32 i = 0; i < 2; i++) {
            struct Pokemon mon = party[i]; party[i] = party[pair * 2 + i]; party[pair * 2 + i] = mon;
        }
        if (reserves != 3) for (u32 i = 2; i < 6; i++) { u16 zero = 0; SetMonData(&party[i], MON_DATA_HP, &zero); }
    }
}

static void IsaacTiming(u32 board)
{
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
    Test_MgbaPrintf("ISAAC_1_TIMING board=%d setup=%d bothActors=%d complete=%d warm=%d", board, setup, complete - setup, complete, gMain.vblankCounter1 - warmStart);
    EXPECT(complete <= 72);
}

static void IsaacResult(u32 board, u32 turns, u32 reserves)
{
    Test_MgbaPrintf("ISAAC_NATIVE_TIMER=%d", gBattleStruct->aiDelayFrames);
    Test_MgbaPrintf("ISAAC_FORM_STATS HP=%d/%d Atk=%d/%d Def=%d/%d SpD=%d/%d speed=%d/%d", gBattleMons[1].maxHP, gBattleMons[3].maxHP, gBattleMons[1].attack, gBattleMons[3].attack, gBattleMons[1].defense, gBattleMons[3].defense, gBattleMons[1].spDefense, gBattleMons[3].spDefense, gBattleMons[1].speed, gBattleMons[3].speed);
    Test_MgbaPrintf("ISAAC_1_AFTER board=%d turns=%d reserves=%d species=%d/%d HP=%d/%d foes=%d/%d Atk=%d/%d SpA=%d/%d Spe=%d/%d item=%d/%d faints=%d/%d", board, turns, reserves,
        gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp, gBattleMons[0].hp, gBattleMons[2].hp,
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK], gBattleMons[1].statStages[STAT_SPATK], gBattleMons[3].statStages[STAT_SPATK],
        gBattleMons[1].statStages[STAT_SPEED], gBattleMons[3].statStages[STAT_SPEED], gBattleMons[1].item, gBattleMons[3].item,
        gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
    Test_MgbaPrintf("ISAAC_1_STATE moves=%S/%S PP=%d/%d/%d/%d otherPP=%d/%d/%d/%d partyHP=%d/%d/%d/%d type=%d/%d status=%d/%d",
        GetMoveName(gLastMoves[1] < MOVES_COUNT ? gLastMoves[1] : MOVE_NONE), GetMoveName(gLastMoves[3] < MOVES_COUNT ? gLastMoves[3] : MOVE_NONE),
        gBattleMons[1].pp[0], gBattleMons[1].pp[1], gBattleMons[1].pp[2], gBattleMons[1].pp[3],
        gBattleMons[3].pp[0], gBattleMons[3].pp[1], gBattleMons[3].pp[2], gBattleMons[3].pp[3],
        GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP), GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
        GetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_HP), GetMonData(&gParties[B_TRAINER_OPPONENT_A][3], MON_DATA_HP),
        gBattleMons[1].types[0], gBattleMons[3].types[0], gBattleMons[1].status1, gBattleMons[3].status1);
    Test_MgbaPrintf("ISAAC_1_FIELD foeStatus=%d/%d seed=%d/%d substitute=%d/%d rocks=%d tailwind=%d/%d ability=%d/%d", gBattleMons[0].status1, gBattleMons[2].status1, gBattleMons[0].volatiles.leechSeed, gBattleMons[2].volatiles.leechSeed, gBattleMons[1].volatiles.substituteHP, gBattleMons[3].volatiles.substituteHP, IsHazardOnSide(B_SIDE_PLAYER, HAZARDS_STEALTH_ROCK), gSideTimers[0].tailwindTimer, gSideTimers[1].tailwindTimer, GetBattlerAbility(1), GetBattlerAbility(3));
}

AI_DOUBLE_BATTLE_TEST("EC live review Isaac core: pressure and promoted reserves")
{
    u32 board = 0, turns = 0; u32 reserves = 0;
    for (u32 p = 0; p < 3; p++) for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= (p == 2 ? (b == 0 ? 3 : 2) : (p == 1 && b == 0 ? 3 : 5)); t++)
        PARAMETRIZE { reserves = p; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        IsaacOpponent(reserves);
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
        IsaacResult(board, turns, reserves);
        if (reserves == 2 && turns == 1) {
            EXPECT_EQ(opponentRight->species, SPECIES_KANGASKHAN_MEGA);
            EXPECT_EQ(GetBattlerAbility(3), ABILITY_PARENTAL_BOND);
            EXPECT_EQ(opponentRight->speed, 83);
        }
        if (reserves == 0 && board == 0 && turns >= 2) {
            EXPECT_EQ(opponentLeft->species, SPECIES_MUNCHLAX);
            EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(opponentLeft->pp[3], 9);
            EXPECT(opponentLeft->pp[0] < 15);
            EXPECT(opponentRight->pp[3] < 20);
        }
        if (turns == 1) IsaacTiming(board + 10 * reserves);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Isaac disruption: flinch and Water pressure")
{
    u32 turns = 0; u32 reserves = 0;
    for (u32 p = 0; p < 3; p++) for (u32 t = 1; t <= (p == 2 ? 3 : (p ? 4 : 5)); t++) PARAMETRIZE { reserves = p; turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet mienfoo = sMuseumOneMienfoo; mienfoo.item = ITEM_FOCUS_SASH;
        struct EmeraldChampionsBattleSet lombre = sMuseumOneLotad; lombre.moves[1] = MOVE_SCALD;
        IsabelPlayer(SPECIES_MIENFOO, &mienfoo); IsabelPlayer(SPECIES_LOMBRE, &lombre);
        IsaacOpponent(reserves);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (t == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { IsaacResult(10, turns, reserves); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Isaac counterplay: burn and redirection")
{
    u32 turns = 0; u32 reserves = 0;
    for (u32 p = 0; p < 3; p++) for (u32 t = 1; t <= (p == 2 ? 3 : 5); t++) PARAMETRIZE { reserves = p; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet sableye = {
            .moves = {MOVE_FOUL_PLAY, MOVE_WILL_O_WISP, MOVE_TAUNT, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER, .evs = {252, 0, 252, 0, 4, 0},
        };
        IsabelPlayer(SPECIES_SABLEYE, &sableye); IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        IsaacOpponent(reserves);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (t == 0) MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentLeft, hit: TRUE);
            else MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } THEN { IsaacResult(20, turns, reserves); }
}


AI_DOUBLE_BATTLE_TEST("EC live review Isaac timing: six-player party")
{
    u32 pair = 0;
    PARAMETRIZE { pair = 0; }
    PARAMETRIZE { pair = 3; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) IsabelPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]]);
        IsaacOpponent(pair);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        IsaacResult(99, 1, pair);
        // Existing native timer includes initial data setup and both actors,
        // including the Mega alternatives, not only the post-Mega warm state.
        EXPECT(gBattleStruct->aiDelayFrames <= 72);
        IsaacTiming(99 + pair);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Isaac denial: Taunt and spread Ground")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= (b ? 3 : 4); t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet lombre = sMuseumOneLotad; lombre.moves[1] = MOVE_SCALD;
        const struct EmeraldChampionsBattleSet mudbray = {
            .moves = {MOVE_EARTHQUAKE, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_STAMINA, .evs = {252, 252, 4, 0, 0, 0},
        };
        IsabelPlayer(board ? SPECIES_MUDBRAY : SPECIES_MIENFOO, board ? &mudbray : &sMuseumOneMienfoo);
        IsabelPlayer(board ? SPECIES_XATU : SPECIES_LOMBRE, board ? &sMuseumOneNatu : &lombre);
        IsaacOpponent(0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board) {
                MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, MOVE_TAUNT, target: opponentRight, hit: TRUE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN { IsaacResult(40 + board, turns, 0); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Isaac dragon support: healthy repositioned pair")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 4; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet lombre = sMuseumOneLotad; lombre.moves[1] = MOVE_SCALD;
        IsabelPlayer(SPECIES_LOMBRE, &lombre); IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        IsaacOpponent(0);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        struct Pokemon mon = party[0]; party[0] = party[3]; party[3] = mon;
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_SCALD, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        IsaacResult(50, turns, 0);
        EXPECT_EQ(opponentLeft->species, SPECIES_DRATINI);
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentLeft->pp[0], 19);
        EXPECT_EQ(GetBattlerTotalSpeedStat(1, GetBattlerAbility(1), GetBattlerHoldEffect(1)), 94);
        if (turns >= 2) { EXPECT(opponentLeft->pp[2] < 15); EXPECT(opponentRight->pp[3] < 20); }
    }
}
