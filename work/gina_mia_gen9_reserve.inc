// Temporary E0014 healthy reserves, native party-size timing and level
// calibration. Include after GinaMiaLeadPlayer; no trainer choice is forced.
#line 61000 "work/gina_mia_finish_probe.inc"
#include "main.h"
#include "battle_ai_switch.h"

static void GinaFinishPrint(u32 context)
{
    for (u32 slot = 0; slot < 4; slot++)
    {
        struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][slot];
        Test_MgbaPrintf("GINA_FINISH_INPUT context=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
            context, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
            GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
            GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
            GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Gina and Mia reserves: healthy insect reserves against physical and special bulk")
{
    bool32 special;
    bool32 screen;
    u32 turns;
    PARAMETRIZE { special = FALSE; screen = FALSE; turns = 1; }
    PARAMETRIZE { special = FALSE; screen = FALSE; turns = 2; }
    PARAMETRIZE { special = TRUE; screen = FALSE; turns = 1; }
    PARAMETRIZE { special = TRUE; screen = FALSE; turns = 2; }
    PARAMETRIZE { special = TRUE; screen = TRUE; turns = 2; }
    PARAMETRIZE { special = TRUE; screen = TRUE; turns = 3; }
    PARAMETRIZE { special = TRUE; screen = TRUE; turns = 4; }
    GIVEN {
        const struct EmeraldChampionsBattleSet pachi = {
            .moves = {MOVE_THUNDERBOLT, screen ? MOVE_LIGHT_SCREEN : MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        static const struct EmeraldChampionsBattleSet timburr = {
            .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        static const struct EmeraldChampionsBattleSet lotad = {
            .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_CALM, .ability = ABILITY_RAIN_DISH,
            .evs = {252, 0, 4, 0, 252, 0},
        };
        GinaMiaLeadPlayer(SPECIES_PACHIRISU, &pachi);
        EXPECT(CanSpeciesUseEmeraldChampionsPreparationMove(SPECIES_PACHIRISU, MOVE_LIGHT_SCREEN));
        GinaMiaLeadPlayer(special ? SPECIES_LOTAD : SPECIES_TIMBURR, special ? &lotad : &timburr);
        AuthoredOpponent(TRAINER_GINA_AND_MIA_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PLUSLE);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_MINUN);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_VOLBEAT);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_ILLUMISE);
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
        GinaFinishPrint(special);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (screen && turn == 0)
                MOVE(playerLeft, MOVE_LIGHT_SCREEN);
            else
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, special ? MOVE_GIGA_DRAIN : MOVE_ICE_PUNCH, target: opponentRight,
                hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("GINA_SCREEN_CONTEXT screen=%d special=%d turns=%d timer=%d HP=%d/%d SpA=%d/%d",
            screen, special, turns, gSideTimers[B_SIDE_PLAYER].lightscreenTimer,
            opponentLeft->hp, opponentRight->hp, opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK]);
        Test_MgbaPrintf("GINA_FINISH_RESERVES special=%d turns=%d actualTurns=%d species=%d/%d ownHP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S ownSpA=%d/%d ownStatus=%d/%d faints=%d/%d",
            special, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            opponentLeft->status1, opponentRight->status1,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
    }
}
