#include "global.h"
#include "battle.h"
#include "battle_ai_util.h"
#include "battle_setup.h"
#include "emerald_champions_perish.h"
#include "test/battle.h"
#include "constants/opponents.h"

// Policy guards are tested on an initialized native battle. This does not
// replace the authored multi-turn Maura test or the deadline-switch test.
AI_DOUBLE_BATTLE_TEST("EC Perish policy: fresh trapped targets, escape timing and immunity guards")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_BULBASAUR) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_JYNX) { Ability(ABILITY_DRY_SKIN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GOTHITELLE) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_AMOONGUSS) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SLOWBRO) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { }
    } THEN {
        u32 flags = gBattleTypeFlags;
        u16 trainer = TRAINER_BATTLE_PARAM.opponentA;
        gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
        TRAINER_BATTLE_PARAM.opponentA = TRAINER_MAURA;

        EXPECT(EC_PerishPlanScore(B_BATTLER_1, MOVE_PERISH_SONG) > 0);
        EXPECT(!EC_PerishMustEscape(B_BATTLER_1));
        EXPECT(!EC_PerishShouldPivotEarly(B_BATTLER_1));

        // Ghosts and Shed Shell can manually leave Shadow Tag. Neither is a
        // guaranteed trapped target even though an opposing bench exists.
        gBattleMons[B_BATTLER_0].types[0] = TYPE_GHOST;
        gBattleMons[B_BATTLER_0].types[1] = TYPE_GHOST;
        gAiLogicData->holdEffects[B_BATTLER_2] = HOLD_EFFECT_SHED_SHELL;
        EXPECT_EQ(EC_PerishPlanScore(B_BATTLER_1, MOVE_PERISH_SONG), 0);

        gBattleMons[B_BATTLER_0].types[0] = TYPE_PSYCHIC;
        gBattleMons[B_BATTLER_0].types[1] = TYPE_PSYCHIC;
        gAiLogicData->holdEffects[B_BATTLER_2] = HOLD_EFFECT_NONE;
        gAiLogicData->abilities[B_BATTLER_0] = ABILITY_SOUNDPROOF;
        gAiLogicData->abilities[B_BATTLER_2] = ABILITY_SOUNDPROOF;
        EXPECT_EQ(EC_PerishPlanScore(B_BATTLER_1, MOVE_PERISH_SONG), -10000);

        gAiLogicData->abilities[B_BATTLER_0] = ABILITY_TELEPATHY;
        gAiLogicData->abilities[B_BATTLER_2] = ABILITY_TELEPATHY;
        for (enum BattlerId battler = 0; battler < gBattlersCount; battler++)
        {
            gBattleMons[battler].volatiles.perishSong = TRUE;
            gBattleMons[battler].volatiles.perishSongTimer = 1;
        }
        EXPECT_EQ(EC_PerishPlanScore(B_BATTLER_1, MOVE_PERISH_SONG), -10000);
        EXPECT(EC_PerishShouldPivotEarly(B_BATTLER_1));
        EXPECT(!EC_PerishShouldPivotEarly(B_BATTLER_3));
        EXPECT(!EC_PerishMustEscape(B_BATTLER_1));

        // Losing the trapper's active ability removes the authored early-exit
        // preference. At the actual deadline, the volatile remains decisive.
        gAiLogicData->abilities[B_BATTLER_3] = ABILITY_NONE;
        EXPECT(!EC_PerishShouldPivotEarly(B_BATTLER_1));
        gBattleMons[B_BATTLER_1].volatiles.perishSongTimer = 0;
        gAiLogicData->abilities[B_BATTLER_1] = ABILITY_SOUNDPROOF;
        EXPECT(EC_PerishMustEscape(B_BATTLER_1));
        gBattleMons[B_BATTLER_1].volatiles.perishSong = FALSE;
        EXPECT(!EC_PerishMustEscape(B_BATTLER_1));

        TRAINER_BATTLE_PARAM.opponentA = trainer;
        gBattleTypeFlags = flags;
    }
}
