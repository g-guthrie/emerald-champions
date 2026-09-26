// Champions random-doubles generation ported from Pokemon Showdown commit
// bb179fbf8449e3c31632bd56f671ffb4404fa6e7. See THIRD_PARTY_NOTICES.md.
#include "global.h"
#include "battle.h"
#include "battle_frontier.h"
#include "battle_util.h"
#include "battle_tent.h"
#include "caps.h"
#include "champions_circuit.h"
#include "data.h"
#include "difficulty.h"
#include "event_data.h"
#include "fpmath.h"
#include "legendary_signs.h"
#include "load_save.h"
#include "main.h"
#include "move.h"
#include "pokemon.h"
#include "random.h"
#include "script_pokemon_util.h"
#include "showdown_champions_circuit.h"
#include "string_util.h"
#include "tv.h"
#include "constants/battle_frontier.h"
#include "constants/battle_frontier_trainers.h"
#include "constants/items.h"
#include "constants/pokedex.h"
#include "constants/pokemon.h"
#include "constants/vars.h"

#include "data/pokemon/showdown_champions_circuit.h"

#define CIRCUIT_TEAM_SIZE PARTY_SIZE
#define CIRCUIT_BASE_LEVEL CHAMPIONS_CIRCUIT_BASE_LEVEL
struct CircuitTeamDetails
{
    bool8 rain;
    bool8 sun;
    bool8 sand;
    bool8 snow;
    bool8 statusCure;
    bool8 toxicSpikes;
    bool8 stealthRock;
    bool8 stickyWeb;
    bool8 defog;
    bool8 rapidSpin;
    bool8 screens;
    u8 terrain; // Bitmask of B_TERRAIN_* values; the terrain type matters.
    u8 spikes;
};

struct CircuitGeneratedSet
{
    u16 variantIndex;
    enum Move moves[MAX_MON_MOVES];
    enum Ability ability;
    enum Item item;
    u8 evs[NUM_STATS];
    u8 nature;
    enum CircuitDependency dependency;
};

struct CircuitTeamState
{
    struct CircuitGeneratedSet sets[CIRCUIT_TEAM_SIZE];
    u8 typeCounts[NUMBER_OF_MON_TYPES];
    u8 weaknessCounts[NUMBER_OF_MON_TYPES];
    u8 doubleWeaknessCounts[NUMBER_OF_MON_TYPES];
    struct CircuitTeamDetails details;
    u16 compatibilityFlags;
    u8 freezeDryWeakCount;
    u8 count;
    bool8 hasMega;
};

struct CircuitMovePool
{
    enum Move moves[SHOWDOWN_CIRCUIT_MAX_MOVES];
    u8 count;
};

static EWRAM_DATA bool8 sExhaustedBaseDex[NATIONAL_DEX_COUNT + 1];
static EWRAM_DATA bool8 sTentActive;
static EWRAM_DATA u8 sTentId;
static EWRAM_DATA u8 sTentWins;
static EWRAM_DATA u8 sTentCap;
// The Circuit borrows the Frontier trainer table for names and pictures. A Tent
// challenge leaves lvlMode at FRONTIER_LVL_TENT, which would point that lookup at
// the much smaller Tent tables, so a run forces Open Level and restores it after.
static EWRAM_DATA u8 sCircuitSavedLvlMode;

static const u8 sCircuitRecordUnrecorded[] = _("--");

static const u8 sCircuitStyleShowdown[] = _("a balanced doubles team");
static const u8 sCircuitStyleRain[] = _("a rain offense team");
static const u8 sCircuitStyleSun[] = _("a sun offense team");
static const u8 sCircuitStyleSand[] = _("a sand offense team");
static const u8 sCircuitStyleSnow[] = _("a snow offense team");
static const u8 sCircuitStyleTrickRoom[] = _("a Trick Room team");

static const enum Move sRecoveryMoves[] =
{
    MOVE_HEAL_ORDER, MOVE_MILK_DRINK, MOVE_MOONLIGHT, MOVE_MORNING_SUN,
    MOVE_RECOVER, MOVE_ROOST, MOVE_SHORE_UP, MOVE_SLACK_OFF,
    MOVE_SOFT_BOILED, MOVE_STRENGTH_SAP, MOVE_SYNTHESIS,
};

static const enum Move sPhysicalSetupMoves[] =
{
    MOVE_BELLY_DRUM, MOVE_BULK_UP, MOVE_COIL, MOVE_CURSE, MOVE_DRAGON_DANCE,
    MOVE_HONE_CLAWS, MOVE_HOWL, MOVE_MEDITATE, MOVE_POWER_UP_PUNCH,
    MOVE_SWORDS_DANCE, MOVE_TIDY_UP, MOVE_VICTORY_DANCE,
};

static const enum Move sSpeedSetupMoves[] =
{
    MOVE_AGILITY, MOVE_AUTOTOMIZE, MOVE_FLAME_CHARGE, MOVE_RAIN_DANCE,
    MOVE_ROCK_POLISH, MOVE_SNOWSCAPE, MOVE_SUNNY_DAY, MOVE_TRAILBLAZE,
};

static const enum Move sSetupMoves[] =
{
    MOVE_ACID_ARMOR, MOVE_AGILITY, MOVE_AUTOTOMIZE, MOVE_BELLY_DRUM,
    MOVE_BULK_UP, MOVE_CALM_MIND, MOVE_CLANGOROUS_SOUL, MOVE_COIL,
    MOVE_COSMIC_POWER, MOVE_CURSE, MOVE_DRAGON_DANCE, MOVE_FLAME_CHARGE,
    MOVE_GROWTH, MOVE_HONE_CLAWS, MOVE_HOWL, MOVE_IRON_DEFENSE,
    MOVE_MEDITATE, MOVE_NASTY_PLOT, MOVE_NO_RETREAT, MOVE_POWER_UP_PUNCH,
    MOVE_QUIVER_DANCE, MOVE_RAIN_DANCE, MOVE_ROCK_POLISH, MOVE_SHELL_SMASH,
    MOVE_SHELTER, MOVE_SHIFT_GEAR, MOVE_SUNNY_DAY, MOVE_SWORDS_DANCE,
    MOVE_TAIL_GLOW, MOVE_TAKE_HEART, MOVE_TIDY_UP, MOVE_TRAILBLAZE,
    MOVE_VICTORY_DANCE, MOVE_WORK_UP,
};

static const enum Move sSpeedControlMoves[] =
{
    MOVE_ELECTROWEB, MOVE_GLARE, MOVE_ICY_WIND, MOVE_NUZZLE,
    MOVE_QUASH, MOVE_TAILWIND, MOVE_THUNDER_WAVE, MOVE_TRICK_ROOM,
};

static const enum Move sProtectMoves[] =
{
    MOVE_BANEFUL_BUNKER, MOVE_BURNING_BULWARK, MOVE_DETECT,
    MOVE_KINGS_SHIELD, MOVE_PROTECT, MOVE_SILK_TRAP, MOVE_SPIKY_SHIELD,
};

static const enum Move sHazardMoves[] =
{
    MOVE_SPIKES, MOVE_STEALTH_ROCK, MOVE_STICKY_WEB, MOVE_TOXIC_SPIKES,
};

static const enum Move sPivotMoves[] =
{
    MOVE_CHILLY_RECEPTION, MOVE_FLIP_TURN, MOVE_PARTING_SHOT, MOVE_SHED_TAIL,
    MOVE_TELEPORT, MOVE_U_TURN, MOVE_VOLT_SWITCH,
};

static const enum Move sStatusInflictingMoves[] =
{
    MOVE_NUZZLE, MOVE_THUNDER_WAVE, MOVE_TOXIC, MOVE_WILL_O_WISP, MOVE_YAWN,
};

static const enum Move sNoStabMoves[] =
{
    MOVE_ACCELEROCK, MOVE_AQUA_JET, MOVE_BREAKING_SWIPE, MOVE_BULLET_PUNCH,
    MOVE_DRAGON_TAIL, MOVE_ELECTROWEB, MOVE_ERUPTION, MOVE_EXPLOSION,
    MOVE_FAKE_OUT, MOVE_FEINT, MOVE_FLAME_CHARGE, MOVE_FLIP_TURN,
    MOVE_GRASSY_GLIDE, MOVE_ICE_SHARD, MOVE_ICY_WIND, MOVE_INFESTATION,
    MOVE_MACH_PUNCH, MOVE_MORTAL_SPIN, MOVE_NUZZLE, MOVE_QUICK_ATTACK,
    MOVE_RAPID_SPIN, MOVE_REVERSAL, MOVE_SELF_DESTRUCT, MOVE_SHADOW_SNEAK,
    MOVE_SNARL, MOVE_STRUGGLE_BUG, MOVE_SUCKER_PUNCH, MOVE_TRAILBLAZE,
    MOVE_U_TURN, MOVE_VACUUM_WAVE, MOVE_VOLT_SWITCH, MOVE_WATER_SHURIKEN,
    MOVE_WATER_SPOUT,
};

bool32 IsChampionsCircuitBattle(void)
{
    return VarGet(VAR_CHAMPIONS_CIRCUIT_ACTIVE) != 0 && gMain.inBattle;
}

bool32 IsChampionsCircuitOpponent(const struct Pokemon *mon)
{
    if (!VarGet(VAR_CHAMPIONS_CIRCUIT_ACTIVE))
        return FALSE;
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        if (mon == &gParties[B_TRAINER_OPPONENT_A][slot])
            return TRUE;
    return FALSE;
}

u8 GetChampionsCircuitOpponentLevel(u16 wins, u32 slot)
{
    // Mirrors campaign difficulty: Hard as designed, Medium -1, Easy -3.
    u32 level = CIRCUIT_BASE_LEVEL + 2 - GetTrainerLevelReduction()
              + wins / PARTY_SIZE + (slot < wins % PARTY_SIZE);
    return min(CHAMPIONS_CIRCUIT_MAX_LEVEL, level);
}

static u32 CircuitRandomUniform(u32 lo, u32 hi)
{
#if TESTING
    // Function tests normally rig RNG_NONE to zero. The Circuit's multi-seed
    // generator test needs the real seeded stream used by production.
    return RandomUniformDefault(RNG_NONE, lo, hi);
#else
    return RandomUniform(RNG_NONE, lo, hi);
#endif
}

static bool32 MoveInList(enum Move move, const enum Move *list, u32 count)
{
    for (u32 i = 0; i < count; i++)
        if (move == list[i])
            return TRUE;
    return FALSE;
}

static bool32 SetHasMove(const struct CircuitGeneratedSet *set, enum Move move)
{
    return MoveInList(move, set->moves, MAX_MON_MOVES);
}

