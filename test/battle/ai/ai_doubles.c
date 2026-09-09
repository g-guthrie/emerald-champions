#include "global.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "battle_ai_main.h"
#include "battle_ai_switch.h"
#include "battle_util.h"
#include "random.h"
#include "malloc.h"
#include "main.h"

#define EC_EXPERT_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES \
    | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO \
    | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

// Synthetic shared regressions; each fails with its corresponding new forecast disabled.
static void StaminaFollowupBoard(bool32 stamina)
{
    PLAYER(SPECIES_MUDBRAY) {
        Level(14); HP(100); MaxHP(100); Attack(20); Defense(50); SpAttack(20); SpDefense(50); Speed(30);
        Ability(stamina ? ABILITY_STAMINA : ABILITY_OWN_TEMPO);
        Item(ITEM_NONE); Moves(MOVE_HIGH_HORSEPOWER);
    }
    PLAYER(SPECIES_GIMMIGHOUL) {
        Level(14); HP(100); MaxHP(100); Attack(20); Defense(100); SpAttack(20); SpDefense(100); Speed(20);
        Ability(ABILITY_RATTLED); Item(ITEM_NONE); Moves(MOVE_PROTECT);
    }
    OPPONENT(SPECIES_PACHIRISU) {
        Level(14); HP(100); MaxHP(100); Attack(10); Defense(100); SpAttack(20); SpDefense(100); Speed(70);
        Ability(ABILITY_VOLT_ABSORB); Item(ITEM_NONE); Moves(MOVE_QUICK_ATTACK);
    }
    OPPONENT(SPECIES_CLAMPERL) {
        Level(14); HP(100); MaxHP(100); Attack(40); Defense(50); SpAttack(40); SpDefense(50); Speed(40);
        Ability(ABILITY_SHELL_ARMOR); Item(ITEM_NONE); Moves(MOVE_WATERFALL, MOVE_ICE_BEAM);
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: Stamina changes the followup after partner chip")
{
    bool32 stamina;
    PARAMETRIZE { stamina = FALSE; }
    PARAMETRIZE { stamina = TRUE; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        StaminaFollowupBoard(stamina);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_HIGH_HORSEPOWER, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(gLastMoves[B_BATTLER_3], stamina ? MOVE_ICE_BEAM : MOVE_WATERFALL);
        EXPECT_EQ(playerLeft->hp, stamina ? 78 : 74);
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: Rock Tomb earns a partner crossing unless Cloak blocks it")
{
    bool32 cloak;
    PARAMETRIZE { cloak = FALSE; }
    PARAMETRIZE { cloak = TRUE; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_TAUROS) {
            Level(50); HP(100); MaxHP(100); Attack(300); SpAttack(210);
            Defense(100); SpDefense(100); Speed(80); Ability(ABILITY_ANGER_POINT);
            Item(cloak ? ITEM_COVERT_CLOAK : ITEM_NONE); Moves(MOVE_TACKLE);
        }
        PLAYER(SPECIES_CHANSEY) {
            Level(50); HP(300); MaxHP(300); Attack(100); SpAttack(100);
            Defense(300); SpDefense(300); Speed(10); Ability(ABILITY_NATURAL_CURE); Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_SMEARGLE) {
            Level(50); HP(300); MaxHP(300); Attack(60); SpAttack(10);
            Defense(300); SpDefense(300); Speed(100); Ability(ABILITY_OWN_TEMPO);
            Moves(MOVE_ROCK_TOMB, MOVE_STRENGTH);
        }
        OPPONENT(SPECIES_ORANGURU) {
            Level(50); HP(65); MaxHP(65); Attack(100); SpAttack(300);
            Defense(100); SpDefense(100); Speed(60); Ability(ABILITY_TELEPATHY);
            Moves(MOVE_PSYCHIC, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(gLastMoves[B_BATTLER_1], cloak ? MOVE_STRENGTH : MOVE_ROCK_TOMB);
        EXPECT_EQ(gLastMoves[B_BATTLER_3], cloak ? MOVE_PROTECT : MOVE_PSYCHIC);
        EXPECT_EQ(playerLeft->hp, cloak ? 72 : 0);
        EXPECT_GT(opponentRight->hp, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: Iron Defense mitigates only later physical attacks")
{
    bool32 fast;
    PARAMETRIZE { fast = FALSE; }
    PARAMETRIZE { fast = TRUE; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_LOTAD) {
            Level(14); HP(100); MaxHP(100); Attack(100); Defense(100);
            SpAttack(100); SpDefense(100); Speed(10);
            Ability(ABILITY_RAIN_DISH); Item(ITEM_EVIOLITE); Moves(MOVE_PROTECT);
        }
        PLAYER(SPECIES_MUNCHLAX) {
            Level(14); HP(98); MaxHP(98); Attack(80); Defense(22);
            SpAttack(39); SpDefense(37); Speed(fast ? 25 : 9);
            Nature(NATURE_QUIET); Ability(ABILITY_THICK_FAT); Item(ITEM_EVIOLITE);
            Moves(MOVE_BRICK_BREAK);
        }
        OPPONENT(SPECIES_GIMMIGHOUL) {
            Level(12); HP(100); MaxHP(100); Attack(100); Defense(100);
            SpAttack(100); SpDefense(100); Speed(10);
            Ability(ABILITY_RATTLED); Item(ITEM_FOCUS_SASH); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_ROGGENROLA) {
            Level(12); HP(50); MaxHP(70); Attack(30); Defense(40);
            SpAttack(15); SpDefense(20); Speed(12); Nature(NATURE_IMPISH);
            Ability(ABILITY_STURDY); Item(ITEM_NONE);
            Moves(MOVE_IRON_DEFENSE, MOVE_BODY_PRESS);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentRight,
                hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        EXPECT_EQ(gLastMoves[B_BATTLER_3], fast ? MOVE_BODY_PRESS : MOVE_IRON_DEFENSE);
        EXPECT_EQ(opponentRight->hp, fast ? 12 : 30);
        EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE + (fast ? 0 : 2));
    }
}

static void DefeatistPriorityBoard(u32 actorHp)
{
    PLAYER(SPECIES_EEVEE) {
        Level(14); HP(1); MaxHP(100); Attack(60); Defense(20); SpAttack(20); SpDefense(50); Speed(40);
        Ability(ABILITY_ADAPTABILITY); Item(ITEM_NONE); Moves(MOVE_QUICK_ATTACK);
    }
    PLAYER(SPECIES_MUNCHLAX) {
        Level(14); HP(20); MaxHP(20);
        Attack(20); Defense(50); SpAttack(20); SpDefense(50); Speed(5);
        Ability(ABILITY_THICK_FAT); Item(ITEM_NONE); Moves(MOVE_STOCKPILE);
    }
    OPPONENT(SPECIES_ARCHEN) {
        Level(12); HP(actorHp); MaxHP(100); Attack(60); Defense(30); SpAttack(20); SpDefense(30); Speed(70);
        Ability(ABILITY_DEFEATIST); Item(ITEM_NONE); Moves(MOVE_ACROBATICS, MOVE_QUICK_ATTACK);
    }
    OPPONENT(SPECIES_PACHIRISU) {
        Level(12); HP(100); MaxHP(100); Attack(20); Defense(50); SpAttack(20); SpDefense(50); Speed(10);
        Ability(ABILITY_VOLT_ABSORB); Item(ITEM_NONE); Moves(MOVE_CELEBRATE);
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: prevent a priority hit from activating Defeatist before attack")
{
    u32 actorHp;
    PARAMETRIZE { actorHp = 40; }
    PARAMETRIZE { actorHp = 60; }
    PARAMETRIZE { actorHp = 100; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        DefeatistPriorityBoard(actorHp);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_QUICK_ATTACK, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_STOCKPILE);
        }
    } THEN {
        EXPECT_EQ(gLastMoves[B_BATTLER_1], actorHp == 100 ? MOVE_ACROBATICS : MOVE_QUICK_ATTACK);
        EXPECT_EQ(opponentLeft->hp, actorHp == 100 ? 89 : actorHp);
        EXPECT_EQ(actorHp == 100 ? playerRight->hp : playerLeft->hp, 0);
    }
}


// Isolated synthetic decision boundaries, not authored trainer locks.
// Each positive fails when its new per-move forecast is disabled.
AI_DOUBLE_BATTLE_TEST("EC expert pair: Sleep Powder denial respects native accuracy Grass and Goggles")
{
    u32 control;
    PARAMETRIZE { control = 0; }
    PARAMETRIZE { control = 1; }
    PARAMETRIZE { control = 2; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(control == 2 ? SPECIES_BULBASAUR : SPECIES_MUNCHLAX) {
            Level(14); HP(100); MaxHP(100); Attack(160); Defense(120); SpAttack(20); SpDefense(100); Speed(60);
            Ability(control == 2 ? ABILITY_OVERGROW : ABILITY_THICK_FAT);
            Item(control == 1 ? ITEM_SAFETY_GOGGLES : ITEM_NONE); Moves(MOVE_BODY_SLAM);
        }
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(100); MaxHP(100); Attack(20); Defense(100); SpAttack(20); SpDefense(100); Speed(20);
            Ability(ABILITY_VOLT_ABSORB); Item(ITEM_NONE); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_VENONAT) {
            Level(12); HP(70); MaxHP(70); Attack(20); Defense(25); SpAttack(20); SpDefense(40); Speed(70);
            Ability(ABILITY_COMPOUND_EYES); Item(ITEM_NONE);
            Moves(MOVE_RAGE_POWDER, MOVE_SLEEP_POWDER, MOVE_STRUGGLE_BUG, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_KRICKETUNE) {
            Level(12); HP(45); MaxHP(45); Attack(80); Defense(25); SpAttack(20); SpDefense(30); Speed(50);
            Ability(ABILITY_TECHNICIAN); Item(ITEM_LIFE_ORB);
            Moves(MOVE_BUG_BITE, MOVE_AERIAL_ACE, MOVE_TAUNT, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BODY_SLAM, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            if (control == 0)
                EXPECT_MOVE(opponentLeft, MOVE_SLEEP_POWDER, target: playerLeft);
            else
                EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_0][1], 97);
        if (control)
            EXPECT_EQ(playerLeft->status1 & STATUS1_SLEEP, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: Quiver Dance mitigates only later special attacks")
{
    u32 board;
    PARAMETRIZE { board = 0; } // Slow special attacker: possible immediate payoff.
    PARAMETRIZE { board = 1; } // Faster special attacker: boost arrives too late.
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_LOTAD) {
            Level(14); HP(100); MaxHP(100); Attack(100); Defense(100);
            SpAttack(100); SpDefense(100); Speed(10);
            Ability(ABILITY_RAIN_DISH); Item(ITEM_EVIOLITE); Moves(MOVE_PROTECT);
        }
        PLAYER(SPECIES_WOOPER) {
            Level(14); HP(98); MaxHP(98); Attack(49); Defense(22);
            SpAttack(39); SpDefense(37); Speed(board == 1 ? 65 : 9);
            Nature(NATURE_QUIET); Ability(ABILITY_WATER_ABSORB);
            Item(ITEM_EVIOLITE); Moves(MOVE_ICE_BEAM);
        }
        OPPONENT(SPECIES_GIMMIGHOUL) {
            Level(12); HP(100); MaxHP(100); Attack(100); Defense(100);
            SpAttack(100); SpDefense(100); Speed(10);
            Ability(ABILITY_RATTLED); Item(ITEM_FOCUS_SASH); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_BEAUTIFLY) {
            Level(12); HP(40); MaxHP(50); Attack(18); Defense(23);
            SpAttack(67); SpDefense(23); Speed(58); Nature(NATURE_TIMID);
            Ability(ABILITY_SWARM); Item(ITEM_NONE);
            Moves(MOVE_QUIVER_DANCE, MOVE_BUG_BUZZ);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_ICE_BEAM, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            EXPECT_MOVE(opponentRight, board == 0 ? MOVE_QUIVER_DANCE : MOVE_BUG_BUZZ);
        }
    } THEN {
        EXPECT_EQ(opponentRight->hp, board == 0 ? 14 : 2);
        EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + (board == 0));
    }
}

// Synthetic decision boundary, not a trainer loadout lock. Disabling the
// flavor-berry forecast changes the useful healing attack into a wasted guard.
AI_DOUBLE_BATTLE_TEST("EC expert pair: recoil can trigger Gluttony healing unless Unnerve blocks it")
{
    u32 control;
    PARAMETRIZE { control = 0; }
    PARAMETRIZE { control = 1; }
    PARAMETRIZE { control = 2; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_CORVIKNIGHT) {
            Level(14); HP(500); MaxHP(500); Attack(10); Defense(40); SpAttack(30); SpDefense(30); Speed(40);
            Ability(control == 2 ? ABILITY_UNNERVE : ABILITY_PRESSURE); Item(ITEM_NONE); Moves(MOVE_ROOST);
        }
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(100); MaxHP(100); Attack(10); Defense(100); SpAttack(30); SpDefense(30); Speed(30);
            Ability(ABILITY_VOLT_ABSORB); Item(ITEM_NONE); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_WOBBUFFET) {
            Level(12); HP(200); MaxHP(200); Defense(100); SpDefense(100); Speed(10);
            Ability(ABILITY_TELEPATHY); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_ZIGZAGOON) {
            Level(12); HP(19); MaxHP(36); Attack(50); Defense(30); SpAttack(20); SpDefense(30); Speed(60);
            Nature(NATURE_ADAMANT); Ability(ABILITY_GLUTTONY); Item(control ? ITEM_FIGY_BERRY : ITEM_NONE);
            Moves(MOVE_DOUBLE_EDGE, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ROOST);
            MOVE(playerRight, MOVE_PROTECT);
            if (control == 1)
                EXPECT_MOVE(opponentRight, MOVE_DOUBLE_EDGE, target: playerLeft);
            else
                EXPECT_MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(opponentRight->hp, control == 1 ? 28 : 19);
        EXPECT_EQ(opponentRight->item, control == 2 ? ITEM_FIGY_BERRY : ITEM_NONE);
        EXPECT_EQ((u32)opponentRight->volatiles.confusionTimer, 0);
    }
}

// One isolated regression for two demonstrated faults: a nominal immune
// spread target must not hide the damageable recipient, and faster public
// chip must weaken HP-powered damage before the attacker acts. No trainer
// catalogue or authored set is locked by these synthetic mechanical stats.
AI_DOUBLE_BATTLE_TEST("EC expert pair: HP-powered damage follows public faster chip")
{
    u32 attackerSpeed;
    PARAMETRIZE { attackerSpeed = 10; }
    PARAMETRIZE { attackerSpeed = 35; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(100); MaxHP(100); SpAttack(17); SpDefense(50);
            Speed(attackerSpeed); Ability(ABILITY_VOLT_ABSORB);
            Moves(MOVE_THUNDERBOLT);
        }
        PLAYER(SPECIES_MANTINE) {
            Level(14); HP(100); MaxHP(100); Speed(5);
            Ability(ABILITY_WATER_ABSORB); Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_ORANGURU) {
            Level(12); HP(100); MaxHP(100); SpDefense(200); Speed(5);
            Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WAILMER) {
            Level(12); HP(88); MaxHP(88); SpAttack(62); SpDefense(17);
            Speed(23); Ability(ABILITY_OBLIVIOUS);
            Moves(MOVE_WATER_SPOUT, MOVE_SCALD);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentRight, attackerSpeed < 23 ? MOVE_WATER_SPOUT : MOVE_SCALD);
        }
    } THEN {
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_LT(opponentRight->hp, opponentRight->maxHP);
        EXPECT_GT(playerLeft->hp, 0);
        EXPECT_LT(playerLeft->hp, playerLeft->maxHP);
        EXPECT_EQ(playerRight->hp, playerRight->maxHP);
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: offensive drops protect a slower partner only when timely and unblocked")
{
    bool32 special;
    u32 boundary;
    PARAMETRIZE { special = FALSE; boundary = 0; }
    PARAMETRIZE { special = TRUE; boundary = 0; }
    PARAMETRIZE { special = FALSE; boundary = 1; }
    PARAMETRIZE { special = TRUE; boundary = 1; }
    PARAMETRIZE { special = FALSE; boundary = 2; }
    PARAMETRIZE { special = TRUE; boundary = 2; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_TAUROS) {
            Level(50); HP(100); MaxHP(100); Attack(300); SpAttack(210);
            Defense(100); SpDefense(100); Speed(80); Ability(ABILITY_ANGER_POINT);
            Item(boundary == 1 ? ITEM_COVERT_CLOAK : ITEM_NONE);
            Moves(special ? MOVE_ICE_BEAM : MOVE_TACKLE);
        }
        PLAYER(SPECIES_CHANSEY) {
            Level(50); HP(300); MaxHP(300); Defense(300); SpDefense(300);
            Speed(10); Ability(ABILITY_NATURAL_CURE); Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_SMEARGLE) {
            Level(50); HP(300); MaxHP(300); Attack(60); SpAttack(10);
            Defense(300); SpDefense(300); Speed(boundary == 2 ? 60 : 100); Ability(ABILITY_OWN_TEMPO);
            Moves(special ? MOVE_SKITTER_SMACK : MOVE_LUNGE, MOVE_STRENGTH);
        }
        OPPONENT(SPECIES_ORANGURU) {
            Level(50); HP(65); MaxHP(65); SpAttack(300);
            Defense(100); SpDefense(100); Speed(20); Ability(ABILITY_TELEPATHY);
            Moves(MOVE_PSYCHIC, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, special ? MOVE_ICE_BEAM : MOVE_TACKLE, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_CELEBRATE);
            // Strength wins on damage alone. Only a useful, earlier drop lets
            // the slower partner attack safely; Cloak and timing remove that.
            EXPECT_MOVE(opponentLeft, boundary ? MOVE_STRENGTH : special ? MOVE_SKITTER_SMACK : MOVE_LUNGE, target: playerLeft);
            EXPECT_MOVE(opponentRight, boundary ? MOVE_PROTECT : MOVE_PSYCHIC);
        }
    } THEN {
        if (boundary)
            EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
        else if (!special || playerLeft->hp != playerLeft->maxHP)
        {
            // Keep Skitter's native accuracy; a miss is not a landed payoff.
            EXPECT_EQ(playerLeft->hp, 0);
            EXPECT_GT(opponentRight->hp, 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: Life Orb costs a hit and stolen Sitrus heals before recoil")
{
    u32 injury;
    bool32 berry, barbs;
    PARAMETRIZE { injury = 4; berry = FALSE; barbs = FALSE; }
    PARAMETRIZE { injury = 5; berry = FALSE; barbs = FALSE; }
    PARAMETRIZE { injury = 4; berry = TRUE; barbs = FALSE; }
    PARAMETRIZE { injury = 5; berry = FALSE; barbs = TRUE; }
    PARAMETRIZE { injury = 5; berry = TRUE; barbs = TRUE; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(barbs ? SPECIES_FERROSEED : SPECIES_PACHIRISU) {
            Level(14); HP(77); MaxHP(77); Attack(18); Defense(66);
            SpAttack(21); SpDefense(36); Speed(35);
            Nature(NATURE_BOLD); Ability(barbs ? ABILITY_IRON_BARBS : ABILITY_VOLT_ABSORB);
            Item(berry ? ITEM_SITRUS_BERRY : ITEM_NONE);
            Moves(barbs ? MOVE_HARDEN : MOVE_FOLLOW_ME, MOVE_PROTECT);
        }
        PLAYER(barbs ? SPECIES_SHEDINJA : SPECIES_TIMBURR) {
            Level(14); HP(barbs ? 1 : 81); MaxHP(barbs ? 1 : 81); Attack(31); Defense(61);
            SpAttack(14); SpDefense(21); Speed(19);
            Nature(NATURE_IMPISH); Ability(barbs ? ABILITY_WONDER_GUARD : ABILITY_IRON_FIST); Item(ITEM_EVIOLITE);
            Moves(barbs ? MOVE_SWORDS_DANCE : MOVE_BULK_UP, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_DEWPIDER) {
            Level(12); HP(66); MaxHP(66); Attack(55); Defense(23);
            SpAttack(16); SpDefense(26); Speed(15);
            Nature(NATURE_ADAMANT); Ability(ABILITY_WATER_BUBBLE); Item(ITEM_SITRUS_BERRY);
            Moves(MOVE_LIQUIDATION, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_KRICKETUNE) {
            Level(12); HP(injury); MaxHP(46); Attack(61); Defense(20);
            SpAttack(18); SpDefense(20); Speed(61);
            Nature(NATURE_JOLLY); Ability(ABILITY_TECHNICIAN); Item(ITEM_LIFE_ORB);
            Moves(MOVE_BUG_BITE, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, barbs ? MOVE_HARDEN : MOVE_FOLLOW_ME);
            MOVE(playerRight, barbs ? MOVE_SWORDS_DANCE : MOVE_BULK_UP);
            if (!barbs)
                EXPECT_MOVE(opponentLeft, MOVE_LIQUIDATION);
            EXPECT_MOVE(opponentRight, !berry && (injury == 4 || barbs) ? MOVE_PROTECT : MOVE_BUG_BITE);
        }
    } THEN {
        EXPECT_EQ(opponentRight->hp, barbs ? (berry ? 7 : 5) : berry ? 11 : injury == 4 ? 4 : 1);
        EXPECT_EQ(opponentRight->item, ITEM_LIFE_ORB);
        EXPECT_EQ(playerLeft->item, ITEM_NONE);
        EXPECT_EQ(gBattleResults.opponentFaintCounter, 0);
        // No opposing attack can explain these HP changes. Contact, when
        // present, follows stolen healing and precedes the one Orb payment.
        // The passive Wonder Guard partner cannot offer a safe chip target.
    }
}

// Isolated explicit-stat regressions, not a duplicate campaign trainer table.
// Distinct regressions for demonstrated Trace entry and Wish timing failures.
// Self-contained mechanical positions; no campaign roster or plan lock.
static u32 TraceCandidateStateHash(void)
{
    const void *blocks[] = {gBattleMons, gParties, gBattleStruct, gAiLogicData, &gRngValue, &gRng2Value};
    const u32 sizes[] = {sizeof(gBattleMons), sizeof(gParties), sizeof(*gBattleStruct), sizeof(*gAiLogicData), sizeof(gRngValue), sizeof(gRng2Value)};
    u32 hash = 2166136261u;
    for (u32 block = 0; block < ARRAY_COUNT(blocks); block++)
        for (u32 index = 0; index < sizes[block]; index++)
            hash = (hash ^ ((const u8 *)blocks[block])[index]) * 16777619u;
    return hash;
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: candidate Trace copies deterministic abilities and restores state")
{
    u32 control;
    PARAMETRIZE { control = 0; } // Identical Volt Absorb.
    PARAMETRIZE { control = 1; } // Identical Intimidate, including its entry effect.
    PARAMETRIZE { control = 2; } // Ability Shield prevents copying.
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(control == 1 ? SPECIES_GROWLITHE : SPECIES_PACHIRISU) {
            Level(14); Ability(control == 1 ? ABILITY_INTIMIDATE : ABILITY_VOLT_ABSORB);
            Moves(MOVE_PROTECT);
        }
        PLAYER(control == 1 ? SPECIES_GROWLITHE : SPECIES_PACHIRISU) {
            Level(14); Ability(control == 1 ? ABILITY_INTIMIDATE : ABILITY_VOLT_ABSORB);
            Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_EEVEE) {
            Level(12); Ability(ABILITY_ADAPTABILITY); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_SNUBBULL) {
            Level(12); Ability(ABILITY_INTIMIDATE); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_RALTS) {
            Level(12); Ability(ABILITY_TRACE);
            Item(control == 2 ? ITEM_ABILITY_SHIELD : ITEM_FOCUS_SASH);
            Moves(MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
            EXPECT_MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT(gAiThinkingStruct->aiFlags[B_BATTLER_3] & AI_FLAG_OMNISCIENT);
        EXPECT_EQ(gBattleMons[B_BATTLER_3].species, SPECIES_SNUBBULL);
        EXPECT_EQ(gBattleMons[B_BATTLER_0].statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(gBattleMons[B_BATTLER_2].statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        u32 before = TraceCandidateStateHash();
        struct SwitchCandidateSnapshot *state = AI_SaveCandidateState();
        EXPECT(state != NULL);
        AI_LoadSwitchCandidate(B_BATTLER_3, 2, FALSE);
        enum Ability expected = control == 0 ? ABILITY_VOLT_ABSORB : control == 1 ? ABILITY_INTIMIDATE : ABILITY_TRACE;
        EXPECT_EQ(gBattleMons[B_BATTLER_3].species, SPECIES_RALTS);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_3), expected);
        EXPECT_EQ(gAiLogicData->abilities[B_BATTLER_3], expected);
        EXPECT_EQ((u32)gBattleMons[B_BATTLER_3].volatiles.overwrittenAbility, control < 2 ? expected : ABILITY_NONE);
        EXPECT_EQ((u32)gBattleMons[B_BATTLER_3].volatiles.traceActivated, TRUE);
        EXPECT_EQ(gBattleMons[B_BATTLER_0].statStages[STAT_ATK], DEFAULT_STAT_STAGE - (control == 1 ? 2 : 1));
        EXPECT_EQ(gBattleMons[B_BATTLER_2].statStages[STAT_ATK], DEFAULT_STAT_STAGE - (control == 1 ? 2 : 1));
        // Internal trial RNG use is permitted. No complete pair evaluation is
        // nested inside this snapshot; only the loader and its entry effects.
        AI_RestoreCandidateState(state);
        AI_FreeCandidateState(state);
        EXPECT_EQ(TraceCandidateStateHash(), before);
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: Wish survives modest pressure and pays at the next end turn", s16 healing)
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(77); MaxHP(77); Attack(18); Defense(66);
            SpAttack(21); SpDefense(36); Speed(35); Nature(NATURE_BOLD);
            Ability(ABILITY_VOLT_ABSORB); Item(ITEM_EVIOLITE);
            Moves(MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT);
        }
        PLAYER(SPECIES_LOTAD) {
            Level(14); HP(71); MaxHP(71); Attack(15); Defense(53);
            SpAttack(20); SpDefense(25); Speed(17); Nature(NATURE_BOLD);
            Ability(ABILITY_RAIN_DISH); Item(ITEM_EVIOLITE);
            Moves(MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_REST, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_EEVEE) {
            Level(12); HP(35); MaxHP(70); Attack(58); Defense(22);
            SpAttack(17); SpDefense(24); Speed(21); Nature(NATURE_ADAMANT);
            Ability(ABILITY_ADAPTABILITY); Item(ITEM_EVIOLITE);
            Moves(MOVE_QUICK_ATTACK, MOVE_DOUBLE_EDGE, MOVE_WISH, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_RALTS) {
            Level(12); HP(34); MaxHP(34); Attack(12); Defense(14);
            SpAttack(51); SpDefense(17); Speed(55); Nature(NATURE_TIMID);
            Ability(ABILITY_TRACE); Item(ITEM_FOCUS_SASH);
            Moves(MOVE_PSYCHIC, MOVE_DAZZLING_GLEAM, MOVE_THUNDER_WAVE, MOVE_HELPING_HAND);
        }
        OPPONENT(SPECIES_MARILL) {
            Level(12); HP(74); MaxHP(74); Attack(49); Defense(22);
            SpAttack(11); SpDefense(20); Speed(18); Nature(NATURE_ADAMANT);
            Ability(ABILITY_HUGE_POWER); Item(ITEM_SITRUS_BERRY);
            Moves(MOVE_AQUA_JET, MOVE_PLAY_ROUGH, MOVE_BRICK_BREAK, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_SNUBBULL) {
            Level(12); HP(72); MaxHP(72); Attack(64); Defense(22);
            SpAttack(16); SpDefense(18); Speed(15); Nature(NATURE_ADAMANT);
            Ability(ABILITY_INTIMIDATE); Item(ITEM_LUM_BERRY);
            Moves(MOVE_PLAY_ROUGH, MOVE_FIRE_PUNCH, MOVE_THUNDER_WAVE, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentLeft, MOVE_WISH);
        }
        if (turns == 2)
        {
            TURN {
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_PROTECT);
                // Partner switching and Eevee's second action stay free;
                // this protects a healing payoff, not a prescribed script.
            }
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, opponentLeft);
        if (turns == 2)
        {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_WISH_HEAL, opponentLeft);
            HP_BAR(opponentLeft, captureDamage: &results[i].healing);
        }
    } THEN {
        EXPECT_EQ(gBattleStruct->wish[B_BATTLER_1].counter, turns == 1 ? 1 : 0);
        if (turns == 2)
        {
            EXPECT_EQ(results[i].healing, -35);
            EXPECT_GT(opponentLeft->hp, 35);
        }
        else
        {
            EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_WISH);
            // Same genuine post-Wish board, with only the pending counter
            // removed for the second query. No outer heap snapshot: both
            // evaluators restore their trial positions internally.
            rng_value_t rng = gRngValue, rng2 = gRng2Value;
            u16 counter = gBattleStruct->wish[B_BATTLER_1].counter;
            SetAiLogicDataForTurn(gAiLogicData);
            s32 dueValue = AI_EvaluateDoublesPosition(B_BATTLER_1, 0);
            gBattleStruct->wish[B_BATTLER_1].counter = 0;
            gRngValue = rng;
            gRng2Value = rng2;
            SetAiLogicDataForTurn(gAiLogicData);
            s32 absentValue = AI_EvaluateDoublesPosition(B_BATTLER_1, 0);
            gBattleStruct->wish[B_BATTLER_1].counter = counter;
            gRngValue = rng;
            gRng2Value = rng2;
            SetAiLogicDataForTurn(gAiLogicData);
            gRngValue = rng;
            gRng2Value = rng2;
            EXPECT_GT(dueValue, absentValue);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: Cotton Guard earns only timely physical mitigation")
{
    u32 board;
    PARAMETRIZE { board = 0; } // Slow physical pressure.
    PARAMETRIZE { board = 1; } // Fast physical pressure, with the native HP/Def trade.
    PARAMETRIZE { board = 2; } // Slow special pressure, publicly different menu.
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO
            | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_LOTAD) {
            Level(14); HP(100); MaxHP(100); Attack(100); Defense(100);
            SpAttack(100); SpDefense(100); Speed(10);
            Ability(ABILITY_RAIN_DISH); Item(ITEM_EVIOLITE); Moves(MOVE_PROTECT);
        }
        PLAYER(SPECIES_MUNCHLAX) {
            Level(14); HP(board == 1 ? 82 : 98); MaxHP(board == 1 ? 82 : 98);
            Attack(49); Defense(board == 1 ? 20 : 22); SpAttack(39); SpDefense(37);
            Speed(board == 1 ? 25 : 9); Nature(NATURE_QUIET);
            Ability(ABILITY_THICK_FAT); Item(ITEM_EVIOLITE);
            Moves(board == 2 ? MOVE_ICE_BEAM : MOVE_BODY_SLAM);
        }
        OPPONENT(SPECIES_GIMMIGHOUL) {
            Level(12); HP(100); MaxHP(100); Attack(100); Defense(100);
            SpAttack(100); SpDefense(100); Speed(10);
            Ability(ABILITY_RATTLED); Item(ITEM_FOCUS_SASH); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_WOOLOO) {
            Level(12); HP(67); MaxHP(67); Attack(55); Defense(23);
            SpAttack(18); SpDefense(19); Speed(18); Nature(NATURE_BRAVE);
            Ability(ABILITY_FLUFFY); Item(ITEM_LEFTOVERS);
            Moves(MOVE_COTTON_GUARD, MOVE_DOUBLE_EDGE);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, board == 2 ? MOVE_ICE_BEAM : MOVE_BODY_SLAM, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            EXPECT_MOVE(opponentRight, board == 0 ? MOVE_COTTON_GUARD : MOVE_DOUBLE_EDGE);
        }
    } THEN {
        EXPECT_EQ(opponentRight->statStages[STAT_DEF], board == 0 ? DEFAULT_STAT_STAGE + 3 : DEFAULT_STAT_STAGE);
        EXPECT_GT(opponentRight->hp, 0);
        if (board == 0)
            EXPECT_EQ(playerRight->hp, playerRight->maxHP);
        else
            EXPECT_LT(playerRight->hp, playerRight->maxHP);
        // Native defense boosts cannot reduce an earlier or special hit.
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: damage-based recoil distinguishes unsafe and surviving attacks")
{
    u32 injury;
    PARAMETRIZE { injury = 5; }
    PARAMETRIZE { injury = 6; }
    PARAMETRIZE { injury = 7; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY
            | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(77); MaxHP(77); Attack(18); Defense(66);
            SpAttack(21); SpDefense(36); Speed(35);
            Nature(NATURE_BOLD); Ability(ABILITY_VOLT_ABSORB); Item(ITEM_NONE);
            Moves(MOVE_FOLLOW_ME, MOVE_PROTECT);
        }
        PLAYER(SPECIES_TIMBURR) {
            Level(14); HP(81); MaxHP(81); Attack(31); Defense(61);
            SpAttack(14); SpDefense(21); Speed(19);
            Nature(NATURE_IMPISH); Ability(ABILITY_IRON_FIST); Item(ITEM_EVIOLITE);
            Moves(MOVE_BULK_UP, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_GIMMIGHOUL) {
            Level(12); HP(68); MaxHP(68); Attack(15); Defense(27);
            SpAttack(63); SpDefense(25); Speed(9);
            Nature(NATURE_QUIET); Ability(ABILITY_RATTLED); Item(ITEM_EVIOLITE);
            Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_WOOLOO) {
            Level(12); HP(injury); MaxHP(67); Attack(55); Defense(23);
            SpAttack(18); SpDefense(19); Speed(18);
            Nature(NATURE_BRAVE); Ability(ABILITY_FLUFFY); Item(ITEM_LEFTOVERS);
            Moves(MOVE_DOUBLE_EDGE, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_BULK_UP);
            EXPECT_MOVE(opponentRight, injury < 7 ? MOVE_PROTECT : MOVE_DOUBLE_EDGE);
        }
    } THEN {
        EXPECT_EQ(opponentRight->hp, injury < 7 ? injury + 4 : 6);
        EXPECT_EQ(opponentRight->item, ITEM_LEFTOVERS);
        EXPECT_EQ(gBattleResults.opponentFaintCounter, 0);
        if (injury < 7)
            EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
        else
            EXPECT_LT(playerLeft->hp, playerLeft->maxHP);
        // Protection retains 5/6HP and then Leftovers restores four. The
        // accepted 7HP attack pays actual five-HP recoil before that recovery.
        // This is a bounded damage-roll risk decision, not an assertion that
        // every possible hit from six HP must be fatal.
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: timing probe with full parties and legal Mega reserves")
{
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_ORANGURU) { HP(500); MaxHP(500); Ability(ABILITY_TELEPATHY); Speed(90); Moves(MOVE_CELEBRATE, MOVE_THUNDERBOLT, MOVE_SHADOW_BALL, MOVE_PROTECT); }
        PLAYER(SPECIES_ORANGURU) { HP(500); MaxHP(500); Ability(ABILITY_TELEPATHY); Speed(80); Moves(MOVE_CELEBRATE, MOVE_SURF, MOVE_ICE_BEAM, MOVE_HELPING_HAND); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(60); Moves(MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_ENCORE, MOVE_DESTINY_BOND); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(60); Moves(MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_ENCORE, MOVE_DESTINY_BOND); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(60); Moves(MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_ENCORE, MOVE_DESTINY_BOND); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(60); Moves(MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_ENCORE, MOVE_DESTINY_BOND); }
        OPPONENT(SPECIES_MAWILE) { HP(300); MaxHP(300); Speed(70); Ability(ABILITY_INTIMIDATE); Item(ITEM_MAWILITE); Moves(MOVE_IRON_HEAD, MOVE_PLAY_ROUGH, MOVE_SUCKER_PUNCH, MOVE_PROTECT); }
        OPPONENT(SPECIES_TORKOAL) { HP(300); MaxHP(300); Speed(70); Ability(ABILITY_DROUGHT); Moves(MOVE_ERUPTION, MOVE_HEAT_WAVE, MOVE_BODY_PRESS, MOVE_PROTECT); }
        OPPONENT(SPECIES_VENUSAUR) { HP(300); MaxHP(300); Speed(70); Ability(ABILITY_CHLOROPHYLL); Item(ITEM_VENUSAURITE); Moves(MOVE_GIGA_DRAIN, MOVE_SLUDGE_BOMB, MOVE_SLEEP_POWDER, MOVE_PROTECT); }
        OPPONENT(SPECIES_METAGROSS) { HP(300); MaxHP(300); Speed(70); Item(ITEM_METAGROSSITE); Moves(MOVE_IRON_HEAD, MOVE_ZEN_HEADBUTT, MOVE_BULLET_PUNCH, MOVE_PROTECT); }
        OPPONENT(SPECIES_GASTRODON) { HP(300); MaxHP(300); Speed(70); Ability(ABILITY_STORM_DRAIN); Moves(MOVE_MUDDY_WATER, MOVE_EARTH_POWER, MOVE_RECOVER, MOVE_PROTECT); }
        OPPONENT(SPECIES_ORANGURU) { HP(300); MaxHP(300); Speed(70); Ability(ABILITY_TELEPATHY); Moves(MOVE_INSTRUCT, MOVE_TRICK_ROOM, MOVE_PSYCHIC, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); }
    } THEN {
        gAiLogicData->battlerMovesScored = 0;
        u32 start = gMain.vblankCounter1;
        bool32 selected = AI_ComputeDoublesDecisions(opponentLeft - gBattleMons);
        Test_MgbaPrintf("full-party paired decision selected=%d frames=%d", selected, gMain.vblankCounter1 - start);
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: Perish countdown survives and simultaneous deadlines choose distinct escapes")
{
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_EXPLOUD) { HP(300); MaxHP(300); Ability(ABILITY_SOUNDPROOF); Moves(MOVE_PERISH_SONG, MOVE_CELEBRATE); }
        PLAYER(SPECIES_MR_MIME) { HP(300); MaxHP(300); Ability(ABILITY_SOUNDPROOF); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(300); MaxHP(300); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { HP(300); MaxHP(300); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_EEVEE) { HP(300); MaxHP(300); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIKACHU) { HP(300); MaxHP(300); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PERISH_SONG); }
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_NE(gBattlerPartyIndexes[opponentLeft - gBattleMons], gBattlerPartyIndexes[opponentRight - gBattleMons]);
        EXPECT_EQ(GetMonData(&GetBattlerParty(opponentLeft - gBattleMons)[0], MON_DATA_HP), 300);
        EXPECT_EQ(GetMonData(&GetBattlerParty(opponentLeft - gBattleMons)[1], MON_DATA_HP), 300);
        EXPECT(!opponentLeft->volatiles.perishSong);
        EXPECT(!opponentRight->volatiles.perishSong);
        // The live countdown may sensibly stagger its switches. Separately
        // exercise the simultaneous deadline on this initialized native board.
        opponentLeft->volatiles.perishSong = opponentRight->volatiles.perishSong = TRUE;
        opponentLeft->volatiles.perishSongTimer = opponentRight->volatiles.perishSongTimer = 0;
        gAiLogicData->abilities[opponentLeft - gBattleMons] = ABILITY_SOUNDPROOF;
        gAiLogicData->abilities[opponentRight - gBattleMons] = ABILITY_SOUNDPROOF;
        gAiLogicData->battlerMovesScored = 0;
        EXPECT(AI_ComputeDoublesDecisions(opponentLeft - gBattleMons));
        EXPECT(gAiLogicData->shouldSwitch & (1u << (opponentLeft - gBattleMons)));
        EXPECT(gAiLogicData->shouldSwitch & (1u << (opponentRight - gBattleMons)));
        EXPECT_NE(gAiLogicData->monToSwitchInId[opponentLeft - gBattleMons], gAiLogicData->monToSwitchInId[opponentRight - gBattleMons]);
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: partner healing preserves the slower winning attack")
{
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_TAUROS) { Level(50); HP(80); MaxHP(80); Ability(ABILITY_ANGER_POINT); Attack(100); Defense(100); Speed(200); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_TAUROS) { Level(50); HP(80); MaxHP(80); Ability(ABILITY_ANGER_POINT); Attack(100); Defense(100); Speed(190); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_ORANGURU) { Level(50); HP(300); MaxHP(300); Attack(1); Defense(200); Speed(300); Ability(ABILITY_TELEPATHY); Moves(MOVE_HEAL_PULSE, MOVE_SCRATCH); }
        OPPONENT(SPECIES_GROUDON) { Level(50); HP(20); MaxHP(300); Attack(300); Defense(80); Speed(100); Moves(MOVE_EARTHQUAKE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_HEAL_PULSE, target: opponentRight);
            EXPECT_MOVE(opponentRight, MOVE_EARTHQUAKE);
        }
    } SCENE {
        MESSAGE("The opposing Oranguru used Heal Pulse!");
        HP_BAR(opponentRight);
        HP_BAR(opponentRight);
        HP_BAR(opponentRight);
        MESSAGE("The opposing Groudon used Earthquake!");
        HP_BAR(playerLeft);
        HP_BAR(playerRight);
    } THEN {
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_EQ(playerLeft->hp, 0);
        EXPECT_EQ(playerRight->hp, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: Follow Me protects the Trick Room setter")
{
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_TAUROS) { Level(50); HP(500); MaxHP(500); Attack(180); Defense(300); Speed(120); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_TAUROS) { Level(50); HP(500); MaxHP(500); Attack(180); Defense(300); Speed(110); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_CLEFABLE) { Level(50); HP(500); MaxHP(500); Attack(1); Defense(300); Speed(50); Moves(MOVE_FOLLOW_ME, MOVE_POUND); }
        OPPONENT(SPECIES_ORANGURU) { Level(50); HP(90); MaxHP(90); Defense(50); SpAttack(10); Speed(20); Moves(MOVE_TRICK_ROOM, MOVE_PSYCHIC); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_FOLLOW_ME);
            EXPECT_MOVE(opponentRight, MOVE_TRICK_ROOM);
        }
    } SCENE {
        MESSAGE("The opposing Clefable used Follow Me!");
        HP_BAR(opponentLeft);
        HP_BAR(opponentLeft);
        MESSAGE("The opposing Oranguru used Trick Room!");
    } THEN {
        EXPECT_EQ(opponentRight->hp, 90);
        EXPECT(gFieldStatuses & STATUS_FIELD_TRICK_ROOM);
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: beneficial weather is established once")
{
    enum Move weatherMove, attack;
    enum Ability ability;
    u32 expectedWeather;
    PARAMETRIZE { weatherMove = MOVE_RAIN_DANCE; attack = MOVE_WATER_PULSE; ability = ABILITY_SWIFT_SWIM; expectedWeather = B_WEATHER_RAIN; }
    PARAMETRIZE { weatherMove = MOVE_SUNNY_DAY; attack = MOVE_FLAMETHROWER; ability = ABILITY_CHLOROPHYLL; expectedWeather = B_WEATHER_SUN; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); SpDefense(300); Speed(100); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); SpDefense(300); Speed(90); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WHIMSICOTT) { HP(500); MaxHP(500); Attack(1); Speed(60); Ability(ABILITY_PRANKSTER); Moves(weatherMove, MOVE_SCRATCH); }
        OPPONENT(SPECIES_VENUSAUR) { HP(500); MaxHP(500); SpAttack(80); Speed(70); Ability(ability); Moves(attack); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, weatherMove); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_SCRATCH); }
    } THEN {
        EXPECT(gBattleWeather & expectedWeather);
    }
}

AI_DOUBLE_BATTLE_TEST("AI coordinates complementary Pledge moves even when both users have stronger physical STAB")
{
    u32 reverseOrderChance;

    PARAMETRIZE { reverseOrderChance = 0; }
    PARAMETRIZE { reverseOrderChance = 100; }

    GIVEN {
        WITH_CONFIG(AI_REVERSE_BATTLER_LOGIC_ORDER_CHANCE, reverseOrderChance);
        ASSUME(GetMoveEffect(MOVE_GRASS_PLEDGE) == EFFECT_PLEDGE);
        ASSUME(GetMoveEffect(MOVE_FIRE_PLEDGE) == EFFECT_PLEDGE);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { HP(400); }
        PLAYER(SPECIES_WOBBUFFET) { HP(400); }
        OPPONENT(SPECIES_THWACKEY) { Attack(120); SpAttack(40); Moves(MOVE_GRASS_PLEDGE, MOVE_WOOD_HAMMER, MOVE_PROTECT); }
        OPPONENT(SPECIES_RABOOT) { Attack(120); SpAttack(40); Moves(MOVE_FIRE_PLEDGE, MOVE_FLARE_BLITZ, MOVE_PROTECT); }
    } WHEN {
        TURN {
            EXPECT_MOVE(opponentLeft, MOVE_GRASS_PLEDGE);
            EXPECT_MOVE(opponentRight, MOVE_FIRE_PLEDGE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("AI only targets its ally with Decorate")
{
    GIVEN {
        ASSUME_STAT_CHANGE(MOVE_DECORATE, attack: +2, spAtk: +2);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ALCREMIE) { Moves(MOVE_DECORATE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_TAUROS) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN {
            EXPECT_MOVE(opponentLeft, MOVE_DECORATE, target: opponentRight);
            SCORE_EQ_VAL(opponentLeft, MOVE_DECORATE, 0, target: playerLeft);
            SCORE_EQ_VAL(opponentLeft, MOVE_DECORATE, 0, target: playerRight);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("AI won't use a Weather changing move if partner already chose such move")
{
    enum Move j, k;
    static const enum Move weatherMoves[] = {MOVE_SUNNY_DAY, MOVE_HAIL, MOVE_RAIN_DANCE, MOVE_SANDSTORM, MOVE_SNOWSCAPE};
    enum Move weatherMoveLeft = MOVE_NONE, weatherMoveRight = MOVE_NONE;

    for (j = 0; j < ARRAY_COUNT(weatherMoves); j++)
    {
        for (k = 0; k < ARRAY_COUNT(weatherMoves); k++)
        {
            PARAMETRIZE { weatherMoveLeft = weatherMoves[j]; weatherMoveRight = weatherMoves[k]; }
        }
    }

    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(weatherMoveLeft); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH, weatherMoveRight); }
    } WHEN {
        TURN {
            NOT_EXPECT_MOVE(opponentRight, weatherMoveRight);
            SCORE_LT_VAL(opponentRight, weatherMoveRight, AI_SCORE_DEFAULT, target:playerLeft);
            SCORE_LT_VAL(opponentRight, weatherMoveRight, AI_SCORE_DEFAULT, target:playerRight);
            SCORE_LT_VAL(opponentRight, weatherMoveRight, AI_SCORE_DEFAULT, target:opponentLeft);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: Helping Hand requires a damaging partner action")
{
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Speed(40); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Speed(30); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Speed(20); Moves(MOVE_HELPING_HAND, MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Speed(10); Moves(MOVE_TOXIC, MOVE_PROTECT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_SCRATCH);
        }
    } SCENE {
        NOT MESSAGE("The opposing Wobbuffet used Helping Hand!");
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: Coaching values the partner attack and physical survival")
{
    enum Ability ability;
    PARAMETRIZE { ability = ABILITY_TELEPATHY; }
    PARAMETRIZE { ability = ABILITY_SIMPLE; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(80); MaxHP(80); Attack(180); Defense(100); Speed(60); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(80); MaxHP(80); Attack(180); Defense(100); Speed(50); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_RIOLU) { Level(50); HP(300); MaxHP(300); Attack(10); Defense(300); Speed(30); Ability(ABILITY_PRANKSTER); Moves(MOVE_COACHING, MOVE_TACKLE); }
        OPPONENT(SPECIES_ORANGURU) { Level(50); HP(55); MaxHP(55); Attack(200); Defense(100); Speed(40); Ability(ability); Moves(MOVE_EARTHQUAKE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            EXPECT_MOVE(opponentLeft, MOVE_COACHING);
            EXPECT_MOVE(opponentRight, MOVE_EARTHQUAKE);
        }
    } THEN {
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + (ability == ABILITY_SIMPLE ? 2 : 1));
        EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE + (ability == ABILITY_SIMPLE ? 2 : 1));
        EXPECT_EQ(playerLeft->hp, 0);
        EXPECT_EQ(playerRight->hp, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: Acid Spray enables only a later unblocked special attack")
{
    u32 setterSpeed;
    enum Item item;
    bool32 shouldSpray;
    PARAMETRIZE { setterSpeed = 100; item = ITEM_NONE; shouldSpray = TRUE; }
    PARAMETRIZE { setterSpeed = 10; item = ITEM_NONE; shouldSpray = FALSE; }
    PARAMETRIZE { setterSpeed = 100; item = ITEM_COVERT_CLOAK; shouldSpray = FALSE; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(155); MaxHP(155); SpDefense(100); Speed(30); Item(item); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(155); MaxHP(155); SpDefense(100); Speed(20); Item(item); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_TENTACOOL) { Level(50); HP(300); MaxHP(300); SpAttack(100); Speed(setterSpeed); Moves(MOVE_ACID_SPRAY, MOVE_SLUDGE_BOMB); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); HP(300); MaxHP(300); SpAttack(200); Speed(50); Moves(MOVE_THUNDERBOLT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            if (shouldSpray)
                EXPECT_MOVE(opponentLeft, MOVE_ACID_SPRAY);
            else
                EXPECT_MOVE(opponentLeft, MOVE_SLUDGE_BOMB);
            EXPECT_MOVE(opponentRight, MOVE_THUNDERBOLT);
        }
    } THEN {
        if (shouldSpray)
            EXPECT(playerLeft->hp == 0 || playerRight->hp == 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: Motor Drive activation earns its same-turn speed crossing")
{
    u32 recipientSpeed;
    bool32 needsBoost;
    PARAMETRIZE { recipientSpeed = 80; needsBoost = TRUE; }
    PARAMETRIZE { recipientSpeed = 120; needsBoost = FALSE; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_GARCHOMP) { Level(50); HP(130); MaxHP(130); Attack(300); Defense(100); Speed(100); Ability(ABILITY_SAND_VEIL); Moves(MOVE_EARTHQUAKE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(300); MaxHP(300); SpDefense(200); Speed(30); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ELECTRODE) { Level(50); HP(200); MaxHP(200); SpAttack(100); Speed(150); Ability(ABILITY_SOUNDPROOF); Moves(MOVE_DISCHARGE, MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_ELECTIVIRE) { Level(50); HP(180); MaxHP(180); Attack(200); Defense(100); Speed(recipientSpeed); Ability(ABILITY_MOTOR_DRIVE); Moves(MOVE_ICE_PUNCH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_EARTHQUAKE);
            MOVE(playerRight, MOVE_CELEBRATE);
            if (needsBoost)
                EXPECT_MOVE(opponentLeft, MOVE_DISCHARGE);
            else
                EXPECT_MOVE(opponentLeft, MOVE_THUNDERBOLT, target: playerRight);
            EXPECT_MOVE(opponentRight, MOVE_ICE_PUNCH, target: playerLeft);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 0);
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
        EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + needsBoost);
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: Klutz transfers Flame Orb to a vulnerable physical attacker")
{
    enum Species targetSpecies;
    enum Ability targetAbility;
    bool32 shouldTransfer;
    PARAMETRIZE { targetSpecies = SPECIES_SNORLAX; targetAbility = ABILITY_THICK_FAT; shouldTransfer = TRUE; }
    PARAMETRIZE { targetSpecies = SPECIES_RATTATA; targetAbility = ABILITY_GUTS; shouldTransfer = FALSE; }
    PARAMETRIZE { targetSpecies = SPECIES_TORKOAL; targetAbility = ABILITY_SHELL_ARMOR; shouldTransfer = FALSE; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(targetSpecies) { Ability(targetAbility); HP(500); MaxHP(500); Defense(200); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_TORKOAL) { Ability(ABILITY_SHELL_ARMOR); HP(500); MaxHP(500); Defense(200); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_BUNEARY) { Ability(ABILITY_KLUTZ); Item(ITEM_FLAME_ORB); Moves(MOVE_SWITCHEROO, MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        if (shouldTransfer)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_SWITCHEROO, target: playerLeft); }
        else
            TURN { EXPECT_MOVE(opponentLeft, MOVE_SCRATCH); }
    } THEN {
        EXPECT_EQ(playerLeft->item, shouldTransfer ? ITEM_FLAME_ORB : ITEM_NONE);
        EXPECT_EQ(!!(playerLeft->status1 & STATUS1_BURN), shouldTransfer);
        EXPECT_EQ(opponentLeft->item, shouldTransfer ? ITEM_NONE : ITEM_FLAME_ORB);
        EXPECT(!(opponentLeft->status1 & STATUS1_BURN));
    }
}

AI_DOUBLE_BATTLE_TEST("AI considers status orbs and abilities for Trick/Bestow")
{
    enum Move move = MOVE_NONE;
    enum Item item = ITEM_NONE;
    u16 status = STATUS1_NONE;
    enum Species species = SPECIES_NONE;
    enum Ability ability = ABILITY_NONE;
    u8 turnToTrick = 0;

    PARAMETRIZE { move = MOVE_TRICK;  item = ITEM_TOXIC_ORB; status = STATUS1_NONE;   species = SPECIES_WAILMER; ability = ABILITY_PRESSURE;    turnToTrick = 1; }
    PARAMETRIZE { move = MOVE_TRICK;  item = ITEM_FLAME_ORB; status = STATUS1_NONE;   species = SPECIES_WAILMER; ability = ABILITY_PRESSURE;    turnToTrick = 1; }
    PARAMETRIZE { move = MOVE_BESTOW; item = ITEM_TOXIC_ORB; status = STATUS1_NONE;   species = SPECIES_GLISCOR; ability = ABILITY_POISON_HEAL; turnToTrick = 2; }
    PARAMETRIZE { move = MOVE_BESTOW; item = ITEM_FLAME_ORB; status = STATUS1_NONE;   species = SPECIES_RATTATA; ability = ABILITY_GUTS;        turnToTrick = 2; }
    PARAMETRIZE { move = MOVE_TRICK;  item = ITEM_TOXIC_ORB; status = STATUS1_POISON; species = SPECIES_GLISCOR; ability = ABILITY_POISON_HEAL; turnToTrick = 1; }
    PARAMETRIZE { move = MOVE_TRICK;  item = ITEM_FLAME_ORB; status = STATUS1_BURN;   species = SPECIES_RATTATA; ability = ABILITY_GUTS;        turnToTrick = 1; }

    GIVEN {
        ASSUME(gItemsInfo[ITEM_TOXIC_ORB].holdEffect == HOLD_EFFECT_TOXIC_ORB);
        ASSUME(gItemsInfo[ITEM_FLAME_ORB].holdEffect == HOLD_EFFECT_FLAME_ORB);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(species) { Ability(ability); Item(item); Moves(move, MOVE_SCRATCH); Status1(status); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        if (turnToTrick == 1)
        {
            TURN { EXPECT_MOVE(opponentLeft, move, target: playerLeft); }
        }
        else if (turnToTrick == 2)
        {
            TURN { EXPECT_MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft); }
            TURN { EXPECT_MOVE(opponentLeft, move, target: playerLeft); }
        }
        else
        {
            TURN { EXPECT_MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft); }
        }
    }
}

AI_DOUBLE_BATTLE_TEST("AI gifts Utility Umbrella only when it removes the foe's weather benefit")
{
    enum Species weatherSpecies = SPECIES_NONE, targetSpecies = SPECIES_NONE, attackerSpecies = SPECIES_NONE;
    enum Ability weatherAbility = ABILITY_NONE, targetAbility = ABILITY_NONE, attackerAbility = ABILITY_NONE;
    bool32 expectTrick = FALSE;

    PARAMETRIZE { weatherSpecies = SPECIES_TORKOAL;  weatherAbility = ABILITY_DROUGHT; targetSpecies = SPECIES_EXEGGCUTE; targetAbility = ABILITY_CHLOROPHYLL;      attackerSpecies = SPECIES_PARAS;   attackerAbility = ABILITY_DRY_SKIN; expectTrick = FALSE; }
    PARAMETRIZE { weatherSpecies = SPECIES_TORKOAL;  weatherAbility = ABILITY_DROUGHT; targetSpecies = SPECIES_FOMANTIS;  targetAbility = ABILITY_LEAF_GUARD;       attackerSpecies = SPECIES_WAILMER; attackerAbility = ABILITY_PRESSURE; expectTrick = TRUE; }
    PARAMETRIZE { weatherSpecies = SPECIES_TORKOAL;  weatherAbility = ABILITY_DROUGHT; targetSpecies = SPECIES_CHERRIM;   targetAbility = ABILITY_FLOWER_GIFT;      attackerSpecies = SPECIES_WAILMER; attackerAbility = ABILITY_PRESSURE; expectTrick = TRUE; }
    PARAMETRIZE { weatherSpecies = SPECIES_TORKOAL;  weatherAbility = ABILITY_DROUGHT; targetSpecies = SPECIES_TROPIUS;   targetAbility = ABILITY_SOLAR_POWER;      attackerSpecies = SPECIES_WAILMER; attackerAbility = ABILITY_PRESSURE; expectTrick = TRUE; }
    PARAMETRIZE { weatherSpecies = SPECIES_TORKOAL;  weatherAbility = ABILITY_DROUGHT; targetSpecies = SPECIES_KORAIDON;  targetAbility = ABILITY_ORICHALCUM_PULSE; attackerSpecies = SPECIES_WAILMER; attackerAbility = ABILITY_PRESSURE; expectTrick = TRUE; }
    PARAMETRIZE { weatherSpecies = SPECIES_POLITOED; weatherAbility = ABILITY_DRIZZLE; targetSpecies = SPECIES_LOTAD;     targetAbility = ABILITY_SWIFT_SWIM;       attackerSpecies = SPECIES_PARAS;   attackerAbility = ABILITY_DRY_SKIN; expectTrick = TRUE; }
    PARAMETRIZE { weatherSpecies = SPECIES_POLITOED; weatherAbility = ABILITY_DRIZZLE; targetSpecies = SPECIES_LOTAD;     targetAbility = ABILITY_RAIN_DISH;        attackerSpecies = SPECIES_WAILMER; attackerAbility = ABILITY_PRESSURE; expectTrick = TRUE; }
    PARAMETRIZE { weatherSpecies = SPECIES_POLITOED; weatherAbility = ABILITY_DRIZZLE; targetSpecies = SPECIES_PARAS;     targetAbility = ABILITY_DRY_SKIN;         attackerSpecies = SPECIES_WAILMER; attackerAbility = ABILITY_PRESSURE; expectTrick = TRUE; }
    PARAMETRIZE { weatherSpecies = SPECIES_POLITOED; weatherAbility = ABILITY_DRIZZLE; targetSpecies = SPECIES_WINGULL;   targetAbility = ABILITY_HYDRATION;        attackerSpecies = SPECIES_WAILMER; attackerAbility = ABILITY_PRESSURE; expectTrick = TRUE; }

    GIVEN {
        ASSUME(gItemsInfo[ITEM_UTILITY_UMBRELLA].holdEffect == HOLD_EFFECT_UTILITY_UMBRELLA);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(targetSpecies) { Ability(targetAbility); }
        PLAYER(weatherSpecies) { Ability(weatherAbility); }
        OPPONENT(attackerSpecies) { Ability(attackerAbility); Item(ITEM_UTILITY_UMBRELLA); Moves(MOVE_TRICK, MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        if (expectTrick)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_TRICK, target: playerLeft); }
        else
            TURN { EXPECT_MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI steals Utility Umbrella to handle sun and Dry Skin but keeps its own weather perks")
{
    enum Species weatherSpecies = SPECIES_NONE, targetSpecies = SPECIES_NONE, attackerSpecies = SPECIES_NONE;
    enum Ability weatherAbility = ABILITY_NONE, targetAbility = ABILITY_NONE, attackerAbility = ABILITY_NONE;
    bool32 expectTrick = FALSE;

    PARAMETRIZE { weatherSpecies = SPECIES_TORKOAL;  weatherAbility = ABILITY_DROUGHT; targetSpecies = SPECIES_WAILMER; targetAbility = ABILITY_PRESSURE; attackerSpecies = SPECIES_PARAS;     attackerAbility = ABILITY_DRY_SKIN;         expectTrick = TRUE; }
    PARAMETRIZE { weatherSpecies = SPECIES_TORKOAL;  weatherAbility = ABILITY_DROUGHT; targetSpecies = SPECIES_PARAS;   targetAbility = ABILITY_DRY_SKIN; attackerSpecies = SPECIES_WAILMER;   attackerAbility = ABILITY_PRESSURE;         expectTrick = TRUE; }
    PARAMETRIZE { weatherSpecies = SPECIES_TORKOAL;  weatherAbility = ABILITY_DROUGHT; targetSpecies = SPECIES_WAILMER; targetAbility = ABILITY_PRESSURE; attackerSpecies = SPECIES_EXEGGCUTE; attackerAbility = ABILITY_CHLOROPHYLL;      expectTrick = FALSE; }
    PARAMETRIZE { weatherSpecies = SPECIES_TORKOAL;  weatherAbility = ABILITY_DROUGHT; targetSpecies = SPECIES_WAILMER; targetAbility = ABILITY_PRESSURE; attackerSpecies = SPECIES_CHERRIM;   attackerAbility = ABILITY_FLOWER_GIFT;      expectTrick = FALSE; }
    PARAMETRIZE { weatherSpecies = SPECIES_TORKOAL;  weatherAbility = ABILITY_DROUGHT; targetSpecies = SPECIES_WAILMER; targetAbility = ABILITY_PRESSURE; attackerSpecies = SPECIES_FOMANTIS;  attackerAbility = ABILITY_LEAF_GUARD;       expectTrick = FALSE; }
    PARAMETRIZE { weatherSpecies = SPECIES_TORKOAL;  weatherAbility = ABILITY_DROUGHT; targetSpecies = SPECIES_WAILMER; targetAbility = ABILITY_PRESSURE; attackerSpecies = SPECIES_TROPIUS;   attackerAbility = ABILITY_SOLAR_POWER;      expectTrick = FALSE; }
    PARAMETRIZE { weatherSpecies = SPECIES_TORKOAL;  weatherAbility = ABILITY_DROUGHT; targetSpecies = SPECIES_WAILMER; targetAbility = ABILITY_PRESSURE; attackerSpecies = SPECIES_KORAIDON;  attackerAbility = ABILITY_ORICHALCUM_PULSE; expectTrick = FALSE; }
    PARAMETRIZE { weatherSpecies = SPECIES_POLITOED; weatherAbility = ABILITY_DRIZZLE; targetSpecies = SPECIES_WAILMER; targetAbility = ABILITY_PRESSURE; attackerSpecies = SPECIES_LOTAD;     attackerAbility = ABILITY_SWIFT_SWIM;       expectTrick = FALSE; }
    PARAMETRIZE { weatherSpecies = SPECIES_POLITOED; weatherAbility = ABILITY_DRIZZLE; targetSpecies = SPECIES_WAILMER; targetAbility = ABILITY_PRESSURE; attackerSpecies = SPECIES_LOTAD;     attackerAbility = ABILITY_RAIN_DISH;        expectTrick = FALSE; }
    PARAMETRIZE { weatherSpecies = SPECIES_POLITOED; weatherAbility = ABILITY_DRIZZLE; targetSpecies = SPECIES_WAILMER; targetAbility = ABILITY_PRESSURE; attackerSpecies = SPECIES_WINGULL;   attackerAbility = ABILITY_HYDRATION;        expectTrick = FALSE; }
    PARAMETRIZE { weatherSpecies = SPECIES_POLITOED; weatherAbility = ABILITY_DRIZZLE; targetSpecies = SPECIES_WAILMER; targetAbility = ABILITY_PRESSURE; attackerSpecies = SPECIES_PARAS;     attackerAbility = ABILITY_DRY_SKIN;         expectTrick = FALSE; }

    GIVEN {
        ASSUME(gItemsInfo[ITEM_UTILITY_UMBRELLA].holdEffect == HOLD_EFFECT_UTILITY_UMBRELLA);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(targetSpecies) { Ability(targetAbility); Item(ITEM_UTILITY_UMBRELLA); }
        PLAYER(weatherSpecies) { Ability(weatherAbility); }
        OPPONENT(attackerSpecies) { Ability(attackerAbility); Item(ITEM_NONE); Moves(MOVE_TRICK, MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        if (expectTrick)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_TRICK, target: playerLeft); }
        else
            TURN { EXPECT_MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI treats Harvest as a sun benefit only when a berry is involved")
{
    enum Item targetItem = ITEM_NONE;
    bool32 expectTrick = FALSE;

    PARAMETRIZE { targetItem = ITEM_ORAN_BERRY; expectTrick = TRUE; }
    PARAMETRIZE { targetItem = ITEM_LEFTOVERS;  expectTrick = FALSE; }

    GIVEN {
        ASSUME(gItemsInfo[ITEM_UTILITY_UMBRELLA].holdEffect == HOLD_EFFECT_UTILITY_UMBRELLA);
        ASSUME(GetItemPocket(ITEM_ORAN_BERRY) == POCKET_BERRIES);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_EXEGGCUTE) { Ability(ABILITY_HARVEST); Item(targetItem); }
        PLAYER(SPECIES_TORKOAL) { Ability(ABILITY_DROUGHT); }
        OPPONENT(SPECIES_WAILMER) { Ability(ABILITY_PRESSURE); Item(ITEM_UTILITY_UMBRELLA); Moves(MOVE_TRICK, MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        if (expectTrick)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_TRICK, target: playerLeft); }
        else
            TURN { EXPECT_MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI will not use a status move if partner already chose Helping Hand")
{
    enum Move j;
    enum Move statusMove = MOVE_NONE;

    for (j = MOVE_NONE + 1; j < MOVES_COUNT; j++)
    {
        if (GetMoveCategory(j) == DAMAGE_CATEGORY_STATUS) {
            PARAMETRIZE { statusMove = j; }
        }
    }

    GIVEN {
        WITH_CONFIG(AI_REVERSE_BATTLER_LOGIC_ORDER_CHANCE, 100);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_SCRATCH, statusMove, MOVE_WATER_GUN); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_SCRATCH, statusMove, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_HELPING_HAND, MOVE_EXPLOSION); }
        OPPONENT(SPECIES_BIBAREL) { Moves(MOVE_SCRATCH, statusMove, MOVE_WATER_GUN); Ability(ABILITY_SIMPLE); Item(ITEM_WHITE_HERB); }
    } WHEN {
        TURN {
            EXPECT_MOVE(opponentLeft, MOVE_HELPING_HAND);
            NOT_EXPECT_MOVE(opponentRight, statusMove);
            SCORE_LT_VAL(opponentRight, statusMove, AI_SCORE_DEFAULT, target:playerLeft);
            SCORE_LT_VAL(opponentRight, statusMove, AI_SCORE_DEFAULT, target:playerRight);
            SCORE_LT_VAL(opponentRight, statusMove, AI_SCORE_DEFAULT, target:opponentLeft);
        }
    } SCENE {
        MESSAGE("The opposing Wobbuffet used Helping Hand!");
    }
}

AI_DOUBLE_BATTLE_TEST("AI understands Instruct by repeating its ally's spread attack")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_INSTRUCT) == EFFECT_INSTRUCT);
        ASSUME(IsSpreadMove(GetMoveTarget(MOVE_HEAT_WAVE)));
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT
               | AI_FLAG_OMNISCIENT | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { HP(500); Speed(40); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); Speed(30); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ORANGURU) { Speed(10); Moves(MOVE_INSTRUCT, MOVE_PSYCHIC); }
        OPPONENT(SPECIES_TORKOAL) { Speed(20); SpAttack(100); Moves(MOVE_HEAT_WAVE); }
    } WHEN {
        TURN {
            EXPECT_MOVE(opponentLeft, MOVE_INSTRUCT, target: opponentRight);
            EXPECT_MOVE(opponentRight, MOVE_HEAT_WAVE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("AI avoids Instruct when its faster ally has no last move")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_INSTRUCT) == EFFECT_INSTRUCT);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT
               | AI_FLAG_OMNISCIENT | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { HP(500); Speed(50); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); Speed(40); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ORANGURU) { Speed(30); Moves(MOVE_INSTRUCT, MOVE_PSYCHIC); }
        OPPONENT(SPECIES_TORKOAL) { Speed(20); Moves(MOVE_HEAT_WAVE); }
    } WHEN {
        TURN {
            NOT_EXPECT_MOVE(opponentLeft, MOVE_INSTRUCT);
            SCORE_LT_VAL(opponentLeft, MOVE_INSTRUCT, AI_SCORE_DEFAULT, target: opponentRight);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("AI avoids Instruct when its ally's pending move is banned")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_INSTRUCT) == EFFECT_INSTRUCT);
        ASSUME(IsMoveInstructBanned(MOVE_CELEBRATE));
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT
               | AI_FLAG_OMNISCIENT | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { HP(500); Speed(50); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); Speed(40); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ORANGURU) { Speed(10); Moves(MOVE_INSTRUCT, MOVE_PSYCHIC); }
        OPPONENT(SPECIES_TORKOAL) { Speed(20); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            NOT_EXPECT_MOVE(opponentLeft, MOVE_INSTRUCT);
            SCORE_LT_VAL(opponentLeft, MOVE_INSTRUCT, AI_SCORE_DEFAULT, target: opponentRight);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("AI understands Quick Guard")
{
    u32 reverseOrderChance;

    PARAMETRIZE { reverseOrderChance = 0; }
    PARAMETRIZE { reverseOrderChance = 100; }

    GIVEN {
        WITH_CONFIG(AI_REVERSE_BATTLER_LOGIC_ORDER_CHANCE, reverseOrderChance);
        ASSUME(GetMovePriority(MOVE_QUICK_ATTACK) > 0);
        ASSUME(GetMoveProtectMethod(MOVE_QUICK_GUARD) == PROTECT_QUICK_GUARD);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT
               | AI_FLAG_OMNISCIENT | AI_FLAG_PREDICT_MOVE | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_RATTATA) { Moves(MOVE_QUICK_ATTACK); Status1(STATUS1_TOXIC_POISON); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_RATTATA) { Moves(MOVE_QUICK_GUARD, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_QUICK_ATTACK, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_QUICK_GUARD);
            EXPECT_MOVE(opponentRight, MOVE_CELEBRATE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("AI understands Wide Guard")
{
    u32 reverseOrderChance;

    PARAMETRIZE { reverseOrderChance = 0; }
    PARAMETRIZE { reverseOrderChance = 100; }

    GIVEN {
        WITH_CONFIG(AI_REVERSE_BATTLER_LOGIC_ORDER_CHANCE, reverseOrderChance);
        ASSUME(IsSpreadMove(GetMoveTarget(MOVE_EARTHQUAKE)));
        ASSUME(GetMoveProtectMethod(MOVE_WIDE_GUARD) == PROTECT_WIDE_GUARD);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT
               | AI_FLAG_OMNISCIENT | AI_FLAG_PREDICT_MOVE | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_RATTATA) { Moves(MOVE_EARTHQUAKE); Status1(STATUS1_TOXIC_POISON); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_RATTATA) { Moves(MOVE_WIDE_GUARD, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_EARTHQUAKE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_WIDE_GUARD);
            EXPECT_MOVE(opponentRight, MOVE_CELEBRATE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("AI won't use the same nondamaging move as its partner for no reason")
{
    u32 chance = 0;

    PARAMETRIZE { chance = 0; }
    PARAMETRIZE { chance = 100; }

    enum Move move;
    PARAMETRIZE { move = MOVE_AROMATHERAPY; }
    PARAMETRIZE { move = MOVE_ELECTRIC_TERRAIN; }
    PARAMETRIZE { move = MOVE_FOLLOW_ME; }
    PARAMETRIZE { move = MOVE_GRASSY_TERRAIN; }
    PARAMETRIZE { move = MOVE_GRAVITY; }
    PARAMETRIZE { move = MOVE_HAIL; }
    PARAMETRIZE { move = MOVE_HEAL_BELL; }
    PARAMETRIZE { move = MOVE_LIGHT_SCREEN; }
    PARAMETRIZE { move = MOVE_LUCKY_CHANT; }
    PARAMETRIZE { move = MOVE_MAGIC_ROOM; }
    PARAMETRIZE { move = MOVE_MISTY_TERRAIN; }
    PARAMETRIZE { move = MOVE_MUD_SPORT; }
    PARAMETRIZE { move = MOVE_PSYCHIC_TERRAIN; }
    PARAMETRIZE { move = MOVE_RAIN_DANCE; }
    PARAMETRIZE { move = MOVE_REFLECT; }
    PARAMETRIZE { move = MOVE_SAFEGUARD; }
    PARAMETRIZE { move = MOVE_SANDSTORM; }
    PARAMETRIZE { move = MOVE_SNOWSCAPE; }
    PARAMETRIZE { move = MOVE_SPOTLIGHT; }
    PARAMETRIZE { move = MOVE_STEALTH_ROCK; }
    PARAMETRIZE { move = MOVE_SUNNY_DAY; }
    PARAMETRIZE { move = MOVE_TAILWIND; }
    PARAMETRIZE { move = MOVE_TEETER_DANCE; }
    PARAMETRIZE { move = MOVE_TRICK_ROOM; }
    PARAMETRIZE { move = MOVE_WATER_SPORT; }
    PARAMETRIZE { move = MOVE_COURT_CHANGE; }
    PARAMETRIZE { move = MOVE_PERISH_SONG; }
    PARAMETRIZE { move = MOVE_STICKY_WEB; }
    PARAMETRIZE { move = MOVE_TEATIME; }
    PARAMETRIZE { move = MOVE_WONDER_ROOM; }
    GIVEN {
        WITH_CONFIG(AI_REVERSE_BATTLER_LOGIC_ORDER_CHANCE, chance);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move, MOVE_TACKLE); Status1(STATUS1_BURN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move, MOVE_TACKLE); }
    } WHEN {
        TURN {
            if (chance == 100) {
                EXPECT_MOVE(opponentLeft, move);
                EXPECT_MOVE(opponentRight, MOVE_TACKLE);
            } else {
                EXPECT_MOVE(opponentLeft, MOVE_TACKLE);
                EXPECT_MOVE(opponentRight, move);
            }
        }
    }
}

AI_DOUBLE_BATTLE_TEST("Heal Bell and Jungle Healing skip curing a partner that benefits from burn")
{
    enum Move move;

    PARAMETRIZE { move = MOVE_HEAL_BELL; }
    PARAMETRIZE { move = MOVE_JUNGLE_HEALING; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_HEAL_BELL) == EFFECT_HEAL_BELL);
        ASSUME(GetMoveEffect(MOVE_JUNGLE_HEALING) == EFFECT_JUNGLE_HEALING);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_MEMENTO); Speed(1); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_MEMENTO); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move, MOVE_SCRATCH); Speed(20); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_GUTS); Moves(MOVE_TACKLE); Status1(STATUS1_BURN); MaxHP(200); HP(200); Speed(10); }
    } WHEN {
        TURN {
            NOT_EXPECT_MOVE(opponentLeft, move);
            EXPECT_MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft);
            EXPECT_MOVE(opponentRight, MOVE_TACKLE, target: playerLeft);
            MOVE(playerLeft, MOVE_MEMENTO);
            MOVE(playerRight, MOVE_MEMENTO);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("AI will not choose Earthquake if it damages the partner without a positive effect")
{
    enum Species species;

    PARAMETRIZE { species = SPECIES_CHARIZARD; }
    PARAMETRIZE { species = SPECIES_CHARMANDER; }
    PARAMETRIZE { species = SPECIES_CHIKORITA; }

    GIVEN {
        ASSUME(GetMoveTarget(MOVE_EARTHQUAKE) == TARGET_FOES_AND_ALLY);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_PHANPY) { Moves(MOVE_EARTHQUAKE, MOVE_SCRATCH); }
        OPPONENT(species) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        if (species == SPECIES_CHARIZARD)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_EARTHQUAKE); }
        else
            TURN { EXPECT_MOVE(opponentLeft, MOVE_SCRATCH, target: playerLeft); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI will not choose Earthquake if its ally has Levitate but both foes are immune to Ground")
{
    GIVEN {
        ASSUME(IsSpeciesOfType(SPECIES_CHARIZARD, TYPE_FLYING));
        ASSUME(IsSpeciesOfType(SPECIES_ZAPDOS, TYPE_FLYING));
        ASSUME(GetMoveTarget(MOVE_EARTHQUAKE) == TARGET_FOES_AND_ALLY);
        ASSUME(GetMoveType(MOVE_EARTHQUAKE) == TYPE_GROUND);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_CHARIZARD);
        PLAYER(SPECIES_ZAPDOS);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_EARTHQUAKE, MOVE_SCRATCH); }
        OPPONENT(SPECIES_KOFFING) { Ability(ABILITY_LEVITATE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_EARTHQUAKE); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI only rewards spread moves when an ally's absorbing ability provides a useful benefit")
{
    enum Ability ability;
    enum Move move, partnerMove;
    u32 partnerHP;
    bool32 shouldReward;

    PARAMETRIZE { ability = ABILITY_LEVITATE;      move = MOVE_EARTHQUAKE; partnerMove = MOVE_CELEBRATE; partnerHP = 100; shouldReward = FALSE; }
    PARAMETRIZE { ability = ABILITY_EARTH_EATER;   move = MOVE_EARTHQUAKE; partnerMove = MOVE_CELEBRATE; partnerHP = 100; shouldReward = FALSE; }
    PARAMETRIZE { ability = ABILITY_EARTH_EATER;   move = MOVE_EARTHQUAKE; partnerMove = MOVE_CELEBRATE; partnerHP = 20;  shouldReward = TRUE; }
    PARAMETRIZE { ability = ABILITY_VOLT_ABSORB;   move = MOVE_DISCHARGE;  partnerMove = MOVE_CELEBRATE; partnerHP = 100; shouldReward = FALSE; }
    PARAMETRIZE { ability = ABILITY_VOLT_ABSORB;   move = MOVE_DISCHARGE;  partnerMove = MOVE_CELEBRATE; partnerHP = 20;  shouldReward = TRUE; }
    PARAMETRIZE { ability = ABILITY_WATER_ABSORB;  move = MOVE_SURF;       partnerMove = MOVE_CELEBRATE; partnerHP = 100; shouldReward = FALSE; }
    PARAMETRIZE { ability = ABILITY_WATER_ABSORB;  move = MOVE_SURF;       partnerMove = MOVE_CELEBRATE; partnerHP = 20;  shouldReward = TRUE; }
    PARAMETRIZE { ability = ABILITY_FLASH_FIRE;    move = MOVE_LAVA_PLUME; partnerMove = MOVE_CELEBRATE; partnerHP = 100; shouldReward = FALSE; }
    PARAMETRIZE { ability = ABILITY_FLASH_FIRE;    move = MOVE_LAVA_PLUME; partnerMove = MOVE_EMBER;     partnerHP = 100; shouldReward = TRUE; }
    PARAMETRIZE { ability = ABILITY_LIGHTNING_ROD; move = MOVE_DISCHARGE;  partnerMove = MOVE_CELEBRATE; partnerHP = 100; shouldReward = FALSE; }
    PARAMETRIZE { ability = ABILITY_LIGHTNING_ROD; move = MOVE_DISCHARGE;  partnerMove = MOVE_EMBER;     partnerHP = 100; shouldReward = TRUE; }
    PARAMETRIZE { ability = ABILITY_STORM_DRAIN;   move = MOVE_SURF;       partnerMove = MOVE_CELEBRATE; partnerHP = 100; shouldReward = FALSE; }
    PARAMETRIZE { ability = ABILITY_STORM_DRAIN;   move = MOVE_SURF;       partnerMove = MOVE_EMBER;     partnerHP = 100; shouldReward = TRUE; }

    GIVEN {
        ASSUME(GetMoveTarget(move) == TARGET_FOES_AND_ALLY);
        ASSUME(GetMoveEffect(MOVE_DRAGON_RAGE) == EFFECT_FIXED_HP_DAMAGE);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_DRAGON_RAGE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(90); Moves(move, MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); HP(partnerHP); MaxHP(100); Speed(100); Moves(partnerMove); }
    } WHEN {
        if (shouldReward)
            TURN { SCORE_GT_VAL(opponentLeft, move, AI_SCORE_DEFAULT, target: opponentRight); }
        else
            TURN { SCORE_LT_VAL(opponentLeft, move, AI_SCORE_DEFAULT, target: opponentRight); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI recognizes its ally's Telepathy")
{
    GIVEN {
        ASSUME(GetMoveTarget(MOVE_EARTHQUAKE) == TARGET_FOES_AND_ALLY);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_PHANPY) { Moves(MOVE_EARTHQUAKE, MOVE_SCRATCH); }
        OPPONENT(SPECIES_ELGYEM) { Level(1); Ability(ABILITY_TELEPATHY); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_EARTHQUAKE); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI will choose Bulldoze if it triggers its ally's ability but will not KO the ally needlessly")
{
    enum Species species;
    enum Ability ability;
    u32 currentHP;

    PARAMETRIZE { species = SPECIES_KINGAMBIT; ability = ABILITY_DEFIANT;  currentHP = 400; }
    PARAMETRIZE { species = SPECIES_SHUCKLE;   ability = ABILITY_CONTRARY; currentHP = 400; }
    PARAMETRIZE { species = SPECIES_PAWNIARD;  ability = ABILITY_PRESSURE; currentHP = 1; }
    PARAMETRIZE { species = SPECIES_PAWNIARD;  ability = ABILITY_DEFIANT;  currentHP = 1; }
    PARAMETRIZE { species = SPECIES_SHUCKLE;   ability = ABILITY_CONTRARY; currentHP = 1; }

    GIVEN {
        ASSUME(GetMoveTarget(MOVE_BULLDOZE) == TARGET_FOES_AND_ALLY);
        ASSUME(GetMoveType(MOVE_BULLDOZE) == TYPE_GROUND);
        ASSUME_MOVE_EFFECT_STAT_CHANGE(MOVE_BULLDOZE, speed: -1);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_PHANPY) { Moves(MOVE_BULLDOZE, MOVE_HIGH_HORSEPOWER); }
        OPPONENT(species) { Moves(MOVE_CELEBRATE, MOVE_POUND); HP(currentHP); Ability(ability); }
    } WHEN {
        if (currentHP != 1)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_BULLDOZE); }
        else
            TURN { EXPECT_MOVE(opponentLeft, MOVE_HIGH_HORSEPOWER); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI uses Spicy Extract if its ally holds Clear Amulet and has a physical move")
{
    enum Move move;

    PARAMETRIZE { move = MOVE_SCRATCH; }
    PARAMETRIZE { move = MOVE_SWIFT; }

    GIVEN {
        ASSUME_STAT_CHANGE(MOVE_SPICY_EXTRACT, attack: +2, defense: -2);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(20); Item(ITEM_CLEAR_AMULET); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(40); Moves(MOVE_SCRATCH, MOVE_SPICY_EXTRACT); }
    } WHEN {
        TURN {
            if (move == MOVE_SCRATCH)
                EXPECT_MOVE(opponentRight, MOVE_SPICY_EXTRACT, target: opponentLeft);
            else
                EXPECT_MOVE(opponentRight, MOVE_SCRATCH);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("AI does not use Spicy Extract if its ally would not benefit")
{
    u32 species;
    enum Ability ability;

    PARAMETRIZE { species = SPECIES_GHOLDENGO; ability = ABILITY_GOOD_AS_GOLD; }
    PARAMETRIZE { species = SPECIES_SNIVY;     ability = ABILITY_CONTRARY; }

    GIVEN {
        ASSUME_STAT_CHANGE(MOVE_SPICY_EXTRACT, attack: +2, defense: -2);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_SCRATCH); }
        OPPONENT(species) { Speed(20); Ability(ability); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(40); Moves(MOVE_SCRATCH, MOVE_SPICY_EXTRACT); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentRight, MOVE_SCRATCH); }
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: reciprocal Justified activation preserves the recipient's attack")
{
    bool32 reverse;
    enum Ability abilityAtk, abilityDef;
    u32 hp;
    PARAMETRIZE { reverse = FALSE; abilityAtk = ABILITY_SCRAPPY; abilityDef = ABILITY_JUSTIFIED; hp = 400; }
    PARAMETRIZE { reverse = TRUE; abilityAtk = ABILITY_SCRAPPY; abilityDef = ABILITY_JUSTIFIED; hp = 400; }
    PARAMETRIZE { reverse = FALSE; abilityAtk = ABILITY_MOLD_BREAKER; abilityDef = ABILITY_JUSTIFIED; hp = 400; }
    PARAMETRIZE { reverse = FALSE; abilityAtk = ABILITY_SCRAPPY; abilityDef = ABILITY_FLASH_FIRE; hp = 400; }
    PARAMETRIZE { reverse = FALSE; abilityAtk = ABILITY_SCRAPPY; abilityDef = ABILITY_JUSTIFIED; hp = 1; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(400); MaxHP(400); Defense(100); Attack(1); Speed(100); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_CLEFABLE) { Level(50); HP(400); MaxHP(400); Defense(100); Attack(1); Speed(90); Moves(MOVE_SCRATCH); }
        if (!reverse) {
            OPPONENT(SPECIES_PANGORO) { Level(50); HP(400); MaxHP(400); Attack(20); Defense(400); Speed(300); Ability(abilityAtk); Moves(MOVE_BEAT_UP); }
            OPPONENT(SPECIES_GROWLITHE) { Level(50); HP(hp); MaxHP(400); Attack(180); Defense(400); Speed(200); Ability(abilityDef); Moves(MOVE_PROTECT, MOVE_TACKLE); }
        } else {
            OPPONENT(SPECIES_GROWLITHE) { Level(50); HP(hp); MaxHP(400); Attack(180); Defense(400); Speed(200); Ability(abilityDef); Moves(MOVE_PROTECT, MOVE_TACKLE); }
            OPPONENT(SPECIES_PANGORO) { Level(50); HP(400); MaxHP(400); Attack(20); Defense(400); Speed(300); Ability(abilityAtk); Moves(MOVE_BEAT_UP); }
        }
    } WHEN {
        struct BattlePokemon *activator = reverse ? opponentRight : opponentLeft;
        struct BattlePokemon *recipient = reverse ? opponentLeft : opponentRight;
        TURN {
            MOVE(playerLeft, MOVE_SCRATCH, target: opponentLeft);
            MOVE(playerRight, MOVE_SCRATCH, target: opponentRight);
            if (hp > 1 && abilityDef == ABILITY_JUSTIFIED && abilityAtk != ABILITY_MOLD_BREAKER) {
                EXPECT_MOVE(activator, MOVE_BEAT_UP, target: recipient);
                EXPECT_MOVE(recipient, MOVE_TACKLE);
            } else {
                EXPECT_MOVE(activator, MOVE_BEAT_UP, target: playerLeft);
            }
        }
    } THEN {
        struct BattlePokemon *recipient = reverse ? opponentLeft : opponentRight;
        if (hp > 1 && abilityDef == ABILITY_JUSTIFIED && abilityAtk != ABILITY_MOLD_BREAKER)
            EXPECT_EQ(recipient->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

AI_DOUBLE_BATTLE_TEST("AI does not penalize Protect if its ally switches instead of triggering Weakness Policy")
{
    ASSUME(GetMoveTarget(MOVE_EARTHQUAKE) == TARGET_FOES_AND_ALLY);
    ASSUME(GetMoveType(MOVE_EARTHQUAKE) == TYPE_GROUND);
    ASSUME(GetItemHoldEffect(ITEM_WEAKNESS_POLICY) == HOLD_EFFECT_WEAKNESS_POLICY);

    GIVEN {
        WITH_CONFIG(AI_REVERSE_BATTLER_LOGIC_ORDER_CHANCE, 0);
        WITH_CONFIG(SHOULD_SWITCH_ALL_MOVES_BAD_PERCENTAGE, 100);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_CHARIZARD) { Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_CHARIZARD) { Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_GIBLE)   { Level(1); Attack(1); Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_PIKACHU) { Level(100); HP(400); Defense(400); Item(ITEM_WEAKNESS_POLICY); Moves(MOVE_PROTECT, MOVE_TACKLE); }
        OPPONENT(SPECIES_RAMPARDOS) { Level(100); Moves(MOVE_ROCK_SLIDE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SCRATCH, target: opponentLeft);
            MOVE(playerRight, MOVE_SCRATCH, target: opponentRight);
            EXPECT_SWITCH(opponentLeft, 2);
            SCORE_GT_VAL(opponentRight, MOVE_PROTECT, AI_SCORE_DEFAULT + WORST_EFFECT, target: playerRight);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("AI will choose Beat Up on an ally with Rage Fist if it will benefit the ally")
{
    u32 currentHP, movePP;
    enum Move move;
    bool32 shouldBeatUp;

    PARAMETRIZE { move = MOVE_DRAIN_PUNCH; currentHP = 400; movePP = 10; shouldBeatUp = FALSE; }
    PARAMETRIZE { move = MOVE_RAGE_FIST;   currentHP = 400; movePP = 10; shouldBeatUp = TRUE; }
    PARAMETRIZE { move = MOVE_RAGE_FIST;   currentHP = 400; movePP = 0;  shouldBeatUp = FALSE; }
    PARAMETRIZE { move = MOVE_RAGE_FIST;   currentHP = 1;   movePP = 10; shouldBeatUp = FALSE; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_BEAT_UP) == EFFECT_BEAT_UP);
        ASSUME(GetMoveEffect(MOVE_RAGE_FIST) == EFFECT_RAGE_FIST);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WHIMSICOTT) { Moves(MOVE_BEAT_UP); }
        OPPONENT(SPECIES_ANNIHILAPE) { MovesWithPP({move, movePP}, {MOVE_CELEBRATE, 10}); HP(currentHP); }
    } WHEN {
        if (shouldBeatUp)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_BEAT_UP, target: opponentRight); }
        else
            TURN { EXPECT_MOVE(opponentLeft, MOVE_BEAT_UP, target: playerLeft); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI will only Beat Up for Rage Fist if it can hit at least one opponent")
{
    enum Species species;
    bool32 shouldBeatUp;

    PARAMETRIZE { species = SPECIES_MEOWTH;    shouldBeatUp = FALSE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; shouldBeatUp = TRUE; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_BEAT_UP) == EFFECT_BEAT_UP);
        ASSUME(GetMoveEffect(MOVE_RAGE_FIST) == EFFECT_RAGE_FIST);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_MEOWTH);
        PLAYER(species);
        OPPONENT(SPECIES_WHIMSICOTT) { Moves(MOVE_BEAT_UP); }
        OPPONENT(SPECIES_ANNIHILAPE) { MovesWithPP({MOVE_RAGE_FIST, 10}, {MOVE_CELEBRATE, 10}); HP(400); }
    } WHEN {
        if (shouldBeatUp)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_BEAT_UP, target: opponentRight); }
        else
            TURN { EXPECT_MOVE(opponentLeft, MOVE_BEAT_UP, target: playerLeft); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI will choose Earthquake if partner is not alive")
{
    GIVEN {
        ASSUME(GetMoveTarget(MOVE_EARTHQUAKE) == TARGET_FOES_AND_ALLY);
        ASSUME(GetMoveType(MOVE_EARTHQUAKE) == TYPE_GROUND);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_EARTHQUAKE, MOVE_SCRATCH); }
        OPPONENT(SPECIES_PIKACHU) { HP(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SCRATCH, target: opponentRight); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_EARTHQUAKE); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI will choose Earthquake if it kills one opposing mon and does not kill the partner needlessly")
{
    u32 currentHP;
    PARAMETRIZE { currentHP = 1; }
    PARAMETRIZE { currentHP = 200; }

    GIVEN {
        ASSUME(GetMoveTarget(MOVE_EARTHQUAKE) == TARGET_FOES_AND_ALLY);
        ASSUME(GetMoveType(MOVE_EARTHQUAKE) == TYPE_GROUND);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_EARTHQUAKE, MOVE_SCRATCH); }
        OPPONENT(SPECIES_PARAS) { Moves(MOVE_CELEBRATE); HP(currentHP); }
    } WHEN {
        if (currentHP == 1)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_SCRATCH); }
        else
            TURN { EXPECT_MOVE(opponentLeft, MOVE_EARTHQUAKE); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI will choose Earthquake if it kills one opposing mon and a partner it believes is about to die")
{
    GIVEN {
        ASSUME(GetMoveTarget(MOVE_EARTHQUAKE) == TARGET_FOES_AND_ALLY);
        ASSUME(GetMoveType(MOVE_EARTHQUAKE) == TYPE_GROUND);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_SCRATCH); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Moves(MOVE_CELEBRATE, MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_EARTHQUAKE, MOVE_SCRATCH); }
        OPPONENT(SPECIES_PARAS) { Moves(MOVE_CELEBRATE); HP(1); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SCRATCH); }
        TURN { MOVE(playerRight, MOVE_SCRATCH); }
        TURN { EXPECT_MOVE(opponentLeft, MOVE_EARTHQUAKE); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI will choose Earthquake if it kills both opposing mons")
{
    GIVEN {
        ASSUME(GetMoveTarget(MOVE_EARTHQUAKE) == TARGET_FOES_AND_ALLY);
        ASSUME(GetMoveType(MOVE_EARTHQUAKE) == TYPE_GROUND);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_EARTHQUAKE, MOVE_SCRATCH); }
        OPPONENT(SPECIES_PARAS) { Moves(MOVE_CELEBRATE); HP(1); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_EARTHQUAKE); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI will trigger its ally's Weakness Policy")
{
    enum Species species;
    PARAMETRIZE { species = SPECIES_INCINEROAR; }
    PARAMETRIZE { species = SPECIES_CLEFFA; }

    GIVEN {
        ASSUME(gItemsInfo[ITEM_WEAKNESS_POLICY].holdEffect == HOLD_EFFECT_WEAKNESS_POLICY);
        ASSUME(GetMoveTarget(MOVE_EARTHQUAKE) == TARGET_FOES_AND_ALLY);
        ASSUME(GetMoveType(MOVE_EARTHQUAKE) == TYPE_GROUND);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_EARTHQUAKE, MOVE_STOMPING_TANTRUM); }
        OPPONENT(species) { Moves(MOVE_CELEBRATE); Item(ITEM_WEAKNESS_POLICY); }
    } WHEN {
        if (species == SPECIES_INCINEROAR)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_EARTHQUAKE); }
        else
            TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_EARTHQUAKE); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI will trigger its ally's Spicy Spray if burn benefits it")
{
    enum Ability atkAbility;
    bool32 shouldTrigger;

    PARAMETRIZE { atkAbility = ABILITY_GUTS;  shouldTrigger = TRUE;  }
    PARAMETRIZE { atkAbility = ABILITY_SWARM; shouldTrigger = FALSE; }

    GIVEN {
        ASSUME(GetMoveTarget(MOVE_EARTHQUAKE) == TARGET_FOES_AND_ALLY);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_HERACROSS) { Ability(atkAbility); Moves(MOVE_EARTHQUAKE, MOVE_SCRATCH); }
        OPPONENT(SPECIES_SCOVILLAIN_MEGA) { Ability(ABILITY_SPICY_SPRAY); Moves(MOVE_CELEBRATE); MaxHP(400); HP(400); }
    } WHEN {
        if (shouldTrigger)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_EARTHQUAKE); }
        else
            TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_EARTHQUAKE); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI will trigger its ally's Spicy Spray if the ally is about to faint")
{
    enum Ability atkAbility;
    bool32 shouldTrigger;

    PARAMETRIZE { atkAbility = ABILITY_GUTS;  shouldTrigger = TRUE;  }
    PARAMETRIZE { atkAbility = ABILITY_SWARM; shouldTrigger = FALSE; }

    GIVEN {
        ASSUME(GetMoveTarget(MOVE_EARTHQUAKE) == TARGET_FOES_AND_ALLY);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_HERACROSS) { Ability(atkAbility); Moves(MOVE_EARTHQUAKE, MOVE_SCRATCH); }
        OPPONENT(SPECIES_SCOVILLAIN_MEGA) { Ability(ABILITY_SPICY_SPRAY); Moves(MOVE_CELEBRATE); MaxHP(400); HP(1); }
    } WHEN {
        if (shouldTrigger)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_EARTHQUAKE); }
        else
            TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_EARTHQUAKE); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI will only explode and kill everything on the field with Risky or Will Suicide (doubles)")
{
    u32 aiFlags;

    PARAMETRIZE { aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT; }
    PARAMETRIZE { aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_RISKY; }
    PARAMETRIZE { aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_WILL_SUICIDE; }

    GIVEN {
        ASSUME(GetMoveTarget(MOVE_EXPLOSION) == TARGET_FOES_AND_ALLY);
        ASSUME(IsExplosionMove(MOVE_EXPLOSION));
        AI_FLAGS(aiFlags);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_ELECTRODE) { Moves(MOVE_EXPLOSION, MOVE_ELECTRO_BALL); }
        OPPONENT(SPECIES_ELECTRODE) { Moves(MOVE_CELEBRATE); HP(1); }
    } WHEN {
        if (aiFlags == (AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT))
            TURN { EXPECT_MOVE(opponentLeft, MOVE_ELECTRO_BALL); }
        else
            TURN { EXPECT_MOVE(opponentLeft, MOVE_EXPLOSION); }
    }
}

AI_DOUBLE_BATTLE_TEST("Battler 3 has Battler 1 AI flags set correctly (doubles)")
{
    u32 aiFlags;
    struct BattlePokemon *battler = NULL;

    PARAMETRIZE { aiFlags = 0; battler = opponentLeft; }
    PARAMETRIZE { aiFlags = 0; battler = opponentRight; }
    PARAMETRIZE { aiFlags = AI_FLAG_RISKY; battler = opponentRight; }
    PARAMETRIZE { aiFlags = AI_FLAG_RISKY; battler = opponentLeft; }
    PARAMETRIZE { aiFlags = AI_FLAG_WILL_SUICIDE; battler = opponentLeft; }
    PARAMETRIZE { aiFlags = AI_FLAG_WILL_SUICIDE; battler = opponentRight; }

    GIVEN {
        ASSUME(GetMoveTarget(MOVE_EXPLOSION) == TARGET_FOES_AND_ALLY);
        ASSUME(IsExplosionMove(MOVE_EXPLOSION));
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        BATTLER_AI_FLAGS(battler, aiFlags);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_VOLTORB) { Moves(MOVE_EXPLOSION, MOVE_ELECTRO_BALL); HP(1); }
        OPPONENT(SPECIES_ELECTRODE) { Moves(MOVE_EXPLOSION, MOVE_ELECTRO_BALL); HP(1); }
    } WHEN {
        if (aiFlags == 0 || battler == opponentRight)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_ELECTRO_BALL, target: playerLeft); EXPECT_MOVE(opponentRight, MOVE_ELECTRO_BALL, target: playerLeft); }
        else
            TURN { EXPECT_MOVE(opponentLeft, MOVE_EXPLOSION, target: playerLeft); EXPECT_MOVE(opponentRight, MOVE_EXPLOSION); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI sees corresponding absorbing abilities on partners")
{
    enum Ability ability;
    enum Move move;
    enum Species species;

    PARAMETRIZE { species = SPECIES_PSYDUCK;    ability = ABILITY_CLOUD_NINE;         move = MOVE_DISCHARGE; }
    PARAMETRIZE { species = SPECIES_PIKACHU;    ability = ABILITY_LIGHTNING_ROD;      move = MOVE_DISCHARGE; }
    PARAMETRIZE { species = SPECIES_LANTURN;    ability = ABILITY_VOLT_ABSORB;        move = MOVE_DISCHARGE; }
    PARAMETRIZE { species = SPECIES_EMOLGA;     ability = ABILITY_MOTOR_DRIVE;        move = MOVE_DISCHARGE; }
    PARAMETRIZE { species = SPECIES_SEAKING;    ability = ABILITY_LIGHTNING_ROD;      move = MOVE_DISCHARGE; }
    PARAMETRIZE { species = SPECIES_GROWLITHE;  ability = ABILITY_FLASH_FIRE;         move = MOVE_LAVA_PLUME; }
    PARAMETRIZE { species = SPECIES_DACHSBUN;   ability = ABILITY_WELL_BAKED_BODY;    move = MOVE_LAVA_PLUME; }
    PARAMETRIZE { species = SPECIES_QUAGSIRE;   ability = ABILITY_WATER_ABSORB;       move = MOVE_SURF; }
    PARAMETRIZE { species = SPECIES_SHELLOS;    ability = ABILITY_STORM_DRAIN;        move = MOVE_SURF; }
    PARAMETRIZE { species = SPECIES_UNOWN;      ability = ABILITY_LEVITATE;           move = MOVE_EARTHQUAKE; }
    PARAMETRIZE { species = SPECIES_ORTHWORM;   ability = ABILITY_EARTH_EATER;        move = MOVE_EARTHQUAKE; }

    GIVEN {
        ASSUME(GetMoveTarget(MOVE_DISCHARGE) == TARGET_FOES_AND_ALLY);
        ASSUME(GetMoveType(MOVE_DISCHARGE) == TYPE_ELECTRIC);
        ASSUME(GetMoveTarget(MOVE_LAVA_PLUME) == TARGET_FOES_AND_ALLY);
        ASSUME(GetMoveType(MOVE_LAVA_PLUME) == TYPE_FIRE);
        ASSUME(GetMoveTarget(MOVE_SURF) == TARGET_FOES_AND_ALLY);
        ASSUME(GetMoveType(MOVE_SURF) == TYPE_WATER);
        ASSUME(GetMoveTarget(MOVE_EARTHQUAKE) == TARGET_FOES_AND_ALLY);
        ASSUME(GetMoveType(MOVE_EARTHQUAKE) == TYPE_GROUND);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_ZIGZAGOON);
        PLAYER(SPECIES_ZIGZAGOON);
        OPPONENT(SPECIES_SLAKING) { Moves(move, MOVE_CONSTRICT); }
        OPPONENT(species) { HP(1); Ability(ability); Moves(MOVE_POUND, MOVE_EMBER, MOVE_ROUND); }
    } WHEN {
        if (ability != ABILITY_CLOUD_NINE)
            TURN { EXPECT_MOVE(opponentLeft, move); }
        else
            TURN { EXPECT_MOVE(opponentLeft, MOVE_CONSTRICT); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI treats an ally's redirection ability appropriately (gen 5+)")
{
    enum Move move, expectedMove;
    enum Species species;
    u32 config;
    enum Ability ability;

    PARAMETRIZE { species = SPECIES_SEAKING; ability = ABILITY_LIGHTNING_ROD; move = MOVE_DISCHARGE; config = GEN_5; expectedMove = MOVE_DISCHARGE; }
    PARAMETRIZE { species = SPECIES_SHELLOS; ability = ABILITY_STORM_DRAIN;   move = MOVE_SURF;      config = GEN_5; expectedMove = MOVE_SURF; }

    GIVEN {
        ASSUME(GetMoveTarget(MOVE_DISCHARGE) == TARGET_FOES_AND_ALLY);
        ASSUME(GetMoveType(MOVE_DISCHARGE) == TYPE_ELECTRIC);
        ASSUME(GetMoveTarget(MOVE_SURF) == TARGET_FOES_AND_ALLY);
        ASSUME(GetMoveType(MOVE_SURF) == TYPE_WATER);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_HP_AWARE);
        WITH_CONFIG(B_REDIRECT_ABILITY_IMMUNITY, config);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move, MOVE_HEADBUTT); }
        OPPONENT(species) { HP(1); Ability(ability); Moves(MOVE_ROUND); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, expectedMove); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI recognizes Volt Absorb received from Trace")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT);
        PLAYER(SPECIES_MAGNETON);
        PLAYER(SPECIES_GARDEVOIR) { Ability(ABILITY_TRACE); }
        OPPONENT(SPECIES_JOLTEON) { Ability(ABILITY_VOLT_ABSORB); Moves(MOVE_THUNDER_WAVE, MOVE_THUNDERSHOCK, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_JOLTEON) { Ability(ABILITY_VOLT_ABSORB); Moves(MOVE_THUNDER_WAVE, MOVE_THUNDERSHOCK, MOVE_WATER_GUN); }
    } WHEN {
        TURN { NOT_EXPECT_MOVES(opponentLeft, MOVE_THUNDERSHOCK, MOVE_THUNDER_WAVE); NOT_EXPECT_MOVE(opponentRight, MOVE_THUNDER_WAVE); }
    } THEN {
        EXPECT(gAiLogicData->abilities[B_POSITION_PLAYER_RIGHT] == ABILITY_VOLT_ABSORB);
    }
}

AI_DOUBLE_BATTLE_TEST("AI prioritizes Skill Swapping Contrary to allied mons that would benefit from it")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_SKILL_SWAP) == EFFECT_SKILL_SWAP);
        ASSUME_MOVE_EFFECT_STAT_CHANGE(MOVE_OVERHEAT, self: TRUE, spAtk: -2);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Speed(3); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(3); }
        OPPONENT(SPECIES_SPINDA) { Ability(ABILITY_CONTRARY); Speed(5); Moves(MOVE_SKILL_SWAP, MOVE_ENCORE, MOVE_FAKE_TEARS, MOVE_SWAGGER); }
        OPPONENT(SPECIES_ARCANINE) { Ability(ABILITY_INTIMIDATE); Speed(4); Moves (MOVE_OVERHEAT); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_SKILL_SWAP, target:opponentRight); EXPECT_MOVE(opponentRight, MOVE_OVERHEAT); }
    }
}

// These weather actions improve the partner's actual attack. The old Shore Up
// parameter was invalid: a full-HP partner gains no healing from Sandstorm.
AI_DOUBLE_BATTLE_TEST("AI sets up weather for its ally")
{
    u32 goodWeather, badWeather, weatherTrigger;
    u64 aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT;

    PARAMETRIZE { goodWeather = MOVE_SUNNY_DAY; badWeather = MOVE_RAIN_DANCE; weatherTrigger = MOVE_SOLAR_BEAM; }
    PARAMETRIZE { goodWeather = MOVE_RAIN_DANCE; badWeather = MOVE_SUNNY_DAY; weatherTrigger = MOVE_THUNDER; }
    PARAMETRIZE { goodWeather = MOVE_HAIL; badWeather = MOVE_SUNNY_DAY; weatherTrigger = MOVE_BLIZZARD; }
    PARAMETRIZE { goodWeather = MOVE_SNOWSCAPE; badWeather = MOVE_SUNNY_DAY; weatherTrigger = MOVE_BLIZZARD; }
    PARAMETRIZE { aiFlags |= AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION;
                  goodWeather = MOVE_SUNNY_DAY; badWeather = MOVE_RAIN_DANCE; weatherTrigger = MOVE_SOLAR_BEAM; }
    PARAMETRIZE { aiFlags |= AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION;
                  goodWeather = MOVE_RAIN_DANCE; badWeather = MOVE_SUNNY_DAY; weatherTrigger = MOVE_THUNDER; }
    PARAMETRIZE { aiFlags |= AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION;
                  goodWeather = MOVE_HAIL; badWeather = MOVE_SUNNY_DAY; weatherTrigger = MOVE_BLIZZARD; }
    PARAMETRIZE { aiFlags |= AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION;
                  goodWeather = MOVE_SNOWSCAPE; badWeather = MOVE_SUNNY_DAY; weatherTrigger = MOVE_BLIZZARD; }

    GIVEN {
        AI_FLAGS(aiFlags);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_TORNADUS) { Item(ITEM_SAFETY_GOGGLES); Ability(ABILITY_PRANKSTER); Moves(goodWeather, badWeather, MOVE_RETURN, MOVE_TAUNT); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_SAFETY_GOGGLES); Moves(weatherTrigger, MOVE_EARTH_POWER); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, goodWeather); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI sets up terrain for its ally")
{
    u32 goodTerrain, badTerrain, terrainTrigger;
    u64 aiFlags = AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT;

    PARAMETRIZE { goodTerrain = MOVE_ELECTRIC_TERRAIN; badTerrain = MOVE_PSYCHIC_TERRAIN; terrainTrigger = MOVE_RISING_VOLTAGE; }
    PARAMETRIZE { goodTerrain = MOVE_GRASSY_TERRAIN; badTerrain = MOVE_PSYCHIC_TERRAIN; terrainTrigger = MOVE_GRASSY_GLIDE; }
    PARAMETRIZE { goodTerrain = MOVE_MISTY_TERRAIN; badTerrain = MOVE_PSYCHIC_TERRAIN; terrainTrigger = MOVE_MISTY_EXPLOSION; }
    PARAMETRIZE { goodTerrain = MOVE_PSYCHIC_TERRAIN; badTerrain = MOVE_ELECTRIC_TERRAIN; terrainTrigger = MOVE_EXPANDING_FORCE; }
    PARAMETRIZE { aiFlags |= AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION;
                  goodTerrain = MOVE_ELECTRIC_TERRAIN; badTerrain = MOVE_PSYCHIC_TERRAIN; terrainTrigger = MOVE_RISING_VOLTAGE; }
    PARAMETRIZE { aiFlags |= AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION;
                  goodTerrain = MOVE_GRASSY_TERRAIN; badTerrain = MOVE_PSYCHIC_TERRAIN; terrainTrigger = MOVE_GRASSY_GLIDE; }
    PARAMETRIZE { aiFlags |= AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION;
                  goodTerrain = MOVE_MISTY_TERRAIN; badTerrain = MOVE_PSYCHIC_TERRAIN; terrainTrigger = MOVE_MISTY_EXPLOSION; }
    PARAMETRIZE { aiFlags |= AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION;
                  goodTerrain = MOVE_PSYCHIC_TERRAIN; badTerrain = MOVE_ELECTRIC_TERRAIN; terrainTrigger = MOVE_EXPANDING_FORCE; }

    GIVEN {
        AI_FLAGS(aiFlags);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(goodTerrain, badTerrain, MOVE_RETURN, MOVE_TAUNT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(terrainTrigger, MOVE_EARTH_POWER); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, goodTerrain); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI terrain decisions preserve the setter's opinion when its ally disagrees")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_ELECTRIC_TERRAIN) == EFFECT_TERRAIN);
        ASSUME(GetMoveEffect(MOVE_GRASSY_TERRAIN) == EFFECT_TERRAIN);
        ASSUME(GetMoveEffect(MOVE_RISING_VOLTAGE) == EFFECT_TERRAIN_BOOST);
        TIE_BREAK_SCORE(RNG_AI_SCORE_TIE_DOUBLES_MOVE, SCORE_TIE_CHOSEN, 1);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_RISING_VOLTAGE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ELECTRIC_TERRAIN, MOVE_GRASSY_TERRAIN, MOVE_ENERGY_BALL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_RISING_VOLTAGE, MOVE_TACKLE); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_GRASSY_TERRAIN); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI uses After You to set up Trick Room")
{
    enum Move move;

    PARAMETRIZE { move = MOVE_TRICK_ROOM; }
    PARAMETRIZE { move = MOVE_MOONBLAST; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_AFTER_YOU) == EFFECT_AFTER_YOU);
        ASSUME(GetMoveEffect(MOVE_TRICK_ROOM) == EFFECT_TRICK_ROOM);
        ASSUME(IsHealingMove(MOVE_DRAINING_KISS)); // Doesn't have the Healing Move flag in Gen 5
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Speed(4); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(4); }
        OPPONENT(SPECIES_COMFEY) { Ability(ABILITY_TRIAGE); Speed(5); Moves(MOVE_AFTER_YOU, MOVE_DRAINING_KISS); }
        OPPONENT(SPECIES_CLEFAIRY) { Speed(3); Moves(move, MOVE_PSYCHIC); }
    } WHEN {
        if (move == MOVE_TRICK_ROOM)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_AFTER_YOU, target:opponentRight); EXPECT_MOVE(opponentRight, MOVE_TRICK_ROOM); }
        else
            TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_AFTER_YOU); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI uses Trick Room intelligently")
{
    enum Move move;
    enum Ability ability;
    u32 speed;

    PARAMETRIZE { move = MOVE_DRAINING_KISS; ability = ABILITY_SYNCHRONIZE; speed = 4; }
    // PARAMETRIZE { move = MOVE_DAZZLING_GLEAM; ability = ABILITY_SYNCHRONIZE; speed = 4; }
    // PARAMETRIZE { move = MOVE_DRAINING_KISS; ability = ABILITY_PSYCHIC_SURGE; speed = 4; }
    PARAMETRIZE { move = MOVE_DRAINING_KISS; ability = ABILITY_SYNCHRONIZE; speed = 2; }
    PARAMETRIZE { move = MOVE_DAZZLING_GLEAM; ability = ABILITY_SYNCHRONIZE; speed = 2; }
    PARAMETRIZE { move = MOVE_DRAINING_KISS; ability = ABILITY_PSYCHIC_SURGE; speed = 2; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_AFTER_YOU) == EFFECT_AFTER_YOU);
        ASSUME(GetMoveEffect(MOVE_TRICK_ROOM) == EFFECT_TRICK_ROOM);
        ASSUME(IsHealingMove(MOVE_DRAINING_KISS)); // Doesn't have the Healing Move flag in Gen 5
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Speed(4); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(speed); }
        OPPONENT(SPECIES_COMFEY) { Ability(ABILITY_TRIAGE); Speed(5); Moves(move); }
        OPPONENT(SPECIES_INDEEDEE) { Ability(ability); Speed(3); Moves(MOVE_TRICK_ROOM, MOVE_PSYCHIC); }
    } WHEN {
        if (move == MOVE_DRAINING_KISS && ability != ABILITY_PSYCHIC_SURGE && speed > 3)
            TURN { EXPECT_MOVE(opponentRight, MOVE_TRICK_ROOM); }
        else
            TURN { NOT_EXPECT_MOVE(opponentRight, MOVE_TRICK_ROOM); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI Trick Room decisions compare a right-side setter with its earlier-slot ally")
{
    u32 partnerSpeed;

    PARAMETRIZE { partnerSpeed = 1; }
    PARAMETRIZE { partnerSpeed = 5; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_TRICK_ROOM) == EFFECT_TRICK_ROOM);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Speed(4); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(4); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(partnerSpeed); Moves(MOVE_PSYCHIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); Moves(MOVE_TRICK_ROOM, MOVE_PSYCHIC); }
    } WHEN {
        if (partnerSpeed < 4)
            TURN { EXPECT_MOVE(opponentRight, MOVE_TRICK_ROOM); }
        else
            TURN { NOT_EXPECT_MOVE(opponentRight, MOVE_TRICK_ROOM); }
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: Trick Room never cancels itself and is reset after expiry")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_TRICK_ROOM) == EFFECT_TRICK_ROOM);
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); SpDefense(300); Speed(40); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); SpDefense(300); Speed(30); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { HP(500); MaxHP(500); SpAttack(10); Moves(MOVE_TRICK_ROOM, MOVE_PSYCHIC); Speed(20); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); SpAttack(10); Moves(MOVE_TRICK_ROOM, MOVE_PSYCHIC); Speed(10); }
    } WHEN {
        TURN { EXPECT_MOVES(opponentLeft, MOVE_TRICK_ROOM, MOVE_PSYCHIC); EXPECT_MOVES(opponentRight, MOVE_TRICK_ROOM, MOVE_PSYCHIC); }
        TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM); NOT_EXPECT_MOVE(opponentRight, MOVE_TRICK_ROOM); }
        TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM); NOT_EXPECT_MOVE(opponentRight, MOVE_TRICK_ROOM); }
        TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM); NOT_EXPECT_MOVE(opponentRight, MOVE_TRICK_ROOM); }
        TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM); NOT_EXPECT_MOVE(opponentRight, MOVE_TRICK_ROOM); }
        TURN { EXPECT_MOVES(opponentLeft, MOVE_TRICK_ROOM, MOVE_PSYCHIC); EXPECT_MOVES(opponentRight, MOVE_TRICK_ROOM, MOVE_PSYCHIC); }
    } THEN {
        // Either flank may set it, but exactly one cast is spent per cycle.
        EXPECT_EQ(opponentLeft->pp[0] + opponentRight->pp[0], GetMovePP(MOVE_TRICK_ROOM) * 2 - 2);
        EXPECT(gFieldStatuses & STATUS_FIELD_TRICK_ROOM);
        EXPECT_EQ(gFieldTimers.trickRoomTimer, 4);
    }
}

