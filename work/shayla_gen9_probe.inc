#include "ec_review_common.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Shayla: physical setup and reserves")
{
    u32 board = 0, turns = 0, phase = 0, visit = 0;
    for (u32 v = 1; v < 2; v++) for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= (p ? (b == 1 ? 3 : b == 3 ? 4 : 2) : (b == 1 ? 3 : b == 4 ? 5 : 4)); t++)
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
        ReviewOpponent(TRAINER_SHAYLA, visit ? 3 : 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_AIR_SLASH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
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
    } SCENE {
        if (phase && board == 2) ABILITY_POPUP(opponentRight, ABILITY_QUEENLY_MAJESTY);
    } THEN {
        ReviewResult(board + visit * 10, turns, phase);
        Test_MgbaPrintf("SHAYLA_WEATHER=%d leftSpeed=%d rightSpeed=%d", gBattleWeather,
            GetBattlerTotalSpeedStat(B_BATTLER_1, GetBattlerAbility(B_BATTLER_1), GetBattlerHoldEffect(B_BATTLER_1)),
            GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)));
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Shayla: opposing rain and weather recovery")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 4; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet lombre = {
            .moves = {MOVE_RAIN_DANCE, MOVE_SCALD, MOVE_GIGA_DRAIN, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .ability = ABILITY_SWIFT_SWIM, .nature = NATURE_MODEST, .evs = {252, 0, 4, 252, 0, 0},
        };
        const struct EmeraldChampionsBattleSet oranguru = {
            .moves = {MOVE_PSYCHIC, MOVE_INSTRUCT, MOVE_TRICK_ROOM, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .ability = ABILITY_TELEPATHY, .nature = NATURE_MODEST, .evs = {252, 0, 4, 252, 0, 0},
        };
        ReviewPlayer(SPECIES_LOMBRE, &lombre, 40);
        ReviewPlayer(SPECIES_ORANGURU, &oranguru, 40);
        ReviewOpponent(TRAINER_SHAYLA, 3, 0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (!t) MOVE(playerLeft, MOVE_RAIN_DANCE);
            else MOVE(playerLeft, MOVE_SCALD, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(70, turns, 0);
        Test_MgbaPrintf("SHAYLA_RAIN_CASE weather=%d duration=%d leftSpeed=%d rightSpeed=%d", gBattleWeather, gBattleStruct->weatherDuration,
            GetBattlerTotalSpeedStat(B_BATTLER_1, GetBattlerAbility(B_BATTLER_1), GetBattlerHoldEffect(B_BATTLER_1)),
            GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)));
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Shayla: Flower Veil supports the physical reserve")
{
    GIVEN {
        const struct EmeraldChampionsBattleSet mawile = {
            .moves = {MOVE_IRON_HEAD, MOVE_SUCKER_PUNCH, MOVE_PLAY_ROUGH, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .ability = ABILITY_INTIMIDATE, .nature = NATURE_ADAMANT, .evs = {252, 252, 4, 0, 0, 0},
        };
        struct EmeraldChampionsBattleSet pachirisu = sReviewPachirisu;
        pachirisu.moves[1] = MOVE_THUNDER_WAVE;
        ReviewPlayer(SPECIES_MAWILE, &mawile, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &pachirisu, 40);
        ReviewOpponent(TRAINER_SHAYLA, 3, 1);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_IRON_HEAD, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDER_WAVE, target: opponentRight, hit: TRUE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_INTIMIDATE);
        ABILITY_POPUP(opponentLeft, ABILITY_FLOWER_VEIL);
    } THEN {
        ReviewResult(80, 1, 1);
        EXPECT_EQ(gBattleMons[3].statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(gBattleMons[3].status1, STATUS1_NONE);
        EXPECT(gBattleMons[1].hp > 0);
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Shayla timing: six-player bench")
{
    u32 phase = 0, visit = 0;
    for (u32 v = 1; v < 2; v++) for (u32 p = 0; p < 2; p++) PARAMETRIZE { visit = v; phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], visit ? 40 : 30);
        ReviewOpponent(TRAINER_SHAYLA, visit ? 3 : 2, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(98 + visit, 1, phase); ReviewTiming(); }
}
