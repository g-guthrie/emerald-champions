#include "global.h"
#include "center_guide.h"
#include "event_data.h"
#include "item.h"
#include "legendary_signs.h"
#include "overworld.h"
#include "quest_states.h"
#include "script.h"
#include "string_util.h"
#include "test/test.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/map_scripts.h"
#include "constants/quest_states.h"
#include "constants/region_map_sections.h"
#include "constants/vars.h"

// The Center local guide and the quest scripts read the same saved state
// through include/quest_states.h. These tests walk every state of each quest
// and run the quest's own scripts next to the guide, so a change to either
// side that makes them disagree fails here.

extern ScrCmdFunc gScriptCmdTable[];
extern ScrCmdFunc gScriptCmdTableEnd[];
extern const u8 Route111_EventScript_UpdateChanseyVisibility[];
extern const u8 Route111_EventScript_UpdateNurseVisibility[];
extern const u8 Route111_EventScript_VialUpgradeNurseReply[];
extern const u8 Route111_EventScript_CheckHealBallSpace[];
extern const u8 Route111_EventScript_UpgradeVialHideNurse[];
extern const u8 Route111_EventScript_NurseAwaitsBlob[];
extern const u8 Route112_MapScripts[];
extern const u8 JaggedPass_MapScripts[];
extern const u8 AshenWoods_EventScript_UpdateChanseyVisibility[];
extern const u8 Route133_MapScripts[];
extern const u8 Route110_TrickHouseEntrance_EventScript_CheckReadyForNextPuzzle[];

static const u8 *GetTransitionScript(const u8 *mapScripts)
{
    const u8 *entry = mapScripts;

    while (*entry && *entry != MAP_SCRIPT_ON_TRANSITION)
        entry += 5;
    EXPECT_EQ(*entry, MAP_SCRIPT_ON_TRANSITION);
    return (const u8 *)(entry[1] | (entry[2] << 8) | (entry[3] << 16) | (entry[4] << 24));
}

static bool32 GuideSays(const u8 *needle)
{
    u32 length = StringLength(gStringVar4);
    u32 needleLength = StringLength(needle);

    for (u32 i = 0; i + needleLength <= length; i++)
        if (StringCompareN(gStringVar4 + i, needle, needleLength) == 0)
            return TRUE;
    return FALSE;
}

// TRUE if any "Things to Do" page in this city contains the text.
static bool32 GuideTipsMention(u16 city, const u8 *needle)
{
    bool32 found = FALSE;

    gMapHeader.regionMapSectionId = city;
    gSpecialVar_0x8005 = CENTER_GUIDE_TOPIC_TIPS;
    gSpecialVar_0x8004 = 0;
    while (TRUE)
    {
        BufferNextCenterLegendaryLead();
        if (!gSpecialVar_Result)
            break;
        found |= GuideSays(needle);
    }
    gSpecialVar_0x8005 = CENTER_GUIDE_TOPIC_LEGENDS;
    return found;
}

// Steps the Route 111 nurse's reply up to its branch and returns where it went.
static const u8 *RunNurseReplyToBranch(void)
{
    struct ScriptContext ctx;

    InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
    SetupBytecodeScript(&ctx, Route111_EventScript_VialUpgradeNurseReply);
    for (u32 step = 0; step < 8
      && ctx.scriptPtr != Route111_EventScript_CheckHealBallSpace
      && ctx.scriptPtr != Route111_EventScript_UpgradeVialHideNurse
      && ctx.scriptPtr != Route111_EventScript_NurseAwaitsBlob; step++)
    {
        u8 command = *ctx.scriptPtr++;
        EXPECT(!ctx.cmdTable[command](&ctx));
    }
    return ctx.scriptPtr;
}

