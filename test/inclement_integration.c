#include "global.h"
#include "lottery_corner.h"
#include "pokemon_storage_system.h"
#include "script.h"
#include "caps.h"
#include "braille_puzzles.h"
#include "battle_util.h"
#include "chooseboxmon.h"
#include "sound.h"
#include "task.h"
#include "battle.h"
#include "battle_main.h"
#include "coins.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "overworld.h"
#include "constants/event_objects.h"
#include "field_specials.h"
#include "field_move.h"
#include "string_util.h"
#include "item.h"
#include "money.h"
#include "legendary_signs.h"
#include "random.h"
#include "constants/maps.h"
#include "constants/vars.h"
#include "pokemon.h"
#include "test/test.h"

TEST("Inclement integration: every HM needs badge, license, and a capable party member without a moveslot")
{
    static const enum FieldMove moves[] = {FIELD_MOVE_CUT, FIELD_MOVE_FLASH, FIELD_MOVE_ROCK_SMASH,
        FIELD_MOVE_STRENGTH, FIELD_MOVE_SURF, FIELD_MOVE_FLY, FIELD_MOVE_DIVE, FIELD_MOVE_WATERFALL};
    static const u16 licenses[] = {FLAG_RECEIVED_HM_CUT, FLAG_RECEIVED_HM_FLASH, FLAG_RECEIVED_HM_ROCK_SMASH,
        FLAG_RECEIVED_HM_STRENGTH, FLAG_RECEIVED_HM_SURF, FLAG_RECEIVED_HM_FLY, FLAG_RECEIVED_HM_DIVE, FLAG_RECEIVED_HM_WATERFALL};
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_MAGIKARP, 5, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_MEW, 5, 0, OTID_STRUCT_PLAYER_ID);
    for (u32 i = 0; i < ARRAY_COUNT(moves); i++)
    {
        u16 badge = FLAG_BADGE01_GET + gFieldMoveInfo[moves[i]].arg;
        FlagClear(badge);
        FlagClear(licenses[i]);
        EXPECT_EQ(FieldMove_GetUserSlot(moves[i], TRUE), PARTY_SIZE);
        FlagSet(licenses[i]);
        EXPECT_EQ(FieldMove_GetUserSlot(moves[i], TRUE), PARTY_SIZE);
        FlagSet(badge);
        EXPECT(SpeciesCanLearnFieldMove(SPECIES_MEW, FieldMove_GetMoveId(moves[i])));
        EXPECT_EQ(FieldMove_GetUserSlot(moves[i], TRUE), 1);
        // The license persists, but the party must still include a capable mon.
        ZeroPlayerPartyMons();
        EXPECT_EQ(FieldMove_GetUserSlot(moves[i], TRUE), PARTY_SIZE);
        CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_MAGIKARP, 5, 0, OTID_STRUCT_PLAYER_ID);
        EXPECT_EQ(FieldMove_GetUserSlot(moves[i], TRUE), PARTY_SIZE);
        CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_MEW, 5, 0, OTID_STRUCT_PLAYER_ID);
        FlagClear(licenses[i]);
        EXPECT_EQ(FieldMove_GetUserSlot(moves[i], FALSE), IS_FRLG ? 1 : PARTY_SIZE);
        FlagClear(badge);
    }
    ZeroPlayerPartyMons();
}

TEST("Inclement integration: failed item delivery cannot unlock vendor stock")
{
    ClearBag();
    memset(gSaveBlock1Ptr->battleItemsUnlocked, 0, sizeof(gSaveBlock1Ptr->battleItemsUnlocked));
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_CHOICE_BAND)];
    for (u32 i = 0; i < pocket->capacity; i++)
        BagPocket_SetSlotItemIdAndCount(pocket, i, ITEM_LEFTOVERS, 1);
    EXPECT(!AddBagItem(ITEM_CHOICE_BAND, 1));
    EXPECT(!IsEmeraldChampionsBattleItemUnlocked(ITEM_CHOICE_BAND));
    EXPECT(!AddBagItem(ITEM_CHOICE_BAND, 0));
    EXPECT(!IsEmeraldChampionsBattleItemUnlocked(ITEM_CHOICE_BAND));
    BagPocket_SetSlotItemIdAndCount(pocket, 0, ITEM_NONE, 0);
    EXPECT(AddBagItem(ITEM_CHOICE_BAND, 1));
    EXPECT(IsEmeraldChampionsBattleItemUnlocked(ITEM_CHOICE_BAND));
    EXPECT(RemoveBagItem(ITEM_CHOICE_BAND, 1));
    EXPECT(IsEmeraldChampionsBattleItemUnlocked(ITEM_CHOICE_BAND));
}

