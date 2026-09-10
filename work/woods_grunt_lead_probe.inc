// Temporary actual-party E0010 decisions; not a permanent team lock.
#line 10000 "work/woods_grunt_lead_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"
static void WoodsLeadPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
              GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}
static const struct EmeraldChampionsBattleSet sWoodsEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 252, 4, 0, 0, 0},
};
static const struct EmeraldChampionsBattleSet sWoodsTimburr = {
    .moves = {MOVE_ICE_PUNCH, MOVE_DRAIN_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sWoodsLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_RAIN_DANCE, MOVE_ICE_BEAM, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 136, 120, 0, 0},
};
static const struct EmeraldChampionsBattleSet sWoodsPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sWoodsMienfoo = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {136, 120, 0, 0, 0, 252},
};
AI_DOUBLE_BATTLE_TEST("EC live review Woods grunt leads: physical special and opening denial")
{
    u32 board, turns, speedPoints;
    PARAMETRIZE { board = 0; turns = 1; speedPoints = 0; }
    PARAMETRIZE { board = 0; turns = 2; speedPoints = 0; }
    PARAMETRIZE { board = 1; turns = 1; speedPoints = 0; }
    PARAMETRIZE { board = 1; turns = 2; speedPoints = 0; }
    PARAMETRIZE { board = 2; turns = 1; speedPoints = 0; }
    PARAMETRIZE { board = 2; turns = 2; speedPoints = 0; }
    GIVEN {
        WoodsLeadPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_LOTAD : SPECIES_MIENFOO,
            board == 0 ? &sWoodsEevee : board == 1 ? &sWoodsLotad : &sWoodsMienfoo);
        WoodsLeadPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU,
            board == 0 ? &sWoodsTimburr : &sWoodsPachirisu);
        AuthoredOpponent(TRAINER_GRUNT_PETALBURG_WOODS, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_CARVANHA);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_WAILMER);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_GRIMER);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_POOCHYENA);
        Test_MgbaPrintf("WOODS_LEAD_CANDIDATE speedPoints=%d", speedPoints);
        for (u32 slot = 0; slot < 4; slot++) {
            struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][slot];
            for (u32 moveSlot = 0; moveSlot < MAX_MON_MOVES; moveSlot++)
                EXPECT(CanSpeciesUseEmeraldChampionsPreparationMove(GetMonData(mon, MON_DATA_SPECIES),
                    GetMonData(mon, MON_DATA_MOVE1 + moveSlot)));
            Test_MgbaPrintf("WOODS_LEAD_INPUT board=%d slot=%d species=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d",
                board, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED));
        }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                MOVE(playerLeft, turn == 0 ? MOVE_FAKE_OUT : MOVE_DRAIN_PUNCH,
                    target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, turn == 0 ? MOVE_SUPER_FANG : MOVE_THUNDERBOLT,
                    target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("WOODS_LEAD_AFTER speedPoints=%d board=%d turns=%d species=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S SpeedStages=%d/%d foeSpeed=%d/%d party=%d/%d",
            speedPoints, board, turns, opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED],
            playerLeft->statStages[STAT_SPEED], playerRight->statStages[STAT_SPEED],
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3]);
        Test_MgbaPrintf("WOODS_LEAD_PARTY board=%d turns=%d Carvanha=%d Wailmer=%d Grimer=%d Poochyena=%d",
            board, turns, GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][3], MON_DATA_HP));
    }
}