// The Chansey chase and both Poké Vial upgrades, at every saved state.
TEST("Quest states: Blob's maps, both nurses and the Center guide agree on the Chansey quest")
{
    // Every save the quest can produce: the chase runs with or without a
    // Vial, and the rewards follow a caught Blob. The Route 111 claim and
    // Blob's last position live in two variables; the others only arise from
    // fixtures, which must still get consistent advice.
    u16 savedCapacity = VarGet(VAR_POKE_VIAL_MAX_CHARGES);
    u16 savedBlob = VarGet(VAR_CHANSEY_NURSE_STATE);
    u16 savedAsh = VarGet(VAR_JAGGED_PASS_ASH_WEATHER);
    u32 stagesSeen = 0;

    VarSet(VAR_JAGGED_PASS_ASH_WEATHER, 0);
    for (u16 capacity = POKE_VIAL_CAPACITY_NONE; capacity <= POKE_VIAL_CAPACITY_ROUTE133; capacity++)
    {
        for (u16 blob = CHANSEY_NURSE_NEEDS_HELP; blob <= CHANSEY_NURSE_RETIRED; blob++)
        {
            VarSet(VAR_POKE_VIAL_MAX_CHARGES, capacity);
            VarSet(VAR_CHANSEY_NURSE_STATE, blob);
            u16 stage = GetChanseyQuestStage();
            stagesSeen |= 1u << stage;

            // The quest's own scripts.
            RunScriptImmediately(Route111_EventScript_UpdateChanseyVisibility);
            RunScriptImmediately(GetTransitionScript(Route112_MapScripts));
            RunScriptImmediately(GetTransitionScript(JaggedPass_MapScripts));
            RunScriptImmediately(AshenWoods_EventScript_UpdateChanseyVisibility);
            RunScriptImmediately(Route111_EventScript_UpdateNurseVisibility);
            RunScriptImmediately(GetTransitionScript(Route133_MapScripts));
            bool32 blobOnRoute111 = !FlagGet(FLAG_HIDE_ROUTE111_CHANSEY);
            u32 blobOnChase = !FlagGet(FLAG_HIDE_ROUTE112_CHANSEY)
                            + !FlagGet(FLAG_HIDE_JAGGED_PASS_CHANSEY)
                            + !FlagGet(FLAG_HIDE_ASHEN_WOODS_CHANSEY);
            bool32 route111Nurse = !FlagGet(FLAG_HIDE_ROUTE_111_NURSE);
            bool32 route133Nurse = !FlagGet(FLAG_LENT_NURSE_SURF);
            const u8 *reply = route111Nurse ? RunNurseReplyToBranch() : NULL;

            // The guide's advice.
            bool32 saysLost = GuideTipsMention(MAPSEC_MAUVILLE_CITY, COMPOUND_STRING("has lost her"));
            bool32 saysChase = GuideTipsMention(MAPSEC_MAUVILLE_CITY, COMPOUND_STRING("Ashen Woods"));
            bool32 saysChaseLavaridge = GuideTipsMention(MAPSEC_LAVARIDGE_TOWN, COMPOUND_STRING("Ashen Woods"));
            bool32 saysCaught = GuideTipsMention(MAPSEC_MAUVILLE_CITY, COMPOUND_STRING("You caught Blob"));
            bool32 saysRoute133 = GuideTipsMention(MAPSEC_PACIFIDLOG_TOWN, COMPOUND_STRING("Route 133"));

            EXPECT_EQ(saysLost, stage == CHANSEY_STAGE_NEEDS_HELP);
            EXPECT_EQ(saysChase, stage == CHANSEY_STAGE_CHASE);
            EXPECT_EQ(saysChaseLavaridge, stage == CHANSEY_STAGE_CHASE);
            EXPECT_EQ(saysCaught, stage == CHANSEY_STAGE_CAUGHT);
            EXPECT_EQ(saysRoute133, stage == CHANSEY_STAGE_ROUTE133);
            EXPECT_EQ(IsChanseyVialRewardAvailable(), stage == CHANSEY_STAGE_CAUGHT);
            EXPECT_EQ(IsRoute133VialUpgradeAvailable(), stage == CHANSEY_STAGE_ROUTE133);

            // Each stage's advice points at something the world really offers.
            switch (stage)
            {
            case CHANSEY_STAGE_NEEDS_HELP:
                EXPECT(blobOnRoute111);
                EXPECT(route111Nurse);
                EXPECT_EQ(reply, Route111_EventScript_CheckHealBallSpace);
                break;
            case CHANSEY_STAGE_CHASE:
                // Blob waits on exactly one map of the chase the guide names.
                EXPECT(!blobOnRoute111);
                EXPECT_EQ(blobOnChase, 1);
                EXPECT(route111Nurse);
                EXPECT_EQ(reply, Route111_EventScript_NurseAwaitsBlob);
                break;
            case CHANSEY_STAGE_CAUGHT:
                EXPECT(!blobOnRoute111);
                EXPECT_EQ(blobOnChase, 0);
                EXPECT(route111Nurse);
                EXPECT_EQ(reply, Route111_EventScript_UpgradeVialHideNurse);
                EXPECT(!route133Nurse);
                break;
            case CHANSEY_STAGE_ROUTE133:
                EXPECT(!route111Nurse);
                EXPECT(route133Nurse);
                break;
            case CHANSEY_STAGE_COMPLETE:
                EXPECT(!route111Nurse);
                EXPECT(!route133Nurse);
                break;
            case CHANSEY_STAGE_RETIRED:
                // Blob is gone and no reward is offered; the guide stays quiet.
                EXPECT(!blobOnRoute111);
                EXPECT_EQ(blobOnChase, 0);
                EXPECT_EQ(reply, Route111_EventScript_NurseAwaitsBlob);
                EXPECT(!route133Nurse);
                break;
            default:
                EXPECT(FALSE);
                break;
            }
            // The Route 111 nurse stays until she has paid out.
            EXPECT_EQ(route111Nurse, !IsChanseyVialRewardClaimed());
            EXPECT_EQ(route111Nurse, capacity < POKE_VIAL_CAPACITY_BLOB);
        }
    }
    EXPECT_EQ(stagesSeen, (1u << (CHANSEY_STAGE_RETIRED + 1)) - 1);

    // The chase stage in order: Blob moves forward one map at a time.
    VarSet(VAR_POKE_VIAL_MAX_CHARGES, POKE_VIAL_CAPACITY_BASE);
    VarSet(VAR_CHANSEY_NURSE_STATE, CHANSEY_NURSE_BLOB_ON_ROUTE112);
    RunScriptImmediately(GetTransitionScript(Route112_MapScripts));
    EXPECT(!FlagGet(FLAG_HIDE_ROUTE112_CHANSEY));
    VarSet(VAR_CHANSEY_NURSE_STATE, CHANSEY_NURSE_BLOB_ON_JAGGED_PASS);
    RunScriptImmediately(GetTransitionScript(JaggedPass_MapScripts));
    EXPECT(!FlagGet(FLAG_HIDE_JAGGED_PASS_CHANSEY));
    for (u16 blob = CHANSEY_NURSE_BLOB_IN_ASHEN_WOODS; blob <= CHANSEY_NURSE_BLOB_ASHEN_WOODS_EAST; blob++)
    {
        VarSet(VAR_CHANSEY_NURSE_STATE, blob);
        RunScriptImmediately(AshenWoods_EventScript_UpdateChanseyVisibility);
        EXPECT(!FlagGet(FLAG_HIDE_ASHEN_WOODS_CHANSEY));
    }

    VarSet(VAR_POKE_VIAL_MAX_CHARGES, savedCapacity);
    VarSet(VAR_CHANSEY_NURSE_STATE, savedBlob);
    VarSet(VAR_JAGGED_PASS_ASH_WEATHER, savedAsh);
}

