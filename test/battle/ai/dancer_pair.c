#include "global.h"
#include "battle.h"
#include "test/battle.h"
#include "battle_ai_util.h"
#include "random.h"
#include "main.h"

#define DANCER_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC attacking Dancer: copied attack removes a threat before the dancer's normal turn")
{
    u32 denial = 0;
    PARAMETRIZE { denial = 0; }
    PARAMETRIZE { denial = 1; } // Already asleep.
    PARAMETRIZE { denial = 2; } // Earlier Fake Out.
    PARAMETRIZE { denial = 3; } // Taunt does not block attacking copies.
    PARAMETRIZE { denial = 4; } // Protect blocks Fake Out, preserving the copy.
    GIVEN {
        AI_FLAGS(DANCER_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(75); MaxHP(75); SpDefense(100); SpAttack(300); Speed(60); Ability(ABILITY_TELEPATHY); Moves(MOVE_SLUDGE_BOMB); }
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(300); MaxHP(300); SpDefense(100); Speed(10); Ability(ABILITY_PRANKSTER); Moves(MOVE_CELEBRATE, MOVE_FAKE_OUT, MOVE_TAUNT); }
        OPPONENT(SPECIES_VOLCARONA) { Level(50); HP(200); MaxHP(200); SpAttack(100); Speed(100); Moves(MOVE_FIERY_DANCE, MOVE_HEAT_WAVE); }
        OPPONENT(SPECIES_ORICORIO_POM_POM) { Level(50); HP(100); MaxHP(100); SpAttack(100); SpDefense(100); Speed(20); Status1(denial == 1 ? 3 : STATUS1_NONE); Ability(ABILITY_DANCER); Moves(MOVE_REVELATION_DANCE, denial == 2 ? MOVE_CELEBRATE : MOVE_PROTECT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SLUDGE_BOMB, target: opponentRight, secondaryEffect: FALSE, criticalHit: FALSE);
            if (denial == 2 || denial == 4)
                MOVE(playerRight, MOVE_FAKE_OUT, target: opponentRight);
            else if (denial == 3)
                MOVE(playerRight, MOVE_TAUNT, target: opponentRight);
            else
                MOVE(playerRight, MOVE_CELEBRATE);
            if (denial == 0 || denial == 3 || denial == 4)
            {
                EXPECT_MOVE(opponentLeft, MOVE_FIERY_DANCE, target: playerLeft);
                EXPECT_MOVE(opponentRight, denial == 4 ? MOVE_PROTECT : MOVE_REVELATION_DANCE);
            }
        }
    } THEN {
        if (denial == 0 || denial == 3 || denial == 4)
        {
            EXPECT_EQ(playerLeft->hp, 0);
            EXPECT(opponentRight->hp > 0);
        }
        else
        {
            EXPECT(playerLeft->hp > 0);
            EXPECT_EQ(opponentRight->hp, 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC attacking Dancer: do not give the opposing dancer a lethal free attack")
{
    GIVEN {
        AI_FLAGS(DANCER_FLAGS);
        PLAYER(SPECIES_FERROTHORN) { Level(50); HP(100); MaxHP(100); SpDefense(100); Speed(30); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_ORICORIO_POM_POM) { Level(50); HP(300); MaxHP(300); SpAttack(300); Speed(10); Ability(ABILITY_DANCER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_VOLCARONA) { Level(50); HP(50); MaxHP(200); SpAttack(100); SpDefense(100); Speed(100); Moves(MOVE_FIERY_DANCE, MOVE_FIRE_BLAST, MOVE_PROTECT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(20); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_FIRE_BLAST, target: playerLeft);
        }
    } THEN {
        EXPECT(opponentLeft->hp > 0);
    }
}

DOUBLE_BATTLE_TEST("EC Dancer mechanics: copying uses the receiver's Revelation Dance type after Protect")
{
    s16 sourceDamage, copiedDamage;
    GIVEN {
        PLAYER(SPECIES_GYARADOS) { Level(50); HP(500); MaxHP(500); SpDefense(100); Speed(30); Ability(ABILITY_MOXIE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ORICORIO_BAILE) { Level(50); SpAttack(100); Speed(100); Ability(ABILITY_DANCER); Moves(MOVE_REVELATION_DANCE); }
        OPPONENT(SPECIES_ORICORIO_POM_POM) { Level(50); SpAttack(100); Speed(20); Ability(ABILITY_DANCER); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_REVELATION_DANCE, target: playerLeft);
            MOVE(opponentRight, MOVE_PROTECT);
        }
    } SCENE {
        HP_BAR(playerLeft, captureDamage: &sourceDamage);
        HP_BAR(playerLeft, captureDamage: &copiedDamage);
    } THEN {
        EXPECT(copiedDamage > sourceDamage * 4);
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP - sourceDamage - copiedDamage);
        Test_MgbaPrintf("DANCER_REVELATION_SOURCE=%d COPY=%d", sourceDamage, copiedDamage);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Dancer anchors: copied damage respects both lost items and preserves native state")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Level(50); SpDefense(100); Speed(50); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_PIKACHU) { Speed(40); Ability(ABILITY_LIGHTNING_ROD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(30); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ORICORIO_POM_POM) { Level(50); SpAttack(100); Speed(20); Ability(ABILITY_DANCER); Item(ITEM_CHOICE_SPECS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); }
    } THEN {
        playerLeft->item = ITEM_ASSAULT_VEST;
        gAiLogicData->items[B_BATTLER_0] = ITEM_ASSAULT_VEST;
        gAiLogicData->holdEffects[B_BATTLER_0] = HOLD_EFFECT_ASSAULT_VEST;
        struct BattlePokemon saved[MAX_BATTLERS_COUNT];
        enum Item items[MAX_BATTLERS_COUNT];
        enum HoldEffect held[MAX_BATTLERS_COUNT];
        enum Ability abilities[MAX_BATTLERS_COUNT];
        memcpy(saved, gBattleMons, sizeof(saved));
        memcpy(items, gAiLogicData->items, sizeof(items));
        memcpy(held, gAiLogicData->holdEffects, sizeof(held));
        memcpy(abilities, gAiLogicData->abilities, sizeof(abilities));
        rng_value_t first = gRngValue, second = gRng2Value;
        struct AiCalcValues calc = {.move = MOVE_REVELATION_DANCE, .weather = AI_GetWeather(), .terrain = gFieldTimers.terrain};
        struct SimulatedDamage both = AI_CalcDancerDamage(&calc, B_BATTLER_3, B_BATTLER_0, FALSE, FALSE, FALSE, FALSE, FALSE);
        struct SimulatedDamage noSpecs = AI_CalcDancerDamage(&calc, B_BATTLER_3, B_BATTLER_0, FALSE, FALSE, FALSE, TRUE, FALSE);
        struct SimulatedDamage noVest = AI_CalcDancerDamage(&calc, B_BATTLER_3, B_BATTLER_0, FALSE, FALSE, FALSE, FALSE, TRUE);
        struct SimulatedDamage neither = AI_CalcDancerDamage(&calc, B_BATTLER_3, B_BATTLER_0, FALSE, FALSE, FALSE, TRUE, TRUE);
        EXPECT(both.minimum > noSpecs.maximum);
        EXPECT(noVest.minimum > both.maximum);
        EXPECT(neither.minimum > noSpecs.maximum);
        struct SimulatedDamage absorbed = AI_CalcDancerDamage(&calc, B_BATTLER_3, B_BATTLER_2, FALSE, FALSE, FALSE, FALSE, FALSE);
        EXPECT_EQ(absorbed.maximum, 0);
        EXPECT_EQ(memcmp(saved, gBattleMons, sizeof(saved)), 0);
        EXPECT_EQ(memcmp(items, gAiLogicData->items, sizeof(items)), 0);
        EXPECT_EQ(memcmp(held, gAiLogicData->holdEffects, sizeof(held)), 0);
        EXPECT_EQ(memcmp(abilities, gAiLogicData->abilities, sizeof(abilities)), 0);
        EXPECT_EQ(memcmp(&gRngValue, &first, sizeof(first)), 0);
        EXPECT_EQ(memcmp(&gRng2Value, &second, sizeof(second)), 0);
    }
}

DOUBLE_BATTLE_TEST("EC Dancer mechanics: failed original attacks cannot queue a copy")
{
    enum Move guard;
    enum Ability ability;
    PARAMETRIZE { guard = MOVE_PROTECT; ability = ABILITY_TELEPATHY; }
    PARAMETRIZE { guard = MOVE_CELEBRATE; ability = ABILITY_FLASH_FIRE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Ability(ability); Moves(guard); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(40); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_VOLCARONA) { Speed(100); Moves(MOVE_FIERY_DANCE); }
        OPPONENT(SPECIES_ORICORIO_POM_POM) { Speed(20); Ability(ABILITY_DANCER); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, guard); MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_FIERY_DANCE, target: playerLeft);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
        EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("EC Dancer mechanics: copied Aqua Step pays contact cost and boosts before the selected attack")
{
    s16 sourceDamage, copiedDamage, ordinaryDamage, contact;
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(1000); MaxHP(1000); Defense(100); Speed(10); Ability(ABILITY_TELEPATHY); Item(ITEM_ROCKY_HELMET); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(70); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_QUAQUAVAL) { Level(50); HP(1000); MaxHP(1000); Attack(100); Speed(100); Moves(MOVE_AQUA_STEP); }
        OPPONENT(SPECIES_ORICORIO_POM_POM) { Level(50); HP(300); MaxHP(300); Attack(100); Speed(60); Ability(ABILITY_DANCER); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_AQUA_STEP, target: playerLeft);
            MOVE(opponentRight, MOVE_TACKLE, target: playerLeft);
        }
    } SCENE {
        HP_BAR(playerLeft, captureDamage: &sourceDamage);
        HP_BAR(opponentLeft);
        HP_BAR(playerLeft, captureDamage: &copiedDamage);
        HP_BAR(opponentRight, captureDamage: &contact);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponentRight);
        HP_BAR(playerLeft, captureDamage: &ordinaryDamage);
        HP_BAR(opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerRight);
    } THEN {
        EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP - 2 * contact);
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP - sourceDamage - copiedDamage - ordinaryDamage);
    }
}

