// Temporary actual-reserve diagnostic. Include after rick_ai_probe.inc.
// Reuses RickProbePlayer's level14/IV31 native preparation and stat generation.
#line 11000 "work/rick_reserve_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Rick reserves: physical walls and reciprocal Fire pressure")
{
    u32 scenario;
    PARAMETRIZE { scenario = 0; } // Two early physical walls.
    PARAMETRIZE { scenario = 1; } // Early Fire pressure, without Intimidate.
    PARAMETRIZE { scenario = 2; } // Same native players, real Intimidate entry.
    GIVEN {
        static const u8 wall[NUM_STATS] = {252, 0, 252, 0, 4, 0};
        static const u8 firePoints[NUM_STATS] = {252, 0, 4, 252, 0, 0};
        static const u16 pachiMoves[MAX_MON_MOVES] = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT};
        static const u16 lotadMoves[MAX_MON_MOVES] = {MOVE_GIGA_DRAIN, MOVE_RAIN_DANCE, MOVE_SYNTHESIS, MOVE_PROTECT};
        static const u16 seedMoves[MAX_MON_MOVES] = {MOVE_GIGA_DRAIN, MOVE_LEECH_SEED, MOVE_SYNTHESIS, MOVE_PROTECT};
        static const u16 fireMoves[MAX_MON_MOVES] = {MOVE_FIRE_BLAST, MOVE_FLAMETHROWER, MOVE_WILL_O_WISP, MOVE_PROTECT};
        if (scenario == 0)
        {
            RickProbePlayer(SPECIES_PACHIRISU, NATURE_BOLD, ABILITY_VOLT_ABSORB, ITEM_SITRUS_BERRY, wall, pachiMoves);
            RickProbePlayer(SPECIES_LOTAD, NATURE_BOLD, ABILITY_RAIN_DISH, ITEM_EVIOLITE, wall, lotadMoves);
        }
        else
        {
            RickProbePlayer(SPECIES_SEEDOT, NATURE_BOLD, ABILITY_CHLOROPHYLL, ITEM_EVIOLITE, wall, seedMoves);
            RickProbePlayer(SPECIES_GROWLITHE, NATURE_MODEST,
                scenario == 1 ? ABILITY_FLASH_FIRE : ABILITY_INTIMIDATE, ITEM_EVIOLITE, firePoints, fireMoves);
        }
        AuthoredOpponent(TRAINER_RICK, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        u16 zero = 0;
        SetMonData(&party[0], MON_DATA_HP, &zero);
        SetMonData(&party[1], MON_DATA_HP, &zero);
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 2;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 3;
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_KARRABLAST);
        EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_NO_GUARD);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_DEWPIDER);
        EXPECT_EQ(GetMonAbility(&party[3]), ABILITY_WATER_BUBBLE);
        // Only the two defeated lead slots are changed. Reserve stats, moves,
        // items and abilities remain the current compiled Rick loadouts. These
        // boards can be rerun unchanged if a later authored revision is approved.
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = owner == B_TRAINER_PLAYER ? 0 : 2;
                 slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("RICK_RESERVE_INPUT scenario=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                    scenario, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
            }
    } WHEN {
        TURN {
            if (scenario == 0)
            {
                // The earlier baseline sampled full paralysis. Suppress the
                // secondary here to inspect the chosen reserve attack's
                // execution, not to claim paralysis cannot stop this team.
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentLeft);
            }
            else
            {
                MOVE(playerLeft, MOVE_GIGA_DRAIN, target: opponentRight);
                // No Guard benefits the opponent's Fire Blast too. Do not
                // assume it only makes Karrablast's Megahorn reliable.
                MOVE(playerRight, MOVE_FIRE_BLAST, target: opponentLeft);
            }
        }
    } THEN {
        EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_1], 2);
        EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_3], 3);
        Test_MgbaPrintf("RICK_RESERVE_AFTER scenario=%d LAST=%S/%S liveSpecies=%d/%d HP=%d/%d playerHP=%d/%d AttackStage=%d/%d playerAttack=%d/%d",
            scenario, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, opponentLeft->statStages[STAT_ATK], opponentRight->statStages[STAT_ATK],
            playerLeft->statStages[STAT_ATK], playerRight->statStages[STAT_ATK]);
        // Diagnostic execution is not a verdict on the current physical set or
        // an unimplemented special alternative. Native decision-time output
        // establishes choices; THEN may already contain next-turn caches.
    }
}