TEST("Inclement integration: opening held items arrive together or not at all, never twice")
{
    static const enum Item items[] = {ITEM_CHOICE_BAND, ITEM_CHOICE_SPECS,
        ITEM_CHOICE_SCARF, ITEM_FOCUS_SASH, ITEM_EVIOLITE};
    ClearBag();
    FlagClear(FLAG_EC_RECEIVED_STARTER_BATTLE_ITEMS);
    memset(gSaveBlock1Ptr->battleItemsUnlocked, 0, sizeof(gSaveBlock1Ptr->battleItemsUnlocked));
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_CHOICE_BAND)];
    EXPECT_GT((u32)pocket->capacity, 6);
    for (u32 i = 0; i < pocket->capacity - 2; i++)
        BagPocket_SetSlotItemIdAndCount(pocket, i, ITEM_LEFTOVERS, 1);
    GiveEmeraldChampionsStarterBattleItems();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    EXPECT(!FlagGet(FLAG_EC_RECEIVED_STARTER_BATTLE_ITEMS));
    // All or nothing: a full Bag hands over no part of the kit.
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_CHOICE_BAND), 0);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_CHOICE_SPECS), 0);
    for (u32 i = 0; i < 3; i++)
        BagPocket_SetSlotItemIdAndCount(pocket, i, ITEM_NONE, 0);
    GiveEmeraldChampionsStarterBattleItems();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT(FlagGet(FLAG_EC_RECEIVED_STARTER_BATTLE_ITEMS));
    GiveEmeraldChampionsStarterBattleItems();
    for (u32 i = 0; i < ARRAY_COUNT(items); i++)
    {
        EXPECT_EQ(CountTotalItemQuantityInBag(items[i]), 1);
        EXPECT(IsEmeraldChampionsBattleItemUnlocked(items[i]));
    }
}

TEST("Inclement integration: Leveler reaches the current cap and remains safe on repeated use")
{
    struct Pokemon mon;
    for (u32 flag = FLAG_BADGE01_GET; flag <= FLAG_BADGE08_GET; flag++)
        FlagClear(flag);
    FlagClear(FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT);
    FlagClear(FLAG_IS_CHAMPION);
    FlagSet(FLAG_BADGE01_GET);
    EXPECT_EQ(GetCurrentLevelCap(), 20);
    CreateMon(&mon, SPECIES_LINOONE, 5, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT(RaiseMonToLevelerTarget(&mon));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), 20);
    EXPECT(!RaiseMonToLevelerTarget(&mon));
    EXPECT(!IsMonEligibleForLeveler(&mon));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), 20);
    FlagSet(FLAG_BADGE02_GET);
    EXPECT(RaiseMonToLevelerTarget(&mon));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_LEVEL), 30);
    EXPECT(!RaiseMonToLevelerTarget(&mon));
}

TEST("Inclement integration: Coins and cash are independent balances")
{
    SetMoney(&gSaveBlock1Ptr->money, 6000);
    SetCoins(0);
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 6000);
    EXPECT_EQ(GetCoins(), 0);
    EXPECT(AddCoins(50));
    RemoveMoney(&gSaveBlock1Ptr->money, 500);
    EXPECT_EQ(GetCoins(), 50);
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 5500);
    EXPECT(RemoveCoins(10));
    EXPECT_EQ(GetCoins(), 40);
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 5500);
}

TEST("Inclement integration: natural IVs keep three perfect stats and fixed IVs stay exact")
{
    struct Pokemon mon;
    u32 imperfect = 0;
    for (u32 sample = 0; sample < 8; sample++)
    {
        CreateRandomMon(&mon, SPECIES_ZIGZAGOON, 5);
        u32 perfect = 0;
        for (u32 stat = 0; stat < NUM_STATS; stat++)
            perfect += GetMonData(&mon, MON_DATA_HP_IV + stat) == MAX_PER_STAT_IVS;
        EXPECT_GE(perfect, 3);
        imperfect += perfect < NUM_STATS;
    }
    EXPECT_GT(imperfect, 0);
    SetBoxMonIVs(&mon.box, 0);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_IVS), 0);
    SetBoxMonIVs(&mon.box, MAX_PER_STAT_IVS);
    for (u32 stat = 0; stat < NUM_STATS; stat++)
        EXPECT_EQ(GetMonData(&mon, MON_DATA_HP_IV + stat), MAX_PER_STAT_IVS);
}

TEST("Inclement integration: owned captured and boxed held items unlock paid copies")
{
    struct Pokemon mon;
    memset(gSaveBlock1Ptr->battleItemsUnlocked, 0, sizeof(gSaveBlock1Ptr->battleItemsUnlocked));
    memset(gParties[B_TRAINER_PLAYER], 0, sizeof(gParties[B_TRAINER_PLAYER]));
    ResetPokemonStorageSystem(); // earlier tests in this worker may leave the PC full
    CreateRandomMon(&mon, SPECIES_ZIGZAGOON, 5);
    enum Item item = ITEM_LIFE_ORB;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    EXPECT(!IsEmeraldChampionsBattleItemUnlocked(item));
    EXPECT_EQ(GiveCapturedMonToPlayer(&mon), MON_GIVEN_TO_PARTY);
    EXPECT(IsEmeraldChampionsBattleItemUnlocked(item));
    item = ITEM_ASSAULT_VEST;
    SetMonData(&mon, MON_DATA_HELD_ITEM, &item);
    EXPECT(!IsEmeraldChampionsBattleItemUnlocked(item));
    EXPECT_EQ(CopyMonToPC(&mon), MON_GIVEN_TO_PC);
    EXPECT(IsEmeraldChampionsBattleItemUnlocked(item));
    EXPECT_GT(GetItemPrice(ITEM_ADAMANT_CRYSTAL), 0);
    EXPECT(!IsEmeraldChampionsFreeCatalogueItem(ITEM_ADAMANT_CRYSTAL));
    EXPECT(IsEmeraldChampionsFreeCatalogueItem(ITEM_ROTOM_CATALOG));
}


