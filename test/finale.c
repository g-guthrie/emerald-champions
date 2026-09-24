#include "global.h"
#include "data.h"
#include "event_data.h"
#include "item.h"
#include "battle.h"
#include "script.h"
#include "field_specials.h"
#include "constants/field_specials.h"
#include "constants/maps.h"
#include "constants/script_menu.h"
#include "test/test.h"
#include "constants/emerald_champions.h"
#include "constants/opponents.h"
#include "constants/battle_ai.h"

extern u16 GetEmeraldChampionsFinaleStage(void);

TEST("Finale: earned steps, all ship teams, and permanent expedition completion")
{
    static const u16 trainers[] = {
        TRAINER_WALLY_VR_2, TRAINER_COLTON, TRAINER_MICAH, TRAINER_THOMAS,
        TRAINER_LEA_AND_JED, TRAINER_NAOMI, TRAINER_STEVEN, TRAINER_BUFFEL,
    };
    FlagClear(FLAG_SYS_GAME_CLEAR);
    FlagClear(FLAG_RECEIVED_AURORA_TICKET);
    FlagClear(FLAG_BATTLED_DEOXYS);
    FlagClear(FLAG_DEFEATED_DEOXYS);
    FlagClear(FLAG_EC_FINALE_DEOXYS_RESOLVED);
    for (u32 i = 0; i < ARRAY_COUNT(trainers); i++)
        FlagClear(TRAINER_FLAGS_START + trainers[i]);
    EXPECT_EQ(GetEmeraldChampionsFinaleStage(), EC_FINALE_LEAGUE);
    FlagSet(FLAG_SYS_GAME_CLEAR);
    EXPECT_EQ(GetEmeraldChampionsFinaleStage(), EC_FINALE_WALLY);
    FlagSet(TRAINER_FLAGS_START + TRAINER_WALLY_VR_2);
    for (u32 i = 1; i <= 5; i++)
    {
        EXPECT_EQ(GetEmeraldChampionsFinaleStage(), EC_FINALE_VOYAGE);
        FlagSet(TRAINER_FLAGS_START + trainers[i]);
    }
    EXPECT_EQ(GetEmeraldChampionsFinaleStage(), EC_FINALE_STEVEN);
    FlagSet(TRAINER_FLAGS_START + TRAINER_STEVEN);
    EXPECT_EQ(GetEmeraldChampionsFinaleStage(), EC_FINALE_STEVEN);
    FlagSet(FLAG_RECEIVED_AURORA_TICKET);
    EXPECT_EQ(GetEmeraldChampionsFinaleStage(), EC_FINALE_DEOXYS);
    FlagSet(FLAG_EC_FINALE_DEOXYS_RESOLVED);
    EXPECT_EQ(GetEmeraldChampionsFinaleStage(), EC_FINALE_BUFFEL);
    FlagSet(TRAINER_FLAGS_START + TRAINER_BUFFEL);
    EXPECT_EQ(GetEmeraldChampionsFinaleStage(), EC_FINALE_COMPLETE);
    // Replaying the League can clear the legacy legendary-defeat flag.
    FlagClear(FLAG_DEFEATED_DEOXYS);
    EXPECT_EQ(GetEmeraldChampionsFinaleStage(), EC_FINALE_COMPLETE);
    for (u32 i = 0; i < ARRAY_COUNT(trainers); i++)
        FlagClear(TRAINER_FLAGS_START + trainers[i]);
    FlagClear(FLAG_SYS_GAME_CLEAR);
    FlagClear(FLAG_RECEIVED_AURORA_TICKET);
    FlagClear(FLAG_EC_FINALE_DEOXYS_RESOLVED);
}

