#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"

// Emerald Champions: a Water move is never super effective against a Steam
// Engine holder (src/battle_util.c SteamEngineCapsWater); the Speed trigger
// is unchanged.

SINGLE_BATTLE_TEST("Steam Engine: Water hits Coalossal neutrally and still raises Speed by 6", s16 damage)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_STEAM_ENGINE; }
    PARAMETRIZE { ability = ABILITY_FLAME_BODY; }
    GIVEN {
        PLAYER(SPECIES_COALOSSAL) { Ability(ability); HP(600); MaxHP(600); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        HP_BAR(player, captureDamage: &results[i].damage);
        if (ability == ABILITY_STEAM_ENGINE)
        {
            NOT MESSAGE("It's extremely effective!");
            ABILITY_POPUP(player, ABILITY_STEAM_ENGINE);
        }
        else
        {
            MESSAGE("It's extremely effective!");
        }
    } THEN {
        if (ability == ABILITY_STEAM_ENGINE)
            EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 6);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(4.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Steam Engine: Water resistances stay; Fire is unchanged", s16 damage)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_STEAM_ENGINE; }
    PARAMETRIZE { ability = ABILITY_FLAME_BODY; }
    GIVEN {
        PLAYER(SPECIES_COALOSSAL) { Ability(ability); HP(600); MaxHP(600); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_EMBER); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_EMBER); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Steam Engine: Mold Breaker ignores the Water cap", s16 damage)
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_MOLD_BREAKER; }
    PARAMETRIZE { ability = ABILITY_TELEPATHY; }
    GIVEN {
        PLAYER(SPECIES_COALOSSAL) { Ability(ABILITY_STEAM_ENGINE); HP(600); MaxHP(600); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
        if (ability == ABILITY_MOLD_BREAKER)
            MESSAGE("It's extremely effective!");
        else
            NOT MESSAGE("It's extremely effective!");
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(4.0), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Steam Engine: Gastro Acid removes the Water cap")
{
    GIVEN {
        PLAYER(SPECIES_COALOSSAL) { Ability(ABILITY_STEAM_ENGINE); HP(600); MaxHP(600); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_GASTRO_ACID, MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_GASTRO_ACID); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        MESSAGE("It's extremely effective!");
    }
}

AI_SINGLE_BATTLE_TEST("Steam Engine: the AI's effectiveness and damage estimates use the Water cap")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_STEAM_ENGINE; }
    PARAMETRIZE { ability = ABILITY_FLAME_BODY; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_COALOSSAL) { Ability(ability); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); EXPECT_MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        enum BattlerId attacker = player - gBattleMons;
        enum BattlerId target = opponent - gBattleMons;
        EXPECT_EQ(AI_GetMoveEffectiveness(MOVE_WATER_GUN, attacker, target),
                  ability == ABILITY_STEAM_ENGINE ? UQ_4_12(1.0) : UQ_4_12(4.0));
        EXPECT_EQ(CalcPartyMonTypeEffectivenessMultiplier(MOVE_WATER_GUN, SPECIES_COALOSSAL, ability),
                  ability == ABILITY_STEAM_ENGINE ? UQ_4_12(1.0) : UQ_4_12(4.0));
    }
}
