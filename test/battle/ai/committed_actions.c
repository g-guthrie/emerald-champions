#include "global.h"
#include "test/battle.h"

#define EC_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_DOUBLE_BATTLE | AI_FLAG_KNOW_OPPONENT_PARTY)

AI_DOUBLE_BATTLE_TEST("EC committed actions: attack the flank that did not choose Protect")
{
    u32 flank;
    PARAMETRIZE { flank = 0; }
    PARAMETRIZE { flank = 1; }
    GIVEN {
        AI_FLAGS(EC_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Moves(MOVE_PROTECT, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Moves(MOVE_PROTECT, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_TAUROS) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, flank == 0 ? MOVE_PROTECT : MOVE_CELEBRATE);
            MOVE(playerRight, flank == 1 ? MOVE_PROTECT : MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_TACKLE, target: flank == 0 ? playerRight : playerLeft);
        }
    } THEN {
        EXPECT_EQ(flank == 0 ? playerLeft->hp : playerRight->hp, 500);
        EXPECT_LT(flank == 0 ? playerRight->hp : playerLeft->hp, 500);
    }
}

AI_DOUBLE_BATTLE_TEST("EC committed actions: Protect only the target of the confirmed attack")
{
    u32 flank;
    PARAMETRIZE { flank = 0; }
    PARAMETRIZE { flank = 1; }
    GIVEN {
        AI_FLAGS(EC_FLAGS);
        PLAYER(SPECIES_TAUROS) { HP(500); MaxHP(500); Attack(300); Speed(100); Moves(MOVE_STRENGTH); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ESPEON) { HP(100); MaxHP(100); Defense(100); Speed(50); Moves(MOVE_PSYBEAM, MOVE_PROTECT); }
        OPPONENT(SPECIES_ESPEON) { HP(100); MaxHP(100); Defense(100); Speed(50); Moves(MOVE_PSYBEAM, MOVE_PROTECT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_STRENGTH, target: flank == 0 ? opponentLeft : opponentRight);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, flank == 0 ? MOVE_PROTECT : MOVE_PSYBEAM);
            EXPECT_MOVE(opponentRight, flank == 1 ? MOVE_PROTECT : MOVE_PSYBEAM);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->hp, 100);
        EXPECT_EQ(opponentRight->hp, 100);
        EXPECT_LT(playerLeft->hp, 500);
    }
}

AI_DOUBLE_BATTLE_TEST("EC committed actions: score the actual switch recipient without giving it an attack")
{
    GIVEN {
        AI_FLAGS(EC_FLAGS);
        PLAYER(SPECIES_GYARADOS) { HP(500); MaxHP(500); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Moves(MOVE_PROTECT); }
        PLAYER(SPECIES_GASTRODON) { HP(500); MaxHP(500); Moves(MOVE_EARTH_POWER); }
        OPPONENT(SPECIES_ROTOM) { Moves(MOVE_THUNDERBOLT, MOVE_SHADOW_BALL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            SWITCH(playerLeft, 2);
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentLeft, MOVE_SHADOW_BALL, target: playerLeft);
        }
    } THEN {
        EXPECT_EQ(playerLeft->species, SPECIES_GASTRODON);
        EXPECT_LT(playerLeft->hp, 500);
        EXPECT_EQ(playerRight->hp, 500);
    }
}

AI_DOUBLE_BATTLE_TEST("EC committed actions: retain the shared decision after either AI flank faints")
{
    u32 flank;
    PARAMETRIZE { flank = 0; }
    PARAMETRIZE { flank = 1; }
    GIVEN {
        AI_FLAGS(EC_FLAGS);
        PLAYER(SPECIES_TAUROS) { HP(500); MaxHP(500); Moves(MOVE_QUICK_ATTACK, MOVE_PROTECT); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
        OPPONENT(flank == 0 ? SPECIES_WOBBUFFET : SPECIES_TAUROS) {
            HP(flank == 0 ? 1 : 100); MaxHP(100); Moves(flank == 0 ? MOVE_CELEBRATE : MOVE_TACKLE);
        }
        OPPONENT(flank == 1 ? SPECIES_WOBBUFFET : SPECIES_TAUROS) {
            HP(flank == 1 ? 1 : 100); MaxHP(100); Moves(flank == 1 ? MOVE_CELEBRATE : MOVE_TACKLE);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_QUICK_ATTACK, target: flank == 0 ? opponentLeft : opponentRight);
            MOVE(playerRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(flank == 0 ? opponentRight : opponentLeft, MOVE_TACKLE, target: playerRight);
        }
    } THEN {
        EXPECT_EQ(flank == 0 ? opponentLeft->hp : opponentRight->hp, 0);
        EXPECT_LT(playerRight->hp, 500);
    }
}

AI_DOUBLE_BATTLE_TEST("EC committed actions: opposing Sturdy Policy changes whether attacking is safe")
{
    bool32 policy;
    PARAMETRIZE { policy = FALSE; }
    PARAMETRIZE { policy = TRUE; }
    GIVEN {
        AI_FLAGS(EC_FLAGS | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS);
        PLAYER(SPECIES_MIENFOO) {
            Level(14); HP(49); MaxHP(49); Attack(33); Defense(23); Speed(27); SpAttack(21); SpDefense(35);
            Ability(ABILITY_INNER_FOCUS); Item(ITEM_EVIOLITE); Moves(MOVE_PROTECT);
        }
        PLAYER(SPECIES_BONSLY) {
            Level(14); HP(51); MaxHP(51); Attack(44); Defense(36); Speed(12); SpAttack(10); SpDefense(21);
            Ability(ABILITY_STURDY); Item(policy ? ITEM_WEAKNESS_POLICY : ITEM_NONE); Moves(MOVE_ROCK_SLIDE);
        }
        OPPONENT(SPECIES_TREECKO) {
            Level(16); HP(43); MaxHP(43); Attack(34); Defense(21); Speed(46); SpAttack(27); SpDefense(27);
            Ability(ABILITY_OVERGROW); Item(ITEM_SITRUS_BERRY);
            Moves(MOVE_SEED_BOMB, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_PIKACHU) {
            Level(16); HP(42); MaxHP(42); Attack(24); Defense(22); Speed(52); SpAttack(36); SpDefense(25);
            Ability(ABILITY_LIGHTNING_ROD); Item(ITEM_LIGHT_BALL);
            // Isolate the observed Helping Hand + Seed Bomb exchange. The
            // protected flank leaves one attack target and a guard alternative.
            Moves(MOVE_HELPING_HAND);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_ROCK_SLIDE, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (policy)
                EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
            else {
                EXPECT_MOVE(opponentLeft, MOVE_SEED_BOMB, target: playerRight);
                EXPECT_MOVE(opponentRight, MOVE_HELPING_HAND, target: opponentLeft);
            }
        }
    } THEN {
        // Activating the opponent's Policy must not hand it two knockouts.
        EXPECT_GT(opponentLeft->hp + opponentRight->hp, 0);
        if (!policy)
            EXPECT_EQ(playerRight->hp, 1);
    }
}

AI_DOUBLE_BATTLE_TEST("EC committed actions: a double knockout loads both AI reserves and continues")
{
    GIVEN {
        AI_FLAGS(EC_FLAGS);
        PLAYER(SPECIES_ARTICUNO) { Moves(MOVE_BLIZZARD); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BLIZZARD, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_BLIZZARD, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_WOBBUFFET);
        EXPECT_EQ(opponentRight->species, SPECIES_WOBBUFFET);
        EXPECT_GT(opponentLeft->hp, 0);
        EXPECT_GT(opponentRight->hp, 0);
    }
}
