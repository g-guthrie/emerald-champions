#include "global.h"
#include "battle.h"
#include "battle_main.h"
#include "battle_setup.h"
#include "emerald_champions_battle_plan.h"
#include "test/battle.h"
#include "constants/opponents.h"

// Buffel fields the 2016 Big Six under 2016 rules; everyone else keeps current rules.
TEST("Vintage rules: only Buffel's own battlers use them")
{
    u32 savedFlags = gBattleTypeFlags;
    TrainerBattleParameter savedParams = gTrainerBattleParameter;

    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_IS_MASTER;
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_BUFFEL;
    EXPECT(EmeraldChampions_UsesVintageRules(B_BATTLER_1));
    EXPECT(EmeraldChampions_UsesVintageRules(B_BATTLER_3));
    EXPECT(!EmeraldChampions_UsesVintageRules(B_BATTLER_0));
    EXPECT(!EmeraldChampions_UsesVintageRules(B_BATTLER_2));

    TRAINER_BATTLE_PARAM.opponentA = TRAINER_STEVEN;
    EXPECT(!EmeraldChampions_UsesVintageRules(B_BATTLER_1));

    TRAINER_BATTLE_PARAM.opponentA = TRAINER_BUFFEL;
    gBattleTypeFlags = BATTLE_TYPE_DOUBLE;
    EXPECT(!EmeraldChampions_UsesVintageRules(B_BATTLER_1));

    gBattleTypeFlags = savedFlags;
    gTrainerBattleParameter = savedParams;
}

DOUBLE_BATTLE_TEST("Vintage rules: Buffel's Gale Wings keeps priority below full HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_TALONFLAME) { Ability(ABILITY_GALE_WINGS); HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        u32 savedFlags = gBattleTypeFlags;
        TrainerBattleParameter savedParams = gTrainerBattleParameter;

        gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_IS_MASTER;
        TRAINER_BATTLE_PARAM.opponentA = TRAINER_BUFFEL;
        EXPECT_EQ(GetBattleMovePriority(B_BATTLER_1, ABILITY_GALE_WINGS, MOVE_BRAVE_BIRD), 1);
        TRAINER_BATTLE_PARAM.opponentA = TRAINER_STEVEN;
        EXPECT_EQ(GetBattleMovePriority(B_BATTLER_1, ABILITY_GALE_WINGS, MOVE_BRAVE_BIRD), 0);

        gBattleTypeFlags = savedFlags;
        gTrainerBattleParameter = savedParams;
    }
}
