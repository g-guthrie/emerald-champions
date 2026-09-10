#include "ec_review_common.inc"
#line 58000 "work/lydia_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Lydia: physical special and flinch pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 3; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (p == 1 ? (b == 1 ? 5 : 4) : b != 0 && (!p || b == 1) ? 3 : 4); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_LYDIA_1, 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board, turns, phase);
        Test_MgbaPrintf("LYDIA_TYPES left=%d/%d right=%d/%d proteanUsed=%d/%d", opponentLeft->types[0], opponentLeft->types[1], opponentRight->types[0], opponentRight->types[1], opponentLeft->volatiles.usedProteanLibero, opponentRight->volatiles.usedProteanLibero);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Lydia Friend Guard: actual damage and ability-disabled control")
{
    bool32 guard = TRUE;
    PARAMETRIZE { guard = TRUE; }
    PARAMETRIZE { guard = FALSE; }
    GIVEN {
        ReviewPlayer(SPECIES_EEVEE, &sReviewEevee, 30); ReviewPlayer(SPECIES_GURDURR, &sReviewGurdurr, 30);
        ReviewOpponent(TRAINER_LYDIA_1, 2, 1);
        if (!guard) {
            struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
            u32 abilityNum = 0; // Native Natural Cure; no damage reduction.
            SetMonData(&party[1], MON_DATA_ABILITY_NUM, &abilityNum);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        ReviewResult(10 + guard, 1, 1);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_3), guard ? ABILITY_FRIEND_GUARD : ABILITY_NATURAL_CURE);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Lydia disruption: sleep immunity and healthy-bench Taunt")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= (b ? 5 : 3); t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet amoonguss = {
            .moves = {MOVE_SPORE, MOVE_GIGA_DRAIN, MOVE_SLUDGE_BOMB, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .nature = NATURE_BOLD, .ability = ABILITY_REGENERATOR, .evs = {252, 0, 252, 0, 4, 0},
        };
        ReviewPlayer(board ? SPECIES_MIENFOO : SPECIES_AMOONGUSS, board ? &sReviewMienfoo : &amoonguss, 30);
        ReviewPlayer(SPECIES_LOMBRE, &sReviewLombre, 30);
        ReviewOpponent(TRAINER_LYDIA_1, 2, 0);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        for (u32 i = 0; i < 2; i++) {
            struct Pokemon mon = party[i]; party[i] = party[i + 2]; party[i + 2] = mon;
        }
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (!board) MOVE(playerLeft, MOVE_SPORE, target: opponentLeft, hit: TRUE);
            else if (t < 2) MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
            else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_SCALD, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(20 + board, turns, 0);
        Test_MgbaPrintf("LYDIA_STATUS taunt=%d/%d sleep=%d/%d", opponentLeft->volatiles.tauntTimer, opponentRight->volatiles.tauntTimer, opponentLeft->status1 & STATUS1_SLEEP, opponentRight->status1 & STATUS1_SLEEP);
        if (board && turns == 2) {
            EXPECT_EQ(opponentRight->species, SPECIES_HAPPINY);
            EXPECT(opponentRight->volatiles.tauntTimer);
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_SEISMIC_TOSS);
            EXPECT_EQ(opponentRight->pp[3], 19);
        }
        if (!board && turns == 2) {
            EXPECT_EQ(opponentLeft->species, SPECIES_MAGBY);
            EXPECT_EQ(GetBattlerAbility(B_BATTLER_1), ABILITY_VITAL_SPIRIT);
            EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_FIRE_PUNCH);
            EXPECT_EQ(playerLeft->pp[0], 13);
            EXPECT(!(opponentLeft->status1 & STATUS1_SLEEP));
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Lydia timing: healthy six-player bench")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_LYDIA_1, 2, 0);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, 0); ReviewTiming(); }
}
