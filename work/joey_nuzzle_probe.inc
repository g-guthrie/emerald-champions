// Native faster Inner Focus opponent; trainer moves remain unrestricted.
#line 56000 "work/joey_nuzzle_probe.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Joey Nuzzle: fast Fighting pressure and partner follow-through")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    PARAMETRIZE { turns = 3; }
    GIVEN {
        JoeyPlayer(SPECIES_MIENFOO, &sJoeyMienfoo);
        JoeyPlayer(SPECIES_PACHIRISU, &sJoeyPachirisu);
        AuthoredOpponent(TRAINER_JOEY, 0, FALSE);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("JOEY_NUZZLE turns=%d active=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S status=%d/%d foeSpe=%d/%d ownSpe=%d/%d faints=%d/%d",
            turns, opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            playerLeft->status1, playerRight->status1,
            GetBattlerTotalSpeedStat(B_BATTLER_0, playerLeft->ability, GetBattlerHoldEffect(B_BATTLER_0)),
            GetBattlerTotalSpeedStat(B_BATTLER_2, playerRight->ability, GetBattlerHoldEffect(B_BATTLER_2)),
            GetBattlerTotalSpeedStat(B_BATTLER_1, opponentLeft->ability, GetBattlerHoldEffect(B_BATTLER_1)),
            GetBattlerTotalSpeedStat(B_BATTLER_3, opponentRight->ability, GetBattlerHoldEffect(B_BATTLER_3)),
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
    }
}
