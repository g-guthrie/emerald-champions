#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "emerald_champions_ai.h"
#include "data.h"
#include "battle_transition.h"
#include "pokemon.h"
#include "emerald_champions_battle_sets.h"
#include "trainer_util.h"
#include "constants/characters.h"

// gTrainers is replaced by mock trainers in the battle harness. Import the
// generated campaign table separately so changing a real boss loadout can
// invalidate its coordination test instead of leaving a stale toy team green.
static const struct Trainer sCampaignTrainers[DIFFICULTY_COUNT][TRAINERS_COUNT] =
{
#include "../../../src/data/trainers.h"
};

static void UseCampaignMon(const struct TrainerMon *mon)
{
    struct TrainerGenerator generator = {0};
    generator.otID = OTID_STRUCT_PRESET(0);
    generator.name[0] = EOS;
    GenerateMonFromTrainerMon(gBattleTestRunnerState->data.currentMon, mon, &generator);
    Ability(mon->ability);
    Nature(mon->nature);
    Gender(GetMonGender(gBattleTestRunnerState->data.currentMon));
    Item(mon->heldItem);
    Moves(mon->moves[0], mon->moves[1], mon->moves[2], mon->moves[3]);
    // Otherwise the test runner invents speeds from the scripted turn order.
    Speed(GetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_SPEED));
}

AI_DOUBLE_BATTLE_TEST("Emerald Champions every authored Trick Room setter preserves a useful active room")
{
    u32 trainerId = TRAINER_ROXANNE_1, setterSlot = 0;
    for (u32 id = 1; id < TRAINERS_COUNT; id++)
    {
        const struct Trainer *candidate = &sCampaignTrainers[DIFFICULTY_NORMAL][id];
        for (u32 slot = 0; slot < candidate->partySize; slot++)
        {
            for (u32 moveSlot = 0; moveSlot < MAX_MON_MOVES; moveSlot++)
            {
                if (candidate->partySize > 1 && candidate->party[slot].moves[moveSlot] == MOVE_TRICK_ROOM)
                    PARAMETRIZE { trainerId = id; setterSlot = slot; }
            }
        }
    }
    const struct Trainer *trainer = &sCampaignTrainers[DIFFICULTY_NORMAL][trainerId];
    const struct TrainerMon *setter = &trainer->party[setterSlot];
    const struct TrainerMon *partner = &trainer->party[setterSlot == 0 ? 1 : 0];
    GIVEN {
        AI_FLAGS(trainer->aiFlags | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { HP(5000); MaxHP(5000); Defense(1000); SpDefense(1000); Speed(1000); }
        PLAYER(SPECIES_WOBBUFFET) { HP(5000); MaxHP(5000); Defense(1000); SpDefense(1000); Speed(1000); }
        OPPONENT(setter->species) { UseCampaignMon(setter); }
        OPPONENT(partner->species) { UseCampaignMon(partner); }
    } WHEN {
        BattleAI_SetDynamicFunc(GetEmeraldChampionsDynamicAiFunc(trainerId));
        TURN { EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM); }
        TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM); }
        TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM); }
        TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM); }
    } THEN {
        // Check the fixture after the harness has normalized the Pokemon.
        // These assertions catch omitted IVs, delayed Nature application and
        // implicit Speed overrides, independently of which move won a score.
        struct Pokemon *actual = &gParties[B_TRAINER_OPPONENT_A][0];
        struct Pokemon expected;
        struct TrainerGenerator generator = {0};
        generator.otID = OTID_STRUCT_PRESET(0);
        generator.name[0] = EOS;
        GenerateMonFromTrainerMon(&expected, setter, &generator);
        EXPECT_EQ(GetMonData(actual, MON_DATA_IVS), setter->iv);
        EXPECT_EQ(GetNature(actual), setter->nature);
        EXPECT_EQ(GetMonData(actual, MON_DATA_LEVEL), setter->lvl);
        for (u32 stat = 0; stat < NUM_STATS; stat++)
            EXPECT_EQ(GetMonData(actual, EC_STAT_POINT_DATA(stat)), setter->ev[stat]);
        for (u32 field = MON_DATA_MAX_HP; field <= MON_DATA_SPDEF; field++)
            EXPECT_EQ(GetMonData(actual, field), GetMonData(&expected, field));
    }
}

