#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "caps.h"
#include "event_data.h"
#include "overworld.h"
#include "constants/layouts.h"
#include "item.h"
#include "pokemon.h"
#include "script.h"
#include "script_pokemon_util.h"
#include "test/test.h"
#include "constants/emerald_champions.h"
#include "constants/map_scripts.h"
#include "constants/opponents.h"

extern const u8 JaggedPass_MapScripts[];
extern const u8 EverGrandeCity_HallOfFame_EventScript_ResetDefeatedEventLegendaries[];
extern const u8 EverGrandeCity_HallOfFame_EventScript_ReadyReceiveSSTicketEvent[];
extern const u8 EverGrandeCity_HallOfFame_EventScript_ReadyCynthiaEvent[];
extern u16 GetEmeraldChampionsFinaleStage(void);
extern ScrCmdFunc gScriptCmdTable[];
extern ScrCmdFunc gScriptCmdTableEnd[];
extern const u8 MeteorFalls_1F_1R_EventScript_CheckMultiResult[];
extern const u8 MeteorFalls_1F_1R_EventScript_MagmaAfterBattle[];
extern const u8 MeteorFalls_1F_1R_EventScript_Lost[];
extern const u8 MossdeepCity_SpaceCenter_2F_EventScript_CheckMultiResult[];
extern const u8 MossdeepCity_SpaceCenter_2F_EventScript_MultiWon[];
extern const u8 MossdeepCity_SpaceCenter_2F_EventScript_Lost[];
extern const u8 EverGrandeCity_HallOfFame_EventScript_ResetEliteFour[];

TEST("Campaign gates: one usable Pokemon still triggers route and scripted doubles")
{
    ZeroPlayerPartyMons();
    CreateMon(&gParties[B_TRAINER_PLAYER][0], SPECIES_MAGIKARP, 80, 0, OTID_STRUCT_PLAYER_ID);
    CalculatePlayerPartyCount();
    EXPECT_EQ(GetMonsStateToDoubles(), PLAYER_HAS_TWO_USABLE_MONS);
    EXPECT_EQ(GetMonsStateToDoubles_2(), PLAYER_HAS_TWO_USABLE_MONS);
    EXPECT(HasEnoughMonsForDoubleBattle2());
    // A larger roster with only one conscious partner must not evade trainers.
    CreateMon(&gParties[B_TRAINER_PLAYER][1], SPECIES_MAGIKARP, 80, 0, OTID_STRUCT_PLAYER_ID);
    u16 hp = 0;
    SetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HP, &hp);
    CalculatePlayerPartyCount();
    EXPECT_EQ(GetMonsStateToDoubles(), PLAYER_HAS_TWO_USABLE_MONS);
    EXPECT_EQ(GetMonsStateToDoubles_2(), PLAYER_HAS_TWO_USABLE_MONS);
    EXPECT(HasEnoughMonsForDoubleBattle2());
    ZeroPlayerPartyMons();
}

TEST("Campaign gates: retiring or losing resets the Elite Four for another run")
{
    FlagSet(FLAG_DEFEATED_ELITE_4_SIDNEY);
    FlagSet(FLAG_DEFEATED_ELITE_4_PHOEBE);
    FlagSet(FLAG_DEFEATED_ELITE_4_GLACIA);
    FlagSet(FLAG_DEFEATED_ELITE_4_DRAKE);
    VarSet(VAR_ELITE_4_STATE, 4);
    RunScriptImmediately(EverGrandeCity_HallOfFame_EventScript_ResetEliteFour);
    EXPECT_EQ(VarGet(VAR_ELITE_4_STATE), 0);
    EXPECT(!FlagGet(FLAG_DEFEATED_ELITE_4_SIDNEY));
    EXPECT(!FlagGet(FLAG_DEFEATED_ELITE_4_PHOEBE));
    EXPECT(!FlagGet(FLAG_DEFEATED_ELITE_4_GLACIA));
    EXPECT(!FlagGet(FLAG_DEFEATED_ELITE_4_DRAKE));
}

