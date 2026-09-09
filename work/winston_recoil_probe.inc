// Temporary actual Wooloo damage/recoil control, not an AI choice claim.
#line 43000 "work/winston_recoil_probe.inc"
extern bool32 DoesBattlerKOItselfWithRecoil(enum BattlerId battlerAtk, enum BattlerId battlerDef, enum Move move);
DOUBLE_BATTLE_TEST("EC live review Winston recoil: actual Double Edge uses damage not user max HP", s16 damage; s16 recoil)
{
    bool32 injured;
    PARAMETRIZE { injured = FALSE; }
    PARAMETRIZE { injured = TRUE; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet pachirisu = {
            .moves = {MOVE_FOLLOW_ME, MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet timburr = {
            .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        WinstonReservePlayer(SPECIES_PACHIRISU, &pachirisu);
        WinstonReservePlayer(SPECIES_TIMBURR, &timburr);
        // Existing raw move comparison applies its self-sacrifice penalty
        // only while opposing reserves remain. Exercise that actual consumer.
        WinstonReservePlayer(SPECIES_TIMBURR, &timburr);
        AuthoredOpponent(TRAINER_WINSTON_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PORYGON);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_HONEDGE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_GIMMIGHOUL);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_WOOLOO);
        if (injured) {
            u16 hp = 13;
            SetMonData(&party[3], MON_DATA_HP, &hp);
        }
        for (u32 slot = 0; slot < 2; slot++) {
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + 2];
            party[slot + 2] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_DOUBLE_EDGE, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_EDGE, opponentRight);
        HP_BAR(playerLeft, captureDamage: &results[i].damage);
        HP_BAR(opponentRight, captureDamage: &results[i].recoil);
    } THEN {
        u32 recoil = max(1, results[i].damage * GetMoveRecoil(MOVE_DOUBLE_EDGE) / 100);
        u32 initial = injured ? 13 : opponentRight->maxHP;
        u32 expected = initial > recoil ? min(opponentRight->maxHP,
            initial - recoil + max(1, opponentRight->maxHP / 16)) : 0;
        EXPECT_EQ(results[i].recoil, min(initial, recoil));
        EXPECT_EQ(opponentRight->hp, expected);
        Test_MgbaPrintf("WINSTON_RECOIL injured=%d maxHP=%d damage=%d nativeRecoil=%d finalHP=%d oldMaxHpEstimate=%d",
            injured, opponentRight->maxHP, results[i].damage, recoil, opponentRight->hp,
            opponentRight->maxHP * GetMoveRecoil(MOVE_DOUBLE_EDGE) / 100);
        if (injured)
        {
            // Direct native-cache helper query, not an unrestricted move
            // selection. Preserve the actual completed battle HP afterward.
            SetAiLogicDataForTurn(gAiLogicData);
            u32 slot = GetMoveIndex(B_BATTLER_3, MOVE_DOUBLE_EDGE);
            u32 lowDamage = min(gAiLogicData->simulatedDmg[B_BATTLER_3][B_BATTLER_0][slot].minimum, playerLeft->hp);
            u32 lowRecoil = max(1, lowDamage * GetMoveRecoil(MOVE_DOUBLE_EDGE) / 100);
            EXPECT_GT(lowDamage, 0);
            u16 savedHp = opponentRight->hp;
            opponentRight->hp = lowRecoil + 1;
            EXPECT(!DoesBattlerKOItselfWithRecoil(B_BATTLER_3, B_BATTLER_0, MOVE_DOUBLE_EDGE));
            opponentRight->hp = lowRecoil;
            EXPECT(DoesBattlerKOItselfWithRecoil(B_BATTLER_3, B_BATTLER_0, MOVE_DOUBLE_EDGE));
            opponentRight->hp = savedHp;
            Test_MgbaPrintf("WINSTON_RECOIL_QUERY minimumDamage=%d lethalOnHitHP=%d safeHP=%d", lowDamage, lowRecoil, lowRecoil + 1);
        }
    }
}
