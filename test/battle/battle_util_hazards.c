#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Battle utility: Toxic Spikes absorption and Boots protection stay distinct")
{
    enum Species species;
    enum Item item;
    PARAMETRIZE { species = SPECIES_NIDORINA; item = ITEM_HEAVY_DUTY_BOOTS; }
    PARAMETRIZE { species = SPECIES_EEVEE; item = ITEM_HEAVY_DUTY_BOOTS; }
    PARAMETRIZE { species = SPECIES_EEVEE; item = ITEM_NONE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); Speed(30); Moves(MOVE_SPLASH); }
        PLAYER(species) { Item(item); Speed(10); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); Speed(20); Moves(MOVE_TOXIC_SPIKES, MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_TOXIC_SPIKES); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_SPLASH); }
    } THEN {
        EXPECT_EQ((u32)gSideTimers[B_SIDE_PLAYER].toxicSpikesAmount, species == SPECIES_NIDORINA ? 0 : 1);
        EXPECT_EQ(player->status1, item == ITEM_NONE ? STATUS1_POISON : STATUS1_NONE);
    }
}
