#include "global.h"
#include "test/battle.h"

#define CADENCE_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

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
            EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
    } THEN {
        if (!airborne)
            EXPECT_EQ(playerRight->pp[0], 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Protect cadence: Fake Out permission follows the committed move")
{
    bool32 fakeOut;
    PARAMETRIZE { fakeOut = FALSE; }
    PARAMETRIZE { fakeOut = TRUE; }
    GIVEN {
        AI_FLAGS(CADENCE_FLAGS);
        PLAYER(SPECIES_MIENFOO) { HP(500); MaxHP(500); Attack(300); Speed(100); Moves(MOVE_BRICK_BREAK, MOVE_FAKE_OUT); }
        PLAYER(SPECIES_MACHOP) { HP(500); MaxHP(500); Attack(300); Speed(90); Moves(MOVE_BRICK_BREAK); }
        OPPONENT(SPECIES_NOSEPASS) { HP(50); MaxHP(50); Defense(50); Speed(20); Ability(ABILITY_MAGNET_PULL); Moves(MOVE_TACKLE, MOVE_PROTECT); }
        OPPONENT(SPECIES_NOSEPASS) { HP(50); MaxHP(50); Defense(50); Speed(10); Ability(ABILITY_MAGNET_PULL); Moves(MOVE_TACKLE, MOVE_PROTECT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, fakeOut ? MOVE_FAKE_OUT : MOVE_BRICK_BREAK, target: opponentLeft);
            MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentRight);
        }
    } THEN {
        if (fakeOut) {
            EXPECT_EQ(opponentLeft->hp, 50);
            EXPECT_EQ(opponentRight->hp, 50);
        } else {
            EXPECT(opponentLeft->hp == 0 || opponentRight->hp == 0);
        }
    }
}