AI_DOUBLE_BATTLE_TEST("AI uses Helping Hand if it's about to die")
{
    u32 hp;

    PARAMETRIZE { hp = 1; }
    PARAMETRIZE { hp = 500; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_HELPING_HAND) == EFFECT_HELPING_HAND);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(hp); Moves(MOVE_HELPING_HAND, MOVE_MUDDY_WATER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_MUDDY_WATER); }
    } WHEN {
        if (hp == 1)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_HELPING_HAND); }
        else
            TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_HELPING_HAND); }
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: reciprocal Helping Hand secures its concrete partner's spread KOs")
{
    bool32 reverse;
    PARAMETRIZE { reverse = FALSE; }
    PARAMETRIZE { reverse = TRUE; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(80); MaxHP(80); Defense(200); SpDefense(100); Attack(1); Speed(50); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(80); MaxHP(80); Defense(200); SpDefense(100); Attack(1); Speed(40); Moves(MOVE_TACKLE); }
        if (!reverse) {
            OPPONENT(SPECIES_ORANGURU) { Level(50); HP(300); MaxHP(300); Speed(60); SpAttack(10); Ability(ABILITY_TELEPATHY); Moves(MOVE_HELPING_HAND, MOVE_MUD_SLAP); }
            OPPONENT(SPECIES_POLITOED) { Level(50); HP(300); MaxHP(300); Speed(100); SpAttack(150); Ability(ABILITY_WATER_ABSORB); Moves(MOVE_SURF); }
        } else {
            OPPONENT(SPECIES_POLITOED) { Level(50); HP(300); MaxHP(300); Speed(100); SpAttack(150); Ability(ABILITY_WATER_ABSORB); Moves(MOVE_SURF); }
            OPPONENT(SPECIES_ORANGURU) { Level(50); HP(300); MaxHP(300); Speed(60); SpAttack(10); Ability(ABILITY_TELEPATHY); Moves(MOVE_HELPING_HAND, MOVE_MUD_SLAP); }
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            EXPECT_MOVE(reverse ? opponentRight : opponentLeft, MOVE_HELPING_HAND, target: reverse ? opponentLeft : opponentRight);
            EXPECT_MOVE(reverse ? opponentLeft : opponentRight, MOVE_SURF);
        }
    } SCENE {
        MESSAGE("The opposing Oranguru used Helping Hand!");
        MESSAGE("The opposing Politoed used Surf!");
        HP_BAR(playerLeft, hp: 0);
        HP_BAR(playerRight, hp: 0);
    }
}

