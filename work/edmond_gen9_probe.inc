// Temporary actual E0043 native observations; independent of prior encounters.
#line 51000 "work/edmond_gen9_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void EdmondPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sEdmondNatu = {
    .moves = {MOVE_PSYCHIC, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE,
    .evs = {4, 0, 0, 252, 0, 252},
};
static const struct EmeraldChampionsBattleSet sEdmondEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sEdmondTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sEdmondPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sEdmondLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sEdmondMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};



static void EdmondOpponent(u32 reserves)
{
    AuthoredOpponent(TRAINER_EDMOND, 1, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species expected[] = {SPECIES_MANTYKE, SPECIES_KRABBY, SPECIES_KOFFING, SPECIES_STARAVIA};
    for (u32 slot = 0; slot < ARRAY_COUNT(expected); slot++)
    {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), expected[slot]);
        Test_MgbaPrintf("EDMOND_INPUT reserves=%d slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
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


static void EdmondResult(u32 board, u32 turns, u32 reserves)
{
    Test_MgbaPrintf("EDMOND_TW board=%d turns=%d pair=%d wind=%d/%d tailwindPP=%d foesStatus=%d/%d foeAtk=%d/%d", board, turns, reserves, gSideTimers[1].tailwindTimer, gSideTimers[0].tailwindTimer, gBattleMons[1].pp[1], gBattleMons[0].status1, gBattleMons[2].status1, gBattleMons[0].statStages[STAT_ATK], gBattleMons[2].statStages[STAT_ATK]);
    Test_MgbaPrintf("EDMOND_FLAGS board=%d turns=%d pair=%d reflect=%d light=%d", board, turns, reserves, !!(gSideStatuses[1] & SIDE_STATUS_REFLECT), !!(gSideStatuses[1] & SIDE_STATUS_LIGHTSCREEN));
    Test_MgbaPrintf("EDMOND_SCREENS board=%d turns=%d pair=%d reflect=%d light=%d SpA=%d/%d", board, turns, reserves, gSideTimers[1].reflectTimer, gSideTimers[1].lightscreenTimer, gBattleMons[1].statStages[STAT_SPATK], gBattleMons[3].statStages[STAT_SPATK]);
    Test_MgbaPrintf("EDMOND_ENGINE board=%d turns=%d pair=%d status=%d/%d items=%d/%d unburden=%d/%d", board, turns, reserves, gBattleMons[1].status1, gBattleMons[3].status1, gBattleMons[1].item, gBattleMons[3].item, (u32)gBattleMons[1].volatiles.unburdenActive, (u32)gBattleMons[3].volatiles.unburdenActive);
    Test_MgbaPrintf("EDMOND_AFTER board=%d turns=%d pair=%d native=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speeds=%d/%d foeSpeeds=%d/%d Atk=%d/%d Def=%d/%d PP=%d/%d faints=%d/%d",
        board, turns, reserves, gBattleResults.battleTurnCounter,
        gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[0].hp, gBattleMons[2].hp, GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK],
        gBattleMons[1].statStages[STAT_DEF], gBattleMons[3].statStages[STAT_DEF],
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}




AI_DOUBLE_BATTLE_TEST("EC live review Edmond core: physical special leads and reserves")
{
    u32 board = 0, turns = 0, reserves = 0;
    for (u32 p = 0; p < 2; p++)
        for (u32 b = 0; b < 2; b++)
            for (u32 t = 1; t <= (p == 1 ? 4 : 3); t++)
                PARAMETRIZE { board = b; turns = t; reserves = p; }
    GIVEN {
        EdmondPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_NATU, board == 0 ? &sEdmondEevee : &sEdmondNatu);
        EdmondPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU, board == 0 ? &sEdmondTimburr : &sEdmondPachirisu);
        EdmondOpponent(reserves);
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
        EdmondResult(board, turns, reserves);
        if (reserves == 0 && board == 1 && turns == 2) {
            EXPECT_EQ(opponentLeft->pp[1], GetMovePP(MOVE_TAILWIND) - 1);
            EXPECT_EQ(gLastMoves[1], MOVE_HELPING_HAND);
            EXPECT_EQ(gLastMoves[3], MOVE_LIQUIDATION);
            EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
            EXPECT_EQ(gAiLogicData->speedStats[3], 78);
            EXPECT_EQ(gBattleResults.playerFaintCounter, 1);
        }
        if (reserves == 0 && board == 1 && turns == 3) {
            EXPECT_EQ(opponentRight->species, SPECIES_KOFFING);
            EXPECT_EQ(gBattleResults.opponentFaintCounter, 0);
        }
        if (reserves == 0 && board == 0 && turns == 3) {
            EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
            EXPECT_EQ(opponentLeft->species, SPECIES_STARAVIA);
            EXPECT(opponentRight->hp);
        }
        if (reserves == 1 && turns == 1) EXPECT(opponentRight->hp);
        if (reserves == 1 && turns == 4) {
            EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
            EXPECT(opponentLeft->hp);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Edmond disruption: Fake Out Taunt and opposing Tailwind")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= 3; t++)
            PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        EdmondPlayer(board == 0 ? SPECIES_MIENFOO : SPECIES_NATU, board == 0 ? &sEdmondMienfoo : &sEdmondNatu);
        EdmondPlayer(SPECIES_PACHIRISU, &sEdmondPachirisu);
        EdmondOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
            } else if (turn == 0)
                MOVE(playerLeft, MOVE_TAILWIND);
            else
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        EdmondResult(10 + board, turns, 0);
        if (board == 0) EXPECT_EQ(opponentLeft->pp[1], GetMovePP(MOVE_TAILWIND));
        if (board == 1) EXPECT_EQ(opponentLeft->pp[1], GetMovePP(MOVE_TAILWIND) - 1);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Edmond reserve support: redirected setup and offensive follow-through")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        EdmondPlayer(SPECIES_TIMBURR, &sEdmondTimburr);
        EdmondPlayer(SPECIES_PACHIRISU, &sEdmondPachirisu);
        EdmondOpponent(1);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn < 2) {
                MOVE(playerLeft, MOVE_BULK_UP);
                MOVE(playerRight, MOVE_FOLLOW_ME);
            } else {
                MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        EdmondResult(20, turns, 1);
        if (turns == 2) {
            EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(gBattleResults.playerFaintCounter, 1);
        }
        if (turns == 3) {
            EXPECT_EQ(gLastMoves[1], MOVE_CLEAR_SMOG);
            EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
            EXPECT_EQ(playerLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
            EXPECT(opponentLeft->hp);
        }
    }
}
