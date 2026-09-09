// Candidate synthetic regression: no trainer catalogue or generation helper.
// Explicit stats preserve a demonstrated ordinary recoil decision boundary.
#line 50000 "work/winston_recoil_shared_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC expert pair: damage-based recoil distinguishes unsafe and surviving attacks")
{
    u32 injury;
    PARAMETRIZE { injury = 5; }
    PARAMETRIZE { injury = 6; }
    PARAMETRIZE { injury = 7; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY
            | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(77); MaxHP(77); Attack(18); Defense(66);
            SpAttack(21); SpDefense(36); Speed(35);
            Nature(NATURE_BOLD); Ability(ABILITY_VOLT_ABSORB); Item(ITEM_NONE);
            Moves(MOVE_FOLLOW_ME, MOVE_PROTECT);
        }
        PLAYER(SPECIES_TIMBURR) {
            Level(14); HP(81); MaxHP(81); Attack(31); Defense(61);
            SpAttack(14); SpDefense(21); Speed(19);
            Nature(NATURE_IMPISH); Ability(ABILITY_IRON_FIST); Item(ITEM_EVIOLITE);
            Moves(MOVE_BULK_UP, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_GIMMIGHOUL) {
            Level(12); HP(68); MaxHP(68); Attack(15); Defense(27);
            SpAttack(63); SpDefense(25); Speed(9);
            Nature(NATURE_QUIET); Ability(ABILITY_RATTLED); Item(ITEM_EVIOLITE);
            Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_WOOLOO) {
            Level(12); HP(injury); MaxHP(67); Attack(55); Defense(23);
            SpAttack(18); SpDefense(19); Speed(18);
            Nature(NATURE_BRAVE); Ability(ABILITY_FLUFFY); Item(ITEM_LEFTOVERS);
            Moves(MOVE_DOUBLE_EDGE, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_BULK_UP);
            EXPECT_MOVE(opponentRight, injury < 7 ? MOVE_PROTECT : MOVE_DOUBLE_EDGE);
        }
    } THEN {
        EXPECT_EQ(opponentRight->hp, injury < 7 ? injury + 4 : 6);
        EXPECT_EQ(opponentRight->item, ITEM_LEFTOVERS);
        EXPECT_EQ(gBattleResults.opponentFaintCounter, 0);
        if (injury < 7)
            EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
        else
            EXPECT_LT(playerLeft->hp, playerLeft->maxHP);
        // Protection retains 5/6HP and then Leftovers restores four. The
        // accepted 7HP attack pays actual five-HP recoil before that recovery.
        // This is a bounded damage-roll risk decision, not an assertion that
        // every possible hit from six HP must be fatal.
    }
}
