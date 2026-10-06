#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"

extern void (*gTestAiTurnSetupHook)(void);

// Moves the doubles AI chose that the battle engine then refused: a side
// condition already up, a Choice lock into an immune target, a reflection
// that cannot land, and a guard repeated into its own failure chance.
#define FAIL_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY \
    | AI_FLAG_CONSERVATIVE | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC failed moves: Tailwind is not set again while it is still blowing")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_MARSHTOMP) { Level(32); Speed(60); Moves(MOVE_MUDDY_WATER, MOVE_ROCK_SLIDE, MOVE_PROTECT, MOVE_TAKE_DOWN); }
        PLAYER(SPECIES_BRELOOM) { Level(32); Speed(70); Moves(MOVE_MACH_PUNCH, MOVE_BULLET_SEED, MOVE_SPORE, MOVE_PROTECT); }
        OPPONENT(SPECIES_SWABLU) {
            Level(35); Item(ITEM_EVIOLITE); Ability(ABILITY_NATURAL_CURE); Nature(NATURE_BOLD); Speed(45);
            Moves(MOVE_HYPER_VOICE, MOVE_ROOST, MOVE_TAILWIND, MOVE_HELPING_HAND);
        }
        OPPONENT(SPECIES_KIRLIA) {
            Level(35); Item(ITEM_EVIOLITE); Ability(ABILITY_TRACE); Nature(NATURE_MODEST); Speed(55);
            Moves(MOVE_PSYCHIC, MOVE_DAZZLING_GLEAM, MOVE_CALM_MIND, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentLeft, MOVE_TAILWIND);
        }
        TURN {
            MOVE(playerLeft, MOVE_TAKE_DOWN, target: opponentRight);
            MOVE(playerRight, MOVE_BULLET_SEED, target: opponentRight);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_TAILWIND);
        }
        TURN {
            MOVE(playerLeft, MOVE_TAKE_DOWN, target: opponentRight);
            MOVE(playerRight, MOVE_BULLET_SEED, target: opponentRight);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_TAILWIND);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC failed moves: a Choice lock that cannot hurt either foe is left, not repeated")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        // Locked into Psychic by the board it chose on, then faced with a
        // Dark body that is immune and a Steel/Psychic body that takes a
        // quarter. Every turn it stays is a turn of nothing.
        PLAYER(SPECIES_MACHAMP) { Level(50); Speed(40); Moves(MOVE_CLOSE_COMBAT, MOVE_ROCK_SLIDE, MOVE_PROTECT, MOVE_KNOCK_OFF); }
        PLAYER(SPECIES_TOXICROAK) { Level(50); Speed(45); Moves(MOVE_DRAIN_PUNCH, MOVE_GUNK_SHOT, MOVE_PROTECT, MOVE_SUCKER_PUNCH); }
        PLAYER(SPECIES_INCINEROAR) { Level(50); Speed(50); Moves(MOVE_FLARE_BLITZ, MOVE_KNOCK_OFF, MOVE_PROTECT, MOVE_PARTING_SHOT); }
        PLAYER(SPECIES_METAGROSS) { Level(50); Speed(60); Moves(MOVE_METEOR_MASH, MOVE_ZEN_HEADBUTT, MOVE_PROTECT, MOVE_ICE_PUNCH); }
        OPPONENT(SPECIES_VICTINI) {
            Level(50); Speed(120); Item(ITEM_CHOICE_SPECS); Ability(ABILITY_VICTORY_STAR); Nature(NATURE_TIMID);
            Moves(MOVE_HEAT_WAVE, MOVE_PSYCHIC, MOVE_FOCUS_BLAST, MOVE_ENERGY_BALL);
        }
        OPPONENT(SPECIES_AUDINO) {
            Level(50); Speed(50); Item(ITEM_SITRUS_BERRY); Ability(ABILITY_REGENERATOR);
            Moves(MOVE_WISH, MOVE_PROTECT, MOVE_HELPING_HAND, MOVE_THROAT_CHOP);
        }
        OPPONENT(SPECIES_CENTISKORCH) {
            Level(50); Speed(70); Item(ITEM_LIFE_ORB); Ability(ABILITY_FLASH_FIRE);
            Moves(MOVE_FIRE_LASH, MOVE_LEECH_LIFE, MOVE_POWER_WHIP, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_BEWEAR) {
            Level(50); Speed(60); Item(ITEM_LIFE_ORB); Ability(ABILITY_FLUFFY);
            Moves(MOVE_DOUBLE_EDGE, MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            SWITCH(playerLeft, 2);
            SWITCH(playerRight, 3);
            EXPECT_MOVE(opponentLeft, MOVE_PSYCHIC);
        }
        TURN {
            MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentRight);
            MOVE(playerRight, MOVE_METEOR_MASH, target: opponentRight);
        }
    } THEN {
        // Which reserve takes the slot is the board's call; both are priced.
        EXPECT_NE(opponentLeft->species, SPECIES_VICTINI);
    }
}

AI_DOUBLE_BATTLE_TEST("EC failed moves: a fresh foe Choice item does not bring Tailwind back while it blows")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        // An unlocked Choice holder on the other side used to wrap every
        // board's score through unsigned arithmetic. A small board keeps all
        // of its pairs, so the -10000 veto on a second Tailwind became the
        // best one on it.
        PLAYER(SPECIES_MARSHTOMP) { Level(32); Speed(60); Moves(MOVE_MUDDY_WATER, MOVE_ROCK_SLIDE, MOVE_PROTECT, MOVE_TAKE_DOWN); }
        PLAYER(SPECIES_MAWILE) { Level(32); Speed(50); Moves(MOVE_IRON_HEAD, MOVE_PLAY_ROUGH, MOVE_SWORDS_DANCE, MOVE_PROTECT); }
        PLAYER(SPECIES_LUCARIO) { Level(32); Speed(75); Item(ITEM_CHOICE_SPECS); Moves(MOVE_AURA_SPHERE, MOVE_FLASH_CANNON, MOVE_DARK_PULSE, MOVE_PROTECT); }
        OPPONENT(SPECIES_SWABLU) {
            Level(35); Item(ITEM_EVIOLITE); Ability(ABILITY_NATURAL_CURE); Nature(NATURE_BOLD); Speed(45);
            Moves(MOVE_HYPER_VOICE, MOVE_ROOST, MOVE_TAILWIND, MOVE_HELPING_HAND);
        }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            SWITCH(playerLeft, 2);
            MOVE(playerRight, MOVE_IRON_HEAD, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_TAILWIND);
        }
        TURN {
            MOVE(playerLeft, MOVE_FLASH_CANNON, target: opponentLeft);
            MOVE(playerRight, MOVE_IRON_HEAD, target: opponentLeft);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_TAILWIND);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC failed moves: Counter and Mirror Coat are not aimed back at bodies they cannot affect")
{
    enum Species attacker;
    enum Move attack, reflect;
    // Counter is Fighting, so a Ghost's physical hit cannot be returned.
    // Mirror Coat is Psychic, so a Dark body's special hit cannot either.
    // With Safeguard already up and a fresh partner beside it that has not
    // moved, Encore on the attacker is the working turn. The Clefairy's
    // Thunder Wave is what makes the Safeguard worth setting.
    PARAMETRIZE { attacker = SPECIES_SABLEYE; attack = MOVE_SHADOW_CLAW; reflect = MOVE_COUNTER; }
    PARAMETRIZE { attacker = SPECIES_HOUNDOOM; attack = MOVE_DARK_PULSE; reflect = MOVE_MIRROR_COAT; }
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(attacker) { HP(300); MaxHP(300); Attack(200); SpAttack(200); Speed(100); Moves(attack); }
        PLAYER(SPECIES_MAGIKARP) { HP(300); MaxHP(300); Speed(90); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_CLEFAIRY) { HP(300); MaxHP(300); Speed(90); Moves(MOVE_CELEBRATE, MOVE_THUNDER_WAVE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(300); MaxHP(300); Speed(50); Moves(MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_ENCORE, MOVE_SAFEGUARD); }
        OPPONENT(SPECIES_MAGIKARP) { Speed(40); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, attack, target: opponentLeft);
            SWITCH(playerRight, 2);
            EXPECT_MOVE(opponentLeft, MOVE_SAFEGUARD);
        }
        TURN {
            MOVE(playerLeft, attack, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            NOT_EXPECT_MOVE(opponentLeft, reflect);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC failed moves: Encore is not spent where it must fail")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        // The player's Whimsicott encores its own partner into Calm Mind. The
        // lock cannot be applied twice, and Whimsicott's own last move is
        // Encore, which cannot be encored while Wobbuffet moves first. An
        // attack is the only turn that does anything.
        PLAYER(SPECIES_CLEFABLE) { Level(30); Speed(45); Moves(MOVE_CALM_MIND, MOVE_MOONBLAST, MOVE_PROTECT, MOVE_SOFT_BOILED); }
        PLAYER(SPECIES_WHIMSICOTT) { Level(30); Speed(40); Ability(ABILITY_INFILTRATOR); Moves(MOVE_ENCORE, MOVE_MOONBLAST, MOVE_PROTECT, MOVE_GIGA_DRAIN); }
        OPPONENT(SPECIES_WOBBUFFET) {
            Level(31); Speed(50); Item(ITEM_SITRUS_BERRY); Ability(ABILITY_SHADOW_TAG);
            Moves(MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_ENCORE, MOVE_PSYCHIC);
        }
        OPPONENT(SPECIES_MUSHARNA) {
            Level(30); Speed(25); Item(ITEM_LEFTOVERS); Ability(ABILITY_SYNCHRONIZE);
            Moves(MOVE_PSYCHIC, MOVE_YAWN, MOVE_PROTECT, MOVE_MOONBLAST);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CALM_MIND);
            MOVE(playerRight, MOVE_ENCORE, target: playerLeft);
        }
        TURN {
            MOVE(playerLeft, MOVE_CALM_MIND);
            MOVE(playerRight, MOVE_PROTECT);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_ENCORE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC failed moves: a guard is not repeated into its own failure beside a fresh foe Choice item")
{
    u16 item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_CHOICE_SPECS; }
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        // The lone-survivor cadence board, with the player bringing in a
        // second attacker on turn one. Holding an unlocked Choice
        // item must not change the answer: with no payoff behind it, a
        // second shield is a one-in-three gamble that buys nothing.
        PLAYER(SPECIES_SHAYMIN) {
            Level(14); HP(45); MaxHP(56); SpAttack(46); SpDefense(37); Speed(25);
            Ability(ABILITY_NATURAL_CURE); Item(ITEM_LIFE_ORB);
            Moves(MOVE_GIGA_DRAIN, MOVE_SEED_FLARE, MOVE_EARTH_POWER, MOVE_PROTECT);
        }
        PLAYER(SPECIES_MIENFOO) {
            Level(14); HP(33); MaxHP(49); Attack(33); Defense(23); SpDefense(35); Speed(18);
            Ability(ABILITY_INNER_FOCUS); Item(ITEM_EVIOLITE);
            Moves(MOVE_FAKE_OUT, MOVE_BRICK_BREAK, MOVE_DRAIN_PUNCH, MOVE_HELPING_HAND);
        }
        PLAYER(SPECIES_SNORLAX) {
            Level(14); HP(120); MaxHP(120); Attack(40); Defense(40); SpDefense(40); Speed(12);
            Ability(ABILITY_THICK_FAT); Item(item);
            Moves(MOVE_BODY_SLAM, MOVE_HEAVY_SLAM, MOVE_PROTECT, MOVE_CRUNCH);
        }
        OPPONENT(SPECIES_NOSEPASS) {
            Level(14); HP(45); MaxHP(45); Defense(47); SpAttack(33); SpDefense(34); Speed(17);
            Ability(ABILITY_MAGNET_PULL); Item(ITEM_SITRUS_BERRY);
            Moves(MOVE_POWER_GEM, MOVE_THUNDERBOLT, MOVE_EARTH_POWER, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            SWITCH(playerLeft, 2);
            MOVE(playerRight, MOVE_FAKE_OUT, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
        TURN {
            MOVE(playerLeft, MOVE_HEAVY_SLAM, target: opponentLeft);
            MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentLeft);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
    }
}

AI_SINGLE_BATTLE_TEST("EC failed moves: Encore scores as a failure on a body already encored")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_SMEARGLE) { Speed(40); Moves(MOVE_SPLASH, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_ENCORE, MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_SAFEGUARD); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_SPLASH); EXPECT_MOVE(opponent, MOVE_ENCORE); }
        TURN { MOVE(player, MOVE_SPLASH); SCORE_LT_VAL(opponent, MOVE_ENCORE, AI_SCORE_DEFAULT); }
    }
}