DOUBLE_BATTLE_TEST("EC Dancer mechanics: a Life Orb faint still queues the surviving dancer")
{
    s16 sourceDamage, copiedDamage;
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(1000); MaxHP(1000); SpDefense(100); Speed(10); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(40); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_VOLCARONA) { Level(50); HP(1); MaxHP(100); SpAttack(100); Speed(100); Item(ITEM_LIFE_ORB); Moves(MOVE_FIERY_DANCE); }
        OPPONENT(SPECIES_ORICORIO_POM_POM) { Level(50); SpAttack(100); Speed(20); Ability(ABILITY_DANCER); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_FIERY_DANCE, target: playerLeft);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        HP_BAR(playerLeft, captureDamage: &sourceDamage);
        HP_BAR(opponentLeft);
        HP_BAR(playerLeft, captureDamage: &copiedDamage);
    } THEN {
        EXPECT_EQ(opponentLeft->hp, 0);
        EXPECT(copiedDamage > 0);
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP - sourceDamage - copiedDamage);
    }
}

DOUBLE_BATTLE_TEST("EC Dancer mechanics: Clangorous Soul charges each successful user separately")
{
    u32 copierHp;
    PARAMETRIZE { copierHp = 300; }
    PARAMETRIZE { copierHp = 100; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(20); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_KOMMO_O) { HP(300); MaxHP(300); Speed(100); Moves(MOVE_CLANGOROUS_SOUL); }
        OPPONENT(SPECIES_ORICORIO_POM_POM) { HP(copierHp); MaxHP(300); Speed(30); Ability(ABILITY_DANCER); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CLANGOROUS_SOUL); MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->hp, 200);
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentRight->hp, copierHp > 100 ? copierHp - 100 : copierHp);
        for (u32 stat = STAT_ATK; stat <= STAT_SPDEF; stat++)
            EXPECT_EQ(opponentRight->statStages[stat], DEFAULT_STAT_STAGE + (copierHp > 100));
    }
}

