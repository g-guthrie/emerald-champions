#include "ec_review_common.inc"
#line 60000 "work/tyron_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Tyron: physical special and flinch pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (p && b == 2 ? 2 : p && b == 1 ? 5 : 3); t++)
        PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_TYRON, 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, phase ? MOVE_ICE_PUNCH : MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
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
        Test_MgbaPrintf("TYRON_SPEED actual=%d/%d", GetBattlerTotalSpeedStat(B_BATTLER_1, GetBattlerAbility(B_BATTLER_1), GetBattlerHoldEffect(B_BATTLER_1)), GetBattlerTotalSpeedStat(B_BATTLER_3, GetBattlerAbility(B_BATTLER_3), GetBattlerHoldEffect(B_BATTLER_3)));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Tyron: Rock and Fire pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= (p ? 3 : 4); t++) PARAMETRIZE { phase = p; board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet hippowdon = {
            .moves = {MOVE_ROCK_SLIDE, MOVE_HIGH_HORSEPOWER, MOVE_SLACK_OFF, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .ability = ABILITY_SAND_FORCE, .nature = NATURE_IMPISH, .evs = {252, 0, 252, 0, 4, 0},
        };
        const struct EmeraldChampionsBattleSet magmar = {
            .moves = {MOVE_FLAMETHROWER, MOVE_PSYCHIC, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .ability = ABILITY_FLAME_BODY, .nature = NATURE_TIMID, .evs = {4, 0, 0, 252, 0, 252},
        };
        ReviewPlayer(board ? SPECIES_MAGMAR : SPECIES_HIPPOWDON, board ? &magmar : &hippowdon, 30);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewOpponent(TRAINER_TYRON, 2, phase);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board) MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else MOVE(playerLeft, MOVE_ROCK_SLIDE, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { ReviewResult(10 + board, turns, phase); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Tyron Mega order")
{
    GIVEN {
        ReviewPlayer(SPECIES_XATU, &sReviewXatu, 30);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewOpponent(TRAINER_TYRON, 2, 1);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POISON_JAB, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, playerLeft);
    } THEN {
        ReviewResult(90, 1, 1);
        Test_MgbaPrintf("TYRON_MEGA_STATS Atk=%d Speed=%d XatuDef=%d XatuSpeed=%d orderConfig=%d", opponentLeft->attack, opponentLeft->speed, playerLeft->defense, playerLeft->speed, GetConfig(B_MEGA_EVO_TURN_ORDER));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Tyron timing: healthy six-player bench")
{
    u32 phase = 0;
    PARAMETRIZE { phase = 0; }
    PARAMETRIZE { phase = 1; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_TYRON, 2, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Tyron coverage: Fire Flying reserves")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet bird = {
            .moves = {MOVE_ACROBATICS, MOVE_FLAME_CHARGE, MOVE_ROOST, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .ability = ABILITY_FLAME_BODY, .nature = NATURE_JOLLY, .evs = {4, 252, 0, 0, 0, 252},
        };
        ReviewPlayer(SPECIES_FLETCHINDER, &bird, 30);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewOpponent(TRAINER_TYRON, 2, 1);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_ACROBATICS, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { ReviewResult(20, turns, 1); }
}
