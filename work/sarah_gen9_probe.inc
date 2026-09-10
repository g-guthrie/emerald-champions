// Temporary actual E0025 native observations; independent of prior encounters.
#line 51000 "work/sarah_gen9_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void SarahPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sSarahEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sSarahTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sSarahPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sSarahLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sSarahMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};



static void SarahOpponent(bool32 reserves)
{
    AuthoredOpponent(TRAINER_SARAH, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species expected[] = {SPECIES_SPRITZEE, SPECIES_GOTHITA, SPECIES_FIDOUGH, SPECIES_BALTOY};
    for (u32 slot = 0; slot < 4; slot++)
    {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), expected[slot]);
        Test_MgbaPrintf("SARAH_INPUT reserves=%d slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
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

static void SarahResult(u32 board, u32 turns, bool32 reserves)
{
    Test_MgbaPrintf("SARAH_TEMPO board=%d turns=%d reserves=%d room=%d rawSpeeds=%d/%d ownHP=%d/%d friendship=%d/%d",
        board, turns, reserves, gFieldTimers.trickRoomTimer,
        gBattleMons[1].speed, gBattleMons[3].speed, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[1].friendship, gBattleMons[3].friendship);
    Test_MgbaPrintf("SARAH_AFTER board=%d turns=%d reserves=%d native=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S weather=%d duration=%d speeds=%d/%d foeSpeeds=%d/%d AtkStages=%d/%d foeStatus=%d/%d faints=%d/%d",
        board, turns, reserves, gBattleResults.battleTurnCounter, gBattleMons[1].species, gBattleMons[3].species,
        gBattleMons[1].hp, gBattleMons[3].hp, gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]), gBattleWeather, gBattleStruct->weatherDuration,
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK],
        gBattleMons[0].status1, gBattleMons[2].status1, gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}

AI_DOUBLE_BATTLE_TEST("EC live review Sarah core: physical special leads and reserves")
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
    PARAMETRIZE { board = 1; turns = 4; reserves = FALSE; }
    PARAMETRIZE { board = 1; turns = 5; reserves = FALSE; }
    PARAMETRIZE { board = 1; turns = 6; reserves = FALSE; }
    PARAMETRIZE { board = 1; turns = 7; reserves = FALSE; }
    PARAMETRIZE { board = 0; turns = 1; reserves = TRUE; }
    PARAMETRIZE { board = 0; turns = 2; reserves = TRUE; }
    PARAMETRIZE { board = 0; turns = 3; reserves = TRUE; }
    PARAMETRIZE { board = 0; turns = 4; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 1; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 2; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 3; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 4; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 5; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 6; reserves = TRUE; }
    GIVEN {
        SarahPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_PACHIRISU, board == 0 ? &sSarahEevee : &sSarahPachirisu);
        SarahPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_LOTAD, board == 0 ? &sSarahTimburr : &sSarahLotad);
        SarahOpponent(reserves);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE);
                MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            }
        }
    } THEN {
        SarahResult(board, turns, reserves);
        if (board == 1 && !reserves && turns == 5)
            EXPECT_EQ(gFieldTimers.trickRoomTimer, 0);
    }
}


AI_DOUBLE_BATTLE_TEST("EC live review Sarah disruption: Ghost phazing and noncontact Fire")
{
    u32 board, turns;
    PARAMETRIZE { board = 0; turns = 1; }
    PARAMETRIZE { board = 0; turns = 2; }
    PARAMETRIZE { board = 0; turns = 3; }
    PARAMETRIZE { board = 1; turns = 1; }
    PARAMETRIZE { board = 1; turns = 2; }
    PARAMETRIZE { board = 1; turns = 3; }
    PARAMETRIZE { board = 2; turns = 1; }
    PARAMETRIZE { board = 2; turns = 2; }
    PARAMETRIZE { board = 2; turns = 3; }
    PARAMETRIZE { board = 3; turns = 1; }
    PARAMETRIZE { board = 3; turns = 2; }
    PARAMETRIZE { board = 3; turns = 3; }
    PARAMETRIZE { board = 4; turns = 1; }
    PARAMETRIZE { board = 4; turns = 2; }
    PARAMETRIZE { board = 4; turns = 3; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet dreepy = {
            .moves = {MOVE_DRAGON_TAIL, MOVE_INFESTATION, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_CLEAR_BODY,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        static const struct EmeraldChampionsBattleSet electrike = {
            .moves = {MOVE_FLAMETHROWER, MOVE_THUNDERBOLT, MOVE_SNARL, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_LIGHTNING_ROD,
            .evs = {252, 0, 136, 120, 0, 0},
        };
        SarahPlayer(board == 0 || board == 3 ? SPECIES_MIENFOO : SPECIES_ELECTRIKE, board == 0 || board == 3 ? &sSarahMienfoo : &electrike);
        SarahPlayer(board == 0 || board == 3 ? SPECIES_DREEPY : SPECIES_LOTAD, board == 0 || board == 3 ? &dreepy : &sSarahLotad);
        SarahOpponent(board == 2);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0 || board == 3) {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: board == 3 ? opponentLeft : opponentRight, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
                MOVE(playerRight, turn == 0 ? MOVE_DRAGON_TAIL : MOVE_BITE, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (board == 4)
                    MOVE(playerLeft, MOVE_SNARL, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            }
        }
    } THEN {
        SarahResult(10 + board, turns, board == 2);
        Test_MgbaPrintf("SARAH_SPECIAL board=%d turns=%d SpA=%d/%d", board, turns,
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK]);
        if (board == 4 && turns == 2)
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        Test_MgbaPrintf("SARAH_CONTROL board=%d turns=%d room=%d Taunt=%d/%d items=%d/%d statuses=%d/%d",
            board, turns, gFieldTimers.trickRoomTimer,
            opponentLeft->volatiles.tauntTimer, opponentRight->volatiles.tauntTimer,
            opponentLeft->item, opponentRight->item, opponentLeft->status1, opponentRight->status1);
    }
}
