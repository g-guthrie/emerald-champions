#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Leppa Berry: restores an exhausted copied move without changing the permanent moves")
{
    enum Move copyingMove;
    PARAMETRIZE { copyingMove = MOVE_TRANSFORM; }
    PARAMETRIZE { copyingMove = MOVE_MIMIC; }
    GIVEN {
        PLAYER(SPECIES_DITTO) { Ability(ABILITY_LIMBER); Item(ITEM_LEPPA_BERRY); Speed(1); MovesWithPP({copyingMove, 10}); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, copyingMove); MOVE(opponent, MOVE_SPLASH); }
        for (u32 i = 0; i < 5; i++)
            TURN { MOVE(player, moveSlot: 0); MOVE(opponent, MOVE_SPLASH); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(player->moves[0], MOVE_SPLASH);
        EXPECT_EQ(player->pp[0], 5);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE1), copyingMove);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP1), 9);
    }
}

SINGLE_BATTLE_TEST("Leppa Berry: permanent PP restoration remains synchronized and Ripen doubles it")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_GLUTTONY; }
    PARAMETRIZE { ability = ABILITY_RIPEN; }
    GIVEN {
        PLAYER(SPECIES_SNORLAX) { Ability(ability); Item(ITEM_LEPPA_BERRY); MovesWithPP({MOVE_SPLASH, 1}); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_SPLASH); }
    } THEN {
        u32 restored = ability == ABILITY_RIPEN ? 20 : 10;
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(player->pp[0], restored);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP1), restored);
    }
}

SINGLE_BATTLE_TEST("PP drain: draining copied PP preserves permanent PP and permits Leppa recovery")
{
    enum Move copyingMove;
    enum Move drainMove = MOVE_EERIE_SPELL;
    enum Item item;
    PARAMETRIZE { copyingMove = MOVE_TRANSFORM; item = ITEM_NONE; }
    PARAMETRIZE { copyingMove = MOVE_MIMIC; item = ITEM_NONE; }
    PARAMETRIZE { copyingMove = MOVE_TRANSFORM; item = ITEM_LEPPA_BERRY; }
    PARAMETRIZE { copyingMove = MOVE_MIMIC; item = ITEM_LEPPA_BERRY; }
    PARAMETRIZE { copyingMove = MOVE_TRANSFORM; item = ITEM_NONE; drainMove = MOVE_SPITE; }
    PARAMETRIZE { copyingMove = MOVE_MIMIC; item = ITEM_NONE; drainMove = MOVE_SPITE; }
    GIVEN {
        PLAYER(SPECIES_DITTO) { Ability(ABILITY_LIMBER); HP(500); MaxHP(500); Item(item); Speed(1); MovesWithPP({copyingMove, 10}); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); SpAttack(1); Moves(MOVE_SPLASH, drainMove); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPLASH); MOVE(player, copyingMove); }
        TURN { MOVE(opponent, MOVE_SPLASH); MOVE(player, moveSlot: 0); }
        TURN { MOVE(opponent, drainMove); MOVE(player, moveSlot: 0); }
    } THEN {
        EXPECT_EQ(player->moves[0], MOVE_SPLASH);
        EXPECT_EQ(player->pp[0], item == ITEM_LEPPA_BERRY ? 5 : 0);
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE1), copyingMove);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP1), 9);
    }
}

SINGLE_BATTLE_TEST("PP drain: permanent PP drain clamps to the remaining PP and synchronizes the party")
{
    u32 pp;
    enum Move drainMove;
    PARAMETRIZE { pp = 2; drainMove = MOVE_EERIE_SPELL; }
    PARAMETRIZE { pp = 6; drainMove = MOVE_EERIE_SPELL; }
    PARAMETRIZE { pp = 2; drainMove = MOVE_SPITE; }
    PARAMETRIZE { pp = 6; drainMove = MOVE_SPITE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(2); MovesWithPP({MOVE_SPLASH, pp}); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(1); Speed(1); Moves(drainMove); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, drainMove); }
    } THEN {
        u32 used = drainMove == MOVE_SPITE ? 5 : 4;
        u32 expected = pp > used ? pp - used : 0;
        EXPECT_EQ(player->pp[0], expected);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP1), expected);
    }
}
