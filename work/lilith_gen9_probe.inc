// Temporary actual E0036 native observations; independent of prior encounters.
#line 51000 "work/lilith_gen9_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void LilithPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sLilithNatu = {
    .moves = {MOVE_PSYCHIC, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE,
    .evs = {4, 0, 0, 252, 0, 252},
};
static const struct EmeraldChampionsBattleSet sLilithEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sLilithTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sLilithPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sLilithLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sLilithMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};



static void LilithOpponent(u32 reserves)
{
    AuthoredOpponent(TRAINER_LILITH, 1, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species expected[] = {SPECIES_HITMONTOP, SPECIES_SCRAGGY, SPECIES_PANCHAM, SPECIES_FARFETCHD_GALAR, SPECIES_MEDITITE, SPECIES_CLOBBOPUS};
    for (u32 slot = 0; slot < ARRAY_COUNT(expected); slot++)
    {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), expected[slot]);
        Test_MgbaPrintf("LILITH_INPUT reserves=%d slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
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

static void LilithResult(u32 board, u32 turns, u32 reserves)
{
    Test_MgbaPrintf("LILITH_PARTIES board=%d turns=%d pair=%d player=%d/%d opponent=%d/%d foeAtk=%d/%d foeSpA=%d/%d",
        board, turns, reserves, gBattlerPartyIndexes[0], gBattlerPartyIndexes[2],
        gBattlerPartyIndexes[1], gBattlerPartyIndexes[3], gBattleMons[0].statStages[STAT_ATK],
        gBattleMons[2].statStages[STAT_ATK], gBattleMons[0].statStages[STAT_SPATK], gBattleMons[2].statStages[STAT_SPATK]);
    Test_MgbaPrintf("LILITH_DEFENSE board=%d turns=%d pair=%d DefStages=%d/%d", board, turns, reserves, gBattleMons[1].statStages[STAT_DEF], gBattleMons[3].statStages[STAT_DEF]);
    Test_MgbaPrintf("LILITH_STATUS board=%d turns=%d pair=%d ownStatus=%d/%d party0HP=%d items=%d/%d",
        board, turns, reserves, gBattleMons[1].status1, gBattleMons[3].status1,
        GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP), gBattleMons[1].item, gBattleMons[3].item);
    Test_MgbaPrintf("LILITH_SUPPORT board=%d turns=%d pair=%d tailwind=%d SpAStages=%d/%d foeStatus=%d/%d",
        board, turns, reserves, gSideTimers[B_SIDE_OPPONENT].tailwindTimer,
        gBattleMons[1].statStages[STAT_SPATK], gBattleMons[3].statStages[STAT_SPATK], gBattleMons[0].status1, gBattleMons[2].status1);
    Test_MgbaPrintf("LILITH_TEMPO board=%d turns=%d reserves=%d tailwind=%d rawSpeeds=%d/%d ownHP=%d/%d friendship=%d/%d",
        board, turns, reserves, gSideTimers[B_SIDE_OPPONENT].tailwindTimer,
        gBattleMons[1].speed, gBattleMons[3].speed, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[1].friendship, gBattleMons[3].friendship);
    Test_MgbaPrintf("LILITH_AFTER board=%d turns=%d reserves=%d native=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S weather=%d duration=%d speeds=%d/%d foeSpeeds=%d/%d AtkStages=%d/%d foeStatus=%d/%d faints=%d/%d",
        board, turns, reserves, gBattleResults.battleTurnCounter, gBattleMons[1].species, gBattleMons[3].species,
        gBattleMons[1].hp, gBattleMons[3].hp, gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]), gBattleWeather, gBattleStruct->weatherDuration,
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK],
        gBattleMons[0].status1, gBattleMons[2].status1, gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}