// Ultra Beast habitats, odds and capture gating are covered by test/ultra_beast_access.c.

TEST("Inclement integration: New Mauville discoveries use Wattson's native completion receipt")
{
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_2, 0);
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_3, 0);
    FlagSet(FLAG_BADGE01_GET);
    FlagSet(FLAG_BADGE02_GET);
    FlagSet(FLAG_BADGE03_GET);
    FlagSet(FLAG_BADGE04_GET);
    FlagSet(FLAG_BADGE05_GET);
    FlagSet(FLAG_BADGE06_GET);
    FlagClear(FLAG_GOT_TM24_FROM_WATTSON);
    FlagSet(FLAG_EC_REPORT_C28_COMPLETE);
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_ZERAORA));
    EXPECT(!CanAcquireLegendarySignSpecies(SPECIES_XURKITREE));
    FlagClear(FLAG_EC_REPORT_C28_COMPLETE);
    FlagSet(FLAG_GOT_TM24_FROM_WATTSON);
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_ZERAORA));
    EXPECT(CanAcquireLegendarySignSpecies(SPECIES_XURKITREE));
}


TEST("Inclement integration: restored Hoenn actors have compiled native graphics")
{
    static const u16 maps[] = {
        MAP_ROUTE111, MAP_ROUTE112, MAP_ROUTE133, MAP_JAGGED_PASS,
        MAP_ASHEN_WOODS, MAP_NEW_MAUVILLE_INSIDE, MAP_ALTERING_CAVE_B1F,
        MAP_EMBER_PATH, MAP_SHOAL_CAVE_LOW_TIDE_ICE_ROOM, MAP_SEALED_CHAMBER_INNER_ROOM,
    };
    for (u32 i = 0; i < ARRAY_COUNT(maps); i++)
    {
        const struct MapHeader *map = Overworld_GetMapHeaderByGroupAndId(maps[i] >> 8, maps[i] & 0xff);
        for (u32 j = 0; j < map->events->objectEventCount; j++)
        {
            const struct ObjectEventTemplate *object = &map->events->objectEvents[j];
            if (object->graphicsId >= OBJ_EVENT_GFX_VARS && object->graphicsId <= OBJ_EVENT_GFX_VAR_F)
                continue; // These depend on each scene's configured variables.
            const struct ObjectEventGraphicsInfo *info = GetObjectEventGraphicsInfo(object->graphicsId);
            if (info == NULL)
                Test_MgbaPrintf("Missing graphics: map %d, local actor %d, graphics %d", maps[i], object->localId, object->graphicsId);
            EXPECT(info != NULL);
            EXPECT(info->oam != NULL);
            EXPECT(info->anims != NULL);
            EXPECT(info->images != NULL);
            if (!info->compressed && !(object->graphicsId & OBJ_EVENT_MON))
            {
                // Berry trees reserve space for later growth stages, so the
                // initial frame may be smaller than the allocation, never larger.
                EXPECT_LE(info->images[0].size, info->size);
                EXPECT_LE(info->width * info->height / 2, info->size);
            }
        }
    }
}


u32 Test_ComputeCaptureOdds(u32 wildMonBattler, u32 playerBattler);

TEST("Inclement integration: Master Ball guarantees Ultra Beast capture without changing other balls")
{
    memset(gBattleMons, 0, sizeof(gBattleMons));
    gBattleTypeFlags = 0;
    gBattleMons[0].level = 70;
    gBattleMons[1].species = SPECIES_POIPOLE;
    gBattleMons[1].level = 50;
    gBattleMons[1].hp = gBattleMons[1].maxHP = 100;
    for (u32 flag = FLAG_BADGE01_GET; flag <= FLAG_BADGE08_GET; flag++)
        FlagSet(flag);

    gLastUsedItem = ITEM_MASTER_BALL;
    EXPECT_EQ(Test_ComputeCaptureOdds(1, 0), (u32)-1);
    gBattleMons[1].species = SPECIES_GUZZLORD;
    EXPECT_EQ(Test_ComputeCaptureOdds(1, 0), (u32)-1);
    gBattleMons[1].species = SPECIES_RATTATA;
    EXPECT_EQ(Test_ComputeCaptureOdds(1, 0), (u32)-1);

    gBattleMons[1].species = SPECIES_POIPOLE;
    gLastUsedItem = ITEM_POKE_BALL;
    u32 ordinaryOdds = Test_ComputeCaptureOdds(1, 0);
    EXPECT_NE(ordinaryOdds, (u32)-1);
    gLastUsedItem = ITEM_ULTRA_BALL;
    EXPECT_EQ(Test_ComputeCaptureOdds(1, 0), ordinaryOdds);
    gLastUsedItem = ITEM_BEAST_BALL;
    EXPECT_GT(Test_ComputeCaptureOdds(1, 0), ordinaryOdds);
    EXPECT_NE(Test_ComputeCaptureOdds(1, 0), (u32)-1);

    gBattleMons[1].species = SPECIES_RATTATA;
    gLastUsedItem = ITEM_POKE_BALL;
    ordinaryOdds = Test_ComputeCaptureOdds(1, 0);
    gLastUsedItem = ITEM_BEAST_BALL;
    EXPECT_LT(Test_ComputeCaptureOdds(1, 0), ordinaryOdds);
}


