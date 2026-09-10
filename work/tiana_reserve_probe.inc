// Temporary native Tiana reserve positions. Include after AuthoredOpponent.
// No production changes, copied trainer sets, or prescribed opponent actions.
#line 23000 "work/tiana_reserve_probe.inc"
#include "emerald_champions_battle_sets.h"

static void TianaReservePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sTianaReserveTimburr = {
    .moves = {MOVE_ICE_PUNCH, MOVE_DRAIN_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_IRON_FIST,
    .evs = {252, 120, 136, 0, 0, 0},
};
static const struct EmeraldChampionsBattleSet sTianaReserveTorchic = {
    .moves = {MOVE_BODY_SLAM, MOVE_FLARE_BLITZ, MOVE_QUICK_ATTACK, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_BLAZE,
    .evs = {252, 120, 136, 0, 0, 0},
};
static const struct EmeraldChampionsBattleSet sTianaReservePachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};

AI_DOUBLE_BATTLE_TEST("EC live review Tiana reserves: physical burn pressure and recipient counterplay")
{
    u32 control;
    PARAMETRIZE { control = 0; } // Healthy physical Iron Fist: burn can weaken it.
    PARAMETRIZE { control = 1; } // Same stats/moves/item, but native Guts benefits.
    PARAMETRIZE { control = 2; } // Native Fire starter cannot be burned.
    PARAMETRIZE { control = 3; } // Existing poison prevents a second major status.
    PARAMETRIZE { control = 4; } // Actual turn-one Protect creates an Encore opportunity.
    GIVEN {
        struct EmeraldChampionsBattleSet timburr = sTianaReserveTimburr;
        if (control == 1)
            timburr.ability = ABILITY_GUTS;
        if (control == 2)
            TianaReservePlayer(SPECIES_TORCHIC, &sTianaReserveTorchic);
        else
            TianaReservePlayer(SPECIES_TIMBURR, &timburr);
        TianaReservePlayer(SPECIES_PACHIRISU, &sTianaReservePachirisu);
        if (control == 3)
        {
            // Reachable later-board status, not an invented derived stat.
            u32 poison = STATUS1_POISON;
            SetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_PLAYER][0], MON_DATA_STATUS, &poison);
        }
        AuthoredOpponent(TRAINER_TIANA, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_EEVEE);
        EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_ADAPTABILITY);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_BUNEARY);
        EXPECT_EQ(GetMonAbility(&party[3]), ABILITY_KLUTZ);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_HELD_ITEM), ITEM_FLAME_ORB);
        // Model a later position with the leads defeated. Keep the full party
        // and permute exact native records; first-living initialization must
        // actually send Eevee/Buneary, not silently return to Skitty/Swirlix.
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
        // Timburr/Pachirisu are current Route102 grass; Torchic is an actual
        // selectable starter, not a claim that all runs selected that starter.
        // Each player move is in that species' current preparation learnset.
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("TIANA_RESERVE_INPUT c=%d owner=%d slot=%d species=%d level=%d hp=%d atk=%d def=%d spa=%d spd=%d speed=%d item=%d ability=%d status=%d",
                    control, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon), GetMonData(mon, MON_DATA_STATUS));
            }
    } WHEN {
        // Do not force Switcheroo or ban Fake Out/Encore/attacks. Two turns
        // expose a post-entry choice while retaining real accumulated damage.
        // Suppress the player's punch/slam secondary only to avoid a sampled
        // freeze/paralysis hiding the opponent action we are investigating.
        TURN {
            if (control == 4)
                MOVE(playerLeft, MOVE_PROTECT);
            else
                MOVE(playerLeft, control == 2 ? MOVE_BODY_SLAM : MOVE_ICE_PUNCH,
                    target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_SUPER_FANG, target: opponentRight, hit: TRUE);
        }
        TURN {
            // Control 4 uses genuinely executed Protect from the preceding
            // turn, not an injected last move/Encore volatile. The human now
            // requests an attack; fast Encore may replace it natively.
            MOVE(playerLeft, control == 2 ? MOVE_BODY_SLAM : MOVE_ICE_PUNCH,
                target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_SUPER_FANG, target: opponentRight, hit: TRUE);
        }
    } THEN {
        EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_1], 0);
        EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_3], 1);
        EXPECT_EQ(opponentLeft->species, SPECIES_EEVEE);
        EXPECT_EQ(opponentRight->species, SPECIES_BUNEARY);
        Test_MgbaPrintf("TIANA_RESERVE_AFTER c=%d LAST=%S/%S foeLAST=%S/%S hp=%d/%d foeHP=%d/%d items=%d/%d foeItems=%d/%d statuses=%d/%d foeStatuses=%d/%d",
            control, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            opponentLeft->item, opponentRight->item, playerLeft->item, playerRight->item,
            opponentLeft->status1, opponentRight->status1, playerLeft->status1, playerRight->status1);
        // Read the root's decision-time logger for each chosen target/action.
        // THEN can already contain next-turn decisions, and a last move can
        // persist across an action denied by fainting/flinch. No hope-locks:
        // this records the baseline, not a tactical-success verdict.
    }
}
