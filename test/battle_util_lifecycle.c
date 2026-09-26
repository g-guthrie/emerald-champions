#include "global.h"
#include "battle.h"
#include "main.h"
#include "battle_util.h"
#include "recorded_battle.h"
#include "battle_util2.h"
#include "palette.h"
#include "dexnav.h"
#include "emerald_champions_opening.h"
#include "field_specials.h"
#include "event_data.h"
#include "pokemon.h"
#include "starter_choose.h"
#include "constants/emerald_champions.h"
#include "test/test.h"

TEST("Battle utility: field gifts and starter grants need no battle allocation")
{
    struct BattleStruct *savedBattle = gBattleStruct;
    bool32 savedInBattle = gMain.inBattle;
    gMain.inBattle = FALSE;
    gBattleStruct = NULL;
    ZeroPlayerPartyMons();
    EXPECT(GiveEmeraldChampionsStarterPair(0, 1));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), GetStarterPokemon(0));
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SPECIES), GetStarterPokemon(1));
    EXPECT_EQ(VarGet(VAR_EC_OPENING_STATE), EC_OPENING_PAIR_GRANTED);
    EXPECT(FlagGet(FLAG_SYS_POKEMON_GET));
    EXPECT_EQ(gBattleStruct, NULL);
    EXPECT_EQ(GiveEmeraldChampionsPreparedPokemonForTesting(SPECIES_BELDUM, 5), MON_GIVEN_TO_PARTY);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][2], MON_DATA_SPECIES), SPECIES_BELDUM);

    enum Item berry = ITEM_SITRUS_BERRY;
    SetMonData(&gParties[B_TRAINER_PLAYER][2], MON_DATA_HELD_ITEM, &berry);
    RecordPlayerPartyMonHeldItemForRestoration(2);
    RestorePlayerPartyMonHeldItem(2);
    TryRestoreHeldItems();
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][2], MON_DATA_HELD_ITEM), ITEM_SITRUS_BERRY);
    EXPECT_EQ(gBattleStruct, NULL);
    gBattleStruct = savedBattle;
    gMain.inBattle = savedInBattle;
    ZeroPlayerPartyMons();
}


extern void Test_FinishBattleResourceCleanup(void);

TEST("Battle cleanup: completed fade still releases resources when evolution is skipped")
{
    u32 savedFlags = gBattleTypeFlags;
    u8 savedOutcome = gBattleOutcome;
    bool32 savedFade = gPaletteFade.active;
    gBattleTypeFlags = BATTLE_TYPE_FIRST_BATTLE;
    gBattleOutcome = B_OUTCOME_WON;
    gPaletteFade.active = FALSE;
    gDexNavSpecies = SPECIES_NONE;
    AllocateBattleResources();
    EXPECT(gBattleResources != NULL);
    EXPECT(gBattleStruct != NULL);
    Test_FinishBattleResourceCleanup();
    EXPECT_EQ(gBattleResources, NULL);
    EXPECT_EQ(gBattleStruct, NULL);
    // Cleanup also remains safe when a preceding fade frame already freed it.
    Test_FinishBattleResourceCleanup();
    EXPECT_EQ(gBattleResources, NULL);
    gBattleTypeFlags = savedFlags;
    gBattleOutcome = savedOutcome;
    gPaletteFade.active = savedFade;
}

extern void BeginBattleIntro(void);

