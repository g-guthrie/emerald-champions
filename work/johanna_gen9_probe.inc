// Temporary actual E0048 native observations; independent of prior encounters.
#line 51000 "work/johanna_gen9_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void JohannaPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sJohannaNatu = {
    .moves = {MOVE_PSYCHIC, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE,
    .evs = {4, 0, 0, 252, 0, 252},
};
static const struct EmeraldChampionsBattleSet sJohannaEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sJohannaTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sJohannaPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sJohannaLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sJohannaMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};



static void JohannaOpponent(u32 reserves)
{
    AuthoredOpponent(TRAINER_JOHANNA, 1, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species expected[] = {SPECIES_BUDEW, SPECIES_SYLVEON, SPECIES_FEEBAS, SPECIES_QWILFISH};
    for (u32 slot = 0; slot < ARRAY_COUNT(expected); slot++)
    {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), expected[slot]);
        Test_MgbaPrintf("JOHANNA_INPUT reserves=%d slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
            reserves, slot, GetMonData(&party[slot], MON_DATA_LEVEL), GetMonData(&party[slot], MON_DATA_HP),
            GetMonData(&party[slot], MON_DATA_ATK), GetMonData(&party[slot], MON_DATA_DEF),
            GetMonData(&party[slot], MON_DATA_SPATK), GetMonData(&party[slot], MON_DATA_SPDEF),
            GetMonData(&party[slot], MON_DATA_SPEED), GetMonData(&party[slot], MON_DATA_HELD_ITEM), GetMonAbility(&party[slot]));
    }
    if (reserves == 2)
    {
        u16 zero = 0;
        struct Pokemon budew = party[0];
        party[0] = party[1];
        party[1] = party[2];
        party[2] = budew;
        SetMonData(&party[2], MON_DATA_HP, &zero);
        SetMonData(&party[3], MON_DATA_HP, &zero);
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
    }
    else if (reserves)
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


static void JohannaResult(u32 board, u32 turns, u32 reserves)
{
    Test_MgbaPrintf("JOHANNA_AFTER board=%d turns=%d pair=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speed=%d/%d foeSpeed=%d/%d SpA=%d/%d foeAtk=%d/%d status=%d/%d foeStatus=%d/%d items=%d/%d firstPP=%d/%d faints=%d/%d",
        board, turns, reserves, gBattleMons[1].species, gBattleMons[3].species,
        gBattleMons[1].hp, gBattleMons[3].hp, gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].statStages[STAT_SPATK], gBattleMons[3].statStages[STAT_SPATK],
        gBattleMons[0].statStages[STAT_ATK], gBattleMons[2].statStages[STAT_ATK],
        gBattleMons[1].status1, gBattleMons[3].status1, gBattleMons[0].status1, gBattleMons[2].status1,
        gBattleMons[1].item, gBattleMons[3].item, gBattleMons[1].pp[0], gBattleMons[3].pp[0],
        gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}
AI_DOUBLE_BATTLE_TEST("EC live review Johanna core: physical special leads and reserves")
{
    u32 board = 0, turns = 0, reserves = 0;
    for (u32 p = 0; p < 2; p++)
        for (u32 b = 0; b < 2; b++)
            for (u32 t = 1; t <= (p ? b == 0 ? 4 : 3 : 3); t++)
                PARAMETRIZE { board = b; turns = t; reserves = p; }
    GIVEN {
        JohannaPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_NATU, board == 0 ? &sJohannaEevee : &sJohannaNatu);
        JohannaPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU, board == 0 ? &sJohannaTimburr : &sJohannaPachirisu);
        JohannaOpponent(reserves);
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
        JohannaResult(board, turns, reserves);
        if (!reserves) {
            EXPECT_EQ(gBattleMons[3].statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(gBattleMons[3].item, ITEM_NONE);
            if (board == 0 && turns == 2) EXPECT(gBattleMons[0].status1 & STATUS1_SLEEP);
        }
        if (reserves && board == 1 && turns == 1) {
            EXPECT_EQ(gBattleMons[3].item, ITEM_NONE);
            EXPECT_EQ(gBattleMons[3].hp, 28);
            EXPECT_EQ(gAiLogicData->speedStats[0], 37);
            EXPECT_EQ(gAiLogicData->speedStats[2], 32);
            EXPECT_EQ(gAiLogicData->speedStats[3], 41);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Johanna disruption: Fake Out Taunt and powder immunity")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (b == 0 ? 3 : 2); t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet treecko = {
            .moves = {MOVE_GIGA_DRAIN, MOVE_BULLET_SEED, MOVE_BRICK_BREAK, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_OVERGROW,
            .evs = {4, 0, 0, 252, 0, 252},
        };
        struct EmeraldChampionsBattleSet pachirisu = sJohannaPachirisu;
        if (board == 1) pachirisu.item = ITEM_SAFETY_GOGGLES;
        JohannaPlayer(board == 0 ? SPECIES_MIENFOO : SPECIES_TREECKO, board == 0 ? &sJohannaMienfoo : &treecko);
        JohannaPlayer(SPECIES_PACHIRISU, &pachirisu);
        JohannaOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                if (turn == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
            } else MOVE(playerLeft, MOVE_GIGA_DRAIN, target: opponentRight, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        JohannaResult(10 + board, turns, 0);
        if (board == 1) EXPECT_EQ(gBattleMons[1].pp[0], 15);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Johanna spread denial: opposing Wide Guard")
{
    u32 turns = 0;
    for (u32 t = 1; t <= 3; t++) PARAMETRIZE { turns = t; }
    GIVEN {
        const struct EmeraldChampionsBattleSet hariyama = {
            .moves = {MOVE_FAKE_OUT, MOVE_WIDE_GUARD, MOVE_DRAIN_PUNCH, MOVE_PROTECT},
            .item = ITEM_SITRUS_BERRY, .nature = NATURE_CAREFUL, .ability = ABILITY_THICK_FAT,
            .evs = {252, 4, 0, 0, 252, 0},
        };
        JohannaPlayer(SPECIES_HARIYAMA, &hariyama);
        JohannaPlayer(SPECIES_PACHIRISU, &sJohannaPachirisu);
        JohannaOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_WIDE_GUARD);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN { JohannaResult(20, turns, 0); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Johanna mixed pair: Feebas supports surviving Sylveon")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (b == 0 ? 2 : 3); t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        JohannaPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_NATU, board == 0 ? &sJohannaEevee : &sJohannaNatu);
        JohannaPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU, board == 0 ? &sJohannaTimburr : &sJohannaPachirisu);
        JohannaOpponent(2);
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
    } SCENE {
        if (board == 0 && turns == 2) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, opponentRight);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, opponentLeft);
        }
    } THEN { JohannaResult(30 + board, turns, 2); }
}
