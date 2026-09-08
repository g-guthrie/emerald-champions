#include "global.h"
#include "battle_z_move.h"
#include "malloc.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_ai_field_statuses.h"
#include "battle_ai_util.h"
#include "battle_ai_main.h"
#include "battle_util.h"
#include "battle_factory.h"
#include "battle_setup.h"
#include "event_data.h"
#include "data.h"
#include "item.h"
#include "move.h"
#include "pokemon.h"
#include "random.h"
#include "recorded_battle.h"
#include "util.h"
#include "constants/abilities.h"
#include "constants/battle_ai.h"
#include "constants/battle_move_effects.h"
#include "constants/moves.h"
#include "constants/items.h"

static bool32 DoesAbilityBenefitFromWeather(enum Ability ability, u32 weather);
static bool32 DoesAbilityBenefitFromTerrain(enum Ability ability, enum BattleTerrain terrain);
// A move is light sensitive if it is boosted by Sunny Day and weakened by low light weathers.
static bool32 IsLightSensitiveMove(enum Move move);
static bool32 HasLightSensitiveMove(enum BattlerId battler);
// The following functions all feed into WeatherChecker, which is then called by ShouldSetWeather and ShouldClearWeather.
// BenefitsFrom functions all return FIELD_EFFECT_POSITIVE if the weather or field effect is good to have in place from the perspective of the battler, FIELD_EFFECT_NEUTRAL if it is neither good nor bad, and FIELD_EFFECT_NEGATIVE if it is bad.
// The purpose of WeatherChecker and FieldStatusChecker is to cleanly homogenize the logic that's the same with all of them, and to more easily apply single battle logic to double battles.
// ShouldSetWeather and ShouldClearWeather are looking for a positive or negative result respectively, and check the entire side.
// If one Pokémon has a positive result and the other has a negative result, it defaults to the opinion of the battler that may change the weather or field status.
static enum FieldEffectOutcome BenefitsFromSun(enum BattlerId battler);
static enum FieldEffectOutcome BenefitsFromSandstorm(enum BattlerId battler);
static enum FieldEffectOutcome BenefitsFromHailOrSnow(enum BattlerId battler, u32 weather);
static enum FieldEffectOutcome BenefitsFromRain(enum BattlerId battler);
// The following functions all feed into FieldStatusChecker, which is then called by ShouldSetFieldStatus and ShouldClearFieldStatus.
// They work approximately the same as the weather functions.
static enum FieldEffectOutcome BenefitsFromElectricTerrain(enum BattlerId battler);
static enum FieldEffectOutcome BenefitsFromGrassyTerrain(enum BattlerId battler);
static enum FieldEffectOutcome BenefitsFromMistyTerrain(enum BattlerId battler);
static enum FieldEffectOutcome BenefitsFromPsychicTerrain(enum BattlerId battler);
static enum FieldEffectOutcome BenefitsFromGravity(enum BattlerId battler);
static enum FieldEffectOutcome BenefitsFromTrickRoom(enum BattlerId battler);

static bool32 HasUsableMoveWithEffect(enum BattlerId battler, enum BattleMoveEffects effect)
{
    enum Move *moves = GetMovesArray(battler);

    if (!IsBattlerAlive(battler))
        return FALSE;
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        if (!IsMoveUnusable(i, moves[i], gAiLogicData->moveLimitations[battler])
         && GetMoveEffect(moves[i]) == effect)
            return TRUE;
    return FALSE;
}

static bool32 HasUsableSleepMoveAgainst(enum BattlerId battler, enum BattlerId target)
{
    enum Move *moves = GetMovesArray(battler);
    enum Ability targetAbility = gAiLogicData->abilities[target];

    // This is the sleep value lost to the proposed terrain, including when
    // considering clearing existing Electric Terrain. Do not query the old
    // terrain's sleep prohibition and mistake it for permanent immunity.
    if (!IsBattlerAlive(battler) || !IsBattlerAlive(target)
     || !AI_IsBattlerGrounded(target) || gBattleMons[target].status1 & STATUS1_ANY
     || IsBattlerIncapacitated(battler, gAiLogicData->abilities[battler])
     || IsSleepClauseActiveForSide(GetBattlerSide(target))
     || targetAbility == ABILITY_INSOMNIA || targetAbility == ABILITY_VITAL_SPIRIT
     || targetAbility == ABILITY_COMATOSE || targetAbility == ABILITY_PURIFYING_SALT
     || AI_IsAbilityOnSide(target, ABILITY_SWEET_VEIL))
        return FALSE;

    for (u32 i = 0; i < MAX_MON_MOVES; i++)
    {
        enum Move move = moves[i];
        if (IsMoveUnusable(i, move, gAiLogicData->moveLimitations[battler])
         || (GetMoveNonVolatileStatus(move) != MOVE_EFFECT_SLEEP
             && !MoveHasAdditionalEffect(move, MOVE_EFFECT_SLEEP))
         || DoesSubstituteBlockMove(battler, target, move)
         || (IsPowderMove(move) && !IsAffectedByPowderMove(target, targetAbility, gAiLogicData->holdEffects[target])))
            continue;
        return TRUE;
    }
    return FALSE;
}

