#include "global.h"
#include "test/battle.h"
#include "constants/opponents.h"

AI_DOUBLE_BATTLE_TEST("EC weather survival: Cherubi uses timely support before lethal Wingbeat")
{
    enum Move setup = MOVE_SUNNY_DAY;
    enum Move incoming = MOVE_DUAL_WINGBEAT;
    enum Ability attackerAbility = ABILITY_UNNERVE;
    PARAMETRIZE { }
    PARAMETRIZE { attackerAbility = ABILITY_NO_GUARD; }
    PARAMETRIZE { setup = MOVE_RAIN_DANCE; }
    PARAMETRIZE { setup = MOVE_SANDSTORM; }
    PARAMETRIZE { setup = MOVE_SNOWSCAPE; }
    PARAMETRIZE { setup = MOVE_TAILWIND; }
    PARAMETRIZE { setup = MOVE_TRICK_ROOM; }
    PARAMETRIZE { incoming = MOVE_PROTECT; }
    GIVEN {
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_LOLA_1;
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_AERODACTYL) {
            Level(24); HP(80); MaxHP(80); Attack(77); Defense(43);
            SpAttack(36); SpDefense(48); Speed(97);
            Ability(attackerAbility); Item(ITEM_FOCUS_SASH); Moves(incoming);
        }
        PLAYER(SPECIES_SHAYMIN) {
            Level(24); HP(89); MaxHP(89); Attack(54); Defense(60);
            SpAttack(75); SpDefense(60); Speed(82);
            Ability(ABILITY_NATURAL_CURE); Item(ITEM_LIFE_ORB); Moves(MOVE_PSYCHIC);
        }
        OPPONENT(SPECIES_CHERUBI) {
            Level(23); HP(61); MaxHP(61); Attack(25); Defense(32);
            SpAttack(60); SpDefense(36); Speed(42);
            Ability(ABILITY_CHLOROPHYLL); Item(ITEM_FOCUS_SASH);
            Moves(setup, MOVE_ENERGY_BALL, MOVE_WEATHER_BALL, MOVE_HELPING_HAND);
        }
        OPPONENT(SPECIES_LEAFEON) {
            Level(24); HP(72); MaxHP(72); Attack(88); Defense(74);
            SpAttack(36); SpDefense(43); Speed(73);
            Ability(ABILITY_CHLOROPHYLL); Item(ITEM_LIFE_ORB);
            Moves(MOVE_LEAF_BLADE, MOVE_DOUBLE_EDGE, MOVE_X_SCISSOR, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            if (incoming == MOVE_PROTECT)
                MOVE(playerLeft, MOVE_PROTECT);
            else
                MOVE(playerLeft, incoming, target: opponentLeft);
            MOVE(playerRight, MOVE_PSYCHIC, target: opponentRight);
            EXPECT_MOVE(opponentLeft, incoming == MOVE_PROTECT ? MOVE_SUNNY_DAY : MOVE_HELPING_HAND);
        }
    }
}
