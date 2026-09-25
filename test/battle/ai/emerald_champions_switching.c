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

// A member's state on the board being replayed. A held move leaves every
// other move without PP, so a replay can re-create the Choice lock it had.
// noItem: the member's item was spent before this board.
struct SwitchInjury { enum Species species; u16 hp; u32 status; enum Move held; bool32 noItem; };
// A milestone past the badges that sets the fight's level cap (the Hall of
// Fame, Groudon's awakening). Consumed by the next authored build.
EWRAM_DATA static u16 sSwitchMilestone = 0;
// Two authored party slots that trade places, so a board's leads stand where
// they stood on the benchmark. Consumed by the next authored build.
EWRAM_DATA static u8 sSwitchSwap[2] = {0};
// The species a board led with, brought to the front in this order.
EWRAM_DATA static enum Species sSwitchLeadSpecies[2] = {0};
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
    bool8 savedMilestone = sSwitchMilestone ? FlagGet(sSwitchMilestone) : FALSE;
    if (sSwitchMilestone)
        FlagSet(sSwitchMilestone);
    SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
    gBattleTypeFlags = flags;
    CreateNPCTrainerPartyFromTrainer(party, trainer);
    gBattleTypeFlags = savedFlags;
    if (sSwitchMilestone && !savedMilestone)
        FlagClear(sSwitchMilestone);
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
            for (u32 slot = 0; slot < MAX_MON_MOVES && sSwitchInjuries[j].held != MOVE_NONE; slot++)
            {
                u8 spent = 0;
                if (GetMonData(mon, MON_DATA_MOVE1 + slot) != sSwitchInjuries[j].held)
                    SetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_PP1 + slot, &spent);
            }
            if (sSwitchInjuries[j].noItem)
                Item(ITEM_NONE);
        }
}

// A doubles opponent built from its authored team, in authored order.
static void SwitchAuthoredOpponent(u16 trainerId, u32 badges)
{
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][trainerId];
    struct Pokemon *party = AllocZeroed(sizeof(struct Pokemon) * PARTY_SIZE);
    BuildAuthoredParty(trainerId, badges, party, BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE);
    if (sSwitchSwap[0] != sSwitchSwap[1])
    {
        struct Pokemon swap = party[sSwitchSwap[0]];
        party[sSwitchSwap[0]] = party[sSwitchSwap[1]];
        party[sSwitchSwap[1]] = swap;
        sSwitchSwap[0] = sSwitchSwap[1] = 0;
    }
    for (u32 lead = 0; lead < ARRAY_COUNT(sSwitchLeadSpecies); lead++)
    {
        for (u32 i = 0; sSwitchLeadSpecies[lead] != SPECIES_NONE && i < PARTY_SIZE; i++)
            if (GetMonData(&party[i], MON_DATA_SPECIES) == sSwitchLeadSpecies[lead])
            {
                struct Pokemon swap = party[lead];
                party[lead] = party[i];
                party[i] = swap;
                break;
            }
        sSwitchLeadSpecies[lead] = SPECIES_NONE;
    }
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
    sSwitchMilestone = 0;
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
EWRAM_DATA static u32 sSwitchPlayerStatus = 0; // Consumed by the next SwitchPlayer.

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
        if (sSwitchPlayerStatus)
            Status1(sSwitchPlayerStatus);
    }
    sSwitchPlayerStatus = 0;
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
// Both reserves are Wallace's aces. As on the benchmark board, Miraidon has
// spent a Draco Meteor, and neither ace falls to its Electro Drift on arrival.
// At full Special Attack that Electro Drift removes either one, and Kyogre
// waits the Encore out rather than hand an ace over for it.
AI_DOUBLE_BATTLE_TEST("EC switching: Wallace's Kyogre leaves a useless Encore for an ace only when the ace survives entry")
{
    bool32 dracoSpent;
    PARAMETRIZE { dracoSpent = TRUE; }
    PARAMETRIZE { dracoSpent = FALSE; }
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
            if (dracoSpent)
                MOVE(playerRight, MOVE_DRACO_METEOR, target: opponentRight);
            else
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
        if (dracoSpent)
            EXPECT_NE(opponentLeft->species, SPECIES_KYOGRE_PRIMAL);
        else
            EXPECT_EQ(opponentLeft->species, SPECIES_KYOGRE_PRIMAL);
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

// An ace leaves the bench to end a useless lock only into a board where it
// lives to act. Each of these replays a benchmark board whose only reserve (or
// the reserve the exit chose) was the trainer's ace. Where a replay's first
// turn re-creates a Choice lock, the locked member's other moves are spent so
// the lock is the one the benchmark board had.

#define SHAWN_LANTURN_SET SET(MOVE_SCALD, MOVE_ICE_BEAM, MOVE_THUNDER_WAVE, MOVE_PROTECT, NATURE_MODEST, ABILITY_VOLT_ABSORB, ITEM_LIFE_ORB, 0, 0, 0, 0, 0, 0)
#define SHAWN_DURALUDON_SET SET(MOVE_FLASH_CANNON, MOVE_DRACO_METEOR, MOVE_THUNDERBOLT, MOVE_PROTECT, NATURE_MODEST, ABILITY_STALWART, ITEM_CHOICE_SPECS, 0, 0, 0, 0, 0, 0)

// w4-02/r36c turn 8: Crabominable's Band Ice Punch is held into Bronzor, the
// last foe, which resists it. Jocelyn's ace Sirfetch'd takes the lock's place;
// Bronzor's best answer, Psychic, leaves it standing. The same board with
// Sirfetch'd low enough for that Psychic to finish it keeps the lock: the exit
// would hand the ace over for the one turn the lock costs. The first turn
// sets the lock on a stand-in that falls to it, brings in Breloom, and has
// Bronzor restore the board's Trick Room.
AI_DOUBLE_BATTLE_TEST("EC switching: Jocelyn's Crabominable leaves a useless lock for her ace only when the ace survives entry")
{
    u16 aceHp = 0;
    PARAMETRIZE { aceHp = 0; }
    PARAMETRIZE { aceHp = 12; }
    GIVEN {
        SwitchPlayer(SPECIES_BRONZOR, 20, 0, SET(MOVE_TRICK_ROOM, MOVE_GYRO_BALL, MOVE_PSYCHIC, MOVE_PROTECT, NATURE_RELAXED, ABILITY_LEVITATE, ITEM_EVIOLITE, 0, 0, 0, 0, 0, 0));
        PLAYER(SPECIES_TROPIUS) { Level(20); HP(30); Speed(200); Moves(MOVE_AIR_SLASH); }
        sSwitchInjuries[0] = (struct SwitchInjury){SPECIES_LILLIGANT_HISUI, 0};
        sSwitchInjuries[1] = (struct SwitchInjury){SPECIES_ORICORIO_POM_POM, 0};
        sSwitchInjuries[2] = (struct SwitchInjury){SPECIES_WHIMSICOTT, 10};
        sSwitchInjuries[3] = (struct SwitchInjury){SPECIES_CRABOMINABLE, 56, 0, MOVE_ICE_PUNCH};
        if (aceHp)
            sSwitchInjuries[4] = (struct SwitchInjury){SPECIES_SIRFETCHD, aceHp};
        SwitchLeads(2, 3);
        SwitchAuthoredOpponent(TRAINER_JOCELYN, 1);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TRICK_ROOM);
            MOVE(playerRight, MOVE_AIR_SLASH, target: opponentLeft);
            EXPECT_MOVE(opponentRight, MOVE_ICE_PUNCH, target: playerRight);
            EXPECT_SEND_OUT(opponentLeft, 4);
        }
        TURN {
            MOVE(playerLeft, MOVE_PSYCHIC, target: opponentRight);
            SKIP_TURN(playerRight);
            if (aceHp == 0)
                EXPECT_SWITCH(opponentRight, 5);
            else
                EXPECT_MOVE(opponentRight, MOVE_ICE_PUNCH);
        }
    }
}

