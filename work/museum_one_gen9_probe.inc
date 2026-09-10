// Temporary actual E0050 native observations; independent of prior encounters.
#line 51000 "work/museum_one_gen9_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void MuseumOnePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sMuseumOneNatu = {
    .moves = {MOVE_PSYCHIC, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE,
    .evs = {4, 0, 0, 252, 0, 252},
};
static const struct EmeraldChampionsBattleSet sMuseumOneEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sMuseumOneTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sMuseumOnePachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sMuseumOneLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sMuseumOneMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};



static void MuseumOneOpponent(u32 reserves)
{
    AuthoredOpponent(TRAINER_GRUNT_MUSEUM_1, 1, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species expected[] = {SPECIES_ZUBAT, SPECIES_CARVANHA, SPECIES_RATTATA_ALOLA, SPECIES_WAILMER};
    for (u32 slot = 0; slot < ARRAY_COUNT(expected); slot++)
    {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), expected[slot]);
        Test_MgbaPrintf("MUSEUM_ONE_INPUT reserves=%d slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
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


static void MuseumOneResult(u32 board, u32 turns, u32 reserves)
{
    Test_MgbaPrintf("MUSEUM_ONE_AFTER board=%d turns=%d pair=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speed=%d/%d foeSpeed=%d/%d speedStage=%d/%d items=%d/%d status=%d/%d foeStatus=%d/%d PP=%d/%d guardPP=%d faints=%d/%d",
        board, turns, reserves, gBattleMons[1].species, gBattleMons[3].species,
        gBattleMons[1].hp, gBattleMons[3].hp, gBattleMons[0].hp, gBattleMons[2].hp,
        GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].statStages[STAT_SPEED], gBattleMons[3].statStages[STAT_SPEED],
        gBattleMons[1].item, gBattleMons[3].item, gBattleMons[1].status1, gBattleMons[3].status1,
        gBattleMons[0].status1, gBattleMons[2].status1, gBattleMons[1].pp[0], gBattleMons[3].pp[0],
        gBattleMons[3].pp[3], gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}
AI_DOUBLE_BATTLE_TEST("EC live review Museum One core: physical special leads and reserves")
{
    u32 board = 0, turns = 0, reserves = 0;
    for (u32 p = 0; p < 2; p++)
        for (u32 b = 0; b < 2; b++)
            for (u32 t = 1; t <= (b == 0 ? 3 : 4); t++) PARAMETRIZE { board = b; turns = t; reserves = p; }
    GIVEN {
        MuseumOnePlayer(board == 0 ? SPECIES_EEVEE : SPECIES_NATU, board == 0 ? &sMuseumOneEevee : &sMuseumOneNatu);
        MuseumOnePlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU, board == 0 ? &sMuseumOneTimburr : &sMuseumOnePachirisu);
        MuseumOneOpponent(reserves);
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
        MuseumOneResult(board, turns, reserves);
        if (reserves && board == 0 && turns == 1) {
            EXPECT_EQ(opponentLeft->hp, 35);
            EXPECT_EQ(opponentLeft->item, ITEM_NONE);
        }
        if (!reserves && board == 1 && turns == 2) {
            EXPECT_EQ(opponentRight->species, SPECIES_CARVANHA);
            EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
            EXPECT_EQ(opponentRight->pp[3], 4);
            EXPECT_EQ(gLastMoves[3], MOVE_CRUNCH);
        }
    }
}
AI_DOUBLE_BATTLE_TEST("EC live review Museum One disruption: Fake Out redirection and opposing Tailwind")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= 4; t++) PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        MuseumOnePlayer(board == 0 ? SPECIES_MIENFOO : SPECIES_NATU, board == 0 ? &sMuseumOneMienfoo : &sMuseumOneNatu);
        MuseumOnePlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        MuseumOneOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                if (turn == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                else MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentRight, hit: TRUE, criticalHit: FALSE);
                MOVE(playerRight, MOVE_FOLLOW_ME);
            } else {
                if (turn == 0) MOVE(playerLeft, MOVE_TAILWIND);
                else MOVE(playerLeft, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN { MuseumOneResult(10 + board, turns, 0); }
}

AI_DOUBLE_BATTLE_TEST("EC live review Museum One abilities: Inner Focus and Water Veil")
{
    u32 board = 0;
    PARAMETRIZE { board = 0; }
    PARAMETRIZE { board = 1; }
    GIVEN {
        if (board == 0) MuseumOnePlayer(SPECIES_MIENFOO, &sMuseumOneMienfoo);
        else {
            struct EmeraldChampionsBattleSet set = {
                .moves = {MOVE_WILL_O_WISP, MOVE_FOUL_PLAY, MOVE_RECOVER, MOVE_PROTECT},
                .item = ITEM_FOCUS_SASH, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER,
                .evs = {252, 0, 252, 0, 4, 0},
            };
            MuseumOnePlayer(SPECIES_SABLEYE, &set);
        }
        MuseumOnePlayer(SPECIES_PACHIRISU, &sMuseumOnePachirisu);
        MuseumOneOpponent(board);
    } WHEN {
        for (u32 turn = 0; turn < (board == 0 ? 1 : 2); turn++) TURN {
            if (board == 0) MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            else MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentRight, hit: TRUE);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } SCENE {
        if (board == 0) ANIMATION(ANIM_TYPE_MOVE, MOVE_POISON_FANG, opponentLeft);
        else ABILITY_POPUP(opponentRight, ABILITY_WATER_VEIL);
    } THEN {
        MuseumOneResult(20 + board, board == 0 ? 1 : 2, board);
        if (board == 0) {
            EXPECT_EQ(opponentLeft->ability, ABILITY_INNER_FOCUS);
            EXPECT_LT(opponentLeft->hp, opponentLeft->maxHP);
            EXPECT_EQ(gLastMoves[1], MOVE_POISON_FANG);
        }
        else EXPECT_EQ(opponentRight->status1 & STATUS1_BURN, 0);
    }
}
