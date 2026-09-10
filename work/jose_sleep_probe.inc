// Generic decision boundary; not Jose's authored party or player access.
#line 55000 "work/jose_sleep_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Jose sleep: fast powder prevents retaliation unless Grass or Goggles blocks it")
{
    u32 control;
    PARAMETRIZE { control = 0; }
    PARAMETRIZE { control = 1; }
    PARAMETRIZE { control = 2; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY
            | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(control == 2 ? SPECIES_BULBASAUR : SPECIES_MUNCHLAX) {
            Level(14); HP(100); MaxHP(100); Attack(160); Defense(120); SpAttack(20); SpDefense(100); Speed(60);
            Ability(control == 2 ? ABILITY_OVERGROW : ABILITY_THICK_FAT);
            Item(control == 1 ? ITEM_SAFETY_GOGGLES : ITEM_NONE); Moves(MOVE_BODY_SLAM);
        }
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(100); MaxHP(100); Attack(20); Defense(100); SpAttack(20); SpDefense(100); Speed(20);
            Ability(ABILITY_VOLT_ABSORB); Item(ITEM_NONE); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_VENONAT) {
            Level(12); HP(70); MaxHP(70); Attack(20); Defense(25); SpAttack(20); SpDefense(40); Speed(70);
            Ability(ABILITY_COMPOUND_EYES); Item(ITEM_NONE);
            Moves(MOVE_RAGE_POWDER, MOVE_SLEEP_POWDER, MOVE_STRUGGLE_BUG, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_KRICKETUNE) {
            Level(12); HP(45); MaxHP(45); Attack(80); Defense(25); SpAttack(20); SpDefense(30); Speed(50);
            Ability(ABILITY_TECHNICIAN); Item(ITEM_LIFE_ORB);
            Moves(MOVE_BUG_BITE, MOVE_AERIAL_ACE, MOVE_TAUNT, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BODY_SLAM, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            if (control == 0)
                EXPECT_MOVE(opponentLeft, MOVE_SLEEP_POWDER, target: playerLeft);
            else
                EXPECT_MOVE(opponentLeft, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_0][1], 97);
        if (control)
            EXPECT_EQ(playerLeft->status1 & STATUS1_SLEEP, 0);
        Test_MgbaPrintf("JOSE_SLEEP_BOUNDARY control=%d moves=%S/%S ownHP=%d/%d foes=%d/%d sleep=%d accuracy=%d faint=%d/%d",
            control, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            playerLeft->status1 & STATUS1_SLEEP, gAiLogicData->moveAccuracy[B_BATTLER_1][B_BATTLER_0][1],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        // Observed native choices, not a forced command or guaranteed proc.
        // The partner remains unrestricted: its unrelated Taunt decision is
        // not the regression contract. Root verifies the disabled whitelist.
    }
}
