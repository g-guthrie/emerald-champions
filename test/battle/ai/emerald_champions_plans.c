#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "battle_ai_util.h"
#include "emerald_champions_battle_plan.h"
#include "test/battle.h"
#include "data.h"
#include "difficulty.h"
#include "event_data.h"
#include "malloc.h"
#include "trainer_pools.h"
#include "battle_transition.h"
#include "constants/opponents.h"
#include "emerald_champions_battle_sets.h"

// Temporary EV-migration encounter probes; removed after the native run.
static void AuthoredOpponent(u16 trainerId, u32 badges, bool32 injuredCoalossal);
#include "../../../work/rival_ai_probe.inc"
#include "../../../work/rival_regional_ai_probe.inc"

// Execute the production lookup and generated table. No duplicate plan table
// or mock trainer-owner resolver: these are the ROM's actual trainer IDs.
TEST("EC battle plans: compiled directives follow trainer ownership and exclude other battle namespaces")
{
    u32 savedFlags = gBattleTypeFlags;
    u16 savedA = TRAINER_BATTLE_PARAM.opponentA;
    u16 savedB = TRAINER_BATTLE_PARAM.opponentB;

    TRAINER_BATTLE_PARAM.opponentA = TRAINER_ROXANNE_1;
    TRAINER_BATTLE_PARAM.opponentB = TRAINER_BILLY;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_1), EC_BATTLE_PLAN_TRICK_ROOM);
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_3), EC_BATTLE_PLAN_TRICK_ROOM);
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_0), 0);
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_2), 0);

    gBattleTypeFlags |= BATTLE_TYPE_TWO_OPPONENTS;
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_1), EC_BATTLE_PLAN_TRICK_ROOM);
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_3), EC_BATTLE_PLAN_RAIN);

    TRAINER_BATTLE_PARAM.opponentA = TRAINER_TATE_AND_LIZA_1;
    EXPECT(EmeraldChampions_GetBattlePlan(B_BATTLER_1) & EC_BATTLE_PLAN_TRICK_ROOM);
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_TABITHA_MAGMA_HIDEOUT;
    EXPECT(EmeraldChampions_GetBattlePlan(B_BATTLER_1) & EC_BATTLE_PLAN_ALLY_COMBO);
    EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_1, SPECIES_DRAGAPULT, SPECIES_COALOSSAL), EC_BATTLE_TACTIC_ACTIVATE);
    EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_1, SPECIES_COALOSSAL, SPECIES_DRAGAPULT), EC_BATTLE_TACTIC_ACTIVATE);
    EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_1, SPECIES_VOLCANION, SPECIES_COALOSSAL), 0);
    EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_3, SPECIES_DRAGAPULT, SPECIES_COALOSSAL), 0);
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_CRISTIAN;
    EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_1, SPECIES_FALINKS, SPECIES_GALLADE_MEGA), EC_BATTLE_TACTIC_ACTIVATE);
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_SYLVIA;
    EXPECT(EmeraldChampions_GetBattlePlan(B_BATTLER_1) & EC_BATTLE_PLAN_ALLY_COMBO);
    EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_1, SPECIES_DONDOZO, SPECIES_TATSUGIRI), EC_BATTLE_TACTIC_COMMANDER);

    static const u32 foreignContexts[] = {
        BATTLE_TYPE_LINK, BATTLE_TYPE_BATTLE_TOWER, BATTLE_TYPE_TRAINER_HILL,
        BATTLE_TYPE_EREADER_TRAINER, BATTLE_TYPE_SECRET_BASE, BATTLE_TYPE_RECORDED_LINK,
    };
    for (u32 i = 0; i < ARRAY_COUNT(foreignContexts); i++)
    {
        gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE | foreignContexts[i];
        EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_1), 0);
        EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_3), 0);
        EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_1, SPECIES_DONDOZO, SPECIES_TATSUGIRI), 0);
    }
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_RECORDED;
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_1), EC_BATTLE_PLAN_ALLY_COMBO);
    EXPECT_EQ(EmeraldChampions_GetPartnerTactics(B_BATTLER_1, SPECIES_DONDOZO, SPECIES_TATSUGIRI), EC_BATTLE_TACTIC_COMMANDER);
    gBattleTypeFlags |= BATTLE_TYPE_BATTLE_TOWER;
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_1), 0);
    gBattleTypeFlags = BATTLE_TYPE_DOUBLE;
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_1), 0);
    gBattleTypeFlags |= BATTLE_TYPE_TRAINER;
    TRAINER_BATTLE_PARAM.opponentA = TRAINERS_COUNT;
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_1), 0);
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_NONE;
    EXPECT_EQ(EmeraldChampions_GetBattlePlan(B_BATTLER_1), 0);

    TRAINER_BATTLE_PARAM.opponentA = savedA;
    TRAINER_BATTLE_PARAM.opponentB = savedB;
    gBattleTypeFlags = savedFlags;
}

