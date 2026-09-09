// Temporary candidate for the demonstrated non-Flying Roost omission.
// Explicit stats match the executed native reserve fixture; no authored ID,
// SETUP bonus, party-generation helper, or forced opponent commands.
#line 58000 "work/gina_mia_shared_probe.inc"
#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

AI_DOUBLE_BATTLE_TEST("EC live review Gina shared Roost: non-Flying priority recovery", s16 firstDamage, s16 healing)
{
    PARAMETRIZE {}
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO
            | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(77); MaxHP(77); Attack(18); Defense(66);
            SpAttack(21); SpDefense(36); Speed(35); Nature(NATURE_BOLD);
            Ability(ABILITY_VOLT_ABSORB); Item(ITEM_EVIOLITE);
            Moves(MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT);
        }
        PLAYER(SPECIES_LOTAD) {
            Level(14); HP(71); MaxHP(71); Attack(15); Defense(53);
            SpAttack(20); SpDefense(25); Speed(17); Nature(NATURE_BOLD);
            Ability(ABILITY_RAIN_DISH); Item(ITEM_EVIOLITE);
            Moves(MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_REST, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_VOLBEAT) {
            Level(12); HP(25); MaxHP(73); Attack(23); Defense(28);
            SpAttack(57); SpDefense(29); Speed(29); Nature(NATURE_MODEST);
            Ability(ABILITY_PRANKSTER); Item(ITEM_LUM_BERRY);
            Moves(MOVE_TAIL_GLOW, MOVE_ROOST, MOVE_BUG_BUZZ, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_ILLUMISE) {
            Level(12); HP(43); MaxHP(43); Attack(18); Defense(26);
            SpAttack(63); SpDefense(29); Speed(61); Nature(NATURE_MODEST);
            Ability(ABILITY_PRANKSTER); Item(ITEM_LEFTOVERS);
            Moves(MOVE_DAZZLING_GLEAM, MOVE_BUG_BUZZ, MOVE_ENCORE, MOVE_HELPING_HAND);
        }
        // The actual reserve fixture's only other teammates were fainted.
        // Omitting those dead records preserves the available action menu.
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
        }
        TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentLeft, MOVE_ROOST);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERBOLT, playerLeft);
        HP_BAR(opponentLeft, captureDamage: &results[i].firstDamage);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROOST, opponentLeft);
        HP_BAR(opponentLeft, captureDamage: &results[i].healing);
    } THEN {
        EXPECT_GT(results[i].firstDamage, 0);
        EXPECT_EQ(results[i].healing, -36);
        EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_ROOST);
        EXPECT_GT(opponentLeft->hp, 25 - results[i].firstDamage);
        Test_MgbaPrintf("GINA_SHARED_ROOST firstDamage=%d healing=%d HP=%d/%d foes=%d/%d LAST=%S/%S faints=%d/%d",
            results[i].firstDamage, results[i].healing, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, GetMoveName(gLastMoves[B_BATTLER_1]),
            GetMoveName(gLastMoves[B_BATTLER_3]), gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        // Candidate only: retain after native positive/disabled-repair runs
        // demonstrate distinct regression value, not merely test execution.
    }
}
