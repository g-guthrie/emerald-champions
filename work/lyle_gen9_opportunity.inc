#line 55000 "work/lyle_gen9_opportunity.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Lyle pressure opportunity: actual fresh lead against low-threat Lotad pair")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet lotad = {
            .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
            .evs = {252, 0, 136, 120, 0, 0},
        };
        // Two legal native Route102 Lotad are a controlled player matchup,
        // not a change to the trainer's species distribution or its party.
        LyleLeadPlayer(SPECIES_LOTAD, &lotad);
        LyleLeadPlayer(SPECIES_LOTAD, &lotad);
        AuthoredOpponent(TRAINER_LYLE, 0, FALSE);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            // No opponent move or entry is forced. First Impression is a
            // possible useful fresh-entry attack, not a required assertion.
        }
    } THEN {
        Test_MgbaPrintf("LYLE_PRESSURE_OPPORTUNITY turns=%d nativeTurns=%d species=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S activeParty=%d/%d ownFaints=%d foeFaints=%d",
            turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("LYLE_PRESSURE_OPPORTUNITY_PARTY turns=%d slot=%d species=%d HP=%d",
                turns, slot, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP));
    }
}
