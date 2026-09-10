// Temporary actual E0032 native observations; independent of prior encounters.
#line 51000 "work/rustboro_gen9_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void RustboroPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 20, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    for (u32 index = 0; index < MAX_MON_MOVES; index++)
        if (set->moves[index] != MOVE_NONE)
            EXPECT(CanSpeciesUseEmeraldChampionsPreparationMove(species, set->moves[index]));
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
            GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

static const struct EmeraldChampionsBattleSet sRustboroEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sRustboroTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sRustboroPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sRustboroLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sRustboroMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};




#include "starter_choose.h"
#include "emerald_champions_opening.h"
void E32ReviewCreateNativeParty(struct Pokemon *party, u16 trainer);
static void RustboroFactory(u32 gen, u32 remaining, u32 gender, bool32 reserves)
{
    const u16 routes[2][3] = {
        {TRAINER_BRENDAN_RUSTBORO_MUDKIP, TRAINER_BRENDAN_RUSTBORO_TREECKO, TRAINER_BRENDAN_RUSTBORO_TORCHIC},
        {TRAINER_MAY_RUSTBORO_MUDKIP, TRAINER_MAY_RUSTBORO_TREECKO, TRAINER_MAY_RUSTBORO_TORCHIC},
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
        if (i < 1)
            FlagSet(FLAG_BADGE01_GET + i);
        else
            FlagClear(FLAG_BADGE01_GET + i);
    }
    SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
    E32ReviewCreateNativeParty(party, trainerId);
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
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), GetMiddleEvolutionForStarter(GetStarterPokemonForGeneration(remaining, gen)));
    if (gen != 3)
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_HELD_ITEM),
            gEmeraldChampionsDefaultBattleSets[GetMonData(&party[3], MON_DATA_SPECIES)].item);
    for (u32 slot = 0; slot < 4; slot++) {
        struct Pokemon *mon = &party[slot];
        Test_MgbaPrintf("E32_INPUT gen=%d rem=%d gender=%d pair=%d slot=%d species=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d moves=%S/%S/%S/%S EV=%d/%d/%d/%d/%d/%d",
            gen, remaining, gender, reserves, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
            GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF),
            GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED),
            GetMonData(mon, MON_DATA_HELD_ITEM), GetMonAbility(mon),
            GetMoveName(GetMonData(mon, MON_DATA_MOVE1)), GetMoveName(GetMonData(mon, MON_DATA_MOVE2)),
            GetMoveName(GetMonData(mon, MON_DATA_MOVE3)), GetMoveName(GetMonData(mon, MON_DATA_MOVE4)),
            GetMonData(mon, MON_DATA_HP_EV), GetMonData(mon, MON_DATA_ATK_EV), GetMonData(mon, MON_DATA_DEF_EV),
            GetMonData(mon, MON_DATA_SPATK_EV), GetMonData(mon, MON_DATA_SPDEF_EV), GetMonData(mon, MON_DATA_SPEED_EV));
    }
    if (reserves) {
        u16 zero = 0;
        for (u32 slot = 0; slot < 2; slot++) {
            SetMonData(&party[slot], MON_DATA_HP, &zero);
            struct Pokemon saved = party[slot]; party[slot] = party[slot + 2]; party[slot + 2] = saved;
        }
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




static void RustboroResult(u32 gen, u32 rem, u32 gender, u32 pair, u32 board, u32 turns)
{
    Test_MgbaPrintf("E32_STATS gen=%d rem=%d pair=%d board=%d turns=%d SpA=%d/%d Atk=%d/%d tailwind=%d foeStatus=%d/%d",
        gen, rem, pair, board, turns, gBattleMons[1].statStages[STAT_SPATK], gBattleMons[3].statStages[STAT_SPATK],
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK], gSideTimers[B_SIDE_OPPONENT].tailwindTimer,
        gBattleMons[0].status1, gBattleMons[2].status1);
    Test_MgbaPrintf("E32_AFTER gen=%d rem=%d gender=%d pair=%d board=%d turns=%d native=%d own=%d/%d HP=%d/%d foe=%d/%d moves=%S/%S status=%d/%d spd=%d/%d stages=%d/%d faints=%d/%d",
        gen, rem, gender, pair, board, turns, gBattleResults.battleTurnCounter,
        gBattleMons[1].species,gBattleMons[3].species,gBattleMons[1].hp,gBattleMons[3].hp,gBattleMons[0].hp,gBattleMons[2].hp,
        GetMoveName(gLastMoves[1]),GetMoveName(gLastMoves[3]),gBattleMons[1].status1,gBattleMons[3].status1,
        gAiLogicData->speedStats[1],gAiLogicData->speedStats[3],gBattleMons[1].statStages[STAT_SPEED],gBattleMons[3].statStages[STAT_SPEED],
        gBattleResults.opponentFaintCounter,gBattleResults.playerFaintCounter);
}