// The Trick House entrance marks the house finished (its own state 4) exactly
// when every puzzle is solved; the guide stops advertising it at that point.
#define TRICK_HOUSE_ENTRANCE_FINISHED 4

TEST("Quest states: the Trick House tip retires exactly when the house says it is finished")
{
    u16 savedLevel = VarGet(VAR_TRICK_HOUSE_LEVEL);
    u16 savedEntrance = VarGet(VAR_TRICK_HOUSE_ENTRANCE_STATE);

    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    FlagSet(FLAG_SYS_GAME_CLEAR);
    for (u16 level = 0; level <= TRICK_HOUSE_ALL_SOLVED; level++)
    {
        VarSet(VAR_TRICK_HOUSE_LEVEL, level);
        RunScriptImmediately(Route110_TrickHouseEntrance_EventScript_CheckReadyForNextPuzzle);
        bool32 finished = VarGet(VAR_TRICK_HOUSE_ENTRANCE_STATE) == TRICK_HOUSE_ENTRANCE_FINISHED;
        EXPECT_EQ(finished, IsTrickHouseComplete());
        EXPECT_EQ(finished, level == TRICK_HOUSE_ALL_SOLVED);
        EXPECT_EQ(GuideTipsMention(MAPSEC_SLATEPORT_CITY, COMPOUND_STRING("Trick Master")), !finished);
        EXPECT_EQ(GuideTipsMention(MAPSEC_MAUVILLE_CITY, COMPOUND_STRING("Trick Master")), !finished);
    }
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
    FlagClear(FLAG_SYS_GAME_CLEAR);
    VarSet(VAR_TRICK_HOUSE_LEVEL, savedLevel);
    VarSet(VAR_TRICK_HOUSE_ENTRANCE_STATE, savedEntrance);
}