static bool32 TeamHasMove(const struct CircuitTeamState *team, enum Move move)
{
    for (u32 i = 0; i < team->count; i++)
        if (SetHasMove(&team->sets[i], move))
            return TRUE;
    return FALSE;
}

static bool32 SetHasMoveFromList(const struct CircuitGeneratedSet *set, const enum Move *list, u32 count)
{
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        if (MoveInList(set->moves[i], list, count))
            return TRUE;
    return FALSE;
}

static u8 SetMoveCount(const struct CircuitGeneratedSet *set)
{
    u8 count = 0;

    while (count < MAX_MON_MOVES && set->moves[count] != MOVE_NONE)
        count++;
    return count;
}

static bool32 PoolContains(const struct CircuitMovePool *pool, enum Move move)
{
    return MoveInList(move, pool->moves, pool->count);
}

static void RemovePoolIndex(struct CircuitMovePool *pool, u8 index)
{
    if (index >= pool->count)
        return;
    pool->moves[index] = pool->moves[--pool->count];
}

static void RemovePoolMove(struct CircuitMovePool *pool, enum Move move)
{
    for (u32 i = 0; i < pool->count; i++)
    {
        if (pool->moves[i] == move)
        {
            RemovePoolIndex(pool, i);
            return;
        }
    }
}

static void RemovePoolMoves(struct CircuitMovePool *pool, const enum Move *moves, u32 count)
{
    for (u32 i = 0; i < count; i++)
        RemovePoolMove(pool, moves[i]);
}

static bool32 IsDamagingMove(enum Move move)
{
    return move != MOVE_NONE && GetMoveCategory(move) != DAMAGE_CATEGORY_STATUS;
}

static enum Type GetTemplateMoveType(
    enum Move move,
    const struct ShowdownCircuitTemplate *template,
    const struct ShowdownCircuitVariant *variant)
{
    enum Type type = GetMoveType(move);
    bool32 hasAerilate = FALSE;
    bool32 hasGalvanize = FALSE;
    bool32 hasNormalize = FALSE;
    bool32 hasPixilate = FALSE;
    bool32 hasRefrigerate = FALSE;
    bool32 hasLiquidVoice = FALSE;
    u32 abilityCount = template->abilityCount;

    for (u32 i = 0; i < abilityCount; i++)
    {
        enum Ability ability = variant->requiredItem != ITEM_NONE
                             ? gSpeciesInfo[variant->formSpecies].abilities[i]
                             : template->abilities[i];

        hasAerilate |= ability == ABILITY_AERILATE;
        hasGalvanize |= ability == ABILITY_GALVANIZE;
        hasNormalize |= ability == ABILITY_NORMALIZE;
        hasPixilate |= ability == ABILITY_PIXILATE;
        hasRefrigerate |= ability == ABILITY_REFRIGERATE;
        hasLiquidVoice |= ability == ABILITY_LIQUID_VOICE;
    }
    if (hasNormalize)
        return TYPE_NORMAL;
    if (hasLiquidVoice && IsSoundMove(move))
        return TYPE_WATER;
    if (hasAerilate && type == TYPE_NORMAL)
        return TYPE_FLYING;
    if (hasGalvanize && type == TYPE_NORMAL)
        return TYPE_ELECTRIC;
    if (hasPixilate && type == TYPE_NORMAL)
        return TYPE_FAIRY;
    if (hasRefrigerate && type == TYPE_NORMAL)
        return TYPE_ICE;
    return type;
}

static bool32 IsNoStabMove(enum Move move)
{
    return MoveInList(move, sNoStabMoves, ARRAY_COUNT(sNoStabMoves));
}

static void CullSelectedIncompatibilities(
    struct CircuitGeneratedSet *set,
    struct CircuitMovePool *pool,
    const struct ShowdownCircuitTemplate *template,
    const struct ShowdownCircuitVariant *variant)
{
    bool32 hasSetup = SetHasMoveFromList(set, sSetupMoves, ARRAY_COUNT(sSetupMoves));
    bool32 hasPhysicalSetup = SetHasMoveFromList(set, sPhysicalSetupMoves, ARRAY_COUNT(sPhysicalSetupMoves));
    bool32 hasSpeedSetup = SetHasMoveFromList(set, sSpeedSetupMoves, ARRAY_COUNT(sSpeedSetupMoves));
    bool32 hasSpeedControl = SetHasMoveFromList(set, sSpeedControlMoves, ARRAY_COUNT(sSpeedControlMoves));
    bool32 hasRecovery = SetHasMoveFromList(set, sRecoveryMoves, ARRAY_COUNT(sRecoveryMoves));
    bool32 hasHazard = SetHasMoveFromList(set, sHazardMoves, ARRAY_COUNT(sHazardMoves));
    bool32 hasPivot = SetHasMoveFromList(set, sPivotMoves, ARRAY_COUNT(sPivotMoves));
    bool32 hasStatus = SetHasMoveFromList(set, sStatusInflictingMoves, ARRAY_COUNT(sStatusInflictingMoves));

    // Showdown's statusMoves / healingwish-switcheroo-trick incompatibility.
    // Without both directions, Latios can receive Trick + Protect + a Scarf.
    static const enum Move choiceUtility[] = {MOVE_HEALING_WISH, MOVE_SWITCHEROO, MOVE_TRICK};
    if (SetHasMoveFromList(set, choiceUtility, ARRAY_COUNT(choiceUtility)))
    {
        for (u32 i = pool->count; i > 0; i--)
            if (GetMoveCategory(pool->moves[i - 1]) == DAMAGE_CATEGORY_STATUS)
                RemovePoolIndex(pool, i - 1);
    }
    else
    {
        for (u32 i = 0; i < MAX_MON_MOVES; i++)
            if (set->moves[i] != MOVE_NONE && GetMoveCategory(set->moves[i]) == DAMAGE_CATEGORY_STATUS)
                RemovePoolMoves(pool, choiceUtility, ARRAY_COUNT(choiceUtility));
    }

    // Redundant attacks culled by Champions teams.ts. Preserve alternatives
    // in the source pool while preventing them from occupying the same set.
    static const enum Move redundant[][2][4] =
    {
        {{MOVE_PSYCHIC, MOVE_PSYCHIC_NOISE}, {MOVE_PSYSHOCK, MOVE_PSYCHIC_NOISE}},
        {{MOVE_MUDDY_WATER, MOVE_SCALD, MOVE_SURF, MOVE_WATERFALL}, {MOVE_HYDRO_PUMP}},
        {{MOVE_GIGA_DRAIN, MOVE_HORN_LEECH, MOVE_TROP_KICK}, {MOVE_LEAF_STORM, MOVE_POWER_WHIP, MOVE_WOOD_HAMMER}},
        {{MOVE_DAZZLING_GLEAM}, {MOVE_ALLURING_VOICE, MOVE_MOONBLAST}},
        {{MOVE_FIRE_BLAST, MOVE_FLAMETHROWER}, {MOVE_FIERY_DANCE, MOVE_HEAT_WAVE, MOVE_OVERHEAT}},
        {{MOVE_AURA_SPHERE}, {MOVE_FOCUS_BLAST}},
        {{MOVE_CLOSE_COMBAT}, {MOVE_DRAIN_PUNCH}},
        {{MOVE_DRAGON_PULSE, MOVE_FICKLE_BEAM}, {MOVE_DRACO_METEOR}},
        {{MOVE_RISING_VOLTAGE}, {MOVE_VOLT_TACKLE}},
        {{MOVE_ROCK_SLIDE}, {MOVE_STONE_EDGE}},
        {{MOVE_FOUL_PLAY}, {MOVE_KNOCK_OFF}},
    };
    for (u32 i = 0; i < ARRAY_COUNT(redundant); i++)
        for (u32 side = 0; side < 2; side++)
            for (u32 m = 0; m < 4; m++)
                if (redundant[i][side][m] != MOVE_NONE && SetHasMove(set, redundant[i][side][m]))
                    RemovePoolMoves(pool, redundant[i][1 - side], 4);

    if (hasSpeedControl)
        RemovePoolMoves(pool, sSpeedControlMoves, ARRAY_COUNT(sSpeedControlMoves));
    if (hasPhysicalSetup)
        RemovePoolMoves(pool, sPhysicalSetupMoves, ARRAY_COUNT(sPhysicalSetupMoves));
    if (hasSpeedSetup)
        RemovePoolMove(pool, MOVE_QUICK_ATTACK);
    if (hasSetup)
    {
        static const enum Move sSetupConflicts[] =
        {
            MOVE_FAKE_OUT, MOVE_HELPING_HAND, MOVE_DEFOG, MOVE_HAZE, MOVE_TOXIC,
        };
        RemovePoolMoves(pool, sPivotMoves, ARRAY_COUNT(sPivotMoves));
        RemovePoolMoves(pool, sHazardMoves, ARRAY_COUNT(sHazardMoves));
        RemovePoolMoves(pool, sSetupConflicts, ARRAY_COUNT(sSetupConflicts));
    }
    if (SetHasMove(set, MOVE_FAKE_OUT) || SetHasMove(set, MOVE_HELPING_HAND))
        RemovePoolMoves(pool, sSetupMoves, ARRAY_COUNT(sSetupMoves));
    if (hasRecovery)
    {
        RemovePoolMove(pool, MOVE_HEAL_PULSE);
        RemovePoolMove(pool, MOVE_LIFE_DEW);
    }
    if (SetHasMove(set, MOVE_HEAL_PULSE))
        RemovePoolMove(pool, MOVE_LIFE_DEW);
    if (SetHasMove(set, MOVE_LIFE_DEW))
        RemovePoolMove(pool, MOVE_HEAL_PULSE);
    if (SetHasMove(set, MOVE_COACHING))
        RemovePoolMove(pool, MOVE_HELPING_HAND);
    if (SetHasMove(set, MOVE_HELPING_HAND))
        RemovePoolMove(pool, MOVE_COACHING);
    if (template->role != SHOWDOWN_ROLE_OFFENSIVE_PROTECT && SetHasMoveFromList(set, sProtectMoves, ARRAY_COUNT(sProtectMoves)))
        RemovePoolMove(pool, MOVE_U_TURN);
    if (hasPivot)
    {
        RemovePoolMoves(pool, sSetupMoves, ARRAY_COUNT(sSetupMoves));
        RemovePoolMove(pool, MOVE_SUBSTITUTE);
    }
    if (hasHazard)
        RemovePoolMoves(pool, sSetupMoves, ARRAY_COUNT(sSetupMoves));
    if (SetHasMove(set, MOVE_DEFOG))
        RemovePoolMoves(pool, sHazardMoves, ARRAY_COUNT(sHazardMoves));
    if (SetHasMove(set, MOVE_U_TURN))
        RemovePoolMove(pool, MOVE_TRICK);
    if (SetHasMove(set, MOVE_TRICK))
        RemovePoolMove(pool, MOVE_U_TURN);
    if (SetHasMove(set, MOVE_TAUNT))
        RemovePoolMove(pool, MOVE_ENCORE);
    if (SetHasMove(set, MOVE_ENCORE))
        RemovePoolMove(pool, MOVE_TAUNT);
    if (SetHasMove(set, MOVE_ROAR))
        RemovePoolMove(pool, MOVE_YAWN);
    if (SetHasMove(set, MOVE_YAWN))
        RemovePoolMove(pool, MOVE_ROAR);
    if (hasStatus)
    {
        RemovePoolMoves(pool, sStatusInflictingMoves, ARRAY_COUNT(sStatusInflictingMoves));
        RemovePoolMove(pool, MOVE_TOXIC_SPIKES);
    }

    enum Type type1 = gSpeciesInfo[variant->formSpecies].types[0];
    enum Type type2 = gSpeciesInfo[variant->formSpecies].types[1];
    if (type1 != TYPE_DARK && type2 != TYPE_DARK)
    {
        if (SetHasMove(set, MOVE_KNOCK_OFF))
            RemovePoolMove(pool, MOVE_SUCKER_PUNCH);
        if (SetHasMove(set, MOVE_SUCKER_PUNCH))
            RemovePoolMove(pool, MOVE_KNOCK_OFF);
    }
    if (type1 != TYPE_ICE && type2 != TYPE_ICE)
    {
        if (SetHasMove(set, MOVE_ICE_BEAM))
            RemovePoolMove(pool, MOVE_ICY_WIND);
        if (SetHasMove(set, MOVE_ICY_WIND))
            RemovePoolMove(pool, MOVE_ICE_BEAM);
    }
}

