// Temporary fixed-line counterfactual; include after calvin_hoothoot_probe.inc.
// Reuses its legal player sets and the production party-generation helper.
#line 10000 "work/calvin_spread_probe.inc"

DOUBLE_BATTLE_TEST("EC live review Calvin spread: actual Tailwind follow-up Air Slash versus Hyper Voice", s16 quickDamage; s16 braveDamage; s16 pachiDamage; s16 natuDamage; u16 hootHP; u16 natuHP)
{
    bool32 spread;
    PARAMETRIZE { spread = FALSE; }
    PARAMETRIZE { spread = TRUE; }
    GIVEN {
        CalvinProbePlayer(SPECIES_PACHIRISU, &sCalvinHootFastPachirisu);
        CalvinProbePlayer(SPECIES_NATU, &sCalvinHootFastNatu);
        AuthoredOpponent(TRAINER_CALVIN_1, 0, FALSE);
        u16 zero = 0;
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        SetMonData(&party[0], MON_DATA_HP, &zero);
        SetMonData(&party[1], MON_DATA_HP, &zero);
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 2;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 3;
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_HOOTHOOT);
        EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_TINTED_LENS);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_STARLY);
        // No stat/HP edits beyond the already-defeated original lead slots.
        // Hoothoot's revised point spread comes only from the compiled party.
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentLeft, MOVE_TAILWIND);
            MOVE(opponentRight, MOVE_QUICK_ATTACK, target: playerRight, hit: TRUE, criticalHit: FALSE);
        }
        TURN {
            MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentRight, MOVE_BRAVE_BIRD, target: playerRight, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentLeft, spread ? MOVE_HYPER_VOICE : MOVE_AIR_SLASH,
                target: playerLeft, hit: TRUE, criticalHit: FALSE);
        }
        // The native harness uses the same damage modifier in both parameters;
        // no fabricated damage or derived combat stat is injected. Turn one and
        // every turn-two command except Hoothoot's move are identical.
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponentRight);
        HP_BAR(playerRight, captureDamage: &results[i].quickDamage);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BRAVE_BIRD, opponentRight);
        HP_BAR(playerRight, captureDamage: &results[i].braveDamage);
        ANIMATION(ANIM_TYPE_MOVE, spread ? MOVE_HYPER_VOICE : MOVE_AIR_SLASH, opponentLeft);
        HP_BAR(playerLeft, captureDamage: &results[i].pachiDamage);
        if (spread)
            HP_BAR(playerRight, captureDamage: &results[i].natuDamage);
    } THEN {
        results[i].hootHP = opponentLeft->hp;
        results[i].natuHP = playerRight->hp;
        Test_MgbaPrintf("Calvin spread=%d QA=%d BraveBird=%d HootDamagePachi=%d HootDamageNatu=%d HootHP=%d/%d NatuHP=%d/%d PachiHP=%d HootSpA=%d Speed=%d/%d versus%d/%d LAST=%S/%S",
            spread, results[i].quickDamage, results[i].braveDamage,
            results[i].pachiDamage, spread ? results[i].natuDamage : 0,
            opponentLeft->hp, opponentLeft->maxHP, playerRight->hp, playerRight->maxHP,
            playerLeft->hp, opponentLeft->spAttack, opponentLeft->speed, opponentRight->speed,
            playerLeft->speed, playerRight->speed,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]));
        if (spread)
        {
            EXPECT_EQ(playerRight->hp, 0);
            EXPECT_GT(opponentLeft->hp, 0);
        }
        else
        {
            EXPECT_GT(playerRight->hp, 0);
            EXPECT_EQ(opponentLeft->hp, 0);
        }
    } FINALLY {
        EXPECT_EQ(results[0].quickDamage, results[1].quickDamage);
        EXPECT_EQ(results[0].braveDamage, results[1].braveDamage);
        EXPECT_GT(results[1].hootHP, results[0].hootHP);
        EXPECT_LT(results[1].natuHP, results[0].natuHP);
    }
}
