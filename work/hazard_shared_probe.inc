// Shared regression for the player's Marc hazard report.

AI_DOUBLE_BATTLE_TEST("EC hazard targeting: damaging hazard moves remain attacks at capacity")
{
    enum Move move = MOVE_NONE;
    PARAMETRIZE { move = MOVE_STONE_AXE; }
    PARAMETRIZE { move = MOVE_CEASELESS_EDGE; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Defense(300); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Defense(300); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET);
        // Native STAB/Sharpness makes the hazard attack valuable for damage,
        // unlike Normal Smeargle, which can correctly prefer STAB Tackle.
        OPPONENT(move == MOVE_STONE_AXE ? SPECIES_KLEAVOR : SPECIES_SAMUROTT_HISUI) { HP(500); MaxHP(500); Attack(100); Ability(ABILITY_SHARPNESS); Moves(move, MOVE_TACKLE); }
        OPPONENT(SPECIES_SMEARGLE) { HP(500); MaxHP(500); Ability(ABILITY_OWN_TEMPO); Moves(MOVE_SPLASH); }
    } WHEN {
        for (u32 turn = 0; turn < 4; turn++) TURN {
            EXPECT_MOVE(opponentLeft, move);
        }
    } THEN {
        EXPECT(!AreAnyHazardsOnSide(B_SIDE_OPPONENT));
        EXPECT(AI_IsHazardAtCapacity(B_SIDE_PLAYER, move));
        EXPECT_EQ(opponentLeft->pp[0], GetMovePP(move) - 4);
        EXPECT(playerLeft->hp < playerLeft->maxHP || playerRight->hp < playerRight->maxHP);
    }
}


AI_DOUBLE_BATTLE_TEST("EC hazard targeting: partners do not duplicate a full hazard")
{
    enum Move hazard = MOVE_NONE;
    u32 layers = 0;
    PARAMETRIZE { hazard = MOVE_STEALTH_ROCK; layers = 1; }
    PARAMETRIZE { hazard = MOVE_STICKY_WEB; layers = 1; }
    PARAMETRIZE { hazard = MOVE_SPIKES; layers = 3; }
    PARAMETRIZE { hazard = MOVE_TOXIC_SPIKES; layers = 2; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SMEARGLE) { HP(500); MaxHP(500); Attack(10); Ability(ABILITY_OWN_TEMPO); Moves(hazard, MOVE_TACKLE); }
        OPPONENT(SPECIES_SMEARGLE) { HP(500); MaxHP(500); Attack(10); Ability(ABILITY_OWN_TEMPO); Moves(hazard, MOVE_TACKLE); }
    } WHEN {
        TURN {}
        TURN {}
        TURN {}
    } THEN {
        EXPECT(!AreAnyHazardsOnSide(B_SIDE_OPPONENT));
        EXPECT(AI_IsHazardAtCapacity(B_SIDE_PLAYER, hazard));
        EXPECT_EQ(opponentLeft->pp[0] + opponentRight->pp[0], 2 * GetMovePP(hazard) - layers);
    }
}

AI_DOUBLE_BATTLE_TEST("EC hazard targeting: correct opposing side, valid layers, no saturated repeat")
{
    enum Move hazard = MOVE_NONE;
    u32 layers = 0, flank = 0;
    for (u32 f = 0; f < 2; f++) {
        PARAMETRIZE { hazard = MOVE_STEALTH_ROCK; layers = 1; flank = f; }
        PARAMETRIZE { hazard = MOVE_STICKY_WEB; layers = 1; flank = f; }
        PARAMETRIZE { hazard = MOVE_SPIKES; layers = 3; flank = f; }
        PARAMETRIZE { hazard = MOVE_TOXIC_SPIKES; layers = 2; flank = f; }
    }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(80); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(70); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(90); }
        OPPONENT(SPECIES_SMEARGLE) { HP(500); MaxHP(500); Attack(10); Speed(60); Ability(ABILITY_OWN_TEMPO); Moves(flank == 0 ? hazard : MOVE_SPLASH, flank == 0 ? MOVE_TACKLE : MOVE_NONE); }
        OPPONENT(SPECIES_SMEARGLE) { HP(500); MaxHP(500); Attack(10); Speed(50); Ability(ABILITY_OWN_TEMPO); Moves(flank == 1 ? hazard : MOVE_SPLASH, flank == 1 ? MOVE_TACKLE : MOVE_NONE); }
    } WHEN {
        for (u32 turn = 0; turn < 4; turn++) TURN {
            if (flank == 0) {
                if (turn < layers) EXPECT_MOVE(opponentLeft, hazard, target: playerLeft);
                else NOT_EXPECT_MOVE(opponentLeft, hazard);
            } else {
                if (turn < layers) EXPECT_MOVE(opponentRight, hazard, target: playerRight);
                else NOT_EXPECT_MOVE(opponentRight, hazard);
            }
        }
    } THEN {
        EXPECT(!AreAnyHazardsOnSide(B_SIDE_OPPONENT));
        EXPECT(AI_IsHazardAtCapacity(B_SIDE_PLAYER, hazard));
        EXPECT_EQ((flank == 0 ? opponentLeft : opponentRight)->pp[0], GetMovePP(hazard) - layers);
        EXPECT_EQ((u32)gSideTimers[B_SIDE_PLAYER].spikesAmount, hazard == MOVE_SPIKES ? 3 : 0);
        EXPECT_EQ((u32)gSideTimers[B_SIDE_PLAYER].toxicSpikesAmount, hazard == MOVE_TOXIC_SPIKES ? 2 : 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC hazard targeting: Magic Bounce discourages hazards on either foe flank")
{
    u32 bounceFlank = 0;
    PARAMETRIZE { bounceFlank = 0; }
    PARAMETRIZE { bounceFlank = 1; }
    GIVEN {
        AI_FLAGS(EC_EXPERT_FLAGS);
        PLAYER(SPECIES_NATU) { HP(500); MaxHP(500); Ability(bounceFlank == 0 ? ABILITY_MAGIC_BOUNCE : ABILITY_SYNCHRONIZE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_NATU) { HP(500); MaxHP(500); Ability(bounceFlank == 1 ? ABILITY_MAGIC_BOUNCE : ABILITY_SYNCHRONIZE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_STONJOURNER) { HP(500); MaxHP(500); Attack(20); Moves(MOVE_STEALTH_ROCK, MOVE_TACKLE); }
        OPPONENT(SPECIES_SMEARGLE) { HP(500); MaxHP(500); Ability(ABILITY_OWN_TEMPO); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_STEALTH_ROCK); }
        TURN { NOT_EXPECT_MOVE(opponentLeft, MOVE_STEALTH_ROCK); }
    } THEN {
        EXPECT(!AreAnyHazardsOnSide(B_SIDE_PLAYER));
        EXPECT(!AreAnyHazardsOnSide(B_SIDE_OPPONENT));
    }
}
