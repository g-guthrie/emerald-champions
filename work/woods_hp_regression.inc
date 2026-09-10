// Draft isolated regression, not an authored trainer/team lock. Synthetic
// stats remove reserve, weather, status and two-target chip tradeoffs. The
// only parameter changes whether the public Thunderbolt user acts before
// Water Spout. Water Absorb (not Storm Drain) removes the other damage target
// without redirecting Scald. Opposing choices remain entirely unrestricted.
#line 15000 "work/woods_hp_regression.inc"

AI_DOUBLE_BATTLE_TEST("EC expert pair: HP-powered damage follows public faster chip")
{
    u32 attackerSpeed;
    PARAMETRIZE { attackerSpeed = 10; }
    PARAMETRIZE { attackerSpeed = 35; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(100); MaxHP(100); SpAttack(17); SpDefense(50);
            Speed(attackerSpeed); Ability(ABILITY_VOLT_ABSORB);
            Moves(MOVE_THUNDERBOLT);
        }
        PLAYER(SPECIES_MANTINE) {
            Level(14); HP(100); MaxHP(100); Speed(5);
            Ability(ABILITY_WATER_ABSORB); Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_ORANGURU) {
            Level(12); HP(100); MaxHP(100); SpDefense(200); Speed(5);
            Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WAILMER) {
            Level(12); HP(88); MaxHP(88); SpAttack(62); SpDefense(17);
            Speed(23); Ability(ABILITY_OBLIVIOUS);
            Moves(MOVE_WATER_SPOUT, MOVE_SCALD);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentRight, attackerSpeed < 23 ? MOVE_WATER_SPOUT : MOVE_SCALD);
        }
    } THEN {
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_LT(opponentRight->hp, opponentRight->maxHP);
        EXPECT_GT(playerLeft->hp, 0);
        EXPECT_LT(playerLeft->hp, playerLeft->maxHP);
        EXPECT_EQ(playerRight->hp, playerRight->maxHP);
        Test_MgbaPrintf("HP_POWER_REGRESSION attackerSpeed=%d move=%S WailHP=%d targetHP=%d",
            attackerSpeed, GetMoveName(gLastMoves[B_BATTLER_3]), opponentRight->hp, playerLeft->hp);
    }
}
