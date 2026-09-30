#include "global.h"
#include "test/test.h"
#include "script.h"
#include "pokemon.h"
#include "event_data.h"
#include "item.h"
#include "random.h"
#include "pokemon_storage_system.h"
#include "script_pokemon_util.h"
#include "caps.h"
#include "constants/flags.h"
#include "constants/vars.h"
#include "constants/moves.h"

extern void SetSpeciesAndEggMove(void);
extern ScrCmdFunc gScriptCmdTable[];
extern ScrCmdFunc gScriptCmdTableEnd[];
extern const u8 LittlerootTown_ProfessorBirchsLab_EventScript_GiveDexReward[];
extern const u8 LittlerootTown_ProfessorBirchsLab_EventScript_DexRewardReceived[];
extern const u8 LittlerootTown_ProfessorBirchsLab_EventScript_DexRewardBagFull[];
extern const u8 LittlerootTown_ProfessorBirchsLab_EventScript_DexRewardDone[];

static void ClearEggProgress(void)
{
    ClearBag();
    for (u32 i = 0; i < 8; i++)
        FlagClear(FLAG_BADGE01_GET + i);
    FlagClear(FLAG_RECEIVED_HM_CUT);
    FlagClear(FLAG_RECEIVED_HM_ROCK_SMASH);
    FlagClear(FLAG_RECEIVED_HM_SURF);
    FlagClear(FLAG_RECEIVED_HM_WATERFALL);
    FlagClear(FLAG_MET_ARCHIE_METEOR_FALLS);
}

static bool32 EggPoolContains(enum Species wanted)
{
    bool32 found = FALSE;
    for (u32 seed = 0; seed < 256; seed++)
    {
        SeedRng(seed);
        SetSpeciesAndEggMove();
        EXPECT_NE(gSpecialVar_0x8004, SPECIES_FEEBAS);
        EXPECT_NE(gSpecialVar_0x8005, MOVE_NONE);
        found |= gSpecialVar_0x8004 == wanted;
    }
    return found;
}

TEST("Pokemon economy: early repeat eggs retain useful options without bypassing habitats")
{
    ClearEggProgress();
    EXPECT(EggPoolContains(SPECIES_CORPHISH));
    EXPECT(EggPoolContains(SPECIES_MARILL));
    EXPECT(EggPoolContains(SPECIES_GASTLY));
    EXPECT(EggPoolContains(SPECIES_PICHU));
    EXPECT(EggPoolContains(SPECIES_TAILLOW));
    EXPECT(!EggPoolContains(SPECIES_BAGON));
    EXPECT(!EggPoolContains(SPECIES_SHUPPET));
    EXPECT(!EggPoolContains(SPECIES_SNEASEL));
    EXPECT(!EggPoolContains(SPECIES_GOOMY));
    EXPECT(!EggPoolContains(SPECIES_EMOLGA));
    EXPECT(!EggPoolContains(SPECIES_RHYHORN));
    EXPECT(!EggPoolContains(SPECIES_WIMPOD));
    EXPECT(!EggPoolContains(SPECIES_PONYTA));
    EXPECT(!EggPoolContains(SPECIES_SNOVER));
    EXPECT(!EggPoolContains(SPECIES_FERROSEED));
    EXPECT(!EggPoolContains(SPECIES_DRATINI));
}

TEST("Pokemon economy: repeat eggs require field licenses and their badges")
{
    const struct {u16 species, license, badge;} habitats[] =
    {
        {SPECIES_EMOLGA, FLAG_RECEIVED_HM_CUT, FLAG_BADGE01_GET},
        {SPECIES_GOOMY, FLAG_RECEIVED_HM_CUT, FLAG_BADGE01_GET},
        {SPECIES_WIMPOD, FLAG_RECEIVED_HM_ROCK_SMASH, FLAG_BADGE03_GET},
        {SPECIES_SHUPPET, FLAG_RECEIVED_HM_SURF, FLAG_BADGE05_GET},
        {SPECIES_DRATINI, FLAG_RECEIVED_HM_SURF, FLAG_BADGE05_GET},
        {SPECIES_BAGON, FLAG_RECEIVED_HM_WATERFALL, FLAG_BADGE08_GET},
    };
    for (u32 i = 0; i < ARRAY_COUNT(habitats); i++)
    {
        ClearEggProgress();
        FlagSet(habitats[i].license);
        EXPECT(!EggPoolContains(habitats[i].species));
        FlagClear(habitats[i].license);
        FlagSet(habitats[i].badge);
        EXPECT(!EggPoolContains(habitats[i].species));
        FlagSet(habitats[i].license);
        EXPECT(EggPoolContains(habitats[i].species));
    }
    ClearEggProgress();
    FlagSet(FLAG_BADGE03_GET);
    EXPECT(EggPoolContains(SPECIES_PONYTA));
    FlagSet(FLAG_MET_ARCHIE_METEOR_FALLS);
    EXPECT(EggPoolContains(SPECIES_FERROSEED));
    ClearEggProgress();
}

