#include "global.h"
#include "new_game.h"
#include "main.h"
#include "malloc.h"
#include "event_data.h"
#include "item.h"
#include "money.h"
#include "pokedex.h"
#include "pokemon.h"
#include "starter_choose.h"
#include "birch_pc.h"
#include "strings.h"
#include "test/test.h"

extern u32 Test_DetermineCreditsMons(u16 *output, u32 capacity);

TEST("Regional Dex tables: every entry round-trips and nonmembers return none")
{
    for (u32 i = 1; i < KANTO_DEX_COUNT; i++)
        EXPECT_EQ(NationalToKantoOrder(KantoToNationalOrder(i)), i);
    for (u32 i = 1; i < HOENN_DEX_COUNT; i++)
        EXPECT_EQ(NationalToHoennOrder(HoennToNationalOrder(i)), i);
    EXPECT_EQ(NationalToKantoOrder(NATIONAL_DEX_NONE), KANTO_DEX_NONE);
    EXPECT_EQ(NationalToHoennOrder(NATIONAL_DEX_NONE), HOENN_DEX_NONE);
    EXPECT_EQ(NationalToKantoOrder(NATIONAL_DEX_CHIKORITA), KANTO_DEX_NONE);
    EXPECT_EQ(NationalToHoennOrder(NATIONAL_DEX_CHIKORITA), HOENN_DEX_NONE);
    EXPECT_EQ(NationalToKantoOrder(NATIONAL_DEX_COUNT + 1), KANTO_DEX_NONE);
    EXPECT_EQ(NationalToHoennOrder(NATIONAL_DEX_COUNT + 1), HOENN_DEX_NONE);
}

TEST("Pokedex conversion and Birch ratings reject sentinel indices and clamp stale counts")
{
    EXPECT_EQ(KantoToNationalOrder(KANTO_DEX_NONE), NATIONAL_DEX_NONE);
    EXPECT_EQ(KantoToNationalOrder(KANTO_DEX_BULBASAUR), NATIONAL_DEX_BULBASAUR);
    EXPECT_EQ(KantoToNationalOrder(KANTO_DEX_MEW), NATIONAL_DEX_MEW);
    EXPECT_EQ(KantoToNationalOrder(KANTO_DEX_COUNT), NATIONAL_DEX_NONE);
    EXPECT_EQ(HoennToNationalOrder(HOENN_DEX_COUNT), NATIONAL_DEX_NONE);
    u8 savedCaught[sizeof(gSaveBlock1Ptr->dexCaught)];
    memcpy(savedCaught, gSaveBlock1Ptr->dexCaught, sizeof(savedCaught));
    memset(gSaveBlock1Ptr->dexCaught, 0, sizeof(savedCaught));
    EXPECT(GetPokedexRatingText(0) == gBirchDexRatingText_LessThan10);
    EXPECT(GetPokedexRatingText(0xFFFFFFFF) == gBirchDexRatingText_DexCompleted);
    GetSetPokedexFlag(NATIONAL_DEX_JIRACHI, FLAG_SET_CAUGHT);
    EXPECT(GetPokedexRatingText(0) == gBirchDexRatingText_LessThan10);
    EXPECT(GetPokedexRatingText(1) == gBirchDexRatingText_LessThan10);
    for (u32 i = 1; i < HOENN_DEX_COUNT; i++)
        GetSetPokedexFlag(HoennToNationalOrder(i), FLAG_SET_CAUGHT);
    EXPECT(GetPokedexRatingText(GetHoennPokedexCount(FLAG_GET_CAUGHT)) == gBirchDexRatingText_DexCompleted);
    memcpy(gSaveBlock1Ptr->dexCaught, savedCaught, sizeof(savedCaught));
}

TEST("Pokedex flags: invalid numbers are inert and both valid endpoints are independent")
{
    u8 savedSeen[sizeof(gSaveBlock1Ptr->dexSeen)];
    u8 savedCaught[sizeof(gSaveBlock1Ptr->dexCaught)];
    memcpy(savedSeen, gSaveBlock1Ptr->dexSeen, sizeof(savedSeen));
    memcpy(savedCaught, gSaveBlock1Ptr->dexCaught, sizeof(savedCaught));
    memset(gSaveBlock1Ptr->dexSeen, 0, sizeof(savedSeen));
    memset(gSaveBlock1Ptr->dexCaught, 0, sizeof(savedCaught));
    const u16 invalid[] = {0, NATIONAL_DEX_COUNT + 1, 0xFFFF};
    for (u32 i = 0; i < ARRAY_COUNT(invalid); i++)
    {
        EXPECT_EQ(GetSetPokedexFlag(invalid[i], FLAG_SET_SEEN), FALSE);
        EXPECT_EQ(GetSetPokedexFlag(invalid[i], FLAG_SET_CAUGHT), FALSE);
        EXPECT_EQ(GetSetPokedexFlag(invalid[i], FLAG_GET_SEEN), FALSE);
        EXPECT_EQ(GetSetPokedexFlag(invalid[i], FLAG_GET_CAUGHT), FALSE);
    }
    EXPECT_EQ(GetNationalPokedexCount(FLAG_GET_SEEN), 0);
    EXPECT_EQ(GetNationalPokedexCount(FLAG_GET_CAUGHT), 0);
    GetSetPokedexFlag(1, FLAG_SET_SEEN);
    GetSetPokedexFlag(NATIONAL_DEX_COUNT, FLAG_SET_CAUGHT);
    EXPECT(GetSetPokedexFlag(1, FLAG_GET_SEEN));
    EXPECT(!GetSetPokedexFlag(1, FLAG_GET_CAUGHT));
    EXPECT(!GetSetPokedexFlag(NATIONAL_DEX_COUNT, FLAG_GET_SEEN));
    EXPECT(GetSetPokedexFlag(NATIONAL_DEX_COUNT, FLAG_GET_CAUGHT));
    EXPECT_EQ(GetNationalPokedexCount(FLAG_GET_SEEN), 1);
    EXPECT_EQ(GetNationalPokedexCount(FLAG_GET_CAUGHT), 1);
    memcpy(gSaveBlock1Ptr->dexSeen, savedSeen, sizeof(savedSeen));
    memcpy(gSaveBlock1Ptr->dexCaught, savedCaught, sizeof(savedCaught));
}

