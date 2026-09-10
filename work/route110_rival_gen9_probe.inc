// Temporary E0059 native review, excluded after completion.
#include "isabel_gen9_probe.inc"
#include "starter_choose.h"
#include "emerald_champions_opening.h"
#line 61000 "work/route110_rival_gen9_probe.inc"
void E59ReviewCreateNativeParty(struct Pokemon *party, u16 trainer);
static void Route110Factory(u32 gen, u32 remaining, u32 gender, u32 pair)
{
    const u16 routes[2][3] = {
        {TRAINER_BRENDAN_ROUTE_110_MUDKIP, TRAINER_BRENDAN_ROUTE_110_TREECKO, TRAINER_BRENDAN_ROUTE_110_TORCHIC},
        {TRAINER_MAY_ROUTE_110_MUDKIP, TRAINER_MAY_ROUTE_110_TREECKO, TRAINER_MAY_ROUTE_110_TORCHIC},
    };
    u16 trainerId = routes[gender][remaining];
    u16 oldGen = VarGet(VAR_STARTER_GEN), oldFirst = VarGet(VAR_STARTER_MON), oldSecond = VarGet(VAR_EC_SECOND_STARTER);
    VarSet(VAR_STARTER_GEN, gen);
    VarSet(VAR_STARTER_MON, (remaining + 1) % 3);
    VarSet(VAR_EC_SECOND_STARTER, (remaining + 2) % 3 + 1);
    EXPECT_EQ(GetEmeraldChampionsRivalStarterIndex(), remaining);
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][trainerId];
    struct Pokemon *party = AllocZeroed(sizeof(struct Pokemon) * PARTY_SIZE);
    u32 savedFlags = gBattleTypeFlags;
    enum DifficultyLevel savedDifficulty = GetCurrentDifficultyLevel();
    bool8 savedBadges[8];
    for (u32 i = 0; i < ARRAY_COUNT(savedBadges); i++)
    {
        savedBadges[i] = FlagGet(FLAG_BADGE01_GET + i);
        if (i < 2)
            FlagSet(FLAG_BADGE01_GET + i);
        else
            FlagClear(FLAG_BADGE01_GET + i);
    }
    SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
    E59ReviewCreateNativeParty(party, trainerId);
    ApplyTrainerLevelDifficulty(party);
    gBattleTypeFlags = savedFlags;
    SetCurrentDifficultyLevel(savedDifficulty);
    for (u32 i = 0; i < ARRAY_COUNT(savedBadges); i++)
    {
        if (savedBadges[i])
            FlagSet(FLAG_BADGE01_GET + i);
        else
            FlagClear(FLAG_BADGE01_GET + i);
    }

    VarSet(VAR_STARTER_GEN, oldGen);
    VarSet(VAR_STARTER_MON, oldFirst);
    VarSet(VAR_EC_SECOND_STARTER, oldSecond);
    EXPECT_EQ(GetMonData(&party[4], MON_DATA_SPECIES), GetMiddleEvolutionForStarter(GetStarterPokemonForGeneration(remaining, gen)));
    if (gen != 3)
        EXPECT_EQ(GetMonData(&party[4], MON_DATA_HELD_ITEM),
            gEmeraldChampionsDefaultBattleSets[GetMonData(&party[4], MON_DATA_SPECIES)].item);
    for (u32 slot = 0; slot < 5; slot++) {
        struct Pokemon *mon = &party[slot];
        Test_MgbaPrintf("E59_INPUT gen=%d rem=%d gender=%d pair=%d slot=%d species=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d moves=%S/%S/%S/%S EV=%d/%d/%d/%d/%d/%d",
            gen, remaining, gender, pair, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
            GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
            GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
            GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon),
            GetMoveName(GetMonData(mon, MON_DATA_MOVE1)), GetMoveName(GetMonData(mon, MON_DATA_MOVE2)),
            GetMoveName(GetMonData(mon, MON_DATA_MOVE3)), GetMoveName(GetMonData(mon, MON_DATA_MOVE4)),
            GetMonData(mon, MON_DATA_HP_EV), GetMonData(mon, MON_DATA_ATK_EV), GetMonData(mon, MON_DATA_DEF_EV),
            GetMonData(mon, MON_DATA_SPATK_EV), GetMonData(mon, MON_DATA_SPDEF_EV), GetMonData(mon, MON_DATA_SPEED_EV));
    }
    if (pair) {
        struct Pokemon saved[5];
        memcpy(saved, party, sizeof(saved));
        u16 zero = 0;
        for (u32 i = 0; i < 5; i++) SetMonData(&party[i], MON_DATA_HP, &zero);
        party[0] = saved[pair == 1 ? 3 : 2];
        party[1] = saved[4];
    }
    if (IsAITest())
        AI_FLAGS(trainer->aiFlags | AI_FLAG_DOUBLE_BATTLE);
    gBattleTestRunnerState->data.recordedBattle.opponentA = trainerId;
    for (u32 i = 0; i < trainer->partySize; i++)
    {
        OPPONENT(GetMonData(&party[i], MON_DATA_SPECIES))
        {
            *gBattleTestRunnerState->data.currentMon = party[i];
            Nature(GetNature(&party[i]));
            Ability(GetMonAbility(&party[i]));
            Speed(GetMonData(&party[i], MON_DATA_SPEED));
            Moves(GetMonData(&party[i], MON_DATA_MOVE1), GetMonData(&party[i], MON_DATA_MOVE2),
                  GetMonData(&party[i], MON_DATA_MOVE3), GetMonData(&party[i], MON_DATA_MOVE4));
        }
    }
    Free(party);
}





