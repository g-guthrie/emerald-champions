#include "global.h"
#include "battle.h"
#include "battle_gimmick.h"
#include "battle_setup.h"
#include "battle_ai_util.h"
#include "emerald_champions_battle_plan.h"
#include "test/battle.h"
#include "data.h"
#include "difficulty.h"
#include "event_data.h"
#include "malloc.h"
#include "constants/opponents.h"
#include "emerald_champions_battle_sets.h"

// Switching and Mega timing on the campaign's own teams: the compiled
// loadouts and production stat generation, never a hand-copied set.

#if TESTING
extern bool8 gTestPairBudgetSpent;
#endif

struct SwitchInjury { enum Species species; u16 hp; u32 status; };
EWRAM_DATA static struct SwitchInjury sSwitchInjuries[6] = {0};

static void BuildAuthoredParty(u16 trainerId, u32 badges, struct Pokemon *party, u32 flags)
{
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][trainerId];
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
    gBattleTypeFlags = flags;
    CreateNPCTrainerPartyFromTrainer(party, trainer);
    gBattleTypeFlags = savedFlags;
    SetCurrentDifficultyLevel(savedDifficulty);
    for (u32 i = 0; i < ARRAY_COUNT(savedBadges); i++)
    {
        if (savedBadges[i])
            FlagSet(FLAG_BADGE01_GET + i);
        else
            FlagClear(FLAG_BADGE01_GET + i);
    }
}

static void DeclareAuthoredMon(struct Pokemon *mon)
{
    *gBattleTestRunnerState->data.currentMon = *mon;
    Nature(GetNature(mon));
    Ability(GetMonAbility(mon));
    Speed(GetMonData(mon, MON_DATA_SPEED));
    bool32 fixedSpeed = FALSE;
    SetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_HYPER_TRAINED_SPEED, &fixedSpeed);
    Moves(GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
          GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
    for (u32 j = 0; j < ARRAY_COUNT(sSwitchInjuries); j++)
        if (sSwitchInjuries[j].species != SPECIES_NONE
         && GetMonData(mon, MON_DATA_SPECIES) == sSwitchInjuries[j].species)
        {
            HP(sSwitchInjuries[j].hp);
            if (sSwitchInjuries[j].status)
                Status1(sSwitchInjuries[j].status);
        }
}

// A doubles opponent built from its authored team, in authored order.
static void SwitchAuthoredOpponent(u16 trainerId, u32 badges)
{
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][trainerId];
    struct Pokemon *party = AllocZeroed(sizeof(struct Pokemon) * PARTY_SIZE);
    BuildAuthoredParty(trainerId, badges, party, BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE);
    if (IsAITest())
        AI_FLAGS(trainer->aiFlags | AI_FLAG_DOUBLE_BATTLE);
    gBattleTestRunnerState->data.recordedBattle.opponentA = trainerId;
    for (u32 i = 0; i < trainer->partySize; i++)
    {
        OPPONENT(GetMonData(&party[i], MON_DATA_SPECIES))
        {
            DeclareAuthoredMon(&party[i]);
        }
    }
    memset(sSwitchInjuries, 0, sizeof(sSwitchInjuries));
    Free(party);
}

// Both owners of a two-trainer multi, each from its own authored team.
static void SwitchAuthoredMultiOpponents(u16 trainerA, u16 trainerB, u32 badges)
{
    const struct Trainer *a = &gTrainers[DIFFICULTY_NORMAL][trainerA];
    const struct Trainer *b = &gTrainers[DIFFICULTY_NORMAL][trainerB];
    struct Pokemon *party = AllocZeroed(sizeof(struct Pokemon) * PARTY_SIZE);
    u32 flags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_TWO_OPPONENTS;
    AI_FLAGS(a->aiFlags | b->aiFlags | AI_FLAG_DOUBLE_BATTLE);
    gBattleTestRunnerState->data.recordedBattle.opponentA = trainerA;
    gBattleTestRunnerState->data.recordedBattle.opponentB = trainerB;
    BuildAuthoredParty(trainerA, badges, party, flags);
    for (u32 i = 0; i < a->partySize; i++)
    {
        OPPONENT_A(GetMonData(&party[i], MON_DATA_SPECIES))
        {
            DeclareAuthoredMon(&party[i]);
        }
    }
    memset(party, 0, sizeof(struct Pokemon) * PARTY_SIZE);
    BuildAuthoredParty(trainerB, badges, party, flags);
    for (u32 i = 0; i < b->partySize; i++)
    {
        OPPONENT_B(GetMonData(&party[i], MON_DATA_SPECIES))
        {
            DeclareAuthoredMon(&party[i]);
        }
    }
    memset(sSwitchInjuries, 0, sizeof(sSwitchInjuries));
    Free(party);
}

