#include "global.h"
#include "berry.h"
#include "event_data.h"
#include "field_specials.h"
#include "item.h"
#include "item_use.h"
#include "legendary_signs.h"
#include "mega_stone_rewards.h"
#include "pokemon.h"
#include "test/test.h"
#include "constants/emerald_champions.h"

static void ResetHarvest(void)
{
    ClearBag();
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
    ZeroPlayerPartyMons();
    memset(gSaveBlock2Ptr->pokedex.harvestedBerries, 0, NUM_BERRIES);
    gSaveBlock2Ptr->pokedex.gardenCelebiUnlocked = FALSE;
    FlagClear(FLAG_EC_BERRY_TRADE_BAXCALIBRITE);
    FlagClear(FLAG_EC_BERRY_TRADE_DRAGONINITE);
    FlagClear(FLAG_EC_BERRY_TRADE_TYRANITARITE);
}

TEST("Harvest economy: ordinary free stock never pays a harvest recipe")
{
    ResetHarvest();
    for (u32 berry = 1; berry < NUM_BERRIES; berry++)
    {
        enum Item item = BerryTypeToItemId(berry);
        EXPECT(AddBagItem(item, 30));
        EXPECT_EQ(GetHarvestedBerryCount(berry), 0);
        // Berries are bought now, so they sell back like any other stock. What
        // this test is really guarding is that ordinary stock never counts as
        // harvest, which is the assertion above and the trade attempts below.
        EXPECT_EQ(GetItemSellPrice(item), GetItemPrice(item) / ITEM_SELL_FACTOR);
        EXPECT(GetItemFieldFunc(item) == ItemUseOutOfBattle_CannotUse);
    }
    for (u32 choice = 0; choice < 3; choice++)
    {
        gSpecialVar_0x8004 = choice;
        TradeEmeraldChampionsGardenBerries();
        EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_NOT_ENOUGH);
    }
    ResetHarvest();
}

TEST("Harvest economy: each stone spends typed harvest once and leaves equipment untouched")
{
    u32 choice;
    enum Item stone;
    PARAMETRIZE { choice = 0; stone = ITEM_BAXCALIBRITE; }
    PARAMETRIZE { choice = 1; stone = ITEM_DRAGONINITE; }
    PARAMETRIZE { choice = 2; stone = ITEM_TYRANITARITE; }
    ResetHarvest();
    memset(gSaveBlock2Ptr->pokedex.harvestedBerries, 30, NUM_BERRIES);
    EXPECT(AddBagItem(ITEM_LUM_BERRY, 6));
    gSpecialVar_0x8004 = choice;
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_SUCCESS);
    EXPECT_EQ(CountTotalItemQuantityInBag(stone), 1);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_LUM_BERRY), 6);
    u32 remaining = 0;
    for (u32 berry = 1; berry <= NUM_BERRIES; berry++)
        remaining += GetHarvestedBerryCount(berry);
    EXPECT_EQ(remaining, 30 * NUM_BERRIES - (choice == 0 ? 20 : 24));
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_ALREADY_DONE);
    u32 after = 0;
    for (u32 berry = 1; berry <= NUM_BERRIES; berry++)
        after += GetHarvestedBerryCount(berry);
    EXPECT_EQ(after, remaining);
    ResetHarvest();
}

TEST("Harvest economy: missing one type cannot be replaced by a surplus of another")
{
    ResetHarvest();
    memset(gSaveBlock2Ptr->pokedex.harvestedBerries, 30, NUM_BERRIES);
    gSaveBlock2Ptr->pokedex.harvestedBerries[BERRY_ID_BLUK - 1] = 5;
    AddHarvestedBerries(BERRY_ID_RAZZ, 200);
    gSpecialVar_0x8004 = 0;
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_NOT_ENOUGH);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_RAZZ), 230);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_BLUK), 5);
    EXPECT(!FlagGet(FLAG_EC_BERRY_TRADE_BAXCALIBRITE));
    ResetHarvest();
}

TEST("Harvest economy: full reward pocket preserves all payment and allows retry")
{
    ResetHarvest();
    memset(gSaveBlock2Ptr->pokedex.harvestedBerries, 30, NUM_BERRIES);
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_BAXCALIBRITE)];
    for (u32 i = 0; i < pocket->capacity; i++)
        BagPocket_SetSlotItemIdAndCount(pocket, i, ITEM_DRAGONINITE, 1);
    gSpecialVar_0x8004 = 0;
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_BAG_FULL);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_RAZZ), 30);
    EXPECT(!FlagGet(FLAG_EC_BERRY_TRADE_BAXCALIBRITE));
    BagPocket_SetSlotItemIdAndCount(pocket, 0, ITEM_NONE, 0);
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_SUCCESS);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_RAZZ), 24);
    ResetHarvest();
}

