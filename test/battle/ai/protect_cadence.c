#include "global.h"
#include "test/battle.h"

#define CADENCE_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC Protect cadence: a last entrant takes its Fake Out knockout instead of an empty first shield")
{
    GIVEN {
        AI_FLAGS(CADENCE_FLAGS);
        PLAYER(SPECIES_MARSHTOMP) {
            Level(20); HP(76); MaxHP(76); Attack(45); Defense(39);
            SpAttack(51); SpDefense(39); Speed(27);
            Ability(ABILITY_DAMP); Item(ITEM_EVIOLITE);
            Moves(MOVE_MUDDY_WATER, MOVE_EARTH_POWER, MOVE_ICY_WIND, MOVE_WIDE_GUARD);
        }
        PLAYER(SPECIES_AERODACTYL) {
            Level(20); HP(1); MaxHP(68); Attack(65); Defense(37);
            SpAttack(31); SpDefense(41); Speed(82);
            Ability(ABILITY_UNNERVE);
            Moves(MOVE_ROCK_SLIDE, MOVE_TAILWIND, MOVE_DUAL_WINGBEAT, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CROAGUNK) {
            Level(20); HP(55); MaxHP(55); Attack(52); Defense(27);
            SpAttack(31); SpDefense(27); Speed(43);
            Ability(ABILITY_DRY_SKIN); Item(ITEM_EVIOLITE);
            Moves(MOVE_POISON_JAB, MOVE_DRAIN_PUNCH, MOVE_FAKE_OUT, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_EARTH_POWER, target: opponentLeft);
            MOVE(playerRight, MOVE_DUAL_WINGBEAT, target: opponentRight, hit: TRUE);
            EXPECT_SEND_OUT(opponentLeft, 2);
        }
        TURN {
            MOVE(playerLeft, MOVE_EARTH_POWER, target: opponentLeft);
            MOVE(playerRight, MOVE_DUAL_WINGBEAT, target: opponentLeft, hit: TRUE);
            // The flank it flinches is an expected-value choice it may make
            // either way; spending the turn on an empty shield is not.
            EXPECT_MOVE(opponentLeft, MOVE_FAKE_OUT);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->hp, 0);
    }
}

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
            // Exhausting the last Earthquake is a decisive reason to repeat a
            // one-in-three shield. An incoming Fly that lands either way is
            // not: the threat simply returns, so the repeat is not banked.
            if (airborne)
                NOT_EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
            else
                EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
    } THEN {
        if (!airborne)
            EXPECT_EQ(playerRight->pp[0], 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Protect cadence: Fake Out takes the target with the larger expected denial")
{
    GIVEN {
        AI_FLAGS(CADENCE_FLAGS);
        // One flank is a knockout, the other is chip on a bulky attacker.
        // Nothing about the human's pending commands separates them.
        PLAYER(SPECIES_MACHOP) { HP(500); MaxHP(500); Attack(300); Defense(300); Speed(90); Moves(MOVE_BRICK_BREAK); }
        PLAYER(SPECIES_MIENFOO) { HP(3); MaxHP(500); Attack(300); Defense(5); Speed(80); Moves(MOVE_BRICK_BREAK); }
        OPPONENT(SPECIES_MIENFOO) {
            Level(30); HP(200); MaxHP(200); Attack(120); Speed(100);
            Ability(ABILITY_INNER_FOCUS); Moves(MOVE_FAKE_OUT, MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BRICK_BREAK, target: opponentLeft);
            MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentLeft);
            EXPECT_MOVE(opponentLeft, MOVE_FAKE_OUT, target: playerRight);
        }
    } THEN {
        EXPECT_EQ(playerRight->hp, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Protect cadence: an unknowable double target does not outrank an available knockout")
{
    GIVEN {
        AI_FLAGS(CADENCE_FLAGS);
        // Either human flank could add its Brick Break to the other and end
        // Nosepass this turn. Nothing reveals whether they will, and Power Gem
        // removes half of that threat outright, so the shield is not worth it.
        PLAYER(SPECIES_TAUROS) { Level(30); HP(150); MaxHP(150); Attack(300); Defense(80); SpDefense(40); Speed(40); Moves(MOVE_BRICK_BREAK, MOVE_CELEBRATE); }
        PLAYER(SPECIES_MACHOP) { Level(30); HP(500); MaxHP(500); Attack(300); Defense(80); SpDefense(40); Speed(20); Moves(MOVE_BRICK_BREAK, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_NOSEPASS) {
            Level(30); HP(300); MaxHP(300); Defense(60); SpAttack(300); SpDefense(60); Speed(100);
            Ability(ABILITY_MAGNET_PULL); Moves(MOVE_POWER_GEM, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_MAGIKARP) { HP(1); Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BRICK_BREAK, target: opponentLeft);
            MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentLeft);
            // Which flank it removes is an expected-value choice; spending the
            // turn behind a shield it cannot justify is not.
            NOT_EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
            EXPECT_MOVE(opponentLeft, MOVE_POWER_GEM);
        }
        TURN {
            MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentLeft);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
        TURN { MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentRight); }
    } THEN {
        EXPECT_EQ(opponentLeft->hp, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Protect cadence: a shield that buys the partner's Trick Room is worth the turn")
{
    bool32 payoff;
    PARAMETRIZE { payoff = TRUE; }
    PARAMETRIZE { payoff = FALSE; }
    GIVEN {
        AI_FLAGS(CADENCE_FLAGS);
        PLAYER(SPECIES_TAUROS) { Level(30); HP(400); MaxHP(400); Attack(250); Defense(200); SpDefense(200); Speed(120); Moves(MOVE_STRENGTH); }
        PLAYER(SPECIES_MACHOP) { Level(30); HP(400); MaxHP(400); Attack(250); Defense(200); SpDefense(200); Speed(110); Moves(MOVE_BRICK_BREAK); }
        // The guard's own slot is the obvious double target and cannot win the
        // exchange. Only the partner's turn makes the shield worth its tempo.
        OPPONENT(SPECIES_BRONZOR) {
            Level(30); HP(50); MaxHP(50); Defense(30); SpAttack(20); SpDefense(30); Speed(30);
            Ability(ABILITY_LEVITATE); Moves(MOVE_CONFUSION, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_BRONZONG) {
            Level(30); HP(400); MaxHP(400); Defense(200); SpDefense(200); Speed(10);
            Ability(ABILITY_LEVITATE);
            Moves(payoff ? MOVE_TRICK_ROOM : MOVE_CONFUSION, MOVE_CELEBRATE);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_STRENGTH, target: opponentLeft);
            MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentLeft);
            if (payoff)
                EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
            else
                NOT_EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
    }
}

// Michelle E0484, build-16 turn 4: Machamp at 66/306 repeated Wide Guard into
// two single-target attacks and died, which the receipt called empty and
// consecutive together. Ex post it was; ex ante Chi-Yu had used a lethal Heat
// Wave the turn before and was still alive, which is exactly the condition
// Wide Guard exists for. The question a replay cannot answer - the AI's own
// scoring consumes frames, and the battle burns RNG per frame - is whether the
// choice rests on that live spread threat or on nothing. So: the same board
// twice, once with the spread threat on the record and once with every spread
// move removed from both foes, and the guard has to tell the difference.
AI_DOUBLE_BATTLE_TEST("EC Protect cadence: Wide Guard rests on a live spread threat, not on habit")
{
    bool32 spreadThreat;
    PARAMETRIZE { spreadThreat = TRUE; }
    PARAMETRIZE { spreadThreat = FALSE; }
    GIVEN {
        AI_FLAGS(CADENCE_FLAGS);
        // Both foes hit hard enough to kill either body next turn, so the AI
        // always has a real competing use for the turn: this board fights back.
        PLAYER(SPECIES_CHI_YU) {
            Level(50); HP(260); MaxHP(260); Attack(60); Defense(100);
            SpAttack(190); SpDefense(110); Speed(180); Ability(ABILITY_BEADS_OF_RUIN);
            // Fire Blast is the control because it is the *harsher* move on
            // the guard user: 110 base power onto Machamp alone against Heat
            // Wave's 95 split across the pair. So the arm without a spread
            // threat leaves Machamp lower, not higher, and if the guard were
            // habit rather than an answer to Heat Wave that arm is the one
            // that should reach for it hardest. Neither move drops the user's
            // Special Attack, so turn two is identical in both arms.
            Moves(spreadThreat ? MOVE_HEAT_WAVE : MOVE_FIRE_BLAST, MOVE_FLAMETHROWER);
        }
        PLAYER(SPECIES_KANGASKHAN) {
            Level(50); HP(300); MaxHP(300); Attack(150); Defense(110);
            SpAttack(60); SpDefense(110); Speed(120); Ability(ABILITY_PARENTAL_BOND);
            Moves(MOVE_DOUBLE_EDGE);
        }
        OPPONENT(SPECIES_MACHAMP) {
            Level(50); HP(306); MaxHP(306); Attack(160); Defense(100);
            SpAttack(60); SpDefense(90); Speed(70);
            Ability(ABILITY_NO_GUARD); Item(ITEM_CLEAR_AMULET);
            Moves(MOVE_DYNAMIC_PUNCH, MOVE_WIDE_GUARD, MOVE_STONE_EDGE, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_MEGANIUM) {
            Level(50); HP(349); MaxHP(349); Attack(70); Defense(140);
            SpAttack(90); SpDefense(140); Speed(80);
            Moves(MOVE_GIGA_DRAIN, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            // Turn one puts the foe's move on the record. Wide Guard's plan
            // score keys off a spread move seen from a living foe last turn,
            // so this is the turn that arms it in one arm and not the other.
            MOVE(playerLeft, spreadThreat ? MOVE_HEAT_WAVE : MOVE_FIRE_BLAST, target: opponentLeft);
            MOVE(playerRight, MOVE_DOUBLE_EDGE, target: opponentRight);
        }
        TURN {
            MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentLeft);
            MOVE(playerRight, MOVE_DOUBLE_EDGE, target: opponentLeft);
            if (spreadThreat)
                EXPECT_MOVE(opponentLeft, MOVE_WIDE_GUARD);
            else
                NOT_EXPECT_MOVE(opponentLeft, MOVE_WIDE_GUARD);
        }
    }
}

extern void (*gTestAiTurnSetupHook)(void);

static void ShieldedLastTurn(enum BattlerId battler)
{
    gBattleMons[battler].volatiles.consecutiveMoveUses = 1;
    gLastMoves[battler] = gLastResultingMoves[battler] = gLastLandedMoves[battler] = MOVE_PROTECT;
}

static void JaredShieldedPairBoard(void)
{
    ShieldedLastTurn(B_BATTLER_1);
    ShieldedLastTurn(B_BATTLER_3);
    gBattleMons[B_BATTLER_1].statStages[STAT_SPATK] = DEFAULT_STAT_STAGE + 1;
    gBattleMons[B_BATTLER_1].statStages[STAT_SPDEF] = DEFAULT_STAT_STAGE + 1;
    gBattleMons[B_BATTLER_1].statStages[STAT_SPEED] = DEFAULT_STAT_STAGE + 1;
}

// E0283 a02 turns 9-12: Oricorio and Decidueye shielded four turns running in
// front of a lone Kingambit, three of the shields failing. Weighing Oricorio's
// Revelation Dance left the move type Psychic for every later check, so each
// attack of both - Triple Arrows included, four times effective - was dropped
// as useless into a Dark type, and Protect was the only action left.
AI_DOUBLE_BATTLE_TEST("EC Protect cadence: a Revelation Dance does not turn every later attack Psychic")
{
    GIVEN {
        AI_FLAGS(CADENCE_FLAGS);
        PLAYER(SPECIES_KINGAMBIT) {
            Level(55); MaxHP(226); HP(109); Attack(225); Defense(154); SpAttack(79); SpDefense(115); Speed(77);
            Nature(NATURE_ADAMANT); Ability(ABILITY_DEFIANT); Item(ITEM_LEFTOVERS);
            Moves(MOVE_KOWTOW_CLEAVE, MOVE_SUCKER_PUNCH, MOVE_IRON_HEAD, MOVE_PROTECT);
        }
        PLAYER(SPECIES_GARCHOMP) { HP(0); Speed(1); }
        OPPONENT(SPECIES_ORICORIO_PAU) {
            Level(63); MaxHP(187); HP(129); Attack(100); Defense(112); SpAttack(200); SpDefense(112); Speed(213);
            Nature(NATURE_TIMID); Ability(ABILITY_DANCER); Item(ITEM_LIFE_ORB);
            Moves(MOVE_REVELATION_DANCE, MOVE_AIR_SLASH, MOVE_QUIVER_DANCE, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_DECIDUEYE_HISUI) {
            Level(62); MaxHP(200); HP(82); Attack(222); Defense(123); SpAttack(127); SpDefense(142); Speed(137);
            Nature(NATURE_ADAMANT); Ability(ABILITY_SCRAPPY); Item(ITEM_YACHE_BERRY);
            Moves(MOVE_TRIPLE_ARROWS, MOVE_LEAF_BLADE, MOVE_SWORDS_DANCE, MOVE_PROTECT);
        }
        gTestAiTurnSetupHook = JaredShieldedPairBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SUCKER_PUNCH, target: opponentRight);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
            EXPECT_MOVE(opponentRight, MOVE_TRIPLE_ARROWS, target: playerLeft);
        }
    }
}

static void ColinShieldedOricorioBoard(void)
{
    ShieldedLastTurn(B_BATTLER_3);
}

static void ColinLoneOricorioBoard(void)
{
    ShieldedLastTurn(B_BATTLER_1);
}

// E0253 (a01-a05): Oricorio-Pa'u shielded nearly every turn in front of Mega
// Tyranitar and Incineroar, often into its own failure, and never used Air
// Slash or Quiver Dance. Its Psychic Revelation Dance cannot touch either Dark
// type; the stale Psychic type made Air Slash look just as useless. It now
// takes the knockout on the worn-down Incineroar, beside Braviary and alone.
AI_DOUBLE_BATTLE_TEST("EC Protect cadence: a shield is not repeated over Air Slash beside an immune Revelation Dance")
{
    bool32 alone;
    PARAMETRIZE { alone = FALSE; }
    PARAMETRIZE { alone = TRUE; }
    GIVEN {
        AI_FLAGS(CADENCE_FLAGS);
        PLAYER(SPECIES_TYRANITAR_MEGA) {
            Level(55); MaxHP(226); HP(226); Attack(260); Defense(187); SpAttack(113); SpDefense(154); Speed(100);
            Nature(NATURE_ADAMANT); Ability(ABILITY_SAND_STREAM); Item(ITEM_TYRANITARITE);
            Moves(MOVE_ROCK_SLIDE, MOVE_KNOCK_OFF, MOVE_LOW_KICK, MOVE_PROTECT);
        }
        PLAYER(SPECIES_INCINEROAR) {
            Level(55); MaxHP(221); HP(alone ? 25 : 38); Attack(149); Defense(121); SpAttack(99); SpDefense(170); Speed(88);
            Nature(NATURE_CAREFUL); Ability(ABILITY_INTIMIDATE);
            Moves(MOVE_FAKE_OUT, MOVE_KNOCK_OFF, MOVE_FLARE_BLITZ, MOVE_PARTING_SHOT);
        }
        PLAYER(SPECIES_FLUTTER_MANE) { Level(55); Speed(225); Nature(NATURE_TIMID); Ability(ABILITY_PROTOSYNTHESIS); Item(ITEM_CHOICE_SPECS); Moves(MOVE_MOONBLAST, MOVE_SHADOW_BALL, MOVE_DAZZLING_GLEAM, MOVE_THUNDERBOLT); }
        PLAYER(SPECIES_GHOLDENGO) { Level(55); Speed(114); Nature(NATURE_MODEST); Ability(ABILITY_GOOD_AS_GOLD); Item(ITEM_LIFE_ORB); Moves(MOVE_MAKE_IT_RAIN, MOVE_THUNDERBOLT, MOVE_SHADOW_BALL, MOVE_PROTECT); }
        if (alone)
        {
            OPPONENT(SPECIES_ORICORIO_PAU) {
                Level(67); MaxHP(198); HP(18); Attack(107); Defense(119); SpAttack(212); SpDefense(119); Speed(226);
                Nature(NATURE_TIMID); Ability(ABILITY_DANCER); Item(ITEM_WISE_GLASSES);
                Moves(MOVE_REVELATION_DANCE, MOVE_AIR_SLASH, MOVE_QUIVER_DANCE, MOVE_PROTECT);
            }
            OPPONENT(SPECIES_BRAVIARY_HISUI) { HP(0); Speed(155); }
            gTestAiTurnSetupHook = ColinLoneOricorioBoard;
        }
        else
        {
            OPPONENT(SPECIES_BRAVIARY_HISUI) {
                Level(67); MaxHP(245); HP(245); Attack(122); Defense(119); SpAttack(239); SpDefense(119); Speed(155);
                Nature(NATURE_MODEST); Ability(ABILITY_SHEER_FORCE); Item(ITEM_CHOICE_SPECS);
                Moves(MOVE_HURRICANE, MOVE_ESPER_WING, MOVE_HEAT_WAVE, MOVE_U_TURN);
            }
            OPPONENT(SPECIES_ORICORIO_PAU) {
                Level(67); MaxHP(198); HP(186); Attack(107); Defense(119); SpAttack(212); SpDefense(119); Speed(226);
                Nature(NATURE_TIMID); Ability(ABILITY_DANCER); Item(ITEM_WISE_GLASSES);
                Moves(MOVE_REVELATION_DANCE, MOVE_AIR_SLASH, MOVE_QUIVER_DANCE, MOVE_PROTECT);
            }
            gTestAiTurnSetupHook = ColinShieldedOricorioBoard;
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, alone ? MOVE_KNOCK_OFF : MOVE_ROCK_SLIDE, target: opponentLeft);
            MOVE(playerRight, MOVE_KNOCK_OFF, target: opponentLeft);
            if (alone)
                EXPECT_MOVE(opponentLeft, MOVE_AIR_SLASH, target: playerRight);
            else
                EXPECT_MOVE(opponentRight, MOVE_AIR_SLASH, target: playerRight);
            SEND_OUT(playerRight, 2);
        }
    }
}
