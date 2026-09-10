#line 7000 "work/rival_cyndaquil_probe.inc"
// Temporary native E1 investigation. No copied trainer stats or permanent test.
#include "emerald_champions_opening.h"
#include "emerald_champions_battle_sets.h"
#include "battle_util.h"

static void CyndaquilProbePlayers(void)
{
    static const enum Species species[] = {SPECIES_MARILL, SPECIES_BONSLY};
    static const struct EmeraldChampionsBattleSet sets[] = {
        {
            .moves = {MOVE_LIQUIDATION, MOVE_KNOCK_OFF, MOVE_HELPING_HAND, MOVE_PROTECT},
            .item = ITEM_COVERT_CLOAK, .nature = NATURE_CALM, .ability = ABILITY_THICK_FAT,
            .statPoints = {32, 0, 2, 0, 32, 0},
        },
        {
            .moves = {MOVE_ROCK_THROW, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
            .item = ITEM_EVIOLITE, .nature = NATURE_SASSY, .ability = ABILITY_STURDY,
            .statPoints = {32, 0, 2, 0, 32, 0},
        },
    };
    // Current Route102 Marill / Route101 Bonsly, levelled to the live cap;
    // allocations, abilities and moves are legal native preparation choices.
    for (u32 i = 0; i < ARRAY_COUNT(species); i++)
    {
        struct Pokemon mon;
        CreateRandomMonWithIVs(&mon, species[i], 14, MAX_PER_STAT_IVS);
        EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &sets[i]), EC_BATTLE_SET_SUCCESS);
        PLAYER(species[i])
        {
            *gBattleTestRunnerState->data.currentMon = mon;
            Nature(GetNature(&mon)); Ability(GetMonAbility(&mon));
            Speed(GetMonData(&mon, MON_DATA_SPEED));
            Moves(GetMonData(&mon, MON_DATA_MOVE1), GetMonData(&mon, MON_DATA_MOVE2),
                  GetMonData(&mon, MON_DATA_MOVE3), GetMonData(&mon, MON_DATA_MOVE4));
        }
    }
}

static void CyndaquilProbeOpponent(void)
{
    AuthoredOpponent(TRAINER_BRENDAN_ROUTE_103_TREECKO, 0, FALSE);
    struct Pokemon *party = gBattleTestRunnerState->data.recordedBattle.parties[B_TRAINER_OPPONENT_A];
    enum Species species = SPECIES_CYNDAQUIL;
    u8 level = GetMonData(&party[0], MON_DATA_LEVEL);
    SetMonData(&party[0], MON_DATA_SPECIES, &species);
    u32 experience = gExperienceTables[gSpeciesInfo[species].growthRate][level];
    SetMonData(&party[0], MON_DATA_EXP, &experience);
    ApplyEmeraldChampionsRegionalRivalSet(party, 0, TRUE);
    CalculateMonStats(&party[0]);
    gBattleTestRunnerState->data.forcedAbilities[B_TRAINER_OPPONENT_A][0] = GetMonAbility(&party[0]);
    // Reachable injured re-entry snapshot: preserve generated max HP, stats,
    // Scarf, ability, all four moves and reserves. This is NOT a full-health
    // first-entry claim and not a modified trainer loadout.
    u16 hp = max(1, GetMonData(&party[0], MON_DATA_MAX_HP) / 4);
    SetMonData(&party[0], MON_DATA_HP, &hp);
}

AI_DOUBLE_BATTLE_TEST("EC live review E0001 Cyndaquil: native injured regional choice by environment")
{
    bool32 grass;
    PARAMETRIZE { grass = TRUE; }
    PARAMETRIZE { grass = FALSE; }
    GIVEN {
        // Route103 rival (10,3): approach from (10,2)/(11,3) is walkable
        // tall grass; west/south is normal. The script does not move player
        // before battle. Explicit native environment models both approaches.
        Environment(grass ? BATTLE_ENVIRONMENT_GRASS : BATTLE_ENVIRONMENT_PLAIN);
        CyndaquilProbePlayers();
        CyndaquilProbeOpponent();
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_LIQUIDATION, target: opponentLeft);
            MOVE(playerRight, MOVE_ROCK_THROW, target: opponentLeft);
        }
    } THEN {
        EXPECT_EQ(GetNaturePowerMove(), grass ? MOVE_ENERGY_BALL : MOVE_TRI_ATTACK);
        Test_MgbaPrintf("Cyndaquil AI environment=%d called=%S LAST=%S/%S liveSpecies=%S/%S nativeHP=%d/%d playerHP=%d/%d CyndaPartyHP=%d/%d",
            gBattleEnvironment, GetMoveName(GetNaturePowerMove()),
            GetMoveName(gLastMoves[B_BATTLER_1]), GetMoveName(gLastMoves[B_BATTLER_3]),
            GetSpeciesName(opponentLeft->species), GetSpeciesName(opponentRight->species),
            opponentLeft->hp, opponentRight->hp, playerLeft->hp, playerRight->hp,
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP),
            GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_MAX_HP));
        // Root's temporary logger records decision-time choices. THEN is
        // after the next decision and must not be used as the turn-one cache.
    }
}

DOUBLE_BATTLE_TEST("EC live review E0001 Cyndaquil: native Grass Nature Power versus Extrasensory", s16 damage)
{
    bool32 naturePower;
    PARAMETRIZE { naturePower = FALSE; }
    PARAMETRIZE { naturePower = TRUE; }
    GIVEN {
        Environment(BATTLE_ENVIRONMENT_GRASS);
        CyndaquilProbePlayers();
        CyndaquilProbeOpponent();
    } WHEN {
        TURN {
            // Protect keeps the fixed comparison alive without any healing,
            // damage-rigging or changes to the generated opponent loadouts.
            MOVE(opponentRight, MOVE_PROTECT);
            MOVE(playerLeft, MOVE_LIQUIDATION, target: opponentRight);
            MOVE(playerRight, MOVE_ROCK_THROW, target: opponentRight);
            MOVE(opponentLeft, naturePower ? MOVE_NATURE_POWER : MOVE_EXTRASENSORY,
                 target: playerLeft, hit: TRUE, criticalHit: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, naturePower ? MOVE_ENERGY_BALL : MOVE_EXTRASENSORY, opponentLeft);
        HP_BAR(playerLeft, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(GetNaturePowerMove(), MOVE_ENERGY_BALL);
        Test_MgbaPrintf("Cyndaquil counterfactual naturePower=%d called=%S nativeDamage=%d nativeSpA=%d targetSpD=%d targetHP=%d/%d CyndaHP=%d/%d",
            naturePower, GetMoveName(GetNaturePowerMove()), results[i].damage,
            opponentLeft->spAttack, playerLeft->spDefense, playerLeft->hp, playerLeft->maxHP,
            opponentLeft->hp, opponentLeft->maxHP);
        EXPECT(results[i].damage > 0);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}
