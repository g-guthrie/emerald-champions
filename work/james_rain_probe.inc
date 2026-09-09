// Temporary forced native rain comparison; after JamesLeadPlayer/helpers.
// This proves execution/mechanics, not unrestricted rain selection.
#line 22000 "work/james_rain_probe.inc"
#include "battle_main.h"

DOUBLE_BATTLE_TEST("EC live review James rain: native Water Bubble Liquidation after Rain Dance", s16 quickAttackDamage, s16 bugDamage, s16 liquidationDamage)
{
    bool32 rain;
    PARAMETRIZE { rain = FALSE; }
    PARAMETRIZE { rain = TRUE; }
    GIVEN {
        JamesLeadPlayer(SPECIES_EEVEE, &sJamesEevee);
        JamesLeadPlayer(SPECIES_TIMBURR, &sJamesTimburr);
        AuthoredOpponent(TRAINER_JAMES_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SURSKIT);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_DEWPIDER);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_VENIPEDE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_KRICKETUNE);
        EXPECT_EQ(GetMonAbility(&party[0]), ABILITY_SWIFT_SWIM);
        EXPECT_EQ(GetMonAbility(&party[1]), ABILITY_WATER_BUBBLE);
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_MOVE2), MOVE_RAIN_DANCE);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_MOVE1), MOVE_LIQUIDATION);
        EXPECT_GT(GetMonData(&party[0], MON_DATA_SPEED), GetMonData(&party[1], MON_DATA_SPEED));
        Test_MgbaPrintf("JAMES_RAIN_INPUT rain=%d SursHP=%d Speed=%d DewHP=%d Atk=%d Speed=%d ability=%d",
            rain, GetMonData(&party[0], MON_DATA_HP), GetMonData(&party[0], MON_DATA_SPEED),
            GetMonData(&party[1], MON_DATA_HP), GetMonData(&party[1], MON_DATA_ATK),
            GetMonData(&party[1], MON_DATA_SPEED), GetMonAbility(&party[1]));
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_QUICK_ATTACK, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_BULK_UP, target: playerRight);
            if (rain)
                MOVE(opponentLeft, MOVE_RAIN_DANCE);
            else
                MOVE(opponentLeft, MOVE_STRUGGLE_BUG, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentRight, MOVE_LIQUIDATION, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, playerLeft);
        HP_BAR(opponentLeft, captureDamage: &results[i].quickAttackDamage);
        ANIMATION(ANIM_TYPE_MOVE, rain ? MOVE_RAIN_DANCE : MOVE_STRUGGLE_BUG, opponentLeft);
        if (!rain)
            HP_BAR(playerLeft, captureDamage: &results[i].bugDamage);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LIQUIDATION, opponentRight);
        HP_BAR(playerLeft, captureDamage: &results[i].liquidationDamage);
    } THEN {
        EXPECT_EQ((gBattleWeather & B_WEATHER_RAIN) != 0, rain);
        EXPECT_EQ((u32)opponentRight->ability, ABILITY_WATER_BUBBLE);
        EXPECT_EQ(gLastMoves[B_BATTLER_1], rain ? MOVE_RAIN_DANCE : MOVE_STRUGGLE_BUG);
        EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_LIQUIDATION);
        EXPECT_GT(results[i].quickAttackDamage, 0);
        EXPECT_GT(results[i].liquidationDamage, 0);
        Test_MgbaPrintf("JAMES_RAIN_AFTER rain=%d weather=%d timer=%d quickDamage=%d priorBugDamage=%d liquidationDamage=%d ownHP=%d/%d foeHP=%d/%d Speed=%d/%d speedStages=%d/%d LAST=%S/%S ownFaints=%d foeFaints=%d",
            rain, gBattleWeather, gBattleStruct->weatherDuration, results[i].quickAttackDamage,
            results[i].bugDamage, results[i].liquidationDamage, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, opponentLeft->speed, opponentRight->speed,
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED],
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        // Native HP-bar damage is capped by remaining HP. In the dry branch,
        // Struggle Bug also chips Eevee first, so this is not an uncapped
        // equal-HP damage-formula ratio or proof that Rain is always better.
    }
}