AI_DOUBLE_BATTLE_TEST("AI does not use Helping Hand on Good as Gold ally")
{
    GIVEN {
        ASSUME(GetMoveEffect(MOVE_HELPING_HAND) == EFFECT_HELPING_HAND);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_HELPING_HAND, MOVE_MUD_SLAP); }
        OPPONENT(SPECIES_GHOLDENGO) { Ability(ABILITY_GOOD_AS_GOLD); Moves(MOVE_MUDDY_WATER); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_MUD_SLAP); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI uses Tailwind based on speed matchups")
{
    u32 speed1, speed2, speed3, speed4;
    bool32 expectTailwind;

    // All four comparisons qualify -> tailwindScore = 5
    PARAMETRIZE { speed1 = 20; speed2 = 20; speed3 = 20; speed4 = 20; expectTailwind = TRUE; }
    // Only the attacker flips one foe matchup -> tailwindScore = 2
    PARAMETRIZE { speed1 = 20; speed2 = 40; speed3 = 20; speed4 = 50; expectTailwind = TRUE; }
    // Only the partner flips one foe matchup -> tailwindScore = 2
    PARAMETRIZE { speed1 = 10; speed2 = 29; speed3 = 50; speed4 = 15; expectTailwind = TRUE; }
    // Too slow: even after doubling, still slower than both foes -> tailwindScore = 0.
    PARAMETRIZE { speed1 = 40; speed2 = 40; speed3 = 10; speed4 = 10; expectTailwind = FALSE; }
    // Already faster: Tailwind doesn't improve matchups -> tailwindScore = 0.
    PARAMETRIZE { speed1 =  5; speed2 =  5; speed3 = 10; speed4 = 10; expectTailwind = FALSE; }
    // Boundary: speed*2 == foe speed does not count -> tailwindScore = 0.
    PARAMETRIZE { speed1 = 20; speed2 = 20; speed3 = 10; speed4 = 30; expectTailwind = FALSE; }

    GIVEN {
        WITH_CONFIG(AI_REVERSE_BATTLER_LOGIC_ORDER_CHANCE, 100);
        ASSUME(GetMoveEffect(MOVE_TAILWIND) == EFFECT_TAILWIND);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Speed(speed1); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(speed2); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(speed3); Moves(MOVE_TAILWIND, MOVE_HEADBUTT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(speed4); Moves(MOVE_TAILWIND, MOVE_HEADBUTT); }
    } WHEN {
        if (expectTailwind)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_TAILWIND); }
        else
            TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_TAILWIND); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI uses Tailwind to trigger Wind Rider (Doubles)")
{
    bool32 expectTailwind;
    enum Species tailwindSpecies, partnerSpecies;
    enum Ability tailwindAbility, partnerAbility;

    PARAMETRIZE { tailwindSpecies = SPECIES_BRAMBLEGHAST; tailwindAbility = ABILITY_WIND_RIDER;  partnerSpecies = SPECIES_BRAMBLEGHAST; partnerAbility = ABILITY_WIND_RIDER;  expectTailwind = TRUE; }
    PARAMETRIZE { tailwindSpecies = SPECIES_BRAMBLEGHAST; tailwindAbility = ABILITY_WIND_RIDER;  partnerSpecies = SPECIES_BRAMBLEGHAST; partnerAbility = ABILITY_INFILTRATOR; expectTailwind = TRUE; }
    PARAMETRIZE { tailwindSpecies = SPECIES_BRAMBLEGHAST; tailwindAbility = ABILITY_INFILTRATOR; partnerSpecies = SPECIES_BRAMBLEGHAST; partnerAbility = ABILITY_WIND_RIDER;  expectTailwind = TRUE; }
    PARAMETRIZE { tailwindSpecies = SPECIES_BRAMBLEGHAST; tailwindAbility = ABILITY_INFILTRATOR; partnerSpecies = SPECIES_BRAMBLEGHAST; partnerAbility = ABILITY_INFILTRATOR; expectTailwind = FALSE; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_TAILWIND) == EFFECT_TAILWIND);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Speed(20); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(20); }
        OPPONENT(tailwindSpecies) { Ability(tailwindAbility); Speed(9); Moves(MOVE_TAILWIND, MOVE_HEADBUTT); }
        OPPONENT(partnerSpecies) { Ability(partnerAbility); Speed(9); Moves(MOVE_HEADBUTT); }
    } WHEN {
        if (expectTailwind)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_TAILWIND); }
        else
            TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_TAILWIND); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI uses Tailwind to trigger Wind Power (Doubles)")
{
    bool32 expectTailwind;
    enum Species tailwindSpecies, partnerSpecies;
    enum Ability tailwindAbility, partnerAbility;

    PARAMETRIZE { tailwindSpecies = SPECIES_KILOWATTREL; tailwindAbility = ABILITY_WIND_POWER;  partnerSpecies = SPECIES_KILOWATTREL; partnerAbility = ABILITY_WIND_POWER;  expectTailwind = TRUE; }
    PARAMETRIZE { tailwindSpecies = SPECIES_KILOWATTREL; tailwindAbility = ABILITY_WIND_POWER;  partnerSpecies = SPECIES_KILOWATTREL; partnerAbility = ABILITY_COMPETITIVE; expectTailwind = TRUE; }
    PARAMETRIZE { tailwindSpecies = SPECIES_KILOWATTREL; tailwindAbility = ABILITY_COMPETITIVE; partnerSpecies = SPECIES_KILOWATTREL; partnerAbility = ABILITY_WIND_POWER;  expectTailwind = TRUE; }
    PARAMETRIZE { tailwindSpecies = SPECIES_KILOWATTREL; tailwindAbility = ABILITY_COMPETITIVE; partnerSpecies = SPECIES_KILOWATTREL; partnerAbility = ABILITY_COMPETITIVE; expectTailwind = FALSE; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_TAILWIND) == EFFECT_TAILWIND);
        ASSUME(GetMoveType(MOVE_THUNDERSHOCK) == TYPE_ELECTRIC);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Speed(20); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(20); }
        OPPONENT(tailwindSpecies) { Ability(tailwindAbility); Speed(21); Moves(MOVE_TAILWIND, MOVE_THUNDERSHOCK); }
        OPPONENT(partnerSpecies) { Ability(partnerAbility); Speed(21); Moves(MOVE_THUNDERSHOCK); }
    } WHEN {
        if (expectTailwind)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_TAILWIND); }
        else
            TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_TAILWIND); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI uses Guard Split to improve its stats")
{
    u32 player, opponent;

    PARAMETRIZE { player = SPECIES_SHUCKLE; opponent = SPECIES_PHEROMOSA; }
    PARAMETRIZE { player = SPECIES_PHEROMOSA; opponent = SPECIES_SHUCKLE; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_GUARD_SPLIT) == EFFECT_GUARD_SPLIT);
        ASSUME(gSpeciesInfo[SPECIES_PHEROMOSA].baseDefense < gSpeciesInfo[SPECIES_WOBBUFFET].baseDefense);
        ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].baseDefense < gSpeciesInfo[SPECIES_SHUCKLE].baseDefense);
        ASSUME(gSpeciesInfo[SPECIES_PHEROMOSA].baseSpDefense < gSpeciesInfo[SPECIES_WOBBUFFET].baseSpDefense);
        ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].baseSpDefense < gSpeciesInfo[SPECIES_SHUCKLE].baseSpDefense);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(player);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_GUARD_SPLIT, MOVE_NIGHT_SHADE); }
        OPPONENT(opponent);
    } WHEN {
        if (player == SPECIES_SHUCKLE)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_GUARD_SPLIT, target:playerLeft); }
        else
            TURN { EXPECT_MOVE(opponentLeft, MOVE_GUARD_SPLIT, target:opponentRight); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI uses Power Split to improve its stats")
{
    u32 player, opponent;

    PARAMETRIZE { player = SPECIES_SHUCKLE; opponent = SPECIES_PHEROMOSA; }
    PARAMETRIZE { player = SPECIES_PHEROMOSA; opponent = SPECIES_SHUCKLE; }

    GIVEN {
        ASSUME(GetMoveEffect(MOVE_POWER_SPLIT) == EFFECT_POWER_SPLIT);
        ASSUME(gSpeciesInfo[SPECIES_PHEROMOSA].baseAttack > gSpeciesInfo[SPECIES_WOBBUFFET].baseAttack);
        ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].baseAttack > gSpeciesInfo[SPECIES_SHUCKLE].baseAttack);
        ASSUME(gSpeciesInfo[SPECIES_PHEROMOSA].baseSpAttack > gSpeciesInfo[SPECIES_WOBBUFFET].baseSpAttack);
        ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].baseSpAttack > gSpeciesInfo[SPECIES_SHUCKLE].baseSpAttack);
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(player);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_POWER_SPLIT, MOVE_TACKLE, MOVE_MAGICAL_LEAF); }
        OPPONENT(opponent) { Moves(MOVE_TACKLE, MOVE_MAGICAL_LEAF); }
    } WHEN {
        if (player == SPECIES_PHEROMOSA)
            TURN { EXPECT_MOVE(opponentLeft, MOVE_POWER_SPLIT, target:playerLeft); }
        else
            TURN { EXPECT_MOVE(opponentLeft, MOVE_POWER_SPLIT, target:opponentRight); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI prefers to Fake Out the opponent vulnerable to flinching.")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_TRY_TO_FAINT | AI_FLAG_CHECK_VIABILITY | AI_FLAG_DOUBLE_BATTLE | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_ZUBAT) { Ability(ABILITY_INNER_FOCUS); }
        PLAYER(SPECIES_BRAIXEN) { Ability(ABILITY_BLAZE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_FAKE_OUT, MOVE_BRANCH_POKE, MOVE_ROCK_SMASH); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_FAKE_OUT, target:playerRight); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI uses Gear Up")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_POUND, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_POUND, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_KLINKLANG) { Ability(ABILITY_PLUS); Moves(MOVE_GEAR_UP, MOVE_WATER_GUN, MOVE_POUND); }
        OPPONENT(SPECIES_KLINKLANG) { Ability(ABILITY_PLUS); Moves(MOVE_GEAR_UP, MOVE_WATER_GUN, MOVE_POUND); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_GEAR_UP); }
    }
}

