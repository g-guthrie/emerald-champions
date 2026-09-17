#include "global.h"
#include "battle.h"
#include "battle_util.h"
#include "test/battle.h"
#include "constants/opponents.h"

AI_DOUBLE_BATTLE_TEST("EC Mega reveal: shows the form against guarding foes without inventing eligibility")
{
    u16 item;
    PARAMETRIZE { item = ITEM_AERODACTYLITE; }
    PARAMETRIZE { item = ITEM_NONE; }
    GIVEN {
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_ROXANNE_1;
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY
            | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Level(14); Moves(MOVE_PROTECT); }
        PLAYER(SPECIES_WOBBUFFET) { Level(14); Moves(MOVE_PROTECT); }
        OPPONENT(SPECIES_AERODACTYL) {
            Level(14); Ability(ABILITY_UNNERVE); Item(item);
            Moves(MOVE_ROCK_SLIDE, MOVE_DUAL_WINGBEAT, MOVE_TAILWIND, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_CARBINK) {
            Level(17); Ability(ABILITY_STURDY); Item(ITEM_LIGHT_CLAY);
            Moves(MOVE_REFLECT, MOVE_LIGHT_SCREEN, MOVE_BODY_PRESS, MOVE_ROCK_TOMB);
        }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        EXPECT_EQ(opponentLeft->species, item == ITEM_AERODACTYLITE ? SPECIES_AERODACTYL_MEGA : SPECIES_AERODACTYL);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_1), item == ITEM_AERODACTYLITE ? ABILITY_TOUGH_CLAWS : ABILITY_UNNERVE);
    }
}

// Nicolas E0128 turn 4, and Michelle E0484 turn 1 in the same shape across
// three builds: the holder spent its turn on a status move and kept its stone.
// Altaria is the expensive case, because base form is Dragon/Flying and takes
// Ice at four times while Mega Altaria is Dragon/Fairy and takes it at two, so
// declining the form is most of a body. The only existing Mega fixture stands
// on two Wobbuffet using Protect, which by the board rule is no evidence at
// all about a live decision - the AI has no competing use for the turn there.
// This one has a partner that attacks and two foes that can kill.
#define MEGA_ELECTION_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY \
    | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC Mega election: the stone is spent on a status turn as readily as on an attack")
{
    bool32 statusOnly;
    // One arm keeps Nicolas's authored Altaria whole, so the AI is free to
    // attack; the other removes Double-Edge, so whatever it picks is a status
    // move. If the election only happens behind an attack, the second arm is
    // the one that fails and the first is the control that says the board is
    // otherwise sound.
    PARAMETRIZE { statusOnly = FALSE; }
    PARAMETRIZE { statusOnly = TRUE; }
    GIVEN {
        AI_FLAGS(MEGA_ELECTION_FLAGS);
        PLAYER(SPECIES_WEAVILE) {
            Level(50); HP(280); MaxHP(280); Attack(170); Defense(90);
            SpAttack(70); SpDefense(90); Speed(175);
            Moves(MOVE_ICE_FANG, MOVE_PROTECT);
        }
        PLAYER(SPECIES_KANGASKHAN) {
            Level(50); HP(320); MaxHP(320); Attack(150); Defense(110);
            SpAttack(60); SpDefense(110); Speed(120);
            Moves(MOVE_DOUBLE_EDGE, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_ALTARIA) {
            Level(50); HP(280); MaxHP(280); Attack(140); Defense(110);
            SpAttack(80); SpDefense(125); Speed(100);
            Ability(ABILITY_NATURAL_CURE); Item(ITEM_ALTARIANITE);
            Moves(MOVE_DRAGON_DANCE, statusOnly ? MOVE_ROOST : MOVE_DOUBLE_EDGE, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_DRACOZOLT) {
            Level(50); HP(300); MaxHP(300); Attack(160); Defense(100);
            SpAttack(70); SpDefense(100); Speed(115);
            Ability(ABILITY_HUSTLE); Moves(MOVE_BOLT_BEAK, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ICE_FANG, target: opponentRight);
            MOVE(playerRight, MOVE_DOUBLE_EDGE, target: opponentRight);
        }
    } THEN {
        // Evolving costs nothing and the form is strictly better here, so the
        // turn's chosen action must not decide whether it happens.
        EXPECT_EQ(opponentLeft->species, SPECIES_ALTARIA_MEGA);
    }
}

// Champion Wallace's Mega Starmie sat four turns across three builds reporting
// the Mega as usable and never evolving. One activation marks both partner
// slots as having used the gimmick, and the gate the AI controller consults on
// its way to executing a choice asked only that, never the trainer's allowance.
// Drake is the cheap board for it: his authored stones are party slots 1 and 6,
// so the lead evolves immediately and the reserve has to evolve after it.
AI_DOUBLE_BATTLE_TEST("EC Mega budget: a licensed trainer's reserve still evolves after its partner")
{
    GIVEN {
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_DRAKE;
        AI_FLAGS(MEGA_ELECTION_FLAGS);
        PLAYER(SPECIES_WEAVILE) {
            Level(50); HP(400); MaxHP(400); Attack(60); Defense(200);
            SpAttack(200); SpDefense(200); Speed(200);
            Moves(MOVE_BLIZZARD, MOVE_PROTECT);
        }
        PLAYER(SPECIES_WOBBUFFET) {
            Level(50); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5);
            Moves(MOVE_PROTECT);
        }
        // Slot 1: licensed, and bulky enough to stand through the sweep.
        OPPONENT(SPECIES_KANGASKHAN) {
            Level(50); HP(400); MaxHP(400); Attack(120); Defense(200);
            SpDefense(200); Speed(100); Item(ITEM_KANGASKHANITE);
            Moves(MOVE_DOUBLE_EDGE, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(5); HP(1); MaxHP(1); Speed(1); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_MAGIKARP) { Level(5); HP(1); MaxHP(1); Speed(1); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_MAGIKARP) { Level(5); HP(1); MaxHP(1); Speed(1); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_MAGIKARP) { Level(5); HP(1); MaxHP(1); Speed(1); Moves(MOVE_SPLASH); }
        // Slot 6: the reserve the defect silenced.
        OPPONENT(SPECIES_SALAMENCE) {
            Level(50); HP(300); MaxHP(300); Attack(130); Defense(120);
            SpDefense(120); Speed(110); Item(ITEM_SALAMENCITE);
            Moves(MOVE_DRAGON_CLAW, MOVE_PROTECT);
        }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_BLIZZARD); MOVE(playerRight, MOVE_PROTECT); }
        TURN { MOVE(playerLeft, MOVE_BLIZZARD); MOVE(playerRight, MOVE_PROTECT); }
        TURN { MOVE(playerLeft, MOVE_BLIZZARD); MOVE(playerRight, MOVE_PROTECT); }
        TURN { MOVE(playerLeft, MOVE_BLIZZARD); MOVE(playerRight, MOVE_PROTECT); }
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        // The lead spent the side's first stone.
        EXPECT_EQ(opponentLeft->species, SPECIES_KANGASKHAN_MEGA);
        // And the licensed reserve still evolved behind it.
        EXPECT_EQ(opponentRight->species, SPECIES_SALAMENCE_MEGA);
        EXPECT_EQ(gBattleStruct->gimmick.megaEvolutionsUsed[B_TRAINER_OPPONENT_A], 2);
    }
}
