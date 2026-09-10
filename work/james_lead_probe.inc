// Temporary E0011 actual-party observations, not a permanent team lock.
#line 20000 "work/james_lead_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"
static void JamesLeadPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
            GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}
static const struct EmeraldChampionsBattleSet sJamesEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 252, 4, 0, 0, 0},
};
static const struct EmeraldChampionsBattleSet sJamesTimburr = {
    .moves = {MOVE_ICE_PUNCH, MOVE_DRAIN_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sJamesNatu = {
    .moves = {MOVE_PSYCHIC, MOVE_HEAT_WAVE, MOVE_REFLECT, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_SYNCHRONIZE,
    .evs = {252, 0, 0, 252, 4, 0},
};
static const struct EmeraldChampionsBattleSet sJamesPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sJamesMienfoo = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {136, 120, 0, 0, 0, 252},
};
AI_DOUBLE_BATTLE_TEST("EC live review James leads: rain physical special and Fake Out pressure")
{
    u32 board, turns;
    PARAMETRIZE { board = 0; turns = 1; }
    PARAMETRIZE { board = 0; turns = 2; }
    PARAMETRIZE { board = 1; turns = 1; }
    PARAMETRIZE { board = 1; turns = 2; }
    PARAMETRIZE { board = 2; turns = 1; }
    PARAMETRIZE { board = 2; turns = 2; }
    PARAMETRIZE { board = 3; turns = 1; }
    PARAMETRIZE { board = 3; turns = 2; }
    GIVEN {
        JamesLeadPlayer(board == 0 ? SPECIES_EEVEE : board == 1 ? SPECIES_NATU : SPECIES_MIENFOO,
            board == 0 ? &sJamesEevee : board == 1 ? &sJamesNatu : &sJamesMienfoo);
        JamesLeadPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU,
            board == 0 ? &sJamesTimburr : &sJamesPachirisu);
        AuthoredOpponent(TRAINER_JAMES_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SURSKIT);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_DEWPIDER);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_VENIPEDE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_KRICKETUNE);
        for (u32 slot = 0; slot < 4; slot++)
            for (u32 moveSlot = 0; moveSlot < MAX_MON_MOVES; moveSlot++)
                EXPECT(CanSpeciesUseEmeraldChampionsPreparationMove(GetMonData(&party[slot], MON_DATA_SPECIES),
                    GetMonData(&party[slot], MON_DATA_MOVE1 + moveSlot)));
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("JAMES_LEAD_INPUT board=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                board, slot, GetMonData(&party[slot], MON_DATA_SPECIES), GetMonData(&party[slot], MON_DATA_LEVEL),
                GetMonData(&party[slot], MON_DATA_HP), GetMonData(&party[slot], MON_DATA_ATK), GetMonData(&party[slot], MON_DATA_DEF),
                GetMonData(&party[slot], MON_DATA_SPATK), GetMonData(&party[slot], MON_DATA_SPDEF), GetMonData(&party[slot], MON_DATA_SPEED),
                GetMonData(&party[slot], MON_DATA_HELD_ITEM), GetMonAbility(&party[slot]));
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else if (board == 1) {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            } else {
                MOVE(playerLeft, turn == 0 ? (board == 3 ? MOVE_TAUNT : MOVE_FAKE_OUT) : MOVE_DRAIN_PUNCH, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("JAMES_LEAD_AFTER board=%d turns=%d weather=%d rainTimer=%d active=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeSpA=%d/%d foeAtk=%d/%d foeSpeed=%d/%d ownFaints=%d foeFaints=%d",
            board, turns, gBattleWeather, gBattleStruct->weatherDuration, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            playerLeft->statStages[STAT_SPATK], playerRight->statStages[STAT_SPATK],
            playerLeft->statStages[STAT_ATK], playerRight->statStages[STAT_ATK],
            playerLeft->statStages[STAT_SPEED], playerRight->statStages[STAT_SPEED],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
    }
}
