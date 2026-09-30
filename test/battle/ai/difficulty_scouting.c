#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_record.h"
#include "battle_ai_switch.h"
#include "battle_ai_util.h"
#include "battle_ai_field_statuses.h"
#include "battle_setup.h"
#include "battle_controllers.h"
#include "data.h"
#include "difficulty.h"
#include "pokemon.h"
#include "random.h"
#include "constants/opponents.h"
#include "constants/party_menu.h"

extern void (*gTestAiTurnSetupHook)(void);

// A campaign trainer's profile on Easy and Medium: the authored strategy
// flags without the five information flags Hard adds.
#define EC_SCOUTING_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES \
    | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS)

AI_DOUBLE_BATTLE_TEST("EC scouting: human reserve prediction uses only known slots and perceived entry effects")
{
    bool32 hard;
    PARAMETRIZE { hard = FALSE; }
    PARAMETRIZE { hard = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_POLITOED) { Moves(MOVE_PROTECT, MOVE_SURF); Ability(ABILITY_DRIZZLE); Item(ITEM_CHOICE_SPECS); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_CELEBRATE); EXPECT_MOVE(opponentRight, MOVE_CELEBRATE); }
    } THEN {
        for (u32 battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
            gAiThinkingStruct->aiFlags[battler] = AI_FLAG_BASIC_TRAINER
                | (hard ? AI_FLAG_OMNISCIENT | AI_FLAG_KNOW_OPPONENT_PARTY : 0);
        memset(gAiPartyData, 0, sizeof(*gAiPartyData));
        Ai_InitPartyStruct();
        struct SwitchAiContext context = {
            .battler = B_BATTLER_0,
            .battlerIn1 = B_BATTLER_0,
            .battlerIn2 = B_BATTLER_2,
            .lastId = PARTY_SIZE,
            .party = GetBattlerParty(B_BATTLER_0),
        };
        GetShouldSwitchPartyMonEligibility(&context);
        EXPECT_EQ((u32)context.eligiblePartyMons, hard ? 1u << 2 : 0);
        // Once a reserve has been seen, predict its entry without restoring
        // moves, item or weather ability that it has never revealed.
        if (!hard)
        {
            gAiPartyData->mons[B_TRAINER_PLAYER][2].species = SPECIES_POLITOED;
            gAiPartyData->mons[B_TRAINER_PLAYER][2].moves[0] = MOVE_PROTECT;
            GetShouldSwitchPartyMonEligibility(&context);
            EXPECT_EQ((u32)context.eligiblePartyMons, 1u << 2);
        }
        struct SwitchCandidateSnapshot *baseline = AI_SaveCandidateState();
        u32 visibility = AI_MaskUnknownBattlers();
        AI_LoadSwitchCandidate(B_BATTLER_0, 2, FALSE);
        EXPECT_EQ(playerLeft->species, SPECIES_POLITOED);
        EXPECT_EQ(playerLeft->moves[0], MOVE_PROTECT);
        EXPECT_EQ(playerLeft->moves[1], hard ? MOVE_SURF : MOVE_NONE);
        EXPECT_EQ(playerLeft->item, hard ? ITEM_CHOICE_SPECS : ITEM_AI_UNIDENTIFIED);
        EXPECT_EQ(playerLeft->ability, hard ? ABILITY_DRIZZLE : ABILITY_NONE);
        // The entry follows what the AI believes: the real Drizzle on Hard,
        // otherwise this turn's guess among Politoed's abilities.
        EXPECT_EQ((bool32)((gBattleWeather & B_WEATHER_RAIN) != 0),
            hard || gAiLogicData->abilities[B_BATTLER_0] == ABILITY_DRIZZLE);
        AI_RestoreCandidateState(baseline);
        AI_FreeCandidateState(baseline);
        AI_RestoreMaskedBattlers(visibility);
        EXPECT_EQ(playerLeft->species, SPECIES_WOBBUFFET);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][2], MON_DATA_HELD_ITEM), ITEM_CHOICE_SPECS);
        EXPECT_EQ(AI_TestVisibilityDepth(), 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC scouting: campaign modes retain strategy and differ only in information")
{
    enum DifficultyLevel difficulty;
    PARAMETRIZE { difficulty = DIFFICULTY_EASY; }
    PARAMETRIZE { difficulty = DIFFICULTY_NORMAL; }
    PARAMETRIZE { difficulty = DIFFICULTY_HARD; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_CELEBRATE); EXPECT_MOVE(opponentRight, MOVE_CELEBRATE); }
    } THEN {
        enum DifficultyLevel savedDifficulty = GetCurrentDifficultyLevel();
        u32 savedFlags = gBattleTypeFlags;
        u16 savedTrainer = TRAINER_BATTLE_PARAM.opponentA;
        u16 savedOther = TRAINER_BATTLE_PARAM.opponentB;
        const u64 information = AI_FLAG_OMNISCIENT | AI_FLAG_KNOW_OPPONENT_PARTY
            | AI_FLAG_MOVE_OMNISCIENCE | AI_FLAG_ABILITY_OMNISCIENCE | AI_FLAG_ITEM_OMNISCIENCE;
        SetCurrentDifficultyLevel(difficulty);
        TRAINER_BATTLE_PARAM.opponentA = TRAINER_ROXANNE_1;
        TRAINER_BATTLE_PARAM.opponentB = 0;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
        BattleAI_SetupFlags();
        u64 authored = GetTrainerAIFlagsFromId(TRAINER_ROXANNE_1) | AI_FLAG_DOUBLE_BATTLE;
        EXPECT_EQ(gAiThinkingStruct->aiFlags[B_BATTLER_1] & ~information, authored & ~information);
        EXPECT_EQ(gAiThinkingStruct->aiFlags[B_BATTLER_3], gAiThinkingStruct->aiFlags[B_BATTLER_1]);
        EXPECT_EQ((bool32)IsAiFlagPresent(AI_FLAG_OMNISCIENT), difficulty == DIFFICULTY_HARD);
        EXPECT_EQ((bool32)IsAiFlagPresent(AI_FLAG_KNOW_OPPONENT_PARTY), difficulty == DIFFICULTY_HARD);
        // A local difficulty cannot change a link opponent's information.
        gBattleTypeFlags |= BATTLE_TYPE_LINK;
        BattleAI_SetupFlags();
        EXPECT_EQ(gAiThinkingStruct->aiFlags[B_BATTLER_1], authored);
        gBattleTypeFlags = savedFlags;
        TRAINER_BATTLE_PARAM.opponentA = savedTrainer;
        TRAINER_BATTLE_PARAM.opponentB = savedOther;
        SetCurrentDifficultyLevel(savedDifficulty);
    }
}

