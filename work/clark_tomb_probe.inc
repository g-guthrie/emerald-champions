// Synthetic shared Rock Tomb ordering boundary, not Clark's native party.
#line 57000 "work/clark_tomb_probe.inc"
AI_DOUBLE_BATTLE_TEST("EC live review Clark Rock Tomb: partner crossing respects secondary immunity")
{
    bool32 cloak;
    PARAMETRIZE { cloak = FALSE; }
    PARAMETRIZE { cloak = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_TAUROS) {
            Level(50); HP(100); MaxHP(100); Attack(300); SpAttack(210);
            Defense(100); SpDefense(100); Speed(80); Ability(ABILITY_ANGER_POINT);
            Item(cloak ? ITEM_COVERT_CLOAK : ITEM_NONE); Moves(MOVE_TACKLE);
        }
        PLAYER(SPECIES_CHANSEY) {
            Level(50); HP(300); MaxHP(300); Attack(100); SpAttack(100);
            Defense(300); SpDefense(300); Speed(10); Ability(ABILITY_NATURAL_CURE); Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_SMEARGLE) {
            Level(50); HP(300); MaxHP(300); Attack(60); SpAttack(10);
            Defense(300); SpDefense(300); Speed(100); Ability(ABILITY_OWN_TEMPO);
            Moves(MOVE_ROCK_TOMB, MOVE_STRENGTH);
        }
        OPPONENT(SPECIES_ORANGURU) {
            Level(50); HP(65); MaxHP(65); Attack(100); SpAttack(300);
            Defense(100); SpDefense(100); Speed(60); Ability(ABILITY_TELEPATHY);
            Moves(MOVE_PSYCHIC, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(gLastMoves[B_BATTLER_1], cloak ? MOVE_STRENGTH : MOVE_ROCK_TOMB);
        EXPECT_EQ(gLastMoves[B_BATTLER_3], cloak ? MOVE_PROTECT : MOVE_PSYCHIC);
        EXPECT_EQ(playerLeft->hp, cloak ? 72 : 0);
        EXPECT_GT(opponentRight->hp, 0);
        Test_MgbaPrintf("CLARK_TOMB cloak=%d moves=%S/%S ownHP=%d/%d targetHP=%d speedStage=%d",
            cloak, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerLeft->statStages[STAT_SPEED]);
    }
}