TEST("Quest states: the Odd Keystone tip lasts until the Keystone is spent")
{
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        FlagSet(FLAG_BADGE01_GET + badge);
    // Still in the ruins, carried, or stored in the PC: the tip stays.
    FlagClear(FLAG_SANDSTREWN_RUINS_ODD_KEYSTONE);
    EXPECT(!IsOddKeystoneSpent());
    EXPECT(GuideTipsMention(MAPSEC_SLATEPORT_CITY, COMPOUND_STRING("Odd Keystone")));
    FlagSet(FLAG_SANDSTREWN_RUINS_ODD_KEYSTONE);
    AddBagItem(ITEM_ODD_KEYSTONE, 1);
    EXPECT(!IsOddKeystoneSpent());
    EXPECT(GuideTipsMention(MAPSEC_SLATEPORT_CITY, COMPOUND_STRING("Odd Keystone")));
    RemoveBagItem(ITEM_ODD_KEYSTONE, 1);
    AddPCItem(ITEM_ODD_KEYSTONE, 1);
    EXPECT(!IsOddKeystoneSpent());
    EXPECT(GuideTipsMention(MAPSEC_SLATEPORT_CITY, COMPOUND_STRING("Odd Keystone")));
    for (u32 slot = 0; slot < PC_ITEMS_COUNT; slot++)
        if (gSaveBlock1Ptr->pcItems[slot].itemId == ITEM_ODD_KEYSTONE)
            RemovePCItem(slot, 1);
    // Picked up and gone from both: the Abandoned Ship took it.
    EXPECT(IsOddKeystoneSpent());
    EXPECT(!GuideTipsMention(MAPSEC_SLATEPORT_CITY, COMPOUND_STRING("Odd Keystone")));
    FlagClear(FLAG_SANDSTREWN_RUINS_ODD_KEYSTONE);
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        FlagClear(FLAG_BADGE01_GET + badge);
}

TEST("Quest states: Where to Go sends the player to Sky Pillar until Rayquaza calms Sootopolis")
{
    static const u16 sStoryBeforeSkyPillar[] = {
        FLAG_DEFEATED_RIVAL_ROUTE103, FLAG_ADVENTURE_STARTED, FLAG_BADGE01_GET,
        FLAG_RECOVERED_DEVON_GOODS, FLAG_RETURNED_DEVON_GOODS, FLAG_RECEIVED_POKENAV,
        FLAG_DELIVERED_STEVEN_LETTER, FLAG_DELIVERED_DEVON_GOODS, FLAG_HIDE_SLATEPORT_CITY_BRAWLY,
        FLAG_BADGE02_GET, FLAG_BADGE03_GET, FLAG_MET_ARCHIE_METEOR_FALLS,
        FLAG_DEFEATED_EVIL_TEAM_MT_CHIMNEY, FLAG_BADGE04_GET, FLAG_BADGE05_GET,
        FLAG_HIDE_ROUTE_119_TEAM_AQUA, FLAG_RECEIVED_DEVON_SCOPE, FLAG_KECLEON_FLED_FORTREE,
        FLAG_BADGE06_GET, FLAG_RECEIVED_RED_OR_BLUE_ORB, FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT,
        FLAG_MET_TEAM_AQUA_HARBOR, FLAG_TEAM_AQUA_ESCAPED_IN_SUBMARINE, FLAG_BADGE07_GET,
        FLAG_DEFEATED_MAGMA_SPACE_CENTER, FLAG_RECEIVED_HM08, FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN,
        FLAG_WALLACE_GOES_TO_SKY_PILLAR,
    };
    u16 savedCity = VarGet(VAR_SOOTOPOLIS_CITY_STATE);

    for (u32 i = 0; i < ARRAY_COUNT(sStoryBeforeSkyPillar); i++)
        FlagSet(sStoryBeforeSkyPillar[i]);
    FlagClear(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE);
    FlagClear(FLAG_SYS_GAME_CLEAR);
    gMapHeader.regionMapSectionId = MAPSEC_SOOTOPOLIS_CITY;
    // Wallace has left for Sky Pillar (the story's last flag) through Juan's Badge.
    for (u16 city = SOOTOPOLIS_STATE_TO_SKY_PILLAR; city <= SOOTOPOLIS_STATE_GYM_BEATEN; city++)
    {
        VarSet(VAR_SOOTOPOLIS_CITY_STATE, city);
        gSpecialVar_0x8005 = CENTER_GUIDE_TOPIC_STORY;
        gSpecialVar_0x8004 = 0;
        BufferNextCenterLegendaryLead();
        EXPECT_EQ(gSpecialVar_Result, TRUE);
        EXPECT_EQ(GuideSays(COMPOUND_STRING("Sky\nPillar")), !HasRayquazaCalmedSootopolis());
        EXPECT_EQ(HasRayquazaCalmedSootopolis(), city >= SOOTOPOLIS_STATE_RAYQUAZA_AWAKE);
    }
    for (u32 i = 0; i < ARRAY_COUNT(sStoryBeforeSkyPillar); i++)
        FlagClear(sStoryBeforeSkyPillar[i]);
    VarSet(VAR_SOOTOPOLIS_CITY_STATE, savedCity);
    gSpecialVar_0x8005 = CENTER_GUIDE_TOPIC_LEGENDS;
}