// w4-04/sh83a turn 7: Jolteon's Specs Thunderbolt is held into Duraludon
// (resists) and Lanturn (Volt Absorb), and the one reserve left is Shawn's ace
// Ampharos, which nothing on the board knocks out on arrival. The lock still
// pays: the resisted Specs hit and Pawmot's Ice Punch take Duraludon down
// together, so the exit loses on the board and Jolteon stays.
AI_DOUBLE_BATTLE_TEST("EC switching: Shawn's Jolteon keeps a resisted lock that still threatens Duraludon")
{
    GIVEN {
        PLAYER(SPECIES_GYARADOS) { Level(30); HP(80); Ability(ABILITY_MOXIE); Speed(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_FLYGON) { Level(30); Ability(ABILITY_LEVITATE); Speed(1); Moves(MOVE_CELEBRATE); }
        SwitchPlayer(SPECIES_DURALUDON, 30, 0, SHAWN_DURALUDON_SET);
        SwitchPlayer(SPECIES_LANTURN, 30, 112, SHAWN_LANTURN_SET);
        sSwitchInjuries[0] = (struct SwitchInjury){SPECIES_ELECTRODE_HISUI, 0};
        sSwitchInjuries[1] = (struct SwitchInjury){SPECIES_BELLIBOLT, 0};
        sSwitchInjuries[2] = (struct SwitchInjury){SPECIES_ROTOM_WASH, 0};
        sSwitchInjuries[3] = (struct SwitchInjury){SPECIES_PAWMOT, 73};
        sSwitchInjuries[4] = (struct SwitchInjury){SPECIES_JOLTEON, 49, STATUS1_BURN};
        SwitchLeads(3, 4);
        SwitchAuthoredOpponent(TRAINER_SHAWN, 2);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            EXPECT_MOVE(opponentLeft, MOVE_ICE_PUNCH, target: playerRight);
            EXPECT_MOVE(opponentRight, MOVE_THUNDERBOLT, target: playerLeft);
            SEND_OUT(playerLeft, 2);
            SEND_OUT(playerRight, 3);
        }
        TURN {
            MOVE(playerLeft, MOVE_DRACO_METEOR, target: opponentLeft);
            MOVE(playerRight, MOVE_SCALD, target: opponentRight);
            EXPECT_MOVE(opponentRight, MOVE_THUNDERBOLT);
        }
    }
}

