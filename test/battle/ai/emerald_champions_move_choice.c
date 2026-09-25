#include "global.h"
#include "battle.h"
#include "battle_setup.h"
#include "battle_ai_util.h"
#include "test/battle.h"
#include "data.h"
#include "difficulty.h"
#include "event_data.h"
#include "malloc.h"
#include "constants/opponents.h"
#include "emerald_champions_battle_sets.h"

// Move and target choice on boards taken from the September 25 benchmark
// runs, replayed on the campaign's own teams: the compiled loadouts, the
// production stat generation, and each player set as its manifest prepared
// it. A member caught mid-battle carries the HP, item and status it had.

#if TESTING
extern u8 gTestPairBudgetPairs;
#endif

struct ChoiceMember { enum Species species; u16 hp; enum Item item; u32 status; };
EWRAM_DATA static struct ChoiceMember sChoiceMembers[8] = {0};

// The campaign cap is read from the milestone flags, exactly as the benchmark
// driver sets them.
static void ChoiceMilestones(const u16 *flags, u32 count)
{
    for (u32 i = 0; i < 8; i++)
        FlagClear(FLAG_BADGE01_GET + i);
    for (u32 i = 0; i < count; i++)
        FlagSet(flags[i]);
}

static void ChoiceDeclare(struct Pokemon *mon, enum Species form, enum Ability formAbility)
{
    if (form != SPECIES_NONE)
    {
        SetMonData(mon, MON_DATA_SPECIES, &form);
        CalculateMonStats(mon);
    }
    *gBattleTestRunnerState->data.currentMon = *mon;
    Nature(GetNature(mon));
    Ability(form != SPECIES_NONE ? formAbility : GetMonAbility(mon));
    Speed(GetMonData(mon, MON_DATA_SPEED));
    bool32 fixedSpeed = FALSE;
    SetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_HYPER_TRAINED_SPEED, &fixedSpeed);
    Moves(GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
          GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    for (u32 j = 0; j < ARRAY_COUNT(sChoiceMembers); j++)
        if (sChoiceMembers[j].species != SPECIES_NONE
         && (species == sChoiceMembers[j].species || (form != SPECIES_NONE && form == sChoiceMembers[j].species)))
        {
            HP(sChoiceMembers[j].hp);
            if (sChoiceMembers[j].item != ITEMS_COUNT)
                Item(sChoiceMembers[j].item);
            if (sChoiceMembers[j].status)
                Status1(sChoiceMembers[j].status);
        }
}

// The authored team in authored order, so a Mega slot keeps its permission.
// The two members on the field come from the given party slots; one may be
// declared in an already-evolved form.
static void ChoiceOpponent(u16 trainerId, u32 left, u32 right, u32 formSlot, enum Species form, enum Ability formAbility)
{
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][trainerId];
    struct Pokemon *party = AllocZeroed(sizeof(struct Pokemon) * PARTY_SIZE);
    u32 savedFlags = gBattleTypeFlags;
    enum DifficultyLevel savedDifficulty = GetCurrentDifficultyLevel();
    SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
    CreateNPCTrainerPartyFromTrainer(party, trainer);
    gBattleTypeFlags = savedFlags;
    SetCurrentDifficultyLevel(savedDifficulty);
    if (IsAITest())
        AI_FLAGS(trainer->aiFlags | AI_FLAG_DOUBLE_BATTLE);
    gBattleTestRunnerState->data.recordedBattle.opponentA = trainerId;
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = left;
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = right;
    for (u32 i = 0; i < trainer->partySize; i++)
    {
        OPPONENT(GetMonData(&party[i], MON_DATA_SPECIES))
        {
            ChoiceDeclare(&party[i], i == formSlot ? form : SPECIES_NONE, formAbility);
        }
    }
    memset(sChoiceMembers, 0, sizeof(sChoiceMembers));
    Free(party);
}

// A player member as its benchmark manifest prepared it: the cap level,
// perfect IVs and the set, at the HP and status it had on the board.
static void ChoicePlayer(enum Species species, u32 level, const struct EmeraldChampionsBattleSet *set, u32 hp, u32 status)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, level, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(set->moves[0], set->moves[1], set->moves[2], set->moves[3]);
        if (hp != 0xFFFF)
            HP(hp);
        if (status)
            Status1(status);
    }
}