AI_SINGLE_BATTLE_TEST("EC failed moves: Encore scores as a failure on a move that cannot be encored")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        // Wobbuffet moves first, so the move to lock is Encore itself.
        PLAYER(SPECIES_WHIMSICOTT) { Speed(40); Moves(MOVE_ENCORE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_ENCORE, MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_SAFEGUARD); }
    } WHEN {
        TURN { MOVE(player, MOVE_ENCORE); }
        TURN { MOVE(player, MOVE_CELEBRATE); SCORE_LT_VAL(opponent, MOVE_ENCORE, AI_SCORE_DEFAULT); }
    }
}

AI_DOUBLE_BATTLE_TEST("EC failed moves: a Choice Scarf Imposter does not lock itself into Protect")
{
    GIVEN {
        // Transform copies Protect with the rest of the set. Under the Scarf
        // a status move is a lock with nothing in it: Timmy's Ditto shielded
        // once and then repeated a failing Protect until it fell. A shield
        // that is worth a turn to an unlocked body is not worth that.
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_MACHAMP) { Level(30); Speed(80); Moves(MOVE_CLOSE_COMBAT, MOVE_PROTECT); }
        PLAYER(SPECIES_SNORLAX) { Level(30); Speed(70); Moves(MOVE_PROTECT, MOVE_TACKLE, MOVE_SPLASH); }
        OPPONENT(SPECIES_DITTO) { Level(30); Ability(ABILITY_IMPOSTER); Item(ITEM_CHOICE_SCARF); Speed(40); Moves(MOVE_TRANSFORM); }
        OPPONENT(SPECIES_MAGIKARP) { Level(30); Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CLOSE_COMBAT, target: opponentLeft); MOVE(playerRight, MOVE_SPLASH); }
    } SCENE {
        // Its moves are the copied Snorlax's, not the declared set.
        NONE_OF { MESSAGE("The opposing Ditto used Protect!"); }
    }
}

// Takao's Wobbuffet: Safeguard again while its first Safeguard was still up,
// and Mirror Coat into a side that could only hit physically. Both fail on the
// board the AI can see; Counter into the physical Mawile and Encore can work.
AI_DOUBLE_BATTLE_TEST("EC failed moves: Safeguard is not set again while it is up, nor Mirror Coat aimed at a physical side")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_SABLEYE) { Level(20); Ability(ABILITY_PRANKSTER); Speed(50); Moves(MOVE_FOUL_PLAY, MOVE_WILL_O_WISP, MOVE_PROTECT); }
        PLAYER(SPECIES_MAWILE) { Level(20); Ability(ABILITY_INTIMIDATE); Speed(50); Moves(MOVE_PLAY_ROUGH, MOVE_IRON_HEAD, MOVE_PROTECT); }
        OPPONENT(SPECIES_WOBBUFFET) {
            Level(22); Speed(33); Item(ITEM_SITRUS_BERRY); Ability(ABILITY_SHADOW_TAG);
            Moves(MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_ENCORE, MOVE_SAFEGUARD);
        }
        OPPONENT(SPECIES_POLIWRATH) { Level(22); Speed(60); Moves(MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentRight); MOVE(playerRight, MOVE_PLAY_ROUGH, target: opponentRight); }
        TURN { MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentRight); MOVE(playerRight, MOVE_PLAY_ROUGH, target: opponentRight); }
        TURN { MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentRight); MOVE(playerRight, MOVE_PLAY_ROUGH, target: opponentRight); }
    } THEN {
        // Safeguard lasts five turns: one use at most in three.
        EXPECT_GE(opponentLeft->pp[3], GetMovePP(MOVE_SAFEGUARD) - 1);
        EXPECT_EQ(opponentLeft->pp[1], GetMovePP(MOVE_MIRROR_COAT));
    }
}

// Shayla's Maractus: Leech Seed has nowhere to land once the only non-Grass
// foe is seeded.
AI_DOUBLE_BATTLE_TEST("EC failed moves: Leech Seed is not aimed at a Grass body or a seeded one")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_AMOONGUSS) { Speed(30); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SNORLAX) { Speed(30); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MARACTUS) { Speed(60); Moves(MOVE_LEECH_SEED, MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_MAGIKARP) { Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_GE(opponentLeft->pp[0], GetMovePP(MOVE_LEECH_SEED) - 1);
        EXPECT_EQ((u32)playerLeft->volatiles.leechSeed, 0);
    }
}

// Matt's Grimmsnarl: a Prankster Thunder Wave cannot touch a Dark body, and
// no Thunder Wave touches a Ground one. (The benchmark's own miss was a Mega
// Evolution into Dark on the same turn, which the AI could not see.)
AI_DOUBLE_BATTLE_TEST("EC failed moves: Thunder Wave is not aimed at a body it cannot affect")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_UMBREON) { Speed(30); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GARCHOMP) { Speed(30); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GRIMMSNARL) { Speed(60); Ability(ABILITY_PRANKSTER); Moves(MOVE_THUNDER_WAVE, MOVE_SPIRIT_BREAK, MOVE_LIGHT_SCREEN); }
        OPPONENT(SPECIES_MAGIKARP) { Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); NOT_EXPECT_MOVE(opponentLeft, MOVE_THUNDER_WAVE); }
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); NOT_EXPECT_MOVE(opponentLeft, MOVE_THUNDER_WAVE); }
    }
}

// Leaf's Ninetales: Encore cannot touch Good as Gold. (The benchmark's own miss
// was a switch-in the AI could not see; this is the board it can.)
AI_DOUBLE_BATTLE_TEST("EC failed moves: Encore is not aimed at a Good as Gold body")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_GHOLDENGO) { Speed(30); Ability(ABILITY_GOOD_AS_GOLD); Moves(MOVE_NASTY_PLOT); }
        PLAYER(SPECIES_GHOLDENGO) { Speed(30); Ability(ABILITY_GOOD_AS_GOLD); Moves(MOVE_NASTY_PLOT); }
        OPPONENT(SPECIES_NINETALES) { Speed(60); Moves(MOVE_ENCORE, MOVE_EMBER); }
        OPPONENT(SPECIES_MAGIKARP) { Speed(10); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_NASTY_PLOT); MOVE(playerRight, MOVE_NASTY_PLOT); }
        TURN { MOVE(playerLeft, MOVE_NASTY_PLOT); MOVE(playerRight, MOVE_NASTY_PLOT); NOT_EXPECT_MOVE(opponentLeft, MOVE_ENCORE); }
    }
}

// Shannon's Ribombee: Tailwind while its own is still blowing. (The benchmark's
// own miss was forced by the player's Encore; the choice itself was Moonblast.)
AI_DOUBLE_BATTLE_TEST("EC failed moves: a partner does not copy a side condition the other is setting")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        // Two Safeguards, two Tailwinds or two Reflects in one turn: the
        // second meets the condition the first has just set.
        PLAYER(SPECIES_WOBBUFFET) { Speed(30); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(30); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_RIBOMBEE) { Speed(90); Moves(MOVE_TAILWIND, MOVE_SAFEGUARD, MOVE_REFLECT, MOVE_POLLEN_PUFF); }
        OPPONENT(SPECIES_WHIMSICOTT) { Speed(80); Moves(MOVE_TAILWIND, MOVE_SAFEGUARD, MOVE_REFLECT, MOVE_MOONBLAST); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft); MOVE(playerRight, MOVE_TACKLE, target: opponentRight); }
    } SCENE {
        NONE_OF { MESSAGE("But it failed!"); }
    }
}

// Two status moves into the same body: the second meets a status.
AI_DOUBLE_BATTLE_TEST("EC failed moves: partners do not stack two statuses on one body")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_MACHAMP) { Speed(30); Attack(200); Moves(MOVE_CLOSE_COMBAT); }
        PLAYER(SPECIES_BLISSEY) { Speed(20); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SABLEYE) { Speed(60); Moves(MOVE_WILL_O_WISP, MOVE_SHADOW_SNEAK); }
        OPPONENT(SPECIES_JOLTEON) { Speed(90); Moves(MOVE_THUNDER_WAVE, MOVE_THUNDER_SHOCK); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CLOSE_COMBAT, target: opponentRight); MOVE(playerRight, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF { MESSAGE("Machamp is already paralyzed!"); MESSAGE("Machamp is already burned!"); MESSAGE("But it failed!"); }
    }
}

#if TESTING
extern bool8 gTestPairBudgetSpent;
#endif

// A shared multi clock can spend the whole budget before the first board. The
// search then abandoned that board and handed the engine its placeholder:
// slot zero aimed at the user - in the benchmarks a Headlong Rush into the
// AI's own Skarmory. The Ghost leaves Tackle one real target.
AI_DOUBLE_BATTLE_TEST("EC failed moves: a spent decision budget still yields a scored action")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { HP(300); MaxHP(300); Speed(10); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GENGAR) { HP(300); MaxHP(300); Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SNORLAX) { Speed(50); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_SKARMORY) { HP(300); MaxHP(300); Speed(40); Moves(MOVE_PECK); }
        gTestPairBudgetSpent = TRUE;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_TACKLE, target: playerLeft);
        }
    } THEN {
        gTestPairBudgetSpent = FALSE;
        EXPECT_EQ(opponentRight->hp, 300);
    }
}

AI_DOUBLE_BATTLE_TEST("EC failed moves: a spread attack every foe is immune to scores as useless unless the partner absorbs it")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_VOLT_ABSORB; }
    PARAMETRIZE { ability = ABILITY_ILLUMINATE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_GASTRODON_WEST) { Speed(40); Moves(MOVE_RECOVER); }
        PLAYER(SPECIES_DIGGERSBY) { Speed(60); Moves(MOVE_SWORDS_DANCE); }
        OPPONENT(SPECIES_ELECTRODE) { Speed(100); Moves(MOVE_DISCHARGE, MOVE_SPLASH); }
        OPPONENT(SPECIES_LANTURN) { HP(30); MaxHP(150); Ability(ability); Speed(50); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_RECOVER);
            MOVE(playerRight, MOVE_SWORDS_DANCE);
            if (ability == ABILITY_VOLT_ABSORB)
                SCORE_GT_VAL(opponentLeft, MOVE_DISCHARGE, 0, target: playerLeft);
            else
                SCORE_EQ_VAL(opponentLeft, MOVE_DISCHARGE, 0, target: playerLeft);
        }
    }
}

AI_SINGLE_BATTLE_TEST("EC failed moves: an Attack or Sp. Atk drop scores as useless on a body that cannot use it")
{
    enum Species species;
    enum Move drop, move;
    PARAMETRIZE { species = SPECIES_DIGGERSBY; move = MOVE_BODY_SLAM; drop = MOVE_EERIE_IMPULSE; }
    PARAMETRIZE { species = SPECIES_ALAKAZAM; move = MOVE_PSYCHIC; drop = MOVE_CHARM; }
    // Body Press runs on Defense, Foul Play on its target's Attack.
    PARAMETRIZE { species = SPECIES_CORVIKNIGHT; move = MOVE_BODY_PRESS; drop = MOVE_CHARM; }
    PARAMETRIZE { species = SPECIES_SABLEYE; move = MOVE_FOUL_PLAY; drop = MOVE_CHARM; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(species) { Speed(40); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(drop, MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, move); SCORE_EQ_VAL(opponent, drop, 0); }
    }
}

