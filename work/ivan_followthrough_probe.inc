// Temporary actual Ivan follow-through. Include after ivan_lead_probe.inc.
// Full native menus and trainer reserve choices remain unrestricted. The first
// two turns previously left both player leads alive; turns three/four are new
// observations. A newly observed player faint requires an explicit SEND_OUT
// timeline revision, not a dynamic WHEN branch or a claim of an engine freeze.
#line 56000 "work/ivan_followthrough_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Ivan followthrough: four turns with four or six native players")
{
    u32 board, playerCount;
    PARAMETRIZE { board = 0; playerCount = 4; }
    PARAMETRIZE { board = 0; playerCount = 6; }
    PARAMETRIZE { board = 1; playerCount = 4; }
    PARAMETRIZE { board = 1; playerCount = 6; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet fastPachi = {
            .moves = {MOVE_SUPER_FANG, MOVE_THUNDERBOLT, MOVE_FOLLOW_ME, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_CALM, .ability = ABILITY_VOLT_ABSORB,
            .statPoints = {2, 0, 0, 0, 32, 32},
        };
        static const struct EmeraldChampionsBattleSet fastTimburr = {
            .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
            .statPoints = {2, 0, 0, 0, 32, 32},
        };
        static const struct EmeraldChampionsBattleSet electrike = {
            .moves = {MOVE_FLAMETHROWER, MOVE_THUNDERBOLT, MOVE_SNARL, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_LIGHTNING_ROD,
            .statPoints = {32, 0, 18, 16, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet shellos = {
            .moves = {MOVE_EARTH_POWER, MOVE_SCALD, MOVE_RECOVER, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_STORM_DRAIN,
            .statPoints = {32, 0, 0, 32, 2, 0},
        };
        IvanLeadPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_PACHIRISU,
            board == 0 ? &sIvanEevee : &fastPachi);
        IvanLeadPlayer(SPECIES_TIMBURR, board == 0 ? &sIvanTimburr : &fastTimburr);
        IvanLeadPlayer(SPECIES_LOTAD, &sIvanLotad);
        IvanLeadPlayer(SPECIES_MIENFOO, &sIvanMienfoo);
        if (playerCount == 6)
        {
            IvanLeadPlayer(SPECIES_ELECTRIKE, &electrike);
            IvanLeadPlayer(SPECIES_SHELLOS, &shellos);
        }
        AuthoredOpponent(TRAINER_IVAN, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_TENTACOOL);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_CLAMPERL);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_CHEWTLE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_CHINCHOU);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? playerCount : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("IVAN_FOLLOW_INPUT board=%d count=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d moves=%d/%d/%d/%d",
                    board, playerCount, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon),
                    GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                    GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
            }
    } WHEN {
        // Commands target battlefield slots, not a presumed trainer species:
        // any trainer switch/replacement is chosen by the actual native AI.
        for (u32 turn = 0; turn < 4; turn++) TURN {
            if (board == 0)
            {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                // The native four-turn ledger reaches Eevee's faint here.
                // Lotad is the first healthy player reserve, not an AI command.
                if (turn == 3)
                    SEND_OUT(playerLeft, 2);
            }
            else
            {
                MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                if (turn == 3)
                    SEND_OUT(playerLeft, 2);
            }
        }
    } THEN {
        Test_MgbaPrintf("IVAN_FOLLOW_AFTER board=%d count=%d turns=%d active=%d/%d slots=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S ownAtk=%d/%d ownDef=%d/%d ownSpA=%d/%d ownSpD=%d/%d ownSpeed=%d/%d faints=%d/%d",
            board, playerCount, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->statStages[STAT_ATK], opponentRight->statStages[STAT_ATK],
            opponentLeft->statStages[STAT_DEF], opponentRight->statStages[STAT_DEF],
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            opponentLeft->statStages[STAT_SPDEF], opponentRight->statStages[STAT_SPDEF],
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? playerCount : 4); slot++)
            {
                struct Pokemon *mon = &gParties[owner][slot];
                Test_MgbaPrintf("IVAN_FOLLOW_PARTY board=%d count=%d owner=%d slot=%d species=%d HP=%d/%d status=%d item=%d",
                    board, playerCount, owner, slot, GetMonData(mon, MON_DATA_SPECIES),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_MAX_HP),
                    GetMonData(mon, MON_DATA_STATUS), GetMonData(mon, MON_DATA_HELD_ITEM));
            }
        // This is observation, not an assertion that setup must be selected.
        // Root's decision-time logger records each unrestricted native choice;
        // gLastMoves here records only the most recently executed move.
    }
}
