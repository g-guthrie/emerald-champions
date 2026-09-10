// Temporary Jaclyn authoring evidence; actual parties and unforced NPC moves.
#include "isabel_gen9_probe.inc"
#line 4 "work/jaclyn_gen9_probe.inc"

static void JaclynOpponent(bool32 reserves)
{
    AuthoredOpponent(TRAINER_JACLYN, 2, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species species[] = {SPECIES_WOBBUFFET, SPECIES_SWOOBAT, SPECIES_KADABRA, SPECIES_MUSHARNA};
    for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
        EXPECT_EQ(GetMonData(&party[i], MON_DATA_SPECIES), species[i]);
        Test_MgbaPrintf("JACLYN_INPUT slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d", i,
            GetMonData(&party[i], MON_DATA_LEVEL), GetMonData(&party[i], MON_DATA_HP), GetMonData(&party[i], MON_DATA_ATK),
            GetMonData(&party[i], MON_DATA_DEF), GetMonData(&party[i], MON_DATA_SPATK), GetMonData(&party[i], MON_DATA_SPDEF),
            GetMonData(&party[i], MON_DATA_SPEED), GetMonData(&party[i], MON_DATA_HELD_ITEM), GetMonAbility(&party[i]));
    }
    if (reserves) {
        u16 zero = 0;
        for (u32 i = 0; i < 2; i++) {
            struct Pokemon mon = party[i]; party[i] = party[i + 2]; party[i + 2] = mon;
            SetMonData(&party[i + 2], MON_DATA_HP, &zero);
        }
    }
}

static void JaclynResult(u32 board, u32 turns, bool32 reserves)
{
    Test_MgbaPrintf("JACLYN_AFTER board=%d turns=%d reserves=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speed=%d/%d foeSpeed=%d/%d items=%d/%d PP=%d/%d secondPP=%d/%d thirdPP=%d/%d fourthPP=%d/%d stages=%d/%d/%d/%d foeStatus=%d/%d faints=%d/%d partyHP=%d/%d/%d/%d",
        board, turns, reserves, gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1] < MOVES_COUNT ? gLastMoves[1] : MOVE_NONE), GetMoveName(gLastMoves[3] < MOVES_COUNT ? gLastMoves[3] : MOVE_NONE),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].item, gBattleMons[3].item,
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleMons[1].pp[1], gBattleMons[3].pp[1],
        gBattleMons[1].pp[2], gBattleMons[3].pp[2], gBattleMons[1].pp[3], gBattleMons[3].pp[3],
        gBattleMons[1].statStages[STAT_SPATK], gBattleMons[1].statStages[STAT_SPDEF],
        gBattleMons[3].statStages[STAT_SPATK], gBattleMons[3].statStages[STAT_SPDEF],
        gBattleMons[0].status1, gBattleMons[2].status1, gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter,
        GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP), GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
        GetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_HP), GetMonData(&gParties[B_TRAINER_OPPONENT_A][3], MON_DATA_HP));
    Test_MgbaPrintf("JACLYN_CONTROL foeEncore=%d/%d foeEncoredMove=%d/%d taunt=%d/%d",
        gBattleMons[0].volatiles.encoreTimer, gBattleMons[2].volatiles.encoreTimer,
        gBattleMons[0].volatiles.encoredMove, gBattleMons[2].volatiles.encoredMove,
        gBattleMons[1].volatiles.tauntTimer, gBattleMons[3].volatiles.tauntTimer);
}