TEST("Battle initialization: dirty global state resets and held-item origins match the new parties")
{
    gBattleTypeFlags = BATTLE_TYPE_DOUBLE;
    gBattlersCount = MAX_BATTLERS_COUNT;
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_EEVEE, 5, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&gParties[B_TRAINER_OPPONENT_A][0], SPECIES_MAGIKARP, 5, 0, OTID_STRUCT_PLAYER_ID);
    enum Item berry = ITEM_SITRUS_BERRY;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &berry);
    AllocateBattleResources();
    gBattleWeather = 0xFFFF;
    gFieldStatuses = 0xFFFFFFFF;
    gBattleOutcome = B_OUTCOME_LOST;
    gAbsentBattlerFlags = 0xF;
    gBattleControllerExecFlags = 0xFFFFFFFF;
    gPaydayMoney = 999;
    memset(gSideTimers, 0xFF, sizeof(gSideTimers));
    memset(gSideStatuses, 0xFF, sizeof(gSideStatuses));
    for (u32 i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        gLastMoves[i] = MOVE_SURF;
        gLockedMoves[i] = MOVE_PROTECT;
        gBattleStruct->choicedMove[i] = MOVE_TACKLE;
    }
    BeginBattleIntro();
    EXPECT_EQ(gBattleWeather, 0);
    EXPECT_EQ(gFieldStatuses, 0);
    EXPECT_EQ(gBattleOutcome, 0);
    EXPECT_EQ(gAbsentBattlerFlags, 0);
    EXPECT_EQ(gBattleControllerExecFlags, 0);
    EXPECT_EQ(gPaydayMoney, 0);
    for (u32 i = 0; i < MAX_BATTLERS_COUNT; i++)
    {
        EXPECT_EQ(gLastMoves[i], MOVE_NONE);
        EXPECT_EQ(gLockedMoves[i], MOVE_NONE);
        EXPECT_EQ(gBattleStruct->choicedMove[i], MOVE_NONE);
        EXPECT_EQ((u32)gBattleStruct->battlerState[i].originalBattlerPartyId, PARTY_SIZE);
    }
    for (u32 side = 0; side < NUM_BATTLE_SIDES; side++)
    {
        EXPECT_EQ(gSideStatuses[side], 0);
        EXPECT_EQ(gSideTimers[side].stickyWebBattlerId, 0xFF);
    }
    EXPECT_EQ((u32)gBattleStruct->itemLost[B_TRAINER_PLAYER][0].originalItem, berry);
    EXPECT_EQ(gBattleStruct->partyState[B_TRAINER_PLAYER][0].heldItemOrigin, 1);
    EXPECT_EQ(gBattleStruct->partyState[B_TRAINER_OPPONENT_A][0].heldItemOrigin, 0);
    EXPECT_EQ(gBattleStruct->partyState[B_TRAINER_PLAYER][0].usedHeldItem, ITEM_NONE);
    FreeBattleResources();
    ZeroPlayerPartyMons();
    ZeroEnemyPartyMons();
    gBattleTypeFlags = 0;
    gBattlersCount = 0;
}

TEST("Battle action completion: last singles and doubles actions finish without a next slot")
{
    AllocateBattleResources();
    gBattleTypeFlags = 0;
    for (u32 count = 2; count <= MAX_BATTLERS_COUNT; count += 2)
    {
        gBattlersCount = count;
        for (u32 i = 0; i < MAX_BATTLERS_COUNT; i++)
        {
            gBattlerByTurnOrder[i] = i;
            gActionsByTurnOrder[i] = B_ACTION_USE_MOVE;
        }
        gCurrentTurnActionNumber = count - 1;
        HandleAction_NothingIsFainted();
        EXPECT_EQ(gCurrentTurnActionNumber, count);
        EXPECT_EQ(gCurrentActionFuncId, B_ACTION_FINISHED);
        gCurrentTurnActionNumber = count - 1;
        HandleAction_ActionFinished();
        EXPECT_EQ(gCurrentTurnActionNumber, count);
        EXPECT_EQ(gCurrentActionFuncId, B_ACTION_FINISHED);
    }
    FreeBattleResources();
    gBattlersCount = 0;
    gCurrentTurnActionNumber = 0;
}

TEST("After You: every doubles permutation preserves battler action pairs")
{
    gBattlersCount = MAX_BATTLERS_COUNT;
    for (u32 a = 0; a < 4; a++)
    for (u32 b = 0; b < 4; b++)
    for (u32 c = 0; c < 4; c++)
    for (u32 d = 0; d < 4; d++)
    {
        if (a == b || a == c || a == d || b == c || b == d || c == d)
            continue;
        const u8 original[] = {a, b, c, d};
        for (u32 attacker = 0; attacker < 4; attacker++)
        for (u32 target = 0; target < 4; target++)
        {
            for (u32 i = 0; i < 4; i++)
            {
                gBattlerByTurnOrder[i] = original[i];
                gActionsByTurnOrder[i] = original[i];
            }
            gBattlerAttacker = original[attacker];
            bool32 changed = ChangeOrderTargetAfterAttacker(original[target]);
            EXPECT_EQ(changed, target > attacker);
            u32 seen = 0, previous = 0;
            for (u32 i = 0; i < 4; i++)
            {
                EXPECT_EQ(gActionsByTurnOrder[i], gBattlerByTurnOrder[i]);
                seen |= 1 << gBattlerByTurnOrder[i];
                if (target <= attacker)
                    EXPECT_EQ(gBattlerByTurnOrder[i], original[i]);
                else if (i == attacker + 1)
                    EXPECT_EQ(gBattlerByTurnOrder[i], original[target]);
                else
                {
                    if (previous == target) previous++;
                    EXPECT_EQ(gBattlerByTurnOrder[i], original[previous++]);
                }
            }
            EXPECT_EQ(seen, 15);
        }
    }
    gBattlersCount = 0;
}

