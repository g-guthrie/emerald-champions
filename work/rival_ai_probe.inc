// Temporary native investigation; not part of the retained regression suite.
// The existing native DSL uses __LINE__ for symbols in this included fragment.
#line 1000 "work/rival_ai_probe.inc"
static void RivalReviewPlayer(enum Species species)
{
    struct Pokemon mon;
    struct EmeraldChampionsBattleSet preset = gEmeraldChampionsDefaultBattleSets[species];
    if (species == SPECIES_TORCHIC)
        preset = (struct EmeraldChampionsBattleSet) {
            .moves = {MOVE_FLAMETHROWER, MOVE_HELPING_HAND, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_SPEED_BOOST,
            .evs = {4, 0, 0, 252, 0, 252},
        };
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    ApplyEmeraldChampionsScriptedSet(&mon, &preset);
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon));
        Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
              GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

static void RivalReviewMienfoo(bool32 sash)
{
        struct EmeraldChampionsBattleSet threat = {
            .moves = {MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF, MOVE_U_TURN, MOVE_PROTECT},
            .item = sash ? ITEM_FOCUS_SASH : ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
            .evs = {4, 252, 0, 0, 0, 252},
        };
        for (u32 i = 0; i < 2; i++)
        {
            struct Pokemon mon;
            CreateRandomMonWithIVs(&mon, SPECIES_MIENFOO, 14, MAX_PER_STAT_IVS);
            ApplyEmeraldChampionsScriptedSet(&mon, &threat);
            PLAYER(SPECIES_MIENFOO) {
                *gBattleTestRunnerState->data.currentMon = mon;
                Nature(GetNature(&mon)); Ability(GetMonAbility(&mon));
                Speed(GetMonData(&mon, MON_DATA_SPEED));
                Moves(MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF, MOVE_U_TURN, MOVE_PROTECT);
            }
        }
}

AI_DOUBLE_BATTLE_TEST("EC live review E0001: actual setup and immunity support window")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    GIVEN {
        RivalReviewPlayer(SPECIES_PACHIRISU);
        RivalReviewPlayer(SPECIES_EEVEE);
        AuthoredOpponent(TRAINER_BRENDAN_ROUTE_103_MUDKIP, 0, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft);
            MOVE(playerRight, MOVE_YAWN, target: opponentLeft);
        }
        if (turns == 2)
            TURN {
                MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentLeft);
                MOVE(playerRight, MOVE_WISH);
            }
    } THEN {
        Test_MgbaPrintf("E1 setup turns=%d executed=%S/%S TreeckoAttackStage=%d playerHP=%d/%d opponentHP=%d/%d",
            turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->statStages[STAT_ATK], playerLeft->hp, playerRight->hp,
            opponentLeft->hp, opponentRight->hp);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review E0001: actual Treecko pair against faster Inner Focus pressure")
{
    bool32 sash;
    PARAMETRIZE { sash = FALSE; }
    PARAMETRIZE { sash = TRUE; }
    GIVEN {
        RivalReviewMienfoo(sash);
        AuthoredOpponent(TRAINER_BRENDAN_ROUTE_103_MUDKIP, 0, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft);
            MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight);
        }
    } THEN {
        Test_MgbaPrintf("E1 pressure sash=%d executed=%S/%S opponentHP=%d/%d playerHP=%d/%d playerSpeeds=%d/%d opponentSpeeds=%d/%d",
            sash,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            playerLeft->speed, playerRight->speed, opponentLeft->speed, opponentRight->speed);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review E0001: pressure follow-through after opening guards")
{
    bool32 sash;
    PARAMETRIZE { sash = FALSE; }
    PARAMETRIZE { sash = TRUE; }
    GIVEN {
        RivalReviewMienfoo(sash);
        AuthoredOpponent(TRAINER_BRENDAN_ROUTE_103_MUDKIP, 0, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft);
            MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight);
        }
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft);
            MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight);
        }
    } THEN {
        Test_MgbaPrintf("E1 pressure follow-through sash=%d turns=%d lastExecuted=%S/%S partyHP=%d/%d foesHP=%d/%d guardCounters=%d/%d",
            sash, gBattleResults.battleTurnCounter,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
            playerLeft->hp, playerRight->hp,
            opponentLeft->volatiles.consecutiveMoveUses, opponentRight->volatiles.consecutiveMoveUses);
    }
}