AI_SINGLE_BATTLE_TEST("EC failed moves: an Attack or Sp. Atk drop keeps its value on a body that uses it")
{
    enum Species species;
    enum Move drop, move;
    PARAMETRIZE { species = SPECIES_DIGGERSBY; move = MOVE_BODY_SLAM; drop = MOVE_CHARM; }
    PARAMETRIZE { species = SPECIES_ALAKAZAM; move = MOVE_PSYCHIC; drop = MOVE_EERIE_IMPULSE; }
    // Photon Geyser follows whichever attacking stat is higher.
    PARAMETRIZE { species = SPECIES_NECROZMA; move = MOVE_PHOTON_GEYSER; drop = MOVE_CHARM; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(species) { Speed(40); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(drop, MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, move); SCORE_GT_VAL(opponent, drop, 0); }
    }
}

AI_DOUBLE_BATTLE_TEST("EC failed moves: a side does not use Trick Room under its own room")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        // The room pays against the two fast leads. Two slow bodies replace
        // them, and now both of the AI's bodies outspeed the other side, but
        // another Trick Room sets nothing: it ends the room this side set.
        // Tate & Liza's Cresselia twisted the dimensions back the turn after
        // twisting them.
        PLAYER(SPECIES_WEAVILE) { Speed(150); Moves(MOVE_KNOCK_OFF, MOVE_PROTECT); }
        PLAYER(SPECIES_JOLTEON) { Speed(140); Moves(MOVE_THUNDERBOLT, MOVE_PROTECT); }
        PLAYER(SPECIES_HOUNDOOM) { HP(900); MaxHP(900); Speed(10); Moves(MOVE_CRUNCH, MOVE_PROTECT); }
        PLAYER(SPECIES_UMBREON) { HP(900); MaxHP(900); Speed(10); Moves(MOVE_FOUL_PLAY, MOVE_PROTECT); }
        OPPONENT(SPECIES_CRESSELIA) { Speed(60); Moves(MOVE_PSYCHIC, MOVE_TRICK_ROOM, MOVE_ICY_WIND, MOVE_PROTECT); }
        OPPONENT(SPECIES_REUNICLUS) { Speed(40); Moves(MOVE_PSYCHIC, MOVE_FOCUS_BLAST, MOVE_PROTECT, MOVE_SHADOW_BALL); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM);
        }
        TURN { SWITCH(playerLeft, 2); SWITCH(playerRight, 3); }
        TURN {
            MOVE(playerLeft, MOVE_CRUNCH, target: opponentLeft);
            MOVE(playerRight, MOVE_FOUL_PLAY, target: opponentRight);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC failed moves: Taunt is not aimed at a body whose only status move is Protect")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        // Wattson's Electrode, all of whose Electric attacks the Lightning
        // Rod Marowak took, taunted that Marowak - whose only status move is
        // Protect - because the body beside it held Trick Room. Here that body
        // is Oblivious, so the room cannot be taken either; taking Protect
        // alone is not worth the turn.
        PLAYER(SPECIES_MAROWAK) { Speed(40); Ability(ABILITY_LIGHTNING_ROD); Moves(MOVE_BONEMERANG, MOVE_ROCK_SLIDE, MOVE_PROTECT); }
        PLAYER(SPECIES_SLOWBRO) { Speed(20); Ability(ABILITY_OBLIVIOUS); Moves(MOVE_TRICK_ROOM, MOVE_LIGHT_SCREEN, MOVE_SCALD, MOVE_PROTECT); }
        OPPONENT(SPECIES_JOLTEON) { Speed(130); Moves(MOVE_THUNDERBOLT, MOVE_VOLT_SWITCH, MOVE_TAUNT, MOVE_PROTECT); }
        OPPONENT(SPECIES_GALVANTULA) { Speed(108); Moves(MOVE_BUG_BUZZ, MOVE_ENERGY_BALL, MOVE_PROTECT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ROCK_SLIDE);
            MOVE(playerRight, MOVE_SCALD, target: opponentLeft);
        }
        TURN {
            MOVE(playerLeft, MOVE_ROCK_SLIDE);
            MOVE(playerRight, MOVE_SCALD, target: opponentLeft);
        }
    } THEN {
        EXPECT_EQ((u32)playerLeft->volatiles.tauntTimer, 0);
    }
}

AI_SINGLE_BATTLE_TEST("EC failed moves: a drop is not judged useless on a moveset the AI has not seen")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_DIGGERSBY) { Speed(40); Moves(MOVE_BODY_SLAM, MOVE_EARTH_POWER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_EERIE_IMPULSE, MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_BODY_SLAM); SCORE_GT_VAL(opponent, MOVE_EERIE_IMPULSE, 0); }
        TURN { MOVE(player, MOVE_BODY_SLAM); SCORE_GT_VAL(opponent, MOVE_EERIE_IMPULSE, 0); }
    }
}

// vj-a/win-1 turn 4: Winona's Altaria Hazed a field whose only stage change
// was its own Intimidated Attack, on a body with no attack to spend it on.
AI_SINGLE_BATTLE_TEST("EC failed moves: Haze scores as a failure on a field with nothing to reset")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_INCINEROAR) { Ability(ABILITY_INTIMIDATE); Speed(40); Moves(MOVE_KNOCK_OFF, MOVE_SWORDS_DANCE); }
        OPPONENT(SPECIES_ALTARIA) { Ability(ABILITY_NATURAL_CURE); Speed(50); Moves(MOVE_HAZE, MOVE_ROOST, MOVE_TAILWIND); }
    } WHEN {
        TURN { MOVE(player, MOVE_KNOCK_OFF); SCORE_EQ_VAL(opponent, MOVE_HAZE, 0); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

AI_SINGLE_BATTLE_TEST("EC failed moves: Haze keeps its value against a foe's boost")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_INCINEROAR) { Ability(ABILITY_INTIMIDATE); Speed(40); Moves(MOVE_KNOCK_OFF, MOVE_SWORDS_DANCE); }
        OPPONENT(SPECIES_ALTARIA) { Ability(ABILITY_NATURAL_CURE); Speed(50); Moves(MOVE_HAZE, MOVE_ROOST, MOVE_TAILWIND); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); }
        TURN { MOVE(player, MOVE_KNOCK_OFF); SCORE_GT_VAL(opponent, MOVE_HAZE, 0); }
    }
}

// vj-b/wdl-2 turn 9: Wallace's Zamazenta at 3 HP and burned raised Defense,
// and the burn finished it at the end of the same turn. A boost its user
// will not live to spend is certain to buy nothing.
AI_SINGLE_BATTLE_TEST("EC failed moves: a self-boost scores as a failure on a body its burn finishes this turn")
{
    u32 hp;
    PARAMETRIZE { hp = 3; }
    PARAMETRIZE { hp = 300; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_AMOONGUSS) { Speed(30); Moves(MOVE_POLLEN_PUFF, MOVE_PROTECT); }
        OPPONENT(SPECIES_ZAMAZENTA_CROWNED) { Speed(50); HP(hp); Status1(STATUS1_BURN); Moves(MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_HEAVY_SLAM, MOVE_PROTECT); }
    } WHEN {
        if (hp == 3)
            TURN { MOVE(player, MOVE_PROTECT); SCORE_EQ_VAL(opponent, MOVE_IRON_DEFENSE, 0); }
        else
            TURN { MOVE(player, MOVE_PROTECT); SCORE_GT_VAL(opponent, MOVE_IRON_DEFENSE, 0); }
    }
}

// E0001 a05 turn 4 as it stood: Lucario seeded the turn before, and Electric
// Terrain keeping Spore off both grounded foes.
static void SeededLucarioBoard(void)
{
    gBattleMons[B_BATTLER_0].volatiles.leechSeed = LEECHSEEDED_BY(B_BATTLER_3);
    gFieldTimers.terrain = B_TERRAIN_ELECTRIC;
    gFieldTimers.terrainTimer = 3;
}

// Brendan's Shroomish aimed Leech Seed at a Naganadel that its faster Taillow
// was knocking out. The Seed then fell, as native targeting has it, on the
// Lucario beside it, which it had seeded the turn before: a sure failure the
// pair chose together.
AI_DOUBLE_BATTLE_TEST("EC failed moves: Leech Seed is not aimed where the partner's knockout hands it to a seeded foe")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_LUCARIO) { Level(14); Speed(40); Moves(MOVE_EXTREME_SPEED, MOVE_CLOSE_COMBAT, MOVE_ROCK_SLIDE, MOVE_ICE_PUNCH); }
        PLAYER(SPECIES_NAGANADEL) { Level(14); HP(1); Speed(45); Moves(MOVE_SLUDGE_BOMB, MOVE_HEAT_WAVE, MOVE_DRAGON_PULSE, MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_TAILLOW) {
            Level(20); Speed(60); Item(ITEM_TOXIC_ORB); Ability(ABILITY_GUTS); Nature(NATURE_JOLLY);
            Moves(MOVE_FACADE, MOVE_BRAVE_BIRD, MOVE_QUICK_ATTACK, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_SHROOMISH) {
            Level(21); Speed(20); Item(ITEM_EVIOLITE); Ability(ABILITY_EFFECT_SPORE); Nature(NATURE_BOLD);
            Moves(MOVE_SPORE, MOVE_LEECH_SEED, MOVE_GIGA_DRAIN, MOVE_PROTECT);
        }
        gTestAiTurnSetupHook = SeededLucarioBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_EXTREME_SPEED, target: opponentLeft);
            MOVE(playerRight, MOVE_SLUDGE_BOMB, target: opponentRight);
            NOT_EXPECT_MOVE(opponentRight, MOVE_LEECH_SEED);
        }
    }
}

// E0005: Tiana's Skitty kept Faking Tears after Swirlix, her only special
// attacker, had fainted. Eevee and Buneary hit Defense, so a Sp. Def drop
// buys nothing; a Defense drop still does.
AI_DOUBLE_BATTLE_TEST("EC failed moves: a Sp. Def drop is not spent when nothing left on the side attacks Sp. Def")
{
    u32 swirlixHp;
    PARAMETRIZE { swirlixHp = 0; }
    PARAMETRIZE { swirlixHp = 40; }
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_UMBREON) { Level(20); Speed(30); Moves(MOVE_FOUL_PLAY, MOVE_WISH, MOVE_PROTECT, MOVE_YAWN); }
        PLAYER(SPECIES_SYLVEON) { Level(20); Speed(35); Moves(MOVE_HYPER_VOICE, MOVE_PROTECT, MOVE_CALM_MIND, MOVE_WISH); }
        OPPONENT(SPECIES_SKITTY) {
            Level(20); Speed(60); Item(ITEM_FOCUS_SASH); Ability(ABILITY_WONDER_SKIN); Nature(NATURE_JOLLY);
            Moves(MOVE_FAKE_OUT, MOVE_FAKE_TEARS, MOVE_HELPING_HAND, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_EEVEE) {
            Level(21); Speed(50); Item(ITEM_EVIOLITE); Ability(ABILITY_ADAPTABILITY); Nature(NATURE_ADAMANT);
            Moves(MOVE_QUICK_ATTACK, MOVE_DOUBLE_EDGE, MOVE_WISH, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_SWIRLIX) {
            Level(20); HP(swirlixHp); Speed(40); Item(ITEM_SITRUS_BERRY); Ability(ABILITY_UNBURDEN); Nature(NATURE_MODEST);
            Moves(MOVE_DRAINING_KISS, MOVE_DAZZLING_GLEAM, MOVE_THUNDERBOLT, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_BUNEARY) {
            Level(20); Speed(55); Item(ITEM_FLAME_ORB); Ability(ABILITY_KLUTZ); Nature(NATURE_JOLLY);
            Moves(MOVE_FAKE_OUT, MOVE_SWITCHEROO, MOVE_ENCORE, MOVE_DRAIN_PUNCH);
        }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
        if (swirlixHp == 0)
            TURN { MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentRight); MOVE(playerRight, MOVE_CALM_MIND); NOT_EXPECT_MOVE(opponentLeft, MOVE_FAKE_TEARS); }
        else
            TURN { MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentRight); MOVE(playerRight, MOVE_CALM_MIND); }
    } THEN {
        EXPECT_EQ((bool32)AI_IsFoeStatDropUseless(B_BATTLER_1, B_BATTLER_2, MOVE_FAKE_TEARS), swirlixHp == 0);
        EXPECT(!AI_IsFoeStatDropUseless(B_BATTLER_1, B_BATTLER_2, MOVE_SCREECH));
    }
}