static bool32 HasUsablePriorityAgainst(enum BattlerId battler, enum BattlerId target)
{
    enum Move *moves = GetMovesArray(battler);
    enum Ability ability = gAiLogicData->abilities[battler];

    if (!IsBattlerAlive(battler) || !IsBattlerAlive(target)
     || !AI_IsBattlerGrounded(target)
     || IsBattlerIncapacitated(battler, ability))
        return FALSE;

    for (u32 i = 0; i < MAX_MON_MOVES; i++)
    {
        enum Move move = moves[i];
        enum MoveTarget moveTarget;
        enum BattleMoveEffects effect;
        s32 priority;

        if (IsMoveUnusable(i, move, gAiLogicData->moveLimitations[battler]))
            continue;
        moveTarget = AI_GetBattlerMoveTargetType(battler, move);
        effect = GetMoveEffect(move);
        if (moveTarget != TARGET_SELECTED && moveTarget != TARGET_SMART
         && moveTarget != TARGET_OPPONENT && moveTarget != TARGET_RANDOM
         && moveTarget != TARGET_BOTH && moveTarget != TARGET_FOES_AND_ALLY)
            continue;
        // These selected-target actions support an ally; their abilities do
        // not turn them into opposing actions blocked by Psychic Terrain.
        if (effect == EFFECT_HEAL_PULSE || effect == EFFECT_AFTER_YOU || effect == EFFECT_INSTRUCT
         || (effect == EFFECT_FIRST_TURN_ONLY && !IsBattlersFirstTurn(battler)))
            continue;

        priority = AI_GetMovePriority(battler, ability, move);
        // Grassy Glide loses its boost in the proposed Psychic Terrain.
        if (effect == EFFECT_GRASSY_GLIDE && gFieldTimers.terrain == B_TERRAIN_GRASSY
         && AI_IsBattlerGrounded(battler))
            priority--;
        if (priority > 0)
            return TRUE;
    }
    return FALSE;
}

static bool32 HasBattlerTerrainBoostMove(enum BattlerId battler, enum BattleTerrain terrain)
{
    if (!IsBattlerAlive(battler))
        return FALSE;

    enum Move *moves = GetMovesArray(battler);
    for (u32 moveIndex = 0; moveIndex < MAX_MON_MOVES; moveIndex++)
    {
        enum Move move = moves[moveIndex];
        if (!IsMoveUnusable(moveIndex, move, gAiLogicData->moveLimitations[battler])
         && GetMoveEffect(move) == EFFECT_TERRAIN_BOOST
         && GetMoveTerrainBoost_Terrain(move) == terrain)
            return TRUE;
    }

    return FALSE;
}

bool32 WeatherChecker(enum BattlerId battler, u32 weather, enum FieldEffectOutcome desiredResult)
{
    enum FieldEffectOutcome actorResult = FIELD_EFFECT_NEUTRAL;
    enum FieldEffectOutcome partnerResult = FIELD_EFFECT_NEUTRAL;

    if (IsWeatherActive(B_WEATHER_PRIMAL_ANY) != WEATHER_INACTIVE)
        return FIELD_EFFECT_BLOCKED == desiredResult;

    for (u32 battlerIndex = 0; battlerIndex < gBattlersCount; battlerIndex++)
    {
        enum FieldEffectOutcome result = FIELD_EFFECT_NEUTRAL;

        if (!IsBattlerAlive(battlerIndex) || !IsBattlerAlly(battler, battlerIndex))
            continue;

        if (weather & B_WEATHER_RAIN)
            result = BenefitsFromRain(battlerIndex);
        else if (weather & B_WEATHER_SUN)
            result = BenefitsFromSun(battlerIndex);
        else if (weather & B_WEATHER_SANDSTORM)
            result = BenefitsFromSandstorm(battlerIndex);
        else if (weather & B_WEATHER_ICY_ANY)
            result = BenefitsFromHailOrSnow(battlerIndex, weather);

        if (battlerIndex == battler)
            actorResult = result;
        else
            partnerResult = result;
    }

    if (actorResult != FIELD_EFFECT_NEUTRAL)
        return actorResult == desiredResult;
    return partnerResult == desiredResult;
}

