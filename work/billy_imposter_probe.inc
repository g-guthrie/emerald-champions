// Temporary candidate/native entry comparison; include after Billy helpers.
#line 35000 "work/billy_imposter_probe.inc"
#include "battle_ai_switch.h"

static void BillyImposterBoard(u32 control)
{
    struct EmeraldChampionsBattleSet timburr = sBillyReserveTimburr;
    timburr.moves[0] = MOVE_SUBSTITUTE;
    // Retain real native Bulk Up and Substitute. No stat-stage, PP, copied
    // move, ability or Substitute volatile is injected into the live board.
    if (control == 2)
    {
        BillyReservePlayer(SPECIES_TIMBURR, &timburr);
        BillyReservePlayer(SPECIES_PACHIRISU, &sBillyReservePachirisu);
    }
    else
    {
        BillyReservePlayer(SPECIES_PACHIRISU, &sBillyReservePachirisu);
        BillyReservePlayer(SPECIES_TIMBURR, &timburr);
    }
    AuthoredOpponent(TRAINER_BILLY, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_LOTAD);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_WINGULL);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_DITTO);
    EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_IMPOSTER);
}

static void BillyImposterPrint(u32 control, bool32 native)
{
    struct BattlePokemon *mon = &gBattleMons[B_BATTLER_1];
    struct BattlePokemon *diagonal = &gBattleMons[B_BATTLER_2];
    Test_MgbaPrintf("BILLY_IMPOSTER_ENTRY c=%d native=%d species=%d transformed=%d HP=%d/%d item=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d AtkStage=%d DefStage=%d ability=%d diagonal=%d diagSub=%d diagAtk=%d diagDef=%d diagSpeed=%d diagAtkStage=%d diagDefStage=%d",
        control, native, mon->species, mon->volatiles.transformed, mon->hp, mon->maxHP, mon->item,
        mon->attack, mon->defense, mon->spAttack, mon->spDefense, mon->speed,
        mon->statStages[STAT_ATK], mon->statStages[STAT_DEF], GetBattlerAbility(B_BATTLER_1),
        diagonal->species, diagonal->volatiles.substitute, diagonal->attack, diagonal->defense, diagonal->speed,
        diagonal->statStages[STAT_ATK], diagonal->statStages[STAT_DEF]);
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
        Test_MgbaPrintf("BILLY_IMPOSTER_ENTRY_MOVE c=%d native=%d slot=%d move=%S PP=%d diagonalMove=%S diagonalPP=%d",
            control, native, slot, GetMoveName(mon->moves[slot]), mon->pp[slot],
            GetMoveName(diagonal->moves[slot]), diagonal->pp[slot]);
}

DOUBLE_BATTLE_TEST("EC live review Billy Imposter: candidate versus native boosted Substitute and diagonal entry")
{
    u32 control;
    bool32 native;
    PARAMETRIZE { control = 0; native = FALSE; }
    PARAMETRIZE { control = 0; native = TRUE; }
    PARAMETRIZE { control = 1; native = FALSE; }
    PARAMETRIZE { control = 1; native = TRUE; }
    PARAMETRIZE { control = 2; native = FALSE; }
    PARAMETRIZE { control = 2; native = TRUE; }
    GIVEN {
        BillyImposterBoard(control);
    } WHEN {
        TURN {
            if (control == 2)
            {
                MOVE(playerLeft, MOVE_BULK_UP);
                MOVE(playerRight, MOVE_PROTECT);
            }
            else
            {
                MOVE(playerLeft, MOVE_PROTECT);
                MOVE(playerRight, control == 1 ? MOVE_SUBSTITUTE : MOVE_BULK_UP);
            }
            MOVE(opponentLeft, MOVE_RAIN_DANCE);
            MOVE(opponentRight, MOVE_PROTECT);
        }
        if (native)
        {
            TURN {
                MOVE(playerLeft, MOVE_PROTECT);
                MOVE(playerRight, MOVE_PROTECT);
                SWITCH(opponentLeft, 2);
                MOVE(opponentRight, MOVE_PROTECT);
            }
        }
    } THEN {
        if (native)
        {
            EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_1], 2);
            BillyImposterPrint(control, TRUE);
        }
        else
        {
            struct BattlePokemon before = gBattleMons[B_BATTLER_1];
            struct SwitchCandidateSnapshot *state = AI_SaveCandidateState();
            EXPECT(state != NULL);
            AI_LoadSwitchCandidate(B_BATTLER_1, 2, FALSE);
            BillyImposterPrint(control, FALSE);
            AI_RestoreCandidateState(state);
            AI_FreeCandidateState(state);
            EXPECT_EQ(memcmp(&before, &gBattleMons[B_BATTLER_1], sizeof(before)), 0);
        }
        // No candidate transformation expectation before baseline. Compare
        // native entry with its candidate explicitly, including retained Ditto
        // HP/Scarf, copied stages, min(5, base PP), and the actual diagonal.
        // This is not a forced-switch desirability or full AI battle verdict.
    }
}
