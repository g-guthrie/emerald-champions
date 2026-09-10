// Temporary actual Hector reserve observations. Include after HectorPlayer.
// Forced mechanics and unrestricted AI observations are separate groups.
#line 56000 "work/hector_flinch_probe.inc"
#include "battle_main.h"
#include "battle_util.h"

static void HectorFlinchBoard(void)
{
    static const struct EmeraldChampionsBattleSet timburr = {
        .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
        .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
        .evs = {252, 0, 56, 0, 0, 200},
    };
    static const struct EmeraldChampionsBattleSet shellos = {
        .moves = {MOVE_SCALD, MOVE_EARTH_POWER, MOVE_RECOVER, MOVE_PROTECT},
        .item = ITEM_EVIOLITE, .nature = NATURE_BOLD, .ability = ABILITY_STORM_DRAIN,
        .evs = {252, 0, 56, 0, 0, 200},
    };
    // Route102 Timburr / Route103 Shellos, legal native IV31 level14 sets.
    // Shellos is Water, NOT Ground. No Ground/Ghost eligibility claim follows
    // from this fixture; those Glare rules require separate source evidence.
    HectorPlayer(SPECIES_TIMBURR, &timburr);
    HectorPlayer(SPECIES_SHELLOS, &shellos);
    AuthoredOpponent(TRAINER_HECTOR, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SPINDA);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_ZANGOOSE);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_SEVIPER);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_DUNSPARCE);
    EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_INFILTRATOR);
    EXPECT_EQ(GetMonAbility(&party[3]), ABILITY_SERENE_GRACE);
    u16 zero = 0;
    SetMonData(&party[0], MON_DATA_HP, &zero);
    SetMonData(&party[1], MON_DATA_HP, &zero);
    for (u32 slot = 0; slot < 2; slot++)
    {
        struct Pokemon saved = party[slot];
        party[slot] = party[slot + 2];
        party[slot + 2] = saved;
    }
    // Check after the complete permutation; no invented derived Speed values.
    for (u32 slot = 0; slot < 2; slot++)
    {
        struct Pokemon *player = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][slot];
        EXPECT_GT(GetMonData(player, MON_DATA_SPEED), GetMonData(&party[1], MON_DATA_SPEED));
        EXPECT_LT(GetMonData(player, MON_DATA_SPEED), GetMonData(&party[0], MON_DATA_SPEED));
    }
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
    for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
        for (u32 slot = 0; slot < 2; slot++)
        {
            struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
            Test_MgbaPrintf("HECTOR_FLINCH_INPUT owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
        }
}

static void HectorFlinchAfter(u32 context)
{
    struct BattlePokemon *playerLeft = &gBattleMons[B_BATTLER_0];
    struct BattlePokemon *opponentLeft = &gBattleMons[B_BATTLER_1];
    struct BattlePokemon *playerRight = &gBattleMons[B_BATTLER_2];
    struct BattlePokemon *opponentRight = &gBattleMons[B_BATTLER_3];
    const struct AdditionalEffect *effect = GetMoveAdditionalEffectById(MOVE_ROCK_SLIDE, 0);
    EXPECT_EQ((u32)effect->moveEffect, MOVE_EFFECT_FLINCH);
    EXPECT_EQ((u32)effect->chance, 30);
    u32 chance = CalcSecondaryEffectChance(B_BATTLER_3, opponentRight->ability, effect);
    EXPECT_EQ(chance, 60);
    Test_MgbaPrintf("HECTOR_FLINCH_AFTER context=%d turns=%d active=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S status=%d/%d Speed=%d/%d nativeChance=%d faints=%d/%d",
        context, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
        opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
        GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
        GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
        playerLeft->status1, playerRight->status1,
        GetBattlerTotalSpeedStat(B_BATTLER_0, playerLeft->ability, GetBattlerHoldEffect(B_BATTLER_0)),
        GetBattlerTotalSpeedStat(B_BATTLER_2, playerRight->ability, GetBattlerHoldEffect(B_BATTLER_2)),
        chance, gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}

DOUBLE_BATTLE_TEST("EC live review Hector flinch: forced Glare enables slower Rock Slide")
{
    bool32 glare;
    PARAMETRIZE { glare = FALSE; }
    PARAMETRIZE { glare = TRUE; }
    GIVEN { HectorFlinchBoard(); }
    WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ICE_PUNCH, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE, WITH_RNG(RNG_PARALYSIS, FALSE));
            MOVE(playerRight, MOVE_SCALD, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            // A real alternate attack preserves the authored menu. Native
            // always-accurate Glare did not honor a forced-miss control.
            if (glare)
                MOVE(opponentLeft, MOVE_GLARE, target: playerLeft, hit: TRUE);
            else
                MOVE(opponentLeft, MOVE_SLUDGE_BOMB, target: playerLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            // Controlled success demonstrates timing, NOT a measured 60%
            // frequency. The native probability helper is queried separately.
            MOVE(opponentRight, MOVE_ROCK_SLIDE, hit: TRUE, criticalHit: FALSE, secondaryEffect: TRUE);
        }
    } SCENE {
        if (glare)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_GLARE, opponentLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SCALD, playerRight);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_SLIDE, opponentRight);
        }
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SLUDGE_BOMB, opponentLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_PUNCH, playerLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SCALD, playerRight);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_SLIDE, opponentRight);
        }
    } THEN {
        HectorFlinchAfter(glare);
        EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPECIES), SPECIES_TIMBURR);
        if (glare)
        {
            EXPECT(playerLeft->status1 & STATUS1_PARALYSIS);
            EXPECT_EQ(gLastMoves[B_BATTLER_0], MOVE_NONE);
        }
        else
            EXPECT_EQ(gLastMoves[B_BATTLER_0], MOVE_ICE_PUNCH);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Hector flinch: unrestricted identical native reserve board")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    GIVEN { HectorFlinchBoard(); }
    WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_ICE_PUNCH, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE, WITH_RNG(RNG_PARALYSIS, FALSE));
            MOVE(playerRight, MOVE_SCALD, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        HectorFlinchAfter(10 + turns);
        // No Glare / Rock Slide / flinch or winning action is prescribed.
    }
}
