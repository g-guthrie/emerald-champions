// Temporary actual E0038 native observations; independent of prior encounters.
#line 51000 "work/cristian_gen9_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void CristianPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sCristianNatu = {
    .moves = {MOVE_PSYCHIC, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE,
    .evs = {4, 0, 0, 252, 0, 252},
};
static const struct EmeraldChampionsBattleSet sCristianEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sCristianTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sCristianPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sCristianLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sCristianMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};



static void CristianOpponent(u32 reserves)
{
    AuthoredOpponent(TRAINER_CRISTIAN, 1, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species expected[] = {SPECIES_FALINKS, SPECIES_GALLADE, SPECIES_LUCARIO, SPECIES_HITMONCHAN, SPECIES_MACHAMP, SPECIES_ANNIHILAPE};
    for (u32 slot = 0; slot < ARRAY_COUNT(expected); slot++)
    {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), expected[slot]);
        Test_MgbaPrintf("CRISTIAN_INPUT reserves=%d slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
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


static void CristianResult(u32 board, u32 turns, u32 reserves)
{
    Test_MgbaPrintf("CRISTIAN_AFTER board=%d turns=%d pair=%d native=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speeds=%d/%d foeSpeeds=%d/%d Atk=%d/%d Def=%d/%d PP=%d/%d faints=%d/%d",
        board, turns, reserves, gBattleResults.battleTurnCounter,
        gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[0].hp, gBattleMons[2].hp, GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK],
        gBattleMons[1].statStages[STAT_DEF], gBattleMons[3].statStages[STAT_DEF],
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}

AI_DOUBLE_BATTLE_TEST("EC live review Cristian core: physical special leads and reserves")
{
    u32 board = 0, turns = 0, reserves = 0;
    for (u32 p = 0; p < 3; p++)
        for (u32 b = 0; b < 2; b++)
            for (u32 t = 1; t <= 2; t++)
                PARAMETRIZE { board = b; turns = t; reserves = p; }
    GIVEN {
        CristianPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_NATU, board == 0 ? &sCristianEevee : &sCristianNatu);
        CristianPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU, board == 0 ? &sCristianTimburr : &sCristianPachirisu);
        CristianOpponent(reserves);
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
        CristianResult(board, turns, reserves);
        if (reserves == 0 && board == 1)
        {
            EXPECT_EQ(opponentLeft->hp, 1);
            EXPECT_EQ(opponentLeft->item, ITEM_NONE);
            EXPECT_EQ(opponentRight->statStages[STAT_ATK], MAX_STAT_STAGE);
            EXPECT_EQ(opponentLeft->pp[0], GetMovePP(MOVE_BEAT_UP) - 1);
            EXPECT_EQ(playerRight->hp, 0);
            if (turns == 2)
                EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
        }
        if (reserves == 2 && board == 0)
        {
            EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
            if (turns == 2)
                EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Cristian activation: Lucario fallback and support disruption")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 4; b++)
        for (u32 t = 1; t <= 2; t++)
            PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        CristianPlayer(board == 1 ? SPECIES_MIENFOO : SPECIES_EEVEE, board == 1 ? &sCristianMienfoo : &sCristianEevee);
        CristianPlayer(SPECIES_PACHIRISU, &sCristianPachirisu);
        CristianOpponent(0);
        if (board == 0 || board == 3) {
            struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
            u16 zero = 0;
            struct Pokemon gallade = party[1];
            SetMonData(&gallade, MON_DATA_HP, &zero);
            party[1] = party[2];
            party[2] = gallade;
        }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 1) {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
            } else
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            if (board >= 2)
                MOVE(playerRight, MOVE_FOLLOW_ME);
            else
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        CristianResult(10 + board, turns, 0);
        if (board == 0 && turns == 2)
        {
            EXPECT_EQ(opponentRight->species, SPECIES_LUCARIO);
            EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 5);
            EXPECT_EQ(opponentLeft->pp[0], GetMovePP(MOVE_BEAT_UP) - 1);
            EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
            EXPECT(opponentRight->hp > 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Cristian activation: status removes one Beat Up donor")
{
    GIVEN {
        CristianPlayer(SPECIES_NATU, &sCristianNatu);
        CristianPlayer(SPECIES_PACHIRISU, &sCristianPachirisu);
        CristianOpponent(0);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        u32 status = STATUS1_BURN;
        SetMonData(&party[4], MON_DATA_STATUS, &status);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        CristianResult(20, 1, 0);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 5);
        EXPECT_EQ(opponentLeft->pp[0], GetMovePP(MOVE_BEAT_UP) - 1);
        EXPECT(opponentRight->hp > 0);
    }
}