#define FULL 0xFFFF

static const struct EmeraldChampionsBattleSet sWinonaFoes[] = {
    {.moves = {MOVE_FAKE_OUT, MOVE_FLARE_BLITZ, MOVE_KNOCK_OFF, MOVE_PARTING_SHOT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_ADAMANT, .ability = ABILITY_INTIMIDATE, .evs = {252, 252, 4, 0, 0, 0}},
    {.moves = {MOVE_THUNDERBOLT, MOVE_DAZZLING_GLEAM, MOVE_VOLT_SWITCH, MOVE_PROTECT}, .item = ITEM_LIFE_ORB, .nature = NATURE_TIMID, .ability = ABILITY_ELECTRIC_SURGE, .evs = {4, 0, 0, 252, 0, 252}},
    {.moves = {MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH, MOVE_WILD_CHARGE, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_ADAMANT, .ability = ABILITY_QUARK_DRIVE, .evs = {252, 252, 4, 0, 0, 0}},
    {.moves = {MOVE_SCALD, MOVE_THUNDERBOLT, MOVE_ICE_BEAM, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_MODEST, .ability = ABILITY_VOLT_ABSORB, .evs = {252, 0, 4, 252, 0, 0}},
    {.moves = {MOVE_METEOR_MASH, MOVE_ZEN_HEADBUTT, MOVE_BULLET_PUNCH, MOVE_PROTECT}, .item = ITEM_LIFE_ORB, .nature = NATURE_JOLLY, .ability = ABILITY_CLEAR_BODY, .evs = {4, 252, 0, 0, 0, 252}},
    {.moves = {MOVE_HYPER_VOICE, MOVE_PSYSHOCK, MOVE_MOONBLAST, MOVE_PROTECT}, .item = ITEM_GARDEVOIRITE, .nature = NATURE_TIMID, .ability = ABILITY_TRACE, .evs = {4, 0, 0, 252, 0, 252}},
};
static const u16 sFiveBadges[] = {FLAG_BADGE01_GET, FLAG_BADGE02_GET, FLAG_BADGE03_GET, FLAG_BADGE04_GET, FLAG_BADGE05_GET};

// vj-a/win-1 turn 0: Winona's Zapdos opened with a non-STAB Heat Wave that the
// Sitrus Incineroar resists, twice. Thunderbolt or Hurricane into Incineroar
// looked a quarter smaller than they were: the hit that pushes a Sitrus
// holder under half is refunded by its Berry on the one-turn board, and the
// Berry itself - HP the holder still has to spend - was never counted.
AI_DOUBLE_BATTLE_TEST("EC move choice: Winona's Zapdos hits a Sitrus Incineroar with STAB, not a resisted Heat Wave")
{
    GIVEN {
        ChoicePlayer(SPECIES_INCINEROAR, 55, &sWinonaFoes[0], FULL, 0);
        ChoicePlayer(SPECIES_TAPU_KOKO, 55, &sWinonaFoes[1], FULL, 0);
        ChoicePlayer(SPECIES_IRON_HANDS, 55, &sWinonaFoes[2], FULL, 0);
        ChoicePlayer(SPECIES_LANTURN, 55, &sWinonaFoes[3], FULL, 0);
        ChoicePlayer(SPECIES_METAGROSS, 55, &sWinonaFoes[4], FULL, 0);
        ChoicePlayer(SPECIES_GARDEVOIR, 55, &sWinonaFoes[5], FULL, 0);
        ChoiceMilestones(sFiveBadges, ARRAY_COUNT(sFiveBadges));
        ChoiceOpponent(TRAINER_WINONA_1, 0, 1, PARTY_SIZE, SPECIES_NONE, ABILITY_NONE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight);
            MOVE(playerRight, MOVE_DAZZLING_GLEAM);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_HEAT_WAVE);
        }
    }
}

// Both owners of a two-trainer multi from their authored teams, members
// caught mid-battle at the given HP; fallen members are left out, so each
// owner leads with its first standing member.
static void ChoiceMultiOwner(u16 trainerId, bool32 second)
{
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][trainerId];
    struct Pokemon *party = AllocZeroed(sizeof(struct Pokemon) * PARTY_SIZE);
    u32 savedFlags = gBattleTypeFlags;
    enum DifficultyLevel savedDifficulty = GetCurrentDifficultyLevel();
    SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_TWO_OPPONENTS;
    CreateNPCTrainerPartyFromTrainer(party, trainer);
    gBattleTypeFlags = savedFlags;
    SetCurrentDifficultyLevel(savedDifficulty);
    for (u32 i = 0; i < trainer->partySize; i++)
    {
        bool32 fallen = FALSE;
        for (u32 j = 0; j < ARRAY_COUNT(sChoiceMembers); j++)
            if (sChoiceMembers[j].species == GetMonData(&party[i], MON_DATA_SPECIES) && sChoiceMembers[j].hp == 0)
                fallen = TRUE;
        if (fallen)
            continue;
        if (second)
        {
            OPPONENT_B(GetMonData(&party[i], MON_DATA_SPECIES)) { ChoiceDeclare(&party[i], SPECIES_NONE, ABILITY_NONE); }
        }
        else
        {
            OPPONENT_A(GetMonData(&party[i], MON_DATA_SPECIES)) { ChoiceDeclare(&party[i], SPECIES_NONE, ABILITY_NONE); }
        }
    }
    Free(party);
}

