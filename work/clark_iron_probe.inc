// Temporary synthetic shared decision boundary, adapted from retained Cotton
// Guard / Quiver Dance cases. NOT Clark's actual prepared party or stats.
// Only the incoming attacker's Speed changes; trainer choices remain free.
#line 55000 "work/clark_iron_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Clark Iron shared: timely physical defense versus faster incoming attack")
{
    bool32 fast;
    PARAMETRIZE { fast = FALSE; }
    PARAMETRIZE { fast = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO
            | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_LOTAD) {
            Level(14); HP(100); MaxHP(100); Attack(100); Defense(100);
            SpAttack(100); SpDefense(100); Speed(10);
            Ability(ABILITY_RAIN_DISH); Item(ITEM_EVIOLITE); Moves(MOVE_PROTECT);
        }
        PLAYER(SPECIES_MUNCHLAX) {
            Level(14); HP(98); MaxHP(98); Attack(80); Defense(22);
            SpAttack(39); SpDefense(37); Speed(fast ? 25 : 9);
            Nature(NATURE_QUIET); Ability(ABILITY_THICK_FAT); Item(ITEM_EVIOLITE);
            // Native-legal physical move; unlike Superpower it does not add
            // an Attack/Defense drop that would confound the timing comparison.
            Moves(MOVE_BRICK_BREAK);
        }
        OPPONENT(SPECIES_GIMMIGHOUL) {
            Level(12); HP(100); MaxHP(100); Attack(100); Defense(100);
            SpAttack(100); SpDefense(100); Speed(10);
            Ability(ABILITY_RATTLED); Item(ITEM_FOCUS_SASH); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_ROGGENROLA) {
            Level(12); HP(50); MaxHP(70); Attack(30); Defense(40);
            SpAttack(15); SpDefense(20); Speed(12); Nature(NATURE_IMPISH);
            Ability(ABILITY_STURDY); Item(ITEM_NONE);
            // Intentional two-move generic menu. Current injury disables
            // full-HP Sturdy, so survival cannot be credited to the ability.
            Moves(MOVE_IRON_DEFENSE, MOVE_BODY_PRESS);
        }
        Test_MgbaPrintf("CLARK_IRON_INPUT fast=%d synthetic=1 RogHP=50/70 Atk=30 Def=40 SpD=20 Speed=12 MunchHP=98 Atk=80 Def=22 SpD=37 Speed=%d",
            fast, fast ? 25 : 9);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_BRICK_BREAK, target: opponentRight,
                hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        EXPECT_EQ(gLastMoves[B_BATTLER_3], fast ? MOVE_BODY_PRESS : MOVE_IRON_DEFENSE);
        EXPECT_EQ(opponentRight->hp, fast ? 12 : 30);
        EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE + (fast ? 0 : 2));
        Test_MgbaPrintf("CLARK_IRON_END fast=%d synthetic=1 RogHP=%d/%d MunchHP=%d/%d DefStage=%d faints=%d/%d",
            fast, opponentRight->hp, opponentRight->maxHP, playerRight->hp, playerRight->maxHP,
            opponentRight->statStages[STAT_DEF], gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        Test_MgbaPrintf("CLARK_IRON_MOVES fast=%d own=%S/%S incoming=%S",
            fast, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_2]));
        // No desired move or outcome assertion before the native run. A useful
        // regression needs positive and disabled-cache negative evidence.
    }
}
