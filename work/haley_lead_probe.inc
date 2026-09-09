// Temporary E0013 actual-party observations, not a trainer lock or a forced
// support script. Include with AuthoredOpponent; all AI menus stay intact.
#line 51000 "work/haley_lead_probe.inc"
#include "emerald_champions_battle_sets.h"

static void HaleyLeadPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sHaleyPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .statPoints = {32, 0, 32, 0, 2, 0},
};
static const struct EmeraldChampionsBattleSet sHaleyTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
    .statPoints = {32, 0, 2, 0, 32, 0},
};
static const struct EmeraldChampionsBattleSet sHaleyFerroseed = {
    .moves = {MOVE_GYRO_BALL, MOVE_SEED_BOMB, MOVE_LEECH_SEED, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_BRAVE, .ability = ABILITY_IRON_BARBS,
    .statPoints = {32, 32, 2, 0, 0, 0},
};
static const struct EmeraldChampionsBattleSet sHaleyGrowlithe = {
    .moves = {MOVE_FLAMETHROWER, MOVE_SNARL, MOVE_WILL_O_WISP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_INTIMIDATE,
    .statPoints = {32, 0, 18, 16, 0, 0},
};
static const struct EmeraldChampionsBattleSet sHaleyMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .statPoints = {32, 0, 2, 0, 0, 32},
};

AI_DOUBLE_BATTLE_TEST("EC live review Haley leads: partner pressure Steel counterplay and Fake Out denial")
{
    u32 board, turns;
    PARAMETRIZE { board = 0; turns = 1; }
    PARAMETRIZE { board = 0; turns = 2; }
    PARAMETRIZE { board = 1; turns = 1; }
    PARAMETRIZE { board = 1; turns = 2; }
    PARAMETRIZE { board = 2; turns = 1; }
    PARAMETRIZE { board = 2; turns = 2; }
    GIVEN {
        // All are current early land choices: Route102 Pachi/Timburr,
        // first-visit Woods Ferroseed, Route103 Growlithe, Route101 Mienfoo.
        HaleyLeadPlayer(board == 0 ? SPECIES_PACHIRISU : board == 1 ? SPECIES_FERROSEED : SPECIES_MIENFOO,
            board == 0 ? &sHaleyPachirisu : board == 1 ? &sHaleyFerroseed : &sHaleyMienfoo);
        HaleyLeadPlayer(board == 0 ? SPECIES_TIMBURR : board == 1 ? SPECIES_GROWLITHE : SPECIES_PACHIRISU,
            board == 0 ? &sHaleyTimburr : board == 1 ? &sHaleyGrowlithe : &sHaleyPachirisu);
        AuthoredOpponent(TRAINER_HALEY_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_EEVEE);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_RALTS);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_MARILL);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_SNUBBULL);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("HALEY_INPUT board=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                    board, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0)
            {
                // Do not aim Electric at Ralts and assume Trace cannot copy
                // Volt Absorb. Its actual copied ability is logged below.
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
            else if (board == 1)
            {
                MOVE(playerLeft, MOVE_GYRO_BALL, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_FLAMETHROWER, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
            else
            {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("HALEY_AFTER board=%d turns=%d nativeTurns=%d active=%d/%d party=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S abilities=%d/%d ownAtk=%d/%d ownSpA=%d/%d ownSpeed=%d/%d foeSpeed=%d/%d foeStatus=%d/%d taunt=%d/%d wish=%d/%d ownFaints=%d foeFaints=%d",
            board, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->ability, opponentRight->ability,
            opponentLeft->statStages[STAT_ATK], opponentRight->statStages[STAT_ATK],
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED],
            playerLeft->statStages[STAT_SPEED], playerRight->statStages[STAT_SPEED],
            playerLeft->status1, playerRight->status1,
            opponentLeft->volatiles.tauntTimer, opponentRight->volatiles.tauntTimer,
            gBattleStruct->wish[B_BATTLER_1].counter, gBattleStruct->wish[B_BATTLER_3].counter,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("HALEY_PARTY board=%d turns=%d slot=%d species=%d HP=%d/%d status=%d",
                board, turns, slot, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MAX_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_STATUS));
    }
}
