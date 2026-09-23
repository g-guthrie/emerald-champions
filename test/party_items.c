#include "global.h"
#include "battle.h"
#include "party_menu.h"
#include "malloc.h"
#include "window.h"
#include "constants/party_menu.h"
#include "event_data.h"
#include "field_specials.h"
#include "pokemon.h"
#include "move.h"
#include "script_pokemon_util.h"
#include "constants/moves.h"
#include "mail.h"
#include "test/test.h"

extern bool32 Test_TrySwapPartyHeldItems(struct Pokemon *first, struct Pokemon *second);
extern void Test_RestoreHeldItemAfterCanceledMail(struct Pokemon *mon, enum Item item);
extern void TryItemHoldFormChange(struct Pokemon *mon, s8 slotId, enum BattleTrainer trainer);

TEST("Party items: moving onto Mail preserves both holders and the attached message")
{
    struct Pokemon first, second;
    CreateMon(&first, SPECIES_EEVEE, 10, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&second, SPECIES_PIKACHU, 10, 0, OTID_STRUCT_PLAYER_ID);
    enum Item item = ITEM_LEFTOVERS;
    SetMonData(&first, MON_DATA_HELD_ITEM, &item);
    u8 mail = GiveMailToMonByItemId(&second, ITEM_ORANGE_MAIL);
    EXPECT_NE(mail, MAIL_NONE);
    struct Pokemon savedFirst = first, savedSecond = second;
    EXPECT(!Test_TrySwapPartyHeldItems(&first, &second));
    EXPECT_EQ(memcmp(&first, &savedFirst, sizeof(first)), 0);
    EXPECT_EQ(memcmp(&second, &savedSecond, sizeof(second)), 0);
    // Reject the reverse direction too; Mail uses its separate native menu.
    EXPECT(!Test_TrySwapPartyHeldItems(&second, &first));
    EXPECT_EQ(GetMonData(&second, MON_DATA_MAIL), mail);
    EXPECT_EQ(gSaveBlock1Ptr->mail[mail].itemId, ITEM_ORANGE_MAIL);
    TakeMailFromMon(&second);
}

TEST("Party items: ordinary swaps and moves to an empty holder preserve item counts")
{
    struct Pokemon first, second;
    CreateMon(&first, SPECIES_EEVEE, 10, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&second, SPECIES_PIKACHU, 10, 0, OTID_STRUCT_PLAYER_ID);
    enum Item item = ITEM_LEFTOVERS;
    SetMonData(&first, MON_DATA_HELD_ITEM, &item);
    EXPECT(Test_TrySwapPartyHeldItems(&first, &second));
    EXPECT_EQ(GetMonData(&first, MON_DATA_HELD_ITEM), ITEM_NONE);
    EXPECT_EQ(GetMonData(&second, MON_DATA_HELD_ITEM), ITEM_LEFTOVERS);
    item = ITEM_SITRUS_BERRY;
    SetMonData(&first, MON_DATA_HELD_ITEM, &item);
    EXPECT(Test_TrySwapPartyHeldItems(&first, &second));
    EXPECT_EQ(GetMonData(&first, MON_DATA_HELD_ITEM), ITEM_LEFTOVERS);
    EXPECT_EQ(GetMonData(&second, MON_DATA_HELD_ITEM), ITEM_SITRUS_BERRY);
}

TEST("Party items: canceled Mail restores item form and stats with the party menu closed")
{
    struct Pokemon mon;
    CreateMon(&mon, SPECIES_GIRATINA_ORIGIN, 10, 0, OTID_STRUCT_PLAYER_ID);
    enum Item item = ITEM_GRISEOUS_CORE;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    CalculateMonStats(&mon);
    u32 attack = GetMonData(&mon, MON_DATA_ATK);
    u32 defense = GetMonData(&mon, MON_DATA_DEF);
    u8 mail = GiveMailToMonByItemId(&mon, ITEM_ORANGE_MAIL);
    EXPECT_NE(mail, MAIL_NONE);
    // Both giving Mail from the Bag and returning from composition run while
    // the party graphics are absent. Only the Pokemon data should be touched.
    TryItemHoldFormChange(&mon, 0, B_TRAINER_PLAYER);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_GIRATINA_ALTERED);
    Test_RestoreHeldItemAfterCanceledMail(&mon, item);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_GIRATINA_ORIGIN);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HELD_ITEM), ITEM_GRISEOUS_CORE);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MAIL), MAIL_NONE);
    EXPECT_EQ(gSaveBlock1Ptr->mail[mail].itemId, ITEM_NONE);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_ATK), attack);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_DEF), defense);
}