static void RootedRingedCursedBoard(void)
{
    gBattleMons[B_BATTLER_1].volatiles.root = TRUE;
    gBattleMons[B_BATTLER_1].volatiles.aquaRing = TRUE;
    gBattleMons[B_BATTLER_0].volatiles.cursed = TRUE;
}

// A second Ingrain, Aqua Ring or Ghost Curse on the same body fails in view.
AI_SINGLE_BATTLE_TEST("EC failed moves: Ingrain, Aqua Ring and a Ghost's Curse are not set again")
{
    bool32 set;
    PARAMETRIZE { set = FALSE; }
    PARAMETRIZE { set = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_ALAKAZAM) { Speed(30); Moves(MOVE_CALM_MIND, MOVE_PROTECT); }
        OPPONENT(SPECIES_DUSKNOIR) { Speed(20); Moves(MOVE_INGRAIN, MOVE_AQUA_RING, MOVE_CURSE, MOVE_SHADOW_SNEAK); }
        if (set)
            gTestAiTurnSetupHook = RootedRingedCursedBoard;
    } WHEN {
        if (set)
            TURN {
                MOVE(player, MOVE_CALM_MIND);
                SCORE_EQ_VAL(opponent, MOVE_INGRAIN, 0);
                SCORE_EQ_VAL(opponent, MOVE_AQUA_RING, 0);
                SCORE_EQ_VAL(opponent, MOVE_CURSE, 0);
            }
        else
            TURN {
                MOVE(player, MOVE_CALM_MIND);
                SCORE_GT_VAL(opponent, MOVE_INGRAIN, 0);
                SCORE_GT_VAL(opponent, MOVE_AQUA_RING, 0);
                SCORE_GT_VAL(opponent, MOVE_CURSE, 0);
            }
    }
}

// E0004 a06 turn 2: Paras aimed Spore at the Froslass its Pikachu knocked out
// first, and the Spore fell on Grass-type Shaymin: "It doesn't affect
// Shaymin". The Spore's value only counts where it can land.
AI_DOUBLE_BATTLE_TEST("EC failed moves: Spore is not aimed where the partner's knockout hands it to a Grass foe")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_SHAYMIN_LAND) { Level(14); Speed(40); Moves(MOVE_SEED_FLARE, MOVE_EARTH_POWER, MOVE_TAILWIND, MOVE_PROTECT); }
        PLAYER(SPECIES_FROSLASS) { Level(14); HP(1); Speed(45); Moves(MOVE_SHADOW_BALL, MOVE_WILL_O_WISP, MOVE_ICE_BEAM, MOVE_PROTECT); }
        OPPONENT(SPECIES_PIKACHU) {
            Level(20); Speed(60); Item(ITEM_LIGHT_BALL); Ability(ABILITY_LIGHTNING_ROD); Nature(NATURE_JOLLY);
            Moves(MOVE_FAKE_OUT, MOVE_VOLT_TACKLE, MOVE_ELECTROWEB, MOVE_QUICK_ATTACK);
        }
        OPPONENT(SPECIES_PARAS) {
            Level(20); Speed(20); Item(ITEM_EVIOLITE); Ability(ABILITY_DRY_SKIN); Nature(NATURE_ADAMANT);
            Moves(MOVE_SPORE, MOVE_LEECH_LIFE, MOVE_WIDE_GUARD, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_EARTH_POWER, target: opponentLeft);
            MOVE(playerRight, MOVE_SHADOW_BALL, target: opponentRight);
            NOT_EXPECT_MOVE(opponentRight, MOVE_SPORE);
        }
    }
}

// E0099 a01 turn 2: Parasect aimed Spore at the Arcanine its faster Sawsbuck
// was knocking out, and the Spore fell on Entei in Safety Goggles: "But it
// failed!". Only a High Horsepower miss leaves Arcanine there to sleep, and
// that share is all the sleep is worth.
AI_DOUBLE_BATTLE_TEST("EC failed moves: Spore is not aimed where the partner's knockout hands it to a Safety Goggles foe")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_ENTEI) {
            Level(40); MaxHP(154); HP(154); Speed(122); Item(ITEM_SAFETY_GOGGLES); Ability(ABILITY_INNER_FOCUS); Nature(NATURE_ADAMANT);
            Moves(MOVE_SACRED_FIRE, MOVE_EXTREME_SPEED, MOVE_IRON_TAIL, MOVE_PROTECT);
        }
        PLAYER(SPECIES_ARCANINE_HISUI) {
            Level(40); MaxHP(138); HP(92); Speed(114); Item(ITEM_LIFE_ORB); Ability(ABILITY_INTIMIDATE); Nature(NATURE_ADAMANT);
            Moves(MOVE_ROCK_SLIDE, MOVE_FLARE_BLITZ, MOVE_EXTREME_SPEED, MOVE_PROTECT);
        }
        PLAYER(SPECIES_INCINEROAR) { Level(40); Speed(70); Item(ITEM_SAFETY_GOGGLES); Ability(ABILITY_INTIMIDATE); Moves(MOVE_FAKE_OUT, MOVE_KNOCK_OFF, MOVE_FLARE_BLITZ, MOVE_PARTING_SHOT); }
        PLAYER(SPECIES_CHANDELURE) { Level(40); Speed(117); HP(23); Item(ITEM_CHOICE_SPECS); Ability(ABILITY_FLASH_FIRE); Moves(MOVE_HEAT_WAVE, MOVE_SHADOW_BALL, MOVE_ENERGY_BALL, MOVE_FLAMETHROWER); }
        PLAYER(SPECIES_HOUNDOOM) { Level(40); Speed(130); Item(ITEM_LIFE_ORB); Ability(ABILITY_FLASH_FIRE); Moves(MOVE_DARK_PULSE, MOVE_HEAT_WAVE, MOVE_SLUDGE_BOMB, MOVE_PROTECT); }
        PLAYER(SPECIES_AMOONGUSS) { Level(40); Speed(40); Item(ITEM_LEFTOVERS); Ability(ABILITY_REGENERATOR); Moves(MOVE_RAGE_POWDER, MOVE_SPORE, MOVE_POLLEN_PUFF, MOVE_PROTECT); }
        OPPONENT(SPECIES_PARASECT) {
            Level(45); MaxHP(169); HP(169); Speed(46); Item(ITEM_FOCUS_SASH); Ability(ABILITY_DRY_SKIN); Nature(NATURE_CAREFUL);
            Moves(MOVE_SPORE, MOVE_WIDE_GUARD, MOVE_LEECH_LIFE, MOVE_RAGE_POWDER);
        }
        OPPONENT(SPECIES_SAWSBUCK_SPRING) {
            Level(43); MaxHP(135); HP(135); Speed(139); Item(ITEM_COBA_BERRY); Ability(ABILITY_SAP_SIPPER); Nature(NATURE_JOLLY);
            Moves(MOVE_HORN_LEECH, MOVE_DOUBLE_EDGE, MOVE_PROTECT, MOVE_HIGH_HORSEPOWER);
        }
        OPPONENT(SPECIES_DELPHOX) { Level(42); Speed(145); Item(ITEM_LIFE_ORB); Ability(ABILITY_BLAZE); Moves(MOVE_HEAT_WAVE, MOVE_PSYCHIC, MOVE_DAZZLING_GLEAM, MOVE_PROTECT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SACRED_FIRE, target: opponentLeft);
            MOVE(playerRight, MOVE_FLARE_BLITZ, target: opponentRight);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_SPORE);
            SEND_OUT(playerRight, 2);
        }
    }
}

// E0249 a06 turn 9, in Route 119's rain: Breloom aimed Spore at the Gyarados
// that Blaziken's Thunder Punch knocked out first, and the Spore fell on
// Amoonguss: "It doesn't affect Amoonguss...".
static void BrendanRoute119Board(void)
{
    gBattleMons[B_BATTLER_1].statStages[STAT_ATK] = DEFAULT_STAT_STAGE - 1;
    gBattleWeather = B_WEATHER_RAIN_NORMAL;
    gBattleStruct->weatherDuration = 0;
}

AI_DOUBLE_BATTLE_TEST("EC failed moves: Spore is not aimed where the partner's Thunder Punch hands it to Amoonguss")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_GYARADOS) {
            Level(55); MaxHP(221); HP(161); Attack(213); Defense(108); SpAttack(79); SpDefense(132); Speed(111);
            Nature(NATURE_ADAMANT); Ability(ABILITY_INTIMIDATE); Item(ITEM_LEFTOVERS);
            Moves(MOVE_WATERFALL, MOVE_ICE_FANG, MOVE_THUNDER_WAVE, MOVE_PROTECT);
        }
        PLAYER(SPECIES_AMOONGUSS) {
            Level(55); MaxHP(242); HP(242); Attack(115); Defense(146); SpAttack(115); SpDefense(110); Speed(49);
            Nature(NATURE_RELAXED); Ability(ABILITY_REGENERATOR); Item(ITEM_ROCKY_HELMET);
            Moves(MOVE_RAGE_POWDER, MOVE_SPORE, MOVE_POLLEN_PUFF, MOVE_PROTECT);
        }
        PLAYER(SPECIES_SWAMPERT) { Level(55); Speed(88); Nature(NATURE_ADAMANT); Ability(ABILITY_TORRENT); Item(ITEM_SWAMPERTITE); Moves(MOVE_WATERFALL, MOVE_HIGH_HORSEPOWER, MOVE_ICE_PUNCH, MOVE_PROTECT); }
        PLAYER(SPECIES_TORNADUS_INCARNATE) { Level(55); Speed(195); Nature(NATURE_TIMID); Ability(ABILITY_PRANKSTER); Item(ITEM_SITRUS_BERRY); Moves(MOVE_HURRICANE, MOVE_TAILWIND, MOVE_TAUNT, MOVE_PROTECT); }
        OPPONENT(SPECIES_BRELOOM) {
            Level(63); MaxHP(168); HP(73); Attack(228); Defense(125); SpAttack(90); SpDefense(100); Speed(167);
            Nature(NATURE_JOLLY); Ability(ABILITY_POISON_HEAL); Item(ITEM_TOXIC_ORB); Status1(STATUS1_TOXIC_POISON);
            Moves(MOVE_SPORE, MOVE_MACH_PUNCH, MOVE_SEED_BOMB, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_BLAZIKEN) {
            Level(65); MaxHP(200); HP(200); Attack(201); Defense(104); SpAttack(188); SpDefense(116); Speed(187);
            Nature(NATURE_HASTY); Ability(ABILITY_SPEED_BOOST); Item(ITEM_LIFE_ORB);
            Moves(MOVE_HEAT_WAVE, MOVE_CLOSE_COMBAT, MOVE_THUNDER_PUNCH, MOVE_PROTECT);
        }
        gTestAiTurnSetupHook = BrendanRoute119Board;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ICE_FANG, target: opponentLeft);
            MOVE(playerRight, MOVE_SPORE, target: opponentRight);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_SPORE);
            SEND_OUT(playerLeft, 2);
        }
    }
}