AI_DOUBLE_BATTLE_TEST("EC scouting: Hard preloads all six loadouts while other modes reveal them in battle")
{
    bool32 hard;
    PARAMETRIZE { hard = FALSE; }
    PARAMETRIZE { hard = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_PROTECT); Item(ITEM_LEFTOVERS); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE, MOVE_PROTECT); }
        PLAYER(SPECIES_POLITOED) { Moves(MOVE_CELEBRATE, MOVE_SURF); Ability(ABILITY_WATER_ABSORB); Item(ITEM_CHOICE_SPECS); }
        PLAYER(SPECIES_CHARIZARD) { Moves(MOVE_CELEBRATE, MOVE_HEAT_WAVE); Item(ITEM_CHARIZARDITE_Y); }
        PLAYER(SPECIES_GARCHOMP) { Moves(MOVE_CELEBRATE, MOVE_EARTHQUAKE); Item(ITEM_LIFE_ORB); }
        PLAYER(SPECIES_AMOONGUSS) { Moves(MOVE_CELEBRATE, MOVE_SPORE); Item(ITEM_FOCUS_SASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_CELEBRATE); EXPECT_MOVE(opponentRight, MOVE_CELEBRATE); }
    } THEN {
        for (u32 battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
            gAiThinkingStruct->aiFlags[battler] = AI_FLAG_BASIC_TRAINER | (hard ? AI_FLAG_OMNISCIENT | AI_FLAG_KNOW_OPPONENT_PARTY : 0);
        memset(gAiPartyData, 0, sizeof(*gAiPartyData));
        Ai_InitPartyStruct();
        EXPECT_EQ(gAiPartyData->count[B_TRAINER_PLAYER], 6);
        for (u32 slot = 0; slot < PARTY_SIZE; slot++) {
            const struct AiPartyMon *known = &gAiPartyData->mons[B_TRAINER_PLAYER][slot];
            struct Pokemon *actual = &gParties[B_TRAINER_PLAYER][slot];
            EXPECT_EQ(known->species, hard || slot < 2 ? GetMonData(actual, MON_DATA_SPECIES) : SPECIES_NONE);
            EXPECT_EQ(known->ability, hard ? GetMonAbility(actual) : ABILITY_NONE);
            EXPECT_EQ(known->item, hard ? GetMonData(actual, MON_DATA_HELD_ITEM) : ITEM_NONE);
            if (hard)
                EXPECT_EQ(known->level, GetMonData(actual, MON_DATA_LEVEL));
            for (u32 move = 0; move < MAX_MON_MOVES; move++)
                EXPECT_EQ(known->moves[move], hard ? GetMonData(actual, MON_DATA_MOVE1 + move) : MOVE_NONE);
        }
        SetBattlerAiData(B_BATTLER_0, gAiLogicData);
        // An unrevealed item is likely held, but which one stays unknown.
        EXPECT_EQ(gAiLogicData->items[B_BATTLER_0], hard ? ITEM_LEFTOVERS : ITEM_AI_UNIDENTIFIED);
        EXPECT_EQ(gAiLogicData->holdEffects[B_BATTLER_0], hard ? HOLD_EFFECT_LEFTOVERS : HOLD_EFFECT_NONE);
        // An AI partner cannot reveal the human ally's unobserved moves to
        // the opponent through the shared knowledge helper.
        u32 controller = gBattlerBattleController[B_BATTLER_2];
        gBattlerBattleController[B_BATTLER_2] = BATTLE_CONTROLLER_PLAYER_PARTNER;
        EXPECT_EQ(GetMovesArray(B_BATTLER_0)[1], hard ? MOVE_PROTECT : MOVE_NONE);
        gBattlerBattleController[B_BATTLER_2] = controller;
        RecordKnownMove(B_BATTLER_0, MOVE_CELEBRATE);
        RecordAbilityBattle(B_BATTLER_0, playerLeft->ability);
        RecordItemEffectBattle(B_BATTLER_0, HOLD_EFFECT_LEFTOVERS);
        EXPECT_EQ(GetRecordedMove(B_BATTLER_0, 0), MOVE_CELEBRATE);
        EXPECT_EQ(GetRecordedMove(B_BATTLER_0, 1), hard ? MOVE_PROTECT : MOVE_NONE);
        struct BattlePokemon original = *playerLeft;
        u32 masked = AI_MaskUnknownBattlers();
        EXPECT_EQ((bool32)(masked & (1u << B_BATTLER_0)), !hard);
        EXPECT_EQ(playerLeft->moves[0], MOVE_CELEBRATE);
        EXPECT_EQ(playerLeft->moves[1], hard ? MOVE_PROTECT : MOVE_NONE);
        EXPECT_EQ(playerLeft->item, ITEM_LEFTOVERS);
        EXPECT_EQ(playerLeft->ability, original.ability);
        EXPECT_EQ(playerLeft->attack, original.attack);
        EXPECT_EQ(playerLeft->defense, original.defense);
        EXPECT_EQ(playerLeft->spAttack, original.spAttack);
        EXPECT_EQ(playerLeft->spDefense, original.spDefense);
        EXPECT_EQ(playerLeft->speed, original.speed);
        EXPECT_EQ(masked & ((1u << B_BATTLER_1) | (1u << B_BATTLER_3)), 0);
        AI_RestoreMaskedBattlers(masked);
        EXPECT_EQ(memcmp(playerLeft, &original, sizeof(original)), 0);
    }
}