DOUBLE_BATTLE_TEST("EC Dancer mechanics: Feather Dance copies the drop without a recursive chain")
{
    bool32 opposing;
    PARAMETRIZE { opposing = FALSE; }
    PARAMETRIZE { opposing = TRUE; }
    GIVEN {
        PLAYER(opposing ? SPECIES_ORICORIO_POM_POM : SPECIES_WOBBUFFET) { Speed(10); Ability(opposing ? ABILITY_DANCER : ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(20); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ORICORIO_BAILE) { Speed(100); Ability(ABILITY_DANCER); Moves(MOVE_FEATHER_DANCE); }
        OPPONENT(SPECIES_ORICORIO_POM_POM) { Speed(30); Ability(ABILITY_DANCER); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_FEATHER_DANCE, target: playerLeft); MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 4);
        EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - (opposing ? 2 : 0));
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Dancer support: copied Feather Dance enables the threatened partner's attack")
{
    GIVEN {
        AI_FLAGS(DANCER_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(90); MaxHP(90); Attack(400); SpDefense(100); Speed(60); Ability(ABILITY_TELEPATHY); Moves(MOVE_BODY_SLAM); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); SpDefense(300); Speed(10); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SWANNA) { Level(50); HP(200); MaxHP(200); SpAttack(10); Speed(100); Moves(MOVE_FEATHER_DANCE, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_ORICORIO_POM_POM) { Level(50); HP(65); MaxHP(100); Defense(100); SpAttack(300); Speed(20); Ability(ABILITY_DANCER); Moves(MOVE_REVELATION_DANCE, MOVE_PROTECT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BODY_SLAM, target: opponentRight, secondaryEffect: FALSE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_FEATHER_DANCE, target: playerLeft);
            EXPECT_MOVE(opponentRight, MOVE_REVELATION_DANCE, target: playerLeft);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 0);
        EXPECT(opponentRight->hp > 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Dancer support: Clangorous Soul advances the copied boost before an incoming knockout")
{
    GIVEN {
        AI_FLAGS(DANCER_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(90); MaxHP(90); SpDefense(100); SpAttack(400); Speed(80); Ability(ABILITY_TELEPATHY); Moves(MOVE_SLUDGE_BOMB); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); SpDefense(300); Speed(10); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_KOMMO_O) { Level(50); HP(300); MaxHP(300); Speed(100); Moves(MOVE_CLANGOROUS_SOUL); }
        OPPONENT(SPECIES_ORICORIO_POM_POM) { Level(50); HP(100); MaxHP(100); SpDefense(100); SpAttack(150); Speed(60); Ability(ABILITY_DANCER); Moves(MOVE_REVELATION_DANCE, MOVE_PROTECT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SLUDGE_BOMB, target: opponentRight, secondaryEffect: FALSE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_CLANGOROUS_SOUL);
            EXPECT_MOVE(opponentRight, MOVE_REVELATION_DANCE, target: playerLeft);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 0);
        EXPECT_EQ(opponentRight->hp, 67);
    }
}

DOUBLE_BATTLE_TEST("EC Dancer mechanics: Lunar Dance copies the sacrifice and heals both replacements")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(20); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CRESSELIA) { Speed(100); Moves(MOVE_LUNAR_DANCE); }
        OPPONENT(SPECIES_ORICORIO_POM_POM) { Speed(30); Ability(ABILITY_DANCER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(20); MaxHP(500); Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(20); MaxHP(500); Speed(40); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_LUNAR_DANCE); MOVE(opponentRight, MOVE_CELEBRATE);
            SEND_OUT(opponentLeft, 2); SEND_OUT(opponentRight, 3);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Dancer budget: mixed copied attacks and full benches stay below 1.2 seconds")
{
    GIVEN {
        AI_FLAGS(DANCER_FLAGS);
        PLAYER(SPECIES_VOLCARONA) { Level(50); HP(1000); MaxHP(1000); Speed(110); Item(ITEM_LIFE_ORB); Moves(MOVE_FIERY_DANCE, MOVE_PROTECT, MOVE_HEAT_WAVE, MOVE_QUIVER_DANCE); }
        PLAYER(SPECIES_ORICORIO_POM_POM) { Level(50); HP(1000); MaxHP(1000); Speed(100); Item(ITEM_SITRUS_BERRY); Ability(ABILITY_DANCER); Moves(MOVE_REVELATION_DANCE, MOVE_ACROBATICS, MOVE_FEATHER_DANCE, MOVE_PROTECT); }
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(1000); MaxHP(1000); Speed(20); Moves(MOVE_PSYCHIC); }
        PLAYER(SPECIES_GYARADOS) { Level(50); HP(1000); MaxHP(1000); Speed(30); Moves(MOVE_WATERFALL); }
        PLAYER(SPECIES_VENUSAUR) { Level(50); HP(1000); MaxHP(1000); Speed(40); Moves(MOVE_PETAL_DANCE); }
        PLAYER(SPECIES_PIKACHU) { Level(50); HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_LILLIGANT_HISUI) { Level(50); HP(1000); MaxHP(1000); Speed(90); Item(ITEM_LIFE_ORB); Moves(MOVE_VICTORY_DANCE, MOVE_CLOSE_COMBAT, MOVE_PETAL_DANCE, MOVE_PROTECT); }
        OPPONENT(SPECIES_ORICORIO_BAILE) { Level(50); HP(1000); MaxHP(1000); Speed(80); Item(ITEM_SITRUS_BERRY); Ability(ABILITY_DANCER); Moves(MOVE_REVELATION_DANCE, MOVE_AIR_SLASH, MOVE_FEATHER_DANCE, MOVE_PROTECT); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); HP(1000); MaxHP(1000); Speed(21); Moves(MOVE_PSYCHIC); }
        OPPONENT(SPECIES_GYARADOS) { Level(50); HP(1000); MaxHP(1000); Speed(31); Moves(MOVE_WATERFALL); }
        OPPONENT(SPECIES_VENUSAUR) { Level(50); HP(1000); MaxHP(1000); Speed(41); Moves(MOVE_PETAL_DANCE); }
        OPPONENT(SPECIES_PIKACHU) { Level(50); HP(1000); MaxHP(1000); Speed(51); Moves(MOVE_THUNDERBOLT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FIERY_DANCE, target: opponentLeft);
            MOVE(playerRight, MOVE_REVELATION_DANCE, target: opponentRight);
        }
    } THEN {
        Test_MgbaPrintf("DANCER_FULL_BENCH_DECISION_FRAMES=%d", gBattleStruct->aiDelayFrames);
        EXPECT(gBattleStruct->aiDelayFrames <= 72);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Dancer item sequence: a consumed Wacan Berry cannot protect against the selected attack")
{
    GIVEN {
        AI_FLAGS(DANCER_FLAGS);
        PLAYER(SPECIES_GYARADOS) { Level(50); HP(320); MaxHP(320); Attack(400); SpDefense(100); Speed(60); Ability(ABILITY_MOXIE); Item(ITEM_WACAN_BERRY); Moves(MOVE_ROCK_SLIDE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(500); MaxHP(500); SpDefense(300); Speed(10); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ORICORIO_BAILE) { Level(50); HP(200); MaxHP(200); SpAttack(100); Speed(100); Ability(ABILITY_DANCER); Moves(MOVE_REVELATION_DANCE, MOVE_FLAMETHROWER); }
        OPPONENT(SPECIES_ORICORIO_POM_POM) { Level(50); HP(100); MaxHP(100); SpAttack(100); Speed(80); Ability(ABILITY_DANCER); Moves(MOVE_REVELATION_DANCE, MOVE_PROTECT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ROCK_SLIDE, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_REVELATION_DANCE, target: playerLeft);
            EXPECT_MOVE(opponentRight, MOVE_REVELATION_DANCE, target: playerLeft);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 0);
        EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
    }
}

DOUBLE_BATTLE_TEST("EC Dancer item mechanics: Wacan Berry reduces only the first of two Revelation Dances")
{
    s16 sourceDamage, copiedDamage, selectedDamage;
    GIVEN {
        PLAYER(SPECIES_GYARADOS) { Level(50); HP(1000); MaxHP(1000); SpDefense(100); Speed(30); Ability(ABILITY_MOXIE); Item(ITEM_WACAN_BERRY); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ORICORIO_BAILE) { Level(50); SpAttack(100); Speed(100); Ability(ABILITY_DANCER); Moves(MOVE_REVELATION_DANCE); }
        OPPONENT(SPECIES_ORICORIO_POM_POM) { Level(50); SpAttack(100); Speed(80); Ability(ABILITY_DANCER); Moves(MOVE_REVELATION_DANCE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_REVELATION_DANCE, target: playerLeft);
            MOVE(opponentRight, MOVE_REVELATION_DANCE, target: playerLeft);
        }
    } SCENE {
        HP_BAR(playerLeft, captureDamage: &sourceDamage);
        HP_BAR(playerLeft, captureDamage: &copiedDamage);
        HP_BAR(playerLeft, captureDamage: &selectedDamage);
        HP_BAR(playerLeft);
    } THEN {
        EXPECT(selectedDamage > copiedDamage * 3 / 2);
        EXPECT_EQ(playerLeft->item, ITEM_NONE);
        Test_MgbaPrintf("WACAN_SOURCE=%d FIRST_COPY=%d SELECTED=%d", sourceDamage, copiedDamage, selectedDamage);
    }
}

AI_DOUBLE_BATTLE_TEST("EC item sequence: ordinary attacks share the consumed resist Berry state")
{
    bool32 plusMinus;
    PARAMETRIZE { plusMinus = FALSE; }
    PARAMETRIZE { plusMinus = TRUE; }
    GIVEN {
        AI_FLAGS(DANCER_FLAGS);
        PLAYER(SPECIES_GYARADOS) { Level(50); HP(plusMinus ? 320 : 220); MaxHP(plusMinus ? 320 : 220); Attack(400); SpDefense(100); Speed(60); Ability(ABILITY_MOXIE); Item(ITEM_WACAN_BERRY); Moves(MOVE_ROCK_SLIDE); }
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(500); MaxHP(500); SpDefense(300); Speed(10); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIKACHU) { Level(50); HP(100); MaxHP(100); SpAttack(100); Speed(100); Ability(plusMinus ? ABILITY_PLUS : ABILITY_STATIC); Moves(MOVE_THUNDER_SHOCK); }
        OPPONENT(SPECIES_RAICHU) { Level(50); HP(100); MaxHP(100); SpAttack(100); Speed(80); Ability(plusMinus ? ABILITY_MINUS : ABILITY_STATIC); Moves(MOVE_THUNDERBOLT, MOVE_PROTECT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ROCK_SLIDE, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_THUNDER_SHOCK, target: playerLeft);
            EXPECT_MOVE(opponentRight, MOVE_THUNDERBOLT, target: playerLeft);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 0);
        EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
    }
}

AI_DOUBLE_BATTLE_TEST("EC item sequence: Knock Off removes a defensive boost only when the item can be lost")
{
    bool32 sticky;
    PARAMETRIZE { sticky = FALSE; }
    PARAMETRIZE { sticky = TRUE; }
    GIVEN {
        AI_FLAGS(DANCER_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(100); MaxHP(100); Defense(100); SpAttack(400); SpDefense(100); Speed(60); Ability(sticky ? ABILITY_STICKY_HOLD : ABILITY_TELEPATHY); Item(ITEM_ASSAULT_VEST); Moves(MOVE_SLUDGE_BOMB); }
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(500); MaxHP(500); SpDefense(300); Speed(10); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SCRAFTY) { Level(50); HP(100); MaxHP(100); Attack(10); Speed(100); Ability(ABILITY_MOXIE); Moves(MOVE_KNOCK_OFF); }
        OPPONENT(SPECIES_RAICHU) { Level(50); HP(100); MaxHP(100); SpAttack(200); SpDefense(100); Speed(80); Moves(MOVE_THUNDERBOLT, MOVE_PROTECT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SLUDGE_BOMB, target: opponentRight, secondaryEffect: FALSE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_CELEBRATE);
            if (!sticky)
                EXPECT_MOVE(opponentLeft, MOVE_KNOCK_OFF, target: playerLeft);
            if (sticky)
                EXPECT_MOVE(opponentRight, MOVE_PROTECT);
            else
                EXPECT_MOVE(opponentRight, MOVE_THUNDERBOLT, target: playerLeft);
        }
    } THEN {
        if (sticky)
        {
            EXPECT_EQ(playerLeft->item, ITEM_ASSAULT_VEST);
            EXPECT(playerLeft->hp > 0);
        }
        else
        {
            EXPECT_EQ(playerLeft->item, ITEM_NONE);
            EXPECT_EQ(playerLeft->hp, 0);
        }
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
    }
}
