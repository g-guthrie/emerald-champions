// Temporary unrestricted native coverage comparison, not a must-Punch script.
#line 54000 "work/cindy_coverage_probe.inc"
#include "emerald_champions_battle_sets.h"

static void CindyCoveragePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

AI_DOUBLE_BATTLE_TEST("EC live review Cindy coverage: actual Fire Fang versus Fire Punch with Helping Hand available")
{
    bool32 punch;
    u32 turns;
    PARAMETRIZE { punch = FALSE; turns = 1; }
    PARAMETRIZE { punch = TRUE; turns = 1; }
    PARAMETRIZE { punch = FALSE; turns = 2; }
    PARAMETRIZE { punch = TRUE; turns = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet ferroseed = {
            .moves = {MOVE_IRON_HEAD, MOVE_SEED_BOMB, MOVE_LEECH_SEED, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_RELAXED, .ability = ABILITY_IRON_BARBS,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet sewaddle = {
            .moves = {MOVE_BUG_BITE, MOVE_SEED_BOMB, MOVE_STRUGGLE_BUG, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_OVERCOAT,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        // Both species are in first-visit Woods grass, not gated behind Rock
        // Smash. Ferroseed's actual preparation list has Seed Bomb, not an
        // assumed Power Whip; all four moves and Iron Barbs are native legal.
        CindyCoveragePlayer(SPECIES_FERROSEED, &ferroseed);
        CindyCoveragePlayer(SPECIES_SEWADDLE, &sewaddle);
        AuthoredOpponent(TRAINER_CINDY_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SNUBBULL);
        EXPECT_EQ(GetMonAbility(&party[0]), ABILITY_INTIMIDATE);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_MILCERY);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_MOVE3), MOVE_HELPING_HAND);
        u16 move = punch ? MOVE_FIRE_PUNCH : MOVE_FIRE_FANG;
        u8 pp = GetMovePP(move);
        SetMonData(&party[0], MON_DATA_MOVE2, &move);
        SetMonData(&party[0], MON_DATA_PP2, &pp);
        // Every other current compiled field is identical between variants;
        // all four opponents remain alive, including normal switch options.
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < 2; slot++) {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("CINDY_COVER_INPUT punch=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d",
                    punch, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_IRON_HEAD, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_BUG_BITE, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        Test_MgbaPrintf("CINDY_COVER_AFTER punch=%d turns=%d species=%d/%d HP=%d/%d foeHP=%d/%d foeStatus=%d/%d LAST=%S/%S party=%d/%d",
            punch, turns, opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp, playerLeft->status1, playerRight->status1,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3]);
        // This observes whether native AI selects Helping Hand and coverage.
        // No choice, flinch, knockout or superiority is asserted in advance.
    }
}
