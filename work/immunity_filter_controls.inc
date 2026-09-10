// Temporary boundary controls for the demonstrated known-immunity repair.
// Synthetic stats isolate targeting/fallback, not an authored trainer strategy.
AI_DOUBLE_BATTLE_TEST("EC live review immunity filter: damageable recipient and all-immune legal fallback")
{
    bool32 allImmune;
    PARAMETRIZE { allImmune = FALSE; }
    PARAMETRIZE { allImmune = TRUE; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_POOCHYENA) {
            Level(50); HP(300); MaxHP(300); SpDefense(100); Speed(20);
            Ability(ABILITY_RUN_AWAY); Moves(MOVE_CELEBRATE);
        }
        PLAYER(allImmune ? SPECIES_POOCHYENA : SPECIES_CHANSEY) {
            Level(50); HP(300); MaxHP(300); SpDefense(100); Speed(20);
            Ability(ABILITY_RUN_AWAY); Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_GOTHITA) {
            Level(50); HP(300); MaxHP(300); SpAttack(100); Speed(40);
            Ability(ABILITY_COMPETITIVE); Moves(MOVE_PSYCHIC);
        }
        OPPONENT(SPECIES_CHANSEY) {
            Level(50); HP(300); MaxHP(300); Speed(30);
            Ability(ABILITY_NATURAL_CURE); Moves(MOVE_CELEBRATE);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_PSYCHIC);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
        if (allImmune)
            EXPECT_EQ(playerRight->hp, playerRight->maxHP);
        else
            EXPECT_LT(playerRight->hp, playerRight->maxHP);
        EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
    }
}
