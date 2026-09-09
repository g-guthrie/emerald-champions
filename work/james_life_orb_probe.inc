// Temporary native Life Orb boundaries; include after JamesLeadPlayer/sets.
#line 23000 "work/james_life_orb_probe.inc"

static void JamesOrbBoard(u32 injury, bool32 disguise)
{
    static const struct EmeraldChampionsBattleSet pachirisu = {
        .moves = {MOVE_ROUND, MOVE_FOLLOW_ME, MOVE_SUBSTITUTE, MOVE_PROTECT},
        .item = ITEM_NONE, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
        .statPoints = {32, 0, 32, 0, 2, 0},
    };
    static const struct EmeraldChampionsBattleSet mimikyu = {
        .moves = {MOVE_SPLASH, MOVE_SHADOW_SNEAK, MOVE_PLAY_ROUGH, MOVE_PROTECT},
        .item = ITEM_NONE, .nature = NATURE_BOLD, .ability = ABILITY_DISGUISE,
        .statPoints = {32, 0, 32, 0, 2, 0},
    };
    JamesLeadPlayer(disguise ? SPECIES_MIMIKYU_DISGUISED : SPECIES_PACHIRISU,
        disguise ? &mimikyu : &pachirisu);
    JamesLeadPlayer(SPECIES_TIMBURR, &sJamesTimburr);
    AuthoredOpponent(TRAINER_JAMES_1, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SURSKIT);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_DEWPIDER);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_VENIPEDE);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_KRICKETUNE);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_HELD_ITEM), ITEM_LIFE_ORB);
    EXPECT_EQ(GetMonAbility(&party[3]), ABILITY_TECHNICIAN);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_MOVE1), MOVE_BUG_BITE);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_MOVE4), MOVE_PROTECT);
    u16 hp = 0;
    SetMonData(&party[0], MON_DATA_HP, &hp);
    SetMonData(&party[1], MON_DATA_HP, &hp);
    if (injury)
    {
        hp = injury;
        EXPECT_LT(hp, GetMonData(&party[3], MON_DATA_MAX_HP));
        SetMonData(&party[3], MON_DATA_HP, &hp);
    }
    for (u32 slot = 0; slot < 2; slot++)
    {
        struct Pokemon saved = party[slot];
        party[slot] = party[slot + 2];
        party[slot + 2] = saved;
    }
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
    Test_MgbaPrintf("JAMES_ORB_INPUT injury=%d disguise=%d KrickHP=%d/%d level=%d Atk=%d Speed=%d ability=%d item=%d",
        injury, disguise, GetMonData(&party[1], MON_DATA_HP), GetMonData(&party[1], MON_DATA_MAX_HP),
        GetMonData(&party[1], MON_DATA_LEVEL), GetMonData(&party[1], MON_DATA_ATK),
        GetMonData(&party[1], MON_DATA_SPEED), GetMonAbility(&party[1]), GetMonData(&party[1], MON_DATA_HELD_ITEM));
    // Only actual native reserve order and explicit injuries change. The real
    // leads remain fainted records. Mimikyu is a later-access mechanics boundary,
    // not an assertion about player availability before James.
}

