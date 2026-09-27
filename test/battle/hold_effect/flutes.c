#include "global.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("Flutes: protect both active allies repeatedly and respect holder Klutz")
{
    enum Item flute;
    enum Move move;
    enum Ability ability;
    for (u32 suppressed = 0; suppressed < 2; suppressed++)
    {
        PARAMETRIZE { flute = ITEM_BLUE_FLUTE; move = MOVE_SPORE; ability = suppressed ? ABILITY_KLUTZ : ABILITY_GLUTTONY; }
        PARAMETRIZE { flute = ITEM_YELLOW_FLUTE; move = MOVE_CONFUSE_RAY; ability = suppressed ? ABILITY_KLUTZ : ABILITY_GLUTTONY; }
        PARAMETRIZE { flute = ITEM_RED_FLUTE; move = MOVE_TAUNT; ability = suppressed ? ABILITY_KLUTZ : ABILITY_GLUTTONY; }
    }
    GIVEN {
        PLAYER(SPECIES_ZIGZAGOON) { Item(flute); Ability(ability); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, move, target: playerLeft); MOVE(opponentRight, move, target: playerRight); }
        if (ability != ABILITY_KLUTZ)
            TURN { MOVE(opponentLeft, move, target: playerLeft); MOVE(opponentRight, move, target: playerRight); }
    } THEN {
        bool32 blocked = ability != ABILITY_KLUTZ;
        if (flute == ITEM_BLUE_FLUTE)
        {
            EXPECT_EQ(!!(playerLeft->status1 & STATUS1_SLEEP), !blocked);
            EXPECT_EQ(!!(playerRight->status1 & STATUS1_SLEEP), !blocked);
        }
        else if (flute == ITEM_YELLOW_FLUTE)
        {
            EXPECT_EQ(!!playerLeft->volatiles.confusionTimer, !blocked);
            EXPECT_EQ(!!playerRight->volatiles.confusionTimer, !blocked);
        }
        else
        {
            EXPECT_EQ(!!playerLeft->volatiles.tauntTimer, !blocked);
            EXPECT_EQ(!!playerRight->volatiles.tauntTimer, !blocked);
        }
        EXPECT_EQ(playerLeft->item, flute);
    }
}

DOUBLE_BATTLE_TEST("Flutes: Red protects both allies from Encore without being consumed")
{
    GIVEN {
        PLAYER(SPECIES_ZIGZAGOON) { Item(ITEM_RED_FLUTE); Ability(ABILITY_GLUTTONY); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); }
        TURN { MOVE(opponentLeft, MOVE_ENCORE, target: playerLeft); MOVE(opponentRight, MOVE_ENCORE, target: playerRight); }
    } THEN {
        EXPECT_EQ((u32)playerLeft->volatiles.encoredMove, MOVE_NONE);
        EXPECT_EQ((u32)playerRight->volatiles.encoredMove, MOVE_NONE);
        EXPECT_EQ(playerLeft->item, ITEM_RED_FLUTE);
    }
}

DOUBLE_BATTLE_TEST("Flutes: a benched holder no longer protects its active partner")
{
    GIVEN {
        PLAYER(SPECIES_ZIGZAGOON) { Item(ITEM_YELLOW_FLUTE); Ability(ABILITY_GLUTTONY); }
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(playerLeft, 2); MOVE(opponentLeft, MOVE_CONFUSE_RAY, target: playerRight); }
    } THEN {
        EXPECT(playerRight->volatiles.confusionTimer != 0);
    }
}

DOUBLE_BATTLE_TEST("Flutes: Blue prevents an ally using Rest while the flute is active")
{
    GIVEN {
        PLAYER(SPECIES_ZIGZAGOON) { Item(ITEM_BLUE_FLUTE); Ability(ABILITY_GLUTTONY); }
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(200); HP(100); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_REST); }
    } THEN {
        EXPECT_EQ(playerRight->status1 & STATUS1_SLEEP, 0);
        EXPECT_EQ(playerRight->hp, 100);
    }
}
