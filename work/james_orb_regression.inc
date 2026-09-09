// Candidate compact regression, independent of authored trainers and helpers.
// Explicit mechanical stats preserve the demonstrated recoil/berry trade.
// Expected choices below are intended contracts pending this group's native run.
#line 30000 "work/james_orb_regression.inc"

AI_DOUBLE_BATTLE_TEST("EC expert pair: Life Orb costs a hit and stolen Sitrus heals before recoil")
{
    u32 injury;
    bool32 berry;
    PARAMETRIZE { injury = 4; berry = FALSE; }
    PARAMETRIZE { injury = 5; berry = FALSE; }
    PARAMETRIZE { injury = 4; berry = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY
            | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(77); MaxHP(77); Attack(18); Defense(66);
            SpAttack(21); SpDefense(36); Speed(35);
            Nature(NATURE_BOLD); Ability(ABILITY_VOLT_ABSORB);
            Item(berry ? ITEM_SITRUS_BERRY : ITEM_NONE);
            Moves(MOVE_FOLLOW_ME, MOVE_PROTECT);
        }
        PLAYER(SPECIES_TIMBURR) {
            Level(14); HP(81); MaxHP(81); Attack(31); Defense(61);
            SpAttack(14); SpDefense(21); Speed(19);
            Nature(NATURE_IMPISH); Ability(ABILITY_IRON_FIST); Item(ITEM_EVIOLITE);
            Moves(MOVE_BULK_UP, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_DEWPIDER) {
            Level(12); HP(66); MaxHP(66); Attack(55); Defense(23);
            SpAttack(16); SpDefense(26); Speed(15);
            Nature(NATURE_ADAMANT); Ability(ABILITY_WATER_BUBBLE); Item(ITEM_SITRUS_BERRY);
            Moves(MOVE_LIQUIDATION, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_KRICKETUNE) {
            Level(12); HP(injury); MaxHP(46); Attack(61); Defense(20);
            SpAttack(18); SpDefense(20); Speed(61);
            Nature(NATURE_JOLLY); Ability(ABILITY_TECHNICIAN); Item(ITEM_LIFE_ORB);
            Moves(MOVE_BUG_BITE, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_BULK_UP);
            EXPECT_MOVE(opponentLeft, MOVE_LIQUIDATION);
            EXPECT_MOVE(opponentRight, injury == 4 && !berry ? MOVE_PROTECT : MOVE_BUG_BITE);
        }
    } THEN {
        EXPECT_EQ(opponentRight->hp, berry ? 11 : injury == 4 ? 4 : 1);
        EXPECT_EQ(opponentRight->item, ITEM_LIFE_ORB);
        EXPECT_EQ(playerLeft->item, ITEM_NONE);
        EXPECT_EQ(gBattleResults.opponentFaintCounter, 0);
        // No opposing attack or contact effect can explain these HP changes:
        // surviving recoil is useful chip; avoidable self-KO is not. A stolen
        // berry changes that same hit into healing before the one Orb payment.
    }
}
