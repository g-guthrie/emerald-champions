// Temporary numerical native comparison, not a second damage implementation.
#line 14000 "work/woods_grunt_anchor_probe.inc"
#include "battle_ai_switch.h"
extern struct SimulatedDamage AI_ProbeHpPowerDamage(enum BattlerId actor, enum BattlerId target, u32 index, u32 hp);
AI_DOUBLE_BATTLE_TEST("EC live review Woods grunt anchors: both targets native HP curve and restored state")
{
    GIVEN {
        WoodsLeadPlayer(SPECIES_EEVEE, &sWoodsEevee);
        WoodsLeadPlayer(SPECIES_PACHIRISU, &sWoodsPachirisu);
        AuthoredOpponent(TRAINER_GRUNT_PETALBURG_WOODS, 0, FALSE);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        EXPECT_EQ(opponentRight->species, SPECIES_WAILMER);
        struct SwitchCandidateSnapshot *saved = AI_SaveCandidateState();
        EXPECT(saved != NULL);
        rng_value_t rng = gRngValue, rng2 = gRng2Value;
        struct BattlePokemon original = gBattleMons[B_BATTLER_3];
        u8 oldPercent = gAiLogicData->hpPercents[B_BATTLER_3];
        // Query both targets and both initial cache states: healing from a
        // low-HP board must not multiply the native formula's constant term.
        u32 maxHp = opponentRight->maxHP;
        const u32 requested[] = {1, maxHp / 4, maxHp / 2, maxHp * 3 / 4, maxHp};
        for (u32 initial = 0; initial < 2; initial++)
        {
            gBattleMons[B_BATTLER_3].hp = initial ? 1 : maxHp;
            gAiLogicData->hpPercents[B_BATTLER_3] = initial ? 1 : 100;
            struct BattlePokemon before = gBattleMons[B_BATTLER_3];
            u8 beforePercent = gAiLogicData->hpPercents[B_BATTLER_3];
            for (enum BattlerId target = 0; target < MAX_BATTLERS_COUNT; target += 2)
                for (u32 point = 0; point < ARRAY_COUNT(requested); point++)
                {
                    struct AiCalcValues calc = {.move = MOVE_WATER_SPOUT, .weather = AI_GetWeather(), .terrain = gFieldTimers.terrain};
                    struct SimulatedDamage native = AI_CalcHpPowerDamage(&calc, B_BATTLER_3, target, requested[point]);
                    struct SimulatedDamage forecast = AI_ProbeHpPowerDamage(B_BATTLER_3, target, 0, requested[point]);
                    Test_MgbaPrintf("WOODS_ANCHOR initial=%d target=%d hp=%d native=%d/%d/%d forecast=%d/%d/%d",
                        before.hp, target, requested[point], native.minimum, native.median, native.maximum,
                        forecast.minimum, forecast.median, forecast.maximum);
                    EXPECT_GT(native.maximum, 0);
                    EXPECT_GT(forecast.maximum, 0);
                    EXPECT_LE(abs((s32)native.minimum - forecast.minimum), 3);
                    EXPECT_LE(abs((s32)native.median - forecast.median), 3);
                    EXPECT_LE(abs((s32)native.maximum - forecast.maximum), 3);
                    if (point == 0 || point == ARRAY_COUNT(requested) - 1)
                    {
                        EXPECT_EQ(native.minimum, forecast.minimum);
                        EXPECT_EQ(native.median, forecast.median);
                        EXPECT_EQ(native.maximum, forecast.maximum);
                    }
                    EXPECT_EQ(memcmp(&before, &gBattleMons[B_BATTLER_3], sizeof(before)), 0);
                    EXPECT_EQ(beforePercent, gAiLogicData->hpPercents[B_BATTLER_3]);
                    EXPECT_EQ(memcmp(&rng, &gRngValue, sizeof(rng)), 0);
                    EXPECT_EQ(memcmp(&rng2, &gRng2Value, sizeof(rng2)), 0);
                }
        }
        gBattleMons[B_BATTLER_3] = original;
        gAiLogicData->hpPercents[B_BATTLER_3] = oldPercent;
        AI_RestoreCandidateState(saved);
        AI_FreeCandidateState(saved);
    }
}
