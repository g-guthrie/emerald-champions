// Temporary actual E0047 native observations; independent of prior encounters.
#line 51000 "work/dwayne_gen9_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void DwaynePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sDwayneNatu = {
    .moves = {MOVE_PSYCHIC, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE,
    .evs = {4, 0, 0, 252, 0, 252},
};
static const struct EmeraldChampionsBattleSet sDwayneEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sDwayneTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sDwaynePachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sDwayneLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sDwayneMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};



static void DwayneOpponent(u32 reserves)
{
    AuthoredOpponent(TRAINER_DWAYNE, 1, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species expected[] = {SPECIES_MAGBY, SPECIES_SMOOCHUM, SPECIES_ELEKID, SPECIES_MONFERNO};
    for (u32 slot = 0; slot < ARRAY_COUNT(expected); slot++)
    {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), expected[slot]);
        Test_MgbaPrintf("DWAYNE_INPUT reserves=%d slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
            reserves, slot, GetMonData(&party[slot], MON_DATA_LEVEL), GetMonData(&party[slot], MON_DATA_HP),
            GetMonData(&party[slot], MON_DATA_ATK), GetMonData(&party[slot], MON_DATA_DEF),
            GetMonData(&party[slot], MON_DATA_SPATK), GetMonData(&party[slot], MON_DATA_SPDEF),
            GetMonData(&party[slot], MON_DATA_SPEED), GetMonData(&party[slot], MON_DATA_HELD_ITEM), GetMonAbility(&party[slot]));
    }
    if (reserves)
    {
        u16 zero = 0;
        for (u32 slot = 0; slot < reserves * 2; slot++)
            SetMonData(&party[slot], MON_DATA_HP, &zero);
        for (u32 slot = 0; slot < 2; slot++)
        {
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + reserves * 2];
            party[slot + reserves * 2] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
    }
}


static void DwayneResult(u32 board, u32 turns, u32 reserves)
{
    Test_MgbaPrintf("DWAYNE_AFTER board=%d turns=%d pair=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speed=%d/%d foeSpeed=%d/%d SpA=%d/%d foeAtk=%d foeDef=%d status=%d/%d foeStatus=%d/%d lastPP=%d/%d faints=%d/%d",
        board, turns, reserves, gBattleMons[1].species, gBattleMons[3].species,
        gBattleMons[1].hp, gBattleMons[3].hp, gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].statStages[STAT_SPATK], gBattleMons[3].statStages[STAT_SPATK],
        gBattleMons[0].statStages[STAT_ATK], gBattleMons[0].statStages[STAT_DEF],
        gBattleMons[1].status1, gBattleMons[3].status1, gBattleMons[0].status1, gBattleMons[2].status1,
        gBattleMons[1].pp[3], gBattleMons[3].pp[3], gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}
AI_DOUBLE_BATTLE_TEST("EC live review Dwayne core: physical special leads and reserves")
{
    u32 board = 0, turns = 0, reserves = 0;
    for (u32 p = 0; p < 2; p++)
        for (u32 b = 0; b < 2; b++)
            for (u32 t = 1; t <= (p ? b == 0 ? 5 : 4 : b == 0 ? 3 : 4); t++)
                PARAMETRIZE { board = b; turns = t; reserves = p; }
    GIVEN {
        DwaynePlayer(board == 0 ? SPECIES_EEVEE : SPECIES_NATU, board == 0 ? &sDwayneEevee : &sDwayneNatu);
        DwaynePlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU, board == 0 ? &sDwayneTimburr : &sDwaynePachirisu);
        DwayneOpponent(reserves);
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
        DwayneResult(board, turns, reserves);
        if (reserves && board == 0 && turns == 4) {
            EXPECT_EQ(gBattleMons[3].species, SPECIES_MONFERNO);
            EXPECT_EQ(gBattleMons[3].hp, 31);
            EXPECT_EQ(gBattleMons[3].pp[2], 6);
        }
        if (!reserves && board == 0 && turns == 3)
            EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Dwayne disruption: Fake Out and Taunt")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= 3; t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        DwaynePlayer(SPECIES_MIENFOO, &sDwayneMienfoo);
        DwaynePlayer(SPECIES_PACHIRISU, &sDwaynePachirisu);
        DwayneOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0 && turn == 0)
                MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else
                MOVE(playerLeft, MOVE_TAUNT, target: board == 0 ? opponentLeft : opponentRight);
            MOVE(playerRight, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
        }
    } SCENE {
        if (board == 1 && turns == 1)
            ABILITY_POPUP(opponentRight, ABILITY_OBLIVIOUS);
    } THEN { DwayneResult(10 + board, turns, 0); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Dwayne motifs: supported setup and opposing boosts")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= 4; t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        DwaynePlayer(SPECIES_TIMBURR, &sDwayneTimburr);
        DwaynePlayer(SPECIES_PACHIRISU, &sDwaynePachirisu);
        DwayneOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn < 2) MOVE(playerLeft, MOVE_BULK_UP);
            else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            if (board == 0) MOVE(playerRight, MOVE_FOLLOW_ME);
            else MOVE(playerRight, MOVE_SUPER_FANG, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
        }
    } SCENE {
        if (turns == 2) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, opponentRight);
        }
    } THEN { DwayneResult(20 + board, turns, 0); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Dwayne speed: Choice Scarf pressure on reserves")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        struct EmeraldChampionsBattleSet mienfoo = sDwayneMienfoo;
        mienfoo.item = ITEM_CHOICE_SCARF;
        DwaynePlayer(SPECIES_MIENFOO, &mienfoo);
        DwaynePlayer(SPECIES_PACHIRISU, &sDwaynePachirisu);
        DwayneOpponent(1);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { DwayneResult(30, turns, 1); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Dwayne setup: bulky Thick Fat opponent")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet munchlax = {
            .moves = {MOVE_BODY_SLAM, MOVE_CURSE, MOVE_REST, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_THICK_FAT,
            .evs = {252, 0, 0, 0, 252, 4},
        };
        DwaynePlayer(SPECIES_MUNCHLAX, &munchlax);
        DwaynePlayer(SPECIES_PACHIRISU, &sDwaynePachirisu);
        DwayneOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (turn == 0) MOVE(playerLeft, MOVE_CURSE);
            else MOVE(playerLeft, MOVE_BODY_SLAM, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { DwayneResult(40, turns, 0); }
}
