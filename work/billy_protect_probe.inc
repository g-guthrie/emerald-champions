// Actual-team experiment; the sole hypothetical change is Ice Beam -> Protect.
#line 38000 "work/billy_protect_probe.inc"

AI_DOUBLE_BATTLE_TEST("EC live review Billy Protect: actual setter preservation versus fast Fighting pressure")
{
    bool32 protectVariant, flying;
    u32 turns;
    PARAMETRIZE { protectVariant = FALSE; flying = FALSE; turns = 1; }
    PARAMETRIZE { protectVariant = TRUE; flying = FALSE; turns = 1; }
    PARAMETRIZE { protectVariant = FALSE; flying = TRUE; turns = 1; }
    PARAMETRIZE { protectVariant = TRUE; flying = TRUE; turns = 1; }
    PARAMETRIZE { protectVariant = FALSE; flying = FALSE; turns = 2; }
    PARAMETRIZE { protectVariant = TRUE; flying = FALSE; turns = 2; }
    PARAMETRIZE { protectVariant = FALSE; flying = TRUE; turns = 2; }
    PARAMETRIZE { protectVariant = TRUE; flying = TRUE; turns = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet natu = {
            .moves = {MOVE_AIR_SLASH, MOVE_PSYCHIC, MOVE_ROOST, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_MAGIC_BOUNCE,
            .statPoints = {32, 0, 0, 0, 2, 32},
        };
        BillyProbePlayer(flying ? SPECIES_NATU : SPECIES_MIENFOO, flying ? &natu : &sBillyProbeMienfoo);
        BillyProbePlayer(SPECIES_PACHIRISU, &sBillyProbePachirisu);
        AuthoredOpponent(TRAINER_BILLY, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_LOTAD);
        if (protectVariant)
        {
            u16 move = MOVE_PROTECT;
            u8 pp = GetMovePP(move);
            SetMonData(&party[0], MON_DATA_MOVE4, &move);
            SetMonData(&party[0], MON_DATA_PP4, &pp);
        }
        // Native stats/ability/items remain unchanged. This does not assume
        // AI must choose Protect, or weaken the player to make rain happen.
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++)
            TURN {
                MOVE(playerLeft, flying ? MOVE_AIR_SLASH : MOVE_DRAIN_PUNCH, target: opponentLeft, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, flying ? MOVE_SUPER_FANG : MOVE_THUNDERBOLT,
                    target: flying ? opponentLeft : opponentRight, criticalHit: FALSE, secondaryEffect: FALSE);
            }
    } THEN {
        Test_MgbaPrintf("BILLY_PROTECT variant=%d flying=%d turns=%d LAST=%S/%S active=%d/%d HP=%d/%d originalLotadHP=%d originalWingullHP=%d foeHP=%d/%d weather=%d",
            protectVariant, flying, turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->species, opponentRight->species, opponentLeft->hp, opponentRight->hp,
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
            playerLeft->hp, playerRight->hp, gBattleWeather);
    }
}
