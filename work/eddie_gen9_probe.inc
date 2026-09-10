// Temporary E0056 native review; reuse level30 native player construction.
#include "isabel_gen9_probe.inc"
#line 58000 "work/eddie_gen9_probe.inc"

static void EddieOpponent(u32 pair)
{
    AuthoredOpponent(TRAINER_EDDIE, 2, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species species[] = {SPECIES_DUGTRIO, SPECIES_MISDREAVUS, SPECIES_CARNIVINE, SPECIES_GLIGAR};
    for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
        EXPECT_EQ(GetMonData(&party[i], MON_DATA_SPECIES), species[i]);
        Test_MgbaPrintf("EDDIE_INPUT slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d", i,
            GetMonData(&party[i], MON_DATA_LEVEL), GetMonData(&party[i], MON_DATA_HP), GetMonData(&party[i], MON_DATA_ATK),
            GetMonData(&party[i], MON_DATA_DEF), GetMonData(&party[i], MON_DATA_SPATK), GetMonData(&party[i], MON_DATA_SPDEF),
            GetMonData(&party[i], MON_DATA_SPEED), GetMonData(&party[i], MON_DATA_HELD_ITEM), GetMonAbility(&party[i]));
    }
    if (pair == 1) {
        u16 zero = 0;
        for (u32 i = 0; i < 2; i++) {
            SetMonData(&party[i], MON_DATA_HP, &zero);
            struct Pokemon saved = party[i]; party[i] = party[i + 2]; party[i + 2] = saved;
        }
    } else if (pair == 2) {
        struct Pokemon saved = party[1]; party[1] = party[2]; party[2] = saved;
        u16 zero = 0; SetMonData(&party[2], MON_DATA_HP, &zero); SetMonData(&party[3], MON_DATA_HP, &zero);
    }
}

static void EddieResult(u32 board, u32 turns, u32 pair)
{
    Test_MgbaPrintf("EDDIE_AFTER board=%d turns=%d pair=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speed=%d/%d foeSpeed=%d/%d items=%d/%d foeItems=%d/%d status=%d/%d foeStatus=%d/%d PP=%d/%d secondPP=%d/%d thirdPP=%d/%d guardPP=%d/%d faints=%d/%d",
        board, turns, pair, gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[0].hp, gBattleMons[2].hp, GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].item, gBattleMons[3].item, gBattleMons[0].item, gBattleMons[2].item,
        gBattleMons[1].status1, gBattleMons[3].status1, gBattleMons[0].status1, gBattleMons[2].status1,
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleMons[1].pp[1], gBattleMons[3].pp[1],
        gBattleMons[1].pp[2], gBattleMons[3].pp[2], gBattleMons[1].pp[3], gBattleMons[3].pp[3],
        gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}

AI_DOUBLE_BATTLE_TEST("EC live review Eddie core: physical special leads and reserves")
{
    u32 board = 0, turns = 0, pair = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (p == 0 && b == 0 ? 5 : 3); t++) PARAMETRIZE { pair = p; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        if (board == 0) {
            struct EmeraldChampionsBattleSet set = sMuseumOneTimburr;
            set.moves[1] = MOVE_KNOCK_OFF;
            IsabelPlayer(SPECIES_GURDURR, &set);
        } else IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        EddieOpponent(pair);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_KNOCK_OFF, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } SCENE {
        if (pair == 0 && board == 0 && turns >= 3) ANIMATION(ANIM_TYPE_MOVE, MOVE_RAGE_POWDER, opponentLeft);
        if (pair == 0 && board == 0 && turns >= 4) ANIMATION(ANIM_TYPE_MOVE, MOVE_WILL_O_WISP, opponentRight);
        if (pair == 0 && board == 1 && turns == 3) ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, opponentLeft);
        if (pair == 1 && board == 1) ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentLeft);
    } THEN {
        EddieResult(board, turns, pair);
        if (pair == 0 && board == 0 && turns >= 4) {
            EXPECT_EQ(playerRight->status1, STATUS1_BURN);
            EXPECT_EQ(opponentRight->hp, 28);
            EXPECT_EQ(opponentRight->pp[2], 14);
        }
        if (pair == 0 && board == 1 && turns == 3) EXPECT_EQ(opponentRight->hp, 43);
        if (pair == 1 && board == 1 && turns == 1) {
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
            EXPECT_EQ(opponentRight->hp, 16);
            EXPECT_EQ(playerLeft->hp, 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Eddie disruption: Fake Out Taunt or two Ground immunities")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= 3; t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        if (board == 0) {
            struct EmeraldChampionsBattleSet set = sMuseumOneMienfoo; set.item = ITEM_FOCUS_SASH;
            IsabelPlayer(SPECIES_MIENFOO, &set);
            IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        } else {
            const struct EmeraldChampionsBattleSet misdreavus = {
                .moves = {MOVE_SHADOW_BALL, MOVE_THUNDERBOLT, MOVE_DAZZLING_GLEAM, MOVE_PROTECT},
                .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_LEVITATE,
                .evs = {4, 0, 0, 252, 0, 252},
            };
            IsabelPlayer(SPECIES_XATU, &sMuseumOneNatu);
            IsabelPlayer(SPECIES_MISDREAVUS, &misdreavus);
        }
        EddieOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                if (turn == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else if (turn == 1) MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_FOLLOW_ME);
            } else {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_SHADOW_BALL, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        EddieResult(10 + board, turns, 0);
        if (board == 1) EXPECT_EQ(opponentLeft->pp[0], 10);
        if (board == 1 && turns == 1) {
            EXPECT_EQ(IsAbilityPreventingEscape(B_BATTLER_0), 0);
            EXPECT_EQ(IsAbilityPreventingEscape(B_BATTLER_2), 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Eddie support: mixed safe spread under Water and Grass pressure")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= 2; t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet water = {
            .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_PROTECT, MOVE_RAIN_DANCE},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST,
            .ability = board == 0 ? ABILITY_TORRENT : ABILITY_RAIN_DISH,
            .evs = {252, 0, 4, 252, 0, 0},
        };
        IsabelPlayer(board == 0 ? SPECIES_WARTORTLE : SPECIES_LOMBRE, &water);
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        EddieOpponent(2);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_HYDRO_PUMP, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, opponentLeft);
    } THEN {
        EddieResult(20 + board, turns, 2);
        if (turns == 1) {
            EXPECT_EQ(IsAbilityPreventingEscape(B_BATTLER_0), B_BATTLER_1 + 1);
            EXPECT_EQ(IsAbilityPreventingEscape(B_BATTLER_2), B_BATTLER_1 + 1);
            EXPECT_EQ(opponentRight->hp, 93);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Eddie timing: complete cold six-player party")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) IsabelPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]]);
        AuthoredOpponent(TRAINER_EDDIE, 2, FALSE);
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
        Test_MgbaPrintf("EDDIE_TIMING setup=%d bothActors=%d complete=%d warm=%d", setup, complete - setup, complete, gMain.vblankCounter1 - warmStart);
    }
}