AI_SINGLE_BATTLE_TEST("EC scouting: switch prediction cannot read a human's committed replacement")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGIKARP) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MAGIKARP) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        gAiThinkingStruct->aiFlags[B_BATTLER_0] = AI_FLAG_OMNISCIENT | AI_FLAG_SEQUENCE_SWITCHING;
        gAiLogicData->aiPredictionInProgress = TRUE;
        struct SwitchCandidateSnapshot *baseline = AI_SaveCandidateState();
        for (u32 committed = 1; committed <= 2; committed++) {
            AI_RestoreCandidateState(baseline);
            SeedRng(0xDEADBEEF);
            SeedRng2(0xAABBCCDD);
            gBattleStruct->monToSwitchIntoId[B_BATTLER_0] = committed;
            EXPECT_EQ(GetMostSuitableMonToSwitchInto(B_BATTLER_0, SWITCH_AFTER_KO), 1);
            EXPECT(!IsPartyMonOnFieldOrChosenToSwitch(B_BATTLER_0, 1, B_BATTLER_0, B_BATTLER_0));
            EXPECT(!AI_IsBattlerPlannedToSwitch(B_BATTLER_0));
            gAiLogicData->shouldSwitch |= 1u << B_BATTLER_0;
            EXPECT(AI_IsBattlerPlannedToSwitch(B_BATTLER_0));
            // Preserve coordination of an actual AI's chosen reserve.
            gBattleStruct->monToSwitchIntoId[B_BATTLER_1] = committed;
            EXPECT_EQ(GetMostSuitableMonToSwitchInto(B_BATTLER_1, SWITCH_AFTER_KO), committed);
            EXPECT(AI_IsBattlerPlannedToSwitch(B_BATTLER_1));
        }
        AI_RestoreCandidateState(baseline);
        AI_FreeCandidateState(baseline);
    }
}

AI_SINGLE_BATTLE_TEST("EC scouting: Hard decisions ignore committed player moves and targets")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_SMART_TRAINER | AI_FLAG_PREDICT_MOVE);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, MOVE_PROTECT); }
        OPPONENT(SPECIES_UMBREON) { Moves(MOVE_SUCKER_PUNCH, MOVE_CRUNCH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } THEN {
        struct SwitchCandidateSnapshot *baseline = AI_SaveCandidateState();
        u32 expected = MAX_MON_MOVES;
        for (u32 choice = 0; choice < 2; choice++) {
            AI_RestoreCandidateState(baseline);
            SeedRng(0xDEADBEEF);
            SeedRng2(0xAABBCCDD);
            gChosenMoveByBattler[B_BATTLER_0] = choice ? MOVE_PROTECT : MOVE_TACKLE;
            gBattleStruct->moveTarget[B_BATTLER_0] = choice ? B_BATTLER_0 : B_BATTLER_1;
            gChosenActionByBattler[B_BATTLER_0] = choice ? B_ACTION_SWITCH : B_ACTION_USE_MOVE;
            gBattleStruct->monToSwitchIntoId[B_BATTLER_0] = choice ? 2 : PARTY_SIZE;
            BattleAI_SetupAIData(0xF, B_BATTLER_1);
            u32 chosen = BattleAI_ChooseMoveIndex(B_BATTLER_1);
            if (choice == 0)
                expected = chosen;
            else
                EXPECT_EQ(chosen, expected);
        }
        AI_RestoreCandidateState(baseline);
        AI_FreeCandidateState(baseline);
    }
}

