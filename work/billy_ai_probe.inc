// Temporary actual-party E0006 decisions, not a permanent team lock.
#line 32000 "work/billy_ai_probe.inc"
#include "emerald_champions_battle_sets.h"

static void BillyProbePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species)
    {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon));
        Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
              GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}
static const struct EmeraldChampionsBattleSet sBillyProbeTimburr = {
    .moves = {MOVE_ICE_PUNCH, MOVE_DRAIN_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_IRON_FIST,
    .statPoints = {32, 16, 18, 0, 0, 0},
};
static const struct EmeraldChampionsBattleSet sBillyProbePachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .statPoints = {32, 0, 32, 0, 2, 0},
};
static const struct EmeraldChampionsBattleSet sBillyProbeEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
    .statPoints = {32, 32, 2, 0, 0, 0},
};
static const struct EmeraldChampionsBattleSet sBillyProbeMienfoo = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .statPoints = {18, 16, 0, 0, 0, 32},
};

AI_DOUBLE_BATTLE_TEST("EC live review Billy: actual rain leads against pressure and denial")
{
    u32 scenario, turns;
    PARAMETRIZE { scenario = 0; turns = 1; }
    PARAMETRIZE { scenario = 0; turns = 2; }
    PARAMETRIZE { scenario = 1; turns = 1; }
    PARAMETRIZE { scenario = 1; turns = 2; }
    PARAMETRIZE { scenario = 2; turns = 1; }
    PARAMETRIZE { scenario = 2; turns = 2; }
    GIVEN {
        BillyProbePlayer(scenario == 0 ? SPECIES_TIMBURR : scenario == 1 ? SPECIES_PACHIRISU : SPECIES_MIENFOO,
            scenario == 0 ? &sBillyProbeTimburr : scenario == 1 ? &sBillyProbePachirisu : &sBillyProbeMienfoo);
        BillyProbePlayer(SPECIES_EEVEE, &sBillyProbeEevee);
        AuthoredOpponent(TRAINER_BILLY, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_LOTAD);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_WINGULL);
        EXPECT_GT(GetMonData(&party[0], MON_DATA_SPEED), GetMonData(&party[1], MON_DATA_SPEED));
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("BILLY_INPUT c=%d turns=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                    scenario, turns, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++)
            TURN {
                if (scenario == 0)
                    MOVE(playerLeft, MOVE_ICE_PUNCH, target: opponentLeft, secondaryEffect: FALSE);
                else if (scenario == 1)
                    MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight, secondaryEffect: FALSE);
                else if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft);
                else
                    MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
                MOVE(playerRight, MOVE_DOUBLE_EDGE, target: opponentRight, secondaryEffect: FALSE);
            }
    } THEN {
        Test_MgbaPrintf("BILLY_AFTER c=%d turns=%d LAST=%S/%S species=%d/%d party=%d/%d HP=%d/%d foeHP=%d/%d weather=%d speedStages=%d/%d",
            scenario, turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->species, opponentRight->species,
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            gBattleWeather, opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED]);
    }
}
