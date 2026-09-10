// Temporary E0012 actual-party observations. No Trick Room, setup, attack,
// switch or victory is prescribed for the AI. Include with AuthoredOpponent.
#line 41000 "work/winston_lead_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void WinstonLeadPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sWinstonEevee = {
    .moves = {MOVE_BITE, MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 252, 0, 0, 0},
};
static const struct EmeraldChampionsBattleSet sWinstonTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF, MOVE_ICE_PUNCH, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sWinstonLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_RAIN_DANCE, MOVE_ICE_BEAM, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 136, 120, 0, 0},
};
static const struct EmeraldChampionsBattleSet sWinstonCharmander = {
    .moves = {MOVE_FLAMETHROWER, MOVE_DRAGON_PULSE, MOVE_WILL_O_WISP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_BLAZE,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sWinstonMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF, MOVE_TAUNT},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 120, 0, 0, 136},
};
static const struct EmeraldChampionsBattleSet sWinstonPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};

AI_DOUBLE_BATTLE_TEST("EC live review Winston leads: physical special and Fake Out denial")
{
    u32 board, turns;
    PARAMETRIZE { board = 0; turns = 1; }
    PARAMETRIZE { board = 0; turns = 2; }
    PARAMETRIZE { board = 0; turns = 3; }
    PARAMETRIZE { board = 0; turns = 4; }
    PARAMETRIZE { board = 1; turns = 1; }
    PARAMETRIZE { board = 1; turns = 2; }
    PARAMETRIZE { board = 1; turns = 3; }
    PARAMETRIZE { board = 2; turns = 1; }
    PARAMETRIZE { board = 2; turns = 2; }
    GIVEN {
        WinstonLeadPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_LOTAD : SPECIES_MIENFOO,
            board == 0 ? &sWinstonEevee : board == 1 ? &sWinstonLotad : &sWinstonMienfoo);
        WinstonLeadPlayer(board == 0 ? SPECIES_TIMBURR : board == 1 ? SPECIES_CHARMANDER : SPECIES_PACHIRISU,
            board == 0 ? &sWinstonTimburr : board == 1 ? &sWinstonCharmander : &sWinstonPachirisu);
        AuthoredOpponent(TRAINER_WINSTON_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PORYGON);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_HONEDGE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_GIMMIGHOUL);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_WOOLOO);
        for (u32 slot = 0; slot < 4; slot++)
            for (u32 moveSlot = 0; moveSlot < MAX_MON_MOVES; moveSlot++)
                EXPECT(CanSpeciesUseEmeraldChampionsPreparationMove(GetMonData(&party[slot], MON_DATA_SPECIES),
                    GetMonData(&party[slot], MON_DATA_MOVE1 + moveSlot)));
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("WINSTON_INPUT board=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                    board, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0)
            {
                MOVE(playerLeft, MOVE_BITE, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            }
            else if (board == 1)
            {
                MOVE(playerLeft, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_FLAMETHROWER, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
            else
            {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("WINSTON_AFTER board=%d turns=%d nativeTurns=%d room=%d roomTimer=%d active=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S ownAtk=%d/%d ownSpA=%d/%d ownDef=%d/%d SpeedStages=%d/%d taunt=%d/%d ownFaints=%d foeFaints=%d",
            board, turns, gBattleResults.battleTurnCounter, (gFieldStatuses & STATUS_FIELD_TRICK_ROOM) != 0,
            gFieldTimers.trickRoomTimer, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->statStages[STAT_ATK], opponentRight->statStages[STAT_ATK],
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            opponentLeft->statStages[STAT_DEF], opponentRight->statStages[STAT_DEF],
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED],
            opponentLeft->volatiles.tauntTimer, opponentRight->volatiles.tauntTimer,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("WINSTON_PARTY board=%d turns=%d slot=%d species=%d HP=%d/%d status=%d",
                board, turns, slot, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MAX_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_STATUS));
    }
}