AI_DOUBLE_BATTLE_TEST("EC scouting: nested visibility restores the perceived board before the real board")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_PROTECT); Item(ITEM_LEFTOVERS); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE, MOVE_PROTECT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_CELEBRATE); EXPECT_MOVE(opponentRight, MOVE_CELEBRATE); }
    } THEN {
        memset(gAiPartyData, 0, sizeof(*gAiPartyData));
        Ai_InitPartyStruct();
        RecordKnownMove(B_BATTLER_0, MOVE_CELEBRATE);
        struct BattlePokemon original = *playerLeft;
        EXPECT_EQ(AI_TestVisibilityDepth(), 0);
        u32 outer = AI_MaskUnknownBattlers();
        EXPECT_EQ(AI_TestVisibilityDepth(), 1);
        EXPECT_EQ(playerLeft->moves[1], MOVE_NONE);
        EXPECT_EQ(playerLeft->item, ITEM_AI_UNIDENTIFIED);
        // A switch candidate changes the public board inside the outer scope.
        playerLeft->hp--;
        u32 candidateHp = playerLeft->hp;
        u32 inner = AI_MaskUnknownBattlers();
        EXPECT_EQ(AI_TestVisibilityDepth(), 2);
        EXPECT_NE(inner, outer);
        playerLeft->moves[1] = MOVE_TACKLE;
        playerLeft->item = ITEM_SITRUS_BERRY;
        playerLeft->hp = 1;
        AI_RestoreMaskedBattlers(inner);
        EXPECT_EQ(AI_TestVisibilityDepth(), 1);
        EXPECT_EQ(playerLeft->moves[1], MOVE_NONE);
        EXPECT_EQ(playerLeft->item, ITEM_AI_UNIDENTIFIED);
        EXPECT_EQ(playerLeft->hp, candidateHp);
        AI_RestoreMaskedBattlers(outer);
        EXPECT_EQ(AI_TestVisibilityDepth(), 0);
        EXPECT_EQ(memcmp(playerLeft, &original, sizeof(original)), 0);
        // Reuse the bounded slots without leaking a scope or accepting a
        // token from an earlier transaction as the new transaction's token.
        u32 reused = AI_MaskUnknownBattlers();
        EXPECT_NE(reused, outer);
        AI_RestoreMaskedBattlers(reused);
        EXPECT_EQ(AI_TestVisibilityDepth(), 0);
        EXPECT_EQ(memcmp(playerLeft, &original, sizeof(original)), 0);
    }
}

// The AI's lead reaches its last Perish Song turn: switching out is its only
// way to survive the turn.
static void PerishingLeadBoard(void)
{
    gBattleMons[B_BATTLER_1].volatiles.perishSong = TRUE;
    gBattleMons[B_BATTLER_1].volatiles.perishSongTimer = 0;
}

// Gothitelle's Shadow Tag never announced itself, and the species has other
// abilities, but a trap is a fact the game tells any trainer who tries to
// leave. A perishing Tyranitar plans with it and does not plan an exit; a
// switch the engine still refuses is turned back into a move.
AI_SINGLE_BATTLE_TEST("EC scouting: an unrevealed Shadow Tag keeps a perishing Tyranitar in")
{
    GIVEN {
        AI_FLAGS(EC_SCOUTING_FLAGS);
        PLAYER(SPECIES_GOTHITELLE) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_PROTECT, MOVE_PSYCHIC, MOVE_THUNDERBOLT, MOVE_PERISH_SONG); }
        PLAYER(SPECIES_INCINEROAR) { Moves(MOVE_FLARE_BLITZ, MOVE_KNOCK_OFF, MOVE_FAKE_OUT, MOVE_PARTING_SHOT); }
        OPPONENT(SPECIES_TYRANITAR) { Moves(MOVE_CRUNCH, MOVE_STONE_EDGE, MOVE_EARTHQUAKE, MOVE_ICE_PUNCH); }
        OPPONENT(SPECIES_METAGROSS) { Moves(MOVE_METEOR_MASH, MOVE_ZEN_HEADBUTT, MOVE_BULLET_PUNCH, MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_SCIZOR) { Moves(MOVE_BULLET_PUNCH, MOVE_KNOCK_OFF, MOVE_U_TURN, MOVE_SWORDS_DANCE); }
        gTestAiTurnSetupHook = PerishingLeadBoard;
    } WHEN {
        TURN { MOVE(player, MOVE_PROTECT); EXPECT_MOVE(opponent, MOVE_CRUNCH); }
    } THEN {
        EXPECT_EQ(GetBattlerAiPartyMon(B_BATTLER_0)->ability, ABILITY_NONE);
        // Tyranitar's replacement is on its last Perish turn too, behind the
        // same trap, with a healthy reserve still behind it.
        PerishingLeadBoard();
        u32 visibility = AI_MaskUnknownBattlers();
        EXPECT_EQ(player->ability, ABILITY_SHADOW_TAG);
        EXPECT(!ShouldSwitch(B_BATTLER_1));
        AI_RestoreMaskedBattlers(visibility);
        // A plan that still asked to leave is turned back into a move.
        gBattleResources->bufferA[B_BATTLER_1][1] = PARTY_ACTION_CHOOSE_MON;
        EXPECT(!AI_CancelRefusedSwitch(B_BATTLER_1));
        gBattleResources->bufferA[B_BATTLER_1][1] = PARTY_ACTION_ABILITY_PREVENTS;
        gAiLogicData->shouldSwitch |= 1u << B_BATTLER_1;
        gBattleStruct->AI_monToSwitchIntoId[B_BATTLER_1] = 0;
        gBattleStruct->monToSwitchIntoId[B_BATTLER_1] = 0;
        EXPECT(AI_CancelRefusedSwitch(B_BATTLER_1));
        EXPECT_EQ(gAiLogicData->shouldSwitch & (1u << B_BATTLER_1), 0);
        EXPECT_EQ(gBattleStruct->AI_monToSwitchIntoId[B_BATTLER_1], PARTY_SIZE);
        EXPECT_EQ(gBattleStruct->monToSwitchIntoId[B_BATTLER_1], PARTY_SIZE);
        EXPECT_NE(opponent->moves[gAiBattleData->chosenMoveIndex[B_BATTLER_1]], MOVE_NONE);
        EXPECT_EQ(AI_TestVisibilityDepth(), 0);
    }
}

