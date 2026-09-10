// Temporary actual-party spread-support diagnostic; include after Tiana helper.
#line 27000 "work/tiana_gleam_probe.inc"

static void TianaGleamBoard(void)
{
    static const struct EmeraldChampionsBattleSet set = {
        .moves = {MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF, MOVE_FAKE_OUT, MOVE_PROTECT},
        .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
        .evs = {4, 252, 0, 0, 0, 252},
    };
    TianaProbePlayer(SPECIES_MIENFOO, &set);
    TianaProbePlayer(SPECIES_MIENFOO, &set);
    AuthoredOpponent(TRAINER_TIANA, 0, FALSE);
    struct Pokemon *players = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
    struct Pokemon *enemies = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&enemies[1], MON_DATA_MOVE2), MOVE_DAZZLING_GLEAM);
    for (u32 slot = 0; slot < 2; slot++)
    {
        EXPECT_GT(GetMonData(&players[slot], MON_DATA_SPEED), GetMonData(&enemies[1], MON_DATA_SPEED));
        Test_MgbaPrintf("TIANA_GLEAM_INPUT slot=%d playerHP=%d Atk=%d Def=%d SpD=%d Speed=%d enemySpecies=%d HP=%d SpA=%d Speed=%d",
            slot, GetMonData(&players[slot], MON_DATA_MAX_HP), GetMonData(&players[slot], MON_DATA_ATK),
            GetMonData(&players[slot], MON_DATA_DEF), GetMonData(&players[slot], MON_DATA_SPDEF),
            GetMonData(&players[slot], MON_DATA_SPEED), GetMonData(&enemies[slot], MON_DATA_SPECIES),
            GetMonData(&enemies[slot], MON_DATA_MAX_HP), GetMonData(&enemies[slot], MON_DATA_SPATK),
            GetMonData(&enemies[slot], MON_DATA_SPEED));
    }
    // Actual Route101 Mienfoo, native level14/IV31/66-point preparation.
    // Inner Focus legitimately denies Fake Out; no fabricated damage or HP.
}

AI_DOUBLE_BATTLE_TEST("EC live review Tiana Gleam: actual AI versus two Inner Focus attackers")
{
    GIVEN {
        TianaGleamBoard();
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, secondaryEffect: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("TIANA_GLEAM_AI LAST=%S/%S species=%d/%d HP=%d/%d item=%d playerHP=%d/%d unburden=%d",
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            opponentRight->item, playerLeft->hp, playerRight->hp, opponentRight->volatiles.unburdenActive);
        // No desired-move assertion. Native execution plus decision-time trace
        // decides whether the dual-target opportunity actually exists.
    }
}

DOUBLE_BATTLE_TEST("EC live review Tiana Gleam: native Helping Hand spread versus single target")
{
    bool32 spread;
    PARAMETRIZE { spread = FALSE; }
    PARAMETRIZE { spread = TRUE; }
    GIVEN {
        TianaGleamBoard();
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentLeft, MOVE_HELPING_HAND, target: opponentRight);
            if (spread)
                MOVE(opponentRight, MOVE_DAZZLING_GLEAM, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else
                MOVE(opponentRight, MOVE_DRAINING_KISS, target: playerRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentLeft);
        // The identical native speeds can tie in either order. Observe the
        // resulting HP rather than imposing an actor-specific animation order.
        ANIMATION(ANIM_TYPE_MOVE, spread ? MOVE_DAZZLING_GLEAM : MOVE_DRAINING_KISS, opponentRight);
    } THEN {
        Test_MgbaPrintf("TIANA_GLEAM_FIXED spread=%d LAST=%S/%S HP=%d/%d playerHP=%d/%d item=%d unburden=%d",
            spread, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            opponentRight->item, opponentRight->volatiles.unburdenActive);
        // Compare observed outcomes first. A successful scripted execution is
        // neither an unrestricted choice nor a predeclared optimality result.
    }
}
