// Temporary candidate for a narrow shared regression, not a trainer lock.
// Literal inputs below reproduce the executed native Charm board's four
// active members. No authored catalogue, generation helper or plan is used.
#line 59000 "work/gina_mia_charm_shared_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Gina shared Charm: timely primary Attack drop protects its user", s16 replyDamage)
{
    PARAMETRIZE {}
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY
            | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_TIMBURR) {
            Level(14); HP(81); MaxHP(81); Attack(49); Defense(24);
            SpAttack(14); SpDefense(38); Speed(19);
            Nature(NATURE_CAREFUL); Ability(ABILITY_IRON_FIST); Item(ITEM_EVIOLITE);
            Moves(MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT);
        }
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(77); MaxHP(77); Attack(18); Defense(30);
            SpAttack(21); SpDefense(72); Speed(35);
            Nature(NATURE_CALM); Ability(ABILITY_VOLT_ABSORB); Item(ITEM_SITRUS_BERRY);
            Moves(MOVE_SUPER_FANG, MOVE_THUNDERBOLT, MOVE_FOLLOW_ME, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_PLUSLE) {
            Level(12); HP(42); MaxHP(42); Attack(18); Defense(18);
            SpAttack(65); SpDefense(26); Speed(71);
            Nature(NATURE_TIMID); Ability(ABILITY_PLUS); Item(ITEM_FOCUS_SASH);
            Moves(MOVE_THUNDERBOLT, MOVE_NUZZLE, MOVE_HELPING_HAND, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_MINUN) {
            Level(12); HP(42); MaxHP(42); Attack(16); Defense(20);
            SpAttack(58); SpDefense(33); Speed(71);
            Nature(NATURE_TIMID); Ability(ABILITY_MINUS); Item(ITEM_SITRUS_BERRY);
            Moves(MOVE_THUNDERBOLT, MOVE_CHARM, MOVE_ENCORE, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, WITH_RNG(RNG_PARALYSIS, FALSE));
            MOVE(playerRight, MOVE_PROTECT);
        }
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, WITH_RNG(RNG_PARALYSIS, FALSE));
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentRight, MOVE_CHARM, target: playerLeft);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARM, opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAIN_PUNCH, playerLeft);
        HP_BAR(opponentRight, captureDamage: &results[i].replyDamage);
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 2);
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_GT(results[i].replyDamage, 0);
        Test_MgbaPrintf("GINA_SHARED_CHARM turns=%d ownHP=%d/%d foes=%d/%d LAST=%S/%S replyDamage=%d foeAtk=%d faints=%d/%d",
            gBattleResults.battleTurnCounter, opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            results[i].replyDamage, playerLeft->statStages[STAT_ATK],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
    }
}
