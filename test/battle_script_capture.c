#include "global.h"
#include "battle.h"
#include "battle_script_commands.h"
#include "battle_controllers.h"
#include "malloc.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "party_menu.h"
#include "string_util.h"
#include "constants/battle_string_ids.h"
#include "event_data.h"
#include "test/test.h"

u32 Test_ComputeCaptureOdds(u32 wildMonBattler, u32 playerBattler);

static void SetCaptureBoard(enum Species species)
{
    memset(gBattleMons, 0, sizeof(gBattleMons));
    gBattleTypeFlags = 0;
    gBattleMons[0].level = 50;
    gBattleMons[1].species = species;
    gBattleMons[1].level = 50;
    gBattleMons[1].hp = gBattleMons[1].maxHP = 100;
    for (u32 flag = FLAG_BADGE01_GET; flag <= FLAG_BADGE08_GET; flag++)
        FlagSet(flag);
}

TEST("Battle script capture: Heavy Ball penalties cannot wrap into guaranteed catches")
{
    SetCaptureBoard(SPECIES_AZELF);
    ASSUME(gSpeciesInfo[SPECIES_AZELF].catchRate < 20);
    ASSUME(gSpeciesInfo[SPECIES_AZELF].weight < 1000);
    gLastUsedItem = ITEM_POKE_BALL;
    u32 normal = Test_ComputeCaptureOdds(1, 0);
    gLastUsedItem = ITEM_HEAVY_BALL;
    u32 heavy = Test_ComputeCaptureOdds(1, 0);
    EXPECT_GT(heavy, 0);
    EXPECT_LE(heavy, normal);
    EXPECT_LT(heavy, 255);
}

TEST("Battle script capture: very low ball odds remain valid for the shake calculation")
{
    SetCaptureBoard(SPECIES_AZELF);
    gLastUsedItem = ITEM_BEAST_BALL;
    EXPECT_GT(Test_ComputeCaptureOdds(1, 0), 0);
    EXPECT_LT(Test_ComputeCaptureOdds(1, 0), 255);
    gLastUsedItem = ITEM_MASTER_BALL;
    EXPECT_EQ(Test_ComputeCaptureOdds(1, 0), (u32)-1);
}

TEST("Battle script capture: Heavy Balls still reward heavy targets")
{
    SetCaptureBoard(SPECIES_SNORLAX);
    ASSUME(gSpeciesInfo[SPECIES_SNORLAX].weight >= 3000);
    gLastUsedItem = ITEM_POKE_BALL;
    u32 normal = Test_ComputeCaptureOdds(1, 0);
    gLastUsedItem = ITEM_HEAVY_BALL;
    EXPECT_GT(Test_ComputeCaptureOdds(1, 0), normal);
    EXPECT_LT(Test_ComputeCaptureOdds(1, 0), 255);
}

TEST("Battle script capture: special catches offer a named swap and preserve both Pokemon")
{
    bool32 accept, full;
    for (u32 choice = 0; choice < 2; choice++)
    {
        PARAMETRIZE { accept = choice; full = FALSE; }
        PARAMETRIZE { accept = choice; full = TRUE; }
    }
    extern void Test_GiveCaughtMonStep(u32 phase);
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
    for (u32 slot = 0; slot < (full ? PARTY_SIZE : 2); slot++)
        CreateMon(&gParties[B_TRAINER_PLAYER][slot], slot == 1 ? SPECIES_KARTANA : SPECIES_EEVEE, 20, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_FLUTTER_MANE, 20, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    EXPECT(!IsCaughtMonStorageFull());
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            gPokemonStoragePtr->boxes[box][slot] = gParties[B_TRAINER_PLAYER][0].box;
    EXPECT(IsCaughtMonStorageFull());
    ResetPokemonStorageSystem();
    Test_GiveCaughtMonStep(0);
    EXPECT_EQ(gBattleResources->bufferA[0][2] | (gBattleResources->bufferA[0][3] << 8), STRINGID_SWAPSPECIALMON);
    u8 nickname[POKEMON_NAME_LENGTH + 1];
    GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_NICKNAME, nickname);
    EXPECT_EQ(StringCompare(gStringVar1, nickname), 0);
    if (accept)
    {
        gSelectedMonPartyId = GetUniquePartyRestrictedSlot();
        EXPECT_EQ(gSelectedMonPartyId, 1);
        Test_GiveCaughtMonStep(1);
    }
    Test_GiveCaughtMonStep(2);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SPECIES), accept ? SPECIES_FLUTTER_MANE : SPECIES_KARTANA);
    EXPECT_EQ(GetBoxMonDataAt(0, 0, MON_DATA_SPECIES), accept ? SPECIES_KARTANA : SPECIES_FLUTTER_MANE);
    EXPECT_EQ(gBattleCommunication[MULTISTRING_CHOOSER], accept ? B_MSG_SWAPPED_INTO_PARTY : B_MSG_SPECIAL_SENT_TO_PC);
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], full ? PARTY_SIZE : 2);
    EXPECT(PlayerPartyWithinRestrictedLimit());
    Free(gBattleStruct);
    Free(gBattleResources);
    gBattleStruct = savedBattle;
    gBattleResources = savedResources;
    gBattlescriptCurrInstr = savedScript;
    gBattleControllerExecFlags = 0;
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    ResetPokemonStorageSystem();
}