TEST("Campaign gates: compiled partner-battle branches advance only on victory")
{
    static const struct { const u8 *gate, *won, *lost; } gates[] = {
        {MeteorFalls_1F_1R_EventScript_CheckMultiResult,
         MeteorFalls_1F_1R_EventScript_MagmaAfterBattle, MeteorFalls_1F_1R_EventScript_Lost},
        {MossdeepCity_SpaceCenter_2F_EventScript_CheckMultiResult,
         MossdeepCity_SpaceCenter_2F_EventScript_MultiWon, MossdeepCity_SpaceCenter_2F_EventScript_Lost},
    };
    for (u32 i = 0; i < ARRAY_COUNT(gates); i++)
    {
        for (u32 outcome = 0; outcome <= B_OUTCOME_FORFEITED; outcome++)
        {
            struct ScriptContext ctx;
            InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
            SetupBytecodeScript(&ctx, gates[i].gate);
            gBattleOutcome = outcome;
            // Execute the real gate opcodes, stopping before the selected
            // cinematic or whiteout, using controlled outcomes and real commands.
            for (u32 step = 0; step < 4 && ctx.scriptPtr != gates[i].won && ctx.scriptPtr != gates[i].lost; step++)
            {
                u8 command = *ctx.scriptPtr++;
                EXPECT(!ctx.cmdTable[command](&ctx));
            }
            EXPECT_EQ(ctx.scriptPtr, outcome == B_OUTCOME_WON ? gates[i].won : gates[i].lost);
        }
    }
}

TEST("Campaign gates: Jagged Pass's assembled resume callback requires the Emblem")
{
    const u8 *entry = JaggedPass_MapScripts;
    while (*entry && *entry != MAP_SCRIPT_ON_RESUME)
        entry += 5;
    EXPECT_EQ(*entry, MAP_SCRIPT_ON_RESUME);
    const u8 *resume = (const u8 *)(entry[1] | (entry[2] << 8) | (entry[3] << 16) | (entry[4] << 24));
    ClearBag();
    VarSet(VAR_JAGGED_PASS_STATE, 0);
    RunScriptImmediately(resume);
    EXPECT_EQ(VarGet(VAR_JAGGED_PASS_STATE), 0);
    EXPECT(AddBagItem(ITEM_MAGMA_EMBLEM, 1));
    RunScriptImmediately(resume);
    EXPECT_EQ(VarGet(VAR_JAGGED_PASS_STATE), 1);
    VarSet(VAR_JAGGED_PASS_STATE, 2);
    RunScriptImmediately(resume);
    EXPECT_EQ(VarGet(VAR_JAGGED_PASS_STATE), 2);
    EXPECT(RemoveBagItem(ITEM_MAGMA_EMBLEM, 1));
    RunScriptImmediately(resume);
    EXPECT_EQ(VarGet(VAR_JAGGED_PASS_STATE), 2);
}