TEST("Party forms: invalid selections cannot modify the adjacent party")
{
    struct Pokemon *neighbor = &gParties[B_TRAINER_PLAYER + 1][0];
    CreateMon(neighbor, SPECIES_EEVEE, 30, 0, OTID_STRUCT_PLAYER_ID);
    struct Pokemon before = *neighbor;
    u16 oldSlot = gSpecialVar_0x8004, oldSpecies = gSpecialVar_0x8005;
    static const u16 invalidSlots[] = {PARTY_SIZE, 0xFE, 0xFF};
    gSpecialVar_0x8005 = SPECIES_DEOXYS_ATTACK;
    for (u32 i = 0; i < ARRAY_COUNT(invalidSlots); i++)
    {
        gSpecialVar_0x8004 = invalidSlots[i];
        EXPECT_EQ(ScriptGetPartyMonSpecies(), SPECIES_NONE);
        ChangeMonSpecies();
        EXPECT_EQ(memcmp(neighbor, &before, sizeof(before)), 0);
    }
    gSpecialVar_0x8004 = oldSlot;
    gSpecialVar_0x8005 = oldSpecies;
    ZeroMonData(neighbor);
}

TEST("Party forms: Deoxys changes stats without healing or replacing identity")
{
    static const enum Species forms[] = {SPECIES_DEOXYS_ATTACK, SPECIES_DEOXYS_DEFENSE, SPECIES_DEOXYS_SPEED, SPECIES_DEOXYS};
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    u16 oldSlot = gSpecialVar_0x8004, oldSpecies = gSpecialVar_0x8005;
    CreateMon(mon, SPECIES_DEOXYS, 50, 0, OTID_STRUCT_PLAYER_ID);
    CalculateMonStats(mon);
    EXPECT_GT(GetMonData(mon, MON_DATA_HP), 7);
    u32 hp = GetMonData(mon, MON_DATA_HP) - 7;
    SetMonData(mon, MON_DATA_HP, &hp);
    u32 personality = GetMonData(mon, MON_DATA_PERSONALITY);
    u32 experience = GetMonData(mon, MON_DATA_EXP);
    u32 attack = GetMonData(mon, MON_DATA_ATK), defense = GetMonData(mon, MON_DATA_DEF), speed = GetMonData(mon, MON_DATA_SPEED);
    gSpecialVar_0x8004 = 0;
    for (u32 i = 0; i < ARRAY_COUNT(forms); i++)
    {
        gSpecialVar_0x8005 = forms[i];
        ChangeMonSpecies();
        EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), forms[i]);
        EXPECT_EQ(GetMonData(mon, MON_DATA_HP), hp);
        EXPECT_EQ(GetMonData(mon, MON_DATA_PERSONALITY), personality);
        EXPECT_EQ(GetMonData(mon, MON_DATA_EXP), experience);
        if (forms[i] == SPECIES_DEOXYS_ATTACK) EXPECT_GT(GetMonData(mon, MON_DATA_ATK), attack);
        if (forms[i] == SPECIES_DEOXYS_DEFENSE) EXPECT_GT(GetMonData(mon, MON_DATA_DEF), defense);
        if (forms[i] == SPECIES_DEOXYS_SPEED) EXPECT_GT(GetMonData(mon, MON_DATA_SPEED), speed);
    }
    struct Pokemon before = *mon;
    static const u16 invalidSpecies[] = {SPECIES_NONE, SPECIES_EGG, NUM_SPECIES};
    for (u32 i = 0; i < ARRAY_COUNT(invalidSpecies); i++)
    {
        gSpecialVar_0x8005 = invalidSpecies[i];
        ChangeMonSpecies();
        EXPECT_EQ(memcmp(mon, &before, sizeof(before)), 0);
    }
    gSpecialVar_0x8005 = SPECIES_DEOXYS_ATTACK;
    u32 isEgg = TRUE;
    SetMonData(mon, MON_DATA_IS_EGG, &isEgg);
    before = *mon;
    ChangeMonSpecies();
    EXPECT_EQ(memcmp(mon, &before, sizeof(before)), 0);
    ZeroMonData(mon);
    before = *mon;
    ChangeMonSpecies();
    EXPECT_EQ(memcmp(mon, &before, sizeof(before)), 0);
    gSpecialVar_0x8004 = oldSlot;
    gSpecialVar_0x8005 = oldSpecies;
    ZeroMonData(mon);
}