// E0409, Mossdeep: Maxie's Camerupt and Courtney's Charizard lead together,
// and each owner may Mega Evolve once. Courtney's MEGA_REVEAL board was scored
// first, the shared multi clock ran out right after it, and the board where
// both evolve was never reached: Camerupt stayed in base form in every run.
AI_MULTI_BATTLE_TEST("EC multi mega: Maxie's Camerupt evolves beside Courtney's revealed Charizard")
{
    bool32 spent;
    PARAMETRIZE { spent = FALSE; }
    PARAMETRIZE { spent = TRUE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Level(60); HP(500); MaxHP(500); Defense(250); SpDefense(250); Speed(10); Moves(MOVE_CELEBRATE); }
        PARTNER(SPECIES_WOBBUFFET) { Level(60); HP(500); MaxHP(500); Defense(250); SpDefense(250); Speed(9); Moves(MOVE_CELEBRATE); }
        SwitchAuthoredMultiOpponents(TRAINER_MAXIE_MOSSDEEP, TRAINER_COURTNEY_MOSSDEEP, 7);
        gTestPairBudgetSpent = spent;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
        }
    } THEN {
        gTestPairBudgetSpent = FALSE;
        EXPECT_EQ(opponentRight->species, SPECIES_CHARIZARD_MEGA_Y);
        EXPECT_EQ(opponentLeft->species, SPECIES_CAMERUPT_MEGA);
    }
}

// A benchmark player member: the prepared set at the fight's cap, perfect IVs
// and the set's EVs, at the HP it had on the board being replayed.
static void SwitchPlayer(enum Species species, u32 level, u16 hp, const struct EmeraldChampionsBattleSet *set)
{
    struct Pokemon mon;
    CreateRandomMonWithIVs(&mon, species, level, MAX_PER_STAT_IVS);
    EXPECT_EQ(ApplyEmeraldChampionsScriptedSet(&mon, set), EC_BATTLE_SET_SUCCESS);
    CalculateMonStats(&mon);
    PLAYER(species) {
        *gBattleTestRunnerState->data.currentMon = mon;
        Nature(GetNature(&mon)); Ability(GetMonAbility(&mon)); Speed(GetMonData(&mon, MON_DATA_SPEED));
        Moves(set->moves[0], set->moves[1], set->moves[2], set->moves[3]);
        if (hp)
            HP(hp);
    }
}

#define SET(m1, m2, m3, m4, _nature, _ability, _item, hp, atk, def, spa, spd, spe) \
    (&(const struct EmeraldChampionsBattleSet){ .moves = {m1, m2, m3, m4}, .nature = _nature, \
        .ability = _ability, .item = _item, .evs = {hp, atk, def, spa, spd, spe} })

#define INCINEROAR_SET SET(MOVE_FAKE_OUT, MOVE_FLARE_BLITZ, MOVE_KNOCK_OFF, MOVE_PARTING_SHOT, NATURE_ADAMANT, ABILITY_INTIMIDATE, ITEM_SITRUS_BERRY, 252, 252, 4, 0, 0, 0)

// The engine sends out the first living members, so a board that begins
// mid-battle declares its fallen reserves at zero HP. Tell the runner which
// slots those leads came from.
static void SwitchLeads(u32 left, u32 right)
{
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_1] = left;
    gBattleTestRunnerState->data.currentMonIndexes[B_BATTLER_3] = right;
}

