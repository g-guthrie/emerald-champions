#include "global.h"
#include "battle.h"
#include "battle_gimmick.h"
#include "battle_util.h"
#include "battle_setup.h"
#include "battle_ai_util.h"
#include "battle_ai_record.h"
#include "emerald_champions_battle_plan.h"
#include "test/battle.h"
#include "data.h"
#include "difficulty.h"
#include "event_data.h"
#include "malloc.h"
#include "constants/opponents.h"
#include "emerald_champions_battle_sets.h"

// Setup, support, Protect and forecast pricing on real benchmark boards,
// replayed on the campaign's own teams: the compiled loadouts and production
// stat generation. A board's opponents stand at the levels they had on the
// build that played it, so its HP and stats are the ones seen.

#if TESTING
extern void (*gTestAiTurnSetupHook)(void);
#endif

// A member's state on the board being replayed. level: the member's level on
// that build (zero keeps the current authored one). noItem: spent before it.
// hp: FULL_HP keeps the member untouched.
struct SupportMember { enum Species species; u16 hp; u32 status; u8 level; bool8 noItem; };
#define FULL_HP 0xFFFF
EWRAM_DATA static struct SupportMember sSupportMembers[6] = {0};
// The species a board led with, brought to the front in this order.
EWRAM_DATA static enum Species sSupportLeads[2] = {0};
// A milestone past the badges that sets the fight's level cap.
EWRAM_DATA static u16 sSupportMilestone = 0;

static void SupportAuthoredParty(u16 trainerId, u32 badges, struct Pokemon *party)
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
    bool8 savedMilestone = sSupportMilestone ? FlagGet(sSupportMilestone) : FALSE;
    if (sSupportMilestone)
        FlagSet(sSupportMilestone);
    SetCurrentDifficultyLevel(DIFFICULTY_NORMAL);
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;
    CreateNPCTrainerPartyFromTrainer(party, trainer);
    gBattleTypeFlags = savedFlags;
    if (sSupportMilestone && !savedMilestone)
        FlagClear(sSupportMilestone);
    SetCurrentDifficultyLevel(savedDifficulty);
    for (u32 i = 0; i < ARRAY_COUNT(savedBadges); i++)
    {
        if (savedBadges[i])
            FlagSet(FLAG_BADGE01_GET + i);
        else
            FlagClear(FLAG_BADGE01_GET + i);
    }
}

static void SupportDeclareMon(struct Pokemon *mon)
{
    *gBattleTestRunnerState->data.currentMon = *mon;
    for (u32 j = 0; j < ARRAY_COUNT(sSupportMembers); j++)
        if (sSupportMembers[j].species != SPECIES_NONE && sSupportMembers[j].level
         && GetMonData(mon, MON_DATA_SPECIES) == sSupportMembers[j].species)
            Level(sSupportMembers[j].level);
    Nature(GetNature(mon));
    Ability(GetMonAbility(mon));
    Speed(GetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_SPEED));
    bool32 fixedSpeed = FALSE;
    SetMonData(gBattleTestRunnerState->data.currentMon, MON_DATA_HYPER_TRAINED_SPEED, &fixedSpeed);
    Moves(GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
          GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4));
    for (u32 j = 0; j < ARRAY_COUNT(sSupportMembers); j++)
        if (sSupportMembers[j].species != SPECIES_NONE
         && GetMonData(mon, MON_DATA_SPECIES) == sSupportMembers[j].species)
        {
            if (sSupportMembers[j].hp != FULL_HP)
                HP(sSupportMembers[j].hp);
            if (sSupportMembers[j].status)
                Status1(sSupportMembers[j].status);
            if (sSupportMembers[j].noItem)
                Item(ITEM_NONE);
        }
}

