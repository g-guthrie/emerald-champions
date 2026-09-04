// Champions random-doubles generation ported from Pokemon Showdown commit
// bb179fbf8449e3c31632bd56f671ffb4404fa6e7. See THIRD_PARTY_NOTICES.md.
#include "global.h"
#include "battle.h"
#include "battle_frontier.h"
#include "battle_util.h"
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
#define CIRCUIT_BASE_LEVEL 80
#define CIRCUIT_REWARD_INTERVAL 2
#define CIRCUIT_MASTERY_WINS 40

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
    u8 spikes;
};

struct CircuitGeneratedSet
{
    u16 variantIndex;
    enum Move moves[MAX_MON_MOVES];
    enum Ability ability;
    enum Item item;
    u8 statPoints[NUM_STATS];
    u8 nature;
    bool8 zeroAttackIv;
    bool8 zeroSpeedIv;
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
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        if (set->moves[i] == move)
            return TRUE;
    return FALSE;
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
    for (u32 i = 0; i < pool->count; i++)
        if (pool->moves[i] == move)
            return TRUE;
    return FALSE;
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

static bool32 AddRandomStabMove(
    struct CircuitGeneratedSet *set,
    struct CircuitMovePool *pool,
    enum Type wantedType,
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
         && GetTemplateMoveType(move, template, variant) == wantedType
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

static bool32 AddRandomDamagingMove(
    struct CircuitGeneratedSet *set,
    struct CircuitMovePool *pool,
    enum Type excludedType,
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
         && (excludedType == TYPE_NONE || GetTemplateMoveType(move, template, variant) != excludedType)
         && CircuitRandomUniform(0, ++matches - 1) == 0)
            selected = move;
    }
    return selected != MOVE_NONE && AddMove(set, pool, selected, template, variant);
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
        AddRandomStabMove(set, &pool, type1, template, variant);
    if (type2 != type1 && !SetHasDamagingType(set, type2, template, variant))
        AddRandomStabMove(set, &pool, type2, template, variant);
    if (template->preferredType != TYPE_NONE && !SetHasDamagingType(set, template->preferredType, template, variant))
        AddRandomStabMove(set, &pool, template->preferredType, template, variant);
    if (!SetHasDamagingMove(set))
    {
        if (!AddRandomStabMove(set, &pool, type1, template, variant) && type2 != type1)
            AddRandomStabMove(set, &pool, type2, template, variant);
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
        AddRandomDamagingMove(set, &pool, TYPE_NONE, template, variant);
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
            AddRandomDamagingMove(set, &pool, onlyType, template, variant);
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

    // The Showdown data always has at least four moves before culling. This
    // last fallback preserves a legal four-move set if interacting exclusions
    // emptied the compact pool more aggressively than the TypeScript oracle.
    for (u32 i = 0; SetMoveCount(set) < MAX_MON_MOVES && i < template->moveCount; i++)
    {
        enum Move move = template->moves[i];
        if (!SetHasMove(set, move))
            set->moves[SetMoveCount(set)] = move;
    }
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
    if (template->role == SHOWDOWN_ROLE_CHOICE_ITEM)
        return ITEM_CHOICE_SCARF;
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
        return ITEM_CHOICE_SCARF;
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

static void SetShowdownStatPoints(
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
        set->statPoints[i] = 0;
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
        set->statPoints[STAT_HP] = 32;
        if (template->role == SHOWDOWN_ROLE_SUPPORT && !slow)
        {
            set->statPoints[STAT_DEF] = 2;
            set->statPoints[STAT_SPEED] = 32;
            set->nature = NATURE_TIMID;
        }
        else
        {
            set->statPoints[STAT_DEF] = 17;
            set->statPoints[STAT_SPDEF] = 17;
            set->nature = slow ? NATURE_SASSY : NATURE_CALM;
        }
    }
    else if (physicalDamage && specialDamage)
    {
        if (slow || bulky)
        {
            set->statPoints[STAT_HP] = 32;
            set->statPoints[STAT_ATK] = 16;
            set->statPoints[STAT_SPATK] = 16;
            set->statPoints[STAT_DEF] = 2;
            set->nature = slow ? NATURE_QUIET : NATURE_RASH;
        }
        else
        {
            set->statPoints[STAT_ATK] = 22;
            set->statPoints[STAT_SPATK] = 22;
            set->statPoints[STAT_SPEED] = 22;
            set->nature = NATURE_NAIVE;
        }
    }
    else if (physicalDamage)
    {
        set->statPoints[STAT_ATK] = 32;
        if (slow || bulky)
        {
            set->statPoints[STAT_HP] = 32;
            set->statPoints[STAT_DEF] = 2;
            set->nature = slow ? NATURE_BRAVE : NATURE_ADAMANT;
        }
        else
        {
            set->statPoints[STAT_HP] = 2;
            set->statPoints[STAT_SPEED] = 32;
            set->nature = NATURE_JOLLY;
        }
    }
    else
    {
        set->statPoints[STAT_SPATK] = 32;
        if (slow || bulky)
        {
            set->statPoints[STAT_HP] = 32;
            set->statPoints[STAT_SPDEF] = 2;
            set->nature = slow ? NATURE_QUIET : NATURE_MODEST;
        }
        else
        {
            set->statPoints[STAT_HP] = 2;
            set->statPoints[STAT_SPEED] = 32;
            set->nature = NATURE_TIMID;
        }
    }

    if (!physicalDamage && !SetHasMove(set, MOVE_TRANSFORM))
        set->zeroAttackIv = TRUE;
    if (slow)
    {
        set->zeroSpeedIv = TRUE;
    }
}

static bool32 IsBaseDexExhausted(enum NationalDexOrder dex)
{
    return dex <= NATIONAL_DEX_COUNT && sExhaustedBaseDex[dex];
}

static bool32 VariantAllowedByMegaState(const struct ShowdownCircuitVariant *variant, bool32 hasMega, bool32 groupHasMega)
{
    if (hasMega)
        return variant->requiredItem == ITEM_NONE;
    if (groupHasMega)
        return variant->requiredItem != ITEM_NONE;
    return TRUE;
}

static bool32 GroupHasMega(enum NationalDexOrder dex)
{
    for (u32 i = 0; i < SHOWDOWN_CIRCUIT_VARIANT_COUNT; i++)
        if (SpeciesToNationalPokedexNum(gShowdownCircuitVariants[i].partySpecies) == dex
         && gShowdownCircuitVariants[i].requiredItem != ITEM_NONE)
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

static bool32 CandidateAllowed(const struct CircuitTeamState *team, const struct ShowdownCircuitVariant *variant, bool32 strict)
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
    if (!strict)
        return TRUE;
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

static void UpdateTeamDetails(struct CircuitTeamDetails *details, const struct CircuitGeneratedSet *set)
{
    details->rain |= set->ability == ABILITY_DRIZZLE || SetHasMove(set, MOVE_RAIN_DANCE);
    details->sun |= set->ability == ABILITY_DROUGHT || SetHasMove(set, MOVE_SUNNY_DAY);
    details->sand |= set->ability == ABILITY_SAND_STREAM;
    details->snow |= set->ability == ABILITY_SNOW_WARNING || SetHasMove(set, MOVE_SNOWSCAPE) || SetHasMove(set, MOVE_CHILLY_RECEPTION);
    details->statusCure |= SetHasMove(set, MOVE_HEAL_BELL);
    if (SetHasMove(set, MOVE_SPIKES) || SetHasMove(set, MOVE_CEASELESS_EDGE))
        details->spikes++;
    details->toxicSpikes |= SetHasMove(set, MOVE_TOXIC_SPIKES) || set->ability == ABILITY_TOXIC_DEBRIS;
    details->stealthRock |= SetHasMove(set, MOVE_STEALTH_ROCK) || SetHasMove(set, MOVE_STONE_AXE);
    details->stickyWeb |= SetHasMove(set, MOVE_STICKY_WEB);
    details->defog |= SetHasMove(set, MOVE_DEFOG);
    details->rapidSpin |= SetHasMove(set, MOVE_RAPID_SPIN) || SetHasMove(set, MOVE_MORTAL_SPIN);
    details->screens |= SetHasMove(set, MOVE_AURORA_VEIL);
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
    team->hasMega |= variant->requiredItem != ITEM_NONE;
    UpdateTeamDetails(&team->details, set);
}

static bool32 GenerateShowdownTeam(struct CircuitTeamState *team, bool32 strict)
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
        if (!CandidateAllowed(team, variant, strict))
            continue;

        templateIndex = variant->templateOffset
                      + CircuitRandomUniform(0, variant->templateCount - 1);
        template = &gShowdownCircuitTemplates[templateIndex];
        set.variantIndex = variantIndex;
        BuildShowdownMoveset(&set, template, variant, &team->details);
        set.ability = ChooseShowdownAbility(&set, template, variant, &team->details);
        set.item = ChooseShowdownItem(&set, template, variant);
        SetShowdownStatPoints(&set, template);
        AddSetToTeamState(team, &set);
    }
    return team->count == CIRCUIT_TEAM_SIZE;
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