static bool32 sLearnMoveTestEnded;
static void LearnMoveTestTask(u8 taskId) {}
static void LearnMoveTestAsk(void) {}
static void LearnMoveTestPrint(const u8 *message) {}
static s32 LearnMoveTestConfirm(void) { return 0; }
static void LearnMoveTestFanfare(u32 songId) { PlayFanfare(songId); }
static void LearnMoveTestEnd(u8 taskId) { sLearnMoveTestEnded = TRUE; }

TEST("Inclement integration: empty-slot tutor waits for fanfare before UI teardown")
{
    static const struct MoveLearnUI ui = {
        .askConfirmation = LearnMoveTestAsk,
        .waitConfirmation = LearnMoveTestConfirm,
        .printMessage = LearnMoveTestPrint,
        .playFanfare = LearnMoveTestFanfare,
        .endTask = LearnMoveTestEnd,
    };
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ZIGZAGOON, 5, 0, OTID_STRUCT_PLAYER_ID);
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
        SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_NONE, slot);
    CalculatePlayerPartyCount();
    u8 taskId = CreateTask(LearnMoveTestTask, 0);
    EXPECT_NE(taskId, TASK_NONE);
    gTasks[taskId].data[0] = GetLearnMoveStartAfterPromptState();
    gTasks[taskId].data[1] = 0;
    gTasks[taskId].data[2] = MOVE_PROTECT;
    sLearnMoveTestEnded = FALSE;
    for (u32 step = 0; step < 12; step++)
        gTasks[taskId].data[0] = LearnMove(&ui, taskId);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_MOVE1), MOVE_PROTECT);
    EXPECT(!IsFanfareTaskInactive());
    EXPECT(!sLearnMoveTestEnded);

    for (u32 frame = 0; frame < 240 && !IsFanfareTaskInactive(); frame++)
        RunTasks();
    EXPECT(IsFanfareTaskInactive());
    for (u32 step = 0; step < 3 && !sLearnMoveTestEnded; step++)
        gTasks[taskId].data[0] = LearnMove(&ui, taskId);
    EXPECT(sLearnMoveTestEnded);
    DestroyTask(taskId);
}

extern void BufferChosenMonIV(void);
extern void ChangeChosenMonIVs(void);

TEST("Inclement integration: native IV service honors selected partner, stat and value")
{
    static const u8 requested[] = {0, 14, 31};
    ZeroPlayerPartyMons();
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][0], SPECIES_ZIGZAGOON, 20, 0, OTID_STRUCT_PLAYER_ID, 7);
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][1], SPECIES_POIPOLE, 20, 0, OTID_STRUCT_PLAYER_ID, 19);
    gSpecialVar_0x8004 = 1;
    gSpecialVar_0x8005 = STAT_SPEED;
    gSpecialVar_0x8006 = 99;
    BufferChosenMonIV();
    EXPECT_EQ(gSpecialVar_0x8006, 19);
    for (u32 i = 0; i < ARRAY_COUNT(requested); i++)
    {
        gSpecialVar_0x8006 = requested[i];
        ChangeChosenMonIVs();
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SPEED_IV), requested[i]);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_ATK_IV), 19);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPEED_IV), 7);
    }
}

extern void ChangeChosenMonHiddenPower(void);

TEST("Inclement integration: Hidden Power service selects the requested type on the saved partner")
{
    static const enum Type types[] = {TYPE_FIGHTING, TYPE_FLYING, TYPE_POISON, TYPE_GROUND,
        TYPE_ROCK, TYPE_BUG, TYPE_GHOST, TYPE_STEEL, TYPE_FIRE, TYPE_WATER, TYPE_GRASS,
        TYPE_ELECTRIC, TYPE_PSYCHIC, TYPE_ICE, TYPE_DRAGON, TYPE_DARK};
    ZeroPlayerPartyMons();
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][0], SPECIES_ZIGZAGOON, 20, 0, OTID_STRUCT_PLAYER_ID, 7);
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][1], SPECIES_POIPOLE, 20, 0, OTID_STRUCT_PLAYER_ID, 19);
    gSpecialVar_0x8004 = 1;
    gSpecialVar_0x800A = 0; // Script-owned vars are never read.
    for (u32 type = 0; type < ARRAY_COUNT(types); type++)
    {
        gSpecialVar_0x8005 = type;
        ChangeChosenMonHiddenPower();
        EXPECT_EQ(GetDynamicMoveType(&gParties[B_TRAINER_PLAYER][1], MOVE_HIDDEN_POWER,
            0, ABILITY_NONE, HOLD_EFFECT_NONE, MON_OUTSIDE_BATTLE), types[type]);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_ATK_IV), type == 15 ? 1 : 0);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP_IV), 7);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_ATK_IV), 7);
    }
}

extern void BufferChosenMonEV(void);
extern void BufferVarsForIVRater(void);
extern void ChangePokemonNature(void);