// A doubles opponent from its authored team, fainted members declared at zero
// HP and the board's leads in front.
static void SupportOpponent(u16 trainerId, u32 badges)
{
    const struct Trainer *trainer = &gTrainers[DIFFICULTY_NORMAL][trainerId];
    struct Pokemon *party = AllocZeroed(sizeof(struct Pokemon) * PARTY_SIZE);
    SupportAuthoredParty(trainerId, badges, party);
    for (u32 lead = 0; lead < ARRAY_COUNT(sSupportLeads); lead++)
    {
        for (u32 i = 0; sSupportLeads[lead] != SPECIES_NONE && i < PARTY_SIZE; i++)
            if (GetMonData(&party[i], MON_DATA_SPECIES) == sSupportLeads[lead])
            {
                struct Pokemon swap = party[lead];
                party[lead] = party[i];
                party[i] = swap;
                break;
            }
        sSupportLeads[lead] = SPECIES_NONE;
    }
    if (IsAITest())
        AI_FLAGS(trainer->aiFlags | AI_FLAG_DOUBLE_BATTLE);
    gBattleTestRunnerState->data.recordedBattle.opponentA = trainerId;
    for (u32 i = 0; i < trainer->partySize; i++)
    {
        OPPONENT(GetMonData(&party[i], MON_DATA_SPECIES))
        {
            SupportDeclareMon(&party[i]);
        }
    }
    memset(sSupportMembers, 0, sizeof(sSupportMembers));
    sSupportMilestone = 0;
    Free(party);
}

// A benchmark player member: the manifest's set at the fight's cap, perfect
// IVs, at the HP it had on the board (zero keeps it full).
EWRAM_DATA static u32 sSupportPlayerStatus = 0; // Consumed by the next SupportPlayer.

static void SupportPlayer(enum Species species, u32 level, u16 hp, const struct EmeraldChampionsBattleSet *set)
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
        if (sSupportPlayerStatus)
            Status1(sSupportPlayerStatus);
    }
    sSupportPlayerStatus = 0;
}

#define SET(m1, m2, m3, m4, _nature, _ability, _item, hp, atk, def, spa, spd, spe) \
    (&(const struct EmeraldChampionsBattleSet){ .moves = {m1, m2, m3, m4}, .nature = _nature, \
        .ability = _ability, .item = _item, .evs = {hp, atk, def, spa, spd, spe} })

// Every body on the board is past its first turn: nobody can Fake Out.
static void SupportPastFirstTurn(void)
{
    for (enum BattlerId battler = 0; battler < MAX_BATTLERS_COUNT; battler++)
        gBattleStruct->battlerState[battler].isFirstTurn = 0;
}

#define MIRAIDON_ORB SET(MOVE_ELECTRO_DRIFT, MOVE_DRACO_METEOR, MOVE_DAZZLING_GLEAM, MOVE_PROTECT, NATURE_TIMID, ABILITY_HADRON_ENGINE, ITEM_LIFE_ORB, 4, 0, 0, 252, 0, 252)
#define IRON_HANDS_AV SET(MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH, MOVE_WILD_CHARGE, MOVE_HEAVY_SLAM, NATURE_ADAMANT, ABILITY_QUARK_DRIVE, ITEM_ASSAULT_VEST, 252, 252, 4, 0, 0, 0)
#define RILLABOOM_SET SET(MOVE_FAKE_OUT, MOVE_GRASSY_GLIDE, MOVE_WOOD_HAMMER, MOVE_KNOCK_OFF, NATURE_ADAMANT, ABILITY_GRASSY_SURGE, ITEM_SITRUS_BERRY, 252, 252, 4, 0, 0, 0)
#define KARTANA_SET SET(MOVE_LEAF_BLADE, MOVE_SACRED_SWORD, MOVE_SMART_STRIKE, MOVE_PROTECT, NATURE_JOLLY, ABILITY_BEAST_BOOST, ITEM_LIFE_ORB, 4, 252, 0, 0, 0, 252)
#define AMOONGUSS_HELMET SET(MOVE_SPORE, MOVE_RAGE_POWDER, MOVE_POLLEN_PUFF, MOVE_PROTECT, NATURE_RELAXED, ABILITY_REGENERATOR, ITEM_ROCKY_HELMET, 252, 0, 252, 0, 4, 0)
#define TYRANITAR_SET SET(MOVE_ROCK_SLIDE, MOVE_CRUNCH, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT, NATURE_ADAMANT, ABILITY_SAND_STREAM, ITEM_TYRANITARITE, 252, 252, 4, 0, 0, 0)
#define INCINEROAR_ADAMANT SET(MOVE_FAKE_OUT, MOVE_FLARE_BLITZ, MOVE_KNOCK_OFF, MOVE_PARTING_SHOT, NATURE_ADAMANT, ABILITY_INTIMIDATE, ITEM_SITRUS_BERRY, 252, 252, 4, 0, 0, 0)

