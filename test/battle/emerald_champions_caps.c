#include "global.h"
#include "test/battle.h"
#include "battle_controllers.h"
#include "battle_util.h"
#include "caps.h"
#include "event_data.h"
#include "string_util.h"
#include "constants/flags.h"
#include "constants/characters.h"

DOUBLE_BATTLE_TEST("Emerald Champions normalized captures obey regardless of their historical met level")
{
    GIVEN {
        PLAYER(SPECIES_MEWTWO) { Level(12); Moves(MOVE_SPLASH); }
        PLAYER(SPECIES_MAGIKARP) { Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_MAGIKARP) { Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_MAGIKARP) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SPLASH); MOVE(playerRight, MOVE_SPLASH); MOVE(opponentLeft, MOVE_SPLASH); MOVE(opponentRight, MOVE_SPLASH); }
    } THEN {
        u32 savedFlags = gBattleTypeFlags;
        enum BattlerId savedAttacker = gBattlerAttacker;
        struct BattlePokemon savedMon = *playerLeft;
        bool32 rainBadge = FlagGet(FLAG_BADGE08_GET);
        FlagClear(FLAG_BADGE08_GET);
        gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_IS_MASTER;
        gBattlerAttacker = B_BATTLER_0;
        playerLeft->level = GetPlayerLevelCapForSpecies(SPECIES_MEWTWO);
        playerLeft->metLevel = 100;
        memcpy(&playerLeft->otId, gSaveBlock2Ptr->playerTrainerId, sizeof(playerLeft->otId));
        // Function fixtures do not initialize a terminated save-file name.
        memcpy(playerLeft->otName, gSaveBlock2Ptr->playerName, PLAYER_NAME_LENGTH);
        playerLeft->otName[PLAYER_NAME_LENGTH] = EOS;
        EXPECT_LT(playerLeft->level, playerLeft->metLevel);
        EXPECT(!BattlerHasAi(B_BATTLER_0));
        EXPECT(!IsOtherTrainer(playerLeft->otId, playerLeft->otName));
        for (u32 i = 0; i < 32; i++)
            EXPECT_EQ(GetAttackerObedienceForAction(), OBEYS);
        *playerLeft = savedMon;
        gBattlerAttacker = savedAttacker;
        gBattleTypeFlags = savedFlags;
        if (rainBadge)
            FlagSet(FLAG_BADGE08_GET);
    }
}
