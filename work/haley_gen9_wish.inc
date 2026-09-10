// Temporary actual-party Wish mechanics and separate unrestricted observations.
#line 54000 "work/haley_gen9_wish.inc"
#include "emerald_champions_battle_sets.h"
#include "battle_main.h"

static void HaleyWishPlayer(enum Species species, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, 14, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Ability(GetMonAbility(&mon));
        Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
            GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
    }
}

static void HaleyWishBoard(void)
{
    static const struct EmeraldChampionsBattleSet pachirisu = {
        .moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT},
        .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB,
        .evs = {252, 0, 252, 0, 4, 0},
    };
    static const struct EmeraldChampionsBattleSet lotad = {
        .moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_REST, MOVE_PROTECT},
        .item = ITEM_EVIOLITE, .nature = NATURE_BOLD, .ability = ABILITY_RAIN_DISH,
        .evs = {252, 0, 252, 0, 4, 0},
    };
    HaleyWishPlayer(SPECIES_PACHIRISU, &pachirisu);
    HaleyWishPlayer(SPECIES_LOTAD, &lotad);
    AuthoredOpponent(TRAINER_HALEY_1, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_SPECIES), SPECIES_EEVEE);
    EXPECT_EQ(GetMonData(&party[1], MON_DATA_SPECIES), SPECIES_RALTS);
    EXPECT_EQ(GetMonData(&party[2], MON_DATA_SPECIES), SPECIES_MARILL);
    EXPECT_EQ(GetMonData(&party[3], MON_DATA_SPECIES), SPECIES_SNUBBULL);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_MOVE3), MOVE_HELPING_HAND);
    EXPECT_EQ(GetMonData(&party[0], MON_DATA_MOVE4), MOVE_PROTECT);
    u16 injuredHp = GetMonData(&party[0], MON_DATA_MAX_HP) / 2;
    SetMonData(&party[0], MON_DATA_HP, &injuredHp);
    for (u32 owner = B_TRAINER_PLAYER; owner <= B_TRAINER_OPPONENT_A; owner++)
        for (u32 slot = 0; slot < (owner == B_TRAINER_PLAYER ? 2 : 4); slot++)
        {
            struct Pokemon *mon = &gBattleTestRunnerState->data.recordedBattle.parties[owner][slot];
            Test_MgbaPrintf("HALEY_WISH_INPUT owner=%d slot=%d species=%d HP=%d/%d Atk=%d Def=%d SpA=%d SpD=%d Speed=%d item=%d",
                owner, slot, GetMonData(mon, MON_DATA_SPECIES), GetMonData(mon, MON_DATA_HP), GetMonData(mon, MON_DATA_MAX_HP),
                GetMonData(mon, MON_DATA_ATK), GetMonData(mon, MON_DATA_DEF), GetMonData(mon, MON_DATA_SPATK),
                GetMonData(mon, MON_DATA_SPDEF), GetMonData(mon, MON_DATA_SPEED), GetMonData(mon, MON_DATA_HELD_ITEM));
        }
}

AI_DOUBLE_BATTLE_TEST("EC live review Haley Wish: unrestricted injured full-party followthrough")
{
    u32 turns;
    PARAMETRIZE { turns = 1; }
    PARAMETRIZE { turns = 2; }
    PARAMETRIZE { turns = 3; }
    GIVEN { HaleyWishBoard(); }
    WHEN {
        for (u32 turn = 0; turn < turns; turn++)
        {
            TURN {
                MOVE(playerLeft, MOVE_THUNDERBOLT, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
                MOVE(playerRight, MOVE_PROTECT);
            }
        }
    } THEN {
        Test_MgbaPrintf("HALEY_WISH_AI turns=%d nativeTurns=%d species=%d/%d HP=%d/%d foeHP=%d/%d counter=%d LAST=%S/%S foeLAST=%S/%S status=%d/%d faints=%d/%d",
            turns, gBattleResults.battleTurnCounter, opponentLeft->species, opponentRight->species,
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp, gBattleStruct->wish[B_BATTLER_1].counter,
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetMoveName(gLastMoves[B_BATTLER_0]), GetMoveName(gLastMoves[B_BATTLER_2]),
            opponentLeft->status1, opponentRight->status1, gBattleResults.opponentFaintCounter, gBattleResults.playerFaintCounter);
        for (u32 slot = 0; slot < 4; slot++)
            Test_MgbaPrintf("HALEY_WISH_AI_PARTY turns=%d slot=%d species=%d HP=%d/%d",
                turns, slot, GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_SPECIES),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_HP),
                GetMonData(&gParties[B_TRAINER_OPPONENT_A][slot], MON_DATA_MAX_HP));
        // No expected trainer move or healing: switching/attacking may be sound.
    }
}
