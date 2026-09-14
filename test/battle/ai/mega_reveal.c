#include "global.h"
#include "battle.h"
#include "battle_util.h"
#include "test/battle.h"
#include "constants/opponents.h"

AI_DOUBLE_BATTLE_TEST("EC Mega reveal: shows the form against guarding foes without inventing eligibility")
{
    u16 item;
    PARAMETRIZE { item = ITEM_AERODACTYLITE; }
    PARAMETRIZE { item = ITEM_NONE; }
    GIVEN {
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_ROXANNE_1;
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY
            | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Level(14); Moves(MOVE_PROTECT); }
        PLAYER(SPECIES_WOBBUFFET) { Level(14); Moves(MOVE_PROTECT); }
        OPPONENT(SPECIES_AERODACTYL) {
            Level(14); Ability(ABILITY_UNNERVE); Item(item);
            Moves(MOVE_ROCK_SLIDE, MOVE_DUAL_WINGBEAT, MOVE_TAILWIND, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_CARBINK) {
            Level(17); Ability(ABILITY_STURDY); Item(ITEM_LIGHT_CLAY);
            Moves(MOVE_REFLECT, MOVE_LIGHT_SCREEN, MOVE_BODY_PRESS, MOVE_ROCK_TOMB);
        }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        EXPECT_EQ(opponentLeft->species, item == ITEM_AERODACTYLITE ? SPECIES_AERODACTYL_MEGA : SPECIES_AERODACTYL);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_1), item == ITEM_AERODACTYLITE ? ABILITY_TOUGH_CLAWS : ABILITY_UNNERVE);
    }
}