// w4-16b/sid-1 turn 4: Sharpedo arrives beside Muk into Fake Out and Drain
// Punch, shields, and evolves on the shield turn - so Speed Boost never fires
// and Mega Sharpedo fights at base Speed. The plan is the boost first, then
// the Mega: a guarded base-form turn earns +1 Speed the Mega keeps.
AI_DOUBLE_BATTLE_TEST("EC Mega: Sidney's Sharpedo earns its Speed Boost before evolving")
{
    GIVEN {
        SwitchPlayer(SPECIES_INCINEROAR, 80, 0, SET(MOVE_FAKE_OUT, MOVE_THROAT_CHOP, MOVE_FLARE_BLITZ, MOVE_PARTING_SHOT, NATURE_ADAMANT, ABILITY_INTIMIDATE, ITEM_SITRUS_BERRY, 252, 252, 4, 0, 0, 0));
        SwitchPlayer(SPECIES_BUZZWOLE, 80, 220, SET(MOVE_DRAIN_PUNCH, MOVE_LUNGE, MOVE_ICE_PUNCH, MOVE_PROTECT, NATURE_ADAMANT, ABILITY_BEAST_BOOST, ITEM_SITRUS_BERRY, 252, 252, 4, 0, 0, 0));
        sSwitchInjuries[0] = (struct SwitchInjury){SPECIES_INCINEROAR, 0};
        sSwitchInjuries[1] = (struct SwitchInjury){SPECIES_YVELTAL, 0};
        sSwitchInjuries[2] = (struct SwitchInjury){SPECIES_GRIMMSNARL, 0};
        SwitchLeads(3, 4);
        SwitchAuthoredOpponent(TRAINER_SIDNEY, 8);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentRight);
            MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight);
        }
    } THEN {
        EXPECT_EQ(opponentRight->species, SPECIES_SHARPEDO);
        EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

// w4-16b/gla-2 turn 2: Froslass and Sandslash arrive after a turn-one double
// knockout into Mega Charizard Y's sun, and Froslass - whose Mega brings Snow
// Warning - spends its turn in base form. Glacia's plan is snow, and taking
// the weather back is what that Mega is for.
AI_DOUBLE_BATTLE_TEST("EC Mega: Glacia's Froslass evolves into Mega Charizard's sun")
{
    GIVEN {
        SwitchPlayer(SPECIES_CHARIZARD, 80, 24, SET(MOVE_HEAT_WAVE, MOVE_FOCUS_BLAST, MOVE_SOLAR_BEAM, MOVE_PROTECT, NATURE_TIMID, ABILITY_BLAZE, ITEM_CHARIZARDITE_Y, 4, 0, 0, 252, 0, 252));
        SwitchPlayer(SPECIES_INCINEROAR, 80, 316, INCINEROAR_SET);
        sSwitchInjuries[0] = (struct SwitchInjury){SPECIES_NINETALES_ALOLA, 0};
        sSwitchInjuries[1] = (struct SwitchInjury){SPECIES_KYUREM, 1, STATUS1_TOXIC_POISON};
        sSwitchInjuries[2] = (struct SwitchInjury){SPECIES_CHIEN_PAO, 1, STATUS1_TOXIC_POISON};
        sSwitchInjuries[3] = (struct SwitchInjury){SPECIES_BAXCALIBUR, 0};
        SwitchLeads(1, 2);
        SwitchAuthoredOpponent(TRAINER_GLACIA, 8);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT, gimmick: GIMMICK_MEGA);
            MOVE(playerRight, MOVE_FAKE_OUT, target: opponentLeft);
            EXPECT_SEND_OUT(opponentLeft, 3);
            EXPECT_SEND_OUT(opponentRight, 4);
        }
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_FLARE_BLITZ, target: opponentRight);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_FROSLASS_MEGA);
    }
}

#define WHIMSICOTT_SET SET(MOVE_TAILWIND, MOVE_MOONBLAST, MOVE_ENCORE, MOVE_PROTECT, NATURE_TIMID, ABILITY_PRANKSTER, ITEM_FOCUS_SASH, 4, 0, 0, 252, 0, 252)

// w4-17/wdl-5 turns 2-4: Whimsicott's Encore held Wallace's Kyogre in a move
// that could not help it - there a Protect that was one-in-three, here a Water
// Spout both foes resist - and Kyogre stayed in until it fell. A lock that
// buys nothing for turns to come is a reason to leave, not to wait it out.
AI_DOUBLE_BATTLE_TEST("EC switching: Wallace's Kyogre leaves an Encore into a move both foes resist")
{
    GIVEN {
        SwitchPlayer(SPECIES_WHIMSICOTT, 100, 0, WHIMSICOTT_SET);
        SwitchPlayer(SPECIES_MIRAIDON, 100, 0, SET(MOVE_ELECTRO_DRIFT, MOVE_DRACO_METEOR, MOVE_DAZZLING_GLEAM, MOVE_PROTECT, NATURE_TIMID, ABILITY_HADRON_ENGINE, ITEM_LIFE_ORB, 4, 0, 0, 252, 0, 252));
        sSwitchInjuries[0] = (struct SwitchInjury){SPECIES_LUCARIO, 0};
        sSwitchInjuries[1] = (struct SwitchInjury){SPECIES_ZAPDOS, 0};
        SwitchLeads(0, 3);
        SwitchAuthoredOpponent(TRAINER_WALLACE_DOUBLES_LEGENDS, 8);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentLeft, MOVE_WATER_SPOUT);
        }
        TURN {
            MOVE(playerLeft, MOVE_ENCORE, target: opponentLeft);
            MOVE(playerRight, MOVE_DAZZLING_GLEAM, target: opponentLeft);
        }
        TURN {
            MOVE(playerLeft, MOVE_MOONBLAST, target: opponentRight);
            MOVE(playerRight, MOVE_DAZZLING_GLEAM, target: opponentLeft);
        }
    } THEN {
        EXPECT_NE(opponentLeft->species, SPECIES_KYOGRE_PRIMAL);
    }
}