    CreateMon(mon, variant->partySpecies, level, Random32(), OTID_STRUCT_RANDOM_NO_SHINY);
    for (u32 stat = 0; stat < NUM_STATS; stat++)
        SetMonData(mon, MON_DATA_HP_IV + stat, &iv);
    if (set->zeroAttackIv)
    {
        iv = 0;
        SetMonData(mon, MON_DATA_ATK_IV, &iv);
        iv = MAX_PER_STAT_IVS;
    }
    if (set->zeroSpeedIv)
    {
        iv = 0;
        SetMonData(mon, MON_DATA_SPEED_IV, &iv);
    }
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
    SetMonData(mon, MON_DATA_HP_EV, &set->statPoints[STAT_HP]);
    SetMonData(mon, MON_DATA_ATK_EV, &set->statPoints[STAT_ATK]);
    SetMonData(mon, MON_DATA_DEF_EV, &set->statPoints[STAT_DEF]);
    SetMonData(mon, MON_DATA_SPEED_EV, &set->statPoints[STAT_SPEED]);
    SetMonData(mon, MON_DATA_SPATK_EV, &set->statPoints[STAT_SPATK]);
    SetMonData(mon, MON_DATA_SPDEF_EV, &set->statPoints[STAT_SPDEF]);
    SetMonData(mon, MON_DATA_HELD_ITEM, &set->item);
    CalculateMonStats(mon);
}

