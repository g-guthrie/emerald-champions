// Temporary native comparison, using actual Tiana and native player stats.
#line 24000 "work/tiana_drain_probe.inc"
#include "emerald_champions_battle_sets.h"

static void TianaDrainPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

DOUBLE_BATTLE_TEST("EC live review Tiana drain: native healing versus coverage under pressure")
{
    bool32 kiss;
    PARAMETRIZE { kiss = FALSE; }
    PARAMETRIZE { kiss = TRUE; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet timburr = {
            .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_KNOCK_OFF, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_BRAVE, .ability = ABILITY_IRON_FIST,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet pachirisu = {
            .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        TianaDrainPlayer(SPECIES_TIMBURR, &timburr);
        TianaDrainPlayer(SPECIES_PACHIRISU, &pachirisu);
        AuthoredOpponent(TRAINER_TIANA, 0, FALSE);
        struct Pokemon *swirlix = &gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][1];
        EXPECT_EQ(GetMonData(swirlix, MON_DATA_SPECIES), SPECIES_SWIRLIX);
        EXPECT_EQ(GetMonData(swirlix, MON_DATA_MOVE1), MOVE_DRAINING_KISS);
        // A legal injured board, just above native Sitrus's activation threshold.
        // Do not inject stats, berry consumption or Unburden volatiles.
        u16 hp = (GetMonData(swirlix, MON_DATA_MAX_HP) + 1) / 2 + 1;
        SetMonData(swirlix, MON_DATA_HP, &hp);
        Test_MgbaPrintf("Tiana drain input kiss=%d SwirlixHP=%d/%d Speed=%d SpA=%d Def=%d",
            kiss, hp, GetMonData(swirlix, MON_DATA_MAX_HP), GetMonData(swirlix, MON_DATA_SPEED),
            GetMonData(swirlix, MON_DATA_SPATK), GetMonData(swirlix, MON_DATA_DEF));
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ICE_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, kiss ? MOVE_DRAINING_KISS : MOVE_THUNDERBOLT,
                target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            if (!kiss)
                SEND_OUT(opponentRight, 2);
        }
    } THEN {
        struct Pokemon *swirlix = &gParties[B_TRAINER_OPPONENT_A][1];
        if (kiss)
        {
            EXPECT_GT(GetMonData(swirlix, MON_DATA_HP), 0);
            EXPECT_EQ(opponentRight->species, SPECIES_SWIRLIX);
            EXPECT_EQ(opponentRight->item, ITEM_NONE);
            EXPECT_EQ((u32)opponentRight->volatiles.unburdenActive, 1);
        }
        else
            EXPECT_EQ(GetMonData(swirlix, MON_DATA_HP), 0);
        Test_MgbaPrintf("Tiana drain result kiss=%d originalSwirlixHP=%d item=%d active=%d HP=%d/%d foeHP=%d/%d Unburden=%d",
            kiss, GetMonData(swirlix, MON_DATA_HP), GetMonData(swirlix, MON_DATA_HELD_ITEM),
            opponentRight->species, opponentRight->hp, opponentRight->maxHP,
            playerLeft->hp, playerRight->hp, opponentRight->volatiles.unburdenActive);
        // Native execution decides whether the hypothesized replacement is
        // required. A failed fixture is not evidence of a gameplay freeze.
    }
}
