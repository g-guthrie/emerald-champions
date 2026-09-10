// Temporary actual E0045 native observations; independent of prior encounters.
#line 51000 "work/lola_gen9_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void LolaPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sLolaNatu = {
    .moves = {MOVE_PSYCHIC, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE,
    .evs = {4, 0, 0, 252, 0, 252},
};
static const struct EmeraldChampionsBattleSet sLolaEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sLolaTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sLolaPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sLolaLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sLolaMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};



static void LolaOpponent(u32 reserves)
{
    AuthoredOpponent(TRAINER_LOLA_1, 1, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species expected[] = {SPECIES_CHERUBI, SPECIES_LEAFEON, SPECIES_LUVDISC, SPECIES_PONYTA};
    for (u32 slot = 0; slot < ARRAY_COUNT(expected); slot++)
    {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), expected[slot]);
        Test_MgbaPrintf("LOLA_INPUT reserves=%d slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
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


static void LolaResult(u32 board, u32 turns, u32 reserves)
{
    Test_MgbaPrintf("LOLA_SUPPORT board=%d turns=%d pair=%d slot2PP=%d flashFire=%d/%d", board, turns, reserves, gBattleMons[1].pp[2], (u32)gBattleMons[1].volatiles.flashFireBoosted, (u32)gBattleMons[3].volatiles.flashFireBoosted);
    Test_MgbaPrintf("LOLA_WEATHER board=%d turns=%d pair=%d weather=%d duration=%d SpA=%d/%d sunPP=%d", board, turns, reserves, gBattleWeather, gBattleStruct->weatherDuration, gBattleMons[1].statStages[STAT_SPATK], gBattleMons[3].statStages[STAT_SPATK], gBattleMons[1].pp[0]);
    Test_MgbaPrintf("LOLA_ENGINE board=%d turns=%d pair=%d status=%d/%d items=%d/%d unburden=%d/%d", board, turns, reserves, gBattleMons[1].status1, gBattleMons[3].status1, gBattleMons[1].item, gBattleMons[3].item, (u32)gBattleMons[1].volatiles.unburdenActive, (u32)gBattleMons[3].volatiles.unburdenActive);
    Test_MgbaPrintf("LOLA_AFTER board=%d turns=%d pair=%d native=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speeds=%d/%d foeSpeeds=%d/%d Atk=%d/%d Def=%d/%d PP=%d/%d faints=%d/%d",
        board, turns, reserves, gBattleResults.battleTurnCounter,
        gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[0].hp, gBattleMons[2].hp, GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK],
        gBattleMons[1].statStages[STAT_DEF], gBattleMons[3].statStages[STAT_DEF],
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}





AI_DOUBLE_BATTLE_TEST("EC live review Lola core: physical special leads and reserves")
{
    u32 board = 0, turns = 0, reserves = 0;
    for (u32 p = 0; p < 2; p++)
        for (u32 b = 0; b < 2; b++)
            for (u32 t = 1; t <= (p == 1 ? 4 : 3); t++)
                PARAMETRIZE { board = b; turns = t; reserves = p; }
    GIVEN {
        LolaPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_NATU, board == 0 ? &sLolaEevee : &sLolaNatu);
        LolaPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU, board == 0 ? &sLolaTimburr : &sLolaPachirisu);
        LolaOpponent(reserves);
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
        if (board == 1 && reserves == 1 && turns == 3)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_PULSE, opponentLeft);
    } THEN {
        LolaResult(board, turns, reserves);
        if (board == 0 && reserves == 0) {
            EXPECT(gBattleWeather & B_WEATHER_SUN);
            EXPECT_EQ(gBattleMons[1].species, SPECIES_CHERUBI);
            EXPECT_EQ(gBattleMons[1].hp, 1);
            EXPECT_EQ(gBattleMons[1].pp[0], 4);
            if (turns == 3) EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
        }
        if (reserves == 1 && board == 0 && turns == 2)
            EXPECT_EQ(gBattleMons[1].statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        if (reserves == 1 && board == 1 && turns == 3) {
            EXPECT_EQ(gBattleMons[1].pp[2], 9);
            EXPECT_EQ(gBattleMons[3].hp, 38);
        }
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Lola disruption: Fake Out Taunt and weather replacement")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= 3; t++)
            PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet sableye = {
            .moves = {MOVE_RAIN_DANCE, MOVE_FOUL_PLAY, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        LolaPlayer(board == 0 ? SPECIES_MIENFOO : SPECIES_SABLEYE, board == 0 ? &sLolaMienfoo : &sableye);
        LolaPlayer(SPECIES_PACHIRISU, &sLolaPachirisu);
        LolaOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
            } else if (turn == 0)
                MOVE(playerLeft, MOVE_PROTECT);
            else
                MOVE(playerLeft, MOVE_RAIN_DANCE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        LolaResult(10 + board, turns, 0);
        if (board == 1) {
            EXPECT(gBattleWeather & (turns == 2 ? B_WEATHER_RAIN : B_WEATHER_SUN));
            EXPECT_EQ(gBattleMons[1].pp[0], turns == 3 ? 3 : 4);
            EXPECT_EQ(gAiLogicData->speedStats[3], turns == 2 ? 56 : 112);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Lola Fire counterplay: Sash lead and Flash Fire reserve")
{
    u32 turns = 0, reserves = 0;
    for (u32 p = 0; p < 2; p++)
        for (u32 t = 1; t <= 2; t++) PARAMETRIZE { turns = t; reserves = p; }
    GIVEN {
        const struct EmeraldChampionsBattleSet electrike = {
            .moves = {MOVE_THUNDERBOLT, MOVE_SNARL, MOVE_FLAMETHROWER, MOVE_PROTECT},
            .item = ITEM_CHOICE_SCARF, .nature = NATURE_TIMID, .ability = ABILITY_LIGHTNING_ROD,
            .evs = {4, 0, 0, 252, 0, 252},
        };
        LolaPlayer(SPECIES_ELECTRIKE, &electrike);
        LolaPlayer(SPECIES_EEVEE, &sLolaEevee);
        LolaOpponent(reserves);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_FLAMETHROWER, target: reserves ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_DOUBLE_EDGE, target: reserves ? opponentLeft : opponentRight, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        LolaResult(20, turns, reserves);
        if (reserves) EXPECT(gBattleMons[3].volatiles.flashFireBoosted);
    }
}
