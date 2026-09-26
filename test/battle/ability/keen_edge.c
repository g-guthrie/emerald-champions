#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Keen Edge boosts slicing moves by 30%", s16 damage)
{
    enum Ability ability;
    enum Move move;

    PARAMETRIZE { ability = ABILITY_TORRENT; move = MOVE_RAZOR_SHELL; }
    PARAMETRIZE { ability = ABILITY_KEEN_EDGE; move = MOVE_RAZOR_SHELL; }
    PARAMETRIZE { ability = ABILITY_TORRENT; move = MOVE_WATERFALL; }
    PARAMETRIZE { ability = ABILITY_KEEN_EDGE; move = MOVE_WATERFALL; }

    GIVEN {
        ASSUME(IsSlicingMove(MOVE_RAZOR_SHELL));
        ASSUME(!IsSlicingMove(MOVE_WATERFALL));
        PLAYER(SPECIES_SAMUROTT) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}
