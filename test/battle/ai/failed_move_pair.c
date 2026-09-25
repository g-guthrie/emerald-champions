#include "global.h"
#include "test/battle.h"

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
            EXPECT_SWITCH(opponentLeft, 2);
        }
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
    // moved, Encore on the attacker is the working turn.
    PARAMETRIZE { attacker = SPECIES_SABLEYE; attack = MOVE_SHADOW_CLAW; reflect = MOVE_COUNTER; }
    PARAMETRIZE { attacker = SPECIES_HOUNDOOM; attack = MOVE_DARK_PULSE; reflect = MOVE_MIRROR_COAT; }
    GIVEN {
        AI_FLAGS(FAIL_FLAGS);
        PLAYER(attacker) { HP(300); MaxHP(300); Attack(200); SpAttack(200); Speed(100); Moves(attack); }
        PLAYER(SPECIES_MAGIKARP) { HP(300); MaxHP(300); Speed(90); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_CLEFAIRY) { HP(300); MaxHP(300); Speed(90); Moves(MOVE_CELEBRATE); }
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