TEST("Finale: preserve authored set knowledge without move or switch prediction flags")
{
    static const u16 trainers[] = {
        TRAINER_WALLY_VR_2, TRAINER_BUFFEL, TRAINER_STEVEN,
        TRAINER_COLTON, TRAINER_MICAH, TRAINER_THOMAS,
        TRAINER_LEA_AND_JED, TRAINER_NAOMI,
    };
    for (u32 i = 0; i < ARRAY_COUNT(trainers); i++)
    {
        u64 flags = GetTrainerAIFlagsFromId(trainers[i]);
        EXPECT_EQ(flags & (AI_FLAG_OMNISCIENT | AI_FLAG_KNOW_OPPONENT_PARTY),
            AI_FLAG_OMNISCIENT | AI_FLAG_KNOW_OPPONENT_PARTY);
        EXPECT_EQ(flags & (AI_FLAG_PREDICT_MOVE | AI_FLAG_PREDICT_SWITCH | AI_FLAG_PREDICT_INCOMING_MON), 0);
    }
}

extern u32 Test_BuildLilycoveSSTidalSelections(u8 *out);

TEST("Finale: Steven's delivered Aurora Ticket appears in the ferry menu")
{
    ClearBag();
    EXPECT(AddBagItem(ITEM_AURORA_TICKET, 1));
    FlagSet(FLAG_RECEIVED_AURORA_TICKET);
    FlagSet(FLAG_ENABLE_SHIP_BIRTH_ISLAND);
    FlagClear(FLAG_EC_EARNED_AURORA_TICKET);
    gSpecialVar_0x8004 = 0;
    u8 destinations[SSTIDAL_SELECTION_COUNT];
    u32 count = Test_BuildLilycoveSSTidalSelections(destinations);
    bool32 found = FALSE;
    for (u32 i = 0; i < count; i++)
        if (destinations[i] == SSTIDAL_SELECTION_BIRTH_ISLAND)
            found = TRUE;
    EXPECT(found);
    ClearBag();
}

TEST("Finale: the Battle Frontier is on the Lilycove ferry map exactly after the Hall of Fame")
{
    u8 destinations[SSTIDAL_SELECTION_COUNT];
    ClearBag();
    gSpecialVar_0x8004 = 0;
    // Neither Badge 6 nor meeting Scott opens it any more; one rule at both harbors.
    for (u32 bits = 0; bits < 8; bits++)
    {
        bool32 cleared = bits & 1;
        if (cleared) FlagSet(FLAG_SYS_GAME_CLEAR); else FlagClear(FLAG_SYS_GAME_CLEAR);
        if (bits & 2) FlagSet(FLAG_BADGE06_GET); else FlagClear(FLAG_BADGE06_GET);
        if (bits & 4) FlagSet(FLAG_MET_SCOTT_ON_SS_TIDAL); else FlagClear(FLAG_MET_SCOTT_ON_SS_TIDAL);
        u32 count = Test_BuildLilycoveSSTidalSelections(destinations);
        EXPECT_EQ(destinations[0], SSTIDAL_SELECTION_SLATEPORT);
        EXPECT_EQ(count, cleared ? 3 : 2);
        EXPECT_EQ(destinations[1], cleared ? SSTIDAL_SELECTION_BATTLE_FRONTIER : SSTIDAL_SELECTION_EXIT);
    }
    FlagClear(FLAG_SYS_GAME_CLEAR);
    FlagClear(FLAG_BADGE06_GET);
    FlagClear(FLAG_MET_SCOTT_ON_SS_TIDAL);
}

extern void GetLilycoveSSTidalSelection(void);

