#include "global.h"
#include "test/battle.h"

// Every message about the hit itself (critical hit, effectiveness, the
// multi-strike hit count) prints before the knocked-out Pokémon faints, as in
// the modern games. The faint message is the last word on the attack.

WILD_BATTLE_TEST("Knockout order: a multi-strike KO of a wild Pokémon reports effectiveness and hit count before the faint")
{
    GIVEN {
        ASSUME(IsMultiHitMove(MOVE_BULLET_SEED));
        PLAYER(SPECIES_TREECKO) { Moves(MOVE_BULLET_SEED); }
        OPPONENT(SPECIES_WURMPLE) { HP(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_BULLET_SEED); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        HP_BAR(opponent);
        MESSAGE("It's not very effective…");
        MESSAGE("The Pokémon was hit 1 time!");
        MESSAGE("The wild Wurmple fainted!");
    }
}

SINGLE_BATTLE_TEST("Knockout order: a multi-strike move that knocks out on its second hit reports the hits before the faint")
{
    GIVEN {
        ASSUME(IsMultiHitMove(MOVE_BULLET_SEED));
        ASSUME(gItemsInfo[ITEM_FOCUS_SASH].holdEffect == HOLD_EFFECT_FOCUS_SASH);
        PLAYER(SPECIES_TREECKO) { Moves(MOVE_BULLET_SEED); }
        OPPONENT(SPECIES_WURMPLE) { MaxHP(2); HP(2); Item(ITEM_FOCUS_SASH); }
        OPPONENT(SPECIES_WURMPLE);
    } WHEN {
        TURN { MOVE(player, MOVE_BULLET_SEED); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        MESSAGE("The opposing Wurmple hung on using its Focus Sash!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLET_SEED, player);
        MESSAGE("It's not very effective…");
        MESSAGE("The Pokémon was hit 2 times!");
        MESSAGE("The opposing Wurmple fainted!");
    }
}

SINGLE_BATTLE_TEST("Knockout order: a single-strike KO reports a critical hit and effectiveness before the faint")
{
    GIVEN {
        PLAYER(SPECIES_MUDKIP) { Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_CHARMANDER) { HP(1); }
        OPPONENT(SPECIES_CHARMANDER);
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, criticalHit: TRUE); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent);
        MESSAGE("A critical hit!");
        MESSAGE("It's super effective!");
        MESSAGE("The opposing Charmander fainted!");
    }
}

DOUBLE_BATTLE_TEST("Knockout order: a spread move reports every target's hit before either target faints")
{
    GIVEN {
        ASSUME(GetMoveTarget(MOVE_DAZZLING_GLEAM) == TARGET_BOTH);
        ASSUME(GetMoveType(MOVE_DAZZLING_GLEAM) == TYPE_FAIRY);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_DAZZLING_GLEAM); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MACHOP) { HP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARMANDER) { HP(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_DAZZLING_GLEAM, criticalHit: TRUE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DAZZLING_GLEAM, playerLeft);
        HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
        MESSAGE("A critical hit on the opposing Machop!");
        MESSAGE("It's super effective on the opposing Machop!");
        MESSAGE("A critical hit on the opposing Charmander!");
        MESSAGE("It's not very effective on the opposing Charmander.");
        MESSAGE("The opposing Machop fainted!");
        MESSAGE("The opposing Charmander fainted!");
    }
}

DOUBLE_BATTLE_TEST("Knockout order: a spread move that knocks out one target still reports the survivor's hit first")
{
    GIVEN {
        ASSUME(GetMoveTarget(MOVE_DAZZLING_GLEAM) == TARGET_BOTH);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_DAZZLING_GLEAM); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MACHOP) { HP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHARMANDER) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_DAZZLING_GLEAM); SEND_OUT(opponentLeft, 2); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DAZZLING_GLEAM, playerLeft);
        MESSAGE("It's super effective on the opposing Machop!");
        MESSAGE("It's not very effective on the opposing Charmander.");
        MESSAGE("The opposing Machop fainted!");
    }
}
