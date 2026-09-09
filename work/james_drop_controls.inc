// Temporary native mechanics, included after james_lead_probe.inc.
// Commands are explicit counterfactuals, NOT unrestricted AI evidence.
// All James records and native stats remain unchanged; the Skitter case
// permutes Venipede into Surskit's active slot without deleting either mon.
#line 23000 "work/james_drop_controls.inc"

static void JamesDropOpponent(bool32 venipede)
{
    AuthoredOpponent(TRAINER_JAMES_1, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SURSKIT);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_DEWPIDER);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_VENIPEDE);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_KRICKETUNE);
    if (venipede)
    {
        struct Pokemon saved = party[0];
        party[0] = party[2];
        party[2] = saved;
    }
}

DOUBLE_BATTLE_TEST("EC live review James Lunge: landed native Attack drop reduces later Seed Bomb", s16 lungeDamage, s16 replyDamage)
{
    enum Item item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_COVERT_CLOAK; }
    GIVEN {
        // Ferroseed is in first-visit Woods grass. Its native Brave Speed10
        // follows actual Dewpider15; native Iron Barbs remains active in both
        // cases. Cloak blocks the secondary drop, not the hit/contact damage.
        const struct EmeraldChampionsBattleSet ferro = {
            .moves = {MOVE_SEED_BOMB, MOVE_PROTECT, MOVE_GYRO_BALL, MOVE_LEECH_SEED},
            .item = item, .nature = NATURE_BRAVE, .ability = ABILITY_IRON_BARBS,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        JamesLeadPlayer(SPECIES_FERROSEED, &ferro);
        JamesLeadPlayer(SPECIES_PACHIRISU, &sJamesPachirisu);
        JamesDropOpponent(FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_GT(GetMonData(&party[1], MON_DATA_SPEED), GetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][0], MON_DATA_SPEED));
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SEED_BOMB, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_RAIN_DANCE);
            MOVE(opponentRight, MOVE_LUNGE, target: playerLeft, hit: TRUE, criticalHit: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LUNGE, opponentRight);
        HP_BAR(playerLeft, captureDamage: &results[i].lungeDamage);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SEED_BOMB, playerLeft);
        HP_BAR(opponentRight, captureDamage: &results[i].replyDamage);
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - (item == ITEM_NONE));
        EXPECT_GT(playerLeft->hp, 0);
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_GT(results[i].lungeDamage, 0);
        EXPECT_GT(results[i].replyDamage, 0);
        Test_MgbaPrintf("JAMES_LUNGE_CONTROL cloak=%d hitDamage=%d replyDamage=%d foeAtkStage=%d DewHP=%d",
            item == ITEM_COVERT_CLOAK, results[i].lungeDamage, results[i].replyDamage,
            playerLeft->statStages[STAT_ATK], opponentRight->hp);
    } FINALLY {
        EXPECT_LT(results[0].replyDamage, results[1].replyDamage);
    }
}

DOUBLE_BATTLE_TEST("EC live review James Skitter Smack: landed versus missed drop changes later Psychic", s16 skitterDamage, s16 replyDamage)
{
    bool32 hit;
    PARAMETRIZE { hit = TRUE; }
    PARAMETRIZE { hit = FALSE; }
    GIVEN {
        JamesLeadPlayer(SPECIES_NATU, &sJamesNatu);
        JamesLeadPlayer(SPECIES_PACHIRISU, &sJamesPachirisu);
        JamesDropOpponent(TRUE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_GT(GetMonData(&party[0], MON_DATA_SPEED), GetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][0], MON_DATA_SPEED));
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_SKITTER_SMACK, target: playerLeft, hit: hit, criticalHit: FALSE);
            MOVE(opponentRight, MOVE_LIQUIDATION, target: playerRight);
        }
    } SCENE {
        if (hit)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SKITTER_SMACK, opponentLeft);
            HP_BAR(playerLeft, captureDamage: &results[i].skitterDamage);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, playerLeft);
        HP_BAR(opponentRight, captureDamage: &results[i].replyDamage);
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - hit);
        EXPECT_GT(playerLeft->hp, 0);
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_GT(results[i].replyDamage, 0);
        if (hit)
            EXPECT_GT(results[i].skitterDamage, 0);
        else
            EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
        Test_MgbaPrintf("JAMES_SKITTER_CONTROL hit=%d hitDamage=%d replyDamage=%d foeSpAStage=%d DewHP=%d",
            hit, results[i].skitterDamage, results[i].replyDamage, playerLeft->statStages[STAT_SPATK], opponentRight->hp);
    } FINALLY {
        EXPECT_LT(results[0].replyDamage, results[1].replyDamage);
    }
}
