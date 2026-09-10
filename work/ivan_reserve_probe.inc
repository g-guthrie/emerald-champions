// Temporary actual E15 reserve observations. No invented setup or move locks.
#line 52000 "work/ivan_reserve_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "battle_util.h"

static void IvanReservePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    for (u32 move = 0; move < 4; move++)
        EXPECT(CanSpeciesUseEmeraldChampionsPreparationMove(species, set->moves[move]));
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
            GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Ivan reserves: native physical and Grass Electric pressure")
{
    bool32 grass, allowParalysis;
    u32 turns;
    PARAMETRIZE { grass = FALSE; turns = 1; allowParalysis = TRUE; }
    PARAMETRIZE { grass = FALSE; turns = 2; allowParalysis = TRUE; }
    PARAMETRIZE { grass = TRUE; turns = 1; allowParalysis = TRUE; }
    PARAMETRIZE { grass = TRUE; turns = 2; allowParalysis = TRUE; }
    PARAMETRIZE { grass = FALSE; turns = 2; allowParalysis = FALSE; }
    PARAMETRIZE { grass = TRUE; turns = 2; allowParalysis = FALSE; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet eevee = {
            .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
            .evs = {252, 4, 0, 0, 252, 0},
        };
        static const struct EmeraldChampionsBattleSet timburr = {
            .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
            .evs = {252, 4, 0, 0, 252, 0},
        };
        static const struct EmeraldChampionsBattleSet foongus = {
            .moves = {MOVE_GIGA_DRAIN, MOVE_CLEAR_SMOG, MOVE_SPORE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_BOLD, .ability = ABILITY_REGENERATOR,
            .evs = {252, 0, 252, 4, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet pachirisu = {
            .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_CALM, .ability = ABILITY_VOLT_ABSORB,
            .evs = {252, 0, 4, 0, 252, 0},
        };
        // Eevee Route101, Timburr/Pachirisu Route102, Foongus first-visit Woods.
        // Full native prepared menus remain visible to the AI; no claim that
        // it knows these particular pending commands or that Grass always wins.
        IvanReservePlayer(grass ? SPECIES_FOONGUS : SPECIES_EEVEE, grass ? &foongus : &eevee);
        IvanReservePlayer(grass ? SPECIES_PACHIRISU : SPECIES_TIMBURR, grass ? &pachirisu : &timburr);
        AuthoredOpponent(TRAINER_IVAN, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_TENTACOOL);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_CLAMPERL);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_CHEWTLE);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_CHINCHOU);
        EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_STRONG_JAW);
        EXPECT_EQ(GetMonAbility(&party[3]), ABILITY_VOLT_ABSORB);
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
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("IVAN_RESERVE_INPUT grass=%d turns=%d owner=%d slot=%d species=%d level=%d HP=%d/%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                    grass, turns, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_ATK),
                    GetMonData(mon, MON_DATA_DEF), GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF),
                    GetMonData(mon, MON_DATA_SPEED), GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++)
        {
            TURN {
                MOVE(playerLeft, grass ? MOVE_GIGA_DRAIN : MOVE_DOUBLE_EDGE,
                    target: grass ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE, WITH_RNG(RNG_PARALYSIS, allowParalysis));
                MOVE(playerRight, grass ? MOVE_THUNDERBOLT : MOVE_DRAIN_PUNCH,
                    target: grass ? opponentLeft : opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE, WITH_RNG(RNG_PARALYSIS, allowParalysis));
            }
        }
    } THEN {
        Test_MgbaPrintf("IVAN_RESERVE_CONTEXT grass=%d turns=%d allowParalysis=%d", grass, turns, allowParalysis);
        Test_MgbaPrintf("IVAN_RESERVE_AFTER grass=%d turns=%d nativeTurns=%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S status=%d/%d foeStatus=%d/%d Atk=%d/%d SpA=%d/%d Speed=%d/%d accuracy=%d/%d faints=%d/%d",
            grass, turns, gBattleResults.battleTurnCounter, opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->status1, opponentRight->status1, playerLeft->status1, playerRight->status1,
            opponentLeft->statStages[STAT_ATK], opponentRight->statStages[STAT_ATK],
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            playerLeft->statStages[STAT_SPEED], playerRight->statStages[STAT_SPEED],
            playerLeft->statStages[STAT_ACC], playerRight->statStages[STAT_ACC],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("IVAN_RESERVE_PARTY grass=%d turns=%d slot=%d species=%d HP=%d/%d status=%d",
                grass, turns, slot, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP), GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MAX_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_STATUS));
        // No prescribed trainer move, KO, success rate, or full-fight claim.
    }
}