static const struct EmeraldChampionsBattleSet sMossdeepTyranitar = {
    .moves = {MOVE_ROCK_SLIDE, MOVE_CRUNCH, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
    .item = ITEM_TYRANITARITE, .nature = NATURE_ADAMANT, .ability = ABILITY_SAND_STREAM, .evs = {252, 252, 4, 0, 0, 0},
};
// Steven's partner Metagross, as src/data/battle_partners.h builds it.
static const struct EmeraldChampionsBattleSet sMossdeepMetagross = {
    .moves = {MOVE_BULLET_PUNCH, MOVE_HEAVY_SLAM, MOVE_PSYCHIC_FANGS, MOVE_STOMPING_TANTRUM},
    .item = ITEM_ASSAULT_VEST, .nature = NATURE_ADAMANT, .ability = ABILITY_CLEAR_BODY, .evs = {4, 252, 0, 252, 0, 0},
};
static const u16 sMossdeepMilestones[] = {FLAG_BADGE01_GET, FLAG_BADGE02_GET, FLAG_BADGE03_GET, FLAG_BADGE04_GET,
    FLAG_BADGE05_GET, FLAG_BADGE06_GET, FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT, FLAG_BADGE07_GET};

// vj-b/mx-1 turn 2, the crowded Mossdeep multi: Mega Tyranitar and a 1 HP
// Metagross against Raging Bolt and a fresh Heatran. The shared clock ran out
// after seven of forty-nine pairs, all of them Heatran's first-ranked Magma
// Storm into the Metagross that Raging Bolt's priority Thunderclap removes
// first; the Magma Storm then fell, resisted, on the Tyranitar that Earth
// Power and Flash Cannon hit super effectively.
AI_MULTI_BATTLE_TEST("EC move choice: a search the Mossdeep clock cuts short still reaches Heatran's other attacks")
{
    GIVEN {
        ChoiceMilestones(sMossdeepMilestones, ARRAY_COUNT(sMossdeepMilestones));
        const struct Trainer *a = &gTrainers[DIFFICULTY_NORMAL][TRAINER_MAXIE_MOSSDEEP];
        const struct Trainer *b = &gTrainers[DIFFICULTY_NORMAL][TRAINER_COURTNEY_MOSSDEEP];
        AI_FLAGS(a->aiFlags | b->aiFlags | AI_FLAG_DOUBLE_BATTLE);
        gBattleTestRunnerState->data.recordedBattle.opponentA = TRAINER_MAXIE_MOSSDEEP;
        gBattleTestRunnerState->data.recordedBattle.opponentB = TRAINER_COURTNEY_MOSSDEEP;
        struct Pokemon mon;
        CreateRandomMonWithIVs(&mon, SPECIES_TYRANITAR, 70, MAX_PER_STAT_IVS);
        EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &sMossdeepTyranitar), EC_BATTLE_SET_SUCCESS);
        enum Species mega = SPECIES_TYRANITAR_MEGA;
        SetMonData(&mon, MON_DATA_SPECIES, &mega);
        CalculateMonStats(&mon);
        PLAYER(SPECIES_TYRANITAR_MEGA) {
            *gBattleTestRunnerState->data.currentMon = mon;
            Nature(GetNature(&mon)); Ability(ABILITY_SAND_STREAM); Speed(GetMonData(&mon, MON_DATA_SPEED));
            Moves(MOVE_ROCK_SLIDE, MOVE_CRUNCH, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT); HP(256);
        }
        CreateRandomMonWithIVs(&mon, SPECIES_METAGROSS, 72, MAX_PER_STAT_IVS);
        EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, &sMossdeepMetagross), EC_BATTLE_SET_SUCCESS);
        CalculateMonStats(&mon);
        PARTNER(SPECIES_METAGROSS) {
            *gBattleTestRunnerState->data.currentMon = mon;
            Nature(GetNature(&mon)); Ability(ABILITY_CLEAR_BODY); Speed(GetMonData(&mon, MON_DATA_SPEED));
            Moves(MOVE_BULLET_PUNCH, MOVE_HEAVY_SLAM, MOVE_PSYCHIC_FANGS, MOVE_STOMPING_TANTRUM); HP(1);
        }
        sChoiceMembers[0] = (struct ChoiceMember){SPECIES_CAMERUPT, 0, ITEMS_COUNT, 0};
        sChoiceMembers[1] = (struct ChoiceMember){SPECIES_CHARIZARD, 0, ITEMS_COUNT, 0};
        sChoiceMembers[2] = (struct ChoiceMember){SPECIES_GREAT_TUSK, 0, ITEMS_COUNT, 0};
        sChoiceMembers[3] = (struct ChoiceMember){SPECIES_RAGING_BOLT, 309, ITEMS_COUNT, 0};
        ChoiceMultiOwner(TRAINER_MAXIE_MOSSDEEP, FALSE);
        ChoiceMultiOwner(TRAINER_COURTNEY_MOSSDEEP, TRUE);
        memset(sChoiceMembers, 0, sizeof(sChoiceMembers));
        gTestPairBudgetPairs = 7;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_HIGH_HORSEPOWER, target: opponentRight);
            NOT_EXPECT_MOVE(playerRight, MOVE_HEAVY_SLAM);
            NOT_EXPECT_MOVE(opponentRight, MOVE_MAGMA_STORM);
        }
    } THEN {
        gTestPairBudgetPairs = 0;
    }
}