TEST("Harvest economy: existing PC ownership cannot charge harvest again")
{
    ResetHarvest();
    memset(gSaveBlock2Ptr->pokedex.harvestedBerries, 30, NUM_BERRIES);
    EXPECT(AddPCItem(ITEM_BAXCALIBRITE, 1));
    gSpecialVar_0x8004 = 0;
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_ALREADY_DONE);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_RAZZ), 30);
    ResetHarvest();
}

TEST("Harvest economy: quantities saturate without wrapping or spilling into the next berry")
{
    ResetHarvest();
    AddHarvestedBerries(BERRY_ID_LUM, 254);
    AddHarvestedBerries(BERRY_ID_LUM, 2);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_LUM), 255);
    AddHarvestedBerries(BERRY_ID_LUM, 1);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_LUM), 255);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_SITRUS), 0);
    AddHarvestedBerries(0, 1);
    AddHarvestedBerries(NUM_BERRIES + 1, 1);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_CHERI), 0);
    ResetHarvest();
}

TEST("Harvest economy: Celebi invitation is permanent and does not claim a capture")
{
    ResetHarvest();
    // A fresh encounter state, separate from the earned campaign.
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0);
    FlagClear(FLAG_EC_CAUGHT_CELEBI);
    memset(gSaveBlock2Ptr->pokedex.harvestedBerries, 30, NUM_BERRIES);
    gSpecialVar_0x8004 = 3;
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_SUCCESS);
    EXPECT(!IsLegendarySignCaught(LEGENDARY_SIGN_CELEBI));
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_LUM), 26);
    CheckEmeraldChampionsGardenCelebi();
    EXPECT_EQ(gSpecialVar_Result, 1);
    TradeEmeraldChampionsGardenBerries();
    EXPECT_EQ(gSpecialVar_Result, EC_MEGA_BERRY_TRADE_ALREADY_DONE);
    EXPECT_EQ(GetHarvestedBerryCount(BERRY_ID_LUM), 26);
    MarkLegendarySignCaughtBySpecies(SPECIES_CELEBI);
    CheckEmeraldChampionsGardenCelebi();
    EXPECT_EQ(gSpecialVar_Result, 2);
    ResetHarvest();
    VarSet(VAR_LEGENDARY_SIGNS_CAUGHT_0, 0);
    FlagClear(FLAG_EC_CAUGHT_CELEBI);
}

// Norman's starter stone gift (src/mega_stone_rewards.c; reached by callnative).
u16 GetNormanStarterMegaStone(void);
u16 GetNormanPartnerMegaStone(void);
void BufferNormanMegaGiftKind(void);
void MarkStarterMegaStoneReceived(void);

enum { GIFT_SEVERAL, GIFT_ONE, GIFT_FALLBACK, GIFT_ALREADY_HELD };

// The specification: every starter Mega Stone and the one receipt its homes share.
static const struct { u16 item; u16 flag; bool8 hoenn; } sStarterStones[] =
{
    {ITEM_VENUSAURITE,    FLAG_ITEM_PETALBURG_CITY_VENUSAURITE, FALSE},
    {ITEM_CHARIZARDITE_X, FLAG_ITEM_FIERY_PATH_CHARIZARDITE_X,  FALSE},
    {ITEM_CHARIZARDITE_Y, FLAG_EMBER_PATH_CHARIZARDITE_Y,       FALSE},
    {ITEM_BLASTOISINITE,  FLAG_SEASPRAY_CAVE_BLASTOISINITE,     FALSE},
    {ITEM_MEGANIUMITE,    FLAG_ITEM_GRANITE_CAVE_B1F_TM65,      FALSE},
    {ITEM_FERALIGITE,     FLAG_RECEIVED_TM03,                   FALSE},
    {ITEM_SCEPTILITE,     FLAG_EC_MEGA_GIFT_SCEPTILITE,         TRUE},
    {ITEM_BLAZIKENITE,    FLAG_EC_MEGA_GIFT_BLAZIKENITE,        TRUE},
    {ITEM_SWAMPERTITE,    FLAG_EC_MEGA_GIFT_SWAMPERTITE,        TRUE},
    {ITEM_EMBOARITE,      FLAG_RECEIVED_TM08,                   FALSE},
    {ITEM_CHESNAUGHTITE,  FLAG_EMBER_PATH_SMACK_DOWN,           FALSE},
    {ITEM_DELPHOXITE,     FLAG_RECEIVED_TM39,                   FALSE},
    {ITEM_GRENINJITE,     FLAG_ITEM_ROUTE_119_TM62_ACROBATICS,  FALSE},
};