TEST("Campaign gates: every finale prerequisite blocks its later steps")
{
    static const u16 trainers[] = {
        TRAINER_WALLY_VR_2, TRAINER_COLTON, TRAINER_MICAH, TRAINER_THOMAS,
        TRAINER_LEA_AND_JED, TRAINER_NAOMI, TRAINER_STEVEN, TRAINER_BUFFEL,
    };
    // One additional bit covers successful ticket receipt independently of
    // Steven's victory, including a full-Bag retry and older saved progress.
    for (u32 mask = 0; mask < (1u << (ARRAY_COUNT(trainers) + 1)); mask++)
    {
        if (mask & 0x100) FlagSet(FLAG_RECEIVED_AURORA_TICKET);
        else FlagClear(FLAG_RECEIVED_AURORA_TICKET);
        for (u32 i = 0; i < ARRAY_COUNT(trainers); i++)
            if (mask & (1u << i))
                FlagSet(TRAINER_FLAGS_START + trainers[i]);
            else
                FlagClear(TRAINER_FLAGS_START + trainers[i]);
        for (u32 champion = 0; champion < 2; champion++)
        {
            if (champion) FlagSet(FLAG_SYS_GAME_CLEAR);
            else FlagClear(FLAG_SYS_GAME_CLEAR);
            for (u32 resolved = 0; resolved < 4; resolved++)
            {
                FlagClear(FLAG_EC_FINALE_DEOXYS_RESOLVED);
                FlagClear(FLAG_DEFEATED_DEOXYS);
                FlagClear(FLAG_BATTLED_DEOXYS);
                if (resolved == 1) FlagSet(FLAG_EC_FINALE_DEOXYS_RESOLVED);
                if (resolved == 2) FlagSet(FLAG_DEFEATED_DEOXYS);
                if (resolved == 3) FlagSet(FLAG_BATTLED_DEOXYS);
                u16 expected = EC_FINALE_COMPLETE;
                if (!champion) expected = EC_FINALE_LEAGUE;
                else if (!(mask & 1)) expected = EC_FINALE_WALLY;
                else if ((mask & 0x3e) != 0x3e) expected = EC_FINALE_VOYAGE;
                else if (!(mask & 0x40) || !(mask & 0x100)) expected = EC_FINALE_STEVEN;
                else if (!resolved) expected = EC_FINALE_DEOXYS;
                else if (!(mask & 0x80)) expected = EC_FINALE_BUFFEL;
                EXPECT_EQ(GetEmeraldChampionsFinaleStage(), expected);
            }
        }
    }
    for (u32 i = 0; i < ARRAY_COUNT(trainers); i++)
        FlagClear(TRAINER_FLAGS_START + trainers[i]);
    FlagClear(FLAG_SYS_GAME_CLEAR);
    FlagClear(FLAG_BATTLED_DEOXYS);
    FlagClear(FLAG_DEFEATED_DEOXYS);
    FlagClear(FLAG_EC_FINALE_DEOXYS_RESOLVED);
    FlagClear(FLAG_RECEIVED_AURORA_TICKET);
}

TEST("Campaign gates: assembled League replay preserves Deoxys clearance and unlocks ticket paths")
{
    FlagSet(FLAG_DEFEATED_DEOXYS);
    FlagClear(FLAG_EC_FINALE_DEOXYS_RESOLVED);
    RunScriptImmediately(EverGrandeCity_HallOfFame_EventScript_ResetDefeatedEventLegendaries);
    EXPECT(FlagGet(FLAG_EC_FINALE_DEOXYS_RESOLVED));
    EXPECT(!FlagGet(FLAG_DEFEATED_DEOXYS));
    RunScriptImmediately(EverGrandeCity_HallOfFame_EventScript_ResetDefeatedEventLegendaries);
    EXPECT(FlagGet(FLAG_EC_FINALE_DEOXYS_RESOLVED));
    FlagSet(FLAG_HIDE_PLAYERS_HOUSE_DAD);
    RunScriptImmediately(EverGrandeCity_HallOfFame_EventScript_ReadyReceiveSSTicketEvent);
    EXPECT_EQ(VarGet(VAR_LITTLEROOT_HOUSES_STATE_MAY), 3);
    EXPECT_EQ(VarGet(VAR_LITTLEROOT_HOUSES_STATE_BRENDAN), 3);
    EXPECT(!FlagGet(FLAG_HIDE_PLAYERS_HOUSE_DAD));
    FlagSet(FLAG_HIDE_MOSSDEEP_CYNTHIA);
    RunScriptImmediately(EverGrandeCity_HallOfFame_EventScript_ReadyCynthiaEvent);
    EXPECT_EQ(VarGet(VAR_CYNTHIA_STATE), 1);
    EXPECT(!FlagGet(FLAG_HIDE_MOSSDEEP_CYNTHIA));
    FlagClear(FLAG_EC_FINALE_DEOXYS_RESOLVED);
}

extern const u8 Route133_MapScripts[];

