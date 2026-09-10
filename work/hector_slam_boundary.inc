// Generic one-turn decision boundary. Neither the stats nor Charizard's
// presence claim to reproduce Hector's actual team or early player access.
#line 59000 "work/hector_slam_boundary.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Hector Slam: probabilistic slowdown enables the slower partner")
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
        PLAYER(SPECIES_CHARIZARD) {
            Level(14); HP(70); MaxHP(100); Attack(20); Defense(80);
            SpAttack(100); SpDefense(30); Speed(45);
            Ability(ABILITY_BLAZE); Item(control == 2 ? ITEM_COVERT_CLOAK : ITEM_NONE);
            Moves(MOVE_FLAMETHROWER);
        }
        PLAYER(SPECIES_PACHIRISU) {
            Level(14); HP(100); MaxHP(100); Attack(20); Defense(100);
            SpAttack(20); SpDefense(100); Speed(20);
            Ability(ABILITY_VOLT_ABSORB); Item(ITEM_NONE); Moves(MOVE_PROTECT);
        }
        OPPONENT(SPECIES_DUNSPARCE) {
            Level(12); HP(100); MaxHP(100); Attack(80); Defense(60);
            SpAttack(20); SpDefense(100); Speed(60);
            Ability(control == 1 ? ABILITY_RUN_AWAY : ABILITY_SERENE_GRACE); Item(ITEM_NONE);
            Moves(MOVE_BODY_SLAM, MOVE_ROCK_SLIDE, MOVE_ROOST, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_SEVIPER) {
            Level(12); HP(35); MaxHP(70); Attack(20); Defense(25);
            SpAttack(150); SpDefense(25); Speed(30);
            Ability(ABILITY_INFILTRATOR); Item(ITEM_NONE);
            Moves(MOVE_SLUDGE_BOMB, MOVE_FLAMETHROWER, MOVE_GIGA_DRAIN, MOVE_GLARE);
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(gLastMoves[B_BATTLER_1], control == 2 ? MOVE_ROCK_SLIDE : MOVE_BODY_SLAM);
        u32 chance = CalcSecondaryEffectChance(B_BATTLER_1,
            control == 1 ? ABILITY_RUN_AWAY : ABILITY_SERENE_GRACE,
            GetMoveAdditionalEffectById(MOVE_BODY_SLAM, 0));
        EXPECT_EQ(chance, control == 1 ? 30 : 60);
        if (control == 2)
            EXPECT_EQ(playerLeft->status1 & STATUS1_PARALYSIS, 0);
        Test_MgbaPrintf("HECTOR_SLAM_BOUNDARY control=%d nativeChance=%d moves=%S/%S HP=%d/%d foes=%d/%d status=%d faints=%d/%d",
            control, chance, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp, playerLeft->status1,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        // Record actual decisions without prescribing a random proc or reading
        // pending player commands. Only retain a chosen-move assertion after
        // root proves a distinct disabled-consumer failure on this boundary.
    }
}
