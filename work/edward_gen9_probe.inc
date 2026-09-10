// Temporary E0061 Edward review; native parties and unforced NPC choices.
#include "isabel_gen9_probe.inc"
#line 4 "work/edward_gen9_probe.inc"

static void EdwardOpponent(u32 pair)
{
    AuthoredOpponent(TRAINER_EDWARD, 2, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species species[] = {SPECIES_HYPNO, SPECIES_XATU, SPECIES_CHIMECHO};
    for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
        EXPECT_EQ(GetMonData(&party[i], MON_DATA_SPECIES), species[i]);
        Test_MgbaPrintf("EDWARD_INPUT slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d", i,
            GetMonData(&party[i], MON_DATA_LEVEL), GetMonData(&party[i], MON_DATA_HP), GetMonData(&party[i], MON_DATA_ATK),
            GetMonData(&party[i], MON_DATA_DEF), GetMonData(&party[i], MON_DATA_SPATK), GetMonData(&party[i], MON_DATA_SPDEF),
            GetMonData(&party[i], MON_DATA_SPEED), GetMonData(&party[i], MON_DATA_HELD_ITEM), GetMonAbility(&party[i]));
    }
    if (pair) {
        u32 replaced = pair == 1 ? 1 : 0;
        struct Pokemon saved = party[replaced]; party[replaced] = party[2]; party[2] = saved;
        u16 zero = 0; SetMonData(&party[2], MON_DATA_HP, &zero);
    }
}

static void EdwardResult(u32 board, u32 turns, u32 pair)
{
    Test_MgbaPrintf("EDWARD_AFTER board=%d turns=%d pair=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speed=%d/%d foeSpeed=%d/%d items=%d/%d foeStatus=%d/%d PP=%d/%d secondPP=%d/%d thirdPP=%d/%d fourthPP=%d/%d faints=%d/%d partyHP=%d/%d/%d taunt=%d/%d",
        board, turns, pair, gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1] < MOVES_COUNT ? gLastMoves[1] : MOVE_NONE), GetMoveName(gLastMoves[3] < MOVES_COUNT ? gLastMoves[3] : MOVE_NONE),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].item, gBattleMons[3].item, gBattleMons[0].status1, gBattleMons[2].status1,
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleMons[1].pp[1], gBattleMons[3].pp[1],
        gBattleMons[1].pp[2], gBattleMons[3].pp[2], gBattleMons[1].pp[3], gBattleMons[3].pp[3],
        gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter,
        GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP), GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
        GetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_HP), gBattleMons[1].volatiles.tauntTimer, gBattleMons[3].volatiles.tauntTimer);
}

