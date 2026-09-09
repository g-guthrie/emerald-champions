// Temporary actual E0014 lead observations. No trainer move is prescribed.
#line 51000 "work/gina_mia_lead_probe.inc"
#include "emerald_champions_battle_sets.h"

static void GinaMiaLeadPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sGinaMiaEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .statPoints = {32, 2, 0, 0, 32, 0},
};
static const struct EmeraldChampionsBattleSet sGinaMiaTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .statPoints = {32, 2, 0, 0, 32, 0},
};
static const struct EmeraldChampionsBattleSet sGinaMiaElectrike = {
    .moves = {MOVE_FLAMETHROWER, MOVE_THUNDERBOLT, MOVE_SNARL, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_LIGHTNING_ROD,
    .statPoints = {32, 0, 18, 16, 0, 0},
};
static const struct EmeraldChampionsBattleSet sGinaMiaShellos = {
    .moves = {MOVE_EARTH_POWER, MOVE_SCALD, MOVE_RECOVER, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_STORM_DRAIN,
    .statPoints = {32, 0, 0, 32, 2, 0},
};
static const struct EmeraldChampionsBattleSet sGinaMiaMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .statPoints = {32, 0, 2, 0, 0, 32},
};
static const struct EmeraldChampionsBattleSet sGinaMiaPachirisu = {
    .moves = {MOVE_SUPER_FANG, MOVE_THUNDERBOLT, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_CALM, .ability = ABILITY_VOLT_ABSORB,
    .statPoints = {32, 0, 2, 0, 32, 0},
};

AI_DOUBLE_BATTLE_TEST("EC live review Gina and Mia leads: neutral immunity and denial")
{
    u32 board, turns;
    PARAMETRIZE { board = 0; turns = 1; }
    PARAMETRIZE { board = 0; turns = 2; }
    PARAMETRIZE { board = 0; turns = 3; }
    PARAMETRIZE { board = 0; turns = 4; }
    PARAMETRIZE { board = 1; turns = 1; }
    PARAMETRIZE { board = 1; turns = 2; }
    PARAMETRIZE { board = 2; turns = 1; }
    PARAMETRIZE { board = 2; turns = 2; }
    GIVEN {
        GinaMiaLeadPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_ELECTRIKE : SPECIES_MIENFOO,
            board == 0 ? &sGinaMiaEevee : board == 1 ? &sGinaMiaElectrike : &sGinaMiaMienfoo);
        GinaMiaLeadPlayer(board == 0 ? SPECIES_TIMBURR : board == 1 ? SPECIES_SHELLOS : SPECIES_PACHIRISU,
            board == 0 ? &sGinaMiaTimburr : board == 1 ? &sGinaMiaShellos : &sGinaMiaPachirisu);
        // Route103 Shellos is Water, NOT Ground before evolution. This board
        // uses its legal Earth Power and Route103 Electrike's Lightning Rod.
        AuthoredOpponent(TRAINER_GINA_AND_MIA_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PLUSLE);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_MINUN);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_VOLBEAT);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_ILLUMISE);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("GINA_MIA_INPUT board=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d moves=%d/%d/%d/%d",
                    board, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon),
                    GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                    GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0)
            {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            }
            else if (board == 1)
            {
                MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_EARTH_POWER, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
            else
            {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
                MOVE(playerRight, MOVE_SUPER_FANG, target: opponentRight, hit: TRUE);
            }
        }
    } THEN {
        Test_MgbaPrintf("GINA_MIA_AFTER board=%d turns=%d nativeTurns=%d active=%d/%d slots=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S abilities=%d/%d ownSpA=%d/%d foeAtk=%d/%d foeSpA=%d/%d foeSpeed=%d/%d foeStatus=%d/%d taunt=%d/%d encore=%d/%d faints=%d/%d",
            board, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->ability, opponentRight->ability,
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            playerLeft->statStages[STAT_ATK], playerRight->statStages[STAT_ATK],
            playerLeft->statStages[STAT_SPATK], playerRight->statStages[STAT_SPATK],
            playerLeft->statStages[STAT_SPEED], playerRight->statStages[STAT_SPEED],
            playerLeft->status1, playerRight->status1,
            opponentLeft->volatiles.tauntTimer, opponentRight->volatiles.tauntTimer,
            playerLeft->volatiles.encoreTimer, playerRight->volatiles.encoreTimer,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("GINA_MIA_PARTY board=%d turns=%d slot=%d species=%d HP=%d/%d status=%d",
                board, turns, slot, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MAX_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_STATUS));
    }
}
