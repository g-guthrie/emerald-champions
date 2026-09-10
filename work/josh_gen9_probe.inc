// Temporary actual E0027 native observations; independent of prior encounters.
#line 51000 "work/josh_gen9_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void JoshPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

AI_DOUBLE_BATTLE_TEST("EC live review Josh spread: Wide Guard versus targeted Water")
{
    u32 turns, spread;
    PARAMETRIZE { turns = 1; spread = TRUE; }
    PARAMETRIZE { turns = 2; spread = TRUE; }
    PARAMETRIZE { turns = 1; spread = FALSE; }
    PARAMETRIZE { turns = 2; spread = FALSE; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet barboach = {
            .moves = {MOVE_EARTHQUAKE, MOVE_WATERFALL, MOVE_PROTECT},
            .item = ITEM_CHOICE_BAND, .nature = NATURE_ADAMANT, .ability = ABILITY_OBLIVIOUS,
            .evs = {252, 252, 4, 0, 0, 0},
        };
        JoshPlayer(SPECIES_NATU, &gEmeraldChampionsDefaultBattleSets[SPECIES_NATU]);
        JoshPlayer(SPECIES_BARBOACH, &barboach);
        AuthoredOpponent(TRAINER_JOSH, 0, FALSE);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn == 0)
                MOVE(playerLeft, MOVE_PROTECT);
            else
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (spread)
                MOVE(playerRight, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
            else
                MOVE(playerRight, MOVE_WATERFALL, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("JOSH_SPREAD spread=%d turns=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S",
            spread, turns, opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]));
        if (spread && turns == 2)
        {
            EXPECT_EQ(gLastMoves[3], MOVE_WIDE_GUARD);
            EXPECT_EQ(opponentRight->hp, 1);
        }
    }
}

static const struct EmeraldChampionsBattleSet sJoshEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sJoshTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sJoshPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sJoshLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sJoshMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};



static void JoshOpponent(u32 reserves)
{
    AuthoredOpponent(TRAINER_JOSH, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species expected[] = {SPECIES_BONSLY, SPECIES_SHIELDON, SPECIES_ROLYCOLY, SPECIES_NACLI, SPECIES_DWEBBLE, SPECIES_GEODUDE_ALOLA};
    for (u32 slot = 0; slot < 6; slot++)
    {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), expected[slot]);
        Test_MgbaPrintf("JOSH_INPUT reserves=%d slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
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

static void JoshResult(u32 board, u32 turns, u32 reserves)
{
    Test_MgbaPrintf("JOSH_SUPPORT board=%d turns=%d pair=%d screen=%d speedStages=%d/%d foeStatus=%d/%d",
        board, turns, reserves, gSideTimers[B_SIDE_OPPONENT].lightscreenTimer,
        gBattleMons[1].statStages[STAT_SPEED], gBattleMons[3].statStages[STAT_SPEED], gBattleMons[0].status1, gBattleMons[2].status1);
    Test_MgbaPrintf("JOSH_TEMPO board=%d turns=%d reserves=%d tailwind=%d rawSpeeds=%d/%d ownHP=%d/%d friendship=%d/%d",
        board, turns, reserves, gSideTimers[B_SIDE_OPPONENT].tailwindTimer,
        gBattleMons[1].speed, gBattleMons[3].speed, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[1].friendship, gBattleMons[3].friendship);
    Test_MgbaPrintf("JOSH_AFTER board=%d turns=%d reserves=%d native=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S weather=%d duration=%d speeds=%d/%d foeSpeeds=%d/%d AtkStages=%d/%d foeStatus=%d/%d faints=%d/%d",
        board, turns, reserves, gBattleResults.battleTurnCounter, gBattleMons[1].species, gBattleMons[3].species,
        gBattleMons[1].hp, gBattleMons[3].hp, gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]), gBattleWeather, gBattleStruct->weatherDuration,
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK],
        gBattleMons[0].status1, gBattleMons[2].status1, gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}

AI_DOUBLE_BATTLE_TEST("EC live review Josh core: physical special leads and reserves")
{
    u32 board, turns;
    u32 reserves;
    PARAMETRIZE { board = 0; turns = 1; reserves = FALSE; }
    PARAMETRIZE { board = 0; turns = 2; reserves = FALSE; }
    PARAMETRIZE { board = 0; turns = 3; reserves = FALSE; }
    PARAMETRIZE { board = 0; turns = 4; reserves = FALSE; }
    PARAMETRIZE { board = 1; turns = 1; reserves = FALSE; }
    PARAMETRIZE { board = 1; turns = 2; reserves = FALSE; }
    PARAMETRIZE { board = 1; turns = 3; reserves = FALSE; }
    PARAMETRIZE { board = 0; turns = 1; reserves = TRUE; }
    PARAMETRIZE { board = 0; turns = 2; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 1; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 2; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 3; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 4; reserves = TRUE; }
    PARAMETRIZE { board = 1; turns = 5; reserves = TRUE; }
    PARAMETRIZE { board = 0; turns = 1; reserves = 2; }
    PARAMETRIZE { board = 0; turns = 2; reserves = 2; }
    PARAMETRIZE { board = 0; turns = 3; reserves = 2; }
    PARAMETRIZE { board = 0; turns = 4; reserves = 2; }
    PARAMETRIZE { board = 1; turns = 1; reserves = 2; }
    PARAMETRIZE { board = 1; turns = 2; reserves = 2; }
    PARAMETRIZE { board = 1; turns = 3; reserves = 2; }
    PARAMETRIZE { board = 1; turns = 4; reserves = 2; }
    GIVEN {
        JoshPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_PACHIRISU, board == 0 ? &sJoshEevee : &sJoshPachirisu);
        JoshPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_LOTAD, board == 0 ? &sJoshTimburr : &sJoshLotad);
        JoshOpponent(reserves);
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
        JoshResult(board, turns, reserves);
    }
}


AI_DOUBLE_BATTLE_TEST("EC live review Josh disruption: Ghost phazing and noncontact Fire")
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
    PARAMETRIZE { board = 2; turns = 4; }
    PARAMETRIZE { board = 2; turns = 5; }
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
        JoshPlayer(board == 0 ? SPECIES_MIENFOO : SPECIES_ELECTRIKE, board == 0 ? &sJoshMienfoo : &electrike);
        JoshPlayer(board == 0 ? SPECIES_DREEPY : SPECIES_LOTAD, board == 0 ? &dreepy : &sJoshLotad);
        JoshOpponent(board == 2);
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
        JoshResult(10 + board, turns, board == 2);
        if (board == 2 && turns == 2)
        {
            EXPECT_EQ(opponentLeft->species, SPECIES_ROLYCOLY);
            EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 6);
            EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].lightscreenTimer, 3);
        }
        Test_MgbaPrintf("JOSH_CONTROL board=%d turns=%d tailwind=%d Taunt=%d/%d items=%d/%d statuses=%d/%d",
            board, turns, gSideTimers[B_SIDE_OPPONENT].tailwindTimer,
            opponentLeft->volatiles.tauntTimer, opponentRight->volatiles.tauntTimer,
            opponentLeft->item, opponentRight->item, opponentLeft->status1, opponentRight->status1);
    }
}