static bool32 AddMove(
    struct CircuitGeneratedSet *set,
    struct CircuitMovePool *pool,
    enum Move move,
    const struct ShowdownCircuitTemplate *template,
    const struct ShowdownCircuitVariant *variant)
{
    u8 count = SetMoveCount(set);

    if (count >= MAX_MON_MOVES || !PoolContains(pool, move) || SetHasMove(set, move))
        return FALSE;
    set->moves[count] = move;
    RemovePoolMove(pool, move);
    CullSelectedIncompatibilities(set, pool, template, variant);
    return TRUE;
}

static bool32 AddRandomMoveFromList(
    struct CircuitGeneratedSet *set,
    struct CircuitMovePool *pool,
    const enum Move *list,
    u32 listCount,
    const struct ShowdownCircuitTemplate *template,
    const struct ShowdownCircuitVariant *variant)
{
    enum Move selected = MOVE_NONE;
    u32 matches = 0;

    for (u32 i = 0; i < pool->count; i++)
    {
        if (MoveInList(pool->moves[i], list, listCount)
         && CircuitRandomUniform(0, ++matches - 1) == 0)
            selected = pool->moves[i];
    }
    return selected != MOVE_NONE && AddMove(set, pool, selected, template, variant);
}

static bool32 AddRandomDamagingMove(
    struct CircuitGeneratedSet *set,
    struct CircuitMovePool *pool,
    enum Type type,
    bool32 matchType,
    const struct ShowdownCircuitTemplate *template,
    const struct ShowdownCircuitVariant *variant)
{
    enum Move selected = MOVE_NONE;
    u32 matches = 0;

    for (u32 i = 0; i < pool->count; i++)
    {
        enum Move move = pool->moves[i];
        if (IsDamagingMove(move)
         && !IsNoStabMove(move)
         && (matchType ? GetTemplateMoveType(move, template, variant) == type
                       : type == TYPE_NONE || GetTemplateMoveType(move, template, variant) != type)
         && CircuitRandomUniform(0, ++matches - 1) == 0)
            selected = move;
    }
    return selected != MOVE_NONE && AddMove(set, pool, selected, template, variant);
}

static bool32 SetHasDamagingType(
    const struct CircuitGeneratedSet *set,
    enum Type type,
    const struct ShowdownCircuitTemplate *template,
    const struct ShowdownCircuitVariant *variant)
{
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        if (IsDamagingMove(set->moves[i]) && GetTemplateMoveType(set->moves[i], template, variant) == type)
            return TRUE;
    return FALSE;
}

static bool32 SetHasDamagingMove(const struct CircuitGeneratedSet *set)
{
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        if (IsDamagingMove(set->moves[i]) && !IsNoStabMove(set->moves[i]))
            return TRUE;
    return FALSE;
}

static void CullTeamDuplicateMoves(struct CircuitMovePool *pool, const struct CircuitTeamDetails *details)
{
    if (details->stickyWeb)
        RemovePoolMove(pool, MOVE_STICKY_WEB);
    if (details->stealthRock)
        RemovePoolMove(pool, MOVE_STEALTH_ROCK);
    if (details->defog || details->rapidSpin)
    {
        RemovePoolMove(pool, MOVE_DEFOG);
        RemovePoolMove(pool, MOVE_RAPID_SPIN);
    }
    if (details->toxicSpikes)
        RemovePoolMove(pool, MOVE_TOXIC_SPIKES);
    if (details->spikes >= 2)
        RemovePoolMove(pool, MOVE_SPIKES);
    if (details->statusCure)
        RemovePoolMove(pool, MOVE_HEAL_BELL);
}

static void BuildShowdownMoveset(
    struct CircuitGeneratedSet *set,
    const struct ShowdownCircuitTemplate *template,
    const struct ShowdownCircuitVariant *variant,
    const struct CircuitTeamDetails *details)
{
    static const enum Move sInitiallyForcedMoves[] =
    {
        MOVE_AURORA_VEIL, MOVE_BLIZZARD, MOVE_STICKY_WEB,
    };
    struct CircuitMovePool pool = {.count = template->moveCount};
    enum Type type1 = gSpeciesInfo[variant->formSpecies].types[0];
    enum Type type2 = gSpeciesInfo[variant->formSpecies].types[1];

    memcpy(pool.moves, template->moves, sizeof(pool.moves));
    CullTeamDuplicateMoves(&pool, details);

    for (u32 i = 0; i < ARRAY_COUNT(sInitiallyForcedMoves); i++)
        AddMove(set, &pool, sInitiallyForcedMoves[i], template, variant);
    if ((type1 == type2) && (type1 == TYPE_NORMAL || type1 == TYPE_FIGHTING))
        AddMove(set, &pool, MOVE_KNOCK_OFF, template, variant);
    if (PoolContains(&pool, MOVE_IRON_DEFENSE) || PoolContains(&pool, MOVE_SHELTER))
        AddMove(set, &pool, MOVE_BODY_PRESS, template, variant);
    if (variant->partySpecies == SPECIES_SHARPEDO)
        AddMove(set, &pool, MOVE_PROTECT, template, variant);
    if (variant->partySpecies == SPECIES_AEGISLASH && template->role == SHOWDOWN_ROLE_BULKY_ATTACKER)
        AddMove(set, &pool, MOVE_KINGS_SHIELD, template, variant);
    if (variant->partySpecies == SPECIES_QWILFISH)
        AddMove(set, &pool, MOVE_FLIP_TURN, template, variant);

    if (template->role == SHOWDOWN_ROLE_WALLBREAKER)
    {
        enum Move selected = MOVE_NONE;
        u32 matches = 0;
        for (u32 i = 0; i < pool.count; i++)
        {
            enum Move move = pool.moves[i];
            enum Type moveType = GetTemplateMoveType(move, template, variant);
            if (IsDamagingMove(move) && GetMovePriority(move) > 0
             && (moveType == type1 || moveType == type2)
             && CircuitRandomUniform(0, ++matches - 1) == 0)
                selected = move;
        }
        if (selected != MOVE_NONE)
            AddMove(set, &pool, selected, template, variant);
    }

    if (!SetHasDamagingType(set, type1, template, variant))
        AddRandomDamagingMove(set, &pool, type1, TRUE, template, variant);
    if (type2 != type1 && !SetHasDamagingType(set, type2, template, variant))
        AddRandomDamagingMove(set, &pool, type2, TRUE, template, variant);
    if (template->preferredType != TYPE_NONE && !SetHasDamagingType(set, template->preferredType, template, variant))
        AddRandomDamagingMove(set, &pool, template->preferredType, TRUE, template, variant);
    if (!SetHasDamagingMove(set))
    {
        if (!AddRandomDamagingMove(set, &pool, type1, TRUE, template, variant) && type2 != type1)
            AddRandomDamagingMove(set, &pool, type2, TRUE, template, variant);
    }

    if (template->role == SHOWDOWN_ROLE_BULKY_SETUP
     || template->role == SHOWDOWN_ROLE_BULKY_ATTACKER)
        AddRandomMoveFromList(set, &pool, sRecoveryMoves, ARRAY_COUNT(sRecoveryMoves), template, variant);
    if (template->role == SHOWDOWN_ROLE_BULKY_SETUP || template->role == SHOWDOWN_ROLE_SETUP_SWEEPER)
    {
        enum Move nonSpeedSetup[ARRAY_COUNT(sSetupMoves)];
        u8 nonSpeedCount = 0;
        for (u32 i = 0; i < ARRAY_COUNT(sSetupMoves); i++)
            if (!MoveInList(sSetupMoves[i], sSpeedSetupMoves, ARRAY_COUNT(sSpeedSetupMoves)))
                nonSpeedSetup[nonSpeedCount++] = sSetupMoves[i];
        if (!AddRandomMoveFromList(set, &pool, nonSpeedSetup, nonSpeedCount, template, variant))
            AddRandomMoveFromList(set, &pool, sSetupMoves, ARRAY_COUNT(sSetupMoves), template, variant);
    }

    static const enum Move alwaysForced[] =
    {
        MOVE_FINAL_GAMBIT, MOVE_MORTAL_SPIN, MOVE_SHED_TAIL,
        MOVE_FOLLOW_ME, MOVE_RAGE_POWDER,
    };
    for (u32 i = 0; i < ARRAY_COUNT(alwaysForced); i++)
        AddMove(set, &pool, alwaysForced[i], template, variant);

    if (template->role == SHOWDOWN_ROLE_OFFENSIVE_PROTECT)
        AddRandomMoveFromList(set, &pool, sProtectMoves, ARRAY_COUNT(sProtectMoves), template, variant);
    if (template->role == SHOWDOWN_ROLE_SUPPORT)
        AddMove(set, &pool, MOVE_FAKE_OUT, template, variant);
    if (template->role == SHOWDOWN_ROLE_WALLBREAKER || gSpeciesInfo[variant->formSpecies].baseSpeed <= 50)
        AddMove(set, &pool, MOVE_TRICK_ROOM, template, variant);
    if (template->role == SHOWDOWN_ROLE_FAST_ATTACKER || template->role == SHOWDOWN_ROLE_SUPPORT)
        AddRandomMoveFromList(set, &pool, sSpeedControlMoves, ARRAY_COUNT(sSpeedControlMoves), template, variant);
    if (template->role == SHOWDOWN_ROLE_FAST_ATTACKER)
    {
        if (!AddMove(set, &pool, MOVE_FAKE_OUT, template, variant))
            AddRandomMoveFromList(set, &pool, sProtectMoves, ARRAY_COUNT(sProtectMoves), template, variant);
    }
    if (template->role == SHOWDOWN_ROLE_BULKY_SETUP
     && !SetHasMoveFromList(set, sSetupMoves, ARRAY_COUNT(sSetupMoves)))
    {
        AddRandomMoveFromList(set, &pool, sRecoveryMoves, ARRAY_COUNT(sRecoveryMoves), template, variant);
        AddRandomMoveFromList(set, &pool, sProtectMoves, ARRAY_COUNT(sProtectMoves), template, variant);
    }

    if (!SetHasDamagingMove(set))
        AddRandomDamagingMove(set, &pool, TYPE_NONE, FALSE, template, variant);
    if (template->role != SHOWDOWN_ROLE_SUPPORT
     && template->role != SHOWDOWN_ROLE_BULKY_SETUP
     && template->role != SHOWDOWN_ROLE_BULKY_ATTACKER)
    {
        enum Type onlyType = TYPE_NONE;
        u8 damagingCount = 0;
        for (u32 i = 0; i < MAX_MON_MOVES; i++)
        {
            if (IsDamagingMove(set->moves[i]) && !IsNoStabMove(set->moves[i]))
            {
                onlyType = GetTemplateMoveType(set->moves[i], template, variant);
                damagingCount++;
            }
        }
        if (damagingCount == 1)
            AddRandomDamagingMove(set, &pool, onlyType, FALSE, template, variant);
    }

    while (SetMoveCount(set) < MAX_MON_MOVES && pool.count != 0)
    {
        enum Move move = pool.moves[CircuitRandomUniform(0, pool.count - 1)];
        AddMove(set, &pool, move, template, variant);
        if (move == MOVE_SLEEP_TALK)
            AddMove(set, &pool, MOVE_REST, template, variant);
        else if (move == MOVE_REST)
            AddMove(set, &pool, MOVE_SLEEP_TALK, template, variant);
        else if (move == MOVE_WISH)
            AddMove(set, &pool, MOVE_PROTECT, template, variant);
        else if (move == MOVE_LEECH_SEED)
            AddMove(set, &pool, MOVE_SUBSTITUTE, template, variant);
        else if (move == MOVE_REFLECT)
            AddMove(set, &pool, MOVE_LIGHT_SCREEN, template, variant);
    }

    // If culling exhausted this role, the caller rejects it. Reintroducing
    // removed moves here would silently undo every incompatibility rule.
}

