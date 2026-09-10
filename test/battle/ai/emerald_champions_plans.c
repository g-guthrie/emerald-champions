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
            // Declare native order without freezing Speed across Mega/form
            // changes: the harness otherwise treats Speed() as a fixed stat.
            bool32 fixedSpeed = FALSE;
            SetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_HYPER_TRAINED_SPEED, &fixedSpeed);
            Moves(GetMonData(&party[i], MON_DATA_MOVE1), GetMonData(&party[i], MON_DATA_MOVE2),
                  GetMonData(&party[i], MON_DATA_MOVE3), GetMonData(&party[i], MON_DATA_MOVE4));
            if (injuredCoalossal && GetMonData(&party[i], MON_DATA_SPECIES) == SPECIES_COALOSSAL)
                HP(1);
        }
    }
    Free(party);
}



DOUBLE_BATTLE_TEST("EC pivot cleanup: exhausted bench does not skip the next target")
{
    u32 move = MOVE_VOLT_SWITCH;
    PARAMETRIZE { move = MOVE_VOLT_SWITCH; }
    PARAMETRIZE { move = MOVE_U_TURN; }
    PARAMETRIZE { move = MOVE_FLIP_TURN; }
    PARAMETRIZE { move = MOVE_PARTING_SHOT; }
    GIVEN {
        // Frozen mechanic reproduction, independent of authored trainer sets.
        // The first target falls after the pivot; the second turn must still
        // affect the other target even when there is nobody to switch into.
        PLAYER(SPECIES_MIENFOO) { Level(30); MaxHP(76); HP(76); Defense(35); SpDefense(35); Speed(79); Item(move == MOVE_PARTING_SHOT ? ITEM_NONE : ITEM_FOCUS_SASH); Ability(ABILITY_REGENERATOR); Moves(MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH); }
        PLAYER(SPECIES_LOMBRE) { Level(30); MaxHP(400); HP(400); Defense(100); SpDefense(100); Speed(44); Ability(ABILITY_SWIFT_SWIM); Moves(MOVE_SCALD); }
        OPPONENT(SPECIES_ROTOM) { Level(28); MaxHP(200); HP(200); SpAttack(200); Speed(82); Ability(ABILITY_LEVITATE); Moves(MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_KILOWATTREL) { Level(29); MaxHP(300); HP(300); Attack(100); SpAttack(93); SpDefense(100); Speed(114); Ability(ABILITY_VOLT_ABSORB); Moves(move); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft);
            MOVE(opponentRight, move, target: playerLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentLeft, MOVE_THUNDERBOLT, target: playerLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft);
            MOVE(opponentRight, move, target: playerRight, hit: TRUE, criticalHit: FALSE);
            MOVE(opponentLeft, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 0);
        EXPECT_EQ(gBattleStruct->battlerState[B_BATTLER_3].targetsDone[B_BATTLER_2], FALSE);
        EXPECT(!(gAbsentBattlerFlags & (1u << B_BATTLER_3)));
        if (move == MOVE_PARTING_SHOT) {
            EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
            EXPECT_EQ(playerRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
        } else {
            EXPECT(playerRight->hp < playerRight->maxHP);
        }
    }
}

AI_DOUBLE_BATTLE_TEST("EC Tailwind: charge and speed rewards exclude immune or fainted foes")
{
    GIVEN {
        // Frozen two-turn reproduction: Xatu falls, leaving slower Ground
        // Mudbray. Neither its former partner's Speed nor an unusable charge
        // should reward Tailwind. Independent of later trainer authoring.
        const enum Species species[] = {SPECIES_MUDBRAY, SPECIES_XATU, SPECIES_PERSIAN, SPECIES_KILOWATTREL, SPECIES_TRUMBEAK, SPECIES_GLIGAR};
        const struct EmeraldChampionsBattleSet sets[] = {
            {.moves = {MOVE_EARTHQUAKE, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_PROTECT}, .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_STAMINA, .evs = {252, 252, 4, 0, 0, 0}},
            {.moves = {MOVE_PSYCHIC, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT}, .item = ITEM_EVIOLITE, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE, .evs = {4, 0, 0, 252, 0, 252}},
            {.moves = {MOVE_FAKE_OUT, MOVE_ICY_WIND, MOVE_U_TURN, MOVE_PLAY_ROUGH}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_JOLLY, .ability = ABILITY_TECHNICIAN, .evs = {4, 252, 0, 0, 0, 252}},
            {.moves = {MOVE_THUNDERBOLT, MOVE_HURRICANE, MOVE_TAILWIND, MOVE_PROTECT}, .item = ITEM_LIFE_ORB, .nature = NATURE_TIMID, .ability = ABILITY_WIND_POWER, .evs = {4, 0, 0, 252, 0, 252}},
            {.moves = {MOVE_ROCK_BLAST, MOVE_BULLET_SEED, MOVE_BRAVE_BIRD, MOVE_U_TURN}, .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_SKILL_LINK, .evs = {4, 252, 0, 0, 0, 252}},
            {.moves = {MOVE_ACROBATICS, MOVE_HIGH_HORSEPOWER, MOVE_U_TURN, MOVE_PROTECT}, .item = ITEM_FLYING_GEM, .nature = NATURE_JOLLY, .ability = ABILITY_IMMUNITY, .evs = {4, 252, 0, 0, 0, 252}},
        };
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS
            | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
            struct Pokemon mon;
            CreateRandomMonWithIVs(&mon, species[i], i < 2 ? 30 : 28, MAX_PER_STAT_IVS);
            EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &sets[i]), EC_BATTLE_SET_SUCCESS);
            CalculateMonStats(&mon);
            if (i < 2) {
                PLAYER(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            } else {
                OPPONENT(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            }
        }
    } WHEN {
        for (u32 turn = 0; turn < 2; turn++) TURN {
            MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_PSYCHIC, target: opponentLeft, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        EXPECT_EQ(opponentRight->species, SPECIES_KILOWATTREL);
        EXPECT_EQ(opponentRight->pp[2], 15);
        EXPECT_EQ(gSideTimers[1].tailwindTimer, 0);
    }
}


AI_DOUBLE_BATTLE_TEST("EC Leech Seed: do not repeat into a seeded Follow Me recipient")
{
    GIVEN {
        // Frozen native reproduction, independent of later trainer changes.
        // Both turns actually execute: first Seed lands, then revealed Follow
        // Me would redirect another Seed into the same already-seeded foe.
        const enum Species species[] = {SPECIES_SABLEYE, SPECIES_PACHIRISU, SPECIES_FERROSEED, SPECIES_DRUDDIGON, SPECIES_LYCANROC, SPECIES_CYCLIZAR};
        const struct EmeraldChampionsBattleSet sets[] = {
            {.moves = {MOVE_FOUL_PLAY, MOVE_WILL_O_WISP, MOVE_TAUNT, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER, .evs = {252, 0, 252, 0, 4, 0}},
            {.moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB, .evs = {252, 0, 252, 0, 4, 0}},
            {.moves = {MOVE_LEECH_SEED, MOVE_GYRO_BALL, MOVE_STEALTH_ROCK, MOVE_PROTECT}, .item = ITEM_ROCKY_HELMET, .nature = NATURE_RELAXED, .ability = ABILITY_IRON_BARBS, .evs = {252, 0, 252, 0, 4, 0}},
            {.moves = {MOVE_DRAGON_CLAW, MOVE_FIRE_PUNCH, MOVE_SUCKER_PUNCH, MOVE_IRON_HEAD}, .item = ITEM_ASSAULT_VEST, .nature = NATURE_ADAMANT, .ability = ABILITY_ROUGH_SKIN, .evs = {252, 252, 4, 0, 0, 0}},
            {.moves = {MOVE_ACCELEROCK, MOVE_ROCK_SLIDE, MOVE_DRILL_RUN, MOVE_PROTECT}, .item = ITEM_FOCUS_SASH, .nature = NATURE_JOLLY, .ability = ABILITY_STEADFAST, .evs = {4, 252, 0, 0, 0, 252}},
            {.moves = {MOVE_SHED_TAIL, MOVE_DRAGON_CLAW, MOVE_KNOCK_OFF, MOVE_U_TURN}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_JOLLY, .ability = ABILITY_REGENERATOR, .evs = {252, 0, 4, 0, 0, 252}},
        };
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS
            | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
            struct Pokemon mon;
            CreateRandomMonWithIVs(&mon, species[i], i < 2 ? 30 : 28, MAX_PER_STAT_IVS);
            EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &sets[i]), EC_BATTLE_SET_SUCCESS);
            CalculateMonStats(&mon);
            if (i < 2) {
                PLAYER(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            } else {
                OPPONENT(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            }
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentLeft, hit: TRUE);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
        TURN {
            MOVE(playerLeft, MOVE_FOUL_PLAY, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } THEN {
        EXPECT_EQ((u32)playerRight->volatiles.leechSeed, 2);
        // Switching the burned setter is valid counterplay. Check the seed
        // user's actual PP, not a prescribed active species or second attack.
        struct Pokemon *setter = &GetBattlerParty(B_BATTLER_1)[0];
        EXPECT_EQ(GetMonData(setter, MON_DATA_SPECIES), SPECIES_FERROSEED);
        EXPECT_EQ(opponentLeft->species == SPECIES_FERROSEED ? opponentLeft->pp[0]
            : GetMonData(setter, MON_DATA_PP1), 9);
    }
}



AI_DOUBLE_BATTLE_TEST("EC Calm Mind: fast Simple setup accounts for immediate special defense")
{
    GIVEN {
        // Freeze the demonstrated failure board, independent of future trainer
        // authoring. Native stats and unforced choices, not a scripted NPC move.
        const enum Species species[] = {SPECIES_LOMBRE, SPECIES_PACHIRISU, SPECIES_WOBBUFFET, SPECIES_SWOOBAT, SPECIES_KADABRA, SPECIES_MUSHARNA};
        const struct EmeraldChampionsBattleSet sets[] = {
            {.moves = {MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_RAIN_DANCE, MOVE_PROTECT}, .item = ITEM_EVIOLITE, .nature = NATURE_MODEST, .ability = ABILITY_RAIN_DISH, .evs = {252, 0, 0, 136, 120, 0}},
            {.moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB, .evs = {252, 0, 252, 0, 4, 0}},
            {.moves = {MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_ENCORE, MOVE_HELPING_HAND}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_CALM, .ability = ABILITY_SHADOW_TAG, .evs = {252, 0, 4, 0, 252, 0}},
            {.moves = {MOVE_CALM_MIND, MOVE_STORED_POWER, MOVE_AIR_SLASH, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_TIMID, .ability = ABILITY_SIMPLE, .evs = {4, 0, 0, 252, 0, 252}},
            {.moves = {MOVE_PSYCHIC, MOVE_DAZZLING_GLEAM, MOVE_ENCORE, MOVE_PROTECT}, .item = ITEM_FOCUS_SASH, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_GUARD, .evs = {4, 0, 0, 252, 0, 252}},
            {.moves = {MOVE_CALM_MIND, MOVE_PSYCHIC, MOVE_YAWN, MOVE_PROTECT}, .item = ITEM_LEFTOVERS, .nature = NATURE_CALM, .ability = ABILITY_SYNCHRONIZE, .evs = {252, 0, 4, 0, 252, 0}},
        };
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS
            | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_JACLYN;
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
            struct Pokemon mon;
            CreateRandomMonWithIVs(&mon, species[i], i < 2 ? 30 : 28, MAX_PER_STAT_IVS);
            EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &sets[i]), EC_BATTLE_SET_SUCCESS);
            CalculateMonStats(&mon);
            if (i < 2) {
                PLAYER(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            } else {
                OPPONENT(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            }
        }
    } WHEN {
        for (u32 t = 0; t < 2; t++) TURN {
            MOVE(playerLeft, MOVE_GIGA_DRAIN, target: opponentLeft, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_THUNDERBOLT, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CALM_MIND, opponentRight);
    } THEN {
        EXPECT_EQ(opponentRight->species, SPECIES_SWOOBAT);
        EXPECT_EQ(opponentRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponentRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponentRight->hp, 52);
        EXPECT_EQ(opponentRight->pp[0], 19);
    }
}

AI_DOUBLE_BATTLE_TEST("EC Tailwind: faster recoil attackers do not use empty speed setup")
{
    GIVEN {
        // Frozen reproduction of an observed shared scoring failure, not a
        // design lock on Alyssa's evolving trainer party. All stats are native.
        const enum Species species[] = {SPECIES_SABLEYE, SPECIES_PACHIRISU, SPECIES_CYCLIZAR, SPECIES_FLETCHINDER, SPECIES_GLIGAR};
        const struct EmeraldChampionsBattleSet sets[] = {
            {.moves = {MOVE_FOUL_PLAY, MOVE_WILL_O_WISP, MOVE_RECOVER, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_PRANKSTER, .evs = {252, 0, 252, 0, 4, 0}},
            {.moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_BOLD, .ability = ABILITY_VOLT_ABSORB, .evs = {252, 0, 252, 0, 4, 0}},
            {.moves = {MOVE_SHED_TAIL, MOVE_DRAGON_CLAW, MOVE_KNOCK_OFF, MOVE_U_TURN}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_JOLLY, .ability = ABILITY_REGENERATOR, .evs = {4, 252, 0, 0, 0, 252}},
            {.moves = {MOVE_BRAVE_BIRD, MOVE_FLARE_BLITZ, MOVE_TAILWIND, MOVE_U_TURN}, .item = ITEM_LIFE_ORB, .nature = NATURE_JOLLY, .ability = ABILITY_GALE_WINGS, .evs = {4, 252, 0, 0, 0, 252}},
            {.moves = {MOVE_ACROBATICS, MOVE_HIGH_HORSEPOWER, MOVE_U_TURN, MOVE_PROTECT}, .item = ITEM_FLYING_GEM, .nature = NATURE_JOLLY, .ability = ABILITY_IMMUNITY, .evs = {4, 252, 0, 0, 0, 252}},
        };
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS
            | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
            struct Pokemon mon;
            CreateRandomMonWithIVs(&mon, species[i], i < 2 ? 30 : 28, MAX_PER_STAT_IVS);
            EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &sets[i]), EC_BATTLE_SET_SUCCESS);
            CalculateMonStats(&mon);
            if (i < 2) {
                PLAYER(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            } else {
                OPPONENT(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            }
        }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_WILL_O_WISP, target: opponentLeft);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } THEN {
        u32 fasterFoe = max(GetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_SPEED), GetMonData(&gParties[B_TRAINER_PLAYER][1], MON_DATA_SPEED));
        for (u32 i = 0; i < 3; i++) EXPECT(GetMonData(&gParties[B_TRAINER_OPPONENT_A][i], MON_DATA_SPEED) > fasterFoe);
        EXPECT_EQ(opponentRight->species, SPECIES_FLETCHINDER);
        EXPECT_EQ(opponentRight->pp[2], 15);
        EXPECT_EQ(gSideTimers[1].tailwindTimer, 0);
    }
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

AI_DOUBLE_BATTLE_TEST("EC doubles budget: initial Mega and full benches stay below 1.2 seconds")
{
    GIVEN {
        // Frozen reproduction of the measured 89-frame decision, not a
        // trainer-design lock. Keep native form stats; do not hide this cost
        // by timing only the already-Mega board after turn one.
        const enum Species species[] = {SPECIES_MIENFOO, SPECIES_XATU, SPECIES_MARSHTOMP, SPECIES_GROVYLE, SPECIES_PACHIRISU, SPECIES_EEVEE,
            SPECIES_STARYU, SPECIES_KANGASKHAN, SPECIES_ABRA, SPECIES_DRATINI, SPECIES_MUNCHLAX, SPECIES_TOGEPI};
        const struct EmeraldChampionsBattleSet sets[] = {
            {.moves = {MOVE_FAKE_OUT, MOVE_HIGH_JUMP_KICK, MOVE_KNOCK_OFF, MOVE_PROTECT}, .item = ITEM_EVIOLITE, .nature = NATURE_JOLLY, .ability = ABILITY_REGENERATOR, .evs = {4, 252, 0, 0, 0, 252}},
            {.moves = {MOVE_PSYCHIC, MOVE_HEAT_WAVE, MOVE_TAILWIND, MOVE_PROTECT}, .item = ITEM_LIFE_ORB, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_BOUNCE, .evs = {4, 0, 0, 252, 0, 252}},
            {.moves = {MOVE_MUDDY_WATER, MOVE_EARTH_POWER, MOVE_ICY_WIND, MOVE_WIDE_GUARD}, .item = ITEM_EVIOLITE, .nature = NATURE_QUIET, .ability = ABILITY_DAMP, .evs = {252, 0, 4, 252, 0, 0}},
            {.moves = {MOVE_LEAF_STORM, MOVE_DRAGON_PULSE, MOVE_VACUUM_WAVE, MOVE_PROTECT}, .item = ITEM_WHITE_HERB, .nature = NATURE_TIMID, .ability = ABILITY_UNBURDEN, .evs = {4, 0, 0, 252, 0, 252}},
            {.moves = {MOVE_FOLLOW_ME, MOVE_NUZZLE, MOVE_SUPER_FANG, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_CALM, .ability = ABILITY_VOLT_ABSORB, .evs = {252, 0, 4, 0, 252, 0}},
            {.moves = {MOVE_HELPING_HAND, MOVE_YAWN, MOVE_WISH, MOVE_PROTECT}, .item = ITEM_EVIOLITE, .nature = NATURE_CALM, .ability = ABILITY_ANTICIPATION, .evs = {252, 0, 124, 0, 132, 0}},
            {.moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_THUNDERBOLT, MOVE_RECOVER}, .item = ITEM_EXPERT_BELT, .nature = NATURE_MODEST, .ability = ABILITY_ANALYTIC, .evs = {4, 0, 0, 252, 0, 252}},
            {.moves = {MOVE_FAKE_OUT, MOVE_BODY_SLAM, MOVE_DRAIN_PUNCH, MOVE_SUCKER_PUNCH}, .item = ITEM_KANGASKHANITE, .nature = NATURE_ADAMANT, .ability = ABILITY_SCRAPPY, .evs = {140, 252, 0, 0, 0, 116}},
            {.moves = {MOVE_PSYCHIC, MOVE_DAZZLING_GLEAM, MOVE_ENCORE, MOVE_PROTECT}, .item = ITEM_FOCUS_SASH, .nature = NATURE_TIMID, .ability = ABILITY_MAGIC_GUARD, .evs = {4, 0, 0, 252, 0, 252}},
            {.moves = {MOVE_DRAGON_DANCE, MOVE_EXTREME_SPEED, MOVE_DRAGON_CLAW, MOVE_PROTECT}, .item = ITEM_EVIOLITE, .nature = NATURE_ADAMANT, .ability = ABILITY_SHED_SKIN, .evs = {4, 252, 0, 0, 0, 252}},
            {.moves = {MOVE_BODY_SLAM, MOVE_FIRE_PUNCH, MOVE_ICE_PUNCH, MOVE_CURSE}, .item = ITEM_EVIOLITE, .nature = NATURE_BRAVE, .ability = ABILITY_THICK_FAT, .evs = {252, 252, 4, 0, 0, 0}},
            {.moves = {MOVE_FOLLOW_ME, MOVE_YAWN, MOVE_ANCIENT_POWER, MOVE_HELPING_HAND}, .item = ITEM_EVIOLITE, .nature = NATURE_BOLD, .ability = ABILITY_SERENE_GRACE, .evs = {252, 0, 252, 0, 4, 0}},
        };
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS
            | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_CONSERVATIVE | AI_FLAG_DOUBLE_BATTLE);
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
            struct Pokemon mon;
            CreateRandomMonWithIVs(&mon, species[i], i == 8 || i >= 10 ? 29 : 30, MAX_PER_STAT_IVS);
            EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &sets[i]), EC_BATTLE_SET_SUCCESS);
            CalculateMonStats(&mon);
            if (i < 6) {
                PLAYER(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            } else {
                OPPONENT(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    bool32 fixedSpeed = FALSE;
                    SetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_HYPER_TRAINED_SPEED, &fixedSpeed);
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            }
        }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_PROTECT); }
    } THEN {
        EXPECT_EQ(opponentRight->species, SPECIES_KANGASKHAN_MEGA);
        EXPECT_EQ(GetBattlerAbility(B_BATTLER_3), ABILITY_PARENTAL_BOND);
        EXPECT_EQ(opponentRight->speed, 83);
        Test_MgbaPrintf("MEGA_BUDGET_INITIAL_FRAMES=%d", gBattleStruct->aiDelayFrames);
        EXPECT(gBattleStruct->aiDelayFrames <= 72);
    }
}
AI_DOUBLE_BATTLE_TEST("EC status legality: a spent burn is not a substitute for an action")
{
    GIVEN {
        // Frozen pre-authoring reproduction: first burn succeeds, then a
        // Sucker Punch threat made the failed repeat beat real attacks.
        const enum Species species[] = {SPECIES_DUGTRIO, SPECIES_XATU, SPECIES_SHEDINJA, SPECIES_VENOMOTH};
        const struct EmeraldChampionsBattleSet sets[] = {
            {.moves = {MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT}, .item = ITEM_FOCUS_SASH, .ability = ABILITY_ARENA_TRAP, .nature = NATURE_JOLLY, .evs = {4,252,0,0,0,252}},
            {.moves = {MOVE_PSYCHIC, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT}, .item = ITEM_EVIOLITE, .ability = ABILITY_MAGIC_BOUNCE, .nature = NATURE_TIMID, .evs = {4,0,0,252,0,252}},
            {.moves = {MOVE_POLTERGEIST, MOVE_SHADOW_SNEAK, MOVE_X_SCISSOR, MOVE_WILL_O_WISP}, .item = ITEM_SAFETY_GOGGLES, .ability = ABILITY_WONDER_GUARD, .nature = NATURE_JOLLY, .evs = {4,252,0,0,0,252}},
            {.moves = {MOVE_QUIVER_DANCE, MOVE_BUG_BUZZ, MOVE_SLEEP_POWDER, MOVE_PROTECT}, .item = ITEM_BLACK_SLUDGE, .ability = ABILITY_TINTED_LENS, .nature = NATURE_TIMID, .evs = {4,0,0,252,0,252}},
        };
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS
            | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_LUNG;
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
            struct Pokemon mon;
            CreateRandomMonWithIVs(&mon, species[i], i < 2 ? 40 : 38, MAX_PER_STAT_IVS);
            EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &sets[i]), EC_BATTLE_SET_SUCCESS);
            CalculateMonStats(&mon);
            if (i < 2) {
                PLAYER(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            } else {
                OPPONENT(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            }
        }
    } WHEN {
        for (u32 t = 0; t < 2; t++) TURN {
            MOVE(playerLeft, MOVE_EARTHQUAKE, hit: TRUE, criticalHit: FALSE);
            MOVE(playerRight, MOVE_AIR_SLASH, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
        }
    } THEN {
        EXPECT_EQ(playerLeft->status1, STATUS1_BURN);
        EXPECT_EQ(opponentLeft->pp[3], 14);
    }
}