AI_DOUBLE_BATTLE_TEST("EC live review Rustboro Hoenn: actual six aliases leads and reserves")
{
    u32 rem = 0, gender = 0, pair = 0, board = 0, turns = 1;
    for (u32 r = 0; r < 3; r++)
    for (u32 g = 0; g < 2; g++)
    for (u32 p = 0; p < 2; p++)
    for (u32 b = 0; b < 2; b++)
    for (u32 t = 1; t <= 3; t++)
        PARAMETRIZE { rem = r; gender = g; pair = p; board = b; turns = t; }
    GIVEN {
        RustboroPlayer(board ? SPECIES_PACHIRISU : SPECIES_EEVEE, board ? &sRustboroPachirisu : &sRustboroEevee);
        RustboroPlayer(board ? SPECIES_LOTAD : SPECIES_TIMBURR, board ? &sRustboroLotad : &sRustboroTimburr);
        RustboroFactory(3, rem, gender, pair);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, board ? MOVE_SUPER_FANG : MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, board ? MOVE_GIGA_DRAIN : MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        RustboroResult(3, rem, gender, pair, board, turns);
        if (!pair && board == 1 && turns == 3) {
            EXPECT_EQ(opponentLeft->species, SPECIES_TAILLOW);
            EXPECT(opponentLeft->status1 & STATUS1_BURN);
            EXPECT_EQ(gLastMoves[1], MOVE_FACADE);
            EXPECT_EQ(gLastMoves[3], MOVE_HELPING_HAND);
            EXPECT_EQ(playerLeft->hp, 0);
            EXPECT_EQ(playerRight->hp, 0);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Rustboro regional: actual evolved replacement reserves")
{
    u32 gen = 1, rem = 0, turns = 1, board = 1;
    for (u32 g = 1; g <= 9; g++) {
        if (g == 3) continue;
        for (u32 r = 0; r < 3; r++)
        for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (b ? 3 : 2); t++) PARAMETRIZE { gen = g; rem = r; turns = t; board = b; }
    }
    GIVEN {
        RustboroPlayer(board ? SPECIES_PACHIRISU : SPECIES_EEVEE, board ? &sRustboroPachirisu : &sRustboroEevee);
        RustboroPlayer(board ? SPECIES_LOTAD : SPECIES_TIMBURR, board ? &sRustboroLotad : &sRustboroTimburr);
        RustboroFactory(gen, rem, 0, TRUE);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, board ? MOVE_SUPER_FANG : MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, board ? MOVE_GIGA_DRAIN : MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
        }
    } THEN {
        RustboroResult(gen, rem, 0, 1, board, turns);
        if (gen == 5 && rem == 0 && board == 1 && turns == 3) {
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], MAX_STAT_STAGE);
            EXPECT_EQ(gLastMoves[3], MOVE_LEAF_STORM);
        }
        if (gen == 7 && rem == 0 && board == 1 && turns == 2) {
            EXPECT_EQ(gAiLogicData->speedStats[1], 50);
            EXPECT_EQ(gLastMoves[3], MOVE_TAILWIND);
        }
        if (gen == 8 && rem == 0 && board == 1 && turns == 3) {
            EXPECT_EQ(gFieldTimers.terrain, B_TERRAIN_GRASSY);
            EXPECT_EQ(gLastMoves[3], MOVE_GRASSY_GLIDE);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Rustboro electric: native Lightning Rod partner shield")
{
    u32 rem = 0, turns = 1;
    for (u32 r = 0; r < 3; r++)
    for (u32 t = 1; t <= 2; t++) PARAMETRIZE { rem = r; turns = t; }
    GIVEN {
        RustboroPlayer(SPECIES_NATU, &gEmeraldChampionsDefaultBattleSets[SPECIES_NATU]);
        RustboroPlayer(SPECIES_PACHIRISU, &sRustboroPachirisu);
        RustboroFactory(3, rem, 0, FALSE);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        RustboroResult(3, rem, 0, 0, 10, turns);
        Test_MgbaPrintf("E32_ROD rem=%d turns=%d species=%d SpA=%d HP=%d", rem, turns, opponentRight->species, opponentRight->statStages[STAT_SPATK], opponentRight->hp);
        EXPECT_EQ(opponentRight->species, SPECIES_ELECTRIKE);
        EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + turns);
        EXPECT_EQ(opponentLeft->species, SPECIES_TAILLOW);
        EXPECT(opponentLeft->hp > 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Rustboro disruption: Fake Out phazing then Taunt")
{
    u32 rem = 0, turns = 1;
    for (u32 r = 0; r < 3; r++)
    for (u32 t = 1; t <= 2; t++) PARAMETRIZE { rem = r; turns = t; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet dreepy = {
            .moves = {MOVE_DRAGON_TAIL, MOVE_INFESTATION, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_CLEAR_BODY,
            .evs = {252, 0, 252, 0, 4, 0},
        };
        RustboroPlayer(SPECIES_MIENFOO, &sRustboroMienfoo);
        RustboroPlayer(SPECIES_DREEPY, &dreepy);
        RustboroFactory(3, rem, 0, FALSE);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, turn == 0 ? MOVE_FAKE_OUT : MOVE_TAUNT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, turn == 0 ? MOVE_DRAGON_TAIL : MOVE_BITE, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { RustboroResult(3, rem, 0, 0, 11, turns); }
}