static u16 sSavedStarterVars[4];

static void ResetStarterStones(u16 generation, u16 first, u16 second)
{
    ClearBag();
    memset(gSaveBlock1Ptr->pcItems, 0, sizeof(gSaveBlock1Ptr->pcItems));
    ZeroPlayerPartyMons();
    for (u32 i = 0; i < ARRAY_COUNT(sStarterStones); i++)
        FlagClear(sStarterStones[i].flag);
    VarSet(VAR_STARTER_GEN, generation);
    VarSet(VAR_STARTER_MON, first);
    VarSet(VAR_EC_SECOND_STARTER, second + 1);
    VarSet(VAR_EC_OPENING_STATE, EC_OPENING_PAIR_GRANTED);
}

static void SaveStarterVars(void)
{
    sSavedStarterVars[0] = VarGet(VAR_STARTER_GEN);
    sSavedStarterVars[1] = VarGet(VAR_STARTER_MON);
    sSavedStarterVars[2] = VarGet(VAR_EC_SECOND_STARTER);
    sSavedStarterVars[3] = VarGet(VAR_EC_OPENING_STATE);
}

static void RestoreStarterVars(void)
{
    ClearBag();
    ZeroPlayerPartyMons();
    for (u32 i = 0; i < ARRAY_COUNT(sStarterStones); i++)
        FlagClear(sStarterStones[i].flag);
    VarSet(VAR_STARTER_GEN, sSavedStarterVars[0]);
    VarSet(VAR_STARTER_MON, sSavedStarterVars[1]);
    VarSet(VAR_EC_SECOND_STARTER, sSavedStarterVars[2]);
    VarSet(VAR_EC_OPENING_STATE, sSavedStarterVars[3]);
}

// PetalburgCity_Gym_EventScript_NormanGiveKeystone: Ring, then each stone
// through giveitem, then the receipt. Returns the gift kind.
static u32 RunNormanGift(void)
{
    u32 kind;

    EXPECT(CanReceiveNormanMegaGift());
    EXPECT(AddBagItem(ITEM_MEGA_RING, 1));
    BufferNormanMegaGiftKind();
    kind = gSpecialVar_Result;
    for (u32 guard = 0; guard <= ARRAY_COUNT(sStarterStones); guard++)
    {
        u16 stone = GetNormanStarterMegaStone();
        if (stone == ITEM_NONE)
            break;
        EXPECT_LT(guard, ARRAY_COUNT(sStarterStones));
        EXPECT(AddBagItem(stone, 1));
        gSpecialVar_0x8004 = stone;
        MarkStarterMegaStoneReceived();
    }
    EXPECT_EQ(GetNormanStarterMegaStone(), ITEM_NONE);
    return kind;
}

// PetalburgCity_Gym_EventScript_NormanPartnerStones after the battle.
static u32 RunNormanPartnerGifts(void)
{
    u32 given = 0;
    for (u32 guard = 0; guard <= 3; guard++)
    {
        u16 stone = GetNormanPartnerMegaStone();
        if (stone == ITEM_NONE)
            break;
        EXPECT_LT(guard, 3);
        EXPECT(AddBagItem(stone, 1));
        gSpecialVar_0x8004 = stone;
        MarkStarterMegaStoneReceived();
        given++;
    }
    return given;
}