TEST("Finale: ferry ticket choices preserve unlocks presentation flags and destination mapping")
{
    static const struct { enum Item item; u16 enabled, shown, earned; u8 destination; } tickets[] = {
        {ITEM_EON_TICKET, FLAG_ENABLE_SHIP_SOUTHERN_ISLAND, FLAG_SHOWN_EON_TICKET, FLAG_EC_EARNED_EON_TICKET, SSTIDAL_SELECTION_SOUTHERN_ISLAND},
        {ITEM_MYSTIC_TICKET, FLAG_ENABLE_SHIP_NAVEL_ROCK, FLAG_SHOWN_MYSTIC_TICKET, FLAG_ENABLE_SHIP_NAVEL_ROCK, SSTIDAL_SELECTION_NAVEL_ROCK},
        {ITEM_AURORA_TICKET, FLAG_ENABLE_SHIP_BIRTH_ISLAND, FLAG_SHOWN_AURORA_TICKET, FLAG_EC_EARNED_AURORA_TICKET, SSTIDAL_SELECTION_BIRTH_ISLAND},
        {ITEM_OLD_SEA_MAP, FLAG_ENABLE_SHIP_FARAWAY_ISLAND, FLAG_SHOWN_OLD_SEA_MAP, FLAG_EC_EARNED_OLD_SEA_MAP, SSTIDAL_SELECTION_FARAWAY_ISLAND},
    };
    FlagClear(FLAG_SYS_GAME_CLEAR);
    FlagClear(FLAG_MET_SCOTT_ON_SS_TIDAL);
    for (u32 ticket = 0; ticket < ARRAY_COUNT(tickets); ticket++)
    for (u32 bits = 0; bits < 32; bits++)
    {
        ClearBag();
        for (u32 i = 0; i < ARRAY_COUNT(tickets); i++)
        {
            FlagClear(tickets[i].enabled);
            FlagClear(tickets[i].shown);
            if (tickets[i].earned)
                FlagClear(tickets[i].earned);
        }
        bool32 held = bits & 1, enabled = bits & 2, shown = bits & 4, legacy = bits & 8;
        u32 mode = (bits >> 4) & 1;
        if (held) EXPECT(AddBagItem(tickets[ticket].item, 1));
        if (enabled) FlagSet(tickets[ticket].enabled);
        if (shown) FlagSet(tickets[ticket].shown);
        if (legacy && tickets[ticket].earned) FlagSet(tickets[ticket].earned);
        gSpecialVar_0x8004 = mode;
        u8 destinations[SSTIDAL_SELECTION_COUNT];
        u32 count = Test_BuildLilycoveSSTidalSelections(destinations);
        // The Mystic Ticket's entitlement is its enable flag, so read the flags as set.
        bool32 available = FlagGet(tickets[ticket].enabled)
            && (held || (tickets[ticket].earned && FlagGet(tickets[ticket].earned)));
        bool32 offered = available && (mode == 0 || !shown);
        EXPECT_EQ(count, (mode == 0 ? 2 : 1) + offered);
        EXPECT_EQ(destinations[count - 1], SSTIDAL_SELECTION_EXIT);
        if (mode == 0) EXPECT_EQ(destinations[0], SSTIDAL_SELECTION_SLATEPORT);
        if (offered) EXPECT_EQ(destinations[count - 2], tickets[ticket].destination);
        EXPECT_EQ(FlagGet(tickets[ticket].shown), !!shown || (mode == 1 && available));
        for (u32 i = 0; i < count; i++)
        {
            gSpecialVar_Result = i;
            GetLilycoveSSTidalSelection();
            EXPECT_EQ(gSpecialVar_Result, destinations[i]);
        }
        gSpecialVar_Result = MULTI_B_PRESSED;
        GetLilycoveSSTidalSelection();
        EXPECT_EQ(gSpecialVar_Result, MULTI_B_PRESSED);
    }
    ClearBag();
    FlagSet(FLAG_SYS_GAME_CLEAR);
    for (u32 i = 0; i < ARRAY_COUNT(tickets); i++)
    {
        EXPECT(AddBagItem(tickets[i].item, 1));
        FlagSet(tickets[i].enabled);
    }
    gSpecialVar_0x8004 = 0;
    u8 destinations[SSTIDAL_SELECTION_COUNT];
    EXPECT_EQ(Test_BuildLilycoveSSTidalSelections(destinations), SSTIDAL_SELECTION_COUNT);
    for (u32 i = 0; i < SSTIDAL_SELECTION_COUNT; i++)
        EXPECT_EQ(destinations[i], i);
    ClearBag();
    FlagClear(FLAG_SYS_GAME_CLEAR);
    for (u32 i = 0; i < ARRAY_COUNT(tickets); i++)
    {
        FlagClear(tickets[i].enabled);
        FlagClear(tickets[i].shown);
        if (tickets[i].earned) FlagClear(tickets[i].earned);
    }
}

