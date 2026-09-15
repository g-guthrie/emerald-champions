#include "global.h"
#include "test/battle.h"

#define RETARGET_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC fainted target: guard the fallback only when earlier damage can remove the selected foe")
{
    enum Move firstMove;
    bool32 knockout;
    PARAMETRIZE { firstMove = MOVE_STRENGTH; knockout = TRUE; }
    PARAMETRIZE { firstMove = MOVE_ROCK_THROW; knockout = TRUE; }
    PARAMETRIZE { firstMove = MOVE_STRENGTH; knockout = FALSE; }
    GIVEN {
        AI_FLAGS(RETARGET_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Attack(100); Moves(firstMove); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(90); SpAttack(100); Moves(MOVE_ICE_BEAM); }
        OPPONENT(SPECIES_MAGIKARP) { HP(knockout ? 1 : 300); MaxHP(300); Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DRAGONITE) {
            HP(100); MaxHP(100); SpDefense(100); Speed(20);
            Ability(ABILITY_INNER_FOCUS); Moves(MOVE_DRAGON_CLAW, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, firstMove, target: opponentLeft, hit: TRUE);
            MOVE(playerRight, MOVE_ICE_BEAM, target: opponentLeft);
            // Whether the fallback is guarded when the selected foe survives is
            // an expected-value margin: both attacks could still be retargeted.
            if (knockout)
                EXPECT_MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        if (knockout)
            EXPECT_EQ(opponentLeft->hp, 0);
        else
            EXPECT_GT(opponentLeft->hp, 0);
        EXPECT_EQ(opponentRight->hp, 100);
    }
}

AI_DOUBLE_BATTLE_TEST("EC fainted target: Brenden cannot hide his partner behind a doomed switch recipient")
{
    GIVEN {
        AI_FLAGS(RETARGET_FLAGS);
        PLAYER(SPECIES_SHAYMIN) {
            Level(20); HP(76); MaxHP(76); Attack(45); Defense(51);
            SpAttack(69); SpDefense(51); Speed(63);
            Ability(ABILITY_NATURAL_CURE); Item(ITEM_CHOICE_SPECS);
            Moves(MOVE_SEED_FLARE, MOVE_EARTH_POWER, MOVE_GIGA_DRAIN, MOVE_PSYCHIC);
        }
        PLAYER(SPECIES_COMBUSKEN) {
            Level(20); HP(60); MaxHP(60); Attack(40); Defense(35);
            SpAttack(57); SpDefense(35); Speed(49);
            Ability(ABILITY_SPEED_BOOST); Item(ITEM_LIFE_ORB);
            Moves(MOVE_HEAT_WAVE, MOVE_FLAMETHROWER, MOVE_FEINT, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_THROH) {
            Level(19); HP(92); MaxHP(92); Attack(66); Defense(43);
            SpAttack(19); SpDefense(43); Speed(27);
            Ability(ABILITY_GUTS); Item(ITEM_LEFTOVERS);
            Moves(MOVE_STORM_THROW, MOVE_BODY_SLAM, MOVE_BULK_UP, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_MACHOP) {
            // Nothing to do and nearly dead, so departing is unambiguous
            // without reading the pending Feint.
            Level(20); HP(6); MaxHP(76); Attack(1); Defense(31);
            SpAttack(22); SpDefense(25); Speed(25);
            Ability(ABILITY_NO_GUARD); Item(ITEM_EVIOLITE);
            Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_MONFERNO) {
            Level(21); HP(64); MaxHP(64); Attack(57); Defense(33);
            SpAttack(39); SpDefense(33); Speed(63);
            Ability(ABILITY_BLAZE); Item(ITEM_EVIOLITE);
            Moves(MOVE_FIRE_PUNCH, MOVE_MACH_PUNCH, MOVE_BRICK_BREAK, MOVE_U_TURN);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft);
            MOVE(playerRight, MOVE_FEINT, target: opponentRight);
            // The doomed flank leaving is the subject; how the healthy one
            // spends the same turn is an expected-value choice.
            EXPECT_SWITCH(opponentRight, 2);
        }
        TURN {
            MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft);
            MOVE(playerRight, MOVE_HEAT_WAVE, hit: TRUE);
            // Heat Wave removes a switched-in Machop before Psychic acts.
            // Psychic then attacks Monferno; sacrificing Machop cannot hide it.
            EXPECT_MOVES(opponentLeft, MOVE_STORM_THROW, MOVE_BODY_SLAM, MOVE_BULK_UP, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_GT(opponentRight->hp, 0);
    }
}
