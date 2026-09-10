#include "ec_review_common.inc"
#line 58000 "work/maria_gen9_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Maria: physical special and flinch pressure")
{
    u32 board = 0, turns = 0; bool32 reserves = FALSE;
    for (u32 p = 0; p < 2; p++) for (u32 b = 0; b < 3; b++) for (u32 t = 1; t <= (p ? (b == 1 ? 4 : 3) : b == 2 ? 3 : 4); t++)
        PARAMETRIZE { reserves = p; board = b; turns = t; }
    GIVEN {
        ReviewPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_XATU : SPECIES_MIENFOO,
            board == 0 ? &sReviewEevee : board == 1 ? &sReviewXatu : &sReviewMienfoo, 30);
        ReviewPlayer(board == 0 ? SPECIES_GURDURR : board == 1 ? SPECIES_PACHIRISU : SPECIES_LOMBRE,
            board == 0 ? &sReviewGurdurr : board == 1 ? &sReviewPachirisu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_MARIA_1, 2, reserves);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board == 0) {
                if (t == 0) MOVE(playerLeft, MOVE_BITE, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE, WITH_RNG(RNG_CURSED_BODY, FALSE));
                else MOVE(playerLeft, MOVE_QUICK_ATTACK, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else if (board == 1) {
                if (t == 0) {
                    MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                    MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE, WITH_RNG(RNG_CURSED_BODY, FALSE));
                } else {
                    MOVE(playerLeft, MOVE_AIR_SLASH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                    MOVE(playerRight, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE, WITH_RNG(RNG_CURSED_BODY, FALSE));
                }
            } else {
                if (t == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN { ReviewResult(board, turns, reserves); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Maria timing: healthy six-player bench")
{
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 30);
        ReviewOpponent(TRAINER_MARIA_1, 2, FALSE);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, FALSE); ReviewTiming(); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Maria speed support: fast Normal pressure and Amulet denial")
{
    bool32 amulet = FALSE; u32 turns = 0;
    for (u32 a = 0; a < 2; a++) for (u32 t = 1; t <= 5; t++) PARAMETRIZE { amulet = a; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet tauros = {
            .moves = {MOVE_BODY_SLAM, MOVE_ZEN_HEADBUTT, MOVE_CLOSE_COMBAT, MOVE_PROTECT},
            .item = amulet ? ITEM_CLEAR_AMULET : ITEM_SITRUS_BERRY,
            .nature = NATURE_JOLLY, .ability = ABILITY_INTIMIDATE, .evs = {4, 252, 0, 0, 0, 252},
        };
        ReviewPlayer(SPECIES_TAUROS, &tauros, 30);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 30);
        ReviewOpponent(TRAINER_MARIA_1, 2, FALSE);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_ZEN_HEADBUTT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE, WITH_RNG(RNG_CURSED_BODY, FALSE));
        }
    } THEN {
        ReviewResult(10 + amulet, turns, FALSE);
        Test_MgbaPrintf("MARIA_SPEED tauros=%d stage=%d rightSpecies=%d speed=%d stage=%d", playerLeft->speed, playerLeft->statStages[STAT_SPEED], opponentRight->species, opponentRight->speed, opponentRight->statStages[STAT_SPEED]);
        if (turns == 1) {
            EXPECT_EQ(playerLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - !amulet);
            EXPECT_EQ(opponentLeft->pp[0], 14);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Maria setup opportunity: bulky Normal special attackers")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 5; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet blissey = {
            .moves = {MOVE_HYPER_VOICE, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_SOFT_BOILED},
            .item = ITEM_LEFTOVERS, .nature = NATURE_CALM, .ability = ABILITY_NATURAL_CURE, .evs = {252, 0, 252, 0, 4, 0},
        };
        const struct EmeraldChampionsBattleSet porygon = {
            .moves = {MOVE_TRI_ATTACK, MOVE_ICE_BEAM, MOVE_RECOVER, MOVE_TRICK_ROOM},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_TRACE, .evs = {252, 0, 4, 252, 0, 0},
        };
        ReviewPlayer(SPECIES_BLISSEY, &blissey, 30);
        ReviewPlayer(SPECIES_PORYGON2, &porygon, 30);
        ReviewOpponent(TRAINER_MARIA_1, 2, FALSE);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_HYPER_VOICE, hit: TRUE, criticalHit: FALSE, WITH_RNG(RNG_CURSED_BODY, FALSE));
            MOVE(playerRight, MOVE_ICE_BEAM, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE, WITH_RNG(RNG_CURSED_BODY, FALSE));
        }
    } THEN { ReviewResult(30, turns, FALSE); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Maria denial: Taunt and spread Ground")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++) for (u32 t = 1; t <= 3; t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet mudbray = {
            .moves = {MOVE_EARTHQUAKE, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_STAMINA, .evs = {252, 252, 4, 0, 0, 0},
        };
        ReviewPlayer(board ? SPECIES_MUDBRAY : SPECIES_MIENFOO, board ? &mudbray : &sReviewMienfoo, 30);
        ReviewPlayer(board ? SPECIES_XATU : SPECIES_LOMBRE, board ? &sReviewXatu : &sReviewLombre, 30);
        ReviewOpponent(TRAINER_MARIA_1, 2, FALSE);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            if (board) {
                MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE, WITH_RNG(RNG_CURSED_BODY, FALSE));
                MOVE(playerRight, MOVE_AIR_SLASH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                if (t == 0) MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_SCALD, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE, WITH_RNG(RNG_CURSED_BODY, FALSE));
            }
        }
    } THEN {
        ReviewResult(20 + board, turns, FALSE);
        if (!board && turns == 1) {
            EXPECT_EQ(opponentRight->species, SPECIES_SALANDIT);
            EXPECT_EQ(GetBattlerAbility(B_BATTLER_3), ABILITY_OBLIVIOUS);
            EXPECT_EQ((u32)opponentRight->volatiles.tauntTimer, 0);
        }
    }
}
