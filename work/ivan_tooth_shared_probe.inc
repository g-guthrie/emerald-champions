// Generic decision candidate, not Ivan's authored moves or generated stats.
// Promote expectations only after positive and disabled-consumer native runs.
#line 60000 "work/ivan_tooth_shared_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Ivan Tooth: attack category follows the timing of Knock Off")
{
    u32 removerSpeed;
    bool32 orb;
    PARAMETRIZE { removerSpeed = 60; orb = FALSE; }
    PARAMETRIZE { removerSpeed = 5; orb = FALSE; }
    PARAMETRIZE { removerSpeed = 60; orb = TRUE; }
    PARAMETRIZE { removerSpeed = 5; orb = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY
            | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_MIENFOO) {
            Level(14); HP(100); MaxHP(100); Attack(20); Defense(orb ? 100 : 20);
            SpAttack(20); SpDefense(20); Speed(removerSpeed);
            Ability(ABILITY_INNER_FOCUS); Item(ITEM_NONE); Moves(MOVE_KNOCK_OFF);
        }
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(100); MaxHP(100); Attack(20); Defense(60);
            SpAttack(20); SpDefense(60); Speed(20);
            Ability(ABILITY_VOLT_ABSORB); Item(ITEM_NONE); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_TENTACOOL) {
            Level(12); HP(100); MaxHP(100); Attack(20); Defense(200);
            SpAttack(20); SpDefense(50); Speed(10);
            Ability(ABILITY_LIQUID_OOZE); Item(ITEM_NONE); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_CLAMPERL) {
            Level(orb ? 50 : 12); HP(100); MaxHP(100); Attack(orb ? 120 : 80); Defense(50);
            SpAttack(60); SpDefense(40); Speed(30);
            Ability(ABILITY_SHELL_ARMOR); Item(orb ? ITEM_LIFE_ORB : ITEM_DEEP_SEA_TOOTH);
            Moves(orb ? MOVE_STRENGTH : MOVE_ICE_BEAM, orb ? MOVE_SEISMIC_TOSS : MOVE_WATERFALL);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_GT(playerLeft->hp, 0);
        if (!orb)
            EXPECT_EQ(gLastMoves[B_BATTLER_3], removerSpeed == 60 ? MOVE_WATERFALL : MOVE_ICE_BEAM);
        else
            EXPECT_EQ(gLastMoves[B_BATTLER_3], removerSpeed == 60 ? MOVE_SEISMIC_TOSS : MOVE_STRENGTH);
        Test_MgbaPrintf("IVAN_TOOTH_DECISION orb=%d removerSpeed=%d ClamMove=%S ClamHP=%d item=%d MienHP=%d MienLAST=%S MienStatus=%d",
            orb, removerSpeed, GetMoveName(gLastMoves[B_BATTLER_3]), opponentRight->hp,
            opponentRight->item, playerLeft->hp, GetMoveName(gLastMoves[B_BATTLER_0]), playerLeft->status1);
        // Faster Knock Off removes the offensive item before Clamperl acts.
        // Slower Knock Off does not weaken that turn's preceding attack.
        // Physical Waterfall is the unaffected intermediate-damage option.
        // No initial move assertion predetermines whether the fixture exposes
        // a distinct policy change; root checks its negative independently.
    }
}