extern void SetSSTidalFlag(void);
extern void ResetSSTidalFlag(void);

TEST("Finale: cruise steps and route boundaries progress in both directions")
{
    ResetSSTidalFlag();
    VarSet(VAR_CRUISE_STEP_COUNT, 19);
    EXPECT(!CountSSTidalStep(1));
    EXPECT_EQ(VarGet(VAR_CRUISE_STEP_COUNT), 19);
    SetSSTidalFlag();
    EXPECT_EQ(VarGet(VAR_CRUISE_STEP_COUNT), 0);
    for (u32 step = 1; step <= SS_TIDAL_MAX_STEPS; step++)
    {
        EXPECT_EQ(CountSSTidalStep(1), step == SS_TIDAL_MAX_STEPS);
        EXPECT_EQ(VarGet(VAR_CRUISE_STEP_COUNT), step);
    }
    ResetSSTidalFlag();
    EXPECT(!CountSSTidalStep(1));
    EXPECT_EQ(VarGet(VAR_CRUISE_STEP_COUNT), SS_TIDAL_MAX_STEPS);
    static const struct {u16 state, step, map; s16 x;} points[] = {
        {SS_TIDAL_DEPART_SLATEPORT, 0, MAP_ROUTE134, 19},
        {SS_TIDAL_DEPART_SLATEPORT, 59, MAP_ROUTE134, 78},
        {SS_TIDAL_DEPART_SLATEPORT, 60, MAP_ROUTE133, 0},
        {SS_TIDAL_DEPART_SLATEPORT, 139, MAP_ROUTE133, 79},
        {SS_TIDAL_DEPART_SLATEPORT, 140, MAP_ROUTE132, 0},
        {SS_TIDAL_DEPART_SLATEPORT, 204, MAP_ROUTE132, 64},
        {SS_TIDAL_HALFWAY_SLATEPORT, 0, MAP_ROUTE132, 65},
        {SS_TIDAL_HALFWAY_SLATEPORT, 65, MAP_ROUTE132, 0},
        {SS_TIDAL_HALFWAY_SLATEPORT, 66, MAP_ROUTE133, 79},
        {SS_TIDAL_HALFWAY_SLATEPORT, 145, MAP_ROUTE133, 0},
        {SS_TIDAL_HALFWAY_SLATEPORT, 146, MAP_ROUTE134, 78},
        {SS_TIDAL_HALFWAY_SLATEPORT, 204, MAP_ROUTE134, 20},
    };
    for (u32 i = 0; i < ARRAY_COUNT(points); i++)
    {
        VarSet(VAR_SS_TIDAL_STATE, points[i].state);
        VarSet(VAR_CRUISE_STEP_COUNT, points[i].step);
        s8 group = -1, map = -1;
        s16 x = -1, y = -1;
        EXPECT_EQ(GetSSTidalLocation(&group, &map, &x, &y), SS_TIDAL_LOCATION_CURRENTS);
        EXPECT_EQ(group, MAP_GROUP(points[i].map));
        EXPECT_EQ(map, MAP_NUM(points[i].map));
        EXPECT_EQ(x, points[i].x);
        EXPECT_EQ(y, 20);
    }
    VarSet(VAR_SS_TIDAL_STATE, SS_TIDAL_LAND_LILYCOVE);
    VarSet(VAR_CRUISE_STEP_COUNT, 0);
}

