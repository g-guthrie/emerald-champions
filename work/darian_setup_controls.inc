// Temporary native counterfactuals; no forced production AI policy.
#line 45000 "work/darian_setup_controls.inc"
#include "battle_main.h"
DOUBLE_BATTLE_TEST("EC live review Darian setup: native paralysis Dance payoff versus immediate attacks")
{
    u32 opening;
    PARAMETRIZE { opening = 0; }
    PARAMETRIZE { opening = 1; }
    PARAMETRIZE { opening = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet mienfoo = {
            .moves = {MOVE_DRAIN_PUNCH, MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
            .statPoints = {18, 16, 0, 0, 0, 32},
        };
        static const struct EmeraldChampionsBattleSet pachirisu = {
            .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        DarianLeadPlayer(SPECIES_MIENFOO, &mienfoo);
        DarianLeadPlayer(SPECIES_PACHIRISU, &pachirisu);
        AuthoredOpponent(TRAINER_DARIAN, 0, FALSE);
        // Historical Thunderbolt menu for an identical setup counterfactual;
        // production now uses Discharge. All other native records stay exact.
        struct Pokemon *chinchou = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][0];
        u16 bolt = MOVE_THUNDERBOLT;
        u8 pp = GetMovePP(bolt);
        SetMonData(chinchou, MON_DATA_MOVE3, &bolt);
        SetMonData(chinchou, MON_DATA_PP3, &pp);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentLeft, opening == 0 ? MOVE_THUNDERBOLT : MOVE_THUNDER_WAVE,
                 target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (opening == 1)
                MOVE(opponentRight, MOVE_DRAGON_DANCE, target: opponentRight);
            else
                MOVE(opponentRight, MOVE_HIGH_HORSEPOWER, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentLeft, MOVE_MUDDY_WATER, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentRight, MOVE_WATERFALL, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("DARIAN_SETUP opening=%d HP=%d/%d foeHP=%d/%d foeStatus=%d barAtkStage=%d barSpeedStage=%d nativeSpeed=%d/%d",
            opening, opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            playerLeft->status1, opponentRight->statStages[STAT_ATK], opponentRight->statStages[STAT_SPEED],
            GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)),
            GetBattlerTotalSpeedStat(B_BATTLER_0, GetBattlerAbility(B_BATTLER_0), GetBattlerHoldEffect(B_BATTLER_0)));
    }
}
