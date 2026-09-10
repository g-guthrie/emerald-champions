// Temporary native Dustox coverage/support comparison; after JosePlayer.
#line 57000 "work/jose_support_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Jose support: Bug Buzz versus Struggle Bug with actual partners")
{
    bool32 struggle, fast;
    u32 turns;
    PARAMETRIZE { struggle = FALSE; fast = FALSE; turns = 1; }
    PARAMETRIZE { struggle = TRUE; fast = FALSE; turns = 1; }
    PARAMETRIZE { struggle = FALSE; fast = FALSE; turns = 2; }
    PARAMETRIZE { struggle = TRUE; fast = FALSE; turns = 2; }
    PARAMETRIZE { struggle = FALSE; fast = TRUE; turns = 1; }
    PARAMETRIZE { struggle = TRUE; fast = TRUE; turns = 1; }
    PARAMETRIZE { struggle = FALSE; fast = TRUE; turns = 2; }
    PARAMETRIZE { struggle = TRUE; fast = TRUE; turns = 2; }
    PARAMETRIZE { struggle = FALSE; fast = TRUE; turns = 3; }
    PARAMETRIZE { struggle = TRUE; fast = TRUE; turns = 3; }
    GIVEN {
        JosePlayer(fast ? SPECIES_ELECTRIKE : SPECIES_PACHIRISU,
            fast ? &sJoseElectrike : &sJosePachirisu);
        JosePlayer(fast ? SPECIES_LOTAD : SPECIES_TIMBURR,
            fast ? &sJoseLotad : &sJoseTimburr);
        AuthoredOpponent(TRAINER_JOSE, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        const enum Species expected[] = {SPECIES_DUSTOX, SPECIES_BEAUTIFLY, SPECIES_KRICKETUNE, SPECIES_VENONAT};
        for (u32 slot = 0; slot < ARRAY_COUNT(expected); slot++)
            EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), expected[slot]);
        // Both attacks are legal in Dustox's actual mapped preparation list.
        // Normalize both arms if the candidate is later adopted. Only native
        // move slot two and its matching full PP change; no other record edits.
        u16 move = struggle ? MOVE_STRUGGLE_BUG : MOVE_BUG_BUZZ;
        u8 pp = CalculatePPWithBonus(move, GetMonData(&party[0], MON_DATA_PP_BONUSES), 1);
        SetMonData(&party[0], MON_DATA_MOVE2, &move);
        SetMonData(&party[0], MON_DATA_PP2, &pp);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("JOSE_SUPPORT_INPUT struggle=%d fast=%d turns=%d owner=%d slot=%d species=%d lv=%d HP=%d stats=%d/%d/%d/%d/%d item=%d ability=%d moves=%d/%d/%d/%d pp2=%d",
                    struggle, fast, turns, owner, slot,
                    GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon),
                    GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                    GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4), GetMonData(mon, MON_DATA_PP2));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (fast)
            {
                MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE);
            }
            else
            {
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
        // Same player lines/full menus as the lead baseline. Every trainer
        // move, target and reserve decision remains unrestricted.
    } THEN {
        Test_MgbaPrintf("JOSE_SUPPORT_AFTER struggle=%d fast=%d turns=%d native=%d active=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S foeMoves=%S/%S faints=%d/%d tailwind=%d ownSpA=%d/%d ownSpD=%d/%d ownSpe=%d/%d foeSpA=%d/%d statuses=%d/%d items=%d/%d",
            struggle, fast, turns, gBattleResults.battleTurnCounter,
            opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter,
            gSideTimers[B_SIDE_OPPONENT].tailwindTimer,
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            opponentLeft->statStages[STAT_SPDEF], opponentRight->statStages[STAT_SPDEF],
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED],
            playerLeft->statStages[STAT_SPATK], playerRight->statStages[STAT_SPATK],
            playerLeft->status1, playerRight->status1, opponentLeft->item, opponentRight->item);
        for (u32 slot = 0; slot < 4; slot++)
        {
            struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][slot];
            Test_MgbaPrintf("JOSE_SUPPORT_PARTY struggle=%d fast=%d turns=%d slot=%d species=%d HP=%d/%d item=%d",
                struggle, fast, turns, slot, GetMonData(mon, MON_DATA_SPECIES),
                GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_HELD_ITEM));
        }
        // Diagnostic execution is not an assertion of a better tactical line.
    }
}
