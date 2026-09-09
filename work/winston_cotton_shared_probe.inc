// Temporary shared-planner observations, deliberately SYNTHETIC explicit stats.
// No authored trainer ID, authored SETUP opinion, or native party helper.
// Wooloo and Munchlax's named comparison stats reproduce the earlier native
// inputs; the passive partner stats below are explicit synthetic controls.
#line 49000 "work/winston_cotton_shared_probe.inc"
#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

AI_DOUBLE_BATTLE_TEST("EC live review Winston Cotton shared: synthetic unbonused physical speed and special boundaries")
{
    u32 board;
    PARAMETRIZE { board = 0; } // Slow physical pressure.
    PARAMETRIZE { board = 1; } // Fast physical pressure, with the native HP/Def trade.
    PARAMETRIZE { board = 2; } // Slow special pressure, publicly different menu.
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO
            | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_LOTAD) {
            Level(14); HP(100); MaxHP(100); Attack(100); Defense(100);
            SpAttack(100); SpDefense(100); Speed(10);
            Ability(ABILITY_RAIN_DISH); Item(ITEM_EVIOLITE); Moves(MOVE_PROTECT);
        }
        PLAYER(SPECIES_MUNCHLAX) {
            Level(14); HP(board == 1 ? 82 : 98); MaxHP(board == 1 ? 82 : 98);
            Attack(49); Defense(board == 1 ? 20 : 22); SpAttack(39); SpDefense(37);
            Speed(board == 1 ? 25 : 9); Nature(NATURE_QUIET);
            Ability(ABILITY_THICK_FAT); Item(ITEM_EVIOLITE);
            Moves(board == 2 ? MOVE_ICE_BEAM : MOVE_BODY_SLAM);
        }
        OPPONENT(SPECIES_GIMMIGHOUL) {
            Level(12); HP(100); MaxHP(100); Attack(100); Defense(100);
            SpAttack(100); SpDefense(100); Speed(10);
            Ability(ABILITY_RATTLED); Item(ITEM_FOCUS_SASH); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_WOOLOO) {
            Level(12); HP(67); MaxHP(67); Attack(55); Defense(23);
            SpAttack(18); SpDefense(19); Speed(18); Nature(NATURE_BRAVE);
            Ability(ABILITY_FLUFFY); Item(ITEM_LEFTOVERS);
            Moves(MOVE_COTTON_GUARD, MOVE_DOUBLE_EDGE);
        }
        Test_MgbaPrintf("WINSTON_COTTON_SHARED_INPUT board=%d synthetic=1 WoolHP=67 Atk=55 Def=23 SpA=18 SpD=19 Speed=18 MunchHP=%d Atk=49 Def=%d SpA=39 SpD=37 Speed=%d move=%S partnersHP=100 partnersDef=100",
            board, board == 1 ? 82 : 98, board == 1 ? 20 : 22, board == 1 ? 25 : 9,
            GetMoveName(board == 2 ? MOVE_ICE_BEAM : MOVE_BODY_SLAM));
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, board == 2 ? MOVE_ICE_BEAM : MOVE_BODY_SLAM, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            EXPECT_MOVE(opponentRight, board == 0 ? MOVE_COTTON_GUARD : MOVE_DOUBLE_EDGE);
        }
    } THEN {
        EXPECT_EQ(opponentRight->statStages[STAT_DEF], board == 0 ? DEFAULT_STAT_STAGE + 3 : DEFAULT_STAT_STAGE);
        EXPECT_GT(opponentRight->hp, 0);
        if (board == 0)
            EXPECT_EQ(playerRight->hp, playerRight->maxHP);
        else
            EXPECT_LT(playerRight->hp, playerRight->maxHP);
        Test_MgbaPrintf("WINSTON_COTTON_SHARED_AFTER board=%d synthetic=1 WoolHP=%d/%d MunchHP=%d/%d LotadHP=%d GimmiHP=%d LAST=%S/%S foeLAST=%S WoolDefStage=%d status=%d faints=%d/%d room=%d",
            board, opponentRight->hp, opponentRight->maxHP, playerRight->hp, playerRight->maxHP,
            playerLeft->hp, opponentLeft->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_2]), opponentRight->statStages[STAT_DEF], opponentRight->status1,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter,
            (gFieldStatuses & STATUS_FIELD_TRICK_ROOM) != 0);
        // Execution is not a regression proof. Compare actual selections with
        // the defense cache disabled before deciding whether this is distinct.
    }
}