// w4-04/wa84d turn 7: Galvantula's Specs Thunder is held into Duraludon
// (resists) and Clodsire (immune). The one reserve left is Wattson's ace
// Manectric, and Clodsire's Life Orb High Horsepower removes it on arrival.
// The first turn sets the lock on a stand-in that falls to it.
AI_DOUBLE_BATTLE_TEST("EC switching: Wattson's Galvantula keeps a useless lock rather than spend an ace that falls on entry")
{
    GIVEN {
        PLAYER(SPECIES_GYARADOS) { Level(30); HP(60); Ability(ABILITY_MOXIE); Speed(1); Moves(MOVE_CELEBRATE); }
        SwitchPlayer(SPECIES_CLODSIRE, 30, 0, SET(MOVE_HIGH_HORSEPOWER, MOVE_POISON_JAB, MOVE_RECOVER, MOVE_PROTECT, NATURE_ADAMANT, ABILITY_UNAWARE, ITEM_LIFE_ORB, 0, 0, 0, 0, 0, 0));
        SwitchPlayer(SPECIES_DURALUDON, 30, 0, SHAWN_DURALUDON_SET);
        sSwitchInjuries[0] = (struct SwitchInjury){SPECIES_ELECTRODE, 37};
        sSwitchInjuries[1] = (struct SwitchInjury){SPECIES_ELECTIVIRE, 0};
        sSwitchInjuries[2] = (struct SwitchInjury){SPECIES_MAGNEZONE, 0};
        sSwitchInjuries[3] = (struct SwitchInjury){SPECIES_THUNDURUS, 0};
        sSwitchInjuries[4] = (struct SwitchInjury){SPECIES_GALVANTULA, 34, 0, MOVE_THUNDER};
        SwitchLeads(0, 4);
        SwitchAuthoredOpponent(TRAINER_WATTSON_1, 2);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_PROTECT);
            EXPECT_MOVE(opponentRight, MOVE_THUNDER, target: playerLeft);
            SEND_OUT(playerLeft, 2);
        }
        TURN {
            MOVE(playerLeft, MOVE_FLASH_CANNON, target: opponentRight);
            MOVE(playerRight, MOVE_HIGH_HORSEPOWER, target: opponentLeft);
            EXPECT_MOVE(opponentRight, MOVE_THUNDER);
        }
    }
}

// w4-17/cyn-1 turns 6-7: Lucario's Scarf Close Combat takes Kartana, then is
// held into Flutter Mane (immune) and Amoonguss (resists). The one reserve
// left is Cynthia's ace Garchomp at 55 HP, which Flutter Mane's Moonblast
// removes on arrival: the exit would spend the ace for nothing.
AI_DOUBLE_BATTLE_TEST("EC switching: Cynthia's Lucario keeps a useless lock rather than spend an ace that falls on entry")
{
    bool32 champion = FlagGet(FLAG_IS_CHAMPION);
    GIVEN {
        FlagSet(FLAG_IS_CHAMPION);
        SwitchPlayer(SPECIES_FLUTTER_MANE, 100, 0, SET(MOVE_MOONBLAST, MOVE_SHADOW_BALL, MOVE_DAZZLING_GLEAM, MOVE_PROTECT, NATURE_TIMID, ABILITY_PROTOSYNTHESIS, ITEM_BOOSTER_ENERGY, 4, 0, 0, 252, 0, 252));
        SwitchPlayer(SPECIES_KARTANA, 100, 208, SET(MOVE_LEAF_BLADE, MOVE_SACRED_SWORD, MOVE_SMART_STRIKE, MOVE_PROTECT, NATURE_JOLLY, ABILITY_BEAST_BOOST, ITEM_LIFE_ORB, 4, 252, 0, 0, 0, 252));
        SwitchPlayer(SPECIES_AMOONGUSS, 100, 0, SET(MOVE_SPORE, MOVE_RAGE_POWDER, MOVE_POLLEN_PUFF, MOVE_PROTECT, NATURE_RELAXED, ABILITY_REGENERATOR, ITEM_ROCKY_HELMET, 252, 0, 252, 0, 4, 0));
        sSwitchInjuries[0] = (struct SwitchInjury){SPECIES_MILOTIC, 0};
        sSwitchInjuries[1] = (struct SwitchInjury){SPECIES_TOGEKISS, 0};
        sSwitchInjuries[2] = (struct SwitchInjury){SPECIES_MIRAIDON, 0};
        sSwitchInjuries[3] = (struct SwitchInjury){SPECIES_GARCHOMP, 55};
        SwitchLeads(2, 3);
        SwitchAuthoredOpponent(TRAINER_CYNTHIA_1, 8);
        if (!champion)
            FlagClear(FLAG_IS_CHAMPION);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_MOONBLAST, target: opponentLeft);
            MOVE(playerRight, MOVE_SACRED_SWORD, target: opponentRight);
            EXPECT_MOVE(opponentRight, MOVE_CLOSE_COMBAT, target: playerRight);
            SEND_OUT(playerRight, 2);
        }
        TURN {
            MOVE(playerLeft, MOVE_MOONBLAST, target: opponentLeft);
            MOVE(playerRight, MOVE_SPORE, target: opponentRight);
            EXPECT_MOVE(opponentRight, MOVE_CLOSE_COMBAT);
        }
    }
}

