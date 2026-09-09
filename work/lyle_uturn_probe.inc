// Temporary exact-party U-turn decisions and a separate forced reset control.
#line 62000 "work/lyle_uturn_probe.inc"
#include "emerald_champions_battle_sets.h"

static void LylePivotPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sLylePivotEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
    .statPoints = {32, 32, 2, 0, 0, 0},
};
static const struct EmeraldChampionsBattleSet sLylePivotTimburr = {
    .moves = {MOVE_ICE_PUNCH, MOVE_DRAIN_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
    .statPoints = {32, 0, 32, 0, 2, 0},
};
static const struct EmeraldChampionsBattleSet sLylePivotCharmander = {
    .moves = {MOVE_FLAMETHROWER, MOVE_DRAGON_PULSE, MOVE_WILL_O_WISP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_BLAZE,
    .statPoints = {32, 0, 0, 32, 2, 0},
};
static const struct EmeraldChampionsBattleSet sLylePivotPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .statPoints = {32, 0, 32, 0, 2, 0},
};

static void LylePivotOpponent(bool32 uturn)
{
    AuthoredOpponent(TRAINER_LYLE, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_NYMBLE);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_MOVE1), MOVE_FIRST_IMPRESSION);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_MOVE2), MOVE_LEECH_LIFE);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_MOVE4), MOVE_PROTECT);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_SEWADDLE);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_ANORITH);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_DOTTLER);
    u16 move = uturn ? MOVE_U_TURN : MOVE_SUCKER_PUNCH;
    u8 pp = GetMovePP(move);
    SetMonData(&party[0], MON_DATA_MOVE3, &move);
    SetMonData(&party[0], MON_DATA_PP3, &pp);
}

AI_DOUBLE_BATTLE_TEST("EC live review Lyle U-turn: unrestricted actual physical and special pressure")
{
    bool32 special, uturn;
    u32 turns;
    PARAMETRIZE { special = FALSE; uturn = FALSE; turns = 2; }
    PARAMETRIZE { special = FALSE; uturn = TRUE;  turns = 2; }
    PARAMETRIZE { special = FALSE; uturn = FALSE; turns = 3; }
    PARAMETRIZE { special = FALSE; uturn = TRUE;  turns = 3; }
    PARAMETRIZE { special = TRUE;  uturn = FALSE; turns = 3; }
    PARAMETRIZE { special = TRUE;  uturn = TRUE;  turns = 3; }
    GIVEN {
        LylePivotPlayer(special ? SPECIES_CHARMANDER : SPECIES_EEVEE,
            special ? &sLylePivotCharmander : &sLylePivotEevee);
        LylePivotPlayer(special ? SPECIES_PACHIRISU : SPECIES_TIMBURR,
            special ? &sLylePivotPachirisu : &sLylePivotTimburr);
        LylePivotOpponent(uturn);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, special ? MOVE_FLAMETHROWER : MOVE_DOUBLE_EDGE,
                target: special ? opponentRight : opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, special ? MOVE_THUNDERBOLT : MOVE_ICE_PUNCH,
                target: special ? opponentLeft : opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("LYLE_PIVOT_AI special=%d uturn=%d turns=%d nativeTurns=%d species=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S party=%d/%d",
            special, uturn, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3]);
        Test_MgbaPrintf("LYLE_PIVOT_PARTY special=%d uturn=%d turns=%d Nymble=%d Sewaddle=%d Anorith=%d Dottler=%d",
            special, uturn, turns, GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][3], MON_DATA_HP));
        // Voluntary switches, automatic replacements and no-pivot choices are
        // all observations. No opponent command/entry is forced in this group.
    }
}

DOUBLE_BATTLE_TEST("EC live review Lyle U-turn mechanics: native exit re-entry restores First Impression", s16 first, s16 repeated)
{
    bool32 uturn;
    PARAMETRIZE { uturn = TRUE; }
    GIVEN {
        LylePivotPlayer(SPECIES_PACHIRISU, &sLylePivotPachirisu);
        LylePivotPlayer(SPECIES_EEVEE, &sLylePivotEevee);
        LylePivotOpponent(uturn);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_FIRST_IMPRESSION, target: playerLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentRight, MOVE_PROTECT);
        }
        TURN {
            MOVE(playerLeft, MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_U_TURN, target: playerLeft, hit: TRUE, criticalHit: FALSE);
            SEND_OUT(opponentLeft, 3);
            MOVE(opponentRight, MOVE_PROTECT);
        }
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            SWITCH(opponentLeft, 0);
            MOVE(opponentRight, MOVE_PROTECT);
        }
        TURN {
            MOVE(playerLeft, MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_FIRST_IMPRESSION, target: playerLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentRight, MOVE_PROTECT);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIRST_IMPRESSION, opponentLeft);
        HP_BAR(playerLeft, captureDamage: &results[i].first);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_U_TURN, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIRST_IMPRESSION, opponentLeft);
        HP_BAR(playerLeft, captureDamage: &results[i].repeated);
    } THEN {
        EXPECT_GT(results[i].first, 0);
        EXPECT_GT(results[i].repeated, 0);
        EXPECT_EQ(opponentLeft->species, SPECIES_NYMBLE);
        EXPECT_EQ(gBattlerPartyIndexes[B_BATTLER_1], 0);
        EXPECT_EQ(gLastMoves[B_BATTLER_1], MOVE_FIRST_IMPRESSION);
        Test_MgbaPrintf("LYLE_PIVOT_FIXED first=%d reentryFI=%d NymbleHP=%d PachiHP=%d NymblePP=%d turns=%d",
            results[i].first, results[i].repeated, opponentLeft->hp, playerLeft->hp,
            opponentLeft->pp[0], gBattleResults.battleTurnCounter);
        // This proves native move eligibility after a real pivot and return;
        // it is explicitly NOT evidence that the unrestricted AI plans it.
    }
}
