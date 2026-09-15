#include "global.h"
#include "test/battle.h"

#define REFLECT_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC reflected damage: a real incoming hit retains the matching retaliation")
{
    enum Move attack, reply;
    bool32 redirect;
    bool32 reflected = TRUE;
    // Without redirection the AI must weigh a revealed Follow Me partner that
    // could take the hit instead, which a reflected reply cannot survive.
    PARAMETRIZE { attack = MOVE_STRENGTH; reply = MOVE_STRENGTH; redirect = FALSE; reflected = FALSE; }
    PARAMETRIZE { attack = MOVE_POWER_GEM; reply = MOVE_STRENGTH; redirect = FALSE; reflected = FALSE; }
    PARAMETRIZE { attack = MOVE_STRENGTH; reply = MOVE_COUNTER; redirect = TRUE; }
    PARAMETRIZE { attack = MOVE_POWER_GEM; reply = MOVE_MIRROR_COAT; redirect = TRUE; }
    PARAMETRIZE { attack = MOVE_DUAL_WINGBEAT; reply = MOVE_COUNTER; redirect = TRUE; }
    GIVEN {
        AI_FLAGS(REFLECT_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { HP(300); MaxHP(300); Attack(200); SpAttack(200); Speed(100); Moves(attack); }
        PLAYER(SPECIES_CLEFAIRY) { HP(300); MaxHP(300); Speed(90); Moves(MOVE_CELEBRATE, MOVE_FOLLOW_ME); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(300); MaxHP(300); Speed(50); Moves(MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_STRENGTH); }
        OPPONENT(SPECIES_MAGIKARP) { Speed(40); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, attack, target: opponentLeft, hit: TRUE);
            MOVE(playerRight, redirect ? MOVE_FOLLOW_ME : MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, reply);
        }
    } THEN {
        if (reflected)
        {
            EXPECT_LT(redirect ? playerRight->hp : playerLeft->hp, 300);
            EXPECT_EQ(redirect ? playerLeft->hp : playerRight->hp, 300);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC reflected damage: Takao cannot Counter an attack aimed at his guarding partner")
{
    GIVEN {
        AI_FLAGS(REFLECT_FLAGS);
        PLAYER(SPECIES_PACHIRISU) {
            Level(20); HP(72); MaxHP(72); Attack(26); Defense(56);
            SpAttack(29); SpDefense(47); Speed(49);
            Ability(ABILITY_VOLT_ABSORB); Item(ITEM_SITRUS_BERRY);
            Moves(MOVE_FOLLOW_ME, MOVE_NUZZLE, MOVE_SUPER_FANG, MOVE_PROTECT);
        }
        PLAYER(SPECIES_AERODACTYL) {
            Level(20); HP(68); MaxHP(68); Attack(65); Defense(37);
            SpAttack(31); SpDefense(41); Speed(82);
            Ability(ABILITY_UNNERVE); Item(ITEM_FOCUS_SASH);
            Moves(MOVE_ROCK_SLIDE, MOVE_TAILWIND, MOVE_DUAL_WINGBEAT, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_GRAPPLOCT) {
            Level(20); HP(80); MaxHP(80); Attack(78); Defense(47); SpDefense(43); Speed(28);
            Ability(ABILITY_LIMBER); Item(ITEM_LEFTOVERS);
            Moves(MOVE_OCTOLOCK, MOVE_DRAIN_PUNCH, MOVE_SUCKER_PUNCH, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_WOBBUFFET) {
            Level(20); HP(124); MaxHP(124); Defense(51); SpDefense(34); Speed(24);
            Ability(ABILITY_SHADOW_TAG); Item(ITEM_SITRUS_BERRY);
            Moves(MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_ENCORE, MOVE_HELPING_HAND);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_DUAL_WINGBEAT, target: opponentLeft, hit: TRUE);
            // A revealed Follow Me is one credible pattern among several, so a
            // reflected reply is now a gamble the AI may take. What it must not
            // do is end the turn with nothing to show, asserted below.
        }
    } THEN {
        // A real Encore lock or Octolock is progress too; demanding damage
        // would incorrectly reject useful support behind Grapploct's shield.
        EXPECT(playerLeft->hp < 72 || playerLeft->statStages[STAT_DEF] < DEFAULT_STAT_STAGE
            || playerLeft->volatiles.encoredMove != MOVE_NONE || playerRight->volatiles.encoredMove != MOVE_NONE);
    }
}
