// Temporary E0054 Kaleb review. Reuse the native level30 player helper;
// earlier review groups are not selected by this prefix.
#include "isabel_gen9_probe.inc"
#line 56000 "work/kaleb_gen9_probe.inc"

static void KalebOpponent(u32 pair)
{
    AuthoredOpponent(TRAINER_KALEB, 2, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species species[] = {SPECIES_AUDINO, SPECIES_GRANBULL, SPECIES_BOLTUND};
    for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
        EXPECT_EQ(GetMonData(&party[i], MON_DATA_SPECIES), species[i]);
        Test_MgbaPrintf("KALEB_INPUT slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d", i,
            GetMonData(&party[i], MON_DATA_LEVEL), GetMonData(&party[i], MON_DATA_HP), GetMonData(&party[i], MON_DATA_ATK),
            GetMonData(&party[i], MON_DATA_DEF), GetMonData(&party[i], MON_DATA_SPATK), GetMonData(&party[i], MON_DATA_SPDEF),
            GetMonData(&party[i], MON_DATA_SPEED), GetMonData(&party[i], MON_DATA_HELD_ITEM), GetMonAbility(&party[i]));
    }
    if (pair == 1) {
        struct Pokemon saved = party[1]; party[1] = party[2]; party[2] = saved;
    } else if (pair == 2) {
        struct Pokemon saved = party[0]; party[0] = party[1]; party[1] = party[2]; party[2] = saved;
    }
    if (pair == 1 || pair == 2) {
        u16 zero = 0; SetMonData(&party[2], MON_DATA_HP, &zero);
    } else if (pair == 3) {
        u16 hp = GetMonData(&party[1], MON_DATA_MAX_HP) / 3;
        SetMonData(&party[1], MON_DATA_HP, &hp);
    }
}

static void KalebResult(u32 board, u32 turns, u32 pair)
{
    Test_MgbaPrintf("KALEB_AFTER board=%d turns=%d pair=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speed=%d/%d foeSpeed=%d/%d stageAtk=%d/%d foeAtk=%d/%d items=%d/%d PP=%d/%d healPP=%d/%d supportPP=%d/%d guardPP=%d/%d faints=%d/%d",
        board, turns, pair, gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[0].hp, gBattleMons[2].hp, GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK],
        gBattleMons[0].statStages[STAT_ATK], gBattleMons[2].statStages[STAT_ATK],
        gBattleMons[1].item, gBattleMons[3].item, gBattleMons[1].pp[0], gBattleMons[3].pp[0],
        gBattleMons[1].pp[1], gBattleMons[3].pp[1], gBattleMons[1].pp[2], gBattleMons[3].pp[2],
        gBattleMons[1].pp[3], gBattleMons[3].pp[3], gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}

AI_DOUBLE_BATTLE_TEST("EC live review Kaleb core: actual lead and both survivor pairings")
{
    u32 pair = 0, board = 0, turns = 0;
    for (u32 p = 0; p < 3; p++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (p == 2 && b == 1 ? 2 : 3); t++) PARAMETRIZE { pair = p; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        KalebOpponent(pair);
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
        if (pair == 0 && board == 1 && turns == 2) ANIMATION(ANIM_TYPE_MOVE, MOVE_LIFE_DEW, opponentLeft);
        if (pair == 0 && board == 0 && turns == 2) ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentLeft);
    } THEN {
        KalebResult(board, turns, pair);
        if (pair == 0 && board == 0 && turns == 1) {
            EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
            EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        }
        if (pair == 0 && board == 1 && turns == 2) {
            EXPECT_EQ(opponentRight->hp, 100);
            EXPECT_EQ(opponentLeft->pp[1], 9);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Kaleb disruption: Fake Out Taunt or Ground pressure")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= 3; t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        if (board == 0) {
            struct EmeraldChampionsBattleSet set = sMuseumOneMienfoo;
            set.item = ITEM_FOCUS_SASH;
            IsabelPlayer(SPECIES_MIENFOO, &set);
        }
        else {
            const struct EmeraldChampionsBattleSet mudbray = {
                .moves = {MOVE_HIGH_HORSEPOWER, MOVE_HEAVY_SLAM, MOVE_ROCK_SLIDE, MOVE_PROTECT},
                .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_STAMINA,
                .evs = {252, 252, 4, 0, 0, 0},
            };
            IsabelPlayer(SPECIES_MUDBRAY, &mudbray);
        }
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        KalebOpponent(board == 0 ? 0 : 1);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                if (turn == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else if (turn == 1) MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            } else MOVE(playerLeft, MOVE_HIGH_HORSEPOWER, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (board == 0 && turns >= 2) ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, playerLeft);
    } THEN { KalebResult(10 + board, turns, board == 0 ? 0 : 1); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Kaleb sustain: injured Granbull alongside Audino")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 5; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        IsabelPlayer(SPECIES_LOMBRE, &sMuseumOneLotad);
        KalebOpponent(3);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentRight, hit: TRUE, criticalHit: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LIFE_DEW, opponentLeft);
        if (turns >= 3) ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentLeft);
    } THEN {
        KalebResult(20, turns, 3);
        if (turns == 1) {
            EXPECT_EQ(opponentLeft->hp, 122);
            EXPECT_EQ(opponentRight->hp, 94);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Kaleb timing: complete cold six-player party")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) IsabelPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]]);
        AuthoredOpponent(TRAINER_KALEB, 2, FALSE);
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
        Test_MgbaPrintf("KALEB_TIMING setup=%d bothActors=%d complete=%d warm=%d", setup, complete - setup, complete, gMain.vblankCounter1 - warmStart);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Kaleb reserve entry: Specs Psychic and Super Fang pressure")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 4; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet xatu = sMuseumOneNatu;
        xatu.item = ITEM_CHOICE_SPECS;
        IsabelPlayer(SPECIES_XATU, &xatu);
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        KalebOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE);
        }
    } THEN {
        KalebResult(30, turns, 0);
        if (turns >= 3) EXPECT_EQ(opponentRight->species, SPECIES_BOLTUND);
        Test_MgbaPrintf("KALEB_BENCH species=%d HP=%d/%d", GetMonData(&gEnemyParty[0], MON_DATA_SPECIES),
            GetMonData(&gEnemyParty[0], MON_DATA_HP), GetMonData(&gEnemyParty[0], MON_DATA_MAX_HP));
    }
}
