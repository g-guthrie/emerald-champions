// Native actual-party damage control, not an unrestricted trainer decision.
#line 58000 "work/cyndy_cut_probe.inc"
DOUBLE_BATTLE_TEST("EC live review Cyndy Cut: noncritical native hit into prepared Timburr", s16 damage)
{
    PARAMETRIZE {}
    GIVEN {
        CyndyPlayer(SPECIES_PACHIRISU, &sCyndyPachirisu);
        CyndyPlayer(SPECIES_TIMBURR, &sCyndyTimburr);
        AuthoredOpponent(TRAINER_CYNDY_1, 0, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_PSYCHO_CUT, target: playerRight, hit: TRUE, criticalHit: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHO_CUT, opponentRight);
        HP_BAR(playerRight, captureDamage: &results[i].damage);
    } THEN {
        Test_MgbaPrintf("CYNDY_CUT_NATIVE damage=%d targetHP=%d/%d MedHP=%d/%d AttackStage=%d",
            results[i].damage, playerRight->hp, playerRight->maxHP,
            opponentRight->hp, opponentRight->maxHP, opponentRight->statStages[STAT_ATK]);
    }
}
