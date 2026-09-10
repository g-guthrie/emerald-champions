// Temporary actual E14 reserve mechanics and separate unrestricted decisions.
#line 54000 "work/gina_mia_roost_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "battle_ai_main.h"
#include "battle_util.h"
#include "random.h"

static void GinaRoostPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static void GinaRoostBoard(void)
{
    static const struct EmeraldChampionsBattleSet pachirisu = {
        .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
        .item = ITEM_EVIOLITE, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
        .statPoints = {32, 0, 32, 0, 2, 0},
    };
    static const struct EmeraldChampionsBattleSet lotad = {
        .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_REST, MOVE_PROTECT},
        .item = ITEM_EVIOLITE, .nature = NATURE_BOLD, .ability = ABILITY_RAIN_DISH,
        .statPoints = {32, 0, 32, 0, 2, 0},
    };
    // Both species and full move menus are native early preparation options.
    // Pachirisu cannot evolve, so its held Eviolite has no defensive effect.
    GinaRoostPlayer(SPECIES_PACHIRISU, &pachirisu);
    GinaRoostPlayer(SPECIES_LOTAD, &lotad);
    AuthoredOpponent(TRAINER_GINA_AND_MIA_1, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PLUSLE);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_MINUN);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_VOLBEAT);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_ILLUMISE);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_MOVE2), MOVE_ROOST);
    EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_PRANKSTER);
    u16 zero = 0, injury = 25;
    SetMonData(&party[0], MON_DATA_HP, &zero);
    SetMonData(&party[1], MON_DATA_HP, &zero);
    EXPECT_GT(GetMonData(&party[2], MON_DATA_MAX_HP), injury);
    SetMonData(&party[2], MON_DATA_HP, &injury);
    for (u32 slot = 0; slot < 2; slot++)
    {
        struct Pokemon saved = party[slot];
        party[slot] = party[slot + 2];
        party[slot + 2] = saved;
    }
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
    for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
        for (u32 slot = 0; slot < 2; slot++)
        {
            struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
            Test_MgbaPrintf("GINA_ROOST_INPUT owner=%d slot=%d species=%d HP=%d/%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_MAX_HP),
                GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF), GetMonData(mon, MON_DATA_SPATK),
                GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED), GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
        }
}

DOUBLE_BATTLE_TEST("EC live review Gina Roost: forced native recovery versus immediate damage", s16 healing, s16 incoming, s16 outgoing)
{
    bool32 roost;
    PARAMETRIZE { roost = TRUE; }
    PARAMETRIZE { roost = FALSE; }
    GIVEN { GinaRoostBoard(); }
    WHEN {
        TURN {
            MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_HELPING_HAND, target: opponentLeft);
            if (roost)
                MOVE(opponentLeft, MOVE_ROOST);
            else
                MOVE(opponentLeft, MOVE_BUG_BUZZ, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (roost)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ROOST, opponentLeft);
            HP_BAR(opponentLeft, captureDamage: &results[i].healing);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERBOLT, playerLeft);
        HP_BAR(opponentLeft, captureDamage: &results[i].incoming);
        if (!roost)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BUG_BUZZ, opponentLeft);
            HP_BAR(playerLeft, captureDamage: &results[i].outgoing);
        }
    } THEN {
        EXPECT_GT(results[i].incoming, 0);
        if (roost)
            EXPECT_LT(results[i].healing, 0);
        else
            EXPECT_GT(results[i].outgoing, 0);
        EXPECT_EQ(opponentLeft->hp, 25 - results[i].healing - results[i].incoming);
        Test_MgbaPrintf("GINA_ROOST_NATIVE roost=%d healing=%d incoming=%d outgoing=%d HP=%d/%d foes=%d/%d LAST=%S/%S status=%d/%d",
            roost, results[i].healing, results[i].incoming, results[i].outgoing,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]), opponentLeft->status1, playerLeft->status1);
        // These forced actions prove native recovery/damage, not AI preference.
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Gina Roost: actual injured reserve choice and followthrough")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    GIVEN { GinaRoostBoard(); }
    WHEN {
        for (u32 turn = 0; turn < turns; turn++)
        {
            TURN {
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_PROTECT);
            }
        }
    } THEN {
        Test_MgbaPrintf("GINA_ROOST_AI turns=%d HP=%d/%d foes=%d/%d LAST=%S/%S SpAstages=%d/%d status=%d/%d faints=%d/%d",
            turns, opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            opponentLeft->status1, opponentRight->status1, gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        if (turns == 1 && opponentLeft->hp && opponentRight->hp)
        {
            // An initialized, genuinely reached post-turn board. Disable only
            // Roost PP for a value query, not an alternative human command.
            // This is not the initial 25-HP board, nor a timing measurement.
            EXPECT_EQ(opponentLeft->species, SPECIES_VOLBEAT);
            EXPECT_EQ(opponentLeft->moves[1], MOVE_ROOST);
            rng_value_t rng = gRngValue, rng2 = gRng2Value;
            u8 savedPP = opponentLeft->pp[1];
            SetAiLogicDataForTurn(gAiLogicData);
            s32 available = AI_EvaluateDoublesPosition(B_BATTLER_1, 0);
            opponentLeft->pp[1] = 0;
            SetAiLogicDataForTurn(gAiLogicData);
            s32 unavailable = AI_EvaluateDoublesPosition(B_BATTLER_1, 0);
            opponentLeft->pp[1] = savedPP;
            SetAiLogicDataForTurn(gAiLogicData);
            gRngValue = rng;
            gRng2Value = rng2;
            Test_MgbaPrintf("GINA_ROOST_PAIR_QUERY available=%d unavailable=%d HP=%d/%d pp=%d", available, unavailable,
                opponentLeft->hp, opponentRight->hp, savedPP);
            // Raw move opinions can affect value even without a healing model.
            // Neither equal scores nor a difference alone proves the omission.
        }
    }
}
