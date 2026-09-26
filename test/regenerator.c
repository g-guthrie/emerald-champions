#include "global.h"
#include "battle.h"
#include "battle_util.h"
#include "item.h"
#include "pokemon.h"
#include "test/test.h"

TEST("Regenerator: tool restores spent berries; Knock Off returns items; burned berries stay lost")
{
    static EWRAM_DATA struct BattleStruct state;
    struct BattleStruct *saved = gBattleStruct;
    u32 savedFlags = gBattleTypeFlags;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    gBattleStruct = &state;
    u8 savedSlot = gBattlerPartyIndexes[B_BATTLER_0];
    u8 savedOpponentSlot = gBattlerPartyIndexes[B_BATTLER_1];
    gBattlerPartyIndexes[B_BATTLER_0] = gBattlerPartyIndexes[B_BATTLER_1] = 0;
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_EEVEE, 10, 0, OTID_STRUCT_PLAYER_ID);
    for (u32 owned = 0; owned < 2; owned++)
    {
        if (owned)
            EXPECT(AddBagItem(ITEM_REGENERATOR, 1));
        for (u32 kind = 0; kind < 4; kind++)
        {
            enum Item original = kind == 1 ? ITEM_FOCUS_SASH : ITEM_SITRUS_BERRY;
            enum Item empty = ITEM_NONE;
            memset(&state, 0, sizeof(state));
            SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &original);
            RecordPlayerPartyMonHeldItemForRestoration(0);
            SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &empty);
            if (kind == 2) // Knock Off removes the item only for this battle.
            {
                state.itemLost[B_TRAINER_PLAYER][0].stolen = TRUE;
            }
            else if (kind == 3)
                state.partyState[B_TRAINER_PLAYER][0].originalBerryDestroyed = TRUE;
            else
                RecordConsumedHeldItem(B_BATTLER_0, original);
            // The same helper protects a mon boxed by catch-and-swap.
            RestorePlayerPartyMonHeldItem(0);
            // kind 3 is Incinerate or Bug Bite: lost even with the tool.
            enum Item expected = kind == 3 || (!owned && kind == 0) ? ITEM_NONE : original;
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), expected);
            TryRestoreHeldItems();
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), expected);
        }
        if (owned)
            EXPECT(RemoveBagItem(ITEM_REGENERATOR, 1));
    }
    // Eating a berry after an earlier exchange still consumes it without the tool.
    state.itemLost[B_TRAINER_PLAYER][0].stolen = TRUE;
    RecordConsumedHeldItem(B_BATTLER_0, ITEM_SITRUS_BERRY);
    enum Item empty = ITEM_NONE;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &empty);
    TryRestoreHeldItems();
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);
    state.itemLost[B_TRAINER_OPPONENT_A][0].originalItem = ITEM_SITRUS_BERRY;
    state.partyState[B_TRAINER_OPPONENT_A][0].heldItemOrigin = B_TRAINER_OPPONENT_A * PARTY_SIZE + 1;
    RecordConsumedHeldItem(B_BATTLER_1, ITEM_SITRUS_BERRY);
    EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_OPPONENT_A, 0), ITEM_NONE);
    RecordRecoveredHeldItem(B_BATTLER_1, B_BATTLER_1, ITEM_SITRUS_BERRY);
    EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_OPPONENT_A, 0), ITEM_SITRUS_BERRY);
    gBattleTypeFlags = BATTLE_TYPE_FRONTIER;
    EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_SITRUS_BERRY);
    // A second consumed item must not erase the original Berry's history.
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    memset(&state, 0, sizeof(state));
    state.itemLost[B_TRAINER_PLAYER][0].originalItem = ITEM_SITRUS_BERRY;
    state.partyState[B_TRAINER_PLAYER][0].heldItemOrigin = B_TRAINER_PLAYER * PARTY_SIZE + 1;
    RecordConsumedHeldItem(B_BATTLER_0, ITEM_SITRUS_BERRY);
    RecordConsumedHeldItem(B_BATTLER_0, ITEM_ORAN_BERRY);
    EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_NONE);
    // A newly caught replacement starts a fresh restoration record.
    enum Item berry = ITEM_ORAN_BERRY;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &berry);
    state.partyState[B_TRAINER_PLAYER][0].originalBerryDestroyed = TRUE;
    RecordPlayerPartyMonHeldItemForRestoration(0);
    EXPECT(!state.partyState[B_TRAINER_PLAYER][0].originalBerryConsumed);
    EXPECT(!state.partyState[B_TRAINER_PLAYER][0].originalBerryDestroyed);
    EXPECT_EQ(state.partyState[B_TRAINER_PLAYER][0].usedHeldItem, ITEM_NONE);
    EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_ORAN_BERRY);
    gBattlerPartyIndexes[B_BATTLER_0] = savedSlot;
    gBattlerPartyIndexes[B_BATTLER_1] = savedOpponentSlot;
    gBattleTypeFlags = savedFlags;
    gBattleStruct = saved;
    ZeroPlayerPartyMons();
}

TEST("Ordinary Thief and Covet transfers survive held-item cleanup")
{
    static EWRAM_DATA struct BattleStruct state;
    struct BattleStruct *saved = gBattleStruct;
    u32 savedFlags = gBattleTypeFlags;
    u8 savedPlayerSlot = gBattlerPartyIndexes[B_BATTLER_0];
    u8 savedOpponentSlot = gBattlerPartyIndexes[B_BATTLER_1];
    enum Item empty = ITEM_NONE;
    enum Item item = ITEM_FOCUS_SASH;

    gBattleStruct = &state;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;
    gBattlerPartyIndexes[B_BATTLER_0] = gBattlerPartyIndexes[B_BATTLER_1] = 0;
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_EEVEE, 10, 0, OTID_STRUCT_PLAYER_ID);

    memset(&state, 0, sizeof(state));
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &item);
    RecordPlayerPartyMonHeldItemForRestoration(0);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &empty);
    RecordPermanentHeldItemTheft(B_BATTLER_0, B_BATTLER_1, item);
    EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), ITEM_NONE);
    TryRestoreHeldItems();
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_NONE);

    memset(&state, 0, sizeof(state));
    RecordPlayerPartyMonHeldItemForRestoration(0);
    state.itemLost[B_TRAINER_OPPONENT_A][0].originalItem = item;
    RecordPermanentHeldItemTheft(B_BATTLER_1, B_BATTLER_0, item);
    EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_PLAYER, 0), item);
    EXPECT_EQ(GetBattleRestoredHeldItem(B_TRAINER_OPPONENT_A, 0), ITEM_NONE);
    TryRestoreHeldItems();
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), item);

    // Knock Off has no permanent-theft record, so cleanup returns the item.
    memset(&state, 0, sizeof(state));
    RecordPlayerPartyMonHeldItemForRestoration(0);
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &empty);
    TryRestoreHeldItems();
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), item);

    gBattlerPartyIndexes[B_BATTLER_0] = savedPlayerSlot;
    gBattlerPartyIndexes[B_BATTLER_1] = savedOpponentSlot;
    gBattleTypeFlags = savedFlags;
    gBattleStruct = saved;
    ZeroPlayerPartyMons();
}