TEST("Campaign gates: Route133 vial reward follows the two-charge upgrade and stays completed")
{
    u16 savedCapacity = VarGet(VAR_POKE_VIAL_MAX_CHARGES);
    bool8 savedHidden = FlagGet(FLAG_LENT_NURSE_SURF);
    const u8 *entry = Route133_MapScripts;
    while (*entry && *entry != MAP_SCRIPT_ON_TRANSITION)
        entry += 5;
    EXPECT_EQ(*entry, MAP_SCRIPT_ON_TRANSITION);
    const u8 *transition = (const u8 *)(entry[1] | (entry[2] << 8) | (entry[3] << 16) | (entry[4] << 24));
    VarSet(VAR_POKE_VIAL_MAX_CHARGES, 1);
    FlagClear(FLAG_LENT_NURSE_SURF);
    RunScriptImmediately(transition);
    EXPECT(FlagGet(FLAG_LENT_NURSE_SURF));
    VarSet(VAR_POKE_VIAL_MAX_CHARGES, 2);
    RunScriptImmediately(transition);
    EXPECT(!FlagGet(FLAG_LENT_NURSE_SURF));
    // The completed interaction both raises capacity and hides the actors.
    VarSet(VAR_POKE_VIAL_MAX_CHARGES, 3);
    FlagSet(FLAG_LENT_NURSE_SURF);
    RunScriptImmediately(transition);
    EXPECT(FlagGet(FLAG_LENT_NURSE_SURF));
    EXPECT_EQ(VarGet(VAR_POKE_VIAL_MAX_CHARGES), 3);
    VarSet(VAR_POKE_VIAL_MAX_CHARGES, savedCapacity);
    if (savedHidden)
        FlagSet(FLAG_LENT_NURSE_SURF);
    else
        FlagClear(FLAG_LENT_NURSE_SURF);
}

extern const u8 NewMauville_Inside_EventScript_CheckRotomOutcome[];
extern const u8 NewMauville_Inside_EventScript_DefeatedRotom[];
extern const u8 NewMauville_Inside_EventScript_PlayerCaughtRotom[];
extern const u8 NewMauville_Inside_EventScript_RetryRotom[];

TEST("Campaign gates: New Mauville completes on Rotom capture or knockout and retries every other outcome")
{
    u16 oldState = VarGet(VAR_NEW_MAUVILLE_STATE);
    u8 oldOutcome = gBattleOutcome;
    for (u32 outcome = 0; outcome <= B_OUTCOME_FORFEITED; outcome++)
    {
        VarSet(VAR_NEW_MAUVILLE_STATE, 5);
        gBattleOutcome = outcome;
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, NewMauville_Inside_EventScript_CheckRotomOutcome);
        const u8 *expected = outcome == B_OUTCOME_CAUGHT ? NewMauville_Inside_EventScript_PlayerCaughtRotom
            : outcome == B_OUTCOME_WON ? NewMauville_Inside_EventScript_DefeatedRotom
            : NewMauville_Inside_EventScript_RetryRotom;
        for (u32 step = 0; step < 6 && ctx.scriptPtr != expected; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        EXPECT_EQ(ctx.scriptPtr, expected);
        EXPECT_EQ(VarGet(VAR_NEW_MAUVILLE_STATE), 5);
        // Each branch opens by recording the job: finished (6) after a capture
        // or a knockout (which loses Rotom for good), still open (5) otherwise.
        u8 command = *ctx.scriptPtr++;
        EXPECT(!ctx.cmdTable[command](&ctx));
        EXPECT_EQ(VarGet(VAR_NEW_MAUVILLE_STATE),
            (outcome == B_OUTCOME_CAUGHT || outcome == B_OUTCOME_WON) ? 6 : 5);
    }
    VarSet(VAR_NEW_MAUVILLE_STATE, oldState);
    gBattleOutcome = oldOutcome;
}