TEST("Inclement integration: native stat reports and nature honor their script contracts")
{
    static const u8 evData[] = {MON_DATA_HP_EV, MON_DATA_ATK_EV, MON_DATA_DEF_EV,
        MON_DATA_SPEED_EV, MON_DATA_SPATK_EV, MON_DATA_SPDEF_EV};
    ZeroPlayerPartyMons();
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][0], SPECIES_ZIGZAGOON, 20, 0, OTID_STRUCT_PLAYER_ID, 7);
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][1], SPECIES_POIPOLE, 20, 0, OTID_STRUCT_PLAYER_ID, 19);
    u32 originalNature = GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HIDDEN_NATURE);
    gSpecialVar_0x8004 = 1;
    for (u32 stat = 0; stat < NUM_STATS; stat++)
    {
        u32 ev = 4 + stat * 4;
        SetMonData(&gParties[B_TRAINER_PLAYER][1], evData[stat], &ev);
        gSpecialVar_0x8005 = stat;
        gSpecialVar_0x8006 = 99;
        BufferChosenMonEV();
        EXPECT_EQ(gSpecialVar_0x8006, ev);
    }
    u32 best = 31;
    SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SPATK_IV, &best);
    BufferVarsForIVRater();
    EXPECT_EQ(gSpecialVar_0x8005, 19 * 5 + 31);
    EXPECT_EQ(gSpecialVar_0x8006, STAT_SPATK);
    EXPECT_EQ(gSpecialVar_0x8007, 31);
    u32 neutral = NATURE_HARDY;
    SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HIDDEN_NATURE, &neutral);
    CalculateMonStats(&gParties[B_TRAINER_PLAYER][1]);
    u32 oldAttack = GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_ATK);
    u32 oldSpAttack = GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SPATK);
    gSpecialVar_0x8005 = 3; // Special Attack raised, Attack lowered: Modest.
    gSpecialVar_0x8006 = 0;
    ChangePokemonNature();
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HIDDEN_NATURE), NATURE_MODEST);
    EXPECT_LT(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_ATK), oldAttack);
    EXPECT_GT(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SPATK), oldSpAttack);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HIDDEN_NATURE), originalNature);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPATK_IV), 7);
}

extern void CheckChosenMonCanGainEVs(void);
extern void IncreaseChosenMonEVs(void);
extern bool8 Special_AreLeadMonEVsMaxedOut(void);

TEST("Inclement integration: EV service reports resulting stat and rejection total")
{
    ZeroPlayerPartyMons();
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][0], SPECIES_ZIGZAGOON, 20, 0, OTID_STRUCT_PLAYER_ID, 7);
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][1], SPECIES_POIPOLE, 20, 0, OTID_STRUCT_PLAYER_ID, 19);
    u32 ev = 64;
    SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HP_EV, &ev);
    gSpecialVar_0x8004 = 1;
    gSpecialVar_0x8005 = STAT_HP;
    gSpecialVar_0x8006 = 4;
    gSpecialVar_0x8007 = 999;
    gSpecialVar_0x8008 = 999; // Script-owned: must survive the call.
    CheckChosenMonCanGainEVs();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT_EQ(gSpecialVar_0x8007, 64);
    EXPECT_EQ(gSpecialVar_0x8008, 999);
    IncreaseChosenMonEVs();
    EXPECT_EQ(gSpecialVar_0x8007, 68);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HP_EV), 68);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP_EV), 0);
    ev = 252;
    SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_ATK_EV, &ev);
    ev = 190;
    SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_DEF_EV, &ev);
    gSpecialVar_0x8005 = STAT_SPEED;
    CheckChosenMonCanGainEVs();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    EXPECT_EQ(gSpecialVar_0x8007, 510);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SPEED_EV), 0);
}

TEST("Inclement integration: Effort Ribbon predicate returns a boolean for the non-egg lead")
{
    ZeroPlayerPartyMons();
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][0], SPECIES_ZIGZAGOON, 20, 0, OTID_STRUCT_PLAYER_ID, 7);
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][1], SPECIES_POIPOLE, 20, 0, OTID_STRUCT_PLAYER_ID, 19);
    u32 ev = 252;
    SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HP_EV, &ev);
    SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_ATK_EV, &ev);
    ev = 6;
    SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_DEF_EV, &ev);
    gSpecialVar_0x8004 = 1; // Ribbon uses the lead, not a stale service selection.
    EXPECT_EQ(Special_AreLeadMonEVsMaxedOut(), FALSE);
    u32 isEgg = TRUE;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_IS_EGG, &isEgg);
    EXPECT_EQ(GetLeadMonIndex(), 1);
    EXPECT_EQ(Special_AreLeadMonEVsMaxedOut(), TRUE);
}

TEST("Inclement integration: Flash uses the regional badge plus authorization")
{
    FlagClear(FLAG_BADGE01_GET);
    FlagClear(FLAG_BADGE02_GET);
    FlagClear(FLAG_RECEIVED_HM_FLASH);
    EXPECT(!IsFieldMoveUnlocked(FIELD_MOVE_FLASH));
    FlagSet(FLAG_BADGE01_GET);
    FlagSet(FLAG_RECEIVED_HM_FLASH);
    EXPECT_EQ(IsFieldMoveUnlocked(FIELD_MOVE_FLASH), IS_FRLG);
    FlagSet(FLAG_BADGE02_GET);
    EXPECT(IsFieldMoveUnlocked(FIELD_MOVE_FLASH));
    FlagClear(FLAG_RECEIVED_HM_FLASH);
    EXPECT_EQ(IsFieldMoveUnlocked(FIELD_MOVE_FLASH), IS_FRLG);
}

