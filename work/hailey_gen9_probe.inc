// Temporary actual E0042 native observations; independent of prior encounters.
#line 51000 "work/hailey_gen9_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void HaileyPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 20, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    for (u32 index = 0; index < MAX_MON_MOVES; index++)
        if (set->moves[index] != MOVE_NONE)
            EXPECT(CanSpeciesUseEmeraldChampionsPreparationMove(species, set->moves[index]));
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
            GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

static const struct EmeraldChampionsBattleSet sHaileyNatu = {
    .moves = {MOVE_PSYCHIC, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE,
    .evs = {4, 0, 0, 252, 0, 252},
};
static const struct EmeraldChampionsBattleSet sHaileyEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sHaileyTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sHaileyPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sHaileyLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sHaileyMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};



static void HaileyOpponent(u32 reserves)
{
    AuthoredOpponent(TRAINER_HAILEY, 1, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species expected[] = {SPECIES_FLAAFFY, SPECIES_WARTORTLE, SPECIES_MARILL, SPECIES_PALPITOAD};
    for (u32 slot = 0; slot < ARRAY_COUNT(expected); slot++)
    {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), expected[slot]);
        Test_MgbaPrintf("HAILEY_INPUT reserves=%d slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
            reserves, slot, GetMonData(&party[slot], MON_DATA_LEVEL), GetMonData(&party[slot], MON_DATA_HP),
            GetMonData(&party[slot], MON_DATA_ATK), GetMonData(&party[slot], MON_DATA_DEF),
            GetMonData(&party[slot], MON_DATA_SPATK), GetMonData(&party[slot], MON_DATA_SPDEF),
            GetMonData(&party[slot], MON_DATA_SPEED), GetMonData(&party[slot], MON_DATA_HELD_ITEM), GetMonAbility(&party[slot]));
    }
    if (reserves)
    {
        u16 zero = 0;
        for (u32 slot = 0; slot < reserves * 2; slot++)
            SetMonData(&party[slot], MON_DATA_HP, &zero);
        for (u32 slot = 0; slot < 2; slot++)
        {
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + reserves * 2];
            party[slot + reserves * 2] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
    }
}


static void HaileyResult(u32 board, u32 turns, u32 reserves)
{
    Test_MgbaPrintf("HAILEY_FLAGS board=%d turns=%d pair=%d reflect=%d light=%d", board, turns, reserves, !!(gSideStatuses[1] & SIDE_STATUS_REFLECT), !!(gSideStatuses[1] & SIDE_STATUS_LIGHTSCREEN));
    Test_MgbaPrintf("HAILEY_SCREENS board=%d turns=%d pair=%d reflect=%d light=%d SpA=%d/%d", board, turns, reserves, gSideTimers[1].reflectTimer, gSideTimers[1].lightscreenTimer, gBattleMons[1].statStages[STAT_SPATK], gBattleMons[3].statStages[STAT_SPATK]);
    Test_MgbaPrintf("HAILEY_ENGINE board=%d turns=%d pair=%d status=%d/%d items=%d/%d unburden=%d/%d", board, turns, reserves, gBattleMons[1].status1, gBattleMons[3].status1, gBattleMons[1].item, gBattleMons[3].item, (u32)gBattleMons[1].volatiles.unburdenActive, (u32)gBattleMons[3].volatiles.unburdenActive);
    Test_MgbaPrintf("HAILEY_AFTER board=%d turns=%d pair=%d native=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speeds=%d/%d foeSpeeds=%d/%d Atk=%d/%d Def=%d/%d PP=%d/%d faints=%d/%d",
        board, turns, reserves, gBattleResults.battleTurnCounter,
        gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[0].hp, gBattleMons[2].hp, GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK],
        gBattleMons[1].statStages[STAT_DEF], gBattleMons[3].statStages[STAT_DEF],
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}