TEST("Finale: Birth Island puzzle steps never wrap a long route back into a valid one")
{
    static const u16 maps[] = {MAP_BIRTH_ISLAND_EXTERIOR, MAP_BIRTH_ISLAND_EXTERIOR_FRLG};
    struct WarpData oldLocation = gSaveBlock1Ptr->location;
    u16 oldSteps = VarGet(VAR_DEOXYS_ROCK_STEP_COUNT);
    for (u32 i = 0; i < ARRAY_COUNT(maps); i++)
    {
        gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(maps[i]);
        gSaveBlock1Ptr->location.mapNum = MAP_NUM(maps[i]);
        VarSet(VAR_DEOXYS_ROCK_STEP_COUNT, 0);
        for (u32 step = 1; step <= 205; step++)
        {
            IncrementBirthIslandRockStepCount();
            EXPECT_EQ(VarGet(VAR_DEOXYS_ROCK_STEP_COUNT), min(step, 99));
        }
        VarSet(VAR_DEOXYS_ROCK_STEP_COUNT, 65535);
        IncrementBirthIslandRockStepCount();
        EXPECT_EQ(VarGet(VAR_DEOXYS_ROCK_STEP_COUNT), 99);
    }
    gSaveBlock1Ptr->location.mapGroup = MAP_GROUP(MAP_ROUTE102);
    gSaveBlock1Ptr->location.mapNum = MAP_NUM(MAP_ROUTE102);
    VarSet(VAR_DEOXYS_ROCK_STEP_COUNT, 17);
    IncrementBirthIslandRockStepCount();
    EXPECT_EQ(VarGet(VAR_DEOXYS_ROCK_STEP_COUNT), 17);
    gSaveBlock1Ptr->location = oldLocation;
    VarSet(VAR_DEOXYS_ROCK_STEP_COUNT, oldSteps);
}

extern const u8 BirthIsland_Exterior_EventScript_CheckEncounterOutcome[];
extern const u8 BirthIsland_Exterior_EventScript_EncounterFinished[];
extern const u8 BirthIsland_Exterior_OnTransition[];
extern const u8 BirthIsland_Exterior_EventScript_DeoxysLeaves[];
extern ScrCmdFunc gScriptCmdTable[], gScriptCmdTableEnd[];

TEST("Finale: Deoxys outcomes preserve capture defeat and flee progression")
{
    for (u32 outcome = B_OUTCOME_WON; outcome <= B_OUTCOME_MON_TELEPORTED; outcome++)
    {
        FlagClear(FLAG_EC_FINALE_DEOXYS_RESOLVED);
        FlagClear(FLAG_BATTLED_DEOXYS);
        FlagClear(FLAG_DEFEATED_DEOXYS);
        gBattleOutcome = outcome;
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, BirthIsland_Exterior_EventScript_CheckEncounterOutcome);
        for (u32 step = 0; step < 24 && ctx.scriptPtr != BirthIsland_Exterior_EventScript_EncounterFinished
            && ctx.scriptPtr != BirthIsland_Exterior_EventScript_DeoxysLeaves; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        EXPECT(ctx.scriptPtr == BirthIsland_Exterior_EventScript_EncounterFinished
            || ctx.scriptPtr == BirthIsland_Exterior_EventScript_DeoxysLeaves);
        EXPECT_EQ(FlagGet(FLAG_EC_FINALE_DEOXYS_RESOLVED), outcome == B_OUTCOME_WON || outcome == B_OUTCOME_CAUGHT);
        EXPECT_EQ(FlagGet(FLAG_BATTLED_DEOXYS), outcome == B_OUTCOME_CAUGHT);
        EXPECT_EQ(FlagGet(FLAG_DEFEATED_DEOXYS), outcome == B_OUTCOME_WON);
        FlagSet(FLAG_HIDE_BIRTH_ISLAND_DEOXYS_TRIANGLE);
        FlagSet(FLAG_DEOXYS_ROCK_COMPLETE);
        VarSet(VAR_DEOXYS_ROCK_LEVEL, 10);
        VarSet(VAR_DEOXYS_ROCK_STEP_COUNT, 99);
        RunScriptImmediately(BirthIsland_Exterior_OnTransition);
        EXPECT_EQ(VarGet(VAR_DEOXYS_ROCK_LEVEL), 0);
        EXPECT_EQ(VarGet(VAR_DEOXYS_ROCK_STEP_COUNT), 0);
        EXPECT_EQ(FlagGet(FLAG_HIDE_BIRTH_ISLAND_DEOXYS_TRIANGLE), outcome == B_OUTCOME_WON || outcome == B_OUTCOME_CAUGHT);
        EXPECT_EQ(FlagGet(FLAG_DEOXYS_ROCK_COMPLETE), outcome == B_OUTCOME_WON || outcome == B_OUTCOME_CAUGHT);
    }
    FlagClear(FLAG_EC_FINALE_DEOXYS_RESOLVED);
    FlagClear(FLAG_BATTLED_DEOXYS);
    FlagClear(FLAG_DEFEATED_DEOXYS);
}

