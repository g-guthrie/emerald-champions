#include "global.h"
#include "battle.h"
#include "battle_script_commands.h"
#include "battle_util.h"
#include "mail.h"
#include "malloc.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "party_menu.h"
#include "test/test.h"

extern void Test_GiveCaughtMonStep(u32 phase);

TEST("Capture Mail: catch-and-swap preserves the holder and written message")
{
    bool32 special;
    PARAMETRIZE { special = FALSE; }
    PARAMETRIZE { special = TRUE; }
    struct BattleResources *savedResources = gBattleResources;
    struct BattleStruct *savedBattle = gBattleStruct;
    const u8 *savedScript = gBattlescriptCurrInstr;
    static const u8 command[5] = {0};
    gBattleResources = AllocZeroed(sizeof(*gBattleResources));
    gBattleStruct = AllocZeroed(sizeof(*gBattleStruct));
    gBattlescriptCurrInstr = command;
    gBattleTypeFlags = 0;
    gBattlersCount = 2;
    gBattlerAttacker = 0;
    gBattlerTarget = 1;
    gBattlerPositions[0] = B_POSITION_PLAYER_LEFT;
    gBattlerPositions[1] = B_POSITION_OPPONENT_LEFT;
    gBattlerPartyIndexes[0] = gBattlerPartyIndexes[1] = 0;
    memset(gBattleMons, 0, sizeof(gBattleMons));
    gBattleMons[0].hp = gBattleMons[1].hp = 100;
    memset(gBattleCommunication, 0, sizeof(gBattleCommunication));
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    ResetPokemonStorageSystem();
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        CreateMon(&gParties[B_TRAINER_PLAYER][slot], special && slot == 1 ? SPECIES_KARTANA : SPECIES_EEVEE, 20, 0, OTID_STRUCT_PLAYER_ID);
    enum Species caught = special ? SPECIES_FLUTTER_MANE : SPECIES_PIKACHU;
    CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], caught, 20, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    struct Pokemon *holder = &gParties[B_TRAINER_PLAYER][1];
    u8 mailId = GiveMailToMonByItemId(holder, ITEM_ORANGE_MAIL);
    ASSUME(mailId != MAIL_NONE);
    gSaveBlock1Ptr->mail[mailId].words[0] = 123;
    struct Mail message = gSaveBlock1Ptr->mail[mailId];
    struct Pokemon original = *holder;
    RecordPlayerPartyMonHeldItemForRestoration(1);

    if (special)
    {
        // The named special-slot shortcut must not offer an illegal deposit.
        gBattleControllerExecFlags = 0;
        Test_GiveCaughtMonStep(0);
        EXPECT_EQ(gBattleControllerExecFlags, 0);
    }

    // Even a stale selection must not bypass the native Mail deposit rule.
    gSelectedMonPartyId = 1;
    Test_GiveCaughtMonStep(1);
    EXPECT_EQ(memcmp(holder, &original, sizeof(original)), 0);
    Test_GiveCaughtMonStep(2);
    EXPECT_EQ(memcmp(holder, &original, sizeof(original)), 0);
    EXPECT_EQ(memcmp(&gSaveBlock1Ptr->mail[mailId], &message, sizeof(message)), 0);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), caught);
    EXPECT_EQ(GetBoxMonDataAt(0, 1, MON_DATA_SPECIES), SPECIES_NONE);
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], PARTY_SIZE);
    EXPECT(PlayerPartyWithinRestrictedLimit());

    Free(gBattleStruct);
    Free(gBattleResources);
    gBattleStruct = savedBattle;
    gBattleResources = savedResources;
    gBattlescriptCurrInstr = savedScript;
    gBattleControllerExecFlags = 0;
    TakeMailFromMon(holder);
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    ResetPokemonStorageSystem();
}