extern const u8 Route118_EventScript_CheckGyaradositeState[];
extern const u8 Route118_EventScript_OfferMagikarpChallenge[];
extern const u8 Route118_EventScript_GiveGyaradosite[];
extern const u8 Route118_EventScript_ReceivedGyaradosite[];
extern const u8 Route118_EventScript_CheckGyaradositeReceipt[];
extern const u8 Common_EventScript_ShowBagIsFull[];

TEST("Campaign gates: Magikarp challenge retries its reward without another battle")
{
    bool8 won = HasTrainerBeenFought(TRAINER_MAGIKARP_GUY);
    bool8 received = FlagGet(FLAG_ROUTE118_GYARADOSITE);
    for (u32 state = 0; state < 4; state++)
    {
        if (state & 1) SetTrainerFlag(TRAINER_MAGIKARP_GUY);
        else ClearTrainerFlag(TRAINER_MAGIKARP_GUY);
        if (state & 2) FlagSet(FLAG_ROUTE118_GYARADOSITE);
        else FlagClear(FLAG_ROUTE118_GYARADOSITE);
        const u8 *expected = state & 2 ? Route118_EventScript_ReceivedGyaradosite
            : state & 1 ? Route118_EventScript_GiveGyaradosite
            : Route118_EventScript_OfferMagikarpChallenge;
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, Route118_EventScript_CheckGyaradositeState);
        for (u32 step = 0; step < 6 && ctx.scriptPtr != expected; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        EXPECT_EQ(ctx.scriptPtr, expected);
    }
    for (u32 delivered = 0; delivered < 2; delivered++)
    {
        FlagClear(FLAG_ROUTE118_GYARADOSITE);
        gSpecialVar_Result = delivered;
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, Route118_EventScript_CheckGyaradositeReceipt);
        const u8 *expected = delivered ? Route118_EventScript_ReceivedGyaradosite : Common_EventScript_ShowBagIsFull;
        for (u32 step = 0; step < 4 && ctx.scriptPtr != expected; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        EXPECT_EQ(ctx.scriptPtr, expected);
        EXPECT_EQ(FlagGet(FLAG_ROUTE118_GYARADOSITE), delivered);
    }
    if (won) SetTrainerFlag(TRAINER_MAGIKARP_GUY);
    else ClearTrainerFlag(TRAINER_MAGIKARP_GUY);
    if (received) FlagSet(FLAG_ROUTE118_GYARADOSITE);
    else FlagClear(FLAG_ROUTE118_GYARADOSITE);
}

extern const u8 MossdeepCity_SpaceCenter_1F_EventScript_WomanDialogueGate[];
extern const u8 MossdeepCity_SpaceCenter_1F_EventScript_WomanNormal[];
extern const u8 MossdeepCity_SpaceCenter_1F_EventScript_WomanMagma[];
extern const u8 MossdeepCity_SpaceCenter_1F_EventScript_OldManDialogueGate[];
extern const u8 MossdeepCity_SpaceCenter_1F_EventScript_OldManNormal[];
extern const u8 MossdeepCity_SpaceCenter_1F_EventScript_OldManMagma[];
extern const u8 MossdeepCity_SpaceCenter_2F_EventScript_ScientistDialogueGate[];
extern const u8 MossdeepCity_SpaceCenter_2F_EventScript_ScientistNormal[];
extern const u8 MossdeepCity_SpaceCenter_2F_EventScript_ScientistMagma[];
extern const u8 MossdeepCity_SpaceCenter_2F_EventScript_GentlemanDialogueGate[];
extern const u8 MossdeepCity_SpaceCenter_2F_EventScript_GentlemanNormal[];
extern const u8 MossdeepCity_SpaceCenter_2F_EventScript_GentlemanMagma[];
extern const u8 MossdeepCity_SpaceCenter_2F_EventScript_RichBoyDialogueGate[];
extern const u8 MossdeepCity_SpaceCenter_2F_EventScript_RichBoyNormal[];
extern const u8 MossdeepCity_SpaceCenter_2F_EventScript_RichBoyMagma[];

