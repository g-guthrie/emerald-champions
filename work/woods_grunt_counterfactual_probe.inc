// Temporary forced comparisons and actual-team timing; after Woods helpers.
// Forced attack/Protect commands are not unrestricted AI-choice evidence.
#line 13000 "work/woods_grunt_counterfactual_probe.inc"
#include "main.h"
#include "battle_ai_switch.h"

static void WoodsCounterfactualGuards(void)
{
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_CARVANHA);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_WAILMER);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_GRIMER);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_POOCHYENA);
}

DOUBLE_BATTLE_TEST("EC live review Woods grunt forced lead alternatives: preservation Scald Wind Spout")
{
    u32 mode;
    PARAMETRIZE { mode = 0; }
    PARAMETRIZE { mode = 1; }
    PARAMETRIZE { mode = 2; }
    PARAMETRIZE { mode = 3; }
    GIVEN {
        WoodsLeadPlayer(SPECIES_EEVEE, &sWoodsEevee);
        WoodsLeadPlayer(SPECIES_TIMBURR, &sWoodsTimburr);
        AuthoredOpponent(TRAINER_GRUNT_PETALBURG_WOODS, 0, FALSE);
        WoodsCounterfactualGuards();
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        struct Pokemon *player = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
        Test_MgbaPrintf("WOODS_FORCED_LEAD_INPUT mode=%d WailHP=%d Def=%d Speed=%d EeveeAtk=%d Speed=%d TimbSpeed=%d",
            mode, GetMonData(&party[1], MON_DATA_HP), GetMonData(&party[1], MON_DATA_DEF),
            GetMonData(&party[1], MON_DATA_SPEED), GetMonData(&player[0], MON_DATA_ATK),
            GetMonData(&player[0], MON_DATA_SPEED), GetMonData(&player[1], MON_DATA_SPEED));
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentLeft, MOVE_PROTECT);
            if (mode == 0)
                MOVE(opponentRight, MOVE_PROTECT);
            else if (mode == 1)
                MOVE(opponentRight, MOVE_SCALD, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else
                MOVE(opponentRight, mode == 2 ? MOVE_ICY_WIND : MOVE_WATER_SPOUT, hit: TRUE, criticalHit: FALSE);
            // Native Eevee is faster than Wailmer. A commanded Water move
            // need not execute before Double-Edge KOs Wailmer. Complete its
            // genuine replacement prompt, not treating it as a game hang.
            if (mode > 0)
                SEND_OUT(opponentRight, 2);
        }
    } SCENE {
        if (mode > 0)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_EDGE, playerLeft);
            HP_BAR(opponentRight, hp: 0);
        }
    } THEN {
        Test_MgbaPrintf("WOODS_FORCED_LEAD mode=%d nativeTurns=%d active=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S Speed=%d/%d foeSpeed=%d/%d stages=%d/%d foeStages=%d/%d ownFaints=%d foeFaints=%d",
            mode, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->speed, opponentRight->speed, playerLeft->speed, playerRight->speed,
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED],
            playerLeft->statStages[STAT_SPEED], playerRight->statStages[STAT_SPEED],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        Test_MgbaPrintf("WOODS_FORCED_LEAD_PARTY mode=%d Carvanha=%d Wailmer=%d Grimer=%d Poochyena=%d",
            mode, GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][3], MON_DATA_HP));
        Test_MgbaPrintf("WOODS_FORCED_LEAD_WAIL_PP mode=%d Spout=%d Scald=%d Wind=%d Protect=%d",
            mode, GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_PP1),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_PP2),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_PP3),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_PP4));
    }
}

DOUBLE_BATTLE_TEST("EC live review Woods grunt forced reserve alternatives: preservation Snarl Sneak Jab")
{
    u32 mode;
    PARAMETRIZE { mode = 0; }
    PARAMETRIZE { mode = 1; }
    PARAMETRIZE { mode = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet natu = {
            .moves = {MOVE_PSYCHIC, MOVE_HEAT_WAVE, MOVE_REFLECT, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_SYNCHRONIZE,
            .statPoints = {32, 0, 0, 32, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet charmander = {
            .moves = {MOVE_FLAMETHROWER, MOVE_DRAGON_PULSE, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_BLAZE,
            .statPoints = {32, 0, 0, 32, 2, 0},
        };
        WoodsGruntReservePlayer(SPECIES_NATU, &natu);
        WoodsGruntReservePlayer(SPECIES_CHARMANDER, &charmander);
        AuthoredOpponent(TRAINER_GRUNT_PETALBURG_WOODS, 0, FALSE);
        WoodsCounterfactualGuards();
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        u16 zero = 0;
        SetMonData(&party[0], MON_DATA_HP, &zero);
        SetMonData(&party[1], MON_DATA_HP, &zero);
        for (u32 slot = 0; slot < 2; slot++)
        {
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + 2];
            party[slot + 2] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_FLAMETHROWER, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (mode == 0)
            {
                MOVE(opponentLeft, MOVE_PROTECT);
                MOVE(opponentRight, MOVE_PROTECT);
            }
            else
            {
                MOVE(opponentLeft, mode == 1 ? MOVE_SHADOW_SNEAK : MOVE_POISON_JAB,
                    target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(opponentRight, MOVE_SNARL, hit: TRUE, criticalHit: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("WOODS_FORCED_RESERVE mode=%d nativeTurns=%d species=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S foeSpA=%d/%d ownFaints=%d foeFaints=%d status=%d/%d foeStatus=%d/%d",
            mode, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            playerLeft->statStages[STAT_SPATK], playerRight->statStages[STAT_SPATK],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter,
            opponentLeft->status1, opponentRight->status1, playerLeft->status1, playerRight->status1);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Woods grunt timing: six native player members")
{
    GIVEN {
        for (u32 slot = 0; slot < 3; slot++)
        {
            WoodsLeadPlayer(SPECIES_EEVEE, &sWoodsEevee);
            WoodsLeadPlayer(SPECIES_TIMBURR, &sWoodsTimburr);
        }
        AuthoredOpponent(TRAINER_GRUNT_PETALBURG_WOODS, 0, FALSE);
        WoodsCounterfactualGuards();
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        // Do not retain an extra 8 KiB snapshot around the production call:
        // that changes its memory budget. The evaluator restores its native
        // board internally; only decision outputs need invalidating here.
        gAiLogicData->battlerMovesScored = 0;
        u32 start = gMain.vblankCounter1;
        bool32 warm = AI_ComputeDoublesDecisions(B_BATTLER_1);
        u32 warmFrames = gMain.vblankCounter1 - start;
        gAiLogicData->battlerMovesScored = 0;
        start = gMain.vblankCounter1;
        SetAiLogicDataForTurn(gAiLogicData);
        bool32 cold = AI_ComputeDoublesDecisions(B_BATTLER_1);
        u32 coldFrames = gMain.vblankCounter1 - start;
        Test_MgbaPrintf("WOODS_TIMING selected=%d/%d warmFrames=%d cacheRebuildFrames=%d", warm, cold, warmFrames, coldFrames);
    }
}
