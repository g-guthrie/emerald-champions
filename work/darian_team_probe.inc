// Temporary move-comparison observation, not a permanent trainer lock.
#line 43000 "work/darian_team_probe.inc"
#include "emerald_champions_battle_sets.h"

static void DarianTeamPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon));
        Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
              GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Darian team: Discharge opportunity and actual nonimmune partner")
{
    bool32 discharge, corphish, barboachFainted;
    u32 turns;
    PARAMETRIZE { discharge = FALSE; corphish = FALSE; barboachFainted = FALSE; turns = 1; }
    PARAMETRIZE { discharge = TRUE;  corphish = FALSE; barboachFainted = FALSE; turns = 1; }
    PARAMETRIZE { discharge = FALSE; corphish = FALSE; barboachFainted = FALSE; turns = 2; }
    PARAMETRIZE { discharge = TRUE;  corphish = FALSE; barboachFainted = FALSE; turns = 2; }
    PARAMETRIZE { discharge = FALSE; corphish = TRUE;  barboachFainted = FALSE; turns = 1; }
    PARAMETRIZE { discharge = TRUE;  corphish = TRUE;  barboachFainted = FALSE; turns = 1; }
    PARAMETRIZE { discharge = FALSE; corphish = TRUE;  barboachFainted = TRUE;  turns = 1; }
    PARAMETRIZE { discharge = TRUE;  corphish = TRUE;  barboachFainted = TRUE;  turns = 1; }
    PARAMETRIZE { discharge = FALSE; corphish = TRUE;  barboachFainted = TRUE;  turns = 2; }
    PARAMETRIZE { discharge = TRUE;  corphish = TRUE;  barboachFainted = TRUE;  turns = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet wingull = {
            .moves = {MOVE_HURRICANE, MOVE_MUDDY_WATER, MOVE_ICY_WIND, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_HYDRATION,
            .statPoints = {32, 0, 0, 32, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet marill = {
            .moves = {MOVE_LIQUIDATION, MOVE_PLAY_ROUGH, MOVE_AQUA_JET, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_HUGE_POWER,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        DarianTeamPlayer(SPECIES_WINGULL, &wingull);
        DarianTeamPlayer(SPECIES_MARILL, &marill);
        AuthoredOpponent(TRAINER_DARIAN, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_CHINCHOU);
        EXPECT_EQ(GetMonAbility(&party[0]), ABILITY_VOLT_ABSORB);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_BARBOACH);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_CORPHISH);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_CLAMPERL);
        {
            u16 move = discharge ? MOVE_DISCHARGE : MOVE_THUNDERBOLT;
            u8 pp = GetMovePP(move);
            SetMonData(&party[0], MON_DATA_MOVE3, &move);
            SetMonData(&party[0], MON_DATA_PP3, &pp);
        }
        if (barboachFainted) {
            u16 hp = 0;
            SetMonData(&party[1], MON_DATA_HP, &hp);
        }
        if (corphish) {
            struct Pokemon saved = party[1];
            party[1] = party[2];
            party[2] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
        // Both players occur in current opening land tables (Wingull Route103,
        // Marill Route102); moves/abilities use the current preparation arrays.
        // Except the explicit fainted-Barboach control, all four generated
        // opponents remain alive. The two new cases remove the immune pivot,
        // retaining actual live Corphish and the nonimmune Clamperl reserve.
        // Every other native field is retained, including the exact records
        // permuted between slots1/2. Both move variants are explicit so the
        // baseline remains Thunderbolt if the production team adopts Discharge.
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < 2; slot++) {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("DARIAN_TEAM_INPUT discharge=%d corphish=%d faintedBar=%d turns=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                    discharge, corphish, barboachFainted, turns, owner, slot, GetMonData(mon, MON_DATA_SPECIES),
                    GetMonData(mon, MON_DATA_LEVEL), GetMonData(mon, MON_DATA_MAX_HP),
                    GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF),
                    GetMonData(mon, MON_DATA_SPEED), GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++)
            TURN {
                MOVE(playerLeft, MOVE_HURRICANE, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_LIQUIDATION, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                // Unrestricted AI may attack, protect, set up, or switch. A
                // Discharge after switching Corphish to Barboach is legitimate;
                // no blanket must/must-not-Discharge assertion encodes a script.
            }
    } THEN {
        Test_MgbaPrintf("DARIAN_TEAM_AFTER discharge=%d corphish=%d faintedBar=%d turns=%d nativeTurns=%d species=%d/%d HP=%d/%d playerHP=%d/%d playerStatus=%d/%d LAST=%S/%S party=%d/%d",
            discharge, corphish, barboachFainted, turns, gBattleResults.battleTurnCounter,
            opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, playerLeft->status1, playerRight->status1,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3]);
        Test_MgbaPrintf("DARIAN_TEAM_PARTY discharge=%d corphish=%d faintedBar=%d turns=%d ChinchouHP=%d BarboachHP=%d CorphishHP=%d ClamperlHP=%d",
            discharge, corphish, barboachFainted, turns,
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][corphish ? 2 : 1], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][corphish ? 1 : 2], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][3], MON_DATA_HP));
    }
}
