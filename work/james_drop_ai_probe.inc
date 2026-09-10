// Temporary synthetic current-turn coordination observation, not James's
// roster or opening-access evidence. No opponent command is forced. Native
// The positive choices were observed natively; negative controls remain
// observations. Native Skitter Smack accuracy is not overridden.
#line 24000 "work/james_drop_ai_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review offensive drops: synthetic slower partner survival window")
{
    bool32 special;
    u32 boundary;
    PARAMETRIZE { special = FALSE; boundary = 0; }
    PARAMETRIZE { special = TRUE; boundary = 0; }
    PARAMETRIZE { special = FALSE; boundary = 1; }
    PARAMETRIZE { special = TRUE; boundary = 1; }
    PARAMETRIZE { special = FALSE; boundary = 2; }
    PARAMETRIZE { special = TRUE; boundary = 2; }
    GIVEN {
        AI_FLAGS(gTrainers[DIFFICULTY_NORMAL][TRAINER_JAMES_1].aiFlags | AI_FLAG_DOUBLE_BATTLE);
        // Against the 65HP/100Def/100SpD partner, these input stats aim for
        // unlowered Tackle68–81 or IceBeam72–85, versus lowered maxima55/57.
        // They are design calculations, not yet native measured assertions.
        PLAYER(SPECIES_TAUROS) {
            Level(50); HP(100); MaxHP(100); Attack(300); SpAttack(210);
            Defense(100); SpDefense(100); Speed(80); Ability(ABILITY_ANGER_POINT);
            Item(boundary == 1 ? ITEM_COVERT_CLOAK : ITEM_NONE);
            Moves(special ? MOVE_ICE_BEAM : MOVE_TACKLE);
        }
        PLAYER(SPECIES_CHANSEY) {
            Level(50); HP(300); MaxHP(300); Defense(300); SpDefense(300);
            Speed(10); Ability(ABILITY_NATURAL_CURE); Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_SMEARGLE) {
            Level(50); HP(300); MaxHP(300); Attack(60); SpAttack(10);
            Defense(300); SpDefense(300); Speed(boundary == 2 ? 60 : 100); Ability(ABILITY_OWN_TEMPO);
            // STAB Strength is stronger immediate neutral damage than either
            // Bug move, but cannot remove the100HP threat by itself.
            Moves(special ? MOVE_SKITTER_SMACK : MOVE_LUNGE, MOVE_STRENGTH);
        }
        OPPONENT(SPECIES_ORANGURU) {
            Level(50); HP(65); MaxHP(65); SpAttack(300);
            Defense(100); SpDefense(100); Speed(20); Ability(ABILITY_TELEPATHY);
            Moves(MOVE_PSYCHIC, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, special ? MOVE_ICE_BEAM : MOVE_TACKLE, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_CELEBRATE);
            if (boundary == 0)
            {
                EXPECT_MOVE(opponentLeft, special ? MOVE_SKITTER_SMACK : MOVE_LUNGE, target: playerLeft);
                EXPECT_MOVE(opponentRight, MOVE_PSYCHIC, target: playerLeft);
            }
        }
    } THEN {
        enum BattlerId dropper = opponentLeft - gBattleMons;
        enum BattlerId partner = opponentRight - gBattleMons;
        Test_MgbaPrintf("DROP_AI_OBSERVATION special=%d boundary=%d LAST=%S/%S HP=%d/%d foes=%d/%d enemyAtk=%d enemySpA=%d ownFaints=%d foeFaints=%d",
            special, boundary, GetMoveName(gLastMoves[dropper]), GetMoveName(gLastMoves[partner]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            playerLeft->statStages[STAT_ATK], playerLeft->statStages[STAT_SPATK],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        if (boundary == 0)
        {
            // Skitter's unforced 90% accuracy can miss. On this isolated
            // board, full target HP means that miss prevented Psychic from
            // executing; do not label the missing native payoff a success.
            if (!special || playerLeft->hp != playerLeft->maxHP)
            {
                EXPECT_EQ(playerLeft->hp, 0);
                EXPECT_GT(opponentRight->hp, 0);
                EXPECT_EQ(gBattleResults.opponentFaintCounter, 0);
            }
            else
            {
                Test_MgbaPrintf("DROP_AI_POSITIVE_MISSED: choices checked, landed payoff not exercised");
                EXPECT_EQ(opponentRight->hp, 0);
            }
        }
        // Post-turn KO cleanup clears damage caches. Do not report those
        // zeroes as retained decision-time damage evidence.
    }
}