// k3/juan-1 turn 2 as it stood: Politoed's rain under Altaria's Cloud Nine,
// Miraidon's Electric Terrain with three turns left, and Iron Hands past its
// Fake Out.
static void JuanBoard(void)
{
    gBattleStruct->battlerState[B_BATTLER_0].isFirstTurn = 0;
    gBattleStruct->battlerState[B_BATTLER_2].isFirstTurn = 0;
    gLastMoves[B_BATTLER_0] = gLastResultingMoves[B_BATTLER_0] = MOVE_ELECTRO_DRIFT;
    gLastMoves[B_BATTLER_2] = gLastResultingMoves[B_BATTLER_2] = MOVE_WILD_CHARGE;
    gBattleWeather = B_WEATHER_RAIN_NORMAL;
    gBattleStruct->weatherDuration = 6;
    gFieldTimers.terrain = B_TERRAIN_ELECTRIC;
    gFieldTimers.terrainTimer = 3;
}

// k3/juan-1 turn 2: Politoed and Suicune fell to Miraidon and Iron Hands, and
// Manaphy came in beside Altaria at full HP. It used Tail Glow into Iron
// Hands' visible Wild Charge, which knocked it out with the boost unspent,
// while Altaria shielded: the shield was paid a partner payoff for a boost
// that died with its user.
AI_DOUBLE_BATTLE_TEST("EC support: Juan's Manaphy does not Tail Glow into Iron Hands' Wild Charge")
{
    GIVEN {
        SupportPlayer(SPECIES_MIRAIDON, 70, 122, MIRAIDON_ORB);
        SupportPlayer(SPECIES_IRON_HANDS, 70, 290, IRON_HANDS_AV);
        SupportPlayer(SPECIES_RILLABOOM, 70, 0, RILLABOOM_SET);
        SupportPlayer(SPECIES_KARTANA, 70, 0, KARTANA_SET);
        SupportPlayer(SPECIES_AMOONGUSS, 70, 0, AMOONGUSS_HELMET);
        SupportPlayer(SPECIES_TYRANITAR, 70, 0, TYRANITAR_SET);
        sSupportMembers[0] = (struct SupportMember){SPECIES_POLITOED, 0};
        sSupportMembers[1] = (struct SupportMember){SPECIES_LANDORUS_THERIAN, 0};
        sSupportMembers[2] = (struct SupportMember){.species = SPECIES_ALTARIA, .hp = 261, .level = 73};
        sSupportMembers[3] = (struct SupportMember){.species = SPECIES_MANAPHY, .hp = 301, .level = 74};
        sSupportLeads[0] = SPECIES_ALTARIA;
        sSupportLeads[1] = SPECIES_MANAPHY;
        SupportOpponent(TRAINER_JUAN_1, 7);
        gTestAiTurnSetupHook = JuanBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_DRACO_METEOR, target: opponentLeft);
            MOVE(playerRight, MOVE_WILD_CHARGE, target: opponentRight);
            NOT_EXPECT_MOVE(opponentRight, MOVE_TAIL_GLOW);
        }
    }
}