AI_DOUBLE_BATTLE_TEST("EC status legality: preserve damaging moves and legal forced fallback")
{
    bool32 attack = FALSE;
    PARAMETRIZE { attack = FALSE; }
    PARAMETRIZE { attack = TRUE; }
    GIVEN {
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_BURN); Moves(MOVE_SPLASH); }
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_BURN); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_CHARMANDER) { Moves(MOVE_WILL_O_WISP, attack ? MOVE_FLAMETHROWER : MOVE_NONE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SPLASH);
            MOVE(playerRight, MOVE_SPLASH);
            EXPECT_MOVE(opponentLeft, attack ? MOVE_FLAMETHROWER : MOVE_WILL_O_WISP);
        }
    }
}
AI_DOUBLE_BATTLE_TEST("EC revealed redirection: do not repeatedly feed Volt Absorb")
{
    GIVEN {
        // Freeze the demonstrated board, not Jaylen's evolving campaign sets.
        const enum Species species[] = {SPECIES_POLITOED, SPECIES_PACHIRISU, SPECIES_CRYOGONAL, SPECIES_MAGNEZONE};
        const struct EmeraldChampionsBattleSet sets[] = {
            {.moves = {MOVE_SCALD, MOVE_ICE_BEAM, MOVE_HELPING_HAND, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .ability = ABILITY_DRIZZLE, .nature = NATURE_BOLD, .evs = {252,0,252,0,4,0}},
            {.moves = {MOVE_THUNDERBOLT, MOVE_SUPER_FANG, MOVE_FOLLOW_ME, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .ability = ABILITY_VOLT_ABSORB, .nature = NATURE_BOLD, .evs = {252,0,252,0,4,0}},
            {.moves = {MOVE_SNOWSCAPE, MOVE_FREEZE_DRY, MOVE_BLIZZARD, MOVE_RECOVER}, .item = ITEM_NEVER_MELT_ICE, .ability = ABILITY_LEVITATE, .nature = NATURE_TIMID, .evs = {4,0,0,252,0,252}},
            {.moves = {MOVE_THUNDERBOLT, MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_PROTECT}, .item = ITEM_SHUCA_BERRY, .ability = ABILITY_ANALYTIC, .nature = NATURE_BOLD, .evs = {252,0,252,4,0,0}},
        };
        AI_FLAGS(AI_FLAG_BASIC_TRAINER | AI_FLAG_OMNISCIENT | AI_FLAG_SMART_SWITCHING | AI_FLAG_SMART_MON_CHOICES
            | AI_FLAG_PP_STALL_PREVENTION | AI_FLAG_HP_AWARE | AI_FLAG_TRY_TO_2HKO | AI_FLAG_POWERFUL_STATUS
            | AI_FLAG_KNOW_OPPONENT_PARTY | AI_FLAG_DOUBLE_BATTLE);
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_JAYLEN;
        for (u32 i = 0; i < ARRAY_COUNT(species); i++) {
            struct Pokemon mon;
            CreateRandomMonWithIVs(&mon, species[i], i < 2 ? 40 : 38, MAX_PER_STAT_IVS);
            EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &sets[i]), EC_BATTLE_SET_SUCCESS);
            CalculateMonStats(&mon);
            if (i < 2) {
                PLAYER(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            } else {
                OPPONENT(species[i]) {
                    *gBattleTestRunnerState->data.currentMon = mon;
                    Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
                    Moves(sets[i].moves[0], sets[i].moves[1], sets[i].moves[2], sets[i].moves[3]);
                }
            }
        }
    } WHEN {
        for (u32 t = 0; t < 3; t++) TURN {
            if (t == 0) MOVE(playerLeft, MOVE_PROTECT);
            else MOVE(playerLeft, MOVE_SCALD, target: opponentRight, hit: TRUE, criticalHit: FALSE, secondaryEffect: FALSE);
            MOVE(playerRight, MOVE_FOLLOW_ME);
        }
    } THEN {
        // An initial prediction error is allowed. A revealed, still-legal
        // redirector must not receive repeated healing Thunderbolts.
        EXPECT(opponentRight->pp[0] >= 14);
    }
}
