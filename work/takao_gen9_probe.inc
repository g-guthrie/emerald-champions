// Temporary actual E0037 native observations; independent of prior encounters.
#line 51000 "work/takao_gen9_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void TakaoPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sTakaoNatu = {
    .moves = {MOVE_PSYCHIC, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE,
    .evs = {4, 0, 0, 252, 0, 252},
};
static const struct EmeraldChampionsBattleSet sTakaoEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sTakaoTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sTakaoPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sTakaoLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sTakaoMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};



static void TakaoOpponent(u32 reserves)
{
    AuthoredOpponent(TRAINER_TAKAO, 1, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species expected[] = {SPECIES_GRAPPLOCT, SPECIES_WOBBUFFET, SPECIES_HITMONLEE, SPECIES_POLIWRATH, SPECIES_PASSIMIAN, SPECIES_PAWMO};
    for (u32 slot = 0; slot < ARRAY_COUNT(expected); slot++)
    {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), expected[slot]);
        Test_MgbaPrintf("TAKAO_INPUT reserves=%d slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
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

static void TakaoResult(u32 board, u32 turns, u32 reserves)
{
    Test_MgbaPrintf("TAKAO_TRAP board=%d turns=%d pair=%d octolock=%d/%d foeDef=%d/%d foeSpD=%d/%d", board, turns, reserves, (u32)gBattleMons[0].volatiles.octolock, (u32)gBattleMons[2].volatiles.octolock, gBattleMons[0].statStages[STAT_DEF], gBattleMons[2].statStages[STAT_DEF], gBattleMons[0].statStages[STAT_SPDEF], gBattleMons[2].statStages[STAT_SPDEF]);
    Test_MgbaPrintf("TAKAO_DEFENSE board=%d turns=%d pair=%d DefStages=%d/%d", board, turns, reserves, gBattleMons[1].statStages[STAT_DEF], gBattleMons[3].statStages[STAT_DEF]);
    Test_MgbaPrintf("TAKAO_STATUS board=%d turns=%d pair=%d ownStatus=%d/%d party0HP=%d items=%d/%d",
        board, turns, reserves, gBattleMons[1].status1, gBattleMons[3].status1,
        GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP), gBattleMons[1].item, gBattleMons[3].item);
    Test_MgbaPrintf("TAKAO_SUPPORT board=%d turns=%d pair=%d tailwind=%d SpAStages=%d/%d foeStatus=%d/%d",
        board, turns, reserves, gSideTimers[B_SIDE_OPPONENT].tailwindTimer,
        gBattleMons[1].statStages[STAT_SPATK], gBattleMons[3].statStages[STAT_SPATK], gBattleMons[0].status1, gBattleMons[2].status1);
    Test_MgbaPrintf("TAKAO_TEMPO board=%d turns=%d reserves=%d tailwind=%d rawSpeeds=%d/%d ownHP=%d/%d friendship=%d/%d",
        board, turns, reserves, gSideTimers[B_SIDE_OPPONENT].tailwindTimer,
        gBattleMons[1].speed, gBattleMons[3].speed, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[1].friendship, gBattleMons[3].friendship);
    Test_MgbaPrintf("TAKAO_AFTER board=%d turns=%d reserves=%d native=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S weather=%d duration=%d speeds=%d/%d foeSpeeds=%d/%d AtkStages=%d/%d foeStatus=%d/%d faints=%d/%d",
        board, turns, reserves, gBattleResults.battleTurnCounter, gBattleMons[1].species, gBattleMons[3].species,
        gBattleMons[1].hp, gBattleMons[3].hp, gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]), gBattleWeather, gBattleStruct->weatherDuration,
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK],
        gBattleMons[0].status1, gBattleMons[2].status1, gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}


AI_DOUBLE_BATTLE_TEST("EC live review Takao core: physical special leads and reserves")
{
    u32 board = 0, turns = 0, reserves = 0;
    for (u32 p = 0; p < 3; p++)
        for (u32 b = 0; b < 2; b++)
            for (u32 t = 1; t <= (p == 0 ? 3 : 2); t++)
                PARAMETRIZE { board = b; turns = t; reserves = p; }
    GIVEN {
        TakaoPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_NATU, board == 0 ? &sTakaoEevee : &sTakaoNatu);
        TakaoPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU, board == 0 ? &sTakaoTimburr : &sTakaoPachirisu);
        TakaoOpponent(reserves);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        TakaoResult(board, turns, reserves);
        if (reserves == 0 && turns == 1)
        {
            EXPECT_EQ(playerLeft->hp, 0);
            EXPECT_EQ(gLastMoves[3], MOVE_HELPING_HAND);
        }
        if (reserves == 0 && board == 1 && turns == 3)
        {
            EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
            EXPECT_EQ(gBattleResults.opponentFaintCounter, 0);
        }
        if (reserves == 1 && board == 0)
        {
            EXPECT((u32)opponentLeft->volatiles.unburdenActive);
            EXPECT_EQ(opponentLeft->item, ITEM_NONE);
            EXPECT_EQ(gAiLogicData->speedStats[1], opponentLeft->speed * 2);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Takao disruption: Fake Out Taunt and fast Electric pressure")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= 2; t++)
            PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet electrike = {
            .moves = {MOVE_THUNDERBOLT, MOVE_SNARL, MOVE_PROTECT, MOVE_FLAMETHROWER},
            .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_LIGHTNING_ROD,
            .evs = {4, 0, 0, 252, 0, 252},
        };
        TakaoPlayer(board == 0 ? SPECIES_MIENFOO : SPECIES_ELECTRIKE, board == 0 ? &sTakaoMienfoo : &electrike);
        TakaoPlayer(SPECIES_EEVEE, &sTakaoEevee);
        TakaoOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
            } else
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_DOUBLE_EDGE, target: opponentRight, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        TakaoResult(10 + board, turns, 0);

    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Takao motif: bulky targets and opposing setup")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= 3; t++)
            PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet marshtomp = {
            .moves = {MOVE_MUDDY_WATER, MOVE_EARTHQUAKE, MOVE_ICE_PUNCH, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_RELAXED, .ability = ABILITY_TORRENT,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        TakaoPlayer(board == 0 ? SPECIES_MARSHTOMP : SPECIES_TIMBURR, board == 0 ? &marshtomp : &sTakaoTimburr);
        TakaoPlayer(SPECIES_PACHIRISU, &sTakaoPachirisu);
        TakaoOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0)
                MOVE(playerLeft, MOVE_ICE_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else if (turn == 0)
                MOVE(playerLeft, MOVE_BULK_UP);
            else
                MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        TakaoResult(20 + board, turns, 0);
        Test_MgbaPrintf("TAKAO_MOTIF octolockPP=%d encored=%d/%d", gBattleMons[1].pp[0], (u32)gBattleMons[0].volatiles.encoreTimer, (u32)gBattleMons[2].volatiles.encoreTimer);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Takao priority disruption: Tailwind and redirection")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++)
        PARAMETRIZE { turns = t; }
    GIVEN {
        TakaoPlayer(SPECIES_NATU, &sTakaoNatu);
        TakaoPlayer(SPECIES_PACHIRISU, &sTakaoPachirisu);
        TakaoOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn == 0) {
                MOVE(playerLeft, MOVE_TAILWIND);
                MOVE(playerRight, MOVE_FOLLOW_ME);
            } else {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        TakaoResult(30, turns, 0);
    }
}
