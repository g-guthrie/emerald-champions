#include "ec_review_common.inc"
#include "battle_main.h"
#line 60000 "work/wilton_gen9_probe.inc"


AI_DOUBLE_BATTLE_TEST("EC live review Wilton: Trick Room offense and Mega reserves")
{
    u32 board = 0, turns = 0, phase = 0;
    const u32 horizons[2][5] = {{4, 3, 4, 3, 5}, {3, 2, 4, 2, 4}};
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
        ReviewOpponent(TRAINER_WILTON_1, 3, phase);
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
        if (phase == 0 && turns < 5) {
            EXPECT(gFieldStatuses & STATUS_FIELD_TRICK_ROOM);
            if (opponentLeft->species == SPECIES_COFAGRIGUS && opponentRight->species == SPECIES_CLAYDOL)
                EXPECT_EQ(gBattleMons[1].pp[0] + gBattleMons[3].pp[2], 9);
        }
        Test_MgbaPrintf("WILTON_ROOM active=%d timer=%d", !!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM), gFieldTimers.trickRoomTimer);
        Test_MgbaPrintf("WILTON_1_FIELD terrain=%d timer=%d charge=%d/%d speed=%d/%d nativeSpeed=%d/%d",
            gFieldTimers.terrain, gFieldTimers.terrainTimer, gBattleMons[1].volatiles.chargeTimer, gBattleMons[3].volatiles.chargeTimer,
            GetBattlerTotalSpeedStat(1, GetBattlerAbility(1), GetBattlerHoldEffect(1)),
            GetBattlerTotalSpeedStat(3, GetBattlerAbility(3), GetBattlerHoldEffect(3)), gBattleMons[1].speed, gBattleMons[3].speed);
        if (opponentRight->species == SPECIES_STEELIX_MEGA)
            Test_MgbaPrintf("WILTON_1_MEGA HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d ability=%d",
                opponentRight->maxHP, opponentRight->attack, opponentRight->defense, opponentRight->spAttack,
                opponentRight->spDefense, opponentRight->speed, GetBattlerAbility(3));
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Wilton: staged backup setter without Cofagrigus")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 2; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        ReviewPlayer(SPECIES_XATU, &sReviewXatu, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_WILTON_1, 3, 0);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        struct Pokemon claydol = party[1], steelix = party[3];
        party[0] = claydol; party[1] = steelix;
        u16 zero = 0;
        SetMonData(&party[2], MON_DATA_HP, &zero); SetMonData(&party[3], MON_DATA_HP, &zero);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(70, turns, 2);
        Test_MgbaPrintf("WILTON_BACKUP room=%d timer=%d", !!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM), gFieldTimers.trickRoomTimer);
        EXPECT(gFieldStatuses & STATUS_FIELD_TRICK_ROOM);
        EXPECT_EQ(gBattleMons[1].pp[2], 4);
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Wilton timing: six-player bench with Mega reserve")
{
    u32 phase = 0;
    for (u32 p = 0; p < 2; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 40);
        ReviewOpponent(TRAINER_WILTON_1, 3, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}