bool32 TerrainChecker(enum BattlerId battler, enum BattleTerrain terrain, enum FieldEffectOutcome desiredResult)
{
    enum FieldEffectOutcome result = FIELD_EFFECT_NEUTRAL;
    enum FieldEffectOutcome actorResult = FIELD_EFFECT_NEUTRAL;
    enum FieldEffectOutcome partnerResult = FIELD_EFFECT_NEUTRAL;

    for (u32 battlerIndex = 0; battlerIndex < gBattlersCount; battlerIndex++)
    {
        if (!IsBattlerAlive(battlerIndex) || !IsBattlerAlly(battler, battlerIndex))
            continue;

        switch (terrain)
        {
        case B_TERRAIN_ELECTRIC:
            result = BenefitsFromElectricTerrain(battlerIndex);
            break;
        case B_TERRAIN_GRASSY:
            result = BenefitsFromGrassyTerrain(battlerIndex);
            break;
        case B_TERRAIN_MISTY:
            result = BenefitsFromMistyTerrain(battlerIndex);
            break;
        case B_TERRAIN_PSYCHIC:
            result = BenefitsFromPsychicTerrain(battlerIndex);
            break;
        default:
            break;
        }

        if (battlerIndex == battler)
            actorResult = result;
        else
            partnerResult = result;
    }

    if (actorResult != FIELD_EFFECT_NEUTRAL)
        return actorResult == desiredResult;
    return partnerResult == desiredResult;
}

bool32 FieldStatusChecker(enum BattlerId battler, u32 fieldStatus, enum FieldEffectOutcome desiredResult)
{
    enum FieldEffectOutcome result = FIELD_EFFECT_NEUTRAL;
    enum FieldEffectOutcome actorResult = FIELD_EFFECT_NEUTRAL;
    enum FieldEffectOutcome partnerResult = FIELD_EFFECT_NEUTRAL;

    for (u32 battlerIndex = 0; battlerIndex < gBattlersCount; battlerIndex++)
    {
        if (!IsBattlerAlive(battlerIndex) || !IsBattlerAlly(battler, battlerIndex))
            continue;

        // other field statuses
        if (fieldStatus & STATUS_FIELD_GRAVITY)
            result = BenefitsFromGravity(battlerIndex);
        if (fieldStatus & STATUS_FIELD_TRICK_ROOM)
            result = BenefitsFromTrickRoom(battlerIndex);

        if (battlerIndex == battler)
            actorResult = result;
        else
            partnerResult = result;
    }
    if (fieldStatus & STATUS_FIELD_TRICK_ROOM
     && actorResult != FIELD_EFFECT_NEUTRAL
     && partnerResult != FIELD_EFFECT_NEUTRAL)
        return actorResult == partnerResult && actorResult == desiredResult;
    if (actorResult != FIELD_EFFECT_NEUTRAL)
        return actorResult == desiredResult;
    return partnerResult == desiredResult;
}

static bool32 DoesAbilityBenefitFromWeather(enum Ability ability, u32 weather)
{
    switch (ability)
    {
    case ABILITY_FORECAST:
        return (weather & (B_WEATHER_RAIN | B_WEATHER_SUN | B_WEATHER_ICY_ANY));
    case ABILITY_MAGIC_GUARD:
    case ABILITY_OVERCOAT:
        return (weather & B_WEATHER_DAMAGING_ANY);
    case ABILITY_SAND_FORCE:
    case ABILITY_SAND_RUSH:
    case ABILITY_SAND_VEIL:
        return (weather & B_WEATHER_SANDSTORM);
    case ABILITY_ICE_BODY:
    case ABILITY_ICE_FACE:
    case ABILITY_SNOW_CLOAK:
    case ABILITY_SLUSH_RUSH:
        return (weather & B_WEATHER_ICY_ANY);
    case ABILITY_DRY_SKIN:
    case ABILITY_HYDRATION:
    case ABILITY_RAIN_DISH:
    case ABILITY_SWIFT_SWIM:
        return (weather & B_WEATHER_RAIN);
    case ABILITY_CHLOROPHYLL:
    case ABILITY_FLOWER_GIFT:
    case ABILITY_HARVEST:
    case ABILITY_LEAF_GUARD:
    case ABILITY_ORICHALCUM_PULSE:
    case ABILITY_PROTOSYNTHESIS:
    case ABILITY_SOLAR_POWER:
        return (weather & B_WEATHER_SUN);
    default:
        break;
    }
    return FALSE;
}

