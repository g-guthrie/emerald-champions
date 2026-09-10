// Temporary E0055 native review; reuse the level30 player helper.
#include "isabel_gen9_probe.inc"
#line 57000 "work/sally_gen9_probe.inc"

static void SallyOpponent(u32 pair)
{
    AuthoredOpponent(TRAINER_SALLY, 2, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species species[] = {SPECIES_FLORAGATO, SPECIES_KIRLIA, SPECIES_LOPUNNY, SPECIES_WHIMSICOTT};
    for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
        EXPECT_EQ(GetMonData(&party[i], MON_DATA_SPECIES), species[i]);
        Test_MgbaPrintf("SALLY_INPUT slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d", i,
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
        struct Pokemon saved = party[0]; party[0] = party[3]; party[3] = saved;
        u16 zero = 0; SetMonData(&party[2], MON_DATA_HP, &zero); SetMonData(&party[3], MON_DATA_HP, &zero);
    }
}

static void SallyResult(u32 board, u32 turns, u32 pair)
{
    Test_MgbaPrintf("SALLY_AFTER board=%d turns=%d pair=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speed=%d/%d foeSpeed=%d/%d SpA=%d/%d SpD=%d/%d types=%d/%d,%d/%d abilities=%d/%d items=%d/%d PP=%d/%d supportPP=%d/%d guardPP=%d/%d faints=%d/%d",
        board, turns, pair, gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[0].hp, gBattleMons[2].hp, GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].statStages[STAT_SPATK], gBattleMons[3].statStages[STAT_SPATK],
        gBattleMons[1].statStages[STAT_SPDEF], gBattleMons[3].statStages[STAT_SPDEF],
        gBattleMons[1].types[0], gBattleMons[1].types[1], gBattleMons[3].types[0], gBattleMons[3].types[1],
        GetBattlerAbility(1), GetBattlerAbility(3), gBattleMons[1].item, gBattleMons[3].item,
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleMons[1].pp[2], gBattleMons[3].pp[2],
        gBattleMons[1].pp[3], gBattleMons[3].pp[3], gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}

AI_DOUBLE_BATTLE_TEST("EC live review Sally core: physical special leads and reserves")
{
    u32 board = 0, turns = 0, pair = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (p == 1 ? 5 : 4); t++) PARAMETRIZE { pair = p; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        SallyOpponent(pair);
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
        if (pair == 0 && board == 0) ANIMATION(ANIM_TYPE_MOVE, MOVE_CALM_MIND, opponentRight);
        if (pair == 1 && board == 1 && turns >= 3) ANIMATION(ANIM_TYPE_MOVE, MOVE_LEECH_SEED, opponentRight);
    } THEN {
        SallyResult(board, turns, pair);
        if (pair == 1 && board == 1 && turns == 3) {
            EXPECT_EQ((u32)playerRight->volatiles.leechSeed, LEECHSEEDED_BY(B_BATTLER_3));
            EXPECT_EQ(opponentRight->pp[1], 9);
        }
        if (pair == 0 && board == 0) {
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(opponentRight->pp[2], 19);
            if (turns <= 2) {
                EXPECT_EQ(opponentLeft->types[0], TYPE_FAIRY);
                EXPECT_EQ((u32)opponentLeft->volatiles.usedProteanLibero, TRUE);
            }
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Sally disruption: Fake Out Taunt and Follow Me")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 5; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet set = sMuseumOneMienfoo; set.item = ITEM_FOCUS_SASH;
        IsabelPlayer(SPECIES_MIENFOO, &set);
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        SallyOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else if (turn == 1) MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
            else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } THEN { SallyResult(10, turns, 0); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Sally setup: special pressure and mixed support pair")
{
    u32 pair = 0, turns = 0;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= 5; t++) PARAMETRIZE { pair = p == 0 ? 0 : 2; turns = t; }
    GIVEN {
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        IsabelPlayer(SPECIES_LOMBRE, &sMuseumOneLotad);
        SallyOpponent(pair);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CALM_MIND, opponentRight);
        if (pair == 0 && turns >= 2) ANIMATION(ANIM_TYPE_MOVE, MOVE_FAKE_OUT, opponentLeft);
    } THEN {
        SallyResult(20, turns, pair);
        EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + (pair == 0 || turns == 1 ? 1 : 2));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Sally timing: complete cold six-player party")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) IsabelPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]]);
        AuthoredOpponent(TRAINER_SALLY, 2, FALSE);
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
        Test_MgbaPrintf("SALLY_TIMING setup=%d bothActors=%d complete=%d warm=%d", setup, complete - setup, complete, gMain.vblankCounter1 - warmStart);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Sally disruption: Scrappy Ghosts and Encore opportunity")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= 3; t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        if (board == 0) {
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
        } else {
            IsabelPlayer(SPECIES_EEVEE, &sMuseumOneEevee);
            IsabelPlayer(SPECIES_GURDURR, &sMuseumOneTimburr);
        }
        SallyOpponent(1);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_SHADOW_BALL, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (turn == 0) {
                MOVE(playerLeft, MOVE_PROTECT);
                MOVE(playerRight, MOVE_BULK_UP);
            } else {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            }
        }
    } SCENE {
        if (board == 0 && turns >= 2) ANIMATION(ANIM_TYPE_MOVE, MOVE_HIGH_JUMP_KICK, opponentLeft);
    } THEN { SallyResult(30 + board, turns, 1); }
}
