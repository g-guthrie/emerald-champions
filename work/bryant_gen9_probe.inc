#include "ec_review_common.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Bryant: physical setup and reserves")
{
    u32 board = 0, turns = 0, phase = 0, visit = 0;
    for (u32 v = 1; v < 2; v++) for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= (p ? (b == 1 ? 2 : b == 4 ? 4 : 3) : 2); t++)
        PARAMETRIZE { visit = v; phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet dugtrio = {
            .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_ARENA_TRAP, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : board == 3 ? SPECIES_DUGTRIO : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : board == 3 ? &dugtrio : &sReviewMienfoo, visit ? 40 : 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : board == 3 ? SPECIES_XATU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : board == 3 ? &sReviewXatu : &sReviewLombre, visit ? 40 : 30);
        ReviewOpponent(TRAINER_BRYANT, visit ? 3 : 2, phase);
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
                if (t == 0) MOVE(playerLeft, board == 4 ? MOVE_TAUNT : MOVE_FAKE_OUT, target: board == 4 && !phase ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, board == 4 ? MOVE_GIGA_DRAIN : MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } SCENE {
        if (!phase) ABILITY_POPUP(opponentRight, ABILITY_INTIMIDATE);
    } THEN {
        ReviewResult(board + visit * 10, turns, phase);
        if (!phase && gBattleMons[1].species == SPECIES_TYPHLOSION && gBattleMons[1].hp) {
            u32 speed = GetBattlerTotalSpeedStat(B_BATTLER_1, GetBattlerAbility(B_BATTLER_1), GetBattlerHoldEffect(B_BATTLER_1));
            Test_MgbaPrintf("BRYANT_SCARF_SPEED=%d", speed);
            EXPECT_EQ(speed, 190);
        }
        if (!phase && board == 2 && turns == 1) {
            EXPECT_EQ(gBattleMons[3].item, ITEM_NONE);
            EXPECT(gBattleMons[3].hp > 0);
        }
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Bryant: Wide Guard and rain disruption")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= (b ? 5 : 4); t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet hariyama = {
            .moves = {MOVE_FAKE_OUT, MOVE_WIDE_GUARD, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
            .item = ITEM_SITRUS_BERRY, .ability = ABILITY_THICK_FAT, .nature = NATURE_ADAMANT, .evs = {252, 252, 4, 0, 0, 0},
        };
        const struct EmeraldChampionsBattleSet lombre = {
            .moves = {MOVE_RAIN_DANCE, MOVE_SCALD, MOVE_GIGA_DRAIN, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .ability = ABILITY_SWIFT_SWIM, .nature = NATURE_MODEST, .evs = {252, 0, 4, 252, 0, 0},
        };
        ReviewPlayer(SPECIES_HARIYAMA, &hariyama, 40);
        ReviewPlayer(SPECIES_LOMBRE, &lombre, 40);
        ReviewOpponent(TRAINER_BRYANT, 3, 0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board && !t) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else MOVE(playerLeft, MOVE_WIDE_GUARD);
            if (!t) MOVE(playerRight, MOVE_RAIN_DANCE);
            else MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(70 + board, turns, 0);
        Test_MgbaPrintf("BRYANT_RAIN=%d duration=%d", gBattleWeather, gBattleStruct->weatherDuration);
        Test_MgbaPrintf("BRYANT_LOMBRE status=%d", gBattleMons[2].status1);
        struct Pokemon *arcanine = &gParties[B_TRAINER_OPPONENT_A][1];
        Test_MgbaPrintf("BRYANT_ARCANINE HP=%d PP=%d/%d/%d/%d", GetMonData(arcanine, MON_DATA_HP),
            GetMonData(arcanine, MON_DATA_PP1), GetMonData(arcanine, MON_DATA_PP2), GetMonData(arcanine, MON_DATA_PP3), GetMonData(arcanine, MON_DATA_PP4));
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Bryant timing: six-player bench")
{
    u32 phase = 0, visit = 0;
    for (u32 v = 1; v < 2; v++) for (u32 p = 0; p < 2; p++) PARAMETRIZE { visit = v; phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], visit ? 40 : 30);
        ReviewOpponent(TRAINER_BRYANT, visit ? 3 : 2, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(98 + visit, 1, phase); ReviewTiming(); }
}
