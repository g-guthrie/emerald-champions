#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "emerald_champions_ai.h"

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
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_LILLIGANT) { Moves(MOVE_AFTER_YOU, MOVE_SOLAR_BEAM); }
        OPPONENT(SPECIES_TORKOAL) { Moves(MOVE_ERUPTION); }
    } WHEN {
        BattleAI_SetDynamicFunc(AI_EC_FlanneryAfterYou);
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_AFTER_YOU, target: opponentRight);
        }
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

AI_DOUBLE_BATTLE_TEST("Emerald Champions Wallace profile rejects Hypnosis into Misty Terrain")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MILOTIC) { Moves(MOVE_HYPNOSIS, MOVE_MUDDY_WATER); }
        OPPONENT(SPECIES_TAPU_FINI) { Ability(ABILITY_MISTY_SURGE); Moves(MOVE_MOONBLAST); }
    } WHEN {
        BattleAI_SetDynamicFunc(AI_EC_WallaceTerrain);
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_MUDDY_WATER);
        }
    }
}