TEST("Campaign gates: Space Center civilians stop reporting the invasion after victory")
{
    static const struct { const u8 *gate, *normal, *magma; } gates[] = {
        {MossdeepCity_SpaceCenter_1F_EventScript_WomanDialogueGate, MossdeepCity_SpaceCenter_1F_EventScript_WomanNormal, MossdeepCity_SpaceCenter_1F_EventScript_WomanMagma},
        {MossdeepCity_SpaceCenter_1F_EventScript_OldManDialogueGate, MossdeepCity_SpaceCenter_1F_EventScript_OldManNormal, MossdeepCity_SpaceCenter_1F_EventScript_OldManMagma},
        {MossdeepCity_SpaceCenter_2F_EventScript_ScientistDialogueGate, MossdeepCity_SpaceCenter_2F_EventScript_ScientistNormal, MossdeepCity_SpaceCenter_2F_EventScript_ScientistMagma},
        {MossdeepCity_SpaceCenter_2F_EventScript_GentlemanDialogueGate, MossdeepCity_SpaceCenter_2F_EventScript_GentlemanNormal, MossdeepCity_SpaceCenter_2F_EventScript_GentlemanMagma},
        {MossdeepCity_SpaceCenter_2F_EventScript_RichBoyDialogueGate, MossdeepCity_SpaceCenter_2F_EventScript_RichBoyNormal, MossdeepCity_SpaceCenter_2F_EventScript_RichBoyMagma},
    };
    u16 savedState = VarGet(VAR_MOSSDEEP_CITY_STATE);
    bool32 savedClear = FlagGet(FLAG_SYS_GAME_CLEAR);
    for (u32 i = 0; i < ARRAY_COUNT(gates); i++)
    for (u32 cleared = 0; cleared < 2; cleared++)
    for (u32 state = 0; state <= 3; state++)
    {
        VarSet(VAR_MOSSDEEP_CITY_STATE, state);
        if (cleared)
            FlagSet(FLAG_SYS_GAME_CLEAR);
        else
            FlagClear(FLAG_SYS_GAME_CLEAR);
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, gates[i].gate);
        for (u32 step = 0; step < 12 && ctx.scriptPtr != gates[i].normal && ctx.scriptPtr != gates[i].magma; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        EXPECT_EQ(ctx.scriptPtr, !cleared && state == 2 ? gates[i].magma : gates[i].normal);
    }
    VarSet(VAR_MOSSDEEP_CITY_STATE, savedState);
    if (savedClear)
        FlagSet(FLAG_SYS_GAME_CLEAR);
    else
        FlagClear(FLAG_SYS_GAME_CLEAR);
}

extern const u8 MossdeepCity_EventScript_SailorDialogueGate[];
extern const u8 MossdeepCity_EventScript_SailorMagmaGone[];
extern const u8 MossdeepCity_EventScript_SailorMagmaPresent[];

TEST("Campaign gates: Mossdeep Sailor recognizes victory before the Dive handoff")
{
    u16 savedState = VarGet(VAR_MOSSDEEP_CITY_STATE);
    bool32 savedDive = FlagGet(FLAG_RECEIVED_HM08);
    for (u32 dive = 0; dive < 2; dive++)
    for (u32 state = 0; state <= 3; state++)
    {
        VarSet(VAR_MOSSDEEP_CITY_STATE, state);
        if (dive)
            FlagSet(FLAG_RECEIVED_HM08);
        else
            FlagClear(FLAG_RECEIVED_HM08);
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, MossdeepCity_EventScript_SailorDialogueGate);
        for (u32 step = 0; step < 8 && ctx.scriptPtr != MossdeepCity_EventScript_SailorMagmaGone
            && ctx.scriptPtr != MossdeepCity_EventScript_SailorMagmaPresent; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        EXPECT_EQ(ctx.scriptPtr, dive || state >= 3 ? MossdeepCity_EventScript_SailorMagmaGone
                                                 : MossdeepCity_EventScript_SailorMagmaPresent);
    }
    VarSet(VAR_MOSSDEEP_CITY_STATE, savedState);
    if (savedDive)
        FlagSet(FLAG_RECEIVED_HM08);
    else
        FlagClear(FLAG_RECEIVED_HM08);
}