static bool32 DoesAbilityBenefitFromTerrain(enum Ability ability, enum BattleTerrain terrain)
{
    switch (ability)
    {
    case ABILITY_MIMICRY:
        return terrain != B_TERRAIN_NONE;
    case ABILITY_HADRON_ENGINE:
    case ABILITY_QUARK_DRIVE:
    case ABILITY_SURGE_SURFER:
        return terrain == B_TERRAIN_ELECTRIC;
    case ABILITY_GRASS_PELT:
        return terrain == B_TERRAIN_GRASSY;
    // no abilities inherently benefit from Misty or Psychic Terrains
    // return terrain == B_TERRAIN_MISTY;
    // return terrain == B_TERRAIN_PSYCHIC;
    default:
        break;
    }
    return FALSE;
}

static bool32 IsLightSensitiveMove(enum Move move)
{
    switch (GetMoveEffect(move))
    {
    case EFFECT_SOLAR_BEAM:
    case EFFECT_MORNING_SUN:
    case EFFECT_SYNTHESIS:
    case EFFECT_MOONLIGHT:
    case EFFECT_GROWTH:
        return TRUE;
    default:
        return FALSE;
    }
}

static bool32 HasLightSensitiveMove(enum BattlerId battler)
{
    enum Move *moves = GetMovesArray(battler);

    for (u32 moveIndex = 0; moveIndex < MAX_MON_MOVES; moveIndex++)
    {
        if (moves[moveIndex] != MOVE_NONE && moves[moveIndex] != MOVE_UNAVAILABLE && IsLightSensitiveMove(moves[moveIndex]))
            return TRUE;
    }

    return FALSE;
}

// Sun
// Utility Umbrella does not block Protosynthesis, but it does block Orichalcum Pulse's Attack boost.
static enum FieldEffectOutcome BenefitsFromSun(enum BattlerId battler)
{
    enum Ability ability = gAiLogicData->abilities[battler];

    if (gAiLogicData->holdEffects[battler] == HOLD_EFFECT_UTILITY_UMBRELLA)
    {
        if (ability == ABILITY_PROTOSYNTHESIS)
            return FIELD_EFFECT_POSITIVE;
        else
            return FIELD_EFFECT_NEUTRAL;
    }

    if (DoesAbilityBenefitFromWeather(ability, B_WEATHER_SUN)
     || HasLightSensitiveMove(battler)
     || HasDamagingMoveOfType(battler, TYPE_FIRE)
     || HasMoveWithEffect(battler, EFFECT_WEATHER_BALL)
     || HasMoveWithEffect(battler, EFFECT_HYDRO_STEAM))
        return FIELD_EFFECT_POSITIVE;

    if (HasMoveWithFlag(battler, MoveHas50AccuracyInSun) || HasDamagingMoveOfType(battler, TYPE_WATER) || gAiLogicData->abilities[battler] == ABILITY_DRY_SKIN)
        return FIELD_EFFECT_NEGATIVE;

    return FIELD_EFFECT_NEUTRAL;
}

// Sandstorm
static enum FieldEffectOutcome BenefitsFromSandstorm(enum BattlerId battler)
{
    if (DoesAbilityBenefitFromWeather(gAiLogicData->abilities[battler], B_WEATHER_SANDSTORM)
     || IS_BATTLER_OF_TYPE(battler, TYPE_ROCK)
     || HasMoveWithEffect(battler, EFFECT_WEATHER_BALL))
        return FIELD_EFFECT_POSITIVE;

    if (gAiLogicData->holdEffects[battler] == HOLD_EFFECT_SAFETY_GOGGLES || IS_BATTLER_ANY_TYPE(battler, TYPE_ROCK, TYPE_GROUND, TYPE_STEEL))
    {
        if (!IS_BATTLER_ANY_TYPE(GetBattlerLeftFoe(battler), TYPE_ROCK, TYPE_GROUND, TYPE_STEEL)
         && gAiLogicData->holdEffects[GetBattlerLeftFoe(battler)] != HOLD_EFFECT_SAFETY_GOGGLES
         && !DoesAbilityBenefitFromWeather(gAiLogicData->abilities[GetBattlerLeftFoe(battler)], B_WEATHER_SANDSTORM))
            return FIELD_EFFECT_POSITIVE;
        else
            return FIELD_EFFECT_NEUTRAL;
    }

