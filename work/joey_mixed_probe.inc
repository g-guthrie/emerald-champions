// Temporary actual Joey mixed phase; include after JoeyPlayer.
// Promote original Zigzagoon to slot1 and retain Lechonk alive on the bench.
// No stat, item, ability, move, health or trainer-plan intervention.
#line 60000 "work/joey_mixed_probe.inc"
#include "emerald_champions_battle_plan.h"

AI_DOUBLE_BATTLE_TEST("EC live review Joey mixed: Pawmi supports healthy Zigzagoon")
{
    bool32 electric;
    u32 turns;
    PARAMETRIZE { electric = FALSE; turns = 1; }
    PARAMETRIZE { electric = FALSE; turns = 2; }
    PARAMETRIZE { electric = FALSE; turns = 3; }
    PARAMETRIZE { electric = TRUE; turns = 1; }
    PARAMETRIZE { electric = TRUE; turns = 2; }
    PARAMETRIZE { electric = TRUE; turns = 3; }
    GIVEN {
        JoeyPlayer(electric ? SPECIES_PACHIRISU : SPECIES_EEVEE,
            electric ? &sJoeyPachirisu : &sJoeyEevee);
        JoeyPlayer(SPECIES_LOTAD, &sJoeyLotad);
        AuthoredOpponent(TRAINER_JOEY, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PAWMI);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_LECHONK);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_CLEFAIRY);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_ZIGZAGOON);
        struct Pokemon saved = party[1];
        party[1] = party[3];
        party[3] = saved;
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_ZIGZAGOON);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_LECHONK);
        for (u32 slot = 0; slot < 4; slot++)
        {
            EXPECT_GT(GetMonData(&party[slot], MON_DATA_HP), 0);
            EXPECT_EQ(GetMonData(&party[slot], MON_DATA_HP), GetMonData(&party[slot], MON_DATA_MAX_HP));
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("JOEY_MIX_INPUT e=%d owner=%d slot=%d species=%d lv=%d HP=%d stats=%d/%d/%d/%d/%d item=%d ability=%d",
                    electric, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
                Test_MgbaPrintf("JOEY_MIX_MENU e=%d owner=%d slot=%d moves=%d/%d/%d/%d",
                    electric, owner, slot, GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                    GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (electric)
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("JOEY_MIX_END e=%d t=%d native=%d plan=%d active=%d/%d slots=%d/%d HP=%d/%d foes=%d/%d faints=%d/%d",
            electric, turns, gBattleResults.battleTurnCounter, EmeraldChampions_GetBattlePlan(B_BATTLER_1),
            opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        Test_MgbaPrintf("JOEY_MIX_MOVES e=%d t=%d own=%S/%S player=%S/%S",
            electric, turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]));
        Test_MgbaPrintf("JOEY_MIX_STATE e=%d t=%d Atk=%d/%d foeSpeed=%d/%d ownStatus=%d/%d foeStatus=%d/%d items=%d/%d",
            electric, turns, opponentLeft->statStages[STAT_ATK], opponentRight->statStages[STAT_ATK],
            playerLeft->statStages[STAT_SPEED], playerRight->statStages[STAT_SPEED],
            opponentLeft->status1, opponentRight->status1, playerLeft->status1, playerRight->status1,
            opponentLeft->item, opponentRight->item);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gParties[owner][slot];
                Test_MgbaPrintf("JOEY_MIX_PARTY e=%d t=%d owner=%d slot=%d species=%d HP=%d/%d item=%d status=%d",
                    electric, turns, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                    GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_HELD_ITEM), GetMonData(mon, MON_DATA_STATUS));
            }
        // This starts a hypothetical healthy mixed phase, not proof that an
        // unrestricted opening naturally reaches it. All ensuing choices are AI.
    }
}