extern const u8 SeafloorCavern_Room9_MapScripts[];
extern const u8 MagmaHideout_4F_MapScripts[];

TEST("Campaign gates: unfinished awakening scenes restore sleepers without reopening completed scenes")
{
    static const struct { const u8 *scripts; u16 hideFlag; } maps[] = {
        {SeafloorCavern_Room9_MapScripts, FLAG_HIDE_SEAFLOOR_CAVERN_ROOM_9_KYOGRE_ASLEEP},
        {MagmaHideout_4F_MapScripts, FLAG_HIDE_MAGMA_HIDEOUT_4F_GROUDON_ASLEEP},
    };
    for (u32 map = 0; map < ARRAY_COUNT(maps); map++)
    for (u32 completed = 0; completed < 2; completed++)
    {
        const u8 *entry = maps[map].scripts;
        while (*entry && *entry != MAP_SCRIPT_ON_TRANSITION)
            entry += 5;
        EXPECT_EQ(*entry, MAP_SCRIPT_ON_TRANSITION);
        const u8 *transition = (const u8 *)(entry[1] | (entry[2] << 8) | (entry[3] << 16) | (entry[4] << 24));
        VarSet(VAR_SEAFLOOR_CAVERN_STATE, completed);
        if (completed)
            FlagSet(FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT);
        else
            FlagClear(FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT);
        FlagSet(maps[map].hideFlag);
        RunScriptImmediately(transition);
        EXPECT_EQ(FlagGet(maps[map].hideFlag), completed);
        EXPECT_EQ(VarGet(VAR_SEAFLOOR_CAVERN_STATE), completed);
        EXPECT_EQ(FlagGet(FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT), completed);
    }
    VarSet(VAR_SEAFLOOR_CAVERN_STATE, 0);
    FlagClear(FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT);
    FlagClear(FLAG_HIDE_SEAFLOOR_CAVERN_ROOM_9_KYOGRE_ASLEEP);
    FlagClear(FLAG_HIDE_MAGMA_HIDEOUT_4F_GROUDON_ASLEEP);
}

extern const u8 SootopolisCity_EventScript_SetLayout[];

TEST("Campaign gates: Sootopolis crisis layout follows story and Rayquaza states")
{
    u16 oldLayout = gSaveBlock1Ptr->mapLayoutId;
    const struct MapLayout *oldMapLayout = gMapHeader.mapLayout;
    u16 oldCity = VarGet(VAR_SOOTOPOLIS_CITY_STATE);
    u16 oldPillar = VarGet(VAR_SKY_PILLAR_STATE);
    for (u32 city = 0; city <= 7; city++)
    for (u32 pillar = 0; pillar <= 3; pillar++)
    {
        SetCurrentMapLayout(LAYOUT_SOOTOPOLIS_CITY);
        VarSet(VAR_SOOTOPOLIS_CITY_STATE, city);
        VarSet(VAR_SKY_PILLAR_STATE, pillar);
        RunScriptImmediately(SootopolisCity_EventScript_SetLayout);
        bool32 crisis = (city >= 1 && city <= 4) || (city == 5 && pillar <= 1);
        EXPECT_EQ(gSaveBlock1Ptr->mapLayoutId, crisis ? LAYOUT_SOOTOPOLIS_CITY_LEGENDS_BATTLE : LAYOUT_SOOTOPOLIS_CITY);
    }
    gSaveBlock1Ptr->mapLayoutId = oldLayout;
    gMapHeader.mapLayout = oldMapLayout;
    VarSet(VAR_SOOTOPOLIS_CITY_STATE, oldCity);
    VarSet(VAR_SKY_PILLAR_STATE, oldPillar);
}

extern const u8 SootopolisCity_EventScript_SetBattleSpectators[];
extern const u8 SootopolisCity_EventScript_PlaceBattleSpectators[];
extern const u8 Common_EventScript_NopReturn[];

