#include "global.h"
#include "test/battle.h"
#include "battle_ai_main.h"
#include "battle_ai_util.h"
#include "emerald_champions_ai.h"

AI_DOUBLE_BATTLE_TEST("AI strategy: one priority move does not make a fast attacker a Trick Room partner")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_DOUBLE_BATTLE | AI_FLAG_POWERFUL_STATUS);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_CARBINK) { Speed(10); Moves(MOVE_TRICK_ROOM, MOVE_TACKLE); }
        OPPONENT(SPECIES_PERSIAN) { Speed(200); Moves(MOVE_FAKE_OUT, MOVE_TACKLE); }
    } WHEN {
        TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI strategy: tied speeds do not justify cancelling Trick Room")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_DOUBLE_BATTLE | AI_FLAG_POWERFUL_STATUS);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TRICK_ROOM, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_CARBINK) { Speed(100); Moves(MOVE_TRICK_ROOM, MOVE_TACKLE); }
        OPPONENT(SPECIES_RELICANTH) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_TRICK_ROOM); NOT_EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM); }
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); NOT_EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI strategy: Trick Room ignores a fainted opponent's speed")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_DOUBLE_BATTLE | AI_FLAG_POWERFUL_STATUS);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); HP(1); }
        OPPONENT(SPECIES_CARBINK) { Speed(10); Moves(MOVE_TRICK_ROOM, MOVE_TACKLE); }
        OPPONENT(SPECIES_RELICANTH) { Speed(20); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentRight, MOVE_TACKLE, target: playerRight); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI strategy: an exhausted second setter cannot refresh Trick Room")
{
    PASSES_RANDOMLY(100, 100, RNG_AI_REFRESH_TRICK_ROOM_ON_LAST_TURN);
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_CARBINK) { Speed(10); Moves(MOVE_TRICK_ROOM, MOVE_TACKLE); }
        OPPONENT(SPECIES_RELICANTH) { Speed(20); MovesWithPP({MOVE_TRICK_ROOM, 0}, {MOVE_TACKLE, 35}); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_TACKLE); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_TACKLE); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_TACKLE); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_TACKLE); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI strategy: Quincy targets a foe whose ability can actually be changed")
{
    enum Item shield;
    PARAMETRIZE { shield = ITEM_NONE; }
    PARAMETRIZE { shield = ITEM_ABILITY_SHIELD; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_DOUBLE_BATTLE | AI_FLAG_OMNISCIENT);
        // Keep Machamp the stronger threat on BOTH attacking stats. The old
        // Abra's default Sp. Atk outranked it even without Ability Shield.
        PLAYER(SPECIES_MACHAMP) { Attack(400); SpAttack(20); Item(shield); }
        PLAYER(SPECIES_ABRA) { Attack(20); SpAttack(100); }
        OPPONENT(SPECIES_DURANT) { Ability(ABILITY_TRUANT); Moves(MOVE_ENTRAINMENT, MOVE_IRON_HEAD); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        BattleAI_SetDynamicFunc(AI_EC_QuincyTruant);
        TURN { EXPECT_MOVE(opponentLeft, MOVE_ENTRAINMENT, target: shield == ITEM_ABILITY_SHIELD ? playerRight : playerLeft); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI strategy: Quincy does not spam Entrainment while Neutralizing Gas suppresses Truant")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_DOUBLE_BATTLE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Attack(200); }
        PLAYER(SPECIES_WOBBUFFET) { Attack(20); }
        OPPONENT(SPECIES_DURANT) { Ability(ABILITY_TRUANT); Moves(MOVE_ENTRAINMENT, MOVE_IRON_HEAD); }
        OPPONENT(SPECIES_WEEZING) { Ability(ABILITY_NEUTRALIZING_GAS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        BattleAI_SetDynamicFunc(AI_EC_QuincyTruant);
        TURN { EXPECT_MOVE(opponentLeft, MOVE_IRON_HEAD); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_IRON_HEAD); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI strategy: redirection's setup bonus ends when the partner attacks")
{
    GIVEN {
        WITH_CONFIG(AI_REVERSE_BATTLER_LOGIC_ORDER_CHANCE, 0);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GARCHOMP) { MovesWithPP({MOVE_SWORDS_DANCE, 0}, {MOVE_TACKLE, 35}); }
        OPPONENT(SPECIES_TOGEKISS) { Moves(MOVE_FOLLOW_ME, MOVE_TACKLE); }
    } WHEN {
        BattleAI_SetDynamicFunc(AI_EC_RedirectionSetup);
        TURN { EXPECT_MOVE(opponentLeft, MOVE_TACKLE); }
        TURN {
            EXPECT_MOVE(opponentLeft, MOVE_TACKLE);
            SCORE_EQ_VAL(opponentRight, MOVE_FOLLOW_ME, AI_SCORE_DEFAULT, target: playerLeft);
        }
    }
}

AI_SINGLE_BATTLE_TEST("AI strategy: singles damage predictions use the active terrain")
{
    enum Ability surge;
    PARAMETRIZE { surge = ABILITY_PSYCHIC_SURGE; }
    PARAMETRIZE { surge = ABILITY_ELECTRIC_SURGE; }
    PARAMETRIZE { surge = ABILITY_GRASSY_SURGE; }
    PARAMETRIZE { surge = ABILITY_MISTY_SURGE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER);
        PLAYER(SPECIES_WOBBUFFET) { Ability(surge); }
        OPPONENT(SPECIES_METAGROSS) { Moves(MOVE_STEEL_ROLLER, MOVE_IRON_HEAD); }
    } WHEN {
        TURN { EXPECT_MOVE(opponent, MOVE_STEEL_ROLLER); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI strategy: doubles damage predictions use the active terrain")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_PSYCHIC_SURGE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_METAGROSS) { Moves(MOVE_STEEL_ROLLER, MOVE_IRON_HEAD); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_STEEL_ROLLER); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI strategy: Tailwind can prepare for the turn Trick Room expires")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_POWERFUL_STATUS);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_DRAGON_RAGE, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TRICK_ROOM, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ZIGZAGOON) { HP(1); Speed(10); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_TORNADUS) { Speed(50); Moves(MOVE_TAILWIND, MOVE_TACKLE); }
    } WHEN {
        // Bring the setter in AFTER Room starts. Otherwise an already-active
        // Tailwind hides whether the AI actually respects Trick Room.
        TURN {
            MOVE(playerLeft, MOVE_DRAGON_RAGE, target: opponentLeft);
            MOVE(playerRight, MOVE_TRICK_ROOM);
            EXPECT_MOVE(opponentLeft, MOVE_TACKLE);
            EXPECT_SEND_OUT(opponentLeft, 2);
        }
        TURN {
            EXPECT_MOVE(opponentLeft, MOVE_TACKLE);
        }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_TACKLE); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_TACKLE); }
        TURN {
            EXPECT_MOVE(opponentLeft, MOVE_TAILWIND);
        }
    } SCENE {
        MESSAGE("Wobbuffet twisted the dimensions!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, opponentLeft);
        MESSAGE("The twisted dimensions returned to normal!");
    }
}

AI_DOUBLE_BATTLE_TEST("AI strategy: speed boosts and opposing speed drops lose value during Trick Room")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { HP(900); MaxHP(900); Speed(100); }
        PLAYER(SPECIES_WOBBUFFET) { HP(900); MaxHP(900); Speed(100); }
        OPPONENT(SPECIES_CARBINK) { Speed(10); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_RELICANTH) { Speed(20); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_TACKLE); }
    } THEN {
        enum BattlerId attacker = opponentLeft - gBattleMons;
        enum BattlerId target = playerLeft - gBattleMons;
        gFieldStatuses |= STATUS_FIELD_TRICK_ROOM;
        gFieldTimers.trickRoomTimer = 4;
        EXPECT_LT(IncreaseStatUpScore(attacker, target, STAT_SPEED, 2), 0);
        EXPECT_LT(IncreaseStatDownScore(attacker, target, STAT_SPEED), 0);
        gFieldTimers.trickRoomTimer = 1;
        EXPECT_GE(IncreaseStatUpScore(attacker, target, STAT_SPEED, 2), 0);
        EXPECT_GE(IncreaseStatDownScore(attacker, target, STAT_SPEED), 0);
    }
}