TEST("Doubles reserves: active and pending slots cannot be selected twice")
{
    gBattleTypeFlags = BATTLE_TYPE_DOUBLE;
    gBattlersCount = MAX_BATTLERS_COUNT;
    AllocateBattleResources();
    ZeroPlayerPartyMons();
    for (u32 i = 0; i < PARTY_SIZE; i++)
        CreateMon(&gParties[B_TRAINER_PLAYER][i], SPECIES_EEVEE, 5, 0, OTID_STRUCT_PLAYER_ID);
    for (u32 i = 0; i < MAX_BATTLERS_COUNT; i++)
        gBattlerPositions[i] = i;
    gBattlerPartyIndexes[B_POSITION_PLAYER_LEFT] = 0;
    gBattlerPartyIndexes[B_POSITION_PLAYER_RIGHT] = 1;
    for (u32 mask = 0; mask < (1 << PARTY_SIZE); mask++)
    {
        for (u32 i = 0; i < PARTY_SIZE; i++)
        {
            u32 hp = (mask & (1 << i)) ? 1 : 0;
            SetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HP, &hp);
        }
        for (u32 left = 0; left <= PARTY_SIZE; left++)
        for (u32 right = 0; right <= PARTY_SIZE; right++)
        {
            gBattleStruct->monToSwitchIntoId[B_POSITION_PLAYER_LEFT] = left;
            gBattleStruct->monToSwitchIntoId[B_POSITION_PLAYER_RIGHT] = right;
            u32 available = mask & ~3u;
            available &= ~(1u << left);
            available &= ~(1u << right);
            EXPECT_EQ(HasNoMonsToSwitch(B_BATTLER_0, PARTY_SIZE, PARTY_SIZE), available == 0);
            EXPECT_EQ(HasNoMonsToSwitch(B_BATTLER_2, PARTY_SIZE, PARTY_SIZE), available == 0);
        }
    }
    FreeBattleResources();
    ZeroPlayerPartyMons();
    gBattleTypeFlags = 0;
    gBattlersCount = 0;
}

TEST("Recorded choices: empty and oversized cancellations preserve earlier bytes and other battlers")
{
    static const u8 clears[] = {0, 1, 2, 3, 4, 255};
    AllocateBattleResources();
    for (u32 i = 0; i < ARRAY_COUNT(clears); i++)
    {
        RecordedBattle_Init(B_RECORD_MODE_RECORDING);
        RecordedBattle_ClearBattlerAction(B_BATTLER_0, 255);
        RecordedBattle_ClearBattlerAction(B_BATTLER_0, 1);
        RecordedBattle_SetBattlerAction(B_BATTLER_0, 10);
        RecordedBattle_SetBattlerAction(B_BATTLER_0, 20);
        RecordedBattle_SetBattlerAction(B_BATTLER_0, 30);
        RecordedBattle_SetBattlerAction(B_BATTLER_1, 99);
        RecordedBattle_ClearBattlerAction(B_BATTLER_0, clears[i]);
        u32 remaining = clears[i] >= 3 ? 0 : 3 - clears[i];
        u8 buffer[16];
        memset(buffer, 0xCC, sizeof(buffer));
        u32 length = RecordedBattle_BufferNewBattlerData(buffer, sizeof(buffer));
        u32 cursor = 0;
        if (remaining != 0)
        {
            EXPECT_EQ(buffer[cursor++], B_BATTLER_0);
            EXPECT_EQ(buffer[cursor++], remaining);
            for (u32 j = 0; j < remaining; j++)
                EXPECT_EQ(buffer[cursor++], (j + 1) * 10);
        }
        EXPECT_EQ(buffer[cursor++], B_BATTLER_1);
        EXPECT_EQ(buffer[cursor++], 1);
        EXPECT_EQ(buffer[cursor++], 99);
        EXPECT_EQ(length, cursor);
        EXPECT_EQ(buffer[cursor], 0xCC);
    }
    RecordedBattle_Init(B_RECORD_MODE_RECORDING);
    FreeBattleResources();
}

