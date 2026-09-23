#include "global.h"
#include "test/battle.h"
#include "battle_stat_change.h"
#include "battle_ai_util.h"

SINGLE_BATTLE_TEST("Guard Dog: Intimidate replaces the drop at minimum, neutral and maximum Attack")
{
    u32 initial;
    PARAMETRIZE { initial = MIN_STAT_STAGE; }
    PARAMETRIZE { initial = DEFAULT_STAT_STAGE; }
    PARAMETRIZE { initial = MAX_STAT_STAGE; }
    GIVEN {
        PLAYER(SPECIES_DACHSBUN) { Ability(ABILITY_GUARD_DOG); Moves(MOVE_CELEBRATE, MOVE_BELLY_DRUM); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CHARM, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ARCANINE) { Ability(ABILITY_INTIMIDATE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        if (initial == MIN_STAT_STAGE)
            for (u32 i = 0; i < 3; i++)
                TURN { MOVE(opponent, MOVE_CHARM); MOVE(player, MOVE_CELEBRATE); }
        else if (initial == MAX_STAT_STAGE)
            TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_BELLY_DRUM); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], min(initial + 1, MAX_STAT_STAGE));
    }
}

DOUBLE_BATTLE_TEST("Stat reactions: Intimidate independently triggers Defiant and Competitive in doubles")
{
    GIVEN {
        PLAYER(SPECIES_BISHARP) { Ability(ABILITY_DEFIANT); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MILOTIC) { Ability(ABILITY_COMPETITIVE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ARCANINE) { Ability(ABILITY_INTIMIDATE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(playerRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Stat preview: checking Intimidate immunity does not queue reactions or reveal abilities")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_GUARD_DOG; }
    PARAMETRIZE { ability = ABILITY_INNER_FOCUS; }
    PARAMETRIZE { ability = ABILITY_SCRAPPY; }
    PARAMETRIZE { ability = ABILITY_OWN_TEMPO; }
    PARAMETRIZE { ability = ABILITY_OBLIVIOUS; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        struct BattleCalcValues cv = {.battlerAtk = B_BATTLER_0, .battlerDef = B_BATTLER_1, .move = MOVE_NONE};
        cv.abilities[B_BATTLER_1] = ability;
        struct StatChange st = {.stat = STAT_ATK, .stage = -1, .intimidate = TRUE, .onlyChecking = TRUE};
        u32 queued = gSpecialStatuses[B_BATTLER_1].statStageAmount2;
        enum Ability lastAbility = gLastUsedAbility;
        enum BattlerId abilityBattler = gBattlerAbility;
        enum BattlerId scriptBattler = gBattleScripting.battler;
        enum BattlerId effectBattler = gEffectBattler;
        EXPECT(!CanStatChange(&cv, &st));
        EXPECT_EQ((u32)gSpecialStatuses[B_BATTLER_1].statStageAmount2, queued);
        EXPECT_EQ(gLastUsedAbility, lastAbility);
        EXPECT_EQ(gBattlerAbility, abilityBattler);
        EXPECT_EQ(gBattleScripting.battler, scriptBattler);
        EXPECT_EQ(gEffectBattler, effectBattler);
        EXPECT_EQ(st.script, NULL);
    }
}

DOUBLE_BATTLE_TEST("Stat queues: selective resets preserve other battlers pending reactions")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } THEN {
        for (u32 battler = 0; battler < gBattlersCount; battler++)
        {
            SetStatChange(battler, STAT_ATK, 1);
            SetStatChange2(battler, STAT_DEF, 2);
        }
        gBattleStruct->statChangeBattler = 2;
        gBattleStruct->positiveAnimPlayed = TRUE;
        gBattleStruct->negativeAnimPlayed = TRUE;
        ClearStatChangeValues();
        for (u32 battler = 0; battler < gBattlersCount; battler++)
        {
            EXPECT_EQ((u32)gSpecialStatuses[battler].statStageAmount, 0);
            EXPECT_EQ((u32)gSpecialStatuses[battler].statStageAmount2, 1);
            EXPECT_EQ(gSpecialStatuses[battler].statStageQueue2[0].stage, 2);
        }
        EXPECT_EQ((u32)gBattleStruct->statChangeBattler, 0);
        EXPECT(!(bool32)gBattleStruct->positiveAnimPlayed);
        EXPECT(!(bool32)gBattleStruct->negativeAnimPlayed);
        ClearOtherStatChangeValues(B_BATTLER_1);
        for (u32 battler = 0; battler < gBattlersCount; battler++)
        {
            EXPECT_EQ((u32)gSpecialStatuses[battler].statStageAmount2, battler == B_BATTLER_1 ? 0 : 1);
            SetStatChange(battler, STAT_ATK, 1);
        }
        ClearBothStatChangeQueues();
        for (u32 battler = 0; battler < gBattlersCount; battler++)
        {
            EXPECT_EQ((u32)gSpecialStatuses[battler].statStageAmount, 0);
            EXPECT_EQ((u32)gSpecialStatuses[battler].statStageAmount2, 0);
            for (u32 slot = 0; slot < NUM_BATTLE_STATS; slot++)
            {
                EXPECT_EQ(gSpecialStatuses[battler].statStageQueue[slot].stage, 0);
                EXPECT_EQ(gSpecialStatuses[battler].statStageQueue2[slot].stage, 0);
            }
        }
    }
}