static void SetParty(const u16 *species, u32 count)
{
    ZeroPlayerPartyMons();
    for (u32 slot = 0; slot < count; slot++)
        CreateMon(&gParties[B_TRAINER_PLAYER][slot], species[slot], 20, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
}

static u32 StoneCount(u16 item)
{
    return CountTotalItemQuantityInBag(item);
}

TEST("Norman's Mega gift: a Charizard pair receives Charizardite X and Y")
{
    u16 partner, stone;
    PARAMETRIZE { partner = 0; stone = ITEM_VENUSAURITE; }
    PARAMETRIZE { partner = 2; stone = ITEM_BLASTOISINITE; }
    SaveStarterVars();
    ResetStarterStones(1, 1, partner);
    EXPECT_EQ(RunNormanGift(), GIFT_SEVERAL);
    EXPECT_EQ(StoneCount(ITEM_CHARIZARDITE_X), 1);
    EXPECT_EQ(StoneCount(ITEM_CHARIZARDITE_Y), 1);
    EXPECT_EQ(StoneCount(stone), 1);
    EXPECT_EQ(StoneCount(ITEM_SWAMPERTITE), 0);
    EXPECT_EQ(StoneCount(ITEM_SCEPTILITE) + StoneCount(ITEM_BLAZIKENITE), 0);
    // Norman's copies close the Fiery Path and Ember Path sparkles.
    EXPECT(FlagGet(FLAG_ITEM_FIERY_PATH_CHARIZARDITE_X));
    EXPECT(FlagGet(FLAG_EMBER_PATH_CHARIZARDITE_Y));
    EXPECT_EQ(FlagGet(FLAG_ITEM_PETALBURG_CITY_VENUSAURITE), stone == ITEM_VENUSAURITE);
    EXPECT_EQ(FlagGet(FLAG_SEASPRAY_CAVE_BLASTOISINITE), stone == ITEM_BLASTOISINITE);
    RestoreStarterVars();
}

TEST("Norman's Mega gift: a Hoenn pair receives both stones and the third waits for its partner")
{
    SaveStarterVars();
    ResetStarterStones(3, 0, 2);
    EXPECT_EQ(RunNormanGift(), GIFT_SEVERAL);
    EXPECT_EQ(StoneCount(ITEM_SCEPTILITE), 1);
    EXPECT_EQ(StoneCount(ITEM_SWAMPERTITE), 1);
    EXPECT_EQ(StoneCount(ITEM_BLAZIKENITE), 0);
    EXPECT(!FlagGet(FLAG_EC_MEGA_GIFT_BLAZIKENITE));

    static const u16 own[] = {SPECIES_GROVYLE, SPECIES_MARSHTOMP};
    SetParty(own, ARRAY_COUNT(own));
    EXPECT_EQ(GetNormanPartnerMegaStone(), ITEM_NONE);

    static const u16 torchic[] = {SPECIES_GROVYLE, SPECIES_COMBUSKEN};
    SetParty(torchic, ARRAY_COUNT(torchic));
    EXPECT_EQ(RunNormanPartnerGifts(), 1);
    EXPECT_EQ(StoneCount(ITEM_BLAZIKENITE), 1);
    EXPECT(FlagGet(FLAG_EC_MEGA_GIFT_BLAZIKENITE));
    EXPECT_EQ(GetNormanPartnerMegaStone(), ITEM_NONE);
    RestoreStarterVars();
}

TEST("Norman's Mega gift: a pair without Mega Stones receives Swampertite")
{
    u16 generation, first, second;
    PARAMETRIZE { generation = 4; first = 0; second = 2; }
    PARAMETRIZE { generation = 5; first = 0; second = 2; }
    PARAMETRIZE { generation = 7; first = 1; second = 2; }
    PARAMETRIZE { generation = 8; first = 2; second = 0; }
    PARAMETRIZE { generation = 9; first = 0; second = 1; }
    SaveStarterVars();
    ResetStarterStones(generation, first, second);
    EXPECT_EQ(RunNormanGift(), GIFT_FALLBACK);
    EXPECT_EQ(StoneCount(ITEM_SWAMPERTITE), 1);
    for (u32 i = 0; i < ARRAY_COUNT(sStarterStones); i++)
    {
        if (sStarterStones[i].item != ITEM_SWAMPERTITE)
            EXPECT_EQ(StoneCount(sStarterStones[i].item), 0);
    }
    // Swampertite is spent; a Mudkip earns nothing more, the other two do.
    static const u16 partners[] = {SPECIES_MUDKIP, SPECIES_TREECKO, SPECIES_TORCHIC};
    SetParty(partners, ARRAY_COUNT(partners));
    EXPECT_EQ(RunNormanPartnerGifts(), 2);
    EXPECT_EQ(StoneCount(ITEM_SWAMPERTITE), 1);
    EXPECT_EQ(StoneCount(ITEM_SCEPTILITE), 1);
    EXPECT_EQ(StoneCount(ITEM_BLAZIKENITE), 1);
    RestoreStarterVars();
}

TEST("Norman's Mega gift: his Feraligite closes Juan's copy; a Leader's earlier copy closes his")
{
    SaveStarterVars();
    // Totodile + Cyndaquil: Norman gives Feraligite and sets Juan's receipt.
    ResetStarterStones(2, 2, 1);
    EXPECT_EQ(RunNormanGift(), GIFT_ONE);
    EXPECT_EQ(StoneCount(ITEM_FERALIGITE), 1);
    EXPECT(FlagGet(FLAG_RECEIVED_TM03));

    // Tepig + Snivy after Brawly handed over Emboarite: nothing is owed.
    ResetStarterStones(5, 1, 0);
    FlagSet(FLAG_RECEIVED_TM08);
    EXPECT(AddBagItem(ITEM_EMBOARITE, 1));
    EXPECT_EQ(RunNormanGift(), GIFT_ALREADY_HELD);
    EXPECT_EQ(StoneCount(ITEM_EMBOARITE), 1);
    EXPECT_EQ(StoneCount(ITEM_SWAMPERTITE), 0);

    // Fennekin + Froakie after Roxanne's Delphoxite: only Greninjite.
    ResetStarterStones(6, 1, 2);
    FlagSet(FLAG_RECEIVED_TM39);
    EXPECT(AddBagItem(ITEM_DELPHOXITE, 1));
    EXPECT_EQ(RunNormanGift(), GIFT_ONE);
    EXPECT_EQ(StoneCount(ITEM_DELPHOXITE), 1);
    EXPECT_EQ(StoneCount(ITEM_GRENINJITE), 1);
    EXPECT(FlagGet(FLAG_ITEM_ROUTE_119_TM62_ACROBATICS));
    RestoreStarterVars();
}

TEST("Norman's Mega gift: a full Mega Stone pocket refuses the Ring and every stone")
{
    SaveStarterVars();
    ResetStarterStones(1, 1, 2);
    struct BagPocket *pocket = &gBagPockets[GetItemPocket(ITEM_CHARIZARDITE_X)];
    for (u32 slot = 0; slot < pocket->capacity; slot++)
        BagPocket_SetSlotItemIdAndCount(pocket, slot, ITEM_ABOMASITE, MAX_BAG_ITEM_CAPACITY);
    // Two slots fit two of the three stones owed: still refused whole.
    BagPocket_SetSlotItemIdAndCount(pocket, 0, ITEM_NONE, 0);
    BagPocket_SetSlotItemIdAndCount(pocket, 1, ITEM_NONE, 0);
    EXPECT(!CanReceiveNormanMegaGift());
    BagPocket_SetSlotItemIdAndCount(pocket, 2, ITEM_NONE, 0);
    EXPECT(CanReceiveNormanMegaGift());
    RestoreStarterVars();
}

// Every region, every ordered pair: after Norman, the world homes, and showing
// Norman all three Hoenn partners, each starter stone exists exactly once.
TEST("Starter Mega Stones: every pair in every region yields each stone exactly once")
{
    static const u16 hoennPartners[] = {SPECIES_SCEPTILE, SPECIES_BLAZIKEN, SPECIES_SWAMPERT};
    SaveStarterVars();
    for (u16 generation = 1; generation <= 9; generation++)
    for (u16 first = 0; first < 3; first++)
    for (u16 second = 0; second < 3; second++)
    {
        if (first == second)
            continue;
        ResetStarterStones(generation, first, second);
        u32 kind = RunNormanGift();
        u32 fromNorman = 0;
        for (u32 i = 0; i < ARRAY_COUNT(sStarterStones); i++)
            fromNorman += StoneCount(sStarterStones[i].item);
        EXPECT_NE(kind, GIFT_ALREADY_HELD);
        EXPECT_GE(fromNorman, 1);
        EXPECT_LE(fromNorman, 3);
        // World homes still open hand over their copy.
        for (u32 i = 0; i < ARRAY_COUNT(sStarterStones); i++)
        {
            if (sStarterStones[i].hoenn || FlagGet(sStarterStones[i].flag))
                continue;
            EXPECT(AddBagItem(sStarterStones[i].item, 1));
            FlagSet(sStarterStones[i].flag);
        }
        u32 hoennFromNorman = StoneCount(ITEM_SCEPTILITE) + StoneCount(ITEM_BLAZIKENITE) + StoneCount(ITEM_SWAMPERTITE);
        SetParty(hoennPartners, ARRAY_COUNT(hoennPartners));
        EXPECT_EQ(RunNormanPartnerGifts(), 3 - hoennFromNorman);
        for (u32 i = 0; i < ARRAY_COUNT(sStarterStones); i++)
        {
            EXPECT_EQ(StoneCount(sStarterStones[i].item), 1);
            EXPECT(FlagGet(sStarterStones[i].flag));
        }
        // Nothing is owed twice.
        EXPECT_EQ(GetNormanStarterMegaStone(), ITEM_NONE);
        EXPECT_EQ(GetNormanPartnerMegaStone(), ITEM_NONE);
        EXPECT(CanReceiveNormanMegaGift());
    }
    RestoreStarterVars();
}