extern const u8 LilycoveCity_CoveLilyMotel_2F_EventScript_BuffelGate[];
extern const u8 LilycoveCity_CoveLilyMotel_2F_EventScript_BuffelChallenge[];
extern const u8 LilycoveCity_CoveLilyMotel_2F_EventScript_BuffelDefeated[];
extern const u8 Common_EventScript_FinaleWait[];

TEST("Finale: Buffel requires expedition resolution and offers no battle after victory")
{
    static const u16 priorTrainers[] = {TRAINER_WALLY_VR_2, TRAINER_COLTON, TRAINER_MICAH,
        TRAINER_THOMAS, TRAINER_LEA_AND_JED, TRAINER_NAOMI, TRAINER_STEVEN};
    FlagSet(FLAG_SYS_GAME_CLEAR);
    FlagSet(FLAG_RECEIVED_AURORA_TICKET);
    FlagClear(FLAG_BATTLED_DEOXYS);
    FlagClear(FLAG_DEFEATED_DEOXYS);
    for (u32 i = 0; i < ARRAY_COUNT(priorTrainers); i++)
        FlagSet(TRAINER_FLAGS_START + priorTrainers[i]);
    for (u32 resolved = 0; resolved < 2; resolved++)
    for (u32 defeated = 0; defeated < 2; defeated++)
    {
        if (resolved) FlagSet(FLAG_EC_FINALE_DEOXYS_RESOLVED);
        else FlagClear(FLAG_EC_FINALE_DEOXYS_RESOLVED);
        if (defeated) FlagSet(TRAINER_FLAGS_START + TRAINER_BUFFEL);
        else FlagClear(TRAINER_FLAGS_START + TRAINER_BUFFEL);
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, LilycoveCity_CoveLilyMotel_2F_EventScript_BuffelGate);
        for (u32 step = 0; step < 12 && ctx.scriptPtr != Common_EventScript_FinaleWait
            && ctx.scriptPtr != LilycoveCity_CoveLilyMotel_2F_EventScript_BuffelChallenge
            && ctx.scriptPtr != LilycoveCity_CoveLilyMotel_2F_EventScript_BuffelDefeated; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        const u8 *expected = !resolved ? Common_EventScript_FinaleWait
            : defeated ? LilycoveCity_CoveLilyMotel_2F_EventScript_BuffelDefeated
                       : LilycoveCity_CoveLilyMotel_2F_EventScript_BuffelChallenge;
        EXPECT_EQ(ctx.scriptPtr, expected);
    }
    for (u32 i = 0; i < ARRAY_COUNT(priorTrainers); i++)
        FlagClear(TRAINER_FLAGS_START + priorTrainers[i]);
    FlagClear(TRAINER_FLAGS_START + TRAINER_BUFFEL);
    FlagClear(FLAG_EC_FINALE_DEOXYS_RESOLVED);
    FlagClear(FLAG_RECEIVED_AURORA_TICKET);
    FlagClear(FLAG_SYS_GAME_CLEAR);
}