    return FIELD_EFFECT_NEGATIVE;
}

// Hail or Snow
static enum FieldEffectOutcome BenefitsFromHailOrSnow(enum BattlerId battler, u32 weather)
{
    if (DoesAbilityBenefitFromWeather(gAiLogicData->abilities[battler], weather)
     || IS_BATTLER_OF_TYPE(battler, TYPE_ICE)
     || HasMoveWithEffect(battler, EFFECT_WEATHER_BALL)
     || HasMoveWithFlag(battler, MoveAlwaysHitsInHailSnow)
     || HasBattlerSideMoveWithEffect(battler, EFFECT_AURORA_VEIL))
        return FIELD_EFFECT_POSITIVE;

    if ((weather & B_WEATHER_DAMAGING_ANY) && gAiLogicData->holdEffects[battler] != HOLD_EFFECT_SAFETY_GOGGLES)
        return FIELD_EFFECT_NEGATIVE;

    if (HasLightSensitiveMove(battler))
        return FIELD_EFFECT_NEGATIVE;

    if (HasMoveWithFlag(GetBattlerLeftFoe(battler), MoveAlwaysHitsInHailSnow))
        return FIELD_EFFECT_NEGATIVE;

    return FIELD_EFFECT_NEUTRAL;
}

// Rain
static enum FieldEffectOutcome BenefitsFromRain(enum BattlerId battler)
{
    if (gAiLogicData->holdEffects[battler] == HOLD_EFFECT_UTILITY_UMBRELLA)
        return FIELD_EFFECT_NEUTRAL;

    if (DoesAbilityBenefitFromWeather(gAiLogicData->abilities[battler], B_WEATHER_RAIN)
      || HasMoveWithFlag(battler, MoveAlwaysHitsInRain)
      || HasDamagingMoveOfType(battler, TYPE_WATER)
      || HasMoveWithEffect(battler, EFFECT_WEATHER_BALL)
      || HasMove(battler, MOVE_ELECTRO_SHOT))
        return FIELD_EFFECT_POSITIVE;

    if (HasLightSensitiveMove(battler) || HasDamagingMoveOfType(battler, TYPE_FIRE))
        return FIELD_EFFECT_NEGATIVE;

    if (HasMoveWithFlag(GetBattlerLeftFoe(battler), MoveAlwaysHitsInRain))
        return FIELD_EFFECT_NEGATIVE;

    return FIELD_EFFECT_NEUTRAL;
}

static enum FieldEffectOutcome BenefitsFromElectricTerrain(enum BattlerId battler)
{
    bool32 grounded = AI_IsBattlerGrounded(battler);
    bool32 benefits = DoesAbilityBenefitFromTerrain(gAiLogicData->abilities[battler], B_TERRAIN_ELECTRIC)
                  || HasBattlerTerrainBoostMove(battler, B_TERRAIN_ELECTRIC)
                  || (grounded && (gBattleMons[battler].volatiles.yawn || HasDamagingMoveOfType(battler, TYPE_ELECTRIC)));
    bool32 harms = grounded && HasUsableMoveWithEffect(battler, EFFECT_REST);

    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
    {
        if (!IsBattlerAlive(foe) || IsBattlerAlly(battler, foe))
            continue;
        benefits |= HasUsableSleepMoveAgainst(foe, battler)
                 || (AI_IsBattlerGrounded(foe) && HasUsableMoveWithEffect(foe, EFFECT_REST));
        harms |= HasBattlerTerrainBoostMove(foe, B_TERRAIN_ELECTRIC);
        for (enum BattlerId ally = 0; ally < gBattlersCount; ally++)
            if (IsBattlerAlive(ally) && IsBattlerAlly(battler, ally))
                harms |= HasUsableSleepMoveAgainst(ally, foe);
    }

    // Existing sleep is not cured. A remaining allied sleep plan is a real
    // tradeoff even when this actor has an Electric attack or ability.
    if (benefits == harms)
        return FIELD_EFFECT_NEUTRAL;
    return benefits ? FIELD_EFFECT_POSITIVE : FIELD_EFFECT_NEGATIVE;
}

//TODO: when is grassy terrain bad?
static enum FieldEffectOutcome BenefitsFromGrassyTerrain(enum BattlerId battler)
{
    if (DoesAbilityBenefitFromTerrain(gAiLogicData->abilities[battler], B_TERRAIN_GRASSY))
        return FIELD_EFFECT_POSITIVE;

