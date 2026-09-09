// Temporary actual E0019 reserves; include after marlene_lead_probe.inc.
#line 52000 "work/marlene_reserve_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Marlene reserves: physical Dark and Baltoy item comparison")
{
    bool32 dark, eviolite;
    u32 turns;
    PARAMETRIZE { dark = FALSE; eviolite = TRUE; turns = 1; }
    PARAMETRIZE { dark = FALSE; eviolite = TRUE; turns = 2; }
    PARAMETRIZE { dark = TRUE; eviolite = TRUE; turns = 1; }
    PARAMETRIZE { dark = TRUE; eviolite = TRUE; turns = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet poochyena = {
            .moves = {MOVE_CRUNCH, MOVE_SUCKER_PUNCH, MOVE_TAUNT, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_RUN_AWAY,
            .statPoints = {32, 32, 0, 0, 2, 0},
        };
        // Poochyena is current Route101 land access; all four moves are in
        // its mapped preparation array. Dark typing blocks Psychic, while
        // Baltoy still has neutral Earth Power / Ice Beam counterpressure.
        MarlenePlayer(SPECIES_PACHIRISU, &sMarlenePachirisu);
        MarlenePlayer(dark ? SPECIES_POOCHYENA : SPECIES_TIMBURR,
            dark ? &poochyena : &sMarleneTimburr);
        AuthoredOpponent(TRAINER_MARLENE, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SPOINK);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_SLOWPOKE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_BALTOY);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_GOTHITA);
        // Accepted compiled item, no loadout override in this final run.
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_HELD_ITEM), ITEM_EVIOLITE);
        u16 zero = 0;
        SetMonData(&party[0], MON_DATA_HP, &zero);
        SetMonData(&party[1], MON_DATA_HP, &zero);
        for (u32 slot = 0; slot < 2; slot++)
        {
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + 2];
            party[slot + 2] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("MARLENE_RESERVE_INPUT dark=%d eviolite=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d moves=%d/%d/%d/%d",
                    dark, eviolite, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon),
                    GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                    GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            // Split pressure: Thunderbolt targets Gothita, not immune Ground
            // Baltoy. Actual physical damage into Baltoy can distinguish the
            // proposed defense item; Super Fang there could not do so.
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (dark)
                MOVE(playerRight, MOVE_CRUNCH, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("MARLENE_RESERVE_AFTER dark=%d eviolite=%d turns=%d nativeTurns=%d active=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S room=%d timer=%d SpA=%d/%d Speed=%d/%d items=%d/%d faints=%d/%d",
            dark, eviolite, turns, gBattleResults.battleTurnCounter,
            opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            !!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM), gFieldTimers.trickRoomTimer,
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            GetBattlerTotalSpeedStat(B_BATTLER_1, opponentLeft->ability, GetBattlerHoldEffect(B_BATTLER_1)),
            GetBattlerTotalSpeedStat(B_BATTLER_3, opponentRight->ability, GetBattlerHoldEffect(B_BATTLER_3)),
            opponentLeft->item, opponentRight->item,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("MARLENE_RESERVE_PARTY dark=%d eviolite=%d turns=%d slot=%d species=%d HP=%d/%d item=%d",
                dark, eviolite, turns, slot,
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MAX_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HELD_ITEM));
        // No required Helping Hand, Protect, target choice, KO or victory.
    }
}