AI_DOUBLE_BATTLE_TEST("AI uses Magnetic Flux")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_OMNISCIENT);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_POUND, MOVE_SWIFT, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_POUND, MOVE_SWIFT, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_KLINK) { Ability(ABILITY_PLUS); Moves(MOVE_MAGNETIC_FLUX, MOVE_POUND); }
        OPPONENT(SPECIES_KLINK) { Ability(ABILITY_PLUS); Moves(MOVE_MAGNETIC_FLUX, MOVE_POUND); }
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_MAGNETIC_FLUX); }
    }
}

// No existing fixture checks the paired evaluator's transaction boundary.
// Keep this one call-level invariant separate from the chosen-command cases.
AI_DOUBLE_BATTLE_TEST("EC expert pair: candidate evaluation restores board caches field and RNG")
{
    bool32 plusMinus;
    enum Item offensiveItem;
    PARAMETRIZE { plusMinus = FALSE; offensiveItem = ITEM_NONE; }
    PARAMETRIZE { plusMinus = TRUE; offensiveItem = ITEM_NONE; }
    PARAMETRIZE { plusMinus = FALSE; offensiveItem = ITEM_DEEP_SEA_TOOTH; }
    PARAMETRIZE { plusMinus = FALSE; offensiveItem = ITEM_LIFE_ORB; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_CHARMANDER) { Level(50); HP(500); MaxHP(500); SpDefense(300); Speed(40); Item(ITEM_PASSHO_BERRY); Moves(MOVE_CELEBRATE, MOVE_KNOCK_OFF); }
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(500); MaxHP(500); SpDefense(300); Speed(30); Ability(ABILITY_WATER_ABSORB); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ORANGURU) { Level(50); HP(500); MaxHP(500); SpAttack(10); Speed(60); Ability(plusMinus ? ABILITY_PLUS : ABILITY_TELEPATHY); Moves(MOVE_HELPING_HAND, MOVE_MUD_SLAP, MOVE_PROTECT, MOVE_TRICK_ROOM); }
        OPPONENT(offensiveItem == ITEM_DEEP_SEA_TOOTH ? SPECIES_CLAMPERL : SPECIES_BLASTOISE) { Level(50); HP(500); MaxHP(500); SpAttack(100); Speed(100); Item(offensiveItem); Ability(plusMinus ? ABILITY_MINUS : ABILITY_TORRENT); Moves(MOVE_SURF, MOVE_PROTECT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVES(opponentLeft, MOVE_HELPING_HAND, MOVE_MUD_SLAP, MOVE_PROTECT, MOVE_TRICK_ROOM);
            EXPECT_MOVES(opponentRight, MOVE_SURF, MOVE_PROTECT);
        }
    } THEN {
        struct {
            struct BattlePokemon mons[MAX_BATTLERS_COUNT];
            struct AiLogicData logic;
            struct ProtectStruct protect[MAX_BATTLERS_COUNT];
            struct SpecialStatus special[MAX_BATTLERS_COUNT];
            struct BattleStruct battle;
            struct FieldTimer field;
            rng_value_t rng, rng2;
            u16 movePower, weather;
            u32 fieldStatus;
            enum BattlerId itemBattler;
        } *before = Alloc(sizeof(*before));
        s32 score;
        enum BattlerId actor = opponentLeft - gBattleMons;
        enum BattlerId target = playerLeft - gBattleMons;
        // Exercise a consumable and a partially injured absorbing target in
        // the counterfactual without allowing either change to escape it.
        playerLeft->item = ITEM_PASSHO_BERRY;
        gAiLogicData->items[target] = ITEM_PASSHO_BERRY;
        gAiLogicData->holdEffects[target] = HOLD_EFFECT_RESIST_BERRY;
        playerRight->hp = 450;
        memcpy(before->mons, gBattleMons, sizeof(before->mons));
        before->logic = *gAiLogicData;
        memcpy(before->protect, gProtectStructs, sizeof(before->protect));
        memcpy(before->special, gSpecialStatuses, sizeof(before->special));
        before->battle = *gBattleStruct;
        before->field = gFieldTimers;
        before->rng = gRngValue;
        before->rng2 = gRng2Value;
        before->movePower = gBattleMovePower;
        before->weather = gBattleWeather;
        before->fieldStatus = gFieldStatuses;
        before->itemBattler = gPotentialItemEffectBattler;
        memset(gBattleTestRunnerState->data.stack, 0xA5, 32);
        score = AI_EvaluateDoublesPosition(actor, 0);
        EXPECT_EQ(AI_EvaluateDoublesPosition(actor, 0), score);
        EXPECT_EQ(memcmp(before->mons, gBattleMons, sizeof(before->mons)), 0);
        EXPECT_EQ(memcmp(&before->logic, gAiLogicData, sizeof(before->logic)), 0);
        EXPECT_EQ(memcmp(before->protect, gProtectStructs, sizeof(before->protect)), 0);
        EXPECT_EQ(memcmp(before->special, gSpecialStatuses, sizeof(before->special)), 0);
        EXPECT_EQ(memcmp(&before->battle, gBattleStruct, sizeof(before->battle)), 0);
        EXPECT_EQ(memcmp(&before->field, &gFieldTimers, sizeof(before->field)), 0);
        EXPECT_EQ(memcmp(&before->rng, &gRngValue, sizeof(before->rng)), 0);
        EXPECT_EQ(memcmp(&before->rng2, &gRng2Value, sizeof(before->rng2)), 0);
        EXPECT_EQ(gBattleMovePower, before->movePower);
        EXPECT_EQ(gBattleWeather, before->weather);
        EXPECT_EQ(gFieldStatuses, before->fieldStatus);
        EXPECT_EQ(gPotentialItemEffectBattler, before->itemBattler);
        for (u32 i = 0; i < 32; i++)
            EXPECT_EQ(gBattleTestRunnerState->data.stack[i], 0xA5);
        Free(before);
    }
}