#define TALONFLAME_SET SET(MOVE_BRAVE_BIRD, MOVE_FLARE_BLITZ, MOVE_TAILWIND, MOVE_PROTECT, NATURE_JOLLY, ABILITY_GALE_WINGS, ITEM_NONE, 4, 252, 0, 0, 0, 252)
#define GARDEVOIR_SET SET(MOVE_HYPER_VOICE, MOVE_PSYSHOCK, MOVE_MOONBLAST, MOVE_PROTECT, NATURE_TIMID, ABILITY_TRACE, ITEM_GARDEVOIRITE, 4, 0, 0, 252, 0, 252)
#define MIRAIDON_SET SET(MOVE_ELECTRO_DRIFT, MOVE_DRACO_METEOR, MOVE_DAZZLING_GLEAM, MOVE_PROTECT, NATURE_TIMID, ABILITY_HADRON_ENGINE, ITEM_LIFE_ORB, 4, 0, 0, 252, 0, 252)

// Whether this battler's pair decision chose to switch to that party slot.
static bool32 PairSwitchedTo(enum BattlerId battler, u32 slot)
{
    return (gAiSwitchTrace[battler] & AI_SWITCH_FROM_PAIR)
        && ((gAiSwitchTrace[battler] >> AI_SWITCH_SLOT_SHIFT) & 7) == slot;
}

extern void (*gTestAiTurnSetupHook)(void);

// vj-a/lily-1 turn 3, restored as it stood: Hariyama's Close Combat drops,
// Miraidon's Electric Terrain with two turns left (no Spore on Gardevoir),
// Breloom and Hariyama a turn past Fake Out, and the Poison Heal Gardevoir
// Traced from Breloom.
static void LilycoveBoard(void)
{
    gBattleMons[B_BATTLER_3].statStages[STAT_DEF] = DEFAULT_STAT_STAGE - 1;
    gBattleMons[B_BATTLER_3].statStages[STAT_SPDEF] = DEFAULT_STAT_STAGE - 1;
    gBattleStruct->battlerState[B_BATTLER_1].isFirstTurn = 0;
    gBattleStruct->battlerState[B_BATTLER_3].isFirstTurn = 0;
    gFieldTimers.terrain = B_TERRAIN_ELECTRIC;
    gFieldTimers.terrainTimer = 2;
    gBattleStruct->tracedAbility[B_BATTLER_2] = ABILITY_POISON_HEAL;
    gBattleMons[B_BATTLER_2].ability = gBattleMons[B_BATTLER_2].volatiles.overwrittenAbility = ABILITY_POISON_HEAL;
    gBattleMons[B_BATTLER_2].volatiles.traceActivated = TRUE;
}

// vj-a/lily-1 turn 3: Breloom, poisoned by its Orb and facing Talonflame's
// Brave Bird, left for the Mega-ace Salamence beside a Gardevoir holding its
// stone. Gardevoir evolved first and Pixilate Hyper Voice removed Salamence
// before it moved. The entry was only ever checked against moves the player
// had already used, and this Gardevoir had not attacked yet - though even its
// base-form Moonblast removes Salamence on arrival. Hand-built boards without
// the terrain, the Close Combat drops and the spent Fake Out stayed in.
AI_DOUBLE_BATTLE_TEST("EC switching: Lilycove Brendan keeps Salamence out of a visible Mega Gardevoir")
{
    GIVEN {
        memset(gAiSwitchTrace, 0, sizeof(gAiSwitchTrace));
        SwitchPlayer(SPECIES_TALONFLAME, 60, 124, TALONFLAME_SET);
        SwitchPlayer(SPECIES_GARDEVOIR, 60, 154, GARDEVOIR_SET);
        sSwitchPlayerStatus = STATUS1_PARALYSIS;
        SwitchPlayer(SPECIES_INCINEROAR, 60, 107, SET(MOVE_FAKE_OUT, MOVE_FLARE_BLITZ, MOVE_KNOCK_OFF, MOVE_PARTING_SHOT, NATURE_ADAMANT, ABILITY_INTIMIDATE, ITEM_NONE, 252, 252, 4, 0, 0, 0));
        SwitchPlayer(SPECIES_MIRAIDON, 60, 189, MIRAIDON_SET);
        SwitchPlayer(SPECIES_METAGROSS, 60, 0, SET(MOVE_METEOR_MASH, MOVE_ZEN_HEADBUTT, MOVE_BULLET_PUNCH, MOVE_PROTECT, NATURE_JOLLY, ABILITY_CLEAR_BODY, ITEM_LIFE_ORB, 4, 252, 0, 0, 0, 252));
        SwitchPlayer(SPECIES_IRON_HANDS, 60, 0, SET(MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH, MOVE_WILD_CHARGE, MOVE_PROTECT, NATURE_ADAMANT, ABILITY_QUARK_DRIVE, ITEM_SITRUS_BERRY, 252, 252, 4, 0, 0, 0));
        sSwitchInjuries[0] = (struct SwitchInjury){SPECIES_SWELLOW, 0};
        sSwitchInjuries[1] = (struct SwitchInjury){SPECIES_MANECTRIC, 0};
        sSwitchInjuries[2] = (struct SwitchInjury){SPECIES_BRELOOM, 163, STATUS1_TOXIC_POISON};
        SwitchLeads(2, 3);
        SwitchAuthoredOpponent(TRAINER_BRENDAN_LILYCOVE_MUDKIP, 6);
        gTestAiTurnSetupHook = LilycoveBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_BRAVE_BIRD, target: opponentLeft);
            MOVE(playerRight, MOVE_HYPER_VOICE, gimmick: GIMMICK_MEGA);
        }
    } THEN {
        EXPECT(!PairSwitchedTo(B_BATTLER_1, 4) && !PairSwitchedTo(B_BATTLER_3, 4));
    }
}