static const struct EmeraldChampionsBattleSet sShellyFoes[] = {
    {.moves = {MOVE_METEOR_MASH, MOVE_ZEN_HEADBUTT, MOVE_BULLET_PUNCH, MOVE_PROTECT}, .item = ITEM_LIFE_ORB, .nature = NATURE_JOLLY, .ability = ABILITY_CLEAR_BODY, .evs = {4, 252, 0, 0, 0, 252}},
    {.moves = {MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH, MOVE_WILD_CHARGE, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_ADAMANT, .ability = ABILITY_QUARK_DRIVE, .evs = {252, 252, 4, 0, 0, 0}},
    {.moves = {MOVE_LEAF_BLADE, MOVE_SACRED_SWORD, MOVE_SMART_STRIKE, MOVE_PROTECT}, .item = ITEM_LIFE_ORB, .nature = NATURE_JOLLY, .ability = ABILITY_BEAST_BOOST, .evs = {4, 252, 0, 0, 0, 252}},
};

// vj-b/she-1 turn 7: a 31 HP Metagross beside a fresh Iron Hands. Mega
// Feraligatr's Liquidation removes the Metagross first, and Rotom-Wash's
// Thunderbolt into it then falls, resisted, on Iron Hands. The Thunderbolt
// kept the knockout credit its isolated opinion gave the Metagross, so the
// pair sent both attacks at one body while Hydro Pump into Iron Hands stood.
AI_DOUBLE_BATTLE_TEST("EC move choice: Shelly's Rotom does not follow a partner's knockout into the same low body")
{
    GIVEN {
        ChoicePlayer(SPECIES_METAGROSS, 55, &sShellyFoes[0], 31, 0);
        ChoicePlayer(SPECIES_IRON_HANDS, 55, &sShellyFoes[1], FULL, 0);
        ChoicePlayer(SPECIES_KARTANA, 55, &sShellyFoes[2], 66, 0);
        ChoiceMilestones(sFiveBadges, ARRAY_COUNT(sFiveBadges));
        sChoiceMembers[0] = (struct ChoiceMember){SPECIES_NINETALES_ALOLA, 0, ITEMS_COUNT, 0};
        sChoiceMembers[1] = (struct ChoiceMember){SPECIES_EMPOLEON, 0, ITEMS_COUNT, 0};
        sChoiceMembers[2] = (struct ChoiceMember){SPECIES_CLEFABLE, 0, ITEMS_COUNT, 0};
        sChoiceMembers[3] = (struct ChoiceMember){SPECIES_CASTFORM, 0, ITEMS_COUNT, 0};
        sChoiceMembers[4] = (struct ChoiceMember){SPECIES_ROTOM_WASH, 122, ITEMS_COUNT, 0};
        sChoiceMembers[5] = (struct ChoiceMember){SPECIES_FERALIGATR_MEGA, 125, ITEMS_COUNT, 0};
        ChoiceOpponent(TRAINER_SHELLY_WEATHER_INSTITUTE, 4, 5, 5, SPECIES_FERALIGATR_MEGA, ABILITY_DRAGONIZE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_THUNDERBOLT);
        }
    }
}

