// Temporary actual E0023 native observations; independent of prior encounters.
#line 51000 "work/devan_gen9_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void DevanPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
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

static const struct EmeraldChampionsBattleSet sDevanEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sDevanTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sDevanPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sDevanLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sDevanMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};



static void DevanOpponent(bool32 reserves)
{
    AuthoredOpponent(TRAINER_DEVAN, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species expected[] = {SPECIES_HIPPOPOTAS, SPECIES_DRILBUR, SPECIES_ARON, SPECIES_SANDILE};
    for (u32 slot = 0; slot < 4; slot++)
    {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), expected[slot]);
        for (u32 move = 0; move < MAX_MON_MOVES; move++)
            EXPECT(CanSpeciesUseEmeraldChampionsPreparationMove(expected[slot], GetMonData(&party[slot], MON_DATA_MOVE1 + move)));
        Test_MgbaPrintf("DEVAN_INPUT reserves=%d slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
            reserves, slot, GetMonData(&party[slot], MON_DATA_LEVEL), GetMonData(&party[slot], MON_DATA_HP),
            GetMonData(&party[slot], MON_DATA_ATK), GetMonData(&party[slot], MON_DATA_DEF),
            GetMonData(&party[slot], MON_DATA_SPATK), GetMonData(&party[slot], MON_DATA_SPDEF),
            GetMonData(&party[slot], MON_DATA_SPEED), GetMonData(&party[slot], MON_DATA_HELD_ITEM), GetMonAbility(&party[slot]));
    }
    if (reserves)
    {
        u16 zero = 0;
        SetMonData(&party[0], MON_DATA_HP, &zero);
        SetMonData(&party[1], MON_DATA_HP, &zero);
        for (u32 slot = 0; slot < 2; slot++)
        {
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + 2];
            party[slot + 2] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
    }
}

static void DevanResult(u32 board, u32 turns, bool32 reserves)
{
    Test_MgbaPrintf("DEVAN_AFTER board=%d turns=%d reserves=%d native=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S weather=%d duration=%d speeds=%d/%d foeSpeeds=%d/%d AtkStages=%d/%d foeStatus=%d/%d faints=%d/%d",
        board, turns, reserves, gBattleResults.battleTurnCounter, gBattleMons[1].species, gBattleMons[3].species,
        gBattleMons[1].hp, gBattleMons[3].hp, gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]), gBattleWeather, gBattleStruct->weatherDuration,
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK],
        gBattleMons[0].status1, gBattleMons[2].status1, gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}

AI_DOUBLE_BATTLE_TEST("EC live review Devan core: physical special leads and reserves")
{
    u32 board, turns;
    bool32 reserves;
    PARAMETRIZE { board = 0; turns = 1; reserves = FALSE; }
    PARAMETRIZE { board = 0; turns = 2; reserves = FALSE; }
    PARAMETRIZE { board = 0; turns = 3; reserves = FALSE; }
    PARAMETRIZE { board = 0; turns = 4; reserves = FALSE; }
    PARAMETRIZE { board = 1; turns = 1; reserves = FALSE; }
    PARAMETRIZE { board = 1; turns = 2; reserves = FALSE; }
    PARAMETRIZE { board = 1; turns = 3; reserves = FALSE; }
    PARAMETRIZE { board = 0; turns = 1; reserves = TRUE; }
    PARAMETRIZE { board = 0; turns = 2; reserves = TRUE; }
    PARAMETRIZE { board = 0; turns = 3; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 1; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 2; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 3; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 4; reserves = TRUE; }
    GIVEN {
        DevanPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_PACHIRISU, board == 0 ? &sDevanEevee : &sDevanPachirisu);
        DevanPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_LOTAD, board == 0 ? &sDevanTimburr : &sDevanLotad);
        DevanOpponent(reserves);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (reserves && board == 1 && turn == 3)
                NOT_EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE);
                MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            }
        }
    } THEN {
        DevanResult(board, turns, reserves);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Devan weather: rain replacement and setup denial")
{
    u32 board, turns;
    PARAMETRIZE { board = 0; turns = 1; }
    PARAMETRIZE { board = 0; turns = 2; }
    PARAMETRIZE { board = 0; turns = 3; }
    PARAMETRIZE { board = 1; turns = 1; }
    PARAMETRIZE { board = 1; turns = 2; }
    PARAMETRIZE { board = 1; turns = 3; }
    GIVEN {
        struct EmeraldChampionsBattleSet lotad = sDevanLotad;
        lotad.ability = ABILITY_SWIFT_SWIM;
        DevanPlayer(SPECIES_MIENFOO, &sDevanMienfoo);
        DevanPlayer(SPECIES_LOTAD, &lotad);
        DevanOpponent(FALSE);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn == 0)
                MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            else
                MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
            if (board == 0 && turn == 0)
                MOVE(playerRight, MOVE_RAIN_DANCE);
            else
                MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentRight, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        DevanResult(10 + board, turns, FALSE);
    }
}
