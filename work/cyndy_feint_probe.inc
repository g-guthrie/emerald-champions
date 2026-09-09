// Temporary actual reserve guard-break controls, not a trainer move lock.
#line 57000 "work/cyndy_feint_probe.inc"
#include "emerald_champions_battle_sets.h"

static void CyndyFeintPlayer(enum Species species, enum Ability ability)
{
    const struct EmeraldChampionsBattleSet set = {
        .moves = {MOVE_PROTECT}, .item = ITEM_NONE, .nature = NATURE_BOLD,
        .ability = ability, .statPoints = {32, 0, 32, 0, 2, 0},
    };
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(MOVE_PROTECT);
    }
}

static void CyndyFeintBoard(bool32 ghost)
{
    // Legal native early players deliberately know only Protect. This isolates
    // an observable guard opportunity without reading a pending human command.
    CyndyFeintPlayer(ghost ? SPECIES_DUSKULL : SPECIES_PACHIRISU, ghost ? ABILITY_LEVITATE : ABILITY_VOLT_ABSORB);
    CyndyFeintPlayer(SPECIES_MUNCHLAX, ABILITY_THICK_FAT);
    AuthoredOpponent(TRAINER_CYNDY_1, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_RIOLU);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_MEDITITE);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_CRABRAWLER);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_CLOBBOPUS);
    u16 zero = 0;
    SetMonData(&party[0], MON_DATA_HP, &zero);
    SetMonData(&party[1], MON_DATA_HP, &zero);
    for (u32 slot = 0; slot < 2; slot++)
    {
        struct Pokemon saved = party[slot];
        party[slot] = party[slot + 2];
        party[slot + 2] = saved;
    }
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
    for (u32 slot = 0; slot < 2; slot++)
        Test_MgbaPrintf("CYNDY_FEINT_INPUT slot=%d species=%d HP=%d Atk=%d Def=%d Speed=%d ability=%d item=%d",
            slot, GetMonData(&party[slot], MON_DATA_SPECIES), GetMonData(&party[slot], MON_DATA_HP),
            GetMonData(&party[slot], MON_DATA_ATK), GetMonData(&party[slot], MON_DATA_DEF),
            GetMonData(&party[slot], MON_DATA_SPEED), GetMonAbility(&party[slot]), GetMonData(&party[slot], MON_DATA_HELD_ITEM));
}

DOUBLE_BATTLE_TEST("EC live review Cyndy Feint: native guard break enables the partner punch", s16 feintDamage, s16 punchDamage)
{
    bool32 feint;
    PARAMETRIZE { feint = FALSE; }
    PARAMETRIZE { feint = TRUE; }
    GIVEN { CyndyFeintBoard(FALSE); }
    WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_DRAIN_PUNCH, target: playerLeft, hit: TRUE, criticalHit: FALSE);
            if (feint)
                MOVE(opponentRight, MOVE_FEINT, target: playerLeft, hit: TRUE, criticalHit: FALSE);
            else
                MOVE(opponentRight, MOVE_PROTECT);
        }
    } SCENE {
        if (feint)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FEINT, opponentRight);
            HP_BAR(playerLeft, captureDamage: &results[i].feintDamage);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAIN_PUNCH, opponentLeft);
            HP_BAR(playerLeft, captureDamage: &results[i].punchDamage);
        }
    } THEN {
        EXPECT_GT(playerLeft->hp, 0);
        if (feint)
        {
            EXPECT_GT(results[i].feintDamage, 0);
            EXPECT_GT(results[i].punchDamage, 0);
        }
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP - results[i].feintDamage - results[i].punchDamage);
        Test_MgbaPrintf("CYNDY_FEINT_NATIVE feint=%d firstDamage=%d partnerDamage=%d targetHP=%d/%d ownHP=%d/%d",
            feint, results[i].feintDamage, results[i].punchDamage, playerLeft->hp, playerLeft->maxHP,
            opponentLeft->hp, opponentRight->hp);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Cyndy Feint: unrestricted actual reserves against known guards")
{
    GIVEN { CyndyFeintBoard(FALSE); }
    WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        Test_MgbaPrintf("CYNDY_FEINT_AI moves=%S/%S HP=%d/%d foes=%d/%d faints=%d/%d",
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        gAiLogicData->battlerMovesScored = 0;
        u32 start = gMain.vblankCounter1;
        bool32 selected = AI_ComputeDoublesDecisions(B_BATTLER_1);
        u32 warmFrames = gMain.vblankCounter1 - start;
        gAiLogicData->battlerMovesScored = 0;
        start = gMain.vblankCounter1;
        SetAiLogicDataForTurn(gAiLogicData);
        bool32 rebuilt = AI_ComputeDoublesDecisions(B_BATTLER_1);
        EXPECT(selected && rebuilt);
        Test_MgbaPrintf("CYNDY_FEINT_TIMING warm=%d rebuilt=%d", warmFrames, gMain.vblankCounter1 - start);
        // No hoped-for trainer action is prescribed. Compare this native
        // decision against the explicit mechanical controls above.
    }
}

DOUBLE_BATTLE_TEST("EC live review Cyndy Feint: an immune target retains Protect against the partner")
{
    // Duskull is a native mechanical Ghost boundary, not an assertion about
    // its availability at Cyndy's current point in the campaign.
    GIVEN { CyndyFeintBoard(TRUE); }
    WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_ICE_PUNCH, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentRight, MOVE_FEINT, target: playerLeft, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, playerLeft->maxHP);
        EXPECT_EQ(gBattleResults.playerFaintCounter, 0);
        Test_MgbaPrintf("CYNDY_FEINT_GHOST HP=%d/%d moves=%S/%S",
            playerLeft->hp, playerLeft->maxHP,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]));
    }
}
