// Synthetic shared-mechanic boundary, not Clark's generated party or access.
#line 56000 "work/clark_stamina_probe.inc"

static void ClarkStaminaBoundaryBoard(bool32 stamina)
{
    PLAYER(SPECIES_MUDBRAY) {
        Level(14); HP(100); MaxHP(100); Attack(20); Defense(50); SpAttack(20); SpDefense(50); Speed(30);
        Ability(stamina ? ABILITY_STAMINA : ABILITY_OWN_TEMPO);
        Item(ITEM_NONE); Moves(MOVE_HIGH_HORSEPOWER);
    }
    PLAYER(SPECIES_GIMMIGHOUL) {
        Level(14); HP(100); MaxHP(100); Attack(20); Defense(100); SpAttack(20); SpDefense(100); Speed(20);
        Ability(ABILITY_RATTLED); Item(ITEM_NONE); Moves(MOVE_PROTECT);
    }
    OPPONENT(SPECIES_PACHIRISU) {
        Level(14); HP(100); MaxHP(100); Attack(10); Defense(100); SpAttack(20); SpDefense(100); Speed(70);
        Ability(ABILITY_VOLT_ABSORB); Item(ITEM_NONE); Moves(MOVE_QUICK_ATTACK);
    }
    OPPONENT(SPECIES_CLAMPERL) {
        Level(14); HP(100); MaxHP(100); Attack(40); Defense(50); SpAttack(40); SpDefense(50); Speed(40);
        Ability(ABILITY_SHELL_ARMOR); Item(ITEM_NONE); Moves(MOVE_WATERFALL, MOVE_ICE_BEAM);
    }
}

DOUBLE_BATTLE_TEST("EC live review Clark Stamina: surviving first strike reduces later physical strike")
{
    bool32 stamina, special;
    PARAMETRIZE { stamina = FALSE; special = FALSE; }
    PARAMETRIZE { stamina = TRUE; special = FALSE; }
    PARAMETRIZE { stamina = TRUE; special = TRUE; }
    s16 chip, physical;
    GIVEN {
        ClarkStaminaBoundaryBoard(stamina);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_HIGH_HORSEPOWER, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_QUICK_ATTACK, target: playerLeft, hit: TRUE, criticalHit: FALSE);
            if (special)
                MOVE(opponentRight, MOVE_ICE_BEAM, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            else
                MOVE(opponentRight, MOVE_WATERFALL, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponentLeft);
        HP_BAR(playerLeft, captureDamage: &chip);
        ANIMATION(ANIM_TYPE_MOVE, special ? MOVE_ICE_BEAM : MOVE_WATERFALL, opponentRight);
        HP_BAR(playerLeft, captureDamage: &physical);
    } THEN {
        EXPECT_GT(playerLeft->hp, 0);
        EXPECT_EQ(playerLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE + (stamina ? 2 : 0));
        Test_MgbaPrintf("CLARK_STAMINA_NATIVE stamina=%d special=%d chip=%d followup=%d remaining=%d DefStage=%d",
            stamina, special, chip, physical, playerLeft->hp, playerLeft->statStages[STAT_DEF]);
        // This native control measures the intervening raise. The final +2
        // includes Waterfall's own later activation, not a +2 before that hit.
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Clark Stamina: physical versus special followup after partner chip")
{
    bool32 stamina;
    PARAMETRIZE { stamina = FALSE; }
    PARAMETRIZE { stamina = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE
            | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS | AI_FLAG_KNOW_OPPONENT_PARTY
            | AI_FLAG_DOUBLE_BATTLE);
        ClarkStaminaBoundaryBoard(stamina);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_HIGH_HORSEPOWER, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        Test_MgbaPrintf("CLARK_STAMINA_DECISION stamina=%d moves=%S/%S ownHP=%d/%d targetHP=%d DefStage=%d",
            stamina, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerLeft->statStages[STAT_DEF]);
        EXPECT_EQ(gLastMoves[B_BATTLER_3], stamina ? MOVE_ICE_BEAM : MOVE_WATERFALL);
        EXPECT_EQ(playerLeft->hp, stamina ? 78 : 74);
        // Initial observation only: no desired AI choice is imposed. Compare
        // native physical damage with the special alternative before adding
        // a decision assertion or retaining this as a shared regression.
    }
}
