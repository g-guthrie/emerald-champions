// Temporary E0060 Edwin review; no retained per-trainer test corpus.
#include "isabel_gen9_probe.inc"
#line 64000 "work/edwin_gen9_probe.inc"

static void EdwinOpponent(u32 pair)
{
    AuthoredOpponent(TRAINER_EDWIN_1, 2, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species species[] = {SPECIES_LUNATONE, SPECIES_SOLROCK, SPECIES_MEOWSTIC};
    for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
        EXPECT_EQ(GetMonData(&party[i], MON_DATA_SPECIES), species[i]);
        Test_MgbaPrintf("EDWIN_INPUT slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d", i,
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

static void EdwinResult(u32 board, u32 turns, u32 pair)
{
    Test_MgbaPrintf("EDWIN_AFTER board=%d turns=%d pair=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speed=%d/%d foeSpeed=%d/%d items=%d/%d foeStatus=%d/%d PP=%d/%d secondPP=%d/%d thirdPP=%d/%d fourthPP=%d/%d screens=%d/%d faints=%d/%d",
        board, turns, pair, gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1] < MOVES_COUNT ? gLastMoves[1] : MOVE_NONE), GetMoveName(gLastMoves[3] < MOVES_COUNT ? gLastMoves[3] : MOVE_NONE),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].item, gBattleMons[3].item, gBattleMons[0].status1, gBattleMons[2].status1,
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleMons[1].pp[1], gBattleMons[3].pp[1],
        gBattleMons[1].pp[2], gBattleMons[3].pp[2], gBattleMons[1].pp[3], gBattleMons[3].pp[3],
        gSideTimers[1].reflectTimer, gSideTimers[1].lightscreenTimer,
        gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}

AI_DOUBLE_BATTLE_TEST("EC live review Edwin core: physical special and both reserve pairs")
{
    u32 board = 0, turns = 0, pair = 0;
    for (u32 p = 0; p < 3; p++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (p == 2 && b == 0 ? 4 : 3); t++) PARAMETRIZE { pair = p; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        EdwinOpponent(pair);
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
        if (pair) ANIMATION(ANIM_TYPE_MOVE, board == 0 ? MOVE_REFLECT : MOVE_LIGHT_SCREEN, pair == 1 ? opponentRight : opponentLeft);
        if (pair == 1 && board == 1 && turns >= 2) ANIMATION(ANIM_TYPE_MOVE, MOVE_ICY_WIND, opponentLeft);
    } THEN {
        EdwinResult(board, turns, pair);
        if (pair) {
            const struct BattlePokemon *support = pair == 1 ? opponentRight : opponentLeft;
            EXPECT_EQ(support->pp[board == 0 ? 0 : 1], 19);
        }
        if (pair == 1 && board == 1 && turns >= 2) {
            EXPECT_EQ(opponentLeft->pp[2], 14);
            EXPECT(gAiLogicData->speedStats[1] > gAiLogicData->speedStats[2]);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Edwin disruption: Fake Out Taunt and Water pressure")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        IsabelPlayer(SPECIES_MIENFOO, &sMuseumOneMienfoo);
        IsabelPlayer(SPECIES_LOMBRE, &sMuseumOneLotad);
        EdwinOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else if (turn == 1) MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
            else MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentRight, hit: TRUE, criticalHit: FALSE);
        }
    } THEN { EdwinResult(10, turns, 0); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Edwin counterplay: Dark Ghost pressure")
{
    u32 turns = 0, pair = 0;
    for (u32 p = 0; p < 3; p++) for (u32 t = 1; t <= (p <= 1 ? 5 : 3); t++) PARAMETRIZE { turns = t; pair = p; }
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
        EdwinOpponent(pair);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_SHADOW_BALL, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (pair == 1 && turns == 5) ANIMATION(ANIM_TYPE_MOVE, MOVE_DAZZLING_GLEAM, opponentRight);
    } THEN {
        EdwinResult(20, turns, pair);
        if (pair == 1 && turns == 5) {
            EXPECT_EQ(opponentRight->pp[3], 9);
            EXPECT_EQ(playerRight->hp, 0);
            EXPECT(playerLeft->hp < playerLeft->maxHP);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Edwin reserve disruption: actual Taunt after Fake Out")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet mienfoo = sMuseumOneMienfoo;
        mienfoo.item = ITEM_FOCUS_SASH;
        IsabelPlayer(SPECIES_MIENFOO, &mienfoo);
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        EdwinOpponent(1);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else if (turn == 1) MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
            else MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (turns >= 2) ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, playerLeft);
    } THEN { EdwinResult(30, turns, 1); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Edwin timing: complete cold six-player party")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) IsabelPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]]);
        EdwinOpponent(0);
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
        Test_MgbaPrintf("EDWIN_TIMING setup=%d bothActors=%d complete=%d warm=%d", setup, complete - setup, complete, gMain.vblankCounter1 - warmStart);
        EXPECT(complete <= 72);
    }
}
