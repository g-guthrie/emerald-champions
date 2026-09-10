// Temporary actual E0046 native observations; independent of prior encounters.
#line 51000 "work/chandler_gen9_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void ChandlerPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sChandlerNatu = {
    .moves = {MOVE_PSYCHIC, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE,
    .evs = {4, 0, 0, 252, 0, 252},
};
static const struct EmeraldChampionsBattleSet sChandlerEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sChandlerTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sChandlerPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sChandlerLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sChandlerMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};



static void ChandlerOpponent(u32 reserves)
{
    AuthoredOpponent(TRAINER_CHANDLER, 1, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species expected[] = {SPECIES_PHIONE, SPECIES_JOLTIK, SPECIES_BUIZEL, SPECIES_QUAGSIRE};
    for (u32 slot = 0; slot < ARRAY_COUNT(expected); slot++)
    {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), expected[slot]);
        Test_MgbaPrintf("CHANDLER_INPUT reserves=%d slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
            reserves, slot, GetMonData(&party[slot], MON_DATA_LEVEL), GetMonData(&party[slot], MON_DATA_HP),
            GetMonData(&party[slot], MON_DATA_ATK), GetMonData(&party[slot], MON_DATA_DEF),
            GetMonData(&party[slot], MON_DATA_SPATK), GetMonData(&party[slot], MON_DATA_SPDEF),
            GetMonData(&party[slot], MON_DATA_SPEED), GetMonData(&party[slot], MON_DATA_HELD_ITEM), GetMonAbility(&party[slot]));
    }
    if (reserves == 2)
    {
        u16 zero = 0;
        struct Pokemon saved = party[1];
        party[1] = party[2]; party[2] = saved;
        SetMonData(&party[2], MON_DATA_HP, &zero);
        SetMonData(&party[3], MON_DATA_HP, &zero);
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
    }
    else if (reserves)
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


static void ChandlerResult(u32 board, u32 turns, u32 reserves)
{
    Test_MgbaPrintf("CHANDLER_SUPPORT board=%d turns=%d pair=%d lastSlotPP=%d/%d foeAtk=%d foeDef=%d foeStatus=%d/%d", board, turns, reserves, gBattleMons[1].pp[3], gBattleMons[3].pp[3], gBattleMons[0].statStages[STAT_ATK], gBattleMons[0].statStages[STAT_DEF], gBattleMons[0].status1, gBattleMons[2].status1);
    Test_MgbaPrintf("CHANDLER_WEATHER board=%d turns=%d pair=%d weather=%d duration=%d SpA=%d/%d slot0PP=%d", board, turns, reserves, gBattleWeather, gBattleStruct->weatherDuration, gBattleMons[1].statStages[STAT_SPATK], gBattleMons[3].statStages[STAT_SPATK], gBattleMons[1].pp[0]);
    Test_MgbaPrintf("CHANDLER_ENGINE board=%d turns=%d pair=%d status=%d/%d items=%d/%d unburden=%d/%d", board, turns, reserves, gBattleMons[1].status1, gBattleMons[3].status1, gBattleMons[1].item, gBattleMons[3].item, (u32)gBattleMons[1].volatiles.unburdenActive, (u32)gBattleMons[3].volatiles.unburdenActive);
    Test_MgbaPrintf("CHANDLER_AFTER board=%d turns=%d pair=%d native=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speeds=%d/%d foeSpeeds=%d/%d Atk=%d/%d Def=%d/%d PP=%d/%d faints=%d/%d",
        board, turns, reserves, gBattleResults.battleTurnCounter,
        gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[0].hp, gBattleMons[2].hp, GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK],
        gBattleMons[1].statStages[STAT_DEF], gBattleMons[3].statStages[STAT_DEF],
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}