static bool32 SetHasGrassDamage(
    const struct CircuitGeneratedSet *set,
    const struct ShowdownCircuitTemplate *template,
    const struct ShowdownCircuitVariant *variant)
{
    return SetHasDamagingType(set, TYPE_GRASS, template, variant);
}

static bool32 AbilityAllowed(
    enum Ability ability,
    const struct CircuitGeneratedSet *set,
    const struct ShowdownCircuitTemplate *template,
    const struct ShowdownCircuitVariant *variant,
    const struct CircuitTeamDetails *details)
{
    switch (ability)
    {
    case ABILITY_CHLOROPHYLL:
    case ABILITY_SOLAR_POWER:
        return details->sun;
    case ABILITY_HYDRATION:
    case ABILITY_SWIFT_SWIM:
        return details->rain;
    case ABILITY_OVERGROW:
        return SetHasGrassDamage(set, template, variant);
    case ABILITY_SAND_FORCE:
    case ABILITY_SAND_RUSH:
        return details->sand;
    case ABILITY_SLUSH_RUSH:
        return details->snow || SetHasMove(set, MOVE_SNOWSCAPE);
    default:
        return TRUE;
    }
}

static enum Ability ChooseShowdownAbility(
    const struct CircuitGeneratedSet *set,
    const struct ShowdownCircuitTemplate *template,
    const struct ShowdownCircuitVariant *variant,
    const struct CircuitTeamDetails *details)
{
    enum Ability choices[SHOWDOWN_CIRCUIT_MAX_ABILITIES];
    u8 count = 0;

    if (variant->partySpecies == SPECIES_TOUCANNON)
    {
        if (SetHasMove(set, MOVE_BULLET_SEED) || SetHasMove(set, MOVE_ROCK_BLAST))
            return ABILITY_SKILL_LINK;
        return template->abilities[0];
    }
    if (SetHasMove(set, MOVE_SNOWSCAPE))
        for (u32 i = 0; i < template->abilityCount; i++)
            if (template->abilities[i] == ABILITY_SLUSH_RUSH)
                return ABILITY_SLUSH_RUSH;

    for (u32 i = 0; i < template->abilityCount; i++)
        if (AbilityAllowed(template->abilities[i], set, template, variant, details))
            choices[count++] = template->abilities[i];
    if (count != 0)
        return choices[CircuitRandomUniform(0, count - 1)];
    for (u32 i = 0; i < template->abilityCount; i++)
    {
        enum Ability ability = template->abilities[i];
        if (ability == ABILITY_CHLOROPHYLL || ability == ABILITY_SAND_RUSH
         || ability == ABILITY_SLUSH_RUSH || ability == ABILITY_SOLAR_POWER
         || ability == ABILITY_SWIFT_SWIM)
            choices[count++] = ability;
    }
    if (count != 0)
        return choices[CircuitRandomUniform(0, count - 1)];
    return template->abilities[CircuitRandomUniform(0, template->abilityCount - 1)];
}

static enum Item GetTypeBoostingItem(enum Type type)
{
    static const enum Item sTypeItems[NUMBER_OF_MON_TYPES] =
    {
        [TYPE_BUG] = ITEM_SILVER_POWDER,
        [TYPE_DARK] = ITEM_BLACK_GLASSES,
        [TYPE_DRAGON] = ITEM_DRAGON_FANG,
        [TYPE_ELECTRIC] = ITEM_MAGNET,
        [TYPE_FAIRY] = ITEM_FAIRY_FEATHER,
        [TYPE_FIGHTING] = ITEM_BLACK_BELT,
        [TYPE_FIRE] = ITEM_CHARCOAL,
        [TYPE_FLYING] = ITEM_SHARP_BEAK,
        [TYPE_GHOST] = ITEM_SPELL_TAG,
        [TYPE_GRASS] = ITEM_MIRACLE_SEED,
        [TYPE_GROUND] = ITEM_SOFT_SAND,
        [TYPE_ICE] = ITEM_NEVER_MELT_ICE,
        [TYPE_NORMAL] = ITEM_SILK_SCARF,
        [TYPE_POISON] = ITEM_POISON_BARB,
        [TYPE_PSYCHIC] = ITEM_TWISTED_SPOON,
        [TYPE_ROCK] = ITEM_HARD_STONE,
        [TYPE_STEEL] = ITEM_METAL_COAT,
        [TYPE_WATER] = ITEM_MYSTIC_WATER,
    };

    if (type >= NUMBER_OF_MON_TYPES)
        return ITEM_NONE;
    return sTypeItems[type];
}

static enum Item ChooseChoiceItem(const struct CircuitGeneratedSet *set, const struct ShowdownCircuitVariant *variant, bool32 wallbreaker)
{
    u32 physical = 0, special = 0;
    bool32 priority = FALSE;
    u32 speed = gSpeciesInfo[variant->formSpecies].baseSpeed;

    for (u32 i = 0; i < MAX_MON_MOVES; i++)
    {
        enum Move move = set->moves[i];
        if (!IsDamagingMove(move))
            continue;
        physical += GetMoveCategory(move) == DAMAGE_CATEGORY_PHYSICAL;
        special += GetMoveCategory(move) == DAMAGE_CATEGORY_SPECIAL;
        priority |= GetMovePriority(move) > 0;
    }
    // Adapted from Gen 9 getDoublesItem's scarfReqs and Choice role branch.
    if (SetHasMove(set, MOVE_FINAL_GAMBIT)
     || (!wallbreaker && !priority && set->ability != ABILITY_SPEED_BOOST
      && speed >= 60 && speed <= 108 && CircuitRandomUniform(0, 1) == 0))
        return ITEM_CHOICE_SCARF;
    return physical > special ? ITEM_CHOICE_BAND : ITEM_CHOICE_SPECS;
}

