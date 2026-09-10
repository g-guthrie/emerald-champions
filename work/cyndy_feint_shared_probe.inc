// Synthetic decision boundary, not a substitute for the actual Cyndy probes.
// Expected actions require the root's positive and disabled-consumer runs.
#line 60000 "work/cyndy_feint_shared_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Cyndy Feint: coordinated guard breaking respects immunity, not Cloak")
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
        PLAYER(control == 2 ? SPECIES_DUSKULL : SPECIES_PACHIRISU) {
            Level(14); HP(77); MaxHP(77); Attack(18); Defense(66);
            SpAttack(21); SpDefense(36); Speed(35);
            Ability(control == 2 ? ABILITY_LEVITATE : ABILITY_VOLT_ABSORB);
            Item(control == 1 ? ITEM_COVERT_CLOAK : ITEM_NONE); Moves(MOVE_PROTECT);
        }
        PLAYER(control == 2 ? SPECIES_DUSKULL : SPECIES_MUNCHLAX) {
            Level(14); HP(98); MaxHP(98); Attack(33); Defense(57);
            SpAttack(20); SpDefense(35); Speed(7);
            Ability(control == 2 ? ABILITY_LEVITATE : ABILITY_THICK_FAT);
            Item(control == 1 ? ITEM_COVERT_CLOAK : ITEM_NONE); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_CRABRAWLER) {
            Level(12); HP(69); MaxHP(69); Attack(66); Defense(24);
            SpAttack(16); SpDefense(20); Speed(23);
            Ability(ABILITY_IRON_FIST); Item(ITEM_EVIOLITE);
            Moves(MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_THUNDER_PUNCH, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_CLOBBOPUS) {
            Level(12); HP(69); MaxHP(69); Attack(62); Defense(25);
            SpAttack(18); SpDefense(20); Speed(16);
            Ability(ABILITY_TECHNICIAN); Item(ITEM_LEFTOVERS);
            Moves(MOVE_POWER_UP_PUNCH, MOVE_FEINT, MOVE_TAUNT, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(gBattleResults.opponentFaintCounter, 0);
        EXPECT_EQ(gBattleResults.playerFaintCounter, 0);
        if (control != 2)
        {
            EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_DRAIN_PUNCH);
            EXPECT_EQ(gLastMoves[B_BATTLER_3], MOVE_FEINT);
            EXPECT_LT(playerLeft->hp + playerRight->hp, playerLeft->maxHP + playerRight->maxHP);
        }
        else
        {
            EXPECT(gLastMoves[B_BATTLER_3] != MOVE_FEINT);
            EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
            EXPECT_EQ(playerRight->hp, playerRight->maxHP);
        }
        Test_MgbaPrintf("CYNDY_FEINT_SHARED control=%d moves=%S/%S ownHP=%d/%d foeHP=%d/%d",
            control, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp);
    }
}
