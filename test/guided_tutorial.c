#include "global.h"
#include "story.h"
#include "battle.h"
#include "dexnav.h"
#include "event_data.h"
#include "guided_tutorial.h"
#include "pokemon.h"
#include "script.h"
#include "overworld.h"
#include "string_util.h"
#include "constants/flags.h"
#include "constants/trainers.h"
#include "constants/vars.h"
#include "test/test.h"

TEST("Rival tutorial: preparation and cleanup preserve every party byte and DexNav registration")
{
    u32 count;
    PARAMETRIZE { count = 1; }
    PARAMETRIZE { count = 3; }
    PARAMETRIZE { count = PARTY_SIZE; }

    struct Pokemon expected[PARTY_SIZE];
    const u16 registration = (ENCOUNTER_TYPE_WATER << 14) | SPECIES_MAGIKARP;
    u16 oldGymState = VarGet(VAR_PETALBURG_GYM_STATE);
    VarSet(VAR_PETALBURG_GYM_STATE, 1);
    FlagClear(DN_FLAG_SEARCHING);
    ZeroPlayerPartyMons();
    for (u32 i = 0; i < count; i++)
    {
        struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][i];
        u32 item = i % 2 ? ITEM_ORAN_BERRY : ITEM_FOCUS_SASH;
        u32 status = i == 0 ? STATUS1_POISON : STATUS1_NONE;
        u32 hp = i == 1 ? 0 : 1;
        u32 pp = 1;
        u32 receipts = 0x155 ^ i;
        CreateMon(mon, i % 2 ? SPECIES_MUDKIP : SPECIES_EEVEE, 8 + i, 0, OTID_STRUCT_PLAYER_ID);
        CalculateMonStats(mon);
        GiveMonInitialMoveset(mon);
        SetMonData(mon, MON_DATA_HELD_ITEM, &item);
        SetMonData(mon, MON_DATA_STATUS, &status);
        SetMonData(mon, MON_DATA_HP, &hp);
        SetMonData(mon, MON_DATA_PP1, &pp);
        // Exercise both portions of the packed permanent tutor receipts.
        SetMonData(mon, MON_DATA_ICONIC_MOVES, &receipts);
    }
    gPartiesCount[B_TRAINER_PLAYER] = count;
    memcpy(expected, gParties[B_TRAINER_PLAYER], sizeof(expected));
    gSaveBlock3Ptr->dexNavChain = 73;
    VarSet(DN_VAR_SPECIES, registration);

    PrepareRivalDexNavTutorial();
    EXPECT(IsRivalDexNavTutorialActive());
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], 1);
    EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_LEVEL), 5);
    EXPECT_EQ(gSaveBlock3Ptr->dexNavChain, 0);
    // Re-entering preparation must not replace the original party backup.
    PrepareRivalDexNavTutorial();
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_ZIGZAGOON, 5, 0, OTID_STRUCT_PLAYER_ID);
    gPartiesCount[B_TRAINER_PLAYER] = 1;
    gSaveBlock3Ptr->dexNavChain = 2;
    VarSet(DN_VAR_SPECIES, SPECIES_ZIGZAGOON);

    FinishRivalDexNavTutorial();
    EXPECT(!IsRivalDexNavTutorialActive());
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], count);
    EXPECT_EQ(memcmp(expected, gParties[B_TRAINER_PLAYER], sizeof(expected)), 0);
    EXPECT_EQ(gSaveBlock3Ptr->dexNavChain, 73);
    EXPECT_EQ(VarGet(DN_VAR_SPECIES), registration);

    // An extra cleanup must not restore a stale snapshot over subsequent play.
    u32 newItem = ITEM_SITRUS_BERRY;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HELD_ITEM, &newItem);
    memcpy(expected, gParties[B_TRAINER_PLAYER], sizeof(expected));
    gSaveBlock3Ptr->dexNavChain = 19;
    VarSet(DN_VAR_SPECIES, SPECIES_WINGULL);
    FinishRivalDexNavTutorial();
    EXPECT_EQ(memcmp(expected, gParties[B_TRAINER_PLAYER], sizeof(expected)), 0);
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], count);
    EXPECT_EQ(gSaveBlock3Ptr->dexNavChain, 19);
    EXPECT_EQ(VarGet(DN_VAR_SPECIES), SPECIES_WINGULL);
    VarSet(VAR_PETALBURG_GYM_STATE, oldGymState);
    ZeroPlayerPartyMons();
}