AI_DOUBLE_BATTLE_TEST("EC live review Lilith setup and denial: bulky Water and Fairy pressure")
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
        static const struct EmeraldChampionsBattleSet ralts = {
            .moves = {MOVE_DAZZLING_GLEAM, MOVE_PSYCHIC, MOVE_TRICK_ROOM, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_TRACE,
            .evs = {252, 0, 0, 252, 4, 0},
        };
        LilithPlayer(board == 0 ? SPECIES_MARSHTOMP : SPECIES_RALTS, board == 0 ? &marshtomp : &ralts);
        LilithPlayer(SPECIES_PACHIRISU, &sLilithPachirisu);
        LilithOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0)
                MOVE(playerLeft, MOVE_MUDDY_WATER, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else
                MOVE(playerLeft, MOVE_DAZZLING_GLEAM, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        LilithResult(20 + board, turns, 0);
        if (board == 0 && turns == 1) {
            EXPECT_EQ(gLastMoves[1], MOVE_FAKE_OUT);
            EXPECT_EQ(gLastMoves[3], MOVE_DRAGON_DANCE);
            EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        }
        if (board == 0 && turns == 2) {
            EXPECT_EQ(gLastMoves[3], MOVE_KNOCK_OFF);
            EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Lilith phazing: actual final pair with six player members")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++)
        PARAMETRIZE { turns = t; }
    GIVEN {
        LilithPlayer(SPECIES_TIMBURR, &sLilithTimburr);
        LilithPlayer(SPECIES_EEVEE, &sLilithEevee);
        LilithPlayer(SPECIES_MIENFOO, &gEmeraldChampionsDefaultBattleSets[SPECIES_MIENFOO]);
        LilithPlayer(SPECIES_NATU, &gEmeraldChampionsDefaultBattleSets[SPECIES_NATU]);
        LilithPlayer(SPECIES_MUDKIP, &gEmeraldChampionsDefaultBattleSets[SPECIES_MUDKIP]);
        LilithPlayer(SPECIES_PACHIRISU, &gEmeraldChampionsDefaultBattleSets[SPECIES_PACHIRISU]);
        LilithOpponent(2);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BULK_UP);
            MOVE(playerRight, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            SEND_OUT(playerRight, 2);
        }
        if (turns >= 2) TURN {
            MOVE(playerLeft, MOVE_BULK_UP);
            MOVE(playerRight, MOVE_PROTECT);
        }
        if (turns == 3) TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_KNOCK_OFF, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        LilithResult(30, turns, 2);
        if (turns == 3) {
            EXPECT_EQ(gLastMoves[3], MOVE_CIRCLE_THROW);
            EXPECT_NE(gBattlerPartyIndexes[2], 2);
            EXPECT(GetMonData(&gParties[B_TRAINER_PLAYER][2], MON_DATA_HP) > 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Lilith pivot: middle pair with living original leads")
{
    u32 turns = 0, injured = 0;
    for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (b ? 2 : 3); t++)
            PARAMETRIZE { turns = t; injured = b; }
    GIVEN {
        LilithPlayer(SPECIES_EEVEE, &sLilithEevee);
        LilithPlayer(SPECIES_TIMBURR, &sLilithTimburr);
        LilithOpponent(0);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        for (u32 slot = 0; slot < 2; slot++) {
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + 2];
            party[slot + 2] = saved;
        }
        if (injured) {
            u16 hp = 20;
            SetMonData(&party[0], MON_DATA_HP, &hp);
        }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        LilithResult(40 + injured, turns, 1);
    }
}


AI_DOUBLE_BATTLE_TEST("EC live review Lilith core: physical special leads and reserves")
{
    u32 board = 0, turns = 0, reserves = 0;
    for (u32 p = 0; p < 3; p++)
        for (u32 b = 0; b < 2; b++)
            for (u32 t = 1; t <= 3; t++)
                PARAMETRIZE { board = b; turns = t; reserves = p; }
    GIVEN {
        LilithPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_NATU, board == 0 ? &sLilithEevee : &sLilithNatu);
        LilithPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU, board == 0 ? &sLilithTimburr : &sLilithPachirisu);
        LilithOpponent(reserves);
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
        LilithResult(board, turns, reserves);
        if (reserves == 0 && board == 1 && turns == 1) {
            EXPECT_EQ(gLastMoves[1], MOVE_FAKE_OUT);
            EXPECT_EQ(gLastMoves[3], MOVE_KNOCK_OFF);
            EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(playerLeft->hp, 0);
        }
        if (reserves == 1 && board == 1 && turns == 1) {
            EXPECT_EQ(opponentLeft->species, SPECIES_MEDITITE);
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP), 57);
            EXPECT(opponentLeft->hp > 0);
        }
        if (reserves == 2 && board == 0 && turns == 1) {
            EXPECT_EQ(gLastMoves[1], MOVE_DRAIN_PUNCH);
            EXPECT(opponentLeft->hp > 0);
        }
        if (reserves == 2 && board == 0 && turns == 3) {
            EXPECT_EQ(playerLeft->hp, 0);
            EXPECT_EQ(playerRight->hp, 0);
            EXPECT(opponentLeft->hp > 0);
        }

    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Lilith disruption: Fake Out Taunt and fast Electric pressure")
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
        LilithPlayer(board == 0 ? SPECIES_MIENFOO : SPECIES_ELECTRIKE, board == 0 ? &sLilithMienfoo : &electrike);
        LilithPlayer(SPECIES_EEVEE, &sLilithEevee);
        LilithOpponent(0);
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
        LilithResult(10 + board, turns, 0);

    }
}
