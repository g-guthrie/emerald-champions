#include "global.h"
#include "battle.h"
#include "battle_controllers.h"
#include "battle_message.h"
#include "malloc.h"
#include "move.h"
#include "test/test.h"

TEST("Battle messages: selection and ordinary packets snapshot all formatter state")
{
    bool32 selection = FALSE;
    PARAMETRIZE { selection = FALSE; }
    PARAMETRIZE { selection = TRUE; }
    struct BattleResources *savedResources = gBattleResources;
    struct BattleStruct *savedBattle = gBattleStruct;
    u32 savedFlags = gBattleTypeFlags;
    gBattleResources = AllocZeroed(sizeof(*gBattleResources));
    gBattleStruct = AllocZeroed(sizeof(*gBattleStruct));
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    gCurrentMove = MOVE_EMBER;
    gBattleStruct->hpScale = 1;
    gPotentialItemEffectBattler = B_BATTLER_1;
    BtlController_EmitPrintString(B_BATTLER_0, B_COMM_TO_CONTROLLER, STRINGID_USEDMOVE);
    gBattleOutcome = B_OUTCOME_WON;
    gCurrentMove = MOVE_WATER_GUN;
    gChosenMove = MOVE_TACKLE;
    gLastUsedItem = ITEM_SITRUS_BERRY;
    gLastUsedAbility = ABILITY_CUD_CHEW;
    gBattleScripting.battler = B_BATTLER_2;
    gBattleStruct->scriptPartyIdx = 4;
    gBattleStruct->hpScale = 3;
    gPotentialItemEffectBattler = B_BATTLER_3;
    memset(gBattleTextBuff1, 0x31, TEXT_BUFF_ARRAY_COUNT);
    memset(gBattleTextBuff2, 0x32, TEXT_BUFF_ARRAY_COUNT);
    memset(gBattleTextBuff3, 0x33, TEXT_BUFF_ARRAY_COUNT);
    if (selection)
        BtlController_EmitPrintSelectionString(B_BATTLER_2, B_COMM_TO_CONTROLLER, STRINGID_USEDMOVE);
    else
        BtlController_EmitPrintString(B_BATTLER_2, B_COMM_TO_CONTROLLER, STRINGID_USEDMOVE);
    struct BattleMsgData packet;
    memcpy(&packet, &gBattleResources->bufferA[B_BATTLER_2][4], sizeof(packet));
    u8 command = gBattleResources->bufferA[B_BATTLER_2][0];
    u8 argument = gBattleResources->bufferA[B_BATTLER_2][1];
    Free(gBattleStruct);
    Free(gBattleResources);
    gBattleStruct = savedBattle;
    gBattleResources = savedResources;
    gBattleTypeFlags = savedFlags;
    EXPECT_EQ(command, selection ? CONTROLLER_PRINTSTRINGPLAYERONLY : CONTROLLER_PRINTSTRING);
    EXPECT_EQ(argument, selection ? CONTROLLER_PRINTSTRINGPLAYERONLY : B_OUTCOME_WON);
    EXPECT_EQ(packet.currentMove, MOVE_WATER_GUN);
    EXPECT_EQ(packet.originallyUsedMove, MOVE_TACKLE);
    EXPECT_EQ(packet.lastItem, ITEM_SITRUS_BERRY);
    EXPECT_EQ(packet.lastAbility, ABILITY_CUD_CHEW);
    EXPECT_EQ(packet.scrActive, B_BATTLER_2);
    EXPECT_EQ(packet.bakScriptPartyIdx, 4);
    EXPECT_EQ(packet.hpScale, 3);
    EXPECT_EQ(packet.itemEffectBattler, B_BATTLER_3);
    EXPECT_EQ(packet.moveType, GetMoveType(MOVE_WATER_GUN));
    for (u32 i = 0; i < TEXT_BUFF_ARRAY_COUNT; i++)
    {
        EXPECT_EQ(packet.textBuffs[0][i], 0x31);
        EXPECT_EQ(packet.textBuffs[1][i], 0x32);
        EXPECT_EQ(packet.textBuffs[2][i], 0x33);
    }
}