// Distinct E0014 failures, each reproduced by disabling its repair. These
// generic cases use no authored trainer ID and do not lock campaign loadouts.
AI_DOUBLE_BATTLE_TEST("EC expert pair: non-Flying priority Roost recovers under continued pressure", s16 firstDamage, s16 healing)
{
    PARAMETRIZE {}
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(77); MaxHP(77); Attack(18); Defense(66);
            SpAttack(21); SpDefense(36); Speed(35); Nature(NATURE_BOLD);
            Ability(ABILITY_VOLT_ABSORB); Item(ITEM_EVIOLITE);
            Moves(MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT);
        }
        PLAYER(SPECIES_LOTAD) {
            Level(14); HP(71); MaxHP(71); Attack(15); Defense(53);
            SpAttack(20); SpDefense(25); Speed(17); Nature(NATURE_BOLD);
            Ability(ABILITY_RAIN_DISH); Item(ITEM_EVIOLITE);
            Moves(MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_REST, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_VOLBEAT) {
            Level(12); HP(25); MaxHP(73); Attack(23); Defense(28);
            SpAttack(57); SpDefense(29); Speed(29); Nature(NATURE_MODEST);
            Ability(ABILITY_PRANKSTER); Item(ITEM_LUM_BERRY);
            Moves(MOVE_TAIL_GLOW, MOVE_ROOST, MOVE_BUG_BUZZ, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_ILLUMISE) {
            Level(12); HP(43); MaxHP(43); Attack(18); Defense(26);
            SpAttack(63); SpDefense(29); Speed(61); Nature(NATURE_MODEST);
            Ability(ABILITY_PRANKSTER); Item(ITEM_LEFTOVERS);
            Moves(MOVE_DAZZLING_GLEAM, MOVE_BUG_BUZZ, MOVE_ENCORE, MOVE_HELPING_HAND);
        }
        // The actual reserve fixture's only other teammates were fainted.
        // Omitting those dead records preserves the available action menu.
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
        }
        TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentLeft, MOVE_ROOST);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERBOLT, playerLeft);
        HP_BAR(opponentLeft, captureDamage: &results[i].firstDamage);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROOST, opponentLeft);
        HP_BAR(opponentLeft, captureDamage: &results[i].healing);
    } THEN {
        EXPECT_GT(results[i].firstDamage, 0);
        EXPECT_EQ(results[i].healing, -36);
        EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_ROOST);
        EXPECT_GT(opponentLeft->hp, 25 - results[i].firstDamage);
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: timely primary Charm protects its user", s16 replyDamage)
{
    PARAMETRIZE {}
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_TIMBURR) {
            Level(14); HP(81); MaxHP(81); Attack(49); Defense(24);
            SpAttack(14); SpDefense(38); Speed(19);
            Nature(NATURE_CAREFUL); Ability(ABILITY_IRON_FIST); Item(ITEM_EVIOLITE);
            Moves(MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT);
        }
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(77); MaxHP(77); Attack(18); Defense(30);
            SpAttack(21); SpDefense(72); Speed(35);
            Nature(NATURE_CALM); Ability(ABILITY_VOLT_ABSORB); Item(ITEM_SITRUS_BERRY);
            Moves(MOVE_SUPER_FANG, MOVE_THUNDERBOLT, MOVE_FOLLOW_ME, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_PLUSLE) {
            Level(12); HP(42); MaxHP(42); Attack(18); Defense(18);
            SpAttack(65); SpDefense(26); Speed(71);
            Nature(NATURE_TIMID); Ability(ABILITY_PLUS); Item(ITEM_FOCUS_SASH);
            Moves(MOVE_THUNDERBOLT, MOVE_NUZZLE, MOVE_HELPING_HAND, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_MINUN) {
            Level(12); HP(42); MaxHP(42); Attack(16); Defense(20);
            SpAttack(58); SpDefense(33); Speed(71);
            Nature(NATURE_TIMID); Ability(ABILITY_MINUS); Item(ITEM_SITRUS_BERRY);
            Moves(MOVE_THUNDERBOLT, MOVE_CHARM, MOVE_ENCORE, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, WITH_RNG(RNG_PARALYSIS, FALSE));
            MOVE(playerRight, MOVE_PROTECT);
        }
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, WITH_RNG(RNG_PARALYSIS, FALSE));
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentRight, MOVE_CHARM, target: playerLeft);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARM, opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAIN_PUNCH, playerLeft);
        HP_BAR(opponentRight, captureDamage: &results[i].replyDamage);
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 2);
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_GT(results[i].replyDamage, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: Nuzzle speed control distinguishes primary and secondary immunity")
{
    u32 boundary;
    PARAMETRIZE { boundary = 0; } // Nuzzle is the available speed-control move.
    PARAMETRIZE { boundary = 1; } // Cloak blocks Nuzzle's secondary, not damage.
    PARAMETRIZE { boundary = 2; } // Same Cloak target; TW is now a separate slot.
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_TAUROS) {
            Level(50); HP(100); MaxHP(100); Attack(300); SpAttack(210);
            Defense(100); SpDefense(100); Speed(80); Ability(ABILITY_ANGER_POINT);
            Item(boundary ? ITEM_COVERT_CLOAK : ITEM_NONE); Moves(MOVE_TACKLE);
        }
        PLAYER(SPECIES_CHANSEY) {
            Level(50); HP(300); MaxHP(300); Attack(100); SpAttack(100);
            Defense(300); SpDefense(300); Speed(10);
            Ability(ABILITY_NATURAL_CURE); Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_SMEARGLE) {
            Level(50); HP(300); MaxHP(300); Attack(60); SpAttack(10);
            Defense(300); SpDefense(300); Speed(100); Ability(ABILITY_OWN_TEMPO);
            Moves(MOVE_NUZZLE, MOVE_STRENGTH, boundary == 2 ? MOVE_THUNDER_WAVE : MOVE_NONE);
        }
        OPPONENT(SPECIES_ORANGURU) {
            Level(50); HP(65); MaxHP(65); Attack(100); SpAttack(300);
            Defense(100); SpDefense(100); Speed(60); Ability(ABILITY_TELEPATHY);
            Moves(MOVE_PSYCHIC, MOVE_PROTECT);
        }
        // Before paralysis: Smeargle100 > Tauros80 > Oranguru60.
        // Modern native paralysis halves Tauros to40. No manually assigned
        // status, speed stage, cached damage, or pending human input is read.
        // Smeargle's native Sketch access permits these move combinations;
        // the explicit stat scale is solely a generic mechanical isolation.
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_CELEBRATE);
        }
    } THEN {
        if (boundary != 1)
        {
            EXPECT_EQ(gLastMoves[B_BATTLER_1], boundary == 0 ? MOVE_NUZZLE : MOVE_THUNDER_WAVE);
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_PSYCHIC);
            EXPECT_EQ(playerLeft->hp, 0);
            EXPECT_GT(opponentRight->hp, 0);
        }
        else
        {
            EXPECT_NE(gLastMoves[B_BATTLER_1], MOVE_NUZZLE);
            EXPECT_GT(playerLeft->hp, 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC expert pair: preserve the Plus Minus partner before taking a boosted knockout")
{
    u32 partnerHp;
    PARAMETRIZE { partnerHp = 1; }
    PARAMETRIZE { partnerHp = 100; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_EEVEE) {
            Level(14); HP(1); MaxHP(100); Attack(30); Defense(20);
            SpAttack(20); SpDefense(50); Speed(40);
            Ability(ABILITY_ADAPTABILITY); Moves(MOVE_QUICK_ATTACK);
        }
        PLAYER(SPECIES_MUNCHLAX) {
            Level(14); HP(25); MaxHP(25); Attack(20); Defense(50);
            SpAttack(20); SpDefense(50); Speed(5);
            Ability(ABILITY_THICK_FAT); Moves(MOVE_STOCKPILE);
        }
        OPPONENT(SPECIES_PLUSLE) {
            Level(12); HP(100); MaxHP(100); Attack(40); Defense(30);
            SpAttack(60); SpDefense(30); Speed(70);
            Ability(ABILITY_PLUS); Moves(MOVE_THUNDERBOLT, MOVE_QUICK_ATTACK);
        }
        OPPONENT(SPECIES_MINUN) {
            Level(12); HP(partnerHp); MaxHP(100); Attack(20); Defense(20);
            SpAttack(20); SpDefense(30); Speed(10);
            Ability(ABILITY_MINUS); Moves(MOVE_CELEBRATE);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_QUICK_ATTACK, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_STOCKPILE);
        }
    } THEN {
        EXPECT_EQ(gLastMoves[B_BATTLER_1], partnerHp == 1 ? MOVE_QUICK_ATTACK : MOVE_THUNDERBOLT);
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_EQ(partnerHp == 1 ? playerLeft->hp : playerRight->hp, 0);
    }
}