// Use the compiled campaign loadouts and production stat/level generation,
// including reserves, rather than a second hand-maintained copy of the team.
static void AuthoredOpponent(u16 trainerId, u32 badges, bool32 injuredCoalossal)
{
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][trainerId];
    struct Pokemon *party = AllocZeroed(sizeof(struct Pokemon) * PARTY_SIZE);
    u32 savedFlags = gBattleTypeFlags;
    enum DifficultyLevel savedDifficulty = GetCurrentDifficultyLevel();
    bool8 savedBadges[8];
    for (u32 i = 0; i < ARRAY_COUNT(savedBadges); i++)
    {
        savedBadges[i] = FlagGet(FLAG_BADGE01_GET + i);
        if (i < badges)
            FlagSet(FLAG_BADGE01_GET + i);
        else
            FlagClear(FLAG_BADGE01_GET + i);
    }
    SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
    CreateNPCTrainerPartyFromTrainer(party, trainer);
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
            if (injuredCoalossal && GetMonData(&party[i], MON_DATA_SPECIES) == SPECIES_COALOSSAL)
                HP(1);
        }
    }
    Free(party);
}











AI_DOUBLE_BATTLE_TEST("EC authored strategy: Tabitha activation requires survival and a useful boost")
{
    bool32 injuredCoalossal, vest;
    PARAMETRIZE { injuredCoalossal = FALSE; vest = TRUE; }
    PARAMETRIZE { injuredCoalossal = TRUE; vest = TRUE; }
    PARAMETRIZE { injuredCoalossal = FALSE; vest = FALSE; }
    GIVEN {
        // The positive foes withstand unboosted Heat Wave. The weaker negative
        // control is already in its KO range, verified with the native cache.
        PLAYER(SPECIES_WHIMSICOTT) { Level(vest ? 60 : 50); Item(vest ? ITEM_ASSAULT_VEST : ITEM_NONE); Moves(MOVE_MOONBLAST); Speed(GetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_SPEED)); }
        PLAYER(SPECIES_WHIMSICOTT) { Level(vest ? 60 : 50); Item(vest ? ITEM_ASSAULT_VEST : ITEM_NONE); Moves(MOVE_MOONBLAST); Speed(GetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_SPEED)); }
        AuthoredOpponent(TRAINER_TABITHA_MAGMA_HIDEOUT, 6, injuredCoalossal);
    } WHEN {
        if (injuredCoalossal)
            TURN {
                MOVE(playerLeft, MOVE_MOONBLAST, target: opponentLeft);
                MOVE(playerRight, MOVE_MOONBLAST, target: opponentLeft);
            }
        else if (!vest)
            TURN {
                MOVE(playerLeft, MOVE_MOONBLAST, target: opponentLeft);
                MOVE(playerRight, MOVE_MOONBLAST, target: opponentLeft);
                NOT_EXPECT_MOVE(opponentLeft, MOVE_SURF);
            }
        else
            TURN {
                MOVE(playerLeft, MOVE_MOONBLAST, target: opponentLeft);
                MOVE(playerRight, MOVE_MOONBLAST, target: opponentLeft);
                EXPECT_MOVE(opponentLeft, MOVE_SURF);
                EXPECT_MOVE(opponentRight, MOVE_HEAT_WAVE);
            }
    } THEN {
        if (!vest)
        {
            EXPECT(gAiLogicData->simulatedDmg[B_BATTLER_3][B_BATTLER_0][0].minimum >= playerLeft->maxHP);
            EXPECT(gAiLogicData->simulatedDmg[B_BATTLER_3][B_BATTLER_2][0].minimum >= playerRight->maxHP);
        }
        if (injuredCoalossal)
        {
            // Surf remains legal if Coalossal Protects or switches to a safe
            // recipient such as Water Absorb Volcanion. Preserve the injured
            // party member; do not outlaw the move independent of its partner.
            EXPECT(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP) > 0);
            if (opponentRight->species == SPECIES_COALOSSAL)
                EXPECT(opponentRight->hp > 0);
        }
        if (!injuredCoalossal && vest)
        {
            EXPECT_EQ(opponentRight->statStages[STAT_SPEED], MAX_STAT_STAGE);
            EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Flannery advances Eruption before opposing Rock Slide")
{
    GIVEN {
        PLAYER(SPECIES_HERACROSS) { Level(40); Moves(MOVE_ROCK_SLIDE); Speed(GetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_SPEED)); }
        PLAYER(SPECIES_HERACROSS) { Level(40); Moves(MOVE_ROCK_SLIDE); Speed(GetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_SPEED)); }
        AuthoredOpponent(TRAINER_FLANNERY_1, 3, FALSE);
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_ERUPTION); EXPECT_MOVE(opponentRight, MOVE_AFTER_YOU, target: opponentLeft); }
    } SCENE {
        MESSAGE("The opposing Lilligant used After You!");
        MESSAGE("The opposing Torkoal used Eruption!");
    } THEN {
        EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Connie's mutual Surf activates Storm Drain")
{
    GIVEN {
        // Paralysis leaves both foes slower than Gastrodon: this matchup has
        // an absorption payoff but no reason to establish Tailwind first.
        PLAYER(SPECIES_TYRANITAR) { Level(70); Ability(ABILITY_UNNERVE); Item(ITEM_ASSAULT_VEST); Status1(STATUS1_PARALYSIS); Moves(MOVE_SMACK_DOWN); Speed(GetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_SPEED)); }
        PLAYER(SPECIES_TYRANITAR) { Level(70); Ability(ABILITY_UNNERVE); Item(ITEM_ASSAULT_VEST); Status1(STATUS1_PARALYSIS); Moves(MOVE_SMACK_DOWN); Speed(GetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_SPEED)); }
        AuthoredOpponent(TRAINER_CONNIE, 7, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SMACK_DOWN, target: opponentLeft);
            MOVE(playerRight, MOVE_SMACK_DOWN, target: opponentLeft);
            EXPECT_MOVE(opponentLeft, MOVE_SURF);
            EXPECT_MOVE(opponentRight, MOVE_SURF);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT(opponentLeft->hp > 0);
        EXPECT(opponentRight->hp > 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Parker repeats Earthquake safely through Telepathy")
{
    GIVEN {
        // Sash requires two hits; Ground hits both targets harder than Hammer
        // Arm. Speed 84 is attainable by a Jolly, Speed-invested Lv45 Coalossal.
        PLAYER(SPECIES_COALOSSAL) { Level(45); Ability(ABILITY_FLAME_BODY); Item(ITEM_FOCUS_SASH); Speed(84); Moves(MOVE_PROTECT, MOVE_TACKLE); }
        PLAYER(SPECIES_COALOSSAL) { Level(45); Ability(ABILITY_FLAME_BODY); Item(ITEM_FOCUS_SASH); Speed(84); Moves(MOVE_PROTECT, MOVE_TACKLE); }
        AuthoredOpponent(TRAINER_PARKER, 4, FALSE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentLeft, MOVE_TRICK_ROOM);
        }
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentLeft);
            EXPECT_MOVE(opponentRight, MOVE_EARTHQUAKE);
            EXPECT_MOVE(opponentLeft, MOVE_INSTRUCT, target: opponentRight);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->speed, 62);
        EXPECT_EQ(opponentRight->speed, 54);
        EXPECT(gFieldStatuses & STATUS_FIELD_TRICK_ROOM);
        EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Parker preserves a nonimmune reserve beside Earthquake")
{
    GIVEN {
        PLAYER(SPECIES_TYRANITAR) { Level(45); Ability(ABILITY_UNNERVE); Speed(115); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_TYRANITAR) { Level(45); Ability(ABILITY_UNNERVE); Speed(115); Moves(MOVE_TACKLE); }
        AuthoredOpponent(TRAINER_PARKER, 4, FALSE);
        gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = 5; // Girafarig, not Telepathy Oranguru.
        u32 hp = 1;
        SetMonData(&OPPONENT_PARTY[5], MON_DATA_HP, &hp);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
        }
    } THEN {
        EXPECT(GetMonData(&gParties[B_TRAINER_OPPONENT_A][5], MON_DATA_HP) > 0);
        if (opponentLeft->species == SPECIES_GIRAFARIG)
            EXPECT(opponentLeft->hp > 0);
    }
}