TEST("Pokemon economy: cave and Safari eggs respect the Bicycle including legacy bike ids")
{
    const u16 bikes[] = {ITEM_BICYCLE, ITEM_MACH_BIKE, ITEM_ACRO_BIKE};
    for (u32 i = 0; i < ARRAY_COUNT(bikes); i++)
    {
        ClearEggProgress();
        FlagSet(FLAG_RECEIVED_HM_SURF);
        FlagSet(FLAG_BADGE05_GET);
        EXPECT(!EggPoolContains(SPECIES_RHYHORN));
        EXPECT(AddBagItem(bikes[i], 1));
        EXPECT(EggPoolContains(SPECIES_SNEASEL));
        EXPECT(EggPoolContains(SPECIES_SNOVER));
        EXPECT(EggPoolContains(SPECIES_RHYHORN));
    }
    FlagSet(FLAG_RECEIVED_HM_CUT);
    FlagSet(FLAG_BADGE01_GET);
    FlagSet(FLAG_RECEIVED_HM_ROCK_SMASH);
    FlagSet(FLAG_BADGE03_GET);
    FlagSet(FLAG_RECEIVED_HM_WATERFALL);
    FlagSet(FLAG_BADGE08_GET);
    FlagSet(FLAG_MET_ARCHIE_METEOR_FALLS);
    EXPECT(!EggPoolContains(SPECIES_FEEBAS));
    ClearEggProgress();
}

static const u8 *RunDexRewardDelivery(void)
{
    struct ScriptContext ctx;
    InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
    SetupBytecodeScript(&ctx, LittlerootTown_ProfessorBirchsLab_EventScript_GiveDexReward);
    for (u32 step = 0; step < 16
      && ctx.scriptPtr != LittlerootTown_ProfessorBirchsLab_EventScript_DexRewardReceived
      && ctx.scriptPtr != LittlerootTown_ProfessorBirchsLab_EventScript_DexRewardBagFull
      && ctx.scriptPtr != LittlerootTown_ProfessorBirchsLab_EventScript_DexRewardDone; step++)
    {
        u8 command = *ctx.scriptPtr++;
        EXPECT(!ctx.cmdTable[command](&ctx));
    }
    return ctx.scriptPtr;
}

TEST("Pokemon economy: Birch grants a whole-party completion gift once and preserves full-bag retries")
{
    for (u32 pending = 4; pending <= 5; pending++)
    {
        ClearBag();
        struct BagPocket *caps = &gBagPockets[GetItemPocket(ITEM_GOLD_BOTTLE_CAP)];
        for (u32 slot = 0; slot < caps->capacity; slot++)
            BagPocket_SetSlotItemIdAndCount(caps, slot, ITEM_BOTTLE_CAP, MAX_BAG_ITEM_CAPACITY);
        BagPocket_SetSlotItemIdAndCount(caps, 0, ITEM_GOLD_BOTTLE_CAP, MAX_BAG_ITEM_CAPACITY - 5);
        u32 before = CountTotalItemQuantityInBag(ITEM_GOLD_BOTTLE_CAP);
        VarSet(VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, pending);
        EXPECT_EQ(RunDexRewardDelivery(), LittlerootTown_ProfessorBirchsLab_EventScript_DexRewardBagFull);
        EXPECT_EQ(VarGet(VAR_DEX_UPGRADE_JOHTO_STARTER_STATE), pending);
        EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_GOLD_BOTTLE_CAP), before);
        BagPocket_SetSlotItemIdAndCount(caps, 0, ITEM_GOLD_BOTTLE_CAP, MAX_BAG_ITEM_CAPACITY - 6);
        before = CountTotalItemQuantityInBag(ITEM_GOLD_BOTTLE_CAP);
        EXPECT_EQ(RunDexRewardDelivery(), LittlerootTown_ProfessorBirchsLab_EventScript_DexRewardReceived);
        EXPECT_EQ(VarGet(VAR_DEX_UPGRADE_JOHTO_STARTER_STATE), 6);
        EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_GOLD_BOTTLE_CAP), before + PARTY_SIZE);
        EXPECT_EQ(RunDexRewardDelivery(), LittlerootTown_ProfessorBirchsLab_EventScript_DexRewardDone);
        EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_GOLD_BOTTLE_CAP), before + PARTY_SIZE);
    }
    ClearBag();
    // An old claimed Johto starter remains a claimed completion reward.
    VarSet(VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 6);
    EXPECT_EQ(RunDexRewardDelivery(), LittlerootTown_ProfessorBirchsLab_EventScript_DexRewardDone);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_GOLD_BOTTLE_CAP), 0);
    // Guard retired display scripts against granting before completion.
    VarSet(VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 3);
    EXPECT_EQ(RunDexRewardDelivery(), LittlerootTown_ProfessorBirchsLab_EventScript_DexRewardDone);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_GOLD_BOTTLE_CAP), 0);
    VarSet(VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 0);
}