// vj-b/lj-1 turn 1 as it stood: Gholdengo at -1 Attack from Tauros'
// Intimidate and -2 Special Attack from its Make It Rain.
static void LeaAndJedBoard(void)
{
    gBattleMons[B_BATTLER_2].statStages[STAT_ATK] = DEFAULT_STAT_STAGE - 1;
    gBattleMons[B_BATTLER_2].statStages[STAT_SPATK] = DEFAULT_STAT_STAGE - 2;
}

// vj-b/lj-1 turn 1: Kyogre fell and Iron Hands arrived beside Gholdengo. A
// 118 HP Miltank, faster than both, left for Ursaluna - Normal/Ground, taking
// Iron Hands' visible Drain Punch at double - and lost two thirds of it. The
// forecast had both foes aiming at Primeape, so the newcomer never met the
// hit it was walking into. Farigiraf takes it neutrally and Shadow Ball not
// at all.
AI_DOUBLE_BATTLE_TEST("EC switching: Lea & Jed's Miltank does not bring Ursaluna into Iron Hands' Drain Punch")
{
    GIVEN {
        memset(gAiSwitchTrace, 0, sizeof(gAiSwitchTrace));
        SwitchPlayer(SPECIES_IRON_HANDS, 100, 0, SET(MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH, MOVE_WILD_CHARGE, MOVE_HEAVY_SLAM, NATURE_ADAMANT, ABILITY_QUARK_DRIVE, ITEM_ASSAULT_VEST, 252, 252, 4, 0, 0, 0));
        SwitchPlayer(SPECIES_GHOLDENGO, 100, 341, SET(MOVE_MAKE_IT_RAIN, MOVE_SHADOW_BALL, MOVE_NASTY_PLOT, MOVE_PROTECT, NATURE_MODEST, ABILITY_GOOD_AS_GOLD, ITEM_LIFE_ORB, 252, 0, 4, 252, 0, 0));
        SwitchPlayer(SPECIES_CHANDELURE, 100, 0, SET(MOVE_HEAT_WAVE, MOVE_SHADOW_BALL, MOVE_ENERGY_BALL, MOVE_PROTECT, NATURE_TIMID, ABILITY_FLASH_FIRE, ITEM_CHOICE_SPECS, 4, 0, 0, 252, 0, 252));
        SwitchPlayer(SPECIES_INCINEROAR, 100, 0, SET(MOVE_FAKE_OUT, MOVE_THROAT_CHOP, MOVE_FLARE_BLITZ, MOVE_PARTING_SHOT, NATURE_ADAMANT, ABILITY_INTIMIDATE, ITEM_SITRUS_BERRY, 252, 252, 4, 0, 0, 0));
        SwitchPlayer(SPECIES_AMOONGUSS, 100, 0, SET(MOVE_SPORE, MOVE_RAGE_POWDER, MOVE_POLLEN_PUFF, MOVE_PROTECT, NATURE_RELAXED, ABILITY_REGENERATOR, ITEM_ROCKY_HELMET, 252, 0, 252, 0, 4, 0));
        // Miltank's Sitrus Berry went on the first turn; Tauros fell.
        sSwitchInjuries[0] = (struct SwitchInjury){.species = SPECIES_MILTANK, .hp = 118, .noItem = TRUE};
        sSwitchInjuries[1] = (struct SwitchInjury){SPECIES_TAUROS, 0};
        sSwitchMilestone = FLAG_IS_CHAMPION;
        SwitchLeads(0, 2);
        SwitchAuthoredOpponent(TRAINER_LEA_AND_JED, 8);
        gTestAiTurnSetupHook = LeaAndJedBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DRAIN_PUNCH, target: opponentLeft);
            MOVE(playerRight, MOVE_SHADOW_BALL, target: opponentRight);
        }
    } THEN {
        EXPECT(!PairSwitchedTo(B_BATTLER_1, 5));
    }
}

// vj-a/wal-2 turn 5 as it stood: the Gardevoir had Traced Yveltal's Dark Aura
// on entry, and Yveltal has since fallen.
static void WallyVictoryRoadBoard(void)
{
    gBattleStruct->tracedAbility[B_BATTLER_3] = ABILITY_DARK_AURA;
    gBattleMons[B_BATTLER_3].ability = gBattleMons[B_BATTLER_3].volatiles.overwrittenAbility = ABILITY_DARK_AURA;
    gBattleMons[B_BATTLER_3].volatiles.traceActivated = TRUE;
}

