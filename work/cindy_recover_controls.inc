// Temporary native Recover controls; include after Cindy recovery helpers.
#line 55000 "work/cindy_recover_controls.inc"

DOUBLE_BATTLE_TEST("EC live review Cindy Recover control: initially full HP heals after faster Round", s16 hit, s16 heal)
{
    PARAMETRIZE { }
    GIVEN {
        CindyRecoverBoard();
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        u16 hp = GetMonData(&party[1], MON_DATA_MAX_HP);
        SetMonData(&party[1], MON_DATA_HP, &hp);
        EXPECT_GT(GetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][1], MON_DATA_SPEED),
            GetMonData(&party[1], MON_DATA_SPEED));
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_ROUND, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_RECOVER);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROUND, playerRight);
        HP_BAR(opponentRight, captureDamage: &results[i].hit);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, opponentRight);
        HP_BAR(opponentRight, captureDamage: &results[i].heal);
    } THEN {
        EXPECT_GT(results[i].hit, 0);
        EXPECT_EQ(results[i].heal, -results[i].hit);
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
        Test_MgbaPrintf("CINDY_RECOVER_FULL hit=%d heal=%d HP=%d/%d SpeedPachi=%d SpeedMilcery=%d",
            results[i].hit, results[i].heal, opponentRight->hp, opponentRight->maxHP,
            playerRight->speed, opponentRight->speed);
    }
}

DOUBLE_BATTLE_TEST("EC live review Cindy Recover control: native Aroma Veil versus Skill Swap then Heal Block")
{
    bool32 removeVeil;
    PARAMETRIZE { removeVeil = FALSE; }
    PARAMETRIZE { removeVeil = TRUE; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet natu = {
            .moves = {MOVE_SKILL_SWAP, MOVE_PROTECT, MOVE_NIGHT_SHADE, MOVE_PSYCHIC},
            .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_EARLY_BIRD,
            .statPoints = {32, 0, 2, 0, 0, 32},
        };
        static const struct EmeraldChampionsBattleSet elgyem = {
            .moves = {MOVE_HEAL_BLOCK, MOVE_PROTECT, MOVE_PSYCHIC, MOVE_RECOVER},
            .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_TELEPATHY,
            .statPoints = {32, 0, 2, 0, 0, 32},
        };
        CindyReservePlayer(SPECIES_NATU, &natu);
        CindyReservePlayer(SPECIES_ELGYEM, &elgyem);
        AuthoredOpponent(TRAINER_CINDY_1, 0, FALSE);
        struct Pokemon *foes = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        struct Pokemon *players = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER];
        EXPECT_EQ(GetMonData(&foes[1], MON_DATA_SPECIES), SPECIES_MILCERY);
        EXPECT_EQ(GetMonAbility(&foes[1]), ABILITY_AROMA_VEIL);
        u16 hp = 20;
        SetMonData(&foes[1], MON_DATA_HP, &hp);
        EXPECT_GT(GetMonData(&players[0], MON_DATA_SPEED), GetMonData(&players[1], MON_DATA_SPEED));
        EXPECT_GT(GetMonData(&players[1], MON_DATA_SPEED), GetMonData(&foes[1], MON_DATA_SPEED));
        // Elgyem is an explicitly later-access mechanic control, not a claim
        // about Cindy's opening player roster. All moves and stats are native.
        // Skill Swap removes the real Aroma Veil before native Heal Block;
        // Recover was legally selected before either effect resolves.
    } WHEN {
        TURN {
            if (removeVeil)
                MOVE(playerLeft, MOVE_SKILL_SWAP, target: opponentRight);
            else
                MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_HEAL_BLOCK, hit: TRUE);
            MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_RECOVER);
        }
    } SCENE {
        if (removeVeil)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SKILL_SWAP, playerLeft);
            NONE_OF {
                ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, opponentRight);
                HP_BAR(opponentRight);
            }
        }
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, opponentRight);
            HP_BAR(opponentRight);
        }
    } THEN {
        if (removeVeil)
        {
            EXPECT_EQ(GetBattlerAbility(B_BATTLER_3), ABILITY_EARLY_BIRD);
            EXPECT_GT((u32)opponentRight->volatiles.healBlockTimer, 0);
            EXPECT_EQ(opponentRight->hp, 20);
        }
        else
        {
            EXPECT_EQ(GetBattlerAbility(B_BATTLER_3), ABILITY_AROMA_VEIL);
            EXPECT_EQ((u32)opponentRight->volatiles.healBlockTimer, 0);
            EXPECT_EQ(opponentRight->hp, min(opponentRight->maxHP, 20 + max(1, opponentRight->maxHP / 2)));
        }
        Test_MgbaPrintf("CINDY_RECOVER_HEALBLOCK removedVeil=%d ability=%d timer=%d HP=%d/%d Speeds=%d/%d/%d",
            removeVeil, GetBattlerAbility(B_BATTLER_3), opponentRight->volatiles.healBlockTimer,
            opponentRight->hp, opponentRight->maxHP, playerLeft->speed, playerRight->speed, opponentRight->speed);
    }
}