DOUBLE_BATTLE_TEST("EC live review James Life Orb: native HP threshold Protect Substitute and Disguise", s16 orbDamage)
{
    u32 control;
    PARAMETRIZE { control = 0; } // Full native HP, successful attack.
    PARAMETRIZE { control = 1; } // Five HP, four recoil leaves one.
    PARAMETRIZE { control = 2; } // Four HP, successful attack faints to recoil.
    PARAMETRIZE { control = 3; } // Four HP, own Protect has no recoil.
    PARAMETRIZE { control = 4; } // Four HP, target Protect blocks attack/recoil.
    PARAMETRIZE { control = 5; } // Real prior-turn Substitute is still a hit.
    PARAMETRIZE { control = 6; } // Native Disguise blocks occupant damage only.
    GIVEN {
        JamesOrbBoard(control == 0 ? 0 : control == 1 ? 5 : 4, control == 6);
    } WHEN {
        if (control == 5)
            TURN {
                MOVE(playerLeft, MOVE_SUBSTITUTE);
                MOVE(playerRight, MOVE_PROTECT);
                MOVE(opponentLeft, MOVE_PROTECT);
                MOVE(opponentRight, MOVE_PROTECT);
            }
        TURN {
            MOVE(playerLeft, control == 4 ? MOVE_PROTECT : control == 6 ? MOVE_SPLASH : MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_PROTECT);
            if (control == 3)
                MOVE(opponentRight, MOVE_PROTECT);
            else
                MOVE(opponentRight, MOVE_BUG_BITE, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        if (control == 5)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, playerLeft);
        if (control != 3 && control != 4)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BUG_BITE, opponentRight);
            HP_BAR(opponentRight, captureDamage: &results[i].orbDamage);
        }
        else
            NONE_OF { HP_BAR(opponentRight); }
    } THEN {
        struct Pokemon *kricketune = &gParties[B_TRAINER_OPPONENT_A][1];
        u32 maxHp = GetMonData(kricketune, MON_DATA_MAX_HP);
        u32 recoil = max(1, maxHp / 10);
        u32 initialHp = control == 0 ? maxHp : control == 1 ? 5 : 4;
        bool32 triggers = control != 3 && control != 4;
        u32 expectedHp = triggers ? initialHp - min(initialHp, recoil) : initialHp;
        // No player uses a damaging move in the forced controls, so this HP
        // loss cannot be misattributed to an incoming attack or contact item.
        EXPECT_EQ(recoil, 4);
        EXPECT_EQ(GetMonData(kricketune, MON_DATA_HP), expectedHp);
        EXPECT_EQ(opponentRight->hp, expectedHp);
        EXPECT_EQ(GetMonData(kricketune, MON_DATA_HELD_ITEM), ITEM_LIFE_ORB);
        if (triggers)
            EXPECT_EQ(results[i].orbDamage, min(initialHp, recoil));
        if (control == 5)
            EXPECT_EQ(playerLeft->hp, playerLeft->maxHP - playerLeft->maxHP / 4);
        if (control == 6)
            EXPECT_EQ(playerLeft->species, SPECIES_MIMIKYU_BUSTED);
        Test_MgbaPrintf("JAMES_ORB_CONTROL control=%d maxHP=%d recoil=%d nativeHP=%d expectedHP=%d targetHP=%d targetSpecies=%d substituteHP=%d",
            control, maxHp, recoil, opponentRight->hp, expectedHp, playerLeft->hp,
            playerLeft->species, playerLeft->volatiles.substituteHP);
        for (enum BattlerId target = B_BATTLER_0; target <= B_BATTLER_2; target += 2)
        {
            const struct SimulatedDamage *damage = &gAiLogicData->simulatedDmg[B_BATTLER_3][target][0];
            Test_MgbaPrintf("JAMES_ORB_AFTER_CACHE control=%d target=%d bugBite=%d/%d/%d accuracy=%d",
                control, target, damage->minimum, damage->median, damage->maximum,
                gAiLogicData->moveAccuracy[B_BATTLER_3][target][0]);
        }
        // These are the available post-scene caches, not proof of the executed
        // turn's decision cache: the harness may already prepare the next turn.
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review James Life Orb: unrestricted four HP reserve decision")
{
    GIVEN {
        JamesOrbBoard(4, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_ROUND, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            // No trainer command is forced and no particular move is required.
        }
    } THEN {
        Test_MgbaPrintf("JAMES_ORB_AI VenHP=%d KrickHP=%d partyKrickHP=%d foes=%d/%d LAST=%S/%S faintCounts=%d/%d",
            opponentLeft->hp, opponentRight->hp,
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP), playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        // Decision-time trace plus native execution diagnoses the choice;
        // a faint can be a justified trade, and LAST is not a turn-two choice.
    }
}