static void Route110Result(u32 gen, u32 rem, u32 gender, u32 pair, u32 board, u32 turns)
{
    Test_MgbaPrintf("E59_EXTRA gen=%d rem=%d pair=%d board=%d turns=%d items=%d/%d foeStatus=%d/%d foeSpeed=%d/%d", gen, rem, pair, board, turns, gBattleMons[1].item, gBattleMons[3].item, gBattleMons[0].status1, gBattleMons[2].status1, gAiLogicData->speedStats[0], gAiLogicData->speedStats[2]);
    Test_MgbaPrintf("E59_AFTER gen=%d rem=%d gender=%d pair=%d board=%d turns=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S status=%d/%d speed=%d/%d SpA=%d/%d SpeStage=%d/%d PP=%d/%d secondPP=%d/%d thirdPP=%d/%d guardPP=%d/%d faints=%d/%d",
        gen, rem, gender, pair, board, turns, gBattleMons[1].species, gBattleMons[3].species,
        gBattleMons[1].hp, gBattleMons[3].hp, gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1] < MOVES_COUNT ? gLastMoves[1] : MOVE_NONE),
        GetMoveName(gLastMoves[3] < MOVES_COUNT ? gLastMoves[3] : MOVE_NONE),
        gBattleMons[1].status1, gBattleMons[3].status1, gAiLogicData->speedStats[1], gAiLogicData->speedStats[3],
        gBattleMons[1].statStages[STAT_SPATK], gBattleMons[3].statStages[STAT_SPATK],
        gBattleMons[1].statStages[STAT_SPEED], gBattleMons[3].statStages[STAT_SPEED],
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleMons[1].pp[1], gBattleMons[3].pp[1],
        gBattleMons[1].pp[2], gBattleMons[3].pp[2], gBattleMons[1].pp[3], gBattleMons[3].pp[3],
        gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}

AI_DOUBLE_BATTLE_TEST("EC live review Route110 rival core: actual leads and two reserve partners")
{
    u32 rem = 0, gender = 0, pair = 0, board = 0, turns = 0;
    for (u32 g = 0; g < 2; g++) for (u32 r = 0; r < 3; r++) for (u32 p = 0; p < 3; p++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= 3; t++) PARAMETRIZE { gender = g; rem = r; pair = p; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        Route110Factory(3, rem, gender, pair);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        Route110Result(3, rem, gender, pair, board, turns);
        if (pair == 0 && board == 1 && turns == 1) {
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
            EXPECT(opponentLeft->status1 & STATUS1_BURN);
        }
        if (pair == 1 && rem == 0 && board == 0 && turns == 1) {
            EXPECT_EQ(opponentLeft->item, ITEM_EVIOLITE);
            EXPECT_EQ(playerLeft->item, ITEM_NONE);
        }
    }
}


AI_DOUBLE_BATTLE_TEST("EC live review Route110 rival region 1: actual replacement and companion")
{
    u32 rem = 0, board = 0, turns = 0;
    for (u32 r = 0; r < 3; r++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (r == 1 && b == 1 ? 2 : 3); t++) PARAMETRIZE { rem = r; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        Route110Factory(1, rem, 0, 1);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN { Route110Result(1, rem, 0, 1, board, turns); if (rem == 2 && board == 0) { EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2); EXPECT_EQ(opponentRight->pp[0], 14); } }
}


