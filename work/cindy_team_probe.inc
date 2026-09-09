// Temporary exact-party investment comparison, not a permanent trainer lock.
#line 51000 "work/cindy_team_probe.inc"
#include "emerald_champions_battle_sets.h"

static void CindyTeamPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

AI_DOUBLE_BATTLE_TEST("EC live review Cindy team: Milcery offense versus native special bulk")
{
    bool32 split;
    u32 turns;
    PARAMETRIZE { split = FALSE; turns = 1; }
    PARAMETRIZE { split = TRUE;  turns = 1; }
    PARAMETRIZE { split = FALSE; turns = 2; }
    PARAMETRIZE { split = TRUE;  turns = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet charmander = {
            .moves = {MOVE_FLAMETHROWER, MOVE_DRAGON_PULSE, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_BLAZE,
            .statPoints = {32, 0, 0, 32, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet electrike = {
            .moves = {MOVE_THUNDERBOLT, MOVE_FLAMETHROWER, MOVE_SNARL, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_LIGHTNING_ROD,
            .statPoints = {32, 0, 0, 32, 2, 0},
        };
        CindyTeamPlayer(SPECIES_CHARMANDER, &charmander);
        CindyTeamPlayer(SPECIES_ELECTRIKE, &electrike);
        AuthoredOpponent(TRAINER_CINDY_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SNUBBULL);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_MILCERY);
        EXPECT_EQ(GetNature(&party[1]), NATURE_BOLD);
        EXPECT_EQ(GetMonAbility(&party[1]), ABILITY_AROMA_VEIL);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_HELD_ITEM), ITEM_EVIOLITE);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_HP_EV), 32);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_DEF_EV), 2);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_FURFROU);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_SLAKOTH);
        // Directly edit the two native Stat Point storage fields, then use
        // native stat generation. Do not regenerate species, moves, ability,
        // nature, level, personality, items or other party members.
        u8 specialAttack = split ? 16 : 0;
        u8 specialDefense = split ? 16 : 32;
        SetMonData(&party[1], MON_DATA_SPATK_EV, &specialAttack);
        SetMonData(&party[1], MON_DATA_SPDEF_EV, &specialDefense);
        CalculateMonStats(&party[1]);
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++) {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("CINDY_TEAM_INPUT split=%d turns=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d",
                    split, turns, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++)
            TURN {
                MOVE(playerLeft, MOVE_FLAMETHROWER, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                // Same native c1 special-pressure line as the lead baseline.
                // All opponent actions and switches remain unrestricted.
            }
    } THEN {
        Test_MgbaPrintf("CINDY_TEAM_AFTER split=%d turns=%d nativeTurns=%d species=%d/%d HP=%d/%d foeHP=%d/%d LAST=%S/%S party=%d/%d",
            split, turns, gBattleResults.battleTurnCounter,
            opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3]);
        Test_MgbaPrintf("CINDY_TEAM_PARTY split=%d turns=%d SnubbullHP=%d MilceryHP=%d FurfrouHP=%d SlakothHP=%d",
            split, turns, GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][2], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][3], MON_DATA_HP));
    }
}
