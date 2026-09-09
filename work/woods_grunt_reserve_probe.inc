// Temporary current-party E0010 reserve observations, not a trainer lock.
#line 11000 "work/woods_grunt_reserve_probe.inc"
#include "emerald_champions_battle_sets.h"

static void WoodsGruntReservePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

AI_DOUBLE_BATTLE_TEST("EC live review Woods grunt reserves: actual physical and special pressure")
{
    bool32 special;
    u32 turns, candidate;
    // bit0: Poochyena Speed -> SpD; bit1: Grimer item-only Eviolite.
    PARAMETRIZE { special = FALSE; turns = 1; candidate = 0; }
    PARAMETRIZE { special = FALSE; turns = 2; candidate = 0; }
    PARAMETRIZE { special = TRUE;  turns = 1; candidate = 0; }
    PARAMETRIZE { special = TRUE;  turns = 2; candidate = 0; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet eevee = {
            .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet timburr = {
            .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet natu = {
            .moves = {MOVE_PSYCHIC, MOVE_HEAT_WAVE, MOVE_REFLECT, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_SYNCHRONIZE,
            .statPoints = {32, 0, 0, 32, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet charmander = {
            .moves = {MOVE_FLAMETHROWER, MOVE_DRAGON_PULSE, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_BLAZE,
            .statPoints = {32, 0, 0, 32, 2, 0},
        };
        WoodsGruntReservePlayer(special ? SPECIES_NATU : SPECIES_EEVEE, special ? &natu : &eevee);
        WoodsGruntReservePlayer(special ? SPECIES_CHARMANDER : SPECIES_TIMBURR, special ? &charmander : &timburr);
        AuthoredOpponent(TRAINER_GRUNT_PETALBURG_WOODS, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_CARVANHA);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_WAILMER);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_GRIMER);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_POOCHYENA);
        u16 zero = 0;
        SetMonData(&party[0], MON_DATA_HP, &zero);
        SetMonData(&party[1], MON_DATA_HP, &zero);
        for (u32 slot = 0; slot < 2; slot++) {
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + 2];
            party[slot + 2] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
        // Explicit reserve phase: both original leads are fainted, not erased.
        // Baseline keeps all authored data. Candidate bits independently change
        // Poochyena's native points and/or Grimer's held item, never policy.
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < 2; slot++) {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("WOODS_RESERVE_INPUT candidate=%d special=%d turns=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d ability=%d item=%d",
                    candidate, special, turns, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonAbility(mon), GetMonData(mon, MON_DATA_HELD_ITEM));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, special ? MOVE_PSYCHIC : MOVE_DOUBLE_EDGE, target: opponentLeft,
                 hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, special ? MOVE_FLAMETHROWER : MOVE_DRAIN_PUNCH, target: opponentRight,
                 hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            // No opponent action, targeted support, KO or survival is forced.
        }
    } THEN {
        Test_MgbaPrintf("WOODS_RESERVE_AFTER candidate=%d special=%d turns=%d nativeTurns=%d species=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeSpA=%d/%d ownFaints=%d foeFaints=%d foeStatus=%d/%d",
            candidate, special, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            playerLeft->statStages[STAT_SPATK], playerRight->statStages[STAT_SPATK],
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter,
            playerLeft->status1, playerRight->status1);
    }
}