static void TianaEndgameBoard(void)
{
    gBattleStruct->battlerState[B_BATTLER_1].isFirstTurn = 0;
    gBattleStruct->battlerState[B_BATTLER_3].isFirstTurn = 0;
}

// E0005 h16: burned Eevee and Klutz Buneary could not touch a Wonder Guard
// Shedinja. Buneary spent ten turns swapping its Flame Orb and Eevee's
// Eviolite back and forth. The Orb in Shedinja's hands is the only way to win,
// and the swap with Eevee helps nobody.
AI_DOUBLE_BATTLE_TEST("EC failed moves: Klutz Buneary hands its Flame Orb to the Shedinja, not back and forth to Eevee")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_GENGAR) { Level(14); Status1(STATUS1_PARALYSIS); Speed(10); Moves(MOVE_SHADOW_BALL, MOVE_PROTECT); }
        PLAYER(SPECIES_SHEDINJA) { Level(14); Ability(ABILITY_WONDER_GUARD); Speed(20); Moves(MOVE_POLTERGEIST, MOVE_SHADOW_SNEAK, MOVE_WILL_O_WISP, MOVE_PROTECT); }
        OPPONENT(SPECIES_EEVEE) {
            Level(25); Status1(STATUS1_BURN); Item(ITEM_EVIOLITE); Ability(ABILITY_ADAPTABILITY); Speed(40);
            Moves(MOVE_QUICK_ATTACK, MOVE_DOUBLE_EDGE, MOVE_WISH, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_BUNEARY) {
            Level(24); Status1(STATUS1_BURN); Item(ITEM_FLAME_ORB); Ability(ABILITY_KLUTZ); Speed(50);
            Moves(MOVE_FAKE_OUT, MOVE_SWITCHEROO, MOVE_ENCORE, MOVE_DRAIN_PUNCH);
        }
        gTestAiTurnSetupHook = TianaEndgameBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_SHADOW_SNEAK, target: opponentLeft);
            EXPECT_MOVE(opponentRight, MOVE_SWITCHEROO, target: playerRight);
        }
    } THEN {
        EXPECT_EQ(playerRight->item, ITEM_FLAME_ORB);
        // Handing Eevee the Orb costs the pair its Eviolite; handing the
        // Eviolite back is worth one swap, and the swap after that is not.
        EXPECT_LT(AI_AllyItemSwapGain(B_BATTLER_3, B_BATTLER_1, MOVE_SWITCHEROO), 0);
        opponentRight->item = ITEM_EVIOLITE;
        opponentLeft->item = ITEM_FLAME_ORB;
        EXPECT_GT(AI_AllyItemSwapGain(B_BATTLER_3, B_BATTLER_1, MOVE_SWITCHEROO), 0);
        opponentRight->item = ITEM_FLAME_ORB;
        opponentLeft->item = ITEM_EVIOLITE;
        EXPECT_LT(AI_AllyItemSwapGain(B_BATTLER_3, B_BATTLER_1, MOVE_SWITCHEROO), 0);
        // Two empty hands have nothing to exchange.
        opponentRight->item = ITEM_NONE;
        EXPECT(AI_IsMoveCertainToFail(B_BATTLER_3, B_BATTLER_0, MOVE_SWITCHEROO));
    }
}

static void GalvantulaLockBoard(void)
{
    gBattleStruct->choicedMove[B_BATTLER_1] = MOVE_THUNDER;
    gLastMoves[B_BATTLER_1] = MOVE_THUNDER;
}

// E0059 a02: Wattson's Specs Galvantula, locked into Thunder, fired it twice
// into a Lightning Rod Marowak beside a Ground-type Clodsire. Each Thunder
// only raised Marowak's Sp. Atk; the lock does nothing on either target.
AI_DOUBLE_BATTLE_TEST("EC failed moves: a Choice lock into Lightning Rod and a Ground type is left")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_MAROWAK_ALOLA) { Level(40); Speed(40); Ability(ABILITY_LIGHTNING_ROD); Moves(MOVE_FLARE_BLITZ, MOVE_SHADOW_BONE, MOVE_BONEMERANG, MOVE_PROTECT); }
        PLAYER(SPECIES_CLODSIRE) { Level(40); Speed(20); Ability(ABILITY_WATER_ABSORB); Moves(MOVE_EARTHQUAKE, MOVE_POISON_JAB, MOVE_RECOVER, MOVE_PROTECT); }
        OPPONENT(SPECIES_GALVANTULA) {
            Level(40); Speed(100); Item(ITEM_CHOICE_SPECS); Ability(ABILITY_COMPOUND_EYES); Nature(NATURE_TIMID);
            Moves(MOVE_THUNDER, MOVE_BUG_BUZZ, MOVE_ENERGY_BALL, MOVE_STICKY_WEB);
        }
        OPPONENT(SPECIES_MANECTRIC) { Level(40); Speed(90); Moves(MOVE_OVERHEAT, MOVE_SNARL, MOVE_PROTECT, MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_VIKAVOLT) { Level(40); Speed(30); Moves(MOVE_BUG_BUZZ, MOVE_ENERGY_BALL, MOVE_THUNDERBOLT, MOVE_PROTECT); }
        gTestAiTurnSetupHook = GalvantulaLockBoard;
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        EXPECT_NE(opponentLeft->species, SPECIES_GALVANTULA);
    }
}

static void CorphishLockBoard(void)
{
    gBattleStruct->choicedMove[B_BATTLER_1] = MOVE_AQUA_JET;
    gLastMoves[B_BATTLER_1] = MOVE_AQUA_JET;
}

// E0134 a07: Elliot's Band Corphish, locked into Aqua Jet, kept firing it into
// Tsareena's Queenly Majesty, which stops priority against its whole side.
AI_DOUBLE_BATTLE_TEST("EC failed moves: a Choice lock into a priority block is left")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_TSAREENA) { Level(40); Speed(60); Ability(ABILITY_QUEENLY_MAJESTY); Moves(MOVE_TROP_KICK, MOVE_HIGH_JUMP_KICK, MOVE_RAPID_SPIN, MOVE_PROTECT); }
        PLAYER(SPECIES_ARCANINE) { Level(40); Speed(70); Ability(ABILITY_INTIMIDATE); Moves(MOVE_FLARE_BLITZ, MOVE_EXTREME_SPEED, MOVE_WILD_CHARGE, MOVE_PROTECT); }
        OPPONENT(SPECIES_CORPHISH) {
            Level(40); Speed(40); Item(ITEM_CHOICE_BAND); Ability(ABILITY_ADAPTABILITY); Nature(NATURE_ADAMANT);
            Moves(MOVE_AQUA_JET, MOVE_CRABHAMMER, MOVE_KNOCK_OFF, MOVE_ROCK_SLIDE);
        }
        OPPONENT(SPECIES_PELIPPER) { Level(40); Speed(50); Ability(ABILITY_DRIZZLE); Moves(MOVE_HURRICANE, MOVE_SCALD, MOVE_TAILWIND, MOVE_PROTECT); }
        OPPONENT(SPECIES_WHISCASH) { Level(40); Speed(30); Moves(MOVE_EARTHQUAKE, MOVE_WATERFALL, MOVE_PROTECT, MOVE_DRAGON_DANCE); }
        gTestAiTurnSetupHook = CorphishLockBoard;
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        EXPECT_NE(opponentLeft->species, SPECIES_CORPHISH);
    }
}

// E0040: Takao's Wobbuffet Encored fresh switch-ins that had not moved yet and
// would only have moved first by raising Protect, which blocks the Encore.
AI_SINGLE_BATTLE_TEST("EC failed moves: Encore scores as a failure on a fresh switch-in whose only faster move is Protect")
{
    bool32 hasProtect;
    PARAMETRIZE { hasProtect = TRUE; }
    PARAMETRIZE { hasProtect = FALSE; }
    GIVEN {
        AI_FLAGS(FAIL_FLAGS & ~AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_SYLVEON) { Speed(30); Moves(MOVE_HYPER_VOICE, MOVE_PROTECT); }
        PLAYER(SPECIES_QUAGSIRE) {
            Speed(10);
            if (hasProtect)
                Moves(MOVE_HIGH_HORSEPOWER, MOVE_LIQUIDATION, MOVE_RECOVER, MOVE_PROTECT);
            else
                Moves(MOVE_HIGH_HORSEPOWER, MOVE_LIQUIDATION, MOVE_RECOVER, MOVE_AQUA_JET);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(20); Moves(MOVE_ENCORE, MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_SAFEGUARD); }
    } WHEN {
        TURN { SWITCH(player, 1); }
        if (hasProtect)
            TURN { MOVE(player, MOVE_HIGH_HORSEPOWER); SCORE_EQ_VAL(opponent, MOVE_ENCORE, 0); }
        else
            TURN { MOVE(player, MOVE_HIGH_HORSEPOWER); SCORE_GT_VAL(opponent, MOVE_ENCORE, 0); }
    }
}

// E0077 a03: Eddie's Carnivine used Rage Powder when every foe was a Grass
// type, which the powder cannot draw. Follow Me still draws them; with no
// partner on the field, neither has anyone to draw attacks away from.
AI_DOUBLE_BATTLE_TEST("EC failed moves: redirection is useless when no foe is drawn or no partner is left to cover")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_IRON_LEAVES) { Level(40); Speed(90); Moves(MOVE_LEAF_BLADE, MOVE_PSYBLADE, MOVE_CLOSE_COMBAT, MOVE_PROTECT); }
        PLAYER(SPECIES_VENUSAUR) { Level(40); Speed(50); Moves(MOVE_GIGA_DRAIN, MOVE_SLUDGE_BOMB, MOVE_SLEEP_POWDER, MOVE_PROTECT); }
        OPPONENT(SPECIES_CARNIVINE) { Level(40); Speed(30); Moves(MOVE_RAGE_POWDER, MOVE_FOLLOW_ME, MOVE_KNOCK_OFF, MOVE_POWER_WHIP); }
        OPPONENT(SPECIES_GLIGAR) { Level(40); Speed(60); Item(ITEM_EVIOLITE); Moves(MOVE_ACROBATICS, MOVE_EARTHQUAKE, MOVE_PROTECT, MOVE_KNOCK_OFF); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_RAGE_POWDER);
        }
    } THEN {
        EXPECT(AI_IsMoveCertainToFail(B_BATTLER_1, B_BATTLER_0, MOVE_RAGE_POWDER));
        EXPECT(!AI_IsMoveCertainToFail(B_BATTLER_1, B_BATTLER_0, MOVE_FOLLOW_ME));
        opponentRight->hp = 0;
        EXPECT(AI_IsMoveCertainToFail(B_BATTLER_1, B_BATTLER_0, MOVE_FOLLOW_ME));
    }
}

// E0073: Jaclyn's Wobbuffet set Safeguard, and set it again, against a party
// with no way to inflict a status. Hard knows every foe loadout; on the other
// modes an unseen move may still bring one.
AI_SINGLE_BATTLE_TEST("EC failed moves: Safeguard is useless against a known party with no status to give")
{
    u64 information;
    enum Move reserveMove;
    PARAMETRIZE { information = AI_FLAG_OMNISCIENT; reserveMove = MOVE_CLOSE_COMBAT; }
    PARAMETRIZE { information = AI_FLAG_OMNISCIENT; reserveMove = MOVE_SCALD; }
    PARAMETRIZE { information = 0; reserveMove = MOVE_CLOSE_COMBAT; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | information);
        PLAYER(SPECIES_LUCARIO) { Speed(40); Moves(MOVE_METEOR_MASH, MOVE_SWORDS_DANCE); }
        PLAYER(SPECIES_POLIWRATH) { Speed(30); Moves(reserveMove, MOVE_WATERFALL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(20); Moves(MOVE_SAFEGUARD, MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_ENCORE); }
    } WHEN {
        if (information && reserveMove == MOVE_CLOSE_COMBAT)
            TURN { MOVE(player, MOVE_SWORDS_DANCE); SCORE_EQ_VAL(opponent, MOVE_SAFEGUARD, 0); }
        else
            TURN { MOVE(player, MOVE_SWORDS_DANCE); SCORE_GT_VAL(opponent, MOVE_SAFEGUARD, 0); }
    }
}