static void NormalizeCircuitPlayerParty(void)
{
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        enum Species species = GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG);
        u8 level = CIRCUIT_BASE_LEVEL;
        u32 exp;

        if (species == SPECIES_NONE || species == SPECIES_EGG)
            continue;
        exp = gExperienceTables[gSpeciesInfo[species].growthRate][level];
        SetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_EXP, &exp);
        SetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_LEVEL, &level);
        CalculateMonStats(&gParties[B_TRAINER_PLAYER][i]);
    }
    HealPlayerParty();
}

void ChampionsCircuitCanEnter(void)
{
    gSpecialVar_Result = TRUE;
    if (CalculatePlayerPartyCount() != PARTY_SIZE)
    {
        gSpecialVar_Result = FALSE;
        return;
    }
    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        if (GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_SPECIES_OR_EGG) == SPECIES_EGG
         || GetMonData(&gParties[B_TRAINER_PLAYER][i], MON_DATA_HP) == 0)
        {
            gSpecialVar_Result = FALSE;
            return;
        }
    }
}

void ChampionsCircuitBegin(void)
{
    SavePlayerParty();
    VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, 0);
    VarSet(VAR_CHAMPIONS_CIRCUIT_ACTIVE, TRUE);
    NormalizeCircuitPlayerParty();
}