TEST("Credits: empty single and populated caught lists produce valid slides ending with the starter")
{
    u32 catches = 0;
    PARAMETRIZE { catches = 0; }
    PARAMETRIZE { catches = 1; }
    PARAMETRIZE { catches = 100; }
    u8 savedCaught[sizeof(gSaveBlock1Ptr->dexCaught)];
    memcpy(savedCaught, gSaveBlock1Ptr->dexCaught, sizeof(savedCaught));
    memset(gSaveBlock1Ptr->dexCaught, 0, sizeof(gSaveBlock1Ptr->dexCaught));
    for (u32 dex = 1; dex <= catches; dex++)
        GetSetPokedexFlag(dex, FLAG_SET_CAUGHT);
    u16 slides[128];
    u16 starter = SpeciesToNationalPokedexNum(GetStarterPokemon(VarGet(VAR_STARTER_MON)));
    u32 count = Test_DetermineCreditsMons(slides, ARRAY_COUNT(slides));
    EXPECT_EQ(count, 71);
    EXPECT_EQ(slides[count - 1], starter);
    for (u32 i = 0; i < count; i++)
    {
        EXPECT(slides[i] > 0 && slides[i] <= NATIONAL_DEX_COUNT);
        EXPECT(slides[i] == starter || (slides[i] >= 1 && slides[i] <= catches));
        if (catches == 0)
            EXPECT_EQ(slides[i], starter);
    }
    memcpy(gSaveBlock1Ptr->dexCaught, savedCaught, sizeof(savedCaught));
}

TEST("New game: clears added campaign progress while preserving chosen identity and options")
{
    gSaveBlock2Ptr->pokedex.harvestedBerries[0] = 200;
    gSaveBlock2Ptr->pokedex.gardenCelebiUnlocked = TRUE;
    gSaveBlock2Ptr->playerName[0] = 0xBB;
    gSaveBlock2Ptr->optionsWindowFrameType = 3;
    FlagSet(FLAG_EC_FINALE_DEOXYS_RESOLVED);
    EXPECT(AddBagItem(ITEM_REGENERATOR, 1));
    NewGameInitData();
    for (u32 i = 0; i < NUM_BERRIES; i++)
        EXPECT_EQ(gSaveBlock2Ptr->pokedex.harvestedBerries[i], 0);
    EXPECT_EQ(gSaveBlock2Ptr->pokedex.gardenCelebiUnlocked, FALSE);
    EXPECT(!FlagGet(FLAG_EC_FINALE_DEOXYS_RESOLVED));
    EXPECT(!CheckBagHasItem(ITEM_REGENERATOR, 1));
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], 0);
    EXPECT_EQ(GetMoney(&gSaveBlock1Ptr->money), 6000);
    EXPECT_EQ(gSaveBlock2Ptr->playerName[0], 0xBB);
    EXPECT_EQ((u32)gSaveBlock2Ptr->optionsWindowFrameType, 3);
}

extern void Test_ReadKeys(u16 keys);

TEST("Input: L equals A repeats mapped actions without altering raw input")
{
    struct Main *saved = Alloc(sizeof(gMain));
    *saved = gMain;
    u32 oldMode = gSaveBlock2Ptr->optionsButtonMode;
    u16 start = gKeyRepeatStartDelay, repeat = gKeyRepeatContinueDelay;
    for (u32 mapped = 0; mapped < 2; mapped++)
    {
        InitKeys();
        gKeyRepeatStartDelay = 2;
        gKeyRepeatContinueDelay = 2;
        gSaveBlock2Ptr->optionsButtonMode = mapped ? OPTIONS_BUTTON_MODE_L_EQUALS_A : OPTIONS_BUTTON_MODE_NORMAL;
        Test_ReadKeys(L_BUTTON);
        EXPECT_EQ(gMain.newKeysRaw, L_BUTTON);
        EXPECT_EQ(gMain.newKeys, L_BUTTON | (mapped ? A_BUTTON : 0));
        EXPECT_EQ(gMain.newAndRepeatedKeys, gMain.newKeys);
        Test_ReadKeys(L_BUTTON);
        EXPECT_EQ(gMain.newKeys, 0);
        EXPECT_EQ(gMain.newAndRepeatedKeys, 0);
        Test_ReadKeys(L_BUTTON);
        EXPECT_EQ(gMain.newAndRepeatedKeys, L_BUTTON | (mapped ? A_BUTTON : 0));
        EXPECT_EQ(gMain.heldKeysRaw, L_BUTTON);
        Test_ReadKeys(0);
        EXPECT_EQ(gMain.heldKeys, 0);
        EXPECT_EQ(gMain.newAndRepeatedKeys, 0);
    }
    gMain = *saved;
    Free(saved);
    gKeyRepeatStartDelay = start;
    gKeyRepeatContinueDelay = repeat;
    gSaveBlock2Ptr->optionsButtonMode = oldMode;
}