// vj-a/wal-2 turn 5: a 27 HP Gardevoir, below a faster Flutter Mane's Shadow
// Ball, left for Roselia beside Magnezone - into the Heat Wave of a Heatran
// that had just arrived with its stone. Roselia fell to it before moving.
AI_DOUBLE_BATTLE_TEST("EC switching: Wally keeps Roselia out of a visible Heatran's Heat Wave")
{
    GIVEN {
        memset(gAiSwitchTrace, 0, sizeof(gAiSwitchTrace));
        SwitchPlayer(SPECIES_FLUTTER_MANE, 80, 86, SET(MOVE_MOONBLAST, MOVE_SHADOW_BALL, MOVE_DAZZLING_GLEAM, MOVE_PROTECT, NATURE_TIMID, ABILITY_PROTOSYNTHESIS, ITEM_BOOSTER_ENERGY, 4, 0, 0, 252, 0, 252));
        SwitchPlayer(SPECIES_HEATRAN, 80, 0, SET(MOVE_HEAT_WAVE, MOVE_EARTH_POWER, MOVE_FLASH_CANNON, MOVE_PROTECT, NATURE_MODEST, ABILITY_FLASH_FIRE, ITEM_HEATRANITE, 252, 0, 4, 252, 0, 0));
        SwitchPlayer(SPECIES_INCINEROAR, 80, 0, SET(MOVE_FAKE_OUT, MOVE_FLARE_BLITZ, MOVE_KNOCK_OFF, MOVE_PARTING_SHOT, NATURE_CAREFUL, ABILITY_INTIMIDATE, ITEM_SITRUS_BERRY, 252, 0, 4, 0, 252, 0));
        SwitchPlayer(SPECIES_GARCHOMP, 80, 0, SET(MOVE_EARTHQUAKE, MOVE_DRAGON_CLAW, MOVE_ROCK_SLIDE, MOVE_PROTECT, NATURE_JOLLY, ABILITY_ROUGH_SKIN, ITEM_LIFE_ORB, 4, 252, 0, 0, 0, 252));
        sSwitchInjuries[0] = (struct SwitchInjury){SPECIES_LUDICOLO, 0};
        sSwitchInjuries[1] = (struct SwitchInjury){SPECIES_GALLADE, 0};
        sSwitchInjuries[2] = (struct SwitchInjury){SPECIES_ALTARIA, 0};
        sSwitchInjuries[3] = (struct SwitchInjury){SPECIES_GARDEVOIR, 27};
        SwitchLeads(3, 4);
        SwitchAuthoredOpponent(TRAINER_WALLY_VR_1, 8);
        gTestAiTurnSetupHook = WallyVictoryRoadBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SHADOW_BALL, target: opponentRight);
            MOVE(playerRight, MOVE_HEAT_WAVE, gimmick: GIMMICK_MEGA);
        }
    } THEN {
        EXPECT(!PairSwitchedTo(B_BATTLER_3, 5));
    }
}

// vj-b/wal-1 turn 5 as it stood: Tapu Fini shielded last turn, its Misty
// Terrain and Kyogre's rain have run out, and Iron Hands carries Ferrothorn's
// Leech Seed and has spent its Fake Out.
static void WallaceBoard(void)
{
    gBattleMons[B_BATTLER_1].volatiles.consecutiveMoveUses = 1;
    gLastResultingMoves[B_BATTLER_1] = MOVE_PROTECT;
    gLastMoves[B_BATTLER_1] = MOVE_PROTECT;
    gFieldTimers.terrain = B_TERRAIN_NONE;
    gFieldTimers.terrainTimer = 0;
    gBattleMons[B_BATTLER_2].volatiles.leechSeed = LEECHSEEDED_BY(B_BATTLER_3);
    gBattleStruct->battlerState[B_BATTLER_0].isFirstTurn = 0;
    gBattleStruct->battlerState[B_BATTLER_2].isFirstTurn = 0;
    gBattleStruct->battlerState[B_BATTLER_1].isFirstTurn = 0;
    gBattleStruct->battlerState[B_BATTLER_3].isFirstTurn = 0;
}