AI_DOUBLE_BATTLE_TEST("EC live review Route110 rival region 2: actual replacement and companion")
{
    u32 rem = 0, board = 0, turns = 0;
    for (u32 r = 0; r < 3; r++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= 3; t++) PARAMETRIZE { rem = r; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        Route110Factory(2, rem, 0, 1);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN { Route110Result(2, rem, 0, 1, board, turns); }
}


AI_DOUBLE_BATTLE_TEST("EC live review Route110 rival region 4: actual replacement and companion")
{
    u32 rem = 0, board = 0, turns = 0;
    for (u32 r = 0; r < 3; r++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= 3; t++) PARAMETRIZE { rem = r; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        Route110Factory(4, rem, 0, 1);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN { Route110Result(4, rem, 0, 1, board, turns); if (rem == 0 && board == 1) { EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2); EXPECT_EQ(opponentRight->item, ITEM_NONE); EXPECT_EQ(opponentRight->pp[0], 14); } }
}


AI_DOUBLE_BATTLE_TEST("EC live review Route110 rival region 5: actual replacement and companion")
{
    u32 rem = 0, board = 0, turns = 0;
    for (u32 r = 0; r < 3; r++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= 3; t++) PARAMETRIZE { rem = r; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        Route110Factory(5, rem, 0, 1);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN { Route110Result(5, rem, 0, 1, board, turns); if (rem == 0) { EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2 * turns); EXPECT_EQ(opponentRight->pp[0], 5 - turns); } }
}


AI_DOUBLE_BATTLE_TEST("EC live review Route110 rival region 6: actual replacement and companion")
{
    u32 rem = 0, board = 0, turns = 0;
    for (u32 r = 0; r < 3; r++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (r == 1 && b == 1 ? 2 : 3); t++) PARAMETRIZE { rem = r; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        Route110Factory(6, rem, 0, 1);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN { Route110Result(6, rem, 0, 1, board, turns); }
}


AI_DOUBLE_BATTLE_TEST("EC live review Route110 rival region 7: actual replacement and companion")
{
    u32 rem = 0, board = 0, turns = 0;
    for (u32 r = 0; r < 3; r++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= 3; t++) PARAMETRIZE { rem = r; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        Route110Factory(7, rem, 0, 1);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN { Route110Result(7, rem, 0, 1, board, turns); if (rem == 0 && board == 1 && turns >= 2) { EXPECT(gSideTimers[B_SIDE_OPPONENT].tailwindTimer > 0); EXPECT_EQ(opponentRight->pp[0], 14); } }
}


AI_DOUBLE_BATTLE_TEST("EC live review Route110 rival region 8: actual replacement and companion")
{
    u32 rem = 0, board = 0, turns = 0;
    for (u32 r = 0; r < 3; r++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (r == 1 && b == 1 ? 2 : 3); t++) PARAMETRIZE { rem = r; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        Route110Factory(8, rem, 0, 1);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN { Route110Result(8, rem, 0, 1, board, turns); if (rem == 0 && board == 1 && turns == 3) { EXPECT_EQ(gFieldTimers.terrain, B_TERRAIN_GRASSY); EXPECT_EQ(opponentRight->pp[0], 19); } }
}


AI_DOUBLE_BATTLE_TEST("EC live review Route110 rival region 9: actual replacement and companion")
{
    u32 rem = 0, board = 0, turns = 0;
    for (u32 r = 0; r < 3; r++) for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= 3; t++) PARAMETRIZE { rem = r; board = b; turns = t; }
    GIVEN {
        IsabelPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_XATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        IsabelPlayer(board == 0 ? SPECIES_GURDURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        Route110Factory(9, rem, 0, 1);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            } else {
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN { Route110Result(9, rem, 0, 1, board, turns); }
}


AI_DOUBLE_BATTLE_TEST("EC live review Route110 rival disruption: Fake Out phazing and reserve entry")
{
    u32 rem = 0, turns = 0;
    for (u32 r = 0; r < 3; r++) for (u32 t = 1; t <= (r == 2 ? 2 : 3); t++) PARAMETRIZE { rem = r; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet dreepy = {
            .moves = {MOVE_DRAGON_TAIL, MOVE_INFESTATION, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_CLEAR_BODY,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        IsabelPlayer(SPECIES_MIENFOO, &sMuseumOneMienfoo);
        IsabelPlayer(SPECIES_DREEPY, &dreepy);
        Route110Factory(3, rem, 0, 0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            else if (turn == 1) MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
            else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            if (turn < 2) MOVE(playerRight, MOVE_DRAGON_TAIL, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else MOVE(playerRight, MOVE_BITE, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { Route110Result(3, rem, 0, 0, 10, turns); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Route110 rival timing: complete cold six-player party")
{
    u32 rem = 0, gen = 3;
    for (u32 g = 1; g <= 9; g++) for (u32 r = 0; r < 3; r++) PARAMETRIZE { rem = r; gen = g; }
    GIVEN {
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE};
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) IsabelPlayer(species[i], &gEmeraldChampionsDefaultBattleSets[species[i]]);
        Route110Factory(gen, rem, 0, 0);
    } WHEN { TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); } }
    THEN {
        u32 start = gMain.vblankCounter1;
        AssignUsableGimmicks(); SetShellSideArmCategory(); SetAiLogicDataForTurn(gAiLogicData);
        gAiLogicData->reverseBattlerLogicOrder = RandomPercentage(RNG_AI_REVERSE_BATTLER_LOGIC_ORDER,
            GetConfig(AI_REVERSE_BATTLER_LOGIC_ORDER_CHANCE)) && IsDoubleBattle();
        u32 setup = gMain.vblankCounter1 - start;
        enum BattlerId first = gAiLogicData->reverseBattlerLogicOrder ? B_BATTLER_3 : B_BATTLER_1;
        ComputeAiBattlerDecisions(first); ComputeAiBattlerDecisions(GetPartnerBattler(first));
        u32 complete = gMain.vblankCounter1 - start;
        gAiLogicData->battlerMovesScored = 0;
        u32 warmStart = gMain.vblankCounter1;
        ComputeAiBattlerDecisions(first); ComputeAiBattlerDecisions(GetPartnerBattler(first));
        Test_MgbaPrintf("E59_TIMING gen=%d rem=%d setup=%d bothActors=%d complete=%d warm=%d", gen, rem, setup, complete - setup, complete, gMain.vblankCounter1 - warmStart);
    }
}
