// Temporary actual Billy reserve diagnostics; include after AuthoredOpponent.
#line 33000 "work/billy_reserve_probe.inc"
#include "emerald_champions_battle_sets.h"

static void BillyReservePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sBillyReservePachirisu = {
    .moves = {MOVE_ROUND, MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .statPoints = {32, 0, 32, 0, 2, 0},
};
static const struct EmeraldChampionsBattleSet sBillyReserveTimburr = {
    .moves = {MOVE_ICE_PUNCH, MOVE_DRAIN_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
    .statPoints = {32, 0, 32, 0, 2, 0},
};

AI_DOUBLE_BATTLE_TEST("EC live review Billy reserves: copied moves contact counter and genuine Protect history")
{
    u32 control;
    u32 turns;
    PARAMETRIZE { control = 0; turns = 2; }
    PARAMETRIZE { control = 1; turns = 2; } // Helmet is the only player loadout difference.
    PARAMETRIZE { control = 2; turns = 2; } // Real prior Protect, not injected Encore history.
    PARAMETRIZE { control = 1; turns = 3; } // Execute the observed third-turn Helmet decision.
    GIVEN {
        struct EmeraldChampionsBattleSet pachirisu = sBillyReservePachirisu;
        if (control == 1)
            pachirisu.item = ITEM_ROCKY_HELMET;
        BillyReservePlayer(SPECIES_PACHIRISU, &pachirisu);
        BillyReservePlayer(SPECIES_TIMBURR, &sBillyReserveTimburr);
        AuthoredOpponent(TRAINER_BILLY, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_DITTO);
        EXPECT_EQ(GetMonAbility(&party[2]), ABILITY_IMPOSTER);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_HELD_ITEM), ITEM_CHOICE_SCARF);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_TANDEMAUS);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_HELD_ITEM), ITEM_WIDE_LENS);
        // Keep exact generated records and defeated leads. Reorder records,
        // not just indexes: first-living initialization must send the reserves.
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
                Test_MgbaPrintf("BILLY_RESERVE_INPUT c=%d owner=%d slot=%d species=%d level=%d HP=%d maxHP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
                    control, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_ATK),
                    GetMonData(mon, MON_DATA_DEF), GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF),
                    GetMonData(mon, MON_DATA_SPEED), GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
            }
        // Both player species are native Route102 finds; all eight moves are
        // in the current preparation corpus. No copied boosts or HP are forged.
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ROUND, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (control == 2)
                MOVE(playerRight, MOVE_PROTECT);
            else
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
        TURN {
            MOVE(playerLeft, MOVE_ROUND, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
        if (turns == 3)
        {
            TURN {
                MOVE(playerLeft, MOVE_ROUND, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
        // Actual AI decides all copied attacks, Population Bomb targets,
        // Super Fang, Encore and Protect. Native contact retaliation remains.
    } THEN {
        Test_MgbaPrintf("BILLY_RESERVE_AFTER c=%d turns=%d species=%d/%d transformed=%d HP=%d/%d maxHP=%d copiedAtk=%d copiedDef=%d copiedSpeed=%d ability=%d item=%d choice=%S LAST=%S/%S playerHP=%d/%d encore=%d/%d",
            control, turns, opponentLeft->species, opponentRight->species, opponentLeft->volatiles.transformed,
            opponentLeft->hp, opponentRight->hp, opponentLeft->maxHP, opponentLeft->attack, opponentLeft->defense,
            opponentLeft->speed, GetBattlerAbility(B_BATTLER_1), opponentLeft->item,
            GetMoveName(gBattleStruct->choicedMove[B_BATTLER_1]),
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]), playerLeft->hp, playerRight->hp,
            playerLeft->volatiles.encoreTimer, playerRight->volatiles.encoreTimer);
        Test_MgbaPrintf("BILLY_RESERVE_STATUS c=%d turns=%d nativeTurns=%d ownStatus=%d/%d playerStatus=%d/%d ownParty=%d/%d originalTandHP=%d Helmet=%d",
            control, turns, gBattleResults.battleTurnCounter, opponentLeft->status1, opponentRight->status1,
            playerLeft->status1, playerRight->status1, gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP), playerLeft->item);
        for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
            Test_MgbaPrintf("BILLY_IMPOSTER_MOVES c=%d slot=%d move=%S PP=%d sourceMove=%S sourcePP=%d",
                control, slot, GetMoveName(opponentLeft->moves[slot]), opponentLeft->pp[slot],
                GetMoveName(playerRight->moves[slot]), playerRight->pp[slot]);
        // Root's decision-time trace is authoritative for chosen targets.
        // THEN can already contain the next decision. Native diagnostic PASS
        // alone does not prove optimal choice, hit-count odds or campaign wins.
    }
}