TEST("Battle messages: data transfer preserves the maximum payload and adjacent memory")
{
    u32 size = 0;
    PARAMETRIZE { size = 0; }
    PARAMETRIZE { size = 1; }
    PARAMETRIZE { size = sizeof(((struct BattleResources *)0)->transferBuffer) - 4; }
    struct BattleResources *previous = gBattleResources;
    u32 flags = gBattleTypeFlags;
    u8 payload[sizeof(previous->transferBuffer) - 4];
    u8 *allocation = Alloc(sizeof(*previous) + 16);
    memset(allocation, 0xA5, sizeof(*previous) + 16);
    gBattleResources = (struct BattleResources *)allocation;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    for (u32 i = 0; i < sizeof(payload); i++)
        payload[i] = i ^ 0x5A;
    BtlController_EmitDataTransfer(B_BATTLER_0, B_COMM_TO_ENGINE, size, payload);
    bool32 intact = TRUE;
    for (u32 i = 0; i < 16; i++)
        intact &= allocation[sizeof(*previous) + i] == 0xA5;
    bool32 matches = memcmp(&gBattleResources->bufferB[0][4], payload, size) == 0;
    u32 encoded = gBattleResources->bufferB[0][2] | (gBattleResources->bufferB[0][3] << 8);
    Free(allocation);
    gBattleResources = previous;
    gBattleTypeFlags = flags;
    EXPECT(intact && matches);
    EXPECT_EQ(encoded, size);
}

static enum BattlerId sCompletedMessageBattler;
static void CompleteMessageTest(enum BattlerId battler)
{
    sCompletedMessageBattler = battler;
}

TEST("Battle messages: raw record endpoints preserve adjacent bytes and complete")
{
    struct BattleResources *previous = gBattleResources;
    void (*previousEnd)(enum BattlerId) = gBattlerControllerEndFuncs[B_BATTLER_0];
    u32 flags = gBattleTypeFlags;
    u8 previousSlot = gBattlerPartyIndexes[B_BATTLER_0];
    gBattleResources = AllocZeroed(sizeof(*gBattleResources));
    gBattlerControllerEndFuncs[B_BATTLER_0] = CompleteMessageTest;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    gBattlerPartyIndexes[B_BATTLER_0] = 0;
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    CreateMon(mon, SPECIES_EEVEE, 5, 0, OTID_STRUCT_PLAYER_ID);
    struct Pokemon before = *mon;
    u32 offset = sizeof(*mon) - 2;
    gBattleResources->bufferA[0][1] = offset;
    gBattleResources->bufferA[0][2] = 2;
    sCompletedMessageBattler = MAX_BATTLERS_COUNT;
    BtlController_HandleGetRawMonData(B_BATTLER_0);
    EXPECT_EQ(sCompletedMessageBattler, B_BATTLER_0);
    EXPECT_EQ(memcmp(&gBattleResources->bufferB[0][4], (u8 *)mon + offset, 2), 0);
    gBattleResources->bufferA[0][3] = 0x12;
    gBattleResources->bufferA[0][4] = 0x34;
    sCompletedMessageBattler = MAX_BATTLERS_COUNT;
    BtlController_HandleSetRawMonData(B_BATTLER_0);
    EXPECT_EQ(sCompletedMessageBattler, B_BATTLER_0);
    EXPECT_EQ(memcmp(&before, mon, offset), 0);
    EXPECT_EQ(((u8 *)mon)[offset], 0x12);
    EXPECT_EQ(((u8 *)mon)[offset + 1], 0x34);
    *mon = before;
    Free(gBattleResources);
    gBattleResources = previous;
    gBattlerControllerEndFuncs[B_BATTLER_0] = previousEnd;
    gBattleTypeFlags = flags;
    gBattlerPartyIndexes[B_BATTLER_0] = previousSlot;
}