// The pair planner plans with the trap too. An exit it cannot take would
// leave Tyranitar a placeholder turn - its first move, at no chosen target.
AI_DOUBLE_BATTLE_TEST("EC scouting: an unrevealed Shadow Tag keeps the pair planner from a perish exit")
{
    GIVEN {
        AI_FLAGS(EC_SCOUTING_FLAGS | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_GOTHITELLE) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_PROTECT, MOVE_PSYCHIC, MOVE_THUNDERBOLT, MOVE_PERISH_SONG); }
        PLAYER(SPECIES_INCINEROAR) { Moves(MOVE_PROTECT, MOVE_FLARE_BLITZ, MOVE_KNOCK_OFF, MOVE_PARTING_SHOT); }
        OPPONENT(SPECIES_TYRANITAR) { Moves(MOVE_ICE_PUNCH, MOVE_CRUNCH, MOVE_STONE_EDGE, MOVE_ROCK_SLIDE); }
        OPPONENT(SPECIES_GARCHOMP) { Moves(MOVE_DRAGON_CLAW, MOVE_ROCK_SLIDE, MOVE_FIRE_FANG, MOVE_PROTECT); }
        OPPONENT(SPECIES_METAGROSS) { Moves(MOVE_METEOR_MASH, MOVE_ZEN_HEADBUTT, MOVE_BULLET_PUNCH, MOVE_PROTECT); }
        gTestAiTurnSetupHook = PerishingLeadBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVES(opponentLeft, MOVE_CRUNCH, MOVE_STONE_EDGE, MOVE_ROCK_SLIDE);
        }
    }
}

// Steven-style partner flags describe the foes. Before any of the human's
// moves has been seen, the opponents know none of them and forecast nothing
// from its real set; an opponent's own prediction works from what it saw.
AI_MULTI_BATTLE_TEST("EC scouting: an AI partner's flags never reveal its human ally to the foes")
{
    bool32 partnerFlags;
    PARAMETRIZE { partnerFlags = TRUE; }
    PARAMETRIZE { partnerFlags = FALSE; }
    GIVEN {
        AI_FLAGS(EC_SCOUTING_FLAGS | AI_FLAG_DOUBLE_BATTLE);
        if (partnerFlags)
            BATTLER_AI_FLAGS(playerRight, AI_FLAG_PREDICTION | AI_FLAG_ASSUME_STAB | AI_FLAG_ASSUME_STATUS_MOVES);
        else
            BATTLER_AI_FLAGS(opponentLeft, AI_FLAG_PREDICT_MOVE | AI_FLAG_PREDICT_SWITCH);
        PLAYER(SPECIES_GARCHOMP) { Moves(MOVE_EARTHQUAKE, MOVE_DRAGON_CLAW, MOVE_SWORDS_DANCE, MOVE_PROTECT); }
        PARTNER(SPECIES_MILOTIC) { Moves(MOVE_SCALD, MOVE_ICE_BEAM, MOVE_RECOVER, MOVE_PROTECT); }
        OPPONENT_A(SPECIES_HEATRAN) { Moves(MOVE_HEAT_WAVE, MOVE_EARTH_POWER, MOVE_FLASH_CANNON, MOVE_PROTECT); }
        OPPONENT_B(SPECIES_TOGEKISS) { Moves(MOVE_AIR_SLASH, MOVE_DAZZLING_GLEAM, MOVE_FOLLOW_ME, MOVE_PROTECT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            EXPECT_MOVES(playerRight, MOVE_SCALD, MOVE_ICE_BEAM, MOVE_RECOVER, MOVE_PROTECT);
        }
    } THEN {
        // Forget the Protect, then set up a turn with nothing revealed.
        memset(gBattleHistory->usedMoves[B_BATTLER_0], 0, sizeof(gBattleHistory->usedMoves[B_BATTLER_0]));
        memset(GetBattlerAiPartyMon(B_BATTLER_0)->moves, 0, sizeof(GetBattlerAiPartyMon(B_BATTLER_0)->moves));
        SetAiLogicDataForTurn(gAiLogicData);
        for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            EXPECT_EQ(GetRecordedMove(B_BATTLER_0, slot), MOVE_NONE);
        EXPECT_EQ(gAiLogicData->predictedMove[B_BATTLER_0], MOVE_NONE);
        EXPECT_EQ(gAiLogicData->shouldSwitch & (1u << B_BATTLER_0), 0);
        EXPECT_EQ(AI_TestVisibilityDepth(), 0);
        EXPECT_EQ(playerLeft->moves[0], MOVE_EARTHQUAKE);
        if (!partnerFlags)
        {
            // Once Earthquake is seen, the forecast can only name Earthquake.
            RecordKnownMove(B_BATTLER_0, MOVE_EARTHQUAKE);
            SetAiLogicDataForTurn(gAiLogicData);
            EXPECT_EQ(gAiLogicData->predictedMove[B_BATTLER_0], MOVE_EARTHQUAKE);
        }
    }
}

// Most competitive Pokemon hold an item, so an unrevealed one is likely
// there: Poltergeist is worth firing. A Poltergeist that fails shows the slot
// is empty, and the AI does not throw a second.
AI_SINGLE_BATTLE_TEST("EC scouting: an unrevealed item is likely held until the foe is seen without one")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_LEFTOVERS; }
    PARAMETRIZE { item = ITEM_NONE; }
    GIVEN {
        AI_FLAGS(EC_SCOUTING_FLAGS);
        PLAYER(SPECIES_SLOWBRO) { Item(item); Moves(MOVE_SCALD, MOVE_PSYCHIC, MOVE_SLACK_OFF, MOVE_CALM_MIND); }
        OPPONENT(SPECIES_DUSKNOIR) { Moves(MOVE_POLTERGEIST, MOVE_SHADOW_SNEAK, MOVE_ICE_PUNCH, MOVE_BRICK_BREAK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CALM_MIND); EXPECT_MOVE(opponent, MOVE_POLTERGEIST); }
        TURN { MOVE(player, MOVE_CALM_MIND); EXPECT_MOVE(opponent, item == ITEM_NONE ? MOVE_SHADOW_SNEAK : MOVE_POLTERGEIST); }
    }
}

