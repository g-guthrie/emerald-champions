// Temporary actual E0021 native observations; independent of prior encounters.
#line 51000 "work/jose_lead_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void JosePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    for (u32 index = 0; index < MAX_MON_MOVES; index++)
        if (set->moves[index] != MOVE_NONE)
            EXPECT(CanSpeciesUseEmeraldChampionsPreparationMove(species, set->moves[index]));
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
            GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

static const struct EmeraldChampionsBattleSet sJoseEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sJoseTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sJosePachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sJoseLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sJoseMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};

static const struct EmeraldChampionsBattleSet sJoseElectrike = {
    .moves = {MOVE_FLAMETHROWER, MOVE_THUNDERBOLT, MOVE_SNARL, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_LIGHTNING_ROD,
    .evs = {252, 0, 4, 0, 0, 252},
};

AI_DOUBLE_BATTLE_TEST("EC live review Jose leads: native pressure and immediate Tailwind crossing")
{
    bool32 fast;
    u32 turns;
    PARAMETRIZE { fast = FALSE; turns = 1; }
    PARAMETRIZE { fast = FALSE; turns = 2; }
    PARAMETRIZE { fast = FALSE; turns = 3; }
    PARAMETRIZE { fast = TRUE; turns = 1; }
    PARAMETRIZE { fast = TRUE; turns = 2; }
    PARAMETRIZE { fast = TRUE; turns = 3; }
    GIVEN {
        JosePlayer(fast ? SPECIES_ELECTRIKE : SPECIES_PACHIRISU, fast ? &sJoseElectrike : &sJosePachirisu);
        JosePlayer(fast ? SPECIES_LOTAD : SPECIES_TIMBURR, fast ? &sJoseLotad : &sJoseTimburr);
        AuthoredOpponent(TRAINER_JOSE, 0, FALSE);
        const enum Species expected[] = {SPECIES_DUSTOX, SPECIES_BEAUTIFLY, SPECIES_KRICKETUNE, SPECIES_VENONAT};
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++) {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                for (u32 index = 0; index < MAX_MON_MOVES; index++)
                    EXPECT(CanSpeciesUseEmeraldChampionsPreparationMove(GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_MOVE1 + index)));
                if (owner == B_TRAINER_OPPONENT_A)
                    EXPECT_EQ(GetMonData(mon, MON_DATA_SPECIES), expected[slot]);
                Test_MgbaPrintf("JOSE_INPUT fast=%d owner=%d slot=%d species=%d lv=%d HP=%d stats=%d/%d/%d/%d/%d item=%d ability=%d",
                    fast, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon));
                Test_MgbaPrintf("JOSE_MENU fast=%d owner=%d slot=%d moves=%d/%d/%d/%d",
                    fast, owner, slot, GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                    GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (fast) {
                MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        // Observe the new curve; the old forced Quiver expectation is not a game rule.
        Test_MgbaPrintf("JOSE_AFTER fast=%d t=%d native=%d active=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S faints=%d/%d",
            fast, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        Test_MgbaPrintf("JOSE_STATE fast=%d t=%d tailwind=%d SpA=%d/%d SpD=%d/%d Spe=%d/%d statuses=%d/%d items=%d/%d",
            fast, turns, gSideTimers[B_SIDE_OPPONENT].tailwindTimer,
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            opponentLeft->statStages[STAT_SPDEF], opponentRight->statStages[STAT_SPDEF],
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED],
            playerLeft->status1, playerRight->status1, opponentLeft->item, opponentRight->item);
        for (u32 slot = 0; slot < 4; slot++) {
            struct Pokemon *mon = &gParties[B_TRAINER_OPPONENT_A][slot];
            Test_MgbaPrintf("JOSE_PARTY fast=%d t=%d slot=%d species=%d HP=%d/%d item=%d",
                fast, turns, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP),
                GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_HELD_ITEM));
        }
    }
}
