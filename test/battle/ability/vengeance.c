#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Vengeance boosts Ghost moves by 20%, or 50% at a third of HP or less", s16 damage)
{
    enum Ability ability;
    u16 hp;

    PARAMETRIZE { ability = ABILITY_INSOMNIA; hp = 300; }
    PARAMETRIZE { ability = ABILITY_VENGEANCE; hp = 300; }
    PARAMETRIZE { ability = ABILITY_INSOMNIA; hp = 100; }
    PARAMETRIZE { ability = ABILITY_VENGEANCE; hp = 100; }

    GIVEN {
        ASSUME(GetMoveType(MOVE_SHADOW_CLAW) == TYPE_GHOST);
        PLAYER(SPECIES_BANETTE) { Ability(ability); HP(hp); MaxHP(300); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SHADOW_CLAW); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHADOW_CLAW, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        // The boost scales the Attack stat, so allow the damage formula's
        // intermediate rounding (and its fixed +2) a couple of percent either way.
        EXPECT_GE(results[1].damage * 100, results[0].damage * 118);
        EXPECT_LE(results[1].damage * 100, results[0].damage * 121);
        EXPECT_GE(results[3].damage * 100, results[2].damage * 147);
        EXPECT_LE(results[3].damage * 100, results[2].damage * 151);
    }
}

SINGLE_BATTLE_TEST("Vengeance leaves non-Ghost moves unchanged", s16 damage)
{
    enum Ability ability;

    PARAMETRIZE { ability = ABILITY_INSOMNIA; }
    PARAMETRIZE { ability = ABILITY_VENGEANCE; }

    GIVEN {
        PLAYER(SPECIES_BANETTE) { Ability(ability); HP(1); MaxHP(300); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