// E0015: both item paths independently fail when their cache consumer is
// disabled. Synthetic moves/stats isolate timing, not campaign loadouts.
AI_DOUBLE_BATTLE_TEST("EC expert pair: Knock Off timing removes only the later offensive item boost")
{
    u32 removerSpeed;
    bool32 orb;
    PARAMETRIZE { removerSpeed = 60; orb = FALSE; }
    PARAMETRIZE { removerSpeed = 5; orb = FALSE; }
    PARAMETRIZE { removerSpeed = 60; orb = TRUE; }
    PARAMETRIZE { removerSpeed = 5; orb = TRUE; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_MIENFOO) {
            Level(14); HP(100); MaxHP(100); Attack(20); Defense(orb ? 100 : 20);
            SpAttack(20); SpDefense(20); Speed(removerSpeed);
            Ability(ABILITY_INNER_FOCUS); Item(ITEM_NONE); Moves(MOVE_KNOCK_OFF);
        }
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(100); MaxHP(100); Attack(20); Defense(60);
            SpAttack(20); SpDefense(60); Speed(20);
            Ability(ABILITY_VOLT_ABSORB); Item(ITEM_NONE); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_TENTACOOL) {
            Level(12); HP(100); MaxHP(100); Attack(20); Defense(200);
            SpAttack(20); SpDefense(50); Speed(10);
            Ability(ABILITY_LIQUID_OOZE); Item(ITEM_NONE); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_CLAMPERL) {
            Level(orb ? 50 : 12); HP(100); MaxHP(100); Attack(orb ? 120 : 80); Defense(50);
            SpAttack(60); SpDefense(40); Speed(30);
            Ability(ABILITY_SHELL_ARMOR); Item(orb ? ITEM_LIFE_ORB : ITEM_DEEP_SEA_TOOTH);
            Moves(orb ? MOVE_STRENGTH : MOVE_ICE_BEAM, orb ? MOVE_SEISMIC_TOSS : MOVE_WATERFALL);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_GT(playerLeft->hp, 0);
        EXPECT_EQ(opponentRight->item, ITEM_NONE);
        if (orb)
            EXPECT_EQ(gLastMoves[B_BATTLER_3], removerSpeed == 60 ? MOVE_SEISMIC_TOSS : MOVE_STRENGTH);
        else
            EXPECT_EQ(gLastMoves[B_BATTLER_3], removerSpeed == 60 ? MOVE_WATERFALL : MOVE_ICE_BEAM);
    }
}

