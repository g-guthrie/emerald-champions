// Include after HectorPlayer and its native early preparation constants.
#line 57000 "work/hector_orb_probe.inc"

static void HectorOrbBoard(bool32 orb)
{
    static const struct EmeraldChampionsBattleSet munchlax = {
        .moves = {MOVE_BODY_SLAM, MOVE_CRUNCH, MOVE_EARTHQUAKE, MOVE_PROTECT},
        .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_THICK_FAT,
        .evs = {252, 252, 4, 0, 0, 0},
    };
    HectorPlayer(SPECIES_PACHIRISU, &sHectorPachirisu);
    HectorPlayer(SPECIES_MUNCHLAX, &munchlax);
    AuthoredOpponent(TRAINER_HECTOR, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_SPINDA);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_ZANGOOSE);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_SEVIPER);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_DUNSPARCE);
    EXPECT_EQ(GetMonAbility(&party[1]), ABILITY_TOXIC_BOOST);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_HELD_ITEM), ITEM_TOXIC_ORB);
    // The item-free parameter is explicitly a mechanical counterfactual.
    // All stats, moves, abilities and the other three real records are intact.
    if (!orb)
    {
        u16 none = ITEM_NONE;
        SetMonData(&party[1], MON_DATA_HELD_ITEM, &none);
    }
}

DOUBLE_BATTLE_TEST("EC live review Hector Orb: native protected activation powers the following Facade", s16 facadeDamage)
{
    bool32 orb;
    PARAMETRIZE { orb = FALSE; }
    PARAMETRIZE { orb = TRUE; }
    GIVEN { HectorOrbBoard(orb); }
    WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentRight, hit: TRUE);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_FAKE_OUT, target: playerLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentRight, MOVE_PROTECT);
        }
        TURN {
            MOVE(playerLeft, MOVE_FOLLOW_ME);
            MOVE(playerRight, MOVE_PROTECT);
            MOVE(opponentLeft, MOVE_ROCK_SLIDE, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(opponentRight, MOVE_FACADE, target: playerLeft, hit: TRUE, criticalHit: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FACADE, opponentRight);
        HP_BAR(playerLeft, captureDamage: &results[i].facadeDamage);
    } THEN {
        EXPECT_GT(results[i].facadeDamage, 0);
        EXPECT_GT(opponentRight->hp, 0);
        EXPECT_EQ(opponentRight->status1 & STATUS1_TOXIC_POISON, orb ? STATUS1_TOXIC_POISON : 0);
        EXPECT_EQ(opponentRight->item, orb ? ITEM_TOXIC_ORB : ITEM_NONE);
        Test_MgbaPrintf("HECTOR_ORB_NATIVE orb=%d FacadeDamage=%d GooseHP=%d/%d status=%d PachiHP=%d/%d",
            orb, results[i].facadeDamage, opponentRight->hp, opponentRight->maxHP,
            opponentRight->status1, playerLeft->hp, playerLeft->maxHP);
    } FINALLY {
        EXPECT_GT(results[1].facadeDamage, results[0].facadeDamage);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Hector Orb: unrestricted activation and follow-through under one threat")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    GIVEN { HectorOrbBoard(TRUE); }
    WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_SUPER_FANG, target: opponentRight, hit: TRUE);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        Test_MgbaPrintf("HECTOR_ORB_AI turns=%d moves=%S/%S ownHP=%d/%d foes=%d/%d GoosePartyHP=%d status=%d",
            turns, GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP), opponentRight->status1);
        // Actual full menus and free opponent choices; no prescribed opening
        // or assertion that protecting is always superior to an immediate KO.
    }
}