static void MagikarpGuyBoard(void)
{
    for (enum BattlerId battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
        gBattleStruct->battlerState[battler].isFirstTurn = 0;
    gFieldTimers.terrain = B_TERRAIN_GRASSY;
    gFieldTimers.terrainTimer = 3;
}

// E0227 a02-a04: Magikarp Guy's Gyarados Dragon Danced in front of Raging
// Bolt, whose Thunderbolt knocks it out through any boost, and was knocked out
// every time. The forecast's Thunderclap fails into a status move, but on the
// board the foe picks Thunderbolt.
AI_DOUBLE_BATTLE_TEST("EC failed moves: a setup move is not spent in front of a known knockout")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_RAGING_BOLT) {
            Level(55); Nature(NATURE_MODEST); Item(ITEM_LIFE_ORB); Ability(ABILITY_PROTOSYNTHESIS);
            Moves(MOVE_THUNDERCLAP, MOVE_THUNDERBOLT, MOVE_DRACO_METEOR, MOVE_PROTECT);
        }
        PLAYER(SPECIES_RILLABOOM) {
            Level(55); Nature(NATURE_ADAMANT); Item(ITEM_MIRACLE_SEED); Ability(ABILITY_GRASSY_SURGE);
            Moves(MOVE_FAKE_OUT, MOVE_WOOD_HAMMER, MOVE_GRASSY_GLIDE, MOVE_KNOCK_OFF);
        }
        OPPONENT(SPECIES_GYARADOS) {
            Level(66); HP(149); Nature(NATURE_ADAMANT); Item(ITEM_MYSTIC_WATER); Ability(ABILITY_INTIMIDATE);
            Moves(MOVE_WATERFALL, MOVE_BOUNCE, MOVE_DRAGON_DANCE, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_WISHIWASHI) {
            Level(65); Nature(NATURE_BOLD); Item(ITEM_LEFTOVERS); Ability(ABILITY_SCHOOLING);
            Moves(MOVE_LIQUIDATION, MOVE_AQUA_RING, MOVE_HELPING_HAND, MOVE_PROTECT);
        }
        gTestAiTurnSetupHook = MagikarpGuyBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_GRASSY_GLIDE, target: opponentRight);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_DRAGON_DANCE);
        }
    }
}

// E0320 turn 0, every attempt: Matt's Pelipper raised Wide Guard against
// Rillaboom and Raging Bolt, neither of which carries a move that hits more
// than one target, and Raging Bolt's Thunderbolt took it out before its
// Tailwind ever came. Hard knows both sets, so the guard has nothing to stop.
AI_DOUBLE_BATTLE_TEST("EC failed moves: Wide Guard is not raised against foes with no spread move")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_RILLABOOM) { Level(55); Nature(NATURE_ADAMANT); Ability(ABILITY_GRASSY_SURGE); Item(ITEM_ASSAULT_VEST); Moves(MOVE_FAKE_OUT, MOVE_GRASSY_GLIDE, MOVE_WOOD_HAMMER, MOVE_KNOCK_OFF); }
        PLAYER(SPECIES_RAGING_BOLT) { Level(55); Nature(NATURE_MODEST); Ability(ABILITY_PROTOSYNTHESIS); Item(ITEM_BOOSTER_ENERGY); Moves(MOVE_THUNDERCLAP, MOVE_THUNDERBOLT, MOVE_DRACO_METEOR, MOVE_PROTECT); }
        PLAYER(SPECIES_ARCHALUDON) { Level(55); Nature(NATURE_MODEST); Ability(ABILITY_STAMINA); Item(ITEM_LIFE_ORB); Moves(MOVE_ELECTRO_SHOT, MOVE_FLASH_CANNON, MOVE_DRACO_METEOR, MOVE_PROTECT); }
        PLAYER(SPECIES_GARDEVOIR) { Level(55); Nature(NATURE_TIMID); Ability(ABILITY_TRACE); Item(ITEM_GARDEVOIRITE); Moves(MOVE_HYPER_VOICE, MOVE_PSYCHIC, MOVE_MOONBLAST, MOVE_PROTECT); }
        OPPONENT(SPECIES_PELIPPER) { Level(58); Nature(NATURE_BOLD); Ability(ABILITY_DRIZZLE); Item(ITEM_DAMP_ROCK); Moves(MOVE_WEATHER_BALL, MOVE_HURRICANE, MOVE_TAILWIND, MOVE_WIDE_GUARD); }
        OPPONENT(SPECIES_ARAQUANID) { Level(59); Nature(NATURE_ADAMANT); Ability(ABILITY_WATER_BUBBLE); Item(ITEM_LIFE_ORB); Moves(MOVE_LIQUIDATION, MOVE_LEECH_LIFE, MOVE_POISON_JAB, MOVE_PROTECT); }
        OPPONENT(SPECIES_OKIDOGI) { Level(59); Nature(NATURE_ADAMANT); Ability(ABILITY_GUARD_DOG); Item(ITEM_CLEAR_AMULET); Moves(MOVE_DRAIN_PUNCH, MOVE_POISON_JAB, MOVE_KNOCK_OFF, MOVE_PROTECT); }
        OPPONENT(SPECIES_LUDICOLO) { Level(58); Nature(NATURE_MODEST); Ability(ABILITY_SWIFT_SWIM); Item(ITEM_FOCUS_SASH); Moves(MOVE_FAKE_OUT, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_GIGA_DRAIN); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft);
        }
    } THEN {
        // Pelipper may switch out to keep its rain for later, or act; it never spends Wide Guard.
        EXPECT_EQ(GetMonData(&GetBattlerParty(B_BATTLER_1)[0], MON_DATA_PP4), GetMovePP(MOVE_WIDE_GUARD));
    }
}

// E0289 (build-G a04) turn 5 as it stood: Winona's own Tailwind with a turn
// left, Kilowattrel already burned beside a Fire-type Hisuian Typhlosion.
static void WinonaAltariaBoard(void)
{
    gSideStatuses[B_SIDE_OPPONENT] |= SIDE_STATUS_TAILWIND;
    gSideTimers[B_SIDE_OPPONENT].tailwindTimer = 1;
}

// Neither foe could take Will-O-Wisp, and Altaria aimed it at its own Mega
// Skarmory instead: "The opposing Skarmory was burned!".
AI_DOUBLE_BATTLE_TEST("EC failed moves: a status no foe can take is not aimed at the partner")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_KILOWATTREL) {
            Level(55); MaxHP(159); HP(150); Nature(NATURE_TIMID); Ability(ABILITY_WIND_POWER); Item(ITEM_FOCUS_SASH);
            Status1(STATUS1_BURN); Moves(MOVE_TAILWIND, MOVE_THUNDERBOLT, MOVE_HURRICANE, MOVE_PROTECT);
        }
        PLAYER(SPECIES_TYPHLOSION_HISUI) {
            Level(55); Nature(NATURE_TIMID); Ability(ABILITY_BLAZE); Item(ITEM_CHOICE_SPECS);
            Moves(MOVE_FLAMETHROWER, MOVE_SHADOW_BALL, MOVE_FOCUS_BLAST, MOVE_OVERHEAT);
        }
        OPPONENT(SPECIES_SKARMORY_MEGA) {
            Level(64); MaxHP(217); HP(85); Nature(NATURE_IMPISH); Ability(ABILITY_STURDY); Item(ITEM_SKARMORITE);
            Moves(MOVE_BRAVE_BIRD, MOVE_BODY_PRESS, MOVE_ROOST, MOVE_IRON_DEFENSE);
        }
        OPPONENT(SPECIES_ALTARIA) {
            Level(66); Nature(NATURE_BOLD); Ability(ABILITY_NATURAL_CURE); Item(ITEM_LEFTOVERS);
            Moves(MOVE_TAILWIND, MOVE_WILL_O_WISP, MOVE_HAZE, MOVE_ROOST);
        }
        gTestAiTurnSetupHook = WinonaAltariaBoard;
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_FLAMETHROWER, target: opponentRight); }
    } THEN {
        EXPECT_EQ(opponentLeft->status1, STATUS1_NONE);
    }
}

// E0391 a01 turns 3-7: Hannah's Rabsca stood beside three fallen members and
// shielded or attacked every turn; Revival Blessing had no value on the
// one-turn board. A first shield against Talonflame's Brave Bird is fair; the
// turn after, the revival is worth more than another chip.
AI_DOUBLE_BATTLE_TEST("EC failed moves: Revival Blessing is worth a fallen member")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_TALONFLAME) { Level(55); MaxHP(168); HP(168); Attack(145); Defense(100); SpAttack(92); SpDefense(97); Speed(214); Nature(NATURE_JOLLY); Ability(ABILITY_GALE_WINGS); Item(ITEM_SHARP_BEAK); Moves(MOVE_BRAVE_BIRD, MOVE_FLARE_BLITZ, MOVE_U_TURN, MOVE_PROTECT); }
        PLAYER(SPECIES_GHOLDENGO) { Level(55); MaxHP(212); HP(212); Attack(79); Defense(126); SpAttack(223); SpDefense(122); Speed(114); Nature(NATURE_MODEST); Ability(ABILITY_GOOD_AS_GOLD); Item(ITEM_LIFE_ORB); Moves(MOVE_MAKE_IT_RAIN, MOVE_SHADOW_BALL, MOVE_FOCUS_BLAST, MOVE_PROTECT); }
        PLAYER(SPECIES_SCIZOR_MEGA) { Level(55); Speed(93); Nature(NATURE_ADAMANT); Ability(ABILITY_TECHNICIAN); Item(ITEM_SCIZORITE); Moves(MOVE_BULLET_PUNCH, MOVE_U_TURN, MOVE_KNOCK_OFF, MOVE_PROTECT); }
        PLAYER(SPECIES_AZUMARILL) { Level(55); Speed(77); Nature(NATURE_ADAMANT); Ability(ABILITY_HUGE_POWER); Item(ITEM_CHOICE_BAND); Moves(MOVE_PLAY_ROUGH, MOVE_AQUA_JET, MOVE_LIQUIDATION, MOVE_SUPERPOWER); }
        PLAYER(SPECIES_INCINEROAR) { HP(0); Speed(88); }
        PLAYER(SPECIES_FLUTTER_MANE) { HP(0); Speed(225); }
        OPPONENT(SPECIES_ESPATHRA) { Level(58); MaxHP(196); HP(196); Attack(82); Defense(92); SpAttack(176); SpDefense(92); Speed(199); Nature(NATURE_TIMID); Ability(ABILITY_SPEED_BOOST); Item(ITEM_FOCUS_SASH); Moves(MOVE_LUMINA_CRASH, MOVE_DAZZLING_GLEAM, MOVE_ROOST, MOVE_PROTECT); }
        OPPONENT(SPECIES_RABSCA) { Level(58); MaxHP(209); HP(209); Attack(72); Defense(122); SpAttack(211); SpDefense(138); Speed(75); Nature(NATURE_MODEST); Ability(ABILITY_SYNCHRONIZE); Item(ITEM_SITRUS_BERRY); Moves(MOVE_REVIVAL_BLESSING, MOVE_BUG_BUZZ, MOVE_PSYCHIC, MOVE_PROTECT); }
        OPPONENT(SPECIES_GALLADE) { Level(59); Speed(178); Nature(NATURE_ADAMANT); Ability(ABILITY_SHARPNESS); Item(ITEM_GALLADITE); Moves(MOVE_PSYCHO_CUT, MOVE_CLOSE_COMBAT, MOVE_LEAF_BLADE, MOVE_PROTECT); }
        OPPONENT(SPECIES_MALAMAR) { HP(0); Speed(107); Ability(ABILITY_CONTRARY); Moves(MOVE_SUPERPOWER, MOVE_PSYCHO_CUT, MOVE_KNOCK_OFF, MOVE_PROTECT); }
        OPPONENT(SPECIES_VELUZA) { HP(0); Speed(140); Moves(MOVE_AQUA_CUTTER, MOVE_PSYCHO_CUT, MOVE_NIGHT_SLASH, MOVE_FINAL_GAMBIT); }
        OPPONENT(SPECIES_BRUXISH) { HP(0); Speed(182); Moves(MOVE_PSYCHIC_FANGS, MOVE_LIQUIDATION, MOVE_CRUNCH, MOVE_AQUA_JET); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_BRAVE_BIRD, target: opponentRight); MOVE(playerRight, MOVE_SHADOW_BALL, target: opponentLeft); }
        TURN {
            MOVE(playerLeft, MOVE_FLARE_BLITZ, target: opponentRight);
            MOVE(playerRight, MOVE_SHADOW_BALL, target: opponentLeft);
            EXPECT_MOVE(opponentRight, MOVE_REVIVAL_BLESSING);
            SEND_OUT(playerLeft, 2);
        }
    }
}


