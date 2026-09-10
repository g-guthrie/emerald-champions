// Temporary forced native mechanics; after woods_grunt_lead_probe.inc.
// Commands are controlled comparisons, not unrestricted AI-choice evidence.
#line 12000 "work/woods_grunt_spout_probe.inc"

DOUBLE_BATTLE_TEST("EC live review Woods grunt Spout: faster chip native Sitrus and Scald control", s16 chip, s16 berryHeal, s16 waterDamage)
{
    u32 board;
    bool32 spout;
    PARAMETRIZE { board = 0; spout = TRUE; }
    PARAMETRIZE { board = 0; spout = FALSE; }
    PARAMETRIZE { board = 1; spout = TRUE; }
    PARAMETRIZE { board = 1; spout = FALSE; }
    PARAMETRIZE { board = 2; spout = TRUE; }
    PARAMETRIZE { board = 2; spout = FALSE; }
    GIVEN {
        WoodsLeadPlayer(SPECIES_PACHIRISU, &sWoodsPachirisu);
        WoodsLeadPlayer(SPECIES_EEVEE, &sWoodsEevee);
        AuthoredOpponent(TRAINER_GRUNT_PETALBURG_WOODS, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_CARVANHA);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_WAILMER);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_GRIMER);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_POOCHYENA);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_MOVE1), MOVE_WATER_SPOUT);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_MOVE2), MOVE_SCALD);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_HELD_ITEM), ITEM_SITRUS_BERRY);
        struct Pokemon *pachi = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][0];
        EXPECT_GT(GetMonData(pachi, MON_DATA_SPEED), GetMonData(&party[1], MON_DATA_SPEED));
        Test_MgbaPrintf("WOODS_SPOUT_INPUT board=%d spout=%d WailHP=%d/%d level=%d SpA=%d SpD=%d Speed=%d PachiSpA=%d Speed=%d",
            board, spout, GetMonData(&party[1], MON_DATA_HP), GetMonData(&party[1], MON_DATA_MAX_HP),
            GetMonData(&party[1], MON_DATA_LEVEL), GetMonData(&party[1], MON_DATA_SPATK),
            GetMonData(&party[1], MON_DATA_SPDEF), GetMonData(&party[1], MON_DATA_SPEED),
            GetMonData(pachi, MON_DATA_SPATK), GetMonData(pachi, MON_DATA_SPEED));
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_PROTECT);
            // A harmless real attack into Carvanha's Protect lets Eevee take
            // either Water move. Neither target absorbs Water. Spout retains
            // its native spread modifier; Scald is naturally single-target.
            MOVE(playerRight, MOVE_QUICK_ATTACK, target: opponentLeft);
            if (board == 0)
                MOVE(playerLeft, MOVE_PROTECT);
            else
                MOVE(playerLeft, board == 1 ? MOVE_SUPER_FANG : MOVE_THUNDERBOLT,
                    target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (spout)
                MOVE(opponentRight, MOVE_WATER_SPOUT, hit: TRUE, criticalHit: FALSE);
            else
                MOVE(opponentRight, MOVE_SCALD, target: playerRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (board > 0)
        {
            ANIMATION(ANIM_TYPE_MOVE, board == 1 ? MOVE_SUPER_FANG : MOVE_THUNDERBOLT, playerLeft);
            HP_BAR(opponentRight, captureDamage: &results[i].chip);
            if (board == 1)
                HP_BAR(opponentRight, captureDamage: &results[i].berryHeal);
        }
        ANIMATION(ANIM_TYPE_MOVE, spout ? MOVE_WATER_SPOUT : MOVE_SCALD, opponentRight);
        HP_BAR(playerRight, captureDamage: &results[i].waterDamage);
    } THEN {
        EXPECT_EQ(opponentRight->species, SPECIES_WAILMER);
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_EQ(opponentRight->status1, STATUS1_NONE);
        EXPECT_EQ(gLastMoves[B_BATTLER_3], spout ? MOVE_WATER_SPOUT : MOVE_SCALD);
        EXPECT_GT(results[i].waterDamage, 0);
        // No action after Pachirisu changes Wailmer's HP. Thus its current HP
        // is also its actual HP when it executed the Water move. Healing is
        // native readback, not a forced assumed Thunderbolt threshold.
        s32 healed = opponentRight->hp + results[i].chip - opponentRight->maxHP;
        if (board == 0)
        {
            EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
            EXPECT_EQ(opponentRight->item, ITEM_SITRUS_BERRY);
        }
        else
        {
            EXPECT_GT(results[i].chip, 0);
            EXPECT_LT(opponentRight->hp, opponentRight->maxHP);
            EXPECT_EQ(healed, opponentRight->item == ITEM_NONE ? opponentRight->maxHP / 4 : 0);
        }
        if (board == 1)
        {
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
            EXPECT_EQ(healed, -results[i].berryHeal);
        }
        Test_MgbaPrintf("WOODS_SPOUT_AFTER board=%d spout=%d chip=%d heal=%d WailAttackHP=%d/%d item=%d waterDamage=%d PachiHP=%d EeveeHP=%d",
            board, spout, results[i].chip, healed, opponentRight->hp, opponentRight->maxHP,
            opponentRight->item, results[i].waterDamage, playerLeft->hp, playerRight->hp);
    } FINALLY {
        EXPECT_LT(results[2].waterDamage, results[0].waterDamage);
        EXPECT_LT(results[4].waterDamage, results[0].waterDamage);
        EXPECT_EQ(results[3].waterDamage, results[1].waterDamage);
        EXPECT_EQ(results[5].waterDamage, results[1].waterDamage);
        // Do not assume chip makes Scald superior: compare native results,
        // including Spout's damage to the other living target, when deciding.
    }
}