// Revival Blessing runs from the controller, outside any turn decision. It
// judges the foe as known: a Knock Off user is worth more against a foe that
// likely holds an item, and less once that foe is known to hold nothing.
AI_SINGLE_BATTLE_TEST("EC scouting: Revival Blessing prices Knock Off by the item a foe likely holds")
{
    enum Item item;
    PARAMETRIZE { item = ITEM_LEFTOVERS; }
    PARAMETRIZE { item = ITEM_NONE; }
    GIVEN {
        AI_FLAGS(EC_SCOUTING_FLAGS);
        PLAYER(SPECIES_SLOWKING) { Item(item); Moves(MOVE_SCALD, MOVE_PSYCHIC, MOVE_SLACK_OFF, MOVE_PROTECT); }
        OPPONENT(SPECIES_PAWMOT) { Moves(MOVE_REVIVAL_BLESSING, MOVE_PROTECT); }
        OPPONENT(SPECIES_WEAVILE) { Moves(MOVE_KNOCK_OFF, MOVE_PROTECT); }
        OPPONENT(SPECIES_WEAVILE) { Moves(MOVE_CRUNCH, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(player, MOVE_PROTECT); }
    } THEN {
        u32 hp = 0;
        SetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP, &hp);
        SetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_HP, &hp);
        // An empty hand the AI has seen, as after a Knock Off or a failed
        // Poltergeist.
        if (item == ITEM_NONE)
            GetBattlerAiPartyMon(B_BATTLER_0)->seenWithoutItem = TRUE;
        struct BattlePokemon original = *player;
        EXPECT_EQ(AI_SelectRevivalBlessingMon(B_BATTLER_1), item == ITEM_NONE ? 2 : 1);
        EXPECT_EQ(AI_TestVisibilityDepth(), 0);
        EXPECT_EQ(memcmp(player, &original, sizeof(original)), 0);
    }
}