TEST("Inclement integration: locked field message distinguishes badge from authorization")
{
    u32 badgeFlag = IS_FRLG ? FLAG_BADGE06_GET : FLAG_BADGE03_GET;
    FlagClear(badgeFlag);
    FlagSet(FLAG_RECEIVED_HM_ROCK_SMASH);
    gSpecialVar_0x8004 = FIELD_MOVE_ROCK_SMASH;
    EXPECT(!IsFieldMoveUnlocked(FIELD_MOVE_ROCK_SMASH));
    BufferFieldMoveUnlockRequirement();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    EXPECT_EQ(StringCompare(gStringVar2, IS_FRLG ? COMPOUND_STRING("Marsh Badge") : COMPOUND_STRING("Dynamo Badge")), 0);
    FlagSet(badgeFlag);
    FlagClear(FLAG_RECEIVED_HM_ROCK_SMASH);
    EXPECT_EQ(IsFieldMoveUnlocked(FIELD_MOVE_ROCK_SMASH), IS_FRLG);
    BufferFieldMoveUnlockRequirement();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT(FlagGet(badgeFlag));
    EXPECT(!FlagGet(FLAG_RECEIVED_HM_ROCK_SMASH));
    FlagSet(FLAG_RECEIVED_HM_ROCK_SMASH);
    EXPECT(IsFieldMoveUnlocked(FIELD_MOVE_ROCK_SMASH));
    BufferFieldMoveUnlockRequirement();
    EXPECT_EQ(gSpecialVar_Result, 2);
}

TEST("Inclement integration: HM convenience preserves the Regi puzzle conditions")
{
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_WAILORD, 40, 0, OTID_STRUCT_PLAYER_ID);
    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_RELICANTH, 40, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT(CheckRelicanthWailord());
    struct Pokemon swap = gParties[B_TRAINER_PLAYER][0];
    gParties[B_TRAINER_PLAYER][0] = gParties[B_TRAINER_PLAYER][1];
    gParties[B_TRAINER_PLAYER][1] = swap;
    EXPECT(!CheckRelicanthWailord());

    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_DESERT_RUINS);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_DESERT_RUINS);
    gSaveBlock1Ptr->pos.x = 6;
    gSaveBlock1Ptr->pos.y = 23;
    FlagClear(FLAG_SYS_REGIROCK_PUZZLE_COMPLETED);
    EXPECT(ShouldDoBrailleRegirockEffect());
    gSaveBlock1Ptr->pos.y++;
    EXPECT(!ShouldDoBrailleRegirockEffect());
    gSaveBlock1Ptr->pos.y--;
    FlagSet(FLAG_SYS_REGIROCK_PUZZLE_COMPLETED);
    EXPECT(!ShouldDoBrailleRegirockEffect());
    FlagClear(FLAG_SYS_REGIROCK_PUZZLE_COMPLETED);

    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ANCIENT_TOMB);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ANCIENT_TOMB);
    gSaveBlock1Ptr->pos.x = 8;
    gSaveBlock1Ptr->pos.y = 25;
    FlagClear(FLAG_SYS_REGISTEEL_PUZZLE_COMPLETED);
    EXPECT(ShouldDoBrailleRegisteelEffect());
    gSaveBlock1Ptr->pos.x++;
    EXPECT(!ShouldDoBrailleRegisteelEffect());
    gSaveBlock1Ptr->pos.x--;
    FlagSet(FLAG_SYS_REGISTEEL_PUZZLE_COMPLETED);
    EXPECT(!ShouldDoBrailleRegisteelEffect());
    FlagClear(FLAG_SYS_REGISTEEL_PUZZLE_COMPLETED);

    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_SEALED_CHAMBER_OUTER_ROOM);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_SEALED_CHAMBER_OUTER_ROOM);
    gSaveBlock1Ptr->pos.x = 10;
    gSaveBlock1Ptr->pos.y = 3;
    FlagClear(FLAG_SYS_BRAILLE_DIG);
    EXPECT(ShouldDoBrailleDigEffect());
    gSaveBlock1Ptr->pos.y++;
    EXPECT(!ShouldDoBrailleDigEffect());
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_MEW, 40, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT(SpeciesCanLearnFieldMove(SPECIES_MEW, MOVE_DIG));
    EXPECT_EQ(FieldMove_GetUserSlot(FIELD_MOVE_DIG, TRUE), PARTY_SIZE);
    SetMonMoveSlot(&gParties[B_TRAINER_PLAYER][0], MOVE_DIG, 0);
    EXPECT_EQ(FieldMove_GetUserSlot(FIELD_MOVE_DIG, TRUE), 0);
    ZeroPlayerPartyMons();
}

TEST("Inclement integration: Regenerator restores consumed berries after battle")
{
    static EWRAM_DATA struct BattleStruct state;
    struct BattleStruct *saved = gBattleStruct;
    gBattleStruct = &state;
    memset(&state, 0, sizeof(state));
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_MAGIKARP, 14, 0, OTID_STRUCT_PLAYER_ID);
    enum Item berry = ITEM_SITRUS_BERRY;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &berry);
    RecordPlayerPartyMonHeldItemForRestoration(0);
    state.partyState[B_TRAINER_PLAYER][0].usedHeldItem = ITEM_SITRUS_BERRY;
    enum Item consumed = ITEM_NONE;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &consumed);
    AddBagItem(ITEM_REGENERATOR, 1);
    TryRestoreHeldItems();
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM), ITEM_SITRUS_BERRY);
    RemoveBagItem(ITEM_REGENERATOR, 1);
    gBattleStruct = saved;
    ZeroPlayerPartyMons();
}

