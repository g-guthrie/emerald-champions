// Temporary shared-mechanics observations with SYNTHETIC explicit stats.
// Adapt the existing shared offensive-drop regression's physical threat and
// slow Psychic finisher: this is not Gina & Mia's authored roster or balance.
#line 62000 "work/gina_mia_nuzzle_shared_probe.inc"
#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

AI_DOUBLE_BATTLE_TEST("EC live review Gina Nuzzle shared: speed crossing and per-slot secondary immunity")
{
    u32 boundary;
    PARAMETRIZE { boundary = 0; } // Nuzzle is the available speed-control move.
    PARAMETRIZE { boundary = 1; } // Cloak blocks Nuzzle's secondary, not damage.
    PARAMETRIZE { boundary = 2; } // Same Cloak target; TW is now a separate slot.
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO
            | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_TAUROS) {
            Level(50); HP(100); MaxHP(100); Attack(300); SpAttack(210);
            Defense(100); SpDefense(100); Speed(80); Ability(ABILITY_ANGER_POINT);
            Item(boundary ? ITEM_COVERT_CLOAK : ITEM_NONE); Moves(MOVE_TACKLE);
        }
        PLAYER(SPECIES_CHANSEY) {
            Level(50); HP(300); MaxHP(300); Attack(100); SpAttack(100);
            Defense(300); SpDefense(300); Speed(10);
            Ability(ABILITY_NATURAL_CURE); Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_SMEARGLE) {
            Level(50); HP(300); MaxHP(300); Attack(60); SpAttack(10);
            Defense(300); SpDefense(300); Speed(100); Ability(ABILITY_OWN_TEMPO);
            Moves(MOVE_NUZZLE, MOVE_STRENGTH, boundary == 2 ? MOVE_THUNDER_WAVE : MOVE_NONE);
        }
        OPPONENT(SPECIES_ORANGURU) {
            Level(50); HP(65); MaxHP(65); Attack(100); SpAttack(300);
            Defense(100); SpDefense(100); Speed(60); Ability(ABILITY_TELEPATHY);
            Moves(MOVE_PSYCHIC, MOVE_PROTECT);
        }
        // Before paralysis: Smeargle100 > Tauros80 > Oranguru60.
        // Modern native paralysis halves Tauros to40. No manually assigned
        // status, speed stage, cached damage, or pending human input is read.
        // Smeargle's native Sketch access permits these move combinations;
        // the explicit stat scale is solely a generic mechanical isolation.
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_CELEBRATE);
            // Observe unrestricted decisions before selecting assertions.
            // Intended boundary: Nuzzle payoff, no secondary payoff under
            // Cloak, then TW's distinct primary-status eligibility under Cloak.
        }
    } THEN {
        if (boundary != 1)
        {
            EXPECT_EQ(gLastMoves[B_BATTLER_1], boundary == 0 ? MOVE_NUZZLE : MOVE_THUNDER_WAVE);
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_PSYCHIC);
            EXPECT_EQ(playerLeft->hp, 0);
            EXPECT_GT(opponentRight->hp, 0);
        }
        else
        {
            EXPECT_NE(gLastMoves[B_BATTLER_1], MOVE_NUZZLE);
            EXPECT_GT(playerLeft->hp, 0);
        }
        Test_MgbaPrintf("GINA_NUZZLE_SHARED boundary=%d synthetic=1 HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S status=%d/%d Speed=%d/%d/%d faints=%d/%d",
            boundary, opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            playerLeft->status1, playerRight->status1, opponentLeft->speed, playerLeft->speed, opponentRight->speed,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        // A PASS is only execution at this stage. Require positive/disabled-
        // repair comparison before turning this into a retained regression.
    }
}