// The party balls show which of the human's members still stand, not how
// hurt they are. A reserve that has never been out counts as healthy until
// it is seen.
AI_DOUBLE_BATTLE_TEST("EC scouting: the pair planner counts an unseen reserve as healthy")
{
    bool32 hard;
    PARAMETRIZE { hard = FALSE; }
    PARAMETRIZE { hard = TRUE; }
    GIVEN {
        AI_FLAGS(EC_SCOUTING_FLAGS | AI_FLAG_DOUBLE_BATTLE | (hard ? AI_FLAG_OMNISCIENT | AI_FLAG_KNOW_OPPONENT_PARTY : 0));
        PLAYER(SPECIES_INCINEROAR) { Moves(MOVE_PROTECT, MOVE_FLARE_BLITZ, MOVE_KNOCK_OFF, MOVE_PARTING_SHOT); }
        PLAYER(SPECIES_AMOONGUSS) { Moves(MOVE_PROTECT, MOVE_SPORE, MOVE_RAGE_POWDER, MOVE_GIGA_DRAIN); }
        PLAYER(SPECIES_GARCHOMP) { HP(1); Moves(MOVE_EARTHQUAKE, MOVE_DRAGON_CLAW, MOVE_ROCK_SLIDE, MOVE_PROTECT); }
        OPPONENT(SPECIES_HEATRAN) { Moves(MOVE_HEAT_WAVE, MOVE_EARTH_POWER, MOVE_FLASH_CANNON, MOVE_PROTECT); }
        OPPONENT(SPECIES_TOGEKISS) { Moves(MOVE_AIR_SLASH, MOVE_DAZZLING_GLEAM, MOVE_FOLLOW_ME, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        // One reserve at 1 HP: worth 80 of a healthy member's 180.
        EXPECT_EQ(AI_TestPairReserveValue(B_SIDE_OPPONENT, B_SIDE_PLAYER), hard ? 80 : 180);
        gAiPartyData->mons[B_TRAINER_PLAYER][2].wasSentInBattle = TRUE;
        EXPECT_EQ(AI_TestPairReserveValue(B_SIDE_OPPONENT, B_SIDE_PLAYER), 80);
    }
}

// Three scopes deep there is no snapshot left. The third still hides what the
// AI has not seen, and unwinding restores the real board.
AI_DOUBLE_BATTLE_TEST("EC scouting: a visibility scope past the snapshots masks in place and unwinds")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_PROTECT); Item(ITEM_LEFTOVERS); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE, MOVE_PROTECT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_CELEBRATE); EXPECT_MOVE(opponentRight, MOVE_CELEBRATE); }
    } THEN {
        memset(gAiPartyData, 0, sizeof(*gAiPartyData));
        Ai_InitPartyStruct();
        RecordKnownMove(B_BATTLER_0, MOVE_CELEBRATE);
        struct BattlePokemon original = *playerLeft;
        u32 outer = AI_MaskUnknownBattlers();
        u32 middle = AI_MaskUnknownBattlers();
        // A hypothetical loaded unmasked inside the second scope.
        playerLeft->moves[1] = MOVE_TACKLE;
        playerLeft->item = ITEM_SITRUS_BERRY;
        u32 inner = AI_MaskUnknownBattlers();
        EXPECT_EQ(AI_TestVisibilityDepth(), 3);
        EXPECT_EQ(playerLeft->moves[0], MOVE_CELEBRATE);
        EXPECT_EQ(playerLeft->moves[1], MOVE_NONE);
        EXPECT_EQ(playerLeft->item, ITEM_AI_UNIDENTIFIED);
        AI_RestoreMaskedBattlers(inner);
        EXPECT_EQ(AI_TestVisibilityDepth(), 2);
        AI_RestoreMaskedBattlers(middle);
        EXPECT_EQ(AI_TestVisibilityDepth(), 1);
        EXPECT_EQ(playerLeft->moves[1], MOVE_NONE);
        AI_RestoreMaskedBattlers(outer);
        EXPECT_EQ(AI_TestVisibilityDepth(), 0);
        EXPECT_EQ(memcmp(playerLeft, &original, sizeof(original)), 0);
    }
}

// Inclement gives a player's Stunfisk five possible abilities. The guess at an
// unrevealed one is always one of them, and it is one belief for the turn:
// masked, switch and Mega boards all read the same guess, so no board treats
// the foe as having no ability at all.
AI_SINGLE_BATTLE_TEST("EC scouting: a five-ability foe gets one guess per turn that every board shares")
{
    bool32 weighted;
    PARAMETRIZE { weighted = FALSE; }
    PARAMETRIZE { weighted = TRUE; }
    GIVEN {
        AI_FLAGS(EC_SCOUTING_FLAGS | (weighted ? AI_FLAG_WEIGH_ABILITY_PREDICTION : 0));
        PLAYER(SPECIES_STUNFISK) { Ability(ABILITY_STATIC); Moves(MOVE_EARTH_POWER, MOVE_DISCHARGE, MOVE_STEALTH_ROCK, MOVE_PROTECT); }
        OPPONENT(SPECIES_LANTURN) { Moves(MOVE_SCALD, MOVE_ICE_BEAM, MOVE_THUNDERBOLT, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(player, MOVE_PROTECT); }
    } THEN {
        enum Ability possible[NUM_OWNER_ABILITY_SLOTS];
        for (u32 slot = 0; slot < NUM_OWNER_ABILITY_SLOTS; slot++)
            possible[slot] = GetBattlerSpeciesAbility(B_BATTLER_0, SPECIES_STUNFISK, slot);
        EXPECT_NE(possible[NUM_OWNER_ABILITY_SLOTS - 1], ABILITY_NONE);
        EXPECT_EQ(GetRecordedAbility(B_BATTLER_0), ABILITY_NONE);
        for (u32 seed = 0; seed < 24; seed++)
        {
            SeedRng(0x1000 + seed * 0x9E37);
            gAiLogicData->abilityGuessSlot[B_BATTLER_0] = 0;
            enum Ability guess = AI_DecideKnownAbilityForTurn(B_BATTLER_0);
            bool32 legal = FALSE;
            for (u32 slot = 0; slot < NUM_OWNER_ABILITY_SLOTS; slot++)
                legal |= guess == possible[slot];
            EXPECT(legal);
            EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), guess);
            u32 visibility = AI_MaskUnknownBattlers();
            EXPECT_EQ(player->ability, ABILITY_NONE);
            EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), guess);
            SetBattlerAiData(B_BATTLER_0, gAiLogicData);
            EXPECT_EQ(gAiLogicData->abilities[B_BATTLER_0], guess);
            AI_RestoreMaskedBattlers(visibility);
        }
        // Gastro Acid is announced: the ability is known to be suppressed,
        // masked or not.
        player->volatiles.gastroAcid = TRUE;
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_NONE);
        u32 visibility = AI_MaskUnknownBattlers();
        EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_0), ABILITY_NONE);
        AI_RestoreMaskedBattlers(visibility);
        player->volatiles.gastroAcid = FALSE;
    }
}

