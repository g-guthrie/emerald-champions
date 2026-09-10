#include "ec_review_common.inc"
#include "battle_main.h"
#line 60000 "work/angelo_gen9_probe.inc"


AI_DOUBLE_BATTLE_TEST("EC live review Angelo: speed payoff and reserve counterplay")
{
    u32 board = 0, turns = 0, phase = 0;
    const u32 horizons[3][5] = {{3, 5, 4, 3, 3}, {3, 5, 3, 2, 4}, {2, 3, 2, 2, 2}};
    for (u32 p = 0; p < 3; p++) for (u32 b = 0; b < 5; b++) for (u32 t = 1; t <= horizons[p][b]; t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet dugtrio = {
            .moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_FOCUS_SASH, .ability = ABILITY_ARENA_TRAP, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : board == 3 ? SPECIES_DUGTRIO : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : board == 3 ? &dugtrio : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : board == 3 ? SPECIES_XATU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : board == 3 ? &sReviewXatu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_ANGELO, 2, phase);
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
    } SCENE {
        if (!phase && board == 3) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, opponentRight);
            ABILITY_POPUP(opponentRight, ABILITY_WIND_POWER);
            if (turns >= 2) {
                ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERBOLT, opponentRight);
                ANIMATION(ANIM_TYPE_MOVE, MOVE_BUG_BUZZ, opponentLeft);
                ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, playerLeft);
            }
        }
    } THEN {
        ReviewResult(board, turns, phase);
        Test_MgbaPrintf("ANGELO_FIELD tailwind=%d charge=%d/%d foeAbility=%d/%d foeSpeedStage=%d/%d",
            gSideTimers[1].tailwindTimer, gBattleMons[1].volatiles.chargeTimer, gBattleMons[3].volatiles.chargeTimer,
            GetBattlerAbility(0), GetBattlerAbility(2), gBattleMons[0].statStages[STAT_SPEED], gBattleMons[2].statStages[STAT_SPEED]);
        if (!phase && board == 3 && turns <= 2) {
            EXPECT_EQ(GetBattlerTotalSpeedStat(1, GetBattlerAbility(1), GetBattlerHoldEffect(1)), 122);
            EXPECT_EQ(GetBattlerTotalSpeedStat(3, GetBattlerAbility(3), GetBattlerHoldEffect(3)), 154);
            EXPECT_EQ(opponentRight->pp[2], 14);
            EXPECT_EQ((u32)opponentRight->volatiles.chargeTimer, turns == 1 ? 1 : 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Angelo timing: six-player bench")
{
    u32 phase = 0;
    for (u32 p = 0; p < 3; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_ANGELO, 2, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}

DOUBLE_BATTLE_TEST("EC live review Angelo pivot control: available bench still switches")
{
    u32 move = MOVE_VOLT_SWITCH;
    PARAMETRIZE { move = MOVE_VOLT_SWITCH; }
    PARAMETRIZE { move = MOVE_U_TURN; }
    PARAMETRIZE { move = MOVE_FLIP_TURN; }
    PARAMETRIZE { move = MOVE_PARTING_SHOT; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(400); HP(400); Speed(10); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_SPLASH); }
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(400); HP(400); Speed(20); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(30); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_KILOWATTREL) { Speed(40); Moves(move); }
        OPPONENT(SPECIES_LUXRAY) { Speed(50); Ability(ABILITY_INTIMIDATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SPLASH);
            MOVE(playerRight, MOVE_SPLASH);
            MOVE(opponentLeft, MOVE_SPLASH);
            MOVE(opponentRight, move, target: playerLeft, hit: TRUE, criticalHit: FALSE);
            SEND_OUT(opponentRight, 2);
        }
    } THEN { EXPECT_EQ(opponentRight->species, SPECIES_LUXRAY); }
}
