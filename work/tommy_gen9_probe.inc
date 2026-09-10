// Temporary actual E0028 native observations; independent of prior encounters.
#line 51000 "work/tommy_gen9_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void TommyPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

AI_DOUBLE_BATTLE_TEST("EC live review Tommy water: opposing Water is redirected")
{
    GIVEN {
        static const struct EmeraldChampionsBattleSet barboach = {
            .moves = {MOVE_WATERFALL, MOVE_EARTHQUAKE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_OBLIVIOUS,
            .evs = {252, 252, 4, 0, 0, 0},
        };
        TommyPlayer(SPECIES_NATU, &gEmeraldChampionsDefaultBattleSets[SPECIES_NATU]);
        TommyPlayer(SPECIES_BARBOACH, &barboach);
        AuthoredOpponent(TRAINER_TOMMY, 0, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_WATERFALL, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("TOMMY_WATER species=%d/%d HP=%d/%d SpA=%d/%d moves=%S/%S foes=%d/%d",
            opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]), playerLeft->hp, playerRight->hp);
        EXPECT_EQ(opponentLeft->species, SPECIES_LILEEP);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
    }
}

static const struct EmeraldChampionsBattleSet sTommyEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sTommyTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sTommyPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sTommyLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sTommyMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};



static void TommyOpponent(u32 reserves)
{
    AuthoredOpponent(TRAINER_TOMMY, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species expected[] = {SPECIES_LILEEP, SPECIES_TIRTOUGA, SPECIES_ANORITH, SPECIES_SHUCKLE, SPECIES_NOSEPASS, SPECIES_BINACLE};
    for (u32 slot = 0; slot < 6; slot++)
    {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), expected[slot]);
        Test_MgbaPrintf("TOMMY_INPUT reserves=%d slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
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

static void TommyResult(u32 board, u32 turns, u32 reserves)
{
    Test_MgbaPrintf("TOMMY_SUPPORT board=%d turns=%d pair=%d screen=%d SpAStages=%d/%d foeStatus=%d/%d",
        board, turns, reserves, gSideTimers[B_SIDE_OPPONENT].lightscreenTimer,
        gBattleMons[1].statStages[STAT_SPATK], gBattleMons[3].statStages[STAT_SPATK], gBattleMons[0].status1, gBattleMons[2].status1);
    Test_MgbaPrintf("TOMMY_TEMPO board=%d turns=%d reserves=%d tailwind=%d rawSpeeds=%d/%d ownHP=%d/%d friendship=%d/%d",
        board, turns, reserves, gSideTimers[B_SIDE_OPPONENT].tailwindTimer,
        gBattleMons[1].speed, gBattleMons[3].speed, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[1].friendship, gBattleMons[3].friendship);
    Test_MgbaPrintf("TOMMY_AFTER board=%d turns=%d reserves=%d native=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S weather=%d duration=%d speeds=%d/%d foeSpeeds=%d/%d AtkStages=%d/%d foeStatus=%d/%d faints=%d/%d",
        board, turns, reserves, gBattleResults.battleTurnCounter, gBattleMons[1].species, gBattleMons[3].species,
        gBattleMons[1].hp, gBattleMons[3].hp, gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]), gBattleWeather, gBattleStruct->weatherDuration,
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK],
        gBattleMons[0].status1, gBattleMons[2].status1, gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}

AI_DOUBLE_BATTLE_TEST("EC live review Tommy core: physical special leads and reserves")
{
    u32 board, turns;
    u32 reserves;
    PARAMETRIZE { board = 0; turns = 1; reserves = FALSE; }
    PARAMETRIZE { board = 0; turns = 2; reserves = FALSE; }
    PARAMETRIZE { board = 0; turns = 3; reserves = FALSE; }
    PARAMETRIZE { board = 0; turns = 4; reserves = FALSE; }
    PARAMETRIZE { board = 0; turns = 5; reserves = FALSE; }
    PARAMETRIZE { board = 1; turns = 1; reserves = FALSE; }
    PARAMETRIZE { board = 1; turns = 2; reserves = FALSE; }
    PARAMETRIZE { board = 1; turns = 3; reserves = FALSE; }
    PARAMETRIZE { board = 1; turns = 4; reserves = FALSE; }
    PARAMETRIZE { board = 1; turns = 5; reserves = FALSE; }
    PARAMETRIZE { board = 0; turns = 1; reserves = TRUE; }
    PARAMETRIZE { board = 0; turns = 2; reserves = TRUE; }
    PARAMETRIZE { board = 0; turns = 3; reserves = TRUE; }
    PARAMETRIZE { board = 0; turns = 4; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 1; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 2; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 3; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 4; reserves = TRUE; }
    PARAMETRIZE { board = 0; turns = 1; reserves = 2; }
    PARAMETRIZE { board = 0; turns = 2; reserves = 2; }
    PARAMETRIZE { board = 0; turns = 3; reserves = 2; }
    PARAMETRIZE { board = 1; turns = 1; reserves = 2; }
    PARAMETRIZE { board = 1; turns = 2; reserves = 2; }
    PARAMETRIZE { board = 1; turns = 3; reserves = 2; }
    GIVEN {
        TommyPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_PACHIRISU, board == 0 ? &sTommyEevee : &sTommyPachirisu);
        TommyPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_LOTAD, board == 0 ? &sTommyTimburr : &sTommyLotad);
        TommyOpponent(reserves);
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
        TommyResult(board, turns, reserves);
        if (board == 0 && !reserves && turns == 2)
        {
            EXPECT_EQ(opponentLeft->species, SPECIES_LILEEP);
            EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(gLastMoves[3], MOVE_SURF);
        }
        if (board == 1 && !reserves && turns == 5)
        {
            EXPECT_EQ(opponentLeft->species, SPECIES_LILEEP);
            EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(gLastMoves[3], MOVE_AQUA_JET);
            EXPECT_EQ(playerLeft->hp, 0);
            EXPECT_EQ(playerRight->hp, 0);
            EXPECT(opponentLeft->hp && opponentRight->hp);
        }
        // This is an observed counter, not a demand that every board be won.
        if (board == 1 && reserves == 2 && turns == 3)
        {
            EXPECT_EQ(opponentLeft->hp, 0);
            EXPECT_EQ(opponentRight->hp, 0);
        }
    }
}


AI_DOUBLE_BATTLE_TEST("EC live review Tommy disruption: Ghost phazing and noncontact Fire")
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
        TommyPlayer(board == 0 ? SPECIES_MIENFOO : SPECIES_ELECTRIKE, board == 0 ? &sTommyMienfoo : &electrike);
        TommyPlayer(board == 0 ? SPECIES_DREEPY : SPECIES_LOTAD, board == 0 ? &dreepy : &sTommyLotad);
        TommyOpponent(board == 2);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
                MOVE(playerRight, turn == 0 ? MOVE_DRAGON_TAIL : MOVE_BITE, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            }
        }
    } THEN {
        TommyResult(10 + board, turns, board == 2);
        Test_MgbaPrintf("TOMMY_CONTROL board=%d turns=%d tailwind=%d Taunt=%d/%d items=%d/%d statuses=%d/%d",
            board, turns, gSideTimers[B_SIDE_OPPONENT].tailwindTimer,
            opponentLeft->volatiles.tauntTimer, opponentRight->volatiles.tauntTimer,
            opponentLeft->item, opponentRight->item, opponentLeft->status1, opponentRight->status1);
    }
}
