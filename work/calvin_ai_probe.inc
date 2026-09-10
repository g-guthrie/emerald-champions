#line 3000 "work/calvin_ai_probe.inc"
// Temporary diagnostic only. Include after AuthoredOpponent in
// test/battle/ai/emerald_champions_plans.c; remove the include after the probe.
// No copied Calvin loadout, no invented derived stats, no permanent suite.
#include "emerald_champions_battle_sets.h"

static void CalvinProbePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sCalvinProbePachirisu = {
    .moves = {MOVE_NUZZLE, MOVE_FOLLOW_ME, MOVE_HELPING_HAND, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_IMPISH, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sCalvinProbeLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_RAIN_DANCE, MOVE_SYNTHESIS, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_BOLD, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sCalvinProbeMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {4, 252, 0, 0, 0, 252},
};
static const struct EmeraldChampionsBattleSet sCalvinProbeTimburr = {
    .moves = {MOVE_MACH_PUNCH, MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_IRON_FIST,
    .evs = {252, 252, 4, 0, 0, 0},
};
static const struct EmeraldChampionsBattleSet sCalvinProbePoochyena = {
    .moves = {MOVE_TAUNT, MOVE_CRUNCH, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
    .item = ITEM_FOCUS_SASH, .nature = NATURE_JOLLY, .ability = ABILITY_RATTLED,
    .evs = {252, 0, 4, 0, 0, 252},
};

AI_DOUBLE_BATTLE_TEST("EC live review Calvin: actual team opportunity danger and denial")
{
    u32 scenario, turns;
    PARAMETRIZE { scenario = 0; turns = 1; } // Low immediate damage, real bulky opponents.
    PARAMETRIZE { scenario = 0; turns = 2; } // Actual unrestricted payoff.
    PARAMETRIZE { scenario = 3; turns = 2; } // Payoff without paralysis interruption.
    PARAMETRIZE { scenario = 1; turns = 1; } // Two Fighting attacks into Zigzagoon.
    PARAMETRIZE { scenario = 2; turns = 1; } // Fast Taunt, with Fake Out stopping Poochyena.
    GIVEN {
        if (scenario == 0 || scenario == 3)
        {
            CalvinProbePlayer(SPECIES_PACHIRISU, &sCalvinProbePachirisu);
            CalvinProbePlayer(SPECIES_LOTAD, &sCalvinProbeLotad);
        }
        else if (scenario == 1)
        {
            CalvinProbePlayer(SPECIES_MIENFOO, &sCalvinProbeMienfoo);
            CalvinProbePlayer(SPECIES_TIMBURR, &sCalvinProbeTimburr);
        }
        else
        {
            CalvinProbePlayer(SPECIES_POOCHYENA, &sCalvinProbePoochyena);
            CalvinProbePlayer(SPECIES_MIENFOO, &sCalvinProbeMienfoo);
        }
        AuthoredOpponent(TRAINER_CALVIN_1, 0, FALSE);
    } WHEN {
        if (scenario == 0 || scenario == 3)
            TURN {
                MOVE(playerLeft, scenario == 3 ? MOVE_FOLLOW_ME : MOVE_NUZZLE, target: opponentLeft);
                MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentLeft);
            }
        else if (scenario == 1)
            TURN {
                MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight);
                MOVE(playerRight, MOVE_MACH_PUNCH, target: opponentRight);
            }
        else
            TURN {
                MOVE(playerLeft, MOVE_TAUNT, target: opponentRight);
                MOVE(playerRight, MOVE_FAKE_OUT, target: opponentLeft);
            }
        if (turns == 2)
            TURN {
                MOVE(playerLeft, MOVE_FOLLOW_ME);
                MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentLeft);
            }
    } THEN {
        // The harness has already selected the following turn before THEN.
        // Root's temporary decision-time logger records turn-one selections;
        // these fields below are explicitly next-turn diagnostics.
        Test_MgbaPrintf("Calvin scenario=%d NEXT actions=%d/%d NEXT moves=%d/%d LAST executed=%d/%d liveSpecies=%d/%d ZigPartyHP=%d item=%d AttackStage=%d",
            scenario, gChosenActionByBattler[B_BATTLER_1], gChosenActionByBattler[B_BATTLER_3],
            gChosenMoveByBattler[B_BATTLER_1], gChosenMoveByBattler[B_BATTLER_3],
            gLastMoves[B_BATTLER_1], gLastMoves[B_BATTLER_3],
            opponentLeft->species, opponentRight->species,
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HELD_ITEM),
            opponentRight->statStages[STAT_ATK]);
        for (u32 i = 0; i < 4; i++)
            Test_MgbaPrintf("Calvin generated slot=%d species=%d level=%d maxHP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d",
                i, GetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_LEVEL),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_MAX_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_ATK),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_DEF),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_SPATK),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_SPDEF),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_SPEED));
        for (u32 i = 0; i < 2; i++)
            Test_MgbaPrintf("Calvin native player=%d species=%d level=%d maxHP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d",
                i, GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_LEVEL),
                GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_MAX_HP),
                GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_ATK),
                GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_DEF),
                GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPATK),
                GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPDEF),
                GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPEED));
        // A diagnostic PASS only means the native scenario ran. It does not
        // bless the selected move. Inspect the choices, execution, and cache;
        // add a narrow expectation only after proving the tactical contract.
    }
}