AI_DOUBLE_BATTLE_TEST("EC live review Hailey core: physical special leads and reserves")
{
    u32 board = 0, turns = 0, reserves = 0;
    for (u32 p = 0; p < 2; p++)
        for (u32 b = 0; b < 2; b++)
            for (u32 t = 1; t <= (p == 0 && b == 0 ? 5 : 3); t++)
                PARAMETRIZE { board = b; turns = t; reserves = p; }
    GIVEN {
        HaileyPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_NATU, board == 0 ? &sHaileyEevee : &sHaileyNatu);
        HaileyPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU, board == 0 ? &sHaileyTimburr : &sHaileyPachirisu);
        HaileyOpponent(reserves);
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
    } THEN {
        HaileyResult(board, turns, reserves);
        if (reserves == 0 && board == 0 && turns >= 2) {
            EXPECT_EQ(opponentRight->species, SPECIES_WARTORTLE);
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
            EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
            EXPECT_EQ(opponentRight->pp[0], GetMovePP(MOVE_SHELL_SMASH) - 1);
            EXPECT_EQ(gAiLogicData->speedStats[3], 84);
            if (turns >= 3) EXPECT_EQ(gLastMoves[3], MOVE_HYDRO_PUMP);
        }
        if (reserves == 1 && board == 0 && turns == 3) {
            EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
            EXPECT(opponentLeft->hp && opponentRight->hp);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Hailey disruption: Fake Out Taunt and concentrated Electric")
{
    u32 turns = 0, board = 0;
    for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= 3; t++)
            PARAMETRIZE { turns = t; board = b; }
    GIVEN {
        HaileyPlayer(SPECIES_MIENFOO, &sHaileyMienfoo);
        HaileyPlayer(SPECIES_PACHIRISU, &sHaileyPachirisu);
        HaileyOpponent(board);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn == 0)
                MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else
                MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        HaileyResult(10 + board, turns, board);
        if (board == 1 && turns == 3) {
            EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
            EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
            EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
        }
        if (board == 0 && turns == 2) {
            EXPECT_EQ(opponentRight->species, SPECIES_PALPITOAD);
            EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
            EXPECT_EQ(gBattleResults.opponentFaintCounter, 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Hailey follow-through: Taunt screen removal and native reserve entry")
{
    u32 turns = 0, board = 0;
    for (u32 b = 0; b < 3; b++)
        for (u32 t = 1; t <= 3; t++)
            PARAMETRIZE { turns = t; board = b; }
    GIVEN {
        struct EmeraldChampionsBattleSet breaker = sHaileyTimburr;
        breaker.moves[2] = MOVE_BRICK_BREAK;
        struct EmeraldChampionsBattleSet taunter = sHaileyMienfoo;
        taunter.item = ITEM_PROTECTIVE_PADS;
        HaileyPlayer(board == 0 ? SPECIES_MIENFOO : SPECIES_EEVEE, board == 0 ? &taunter : &sHaileyEevee);
        HaileyPlayer(board == 0 ? SPECIES_PACHIRISU : SPECIES_TIMBURR, board == 0 ? &sHaileyPachirisu : &breaker);
        HaileyOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
                else
                    MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, board == 1 ? MOVE_BRICK_BREAK : MOVE_ICE_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        HaileyResult(20 + board, turns, 0);
        if (board == 0) EXPECT(!(gSideStatuses[1] & (SIDE_STATUS_REFLECT | SIDE_STATUS_LIGHTSCREEN)));
        if (board == 1 && turns == 1) EXPECT(!(gSideStatuses[1] & SIDE_STATUS_REFLECT));
        if (board == 1 && turns == 2) {
            EXPECT(playerRight->status1 & STATUS1_PARALYSIS);
            EXPECT_EQ(playerRight->pp[2], GetMovePP(MOVE_BRICK_BREAK) - 1);
        }
        if (board == 2 && turns == 3) {
            EXPECT_EQ(opponentLeft->species, SPECIES_MARILL);
            EXPECT_EQ(opponentRight->species, SPECIES_WARTORTLE);
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(gBattleResults.opponentFaintCounter, 0);
            EXPECT_EQ(gBattleResults.playerFaintCounter, 1);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Hailey counterplay: no Static setup and Grass pressure")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= 3; t++)
            PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet paddedEevee = sHaileyEevee;
        paddedEevee.item = ITEM_PROTECTIVE_PADS;
        const struct EmeraldChampionsBattleSet fastGrass = {
            .moves = {MOVE_GIGA_DRAIN, MOVE_BULLET_SEED, MOVE_BRICK_BREAK, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_OVERGROW,
            .evs = {4, 0, 0, 252, 0, 252},
        };
        HaileyPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_TREECKO, board == 0 ? &paddedEevee : &fastGrass);
        HaileyPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU, board == 0 ? &sHaileyTimburr : &sHaileyPachirisu);
        HaileyOpponent(board);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_GIGA_DRAIN, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN { HaileyResult(30 + board, turns, board); }
}
