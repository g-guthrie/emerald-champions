// Generic native threshold diagnostic, not Clark's authored moves or stats.
#line 59000 "work/clark_defeatist_probe.inc"

static void ClarkDefeatistBoundaryBoard(u32 actorHp, bool32 durableTarget)
{
    PLAYER(SPECIES_EEVEE) {
        Level(14); HP(1); MaxHP(100); Attack(60); Defense(20); SpAttack(20); SpDefense(50); Speed(40);
        Ability(ABILITY_ADAPTABILITY); Item(ITEM_NONE); Moves(MOVE_QUICK_ATTACK);
    }
    PLAYER(SPECIES_MUNCHLAX) {
        Level(14); HP(durableTarget ? 100 : 20); MaxHP(durableTarget ? 100 : 20);
        Attack(20); Defense(50); SpAttack(20); SpDefense(50); Speed(5);
        Ability(ABILITY_THICK_FAT); Item(ITEM_NONE); Moves(MOVE_STOCKPILE);
    }
    OPPONENT(SPECIES_ARCHEN) {
        Level(12); HP(actorHp); MaxHP(100); Attack(60); Defense(30); SpAttack(20); SpDefense(30); Speed(70);
        Ability(ABILITY_DEFEATIST); Item(ITEM_NONE); Moves(MOVE_ACROBATICS, MOVE_QUICK_ATTACK);
    }
    OPPONENT(SPECIES_PACHIRISU) {
        Level(12); HP(100); MaxHP(100); Attack(20); Defense(50); SpAttack(20); SpDefense(50); Speed(10);
        Ability(ABILITY_VOLT_ABSORB); Item(ITEM_NONE); Moves(MOVE_CELEBRATE);
    }
}

DOUBLE_BATTLE_TEST("EC live review Clark Defeatist: existing and newly crossed thresholds reduce native Acrobatics")
{
    u32 actorHp;
    PARAMETRIZE { actorHp = 40; }
    PARAMETRIZE { actorHp = 60; }
    PARAMETRIZE { actorHp = 100; }
    s16 priorityDamage, acrobaticsDamage;
    GIVEN {
        ClarkDefeatistBoundaryBoard(actorHp, TRUE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_QUICK_ATTACK, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_STOCKPILE);
            MOVE(opponentLeft, MOVE_ACROBATICS, target: playerRight, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, playerLeft);
        HP_BAR(opponentLeft, captureDamage: &priorityDamage);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ACROBATICS, opponentLeft);
        HP_BAR(playerRight, captureDamage: &acrobaticsDamage);
    } THEN {
        EXPECT_GT(opponentLeft->hp, 0);
        EXPECT_EQ(opponentLeft->item, ITEM_NONE);
        Test_MgbaPrintf("CLARK_DEFEATIST_NATIVE initialHP=%d priorityDamage=%d finalHP=%d active=%d AcroDamage=%d",
            actorHp, priorityDamage, opponentLeft->hp,
            opponentLeft->hp <= opponentLeft->maxHP / 2, acrobaticsDamage);
        // The native observations establish the threshold; no fabricated
        // damage or assumption about exact native rounding is asserted.
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Clark Defeatist: prevent priority threshold crossing or take healthy Acrobatics KO")
{
    u32 actorHp;
    PARAMETRIZE { actorHp = 40; }
    PARAMETRIZE { actorHp = 60; }
    PARAMETRIZE { actorHp = 100; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY
            | AI_FLAG_DOUBLE_BATTLE);
        ClarkDefeatistBoundaryBoard(actorHp, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_QUICK_ATTACK, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_STOCKPILE);
        }
    } THEN {
        Test_MgbaPrintf("CLARK_DEFEATIST_DECISION initialHP=%d moves=%S/%S ownHP=%d/%d foeHP=%d/%d active=%d faints=%d/%d",
            actorHp, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            opponentLeft->hp <= opponentLeft->maxHP / 2,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        EXPECT_EQ(gLastMoves[B_BATTLER_1], actorHp == 100 ? MOVE_ACROBATICS : MOVE_QUICK_ATTACK);
        EXPECT_EQ(opponentLeft->hp, actorHp == 100 ? 89 : actorHp);
        EXPECT_EQ(actorHp == 100 ? playerRight->hp : playerLeft->hp, 0);
        // Initial observations only. Quick Attack can remove the known
        // priority threat; Acrobatics can exploit a retained full-power KO.
        // This is an explicit generic menu, not a change to actual Archen.
    }
}