void ChampionsCircuitGenerateOpponent(void)
{
    struct CircuitTeamState team;
    u16 wins = VarGet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS);
    u8 baseLevel = min(MAX_LEVEL, CIRCUIT_BASE_LEVEL + wins / PARTY_SIZE);
    u8 boostedSlots = wins % PARTY_SIZE;
    bool32 generated = FALSE;

    for (u32 attempt = 0; attempt < 8 && !generated; attempt++)
        generated = GenerateShowdownTeam(&team, TRUE);
    for (u32 attempt = 0; attempt < 4 && !generated; attempt++)
        generated = GenerateShowdownTeam(&team, FALSE);
    if (!generated)
    {
        gSpecialVar_Result = 0;
        return;
    }

    ZeroEnemyPartyMons();
    // Showdown builds by unshifting each selection; reversing generation order
    // preserves its lead/Illusion convention.
    for (u32 slot = 0; slot < PARTY_SIZE; slot++)
    {
        u8 level = baseLevel;
        if (slot < boostedSlots && level < MAX_LEVEL)
            level++;
        CreateCircuitMon(&gParties[B_TRAINER_OPPONENT_A][slot], &team.sets[PARTY_SIZE - 1 - slot], level);
    }
    ApplyTrainerLevelDifficulty(&gParties[B_TRAINER_OPPONENT_A][0]);
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
    ConvertIntToDecimalStringN(gStringVar2, wins + 1, STR_CONV_MODE_LEFT_ALIGN, 3);
    gSpecialVar_Result = PARTY_SIZE;
}

// Every Circuit victory funds the Battle Point exchange. The old Frontier
// facilities' frontier_givepoints paths are unreachable because every
// challenge desk now leads here, so this is the only repeatable BP source.
// Base award grows with the current streak and every tenth lifetime win pays
// a milestone bonus.
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
        if (total != 0xFFFF)
            VarSet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS, total + 1);
        points = AwardCircuitBattlePoints(wins, total + 1);
        ConvertIntToDecimalStringN(gStringVar3, points, STR_CONV_MODE_LEFT_ALIGN, 2);
        HealPlayerParty();
        gSpecialVar_Result = TRUE;
    }
    else
    {
        ChampionsCircuitEnd();
    }
}

void ChampionsCircuitTryGiveReward(void)
{
    // Reward entitlement is lifetime Circuit progress, not transient streak
    // state.  A full PC can therefore delay delivery without making the player
    // repeat the same milestone after retiring to create room.
    u16 wins = VarGet(VAR_CHAMPIONS_CIRCUIT_TOTAL_WINS);
    u8 rewardIndex = 0;

    gSpecialVar_Result = 0;
    for (enum LegendarySignId signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
    {
        const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[signId];
        u8 giveResult;

        if (sign->source != LEGENDARY_SOURCE_CIRCUIT)
            continue;
        rewardIndex++;
        if (wins < rewardIndex * CIRCUIT_REWARD_INTERVAL || IsLegendarySignCaught(signId))
            continue;
        giveResult = GiveLegendarySignReward(sign->species, CIRCUIT_BASE_LEVEL);
        if (giveResult == MON_CANT_GIVE)
        {
            gSpecialVar_Result = 3;
            return;
        }
        StringCopy(gStringVar1, GetSpeciesName(sign->species));
        gSpecialVar_Result = giveResult == MON_GIVEN_TO_PARTY ? 1 : 2;
        return;
    }

    if (wins >= CIRCUIT_MASTERY_WINS && !IsLegendarySignCaught(LEGENDARY_SIGN_ETERNATUS))
    {
        u8 giveResult;

        for (enum LegendarySignId signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
            if (gLegendarySignDefinitions[signId].source == LEGENDARY_SOURCE_CIRCUIT && !IsLegendarySignCaught(signId))
                return;
        giveResult = GiveLegendarySignReward(SPECIES_ETERNATUS, CIRCUIT_BASE_LEVEL);
        if (giveResult == MON_CANT_GIVE)
        {
            gSpecialVar_Result = 3;
            return;
        }
        StringCopy(gStringVar1, GetSpeciesName(SPECIES_ETERNATUS));
        gSpecialVar_Result = giveResult == MON_GIVEN_TO_PARTY ? 1 : 2;
    }
}

void ChampionsCircuitEnd(void)
{
    if (VarGet(VAR_CHAMPIONS_CIRCUIT_ACTIVE))
    {
        LoadPlayerParty();
        CalculatePlayerPartyCount();
        HealPlayerParty();
    }
    VarSet(VAR_CHAMPIONS_CIRCUIT_CURRENT_WINS, 0);
    VarSet(VAR_CHAMPIONS_CIRCUIT_ACTIVE, FALSE);
}
