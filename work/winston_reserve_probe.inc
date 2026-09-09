// Temporary actual-party E0012 reserve observations; no synthetic Trick Room.
#line 42000 "work/winston_reserve_probe.inc"
#include "emerald_champions_battle_sets.h"

static void WinstonReservePlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species)
    {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon));
        Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
            GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

AI_DOUBLE_BATTLE_TEST("EC live review Winston reserves: actual physical contact and special Fire pressure")
{
    bool32 special;
    u32 turns;
    PARAMETRIZE { special = FALSE; turns = 1; }
    PARAMETRIZE { special = FALSE; turns = 2; }
    PARAMETRIZE { special = TRUE; turns = 1; }
    PARAMETRIZE { special = TRUE; turns = 2; }
    GIVEN {
        static const struct EmeraldChampionsBattleSet eevee = {
            .moves = {MOVE_DOUBLE_EDGE, MOVE_QUICK_ATTACK, MOVE_BITE, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_ADAPTABILITY,
            .statPoints = {32, 32, 2, 0, 0, 0},
        };
        static const struct EmeraldChampionsBattleSet timburr = {
            .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_BULK_UP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_IMPISH, .ability = ABILITY_IRON_FIST,
            .statPoints = {32, 0, 32, 0, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet natu = {
            .moves = {MOVE_PSYCHIC, MOVE_HEAT_WAVE, MOVE_REFLECT, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_SYNCHRONIZE,
            .statPoints = {32, 0, 0, 32, 2, 0},
        };
        static const struct EmeraldChampionsBattleSet charmander = {
            .moves = {MOVE_FLAMETHROWER, MOVE_DRAGON_PULSE, MOVE_WILL_O_WISP, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_BLAZE,
            .statPoints = {32, 0, 0, 32, 2, 0},
        };
        WinstonReservePlayer(special ? SPECIES_NATU : SPECIES_EEVEE, special ? &natu : &eevee);
        WinstonReservePlayer(special ? SPECIES_CHARMANDER : SPECIES_TIMBURR, special ? &charmander : &timburr);
        AuthoredOpponent(TRAINER_WINSTON_1, 0, FALSE);
        struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
        EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_PORYGON);
        EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_HONEDGE);
        EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_GIMMIGHOUL);
        EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_WOOLOO);
        u16 zero = 0;
        SetMonData(&party[0], MON_DATA_HP, &zero);
        SetMonData(&party[1], MON_DATA_HP, &zero);
        for (u32 slot = 0; slot < 2; slot++)
        {
            struct Pokemon saved = party[slot];
            party[slot] = party[slot + 2];
            party[slot + 2] = saved;
        }
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 0;
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = 1;
        // This is the real reserve phase with Room absent, not a claim that
        // Porygon established it before fainting. All opponent menus stay full.
        for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
            for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
            {
                struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
                Test_MgbaPrintf("WINSTON_RESERVE_INPUT special=%d turns=%d owner=%d slot=%d species=%d level=%d HP=%d/%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d ability=%d item=%d",
                    special, turns, owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_LEVEL),
                    GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_MAX_HP), GetMonData(mon, MON_DATA_ATK),
                    GetMonData(mon, MON_DATA_DEF), GetMonData(mon, MON_DATA_SPATK), GetMonData(mon, MON_DATA_SPDEF),
                    GetMonData(mon, MON_DATA_SPEED), GetMonAbility(mon), GetMonData(mon, MON_DATA_HELD_ITEM));
            }
    } WHEN {
        for (u32 turn = 0; turn < turns; turn++)
        {
            TURN {
                // Bite actually hits Ghost Gimmighoul; Normal Double-Edge
                // would be an immunity dummy rather than physical pressure.
                MOVE(playerLeft, special ? MOVE_PSYCHIC : MOVE_BITE, target: opponentLeft,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, special ? MOVE_FLAMETHROWER : MOVE_DRAIN_PUNCH, target: opponentRight,
                    hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            }
        }
    } THEN {
        Test_MgbaPrintf("WINSTON_RESERVE_AFTER special=%d turns=%d nativeTurns=%d species=%d/%d HP=%d/%d foes=%d/%d LAST=%S/%S foeLAST=%S/%S status=%d/%d foeStatus=%d/%d ownFaints=%d foeFaints=%d room=%d roomTimer=%d",
            special, turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->status1, opponentRight->status1, playerLeft->status1, playerRight->status1,
            gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter,
            (gFieldStatuses & STATUS_FIELD_TRICK_ROOM) != 0, gFieldTimers.trickRoomTimer);
        Test_MgbaPrintf("WINSTON_RESERVE_STAGES special=%d turns=%d ownAtk=%d/%d ownDef=%d/%d ownSpA=%d/%d ownSpD=%d/%d ownSpeed=%d/%d foeSpeed=%d/%d substitute=%d/%d",
            special, turns, opponentLeft->statStages[STAT_ATK], opponentRight->statStages[STAT_ATK],
            opponentLeft->statStages[STAT_DEF], opponentRight->statStages[STAT_DEF],
            opponentLeft->statStages[STAT_SPATK], opponentRight->statStages[STAT_SPATK],
            opponentLeft->statStages[STAT_SPDEF], opponentRight->statStages[STAT_SPDEF],
            opponentLeft->statStages[STAT_SPEED], opponentRight->statStages[STAT_SPEED],
            playerLeft->statStages[STAT_SPEED], playerRight->statStages[STAT_SPEED],
            opponentLeft->volatiles.substitute, opponentRight->volatiles.substitute);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("WINSTON_RESERVE_PARTY special=%d turns=%d slot=%d species=%d HP=%d status=%d",
                special, turns, slot, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_STATUS));
    }
}