    if (HasBattlerSideMoveWithEffect(battler, EFFECT_GRASSY_GLIDE))
        return FIELD_EFFECT_POSITIVE;
    if (HasMoveWithAdditionalEffect(battler, MOVE_EFFECT_FLORAL_HEALING))
        return FIELD_EFFECT_POSITIVE;

    bool32 grounded = AI_IsBattlerGrounded(battler);

    // Weaken spamming Earthquake, Magnitude, and Bulldoze.
    if (grounded && (HasBattlerSideMoveWithEffect(GetBattlerLeftFoe(battler), EFFECT_EARTHQUAKE)
    || HasBattlerSideMoveWithEffect(GetBattlerLeftFoe(battler), EFFECT_MAGNITUDE)))
        return FIELD_EFFECT_POSITIVE;

    if (grounded && HasDamagingMoveOfType(battler, TYPE_GRASS))
        return FIELD_EFFECT_POSITIVE;

    if (HasBattlerSideMoveWithEffect(GetBattlerLeftFoe(battler), EFFECT_GRASSY_GLIDE))
        return FIELD_EFFECT_NEGATIVE;


    return FIELD_EFFECT_NEUTRAL;
}

//TODO: when is misty terrain bad?
static enum FieldEffectOutcome BenefitsFromMistyTerrain(enum BattlerId battler)
{
    if (DoesAbilityBenefitFromTerrain(gAiLogicData->abilities[battler], B_TERRAIN_MISTY))
        return FIELD_EFFECT_POSITIVE;

    if (HasBattlerTerrainBoostMove(battler, B_TERRAIN_MISTY)
     || HasBattlerTerrainBoostMove(GetPartnerBattler(battler), B_TERRAIN_MISTY))
        return FIELD_EFFECT_POSITIVE;

    bool32 grounded = AI_IsBattlerGrounded(battler);
    bool32 allyGrounded = FALSE;
    if (HasPartner(battler))
        allyGrounded = AI_IsBattlerGrounded(GetPartnerBattler(battler));

    if ((HasMoveWithEffect(GetBattlerLeftFoe(battler), EFFECT_REST) && AI_IsBattlerGrounded(GetBattlerLeftFoe(battler)))
     || (HasMoveWithEffect(GetBattlerRightFoe(battler), EFFECT_REST) && AI_IsBattlerGrounded(GetBattlerRightFoe(battler))))
        return FIELD_EFFECT_POSITIVE;

    // harass dragons
    if ((grounded || allyGrounded)
     && (HasDamagingMoveOfType(GetBattlerLeftFoe(battler), TYPE_DRAGON) || HasDamagingMoveOfType(GetBattlerRightFoe(battler), TYPE_DRAGON)))
        return FIELD_EFFECT_POSITIVE;

    if ((grounded || allyGrounded)
     && (HasNonVolatileMoveEffect(GetBattlerLeftFoe(battler), MOVE_EFFECT_SLEEP) || HasNonVolatileMoveEffect(GetBattlerRightFoe(battler), MOVE_EFFECT_SLEEP)))
        return FIELD_EFFECT_POSITIVE;

    if (grounded && (gBattleMons[battler].status1 & STATUS1_SLEEP || gBattleMons[battler].volatiles.yawn))
        return FIELD_EFFECT_POSITIVE;

    return FIELD_EFFECT_NEUTRAL;
}

static enum FieldEffectOutcome BenefitsFromPsychicTerrain(enum BattlerId battler)
{
    bool32 benefits = DoesAbilityBenefitFromTerrain(gAiLogicData->abilities[battler], B_TERRAIN_PSYCHIC)
                  || HasBattlerTerrainBoostMove(battler, B_TERRAIN_PSYCHIC)
                  || (AI_IsBattlerGrounded(battler) && HasDamagingMoveOfType(battler, TYPE_PSYCHIC));
    bool32 harms = FALSE;

    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
    {
        if (!IsBattlerAlive(foe) || IsBattlerAlly(battler, foe))
            continue;
        benefits |= HasUsablePriorityAgainst(foe, battler);
        harms |= HasUsablePriorityAgainst(battler, foe)
              || HasBattlerTerrainBoostMove(foe, B_TERRAIN_PSYCHIC);
    }

    if (benefits == harms)
        return FIELD_EFFECT_NEUTRAL;
    return benefits ? FIELD_EFFECT_POSITIVE : FIELD_EFFECT_NEGATIVE;
}