// vj-b/wal-1 turn 5: Tapu Fini shielded a second time - one in three - into a
// faster Kartana whose Leaf Blade removes it, and the shield failed. Leaving
// was the better board before the switch paid the tempo cost of an attack a
// shielding body was never going to make.
AI_DOUBLE_BATTLE_TEST("EC guard: Wallace's Tapu Fini does not shield twice into Kartana's Leaf Blade")
{
    GIVEN {
        memset(gAiSwitchTrace, 0, sizeof(gAiSwitchTrace));
        SwitchPlayer(SPECIES_KARTANA, 80, 0, SET(MOVE_LEAF_BLADE, MOVE_SACRED_SWORD, MOVE_SMART_STRIKE, MOVE_PROTECT, NATURE_JOLLY, ABILITY_BEAST_BOOST, ITEM_LIFE_ORB, 4, 252, 0, 0, 0, 252));
        SwitchPlayer(SPECIES_IRON_HANDS, 80, 83, SET(MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH, MOVE_WILD_CHARGE, MOVE_HEAVY_SLAM, NATURE_ADAMANT, ABILITY_QUARK_DRIVE, ITEM_ASSAULT_VEST, 252, 252, 4, 0, 0, 0));
        SwitchPlayer(SPECIES_MIRAIDON, 80, 173, SET(MOVE_ELECTRO_DRIFT, MOVE_DRACO_METEOR, MOVE_DAZZLING_GLEAM, MOVE_PROTECT, NATURE_TIMID, ABILITY_HADRON_ENGINE, ITEM_CHOICE_SPECS, 4, 0, 0, 252, 0, 252));
        SwitchPlayer(SPECIES_ARCHALUDON, 80, 0, SET(MOVE_ELECTRO_SHOT, MOVE_DRACO_METEOR, MOVE_FLASH_CANNON, MOVE_PROTECT, NATURE_MODEST, ABILITY_STAMINA, ITEM_ASSAULT_VEST, 252, 0, 4, 252, 0, 0));
        SwitchPlayer(SPECIES_AMOONGUSS, 80, 0, SET(MOVE_SPORE, MOVE_RAGE_POWDER, MOVE_POLLEN_PUFF, MOVE_PROTECT, NATURE_RELAXED, ABILITY_REGENERATOR, ITEM_ROCKY_HELMET, 252, 0, 252, 0, 4, 0));
        SwitchPlayer(SPECIES_INCINEROAR, 80, 0, SET(MOVE_FAKE_OUT, MOVE_THROAT_CHOP, MOVE_FLARE_BLITZ, MOVE_PARTING_SHOT, NATURE_ADAMANT, ABILITY_INTIMIDATE, ITEM_SITRUS_BERRY, 252, 252, 4, 0, 0, 0));
        sSwitchInjuries[0] = (struct SwitchInjury){SPECIES_KYOGRE, 0};
        sSwitchInjuries[1] = (struct SwitchInjury){SPECIES_PALKIA, 0};
        // The benchmark had Ferrothorn on the left; the authored order puts
        // Tapu Fini there, and the player's targets follow it.
        SwitchLeads(1, 2);
        SwitchAuthoredOpponent(TRAINER_WALLACE, 8);
        gTestAiTurnSetupHook = WallaceBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_LEAF_BLADE, target: opponentLeft);
            MOVE(playerRight, MOVE_DRAIN_PUNCH, target: opponentRight);
            SEND_OUT(playerRight, 5);
        }
    } SCENE {
        NOT MESSAGE("The opposing Tapu Fini used Protect!");
    }
}

// vj-a/rox-1 turn 3 as it stood: Carbink and Shaymin are past their first
// turn; Lucario and Tyrantrum have just arrived.
static void RoxanneBoard(void)
{
    gBattleStruct->battlerState[B_BATTLER_3].isFirstTurn = 0;
    gBattleStruct->battlerState[B_BATTLER_2].isFirstTurn = 0;
}

#define ROXANNE_FOE(m1, m2, m3, m4, _nature, _ability, _item) \
    SET(m1, m2, m3, m4, _nature, _ability, _item, 0, 0, 0, 0, 0, 0)

// vj-a/rox-1 turn 3: Tyrantrum Earthquaked a 29/51 Carbink - its own partner,
// weak to Ground - beside Lucario and Shaymin, with Fire Fang super effective
// on either foe in hand. Roxanne's pressure plan counts HP at a quarter, and
// Shaymin's Seed Flare was forecast to finish the Carbink anyway, so the
// friendly fire came out nearly free.
AI_DOUBLE_BATTLE_TEST("EC friendly fire: Roxanne's Tyrantrum does not Earthquake its own Carbink")
{
    GIVEN {
        SwitchPlayer(SPECIES_LUCARIO, 14, 0, ROXANNE_FOE(MOVE_CLOSE_COMBAT, MOVE_METEOR_MASH, MOVE_EXTREME_SPEED, MOVE_PROTECT, NATURE_JOLLY, ABILITY_INNER_FOCUS, ITEM_FOCUS_SASH));
        SwitchPlayer(SPECIES_SHAYMIN, 14, 0, ROXANNE_FOE(MOVE_SEED_FLARE, MOVE_EARTH_POWER, MOVE_AIR_SLASH, MOVE_PROTECT, NATURE_TIMID, ABILITY_NATURAL_CURE, ITEM_FOCUS_SASH));
        SwitchPlayer(SPECIES_MARILL, 14, 0, ROXANNE_FOE(MOVE_AQUA_JET, MOVE_PLAY_ROUGH, MOVE_LIQUIDATION, MOVE_PROTECT, NATURE_ADAMANT, ABILITY_HUGE_POWER, ITEM_EVIOLITE));
        SwitchPlayer(SPECIES_TURTWIG, 14, 0, ROXANNE_FOE(MOVE_SEED_BOMB, MOVE_SUPERPOWER, MOVE_WIDE_GUARD, MOVE_PROTECT, NATURE_ADAMANT, ABILITY_SHELL_ARMOR, ITEM_EVIOLITE));
        sSwitchInjuries[0] = (struct SwitchInjury){SPECIES_AERODACTYL, 0};
        sSwitchInjuries[1] = (struct SwitchInjury){SPECIES_RELICANTH, 0};
        sSwitchInjuries[2] = (struct SwitchInjury){SPECIES_CARBINK, 29};
        // Tyrantrum on the left, as on the benchmark.
        sSwitchSwap[0] = 1;
        sSwitchSwap[1] = 3;
        SwitchLeads(1, 3);
        SwitchAuthoredOpponent(TRAINER_ROXANNE_1, 0);
        gTestAiTurnSetupHook = RoxanneBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CLOSE_COMBAT, target: opponentLeft);
            MOVE(playerRight, MOVE_SEED_FLARE, target: opponentRight);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_EARTHQUAKE);
        }
    }
}

