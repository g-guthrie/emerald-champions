// Temporary actual-party Tiana baseline; include after AuthoredOpponent.
#line 22000 "work/tiana_ai_probe.inc"
#include "emerald_champions_battle_sets.h"

static void TianaProbePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species)
    {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon));
        Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
              GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

static const struct EmeraldChampionsBattleSet sTianaProbePachirisu = {
    .moves = {MOVE_ROUND, MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .statPoints = {32, 0, 32, 0, 2, 0},
};
static const struct EmeraldChampionsBattleSet sTianaProbeEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
    .statPoints = {32, 32, 2, 0, 0, 0},
};
static const struct EmeraldChampionsBattleSet sTianaProbeMienfoo = {
    .moves = {MOVE_STRENGTH, MOVE_DRAIN_PUNCH, MOVE_FAKE_OUT, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .statPoints = {18, 16, 0, 0, 0, 32},
};
static const struct EmeraldChampionsBattleSet sTianaProbeNatu = {
    .moves = {MOVE_PSYCHIC, MOVE_REFLECT, MOVE_ROOST, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_MAGIC_BOUNCE,
    .statPoints = {32, 0, 2, 0, 0, 32},
};

AI_DOUBLE_BATTLE_TEST("EC live review Tiana: actual lead support and natural berry continuation")
{
    bool32 berryBoard;
    u32 turns;
    PARAMETRIZE { berryBoard = FALSE; turns = 1; }
    PARAMETRIZE { berryBoard = FALSE; turns = 2; }
    PARAMETRIZE { berryBoard = TRUE; turns = 1; }
    PARAMETRIZE { berryBoard = TRUE; turns = 2; }
    GIVEN {
        if (berryBoard)
        {
            TianaProbePlayer(SPECIES_MIENFOO, &sTianaProbeMienfoo);
            TianaProbePlayer(SPECIES_NATU, &sTianaProbeNatu);
        }
        else
        {
            TianaProbePlayer(SPECIES_PACHIRISU, &sTianaProbePachirisu);
            TianaProbePlayer(SPECIES_EEVEE, &sTianaProbeEevee);
        }
        AuthoredOpponent(TRAINER_TIANA, 0, FALSE);
        struct Pokemon *players = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
        struct Pokemon *enemies = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&enemies[0], MON_DATA_SPECIES), SPECIES_SKITTY);
        EXPECT_EQ(GetMonData(&enemies[1], MON_DATA_SPECIES), SPECIES_SWIRLIX);
        if (berryBoard)
        {
            EXPECT_GT(GetMonData(&players[0], MON_DATA_SPEED), GetMonData(&players[1], MON_DATA_SPEED));
            EXPECT_GT(GetMonData(&players[1], MON_DATA_SPEED), GetMonData(&enemies[1], MON_DATA_SPEED));
            EXPECT_LT(GetMonData(&players[0], MON_DATA_SPEED), GetMonData(&enemies[1], MON_DATA_SPEED) * 2);
        }
        else
        {
            EXPECT_LT(GetMonData(&players[0], MON_DATA_SPEED), GetMonData(&enemies[0], MON_DATA_SPEED));
            EXPECT_LT(GetMonData(&players[1], MON_DATA_SPEED), GetMonData(&enemies[0], MON_DATA_SPEED));
        }
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("TIANA_INPUT berry=%d turns=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                    berryBoard, turns, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
            }
        // Route101 Eevee/Mienfoo and Route102 Pachirisu/Natu, using native
        // preparation moves and real level-14 stats. No bespoke HP override.
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++)
        {
            TURN {
                if (berryBoard)
                {
                    MOVE(playerLeft, MOVE_STRENGTH, target: opponentRight, secondaryEffect: FALSE);
                    MOVE(playerRight, MOVE_PSYCHIC, target: opponentLeft, secondaryEffect: FALSE);
                }
                else
                {
                    MOVE(playerLeft, MOVE_ROUND, target: opponentRight, secondaryEffect: FALSE);
                    MOVE(playerRight, MOVE_DOUBLE_EDGE, target: opponentRight, secondaryEffect: FALSE);
                }
                // Native AI remains unrestricted. The second turn observes
                // support after Fake Out expires, not a mandatory Sing/HH.
            }
        }
    } THEN {
        Test_MgbaPrintf("TIANA_AFTER berry=%d requestedTurns=%d nativeTurns=%d LAST=%S/%S party=%d/%d species=%d/%d HP=%d/%d item=%d/%d unburden=%d/%d playerHP=%d/%d playerSleep=%d/%d",
            berryBoard, turns, gBattleResults.battleTurnCounter,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            opponentLeft->item, opponentRight->item,
            opponentLeft->volatiles.unburdenActive, opponentRight->volatiles.unburdenActive,
            playerLeft->hp, playerRight->hp, playerLeft->status1 & STATUS1_SLEEP, playerRight->status1 & STATUS1_SLEEP);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("TIANA_PARTY berry=%d turns=%d slot=%d species=%d HP=%d/%d item=%d status=%d",
                berryBoard, turns, slot,
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MAX_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HELD_ITEM),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_STATUS));
        // Decision-time trace, not THEN's next-turn cache, owns selected moves.
        // Diagnostic execution alone does not prove support or optimal play.
    }
}
