// Temporary E0057 native review, reusing the native level30 player helper.
#include "isabel_gen9_probe.inc"
#line 59000 "work/robin_gen9_probe.inc"

static void RobinOpponent(u32 pair)
{
    AuthoredOpponent(TRAINER_ROBIN, 2, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species species[] = {SPECIES_INDEEDEE_F, SPECIES_MEOWSTIC_F, SPECIES_EEVEE, SPECIES_CHIMECHO};
    for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
        EXPECT_EQ(GetMonData(&party[i], MON_DATA_SPECIES), species[i]);
        Test_MgbaPrintf("ROBIN_INPUT slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d", i,
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

static void RobinResult(u32 board, u32 turns, u32 pair)
{
    Test_MgbaPrintf("ROBIN_AFTER board=%d turns=%d pair=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speed=%d/%d foeSpeed=%d/%d items=%d/%d terrain=%d turns=%d SpA=%d/%d PP=%d/%d secondPP=%d/%d thirdPP=%d/%d guardPP=%d/%d faints=%d/%d",
        board, turns, pair, gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[0].hp, gBattleMons[2].hp, GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].item, gBattleMons[3].item, gFieldTimers.terrain, gFieldTimers.terrainTimer,
        gBattleMons[1].statStages[STAT_SPATK], gBattleMons[3].statStages[STAT_SPATK],
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleMons[1].pp[1], gBattleMons[3].pp[1],
        gBattleMons[1].pp[2], gBattleMons[3].pp[2], gBattleMons[1].pp[3], gBattleMons[3].pp[3],
        gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}

AI_DOUBLE_BATTLE_TEST("EC live review Robin core: physical special leads and reserves")
{
    u32 board = 0, turns = 0, pair = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (p == 0 ? 2 : 4); t++) PARAMETRIZE { pair = p; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        RobinOpponent(pair);
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
        if (pair == 0 && board == 0) ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentLeft);
    } THEN {
        RobinResult(board, turns, pair);
        if (pair == 0 && turns == 2) EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
        if (pair == 1 && board == 1 && turns == 2) {
            EXPECT_EQ(playerLeft->hp, 0);
            EXPECT_EQ(opponentLeft->pp[0], 19);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Robin disruption: priority denial and terrain replacement")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= 3; t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        if (board == 0) {
            struct EmeraldChampionsBattleSet set = sMuseumOneMienfoo; set.item = ITEM_FOCUS_SASH;
            IsabelPlayer(SPECIES_MIENFOO, &set);
        } else {
            const struct EmeraldChampionsBattleSet jolteon = {
                .moves = {MOVE_THUNDERBOLT, MOVE_SHADOW_BALL, MOVE_ELECTRIC_TERRAIN, MOVE_PROTECT},
                .item = ITEM_SITRUS_BERRY, .nature = NATURE_MODEST, .ability = ABILITY_VOLT_ABSORB,
                .evs = {252, 0, 4, 252, 0, 0},
            };
            IsabelPlayer(SPECIES_JOLTEON, &jolteon);
        }
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        RobinOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                if (turn == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
            } else if (turn == 0) MOVE(playerLeft, MOVE_ELECTRIC_TERRAIN);
            else MOVE(playerLeft, MOVE_SHADOW_BALL, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { RobinResult(10 + board, turns, 0); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Robin counterplay: Dark immunity and Competitive")
{
    u32 pair = 0, turns = 0;
    for (u32 p = 0; p < 2; p++) for (u32 t = 1; t <= 4; t++) PARAMETRIZE { pair = p; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet sableye = {
            .moves = {MOVE_FOUL_PLAY, MOVE_WILL_O_WISP, MOVE_TAUNT, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        const struct EmeraldChampionsBattleSet umbreon = {
            .moves = {MOVE_SNARL, MOVE_FOUL_PLAY, MOVE_MOONLIGHT, MOVE_PROTECT},
            .item = ITEM_LEFTOVERS, .nature = NATURE_CALM, .ability = ABILITY_INNER_FOCUS,
            .evs = {252, 0, 4, 0, 252, 0},
        };
        IsabelPlayer(SPECIES_SABLEYE, &sableye);
        IsabelPlayer(SPECIES_UMBREON, &umbreon);
        RobinOpponent(pair);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_SNARL, hit: TRUE, criticalHit: FALSE);
        }
    } SCENE {
        if ((pair == 1) || turns >= 3) ANIMATION(ANIM_TYPE_MOVE, MOVE_DAZZLING_GLEAM, opponentRight);
    } THEN {
        RobinResult(20, turns, pair);
        if (pair == 0 && turns >= 2) EXPECT_EQ(opponentRight->species, SPECIES_CHIMECHO);
        if (pair == 1 && turns == 1) EXPECT_LT(playerLeft->hp, playerLeft->maxHP);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Robin ability: Competitive and grounded priority control")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= 2; t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        if (board == 0) {
            struct EmeraldChampionsBattleSet set = gEmeraldChampionsDefaultBattleSets[SPECIES_ARCANINE];
            set.ability = ABILITY_INTIMIDATE;
            set.moves[0] = MOVE_FLARE_BLITZ;
            IsabelPlayer(SPECIES_ARCANINE, &set);
        } else IsabelPlayer(SPECIES_EEVEE, &sMuseumOneEevee);
        IsabelPlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        RobinOpponent(board == 0 ? 0 : 2);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) MOVE(playerLeft, MOVE_FLARE_BLITZ, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            else MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        RobinResult(30 + board, turns, board == 0 ? 0 : 2);
        if (board == 0) EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        else EXPECT_EQ(opponentRight->pp[0], 20);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Robin timing: complete cold six-player party")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) IsabelPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]]);
        AuthoredOpponent(TRAINER_ROBIN, 2, FALSE);
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
        Test_MgbaPrintf("ROBIN_TIMING setup=%d bothActors=%d complete=%d warm=%d", setup, complete - setup, complete, gMain.vblankCounter1 - warmStart);
    }
}