extern bool32 Test_IsRecordedMoveChangeValid(const u8 *record, u32 remaining);

TEST("Recorded moves: truncated and invalid slot records fail before indexing Pokemon data")
{
    u8 record[] = {6, 3, 2, 1, 0};
    for (u32 size = 0; size < sizeof(record); size++)
        EXPECT(!Test_IsRecordedMoveChangeValid(record, size));
    EXPECT(Test_IsRecordedMoveChangeValid(record, sizeof(record)));
    for (u32 slot = 1; slot <= MAX_MON_MOVES; slot++)
    {
        u8 saved = record[slot];
        for (u32 invalid = MAX_MON_MOVES; invalid <= 255; invalid++)
        {
            record[slot] = invalid;
            EXPECT(!Test_IsRecordedMoveChangeValid(record, sizeof(record)));
        }
        record[slot] = saved;
    }
    // Existing records can repeat the same empty-move index; keep compatibility.
    memset(record + 1, 0, MAX_MON_MOVES);
    EXPECT(Test_IsRecordedMoveChangeValid(record, sizeof(record)));
}

TEST("Recorded choices: overflow cancellation cannot erase committed or earlier stored bytes")
{
    AllocateBattleResources();
    u8 buffer[110];
    for (u32 cancelled = 0; cancelled <= 6; cancelled++)
    {
        RecordedBattle_Init(B_RECORD_MODE_RECORDING);
        // Publish300 bytes in normal-sized chunks, leaving88 bytes of capacity.
        for (u32 chunk = 0; chunk < 3; chunk++)
        {
            for (u32 i = 0; i < 100; i++)
                RecordedBattle_SetBattlerAction(B_BATTLER_0, 10);
            EXPECT_EQ(RecordedBattle_BufferNewBattlerData(buffer, sizeof(buffer)), 102);
        }
        for (u32 i = 0; i < 85; i++)
            RecordedBattle_SetBattlerAction(B_BATTLER_0, 20);
        // A six-byte choice stores three bytes and drops three.
        for (u32 i = 0; i < 6; i++)
            RecordedBattle_SetBattlerAction(B_BATTLER_0, 30);
        RecordedBattle_ClearBattlerAction(B_BATTLER_0, cancelled);
        u32 stored = 88 - (cancelled > 3 ? cancelled - 3 : 0);
        EXPECT_EQ(RecordedBattle_BufferNewBattlerData(buffer, sizeof(buffer)), stored + 2);
        EXPECT_EQ(buffer[1], stored);
        for (u32 i = 0; i < stored; i++)
            EXPECT_EQ(buffer[i + 2], i < 85 ? 20 : 30);
        RecordedBattle_ClearBattlerAction(B_BATTLER_0, 255);
        EXPECT_EQ(RecordedBattle_BufferNewBattlerData(buffer, sizeof(buffer)), 0);
    }
    RecordedBattle_Init(B_RECORD_MODE_RECORDING);
    FreeBattleResources();
}

extern bool32 Test_AppendRecordedBattleData(const u8 *src, u32 capacity);
extern u32 Test_ReceivedRecordSize(enum BattlerId battler);

