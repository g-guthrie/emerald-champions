#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Prism Scales reduces special damage without reducing physical damage", s16 damage)
{
    enum Move move;
    enum Ability ability;
    PARAMETRIZE { move = MOVE_PSYCHIC; ability = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_PSYCHIC; ability = ABILITY_PRISM_SCALES; }
    PARAMETRIZE { move = MOVE_SCRATCH; ability = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_SCRATCH; ability = ABILITY_PRISM_SCALES; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MILOTIC) { Ability(ability); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.7), results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Power Fists boosts punches and targets Special Defense", s16 damage)
{
    enum Ability ability;
    enum Move move;
    PARAMETRIZE { ability = ABILITY_NONE; move = MOVE_MEGA_PUNCH; }
    PARAMETRIZE { ability = ABILITY_POWER_FISTS; move = MOVE_MEGA_PUNCH; }
    PARAMETRIZE { ability = ABILITY_NONE; move = MOVE_SCRATCH; }
    PARAMETRIZE { ability = ABILITY_POWER_FISTS; move = MOVE_SCRATCH; }
    GIVEN {
        PLAYER(SPECIES_MACHAMP) { Ability(ability); Attack(200); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); Defense(100); SpDefense(200); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.6), results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Sand Song makes sound moves hit Electric types super effectively")
{
    GIVEN {
        PLAYER(SPECIES_FLYGON) { Ability(ABILITY_SAND_SONG); }
        OPPONENT(SPECIES_RAICHU);
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); }
    } SCENE {
        HP_BAR(opponent);
        MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Sand Song leaves nonsound Normal moves unable to hit Ghost types")
{
    GIVEN {
        PLAYER(SPECIES_FLYGON) { Ability(ABILITY_SAND_SONG); }
        OPPONENT(SPECIES_DUSKULL) { Ability(ABILITY_FRISK); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("It doesn't affect the opposing Duskull…");
        NONE_OF { HP_BAR(opponent); }
    }
}
