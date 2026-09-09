// Temporary native two-turn follow-through; include after allen_ai_probe.inc.
#line 18000 "work/allen_followthrough_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Allen follow-through: actual repeated pressure for two turns")
{
    bool32 timburr;
    PARAMETRIZE { timburr = FALSE; }
    PARAMETRIZE { timburr = TRUE; }
    GIVEN {
        if (timburr)
        {
            AllenProbePlayer(SPECIES_TIMBURR, &sAllenProbeSlowTimburr);
            AllenProbePlayer(SPECIES_TIMBURR, &sAllenProbeSlowTimburr);
        }
        else
        {
            AllenProbePlayer(SPECIES_PACHIRISU, &sAllenProbePachirisu);
            AllenProbePlayer(SPECIES_EEVEE, &sAllenProbeEevee);
        }
        AuthoredOpponent(TRAINER_ALLEN, 0, FALSE);
    } WHEN {
        for (u32 turn = 0; turn < 2; turn++)
        {
            TURN {
                if (timburr)
                {
                    MOVE(playerLeft, MOVE_ICE_PUNCH, target: opponentRight);
                    MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight);
                }
                else
                {
                    MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight);
                    MOVE(playerRight, MOVE_DOUBLE_EDGE, target: opponentRight);
                }
                // Same actions as the opening baseline, repeated unchanged.
                // The AI does not receive the player's pending input buffers.
            }
        }
    } THEN {
        Test_MgbaPrintf("Allen follow-through timburr=%d turns=%d LAST=%S/%S activeParty=%d/%d species=%d/%d HP=%d/%d playerHP=%d/%d playerSleep=%d/%d ParasSlotStatus=%d Reflect=%d",
            timburr, gBattleResults.battleTurnCounter,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, playerLeft->status1 & STATUS1_SLEEP, playerRight->status1 & STATUS1_SLEEP,
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_STATUS),
            (gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT) != 0);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("Allen follow-through party=%d species=%d HP=%d/%d status=%d",
                slot, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MAX_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_STATUS));
        // Do not lock active slots: actual replacement decisions are allowed.
        // Root's decision-time trace distinguishes both executed turns from
        // the next decision the harness may already have selected in THEN.
    }
}

DOUBLE_BATTLE_TEST("EC live review Allen follow-through: native Web Spore then focus fire without freeze")
{
    GIVEN {
        AllenProbePlayer(SPECIES_TIMBURR, &sAllenProbeSlowTimburr);
        AllenProbePlayer(SPECIES_TIMBURR, &sAllenProbeSlowTimburr);
        AuthoredOpponent(TRAINER_ALLEN, 0, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE,
                secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE,
                secondaryEffect: FALSE);
            MOVE(opponentLeft, MOVE_ELECTROWEB, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentRight, MOVE_SPORE, target: playerRight, hit: TRUE);
        }
        TURN {
            MOVE(playerLeft, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE,
                secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE,
                secondaryEffect: FALSE);
            MOVE(opponentLeft, MOVE_VOLT_TACKLE, target: playerLeft, hit: TRUE, criticalHit: FALSE,
                secondaryEffect: FALSE);
            MOVE(opponentRight, MOVE_LEECH_LIFE, target: playerLeft, hit: TRUE, criticalHit: FALSE);
            // Explicit hypothesis for the incomplete baseline: Paras falls
            // after its selected Leech Life. SCENE/THEN below reject this if
            // the missing event was something else, rather than blessing it.
            SEND_OUT(opponentRight, 2);
        }
        // This explicitly isolates a no-freeze/no-paralysis branch; it is not
        // a guarantee against Ice Punch or a claim about optimal AI selection.
        // Native damage, HP, recoil, draining, and sleep countdown are intact.
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEECH_LIFE, opponentRight);
        HP_BAR(opponentRight, hp: 0);
    } THEN {
        struct Pokemon *paras = &gParties[B_TRAINER_OPPONENT_A][1];
        Test_MgbaPrintf("Allen fixed follow-through PikaLAST=%S playerLAST=%S/%S activeParty=%d/%d species=%d/%d HP=%d/%d playerHP=%d/%d sleep=%d/%d ParasPartyHP=%d ParasPartyStatus=%d speedStage=%d/%d",
            GetMoveName(gLastMoves[B_BATTLER_1]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            playerLeft->status1 & STATUS1_SLEEP, playerRight->status1 & STATUS1_SLEEP,
            GetMonData(paras, MON_DATA_HP), GetMonData(paras, MON_DATA_STATUS),
            playerLeft->statStages[STAT_SPEED], playerRight->statStages[STAT_SPEED]);
        EXPECT_EQ(GetMonData(paras, MON_DATA_HP), 0);
        EXPECT_EQ(GetMonData(paras, MON_DATA_STATUS) & STATUS1_FREEZE, 0);
        // Observe the actual outcome before claiming that this line improves
        // the two-turn position over the unrestricted baseline.
    }
}