TEST("Party requirements: repeated Regis cannot replace a missing species or invalidate the trio")
{
    u16 saved[] = {gSpecialVar_0x8004, gSpecialVar_0x8005, gSpecialVar_0x8006, gSpecialVar_0x8007};
    gSpecialVar_0x8004 = SPECIES_REGICE;
    gSpecialVar_0x8005 = SPECIES_REGIROCK;
    gSpecialVar_0x8006 = SPECIES_REGISTEEL;
    gSpecialVar_0x8007 = 3;
    ZeroPlayerPartyMons();
    for (u32 i = 0; i < 3; i++)
        CreateMon(&gParties[B_TRAINER_PLAYER][i], SPECIES_REGICE, 50, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT(!CheckSpeciesInParty());
    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_REGIROCK, 50, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&gParties[B_TRAINER_PLAYER][2], SPECIES_REGISTEEL, 50, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT(CheckSpeciesInParty());
    CreateMon(&gParties[B_TRAINER_PLAYER][3], SPECIES_REGICE, 50, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT(CheckSpeciesInParty());
    u32 isEgg = TRUE;
    SetMonData(&gParties[B_TRAINER_PLAYER][2], MON_DATA_IS_EGG, &isEgg);
    EXPECT(!CheckSpeciesInParty());
    ZeroMonData(&gParties[B_TRAINER_PLAYER][2]);
    EXPECT(!CheckSpeciesInParty());
    gSpecialVar_0x8004 = saved[0];
    gSpecialVar_0x8005 = saved[1];
    gSpecialVar_0x8006 = saved[2];
    gSpecialVar_0x8007 = saved[3];
    ZeroPlayerPartyMons();
}

TEST("Script party moves: invalid indices preserve party data and valid Egg updates work")
{
    struct Pokemon before[PARTY_SIZE];
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_PICHU, 5, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    memcpy(before, gParties[B_TRAINER_PLAYER], sizeof(before));
    ScriptSetMonMoveSlot(0, MOVE_SURF, MAX_MON_MOVES);
    EXPECT_EQ(memcmp(before, gParties[B_TRAINER_PLAYER], sizeof(before)), 0);
    ScriptSetMonMoveSlot(0, MOVE_SURF, 255);
    EXPECT_EQ(memcmp(before, gParties[B_TRAINER_PLAYER], sizeof(before)), 0);
    ScriptSetMonMoveSlot(0, MOVES_COUNT_ALL, 0);
    EXPECT_EQ(memcmp(before, gParties[B_TRAINER_PLAYER], sizeof(before)), 0);
    ScriptSetMonMoveSlot(1, MOVE_SURF, 0);
    EXPECT_EQ(memcmp(before, gParties[B_TRAINER_PLAYER], sizeof(before)), 0);

    // Mystery Event Pichu deliberately edits an Egg, so it must remain legal.
    bool32 egg = TRUE;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG, &egg);
    ScriptSetMonMoveSlot(0, MOVE_SURF, 2);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE3), MOVE_SURF);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP3), GetMoveMaxPP(MOVE_SURF));
    EXPECT(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG));
    ScriptSetMonMoveSlot(PARTY_SIZE, MOVE_THUNDER_SHOCK, 2);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE3), MOVE_THUNDER_SHOCK);
    ScriptSetMonMoveSlot(255, MOVE_NONE, 2);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE3), MOVE_NONE);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_PP3), 0);

    ZeroPlayerPartyMons();
    memcpy(before, gParties[B_TRAINER_PLAYER], sizeof(before));
    ScriptSetMonMoveSlot(255, MOVE_SURF, 2);
    EXPECT_EQ(memcmp(before, gParties[B_TRAINER_PLAYER], sizeof(before)), 0);
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], 0);
}

extern void Test_ReorderPartyForMenu(bool32 toBattleOrder);

