#include "ec_review_common.inc"
#include "battle_main.h"
AI_DOUBLE_BATTLE_TEST("EC live review Paul: Corrosion and powder-denial matchup")
{
    u32 turns = 1;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet tentacruel = {
            .moves = {MOVE_SCALD, MOVE_SLUDGE_BOMB, MOVE_ACID_SPRAY, MOVE_PROTECT}, .item = ITEM_SAFETY_GOGGLES,
            .ability = ABILITY_LIQUID_OOZE, .nature = NATURE_CALM, .evs = {252, 0, 4, 0, 252, 0},
        };
        const struct EmeraldChampionsBattleSet bronzong = {
            .moves = {MOVE_GYRO_BALL, MOVE_BODY_PRESS, MOVE_LIGHT_SCREEN, MOVE_PROTECT}, .item = ITEM_SAFETY_GOGGLES,
            .ability = ABILITY_HEATPROOF, .nature = NATURE_SASSY, .evs = {252, 0, 4, 0, 252, 0},
        };
        ReviewPlayer(SPECIES_TENTACRUEL, &tentacruel, 40);
        ReviewPlayer(SPECIES_BRONZONG, &bronzong, 40);
        ReviewOpponent(TRAINER_PAUL, 3, 0);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_SLUDGE_BOMB, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (!t) MOVE(playerRight, MOVE_LIGHT_SCREEN);
            else MOVE(playerRight, MOVE_BODY_PRESS, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        ReviewResult(70, turns, 0);
        Test_MgbaPrintf("PAUL_STATUS players=%d/%d", playerLeft->status1, playerRight->status1);
        if (turns == 1) {
            u32 savedIndex = gAiThinkingStruct->movesetIndex;
            enum Ability savedAbility = gAiLogicData->abilities[B_BATTLER_1];
            gAiThinkingStruct->movesetIndex = 2;
            EXPECT(AI_CanPoison(B_BATTLER_1, B_BATTLER_0, GetBattlerAbility(B_BATTLER_0), MOVE_TOXIC, MOVE_GIGA_DRAIN));
            EXPECT(AI_CanPoison(B_BATTLER_1, B_BATTLER_2, GetBattlerAbility(B_BATTLER_2), MOVE_TOXIC, MOVE_GIGA_DRAIN));
            EXPECT_EQ(gAiLogicData->effectiveness[B_BATTLER_1][B_BATTLER_2][0], UQ_4_12(0.0));
            gAiLogicData->abilities[B_BATTLER_1] = ABILITY_NONE;
            EXPECT(!AI_CanPoison(B_BATTLER_1, B_BATTLER_0, GetBattlerAbility(B_BATTLER_0), MOVE_TOXIC, MOVE_GIGA_DRAIN));
            EXPECT(!AI_CanPoison(B_BATTLER_1, B_BATTLER_2, GetBattlerAbility(B_BATTLER_2), MOVE_TOXIC, MOVE_GIGA_DRAIN));
            gAiLogicData->abilities[B_BATTLER_1] = savedAbility;
            gAiThinkingStruct->movesetIndex = savedIndex;
            Test_MgbaPrintf("PAUL_CORROSION nativeEligibility=PoisonAndSteel damagingPoisonStillImmune=1");
        }
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Paul: damaged Regenerator pivot")
{
    u32 turns = 1;
    for (u32 t = 1; t <= 4; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet magmar = {
            .moves = {MOVE_FLAMETHROWER, MOVE_PSYCHIC, MOVE_WILL_O_WISP, MOVE_PROTECT}, .item = ITEM_EVIOLITE,
            .ability = ABILITY_FLAME_BODY, .nature = NATURE_TIMID, .evs = {4, 0, 0, 252, 0, 252},
        };
        ReviewPlayer(SPECIES_MAGMAR, &magmar, 40);
        ReviewPlayer(SPECIES_PACHIRISU, &sReviewPachirisu, 40);
        ReviewOpponent(TRAINER_PAUL, 3, 0);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        u16 hp = 60; SetMonData(&party[1], MON_DATA_HP, &hp);
    } WHEN {
        for (u32 t = 0; t < turns; t++) TURN {
            MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        ReviewResult(71, turns, 0);
        Test_MgbaPrintf("PAUL_REGEN benchHP=%d", GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP));
        if (turns == 1) {
            EXPECT_EQ(opponentRight->species, SPECIES_DRAPION);
            EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP), 158);
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HELD_ITEM), ITEM_NONE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Paul: poison support and reserve pressure")
{
    u32 board = 0, turns = 0, phase = 0;
    const u32 horizons[2][5] = {{4, 5, 3, 3, 3}, {3, 2, 2, 2, 2}};
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
        ReviewOpponent(TRAINER_PAUL, 3, phase);
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
                if (t == 0) MOVE(playerLeft, board == 4 ? MOVE_TAUNT : MOVE_FAKE_OUT, target: board == 4 && phase ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, board == 4 ? MOVE_GIGA_DRAIN : MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        ReviewResult(board, turns, phase);
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Paul timing: six-player bench")
{
    u32 phase = 0;
    for (u32 p = 0; p < 2; p++) PARAMETRIZE { phase = p; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) ReviewPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]], 40);
        ReviewOpponent(TRAINER_PAUL, 3, phase);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN { ReviewResult(99, 1, phase); ReviewTiming(); }
}