DOUBLE_BATTLE_TEST("EC live review E0001: native counterfactual sash pressure lines")
{
    bool32 web;
    PARAMETRIZE { web = FALSE; }
    PARAMETRIZE { web = TRUE; }
    GIVEN {
        RivalReviewMienfoo(TRUE);
        AuthoredOpponent(TRAINER_BRENDAN_ROUTE_103_MUDKIP, 0, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            if (web)
            {
                MOVE(opponentLeft, MOVE_SEED_BOMB, target: playerLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(opponentRight, MOVE_ELECTROWEB, hit: TRUE, criticalHit: FALSE);
            }
            else
            {
                MOVE(opponentLeft, MOVE_PROTECT);
                MOVE(opponentRight, MOVE_THUNDERBOLT, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("E1 counterfactual web=%d TreeckoPartyHP=%d PikaPartyHP=%d foesHP=%d/%d foeSpeedStages=%d/%d leftSpecies=%S",
            web, GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP), GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP),
            playerLeft->hp, playerRight->hp, playerLeft->statStages[STAT_SPEED], playerRight->statStages[STAT_SPEED],
            GetSpeciesName(opponentLeft->species));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review E0001: actual Hoenn rival opening decisions")
{
    u16 trainer;
    enum Species first, second;
    enum Move attack;
    bool32 disrupt;
    PARAMETRIZE { trainer = TRAINER_BRENDAN_ROUTE_103_MUDKIP; first = SPECIES_MUDKIP; second = SPECIES_TORCHIC; attack = MOVE_LIQUIDATION; disrupt = FALSE; }
    PARAMETRIZE { trainer = TRAINER_BRENDAN_ROUTE_103_TORCHIC; first = SPECIES_TREECKO; second = SPECIES_TORCHIC; attack = MOVE_BULLET_SEED; disrupt = FALSE; }
    PARAMETRIZE { trainer = TRAINER_BRENDAN_ROUTE_103_TREECKO; first = SPECIES_TREECKO; second = SPECIES_MUDKIP; attack = MOVE_BULLET_SEED; disrupt = FALSE; }
    PARAMETRIZE { trainer = TRAINER_MAY_ROUTE_103_MUDKIP; first = SPECIES_MUDKIP; second = SPECIES_TORCHIC; attack = MOVE_LIQUIDATION; disrupt = TRUE; }
    PARAMETRIZE { trainer = TRAINER_MAY_ROUTE_103_TORCHIC; first = SPECIES_TREECKO; second = SPECIES_TORCHIC; attack = MOVE_BULLET_SEED; disrupt = TRUE; }
    PARAMETRIZE { trainer = TRAINER_MAY_ROUTE_103_TREECKO; first = SPECIES_TREECKO; second = SPECIES_MUDKIP; attack = MOVE_BULLET_SEED; disrupt = TRUE; }
    GIVEN {
        RivalReviewPlayer(first);
        RivalReviewPlayer(second);
        AuthoredOpponent(trainer, 0, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, attack, target: opponentLeft);
            if (second == SPECIES_TORCHIC)
                MOVE(playerRight, disrupt ? MOVE_WILL_O_WISP : MOVE_FLAMETHROWER, target: opponentLeft);
            else if (disrupt)
                MOVE(playerRight, MOVE_WIDE_GUARD);
            else
                MOVE(playerRight, MOVE_LIQUIDATION, target: opponentLeft);
        }
    } THEN {
        Test_MgbaPrintf("E1 trainer=%d disruption=%d completedTurns=%d left=%S lastExecuted=%S hp=%d/%d right=%S lastExecuted=%S hp=%d/%d playerHP=%d/%d versus %d/%d levels=%d/%d/%d/%d",
            trainer, disrupt, gBattleResults.battleTurnCounter, GetSpeciesName(opponentLeft->species), GetMoveName(gLastMoves[B_BATTLER_1]), opponentLeft->hp, opponentLeft->maxHP,
            GetSpeciesName(opponentRight->species), GetMoveName(gLastMoves[B_BATTLER_3]), opponentRight->hp, opponentRight->maxHP,
            playerLeft->hp, playerLeft->maxHP, playerRight->hp, playerRight->maxHP,
            playerLeft->level, playerRight->level, opponentLeft->level, opponentRight->level);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review E0001: actual reserves respect Grass and Magic Bounce")
{
    GIVEN {
        RivalReviewPlayer(SPECIES_TREECKO);
        RivalReviewPlayer(SPECIES_NATU);
        AuthoredOpponent(TRAINER_BRENDAN_ROUTE_103_TORCHIC, 0, FALSE);
        // A reachable remaining-party state, not a different trainer roster:
        // both leads have fainted and the original reserves enter together.
        u16 zero = 0;
        SetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP, &zero);
        SetMonData(&gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP, &zero);
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 2;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 3;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BULLET_SEED, target: opponentLeft);
            MOVE(playerRight, MOVE_PSYCHIC, target: opponentLeft);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_SPORE);
        }
    } THEN {
        Test_MgbaPrintf("E1 reserves indices=%d/%d executed=%S/%S HP=%d/%d TaillowStatus=%d",
            gBattlerPartyIndexes[B_BATTLER_1], gBattlerPartyIndexes[B_BATTLER_3],
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp,
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][3], MON_DATA_STATUS));
        EXPECT(GetMonData(&gParties[B_TRAINER_OPPONENT_A][3], MON_DATA_HP) > 0);
        EXPECT(GetMonData(&gParties[B_TRAINER_OPPONENT_A][3], MON_DATA_STATUS) & STATUS1_PSN_ANY);
    }
}
