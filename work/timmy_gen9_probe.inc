// Temporary E0058 review; native level30 player generation and preparation checks.
#include "isabel_gen9_probe.inc"
#line 60000 "work/timmy_gen9_probe.inc"

static void TimmyOpponent(bool32 reserves)
{
    AuthoredOpponent(TRAINER_TIMMY, 2, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species species[] = {SPECIES_STUNKY, SPECIES_HELIOLISK, SPECIES_HERDIER, SPECIES_DITTO};
    for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
        EXPECT_EQ(GetMonData(&party[i], MON_DATA_SPECIES), species[i]);
        Test_MgbaPrintf("TIMMY_INPUT slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d", i,
            GetMonData(&party[i], MON_DATA_LEVEL), GetMonData(&party[i], MON_DATA_HP), GetMonData(&party[i], MON_DATA_ATK),
            GetMonData(&party[i], MON_DATA_DEF), GetMonData(&party[i], MON_DATA_SPATK), GetMonData(&party[i], MON_DATA_SPDEF),
            GetMonData(&party[i], MON_DATA_SPEED), GetMonData(&party[i], MON_DATA_HELD_ITEM), GetMonAbility(&party[i]));
    }
    if (reserves) {
        u16 zero = 0;
        for (u32 i = 0; i < 2; i++) {
            SetMonData(&party[i], MON_DATA_HP, &zero);
            struct Pokemon saved = party[i]; party[i] = party[i + 2]; party[i + 2] = saved;
        }
    }
}

static void TimmyResult(u32 board, u32 turns, u32 pair)
{
    Test_MgbaPrintf("TIMMY_AFTER board=%d turns=%d pair=%d species=%d/%d HP=%d/%d maxHP=%d/%d foes=%d/%d moves=%S/%S speed=%d/%d foeSpeed=%d/%d ability=%d/%d items=%d/%d stagesAtk=%d/%d foeAtk=%d/%d PP=%d/%d secondPP=%d/%d thirdPP=%d/%d fourthPP=%d/%d faints=%d/%d",
        board, turns, pair, gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[1].maxHP, gBattleMons[3].maxHP, gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1] < MOVES_COUNT ? gLastMoves[1] : MOVE_NONE), GetMoveName(gLastMoves[3] < MOVES_COUNT ? gLastMoves[3] : MOVE_NONE),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].ability, gBattleMons[3].ability, gBattleMons[1].item, gBattleMons[3].item,
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK],
        gBattleMons[0].statStages[STAT_ATK], gBattleMons[2].statStages[STAT_ATK],
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleMons[1].pp[1], gBattleMons[3].pp[1],
        gBattleMons[1].pp[2], gBattleMons[3].pp[2], gBattleMons[1].pp[3], gBattleMons[3].pp[3],
        gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}

AI_DOUBLE_BATTLE_TEST("EC live review Timmy core: leads reserves and Imposter")
{
    u32 board = 0, turns = 0, pair = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (p == 0 ? (b == 0 ? 6 : 2) : 4); t++) PARAMETRIZE { pair = p; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        TimmyOpponent(pair);
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
        if (pair == 0 && board == 1 && turns == 2) ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentLeft);
    } THEN {
        TimmyResult(board, turns, pair);
        if (pair == 0 && board == 1 && turns == 2) EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
        if (pair == 1 && board == 1) {
            EXPECT_EQ(opponentRight->species, SPECIES_XATU);
            EXPECT_EQ(opponentRight->item, ITEM_CHOICE_SCARF);
            EXPECT_EQ(opponentRight->maxHP, 91);
            EXPECT_EQ(gAiLogicData->speedStats[3], 157);
        }
        if (pair == 0 && board == 0 && turns == 5)
            Test_MgbaPrintf("TIMMY_MANUAL_TRANSFORM choice=%d limitations=%d", gBattleStruct->choicedMove[3], CheckMoveLimitations(3, 0, MOVE_LIMITATIONS_ALL));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Timmy disruption: priority Ground and Water pressure")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (b == 0 ? 5 : b == 1 ? 2 : 3); t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        if (board == 0) IsabelPlayer(SPECIES_MIENFOO, &sMuseumOneMienfoo);
        else if (board == 1) {
            struct EmeraldChampionsBattleSet set = gEmeraldChampionsDefaultBattleSets[SPECIES_MUDBRAY];
            set.moves[0] = MOVE_HIGH_HORSEPOWER;
            IsabelPlayer(SPECIES_MUDBRAY, &set);
        } else {
            struct EmeraldChampionsBattleSet set = gEmeraldChampionsDefaultBattleSets[SPECIES_WARTORTLE];
            set.moves[0] = MOVE_SURF;
            IsabelPlayer(SPECIES_WARTORTLE, &set);
        }
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        TimmyOpponent(FALSE);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                if (turn == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                else if (turn == 1) MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_FOLLOW_ME);
            } else {
                if (board == 1) MOVE(playerLeft, MOVE_HIGH_HORSEPOWER, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_SURF, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } SCENE {
        if (board == 1 && turns == 2) ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentLeft);
    } THEN {
        TimmyResult(10 + board, turns, 0);
        if (board == 1) EXPECT_EQ(opponentRight->pp[2], 19);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Timmy Water immunity: faster Surf")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 2; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet set = {
            .moves = {MOVE_SURF, MOVE_ICE_BEAM, MOVE_PROTECT, MOVE_SHELL_SMASH},
            .item = ITEM_CHOICE_SCARF, .nature = NATURE_TIMID, .ability = ABILITY_TORRENT,
            .evs = {4, 0, 0, 252, 0, 252},
        };
        IsabelPlayer(SPECIES_WARTORTLE, &set);
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        TimmyOpponent(FALSE);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_SURF, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ABILITY_POPUP(opponentRight, ABILITY_DRY_SKIN);
    } THEN {
        TimmyResult(13, turns, 0);
        EXPECT_EQ(opponentRight->hp, turns == 1 ? 73 : 65);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Timmy timing: complete cold six-player party")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) IsabelPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]]);
        AuthoredOpponent(TRAINER_TIMMY, 2, FALSE);
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
        Test_MgbaPrintf("TIMMY_TIMING setup=%d bothActors=%d complete=%d warm=%d", setup, complete - setup, complete, gMain.vblankCounter1 - warmStart);
    }
}
