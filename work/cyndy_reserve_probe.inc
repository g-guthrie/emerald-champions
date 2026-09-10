// Temporary actual E0017 reserves. Include after cyndy_lead_probe.inc.
// Leads are defeated; the two native reserves and their complete menus remain.
#line 52000 "work/cyndy_reserve_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Cyndy reserves: physical pressure and Psychic counterplay")
{
    bool32 psychic;
    u32 turns;
    PARAMETRIZE { psychic = FALSE; turns = 1; }
    PARAMETRIZE { psychic = FALSE; turns = 2; }
    PARAMETRIZE { psychic = FALSE; turns = 3; }
    PARAMETRIZE { psychic = FALSE; turns = 4; }
    PARAMETRIZE { psychic = TRUE; turns = 1; }
    PARAMETRIZE { psychic = TRUE; turns = 2; }
    PARAMETRIZE { psychic = TRUE; turns = 3; }
    PARAMETRIZE { psychic = TRUE; turns = 4; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet natu = {
            .moves = {MOVE_PSYCHIC, MOVE_HEAT_WAVE, MOVE_REFLECT, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_CALM, .ability = ABILITY_SYNCHRONIZE,
            .evs = {252, 0, 136, 0, 120, 0},
        };
        // All are accessible before this encounter: Route102 Pachi/Timburr/
        // Natu. The bulky Natu keeps a real Psychic weakness/control without
        // manufacturing two maximum-offense super-effective OHKOs.
        CyndyPlayer(SPECIES_PACHIRISU, &sCyndyPachirisu);
        CyndyPlayer(psychic ? SPECIES_NATU : SPECIES_TIMBURR, psychic ? &natu : &sCyndyTimburr);
        AuthoredOpponent(TRAINER_CYNDY_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_RIOLU);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_MEDITITE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_CRABRAWLER);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_CLOBBOPUS);
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
                Test_MgbaPrintf("CYNDY_RESERVE_INPUT psychic=%d owner=%d slot=%d species=%d level=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d moves=%d/%d/%d/%d",
                    psychic, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
                    GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
                    GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon),
                    GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                    GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (psychic)
            {
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_PSYCHIC, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
            else
            {
                MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("CYNDY_RESERVE_AFTER psychic=%d turns=%d nativeTurns=%d active=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S ownAtk=%d/%d ownDef=%d/%d foeTaunt=%d/%d ownStatus=%d/%d faints=%d/%d",
            psychic, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->statStages[STAT_ATK], opponentRight->statStages[STAT_ATK],
            opponentLeft->statStages[STAT_DEF], opponentRight->statStages[STAT_DEF],
            playerLeft->volatiles.tauntTimer, playerRight->volatiles.tauntTimer,
            opponentLeft->status1, opponentRight->status1,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("CYNDY_RESERVE_PARTY psychic=%d turns=%d slot=%d species=%d HP=%d/%d item=%d",
                psychic, turns, slot, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MAX_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HELD_ITEM));
        // No desired setup, Feint, Protect or KO is asserted before observation.
    }
}