// E0017: disabling post-hit guard clearing reproduces the failed pair.
// These fixed stats are a generic discriminator, not a trainer loadout lock.
AI_DOUBLE_BATTLE_TEST("EC expert pair: Feint opens partner damage through Protect but not immunity")
{
    u32 control;
    PARAMETRIZE { control = 0; }
    PARAMETRIZE { control = 1; }
    PARAMETRIZE { control = 2; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(control == 2 ? SPECIES_DUSKULL : SPECIES_PACHIRISU) {
            Level(14); HP(77); MaxHP(77); Attack(18); Defense(66);
            SpAttack(21); SpDefense(36); Speed(35);
            Ability(control == 2 ? ABILITY_LEVITATE : ABILITY_VOLT_ABSORB);
            Item(control == 1 ? ITEM_COVERT_CLOAK : ITEM_NONE); Moves(MOVE_PROTECT);
        }
        PLAYER(control == 2 ? SPECIES_DUSKULL : SPECIES_MUNCHLAX) {
            Level(14); HP(98); MaxHP(98); Attack(33); Defense(57);
            SpAttack(20); SpDefense(35); Speed(7);
            Ability(control == 2 ? ABILITY_LEVITATE : ABILITY_THICK_FAT);
            Item(control == 1 ? ITEM_COVERT_CLOAK : ITEM_NONE); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_CRABRAWLER) {
            Level(12); HP(69); MaxHP(69); Attack(66); Defense(24);
            SpAttack(16); SpDefense(20); Speed(23);
            Ability(ABILITY_IRON_FIST); Item(ITEM_EVIOLITE);
            Moves(MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_THUNDER_PUNCH, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_CLOBBOPUS) {
            Level(12); HP(69); MaxHP(69); Attack(62); Defense(25);
            SpAttack(18); SpDefense(20); Speed(16);
            Ability(ABILITY_TECHNICIAN); Item(ITEM_LEFTOVERS);
            Moves(MOVE_POWER_UP_PUNCH, MOVE_FEINT, MOVE_TAUNT, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(gBattleResults.opponentFaintCounter, 0);
        EXPECT_EQ(gBattleResults.playerFaintCounter, 0);
        if (control != 2)
        {
            EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_DRAIN_PUNCH);
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_FEINT);
            EXPECT_LT(playerLeft->hp + playerRight->hp, playerLeft->maxHP + playerRight->maxHP);
        }
        else
        {
            EXPECT(gLastMoves[B_BATTLER_3] != MOVE_FEINT);
            EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
            EXPECT_EQ(playerRight->hp, playerRight->maxHP);
        }
    }
}

// Distinct regression: disabling paired Glare leaves the slower ally unable
// to remove a threat before it KOs the setter. Cheri removes that payoff.
AI_DOUBLE_BATTLE_TEST("EC expert pair: Glare enables a partner KO but not through a paralysis cure")
{
    bool32 cure;
    PARAMETRIZE { cure = FALSE; }
    PARAMETRIZE { cure = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY
            | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_MUNCHLAX) {
            Level(14); HP(30); MaxHP(100); Attack(100); Defense(20); SpAttack(20); SpDefense(120); Speed(45);
            Ability(ABILITY_THICK_FAT); Item(cure ? ITEM_CHERI_BERRY : ITEM_NONE); Moves(MOVE_BODY_SLAM);
        }
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(100); MaxHP(100); Attack(20); Defense(100); SpAttack(20); SpDefense(100); Speed(20);
            Ability(ABILITY_VOLT_ABSORB); Item(ITEM_NONE); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_SEVIPER) {
            Level(12); HP(35); MaxHP(70); Attack(20); Defense(25); SpAttack(60); SpDefense(30); Speed(60);
            Ability(ABILITY_INFILTRATOR); Item(ITEM_LIFE_ORB);
            Moves(MOVE_SLUDGE_BOMB, MOVE_FLAMETHROWER, MOVE_GIGA_DRAIN, MOVE_GLARE);
        }
        OPPONENT(SPECIES_DUNSPARCE) {
            Level(12); HP(100); MaxHP(100); Attack(100); Defense(40); SpAttack(20); SpDefense(30); Speed(35);
            Ability(ABILITY_SERENE_GRACE); Item(ITEM_LEFTOVERS);
            Moves(MOVE_BODY_SLAM, MOVE_ROCK_SLIDE, MOVE_ROOST, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BODY_SLAM, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        if (!cure)
        {
            EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_GLARE);
            EXPECT_GT(opponentLeft->hp, 0);
            EXPECT_EQ(playerLeft->hp, 0);
        }
        else
            EXPECT_NE(gLastMoves[B_BATTLER_1], MOVE_GLARE);
    }
}