extern const u8 HyperTraining_EventScript_SelectHiddenPower[];
extern const u8 HyperTraining_EventScript_ChangeAllIVs[];
extern const u8 FallarborTown_HyperMainMenu[];
extern ScrCmdFunc gScriptCmdTable[];
extern ScrCmdFunc gScriptCmdTableEnd[];

TEST("Inclement integration: Hidden Power menu preserves every type index and cancellation partner")
{
    static const u16 results[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 127};
    for (u32 i = 0; i < ARRAY_COUNT(results); i++)
    {
        gSpecialVar_Result = results[i];
        gSpecialVar_0x8004 = 99;
        gSpecialVar_0x8005 = 255;
        gSpecialVar_0x800A = 1;
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, HyperTraining_EventScript_SelectHiddenPower);
        for (u32 step = 0; step < 6
          && ctx.scriptPtr != HyperTraining_EventScript_ChangeAllIVs
          && ctx.scriptPtr != FallarborTown_HyperMainMenu; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        if (results[i] < 16)
        {
            EXPECT_EQ(ctx.scriptPtr, HyperTraining_EventScript_ChangeAllIVs);
            // The service's arguments: party slot and type.
            EXPECT_EQ(gSpecialVar_0x8004, 1);
            EXPECT_EQ(gSpecialVar_0x8005, results[i]);
        }
        else
        {
            EXPECT_EQ(ctx.scriptPtr, FallarborTown_HyperMainMenu);
            EXPECT_EQ(gSpecialVar_0x8004, 1);
            EXPECT_EQ(gSpecialVar_0x8005, 255);
        }
    }
}

TEST("Inclement integration: EV mutation refuses over-cap spreads and saturates valid gains")
{
    static const struct {u16 hp, attack, defense, request, expected;} cases[] = {
        {0, 252, 252, 12, 6},
        {250, 0, 0, 12, 252},
        {255, 0, 0, 4, 255},
        {100, 252, 252, 4, 100},
        {0, 0, 0, 65535, 252},
        {0, 252, 252, 0, 0},
    };
    ZeroPlayerPartyMons();
    for (u32 i = 0; i < ARRAY_COUNT(cases); i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
        CreateMonWithIVs(mon, SPECIES_EEVEE, 20, 0, OTID_STRUCT_PLAYER_ID, 31);
        u32 value = cases[i].hp;
        SetMonData(mon, MON_DATA_HP_EV, &value);
        value = cases[i].attack;
        SetMonData(mon, MON_DATA_ATK_EV, &value);
        value = cases[i].defense;
        SetMonData(mon, MON_DATA_DEF_EV, &value);
        CalculateMonStats(mon);
        struct Pokemon before;
        memcpy(&before, mon, sizeof(before));
        gSpecialVar_0x8004 = 0;
        gSpecialVar_0x8005 = STAT_HP;
        gSpecialVar_0x8006 = cases[i].request;
        IncreaseChosenMonEVs();
        EXPECT_EQ(GetMonData(mon, MON_DATA_HP_EV), cases[i].expected);
        EXPECT_EQ(gSpecialVar_0x8007, cases[i].expected);
        EXPECT_EQ(gSpecialVar_Result, cases[i].expected > cases[i].hp);
        EXPECT_EQ(GetMonData(mon, MON_DATA_ATK_EV), cases[i].attack);
        EXPECT_EQ(GetMonData(mon, MON_DATA_DEF_EV), cases[i].defense);
        if (cases[i].hp >= MAX_PER_STAT_EVS || cases[i].hp + cases[i].attack + cases[i].defense >= MAX_TOTAL_EVS)
            EXPECT_EQ(memcmp(&before, mon, sizeof(before)), 0);
    }
    ZeroPlayerPartyMons();
}

TEST("Inclement integration: lottery initializes prizes and matches party PC and leading-zero IDs")
{
    const struct {u16 id, tier, prize;} cases[] = {
        {11445, 1, ITEM_PP_UP}, {13345, 2, ITEM_BOTTLE_CAP},
        {22345, 3, ITEM_MAX_REVIVE}, {12345, 4, ITEM_MASTER_BALL},
        {12346, 0, ITEM_NONE}, {9995, 0, ITEM_NONE},
    };
    ZeroPlayerPartyMons();
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            ZeroBoxMonAt(box, slot);
    struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][0];
    for (u32 i = 0; i < ARRAY_COUNT(cases); i++)
    {
        u32 id = 0xABCD0000u | cases[i].id;
        CreateMon(mon, SPECIES_EEVEE, 10, 0, OTID_STRUCT_PRESET(id));
        gSpecialVar_Result = 12345;
        gSpecialVar_0x8005 = ITEM_POTION; // Unrelated previous script value.
        PickLotteryCornerTicket();
        EXPECT_EQ(gSpecialVar_0x8004, cases[i].tier);
        EXPECT_EQ(gSpecialVar_0x8005, cases[i].prize);
        if (cases[i].tier)
            EXPECT_EQ(gSpecialVar_0x8006, 0);
    }
    // Leading zeros count toward all five digits; boxed mons qualify, Eggs do not.
    u32 id = 42;
    CreateMon(mon, SPECIES_EEVEE, 10, 0, OTID_STRUCT_PRESET(id));
    gPokemonStoragePtr->boxes[0][0] = mon->box;
    bool8 egg = TRUE;
    SetMonData(mon, MON_DATA_IS_EGG, &egg);
    gSpecialVar_Result = 42;
    PickLotteryCornerTicket();
    EXPECT_EQ(gSpecialVar_0x8004, 4);
    EXPECT_EQ(gSpecialVar_0x8005, ITEM_MASTER_BALL);
    EXPECT_EQ(gSpecialVar_0x8006, 1);
    SetBoxMonData(&gPokemonStoragePtr->boxes[0][0], MON_DATA_IS_EGG, &egg);
    PickLotteryCornerTicket();
    EXPECT_EQ(gSpecialVar_0x8004, 0);
    EXPECT_EQ(gSpecialVar_0x8005, ITEM_NONE);
    ZeroBoxMonAt(0, 0);
    ZeroPlayerPartyMons();
}

