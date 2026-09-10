// Temporary unrestricted observation: a revenge opportunity is not guaranteed.
#line 57000 "work/cindy_retaliate_ai_probe.inc"
#include "emerald_champions_battle_sets.h"

static void CindyRevengeAiPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon));
        Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
              GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Cindy natural revenge: unrestricted two-turn response to focused Milcery pressure")
{
    bool32 focusRight;
    PARAMETRIZE { focusRight = TRUE; }
    PARAMETRIZE { focusRight = FALSE; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet pachirisu = {
            .moves = {MOVE_SUPER_FANG, MOVE_ROUND, MOVE_FOLLOW_ME, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        static const struct EmeraldChampionsBattleSet eevee = {
            .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
            .evs = {252, 252, 4, 0, 0, 0},
        };
        CindyRevengeAiPlayer(SPECIES_PACHIRISU, &pachirisu);
        CindyRevengeAiPlayer(SPECIES_EEVEE, &eevee);
        AuthoredOpponent(TRAINER_CINDY_1, 0, FALSE);
        // Read the current materialized party, including its actual moves and
        // points. All four records remain alive and wholly unchanged.
        for (u32 slot = 0; slot < 4; slot++) {
            struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][slot];
            Test_MgbaPrintf("CINDY_REVENGE_AI_INPUT slot=%d species=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d moves=%S/%S/%S/%S",
                slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF), GetMonData(mon, MON_DATA_SPATK),
                GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                GetMoveName(GetMonData(mon, MON_DATA_MOVE1)), GetMoveName(GetMonData(mon, MON_DATA_MOVE2)),
                GetMoveName(GetMonData(mon, MON_DATA_MOVE3)), GetMoveName(GetMonData(mon, MON_DATA_MOVE4)));
        }
    } WHEN {
        for (u32 turn = 0; turn < 2; turn++)
            TURN {
                MOVE(playerLeft, MOVE_SUPER_FANG, target: focusRight ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_DOUBLE_EDGE, target: focusRight ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                // The AI can redirect its strategy, switch, attack or recover.
                // Its native controller chooses replacements if a foe faints;
                // no forced opponent command or send-out creates a timer.
            }
    } THEN {
        Test_MgbaPrintf("CINDY_REVENGE_AI_AFTER focusRight=%d turns=%d species=%d/%d HP=%d/%d foeHP=%d/%d LAST=%S/%S party=%d/%d retaliateTimer=%d faintCount=%d choices=%S/%S",
            focusRight, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            gSideTimers[B_SIDE_OPPONENT].retaliateTimer, gBattleResults.opponentFaintCounter,
            GetMoveName(gBattleStruct->choicedMove[B_BATTLER_1]), GetMoveName(gBattleStruct->choicedMove[B_BATTLER_3]));
        Test_MgbaPrintf("CINDY_REVENGE_AI_PARTY SnubbullHP=%d MilceryHP=%d FurfrouHP=%d SlakothHP=%d",
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][3], MON_DATA_HP));
        // Absence of a natural Slakoth entry or Retaliate selection is a valid
        // observation, not grounds to force a desired revenge story.
    }
}