static enum FieldEffectOutcome BenefitsFromGravity(enum BattlerId battler)
{
    if (!AI_IsBattlerGrounded(battler))
        return FIELD_EFFECT_NEGATIVE;

    if (AI_IsAbilityOnSide(battler, ABILITY_HUSTLE))
        return FIELD_EFFECT_POSITIVE;

    if (HasMoveWithFlag(battler, IsMoveGravityBanned))
        return FIELD_EFFECT_NEGATIVE;

    if (IsBattlerAlive(GetBattlerLeftFoe(battler)))
    {
        if (HasMoveWithLowAccuracy(battler, GetBattlerLeftFoe(battler), LOW_ACCURACY_THRESHOLD, FALSE)
         || (!AI_IsBattlerGrounded(GetBattlerLeftFoe(battler)) && HasDamagingMoveOfType(battler, TYPE_GROUND)))
            return FIELD_EFFECT_POSITIVE;
    }

    if (IsBattlerAlive(GetBattlerRightFoe(battler)))
    {
        if (HasMoveWithLowAccuracy(battler, GetBattlerRightFoe(battler), LOW_ACCURACY_THRESHOLD, FALSE)
         || (!AI_IsBattlerGrounded(GetBattlerRightFoe(battler)) && HasDamagingMoveOfType(battler, TYPE_GROUND)))
            return FIELD_EFFECT_POSITIVE;
    }

    return FIELD_EFFECT_NEUTRAL;
}

static enum FieldEffectOutcome BenefitsFromTrickRoom(enum BattlerId battler)
{
    s32 speedMatchups = 0;
    bool32 hasPriorityAttack = FALSE;
    bool32 hasOrdinaryAttack = FALSE;

    // A priority-only attacker is indifferent to Room, not a reason to set it.
    // Merely carrying Fake Out or a priority finisher does not make its other
    // attacks independent of Speed. Count only currently usable attacks.
    if (!IsBattle1v1() && gFieldTimers.terrain != B_TERRAIN_PSYCHIC)
    {
        enum Move *aiMoves = GetMovesArray(battler);
        for (u32 moveIndex = 0; moveIndex < MAX_MON_MOVES; moveIndex++)
        {
            enum Move move = aiMoves[moveIndex];
            if (IsMoveUnusable(moveIndex, move, gAiLogicData->moveLimitations[battler]) || IsBattleMoveStatus(move))
                continue;
            if (AI_GetMovePriority(battler, gAiLogicData->abilities[battler], move) > 0
             && GetMoveEffect(move) != EFFECT_FIRST_TURN_ONLY
             && GetMoveEffect(move) != EFFECT_SUCKER_PUNCH)
                hasPriorityAttack = TRUE;
            else
                hasOrdinaryAttack = TRUE;
        }
        if (hasPriorityAttack && !hasOrdinaryAttack)
            return FIELD_EFFECT_NEUTRAL;
    }

    for (enum BattlerId foe = 0; foe < gBattlersCount; foe++)
    {
        if (!IsBattlerAlive(foe) || IsBattlerAlly(battler, foe))
            continue;
        if (gAiLogicData->speedStats[battler] < gAiLogicData->speedStats[foe])
            speedMatchups++;
        else if (gAiLogicData->speedStats[battler] > gAiLogicData->speedStats[foe])
            speedMatchups--;
    }
    // Ties and one faster/one slower matchup do not justify toggling the field.
    if (speedMatchups < 0)
        return FIELD_EFFECT_NEGATIVE;
    if (speedMatchups > 0)
        return FIELD_EFFECT_POSITIVE;
    return FIELD_EFFECT_NEUTRAL;
}