static enum Item ChooseShowdownItem(
    const struct CircuitGeneratedSet *set,
    const struct ShowdownCircuitTemplate *template,
    const struct ShowdownCircuitVariant *variant)
{
    enum Ability ability = set->ability;
    enum Type type1 = gSpeciesInfo[variant->formSpecies].types[0];
    enum Type type2 = gSpeciesInfo[variant->formSpecies].types[1];

    if (variant->requiredItem != ITEM_NONE)
        return variant->requiredItem;
    if (variant->partySpecies == SPECIES_PIKACHU)
        return ITEM_LIGHT_BALL;
    if (variant->partySpecies == SPECIES_SMEARGLE)
        return ITEM_FOCUS_SASH;
    if (GetSpeciesEvolutions(variant->partySpecies) != NULL
     && GetSpeciesEvolutions(variant->partySpecies)[0].method != EVOLUTIONS_END)
        return ITEM_EVIOLITE;
    if (SetHasMove(set, MOVE_GEOMANCY) || SetHasMove(set, MOVE_METEOR_BEAM)
     || SetHasMove(set, MOVE_SKY_ATTACK) || SetHasMove(set, MOVE_ELECTRO_SHOT))
        return ITEM_POWER_HERB;
    if (ability == ABILITY_GUTS && !SetHasMove(set, MOVE_SLEEP_TALK))
        return type1 == TYPE_FIRE || type2 == TYPE_FIRE ? ITEM_TOXIC_ORB : ITEM_FLAME_ORB;
    if (ability == ABILITY_POISON_HEAL)
        return ITEM_TOXIC_ORB;
    if (ability == ABILITY_PROTOSYNTHESIS || ability == ABILITY_QUARK_DRIVE)
        return ITEM_BOOSTER_ENERGY;
    if (template->role == SHOWDOWN_ROLE_CHOICE_ITEM)
        return ChooseChoiceItem(set, variant, FALSE);
    if (ability == ABILITY_CHEEK_POUCH || ability == ABILITY_CUD_CHEW
     || ability == ABILITY_HARVEST || ability == ABILITY_RIPEN
     || SetHasMove(set, MOVE_BELLY_DRUM))
        return ITEM_SITRUS_BERRY;
    if (variant->partySpecies == SPECIES_ALAKAZAM && CircuitRandomUniform(0, 1) == 0)
        return ITEM_FOCUS_SASH;
    if (variant->partySpecies == SPECIES_GLIMMORA)
        return ITEM_FOCUS_SASH;
    if (variant->partySpecies == SPECIES_RAMPARDOS && template->role == SHOWDOWN_ROLE_FAST_ATTACKER)
        return ITEM_CHOICE_SCARF;
    if (SetHasMove(set, MOVE_HEALING_WISH) || SetHasMove(set, MOVE_SWITCHEROO) || SetHasMove(set, MOVE_TRICK))
        return ChooseChoiceItem(set, variant, template->role == SHOWDOWN_ROLE_WALLBREAKER);
    if (ability == ABILITY_UNBURDEN)
        return SetHasMove(set, MOVE_CLOSE_COMBAT) || SetHasMove(set, MOVE_LEAF_STORM) ? ITEM_WHITE_HERB : ITEM_SITRUS_BERRY;
    if (SetHasMove(set, MOVE_SHELL_SMASH))
        return ITEM_WHITE_HERB;
    if ((ability == ABILITY_MAGIC_GUARD || ability == ABILITY_SHEER_FORCE) && variant->partySpecies != SPECIES_TOUCANNON)
        return ITEM_LIFE_ORB;
    if (SetHasMove(set, MOVE_ACROBATICS))
        return ITEM_NONE;
    if (SetHasMove(set, MOVE_AURORA_VEIL) || (SetHasMove(set, MOVE_LIGHT_SCREEN) && SetHasMove(set, MOVE_REFLECT)))
        return ITEM_LIGHT_CLAY;
    if (SetHasMove(set, MOVE_REST) && !SetHasMove(set, MOVE_SLEEP_TALK)
     && ability != ABILITY_NATURAL_CURE && ability != ABILITY_SHED_SKIN)
        return ITEM_CHESTO_BERRY;
    if (gSpeciesInfo[variant->formSpecies].baseSpeed <= 70
     && (SetHasMove(set, MOVE_FOLLOW_ME) || SetHasMove(set, MOVE_RAGE_POWDER)))
        return ITEM_ROCKY_HELMET;
    if ((type1 == TYPE_NORMAL || type2 == TYPE_NORMAL)
     && SetHasMove(set, MOVE_DOUBLE_EDGE) && SetHasMove(set, MOVE_FAKE_OUT))
        return ITEM_SILK_SCARF;
    if ((variant->partySpecies == SPECIES_FROSLASS && SetHasMove(set, MOVE_TRIPLE_AXEL))
     || SetHasMove(set, MOVE_POPULATION_BOMB)
     || (ability == ABILITY_HUSTLE
      && SetHasMoveFromList(set, sSetupMoves, ARRAY_COUNT(sSetupMoves))
      && CircuitRandomUniform(0, 1) == 0)
     || (variant->partySpecies == SPECIES_TSAREENA && template->role == SHOWDOWN_ROLE_OFFENSIVE_PROTECT))
        return ITEM_WIDE_LENS;
    if (template->preferredType != TYPE_NONE
     && (type1 == template->preferredType || type2 == template->preferredType))
        return GetTypeBoostingItem(template->preferredType);
    if (template->role == SHOWDOWN_ROLE_FAST_ATTACKER)
        return ITEM_FOCUS_SASH;
    if (template->role == SHOWDOWN_ROLE_BULKY_SETUP && !SetHasMove(set, MOVE_DRAGON_DANCE))
        return ITEM_LEFTOVERS;
    if (template->role == SHOWDOWN_ROLE_OFFENSIVE_PROTECT
     || template->role == SHOWDOWN_ROLE_WALLBREAKER
     || template->role == SHOWDOWN_ROLE_SETUP_SWEEPER)
        return ITEM_LIFE_ORB;
    return ITEM_SITRUS_BERRY;
}

static void SetShowdownEvs(
    struct CircuitGeneratedSet *set,
    const struct ShowdownCircuitTemplate *template)
{
    bool32 physicalDamage = FALSE;
    bool32 specialDamage = FALSE;
    bool32 slow = SetHasMove(set, MOVE_GYRO_BALL)
               || SetHasMove(set, MOVE_METAL_BURST)
               || SetHasMove(set, MOVE_TRICK_ROOM);
    bool32 bulky = template->role == SHOWDOWN_ROLE_BULKY_SETUP
                || template->role == SHOWDOWN_ROLE_BULKY_ATTACKER;

    for (u32 i = 0; i < NUM_STATS; i++)
        set->evs[i] = 0;
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
    {
        enum Move move = set->moves[i];

        if (move != MOVE_NONE
         && GetMoveCategory(move) == DAMAGE_CATEGORY_PHYSICAL
         && move != MOVE_BODY_PRESS && move != MOVE_FOUL_PLAY)
            physicalDamage = TRUE;
        if (move != MOVE_NONE && GetMoveCategory(move) == DAMAGE_CATEGORY_SPECIAL)
            specialDamage = TRUE;
    }

    if (!physicalDamage && !specialDamage)
    {
        set->evs[STAT_HP] = 252;
        if (template->role == SHOWDOWN_ROLE_SUPPORT && !slow)
        {
            set->evs[STAT_DEF] = 4;
            set->evs[STAT_SPEED] = 252;
            set->nature = NATURE_TIMID;
        }
        else
        {
            set->evs[STAT_DEF] = 124;
            set->evs[STAT_SPDEF] = 132;
            set->nature = slow ? NATURE_SASSY : NATURE_CALM;
        }
    }
    else if (physicalDamage && specialDamage)
    {
        if (slow || bulky)
        {
            set->evs[STAT_HP] = 252;
            set->evs[STAT_ATK] = 124;
            set->evs[STAT_SPATK] = 124;
            set->evs[STAT_DEF] = 8;
            set->nature = slow ? NATURE_QUIET : NATURE_RASH;
        }
        else
        {
            set->evs[STAT_ATK] = 164;
            set->evs[STAT_SPATK] = 172;
            set->evs[STAT_SPEED] = 172;
            set->nature = NATURE_NAIVE;
        }
    }
    else if (physicalDamage)
    {
        set->evs[STAT_ATK] = 252;
        if (slow || bulky)
        {
            set->evs[STAT_HP] = 252;
            set->evs[STAT_DEF] = 4;
            set->nature = slow ? NATURE_BRAVE : NATURE_ADAMANT;
        }
        else
        {
            set->evs[STAT_HP] = 4;
            set->evs[STAT_SPEED] = 252;
            set->nature = NATURE_JOLLY;
        }
    }
    else
    {
        set->evs[STAT_SPATK] = 252;
        if (slow || bulky)
        {
            set->evs[STAT_HP] = 252;
            set->evs[STAT_SPDEF] = 4;
            set->nature = slow ? NATURE_QUIET : NATURE_MODEST;
        }
        else
        {
            set->evs[STAT_HP] = 4;
            set->evs[STAT_SPEED] = 252;
            set->nature = NATURE_TIMID;
        }
    }

}

static bool32 IsBaseDexExhausted(enum NationalDexOrder dex)
{
    return dex <= NATIONAL_DEX_COUNT && sExhaustedBaseDex[dex];
}

static bool32 IsMegaVariant(const struct ShowdownCircuitVariant *variant)
{
    return gItemsInfo[variant->requiredItem].sortType == ITEM_TYPE_MEGA_STONE;
}

static bool32 VariantAllowedByMegaState(const struct ShowdownCircuitVariant *variant, bool32 hasMega, bool32 groupHasMega)
{
    if (hasMega)
        return !IsMegaVariant(variant);
    if (groupHasMega)
        return IsMegaVariant(variant);
    return TRUE;
}

static bool32 GroupHasMega(enum NationalDexOrder dex)
{
    for (u32 i = 0; i < SHOWDOWN_CIRCUIT_VARIANT_COUNT; i++)
        if (SpeciesToNationalPokedexNum(gShowdownCircuitVariants[i].partySpecies) == dex
         && IsMegaVariant(&gShowdownCircuitVariants[i]))
            return TRUE;
    return FALSE;
}

static bool32 ChooseBaseDex(enum NationalDexOrder *dexOut)
{
    u32 matches = 0;
    enum NationalDexOrder previousDex = NATIONAL_DEX_NONE;

    for (u32 i = 0; i < SHOWDOWN_CIRCUIT_VARIANT_COUNT; i++)
    {
        enum NationalDexOrder dex = SpeciesToNationalPokedexNum(gShowdownCircuitVariants[i].partySpecies);

        // Generated variants are grouped by National Dex family. Sampling
        // only each group's first row avoids the former nested full-table
        // scans, which were prohibitively expensive on GBA hardware.
        if (dex == previousDex)
            continue;
        previousDex = dex;
        if (IsBaseDexExhausted(dex))
            continue;
        if (CircuitRandomUniform(0, ++matches - 1) == 0)
            *dexOut = dex;
    }
    if (matches == 0)
        return FALSE;
    sExhaustedBaseDex[*dexOut] = TRUE;
    return TRUE;
}

static bool32 ChooseVariantForDex(enum NationalDexOrder dex, bool32 hasMega, u16 *variantOut)
{
    bool32 groupHasMega = GroupHasMega(dex);
    u32 matches = 0;

    for (u32 i = 0; i < SHOWDOWN_CIRCUIT_VARIANT_COUNT; i++)
    {
        const struct ShowdownCircuitVariant *variant = &gShowdownCircuitVariants[i];
        if (SpeciesToNationalPokedexNum(variant->partySpecies) != dex
         || !VariantAllowedByMegaState(variant, hasMega, groupHasMega))
            continue;
        if (CircuitRandomUniform(0, ++matches - 1) == 0)
            *variantOut = i;
    }
    return matches != 0;
}

static uq4_12_t GetSpeciesTypeModifier(enum Type attackType, enum Species species)
{
    enum Type type1 = gSpeciesInfo[species].types[0];
    enum Type type2 = gSpeciesInfo[species].types[1];
    uq4_12_t modifier = GetTypeModifier(attackType, type1);

    if (type2 != type1)
        modifier = uq4_12_multiply(modifier, GetTypeModifier(attackType, type2));
    return modifier;
}