TEST("Campaign gates: Sootopolis spectator positions apply only during the crisis")
{
    u16 oldCity = VarGet(VAR_SOOTOPOLIS_CITY_STATE);
    for (u32 city = 0; city <= 7; city++)
    {
        VarSet(VAR_SOOTOPOLIS_CITY_STATE, city);
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, SootopolisCity_EventScript_SetBattleSpectators);
        for (u32 step = 0; step < 8 && ctx.scriptPtr != SootopolisCity_EventScript_PlaceBattleSpectators
            && ctx.scriptPtr != Common_EventScript_NopReturn; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        EXPECT_EQ(ctx.scriptPtr, city >= 1 && city <= 5 ? SootopolisCity_EventScript_PlaceBattleSpectators
                                                       : Common_EventScript_NopReturn);
    }
    VarSet(VAR_SOOTOPOLIS_CITY_STATE, oldCity);
}

extern const u8 EverGrandeCity_PokemonLeague_1F_EventScript_CheckBadges[];
extern const u8 EverGrandeCity_PokemonLeague_1F_EventScript_AdmitTrainer[];
extern const u8 EverGrandeCity_PokemonLeague_1F_EventScript_NotAllBadges[];

TEST("Campaign gates: first League admission requires every Gym Badge")
{
    u32 oldBadges = 0;
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
        oldBadges |= FlagGet(FLAG_BADGE01_GET + badge) << badge;
    for (u32 mask = 0; mask < (1u << NUM_BADGES); mask++)
    {
        for (u32 badge = 0; badge < NUM_BADGES; badge++)
        {
            if (mask & (1u << badge))
                FlagSet(FLAG_BADGE01_GET + badge);
            else
                FlagClear(FLAG_BADGE01_GET + badge);
        }
        struct ScriptContext ctx;
        InitScriptContext(&ctx, gScriptCmdTable, gScriptCmdTableEnd);
        SetupBytecodeScript(&ctx, EverGrandeCity_PokemonLeague_1F_EventScript_CheckBadges);
        for (u32 step = 0; step < 32 && ctx.scriptPtr != EverGrandeCity_PokemonLeague_1F_EventScript_AdmitTrainer
            && ctx.scriptPtr != EverGrandeCity_PokemonLeague_1F_EventScript_NotAllBadges; step++)
        {
            u8 command = *ctx.scriptPtr++;
            EXPECT(!ctx.cmdTable[command](&ctx));
        }
        EXPECT_EQ(ctx.scriptPtr, mask == (1u << NUM_BADGES) - 1
            ? EverGrandeCity_PokemonLeague_1F_EventScript_AdmitTrainer
            : EverGrandeCity_PokemonLeague_1F_EventScript_NotAllBadges);
    }
    for (u32 badge = 0; badge < NUM_BADGES; badge++)
    {
        if (oldBadges & (1u << badge))
            FlagSet(FLAG_BADGE01_GET + badge);
        else
            FlagClear(FLAG_BADGE01_GET + badge);
    }
}

TEST("Level cap milestones count in order: the Magma Hideout before Winona keeps cap 55")
{
    static const u16 badges[] = {FLAG_BADGE01_GET, FLAG_BADGE02_GET, FLAG_BADGE03_GET,
                                 FLAG_BADGE04_GET, FLAG_BADGE05_GET};
    for (u32 i = 0; i < ARRAY_COUNT(badges); i++)
        FlagSet(badges[i]);
    FlagClear(FLAG_BADGE06_GET);
    FlagSet(FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT);
    EXPECT_EQ(GetCurrentLevelCap(), 55);
    FlagSet(FLAG_BADGE06_GET);
    EXPECT_EQ(GetCurrentLevelCap(), 65);
    FlagClear(FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT);
    FlagClear(FLAG_BADGE06_GET);
    for (u32 i = 0; i < ARRAY_COUNT(badges); i++)
        FlagClear(badges[i]);
}
