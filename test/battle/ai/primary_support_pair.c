#include "global.h"
#include "test/battle.h"
#include "constants/opponents.h"

#define SUPPORT_FLAGS (AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING \
    | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE \
    | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE)

AI_DOUBLE_BATTLE_TEST("EC primary support: timely special-defense drops enable Swirlix's knockout")
{
    enum Move support = MOVE_FAKE_TEARS;
    enum Ability ability = ABILITY_SHADOW_TAG;
    enum Item item = ITEM_NONE;
    u32 setterSpeed = 100;
    bool32 enabled = TRUE;
    PARAMETRIZE { }
    PARAMETRIZE { item = ITEM_COVERT_CLOAK; }
    PARAMETRIZE { item = ITEM_CLEAR_AMULET; enabled = FALSE; }
    PARAMETRIZE { item = ITEM_WHITE_HERB; enabled = FALSE; }
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; enabled = FALSE; }
    PARAMETRIZE { ability = ABILITY_MAGIC_BOUNCE; enabled = FALSE; }
    PARAMETRIZE { ability = ABILITY_CONTRARY; enabled = FALSE; }
    PARAMETRIZE { setterSpeed = 20; enabled = FALSE; }
    PARAMETRIZE { support = MOVE_METAL_SOUND; }
    PARAMETRIZE { support = MOVE_METAL_SOUND; ability = ABILITY_SOUNDPROOF; enabled = FALSE; }
    GIVEN {
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_TIANA;
        AI_FLAGS(SUPPORT_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(68); MaxHP(68); SpDefense(100); SpAttack(200); Speed(80); Ability(ability); Item(item); Moves(MOVE_SLUDGE_BOMB); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SKITTY) { Level(50); Speed(setterSpeed); Moves(support); }
        OPPONENT(SPECIES_SWIRLIX) { Level(50); HP(80); MaxHP(80); SpAttack(100); SpDefense(50); Speed(90); Moves(MOVE_THUNDERBOLT, MOVE_PROTECT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SLUDGE_BOMB, target: opponentRight, criticalHit: FALSE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, support, hit: TRUE);
            if (enabled)
                EXPECT_MOVE(opponentRight, MOVE_THUNDERBOLT, target: playerLeft, criticalHit: FALSE);
            else
                EXPECT_MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_GT(opponentRight->hp, 0);
        if (enabled)
            EXPECT_EQ(playerLeft->hp, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC primary support: String Shot creates a same-turn speed crossing")
{
    enum Move support = MOVE_STRING_SHOT;
    enum Ability ability = ABILITY_SHADOW_TAG;
    enum Item item = ITEM_NONE;
    u32 setterSpeed = 100;
    bool32 enabled = TRUE;
    PARAMETRIZE { }
    PARAMETRIZE { item = ITEM_COVERT_CLOAK; }
    PARAMETRIZE { item = ITEM_CLEAR_AMULET; enabled = FALSE; }
    PARAMETRIZE { item = ITEM_WHITE_HERB; enabled = FALSE; }
    PARAMETRIZE { ability = ABILITY_CLEAR_BODY; enabled = FALSE; }
    PARAMETRIZE { ability = ABILITY_CONTRARY; enabled = FALSE; }
    PARAMETRIZE { setterSpeed = 20; enabled = FALSE; }
    PARAMETRIZE { support = MOVE_COTTON_SPORE; }
    PARAMETRIZE { support = MOVE_SCARY_FACE; }
    GIVEN {
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_RICK;
        AI_FLAGS(SUPPORT_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { Level(50); HP(60); MaxHP(60); Defense(100); SpAttack(200); Speed(80); Ability(ability); Item(item); Moves(MOVE_SLUDGE_BOMB); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SPINARAK) { Level(50); Speed(setterSpeed); Item(ITEM_CHOICE_SCARF); Moves(support); }
        OPPONENT(SPECIES_KARRABLAST) { Level(50); HP(60); MaxHP(60); Attack(100); SpDefense(50); Speed(50); Ability(ABILITY_NO_GUARD); Moves(MOVE_MEGAHORN, MOVE_PROTECT); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SLUDGE_BOMB, target: opponentRight, criticalHit: FALSE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, support, hit: TRUE);
            if (enabled)
                EXPECT_MOVE(opponentRight, MOVE_MEGAHORN, target: playerLeft, criticalHit: FALSE);
            else
                EXPECT_MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_GT(opponentRight->hp, 0);
        if (enabled)
            EXPECT_EQ(playerLeft->hp, 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC primary support: Acid Spray enables only a later unblocked special attack")
{
    u32 setterSpeed;
    enum Item item;
    bool32 shouldSpray;
    PARAMETRIZE { setterSpeed = 100; item = ITEM_NONE; shouldSpray = TRUE; }
    PARAMETRIZE { setterSpeed = 10; item = ITEM_NONE; shouldSpray = FALSE; }
    PARAMETRIZE { setterSpeed = 100; item = ITEM_COVERT_CLOAK; shouldSpray = FALSE; }
    GIVEN {
        AI_FLAGS(SUPPORT_FLAGS);
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