// vj-a/lily-1 turns 3-4: Gardevoir had Traced Breloom's Poison Heal, then Mega
// Evolved into Pixilate. The Traced record outlived the Mega, so the AI still
// read Poison Heal on Mega Gardevoir - and on the Mega it pictured for a
// Gardevoir still holding its stone - and its Hyper Voice as a Normal move.
AI_DOUBLE_BATTLE_TEST("EC Mega: the AI reads a Traced Gardevoir's Mega as Pixilate")
{
    bool32 evolve;
    PARAMETRIZE { evolve = FALSE; }
    PARAMETRIZE { evolve = TRUE; }
    GIVEN {
        SwitchPlayer(SPECIES_TALONFLAME, 60, 124, TALONFLAME_SET);
        SwitchPlayer(SPECIES_GARDEVOIR, 60, 154, GARDEVOIR_SET);
        sSwitchInjuries[0] = (struct SwitchInjury){SPECIES_SWELLOW, 0};
        sSwitchInjuries[1] = (struct SwitchInjury){SPECIES_MANECTRIC, 0};
        sSwitchInjuries[2] = (struct SwitchInjury){SPECIES_BRELOOM, 163, STATUS1_TOXIC_POISON};
        SwitchLeads(2, 3);
        SwitchAuthoredOpponent(TRAINER_BRENDAN_LILYCOVE_MUDKIP, 6);
        gTestAiTurnSetupHook = LilycoveBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            if (evolve)
                MOVE(playerRight, MOVE_PROTECT, gimmick: GIMMICK_MEGA);
            else
                MOVE(playerRight, MOVE_PROTECT);
        }
    } THEN {
        if (evolve)
        {
            EXPECT_EQ(gBattleMons[B_BATTLER_2].species, SPECIES_GARDEVOIR_MEGA);
            EXPECT_EQ(AI_DecideKnownAbilityForTurn(B_BATTLER_2), ABILITY_PIXILATE);
        }
        else
        {
            // The Mega the board prices for a Gardevoir still holding its stone.
            struct BattlePokemon saved = gBattleMons[B_BATTLER_2];
            enum Ability savedAbility = gAiLogicData->abilities[B_BATTLER_2];
            EXPECT(AI_ApplyMegaForm(B_BATTLER_2));
            enum Ability megaAbility = gAiLogicData->abilities[B_BATTLER_2];
            SetActiveGimmick(B_BATTLER_2, GIMMICK_NONE);
            gBattleMons[B_BATTLER_2] = saved;
            gAiLogicData->abilities[B_BATTLER_2] = savedAbility;
            EXPECT_EQ(megaAbility, ABILITY_PIXILATE);
        }
    }
}

// vj-b/dra-1 turn 0: Drake's Salamence and Reshiram against Incineroar and a
// Gardevoir holding its stone. Reshiram's Earth Power was chosen beside the
// Double-Edge that Incineroar's Fake Out then took from Salamence; Mega
// Gardevoir's Hyper Voice removed Salamence the same turn.
AI_DOUBLE_BATTLE_TEST("EC forecast: Drake's lead reckons with Incineroar's Fake Out")
{
    GIVEN {
        SwitchPlayer(SPECIES_INCINEROAR, 80, 0, SET(MOVE_FAKE_OUT, MOVE_FLARE_BLITZ, MOVE_KNOCK_OFF, MOVE_PARTING_SHOT, NATURE_CAREFUL, ABILITY_INTIMIDATE, ITEM_SITRUS_BERRY, 252, 0, 4, 0, 252, 0));
        SwitchPlayer(SPECIES_GARDEVOIR, 80, 0, SET(MOVE_HYPER_VOICE, MOVE_PSYCHIC, MOVE_MOONBLAST, MOVE_PROTECT, NATURE_TIMID, ABILITY_TRACE, ITEM_GARDEVOIRITE, 4, 0, 0, 252, 0, 252));
        SwitchPlayer(SPECIES_ZACIAN, 80, 0, SET(MOVE_PLAY_ROUGH, MOVE_IRON_HEAD, MOVE_CLOSE_COMBAT, MOVE_PROTECT, NATURE_JOLLY, ABILITY_INTREPID_SWORD, ITEM_LIFE_ORB, 4, 252, 0, 0, 0, 252));
        SwitchPlayer(SPECIES_FLUTTER_MANE, 80, 0, SET(MOVE_MOONBLAST, MOVE_SHADOW_BALL, MOVE_DAZZLING_GLEAM, MOVE_PROTECT, NATURE_TIMID, ABILITY_PROTOSYNTHESIS, ITEM_BOOSTER_ENERGY, 4, 0, 0, 252, 0, 252));
        sSwitchLeadSpecies[0] = SPECIES_SALAMENCE;
        sSwitchLeadSpecies[1] = SPECIES_RESHIRAM;
        SwitchAuthoredOpponent(TRAINER_DRAKE, 8);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_FAKE_OUT, target: opponentLeft);
            MOVE(playerRight, MOVE_HYPER_VOICE, gimmick: GIMMICK_MEGA);
            NOT_EXPECT_MOVE(opponentRight, MOVE_EARTH_POWER);
        }
    }
}