// Snowscape is not hail: it deals no residual damage. The pair planner's
// weather plan asks whether a Rock-type setter should set snow for its Ice
// partner, and snow costs the setter nothing; hail would chip it.
AI_DOUBLE_BATTLE_TEST("EC weather: the pair planner reads Snowscape as snow, not hail")
{
    GIVEN {
        AI_FLAGS(EC_SCOUTING_FLAGS | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_METAGROSS) { Moves(MOVE_METEOR_MASH, MOVE_ZEN_HEADBUTT, MOVE_BULLET_PUNCH, MOVE_PROTECT); }
        PLAYER(SPECIES_EMPOLEON) { Moves(MOVE_SURF, MOVE_FLASH_CANNON, MOVE_ICE_BEAM, MOVE_PROTECT); }
        OPPONENT(SPECIES_STONJOURNER) { Ability(ABILITY_POWER_SPOT); Moves(MOVE_SNOWSCAPE, MOVE_ROCK_SLIDE, MOVE_STONE_EDGE, MOVE_PROTECT); }
        OPPONENT(SPECIES_AMAURA) { Ability(ABILITY_REFRIGERATE); Moves(MOVE_FREEZE_DRY, MOVE_ANCIENT_POWER, MOVE_EARTH_POWER, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        gBattleWeather = B_WEATHER_NONE;
        EXPECT_EQ(AI_TestPairWeatherMask(MOVE_SNOWSCAPE), B_WEATHER_SNOW);
        EXPECT_EQ(AI_TestPairWeatherMask(MOVE_HAIL), B_WEATHER_HAIL);
        EXPECT(ShouldSetWeather(B_BATTLER_1, AI_TestPairWeatherMask(MOVE_SNOWSCAPE)));
        EXPECT(!ShouldSetWeather(B_BATTLER_1, AI_TestPairWeatherMask(MOVE_HAIL)));
    }
}

// Chloroplast and Mega Sol keep Synthesis and Solar Beam in harsh sunlight
// whatever the weather, so another weather takes nothing from them.
AI_SINGLE_BATTLE_TEST("EC weather: weather does not count as cutting a sunlit holder's Synthesis")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_VENUSAUR) { Ability(ABILITY_CHLOROPHYLL); Moves(MOVE_SYNTHESIS, MOVE_SOLAR_BEAM, MOVE_SLUDGE_BOMB, MOVE_LEECH_SEED); }
        OPPONENT(SPECIES_OMASTAR) { Moves(MOVE_SANDSTORM, MOVE_RAIN_DANCE, MOVE_SURF, MOVE_ANCIENT_POWER); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEECH_SEED); }
    } THEN {
        static const enum Move weathers[] = {MOVE_SANDSTORM, MOVE_RAIN_DANCE};
        for (u32 i = 0; i < ARRAY_COUNT(weathers); i++)
        {
            gBattleWeather = B_WEATHER_NONE;
            gAiLogicData->abilities[B_BATTLER_0] = ABILITY_CHLOROPHYLL;
            s32 hampered = CalcWeatherScore(B_BATTLER_1, B_BATTLER_0, weathers[i], gAiLogicData);
            EXPECT_GT(hampered, WEAK_EFFECT);
            gAiLogicData->abilities[B_BATTLER_0] = ABILITY_CHLOROPLAST;
            EXPECT_EQ(CalcWeatherScore(B_BATTLER_1, B_BATTLER_0, weathers[i], gAiLogicData), hampered - WEAK_EFFECT);
            gAiLogicData->abilities[B_BATTLER_0] = ABILITY_MEGA_SOL;
            EXPECT_EQ(CalcWeatherScore(B_BATTLER_1, B_BATTLER_0, weathers[i], gAiLogicData), hampered - WEAK_EFFECT);
        }
    }
}

// Skill Swap, Role Play and Entrainment price abilities by their AI rating.
TEST("EC abilities: Inclement and Champions abilities carry AI ratings")
{
    static const enum Ability rated[] = {
        ABILITY_DRAGONIZE, ABILITY_MEGA_SOL, ABILITY_FIRE_MANE,
        ABILITY_PIERCING_DRILL, ABILITY_SPICY_SPRAY, ABILITY_EELEVATE,
    };
    for (u32 i = 0; i < ARRAY_COUNT(rated); i++)
        EXPECT_GT(gAbilitiesInfo[rated[i]].aiRating, 0);
}
