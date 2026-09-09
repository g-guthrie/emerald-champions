// Temporary actual-party Allen diagnostic; include after AuthoredOpponent.
#line 14000 "work/allen_ai_probe.inc"
#include "emerald_champions_battle_sets.h"

static void AllenProbePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species)
    {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon));
        Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
              GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

static const struct EmeraldChampionsBattleSet sAllenProbePachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .statPoints = {32, 0, 32, 0, 2, 0},
};
static const struct EmeraldChampionsBattleSet sAllenProbeEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
    .statPoints = {32, 32, 2, 0, 0, 0},
};
static const struct EmeraldChampionsBattleSet sAllenProbeLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_RAIN_DANCE, MOVE_SYNTHESIS, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_BOLD, .ability = ABILITY_RAIN_DISH,
    .statPoints = {32, 0, 32, 0, 2, 0},
};
static const struct EmeraldChampionsBattleSet sAllenProbeGogglesMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF, MOVE_PROTECT},
    .item = ITEM_SAFETY_GOGGLES, .nature = NATURE_ADAMANT, .ability = ABILITY_REGENERATOR,
    .statPoints = {32, 32, 2, 0, 0, 0},
};
static const struct EmeraldChampionsBattleSet sAllenProbeScarfMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF, MOVE_U_TURN},
    .item = ITEM_CHOICE_SCARF, .nature = NATURE_JOLLY, .ability = ABILITY_REGENERATOR,
    .statPoints = {2, 32, 0, 0, 0, 32},
};
static const struct EmeraldChampionsBattleSet sAllenProbeSlowTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_KNOCK_OFF, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_BRAVE, .ability = ABILITY_IRON_FIST,
    .statPoints = {32, 32, 2, 0, 0, 0},
};
static const struct EmeraldChampionsBattleSet sAllenProbeHeatWaveNatu = {
    .moves = {MOVE_HEAT_WAVE, MOVE_PSYCHIC, MOVE_ROOST, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_BOLD, .ability = ABILITY_MAGIC_BOUNCE,
    .statPoints = {32, 0, 32, 2, 0, 0},
};

AI_DOUBLE_BATTLE_TEST("EC live review Allen: actual opening sleep immunity and Fake Out controls")
{
    u32 scenario;
    PARAMETRIZE { scenario = 0; } // Fake Out can buy time against strong Eevee.
    PARAMETRIZE { scenario = 1; } // Grass and Goggles independently block Spore.
    PARAMETRIZE { scenario = 2; } // Real Scarf outruns Pikachu's Fake Out.
    PARAMETRIZE { scenario = 3; } // Web can move Paras before two physical foes.
    PARAMETRIZE { scenario = 4; } // Real spread Fire pressure, current Reflect.
    GIVEN {
        if (scenario == 0)
        {
            AllenProbePlayer(SPECIES_PACHIRISU, &sAllenProbePachirisu);
            AllenProbePlayer(SPECIES_EEVEE, &sAllenProbeEevee);
        }
        else if (scenario == 1)
        {
            AllenProbePlayer(SPECIES_LOTAD, &sAllenProbeLotad);
            AllenProbePlayer(SPECIES_MIENFOO, &sAllenProbeGogglesMienfoo);
        }
        else if (scenario == 2)
        {
            AllenProbePlayer(SPECIES_MIENFOO, &sAllenProbeScarfMienfoo);
            AllenProbePlayer(SPECIES_EEVEE, &sAllenProbeEevee);
        }
        else if (scenario == 3)
        {
            AllenProbePlayer(SPECIES_TIMBURR, &sAllenProbeSlowTimburr);
            AllenProbePlayer(SPECIES_TIMBURR, &sAllenProbeSlowTimburr);
        }
        else
        {
            AllenProbePlayer(SPECIES_PACHIRISU, &sAllenProbePachirisu);
            AllenProbePlayer(SPECIES_NATU, &sAllenProbeHeatWaveNatu);
        }
        // Every player species is in the current Route101/102 land roster.
        // All four actual Allen party members and trainer AI flags are kept.
        AuthoredOpponent(TRAINER_ALLEN, 0, FALSE);
        struct Pokemon *enemies = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        struct Pokemon *players = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
        EXPECT_EQ(GetMonData(&enemies[0], MON_DATA_SPECIES), SPECIES_PIKACHU);
        EXPECT_EQ(GetMonData(&enemies[1], MON_DATA_SPECIES), SPECIES_PARAS);
        if (scenario == 2)
            EXPECT_GT(GetMonData(&players[0], MON_DATA_SPEED) * 3 / 2,
                GetMonData(&enemies[0], MON_DATA_SPEED));
        if (scenario == 3)
            for (u32 slot = 0; slot < 2; slot++)
            {
                u32 playerSpeed = GetMonData(&players[slot], MON_DATA_SPEED);
                u32 parasSpeed = GetMonData(&enemies[1], MON_DATA_SPEED);
                EXPECT_GT(playerSpeed, parasSpeed);
                EXPECT_LT(playerSpeed * 2 / 3, parasSpeed);
                EXPECT_LT(playerSpeed, GetMonData(&enemies[0], MON_DATA_SPEED));
            }
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("ALLEN_INPUT scenario=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                    scenario, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
            }
    } WHEN {
        TURN {
            if (scenario == 0)
            {
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight);
                MOVE(playerRight, MOVE_DOUBLE_EDGE, target: opponentRight);
            }
            else if (scenario == 1)
            {
                MOVE(playerLeft, MOVE_GIGA_DRAIN, target: opponentRight);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentLeft);
            }
            else if (scenario == 2)
            {
                MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft);
                MOVE(playerRight, MOVE_DOUBLE_EDGE, target: opponentRight);
            }
            else if (scenario == 3)
            {
                MOVE(playerLeft, MOVE_ICE_PUNCH, target: opponentRight);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight);
            }
            else
            {
                MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE);
                MOVE(playerRight, MOVE_HEAT_WAVE);
            }
            // No expected AI moves before the baseline. Sleep, Reflect,
            // damage and Protect are competing options, not a fixed script.
        }
    } THEN {
        Test_MgbaPrintf("ALLEN_AFTER scenario=%d LAST=%S/%S liveSpecies=%d/%d HP=%d/%d playerHP=%d/%d sleep=%d/%d SpeedStage=%d/%d Reflect=%d turns=%d",
            scenario, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, playerLeft->status1 & STATUS1_SLEEP, playerRight->status1 & STATUS1_SLEEP,
            playerLeft->statStages[STAT_SPEED], playerRight->statStages[STAT_SPEED],
            (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT) != 0, gBattleResults.battleTurnCounter);
        // Root's decision-time logger owns choices/targets. THEN can already
        // contain next-turn caches; diagnostic PASS is not optimality proof.
    }
}