// k2/brax-2 turn 0: Braxton leads Braviary and Garchomp into Whimsicott and a
// faster Miraidon whose Draco Meteor knocks Garchomp out from full. Garchomp
// used Swords Dance and fell before it could use the boost.
AI_DOUBLE_BATTLE_TEST("EC support: Braxton's Garchomp does not Swords Dance into Miraidon's Draco Meteor")
{
    GIVEN {
        SupportPlayer(SPECIES_WHIMSICOTT, 60, 0, SET(MOVE_TAILWIND, MOVE_MOONBLAST, MOVE_ENCORE, MOVE_PROTECT, NATURE_TIMID, ABILITY_PRANKSTER, ITEM_FOCUS_SASH, 4, 0, 0, 252, 0, 252));
        SupportPlayer(SPECIES_MIRAIDON, 60, 0, MIRAIDON_ORB);
        SupportPlayer(SPECIES_INCINEROAR, 60, 0, INCINEROAR_ADAMANT);
        SupportPlayer(SPECIES_FLUTTER_MANE, 60, 0, SET(MOVE_MOONBLAST, MOVE_SHADOW_BALL, MOVE_DAZZLING_GLEAM, MOVE_PROTECT, NATURE_TIMID, ABILITY_PROTOSYNTHESIS, ITEM_LIFE_ORB, 4, 0, 0, 252, 0, 252));
        SupportPlayer(SPECIES_KARTANA, 60, 0, KARTANA_SET);
        SupportPlayer(SPECIES_METAGROSS, 60, 0, SET(MOVE_METEOR_MASH, MOVE_ZEN_HEADBUTT, MOVE_BULLET_PUNCH, MOVE_PROTECT, NATURE_JOLLY, ABILITY_CLEAR_BODY, ITEM_LIFE_ORB, 4, 252, 0, 0, 0, 252));
        SupportOpponent(TRAINER_BRAXTON, 6);
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TAILWIND);
            MOVE(playerRight, MOVE_DRACO_METEOR, target: opponentRight);
            NOT_EXPECT_MOVE(opponentRight, MOVE_SWORDS_DANCE);
        }
    }
}

// k2/max-1 turn 1 as it stood: Runerigus' Trick Room from turn zero with four
// turns left, Maxie's locked sun, Runerigus' Mental Herb spent on Taunt, and
// Pelipper's Wide Guard from turn zero on record.
static void MaxieBoard(void)
{
    SupportPastFirstTurn();
    gFieldStatuses |= STATUS_FIELD_TRICK_ROOM;
    gFieldTimers.trickRoomTimer = 4;
    gBattleWeather = B_WEATHER_SUN_NORMAL;
    gLastMoves[B_BATTLER_0] = gLastResultingMoves[B_BATTLER_0] = MOVE_WIDE_GUARD;
    RecordLastUsedMoveBy(B_BATTLER_0, MOVE_WIDE_GUARD);
    gLastMoves[B_BATTLER_2] = gLastResultingMoves[B_BATTLER_2] = MOVE_TAUNT;
    RecordLastUsedMoveBy(B_BATTLER_2, MOVE_TAUNT);
    gLastMoves[B_BATTLER_1] = gLastResultingMoves[B_BATTLER_1] = MOVE_HEAT_WAVE;
    gLastMoves[B_BATTLER_3] = gLastResultingMoves[B_BATTLER_3] = MOVE_TRICK_ROOM;
}