TEST("Battle messages: maximum update payload stays inside its transfer buffer")
{
    struct BattleResources *previous = gBattleResources;
    u32 flags = gBattleTypeFlags;
    u8 payload[sizeof(previous->transferBuffer) - 3];
    u8 *allocation = Alloc(sizeof(*previous) + 16);
    memset(allocation, 0xA5, sizeof(*previous) + 16);
    gBattleResources = (struct BattleResources *)allocation;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    memset(payload, 0x5A, sizeof(payload));
    BtlController_EmitSetMonData(B_BATTLER_0, B_COMM_TO_CONTROLLER, REQUEST_SPECIES_BATTLE, 1, sizeof(payload), payload);
    bool32 correct = memcmp(&gBattleResources->bufferA[0][3], payload, sizeof(payload)) == 0;
    for (u32 i = 0; i < 16; i++)
        correct &= allocation[sizeof(*previous) + i] == 0xA5;
    Free(allocation);
    gBattleResources = previous;
    gBattleTypeFlags = flags;
    EXPECT(correct);
}

TEST("Battle messages: active and masked party queries preserve slot order")
{
    u32 mask = 0;
    PARAMETRIZE { mask = 0; }
    PARAMETRIZE { mask = 0x21; }
    PARAMETRIZE { mask = 0x3F; }
    struct BattleResources *previous = gBattleResources;
    void (*previousEnd)(enum BattlerId) = gBattlerControllerEndFuncs[0];
    u32 flags = gBattleTypeFlags;
    u8 previousSlot = gBattlerPartyIndexes[0];
    gBattleResources = AllocZeroed(sizeof(*gBattleResources));
    gBattlerControllerEndFuncs[0] = CompleteMessageTest;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    gBattlerPartyIndexes[0] = PARTY_SIZE - 1;
    for (u32 i = 0; i < PARTY_SIZE; i++)
        CreateMon(&gParties[B_TRAINER_PLAYER][i], SPECIES_BULBASAUR + i, 5, i, OTID_STRUCT_PLAYER_ID);
    BtlController_EmitGetMonData(B_BATTLER_0, B_COMM_TO_CONTROLLER, REQUEST_SPECIES_BATTLE, mask);
    sCompletedMessageBattler = MAX_BATTLERS_COUNT;
    BtlController_HandleGetMonData(B_BATTLER_0);
    EXPECT_EQ(sCompletedMessageBattler, B_BATTLER_0);
    u32 offset = 4;
    u32 selected = mask ? mask : 1u << (PARTY_SIZE - 1);
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        if (selected & (1u << i))
        {
            u32 species = gBattleResources->bufferB[0][offset] | (gBattleResources->bufferB[0][offset + 1] << 8);
            EXPECT_EQ(species, SPECIES_BULBASAUR + i);
            offset += 2;
        }
    }
    EXPECT_EQ(gBattleResources->bufferB[0][2], offset - 4);
    Free(gBattleResources);
    gBattleResources = previous;
    gBattlerControllerEndFuncs[0] = previousEnd;
    gBattleTypeFlags = flags;
    gBattlerPartyIndexes[0] = previousSlot;
}

TEST("Battle messages: responses without party order cannot reuse the previous order")
{
    u32 partyId = PARTY_SIZE;
    PARAMETRIZE { partyId = PARTY_SIZE; } // Player cancellation.
    PARAMETRIZE { partyId = 2; } // NPC choice without a party-order payload.
    struct BattleResources *previous = gBattleResources;
    u32 flags = gBattleTypeFlags;
    u8 order[] = {0x54, 0x32, 0x10};
    gBattleResources = AllocZeroed(sizeof(*gBattleResources));
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    BtlController_EmitChosenMonReturnValue(B_BATTLER_0, B_COMM_TO_ENGINE, 5, order);
    EXPECT_EQ(memcmp(&gBattleResources->bufferB[0][2], order, sizeof(order)), 0);
    BtlController_EmitChosenMonReturnValue(B_BATTLER_0, B_COMM_TO_ENGINE, partyId, NULL);
    u8 packet[5];
    memcpy(packet, gBattleResources->bufferB[0], sizeof(packet));
    Free(gBattleResources);
    gBattleResources = previous;
    gBattleTypeFlags = flags;
    EXPECT_EQ(packet[0], CONTROLLER_CHOSENMONRETURNVALUE);
    EXPECT_EQ(packet[1], partyId);
    for (u32 i = 2; i < sizeof(packet); i++)
        EXPECT_EQ(packet[i], 0);
}
