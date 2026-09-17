#include "global.h"
#include "test/battle.h"

// L(2)/O(4)/Q(3)/R(2) and the 301-312 block: six rooms where a pressured body
// was withdrawn and the reserve died on entry to an attack the player had
// already shown. The body coming in has to be charged for what the revealed
// attacker does to its typing, and a switch that loses it outright can never
// score above standing still.
#define ENTRY_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC switch-in: a reserve is not sent into the attack that has already been shown")
{
    GIVEN {
        AI_FLAGS(ENTRY_FLAGS);
        // The lead is under real pressure, which is the condition that waives
        // the switch costs. The only reserve worth the slot by matchup is four
        // times weak to the move the attacker opposite used last turn, and
        // dies on entry without acting. Staying is worse for the lead and
        // better for the side.
        PLAYER(SPECIES_GARDEVOIR) { Level(50); HP(300); MaxHP(300); SpAttack(200); Speed(200); Ability(ABILITY_PIXILATE); Moves(MOVE_MOONBLAST); }
        PLAYER(SPECIES_MAGIKARP) { Level(50); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_TREVENANT) { Level(50); HP(120); MaxHP(300); Defense(90); SpDefense(90); Speed(40); Ability(ABILITY_NATURAL_CURE); Moves(MOVE_SHADOW_CLAW); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MANECTRIC) { Level(50); HP(60); MaxHP(300); Defense(60); SpDefense(60); Speed(120); Ability(ABILITY_LIGHTNING_ROD); Moves(MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_STEELIX) { Level(50); HP(300); MaxHP(300); Defense(200); SpDefense(120); Speed(20); Ability(ABILITY_STURDY); Moves(MOVE_IRON_TAIL); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_MOONBLAST, target: opponentLeft);
            MOVE(playerRight, MOVE_SPLASH);
        }
        TURN {
            MOVE(playerLeft, MOVE_MOONBLAST, target: opponentLeft);
            MOVE(playerRight, MOVE_SPLASH);
        }
    } THEN {
        // Whatever it does with the pressured lead, the frail reserve that the
        // revealed attack removes on arrival must not be the answer.
        EXPECT_NE(opponentLeft->species, SPECIES_MANECTRIC);
    }
}

AI_DOUBLE_BATTLE_TEST("EC switch-in: the reserve that hits hardest is not the one that dies first")
{
    GIVEN {
        AI_FLAGS(ENTRY_FLAGS);
        // Olivia's and Bethany's shape: the lead is under pressure, so leaving
        // is free, and the most attractive reserve on offence is a frail
        // special attacker that the revealed priority attack removes before it
        // can use any of it. The bulky body on the same bench survives and
        // attacks next turn.
        PLAYER(SPECIES_RILLABOOM) { Level(50); HP(400); MaxHP(400); Attack(200); Defense(150); SpDefense(150); Speed(80); Ability(ABILITY_GRASSY_SURGE); Moves(MOVE_GRASSY_GLIDE); }
        PLAYER(SPECIES_MAGIKARP) { Level(50); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_ARAQUANID) { Level(50); HP(90); MaxHP(300); Defense(90); SpDefense(90); Speed(40); Ability(ABILITY_WATER_BUBBLE); Moves(MOVE_LIQUIDATION); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_INTELEON) { Level(50); HP(80); MaxHP(260); Defense(50); SpDefense(50); SpAttack(220); Speed(140); Ability(ABILITY_TORRENT); Moves(MOVE_ICE_BEAM); }
        OPPONENT(SPECIES_TOXAPEX) { Level(50); HP(300); MaxHP(300); Defense(180); SpDefense(180); Speed(20); Ability(ABILITY_REGENERATOR); Moves(MOVE_SCALD); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_GRASSY_GLIDE, target: opponentLeft);
            MOVE(playerRight, MOVE_SPLASH);
        }
        TURN {
            MOVE(playerLeft, MOVE_GRASSY_GLIDE, target: opponentLeft);
            MOVE(playerRight, MOVE_SPLASH);
        }
    } THEN {
        // The frail cannon may not be the answer to a revealed priority attack
        // that is doubled against it.
        EXPECT_NE(opponentLeft->species, SPECIES_INTELEON);
    }
}