// k2/max-1 turn 1: Pelipper raised Wide Guard on turn zero and Torkoal's Heat
// Wave met it. Torkoal Heat Waved into it again - and on five more turns
// after - while Solar Beam and Earth Power were single-target moves in hand.
AI_DOUBLE_BATTLE_TEST("EC forecast: Maxie's Torkoal does not Heat Wave into a Wide Guard it has already met")
{
    GIVEN {
        SupportPlayer(SPECIES_PELIPPER, 60, 0, SET(MOVE_HURRICANE, MOVE_WEATHER_BALL, MOVE_WIDE_GUARD, MOVE_PROTECT, NATURE_MODEST, ABILITY_DRIZZLE, ITEM_DAMP_ROCK, 252, 0, 4, 252, 0, 0));
        SupportPlayer(SPECIES_WHIMSICOTT, 60, 0, SET(MOVE_TAUNT, MOVE_MOONBLAST, MOVE_TAILWIND, MOVE_ENCORE, NATURE_TIMID, ABILITY_PRANKSTER, ITEM_FOCUS_SASH, 4, 0, 0, 252, 0, 252));
        SupportPlayer(SPECIES_PRIMARINA, 60, 0, SET(MOVE_HYPER_VOICE, MOVE_MOONBLAST, MOVE_ICE_BEAM, MOVE_PROTECT, NATURE_MODEST, ABILITY_LIQUID_VOICE, ITEM_MYSTIC_WATER, 252, 0, 4, 252, 0, 0));
        SupportPlayer(SPECIES_KARTANA, 60, 0, KARTANA_SET);
        SupportPlayer(SPECIES_INCINEROAR, 60, 0, INCINEROAR_ADAMANT);
        SupportPlayer(SPECIES_LANDORUS, 60, 0, SET(MOVE_EARTH_POWER, MOVE_SLUDGE_BOMB, MOVE_PSYCHIC, MOVE_PROTECT, NATURE_TIMID, ABILITY_SHEER_FORCE, ITEM_LIFE_ORB, 4, 0, 0, 252, 0, 252));
        sSupportMembers[0] = (struct SupportMember){.species = SPECIES_TORKOAL, .hp = 220, .level = 63};
        sSupportMembers[1] = (struct SupportMember){.species = SPECIES_RUNERIGUS, .hp = 208, .level = 64, .noItem = TRUE};
        SupportOpponent(TRAINER_MAXIE_MAGMA_HIDEOUT, 6);
        gTestAiTurnSetupHook = MaxieBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_WIDE_GUARD);
            MOVE(playerRight, MOVE_ENCORE, target: opponentLeft);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_HEAT_WAVE);
        }
    }
}

// k1/r103-4 turn 1 as it stood: Pachirisu's Focus Sash broke on Liquidation
// under its Follow Me, and Pikachu has spent its Fake Out.
static void RivalRoute103Board(void)
{
    SupportPastFirstTurn();
    gLastMoves[B_BATTLER_2] = gLastResultingMoves[B_BATTLER_2] = MOVE_FOLLOW_ME;
    RecordLastUsedMoveBy(B_BATTLER_2, MOVE_FOLLOW_ME);
    gLastMoves[B_BATTLER_3] = gLastResultingMoves[B_BATTLER_3] = MOVE_FAKE_OUT;
    gLastMoves[B_BATTLER_1] = gLastResultingMoves[B_BATTLER_1] = MOVE_LIQUIDATION;
}

// k1/r103-4 turns 1-2: a Volt Absorb Pachirisu used Follow Me beside Espeon on
// every turn, and Pikachu Thunderbolted at Espeon into it twice - healing the
// Pachirisu both times - while Electroweb reaches Espeon around the
// redirection.
AI_DOUBLE_BATTLE_TEST("EC forecast: the Route 103 rival's Pikachu does not Thunderbolt into a Volt Absorb Follow Me")
{
    GIVEN {
        SupportPlayer(SPECIES_ESPEON, 14, 34, SET(MOVE_PSYCHIC, MOVE_DAZZLING_GLEAM, MOVE_SHADOW_BALL, MOVE_PROTECT, NATURE_TIMID, ABILITY_MAGIC_BOUNCE, ITEM_CHOICE_SPECS, 0, 0, 0, 0, 0, 0));
        SupportPlayer(SPECIES_PACHIRISU, 14, 2, SET(MOVE_FOLLOW_ME, MOVE_NUZZLE, MOVE_SUPER_FANG, MOVE_PROTECT, NATURE_BOLD, ABILITY_VOLT_ABSORB, ITEM_NONE, 0, 0, 0, 0, 0, 0));
        SupportPlayer(SPECIES_SYLVEON, 14, 0, SET(MOVE_HYPER_VOICE, MOVE_MOONBLAST, MOVE_QUICK_ATTACK, MOVE_PROTECT, NATURE_MODEST, ABILITY_PIXILATE, ITEM_CHOICE_SPECS, 0, 0, 0, 0, 0, 0));
        SupportPlayer(SPECIES_MONFERNO, 14, 0, SET(MOVE_FAKE_OUT, MOVE_CLOSE_COMBAT, MOVE_FLARE_BLITZ, MOVE_PROTECT, NATURE_JOLLY, ABILITY_IRON_FIST, ITEM_FOCUS_SASH, 0, 0, 0, 0, 0, 0));
        SupportPlayer(SPECIES_SHINX, 14, 0, SET(MOVE_WILD_CHARGE, MOVE_ICE_FANG, MOVE_PROTECT, MOVE_HELPING_HAND, NATURE_ADAMANT, ABILITY_INTIMIDATE, ITEM_EVIOLITE, 0, 0, 0, 0, 0, 0));
        SupportPlayer(SPECIES_TURTWIG, 14, 0, SET(MOVE_SEED_BOMB, MOVE_SUPERPOWER, MOVE_WIDE_GUARD, MOVE_PROTECT, NATURE_ADAMANT, ABILITY_SHELL_ARMOR, ITEM_EVIOLITE, 0, 0, 0, 0, 0, 0));
        sSupportMembers[0] = (struct SupportMember){.species = SPECIES_MUDKIP, .hp = 45, .level = 17};
        sSupportMembers[1] = (struct SupportMember){.species = SPECIES_PIKACHU, .hp = 44, .level = 17};
        SupportOpponent(TRAINER_MAY_ROUTE_103_TORCHIC, 0);
        gTestAiTurnSetupHook = RivalRoute103Board;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            NOT_EXPECT_MOVE(opponentRight, MOVE_THUNDERBOLT);
        }
    }
}