DOUBLE_BATTLE_TEST("EC live review Calvin: native Drum Sitrus and redirected Helping Hand payoff", s16 priorityDamage)
{
    bool32 drum;
    PARAMETRIZE { drum = FALSE; }
    PARAMETRIZE { drum = TRUE; }
    GIVEN {
        CalvinProbePlayer(SPECIES_PACHIRISU, &sCalvinProbePachirisu);
        CalvinProbePlayer(SPECIES_LOTAD, &sCalvinProbeLotad);
        AuthoredOpponent(TRAINER_CALVIN_1, 0, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_TAUNT, target: playerLeft);
            MOVE(opponentRight, drum ? MOVE_BELLY_DRUM : MOVE_PROTECT);
        }
        TURN {
            MOVE(playerLeft, MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentLeft);
            MOVE(opponentLeft, MOVE_HELPING_HAND, target: opponentRight);
            // The intended target is Lotad. Native +2 Follow Me on faster
            // Pachirisu redirects +2 Extreme Speed before it executes.
            MOVE(opponentRight, MOVE_EXTREME_SPEED, target: playerRight, hit: TRUE, criticalHit: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EXTREME_SPEED, opponentRight);
        HP_BAR(playerLeft, captureDamage: &results[i].priorityDamage);
    } THEN {
        u32 maximum = opponentRight->maxHP;
        Test_MgbaPrintf("Calvin fixed Drum=%d ExtremeSpeed redirectedDamage=%d ZigHP=%d/%d item=%d stage=%d PachSpeed=%d ZigSpeed=%d",
            drum, results[i].priorityDamage, opponentRight->hp, maximum,
            opponentRight->item, opponentRight->statStages[STAT_ATK], playerLeft->speed, opponentRight->speed);
        EXPECT_EQ(playerRight->hp, playerRight->maxHP);
        if (drum)
        {
            EXPECT_EQ(opponentRight->statStages[STAT_ATK], MAX_STAT_STAGE);
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
            EXPECT_EQ(opponentRight->hp, maximum - maximum / 2 + maximum / 4);
        }
        else
        {
            EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
            EXPECT_EQ(opponentRight->item, ITEM_SITRUS_BERRY);
        }
    } FINALLY {
        EXPECT_GT(results[1].priorityDamage, results[0].priorityDamage);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Calvin: actual reserves after both leads faint")
{
    GIVEN {
        CalvinProbePlayer(SPECIES_PACHIRISU, &sCalvinProbePachirisu);
        CalvinProbePlayer(SPECIES_LOTAD, &sCalvinProbeLotad);
        AuthoredOpponent(TRAINER_CALVIN_1, 0, FALSE);
        // Reachable remaining-party snapshot, not a replacement roster.
        u16 zero = 0;
        SetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP, &zero);
        SetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP, &zero);
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 2;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 3;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_NUZZLE, target: opponentRight);
            MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentRight);
        }
    } THEN {
        Test_MgbaPrintf("Calvin reserves LAST=%d/%d liveSpecies=%d/%d HP=%d/%d leftReservePartyStatus=%d StarlyPartyHP=%d",
            gLastMoves[B_BATTLER_1], gLastMoves[B_BATTLER_3], opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp,
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_STATUS),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][3], MON_DATA_HP));
    }
}

