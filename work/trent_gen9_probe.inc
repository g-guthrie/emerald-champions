#include "ec_review_common.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Trent: opposing rain")
{
    u32 turns = 1;
    for (u32 t = 1; t <= 4; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet lombre = {
            .moves = {MOVE_SURF, MOVE_GIGA_DRAIN, MOVE_RAIN_DANCE, MOVE_PROTECT}, .item = ITEM_EVIOLITE,
            .ability = ABILITY_SWIFT_SWIM, .nature = NATURE_MODEST, .evs = {252, 0, 4, 252, 0, 0},
        };
        const struct EmeraldChampionsBattleSet oranguru = {
            .moves = {MOVE_PSYCHIC, MOVE_INSTRUCT, MOVE_TRICK_ROOM, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY,
            .ability = ABILITY_TELEPATHY, .nature = NATURE_CALM, .evs = {252, 0, 4, 0, 252, 0},
        };
        ReviewPlayer(SPECIES_LOMBRE, &lombre, 40); ReviewPlayer(SPECIES_ORANGURU, &oranguru, 40);
        ReviewOpponent(TRAINER_TRENT_1, 3, 0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (!t) MOVE(playerLeft, MOVE_RAIN_DANCE);
            else MOVE(playerLeft, MOVE_SURF, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(70, turns, 0);
        Test_MgbaPrintf("TRENT_RAIN weather=%d speed=%d", gBattleWeather,
            GetBattlerTotalSpeedStat(1, GetBattlerAbility(1), GetBattlerHoldEffect(1)));
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Trent: authored friendship preservation")
{
    u32 trainer = TRAINER_TRENT_1;
    PARAMETRIZE { trainer = TRAINER_TRENT_1; }
    PARAMETRIZE { trainer = TRAINER_ALLEN; }
    PARAMETRIZE { trainer = TRAINER_JOHNSON; }
    PARAMETRIZE { trainer = TRAINER_DAWSON; }
    GIVEN {
        ReviewPlayer(SPECIES_EEVEE, &sReviewEevee, 40);
        ReviewPlayer(SPECIES_GURDURR, &sReviewGurdurr, 40);
        ReviewOpponent(trainer, 3, 0);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        u32 checked = 0;
        for (u32 i = 0; i < gTrainers[DIFFICULTY_NORMAL][trainer].partySize; i++)
            for (u32 j = 0; j < MAX_MON_MOVES; j++) {
                u32 move = GetMonData(&party[i], MON_DATA_MOVE1 + j);
                if (move == MOVE_RETURN || move == MOVE_FRUSTRATION) {
                    u32 friendship = GetMonData(&party[i], MON_DATA_FRIENDSHIP);
                    EXPECT_EQ(friendship, move == MOVE_RETURN ? MAX_FRIENDSHIP : 0);
                    Test_MgbaPrintf("TRENT_FRIENDSHIP trainer=%d move=%d friendship=%d power=%d", trainer, move, friendship,
                        10 * (move == MOVE_RETURN ? friendship : MAX_FRIENDSHIP - friendship) / 25);
                    checked++;
                }
            }
        EXPECT_EQ(checked, 1);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); } }
}
AI_DOUBLE_BATTLE_TEST("EC live review Trent: sand offense and reserves")
{
    u32 board = 0, turns = 0, phase = 0, visit = 0;
    const u32 horizons[2][5] = {{4,2,3,3,3},{3,2,4,3,6}};
    for (u32 v = 1; v < 2; v++) for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= horizons[p][b]; t++)
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
        ReviewOpponent(TRAINER_TRENT_1, visit ? 3 : 2, phase);
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
                if (t == 0) MOVE(playerLeft, board == 4 ? MOVE_TAUNT : MOVE_FAKE_OUT, target: board == 4 ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, board == 4 ? MOVE_GIGA_DRAIN : MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board + visit * 10, turns, phase);
        if (!phase && gBattleMons[1].species == SPECIES_STOUTLAND)
            EXPECT_EQ(gBattleMons[1].friendship, MAX_FRIENDSHIP);
        if (!phase && gBattleMons[1].species == SPECIES_STOUTLAND && gBattleMons[1].hp) {
            Test_MgbaPrintf("TRENT_SPEED weather=%d speed=%d", gBattleWeather,
                GetBattlerTotalSpeedStat(1, GetBattlerAbility(1), GetBattlerHoldEffect(1)));
            if (gBattleWeather & B_WEATHER_SANDSTORM)
                EXPECT_EQ(GetBattlerTotalSpeedStat(1, GetBattlerAbility(1), GetBattlerHoldEffect(1)), 202);
        }
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Trent timing: six-player bench")
{
    u32 phase = 0, visit = 0;
    for (u32 v = 1; v < 2; v++) for (u32 p = 0; p < 2; p++) PARAMETRIZE { visit = v; phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], visit ? 40 : 30);
        ReviewOpponent(TRAINER_TRENT_1, visit ? 3 : 2, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(98 + visit, 1, phase); ReviewTiming(); }
}