// k1/wat-2 turn 7 as it stood: the player's last body, a Focus Sash Shedinja
// at -1 Attack from Mega Manectric's Intimidate; Galvantula and Manectric are
// past their first turn.
static void WattsonBoard(void)
{
    u16 fainted = 0;
    SupportPastFirstTurn();
    gBattleMons[B_BATTLER_0].hp = 0;
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_HP, &fainted);
    gAbsentBattlerFlags |= 1u << B_BATTLER_0;
    gBattleMons[B_BATTLER_2].statStages[STAT_ATK] = DEFAULT_STAT_STAGE - 1;
}

// k1/wat-2 turns 7-14: Mega Manectric against a lone Focus Sash Shedinja whose
// Wonder Guard only its Flamethrower gets past. It shielded on seven turns in
// a row - three of the repeats failed: breaking the Sash scored as a wasted
// turn, and a Sucker Punch that barely dents it counted as a waiting payoff.
AI_DOUBLE_BATTLE_TEST("EC guard: Wattson's Manectric does not shield in a loop against a lone Shedinja")
{
    GIVEN {
        // Snorlax fell last; its slot stands empty (see WattsonBoard).
        SupportPlayer(SPECIES_SNORLAX, 30, 1, SET(MOVE_CELEBRATE, MOVE_NONE, MOVE_NONE, MOVE_NONE, NATURE_ADAMANT, ABILITY_THICK_FAT, ITEM_NONE, 0, 0, 0, 0, 0, 0));
        SupportPlayer(SPECIES_SHEDINJA, 30, 0, SET(MOVE_POLTERGEIST, MOVE_SUCKER_PUNCH, MOVE_AERIAL_ACE, MOVE_PROTECT, NATURE_ADAMANT, ABILITY_WONDER_GUARD, ITEM_FOCUS_SASH, 0, 0, 0, 0, 0, 0));
        sSupportMembers[0] = (struct SupportMember){SPECIES_ELECTRODE, 0};
        sSupportMembers[1] = (struct SupportMember){SPECIES_ELECTIVIRE, 0};
        sSupportMembers[2] = (struct SupportMember){SPECIES_MAGNEZONE, 0};
        sSupportMembers[3] = (struct SupportMember){SPECIES_THUNDURUS, 0};
        sSupportMembers[4] = (struct SupportMember){.species = SPECIES_GALVANTULA, .hp = 102, .level = 34};
        sSupportMembers[5] = (struct SupportMember){.species = SPECIES_MANECTRIC, .hp = 65, .level = 33};
        sSupportLeads[0] = SPECIES_GALVANTULA;
        sSupportLeads[1] = SPECIES_MANECTRIC;
        SupportOpponent(TRAINER_WATTSON_1, 2);
        gTestAiTurnSetupHook = WattsonBoard;
    } WHEN {
        TURN {
            MOVE(playerRight, MOVE_SUCKER_PUNCH, target: opponentRight);
            NOT_EXPECT_MOVE(opponentRight, MOVE_PROTECT);
        }
    }
}

