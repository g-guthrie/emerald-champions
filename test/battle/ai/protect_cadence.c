#include "global.h"
#include "test/battle.h"

#define CADENCE_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC Protect cadence: a last entrant takes its Fake Out knockout instead of an empty first shield")
{
    GIVEN {
        AI_FLAGS(CADENCE_FLAGS);
        PLAYER(SPECIES_MARSHTOMP) {
            Level(20); HP(76); MaxHP(76); Attack(45); Defense(39);
            SpAttack(51); SpDefense(39); Speed(27);
            Ability(ABILITY_DAMP); Item(ITEM_EVIOLITE);
            Moves(MOVE_MUDDY_WATER, MOVE_EARTH_POWER, MOVE_ICY_WIND, MOVE_WIDE_GUARD);
        }
        PLAYER(SPECIES_AERODACTYL) {
            Level(20); HP(1); MaxHP(68); Attack(65); Defense(37);
            SpAttack(31); SpDefense(41); Speed(82);
            Ability(ABILITY_UNNERVE);
            Moves(MOVE_ROCK_SLIDE, MOVE_TAILWIND, MOVE_DUAL_WINGBEAT, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CROAGUNK) {
            Level(20); HP(55); MaxHP(55); Attack(52); Defense(27);
            SpAttack(31); SpDefense(27); Speed(43);
            Ability(ABILITY_DRY_SKIN); Item(ITEM_EVIOLITE);
            Moves(MOVE_POISON_JAB, MOVE_DRAIN_PUNCH, MOVE_FAKE_OUT, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_EARTH_POWER, target: opponentLeft);
            MOVE(playerRight, MOVE_DUAL_WINGBEAT, target: opponentRight, hit: TRUE);
            EXPECT_SEND_OUT(opponentLeft, 2);
        }
        TURN {
            MOVE(playerLeft, MOVE_EARTH_POWER, target: opponentLeft);
            MOVE(playerRight, MOVE_DUAL_WINGBEAT, target: opponentLeft, hit: TRUE);
            // The flank it flinches is an expected-value choice it may make
            // either way; spending the turn on an empty shield is not.
            EXPECT_MOVE(opponentLeft, MOVE_FAKE_OUT);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->hp, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Protect cadence: a lone survivor needs a payoff to repeat its guard")
{
    bool32 poisonClock;
    PARAMETRIZE { poisonClock = FALSE; }
    PARAMETRIZE { poisonClock = TRUE; }
    GIVEN {
        AI_FLAGS(CADENCE_FLAGS);
        PLAYER(SPECIES_SHAYMIN) {
            Level(14); HP(45); MaxHP(56); SpAttack(46); SpDefense(37); Speed(25);
            Ability(ABILITY_NATURAL_CURE); Item(ITEM_LIFE_ORB);
            Status1(poisonClock ? STATUS1_POISON : STATUS1_NONE);
            Moves(MOVE_GIGA_DRAIN, MOVE_SEED_FLARE, MOVE_EARTH_POWER, MOVE_PROTECT);
        }
        PLAYER(SPECIES_MIENFOO) {
            Level(14); HP(33); MaxHP(49); Attack(33); Defense(23); SpDefense(35); Speed(18);
            Ability(ABILITY_INNER_FOCUS); Item(ITEM_EVIOLITE);
            Moves(MOVE_FAKE_OUT, MOVE_BRICK_BREAK, MOVE_DRAIN_PUNCH, MOVE_HELPING_HAND);
        }
        OPPONENT(SPECIES_NOSEPASS) {
            Level(14); HP(45); MaxHP(45); Defense(47); SpAttack(33); SpDefense(34); Speed(17);
            Ability(ABILITY_MAGNET_PULL); Item(ITEM_SITRUS_BERRY);
            Moves(MOVE_POWER_GEM, MOVE_THUNDERBOLT, MOVE_EARTH_POWER, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_GIGA_DRAIN, target: opponentLeft);
            MOVE(playerRight, MOVE_FAKE_OUT, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
        TURN {
            MOVE(playerLeft, MOVE_GIGA_DRAIN, target: opponentLeft);
            MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentLeft);
            if (poisonClock)
                EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
            else
                NOT_EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
    } THEN {
        if (!poisonClock)
            EXPECT_EQ(opponentLeft->hp, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Protect cadence: repeated Sucker Punch denial remains useful")
{
    GIVEN {
        AI_FLAGS(CADENCE_FLAGS);
        PLAYER(SPECIES_ABSOL) { HP(500); MaxHP(500); Attack(300); Speed(100); Moves(MOVE_SUCKER_PUNCH); }
        PLAYER(SPECIES_MIENFOO) { HP(500); MaxHP(500); Attack(100); Speed(50); Moves(MOVE_FAKE_OUT, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_NOSEPASS) { HP(45); MaxHP(45); Defense(34); Speed(17); Moves(MOVE_POWER_GEM, MOVE_PROTECT); }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SUCKER_PUNCH, target: opponentLeft);
            MOVE(playerRight, MOVE_FAKE_OUT, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
        TURN {
            MOVE(playerLeft, MOVE_SUCKER_PUNCH, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->hp, 45);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Protect cadence: repeated guards retain transient attack windows")
{
    bool32 airborne;
    PARAMETRIZE { airborne = FALSE; }
    PARAMETRIZE { airborne = TRUE; }
    GIVEN {
        AI_FLAGS(CADENCE_FLAGS);
        PLAYER(SPECIES_ARTICUNO) { HP(500); MaxHP(500); Speed(200); Moves(MOVE_FLY, MOVE_CELEBRATE); }
        PLAYER(SPECIES_GARCHOMP) {
            HP(500); MaxHP(500); Attack(300); Speed(100);
            MovesWithPP({MOVE_EARTHQUAKE, airborne ? 10 : 2});
        }
        OPPONENT(SPECIES_NOSEPASS) {
            HP(45); MaxHP(45); Defense(34); Speed(17); Ability(ABILITY_MAGNET_PULL);
            Moves(MOVE_POWER_GEM, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, airborne ? MOVE_FLY : MOVE_CELEBRATE, target: opponentLeft);
            MOVE(playerRight, MOVE_EARTHQUAKE);
            EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
        TURN {
            if (airborne)
                FORCED_MOVE(playerLeft);
            else
                MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_EARTHQUAKE);
            // Exhausting the last Earthquake is a decisive reason to repeat a
            // one-in-three shield. An incoming Fly that lands either way is
            // not: the threat simply returns, so the repeat is not banked.
            if (airborne)
                NOT_EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
            else
                EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
    } THEN {
        if (!airborne)
            EXPECT_EQ(playerRight->pp[0], 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Protect cadence: Fake Out takes the target with the larger expected denial")
{
    GIVEN {
        AI_FLAGS(CADENCE_FLAGS);
        // One flank is a knockout, the other is chip on a bulky attacker.
        // Nothing about the human's pending commands separates them.
        PLAYER(SPECIES_MACHOP) { HP(500); MaxHP(500); Attack(300); Defense(300); Speed(90); Moves(MOVE_BRICK_BREAK); }
        PLAYER(SPECIES_MIENFOO) { HP(3); MaxHP(500); Attack(300); Defense(5); Speed(80); Moves(MOVE_BRICK_BREAK); }
        OPPONENT(SPECIES_MIENFOO) {
            Level(30); HP(200); MaxHP(200); Attack(120); Speed(100);
            Ability(ABILITY_INNER_FOCUS); Moves(MOVE_FAKE_OUT, MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BRICK_BREAK, target: opponentLeft);
            MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentLeft);
            EXPECT_MOVE(opponentLeft, MOVE_FAKE_OUT, target: playerRight);
        }
    } THEN {
        EXPECT_EQ(playerRight->hp, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Protect cadence: an unknowable double target does not outrank an available knockout")
{
    GIVEN {
        AI_FLAGS(CADENCE_FLAGS);
        // Either human flank could add its Brick Break to the other and end
        // Nosepass this turn. Nothing reveals whether they will, and Power Gem
        // removes half of that threat outright, so the shield is not worth it.
        PLAYER(SPECIES_TAUROS) { Level(30); HP(150); MaxHP(150); Attack(300); Defense(80); SpDefense(40); Speed(40); Moves(MOVE_BRICK_BREAK, MOVE_CELEBRATE); }
        PLAYER(SPECIES_MACHOP) { Level(30); HP(500); MaxHP(500); Attack(300); Defense(80); SpDefense(40); Speed(20); Moves(MOVE_BRICK_BREAK, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_NOSEPASS) {
            Level(30); HP(300); MaxHP(300); Defense(60); SpAttack(300); SpDefense(60); Speed(100);
            Ability(ABILITY_MAGNET_PULL); Moves(MOVE_POWER_GEM, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BRICK_BREAK, target: opponentLeft);
            MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentLeft);
            // Which flank it removes is an expected-value choice; spending the
            // turn behind a shield it cannot justify is not.
            NOT_EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
            EXPECT_MOVE(opponentLeft, MOVE_POWER_GEM);
        }
        TURN {
            MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentLeft);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
        TURN { MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentRight); }
    } THEN {
        EXPECT_EQ(opponentLeft->hp, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Protect cadence: a shield that buys the partner's Trick Room is worth the turn")
{
    bool32 payoff;
    PARAMETRIZE { payoff = TRUE; }
    PARAMETRIZE { payoff = FALSE; }
    GIVEN {
        AI_FLAGS(CADENCE_FLAGS);
        PLAYER(SPECIES_TAUROS) { Level(30); HP(400); MaxHP(400); Attack(250); Defense(200); SpDefense(200); Speed(120); Moves(MOVE_STRENGTH); }
        PLAYER(SPECIES_MACHOP) { Level(30); HP(400); MaxHP(400); Attack(250); Defense(200); SpDefense(200); Speed(110); Moves(MOVE_BRICK_BREAK); }
        // The guard's own slot is the obvious double target and cannot win the
        // exchange. Only the partner's turn makes the shield worth its tempo.
        OPPONENT(SPECIES_BRONZOR) {
            Level(30); HP(50); MaxHP(50); Defense(30); SpAttack(20); SpDefense(30); Speed(30);
            Ability(ABILITY_LEVITATE); Moves(MOVE_CONFUSION, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_BRONZONG) {
            Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(10);
            Ability(ABILITY_LEVITATE);
            Moves(payoff ? MOVE_TRICK_ROOM : MOVE_CONFUSION, MOVE_CELEBRATE);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_STRENGTH, target: opponentLeft);
            MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentLeft);
            if (payoff)
                EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
            else
                NOT_EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
    }
}
