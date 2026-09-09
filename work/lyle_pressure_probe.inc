// Temporary injured/limited-reserve policy observations, not forced choices.
#line 55000 "work/lyle_pressure_probe.inc"
#include "emerald_champions_battle_sets.h"

AI_DOUBLE_BATTLE_TEST("EC live review Lyle pressure controls: injured partners and scarce reserves")
{
    u32 board;
    PARAMETRIZE { board = 0; }
    PARAMETRIZE { board = 1; }
    PARAMETRIZE { board = 2; }
    PARAMETRIZE { board = 3; }
    PARAMETRIZE { board = 4; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet eevee = {
            .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet timburr = {
            .moves = {MOVE_ICE_PUNCH, MOVE_DRAIN_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet munchlax = {
            .moves = {MOVE_BODY_SLAM, MOVE_CRUNCH, MOVE_EARTHQUAKE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_THICK_FAT,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet pachirisu = {
            .moves = {MOVE_ROUND, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        // Eevee/Timburr/Pachirisu are opening access. Munchlax is current
        // pre-badge Route116 access, not claimed as a first-Woods grass catch.
        LyleLeadPlayer(board < 3 ? SPECIES_EEVEE : SPECIES_MUNCHLAX,
            board < 3 ? &eevee : &munchlax);
        LyleLeadPlayer(board < 3 ? SPECIES_TIMBURR : SPECIES_PACHIRISU,
            board < 3 ? &timburr : &pachirisu);
        AuthoredOpponent(TRAINER_LYLE, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_NYMBLE);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_SEWADDLE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_ANORITH);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_DOTTLER);
        u16 zero = 0;
        if (board < 3) {
            u16 nymbleHp = board == 2 ? 1 : 7;
            u16 sewaddleHp = board == 2 ? 35 : 18;
            SetMonData(&party[0], MON_DATA_HP, &nymbleHp);
            SetMonData(&party[1], MON_DATA_HP, &sewaddleHp);
            if (board != 0) {
                SetMonData(&party[2], MON_DATA_HP, &zero);
                SetMonData(&party[3], MON_DATA_HP, &zero);
            }
        } else {
            // Exact reserve records now occupy their native active slots;
            // fainted original leads remain in the party, not deleted.
            SetMonData(&party[0], MON_DATA_HP, &zero);
            SetMonData(&party[1], MON_DATA_HP, &zero);
            u16 dottlerHp = 20;
            SetMonData(&party[3], MON_DATA_HP, &dottlerHp);
            if (board == 3) {
                u16 anorithHp = 12;
                SetMonData(&party[2], MON_DATA_HP, &anorithHp);
            }
            for (u32 slot = 0; slot < 2; slot++) {
                struct Pokemon saved = party[slot];
                party[slot] = party[slot + 2];
                party[slot + 2] = saved;
            }
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
        for (u32 slot = 0; slot < 4; slot++) {
            Test_MgbaPrintf("LYLE_PRESSURE_INPUT board=%d slot=%d species=%d HP=%d/%d Speed=%d",
                board, slot, GetMonData(&party[slot], MON_DATA_SPECIES), GetMonData(&party[slot], MON_DATA_HP),
                GetMonData(&party[slot], MON_DATA_MAX_HP), GetMonData(&party[slot], MON_DATA_SPEED));
        }
        // Only explicit injury/fainting and record permutation differ from
        // the actual team. No move, stat, item, ability or plan is fabricated.
    } WHEN {
        TURN {
            if (board < 3) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                MOVE(playerLeft, MOVE_BODY_SLAM, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_ROUND, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("LYLE_PRESSURE_AFTER board=%d species=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S activeParty=%d/%d ownFaints=%d foeFaints=%d Reflect=%d",
            board, opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter,
            !!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT));
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("LYLE_PRESSURE_PARTY board=%d slot=%d species=%d HP=%d",
                board, slot, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP));
        // Observe native choices without requiring Protect, Reflect, Helping
        // Hand, attack or retreat. Baseline/prototype runs compare the same
        // boards; execution alone does not certify sacrifice quality.
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Lyle pressure opportunity: actual fresh lead against low-threat Lotad pair")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet lotad = {
            .moves = {MOVE_GIGA_DRAIN, MOVE_SCALD, MOVE_RAIN_DANCE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
            .statPoints = {32, 0, 18, 16, 0, 0},
        };
        // Two legal native Route102 Lotad are a controlled player matchup,
        // not a change to the trainer's species distribution or its party.
        LyleLeadPlayer(SPECIES_LOTAD, &lotad);
        LyleLeadPlayer(SPECIES_LOTAD, &lotad);
        AuthoredOpponent(TRAINER_LYLE, 0, FALSE);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            // No opponent move or entry is forced. First Impression is a
            // possible useful fresh-entry attack, not a required assertion.
        }
    } THEN {
        Test_MgbaPrintf("LYLE_PRESSURE_OPPORTUNITY turns=%d nativeTurns=%d species=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S activeParty=%d/%d ownFaints=%d foeFaints=%d",
            turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("LYLE_PRESSURE_OPPORTUNITY_PARTY turns=%d slot=%d species=%d HP=%d",
                turns, slot, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP));
    }
}