static bool32 IsFreezeDryWeak(enum Species species)
{
    enum Type type1 = gSpeciesInfo[species].types[0];
    enum Type type2 = gSpeciesInfo[species].types[1];
    uq4_12_t ice = GetSpeciesTypeModifier(TYPE_ICE, species);

    return ice > UQ_4_12(1.0)
        || ((type1 == TYPE_WATER || type2 == TYPE_WATER) && ice > UQ_4_12(0.25));
}

static bool32 CompatibilityAllowed(const struct CircuitTeamState *team, u16 flags)
{
    u16 weatherFlags = SHOWDOWN_COMPAT_SUN_SETTER | SHOWDOWN_COMPAT_RAIN_SETTER
                     | SHOWDOWN_COMPAT_SAND_SETTER | SHOWDOWN_COMPAT_SNOW_SETTER;

    if ((flags & SHOWDOWN_COMPAT_WEB_SETTER) && (team->compatibilityFlags & SHOWDOWN_COMPAT_WEB_SETTER))
        return FALSE;
    if ((flags & SHOWDOWN_COMPAT_SCREEN_SETTER) && (team->compatibilityFlags & SHOWDOWN_COMPAT_SCREEN_SETTER))
        return FALSE;
    if ((flags & SHOWDOWN_COMPAT_SCREEN_CLEANER) && (team->compatibilityFlags & SHOWDOWN_COMPAT_SCREEN_SETTER))
        return FALSE;
    if ((flags & SHOWDOWN_COMPAT_SCREEN_SETTER) && (team->compatibilityFlags & SHOWDOWN_COMPAT_SCREEN_CLEANER))
        return FALSE;
    if ((flags & SHOWDOWN_COMPAT_DRY_SKIN_SUN) && (team->compatibilityFlags & SHOWDOWN_COMPAT_SUN_SETTER))
        return FALSE;
    if ((flags & SHOWDOWN_COMPAT_SUN_SETTER) && (team->compatibilityFlags & SHOWDOWN_COMPAT_DRY_SKIN_SUN))
        return FALSE;
    if ((flags & SHOWDOWN_COMPAT_LIGHTNING_ROD) && (team->compatibilityFlags & SHOWDOWN_COMPAT_LIGHTNING_ROD))
        return FALSE;
    if ((flags & weatherFlags) && (team->compatibilityFlags & weatherFlags)
     && (flags & weatherFlags) != (team->compatibilityFlags & weatherFlags))
        return FALSE;
    return TRUE;
}

static bool32 CandidateAllowed(const struct CircuitTeamState *team, const struct ShowdownCircuitVariant *variant)
{
    enum Species species = variant->formSpecies;
    enum Type type1 = gSpeciesInfo[species].types[0];
    enum Type type2 = gSpeciesInfo[species].types[1];

    if (type1 < NUMBER_OF_MON_TYPES && team->typeCounts[type1] >= 2)
        return FALSE;
    if (type2 != type1 && type2 < NUMBER_OF_MON_TYPES && team->typeCounts[type2] >= 2)
        return FALSE;
    if (!CompatibilityAllowed(team, variant->compatibilityFlags))
        return FALSE;
    for (enum Type type = TYPE_NORMAL; type < NUMBER_OF_MON_TYPES; type++)
    {
        uq4_12_t modifier = GetSpeciesTypeModifier(type, species);
        if (modifier > UQ_4_12(1.0) && team->weaknessCounts[type] >= 3)
            return FALSE;
        if (modifier > UQ_4_12(2.0) && team->doubleWeaknessCounts[type] >= 1)
            return FALSE;
    }
    if (IsFreezeDryWeak(species) && team->freezeDryWeakCount >= 4)
        return FALSE;
    return TRUE;
}

static enum Ability GetCircuitSetAbility(const struct CircuitGeneratedSet *set)
{
    const struct ShowdownCircuitVariant *variant = &gShowdownCircuitVariants[set->variantIndex];
    return IsMegaVariant(variant) ? gSpeciesInfo[variant->formSpecies].abilities[0] : set->ability;
}

static void UpdateTeamDetails(struct CircuitTeamDetails *details, const struct CircuitGeneratedSet *set)
{
    enum Ability ability = GetCircuitSetAbility(set);

    details->rain |= ability == ABILITY_DRIZZLE || ability == ABILITY_PRIMORDIAL_SEA || SetHasMove(set, MOVE_RAIN_DANCE);
    details->sun |= ability == ABILITY_DROUGHT || ability == ABILITY_ORICHALCUM_PULSE
                 || ability == ABILITY_DESOLATE_LAND || SetHasMove(set, MOVE_SUNNY_DAY);
    details->sand |= ability == ABILITY_SAND_STREAM || SetHasMove(set, MOVE_SANDSTORM);
    details->snow |= ability == ABILITY_SNOW_WARNING || SetHasMove(set, MOVE_SNOWSCAPE) || SetHasMove(set, MOVE_CHILLY_RECEPTION);
    details->statusCure |= SetHasMove(set, MOVE_HEAL_BELL);
    if (SetHasMove(set, MOVE_SPIKES) || SetHasMove(set, MOVE_CEASELESS_EDGE))
        details->spikes++;
    details->toxicSpikes |= SetHasMove(set, MOVE_TOXIC_SPIKES) || ability == ABILITY_TOXIC_DEBRIS;
    details->stealthRock |= SetHasMove(set, MOVE_STEALTH_ROCK) || SetHasMove(set, MOVE_STONE_AXE);
    details->stickyWeb |= SetHasMove(set, MOVE_STICKY_WEB);
    details->defog |= SetHasMove(set, MOVE_DEFOG);
    details->rapidSpin |= SetHasMove(set, MOVE_RAPID_SPIN) || SetHasMove(set, MOVE_MORTAL_SPIN);
    details->screens |= SetHasMove(set, MOVE_AURORA_VEIL)
                     || SetHasMove(set, MOVE_REFLECT) || SetHasMove(set, MOVE_LIGHT_SCREEN);
    if (ability == ABILITY_ELECTRIC_SURGE || ability == ABILITY_HADRON_ENGINE || SetHasMove(set, MOVE_ELECTRIC_TERRAIN))
        details->terrain |= (1u << B_TERRAIN_ELECTRIC);
    if (ability == ABILITY_PSYCHIC_SURGE || SetHasMove(set, MOVE_PSYCHIC_TERRAIN))
        details->terrain |= (1u << B_TERRAIN_PSYCHIC);
    if (ability == ABILITY_GRASSY_SURGE || SetHasMove(set, MOVE_GRASSY_TERRAIN))
        details->terrain |= (1u << B_TERRAIN_GRASSY);
    if (ability == ABILITY_MISTY_SURGE || SetHasMove(set, MOVE_MISTY_TERRAIN))
        details->terrain |= (1u << B_TERRAIN_MISTY);
}

static bool32 TeamHasSpecies(const struct CircuitTeamState *team, enum Species species)
{
    for (u32 i = 0; i < team->count; i++)
        if (gShowdownCircuitVariants[team->sets[i].variantIndex].partySpecies == species)
            return TRUE;
    return FALSE;
}

// Evaluate the selected sets, not only species labels: roles can change an
// Ability, weather, speed plan, or the partner a Pokemon actually needs.
static bool32 TeamIsCoherent(const struct CircuitTeamState *team)
{
    u32 physical = 0, special = 0, attackers = 0, speedControl = 0, slow = 0, fragile = 0;
    bool32 trickRoom = TeamHasMove(team, MOVE_TRICK_ROOM);
    const struct CircuitTeamDetails *d = &team->details;

    if (d->rain + d->sun + d->sand + d->snow > 1)
        return FALSE;
    for (u32 i = 0; i < team->count; i++)
    {
        const struct CircuitGeneratedSet *set = &team->sets[i];
        enum Species species = gShowdownCircuitVariants[set->variantIndex].formSpecies;
        u32 attacks = 0, totalStats = 0;
        enum Ability ability = GetCircuitSetAbility(set);

        for (u32 stat = 0; stat < NUM_STATS; stat++)
            totalStats += GetSpeciesBaseStat(species, stat);
        fragile += totalStats < 420;
        slow += gSpeciesInfo[species].baseSpeed <= 65;
        speedControl += SetHasMoveFromList(set, sSpeedControlMoves, ARRAY_COUNT(sSpeedControlMoves));
        for (u32 m = 0; m < MAX_MON_MOVES; m++)
        {
            enum Move move = set->moves[m];
            if (!IsDamagingMove(move))
                continue;
            attacks++;
            physical += GetMoveCategory(move) == DAMAGE_CATEGORY_PHYSICAL;
            special += GetMoveCategory(move) == DAMAGE_CATEGORY_SPECIAL;
        }
        attackers += attacks != 0;
        if ((set->dependency == CIRCUIT_DEPENDENCY_RAIN && !d->rain)
         || (set->dependency == CIRCUIT_DEPENDENCY_SUN && !d->sun)
         || (set->dependency == CIRCUIT_DEPENDENCY_SAND && !d->sand)
         || (set->dependency == CIRCUIT_DEPENDENCY_SNOW && !d->snow)
         || (set->dependency == CIRCUIT_DEPENDENCY_TRICK_ROOM && !trickRoom)
         || (set->dependency == CIRCUIT_DEPENDENCY_TERRAIN && !d->terrain)
         || (set->dependency == CIRCUIT_DEPENDENCY_GRAVITY && !TeamHasMove(team, MOVE_GRAVITY)))
            return FALSE;
        if (ability == ABILITY_COMMANDER && !TeamHasSpecies(team, SPECIES_DONDOZO))
            return FALSE;
        if (SetHasMove(set, MOVE_AURORA_VEIL) && !d->snow)
            return FALSE;
        if (SetHasMove(set, MOVE_WEATHER_BALL) && !(d->rain || d->sun || d->sand || d->snow))
            return FALSE;
        if ((SetHasMove(set, MOVE_SOLAR_BEAM) || SetHasMove(set, MOVE_SOLAR_BLADE))
         && !d->sun && set->item != ITEM_POWER_HERB)
            return FALSE;
        if (ability == ABILITY_SWIFT_SWIM && !d->rain)
            return FALSE;
        if ((ability == ABILITY_CHLOROPHYLL || ability == ABILITY_SOLAR_POWER) && !d->sun)
            return FALSE;
        if (ability == ABILITY_SAND_RUSH && !d->sand)
            return FALSE;
        if (ability == ABILITY_SLUSH_RUSH && !d->snow)
            return FALSE;
        if (ability == ABILITY_SURGE_SURFER && !(d->terrain & (1u << B_TERRAIN_ELECTRIC)))
            return FALSE;
        if (SetHasMove(set, MOVE_EXPANDING_FORCE) && !(d->terrain & (1u << B_TERRAIN_PSYCHIC)))
            return FALSE;
        if (SetHasMove(set, MOVE_GRASSY_GLIDE) && !(d->terrain & (1u << B_TERRAIN_GRASSY)))
            return FALSE;
        if (ability == ABILITY_SCREEN_CLEANER && d->screens)
            return FALSE;
    }
    return attackers >= 4 && physical && special && speedControl && fragile <= 1
        && (!trickRoom || slow >= 2);
}

