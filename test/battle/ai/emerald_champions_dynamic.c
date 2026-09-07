#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "emerald_champions_ai.h"
#include "data.h"
#include "battle_transition.h"
#include "pokemon.h"
#include "emerald_champions_battle_sets.h"

// gTrainers is replaced by mock trainers in the battle harness. Import the
// generated campaign table separately so changing a real boss loadout can
// invalidate its coordination test instead of leaving a stale toy team green.
static const struct Trainer sCampaignTrainers[DIFFICULTY_COUNT][TRAINERS_COUNT] =
{
#include "../../../src/data/trainers.h"
};

static void UseCampaignMon(const struct TrainerMon *mon)
{
    Level(mon->lvl);
    Ability(mon->ability);
    Nature(mon->nature);
    Item(mon->heldItem);
    Moves(mon->moves[0], mon->moves[1], mon->moves[2], mon->moves[3]);
    for (u32 stat = 0; stat < NUM_STATS; stat++)
        SetMonData(gBattleTestRunnerState->data.currentMon, EC_STAT_POINT_DATA(stat), &mon->ev[stat]);
}

AI_DOUBLE_BATTLE_TEST("Emerald Champions Trick Room profile commits to an inactive room")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CARBINK) { Moves(MOVE_TRICK_ROOM, MOVE_TACKLE); }
        OPPONENT(SPECIES_RELICANTH) { Moves(MOVE_HEAD_SMASH); }
    } WHEN {
        BattleAI_SetDynamicFunc(AI_EC_TrickRoomDiscipline);
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("Emerald Champions Flannery profile recognizes After You into Eruption")
{
    const struct Trainer *trainer = &sCampaignTrainers[DIFFICULTY_NORMAL][TRAINER_FLANNERY_1];
    const struct TrainerMon *torkoal = &trainer->party[0];
    const struct TrainerMon *lilligant = &trainer->party[1];

    GIVEN {
        AI_FLAGS(trainer->aiFlags | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
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
        PLAYER(SPECIES_WOBBUFFET) { HP(900); MaxHP(900); Defense(999); SpDefense(999); }
        PLAYER(SPECIES_WOBBUFFET) { HP(900); MaxHP(900); Defense(999); SpDefense(999); }
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

AI_DOUBLE_BATTLE_TEST("Emerald Champions Quincy profile Entrainments the strongest foe")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER);
        PLAYER(SPECIES_MACHAMP) { Attack(200); }
        PLAYER(SPECIES_ABRA) { Attack(20); }
        OPPONENT(SPECIES_DURANT) { Ability(ABILITY_TRUANT); Moves(MOVE_ENTRAINMENT, MOVE_IRON_HEAD); }
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); Moves(MOVE_SLUDGE_BOMB); }
    } WHEN {
        BattleAI_SetDynamicFunc(AI_EC_QuincyTruant);
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_ENTRAINMENT, target: playerLeft);
        }
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

AI_DOUBLE_BATTLE_TEST("Emerald Champions redirection profile protects a setup partner")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_TOGEKISS) { Moves(MOVE_FOLLOW_ME, MOVE_AIR_SLASH); }
        OPPONENT(SPECIES_GARCHOMP) { Moves(MOVE_SWORDS_DANCE, MOVE_EARTHQUAKE); }
    } WHEN {
        BattleAI_SetDynamicFunc(AI_EC_RedirectionSetup);
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_FOLLOW_ME);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("Emerald Champions generic AI rejects Hypnosis into Misty Terrain")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MILOTIC) { Moves(MOVE_HYPNOSIS, MOVE_MUDDY_WATER); }
        OPPONENT(SPECIES_TAPU_FINI) { Ability(ABILITY_MISTY_SURGE); Moves(MOVE_MOONBLAST); }
    } WHEN {
        BattleAI_SetDynamicFunc(NULL);
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_MUDDY_WATER);
        }
    }
}

DOUBLE_BATTLE_TEST("Emerald Champions Darian attacks without damaging his partner")
{
    const struct Trainer *trainer = &sCampaignTrainers[DIFFICULTY_NORMAL][TRAINER_DARIAN];
    const struct TrainerMon *chinchou = &trainer->party[0];
    const struct TrainerMon *barboach = &trainer->party[1];
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(chinchou->species) { UseCampaignMon(chinchou); }
        OPPONENT(barboach->species) { UseCampaignMon(barboach); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_MUDDY_WATER);
            MOVE(opponentRight, MOVE_HIGH_HORSEPOWER, target: playerLeft);
        }
    } SCENE {
        NONE_OF {
            HP_BAR(opponentLeft);
            HP_BAR(opponentRight);
        }
    }
}