// k4/pho2 turn 5 as it stood: Banette in the Mega form it took on turn
// three, Tyranitar's sand on its last turn, Marshadow at -1 Defense and Sp.
// Def from Close Combat, and Incineroar at -1 Speed with its Sitrus Berry
// eaten.
static void PhoebeBoard(void)
{
    SupportPastFirstTurn();
    gBattleStruct->battlerState[B_BATTLER_0].isFirstTurn = 1;
    SetActiveGimmick(B_BATTLER_3, GIMMICK_MEGA);
    SetGimmickAsActivated(B_BATTLER_3, GIMMICK_MEGA);
    TryBattleFormChange(B_BATTLER_3, FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM, GetBattlerAbility(B_BATTLER_3));
    gBattleMons[B_BATTLER_3].hp = 9;
    gBattleWeather = B_WEATHER_SANDSTORM;
    gBattleStruct->weatherDuration = 1;
    gBattleMons[B_BATTLER_1].statStages[STAT_DEF] = DEFAULT_STAT_STAGE - 1;
    gBattleMons[B_BATTLER_1].statStages[STAT_SPDEF] = DEFAULT_STAT_STAGE - 1;
    gBattleMons[B_BATTLER_2].statStages[STAT_SPEED] = DEFAULT_STAT_STAGE - 1;
    gLastMoves[B_BATTLER_1] = gLastResultingMoves[B_BATTLER_1] = MOVE_CLOSE_COMBAT;
    gLastMoves[B_BATTLER_3] = gLastResultingMoves[B_BATTLER_3] = MOVE_POLTERGEIST;
}

// k4/pho2 turn 5: Mega Banette at 9 of 299 HP in a sandstorm whose end-of-turn
// damage is certain to finish it. It shielded - saving nothing, since the sand
// ended it anyway - instead of spending its last turn.
AI_DOUBLE_BATTLE_TEST("EC guard: Phoebe's Banette does not shield on the turn the sand ends it")
{
    GIVEN {
        SupportPlayer(SPECIES_KINGAMBIT, 80, 0, SET(MOVE_KOWTOW_CLEAVE, MOVE_SUCKER_PUNCH, MOVE_IRON_HEAD, MOVE_PROTECT, NATURE_ADAMANT, ABILITY_SUPREME_OVERLORD, ITEM_LIFE_ORB, 252, 252, 4, 0, 0, 0));
        SupportPlayer(SPECIES_INCINEROAR, 80, 111, SET(MOVE_THROAT_CHOP, MOVE_KNOCK_OFF, MOVE_FLARE_BLITZ, MOVE_PARTING_SHOT, NATURE_ADAMANT, ABILITY_INTIMIDATE, ITEM_NONE, 252, 252, 4, 0, 0, 0));
        sSupportMembers[0] = (struct SupportMember){SPECIES_GIRATINA, 0};
        sSupportMembers[1] = (struct SupportMember){SPECIES_LUNALA, 0};
        sSupportMembers[2] = (struct SupportMember){SPECIES_GENGAR, 0};
        sSupportMembers[3] = (struct SupportMember){.species = SPECIES_MARSHADOW, .hp = 234, .level = 86};
        sSupportMembers[4] = (struct SupportMember){.species = SPECIES_BANETTE, .hp = FULL_HP, .level = 90};
        sSupportMembers[5] = (struct SupportMember){.species = SPECIES_SPECTRIER, .hp = FULL_HP, .level = 87};
        sSupportLeads[0] = SPECIES_MARSHADOW;
        sSupportLeads[1] = SPECIES_BANETTE;
        SupportOpponent(TRAINER_PHOEBE, 8);
        gTestAiTurnSetupHook = PhoebeBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_THROAT_CHOP, target: opponentRight);
            NOT_EXPECT_MOVE(opponentRight, MOVE_PROTECT);
        }
    }
}

