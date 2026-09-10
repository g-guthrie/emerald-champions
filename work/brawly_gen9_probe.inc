// Temporary actual E0040 native observations; independent of prior encounters.
#line 51000 "work/brawly_gen9_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void BrawlyPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sBrawlyNatu = {
    .moves = {MOVE_PSYCHIC, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE,
    .evs = {4, 0, 0, 252, 0, 252},
};
static const struct EmeraldChampionsBattleSet sBrawlyEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sBrawlyTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sBrawlyPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sBrawlyLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sBrawlyMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};



static void BrawlyOpponent(u32 reserves)
{
    AuthoredOpponent(TRAINER_BRAWLY_1, 1, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species expected[] = {SPECIES_PACHIRISU, SPECIES_HERACROSS, SPECIES_CONKELDURR, SPECIES_HARIYAMA, SPECIES_HAWLUCHA, SPECIES_COBALION};
    for (u32 slot = 0; slot < ARRAY_COUNT(expected); slot++)
    {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), expected[slot]);
        Test_MgbaPrintf("BRAWLY_INPUT reserves=%d slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
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


static void BrawlyResult(u32 board, u32 turns, u32 reserves)
{
    Test_MgbaPrintf("BRAWLY_ENGINE board=%d turns=%d pair=%d status=%d/%d items=%d/%d unburden=%d/%d", board, turns, reserves, gBattleMons[1].status1, gBattleMons[3].status1, gBattleMons[1].item, gBattleMons[3].item, (u32)gBattleMons[1].volatiles.unburdenActive, (u32)gBattleMons[3].volatiles.unburdenActive);
    Test_MgbaPrintf("BRAWLY_AFTER board=%d turns=%d pair=%d native=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speeds=%d/%d foeSpeeds=%d/%d Atk=%d/%d Def=%d/%d PP=%d/%d faints=%d/%d",
        board, turns, reserves, gBattleResults.battleTurnCounter,
        gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[0].hp, gBattleMons[2].hp, GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK],
        gBattleMons[1].statStages[STAT_DEF], gBattleMons[3].statStages[STAT_DEF],
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}


AI_DOUBLE_BATTLE_TEST("EC live review Brawly core: physical special leads and reserves")
{
    u32 board = 0, turns = 0, reserves = 0;
    for (u32 p = 0; p < 3; p++)
        for (u32 b = 0; b < 2; b++)
            for (u32 t = 1; t <= (p == 2 && b == 1 ? 4 : 2); t++)
                PARAMETRIZE { board = b; turns = t; reserves = p; }
    GIVEN {
        BrawlyPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_NATU, board == 0 ? &sBrawlyEevee : &sBrawlyNatu);
        BrawlyPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU, board == 0 ? &sBrawlyTimburr : &sBrawlyPachirisu);
        BrawlyOpponent(reserves);
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
        BrawlyResult(board, turns, reserves);
        if (reserves == 0)
        {
            EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
            if (turns == 2)
                EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
        }
        if (reserves == 1)
        {
            EXPECT(opponentLeft->status1 & STATUS1_BURN);
            if (turns == 2)
                EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
        }
        if (reserves == 2 && board == 0)
        {
            EXPECT_EQ(opponentLeft->item, ITEM_NONE);
            EXPECT((u32)opponentLeft->volatiles.unburdenActive);
            EXPECT_EQ(gAiLogicData->speedStats[1], 168);
            EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
            if (turns == 2)
            {
                EXPECT_EQ(gLastMoves[1], MOVE_ACROBATICS);
                EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
            }
        }
        if (reserves == 2 && board == 1 && turns == 4)
            EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Brawly disruption: Fake Out and Fairy spread")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (b == 0 ? 3 : 2); t++)
            PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet ralts = {
            .moves = {MOVE_DAZZLING_GLEAM, MOVE_PSYCHIC, MOVE_TRICK_ROOM, MOVE_PROTECT},
            .item = ITEM_CHOICE_SCARF, .nature = NATURE_TIMID, .ability = ABILITY_TRACE,
            .evs = {4, 0, 0, 252, 0, 252},
        };
        BrawlyPlayer(board == 0 ? SPECIES_MIENFOO : SPECIES_RALTS, board == 0 ? &sBrawlyMienfoo : &ralts);
        BrawlyPlayer(SPECIES_PACHIRISU, &sBrawlyPachirisu);
        BrawlyOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
            } else
                MOVE(playerLeft, MOVE_DAZZLING_GLEAM, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        BrawlyResult(10 + board, turns, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Brawly motif: faster Flying pressure and late Fake Out")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (b == 0 ? 2 : 3); t++)
            PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet scarfNatu = sBrawlyNatu;
        scarfNatu.item = ITEM_CHOICE_SCARF;
        BrawlyPlayer(board == 0 ? SPECIES_NATU : SPECIES_MIENFOO, board == 0 ? &scarfNatu : &sBrawlyMienfoo);
        BrawlyPlayer(SPECIES_PACHIRISU, &sBrawlyPachirisu);
        BrawlyOpponent(board == 0 ? 0 : 2);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0)
                MOVE(playerLeft, MOVE_AIR_SLASH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else if (turn == 0)
                MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else
                MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: board == 0 ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        BrawlyResult(20 + board, turns, board == 0 ? 0 : 2);
        if (board == 0)
        {
            EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
            EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
            EXPECT_EQ(opponentLeft->pp[0], GetMovePP(MOVE_FOLLOW_ME) - 1);
            if (turns == 1)
                EXPECT_EQ(gLastMoves[1], MOVE_FOLLOW_ME);
        }
        if (board == 1 && turns == 3)
            EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
    }
}
