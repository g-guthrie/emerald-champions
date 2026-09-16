#include "global.h"
#include "test/battle.h"

// Group D: values that only arrive on a later turn. A speed drop that buys a
// slow partner the order, an item swap, a boost something can actually cash,
// and a trade made by a body that is already dead. Each has to be worth a real
// number so it can compete with a direct attack, and worth nothing when the
// board does not supply the payoff.
#define NEXT_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC next-turn values: a speed drop is worth the order it buys a slow partner")
{
    bool32 crossing;
    PARAMETRIZE { crossing = TRUE; }
    PARAMETRIZE { crossing = FALSE; }
    GIVEN {
        AI_FLAGS(NEXT_FLAGS);
        // Crossing: the drop puts the slow partner in front of the threat.
        // Otherwise the same two stages leave the order exactly as it was and
        // the drop is only chip, so the attack keeps the turn.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Defense(60); SpDefense(60); Speed(crossing ? 60 : 300); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_ARIADOS) {
            Level(30); HP(200); MaxHP(200); Attack(200); Speed(120);
            Ability(ABILITY_INSOMNIA); Moves(MOVE_STRING_SHOT, MOVE_MEGAHORN);
        }
        OPPONENT(SPECIES_SLAKOTH) { Level(30); HP(200); MaxHP(200); Attack(150); Speed(50); Ability(ABILITY_TRUANT); Moves(MOVE_SLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_SPLASH);
            if (crossing)
                EXPECT_MOVE(opponentLeft, MOVE_STRING_SHOT);
            else
                EXPECT_MOVE(opponentLeft, MOVE_MEGAHORN, target: playerLeft);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC next-turn values: a boost nothing can cash is not worth a turn")
{
    bool32 physical;
    PARAMETRIZE { physical = TRUE; }
    PARAMETRIZE { physical = FALSE; }
    GIVEN {
        AI_FLAGS(NEXT_FLAGS);
        // Howl raises Attack. On a body whose only damaging move is special it
        // buys nothing at all, and the attack has to win instead.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(5); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(5); Defense(200); SpDefense(200); Speed(8); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_POOCHYENA) {
            Level(30); HP(200); MaxHP(200); Attack(90); SpAttack(90); Speed(80);
            Ability(ABILITY_RUN_AWAY);
            Moves(MOVE_HOWL, physical ? MOVE_BITE : MOVE_SWIFT, MOVE_DARK_PULSE);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            if (physical)
                EXPECT_MOVE(opponentLeft, MOVE_HOWL);
            else
                NOT_EXPECT_MOVE(opponentLeft, MOVE_HOWL);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC next-turn values: a swap hands over the item the user cannot use")
{
    bool32 junk;
    PARAMETRIZE { junk = TRUE; }
    PARAMETRIZE { junk = FALSE; }
    GIVEN {
        AI_FLAGS(NEXT_FLAGS);
        // Klutz cannot use anything it holds, so trading a dead Life Orb for
        // the foe's working one is free value. Handing a working item to an
        // empty hand is the same move in reverse and a pure gift.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(10); Item(junk ? ITEM_LIFE_ORB : ITEM_NONE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(8); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BUNEARY) {
            Level(30); HP(200); MaxHP(200); Attack(70); Speed(90);
            Ability(junk ? ABILITY_KLUTZ : ABILITY_RUN_AWAY); Item(ITEM_LIFE_ORB);
            Moves(MOVE_SWITCHEROO, MOVE_DIZZY_PUNCH);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            if (junk)
                EXPECT_MOVE(opponentLeft, MOVE_SWITCHEROO, target: playerLeft);
            else
                NOT_EXPECT_MOVE(opponentLeft, MOVE_SWITCHEROO);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC next-turn values: Destiny Bond trades a body that is already lost")
{
    bool32 doomed;
    PARAMETRIZE { doomed = TRUE; }
    PARAMETRIZE { doomed = FALSE; }
    GIVEN {
        AI_FLAGS(NEXT_FLAGS);
        // Night Shade is fixed damage the AI has now seen, so on the small
        // body the next one is certain and lands first: the bond is a fair
        // trade. On the large body nothing is certain and it would only waste
        // the turn. Turn one is deliberately before the move is known - the AI
        // does not get to trade a body on a threat it has not been shown.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(200); Moves(MOVE_NIGHT_SHADE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MISDREAVUS) {
            Level(30); HP(doomed ? 45 : 400); MaxHP(doomed ? 45 : 400); Defense(60); SpAttack(80); SpDefense(60); Speed(60);
            Ability(ABILITY_LEVITATE); Moves(MOVE_DESTINY_BOND, MOVE_SHADOW_BALL);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_NIGHT_SHADE, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_DESTINY_BOND);
        }
        TURN {
            MOVE(playerLeft, MOVE_NIGHT_SHADE, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            if (doomed)
                EXPECT_MOVE(opponentLeft, MOVE_DESTINY_BOND);
            else
                EXPECT_MOVE(opponentLeft, MOVE_SHADOW_BALL, target: playerLeft);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC next-turn values: Shell Smash is taken behind the Herb that undoes it")
{
    GIVEN {
        AI_FLAGS(NEXT_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(5); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(5); Defense(200); SpDefense(200); Speed(8); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_CLOYSTER) {
            Level(30); HP(200); MaxHP(200); Attack(90); Defense(180); SpDefense(40); Speed(70);
            Ability(ABILITY_SHELL_ARMOR); Item(ITEM_WHITE_HERB); Moves(MOVE_SHELL_SMASH, MOVE_ICICLE_SPEAR);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_SHELL_SMASH);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
    }
}

AI_DOUBLE_BATTLE_TEST("EC next-turn values: a Wish at full health heals nothing")
{
    GIVEN {
        AI_FLAGS(NEXT_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(5); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(5); Defense(200); SpDefense(200); Speed(8); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_TOGETIC) {
            Level(30); HP(200); MaxHP(200); SpAttack(90); Speed(70);
            Ability(ABILITY_HUSTLE); Moves(MOVE_WISH, MOVE_DAZZLING_GLEAM);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(5); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_WISH);
        }
    }
}