// k4/wdl1 turn 5 as it stood: Zamazenta burned by Sableye with Defense at +3,
// Zapdos just arrived, Zapdos' Tailwind on its last turn, and Great Tusk at -1
// Defense and Sp. Def from Headlong Rush.
static void WallaceLegendsBoard(void)
{
    SupportPastFirstTurn();
    gBattleStruct->battlerState[B_BATTLER_3].isFirstTurn = 1;
    gBattleMons[B_BATTLER_1].statStages[STAT_DEF] = DEFAULT_STAT_STAGE + 3;
    gBattleMons[B_BATTLER_0].statStages[STAT_DEF] = DEFAULT_STAT_STAGE - 1;
    gBattleMons[B_BATTLER_0].statStages[STAT_SPDEF] = DEFAULT_STAT_STAGE - 1;
    gSideStatuses[B_SIDE_OPPONENT] |= SIDE_STATUS_TAILWIND;
    gSideTimers[B_SIDE_OPPONENT].tailwindTimer = 1;
    gLastMoves[B_BATTLER_1] = gLastResultingMoves[B_BATTLER_1] = MOVE_IRON_DEFENSE;
}

// k4/wdl1 turns 5-6: a burned Crowned Zamazenta raised Defense from +1 to +3
// and then to +5 in front of Great Tusk and Sableye, instead of Body Pressing
// with the Defense it already had.
AI_DOUBLE_BATTLE_TEST("EC support: Wallace's Zamazenta does not stack Iron Defense past +3")
{
    GIVEN {
        SupportPlayer(SPECIES_GREAT_TUSK, 100, 0, SET(MOVE_HEADLONG_RUSH, MOVE_CLOSE_COMBAT, MOVE_ICE_SPINNER, MOVE_PROTECT, NATURE_JOLLY, ABILITY_PROTOSYNTHESIS, ITEM_BOOSTER_ENERGY, 4, 252, 0, 0, 0, 252));
        SupportPlayer(SPECIES_SABLEYE, 100, 0, SET(MOVE_WILL_O_WISP, MOVE_FAKE_OUT, MOVE_KNOCK_OFF, MOVE_PROTECT, NATURE_CAREFUL, ABILITY_PRANKSTER, ITEM_SITRUS_BERRY, 252, 0, 4, 0, 252, 0));
        SupportPlayer(SPECIES_CHANDELURE, 100, 0, SET(MOVE_HEAT_WAVE, MOVE_SHADOW_BALL, MOVE_ENERGY_BALL, MOVE_PROTECT, NATURE_TIMID, ABILITY_FLASH_FIRE, ITEM_LIFE_ORB, 4, 0, 0, 252, 0, 252));
        SupportPlayer(SPECIES_AMOONGUSS, 100, 0, AMOONGUSS_HELMET);
        sSupportMembers[0] = (struct SupportMember){SPECIES_KYOGRE_PRIMAL, 0};
        sSupportMembers[1] = (struct SupportMember){SPECIES_LUCARIO, 0};
        sSupportMembers[2] = (struct SupportMember){SPECIES_ZYGARDE, 0};
        sSupportMembers[3] = (struct SupportMember){.species = SPECIES_ZAMAZENTA_CROWNED, .hp = 356, .status = STATUS1_BURN};
        sSupportMembers[4] = (struct SupportMember){.species = SPECIES_ZAPDOS, .hp = FULL_HP};
        sSupportMembers[5] = (struct SupportMember){.species = SPECIES_MARSHADOW, .hp = FULL_HP};
        sSupportLeads[0] = SPECIES_ZAMAZENTA_CROWNED;
        sSupportLeads[1] = SPECIES_ZAPDOS;
        sSupportMilestone = FLAG_IS_CHAMPION;
        SupportOpponent(TRAINER_WALLACE_DOUBLES_LEGENDS, 8);
        gTestAiTurnSetupHook = WallaceLegendsBoard;
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_PROTECT);
            MOVE(playerRight, MOVE_PROTECT);
            NOT_EXPECT_MOVE(opponentLeft, MOVE_IRON_DEFENSE);
        }
    }
}