AI_DOUBLE_BATTLE_TEST("EC live review Edward core: pressure and surviving pairs")
{
    u32 board = 0, turns = 0, pair = 0;
    for (u32 p = 0; p < 3; p++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (p == 0 ? (b == 0 ? 4 : 7) : (b == 0 ? 3 : 4)); t++) PARAMETRIZE { pair = p; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        EdwardOpponent(pair);
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
        if (pair == 0 && board == 1 && turns >= 2) ANIMATION(ANIM_TYPE_MOVE, MOVE_LIFE_DEW, opponentRight);
        if (pair == 1 && board == 1 && turns >= 2) ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentLeft);
    } THEN {
        EdwardResult(board, turns, pair);
        if (pair == 0 && board == 0 && turns >= 3) {
            EXPECT_EQ(opponentLeft->species, SPECIES_XATU);
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP), 15);
        }
        if (pair == 0 && board == 0 && turns == 4) {
            EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
            EXPECT_EQ(gBattleResults.opponentFaintCounter, 0);
        }
        if (pair == 0 && board == 1 && turns == 2) {
            EXPECT_EQ(opponentLeft->hp, 108);
            EXPECT_EQ(opponentRight->hp, 111);
            EXPECT_EQ(opponentRight->pp[1], 9);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Edward disruption: Fake Out and Taunt")
{
    u32 turns = 0, pair = 0;
    for (u32 p = 0; p < 3; p++) for (u32 t = 1; t <= 2; t++) PARAMETRIZE { pair = p; turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet mienfoo = sMuseumOneMienfoo;
        mienfoo.item = ITEM_FOCUS_SASH;
        IsabelPlayer(SPECIES_MIENFOO, &mienfoo);
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        EdwardOpponent(pair);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { EdwardResult(10, turns, pair); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Edward counterplay: Dark Ghost pressure")
{
    u32 turns = 0, pair = 0;
    for (u32 p = 0; p < 3; p++) for (u32 t = 1; t <= 4; t++) PARAMETRIZE { turns = t; pair = p; }
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
        EdwardOpponent(pair);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_SHADOW_BALL, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (pair == 0) ANIMATION(ANIM_TYPE_MOVE, MOVE_DAZZLING_GLEAM, opponentLeft);
        if (pair == 2 && turns >= 3) ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentLeft);
    } THEN {
        EdwardResult(20, turns, pair);
        if (pair == 0 && turns == 3) {
            EXPECT_EQ(opponentLeft->pp[0], 19);
            EXPECT_EQ(playerLeft->status1, STATUS1_PARALYSIS);
            Test_MgbaPrintf("EDWARD_PARA sableFoulPlayPP=%d", playerLeft->pp[0]);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Edward support: injury recovery and Taunt")
{
    u32 board = 0, turns = 0, pair = 0;
    for (u32 b = 0; b < 2; b++) for (u32 p = 1; p < 3; p++) for (u32 t = 1; t <= 3; t++)
        PARAMETRIZE { board = b; pair = p; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet sableye = {
            .moves = {MOVE_FOUL_PLAY, MOVE_TAUNT, MOVE_RECOVER, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        IsabelPlayer(SPECIES_SABLEYE, &sableye);
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        EdwardOpponent(pair);
        if (board == 0) {
            struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
            u16 hp = pair == 1 ? 50 : 35;
            SetMonData(&party[pair == 1 ? 0 : 1], MON_DATA_HP, &hp);
            if (pair == 2) { u16 item = ITEM_NONE; SetMonData(&party[1], MON_DATA_HELD_ITEM, &item); }
        }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) MOVE(playerLeft, MOVE_RECOVER);
            else if (turn == 0) MOVE(playerLeft, MOVE_TAUNT, target: pair == 1 ? opponentLeft : opponentRight);
            else MOVE(playerLeft, MOVE_FOUL_PLAY, target: pair == 1 ? opponentLeft : opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: pair == 1 ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (board == 0 && pair == 1) ANIMATION(ANIM_TYPE_MOVE, MOVE_LIFE_DEW, opponentRight);
        if (board == 0 && pair == 2 && turns == 3) ANIMATION(ANIM_TYPE_MOVE, MOVE_LIFE_DEW, opponentLeft);
        if (board == 1 && pair == 2) ABILITY_POPUP(opponentRight, ABILITY_MAGIC_BOUNCE);
    } THEN {
        EdwardResult(30 + board, turns, pair);
        if (board == 0 && pair == 1 && turns == 3) {
            EXPECT_EQ(opponentLeft->hp, 104);
            EXPECT_EQ(opponentRight->pp[1], 8);
            EXPECT_EQ(opponentLeft->pp[3], 19);
        }
        if (board == 0 && pair == 2 && turns == 3) {
            EXPECT_EQ(opponentRight->species, SPECIES_XATU);
            EXPECT_EQ(opponentRight->hp, 55);
            EXPECT_EQ(opponentLeft->hp, 111);
            EXPECT_EQ(opponentLeft->pp[1], 9);
        }
        if (board == 1 && pair == 1 && turns == 1) {
            EXPECT_EQ(opponentLeft->item, ITEM_NONE);
            EXPECT_EQ((u32)opponentLeft->volatiles.tauntTimer, 0);
        }
        if (board == 1 && pair == 2 && turns == 1) EXPECT_EQ((u32)opponentRight->volatiles.tauntTimer, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Edward timing: complete six-player party")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) IsabelPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]]);
        EdwardOpponent(0);
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
        Test_MgbaPrintf("EDWARD_TIMING setup=%d bothActors=%d complete=%d warm=%d", setup, complete - setup, complete, gMain.vblankCounter1 - warmStart);
        EXPECT(complete <= 72);
    }
}