TEST("Rival tutorial: actor identity follows player gender and returns to Wally after cleanup")
{
    u32 gender, picture;
    const u8 *name, *prompt;
    PARAMETRIZE { gender = MALE; picture = TRAINER_PIC_MAY; name = COMPOUND_STRING("May"); prompt = COMPOUND_STRING("What will\nMay do?"); }
    PARAMETRIZE { gender = FEMALE; picture = TRAINER_PIC_BRENDAN; name = COMPOUND_STRING("Brendan"); prompt = COMPOUND_STRING("What will\nBrendan do?"); }

    u8 oldGender = gSaveBlock2Ptr->playerGender;
    u16 oldGymState = VarGet(VAR_PETALBURG_GYM_STATE);
    VarSet(VAR_PETALBURG_GYM_STATE, 1);
    FlagClear(DN_FLAG_SEARCHING);
    ZeroPlayerPartyMons();
    gSaveBlock2Ptr->playerGender = gender;
    EXPECT_EQ(RivalTutorialTrainerPic(), TRAINER_PIC_WALLY);
    EXPECT_EQ(StringCompare(RivalTutorialTrainerName(), COMPOUND_STRING("Wally")), 0);
    PrepareRivalDexNavTutorial();
    EXPECT_EQ(RivalTutorialTrainerPic(), picture);
    EXPECT_EQ(StringCompare(RivalTutorialTrainerName(), name), 0);
    EXPECT_EQ(StringCompare(RivalTutorialActionPrompt(), prompt), 0);
    FinishRivalDexNavTutorial();
    EXPECT_EQ(RivalTutorialTrainerPic(), TRAINER_PIC_WALLY);
    EXPECT_EQ(StringCompare(RivalTutorialTrainerName(), COMPOUND_STRING("Wally")), 0);
    EXPECT_EQ(StringCompare(RivalTutorialActionPrompt(), COMPOUND_STRING("What will\nWally do?")), 0);
    gSaveBlock2Ptr->playerGender = oldGender;
    VarSet(VAR_PETALBURG_GYM_STATE, oldGymState);
}

TEST("Rival tutorial: lab return requires completion and preserves Norman's later hide state")
{
    u32 gymState, complete, hidden;
    PARAMETRIZE { gymState = 0; complete = FALSE; hidden = TRUE; }
    PARAMETRIZE { gymState = 0; complete = TRUE; hidden = FALSE; }
    PARAMETRIZE { gymState = 1; complete = TRUE; hidden = TRUE; }
    PARAMETRIZE { gymState = 2; complete = TRUE; hidden = TRUE; }

    u16 oldGymState = VarGet(VAR_PETALBURG_GYM_STATE);
    bool32 oldHidden = FlagGet(FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_RIVAL);
    bool32 oldComplete = FlagGet(FLAG_EC_RIVAL_DEXNAV_TUTORIAL_COMPLETE);
    VarSet(VAR_PETALBURG_GYM_STATE, gymState);
    FlagSet(FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_RIVAL);
    StoryStageBefore(STORY_STEP_DEXNAV_LESSON_DONE);
    FlagClear(DN_FLAG_SEARCHING);
    ZeroPlayerPartyMons();
    PrepareRivalDexNavTutorial();
    if (complete)
        StoryStageAtLeast(STORY_STEP_DEXNAV_LESSON_DONE);
    FinishRivalDexNavTutorial();
    EXPECT_EQ(FlagGet(FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_RIVAL), hidden);
    EXPECT_EQ(FlagGet(FLAG_EC_RIVAL_DEXNAV_TUTORIAL_COMPLETE), complete);
    if (oldHidden)
        FlagSet(FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_RIVAL);
    else
        FlagClear(FLAG_HIDE_LITTLEROOT_TOWN_BIRCHS_LAB_RIVAL);
    if (oldComplete)
        StoryStageAtLeast(STORY_STEP_DEXNAV_LESSON_DONE);
    else
        StoryStageBefore(STORY_STEP_DEXNAV_LESSON_DONE);
    VarSet(VAR_PETALBURG_GYM_STATE, oldGymState);
}

extern void Test_DexNavTutorialSearchFailure(void);
TEST("Rival tutorial: failed search immediately restores player before retry dialogue")
{
    struct Pokemon expected[PARTY_SIZE];
    ZeroPlayerPartyMons();
    FlagClear(DN_FLAG_SEARCHING);
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][0], SPECIES_EEVEE, 8, 0, OTID_STRUCT_PLAYER_ID, 31);
    gPartiesCount[B_TRAINER_PLAYER] = 1;
    memcpy(expected, gParties[B_TRAINER_PLAYER], sizeof(expected));
    gSaveBlock3Ptr->dexNavChain = 23;
    VarSet(DN_VAR_SPECIES, SPECIES_WINGULL);
    PrepareRivalDexNavTutorial();
    Test_DexNavTutorialSearchFailure();
    EXPECT(!IsRivalDexNavTutorialActive());
    EXPECT_EQ(memcmp(expected, gParties[B_TRAINER_PLAYER], sizeof(expected)), 0);
    EXPECT_EQ(gPartiesCount[B_TRAINER_PLAYER], 1);
    EXPECT_EQ(gSaveBlock3Ptr->dexNavChain, 23);
    EXPECT_EQ(VarGet(DN_VAR_SPECIES), SPECIES_WINGULL);
    ScriptContext_Stop();
    UnlockPlayerFieldControls();
    ZeroPlayerPartyMons();
}