AI_DOUBLE_BATTLE_TEST("EC authored strategy: Maura escapes her countdown while retaining the trap")
{
    u32 turns;
    PARAMETRIZE { turns = 3; }
    PARAMETRIZE { turns = 4; }
    GIVEN {
        for (u32 i = 0; i < 4; i++)
            PLAYER(SPECIES_CHANSEY) { Level(60); Ability(ABILITY_NATURAL_CURE); Moves(MOVE_CELEBRATE); Speed(GetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_SPEED)); }
        AuthoredOpponent(TRAINER_MAURA, 6, FALSE);
    } WHEN {
        TURN { EXPECT_MOVE(opponentLeft, MOVE_PERISH_SONG); }
        TURN { }
        TURN { }
        if (turns == 4)
            TURN { SEND_OUT(playerLeft, 2); SEND_OUT(playerRight, 3); }
    } THEN {
        EXPECT(GetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HP) > 0);
        EXPECT(GetMonData(&gParties[B_TRAINER_OPPONENT_A][1], MON_DATA_HP) > 0);
        EXPECT_NE(opponentLeft->species, SPECIES_JYNX);
        if (turns == 3)
            EXPECT_EQ(opponentRight->species, SPECIES_GOTHITELLE);
        else
        {
            EXPECT_NE(opponentRight->species, SPECIES_GOTHITELLE);
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP), 0);
            EXPECT_EQ(GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_HP), 0);
        }
    }
}
