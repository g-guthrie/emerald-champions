#include "global.h"
#include "test/battle.h"

// W(4), K(1) and S(1): a support turn that is worth more than the attack, a
// second guard with a knockout on the table, and a boost handed to a partner
// that is spending its turn behind a shield.
#define JOINT_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC joint conflicts: the wind beats the attack when the whole side is outsped")
{
    GIVEN {
        AI_FLAGS(JOINT_FLAGS);
        // Yveltal's board: both of its side are slower than both of mine, and
        // the drain attack it preferred does not change that. The wind does,
        // for every turn after this one.
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(400); MaxHP(400); Attack(30); Defense(200); SpDefense(200); Speed(160); Ability(ABILITY_TELEPATHY); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(400); MaxHP(400); Attack(30); Defense(200); SpDefense(200); Speed(150); Ability(ABILITY_TELEPATHY); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_YVELTAL) {
            Level(50); HP(400); MaxHP(400); SpAttack(120); Defense(150); SpDefense(150); Speed(100);
            Ability(ABILITY_DARK_AURA); Moves(MOVE_TAILWIND, MOVE_OBLIVION_WING);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(50); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(90); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_TAILWIND);
        }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_TAILWIND);
    }
}

AI_DOUBLE_BATTLE_TEST("EC joint conflicts: a knockout beats a second guard")
{
    GIVEN {
        AI_FLAGS(JOINT_FLAGS);
        // Mega Camerupt's board. The receipt has it guarding twice with a
        // knockout in hand; here the knockout wins on the first turn, so the
        // second guard never arises to be discounted. The guard the receipt
        // saw cannot be set up from a fixture without scripting the AI's own
        // first move, which an AI test may not do.
        PLAYER(SPECIES_AMOONGUSS) { Level(50); HP(77); MaxHP(400); Attack(150); Defense(60); SpDefense(60); Speed(200); Ability(ABILITY_EFFECT_SPORE); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_MAGIKARP) { Level(50); HP(400); MaxHP(400); Attack(150); Defense(200); SpDefense(200); Speed(150); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_CAMERUPT) {
            Level(50); HP(200); MaxHP(300); SpAttack(200); Defense(90); SpDefense(90); Speed(40);
            Ability(ABILITY_SOLID_ROCK); Moves(MOVE_PROTECT, MOVE_EARTH_POWER);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentLeft);
            EXPECT_MOVE(opponentLeft, MOVE_EARTH_POWER, target: playerLeft);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC joint conflicts: a boost is not handed to a partner that is guarding")
{
    GIVEN {
        AI_FLAGS(JOINT_FLAGS);
        // The pair chooses both actions together, so it knows what the partner
        // is about to do. Helping Hand multiplies nothing behind a shield.
        // The attack beside the support move has to be worth something, or
        // the choice is between two nothings and pins nothing.
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(200); MaxHP(400); Attack(300); Defense(60); SpDefense(60); Speed(200); Ability(ABILITY_TELEPATHY); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_MAGIKARP) { Level(50); HP(400); MaxHP(400); Attack(300); Defense(200); SpDefense(200); Speed(150); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_CRESSELIA) {
            Level(50); HP(140); MaxHP(344); SpAttack(200); Defense(150); SpDefense(150); Speed(60);
            Ability(ABILITY_LEVITATE); Moves(MOVE_HELPING_HAND, MOVE_MOONBLAST);
        }
        OPPONENT(SPECIES_LUNATONE) {
            Level(50); HP(60); MaxHP(300); SpAttack(80); Defense(90); SpDefense(90); Speed(50);
            Ability(ABILITY_LEVITATE); Moves(MOVE_PROTECT, MOVE_PSYCHIC);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_HELPING_HAND);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC joint conflicts: a spread move loses to a single lane once Wide Guard has been shown")
{
    bool32 shown;
    PARAMETRIZE { shown = TRUE; }
    PARAMETRIZE { shown = FALSE; }
    GIVEN {
        AI_FLAGS(JOINT_FLAGS);
        // Wallace's board: the side opposite has used Wide Guard, and modern
        // Wide Guard is freely repeatable, so the spread attack is likely to
        // blank again. The single-target lane still gets through.
        PLAYER(SPECIES_MAGEARNA) { Level(50); HP(300); MaxHP(300); Defense(120); SpDefense(120); Speed(140); Ability(ABILITY_SOUL_HEART); Moves(shown ? MOVE_WIDE_GUARD : MOVE_CELEBRATE, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(300); MaxHP(300); Defense(120); SpDefense(120); Speed(20); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_KYOGRE) {
            Level(50); HP(400); MaxHP(400); SpAttack(180); Defense(150); SpDefense(150); Speed(90);
            Ability(ABILITY_DRIZZLE); Moves(MOVE_ORIGIN_PULSE, MOVE_ICE_BEAM);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(50); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, shown ? MOVE_WIDE_GUARD : MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            // Either slot is a fair lane; what matters is that the spread
            // attack is no longer the choice.
            if (shown)
                EXPECT_MOVE(opponentLeft, MOVE_ICE_BEAM);
            else
                EXPECT_MOVE(opponentLeft, MOVE_ORIGIN_PULSE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC joint conflicts: the HP-scaled spread wins at full health")
{
    GIVEN {
        AI_FLAGS(JOINT_FLAGS);
        // Kyogre's board: at full health Water Spout is a hundred and fifty
        // that cannot miss, against a hundred and ten at eighty-five accuracy.
        // The bodies opposite hit back, because the live board does and a
        // board that cannot fight back was not testing the question: an
        // HP-scaled move must be priced at the health its user will have when
        // it acts, not at the health the forecast leaves it with afterwards.
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(300); MaxHP(300); Attack(200); Defense(120); SpDefense(120); Speed(20); Ability(ABILITY_TELEPATHY); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_MAGIKARP) { Level(50); HP(300); MaxHP(300); Attack(200); Defense(120); SpDefense(120); Speed(10); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_KYOGRE) {
            Level(50); HP(400); MaxHP(400); SpAttack(180); Defense(150); SpDefense(150); Speed(90);
            Ability(ABILITY_DRIZZLE); Moves(MOVE_WATER_SPOUT, MOVE_ORIGIN_PULSE);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(50); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentLeft);
            EXPECT_MOVE(opponentLeft, MOVE_WATER_SPOUT);
        }
    }
}