// Both species are available before Calvin. These are legal preparation-point
// allocations, not overrides of derived combat stats. Their weak single-target
// item-supported attacks isolate two modest hits before the slow Drum user acts.
static const struct EmeraldChampionsBattleSet sCalvinProbeModeratePachirisu = {
    .moves = {MOVE_ROUND, MOVE_FOLLOW_ME, MOVE_HELPING_HAND, MOVE_PROTECT},
    .item = ITEM_SILK_SCARF, .nature = NATURE_MODEST, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 0, 220, 0, 36},
};
static const struct EmeraldChampionsBattleSet sCalvinProbeModerateLotad = {
    .moves = {MOVE_WATER_GUN, MOVE_RAIN_DANCE, MOVE_SYNTHESIS, MOVE_PROTECT},
    .item = ITEM_MYSTIC_WATER, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 204, 0, 52},
};

DOUBLE_BATTLE_TEST("Historical flat-stat Calvin: combined modest hits counterfactual", s16 firstDamage; s16 secondDamage)
{
    bool32 fixedLine;
    PARAMETRIZE { fixedLine = TRUE; }
    GIVEN {
        CalvinProbePlayer(SPECIES_PACHIRISU, &sCalvinProbeModeratePachirisu);
        CalvinProbePlayer(SPECIES_LOTAD, &sCalvinProbeModerateLotad);
        AuthoredOpponent(TRAINER_CALVIN_1, 0, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ROUND, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_WATER_GUN, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentLeft, MOVE_HELPING_HAND, target: opponentRight);
            MOVE(opponentRight, MOVE_BELLY_DRUM);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROUND, playerLeft);
        HP_BAR(opponentRight, captureDamage: &results[i].firstDamage);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, playerRight);
        HP_BAR(opponentRight, captureDamage: &results[i].secondDamage);
    } THEN {
        EXPECT(fixedLine);
        Test_MgbaPrintf("Calvin modest fixed nativeDamage=%d/%d nativeSpeeds=%d/%d/%d ZigHP=%d/%d item=%d stage=%d LAST=%d",
            results[i].firstDamage, results[i].secondDamage,
            playerLeft->speed, playerRight->speed, opponentRight->speed,
            opponentRight->hp, opponentRight->maxHP, opponentRight->item,
            opponentRight->statStages[STAT_ATK], gLastMoves[B_BATTLER_3]);
        EXPECT_GT(playerLeft->speed, opponentRight->speed);
        EXPECT_GT(playerRight->speed, opponentRight->speed);
        EXPECT_LT(results[i].firstDamage, opponentRight->maxHP / 2);
        EXPECT_LT(results[i].secondDamage, opponentRight->maxHP / 2);
        EXPECT_LE(opponentRight->hp, opponentRight->maxHP / 2);
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP - results[i].firstDamage - results[i].secondDamage + opponentRight->maxHP / 4);
        EXPECT_EQ(opponentRight->item, ITEM_NONE);
        EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Calvin: actual AI against combined modest pressure")
{
    GIVEN {
        CalvinProbePlayer(SPECIES_PACHIRISU, &sCalvinProbeModeratePachirisu);
        CalvinProbePlayer(SPECIES_LOTAD, &sCalvinProbeModerateLotad);
        AuthoredOpponent(TRAINER_CALVIN_1, 0, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ROUND, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_WATER_GUN, target: opponentRight, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        // A PASS is only diagnostic execution. The decision-time logger, not
        // the next-turn cache, establishes which action the AI actually chose.
        Test_MgbaPrintf("Calvin modest AI LAST=%d/%d species=%d/%d HP=%d/%d item=%d stage=%d NEXT=%d/%d",
            gLastMoves[B_BATTLER_1], gLastMoves[B_BATTLER_3],
            opponentLeft->species, opponentRight->species,
            opponentRight->hp, opponentRight->maxHP, opponentRight->item,
            opponentRight->statStages[STAT_ATK],
            gChosenMoveByBattler[B_BATTLER_1], gChosenMoveByBattler[B_BATTLER_3]);
    }
}