TEST("Party order: all layouts restore both parties without heap memory")
{
    u32 layout = PARTY_LAYOUT_SINGLE;
    bool32 right = FALSE;
    for (u32 i = 0; i < PARTY_LAYOUT_COUNT; i++)
        for (u32 side = 0; side < 2; side++)
            PARAMETRIZE { layout = i; right = side; }
    struct Pokemon original[2][PARTY_SIZE];
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        CreateMon(&gParties[B_TRAINER_PLAYER][i], SPECIES_EEVEE, 5, i + 1, OTID_STRUCT_PLAYER_ID);
        CreateMon(&gParties[B_TRAINER_PARTNER][i], SPECIES_PIKACHU, 5, i + 11, OTID_STRUCT_PLAYER_ID);
    }
    memcpy(original[0], gParties[B_TRAINER_PLAYER], sizeof(original[0]));
    memcpy(original[1], gParties[B_TRAINER_PARTNER], sizeof(original[1]));
    struct PartyMenu savedMenu = gPartyMenu;
    u32 savedFlags = gBattleTypeFlags;
    u8 savedBattler = gBattlerInMenuId;
    u8 savedOrder[3];
    memcpy(savedOrder, gBattlePartyCurrentOrder, sizeof(savedOrder));
    gPartyMenu.layout = layout;
    gBattleTypeFlags = right ? BATTLE_TYPE_LINK : 0;
    gBattlerInMenuId = right ? B_BATTLER_2 : B_BATTLER_0;
    gBattlePartyCurrentOrder[0] = 0x12;
    gBattlePartyCurrentOrder[1] = 0x34;
    gBattlePartyCurrentOrder[2] = 0x50;

    void *blocks[64];
    u32 count = 0;
    const struct MemBlock *head = HeapHead(), *block = head;
    do
    {
        if (!block->allocated)
        {
            ASSUME(count < ARRAY_COUNT(blocks));
            blocks[count++] = AllocUnchecked(block->size);
        }
        block = block->next;
    } while (block != head);
    Test_ReorderPartyForMenu(TRUE);
    bool32 changed = memcmp(original[0], gParties[B_TRAINER_PLAYER], sizeof(original[0]))
        || memcmp(original[1], gParties[B_TRAINER_PARTNER], sizeof(original[1]));
    Test_ReorderPartyForMenu(FALSE);
    bool32 restored = !memcmp(original[0], gParties[B_TRAINER_PLAYER], sizeof(original[0]))
        && !memcmp(original[1], gParties[B_TRAINER_PARTNER], sizeof(original[1]));
    // Invalid permutations must not partially move or duplicate Pokemon.
    gBattlePartyCurrentOrder[0] = 0x11;
    Test_ReorderPartyForMenu(TRUE);
    Test_ReorderPartyForMenu(FALSE);
    gBattlePartyCurrentOrder[0] = 0xF1;
    Test_ReorderPartyForMenu(TRUE);
    Test_ReorderPartyForMenu(FALSE);
    restored = restored && !memcmp(original[0], gParties[B_TRAINER_PLAYER], sizeof(original[0]))
        && !memcmp(original[1], gParties[B_TRAINER_PARTNER], sizeof(original[1]));
    for (u32 i = 0; i < count; i++)
        Free(blocks[i]);
    gPartyMenu = savedMenu;
    gBattleTypeFlags = savedFlags;
    gBattlerInMenuId = savedBattler;
    memcpy(gBattlePartyCurrentOrder, savedOrder, sizeof(savedOrder));
    EXPECT(changed);
    EXPECT(restored);
}

extern bool32 Test_PreparePartySetupFailure(bool32 battle, MainCallback callback);
extern bool32 Test_RunPartyBackgroundFailure(void);
static void PartySetupTestReturn(void) {}

