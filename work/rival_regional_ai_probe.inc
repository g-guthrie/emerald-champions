// Temporary native scenario sampling, not a permanent test module.
#line 2000 "work/rival_regional_ai_probe.inc"
#include "starter_choose.h"
#include "emerald_champions_opening.h"

AI_DOUBLE_BATTLE_TEST("EC live review E0001: actual native regional opening replacements")
{
    u32 generation = 1, starter = 0;
    for (u32 gen = 1; gen <= 9; gen++)
    {
        if (gen == 3)
            continue;
        for (u32 slot = 0; slot < 3; slot++)
            PARAMETRIZE { generation = gen; starter = slot; }
    }
    GIVEN {
        static const u16 trainers[] = {TRAINER_BRENDAN_ROUTE_103_MUDKIP,
            TRAINER_BRENDAN_ROUTE_103_TREECKO, TRAINER_BRENDAN_ROUTE_103_TORCHIC};
        RivalReviewPlayer(SPECIES_MIENFOO);
        RivalReviewPlayer(SPECIES_NATU);
        AuthoredOpponent(trainers[starter], 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        enum Species species = GetStarterPokemonForGeneration(starter, generation);
        u8 level = GetMonData(&party[0], MON_DATA_LEVEL);
        // Mirror only the field resolver's species/EXP transition, then call
        // its actual public opening set implementation. This samples native
        // replacements; it does not prove the separate field resolver route.
        SetMonData(&party[0], MON_DATA_SPECIES, &species);
        u32 exp = gExperienceTables[gSpeciesInfo[species].growthRate][level];
        SetMonData(&party[0], MON_DATA_EXP, &exp);
        ApplyEmeraldChampionsRegionalRivalSet(party, 0, TRUE);
        CalculateMonStats(&party[0]);
        gBattleTestRunnerState->data.forcedAbilities[B_TRAINER_OPPONENT_A][0] = GetMonAbility(&party[0]);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_KNOCK_OFF, target: opponentLeft);
            MOVE(playerRight, MOVE_PSYCHIC, target: opponentLeft);
        }
    } THEN {
        Test_MgbaPrintf("E1 region=%d slot=%d species=%S initialAbility=%d level=%d hp=%d/%d lastExecuted=%S PikachuHP=%d lastExecuted=%S playerHP=%d/%d",
            generation, starter, GetSpeciesName(GetStarterPokemonForGeneration(starter, generation)),
            GetMonAbility(&gParties[B_TRAINER_OPPONENT_A][0]),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_LEVEL),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_MAX_HP),
            GetMoveName(gLastMoves[B_BATTLER_1]), opponentRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_3]), playerLeft->hp, playerRight->hp);
    }
}
