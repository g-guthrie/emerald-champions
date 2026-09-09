// Temporary native Charm comparison plus a separate unrestricted observation.
// Include after gina_mia_lead_probe.inc; all four trainer records stay intact.
#line 56000 "work/gina_mia_charm_probe.inc"

static void GinaMiaCharmBoard(void)
{
    static const struct EmeraldChampionsBattleSet timburr = {
        .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
        .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
        .statPoints = {32, 18, 0, 0, 16, 0},
    };
    GinaMiaLeadPlayer(SPECIES_TIMBURR, &timburr);
    GinaMiaLeadPlayer(SPECIES_PACHIRISU, &sGinaMiaPachirisu);
    AuthoredOpponent(TRAINER_GINA_AND_MIA_1, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PLUSLE);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_MINUN);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_VOLBEAT);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_ILLUMISE);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_MOVE2), MOVE_CHARM);
    EXPECT_GT(GetMonData(&party[1], MON_DATA_SPEED),
        GetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][0], MON_DATA_SPEED));
    for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
        for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
        {
            struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
            Test_MgbaPrintf("GINA_CHARM_INPUT owner=%d slot=%d species=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF), GetMonData(mon, MON_DATA_SPATK),
                GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
        }
}

DOUBLE_BATTLE_TEST("EC live review Gina Charm: native immediate Attack drop versus Thunderbolt", s16 replyDamage)
{
    bool32 charm;
    PARAMETRIZE { charm = TRUE; }
    PARAMETRIZE { charm = FALSE; }
    GIVEN { GinaMiaCharmBoard(); }
    WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            // Same Plusle action in both mechanical counterfactuals. This is
            // not a claim that Protect is its optimal unrestricted choice.
            MOVE(opponentLeft, MOVE_PROTECT);
            if (charm)
                MOVE(opponentRight, MOVE_CHARM, target: playerLeft, hit: TRUE);
            else
                MOVE(opponentRight, MOVE_THUNDERBOLT, target: playerLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, charm ? MOVE_CHARM : MOVE_THUNDERBOLT, opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAIN_PUNCH, playerLeft);
        HP_BAR(opponentRight, captureDamage: &results[i].replyDamage);
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - (charm ? 2 : 0));
        EXPECT_GT(results[i].replyDamage, 0);
        Test_MgbaPrintf("GINA_CHARM_CONTROL charm=%d replyDamage=%d foeAtkStage=%d foeHP=%d/%d actualMinunHP=%d/%d actualMinunItem=%d active=%d/%d activeHP=%d/%d faints=%d/%d",
            charm, results[i].replyDamage, playerLeft->statStages[STAT_ATK], playerLeft->hp, playerRight->hp,
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_MAX_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HELD_ITEM),
            opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
    } FINALLY {
        EXPECT_LT(results[0].replyDamage, results[1].replyDamage);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Gina Charm: unrestricted actual physical pressure")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    GIVEN { GinaMiaCharmBoard(); }
    WHEN {
        for (u32 turn = 0; turn < turns; turn++)
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        Test_MgbaPrintf("GINA_CHARM_AI turns=%d active=%d/%d slots=%d/%d ownHP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S foeAtk=%d/%d foeStatus=%d/%d faints=%d/%d",
            gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            playerLeft->statStages[STAT_ATK], playerRight->statStages[STAT_ATK],
            playerLeft->status1, playerRight->status1,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("GINA_CHARM_AI_PARTY slot=%d species=%d HP=%d/%d",
                slot, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MAX_HP));
        // Combined attacks may remove the threat without Charm. No move or
        // survival assertion treats the counterfactual as the only good play.
    }
}