AI_DOUBLE_BATTLE_TEST("EC live review Jaclyn core: physical special and reserves")
{
    u32 board = 0, turns = 0;
    bool32 reserves = FALSE;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= (p == 0 && b == 0 ? 3 : 4); t++)
        PARAMETRIZE { reserves = p; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        JaclynOpponent(reserves);
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
    } SCENE {
        if (!reserves && board == 1 && turns >= 2) ANIMATION(ANIM_TYPE_MOVE, MOVE_CALM_MIND, opponentRight);
        if (!reserves && board == 1 && turns >= 3) ANIMATION(ANIM_TYPE_MOVE, MOVE_STORED_POWER, opponentRight);
        if (!reserves && board == 0 && turns >= 2) ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentLeft);
    } THEN {
        JaclynResult(board, turns, reserves);
        if (!reserves && board == 1 && turns >= 2) {
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(opponentRight->pp[0], 19);
        }
        if (!reserves && board == 0 && turns == 3) EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Jaclyn disruption: Fake Out Taunt Ice Beam")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 7; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet mienfoo = sMuseumOneMienfoo;
        mienfoo.item = ITEM_FOCUS_SASH;
        IsabelPlayer(SPECIES_MIENFOO, &mienfoo);
        IsabelPlayer(SPECIES_LOMBRE, &sMuseumOneLotad);
        JaclynOpponent(FALSE);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (t == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            else if (t == 1) MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
            else MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_ICE_BEAM, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { JaclynResult(10, turns, FALSE); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Jaclyn setup window: burn and redirection")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 6; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet sableye = {
            .moves = {MOVE_FOUL_PLAY, MOVE_WILL_O_WISP, MOVE_RECOVER, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        IsabelPlayer(SPECIES_SABLEYE, &sableye);
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        JaclynOpponent(FALSE);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (t == 0) MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentLeft, hit: TRUE);
            else MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } THEN {
        JaclynResult(20, turns, FALSE);
        if (turns == 1 && opponentLeft->species == SPECIES_WOBBUFFET) {
            EXPECT(!IsBattlerTrapped(B_BATTLER_1, B_BATTLER_0));
            EXPECT(IsBattlerTrapped(B_BATTLER_1, B_BATTLER_2));
        }
        if (turns >= 2 && turns <= 5) EXPECT_EQ(opponentLeft->pp[2], 4);
        if (turns == 2) {
            EXPECT_EQ((u32)playerRight->volatiles.encoredMove, MOVE_FOLLOW_ME);
            EXPECT_EQ((u32)playerRight->volatiles.encoreTimer, 3);
        }
        if (turns == 6) {
            EXPECT_EQ(opponentRight->species, SPECIES_MUSHARNA);
            EXPECT_EQ(opponentRight->pp[2], 9);
            EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Jaclyn special setup: bulk before Stored Power")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 4; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        IsabelPlayer(SPECIES_LOMBRE, &sMuseumOneLotad);
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        JaclynOpponent(FALSE);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (turns >= 2) ANIMATION(ANIM_TYPE_MOVE, MOVE_CALM_MIND, opponentRight);
        if (turns >= 3) ANIMATION(ANIM_TYPE_MOVE, MOVE_STORED_POWER, opponentRight);
        if (turns == 4) ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentLeft);
    } THEN {
        JaclynResult(25, turns, FALSE);
        if (turns >= 2) {
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(opponentRight->pp[0], 19);
        }
        if (turns == 4) {
            EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
            EXPECT_EQ(gBattleResults.opponentFaintCounter, 0);
            EXPECT_EQ(opponentRight->hp, 41);
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Jaclyn counterplay: Dark Ghost pressure")
{
    u32 turns = 0;
    bool32 reserves = FALSE;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= (p == 0 ? 4 : 5); t++) PARAMETRIZE { reserves = p; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet sableye = {
            .moves = {MOVE_FOUL_PLAY, MOVE_WILL_O_WISP, MOVE_TAUNT, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        const struct EmeraldChampionsBattleSet misdreavus = {
            .moves = {MOVE_SHADOW_BALL, MOVE_THUNDERBOLT, MOVE_DAZZLING_GLEAM, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_LEVITATE,
            .evs = {4, 0, 0, 252, 0, 252},
        };
        IsabelPlayer(SPECIES_SABLEYE, &sableye);
        IsabelPlayer(SPECIES_MISDREAVUS, &misdreavus);
        JaclynOpponent(reserves);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_SHADOW_BALL, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { JaclynResult(30, turns, reserves); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Jaclyn timing: complete six-player party")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) IsabelPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]]);
        JaclynOpponent(FALSE);
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
        Test_MgbaPrintf("JACLYN_TIMING setup=%d bothActors=%d complete=%d warm=%d", setup, complete - setup, complete, gMain.vblankCounter1 - warmStart);
        EXPECT(complete <= 72);
    }
}