extern void ResetChosenMonEVs(void);
extern void ChangePokemonNature(void);

TEST("Inclement integration: invalid service selections never mutate any party member")
{
    static const u16 slots[] = {PARTY_SIZE, 255, 65535, 1, 2};
    static void (*const services[])(void) = {
        IncreaseChosenMonEVs, ResetChosenMonEVs, ChangeChosenMonIVs,
        ChangeChosenMonHiddenPower, ChangePokemonNature,
    };
    for (u32 i = 0; i < ARRAY_COUNT(slots); i++)
    for (u32 service = 0; service < ARRAY_COUNT(services); service++)
    {
        ZeroPlayerPartyMons();
        CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][0], SPECIES_EEVEE, 20, 0, OTID_STRUCT_PLAYER_ID, 17);
        CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][2], SPECIES_TOGEPI, 20, 0, OTID_STRUCT_PLAYER_ID, 17);
        u32 value = TRUE;
        SetMonData(&gParties[B_TRAINER_PLAYER][2], MON_DATA_IS_EGG, &value);
        value = 100;
        SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP_EV, &value);
        CalculateMonStats(&gParties[B_TRAINER_PLAYER][0]);
        struct Pokemon before[PARTY_SIZE];
        memcpy(before, gParties[B_TRAINER_PLAYER], sizeof(before));
        gSpecialVar_0x8004 = slots[i];
        gSpecialVar_0x800A = slots[i];
        gSpecialVar_0x8005 = STAT_HP;
        gSpecialVar_0x8006 = 4;
        gSpecialVar_0x8007 = 0;
        services[service]();
        EXPECT_EQ(memcmp(before, gParties[B_TRAINER_PLAYER], sizeof(before)), 0);
    }
    ZeroPlayerPartyMons();
}

extern void BufferChosenMonAllEVs(void);
extern void BufferChosenMonAllIVs(void);
extern void BufferChosenMonNature(void);

TEST("Inclement integration: stat service buffers clear invalid selections and preserve valid formatting")
{
    ZeroPlayerPartyMons();
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][0], SPECIES_EEVEE, 20, 0, OTID_STRUCT_PLAYER_ID, 17);
    gSpecialVar_0x8004 = 0;
    BufferChosenMonAllEVs();
    EXPECT_EQ(StringCompare(gStringVar4, COMPOUND_STRING("0/0/0/0/0/0")), 0);
    BufferChosenMonAllIVs();
    EXPECT_EQ(StringCompare(gStringVar4, COMPOUND_STRING("17/17/17/17/17/17")), 0);
    static void (*const buffers[])(void) = {BufferChosenMonAllEVs, BufferChosenMonAllIVs,
        BufferChosenMonEV, BufferChosenMonIV, BufferChosenMonNature};
    gSpecialVar_0x8004 = PARTY_SIZE;
    for (u32 i = 0; i < ARRAY_COUNT(buffers); i++)
    {
        StringCopy(gStringVar2, COMPOUND_STRING("stale"));
        StringCopy(gStringVar4, COMPOUND_STRING("stale"));
        buffers[i]();
        EXPECT_EQ(StringCompare(i < 2 ? gStringVar4 : gStringVar2, COMPOUND_STRING("")), 0);
    }
    gSpecialVar_0x8005 = gSpecialVar_0x8006 = gSpecialVar_0x8007 = 999;
    BufferVarsForIVRater();
    EXPECT_EQ(gSpecialVar_0x8005, 0);
    EXPECT_EQ(gSpecialVar_0x8006, 0);
    EXPECT_EQ(gSpecialVar_0x8007, 0);
    gSpecialVar_0x8004 = 0;
    gSpecialVar_0x8005 = NUM_STATS;
    gSpecialVar_0x8006 = 4;
    CheckChosenMonCanGainEVs();
    EXPECT(!gSpecialVar_Result);
    struct Pokemon before;
    memcpy(&before, &gParties[B_TRAINER_PLAYER][0], sizeof(before));
    IncreaseChosenMonEVs();
    EXPECT(!gSpecialVar_Result);
    EXPECT_EQ(memcmp(&before, &gParties[B_TRAINER_PLAYER][0], sizeof(before)), 0);
    BufferChosenMonEV();
    EXPECT_EQ(gSpecialVar_0x8006, 0);
    EXPECT_EQ(StringCompare(gStringVar2, COMPOUND_STRING("")), 0);
    ZeroPlayerPartyMons();
}
