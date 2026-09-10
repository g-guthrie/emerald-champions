// Temporary E0060 Joseph review; same native construction, separate encounter.
#include "isabel_gen9_probe.inc"
#line 65000 "work/joseph_gen9_probe.inc"

static void JosephOpponent(u32 pair)
{
    AuthoredOpponent(TRAINER_JOSEPH, 2, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species species[] = {SPECIES_TOXTRICITY, SPECIES_LUXRAY, SPECIES_MAGNETON};
    for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
        EXPECT_EQ(GetMonData(&party[i], MON_DATA_SPECIES), species[i]);
        Test_MgbaPrintf("JOSEPH_INPUT slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d", i,
            GetMonData(&party[i], MON_DATA_LEVEL), GetMonData(&party[i], MON_DATA_HP), GetMonData(&party[i], MON_DATA_ATK),
            GetMonData(&party[i], MON_DATA_DEF), GetMonData(&party[i], MON_DATA_SPATK), GetMonData(&party[i], MON_DATA_SPDEF),
            GetMonData(&party[i], MON_DATA_SPEED), GetMonData(&party[i], MON_DATA_HELD_ITEM), GetMonAbility(&party[i]));
    }
    if (pair) {
        u16 zero = 0;
        u32 replaced = pair == 1 ? 1 : 0;
        struct Pokemon saved = party[replaced]; party[replaced] = party[2]; party[2] = saved;
        SetMonData(&party[2], MON_DATA_HP, &zero);
    }
}

static void JosephResult(u32 board, u32 turns, u32 pair)
{
    Test_MgbaPrintf("JOSEPH_AFTER board=%d turns=%d pair=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speed=%d/%d foeSpeed=%d/%d items=%d/%d SpA=%d/%d foeAtk=%d/%d foeSpA=%d/%d PP=%d/%d secondPP=%d/%d thirdPP=%d/%d fourthPP=%d/%d faints=%d/%d",
        board, turns, pair, gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1] < MOVES_COUNT ? gLastMoves[1] : MOVE_NONE), GetMoveName(gLastMoves[3] < MOVES_COUNT ? gLastMoves[3] : MOVE_NONE),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].item, gBattleMons[3].item, gBattleMons[1].statStages[STAT_SPATK], gBattleMons[3].statStages[STAT_SPATK],
        gBattleMons[0].statStages[STAT_ATK], gBattleMons[2].statStages[STAT_ATK],
        gBattleMons[0].statStages[STAT_SPATK], gBattleMons[2].statStages[STAT_SPATK],
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleMons[1].pp[1], gBattleMons[3].pp[1],
        gBattleMons[1].pp[2], gBattleMons[3].pp[2], gBattleMons[1].pp[3], gBattleMons[3].pp[3],
        gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
    Test_MgbaPrintf("JOSEPH_FOE board=%d turns=%d pair=%d status=%d/%d firstPP=%d/%d moves=%S/%S", board, turns, pair,
        gBattleMons[0].status1, gBattleMons[2].status1, gBattleMons[0].pp[0], gBattleMons[2].pp[0],
        GetMoveName(gLastMoves[0] < MOVES_COUNT ? gLastMoves[0] : MOVE_NONE), GetMoveName(gLastMoves[2] < MOVES_COUNT ? gLastMoves[2] : MOVE_NONE));
}

AI_DOUBLE_BATTLE_TEST("EC live review Joseph core: physical special and reserve offense")
{
    u32 board = 0, turns = 0, pair = 0;
    for (u32 p = 0; p < 3; p++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (p != 0 && b == 0 ? 4 : 3); t++) PARAMETRIZE { pair = p; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        JosephOpponent(pair);
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
        if (pair == 0 && board == 0 && turns >= 2) ANIMATION(ANIM_TYPE_MOVE, MOVE_OVERDRIVE, opponentLeft);
        if (pair != 1 && board == 1 && turns == 3) ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentRight);
    } THEN {
        JosephResult(board, turns, pair);
        if (pair == 0 && board == 0 && turns >= 2) {
            EXPECT_EQ(opponentLeft->item, ITEM_NONE);
            EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        }
        if ((pair == 0 || pair == 2) && turns == 1) EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Joseph Ground: native earthquake and balloon reserve")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 4; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet mudbray = {
            .moves = {MOVE_EARTHQUAKE, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_STAMINA,
            .evs = {252, 252, 4, 0, 0, 0},
        };
        IsabelPlayer(SPECIES_MUDBRAY, &mudbray);
        IsabelPlayer(SPECIES_XATU, &sMuseumOneNatu);
        JosephOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, playerLeft);
    } THEN {
        JosephResult(10, turns, 0);
        if (turns >= 2) {
            EXPECT_EQ(opponentLeft->species, SPECIES_MAGNETON);
            EXPECT_EQ(opponentLeft->item, ITEM_AIR_BALLOON);
            EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP), 88);
        }
        if (turns == 1) EXPECT_EQ(opponentRight->item, ITEM_NONE);
        EXPECT_EQ(gBattleResults.opponentFaintCounter, 0);
        if (turns == 4) EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Joseph disruption: Fake Out and Electric redirection")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 6; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        IsabelPlayer(SPECIES_MIENFOO, &sMuseumOneMienfoo);
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        JosephOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } SCENE {
        if (turns >= 3) ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentRight);
    } THEN {
        JosephResult(20, turns, 0);
        if (turns == 4) EXPECT_EQ(playerRight->hp, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Joseph sound immunity: Soundproof and Ground block Overdrive")
{
    u32 turns = 0, pop = 0;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= 4; t++) PARAMETRIZE { turns = t; pop = p; }
    GIVEN {
        const struct EmeraldChampionsBattleSet mime = {
            .moves = {MOVE_PSYCHIC, MOVE_DAZZLING_GLEAM, MOVE_FAKE_OUT, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_CALM, .ability = ABILITY_SOUNDPROOF,
            .evs = {252, 0, 4, 0, 252, 0},
        };
        const struct EmeraldChampionsBattleSet marshtomp = {
            .moves = {MOVE_EARTH_POWER, MOVE_MUDDY_WATER, MOVE_ICE_BEAM, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_TORRENT,
            .evs = {252, 0, 4, 252, 0, 0},
        };
        IsabelPlayer(SPECIES_MR_MIME, &mime);
        IsabelPlayer(SPECIES_MARSHTOMP, &marshtomp);
        JosephOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, pop && turn == 1 ? MOVE_ICE_BEAM : MOVE_EARTH_POWER, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SLUDGE_BOMB, opponentLeft);
        if (turns >= 3) ANIMATION(ANIM_TYPE_MOVE, MOVE_SNARL, opponentLeft);
    } THEN {
        JosephResult(30 + pop, turns, 0);
        EXPECT_EQ(opponentLeft->pp[0], 10);
        if (turns >= 3) {
            EXPECT_EQ(opponentLeft->item, ITEM_NONE);
            EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        }
        if (turns == 3) EXPECT_EQ(playerRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
        if (turns == 2) EXPECT_EQ(opponentRight->item, pop ? ITEM_NONE : ITEM_AIR_BALLOON);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Joseph timing: complete cold six-player party")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) IsabelPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]]);
        JosephOpponent(0);
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
        Test_MgbaPrintf("JOSEPH_TIMING setup=%d bothActors=%d complete=%d warm=%d", setup, complete - setup, complete, gMain.vblankCounter1 - warmStart);
        EXPECT(complete <= 72);
    }
}
