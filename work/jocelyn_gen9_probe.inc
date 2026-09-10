// Temporary actual E0039 native observations; independent of prior encounters.
#line 51000 "work/jocelyn_gen9_probe.inc"
#include "emerald_champions_battle_sets.h"
#include "move_relearner.h"

static void JocelynPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
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

static const struct EmeraldChampionsBattleSet sJocelynNatu = {
    .moves = {MOVE_PSYCHIC, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE,
    .evs = {4, 0, 0, 252, 0, 252},
};
static const struct EmeraldChampionsBattleSet sJocelynEevee = {
    .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_ADAPTABILITY,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sJocelynTimburr = {
    .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_CAREFUL, .ability = ABILITY_IRON_FIST,
    .evs = {252, 4, 0, 0, 252, 0},
};
static const struct EmeraldChampionsBattleSet sJocelynPachirisu = {
    .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
    .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
    .evs = {252, 0, 252, 0, 4, 0},
};
static const struct EmeraldChampionsBattleSet sJocelynLotad = {
    .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT},
    .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH,
    .evs = {252, 0, 0, 136, 120, 0},
};
static const struct EmeraldChampionsBattleSet sJocelynMienfoo = {
    .moves = {MOVE_FAKE_OUT, MOVE_TAUNT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF},
    .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_INNER_FOCUS,
    .evs = {252, 0, 4, 0, 0, 252},
};



static void JocelynOpponent(u32 reserves)
{
    AuthoredOpponent(TRAINER_JOCELYN, 1, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    const enum Species expected[] = {SPECIES_WHIMSICOTT, SPECIES_COMBUSKEN, SPECIES_VOLBEAT, SPECIES_CRABOMINABLE, SPECIES_BRELOOM, SPECIES_SIRFETCHD};
    for (u32 slot = 0; slot < ARRAY_COUNT(expected); slot++)
    {
        EXPECT_EQ(GetMonData(&party[slot], MON_DATA_SPECIES), expected[slot]);
        Test_MgbaPrintf("JOCELYN_INPUT reserves=%d slot=%d lv=%d HP=%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d ability=%d",
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


static void JocelynResult(u32 board, u32 turns, u32 reserves)
{
    Test_MgbaPrintf("JOCELYN_TEMPO board=%d turns=%d pair=%d tailwind=%d screen=%d SpeStages=%d/%d statuses=%d/%d", board, turns, reserves, gSideTimers[B_SIDE_OPPONENT].tailwindTimer, gSideTimers[B_SIDE_OPPONENT].lightscreenTimer, gBattleMons[1].statStages[STAT_SPEED], gBattleMons[3].statStages[STAT_SPEED], gBattleMons[1].status1, gBattleMons[3].status1);
    Test_MgbaPrintf("JOCELYN_AFTER board=%d turns=%d pair=%d native=%d species=%d/%d HP=%d/%d foes=%d/%d moves=%S/%S speeds=%d/%d foeSpeeds=%d/%d Atk=%d/%d Def=%d/%d PP=%d/%d faints=%d/%d",
        board, turns, reserves, gBattleResults.battleTurnCounter,
        gBattleMons[1].species, gBattleMons[3].species, gBattleMons[1].hp, gBattleMons[3].hp,
        gBattleMons[0].hp, gBattleMons[2].hp, GetMoveName(gLastMoves[1]), GetMoveName(gLastMoves[3]),
        gAiLogicData->speedStats[1], gAiLogicData->speedStats[3], gAiLogicData->speedStats[0], gAiLogicData->speedStats[2],
        gBattleMons[1].statStages[STAT_ATK], gBattleMons[3].statStages[STAT_ATK],
        gBattleMons[1].statStages[STAT_DEF], gBattleMons[3].statStages[STAT_DEF],
        gBattleMons[1].pp[0], gBattleMons[3].pp[0], gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
}


AI_DOUBLE_BATTLE_TEST("EC live review Jocelyn transition: focused attacks on Whimsicott")
{
    u32 turns = 0;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    PARAMETRIZE { turns = 3; }
    GIVEN {
        JocelynPlayer(SPECIES_EEVEE, &sJocelynEevee);
        JocelynPlayer(SPECIES_TIMBURR, &sJocelynTimburr);
        JocelynOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            MOVE(playerLeft, MOVE_DOUBLE_EDGE, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_ICE_PUNCH, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        JocelynResult(20, turns, 0);
        if (turns >= 2)
        {
            EXPECT_EQ(opponentLeft->species, SPECIES_CRABOMINABLE);
            EXPECT_EQ(opponentRight->species, SPECIES_COMBUSKEN);
        }
        if (turns == 3)
            EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Jocelyn core: physical special leads and reserves")
{
    u32 board = 0, turns = 0, reserves = 0;
    for (u32 p = 0; p < 3; p++)
        for (u32 b = 0; b < 2; b++)
            for (u32 t = 1; t <= (p == 0 ? 3 : 2); t++)
                PARAMETRIZE { board = b; turns = t; reserves = p; }
    GIVEN {
        JocelynPlayer(board == 0 ? SPECIES_EEVEE : SPECIES_NATU, board == 0 ? &sJocelynEevee : &sJocelynNatu);
        JocelynPlayer(board == 0 ? SPECIES_TIMBURR : SPECIES_PACHIRISU, board == 0 ? &sJocelynTimburr : &sJocelynPachirisu);
        JocelynOpponent(reserves);
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
        JocelynResult(board, turns, reserves);
        if (reserves == 0)
        {
            EXPECT_EQ(opponentRight->species, SPECIES_COMBUSKEN);
            EXPECT(opponentRight->hp > 0);
            if (board == 1)
                EXPECT_EQ(opponentLeft->pp[0], GetMovePP(MOVE_TAILWIND) - 1);
        }
        if (reserves == 1 && board == 1)
        {
            EXPECT_EQ(opponentRight->species, SPECIES_CRABOMINABLE);
            EXPECT_EQ(opponentRight->speed, 29);
            EXPECT_EQ(gAiLogicData->speedStats[3], 58);
            EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
            EXPECT_EQ(opponentLeft->pp[0], GetMovePP(MOVE_TAILWIND) - 1);
            EXPECT_EQ(playerLeft->hp, 0);
            if (turns == 2)
            {
                EXPECT_EQ(gLastMoves[1], MOVE_HELPING_HAND);
                EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
            }
        }
        if (reserves == 2)
        {
            EXPECT(opponentLeft->status1 & STATUS1_TOXIC_POISON);
            if (board == 0 && turns == 1)
                EXPECT(playerRight->status1 & STATUS1_SLEEP);
            if (turns == 2)
                EXPECT_EQ(gBattleResults.playerFaintCounter, 2);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Jocelyn disruption: Fake Out Taunt and opposing Tailwind")
{
    u32 board = 0, turns = 0;
    for (u32 b = 0; b < 2; b++)
        for (u32 t = 1; t <= (b == 0 ? 4 : 3); t++)
            PARAMETRIZE { board = b; turns = t; }
    GIVEN {
        JocelynPlayer(board == 0 ? SPECIES_MIENFOO : SPECIES_NATU, board == 0 ? &sJocelynMienfoo : &sJocelynNatu);
        JocelynPlayer(SPECIES_PACHIRISU, &sJocelynPachirisu);
        JocelynOpponent(0);
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++) TURN {
            if (board == 0) {
                if (turn == 0)
                    MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
                else
                    MOVE(playerLeft, MOVE_TAUNT, target: opponentLeft);
            } else if (turn == 0)
                MOVE(playerLeft, MOVE_TAILWIND);
            else
                MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        JocelynResult(10 + board, turns, 0);
        if (board == 0 && turns >= 3)
        {
            EXPECT_EQ(opponentRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
            if (turns == 4)
            {
                EXPECT_EQ(gLastMoves[3], MOVE_DRAIN_PUNCH);
                EXPECT(opponentRight->hp > opponentRight->maxHP / 2);
            }
        }
        if (board == 1)
            EXPECT_EQ(opponentLeft->pp[0], GetMovePP(MOVE_TAILWIND) - 1);
    }
}
