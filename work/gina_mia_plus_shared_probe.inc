// Candidate isolated decision regression, not campaign loadouts. Run positive
// and disabled-consumer negatives before promoting any move expectation.
#line 60000 "work/gina_mia_plus_shared_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Gina Plus: preserve the boost partner or take the boosted knockout")
{
    u32 partnerHp;
    PARAMETRIZE { partnerHp = 1; }
    PARAMETRIZE { partnerHp = 100; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY
            | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_EEVEE) {
            Level(14); HP(1); MaxHP(100); Attack(30); Defense(20);
            SpAttack(20); SpDefense(50); Speed(40);
            Ability(ABILITY_ADAPTABILITY); Moves(MOVE_QUICK_ATTACK);
        }
        PLAYER(SPECIES_MUNCHLAX) {
            Level(14); HP(25); MaxHP(25); Attack(20); Defense(50);
            SpAttack(20); SpDefense(50); Speed(5);
            Ability(ABILITY_THICK_FAT); Moves(MOVE_STOCKPILE);
        }
        OPPONENT(SPECIES_PLUSLE) {
            Level(12); HP(100); MaxHP(100); Attack(40); Defense(30);
            SpAttack(60); SpDefense(30); Speed(70);
            Ability(ABILITY_PLUS); Moves(MOVE_THUNDERBOLT, MOVE_QUICK_ATTACK);
        }
        OPPONENT(SPECIES_MINUN) {
            Level(12); HP(partnerHp); MaxHP(100); Attack(20); Defense(20);
            SpAttack(20); SpDefense(30); Speed(10);
            Ability(ABILITY_MINUS); Moves(MOVE_CELEBRATE);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_QUICK_ATTACK, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_STOCKPILE);
        }
    } THEN {
        EXPECT_EQ(gLastMoves[B_BATTLER_1], partnerHp == 1 ? MOVE_QUICK_ATTACK : MOVE_THUNDERBOLT);
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_EQ(partnerHp == 1 ? playerLeft->hp : playerRight->hp, 0);
        Test_MgbaPrintf("GINA_PLUS_DECISION initialPartnerHP=%d chosenExecuted=%S partnerHP=%d actorHP=%d enemyHP=%d/%d faints=%d/%d",
            partnerHp, GetMoveName(gLastMoves[B_BATTLER_1]), opponentRight->hp, opponentLeft->hp,
            playerLeft->hp, playerRight->hp, gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        // At 1 HP, a faster Quick Attack can remove the priority killer and
        // retain Minus. A slower Thunderbolt must lose its boost after that
        // killer acts; it must not receive a fictitious boosted Munchlax KO.
        // At 100 HP, the priority hit cannot remove the boost partner. These
        // are unforced initial observations, not predetermined optimum claims.
    }
}
