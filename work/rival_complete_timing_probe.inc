// Temporary E0001 native timing diagnostic. Include after RivalReviewPlayer
// and AuthoredOpponent; this is not part of the retained regression suite.
#line 41000 "work/rival_complete_timing_probe.inc"
#include "battle_ai_main.h"
#include "battle_gimmick.h"
#include "battle_util.h"
#include "main.h"
#include "random.h"
#include "config/ai.h"

AI_DOUBLE_BATTLE_TEST("EC live review E0001 timing: complete cold decision with six-player party")
{
    u16 trainer;
    PARAMETRIZE { trainer = TRAINER_BRENDAN_ROUTE_103_MUDKIP; }
    PARAMETRIZE { trainer = TRAINER_BRENDAN_ROUTE_103_TORCHIC; }
    PARAMETRIZE { trainer = TRAINER_BRENDAN_ROUTE_103_TREECKO; }
    GIVEN {
        // Every player member uses its current native default battle preset.
        // The first two can safely spend the setup turn on moves they know.
        RivalReviewPlayer(SPECIES_MIENFOO);
        RivalReviewPlayer(SPECIES_NATU);
        RivalReviewPlayer(SPECIES_MUDKIP);
        RivalReviewPlayer(SPECIES_TREECKO);
        RivalReviewPlayer(SPECIES_PACHIRISU);
        RivalReviewPlayer(SPECIES_EEVEE);
        AuthoredOpponent(trainer, 0, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        // Match the production turn boundary before action selection. The
        // candidate evaluator owns its required snapshots; an extra outer
        // snapshot has exhausted the native test heap on six-member parties.
        u32 start = gMain.vblankCounter1;
        AssignUsableGimmicks();
        SetShellSideArmCategory();
        SetAiLogicDataForTurn(gAiLogicData);
        gAiLogicData->reverseBattlerLogicOrder = RandomPercentage(
            RNG_AI_REVERSE_BATTLER_LOGIC_ORDER,
            GetConfig(AI_REVERSE_BATTLER_LOGIC_ORDER_CHANCE)) && IsDoubleBattle();
        u32 setupFrames = gMain.vblankCounter1 - start;

        // Follow the same randomized flank order as action selection. Both
        // wrappers are included so the second actor's scored-pair fast path is
        // part of the complete decision measurement.
        enum BattlerId first = gAiLogicData->reverseBattlerLogicOrder
            ? B_BATTLER_3 : B_BATTLER_1;
        enum BattlerId second = GetPartnerBattler(first);
        u32 decisionStart = gMain.vblankCounter1;
        ComputeAiBattlerDecisions(first);
        ComputeAiBattlerDecisions(second);
        u32 decisionFrames = gMain.vblankCounter1 - decisionStart;
        u32 completeFrames = gMain.vblankCounter1 - start;

        // Optional warmed comparison: retain the cold-built cache and rerun
        // the same complete two-actor wrapper path without changing the board.
        gAiLogicData->battlerMovesScored = 0;
        u32 warmStart = gMain.vblankCounter1;
        ComputeAiBattlerDecisions(first);
        ComputeAiBattlerDecisions(second);
        u32 warmFrames = gMain.vblankCounter1 - warmStart;

        Test_MgbaPrintf("E1_COMPLETE_TIMING trainer=%d setupFrames=%d decisionFrames=%d completeFrames=%d warmFrames=%d first=%d moves=%S/%S",
            trainer, setupFrames, decisionFrames, completeFrames, warmFrames, first,
            GetMoveName(gBattleMons[B_BATTLER_1].moves[gAiBattleData->chosenMoveIndex[B_BATTLER_1]]),
            GetMoveName(gBattleMons[B_BATTLER_3].moves[gAiBattleData->chosenMoveIndex[B_BATTLER_3]]));
    }
}
