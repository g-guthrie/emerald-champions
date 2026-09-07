#include "global.h"
#include "test/battle.h"
#include "battle_ai_record.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "battle_controllers.h"

MULTI_BATTLE_TEST("AI knowledge records belong to the trainer, not merely the battle side")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_SPLASH); }
        PARTNER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_GROWL); }
        OPPONENT_A(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_TAIL_WHIP); }
        OPPONENT_B(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_LEER); }
    } WHEN {
        TURN {}
    } THEN {
        const enum Ability abilities[] = {ABILITY_DROUGHT, ABILITY_DRIZZLE, ABILITY_SNOW_WARNING, ABILITY_SAND_STREAM};
        const enum HoldEffect effects[] = {HOLD_EFFECT_LEFTOVERS, HOLD_EFFECT_LIFE_ORB, HOLD_EFFECT_CHOICE_BAND, HOLD_EFFECT_FOCUS_SASH};
        memset(gAiPartyData, 0, sizeof(*gAiPartyData));
        for (enum BattlerId battler = 0; battler < MAX_BATTLERS_COUNT; battler++) {
            EXPECT_EQ(gBattlerPartyIndexes[battler], 0);
            ClearBattlerMoveHistory(battler);
            RecordKnownMove(battler, gBattleMons[battler].moves[1]);
            RecordAbilityBattle(battler, abilities[battler]);
            RecordItemEffectBattle(battler, effects[battler]);
        }
        for (enum BattlerId battler = 0; battler < MAX_BATTLERS_COUNT; battler++) {
            enum BattleTrainer trainer = GetBattlerTrainer(battler);
            const struct AiPartyMon *mon = &gAiPartyData->mons[trainer][0];
            EXPECT_EQ(mon->moves[1], gBattleMons[battler].moves[1]);
            EXPECT_EQ(mon->ability, abilities[battler]);
            EXPECT_EQ(mon->heldEffect, effects[battler]);
            EXPECT_EQ(gBattleHistory->abilities[battler], abilities[battler]);
            RecordAllMoves(battler);
            for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
                EXPECT_EQ(mon->moves[slot], gBattleMons[battler].moves[slot]);
        }
        bool32 fainted[MAX_BATTLE_TRAINERS] = {FALSE};
        for (enum BattlerId battler = 0; battler < MAX_BATTLERS_COUNT; battler++) {
            Ai_UpdateFaintData(battler);
            EXPECT_EQ(gBattleHistory->abilities[battler], ABILITY_NONE);
            EXPECT_EQ(gBattleHistory->itemEffects[battler], HOLD_EFFECT_NONE);
            for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
                EXPECT_EQ(gBattleHistory->usedMoves[battler][slot], MOVE_NONE);
            fainted[GetBattlerTrainer(battler)] = TRUE;
            for (u32 trainer = 0; trainer < MAX_BATTLE_TRAINERS; trainer++)
                EXPECT_EQ((bool32)gAiPartyData->mons[trainer][0].isFainted, fainted[trainer]);
        }
    }
}

DOUBLE_BATTLE_TEST("AI knowledge keeps two active slots in a shared party separate")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN {}
    } THEN {
        const enum Ability abilities[] = {ABILITY_DROUGHT, ABILITY_DRIZZLE, ABILITY_SNOW_WARNING, ABILITY_SAND_STREAM};
        memset(gAiPartyData, 0, sizeof(*gAiPartyData));
        for (enum BattlerId battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
            RecordAbilityBattle(battler, abilities[battler]);
        for (enum BattlerId battler = 0; battler < MAX_BATTLERS_COUNT; battler++) {
            EXPECT_EQ((u32)GetBattlerTrainer(battler), (u32)GetBattlerSide(battler));
            EXPECT_EQ(gAiPartyData->mons[GetBattlerSide(battler)][gBattlerPartyIndexes[battler]].ability, abilities[battler]);
        }
    }
}

SINGLE_BATTLE_TEST("AI knowledge on re-entry replaces every outgoing history field")
{
    bool32 known;
    PARAMETRIZE { known = FALSE; }
    PARAMETRIZE { known = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {}
    } THEN {
        enum BattlerId battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        struct AiPartyMon *mon = GetBattlerAiPartyMon(battler);
        mon->wasSentInBattle = TRUE;
        mon->ability = known ? ABILITY_SHADOW_TAG : ABILITY_NONE;
        mon->heldEffect = known ? HOLD_EFFECT_LEFTOVERS : HOLD_EFFECT_NONE;
        memset(mon->moves, 0, sizeof(mon->moves));
        mon->moves[1] = MOVE_SPLASH;
        gBattleHistory->abilities[battler] = ABILITY_DROUGHT;
        gBattleHistory->itemEffects[battler] = HOLD_EFFECT_LIFE_ORB;
        for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            gBattleHistory->usedMoves[battler][slot] = MOVE_TACKLE;
        for (u32 slot = 0; slot < AI_MOVE_HISTORY_COUNT; slot++)
            gBattleHistory->moveHistory[battler][slot] = MOVE_GROWL;
        gBattleHistory->moveHistoryIndex[battler] = AI_MOVE_HISTORY_COUNT - 1;

        Ai_UpdateSwitchInData(battler);

        EXPECT_EQ(gBattleHistory->abilities[battler], mon->ability);
        EXPECT_EQ(gBattleHistory->itemEffects[battler], mon->heldEffect);
        for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            EXPECT_EQ(gBattleHistory->usedMoves[battler][slot], slot == 1 ? MOVE_SPLASH : MOVE_NONE);
        for (u32 slot = 0; slot < AI_MOVE_HISTORY_COUNT; slot++)
            EXPECT_EQ(gBattleHistory->moveHistory[battler][slot], MOVE_NONE);
        EXPECT_EQ(gBattleHistory->moveHistoryIndex[battler], 0);
        EXPECT_EQ(mon->moves[1], MOVE_SPLASH);
    }
}