AI_DOUBLE_BATTLE_TEST("EC live review Chandler core: physical special leads and reserves")
{
    u32 board = 0, turns = 0, reserves = 0;
    for (u32 p = 0; p < 2; p++)
        for (u32 b = 0; b < 2; b++)
            for (u32 t = 1; t <= (p ? 5 : b == 0 ? 4 : 3); t++)
                PARAMETRIZE { board = b; turns = t; reserves = p; }
    GIVEN {
        ChandlerPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_NATU, board == 0 ? &sChandlerEevee : &sChandlerNatu);
        ChandlerPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU, board == 0 ? &sChandlerTimburr : &sChandlerPachirisu);
        ChandlerOpponent(reserves);
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
        ChandlerResult(board, turns, reserves);
        if (!reserves && turns <= 3) {
            EXPECT(gBattleWeather & B_WEATHER_RAIN);
            EXPECT_EQ(gBattleMons[1].species, SPECIES_PHIONE);
            EXPECT_EQ(gBattleMons[1].pp[0], 4);
        }
        if (reserves && board == 0 && turns == 2) {
            EXPECT_EQ(gBattleMons[1].species, SPECIES_BUIZEL);
            EXPECT(gBattleMons[1].hp > 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Chandler disruption: Fake Out Taunt and sun replacement")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= 3; t++)
            PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet sableye = {
            .moves = {MOVE_SUNNY_DAY, MOVE_FOUL_PLAY, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        ChandlerPlayer(board == 0 ? SPECIES_MIENFOO : SPECIES_SABLEYE, board == 0 ? &sChandlerMienfoo : &sableye);
        ChandlerPlayer(SPECIES_PACHIRISU, &sChandlerPachirisu);
        ChandlerOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                if (turn == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
            } else if (turn == 0) MOVE(playerLeft, MOVE_PROTECT);
            else MOVE(playerLeft, MOVE_SUNNY_DAY);
            MOVE(playerRight, MOVE_SUPER_FANG, target: opponentRight, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        ChandlerResult(10 + board, turns, 0);
        if (board == 1) {
            EXPECT(gBattleWeather & (turns == 2 ? B_WEATHER_SUN : B_WEATHER_RAIN));
            EXPECT_EQ(gBattleMons[1].pp[0], turns == 3 ? 3 : 4);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Chandler motifs: Swift Swim and Unaware support")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (b == 0 ? 3 : 5); t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet natu = sChandlerNatu;
        natu.item = ITEM_CHOICE_SCARF;
        ChandlerPlayer(board == 0 ? SPECIES_NATU : SPECIES_TIMBURR, board == 0 ? &natu : &sChandlerTimburr);
        ChandlerPlayer(SPECIES_PACHIRISU, &sChandlerPachirisu);
        ChandlerOpponent(board == 0 ? 2 : 1);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            } else {
                if (turn < 2) MOVE(playerLeft, MOVE_BULK_UP);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_FOLLOW_ME);
            }
        }
    } THEN {
        ChandlerResult(20 + board, turns, board == 0 ? 2 : 1);
        if (board == 0 && turns >= 2) {
            EXPECT(gBattleWeather & B_WEATHER_RAIN);
            EXPECT_EQ(gAiLogicData->speedStats[3], 114);
            EXPECT_EQ(gBattleMons[1].pp[0], 4);
        }
        if (board == 1 && turns == 3) {
            EXPECT_EQ(gBattleMons[0].statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(gBattleMons[3].ability, ABILITY_UNAWARE);
            EXPECT_EQ(gBattleMons[3].hp, 67);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Chandler Hydration: actual burn and sustained pressure")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet sableye = {
            .moves = {MOVE_WILL_O_WISP, MOVE_FOUL_PLAY, MOVE_RECOVER, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        ChandlerPlayer(SPECIES_SABLEYE, &sableye);
        ChandlerPlayer(SPECIES_PACHIRISU, &sChandlerPachirisu);
        ChandlerOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn == 0) MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentLeft, hit: TRUE);
            else MOVE(playerLeft, MOVE_RECOVER);
            MOVE(playerRight, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WILL_O_WISP, playerLeft);
        ABILITY_POPUP(opponentLeft, ABILITY_HYDRATION);
    } THEN {
        ChandlerResult(30, turns, 0);
        EXPECT_EQ(gBattleMons[1].status1, 0);
        EXPECT(gBattleWeather & B_WEATHER_RAIN);
    }
}