s32 CalcWeatherScore(enum BattlerId battlerAtk, enum BattlerId battlerDef, enum Move move, struct AiLogicData *aiData)
{
    s32 score = 0;

    switch (GetMoveWeatherType(move))
    {
    case BATTLE_WEATHER_RAIN:
        if (ShouldSetWeather(battlerAtk, B_WEATHER_RAIN))
        {
            score += DECENT_EFFECT;

            if (HasBattlerSideMoveWithEffect(battlerAtk, EFFECT_WEATHER_BALL))
                score += WEAK_EFFECT;
            if (aiData->holdEffects[battlerAtk] == HOLD_EFFECT_DAMP_ROCK)
                score += WEAK_EFFECT;
            if (HasBattlerSideMoveWithEffect(battlerDef, EFFECT_MORNING_SUN)
             || HasBattlerSideMoveWithEffect(battlerDef, EFFECT_SYNTHESIS)
             || HasBattlerSideMoveWithEffect(battlerDef, EFFECT_SOLAR_BEAM)
             || HasBattlerSideMoveWithEffect(battlerDef, EFFECT_MOONLIGHT))
                score += WEAK_EFFECT;
            if (HasDamagingMoveOfType(battlerDef, TYPE_FIRE) || HasDamagingMoveOfType(GetPartnerBattler(battlerDef), TYPE_FIRE))
                score += WEAK_EFFECT;
        }
        break;
    case BATTLE_WEATHER_SUN:
        if (ShouldSetWeather(battlerAtk, B_WEATHER_SUN))
        {
            score += DECENT_EFFECT;

            if (HasBattlerSideMoveWithEffect(battlerAtk, EFFECT_WEATHER_BALL))
                score += WEAK_EFFECT;
            if (aiData->holdEffects[battlerAtk] == HOLD_EFFECT_HEAT_ROCK)
                score += WEAK_EFFECT;
            if (HasDamagingMoveOfType(battlerDef, TYPE_WATER) || HasDamagingMoveOfType(GetPartnerBattler(battlerDef), TYPE_WATER))
                score += WEAK_EFFECT;
            if (HasMoveWithFlag(battlerDef, MoveHas50AccuracyInSun) || HasMoveWithFlag(GetPartnerBattler(battlerDef), MoveHas50AccuracyInSun))
                score += WEAK_EFFECT;
        }
        break;
    case BATTLE_WEATHER_SANDSTORM:
        if (ShouldSetWeather(battlerAtk, B_WEATHER_SANDSTORM))
        {
            score += DECENT_EFFECT;

            if (HasBattlerSideMoveWithEffect(battlerAtk, EFFECT_WEATHER_BALL))
                score += WEAK_EFFECT;
            if (aiData->holdEffects[battlerAtk] == HOLD_EFFECT_SMOOTH_ROCK)
                score += WEAK_EFFECT;
            if (HasMoveWithEffect(battlerDef, EFFECT_MORNING_SUN)
             || HasMoveWithEffect(battlerDef, EFFECT_SYNTHESIS)
             || HasMoveWithEffect(battlerDef, EFFECT_MOONLIGHT))
                score += WEAK_EFFECT;
        }
        break;
    case BATTLE_WEATHER_HAIL:
        if (ShouldSetWeather(battlerAtk, B_WEATHER_HAIL))
        {
            score += DECENT_EFFECT;

            if (HasBattlerSideMoveWithEffect(battlerAtk, EFFECT_AURORA_VEIL) && ShouldSetScreen(battlerAtk, battlerDef, EFFECT_AURORA_VEIL))
                score += GOOD_EFFECT;
            if (HasBattlerSideMoveWithEffect(battlerAtk, EFFECT_WEATHER_BALL))
                score += WEAK_EFFECT;
            if (aiData->holdEffects[battlerAtk] == HOLD_EFFECT_ICY_ROCK)
                score += WEAK_EFFECT;
            if (HasMoveWithEffect(battlerDef, EFFECT_MORNING_SUN)
             || HasMoveWithEffect(battlerDef, EFFECT_SYNTHESIS)
             || HasMoveWithEffect(battlerDef, EFFECT_MOONLIGHT))
                score += WEAK_EFFECT;
        }
        break;
    case BATTLE_WEATHER_SNOW:
        if (ShouldSetWeather(battlerAtk, B_WEATHER_SNOW))
        {
            score += DECENT_EFFECT;

            if (HasBattlerSideMoveWithEffect(battlerAtk, EFFECT_AURORA_VEIL) && ShouldSetScreen(battlerAtk, battlerDef, EFFECT_AURORA_VEIL))
                score += GOOD_EFFECT;
            if (HasBattlerSideMoveWithEffect(battlerAtk, EFFECT_WEATHER_BALL))
                score += WEAK_EFFECT;
            if (aiData->holdEffects[battlerAtk] == HOLD_EFFECT_ICY_ROCK)
                score += WEAK_EFFECT;
            if (HasMoveWithEffect(battlerDef, EFFECT_MORNING_SUN)
             || HasMoveWithEffect(battlerDef, EFFECT_SYNTHESIS)
             || HasMoveWithEffect(battlerDef, EFFECT_MOONLIGHT))
                score += WEAK_EFFECT;
        }
        break;
    default:
        break;
    }

    return score;
}
