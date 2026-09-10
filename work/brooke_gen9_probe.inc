#include "ec_review_common.inc"
#include "battle_main.h"
#line 60000 "work/brooke_gen9_probe.inc"


AI_DOUBLE_BATTLE_TEST("EC live review Brooke: elemental relay and Mega reserves")
{
    u32 board = 0, turns = 0, phase = 0;
    const u32 horizons[2][5] = {{2, 3, 2, 2, 2}, {2, 3, 3, 2, 3}};
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= horizons[p][b]; t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet dugtrio = {
            .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_ARENA_TRAP, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : board == 3 ? SPECIES_DUGTRIO : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : board == 3 ? &dugtrio : &sReviewMienfoo, 40);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : board == 3 ? SPECIES_XATU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : board == 3 ? &sReviewXatu : &sReviewLombre, 40);
        ReviewOpponent(TRAINER_BROOKE_1, 3, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 3) {
                MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_AIR_SLASH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, board == 4 ? MOVE_TAUNT : MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, board == 4 ? MOVE_GIGA_DRAIN : MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board, turns, phase);
        if (phase == 0 && board == 3) EXPECT_EQ(opponentRight->pp[0], 9);
        if (phase == 1 && board == 4 && turns == 3) {
            EXPECT(opponentRight->hp > 0);
            EXPECT_EQ(opponentRight->species, SPECIES_BLASTOISE_MEGA);
            EXPECT_EQ(opponentRight->pp[3], 4);
        }
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Brooke timing: six-player bench with Mega reserve")
{
    u32 phase = 0;
    for (u32 p = 0; p < 2; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 40);
        ReviewOpponent(TRAINER_BROOKE_1, 3, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        ReviewResult(99, 1, phase); ReviewTiming();
        Test_MgbaPrintf("BROOKE_FORM species=%d/%d HP=%d SpA=%d Speed=%d ability=%d",
            opponentLeft->species, opponentRight->species, opponentRight->maxHP,
            opponentRight->spAttack, opponentRight->speed, GetBattlerAbility(3));
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Brooke: Snarl survival and priority follow-through")
{
    u32 turns = 1;
    for (u32 t = 1; t <= 2; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet dugtrio = {
            .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_ARENA_TRAP, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        struct EmeraldChampionsBattleSet xatu = sReviewXatu;
        xatu.item = ITEM_FOCUS_SASH;
        ReviewPlayer(SPECIES_DUGTRIO, &dugtrio, 40);
        ReviewPlayer(SPECIES_XATU, &xatu, 40);
        ReviewOpponent(TRAINER_BROOKE_1, 3, 0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(70, turns, 0);
        if (turns == 1) EXPECT_EQ(playerRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
        EXPECT(opponentRight->hp > 0);
        EXPECT_EQ(opponentLeft->pp[2], 14);
        if (turns == 2) EXPECT_EQ(opponentRight->pp[3], 19);
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Brooke: staged berry recovery window")
{
    GIVEN {
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewPlayer(SPECIES_EEVEE, &sReviewEevee, 40);
        ReviewOpponent(TRAINER_BROOKE_1, 3, 1);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        u16 hp = 79;
        SetMonData(&party[0], MON_DATA_HP, &hp);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        ReviewResult(71, 1, 1);
        Test_MgbaPrintf("BROOKE_BERRY used=%d held=%d", GetBattlerPartyState(1)->usedHeldItem, opponentLeft->item);
        EXPECT_EQ(opponentLeft->hp, 116);
        EXPECT_EQ(GetBattlerPartyState(1)->usedHeldItem, ITEM_SITRUS_BERRY);
    }
}
