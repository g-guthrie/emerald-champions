// Temporary E0064 native authoring scenarios, excluded after review.
#include "isabel_gen9_probe.inc"
#line 4 "work/anthony_gen9_probe.inc"

static void AnthonyOpponent(bool32 reserves)
{
    AuthoredOpponent(TRAINER_ANTHONY, 2, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species species[] = {SPECIES_DODUO, SPECIES_ZEBSTRIKA, SPECIES_LINOONE, SPECIES_MAGNETON};
    for (u32 i = 0; i < 4; i++) {
        EXPECT_EQ(GetMonData(&party[i], MON_DATA_SPECIES), species[i]);
        Test_MgbaPrintf("ANTHONY_INPUT slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Spe=%d item=%d ability=%d", i,
            GetMonData(&party[i], MON_DATA_LEVEL), GetMonData(&party[i], MON_DATA_HP), GetMonData(&party[i], MON_DATA_ATK),
            GetMonData(&party[i], MON_DATA_DEF), GetMonData(&party[i], MON_DATA_SPATK), GetMonData(&party[i], MON_DATA_SPDEF),
            GetMonData(&party[i], MON_DATA_SPEED), GetMonData(&party[i], MON_DATA_HELD_ITEM), GetMonAbility(&party[i]));
    }
    if (reserves) for (u32 i = 0; i < 2; i++) {
        struct Pokemon mon = party[i]; party[i] = party[i + 2]; party[i + 2] = mon;
        u16 zero = 0; SetMonData(&party[i + 2], MON_DATA_HP, &zero);
    }
}

static void AnthonyResult(u32 board, u32 turns, bool32 reserves)
{
    Test_MgbaPrintf("ANTHONY_AFTER board=%d turns=%d reserves=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S AtkStage=%d/%d SpAStage=%d/%d items=%d/%d faints=%d/%d",
        board, turns, reserves, gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1] < MOVES_COUNT ? gLastMoves[1] : MOVE_NONE), GetMoveName(gLastMoves[3] < MOVES_COUNT ? gLastMoves[3] : MOVE_NONE),
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK], gBattleMons[1].statStages[STAT_SPATK], gBattleMons[3].statStages[STAT_SPATK],
        gBattleMons[1].item, gBattleMons[3].item, gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
    Test_MgbaPrintf("ANTHONY_STATE PP=%d/%d/%d/%d otherPP=%d/%d/%d/%d partyHP=%d/%d/%d/%d status=%d/%d taunt=%d/%d",
        gBattleMons[1].pp[0], gBattleMons[1].pp[1], gBattleMons[1].pp[2], gBattleMons[1].pp[3],
        gBattleMons[3].pp[0], gBattleMons[3].pp[1], gBattleMons[3].pp[2], gBattleMons[3].pp[3],
        GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP), GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
        GetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_HP), GetMonData(&gParties[B_TRAINER_OPPONENT_A][3], MON_DATA_HP),
        gBattleMons[1].status1, gBattleMons[3].status1, gBattleMons[1].volatiles.tauntTimer, gBattleMons[3].volatiles.tauntTimer);
}

AI_DOUBLE_BATTLE_TEST("EC live review Anthony core: pressure and reserves")
{
    u32 board = 0, turns = 0;
    bool32 reserves = FALSE;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= (p && b == 0 ? 4 : 3); t++)
        PARAMETRIZE { reserves = p; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        AnthonyOpponent(reserves);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        AnthonyResult(board, turns, reserves);
        if (reserves && board == 1 && turns >= 2) {
            EXPECT_EQ(opponentLeft->species, SPECIES_LINOONE);
            EXPECT_EQ(opponentLeft->statStages[STAT_ATK], MAX_STAT_STAGE);
            EXPECT_EQ(opponentLeft->pp[0], 9);
            EXPECT_EQ(opponentLeft->item, ITEM_NONE);
            EXPECT_EQ((u32)opponentLeft->volatiles.confusionTimer, 0);
            if (turns == 3) EXPECT_EQ(opponentLeft->pp[1], 3);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Anthony disruption: Fake Out and Ice Beam")
{
    u32 turns = 0;
    bool32 reserves = FALSE;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= (p ? 4 : 3); t++) PARAMETRIZE { reserves = p; turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet mienfoo = sMuseumOneMienfoo;
        mienfoo.item = ITEM_FOCUS_SASH;
        IsabelPlayer(SPECIES_MIENFOO, &mienfoo);
        IsabelPlayer(SPECIES_LOMBRE, &sMuseumOneLotad);
        AnthonyOpponent(reserves);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (t == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else if (t == 1) MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
            else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_ICE_BEAM, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { AnthonyResult(10, turns, reserves); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Anthony counterplay: burn and redirection")
{
    u32 turns = 0;
    bool32 reserves = FALSE;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= (p ? 6 : 5); t++) PARAMETRIZE { reserves = p; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet sableye = {
            .moves = {MOVE_FOUL_PLAY, MOVE_WILL_O_WISP, MOVE_TAUNT, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        IsabelPlayer(SPECIES_SABLEYE, &sableye);
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        AnthonyOpponent(reserves);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (t == 0) MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentLeft, hit: TRUE);
            else MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } THEN { AnthonyResult(20, turns, reserves); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Anthony Sap Sipper: Grass hit and offense")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 2; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        IsabelPlayer(SPECIES_LOMBRE, &sMuseumOneLotad);
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        AnthonyOpponent(FALSE);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_GIGA_DRAIN, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ABILITY_POPUP(opponentRight, ABILITY_SAP_SIPPER);
    } THEN {
        AnthonyResult(30, turns, FALSE);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Anthony Drum opportunity: special support pair")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 2; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet clefairy = {
            .moves = {MOVE_MOONBLAST, MOVE_FOLLOW_ME, MOVE_HELPING_HAND, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_BOLD, .ability = ABILITY_FRIEND_GUARD,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        IsabelPlayer(SPECIES_CLEFAIRY, &clefairy);
        IsabelPlayer(SPECIES_LOMBRE, &sMuseumOneLotad);
        AnthonyOpponent(TRUE);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_MOONBLAST, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentRight, hit: TRUE, criticalHit: FALSE);
        }
    } THEN { AnthonyResult(40, turns, TRUE); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Anthony Ground: actual party and Balloon")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet mudbray = {
            .moves = {MOVE_EARTHQUAKE, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_STAMINA,
            .evs = {252, 252, 4, 0, 0, 0},
        };
        IsabelPlayer(SPECIES_MUDBRAY, &mudbray);
        IsabelPlayer(SPECIES_XATU, &sMuseumOneNatu);
        AnthonyOpponent(FALSE);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        AnthonyResult(50, turns, FALSE);
        EXPECT_EQ(opponentLeft->species, SPECIES_MAGNETON);
        EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
        EXPECT_EQ(opponentLeft->item, ITEM_AIR_BALLOON);
        if (turns >= 2) EXPECT_EQ(opponentLeft->pp[3], 19);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Anthony timing: complete six-player party")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) IsabelPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]]);
        AnthonyOpponent(FALSE);
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
        Test_MgbaPrintf("ANTHONY_TIMING setup=%d bothActors=%d complete=%d warm=%d", setup, complete - setup, complete, gMain.vblankCounter1 - warmStart);
        EXPECT(complete <= 72);
    }
}