TEST("Recorded packets: bounded batches round-trip and malformed input is atomic")
{
    AllocateBattleResources();
    RecordedBattle_Init(B_RECORD_MODE_RECORDING);
    for (u32 b = 0; b < MAX_BATTLERS_COUNT; b++)
        for (u32 i = 0; i < BATTLER_RECORD_SIZE; i++)
            RecordedBattle_SetBattlerAction(b, i % 200);
    u8 packet[32];
    u32 total = 0;
    for (u32 batches = 0; batches < 100; batches++)
    {
        memset(packet, 0xCC, sizeof(packet));
        u32 length = RecordedBattle_BufferNewBattlerData(packet + 2, sizeof(packet) - 3);
        EXPECT_EQ(packet[31], 0xCC);
        packet[0] = packet[1] = length;
        if (!length) break;
        EXPECT(Test_AppendRecordedBattleData(packet, sizeof(packet) - 1));
        total += length;
    }
    EXPECT(total >= MAX_BATTLERS_COUNT * BATTLER_RECORD_SIZE);
    for (u32 b = 0; b < MAX_BATTLERS_COUNT; b++)
        EXPECT_EQ(Test_ReceivedRecordSize(b), BATTLER_RECORD_SIZE);
    const u8 overflow[] = {3, 3, 0, 1, 42};
    EXPECT(!Test_AppendRecordedBattleData(overflow, sizeof(overflow)));
    RecordedBattle_Init(B_RECORD_MODE_RECORDING);
    const u8 malformed[][8] = {
        {6, 6, 0, 1, 42, 4, 1, 43}, // Valid first record followed by invalid battler.
        {6, 6, 0, 1, 42, 1, 2, 43}, // Truncated second record.
        {6, 5, 0, 1, 42, 1, 1, 43}, // Length headers disagree.
        {7, 7, 0, 1, 42, 1, 1, 43}, // Declared data exceeds supplied capacity.
    };
    for (u32 i = 0; i < ARRAY_COUNT(malformed); i++)
    {
        EXPECT(!Test_AppendRecordedBattleData(malformed[i], sizeof(malformed[i])));
        EXPECT_EQ(Test_ReceivedRecordSize(B_BATTLER_0), 0);
        EXPECT_EQ(Test_ReceivedRecordSize(B_BATTLER_1), 0);
    }
    EXPECT_EQ(RecordedBattle_BufferNewBattlerData(packet, 0), 0);
    EXPECT(!Test_AppendRecordedBattleData(packet, 0));
    FreeBattleResources();
}

extern u32 Test_ReorderMimickedMoveFlags(u32 flags, const u8 *order);

TEST("Recorded move reorder: Mimic flags follow source slots for every permutation")
{
    for (u32 a = 0; a < 4; a++)
    for (u32 b = 0; b < 4; b++)
    for (u32 c = 0; c < 4; c++)
    for (u32 d = 0; d < 4; d++)
    {
        if (a == b || a == c || a == d || b == c || b == d || c == d)
            continue;
        const u8 order[] = {a, b, c, d};
        for (u32 mask = 0; mask < 16; mask++)
        {
            u32 result = Test_ReorderMimickedMoveFlags(mask, order);
            EXPECT_LT(result, 16);
            for (u32 slot = 0; slot < 4; slot++)
                EXPECT_EQ(!!(result & (1 << slot)), !!(mask & (1 << order[slot])));
        }
    }
    const u8 swap[] = {1, 0, 3, 2};
    EXPECT_EQ(Test_ReorderMimickedMoveFlags(1, swap), 2);
    const u8 legacy[] = {0, 0, 1, 2};
    EXPECT_EQ(Test_ReorderMimickedMoveFlags(1, legacy), 3);
}

extern void ModifyPersonalityForNature(u32 *personality, u32 newNature);

TEST("Personality nature adjustment: boundary values always yield the requested nature")
{
    for (u32 distance = 0; distance < NUM_NATURES; distance++)
    for (u32 upper = 0; upper < 2; upper++)
    for (u32 nature = 0; nature < NUM_NATURES; nature++)
    {
        u32 personality = upper ? UINT32_MAX - distance : distance;
        ModifyPersonalityForNature(&personality, nature);
        EXPECT_EQ(GetNatureFromPersonality(personality), nature);
    }
}

extern void Test_ModifyTrainerPersonalityForNature(u32 *personality, u32 nature);

TEST("Trainer nature: generated boundary personalities retain requested nature and gender byte")
{
    static const u8 lowBytes[] = {0, 127, 255};
    for (u32 upper = 0; upper < 2; upper++)
    for (u32 raw = 0; raw < 0x10000; raw += 0x100)
    for (u32 gender = 0; gender < ARRAY_COUNT(lowBytes); gender++)
    for (u32 nature = 0; nature < NUM_NATURES; nature++)
    {
        u32 original = (((upper ? 0xFFFF0000 : 0) | raw) & 0xFFFFDF00) + 0x1000;
        original |= lowBytes[gender];
        u32 personality = original;
        Test_ModifyTrainerPersonalityForNature(&personality, nature);
        EXPECT_EQ(GetNatureFromPersonality(personality), nature);
        EXPECT_EQ(personality & 0xFF, original & 0xFF);
    }
}