static u32 LeadSupportScore(const struct CircuitGeneratedSet *set)
{
    return 3 * SetHasMove(set, MOVE_FAKE_OUT)
         + 3 * SetHasMove(set, MOVE_TAILWIND)
         + 2 * (SetHasMove(set, MOVE_FOLLOW_ME) || SetHasMove(set, MOVE_RAGE_POWDER))
         + (set->ability == ABILITY_INTIMIDATE);
}

static void ChooseCoherentLeads(struct CircuitTeamState *team)
{
    u32 bestA = 0, bestB = 1, bestScore = 0;
    bool32 foundLead = FALSE;
    for (u32 a = 0; a < PARTY_SIZE; a++)
    {
        for (u32 b = a + 1; b < PARTY_SIZE; b++)
        {
            const struct CircuitGeneratedSet *left = &team->sets[a], *right = &team->sets[b];
            enum Species speciesA = gShowdownCircuitVariants[left->variantIndex].formSpecies;
            enum Species speciesB = gShowdownCircuitVariants[right->variantIndex].formSpecies;
            u32 score = LeadSupportScore(left) + LeadSupportScore(right);
            enum Ability leftAbility = GetCircuitSetAbility(left);
            enum Ability rightAbility = GetCircuitSetAbility(right);
            struct CircuitTeamDetails leftField = {0}, rightField = {0};
            bool32 leftAttacks = FALSE, rightAttacks = FALSE;
            UpdateTeamDetails(&leftField, left);
            UpdateTeamDetails(&rightField, right);
            for (u32 m = 0; m < MAX_MON_MOVES; m++)
            {
                leftAttacks |= IsDamagingMove(left->moves[m]) && left->moves[m] != MOVE_FAKE_OUT;
                rightAttacks |= IsDamagingMove(right->moves[m]) && right->moves[m] != MOVE_FAKE_OUT;
            }
            // A pair of support-only leads must not outrank every attacker.
            if (!leftAttacks && !rightAttacks)
                continue;
            if ((leftField.rain && rightAbility == ABILITY_SWIFT_SWIM)
             || (rightField.rain && leftAbility == ABILITY_SWIFT_SWIM)
             || (leftField.sun && rightAbility == ABILITY_CHLOROPHYLL)
             || (rightField.sun && leftAbility == ABILITY_CHLOROPHYLL)
             || (leftField.sand && rightAbility == ABILITY_SAND_RUSH)
             || (rightField.sand && leftAbility == ABILITY_SAND_RUSH)
             || (leftField.snow && rightAbility == ABILITY_SLUSH_RUSH)
             || (rightField.snow && leftAbility == ABILITY_SLUSH_RUSH)
             || ((leftField.terrain & (1u << B_TERRAIN_ELECTRIC)) && rightAbility == ABILITY_SURGE_SURFER)
             || ((rightField.terrain & (1u << B_TERRAIN_ELECTRIC)) && leftAbility == ABILITY_SURGE_SURFER)
             || ((leftField.terrain & (1u << B_TERRAIN_PSYCHIC)) && SetHasMove(right, MOVE_EXPANDING_FORCE))
             || ((rightField.terrain & (1u << B_TERRAIN_PSYCHIC)) && SetHasMove(left, MOVE_EXPANDING_FORCE)))
                score += 6;
            // Retain a healthy last-party disguise for Illusion users.
            if (leftAbility == ABILITY_ILLUSION || rightAbility == ABILITY_ILLUSION)
                continue;
            if (SetHasMove(left, MOVE_TRICK_ROOM) && gSpeciesInfo[speciesB].baseSpeed <= 65)
                score += 5;
            if (SetHasMove(right, MOVE_TRICK_ROOM) && gSpeciesInfo[speciesA].baseSpeed <= 65)
                score += 5;
            if ((leftAbility == ABILITY_COMMANDER && speciesB == SPECIES_DONDOZO)
             || (rightAbility == ABILITY_COMMANDER && speciesA == SPECIES_DONDOZO))
                score += 12;
            if (!foundLead || score > bestScore)
            {
                foundLead = TRUE;
                bestScore = score;
                bestA = a;
                bestB = b;
            }
        }
    }
    // Opponent party creation reverses the selected order.
    struct CircuitGeneratedSet swap = team->sets[PARTY_SIZE - 1];
    team->sets[PARTY_SIZE - 1] = team->sets[bestA];
    team->sets[bestA] = swap;
    if (bestB == PARTY_SIZE - 1)
        bestB = bestA;
    swap = team->sets[PARTY_SIZE - 2];
    team->sets[PARTY_SIZE - 2] = team->sets[bestB];
    team->sets[bestB] = swap;
    if (team->sets[0].ability == ABILITY_ILLUSION)
    {
        swap = team->sets[0];
        team->sets[0] = team->sets[1];
        team->sets[1] = swap;
    }
}

static void AddSetToTeamState(struct CircuitTeamState *team, struct CircuitGeneratedSet *set)
{
    const struct ShowdownCircuitVariant *variant = &gShowdownCircuitVariants[set->variantIndex];
    enum Species species = variant->formSpecies;
    enum Type type1 = gSpeciesInfo[species].types[0];
    enum Type type2 = gSpeciesInfo[species].types[1];

    team->sets[team->count++] = *set;
    if (type1 < NUMBER_OF_MON_TYPES)
        team->typeCounts[type1]++;
    if (type2 != type1 && type2 < NUMBER_OF_MON_TYPES)
        team->typeCounts[type2]++;
    for (enum Type type = TYPE_NORMAL; type < NUMBER_OF_MON_TYPES; type++)
    {
        uq4_12_t modifier = GetSpeciesTypeModifier(type, species);
        if (modifier > UQ_4_12(1.0))
            team->weaknessCounts[type]++;
        if (modifier > UQ_4_12(2.0))
            team->doubleWeaknessCounts[type]++;
    }
    if (IsFreezeDryWeak(species))
        team->freezeDryWeakCount++;
    team->compatibilityFlags |= variant->compatibilityFlags;
    team->hasMega |= IsMegaVariant(variant);
    UpdateTeamDetails(&team->details, set);
}

static bool32 GenerateShowdownTeam(struct CircuitTeamState *team)
{
    memset(sExhaustedBaseDex, 0, sizeof(sExhaustedBaseDex));
    memset(team, 0, sizeof(*team));

    while (team->count < CIRCUIT_TEAM_SIZE)
    {
        enum NationalDexOrder dex = NATIONAL_DEX_NONE;
        u16 variantIndex = 0;
        const struct ShowdownCircuitVariant *variant;
        const struct ShowdownCircuitTemplate *template;
        struct CircuitGeneratedSet set = {0};
        u16 templateIndex;

        if (!ChooseBaseDex(&dex)
         || !ChooseVariantForDex(dex, team->hasMega, &variantIndex))
            break;
        variant = &gShowdownCircuitVariants[variantIndex];
        if (dex == SpeciesToNationalPokedexNum(SPECIES_ZOROARK) && team->count < 1)
            continue;
        if (!CandidateAllowed(team, variant))
            continue;

        templateIndex = variant->templateOffset
                      + CircuitRandomUniform(0, variant->templateCount - 1);
        template = &gShowdownCircuitTemplates[templateIndex];
        set.variantIndex = variantIndex;
        set.dependency = template->dependency;
        if (template->authored)
        {
            memcpy(set.moves, template->moves, sizeof(set.moves));
            set.ability = template->abilities[0];
            set.item = template->item;
            set.nature = template->nature;
            memcpy(set.evs, template->evs, sizeof(set.evs));
        }
        else
        {
            BuildShowdownMoveset(&set, template, variant, &team->details);
            if (SetMoveCount(&set) < min(MAX_MON_MOVES, template->moveCount))
                continue;
            set.ability = ChooseShowdownAbility(&set, template, variant, &team->details);
            set.item = ChooseShowdownItem(&set, template, variant);
            SetShowdownEvs(&set, template);
        }
        AddSetToTeamState(team, &set);
    }
    if (team->count != CIRCUIT_TEAM_SIZE || !TeamIsCoherent(team))
        return FALSE;
    ChooseCoherentLeads(team);
    return TRUE;
}

static bool32 FindAbilitySlot(enum Species species, enum Ability ability, u32 *slot)
{
    for (u32 i = 0; i < NUM_ABILITY_SLOTS; i++)
    {
        if (gSpeciesInfo[species].abilities[i] == ability)
        {
            *slot = i;
            return TRUE;
        }
    }
    return FALSE;
}

static void CreateCircuitMon(struct Pokemon *mon, const struct CircuitGeneratedSet *set, u8 level)
{
    const struct ShowdownCircuitVariant *variant = &gShowdownCircuitVariants[set->variantIndex];
    u8 ppBonuses = 0;
    u8 iv = MAX_PER_STAT_IVS;
    u32 abilitySlot = 0;
    u8 nature = set->nature;

    // Never index a level-100 experience table with an overlevel opponent.
    CreateMon(mon, variant->partySpecies, min(level, MAX_LEVEL), Random32(), OTID_STRUCT_RANDOM_NO_SHINY);
    // Circuit teams are trainer-owned and never read the Inclement layer.
    SetMonTrainerOwned(mon, TRUE);
    SetMonData(mon, MON_DATA_LEVEL, &level);
    for (u32 stat = 0; stat < NUM_STATS; stat++)
        SetMonData(mon, MON_DATA_HP_IV + stat, &iv);
    SetMonData(mon, MON_DATA_PP_BONUSES, &ppBonuses);
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        SetMonMoveSlot(mon, set->moves[i], i);
    SetMonData(mon, MON_DATA_HIDDEN_NATURE, &nature);
    if (!FindAbilitySlot(variant->partySpecies, set->ability, &abilitySlot))
    {
        assertf(FALSE, "Circuit requested illegal Ability %u for species %u", set->ability, variant->partySpecies);
        abilitySlot = 0;
    }
    SetMonData(mon, MON_DATA_ABILITY_NUM, &abilitySlot);
    SetMonData(mon, MON_DATA_HP_EV, &set->evs[STAT_HP]);
    SetMonData(mon, MON_DATA_ATK_EV, &set->evs[STAT_ATK]);
    SetMonData(mon, MON_DATA_DEF_EV, &set->evs[STAT_DEF]);
    SetMonData(mon, MON_DATA_SPEED_EV, &set->evs[STAT_SPEED]);
    SetMonData(mon, MON_DATA_SPATK_EV, &set->evs[STAT_SPATK]);
    SetMonData(mon, MON_DATA_SPDEF_EV, &set->evs[STAT_SPDEF]);
    SetMonData(mon, MON_DATA_HELD_ITEM, &set->item);
    CalculateMonStats(mon);
}