// The reserve a forced exit brings is chosen by what the foes on the board can
// visibly do to it. Excadrill's Earthquake threatens both foes, which the old
// power-times-type shortlist rewarded above everything, but Incineroar's
// Flare Blitz is super effective on it; Centiskorch absorbs that and resists
// Metagross. Only the first name on the shortlist reached the board.
AI_DOUBLE_BATTLE_TEST("EC switching: a Choice-lock exit prices each reserve against the foes' visible attacks")
{
    GIVEN {
        AI_FLAGS(AI_FLAG_CHECK_BAD_MOVE | AI_FLAG_CHECK_VIABILITY | AI_FLAG_TRY_TO_FAINT | AI_FLAG_SMART_SWITCHING
            | AI_FLAG_SMART_MON_CHOICES | AI_FLAG_OMNISCIENT | AI_FLAG_DOUBLE_BATTLE);
        PLAYER(SPECIES_MACHAMP) { Level(50); Speed(40); Moves(MOVE_CLOSE_COMBAT, MOVE_ROCK_SLIDE, MOVE_PROTECT, MOVE_KNOCK_OFF); }
        PLAYER(SPECIES_TOXICROAK) { Level(50); Speed(45); Moves(MOVE_DRAIN_PUNCH, MOVE_GUNK_SHOT, MOVE_PROTECT, MOVE_SUCKER_PUNCH); }
        PLAYER(SPECIES_INCINEROAR) { Level(50); Speed(50); Ability(ABILITY_BLAZE); Moves(MOVE_FLARE_BLITZ, MOVE_KNOCK_OFF, MOVE_PROTECT, MOVE_PARTING_SHOT); }
        PLAYER(SPECIES_METAGROSS) { Level(50); Speed(60); Moves(MOVE_METEOR_MASH, MOVE_ZEN_HEADBUTT, MOVE_PROTECT, MOVE_ICE_PUNCH); }
        OPPONENT(SPECIES_VICTINI) {
            Level(50); Speed(120); Item(ITEM_CHOICE_SPECS); Ability(ABILITY_VICTORY_STAR); Nature(NATURE_TIMID);
            Moves(MOVE_HEAT_WAVE, MOVE_PSYCHIC, MOVE_FOCUS_BLAST, MOVE_ENERGY_BALL);
        }
        OPPONENT(SPECIES_AUDINO) {
            Level(50); Speed(50); Item(ITEM_SITRUS_BERRY); Ability(ABILITY_REGENERATOR);
            Moves(MOVE_WISH, MOVE_PROTECT, MOVE_HELPING_HAND, MOVE_THROAT_CHOP);
        }
        OPPONENT(SPECIES_EXCADRILL) {
            Level(50); Speed(90); Item(ITEM_LIFE_ORB); Ability(ABILITY_MOLD_BREAKER);
            Moves(MOVE_EARTHQUAKE, MOVE_IRON_HEAD, MOVE_ROCK_SLIDE, MOVE_PROTECT);
        }
        OPPONENT(SPECIES_CENTISKORCH) {
            Level(50); Speed(70); Item(ITEM_SITRUS_BERRY); Ability(ABILITY_FLASH_FIRE);
            Moves(MOVE_FIRE_LASH, MOVE_POWER_WHIP, MOVE_PROTECT);
        }
    } WHEN {
        TURN {
            SWITCH(playerLeft, 2);
            SWITCH(playerRight, 3);
            EXPECT_MOVE(opponentLeft, MOVE_PSYCHIC);
        }
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_SWITCH(opponentLeft, 3);
        }
    }
}