static const struct EmeraldChampionsBattleSet sSidneyFoes[] = {
    {.moves = {MOVE_FAKE_OUT, MOVE_THROAT_CHOP, MOVE_FLARE_BLITZ, MOVE_PARTING_SHOT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_ADAMANT, .ability = ABILITY_INTIMIDATE, .evs = {252, 252, 4, 0, 0, 0}},
    {.moves = {MOVE_DRAIN_PUNCH, MOVE_LUNGE, MOVE_ICE_PUNCH, MOVE_PROTECT}, .item = ITEM_SITRUS_BERRY, .nature = NATURE_ADAMANT, .ability = ABILITY_BEAST_BOOST, .evs = {252, 252, 4, 0, 0, 0}},
};
static const u16 sEliteMilestones[] = {FLAG_BADGE01_GET, FLAG_BADGE02_GET, FLAG_BADGE03_GET, FLAG_BADGE04_GET,
    FLAG_BADGE05_GET, FLAG_BADGE06_GET, FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT, FLAG_BADGE07_GET, FLAG_BADGE08_GET};

// vj-b/tho-1 turns 3-5: Thomas's Scolipede arrived fresh, stayed in base form
// for its end-of-turn Speed Boost - the plan - then stayed again with the
// boost already banked, guarding, and evolved only on the third turn. One
// banked boost is the point of waiting; a second base-form turn only spends
// the Mega's turns. Sidney's Sharpedo is the same Speed Boost Mega on the
// board where its first turn is already covered.
AI_DOUBLE_BATTLE_TEST("EC Mega: a Speed Boost Mega evolves once its boost is banked")
{
    GIVEN {
        ChoicePlayer(SPECIES_INCINEROAR, 80, &sSidneyFoes[0], FULL, 0);
        ChoicePlayer(SPECIES_BUZZWOLE, 80, &sSidneyFoes[1], 220, 0);
        ChoiceMilestones(sEliteMilestones, ARRAY_COUNT(sEliteMilestones));
        sChoiceMembers[0] = (struct ChoiceMember){SPECIES_INCINEROAR, 0, ITEMS_COUNT, 0};
        sChoiceMembers[1] = (struct ChoiceMember){SPECIES_YVELTAL, 0, ITEMS_COUNT, 0};
        sChoiceMembers[2] = (struct ChoiceMember){SPECIES_GRIMMSNARL, 0, ITEMS_COUNT, 0};
        ChoiceOpponent(TRAINER_SIDNEY, 3, 4, PARTY_SIZE, SPECIES_NONE, ABILITY_NONE);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight);
            MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight);
        }
        TURN {
            MOVE(playerLeft, MOVE_THROAT_CHOP, target: opponentRight);
            MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        EXPECT_EQ(opponentRight->species, SPECIES_SHARPEDO_MEGA);
    }
}
