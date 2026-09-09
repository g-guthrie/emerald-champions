// Temporary E1 native diagnostic; include after AuthoredOpponent.
#line 6000 "work/rival_chespin_probe.inc"
// Uses generated parties and legal early player preparation, not toy stats.
#include "emerald_champions_opening.h"
#include "emerald_champions_battle_sets.h"

static void ChespinProbePlayers(void)
{
    static const struct EmeraldChampionsBattleSet set = {
        .moves = {MOVE_KNOCK_OFF, MOVE_DRAIN_PUNCH, MOVE_U_TURN, MOVE_PROTECT},
        .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_INNER_FOCUS,
        .statPoints = {32, 0, 32, 0, 2, 0},
    };
    // Mienfoo is a current Route101 encounter. Two legal captures, each using
    // the free native stat/move/item preparation service at the live cap.
    for (u32 slot = 0; slot < 2; slot++)
    {
        struct Pokemon mon;
        CreateRandomMonWithIVs(&mon, SPECIES_MIENFOO, 14, MAX_PER_STAT_IVS);
        EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &set), EC_BATTLE_SET_SUCCESS);
        PLAYER(SPECIES_MIENFOO)
        {
            *gBattleTestRunnerState->data.currentMon = mon;
            Nature(GetNature(&mon));
            Ability(GetMonAbility(&mon));
            Speed(GetMonData(&mon, MON_DATA_SPEED));
            Moves(MOVE_KNOCK_OFF, MOVE_DRAIN_PUNCH, MOVE_U_TURN, MOVE_PROTECT);
        }
    }
}

static void ChespinProbeOpponent(void)
{
    AuthoredOpponent(TRAINER_BRENDAN_ROUTE_103_MUDKIP, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    enum Species species = SPECIES_CHESPIN;
    u8 level = GetMonData(&party[0], MON_DATA_LEVEL);
    // Match the private regional resolver's species/EXP transition, then use
    // its public native preset implementation. Field routing is not certified.
    SetMonData(&party[0], MON_DATA_SPECIES, &species);
    u32 experience = gExperienceTables[gSpeciesInfo[species].growthRate][level];
    SetMonData(&party[0], MON_DATA_EXP, &experience);
    ApplyEmeraldChampionsRegionalRivalSet(party, 0, TRUE);
    CalculateMonStats(&party[0]);
    gBattleTestRunnerState->data.forcedAbilities[B_TRAINER_OPPONENT_A][0] = GetMonAbility(&party[0]);
}

AI_DOUBLE_BATTLE_TEST("EC live review E0001 Chespin: actual pair against legal bulky Mienfoo")
{
    GIVEN {
        ChespinProbePlayers();
        ChespinProbeOpponent();
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentLeft);
            MOVE(playerRight, MOVE_KNOCK_OFF, target: opponentLeft);
        }
    } THEN {
        Test_MgbaPrintf("Chespin AI LAST executed=%S/%S nativeHP=%d/%d playerMaxHP=%d/%d speeds=%d/%d/%d/%d",
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            playerLeft->hp, playerRight->hp, playerLeft->maxHP, playerRight->maxHP,
            opponentLeft->speed, opponentRight->speed, playerLeft->speed, playerRight->speed);
        // Root's temporary decision-time logger, not THEN's next-turn chosen
        // moves/cache, provides the actual first decision and selected targets.
    }
}

DOUBLE_BATTLE_TEST("EC live review E0001 Chespin: native Super Fang after actual Thunderbolt", s16 boltDamage; s16 fangDamage)
{
    bool32 bolt;
    PARAMETRIZE { bolt = FALSE; }
    PARAMETRIZE { bolt = TRUE; }
    GIVEN {
        ChespinProbePlayers();
        ChespinProbeOpponent();
    } WHEN {
        TURN {
            // No draining, healing item or self-recoil changes the target HP
            // between the two opponent attacks. Accuracy and critical-hit
            // outcomes are controlled; ordinary damage variation is retained.
            MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentLeft, criticalHit: FALSE);
            MOVE(playerRight, MOVE_KNOCK_OFF, target: opponentLeft, criticalHit: FALSE);
            if (bolt)
                MOVE(opponentRight, MOVE_THUNDERBOLT, target: playerLeft, hit: TRUE, criticalHit: FALSE);
            else
                MOVE(opponentRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_SUPER_FANG, target: playerLeft, hit: TRUE, criticalHit: FALSE);
        }
    } SCENE {
        if (bolt)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERBOLT, opponentRight);
            HP_BAR(playerLeft, captureDamage: &results[i].boltDamage);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUPER_FANG, opponentLeft);
        HP_BAR(playerLeft, captureDamage: &results[i].fangDamage);
    } THEN {
        u32 initialHp = playerLeft->maxHP;
        u32 boltDamage = bolt ? results[i].boltDamage : 0;
        u32 beforeFang = initialHp - boltDamage;
        u32 nativeFang = max(1, beforeFang * GetMoveDamagePercentage(MOVE_SUPER_FANG) / 100);
        u32 staleFang = min(beforeFang, initialHp * GetMoveDamagePercentage(MOVE_SUPER_FANG) / 100);
        Test_MgbaPrintf("Chespin counterfactual bolt=%d nativeInitialHP=%d nativeBolt=%d beforeFang=%d actualFang=%d currentHPformula=%d staleCachedFormula=%d finalHP=%d PikachuSpA=%d playerSpD=%d",
            bolt, initialHp, boltDamage, beforeFang, results[i].fangDamage,
            nativeFang, staleFang, playerLeft->hp, opponentRight->spAttack, playerLeft->spDefense);
        EXPECT_EQ(results[i].fangDamage, nativeFang);
        EXPECT_EQ(playerLeft->hp, beforeFang - nativeFang);
        if (bolt)
        {
            // These assert the demonstrated native scenario's premise. If
            // current generated stats invalidate it, inspect the diagnostic;
            // do not change fixed stats or silently bless a different case.
            EXPECT(boltDamage > initialHp / 2);
            EXPECT(boltDamage < initialHp);
            EXPECT_EQ(staleFang, beforeFang);
            EXPECT(playerLeft->hp > 0);
        }
    }
}