TEST("Party setup: initial allocation failure cancels without changing party order")
{
    u32 entry = 0;
    PARAMETRIZE { entry = 0; } // Script/daycare selection.
    PARAMETRIZE { entry = 1; } // Optional battle switch.
    PARAMETRIZE { entry = 2; } // In-battle item recipient.
    PARAMETRIZE { entry = 3; } // Return from a battle summary.
    struct Pokemon before[PARTY_SIZE];
    for (u32 i = 0; i < PARTY_SIZE; i++)
        CreateMon(&gParties[B_TRAINER_PLAYER][i], SPECIES_EEVEE, 5, i + 1, OTID_STRUCT_PLAYER_ID);
    memcpy(before, gParties[B_TRAINER_PLAYER], sizeof(before));
    CalculatePlayerPartyCount();
    MainCallback previous = gMain.callback2;
    gBattleTypeFlags = BATTLE_TYPE_DOUBLE;
    gPartyMenu.layout = PARTY_LAYOUT_DOUBLE;
    gPartyMenu.menuType = PARTY_MENU_TYPE_IN_BATTLE;
    gPartyMenu.action = PARTY_ACTION_CHOOSE_MON;
    gPartyMenu.exitCallback = PartySetupTestReturn;
    gBattlePartyCurrentOrder[0] = 0x12;
    gBattlePartyCurrentOrder[1] = 0x34;
    gBattlePartyCurrentOrder[2] = 0x50;
    if (entry == 3)
        Test_ReorderPartyForMenu(TRUE);
    gPartyMenu.slotId = 2;
    gPartyMenuUseExitCallback = TRUE;
    gSelectedMonPartyId = 2;
    gSpecialVar_0x8004 = 2;
    void *blocks[64];
    u32 count = 0;
    const struct MemBlock *head = HeapHead(), *block = head;
    do
    {
        if (!block->allocated)
        {
            ASSUME(count < ARRAY_COUNT(blocks));
            blocks[count++] = AllocUnchecked(block->size);
        }
        block = block->next;
    } while (block != head);
    switch (entry)
    {
    case 0: ChooseMonForDaycare(); break;
    case 1: OpenPartyMenuInBattle(PARTY_ACTION_CHOOSE_MON); break;
    case 2: ChooseMonForInBattleItem(); break;
    case 3: CB2_ReturnToPartyMenuFromSummaryScreen(); break;
    }
    bool32 unchanged = !memcmp(before, gParties[B_TRAINER_PLAYER], sizeof(before));
    for (u32 i = 0; i < count; i++)
        Free(blocks[i]);
    SetMainCallback2(previous);
    EXPECT(unchanged);
    EXPECT_EQ((u32)gPartyMenu.slotId, PARTY_SIZE + 1);
    EXPECT_EQ(gPartyMenuUseExitCallback, FALSE);
    EXPECT_EQ(gSelectedMonPartyId, PARTY_SIZE);
    EXPECT_EQ(gSpecialVar_0x8004, PARTY_NOTHING_CHOSEN);
}

TEST("Party setup: background allocation failure cancels and restores field order")
{
    bool32 battle = FALSE;
    PARAMETRIZE { battle = FALSE; }
    PARAMETRIZE { battle = TRUE; }
    static const struct WindowTemplate windows[] = {DUMMY_WIN_TEMPLATE};
    EXPECT(InitWindows(windows));
    struct Pokemon before[PARTY_SIZE];
    for (u32 i = 0; i < PARTY_SIZE; i++)
        CreateMon(&gParties[B_TRAINER_PLAYER][i], SPECIES_EEVEE, 5, i + 1, OTID_STRUCT_PLAYER_ID);
    memcpy(before, gParties[B_TRAINER_PLAYER], sizeof(before));
    CalculatePlayerPartyCount();
    MainCallback previous = gMain.callback2;
    gBattlePartyCurrentOrder[0] = 0x12;
    gBattlePartyCurrentOrder[1] = 0x34;
    gBattlePartyCurrentOrder[2] = 0x50;
    EXPECT(Test_PreparePartySetupFailure(battle, PartySetupTestReturn));
    gPartyMenuUseExitCallback = TRUE;
    void *blocks[64];
    u32 count = 0;
    const struct MemBlock *head = HeapHead(), *block = head;
    do
    {
        if (!block->allocated)
        {
            ASSUME(count < ARRAY_COUNT(blocks));
            blocks[count++] = AllocUnchecked(block->size);
        }
        block = block->next;
    } while (block != head);
    bool32 exited = Test_RunPartyBackgroundFailure();
    bool32 callback = gMain.callback2 == PartySetupTestReturn;
    bool32 restored = !memcmp(before, gParties[B_TRAINER_PLAYER], sizeof(before));
    for (u32 i = 0; i < count; i++)
        Free(blocks[i]);
    SetMainCallback2(previous);
    EXPECT(exited);
    EXPECT(callback);
    EXPECT(restored);
    EXPECT_EQ((u32)gPartyMenu.slotId, PARTY_SIZE + 1);
    EXPECT_EQ(gPartyMenuUseExitCallback, FALSE);
}

extern bool32 Test_SummaryCanViewMultiMon(struct Pokemon *mon, bool32 infoPage, u8 currentMon);

TEST("Summary multi navigation: Egg visibility depends on page not current party slot")
{
    struct Pokemon mon;
    for (u32 egg = 0; egg < 2; egg++)
    {
        CreateMon(&mon, SPECIES_EEVEE, 5, 0, OTID_STRUCT_PLAYER_ID);
        SetMonData(&mon, MON_DATA_IS_EGG, &egg);
        for (u32 page = 0; page < 2; page++)
        for (u32 slot = 0; slot < PARTY_SIZE; slot++)
            EXPECT_EQ(Test_SummaryCanViewMultiMon(&mon, page, slot), page || !egg);
    }
    memset(&mon, 0, sizeof(mon));
    EXPECT(!Test_SummaryCanViewMultiMon(&mon, TRUE, 1));
}