// E0243: Crobat never Quick Guarded a known Grassy Glide. The guard was marked
// down as useless because Grassy Glide's base priority is 0; in Grassy
// Terrain it moves first, which is exactly what Quick Guard stops. Without the
// terrain the known set has nothing that goes first, and the guard is a no-op.
AI_SINGLE_BATTLE_TEST("EC failed moves: Quick Guard reads Grassy Glide's priority in Grassy Terrain")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_GRASSY_SURGE; }
    PARAMETRIZE { ability = ABILITY_OVERGROW; }
    GIVEN {
        ASSUME(GetMovePriority(MOVE_GRASSY_GLIDE) == 0);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_RILLABOOM) { Ability(ability); Moves(MOVE_GRASSY_GLIDE); }
        OPPONENT(SPECIES_CROBAT) { HP(400); MaxHP(400); Moves(MOVE_QUICK_GUARD, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GRASSY_GLIDE); }
        TURN {
            MOVE(player, MOVE_GRASSY_GLIDE);
            if (ability == ABILITY_GRASSY_SURGE)
                SCORE_GT_VAL(opponent, MOVE_QUICK_GUARD, AI_SCORE_DEFAULT - 10);
            else
                SCORE_EQ_VAL(opponent, MOVE_QUICK_GUARD, 0);
        }
    }
}

// The Zeraora's Fake Out was spent on an earlier turn.
static void SpentFakeOutBoard(void)
{
    gBattleStruct->battlerState[B_BATTLER_0].isFirstTurn = 0;
}

// E0289 (rv2 v003 a03) turn 1: Winona's Talonflame, hurt and off Gale Wings,
// raised Quick Guard in front of a Zeraora whose Fake Out was spent and a Mega
// Gengar with no priority move. Hard knows both sets: the guard had nothing to
// stop. A live Fake Out or a Weavile's Ice Shard keeps it worth pricing.
AI_DOUBLE_BATTLE_TEST("EC failed moves: Quick Guard scores as a failure against foes with no usable priority")
{
    bool32 spent;
    enum Species species;
    enum Move first;
    PARAMETRIZE { spent = TRUE; species = SPECIES_GENGAR_MEGA; first = MOVE_SLUDGE_BOMB; }
    PARAMETRIZE { spent = FALSE; species = SPECIES_GENGAR_MEGA; first = MOVE_SLUDGE_BOMB; }
    PARAMETRIZE { spent = TRUE; species = SPECIES_WEAVILE; first = MOVE_ICE_SHARD; }
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ZERAORA) { Level(55); Nature(NATURE_JOLLY); Ability(ABILITY_VOLT_ABSORB); Item(ITEM_FOCUS_SASH); Moves(MOVE_FAKE_OUT, MOVE_PLASMA_FISTS, MOVE_KNOCK_OFF, MOVE_PROTECT); }
        PLAYER(species) { Level(55); Nature(NATURE_TIMID); Moves(first, MOVE_SHADOW_BALL, MOVE_THUNDERBOLT, MOVE_PROTECT); }
        OPPONENT(SPECIES_TALONFLAME) { Level(60); MaxHP(182); HP(145); Nature(NATURE_JOLLY); Ability(ABILITY_GALE_WINGS); Item(ITEM_COVERT_CLOAK); Moves(MOVE_TAILWIND, MOVE_BRAVE_BIRD, MOVE_FLARE_BLITZ, MOVE_QUICK_GUARD); }
        OPPONENT(SPECIES_CELESTEELA) { Level(59); Nature(NATURE_ADAMANT); Ability(ABILITY_BEAST_BOOST); Item(ITEM_WACAN_BERRY); Moves(MOVE_WIDE_GUARD, MOVE_HEAVY_SLAM, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE); }
        if (spent)
            gTestAiTurnSetupHook = SpentFakeOutBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PLASMA_FISTS, target: opponentLeft);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight);
            if (spent && species == SPECIES_GENGAR_MEGA)
                SCORE_EQ_VAL(opponentLeft, MOVE_QUICK_GUARD, 0, target: playerLeft);
            else
                SCORE_GT_VAL(opponentLeft, MOVE_QUICK_GUARD, 0, target: playerLeft);
        }
    }
}

// E0423 a03 turns 3-4 as they stood: Mantine's own Tailwind with a turn left.
static void SusieMantineBoard(void)
{
    gSideStatuses[B_SIDE_OPPONENT] |= SIDE_STATUS_TAILWIND;
    gSideTimers[B_SIDE_OPPONENT].tailwindTimer = 1;
    for (enum BattlerId battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
        gBattleStruct->battlerState[battler].isFirstTurn = 0;
}

// Susie's Mantine aimed Scald at its Water Absorb partner twice, and the
// player's known Storm Drain Gastrodon drew it both times: "Gastrodon took
// the attack! Gastrodon's Sp. Atk rose!". Any single-target Water move on
// this board ends in the Gastrodon.
AI_DOUBLE_BATTLE_TEST("EC failed moves: a Water move a foe's Storm Drain draws away is not aimed")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_GASTRODON_EAST) {
            Level(55); MaxHP(238); HP(238); Nature(NATURE_MODEST); Ability(ABILITY_STORM_DRAIN); Item(ITEM_SITRUS_BERRY);
            Moves(MOVE_EARTH_POWER, MOVE_ICE_BEAM, MOVE_RECOVER, MOVE_PROTECT);
        }
        PLAYER(SPECIES_KARTANA) {
            Level(55); Nature(NATURE_JOLLY); Ability(ABILITY_BEAST_BOOST); Item(ITEM_CHOICE_SCARF);
            Moves(MOVE_LEAF_BLADE, MOVE_SACRED_SWORD, MOVE_SMART_STRIKE, MOVE_NIGHT_SLASH);
        }
        PLAYER(SPECIES_RILLABOOM) { Level(55); Nature(NATURE_ADAMANT); Ability(ABILITY_GRASSY_SURGE); Item(ITEM_MIRACLE_SEED); Moves(MOVE_FAKE_OUT, MOVE_WOOD_HAMMER, MOVE_GRASSY_GLIDE, MOVE_KNOCK_OFF); }
        PLAYER(SPECIES_GHOLDENGO) { Level(55); Nature(NATURE_MODEST); Ability(ABILITY_GOOD_AS_GOLD); Item(ITEM_LIFE_ORB); Moves(MOVE_MAKE_IT_RAIN, MOVE_THUNDERBOLT, MOVE_SHADOW_BALL, MOVE_PROTECT); }
        OPPONENT(SPECIES_SEISMITOAD) {
            Level(60); MaxHP(252); HP(188); Nature(NATURE_CALM); Ability(ABILITY_WATER_ABSORB); Item(ITEM_SITRUS_BERRY);
            Moves(MOVE_EARTH_POWER, MOVE_SLUDGE_BOMB, MOVE_SCALD, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_MANTINE) {
            Level(60); MaxHP(228); HP(134); Nature(NATURE_CALM); Ability(ABILITY_WATER_ABSORB); Item(ITEM_LEFTOVERS);
            Moves(MOVE_TAILWIND, MOVE_WIDE_GUARD, MOVE_SCALD, MOVE_ROOST);
        }
        OPPONENT(SPECIES_INTELEON) { Level(60); Nature(NATURE_TIMID); Ability(ABILITY_SNIPER); Item(ITEM_SCOPE_LENS); Moves(MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_AIR_SLASH, MOVE_PROTECT); }
        OPPONENT(SPECIES_PRIMARINA) { Level(60); Nature(NATURE_MODEST); Ability(ABILITY_LIQUID_VOICE); Item(ITEM_THROAT_SPRAY); Moves(MOVE_HYPER_VOICE, MOVE_MOONBLAST, MOVE_CALM_MIND, MOVE_PROTECT); }
        gTestAiTurnSetupHook = SusieMantineBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ICE_BEAM, target: opponentRight);
            MOVE(playerRight, MOVE_LEAF_BLADE, target: opponentLeft);
            NOT_EXPECT_MOVE(opponentRight, MOVE_SCALD);
        }
    }
}

// E0462 a01 turn 2 as it stood: Grassy Terrain, both player Pokemon a stage
// slower from Icy Wind, Rillaboom burned by Flame Body, Magmar's Eviolite
// knocked off, Incineroar fresh.
static void ConnieLumineonBoard(void)
{
    gFieldTimers.terrain = B_TERRAIN_GRASSY;
    gFieldTimers.terrainTimer = 3;
    gBattleMons[B_BATTLER_0].statStages[STAT_SPEED] = DEFAULT_STAT_STAGE - 1;
    gBattleMons[B_BATTLER_2].statStages[STAT_SPEED] = DEFAULT_STAT_STAGE - 1;
    gBattleStruct->battlerState[B_BATTLER_0].isFirstTurn = 0;
    gBattleStruct->battlerState[B_BATTLER_1].isFirstTurn = 0;
    gBattleStruct->battlerState[B_BATTLER_3].isFirstTurn = 0;
}