static u32 CountOwnedCosmog(void)
{
    u32 count = 0;
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        count += GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_SPECIES) == SPECIES_COSMOG;
    for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
        for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
            count += GetBoxMonDataAt(box, slot, MON_DATA_SPECIES) == SPECIES_COSMOG;
    return count;
}

TEST("Pokemon economy: Birch's two Cosmog preflight party restrictions and both storage slots")
{
    for (u32 partyKind = 0; partyKind < 3; partyKind++)
    for (u32 freeSlots = 0; freeSlots <= 2; freeSlots++)
    for (u32 pending = 0; pending < 2; pending++)
    {
        ClearBag();
        ZeroPlayerPartyMons();
        memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
        struct Pokemon resident;
        CreateMon(&resident, SPECIES_EEVEE, 10, 0, OTID_STRUCT_PLAYER_ID);
        for (u32 box = 0; box < TOTAL_BOXES_COUNT; box++)
            for (u32 slot = 0; slot < IN_BOX_COUNT; slot++)
                gPokemonStoragePtr->boxes[box][slot] = resident.box;
        // Exercise the final box as well as the first rather than assuming
        // CopyMonToPC can only use the currently selected box.
        for (u32 slot = 0; slot < freeSlots; slot++)
            memset(&gPokemonStoragePtr->boxes[TOTAL_BOXES_COUNT - 1][IN_BOX_COUNT - 1 - slot], 0, sizeof(struct BoxPokemon));
        if (partyKind == 1)
            CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_SHAYMIN, 10, 0, OTID_STRUCT_PLAYER_ID);
        else if (partyKind == 2)
            for (u32 slot = 0; slot < PARTY_SIZE; slot++)
                gParties[B_TRAINER_PLAYER][slot] = resident;
        u16 state = pending ? 7 : 1;
        VarSet(VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, state);
        u32 originalPartyCount = CalculatePlayerPartyCount();
        GiveBirchCosmogPair(NULL);
        bool32 fits = freeSlots >= (partyKind == 0 ? 1 : 2);
        EXPECT_EQ(gSpecialVar_Result, fits);
        EXPECT_EQ(CountOwnedCosmog(), fits ? 2 : 0);
        EXPECT_EQ(CalculatePlayerPartyCount(), originalPartyCount + (fits && partyKind == 0));
        EXPECT(PlayerPartyWithinRestrictedLimit());
        EXPECT_EQ(VarGet(VAR_DEX_UPGRADE_JOHTO_STARTER_STATE), fits ? 2 : state);
        if (fits)
        {
            for (u32 slot = 0; slot < PARTY_SIZE; slot++)
                if (GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_SPECIES) == SPECIES_COSMOG)
                    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_LEVEL), GetCurrentLevelCap());
            GiveBirchCosmogPair(NULL);
            EXPECT_EQ(gSpecialVar_Result, FALSE);
            EXPECT_EQ(CountOwnedCosmog(), 2);
        }
        else
            EXPECT_EQ(GetBoxMonDataAt(TOTAL_BOXES_COUNT - 1, IN_BOX_COUNT - 1, MON_DATA_SPECIES), freeSlots ? SPECIES_NONE : SPECIES_EEVEE);
    }
    ZeroPlayerPartyMons();
    memset(gPokemonStoragePtr, 0, sizeof(*gPokemonStoragePtr));
    ClearBag();
    // Already-received singular gifts on old saves are never granted again.
    VarSet(VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 2);
    GiveBirchCosmogPair(NULL);
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    EXPECT_EQ(CountOwnedCosmog(), 0);
    VarSet(VAR_DEX_UPGRADE_JOHTO_STARTER_STATE, 0);
}