// Different failure from primary Glare: disabling Body Slam's probabilistic
// continuation selects Rock Slide. Do not require the random proc to occur.
AI_DOUBLE_BATTLE_TEST("EC expert pair: secondary paralysis earns probabilistic partner order with a Cloak control")
{
    u32 control;
    PARAMETRIZE { control = 0; }
    PARAMETRIZE { control = 1; }
    PARAMETRIZE { control = 2; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY
            | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_CHARIZARD) {
            Level(14); HP(70); MaxHP(100); Attack(20); Defense(80);
            SpAttack(100); SpDefense(30); Speed(45);
            Ability(ABILITY_BLAZE); Item(control == 2 ? ITEM_COVERT_CLOAK : ITEM_NONE);
            Moves(MOVE_FLAMETHROWER);
        }
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(100); MaxHP(100); Attack(20); Defense(100);
            SpAttack(20); SpDefense(100); Speed(20);
            Ability(ABILITY_VOLT_ABSORB); Item(ITEM_NONE); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_DUNSPARCE) {
            Level(12); HP(100); MaxHP(100); Attack(80); Defense(60);
            SpAttack(20); SpDefense(100); Speed(60);
            Ability(control == 1 ? ABILITY_RUN_AWAY : ABILITY_SERENE_GRACE); Item(ITEM_NONE);
            Moves(MOVE_BODY_SLAM, MOVE_ROCK_SLIDE, MOVE_ROOST, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_SEVIPER) {
            Level(12); HP(35); MaxHP(70); Attack(20); Defense(25);
            SpAttack(150); SpDefense(25); Speed(30);
            Ability(ABILITY_INFILTRATOR); Item(ITEM_NONE);
            Moves(MOVE_SLUDGE_BOMB, MOVE_FLAMETHROWER, MOVE_GIGA_DRAIN, MOVE_GLARE);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(gLastMoves[B_BATTLER_1], control == 2 ? MOVE_ROCK_SLIDE : MOVE_BODY_SLAM);
        u32 chance = CalcSecondaryEffectChance(B_BATTLER_1,
            control == 1 ? ABILITY_RUN_AWAY : ABILITY_SERENE_GRACE,
            GetMoveAdditionalEffectById(MOVE_BODY_SLAM, 0));
        EXPECT_EQ(chance, control == 1 ? 30 : 60);
        if (control == 2)
            EXPECT_EQ(playerLeft->status1 & STATUS1_PARALYSIS, 0);
    }
}
