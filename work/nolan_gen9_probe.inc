#include "ec_review_common.inc"

// A swallowed Tatsugiri makes no AI decisions. The runner's implicit PASS
// queue otherwise survives those turns and marks a future, unexecuted choice
// as an extra played turn after Dondozo faints. No explicit expectations exist
// here: player records remain the turn boundary; native AI/actions are unchanged.
static void NolanClearCommanderPasses(void)
{
    for (u32 i = 0; i < MAX_EXPECTED_ACTIONS; i++) {
        struct ExpectedAIAction *action = &gBattleTestRunnerState->data.expectedAiActions[3][i];
        EXPECT(!action->actionSet || action->pass);
        action->actionSet = FALSE;
    }
}

static void NolanOpponent(u32 phase)
{
    ReviewOpponent(TRAINER_NOLAN, 3, phase == 1);
    if (phase == 2) {
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        struct Pokemon dozo = party[0];
        party[0] = party[2]; party[2] = party[3]; party[3] = dozo;
        u16 zero = 0; SetMonData(&party[3], MON_DATA_HP, &zero);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Nolan: Commander pressure and reserve transitions")
{
    u32 board = 0, turns = 0, phase = 0;
    const u8 horizons[3][5] = {{3,3,5,5,4}, {3,4,5,3,5}, {3,4,5,5,4}};
    for (u32 p = 0; p < 3; p++) for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= horizons[p][b]; t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : board == 3 ? SPECIES_LOMBRE : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : board == 3 ? &sReviewLombre : &sReviewMienfoo, 40);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 || board == 3 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 || board == 3 ? &sReviewPachirisu : &sReviewLombre, 40);
        NolanOpponent(phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: phase ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE);
            } else if (board == 1 || board == 3) {
                MOVE(playerLeft, board == 1 ? MOVE_PSYCHIC : MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: phase ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, board == 4 ? MOVE_TAUNT : MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_GIGA_DRAIN, target: phase ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE);
            }
        }
        if (!phase) NolanClearCommanderPasses();
    } THEN {
        ReviewResult(board, turns, phase);
        if (!phase && turns == 1) {
            EXPECT_EQ(opponentLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(GetBattlerTotalSpeedStat(1, GetBattlerAbility(1), GetBattlerHoldEffect(1)), 94);
            EXPECT(gBattleMons[3].volatiles.semiInvulnerable == STATE_COMMANDER);
            EXPECT_EQ(opponentRight->pp[0], 10);
        }
        if (phase == 2 && board == 0 && turns == 3) {
            EXPECT_EQ(opponentLeft->species, SPECIES_PALAFIN_HERO);
            EXPECT(opponentLeft->pp[1] < 10);
            Test_MgbaPrintf("NOLAN_HERO hp=%d atk=%d def=%d spa=%d spd=%d speed=%d", opponentLeft->maxHP,
                opponentLeft->attack, opponentLeft->defense, opponentLeft->spAttack, opponentLeft->spDefense, opponentLeft->speed);
        }
        Test_MgbaPrintf("NOLAN_ENGINE commander=%d hidden=%d speed=%d/%d", gBattleStruct->battlerState[1].commanderSpecies,
            gBattleMons[3].volatiles.semiInvulnerable, GetBattlerTotalSpeedStat(1, GetBattlerAbility(1), GetBattlerHoldEffect(1)),
            GetBattlerTotalSpeedStat(3, GetBattlerAbility(3), GetBattlerHoldEffect(3)));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Nolan timing: six-player bench")
{
    u32 phase = 0;
    for (u32 p = 0; p < 3; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 40);
        NolanOpponent(phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Nolan: Haze counterplay")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 6; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet dusclops = {
            .moves = {MOVE_HAZE, MOVE_NIGHT_SHADE, MOVE_PAIN_SPLIT, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .ability = ABILITY_PRESSURE, .nature = NATURE_BOLD, .evs = {252,0,252,0,4,0},
        };
        ReviewPlayer(SPECIES_DUSCLOPS, &dusclops, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        NolanOpponent(0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, t == 0 ? MOVE_HAZE : MOVE_NIGHT_SHADE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
        NolanClearCommanderPasses();
    } THEN {
        ReviewResult(80, turns, FALSE);
        if (turns == 1) {
            EXPECT_EQ(gBattleMons[0].pp[0], 19);
            EXPECT_EQ(opponentLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
            EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        }
        if (turns == 4) ReviewTiming();
    }
}
