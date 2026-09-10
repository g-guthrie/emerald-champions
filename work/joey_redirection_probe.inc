// Native actual reserves against neutral pressure, no forced support/setup.
AI_DOUBLE_BATTLE_TEST("EC live review Joey redirection: neutral pressure and setup follow-through")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    PARAMETRIZE { turns = 3; }
    GIVEN {
        JoeyPlayer(SPECIES_EEVEE, &sJoeyEevee);
        JoeyPlayer(SPECIES_LOTAD, &sJoeyLotad);
        JoeyDrumOpponent(TRUE);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("JOEY_REDIRECT turns=%d own=%d/%d foes=%d/%d moves=%S/%S stage=%d berry=%d", turns,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentRight->statStages[STAT_ATK], opponentRight->item);
    }
}
