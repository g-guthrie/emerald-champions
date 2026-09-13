#include "global.h"
#include "test/battle.h"
#include "constants/opponents.h"

AI_DOUBLE_BATTLE_TEST("EC timely Taunt: Mantyke attacks instead of attempting blocked Tailwind")
{
    bool32 denied = TRUE;
    bool32 cast = TRUE;
    bool32 timely = TRUE;
    enum Ability attackerAbility = ABILITY_UNNERVE;
    enum Ability targetAbility = ABILITY_WATER_ABSORB;
    enum Ability partnerAbility = ABILITY_SHEER_FORCE;
    enum Item item = ITEM_EVIOLITE;
    enum Species targetSpecies = SPECIES_MANTYKE;
    PARAMETRIZE { }
    PARAMETRIZE { timely = FALSE; denied = FALSE; }
    PARAMETRIZE { cast = FALSE; denied = FALSE; }
    PARAMETRIZE { item = ITEM_MENTAL_HERB; denied = FALSE; }
    PARAMETRIZE { targetAbility = ABILITY_OBLIVIOUS; denied = FALSE; }
    PARAMETRIZE { targetAbility = ABILITY_OBLIVIOUS; attackerAbility = ABILITY_MOLD_BREAKER; }
    PARAMETRIZE { partnerAbility = ABILITY_AROMA_VEIL; denied = FALSE; }
    PARAMETRIZE { targetAbility = ABILITY_MAGIC_BOUNCE; denied = FALSE; }
    PARAMETRIZE { targetAbility = ABILITY_GOOD_AS_GOLD; denied = FALSE; }
    PARAMETRIZE { attackerAbility = ABILITY_PRANKSTER; targetSpecies = SPECIES_MURKROW; denied = FALSE; }
    GIVEN {
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_EDMOND;
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_AERODACTYL) {
            Level(24); HP(80); MaxHP(80); Attack(77); Defense(43);
            SpAttack(36); SpDefense(48); Speed(timely ? 97 : 20);
            Ability(attackerAbility); Item(ITEM_FOCUS_SASH); Moves(MOVE_TAUNT, MOVE_CELEBRATE);
        }
        PLAYER(SPECIES_SHAYMIN) {
            Level(24); HP(89); MaxHP(89); Attack(54); Defense(60);
            SpAttack(75); SpDefense(60); Speed(82);
            Ability(ABILITY_NATURAL_CURE); Item(ITEM_LIFE_ORB); Moves(MOVE_GIGA_DRAIN);
        }
        OPPONENT(targetSpecies) {
            Level(23); HP(75); MaxHP(75); Attack(18); Defense(35);
            SpAttack(39); SpDefense(89); Speed(35);
            Ability(targetAbility); Item(item);
            Moves(MOVE_SCALD, MOVE_TAILWIND, MOVE_HELPING_HAND, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_KRABBY) {
            Level(24); HP(56); MaxHP(56); Attack(84); Defense(55);
            SpAttack(21); SpDefense(24); Speed(51);
            Ability(partnerAbility); Item(ITEM_EVIOLITE);
            Moves(MOVE_LIQUIDATION, MOVE_ROCK_SLIDE, MOVE_X_SCISSOR, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, cast ? MOVE_TAUNT : MOVE_CELEBRATE, target: opponentLeft);
            MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentRight);
            EXPECT_MOVE(opponentLeft, denied ? MOVE_SCALD : MOVE_TAILWIND);
            EXPECT_MOVE(opponentRight, MOVE_PROTECT);
        }
    } THEN {
        if (denied)
            EXPECT_LT(playerLeft->hp, 80);
        else
            EXPECT(gSideStatuses[1] & SIDE_STATUS_TAILWIND);
        EXPECT_EQ(opponentRight->hp, 56);
    }
}
