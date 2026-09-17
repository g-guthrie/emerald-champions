#include "global.h"
#include "test/battle.h"

// The L(1) family re-run on the depth fix, plus two vetoes from group P that
// belong to the same question: what a turn spent on something other than
// damage is worth, and when it is worth nothing at all.
#define STARVE_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC starvation: Tailwind is taken on the board that was built for it")
{
    GIVEN {
        AI_FLAGS(STARVE_FLAGS);
        // Wendy's board: both of hers are outsped, both live through the turn,
        // and the wind flips the order for the rest of the battle.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(20); Defense(200); SpDefense(200); Speed(150); Ability(ABILITY_TELEPATHY); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(20); Defense(200); SpDefense(200); Speed(140); Ability(ABILITY_TELEPATHY); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_SWANNA) {
            Level(30); HP(300); MaxHP(300); SpAttack(90); Defense(90); Speed(100);
            Ability(ABILITY_KEEN_EYE); Moves(MOVE_TAILWIND, MOVE_AIR_SLASH);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Defense(120); SpDefense(120); Speed(90); Moves(MOVE_SPLASH); }
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

AI_DOUBLE_BATTLE_TEST("EC starvation: Dragon Dance is taken on a turn nothing can punish")
{
    GIVEN {
        AI_FLAGS(STARVE_FLAGS);
        // Flint's board: the setter is faster than both, takes almost nothing
        // from either, and the dance pays on every turn after this one.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(10); SpAttack(10); Defense(200); SpDefense(200); Speed(20); Ability(ABILITY_TELEPATHY); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(10); SpAttack(10); Defense(200); SpDefense(200); Speed(15); Ability(ABILITY_TELEPATHY); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_DRAGONITE) {
            Level(30); HP(300); MaxHP(300); Attack(110); Defense(110); SpDefense(110); Speed(90);
            Ability(ABILITY_INNER_FOCUS); Moves(MOVE_DRAGON_DANCE, MOVE_DRAGON_CLAW);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentLeft);
            EXPECT_MOVE(opponentLeft, MOVE_DRAGON_DANCE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC starvation: Spore goes in when nothing opposite is immune to it")
{
    GIVEN {
        AI_FLAGS(STARVE_FLAGS);
        // Breloom's board: six turns went by without this in the receipt.
        // Neither body is Grass, holds Goggles or has Overcoat.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(300); MaxHP(300); Attack(150); Defense(120); SpDefense(120); Speed(150); Ability(ABILITY_TELEPATHY); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Attack(5); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_BRELOOM) {
            Level(30); HP(200); MaxHP(200); Attack(120); Defense(60); Speed(70);
            Ability(ABILITY_EFFECT_SPORE); Moves(MOVE_SPORE, MOVE_MACH_PUNCH, MOVE_SEED_BOMB);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            // Either body is a fair target; the receipt's complaint was that
            // the sleep never happened at all.
            EXPECT_MOVE(opponentLeft, MOVE_SPORE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC starvation: a single-target Water move is not fired into the partner's Storm Drain")
{
    bool32 drain;
    PARAMETRIZE { drain = TRUE; }
    PARAMETRIZE { drain = FALSE; }
    GIVEN {
        AI_FLAGS(STARVE_FLAGS);
        // Huntail's board: the Water move is the knockout, and the partner
        // beside it takes every Water move on the field. With an ordinary
        // partner the same attack is simply correct.
        // A body the Water move is doubled against and the Ice move halved,
        // so only the absorbing partner can change the answer.
        PLAYER(SPECIES_TORKOAL) { Level(30); HP(200); MaxHP(300); Defense(60); SpDefense(60); Speed(10); Ability(ABILITY_WHITE_SMOKE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_HUNTAIL) {
            Level(30); HP(300); MaxHP(300); SpAttack(120); Speed(60);
            Ability(ABILITY_SWIFT_SWIM); Moves(MOVE_HYDRO_PUMP, MOVE_ICE_BEAM);
        }
        OPPONENT(SPECIES_GASTRODON) { Level(30); HP(300); MaxHP(300); Speed(20); Ability(drain ? ABILITY_STORM_DRAIN : ABILITY_SAND_FORCE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_SPLASH);
            if (drain)
                NOT_EXPECT_MOVE(opponentLeft, MOVE_HYDRO_PUMP);
            else
                EXPECT_MOVE(opponentLeft, MOVE_HYDRO_PUMP, target: playerLeft);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC starvation: Prankster status is not aimed at a Dark body")
{
    GIVEN {
        AI_FLAGS(STARVE_FLAGS);
        // Liepard's board: Prankster gives the status move priority and a Dark
        // target is immune to it because of that priority. The attack beside
        // it is doubled into the same body, so nothing else is close.
        PLAYER(SPECIES_SABLEYE) { Level(30); HP(200); MaxHP(200); Defense(90); SpDefense(90); Speed(20); Ability(ABILITY_KEEN_EYE); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Defense(120); SpDefense(120); Speed(5); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_LIEPARD) {
            Level(30); HP(200); MaxHP(200); Attack(90); Speed(110);
            Ability(ABILITY_PRANKSTER); Moves(MOVE_ENCORE, MOVE_PLAY_ROUGH);
        }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); HP(300); MaxHP(300); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentLeft);
        }
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentLeft);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_ENCORE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC starvation: an ally-targeted support move resolves to the partner")
{
    GIVEN {
        AI_FLAGS(STARVE_FLAGS);
        // Clawitzer's board: the record showed Heal Pulse aimed at a player
        // slot. A move that can only help an ally has to choose one.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(60); Defense(200); SpDefense(200); Speed(200); Ability(ABILITY_TELEPATHY); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Attack(5); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_CLAWITZER) {
            Level(30); HP(300); MaxHP(300); SpAttack(40); Defense(120); SpDefense(120); Speed(40);
            Ability(ABILITY_MEGA_LAUNCHER); Moves(MOVE_HEAL_PULSE, MOVE_WATER_GUN);
        }
        OPPONENT(SPECIES_LAPRAS) { Level(30); HP(60); MaxHP(400); Defense(120); SpDefense(120); Speed(30); Ability(ABILITY_WATER_ABSORB); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_HEAL_PULSE, target: opponentRight);
        }
    } THEN {
        EXPECT(opponentRight->hp > 60);
    }
}

AI_DOUBLE_BATTLE_TEST("EC starvation: a guard nothing is aimed at is not taken, twice over")
{
    GIVEN {
        AI_FLAGS(STARVE_FLAGS);
        // Jared's Noctowl and Foster's Runerigus: full health, both attacks
        // aimed at the other slot, and a guard chosen anyway on two turns
        // running with the second one failing.
        PLAYER(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Attack(150); Defense(200); SpDefense(200); Speed(200); Ability(ABILITY_TELEPATHY); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Attack(150); Defense(200); SpDefense(200); Speed(150); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_NOCTOWL) {
            Level(30); HP(300); MaxHP(300); SpAttack(90); Defense(120); SpDefense(120); Speed(90);
            Ability(ABILITY_INSOMNIA); Moves(MOVE_PROTECT, MOVE_AIR_SLASH);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC starvation: a healing support move is never aimed at the other side")
{
    GIVEN {
        AI_FLAGS(STARVE_FLAGS);
        // The live Clawitzer board: the foe is missing far more HP than the
        // partner is, which is the shape that made the other side look like
        // the better recipient. A move that only helps whoever it lands on
        // can never be worth landing on them.
        PLAYER(SPECIES_AMOONGUSS) { Level(30); HP(80); MaxHP(400); Attack(60); Defense(120); SpDefense(120); Speed(200); Ability(ABILITY_EFFECT_SPORE); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_MAGIKARP) { Level(30); HP(400); MaxHP(400); Attack(5); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_CLAWITZER) {
            Level(30); HP(300); MaxHP(300); SpAttack(40); Defense(120); SpDefense(120); Speed(40);
            Ability(ABILITY_MEGA_LAUNCHER); Moves(MOVE_HEAL_PULSE, MOVE_WATER_GUN);
        }
        OPPONENT(SPECIES_LAPRAS) { Level(30); HP(360); MaxHP(400); Defense(120); SpDefense(120); Speed(30); Ability(ABILITY_SHELL_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            // Whatever it picks, it must not be a heal for the other side.
            // The HP check below is the assertion that matters: a Heal Pulse
            // landing on the player would put that body back up.
        }
    } THEN {
        EXPECT(playerLeft->hp <= 80);
    }
}

AI_DOUBLE_BATTLE_TEST("EC starvation: the per-battler path also refuses to heal the other side")
{
    GIVEN {
        // Deliberately without AI_FLAG_SMART_MON_CHOICES, so the joint search
        // does not run and the per-battler scorer decides. That is the path
        // the Mega Meganium sighting came from: a body whose attacks are all
        // resisted, where a full-HP heal for the player was the least bad
        // score on the board.
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_SKARMORY) { Level(50); HP(200); MaxHP(300); Defense(200); SpDefense(200); Speed(200); Ability(ABILITY_STURDY); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGIKARP) { Level(50); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(5); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_MEGANIUM) { Level(50); HP(300); MaxHP(300); SpAttack(80); Speed(60); Ability(ABILITY_OVERGROW); Moves(MOVE_HEAL_PULSE, MOVE_ENERGY_BALL); }
        OPPONENT(SPECIES_MAGIKARP) { Level(50); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_SPLASH);
        }
    } THEN {
        // A heal for the other side would put this body back up; damage from
        // the attack is fine and expected.
        EXPECT(playerLeft->hp <= 200);
        EXPECT(playerRight->hp <= 400);
    }
}
