#include "global.h"
#include "battle.h"
#include "champions_circuit.h"
#include "event_data.h"
#include "pokemon.h"
#include "string_util.h"
#include "test/test.h"
#include "constants/global.h"
#include "constants/vars.h"

// Contracts the Battle Tower lobby's Champions Circuit desk relies on
// (data/maps/BattleFrontier_BattleTowerLobby/scripts.inc).

static void PrepareSixBulbasaur(u8 level)
{
    ZeroPlayerPartyMons();
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][slot], SPECIES_BULBASAUR, level, 0, OTID_STRUCT_PLAYER_ID, MAX_PER_STAT_IVS);
    CalculatePlayerPartyCount();
}

static bool32 StringVarIs(const u8 *buffer, u32 value)
{
    u8 expected[8];
    ConvertIntToDecimalStringN(expected, value, STR_CONV_MODE_LEFT_ALIGN, 5);
    return StringCompare(buffer, expected) == 0;
}

TEST("Champions Circuit desk: entry enforces the one/one/one party rule")
{
    PrepareSixBulbasaur(20);
    ChampionsCircuitCanEnter();
    EXPECT_EQ(gSpecialVar_Result, CIRCUIT_ENTRY_OK);

    // One of each restricted class is legal.
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][0], SPECIES_MEWTWO, 20, 0, OTID_STRUCT_PLAYER_ID, MAX_PER_STAT_IVS);
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][1], SPECIES_KARTANA, 20, 0, OTID_STRUCT_PLAYER_ID, MAX_PER_STAT_IVS);
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][2], SPECIES_FLUTTER_MANE, 20, 0, OTID_STRUCT_PLAYER_ID, MAX_PER_STAT_IVS);
    EXPECT_EQ(gSpecialVar_Result, CIRCUIT_ENTRY_OK);

    // A second Legendary closes the desk with the party-rule answer.
    CreateMonWithIVs(&gParties[B_TRAINER_PLAYER][3], SPECIES_LUGIA, 20, 0, OTID_STRUCT_PLAYER_ID, MAX_PER_STAT_IVS);
    ChampionsCircuitCanEnter();
    EXPECT_EQ(gSpecialVar_Result, CIRCUIT_ENTRY_PARTY_RULE);

    // Missing members still report the six-Pokemon requirement first.
    ZeroMonData(&gParties[B_TRAINER_PLAYER][PARTY_SIZE - 1]);
    CalculatePlayerPartyCount();
    ChampionsCircuitCanEnter();
    EXPECT_EQ(gSpecialVar_Result, CIRCUIT_ENTRY_NEEDS_SIX);
}

TEST("Champions Circuit desk: a run battles at Lv. 100 and returns the party as brought")
{
    PrepareSixBulbasaur(20);
    gSaveBlock2Ptr->frontier.lvlMode = FRONTIER_LVL_TENT;
    ChampionsCircuitBegin();
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_ACTIVE), TRUE);
    // A Tent's level mode would point Frontier trainer lookups at the Tent tables.
    EXPECT_EQ((u32)gSaveBlock2Ptr->frontier.lvlMode, FRONTIER_LVL_OPEN);
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_LEVEL), CHAMPIONS_CIRCUIT_BASE_LEVEL);

    ChampionsCircuitEnd();
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_ACTIVE), FALSE);
    EXPECT_EQ((u32)gSaveBlock2Ptr->frontier.lvlMode, FRONTIER_LVL_TENT);
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_LEVEL), 20);
    gSaveBlock2Ptr->frontier.lvlMode = FRONTIER_LVL_50;
}

TEST("Champions Circuit desk: a win pays Battle Points and buffers the streak")
{
    u16 pointsBefore;

    PrepareSixBulbasaur(20);
    ChampionsCircuitBegin();
    VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, 9);
    VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 9);
    VarSet(VAR_EC_CIRCUIT_BEST_WINS, 9);
    gSaveBlock2Ptr->frontier.battlePoints = 0;
    pointsBefore = gSaveBlock2Ptr->frontier.battlePoints;

    gBattleOutcome = B_OUTCOME_WON;
    ChampionsCircuitHandleBattleResult();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    // 5 base + 9 for the streak + 20 for the tenth lifetime win.
    EXPECT_EQ(gSaveBlock2Ptr->frontier.battlePoints - pointsBefore, 34);
    EXPECT(StringVarIs(gStringVar1, 34)); // "obtained X Battle Point(s)"
    EXPECT(StringVarIs(gStringVar2, 10)); // "Your streak stands at X"
    EXPECT_EQ(VarGet(VAR_EC_CIRCUIT_BEST_WINS), 10);

    // Retiring keeps the records and reports the streak.
    ChampionsCircuitEnd();
    EXPECT(StringVarIs(gStringVar2, 10));
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS), 0);
    EXPECT_EQ(VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS), 10);
    EXPECT_EQ(VarGet(VAR_EC_CIRCUIT_BEST_WINS), 10);

    VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 0);
    VarSet(VAR_EC_CIRCUIT_BEST_WINS, 0);
    gSaveBlock2Ptr->frontier.battlePoints = 0;
}

TEST("Champions Circuit desk: the record board shows best streak and lifetime wins")
{
    static const u8 sUnrecorded[] = _("--");

    VarSet(VAR_EC_CIRCUIT_BEST_WINS, 0);
    VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 0);
    ChampionsCircuitBufferRecord();
    EXPECT_EQ(StringCompare(gStringVar1, sUnrecorded), 0);
    EXPECT(StringVarIs(gStringVar2, 0));

    VarSet(VAR_EC_CIRCUIT_BEST_WINS, 12);
    VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 40);
    ChampionsCircuitBufferRecord();
    EXPECT(StringVarIs(gStringVar1, 12));
    EXPECT(StringVarIs(gStringVar2, 40));

    VarSet(VAR_EC_CIRCUIT_BEST_WINS, 0);
    VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, 0);
}