SINGLE_BATTLE_TEST("AI knowledge separates Transform observations from the returning party member")
{
    bool32 returnToField;
    PARAMETRIZE { returnToField = FALSE; }
    PARAMETRIZE { returnToField = TRUE; }
    GIVEN {
        PLAYER(SPECIES_DITTO) { Ability(ABILITY_LIMBER); Moves(MOVE_TRANSFORM); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_GASTLY) { Ability(ABILITY_LEVITATE); Moves(MOVE_GROWL, MOVE_SPLASH, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRANSFORM); }
        if (returnToField) {
            TURN { SWITCH(player, 1); }
            TURN { SWITCH(player, 0); }
        }
    } THEN {
        enum BattlerId battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        const struct AiPartyMon *mon = GetBattlerAiPartyMon(battler);
        EXPECT_EQ(mon->moves[0], MOVE_TRANSFORM);
        for (u32 slot = 1; slot < MAX_MON_MOVES; slot++)
            EXPECT_EQ(mon->moves[slot], MOVE_NONE);
        EXPECT_NE(mon->ability, ABILITY_LEVITATE);
        if (returnToField) {
            EXPECT_EQ(GetRecordedMove(battler, 0), MOVE_TRANSFORM);
            EXPECT_EQ(player->moves[0], MOVE_TRANSFORM);
            EXPECT_EQ(player->ability, ABILITY_LIMBER);
        } else {
            SaveBattlerData(battler);
            EXPECT(gAiThinkingStruct->saved[battler].saved);
            SetBattlerData(battler);
            EXPECT_EQ(player->ability, ABILITY_LEVITATE);
            EXPECT_EQ(GetRecordedAbility(battler), ABILITY_LEVITATE);
            for (u32 slot = 0; slot < MAX_MON_MOVES; slot++) {
                EXPECT_EQ(GetRecordedMove(battler, slot), opponent->moves[slot]);
                EXPECT_EQ(player->moves[slot], opponent->moves[slot]);
            }
            RestoreBattlerData(battler);
        }
    }
}

SINGLE_BATTLE_TEST("AI knowledge observes Mimic replacement without forgetting the permanent move")
{
    GIVEN {
        PLAYER(SPECIES_MEW) { Moves(MOVE_MIMIC); Speed(1); }
        OPPONENT(SPECIES_MAGIKARP) { Moves(MOVE_SPLASH); Speed(2); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPLASH); MOVE(player, MOVE_MIMIC); }
    } THEN {
        enum BattlerId battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(player->moves[0], MOVE_SPLASH);
        EXPECT_EQ(gBattleHistory->usedMoves[battler][0], MOVE_SPLASH);
        EXPECT_EQ(GetRecordedMove(battler, 0), MOVE_SPLASH);
        EXPECT_EQ(GetBattlerAiPartyMon(battler)->moves[0], MOVE_MIMIC);
    }
}

SINGLE_BATTLE_TEST("AI knowledge observes Sketch as a permanent replacement before it is used")
{
    bool32 returnToField;
    PARAMETRIZE { returnToField = FALSE; }
    PARAMETRIZE { returnToField = TRUE; }
    GIVEN {
        PLAYER(SPECIES_SMEARGLE) { Moves(MOVE_SKETCH, MOVE_CELEBRATE); Speed(1); }
        PLAYER(SPECIES_WYNAUT) { Speed(1); }
        OPPONENT(SPECIES_MAGIKARP) { Moves(MOVE_SPLASH); Speed(2); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPLASH); MOVE(player, MOVE_SKETCH); }
        if (returnToField) {
            TURN { SWITCH(player, 1); }
            TURN { SWITCH(player, 0); }
        }
    } THEN {
        enum BattlerId battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(player->moves[0], MOVE_SPLASH);
        EXPECT_EQ(gBattleHistory->usedMoves[battler][0], MOVE_SPLASH);
        EXPECT_EQ(GetRecordedMove(battler, 0), MOVE_SPLASH);
        EXPECT_EQ(GetBattlerAiPartyMon(battler)->moves[0], MOVE_SPLASH);
        EXPECT_EQ(GetRecordedMove(battler, 1), MOVE_NONE);
        EXPECT_EQ(GetBattlerAiPartyMon(battler)->moves[1], MOVE_NONE);
    }
}
