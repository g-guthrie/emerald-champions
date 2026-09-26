#include "global.h"
#include "test/battle.h"
#include "constants/songs.h"

// The victory tune starts as the opponents' last Pokémon faints, before its
// "fainted!" message, as in the newer games; never while the opposing side
// still has a Pokémon to send out.

WILD_BATTLE_TEST("Victory music: the wild tune starts as the wild Pokémon faints")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WURMPLE) { HP(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        MUSIC(MUS_VICTORY_WILD);
        MESSAGE("The wild Wurmple fainted!");
    }
}

SINGLE_BATTLE_TEST("Victory music: the trainer tune starts as the last Pokémon faints, not the first")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WURMPLE) { HP(1); }
        OPPONENT(SPECIES_CATERPIE) { HP(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 1); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("The opposing Wurmple fainted!");
        NONE_OF { MUSIC(MUS_VICTORY_TRAINER); }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        MUSIC(MUS_VICTORY_TRAINER);
        MESSAGE("The opposing Caterpie fainted!");
    }
}

DOUBLE_BATTLE_TEST("Victory music: in a double battle the tune waits for the second opposing Pokémon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WURMPLE) { HP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CATERPIE) { HP(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft); }
        TURN { MOVE(playerLeft, MOVE_TACKLE, target: opponentRight); }
    } SCENE {
        MESSAGE("The opposing Wurmple fainted!");
        NONE_OF { MUSIC(MUS_VICTORY_TRAINER); }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerLeft);
        MUSIC(MUS_VICTORY_TRAINER);
        MESSAGE("The opposing Caterpie fainted!");
    }
}

DOUBLE_BATTLE_TEST("Victory music: a spread move that knocks out both last Pokémon starts the tune with the first faint")
{
    GIVEN {
        ASSUME(GetMoveTarget(MOVE_HYPER_VOICE) == TARGET_BOTH);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_HYPER_VOICE); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WURMPLE) { HP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CATERPIE) { HP(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_HYPER_VOICE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, playerLeft);
        MUSIC(MUS_VICTORY_TRAINER);
        MESSAGE("The opposing Wurmple fainted!");
        MESSAGE("The opposing Caterpie fainted!");
    }
}

MULTI_BATTLE_TEST("Victory music: against two trainers the tune waits until both are out of Pokémon")
{
    GIVEN {
        ASSUME(GetMoveTarget(MOVE_HYPER_VOICE) == TARGET_BOTH);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_HYPER_VOICE, MOVE_TACKLE); }
        PARTNER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT_A(SPECIES_WURMPLE) { HP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT_B(SPECIES_CATERPIE) { HP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT_B(SPECIES_WEEDLE) { HP(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_HYPER_VOICE); MOVE(playerRight, MOVE_CELEBRATE); SEND_OUT(opponentRight, 1); }
        TURN { MOVE(playerLeft, MOVE_TACKLE, target: opponentRight); MOVE(playerRight, MOVE_CELEBRATE); }
    } SCENE {
        // Opponent A is out of Pokémon and both opposing slots are empty, but
        // opponent B still has Weedle to send out.
        MESSAGE("The opposing Wurmple fainted!");
        NONE_OF { MUSIC(MUS_VICTORY_TRAINER); }
        MESSAGE("The opposing Caterpie fainted!");
        NONE_OF { MUSIC(MUS_VICTORY_TRAINER); }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerLeft);
        MUSIC(MUS_VICTORY_TRAINER);
        MESSAGE("The opposing Weedle fainted!");
    }
}

SINGLE_BATTLE_TEST("Victory music: a recoil knockout by the player's last Pokémon waits until the recoil")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_DOUBLE_EDGE) == EFFECT_RECOIL);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_DOUBLE_EDGE); }
        OPPONENT(SPECIES_WURMPLE) { HP(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_EDGE); }
    } SCENE {
        MESSAGE("The opposing Wurmple fainted!");
        MESSAGE("Wobbuffet was damaged by the recoil!");
        MUSIC(MUS_VICTORY_TRAINER);
    }
}