// Lumineon used Surf beside its own Magmar at 53%, super effective on it, and
// knocked it out: the damage calculation took Lumineon's own Storm Drain for
// a redirector that drew the hit away from its partner.
AI_DOUBLE_BATTLE_TEST("EC failed moves: a spread move is not aimed through the partner it knocks out")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_RILLABOOM) {
            Level(70); MaxHP(285); HP(233); Attack(269); Defense(152); SpAttack(99); SpDefense(125); Speed(145);
            Nature(NATURE_ADAMANT); Ability(ABILITY_GRASSY_SURGE); Item(ITEM_ASSAULT_VEST); Status1(STATUS1_BURN);
            Moves(MOVE_FAKE_OUT, MOVE_GRASSY_GLIDE, MOVE_WOOD_HAMMER, MOVE_KNOCK_OFF);
        }
        PLAYER(SPECIES_INCINEROAR) {
            Level(70); MaxHP(278); HP(240); Attack(188); Defense(152); SpAttack(124); SpDefense(215); Speed(110);
            Nature(NATURE_CAREFUL); Ability(ABILITY_INTIMIDATE); Item(ITEM_SITRUS_BERRY);
            Moves(MOVE_FAKE_OUT, MOVE_KNOCK_OFF, MOVE_FLARE_BLITZ, MOVE_PARTING_SHOT);
        }
        PLAYER(SPECIES_KARTANA) { Level(70); Speed(245); Nature(NATURE_JOLLY); Ability(ABILITY_BEAST_BOOST); Item(ITEM_FOCUS_SASH); Moves(MOVE_LEAF_BLADE, MOVE_SACRED_SWORD, MOVE_SMART_STRIKE, MOVE_PROTECT); }
        PLAYER(SPECIES_GHOLDENGO) { Level(70); Speed(188); Nature(NATURE_MODEST); Ability(ABILITY_GOOD_AS_GOLD); Item(ITEM_CHOICE_SPECS); Moves(MOVE_MAKE_IT_RAIN, MOVE_SHADOW_BALL, MOVE_THUNDERBOLT, MOVE_DAZZLING_GLEAM); }
        OPPONENT(SPECIES_MAGMAR) {
            Level(81); MaxHP(272); HP(143); Attack(165); Defense(123); SpAttack(267); SpDefense(167); Speed(180);
            Nature(NATURE_MODEST); Ability(ABILITY_FLAME_BODY); Attack(148);
            Moves(MOVE_FOLLOW_ME, MOVE_HEAT_WAVE, MOVE_WILL_O_WISP, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_LUMINEON) {
            Level(80); MaxHP(275); HP(275); Attack(126); Defense(152); SpAttack(204); SpDefense(167); Speed(247);
            Nature(NATURE_TIMID); Ability(ABILITY_STORM_DRAIN); Item(ITEM_SITRUS_BERRY);
            Moves(MOVE_SURF, MOVE_ICY_WIND, MOVE_TAILWIND, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_GASTRODON_WEST) { Level(80); Speed(92); Nature(NATURE_CALM); Ability(ABILITY_STORM_DRAIN); Item(ITEM_LEFTOVERS); Moves(MOVE_SURF, MOVE_EARTH_POWER, MOVE_ICE_BEAM, MOVE_RECOVER); }
        OPPONENT(SPECIES_SEAKING) { Level(80); Speed(186); Nature(NATURE_ADAMANT); Ability(ABILITY_LIGHTNING_ROD); Item(ITEM_CHOICE_BAND); Moves(MOVE_WATERFALL, MOVE_MEGAHORN, MOVE_DRILL_RUN, MOVE_ICY_WIND); }
        gTestAiTurnSetupHook = ConnieLumineonBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_WOOD_HAMMER, target: opponentRight);
            MOVE(playerRight, MOVE_FAKE_OUT, target: opponentLeft);
            NOT_EXPECT_MOVE(opponentRight, MOVE_SURF);
        }
    }
}

// E0446 a03 turn 4 as it stood: Mega Dragonite at +1 Speed after its first
// Dance and Parting Shot's drops, Druddigon's Haban Berry knocked off, Aaron's
// Tailwind with a turn left.
static void AaronDragoniteBoard(void)
{
    SetActiveGimmick(B_BATTLER_1, GIMMICK_MEGA);
    SetGimmickAsActivated(B_BATTLER_1, GIMMICK_MEGA);
    gBattleMons[B_BATTLER_1].statStages[STAT_SPEED] = DEFAULT_STAT_STAGE + 1;
    gBattleMons[B_BATTLER_1].statStages[STAT_SPATK] = DEFAULT_STAT_STAGE - 1;
    gSideStatuses[B_SIDE_OPPONENT] |= SIDE_STATUS_TAILWIND;
    gSideTimers[B_SIDE_OPPONENT].tailwindTimer = 1;
    gBattleStruct->battlerState[B_BATTLER_1].isFirstTurn = 0;
    gBattleStruct->battlerState[B_BATTLER_3].isFirstTurn = 0;
}

// Mega Dragonite danced again in front of a Flutter Mane its Dragon Claw and
// Extreme Speed cannot touch and a Kingambit that resists both. With no foe to
// spend it on, the boost is worth little.
AI_DOUBLE_BATTLE_TEST("EC failed moves: a boost with no attack to spend it on is not the turn")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_FLUTTER_MANE) { Level(55); MaxHP(143); HP(143); SpAttack(205); SpDefense(170); Speed(225); Nature(NATURE_TIMID); Ability(ABILITY_PROTOSYNTHESIS); Item(ITEM_CHOICE_SPECS); Moves(MOVE_MOONBLAST, MOVE_SHADOW_BALL, MOVE_DAZZLING_GLEAM, MOVE_MYSTICAL_FIRE); }
        PLAYER(SPECIES_KINGAMBIT) { Level(55); MaxHP(226); HP(226); Attack(225); Defense(154); Speed(77); Nature(NATURE_ADAMANT); Ability(ABILITY_SUPREME_OVERLORD); Item(ITEM_BLACK_GLASSES); Moves(MOVE_KOWTOW_CLEAVE, MOVE_SUCKER_PUNCH, MOVE_IRON_HEAD, MOVE_PROTECT); }
        PLAYER(SPECIES_INCINEROAR) { Level(55); Speed(88); Nature(NATURE_ADAMANT); Ability(ABILITY_INTIMIDATE); Item(ITEM_SITRUS_BERRY); Moves(MOVE_FAKE_OUT, MOVE_KNOCK_OFF, MOVE_FLARE_BLITZ, MOVE_PARTING_SHOT); }
        OPPONENT(SPECIES_DRAGONITE_MEGA) { Level(71); MaxHP(232); HP(232); Attack(271); Defense(190); SpAttack(208); SpDefense(204); Speed(213); Nature(NATURE_ADAMANT); Ability(ABILITY_MULTISCALE); Item(ITEM_DRAGONINITE); Moves(MOVE_DRAGON_DANCE, MOVE_DRAGON_CLAW, MOVE_EXTREME_SPEED, MOVE_PROTECT); }
        OPPONENT(SPECIES_DRUDDIGON) { Level(71); MaxHP(285); HP(154); Attack(266); Defense(155); Speed(95); Nature(NATURE_ADAMANT); Ability(ABILITY_SHEER_FORCE); Moves(MOVE_DRAGON_CLAW, MOVE_FIRE_PUNCH, MOVE_IRON_HEAD, MOVE_PROTECT); }
        gTestAiTurnSetupHook = AaronDragoniteBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_MOONBLAST, target: opponentLeft);
            MOVE(playerRight, MOVE_SUCKER_PUNCH, target: opponentRight);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_DRAGON_DANCE);
        }
    }
}

// rv3 v032/E0400 a04, a05, a07: Tate and Liza's Mega Gardevoir Hyper Voiced
// into a Bastiodon's Wide Guard on up to nine turns running. The guard never
// fails on repeat, yet each raise was priced at the same even chance as the
// first. Once it has gone up turn after turn, the single-target moves win.
AI_DOUBLE_BATTLE_TEST("EC failed moves: a spread attacker stops feeding a Wide Guard raised turn after turn")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_CHI_YU) { Level(55); Nature(NATURE_TIMID); Ability(ABILITY_BEADS_OF_RUIN); Item(ITEM_CHOICE_SPECS); Moves(MOVE_DARK_PULSE, MOVE_HEAT_WAVE, MOVE_OVERHEAT, MOVE_PSYCHIC); }
        PLAYER(SPECIES_BASTIODON) { Level(55); MaxHP(204); HP(108); Nature(NATURE_SASSY); Ability(ABILITY_STURDY); Item(ITEM_LEFTOVERS); Moves(MOVE_WIDE_GUARD, MOVE_IRON_HEAD, MOVE_METAL_BURST, MOVE_PROTECT); }
        PLAYER(SPECIES_PRIMARINA) { Level(55); Nature(NATURE_MODEST); Ability(ABILITY_LIQUID_VOICE); Item(ITEM_CHOICE_SPECS); Moves(MOVE_HYPER_VOICE, MOVE_MOONBLAST, MOVE_PSYCHIC, MOVE_ENERGY_BALL); }
        PLAYER(SPECIES_INCINEROAR) { Level(55); Nature(NATURE_SASSY); Ability(ABILITY_INTIMIDATE); Item(ITEM_SITRUS_BERRY); Moves(MOVE_FAKE_OUT, MOVE_KNOCK_OFF, MOVE_FLARE_BLITZ, MOVE_PARTING_SHOT); }
        OPPONENT(SPECIES_GARDEVOIR_MEGA) { Level(67); Nature(NATURE_QUIET); Ability(ABILITY_PIXILATE); Item(ITEM_GARDEVOIRITE); Moves(MOVE_HYPER_VOICE, MOVE_EXPANDING_FORCE, MOVE_MYSTICAL_FIRE, MOVE_PROTECT); }
        OPPONENT(SPECIES_CRESSELIA) { Level(67); Nature(NATURE_SASSY); Ability(ABILITY_LEVITATE); Item(ITEM_SITRUS_BERRY); Moves(MOVE_TRICK_ROOM, MOVE_HELPING_HAND, MOVE_MOONBLAST, MOVE_LUNAR_BLESSING); }
        OPPONENT(SPECIES_CLAYDOL) { Level(66); Nature(NATURE_QUIET); Ability(ABILITY_LEVITATE); Item(ITEM_MENTAL_HERB); Moves(MOVE_TRICK_ROOM, MOVE_EARTH_POWER, MOVE_ICE_BEAM, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_DARK_PULSE, target: opponentRight); MOVE(playerRight, MOVE_WIDE_GUARD); }
        TURN { MOVE(playerLeft, MOVE_DARK_PULSE, target: opponentRight); MOVE(playerRight, MOVE_WIDE_GUARD); }
        TURN {
            MOVE(playerLeft, MOVE_DARK_PULSE, target: opponentRight);
            MOVE(playerRight, MOVE_WIDE_GUARD);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_HYPER_VOICE);
        }
        TURN {
            MOVE(playerLeft, MOVE_DARK_PULSE, target: opponentRight);
            MOVE(playerRight, MOVE_WIDE_GUARD);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_HYPER_VOICE);
        }
    }
}

// rv3 v031/E0399 a02 turns 1-5: Kathleen's Mimikyu aimed Drain Punch at
// Kingambit five turns running while Togekiss used Follow Me every turn, and
// each hit went into the Togekiss for a quarter damage. The redirection was
// priced at the same minority chance however often it had been raised.
AI_DOUBLE_BATTLE_TEST("EC failed moves: an attacker stops feeding a Follow Me raised turn after turn")
{
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(SPECIES_TOGEKISS) { Level(55); Nature(NATURE_CALM); Ability(ABILITY_SERENE_GRACE); Item(ITEM_SITRUS_BERRY); Moves(MOVE_FOLLOW_ME, MOVE_DAZZLING_GLEAM, MOVE_AIR_SLASH, MOVE_PROTECT); }
        PLAYER(SPECIES_KINGAMBIT) { Level(55); Nature(NATURE_ADAMANT); Ability(ABILITY_SUPREME_OVERLORD); Item(ITEM_BLACK_GLASSES); Moves(MOVE_KOWTOW_CLEAVE, MOVE_SUCKER_PUNCH, MOVE_IRON_HEAD, MOVE_PROTECT); }
        PLAYER(SPECIES_SYLVEON) { Level(55); Nature(NATURE_MODEST); Ability(ABILITY_PIXILATE); Item(ITEM_CHOICE_SPECS); Moves(MOVE_HYPER_VOICE, MOVE_MOONBLAST, MOVE_SHADOW_BALL, MOVE_PROTECT); }
        OPPONENT(SPECIES_MIMIKYU) { Level(59); Nature(NATURE_JOLLY); Ability(ABILITY_DISGUISE); Item(ITEM_LIFE_ORB); Moves(MOVE_PLAY_ROUGH, MOVE_DRAIN_PUNCH, MOVE_SHADOW_SNEAK, MOVE_PROTECT); }
        OPPONENT(SPECIES_DUSKNOIR) { Level(59); Nature(NATURE_ADAMANT); Ability(ABILITY_IRON_FIST); Item(ITEM_LEFTOVERS); Moves(MOVE_FIRE_PUNCH, MOVE_SHADOW_PUNCH, MOVE_LEECH_LIFE, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_FOLLOW_ME); MOVE(playerRight, MOVE_PROTECT); }
        TURN { MOVE(playerLeft, MOVE_FOLLOW_ME); MOVE(playerRight, MOVE_PROTECT); }
        TURN {
            MOVE(playerLeft, MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_IRON_HEAD, target: opponentLeft);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_DRAIN_PUNCH);
        }
    }
}