static void NormalizeCircuitPlayerParty(u8 level)
{
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        enum Species species = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG);
        u32 exp;

        if (species == SPECIES_NONE || species == SPECIES_EGG)
            continue;
        u8 individualLevel = GetLevelCapForSpecies(species, level);
        exp = gExperienceTables[gSpeciesInfo[species].growthRate][individualLevel];
        SetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_EXP, &exp);
        SetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_LEVEL, &individualLevel);
        CalculateMonStats(&gParties[B_TRAINER_PLAYER][i]);
    }
    HealPlayerParty();
}

// VAR_RESULT: CIRCUIT_ENTRY_OK (TRUE), CIRCUIT_ENTRY_NEEDS_SIX (FALSE) or
// CIRCUIT_ENTRY_PARTY_RULE. The desk script reads the numeric values.
void ChampionsCircuitCanEnter(void)
{
    gSpecialVar_Result = CIRCUIT_ENTRY_OK;
    if (CalculatePlayerPartyCount() != PARTY_SIZE)
    {
        gSpecialVar_Result = CIRCUIT_ENTRY_NEEDS_SIX;
        return;
    }
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG) == SPECIES_EGG
         || GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SANITY_IS_BAD_EGG)
         || !IsSpeciesEnabled(GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES))
         || GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HP) == 0)
        {
            gSpecialVar_Result = CIRCUIT_ENTRY_NEEDS_SIX;
            return;
        }
    }
    // The campaign's party rule holds here too: one Legendary or Mythical,
    // one Ultra Beast and one Paradox Pokemon at most.
    if (!PlayerPartyWithinRestrictedLimit())
        gSpecialVar_Result = CIRCUIT_ENTRY_PARTY_RULE;
}

// The desk opens with the Frontier, after the Hall of Fame, when the level cap
// is already CIRCUIT_BASE_LEVEL (Lv. 100): the whole party battles at the cap.
void ChampionsCircuitBegin(void)
{
    SavePlayerParty();
    sCircuitSavedLvlMode = gSaveBlock2Ptr->frontier.lvlMode;
    gSaveBlock2Ptr->frontier.lvlMode = FRONTIER_LVL_OPEN;
    VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, 0);
    VarSet(VAR_CHAMPIONS_CIRCUIT_ACTIVE, TRUE);
    NormalizeCircuitPlayerParty(CIRCUIT_BASE_LEVEL);
}

static bool32 GenerateCompetitionOpponent(u16 wins, u8 fixedLevel)
{
    struct CircuitTeamState team;
    bool32 generated = FALSE;

    for (u32 attempt = 0; attempt < 64 && !generated; attempt++)
        generated = GenerateShowdownTeam(&team);
    if (!generated)
    {
        gSpecialVar_Result = 0;
        return FALSE;
    }

    ZeroEnemyPartyMons();
    // Showdown builds by unshifting each selection; reversing generation order
    // preserves its lead/Illusion convention.
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
    {
        u8 level = fixedLevel ? fixedLevel : GetChampionsCircuitOpponentLevel(wins, slot);
        CreateCircuitMon(&gParties[B_TRAINER_OPPONENT_A][slot], &team.sets[PARTY_SIZE - 1 - slot], level);
    }
    CalculateEnemyPartyCount();
    if (team.details.rain)
        StringCopy(gStringVar1, sCircuitStyleRain);
    else if (team.details.sun)
        StringCopy(gStringVar1, sCircuitStyleSun);
    else if (team.details.sand)
        StringCopy(gStringVar1, sCircuitStyleSand);
    else if (team.details.snow)
        StringCopy(gStringVar1, sCircuitStyleSnow);
    else if (TeamHasMove(&team, MOVE_TRICK_ROOM))
        StringCopy(gStringVar1, sCircuitStyleTrickRoom);
    else
        StringCopy(gStringVar1, sCircuitStyleShowdown);
    ConvertIntToDecimalStringN(gStringVar2, (u32)wins + 1, STR_CONV_MODE_LEFT_ALIGN, 5);
    ConvertIntToDecimalStringN(gStringVar3, fixedLevel ? fixedLevel : GetChampionsCircuitOpponentLevel(wins, 0), STR_CONV_MODE_LEFT_ALIGN, 3);
    gSpecialVar_Result = PARTY_SIZE;
    return TRUE;
}

void ChampionsCircuitGenerateOpponent(void)
{
    GenerateCompetitionOpponent(VarGet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS), 0);
}

bool32 CreateChampionsExhibitionParty(u8 level)
{
    return GenerateCompetitionOpponent(0, level);
}

bool32 IsChampionsTentBattle(void)
{
    return sTentActive && gMain.inBattle;
}

void ChampionsTentCanEnter(void)
{
    ChampionsCircuitCanEnter();
}

void ChampionsTentBegin(void)
{
    SavePlayerParty();
    sTentId = gSpecialVar_0x8004;
    sTentWins = 0;
    sTentCap = min(MAX_LEVEL, GetCurrentLevelCap());
    sTentActive = TRUE;
    NormalizeCircuitPlayerParty(sTentCap);
}

void ChampionsTentGenerateOpponent(void)
{
    GenerateCompetitionOpponent(sTentWins, max(1, sTentCap + 2 - GetTrainerLevelReduction()));
}

void ChampionsTentHandleBattleResult(void)
{
    gSpecialVar_Result = 0;
    if (!sTentActive || gBattleOutcome != B_OUTCOME_WON)
        return;
    HealPlayerParty();
    sTentWins++;
    if (sTentWins == 3)
    {
        SetChampionsTentPrize(sTentId);
        gSpecialVar_Result = 2;
    }
    else
    {
        gSpecialVar_Result = 1;
    }
}

void ChampionsTentEnd(void)
{
    if (sTentActive)
    {
        LoadPlayerParty();
        CalculatePlayerPartyCount();
        sTentActive = FALSE;
    }
    gSpecialVar_0x8004 = sTentId;
}

// Record board beside the desk: STR_VAR_1 best streak ("--" before the first
// win), STR_VAR_2 lifetime wins. A run lives inside one desk conversation, so
// there is never a current streak to show from the lobby.
void ChampionsCircuitBufferRecord(void)
{
    u16 best = VarGet(VAR_EC_CIRCUIT_BEST_WINS);
    if (best != 0)
        ConvertIntToDecimalStringN(gStringVar1, best, STR_CONV_MODE_LEFT_ALIGN, 5);
    else
        StringCopy(gStringVar1, sCircuitRecordUnrecorded);
    ConvertIntToDecimalStringN(gStringVar2, VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS), STR_CONV_MODE_LEFT_ALIGN, 5);
}

// The Circuit's only prize is Battle Points for the Exchange Service Corner; it
// never gives Pokemon. The native Frontier facilities keep their own
// frontier_givepoints awards. The base award grows with the current streak and
// every tenth lifetime win pays a milestone bonus.
#define CIRCUIT_BP_BASE            5
#define CIRCUIT_BP_STREAK_MAX      15
#define CIRCUIT_BP_MILESTONE_EVERY 10
#define CIRCUIT_BP_MILESTONE_BONUS 20

static u16 AwardCircuitBattlePoints(u16 streakBeforeWin, u16 totalAfterWin)
{
    u32 points = CIRCUIT_BP_BASE + min(streakBeforeWin, CIRCUIT_BP_STREAK_MAX);

    if (totalAfterWin != 0 && totalAfterWin % CIRCUIT_BP_MILESTONE_EVERY == 0)
        points += CIRCUIT_BP_MILESTONE_BONUS;

    gSaveBlock2Ptr->frontier.battlePoints += points;
    if (gSaveBlock2Ptr->frontier.battlePoints > MAX_BATTLE_FRONTIER_POINTS)
        gSaveBlock2Ptr->frontier.battlePoints = MAX_BATTLE_FRONTIER_POINTS;
    gSaveBlock2Ptr->frontier.cardBattlePoints += points;
    if (gSaveBlock2Ptr->frontier.cardBattlePoints > MAX_BATTLE_FRONTIER_POINTS)
        gSaveBlock2Ptr->frontier.cardBattlePoints = MAX_BATTLE_FRONTIER_POINTS;
    IncrementDailyBattlePoints(points);
    return points;
}

void ChampionsCircuitHandleBattleResult(void)
{
    gSpecialVar_Result = FALSE;
    if (gBattleOutcome == B_OUTCOME_WON)
    {
        u16 wins = VarGet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS);
        u16 total = VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS);
        u16 points;

        if (wins != 0xFFFF)
            VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, wins + 1);
        VarSet(VAR_EC_CIRCUIT_BEST_WINS, max(VarGet(VAR_EC_CIRCUIT_BEST_WINS), VarGet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS)));
        if (total != 0xFFFF)
            VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, total + 1);
        points = AwardCircuitBattlePoints(wins, min((u32)total + 1, 0xFFFF));
        // STR_VAR_1 feeds the native "obtained X Battle Point(s)" line;
        // STR_VAR_2 is the new streak.
        ConvertIntToDecimalStringN(gStringVar1, points, STR_CONV_MODE_LEFT_ALIGN, 2);
        ConvertIntToDecimalStringN(gStringVar2, VarGet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS), STR_CONV_MODE_LEFT_ALIGN, 5);
        ConvertIntToDecimalStringN(gStringVar3, points, STR_CONV_MODE_LEFT_ALIGN, 2);
        HealPlayerParty();
        gSpecialVar_Result = TRUE;
    }
    else
    {
        ChampionsCircuitEnd();
    }
}

// The Circuit no longer grants Pokemon: its former legendary rewards live in
// the campaign's wild tables and statics. Scripts read 0 as "nothing to claim".
void ChampionsCircuitTryGiveReward(void)
{
    gSpecialVar_Result = 0;
}

void ChampionsCircuitEnd(void)
{
    ConvertIntToDecimalStringN(gStringVar2, VarGet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS), STR_CONV_MODE_LEFT_ALIGN, 5);
    if (VarGet(VAR_CHAMPIONS_CIRCUIT_ACTIVE))
    {
        LoadPlayerParty();
        CalculatePlayerPartyCount();
        HealPlayerParty();
        gSaveBlock2Ptr->frontier.lvlMode = sCircuitSavedLvlMode;
    }
    VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, 0);
    VarSet(VAR_CHAMPIONS_CIRCUIT_ACTIVE, FALSE);
}