AI_DOUBLE_BATTLE_TEST("AI preserves useful Trick Room through its final turn and then sets it again")
{
    bool32 dynamic;
    u32 powerful;
    PARAMETRIZE { dynamic = FALSE; powerful = 0; }
    PARAMETRIZE { dynamic = FALSE; powerful = AI_FLAG_POWERFUL_STATUS; }
    PARAMETRIZE { dynamic = TRUE; powerful = 0; }
    PARAMETRIZE { dynamic = TRUE; powerful = AI_FLAG_POWERFUL_STATUS; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_DOUBLE_BATTLE | powerful);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_CARBINK) { Speed(10); Moves(MOVE_TRICK_ROOM, MOVE_TACKLE); }
        OPPONENT(SPECIES_RELICANTH) { Speed(20); Moves(MOVE_CELEBRATE); }
    } WHEN {
        if (dynamic)
            BattleAI_SetDynamicFunc(AI_EC_TrickRoomDiscipline);
        TURN { EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_TACKLE); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_TACKLE); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_TACKLE); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_TACKLE); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI can cancel opposing Trick Room even with Powerful Status or a boss profile")
{
    bool32 dynamic;
    PARAMETRIZE { dynamic = FALSE; }
    PARAMETRIZE { dynamic = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_DOUBLE_BATTLE | AI_FLAG_POWERFUL_STATUS);
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_TRICK_ROOM, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(2); }
        OPPONENT(SPECIES_CARBINK) { Speed(100); Moves(MOVE_TRICK_ROOM, MOVE_TACKLE); }
        OPPONENT(SPECIES_RELICANTH) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        if (dynamic)
            BattleAI_SetDynamicFunc(AI_EC_TrickRoomDiscipline);
        TURN { MOVE(playerLeft, MOVE_TRICK_ROOM); EXPECT_MOVE(opponentLeft, MOVE_TACKLE); }
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM); }
    }
}

AI_DOUBLE_BATTLE_TEST("Emerald Champions Flannery profile recognizes After You into Eruption")
{
    const struct Trainer *trainer = &sCampaignTrainers[DIFFICULTY_NORMAL][TRAINER_FLANNERY_1];
    const struct TrainerMon *torkoal = &trainer->party[0];
    const struct TrainerMon *lilligant = &trainer->party[1];

    GIVEN {
        AI_FLAGS(trainer->aiFlags | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(torkoal->species) { UseCampaignMon(torkoal); }
        OPPONENT(lilligant->species) { UseCampaignMon(lilligant); }
    } WHEN {
        BattleAI_SetDynamicFunc(GetEmeraldChampionsDynamicAiFunc(TRAINER_FLANNERY_1));
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentRight, MOVE_AFTER_YOU, target: opponentLeft);
            EXPECT_MOVE(opponentLeft, MOVE_ERUPTION);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("Emerald Champions redirection covers setup but not redundant protection")
{
    enum Move partnerMove;
    s32 expectedPenalty;

    PARAMETRIZE { partnerMove = MOVE_SWORDS_DANCE; expectedPenalty = 0; }
    PARAMETRIZE { partnerMove = MOVE_PROTECT; expectedPenalty = -20; }

    GIVEN {
        // Evaluate the setup/protection choice before the redirector, so
        // CheckBadMove is checking a known partner action in both cases.
        WITH_CONFIG(AI_REVERSE_BATTLER_LOGIC_ORDER_CHANCE, 0);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GARCHOMP) { Moves(partnerMove); }
        OPPONENT(SPECIES_TOGEKISS) { Moves(MOVE_FOLLOW_ME, MOVE_TACKLE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, partnerMove);
            SCORE_EQ_VAL(opponentRight, MOVE_FOLLOW_ME, AI_SCORE_DEFAULT + expectedPenalty, target: playerLeft);
        }
    }
}

DOUBLE_BATTLE_TEST("Emerald Champions Quincy opening lets Slaking attack on consecutive turns")
{
    const struct Trainer *trainer = &sCampaignTrainers[DIFFICULTY_NORMAL][TRAINER_QUINCY];
    const struct TrainerMon *attacker = &trainer->party[0];
    const struct TrainerMon *support = &trainer->party[1];

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(900); MaxHP(900); Defense(999); SpDefense(999); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { HP(900); MaxHP(900); Defense(999); SpDefense(999); Speed(100); }
        OPPONENT(attacker->species) { UseCampaignMon(attacker); }
        OPPONENT(support->species) { UseCampaignMon(support); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_DOUBLE_EDGE, target: playerLeft);
            MOVE(opponentRight, MOVE_SLUDGE_BOMB, target: playerRight);
        }
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_DOUBLE_EDGE, target: playerLeft);
            MOVE(opponentRight, MOVE_SLUDGE_BOMB, target: playerRight);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_EDGE, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SLUDGE_BOMB, opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_EDGE, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SLUDGE_BOMB, opponentRight);
    }
}

AI_DOUBLE_BATTLE_TEST("Emerald Champions snow profile commits to Aurora Veil")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_NINETALES_ALOLA) { Ability(ABILITY_SNOW_WARNING); Moves(MOVE_AURORA_VEIL, MOVE_BLIZZARD); }
        OPPONENT(SPECIES_KYUREM) { Moves(MOVE_BLIZZARD); }
    } WHEN {
        BattleAI_SetDynamicFunc(AI_EC_SnowScreen);
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_AURORA_VEIL);
        }
    }
}
