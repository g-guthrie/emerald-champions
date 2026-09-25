#include "global.h"
#include "battle_setup.h"
#include "caps.h"
#include "center_guide.h"
#include "constants/emerald_champions.h"
#include "data.h"
#include "daycare.h"
#include "emerald_champions_battle_sets.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "item.h"
#include "field_effect.h"
#include "legendary_signs.h"
#include "pokedex.h"
#include "pokemon.h"
#include "random.h"
#include "roamer.h"
#include "battle.h"
#include "script_pokemon_util.h"
#include "string_util.h"
#include "weather_anomaly.h"
#include "constants/weather.h"
#include "constants/characters.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/maps.h"
#include "constants/region_map_sections.h"
#include "overworld.h"
#include "constants/moves.h"
#include "constants/opponents.h"
#include "constants/vars.h"

const struct LegendaryGate gLegendaryGates[LEGENDARY_SIGN_COUNT] =
{
#include "data/pokemon/legendary_signs.h"
};

const struct LegendaryAuthoredSet gLegendaryAuthoredSets[] =
{
#include "data/pokemon/legendary_authored_sets.h"
};

const u32 gLegendaryAuthoredSetCount = ARRAY_COUNT(gLegendaryAuthoredSets);

const struct EmeraldChampionsBattleSet *GetLegendaryAuthoredSet(enum Species species)
{
    for (u32 i = 0; i < gLegendaryAuthoredSetCount; i++)
    {
        if (gLegendaryAuthoredSets[i].species == species)
            return &gLegendaryAuthoredSets[i].set;
    }
    return NULL;
}

bool32 IsLegendaryEncounterSpecies(enum Species species)
{
    enum RestrictedPartyClass kind = GetRestrictedPartyClass(species);
    return kind == RESTRICTED_PARTY_LEGENDARY || kind == RESTRICTED_PARTY_ULTRA_BEAST;
}

u8 GetLegendaryEncounterLevel(enum Species species)
{
    return GetLevelCapForSpecies(species, GetCurrentLevelCap());
}

// One rule for every Legendary-class or Ultra Beast encounter: the authored
// set when the species has one, otherwise a random non-Mega competitive set.
// A caller-supplied held item survives only when the set leaves the mon empty-handed.
void ApplyLegendaryEncounterSet(struct Pokemon *mon, enum Item fallbackItem)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    const struct EmeraldChampionsBattleSet *authored = GetLegendaryAuthoredSet(species);

    if (authored != NULL)
        ApplyEmeraldChampionsScriptedSet(mon, authored);
    else
        ApplyEmeraldChampionsRandomNonMegaSet(mon);
    if (fallbackItem != ITEM_NONE && GetMonData(mon, MON_DATA_HELD_ITEM) == ITEM_NONE)
    {
        u16 item = fallbackItem;
        SetMonData(mon, MON_DATA_HELD_ITEM, &item);
    }
}

bool32 IsWildSlotSpeciesAcquirable(enum Species species)
{
    return !IsLegendaryEncounterSpecies(species) || CanAcquireLegendarySignSpecies(species);
}

static EWRAM_DATA u8 sRestingSigns[(LEGENDARY_SIGN_COUNT + 7) / 8];
static EWRAM_DATA u16 sEncounterSignPlusOne;

static u16 GetLegendaryStateVar(u16 firstVar, enum LegendarySignId signId)
{
    static const u16 sUnlockedVars[] =
    {
        VAR_LEGENDARY_SIGNS_UNLOCKED_0,
        VAR_LEGENDARY_SIGNS_UNLOCKED_1,
        VAR_LEGENDARY_SIGNS_UNLOCKED_2,
        VAR_LEGENDARY_SIGNS_UNLOCKED_3,
        VAR_LEGENDARY_SIGNS_UNLOCKED_4,
        VAR_LEGENDARY_SIGNS_UNLOCKED_5,
    };
    static const u16 sCaughtVars[] =
    {
        VAR_LEGENDARY_SIGNS_CAUGHT_0,
        VAR_LEGENDARY_SIGNS_CAUGHT_1,
        VAR_LEGENDARY_SIGNS_CAUGHT_2,
        VAR_LEGENDARY_SIGNS_CAUGHT_3,
        VAR_LEGENDARY_SIGNS_CAUGHT_4,
        VAR_LEGENDARY_SIGNS_CAUGHT_5,
    };
    u32 index = signId / 16;

    if (index >= ARRAY_COUNT(sUnlockedVars))
        return VAR_LEGENDARY_SIGNS_UNLOCKED_0;
    return firstVar == VAR_LEGENDARY_SIGNS_CAUGHT_0 ? sCaughtVars[index] : sUnlockedVars[index];
}

static bool32 GetLegendaryStateBit(u16 firstVar, enum LegendarySignId signId)
{
    if (signId >= LEGENDARY_SIGN_COUNT)
        return FALSE;
    return (VarGet(GetLegendaryStateVar(firstVar, signId)) & (1u << (signId % 16))) != 0;
}

static void SetLegendaryStateBit(u16 firstVar, enum LegendarySignId signId)
{
    if (signId < LEGENDARY_SIGN_COUNT)
    {
        u16 var = GetLegendaryStateVar(firstVar, signId);
        VarSet(var, VarGet(var) | (1u << (signId % 16)));
    }
}

static u8 GetBadgeCountForLegendarySigns(void)
{
    u8 count = 0;

    for (u8 badge = 0; badge < NUM_BADGES; badge++)
        if (FlagGet(FLAG_BADGE01_GET + badge))
            count++;
    return count;
}

bool32 IsLegendarySignUnlocked(enum LegendarySignId signId)
{
    return GetLegendaryStateBit(VAR_LEGENDARY_SIGNS_UNLOCKED_0, signId);
}

bool32 IsLegendarySignCaught(enum LegendarySignId signId)
{
    return GetLegendaryStateBit(VAR_LEGENDARY_SIGNS_CAUGHT_0, signId);
}

// Object visibility uses the same flag when a Sign is unlocked and caught.
// Zero entries have no separate map-object flag; saved Sign bits still apply.
static u16 GetLegendarySignObjectFlag(enum LegendarySignId signId)
{
    static const u16 sObjectFlags[LEGENDARY_SIGN_COUNT] =
    {
        [LEGENDARY_SIGN_ARTICUNO] = FLAG_EC_CAUGHT_ARTICUNO,
        [LEGENDARY_SIGN_CELEBI] = FLAG_EC_CAUGHT_CELEBI,
        [LEGENDARY_SIGN_DARKRAI] = FLAG_HIDE_LEGENDARY_SIGN_DARKRAI,
        [LEGENDARY_SIGN_CRESSELIA] = FLAG_HIDE_LEGENDARY_SIGN_CRESSELIA,
        [LEGENDARY_SIGN_DIALGA] = FLAG_HIDE_LEGENDARY_SIGN_DIALGA,
        [LEGENDARY_SIGN_HOOPA] = FLAG_EC_CAUGHT_HOOPA,
        [LEGENDARY_SIGN_MELOETTA] = FLAG_EC_CAUGHT_MELOETTA,
        [LEGENDARY_SIGN_MEWTWO] = FLAG_EC_CAUGHT_MEWTWO,
        [LEGENDARY_SIGN_REGIGIGAS] = FLAG_EC_CAUGHT_REGIGIGAS,
        [LEGENDARY_SIGN_PALKIA] = FLAG_EC_CAUGHT_PALKIA,
        [LEGENDARY_SIGN_PECHARUNT] = FLAG_EC_CAUGHT_PECHARUNT,
        [LEGENDARY_SIGN_RESHIRAM] = FLAG_EC_CAUGHT_RESHIRAM,
        [LEGENDARY_SIGN_SHAYMIN] = FLAG_EC_CAUGHT_SHAYMIN,
        [LEGENDARY_SIGN_TERAPAGOS] = FLAG_EC_CAUGHT_TERAPAGOS,
        [LEGENDARY_SIGN_ZAPDOS] = FLAG_EC_CAUGHT_ZAPDOS,
    };

    return (u32)signId < LEGENDARY_SIGN_COUNT ? sObjectFlags[signId] : 0;
}

// A static Legendary or Mythical that faints in battle is lost for good.
// Its map keeps it away with this flag: the object hide flag its knockout's
// removeobject saves, the shrine flag its script sets, or the classic
// defeated flag. A capture sets the same flag, so the caught bit tells the
// two apart. Deoxys is not listed: its defeat completes the finale.
static u16 GetStaticLegendaryGoneFlag(enum LegendarySignId signId)
{
    switch (signId)
    {
    case LEGENDARY_SIGN_ARTICUNO:  return FLAG_EC_CAUGHT_ARTICUNO;
    case LEGENDARY_SIGN_ZAPDOS:    return FLAG_EC_CAUGHT_ZAPDOS;
    case LEGENDARY_SIGN_MEWTWO:    return FLAG_EC_CAUGHT_MEWTWO;
    case LEGENDARY_SIGN_REGIGIGAS: return FLAG_EC_CAUGHT_REGIGIGAS;
    case LEGENDARY_SIGN_PECHARUNT: return FLAG_EC_CAUGHT_PECHARUNT;
    case LEGENDARY_SIGN_DARKRAI:   return FLAG_HIDE_LEGENDARY_SIGN_DARKRAI;
    case LEGENDARY_SIGN_MOLTRES:   return FLAG_DEFEATED_MOLTRES;
    case LEGENDARY_SIGN_HEATRAN:   return FLAG_DEFEATED_HEATRAN;
    case LEGENDARY_SIGN_REGIROCK:  return FLAG_DEFEATED_REGIROCK;
    case LEGENDARY_SIGN_REGICE:    return FLAG_DEFEATED_REGICE;
    case LEGENDARY_SIGN_REGISTEEL: return FLAG_DEFEATED_REGISTEEL;
    case LEGENDARY_SIGN_GROUDON:   return FLAG_DEFEATED_GROUDON;
    case LEGENDARY_SIGN_KYOGRE:    return FLAG_DEFEATED_KYOGRE;
    case LEGENDARY_SIGN_RAYQUAZA:  return FLAG_DEFEATED_RAYQUAZA;
    case LEGENDARY_SIGN_JIRACHI:   return FLAG_DEFEATED_JIRACHI;
    case LEGENDARY_SIGN_DIANCIE:   return FLAG_DEFEATED_DIANCIE;
    default:                       return 0;
    }
}

static bool32 IsLegendarySignLost(enum LegendarySignId signId)
{
    u16 goneFlag = GetStaticLegendaryGoneFlag(signId);

    if (goneFlag == 0 || !FlagGet(goneFlag) || IsLegendarySignCaught(signId))
        return FALSE;
    // These two flags start set, before the Pokémon has ever appeared.
    if (signId == LEGENDARY_SIGN_DARKRAI)
        return IsLegendarySignUnlocked(signId); // Its shrine unlocks it first.
    if (signId == LEGENDARY_SIGN_HEATRAN) // The Magma Stone (a Key Item) was set down.
        return FlagGet(FLAG_ITEM_MAGMA_HIDEOUT_2F_2R_MAGMA_STONE)
            && !CheckBagHasItem(ITEM_MAGMA_STONE, 1) && !CheckPCHasItem(ITEM_MAGMA_STONE, 1);
    return TRUE;
}

void UnlockLegendarySign(enum LegendarySignId signId)
{
    u16 objectFlag = GetLegendarySignObjectFlag(signId);
    bool32 lost = IsLegendarySignLost(signId);

    SetLegendaryStateBit(VAR_LEGENDARY_SIGNS_UNLOCKED_0, signId);
    if (objectFlag != 0 && !IsLegendarySignCaught(signId) && !lost)
        FlagClear(objectFlag);
}

enum LegendarySignId GetLegendarySignIdBySpecies(enum Species species)
{
    species = SanitizeSpeciesId(species);
    for (enum LegendarySignId signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
        if (gLegendaryGates[signId].species == species)
            return signId;

    // Registered regional discoveries are independent; interchangeable
    // battle and item forms still complete their original discovery.
    if (gSpeciesInfo[species].isAlolanForm || gSpeciesInfo[species].isGalarianForm
     || gSpeciesInfo[species].isHisuianForm || gSpeciesInfo[species].isPaldeanForm)
        return LEGENDARY_SIGN_COUNT;
    species = GET_BASE_SPECIES_ID(species);
    for (enum LegendarySignId signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
        if (gLegendaryGates[signId].species == species)
            return signId;
    return LEGENDARY_SIGN_COUNT;
}

// Save indices are append-only and the six Unlocked/Caught var pairs hold
// ids 0-95. Never use caught bits for a failed encounter: ownership-dependent
// quests must still know the difference.
STATIC_ASSERT(LEGENDARY_SIGN_COUNT <= 96, LegendarySignCapacity);

bool32 HasCaughtSpeciesFamily(enum Species species)
{
    if (species == SPECIES_NONE || PlayerPartyHasSpeciesFamily(species))
        return TRUE;
    enum Species root = GetEggSpecies(species);
    for (enum Species candidate = 1; candidate < NUM_SPECIES; candidate++)
    {
        if (!IsSpeciesEnabled(candidate) || GET_BASE_SPECIES_ID(candidate) != candidate)
            continue;
        if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(candidate), FLAG_GET_CAUGHT)
         && GetEggSpecies(candidate) == root)
            return TRUE;
    }
    return FALSE;
}

static bool32 MeetsSignProgression(enum LegendarySignId id)
{
    const struct LegendaryGate *gate = &gLegendaryGates[id];
    return GetBadgeCountForLegendarySigns() >= gate->minimumBadges
        && (gate->unlockFlag == 0 || FlagGet(gate->unlockFlag));
}

static bool32 IsLocalQuestSign(enum LegendarySignId id)
{
    return id == LEGENDARY_SIGN_MELOETTA || id == LEGENDARY_SIGN_LANDORUS || id == LEGENDARY_SIGN_MARSHADOW;
}

static bool32 MeetsSignDiscovery(enum LegendarySignId id)
{
    // The statue answers the three Regis' Pokedex records. The party may
    // hold only one Legendary-class Pokemon, so it never asks for all three.
    if (id == LEGENDARY_SIGN_REGIGIGAS)
        return HasCaughtSpeciesFamily(SPECIES_REGIROCK)
            && HasCaughtSpeciesFamily(SPECIES_REGICE)
            && HasCaughtSpeciesFamily(SPECIES_REGISTEEL);
    if (IsLegendarySignUnlocked(id))
        return TRUE;
    if (IsLocalQuestSign(id))
        return FALSE; // Their local NPC completes a small, permanent discovery quest.
    if (id == LEGENDARY_SIGN_KYUREM)
        return HasCaughtSpeciesFamily(SPECIES_RESHIRAM) || HasCaughtSpeciesFamily(SPECIES_ZEKROM);
    if (id == LEGENDARY_SIGN_PECHARUNT)
        return IsLegendarySignCaught(LEGENDARY_SIGN_OKIDOGI)
            && IsLegendarySignCaught(LEGENDARY_SIGN_MUNKIDORI)
            && IsLegendarySignCaught(LEGENDARY_SIGN_FEZANDIPITI);
    return HasCaughtSpeciesFamily(gLegendaryGates[id].requiredSpecies);
}

static u16 GetLocalQuestMap(enum LegendarySignId id)
{
    switch (id)
    {
    case LEGENDARY_SIGN_MELOETTA:
        return MAP_DEWFORD_MEADOW;
    case LEGENDARY_SIGN_LANDORUS:
        return MAP_ROUTE111_RUINS_EXTERIOR;
    case LEGENDARY_SIGN_MARSHADOW:
        return MAP_ROUTE113_GLASS_WORKSHOP;
    default:
        return MAP_UNDEFINED;
    }
}

void TryUnlockLocalLegendaryDiscovery(void)
{
    enum LegendarySignId id = gSpecialVar_0x8004;
    u16 map = ((u8)gSaveBlock1Ptr->location.mapGroup << 8) | (u8)gSaveBlock1Ptr->location.mapNum;
    bool32 helped = FALSE;

    gSpecialVar_Result = FALSE;
    if (!IsLocalQuestSign(id))
        return;
    if (map != GetLocalQuestMap(id) || !MeetsSignProgression(id)
     || IsLegendarySignUnlocked(id) || IsLegendarySignCaught(id))
        return;
    if (id == LEGENDARY_SIGN_MARSHADOW)
        helped = (VarGet(VAR_EC_SOOT_PROGRESS) & EC_SOOT_TOTAL_MASK) >= EC_SOOT_MARSHADOW_TARGET;
    else if (id == LEGENDARY_SIGN_LANDORUS)
        helped = PlayerPartyHasSpeciesFamily(SPECIES_CASTFORM);
    else
    {
        for (u32 slot = 0; slot < PARTY_SIZE; slot++)
        {
            struct Pokemon *mon = &gParties[B_TRAINER_PLAYER][slot];
            enum Species species = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);
            if (species == SPECIES_NONE || species == SPECIES_EGG)
                continue;
            for (u32 move = 0; move < MAX_MON_MOVES; move++)
                helped |= GetMonData(mon, MON_DATA_MOVE1 + move) == MOVE_SING;
        }
    }
    if (helped)
    {
        UnlockLegendarySign(id);
        gSpecialVar_Result = TRUE;
    }
}

static bool32 IsSignResting(enum LegendarySignId id)
{
    return (sRestingSigns[id / 8] & (1u << (id % 8))) != 0;
}

void ResetLegendaryEncounterVisits(void)
{
    memset(sRestingSigns, 0, sizeof(sRestingSigns));
    sEncounterSignPlusOne = 0;
}

// Script removeobject also saves the object's hide flag. A failed encounter
// must disappear only from the live map, so re-entry can recreate it.
void HideRestingLegendaryObject(void)
{
    u8 objectId;
    if (!TryGetObjectEventIdByLocalIdAndMap(gSpecialVar_LastTalked,
        gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup, &objectId))
        RemoveObjectEvent(&gObjectEvents[objectId]);
}

void FinishLegendaryLandmarkEncounter(void)
{
    if (sEncounterSignPlusOne != 0 && sEncounterSignPlusOne <= LEGENDARY_SIGN_COUNT
     && gBattleOutcome != B_OUTCOME_CAUGHT)
    {
        u16 id = sEncounterSignPlusOne - 1;
        sRestingSigns[id / 8] |= 1u << (id % 8);
    }
    sEncounterSignPlusOne = 0;
}

bool32 CanAcquireLegendarySignSpecies(enum Species species)
{
    enum LegendarySignId id = GetLegendarySignIdBySpecies(species);
    return id >= LEGENDARY_SIGN_COUNT
        || (!IsLegendarySignCaught(id) && !IsLegendarySignLost(id)
         && MeetsSignProgression(id) && MeetsSignDiscovery(id));
}

// A discovery whose required Pokémon was lost in battle (and never caught)
// can no longer be completed.
static bool32 IsRequiredFamilyLost(enum Species species)
{
    return IsLegendarySignLost(GetLegendarySignIdBySpecies(species)) && !HasCaughtSpeciesFamily(species);
}

static bool32 IsSignDiscoveryLost(enum LegendarySignId id)
{
    enum Species required = gLegendaryGates[id].requiredSpecies;

    if (id == LEGENDARY_SIGN_REGIGIGAS)
        return IsRequiredFamilyLost(SPECIES_REGIROCK)
            || IsRequiredFamilyLost(SPECIES_REGICE)
            || IsRequiredFamilyLost(SPECIES_REGISTEEL);
    return required != SPECIES_NONE && IsRequiredFamilyLost(required);
}

// Saved bit indices are append-only: 0-23 are undelivered relics and
// 24-29 mark species whose one-time relic grant has already been earned.
static const enum Item sLegendaryRelicItems[] =
{
    ITEM_RED_ORB, ITEM_BLUE_ORB, ITEM_RUSTED_SWORD, ITEM_RUSTED_SHIELD,
    ITEM_WELLSPRING_MASK, ITEM_HEARTHFLAME_MASK, ITEM_CORNERSTONE_MASK,
    ITEM_FLAME_PLATE, ITEM_SPLASH_PLATE, ITEM_ZAP_PLATE, ITEM_MEADOW_PLATE,
    ITEM_ICICLE_PLATE, ITEM_FIST_PLATE, ITEM_TOXIC_PLATE, ITEM_EARTH_PLATE,
    ITEM_SKY_PLATE, ITEM_MIND_PLATE, ITEM_INSECT_PLATE, ITEM_STONE_PLATE,
    ITEM_SPOOKY_PLATE, ITEM_DRACO_PLATE, ITEM_DREAD_PLATE, ITEM_IRON_PLATE,
    ITEM_PIXIE_PLATE,
};

static const struct
{
    enum Species species;
    u8 firstItem;
    u8 itemCount;
} sLegendaryRelicGrants[] =
{
    {SPECIES_GROUDON, 0, 1},
    {SPECIES_KYOGRE, 1, 1},
    {SPECIES_ZACIAN, 2, 1},
    {SPECIES_ZAMAZENTA, 3, 1},
    {SPECIES_OGERPON_TEAL, 4, 3},
    {SPECIES_ARCEUS, 7, 17},
};

#define LEGENDARY_RELIC_EARNED_SHIFT 24

static u32 GetLegendaryRelicDeliveryState(void)
{
    return VarGet(VAR_LEGENDARY_RELIC_DELIVERY_0)
         | ((u32)VarGet(VAR_LEGENDARY_RELIC_DELIVERY_1) << 16);
}

static void SetLegendaryRelicDeliveryState(u32 state)
{
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_0, state & 0xFFFF);
    VarSet(VAR_LEGENDARY_RELIC_DELIVERY_1, state >> 16);
}

static bool32 GiveLegendaryRelicItem(enum Item item)
{
    return CheckBagHasItem(item, 1) || CheckPCHasItem(item, 1)
        || AddBagItem(item, 1) || AddPCItem(item, 1);
}

// The legend is yours when you catch it; its trump form is the final act's.
// The Red and Blue Orbs and the Rusted Sword and Shield (the first four
// relics) wait for the Hall of Fame and arrive at the next Pokémon Center.
#define LEGENDARY_RELIC_CHAMPION_ONLY_COUNT 4

static bool32 IsRelicHeldForChampion(u32 item)
{
    return item < LEGENDARY_RELIC_CHAMPION_ONLY_COUNT && !FlagGet(FLAG_IS_CHAMPION);
}

// callnative from the nurse; VAR_RESULT counts relics handed over this time.
void RetryPendingLegendaryRelics(void)
{
    u32 state = GetLegendaryRelicDeliveryState();
    u32 delivered = 0;
    for (u32 item = 0; item < ARRAY_COUNT(sLegendaryRelicItems); item++)
    {
        if ((state & (1u << item)) && !IsRelicHeldForChampion(item)
         && GiveLegendaryRelicItem(sLegendaryRelicItems[item]))
        {
            state &= ~(1u << item);
            delivered++;
        }
    }
    SetLegendaryRelicDeliveryState(state);
    gSpecialVar_Result = delivered;
}

static void GiveLegendaryRelicsForSpecies(enum Species species)
{
    u32 state = GetLegendaryRelicDeliveryState();
    for (u32 group = 0; group < ARRAY_COUNT(sLegendaryRelicGrants); group++)
    {
        u32 earned = 1u << (LEGENDARY_RELIC_EARNED_SHIFT + group);
        if (sLegendaryRelicGrants[group].species != species || (state & earned))
            continue;
        state |= earned;
        for (u32 item = sLegendaryRelicGrants[group].firstItem;
             item < sLegendaryRelicGrants[group].firstItem + sLegendaryRelicGrants[group].itemCount;
             item++)
        {
            // A relic held for the Hall of Fame, or a real acquisition with
            // failed insertion, becomes debt the nurse pays later.
            if (IsRelicHeldForChampion(item) || !GiveLegendaryRelicItem(sLegendaryRelicItems[item]))
                state |= 1u << item;
        }
        SetLegendaryRelicDeliveryState(state);
        return;
    }
}

void MarkLegendarySignCaughtBySpecies(enum Species species)
{
    species = SanitizeSpeciesId(species);
    enum LegendarySignId signId = GetLegendarySignIdBySpecies(species);
    u16 objectFlag;

    // Form-defining relics are earned with their Pokémon, never synthesized
    // by the free held-item vendor or a tutor preset.  This call is
    // earned once and persists any undelivered items, including Groudon/Kyogre, whose
    // canonical Emerald encounters are not Legendary Sign rows.
    GiveLegendaryRelicsForSpecies(GET_BASE_SPECIES_ID(species));

    if (signId >= LEGENDARY_SIGN_COUNT)
        return;
    UnlockLegendarySign(signId);
    SetLegendaryStateBit(VAR_LEGENDARY_SIGNS_CAUGHT_0, signId);
    objectFlag = GetLegendarySignObjectFlag(signId);
    if (objectFlag != 0)
        FlagSet(objectFlag);
}

bool32 PlayerPartyHasSpeciesFamily(enum Species species)
{
    enum Species requestedRoot;
    enum NationalDexOrder requestedDex;

    if (species == SPECIES_NONE)
        return TRUE;
    requestedRoot = GetEggSpecies(species);
    requestedDex = SpeciesToNationalPokedexNum(species);
    for (u8 slot = 0; slot < PARTY_SIZE; slot++)
    {
        enum Species partySpecies = GetMonData(&gParties[B_TRAINER_PLAYER][slot], MON_DATA_SPECIES_OR_EGG);

        if (partySpecies != SPECIES_NONE
         && partySpecies != SPECIES_EGG
         && (GetEggSpecies(partySpecies) == requestedRoot
          || SpeciesToNationalPokedexNum(partySpecies) == requestedDex))
            return TRUE;
    }
    return FALSE;
}

void TryUnlockSelectedLegendarySign(void)
{
    enum LegendarySignId id = gSpecialVar_0x8004;
    gSpecialVar_Result = 0;
    if (id >= LEGENDARY_SIGN_COUNT)
        return;
    if (IsLegendarySignCaught(id))
        gSpecialVar_Result = 4;
    else if (IsLegendarySignLost(id))
        gSpecialVar_Result = 6;
    else if (!MeetsSignProgression(id))
        return;
    else if (!MeetsSignDiscovery(id))
        gSpecialVar_Result = 1;
    else if (IsSignResting(id))
        gSpecialVar_Result = 5;
    else
    {
        gSpecialVar_Result = IsLegendarySignUnlocked(id) ? 3 : 2;
        UnlockLegendarySign(id);
    }
}

u16 GetSelectedLegendarySignState(void)
{
    enum LegendarySignId id = gSpecialVar_0x8004;
    if (id >= LEGENDARY_SIGN_COUNT)
        gSpecialVar_Result = 0;
    else if (IsLegendarySignCaught(id))
        gSpecialVar_Result = 2;
    else if (IsLegendarySignLost(id))
        gSpecialVar_Result = 4;
    else if (CanAcquireLegendarySignSpecies(gLegendaryGates[id].species))
        gSpecialVar_Result = IsSignResting(id) ? 3 : 1;
    else
        gSpecialVar_Result = 0;
    return gSpecialVar_Result;
}

// Shrines call this from their own map; the gate row decides eligibility and
// CreateScriptedWildMon applies the shared cap-level and battle-set rule.
void CreateSelectedLegendarySignEncounter(void)
{
    enum LegendarySignId id = gSpecialVar_0x8004;
    gSpecialVar_Result = FALSE;
    if (id >= LEGENDARY_SIGN_COUNT || IsSignResting(id)
     || !CanAcquireLegendarySignSpecies(gLegendaryGates[id].species))
        return;
    sEncounterSignPlusOne = id + 1;
    UnlockLegendarySign(id);
    CreateScriptedWildMon(gLegendaryGates[id].species,
        GetLegendaryEncounterLevel(gLegendaryGates[id].species), ITEM_NONE);
    gSpecialVar_Result = TRUE;
}

// VAR_0x8004 = species. VAR_0x8005 (a legacy level offset) is ignored: static
// legendaries always meet the player at the current cap.
void CreateEmeraldChampionsStaticLegendaryEncounter(void)
{
    enum Species species = gSpecialVar_0x8004;

    if (species == SPECIES_NONE || species >= NUM_SPECIES)
        return;
    CreateScriptedWildMon(species, GetLegendaryEncounterLevel(species), ITEM_NONE);
}

void TryGiveSelectedLegendarySignReward(void)
{
    enum LegendarySignId signId = gSpecialVar_0x8004;
    u8 giveResult;

    gSpecialVar_Result = 0;
    if (signId >= LEGENDARY_SIGN_COUNT || IsLegendarySignCaught(signId))
        return;
    giveResult = GiveLegendarySignReward(gLegendaryGates[signId].species,
        GetLegendaryEncounterLevel(gLegendaryGates[signId].species));
    if (giveResult == LEGENDARY_REWARD_UNAVAILABLE)
        return;
    if (giveResult == MON_CANT_GIVE)
        gSpecialVar_Result = 3;
    else
        gSpecialVar_Result = giveResult == MON_GIVEN_TO_PARTY ? 1 : 2;
}

void TryUnlockDarkraiLegendarySign(void)
{
    gSpecialVar_0x8004 = LEGENDARY_SIGN_DARKRAI;
    TryUnlockSelectedLegendarySign();
}

const u8 *GetLegendaryDisplayName(enum Species species)
{
    switch (species)
    {
    case SPECIES_ARTICUNO_GALAR: return COMPOUND_STRING("Galarian Articuno");
    case SPECIES_ZAPDOS_GALAR: return COMPOUND_STRING("Galarian Zapdos");
    case SPECIES_MOLTRES_GALAR: return COMPOUND_STRING("Galarian Moltres");
    default: return GetSpeciesName(species);
    }
}

static void AppendLegendaryProgressionRequirements(enum LegendarySignId id)
{
    const struct LegendaryGate *gate = &gLegendaryGates[id];
    bool32 needsBadges = GetBadgeCountForLegendarySigns() < gate->minimumBadges;
    u8 buffer[80];

    if (needsBadges)
    {
        ConvertIntToDecimalStringN(gStringVar1, gate->minimumBadges, STR_CONV_MODE_LEFT_ALIGN, 1);
        StringExpandPlaceholders(buffer, COMPOUND_STRING("\pGym Badges required: {STR_VAR_1}."));
        StringAppend(gStringVar4, buffer);
    }
    if (gate->unlockFlag == 0 || FlagGet(gate->unlockFlag))
        return;
    switch (gate->unlockFlag)
    {
    case FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE:
        StringAppend(gStringVar4, COMPOUND_STRING("\pFirst, resolve the weather crisis\nin Sootopolis."));
        break;
    case FLAG_GOT_TM24_FROM_WATTSON:
        StringAppend(gStringVar4, COMPOUND_STRING("\pHelp Wattson stop New Mauville's\ngenerator, then report back to him."));
        break;
    case FLAG_VISITED_FORTREE_CITY:
        StringAppend(gStringVar4, COMPOUND_STRING("\pFirst, travel north along Route 119\nto Fortree City."));
        break;
    case FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT:
        StringAppend(gStringVar4, COMPOUND_STRING("\pFirst, confront Maxie deep inside\nthe Magma Hideout."));
        break;
    case FLAG_RECEIVED_RED_OR_BLUE_ORB:
        StringAppend(gStringVar4, COMPOUND_STRING("\pFirst, help the elders at Mt. Pyre\nand receive the Magma Emblem."));
        break;
    case FLAG_KYOGRE_ESCAPED_SEAFLOOR_CAVERN:
        StringAppend(gStringVar4, COMPOUND_STRING("\pFirst, confront Team Aqua deep in\nthe Seafloor Cavern."));
        break;
    case FLAG_IS_CHAMPION:
    case FLAG_SYS_GAME_CLEAR:
        StringAppend(gStringVar4, COMPOUND_STRING("\pFirst, enter the Hall of Fame."));
        break;
    default:
        if (gate->unlockFlag >= FLAG_BADGE01_GET && gate->unlockFlag <= FLAG_BADGE08_GET)
        {
            if (!needsBadges)
            {
                ConvertIntToDecimalStringN(gStringVar1, gate->unlockFlag - FLAG_BADGE01_GET + 1, STR_CONV_MODE_LEFT_ALIGN, 1);
                StringExpandPlaceholders(buffer, COMPOUND_STRING("\pFirst, earn Gym Badge {STR_VAR_1}."));
                StringAppend(gStringVar4, buffer);
            }
        }
        else
        {
            StringAppend(gStringVar4, COMPOUND_STRING("\pFirst, reach the next milestone\nof your journey."));
        }
        break;
    }
}

// A weather-anomaly visitor outside its anomaly, while the window is open:
// it can only be met in a live storm, which the Weather Institute tracks.
static bool32 IsVisitorAwayInStorms(enum LegendarySignId id)
{
    return IsWeatherAnomalyVisitor(id) && IsWeatherAnomalyWindowOpen() && !IsWeatherAnomalyLive(id);
}

static const u8 sText_VisitorFollowsStorms[] = _("\pIt rides the weather anomalies.\nThe Weather Institute tracks them.");
static const u8 sText_LegendaryLost[] = _("\pIt fainted in battle and vanished.\nA fainted legend never returns.");
static const u8 sText_LegendaryDiscoveryLost[] = _("\pA Pokémon it needs fainted and\nvanished. That lead is closed.");

// Result: 0 = needs progression, 1 = needs discovery, 2 = available,
// 4 = caught, 5 = resting after an escape, 6 = lost after a knockout.
void ResearchSelectedLegendarySign(void)
{
    u16 selection = gSpecialVar_0x8004;
    gSpecialVar_Result = 0;
    if (selection >= LEGENDARY_SIGN_COUNT)
        return;
    enum LegendarySignId id = selection;
    const struct LegendaryGate *gate = &gLegendaryGates[id];
    StringCopy(gStringVar2, GetLegendaryDisplayName(gate->species));
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("{STR_VAR_2}"));
    if (IsLegendarySignCaught(id))
    {
        StringAppend(gStringVar4, COMPOUND_STRING("\pCaught! This discovery is recorded."));
        gSpecialVar_Result = 4;
        return;
    }
    if (IsLegendarySignLost(id))
    {
        StringAppend(gStringVar4, sText_LegendaryLost);
        gSpecialVar_Result = 6;
        return;
    }
    if (!MeetsSignProgression(id))
    {
        AppendLegendaryProgressionRequirements(id);
        if (IsVisitorAwayInStorms(id))
            StringAppend(gStringVar4, sText_VisitorFollowsStorms);
        return;
    }
    if (!MeetsSignDiscovery(id))
    {
        if (IsSignDiscoveryLost(id))
            StringAppend(gStringVar4, sText_LegendaryDiscoveryLost);
        else if (id == LEGENDARY_SIGN_MARSHADOW)
            StringAppend(gStringVar4, COMPOUND_STRING("\pCollect 250 soot in total, then\nspeak to Route 113's glassmaker.\lYour soot total is never spent."));
        else if (id == LEGENDARY_SIGN_MELOETTA)
            StringAppend(gStringVar4, COMPOUND_STRING("\pHelp Dewford Meadow's warden perform\na song. Bring a Pokémon with SING."));
        else if (id == LEGENDARY_SIGN_LANDORUS)
            StringAppend(gStringVar4, COMPOUND_STRING("\pBring CASTFORM to the archaeologist\nat Route 111's elevated ruins.\lHelp with the local weather survey."));
        else if (id == LEGENDARY_SIGN_KYUREM)
            StringAppend(gStringVar4, COMPOUND_STRING("\pFirst, catch RESHIRAM or ZEKROM.\nEither discovery is enough."));
        else if (id == LEGENDARY_SIGN_REGIGIGAS)
            StringAppend(gStringVar4, COMPOUND_STRING("\pCatch REGIROCK, REGICE and\nREGISTEEL. Their Pokédex records\lare enough; they can stay in the PC."));
        else if (id == LEGENDARY_SIGN_PECHARUNT)
            StringAppend(gStringVar4, COMPOUND_STRING("\pCatch OKIDOGI, MUNKIDORI and\nFEZANDIPITI, then inspect the shrine."));
        else
        {
            u8 buffer[180];
            StringCopy(gStringVar1, GetSpeciesName(gate->requiredSpecies));
            StringExpandPlaceholders(buffer, COMPOUND_STRING("\pA clue points to this family:\n{STR_VAR_1}.\pBefriend a member of that family.\nA caught Pokédex entry is enough;\lyour partner can stay in the PC."));
            StringAppend(gStringVar4, buffer);
        }
        gSpecialVar_Result = 1;
        return;
    }
    if (IsSignResting(id))
    {
        StringAppend(gStringVar4, COMPOUND_STRING("\pResting after your last encounter.\nLeave this area and return to retry."));
        gSpecialVar_Result = 5;
        return;
    }
    gSpecialVar_Result = 2;
    if (IsVisitorAwayInStorms(id))
    {
        StringAppend(gStringVar4, sText_VisitorFollowsStorms);
        return;
    }
    switch (gate->kind)
    {
    case LEGENDARY_KIND_WILD:
    case LEGENDARY_KIND_QUEST:
        StringAppend(gStringVar4, COMPOUND_STRING("\pAvailable now! Search the wild\nPokémon where it lives.\lOnce caught, it stops appearing."));
        break;
    case LEGENDARY_KIND_GIFT:
        StringAppend(gStringVar4, COMPOUND_STRING("\pAvailable now! Speak to Devon's\ndream researcher on 2F."));
        break;
    case LEGENDARY_KIND_PRIZE:
        StringAppend(gStringVar4, COMPOUND_STRING("\pAvailable now! Ask the Pokémon prize\ncounter at Mauville's Game Corner."));
        break;
    case LEGENDARY_KIND_BREED:
        StringAppend(gStringVar4, COMPOUND_STRING("\pLeave MANAPHY and DITTO together\nat Route 117's Day Care.\pHatch their Egg to meet PHIONE."));
        break;
    default:
        StringAppend(gStringVar4, COMPOUND_STRING("\pReady! Approach the Pokémon or\ninspect its shrine."));
        break;
    }
}

static const struct
{
    u16 city;
    enum LegendarySignId id;
    const u8 *lead;
    enum Species classicSpecies;
} sCenterLegendaryLeads[] =
{
#include "data/pokemon/center_legendary_leads.h"
};

// The object-hide flag starts set before awakening and is set again on
// capture. The Dex record distinguishes those two states.
u16 GetHeatranDiscoveryState(void)
{
    if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_HEATRAN), FLAG_GET_CAUGHT))
        return 2;
    // The Magma Stone clears Heatran's object hide flag when it awakens.
    return FlagGet(FLAG_DEFEATED_HEATRAN) ? 0 : 1;
}

// Replaces a lead whose static Pokémon fainted in battle.
static void BufferLostLegendaryLead(enum Species species)
{
    StringCopy(gStringVar2, GetLegendaryDisplayName(species));
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("{STR_VAR_2} fainted in a battle\nwith you and vanished.\pA fainted legend never returns,\nso that lead is closed."));
}

// The Center local guide's one special. VAR_0x8005 picks the topic (see
// include/center_guide.h); VAR_0x8004 is the cursor for the listing topics.
void BufferNextCenterLegendaryLead(void)
{
    if (gSpecialVar_0x8005 == CENTER_GUIDE_TOPIC_TIPS)
    {
        BufferNextCenterGuideTip();
        return;
    }
    if (gSpecialVar_0x8005 == CENTER_GUIDE_TOPIC_STORY)
    {
        BufferCenterGuideDirections();
        return;
    }
    for (u32 i = gSpecialVar_0x8004; i < ARRAY_COUNT(sCenterLegendaryLeads); i++)
    {
        if (sCenterLegendaryLeads[i].city != gMapHeader.regionMapSectionId)
            continue;
        enum LegendarySignId id = sCenterLegendaryLeads[i].id;
        gSpecialVar_0x8004 = i + 1;
        StringCopy(gStringVar4, sCenterLegendaryLeads[i].lead);
        if (id == LEGENDARY_SIGN_SHAYMIN && !FlagGet(FLAG_ADVENTURE_STARTED) && !IsLegendarySignCaught(id))
        {
            // The west exit is still closed: no "search there now" line yet.
            StringCopy(gStringVar4, COMPOUND_STRING("After you battle the Prof.'s kid,\nreturn to Birch for your send-off.\pThen head west to Route 102.\nShaymin lives in its grass."));
            gSpecialVar_Result = TRUE;
            return;
        }
        if (id >= LEGENDARY_SIGN_COUNT)
        {
            enum Species species = sCenterLegendaryLeads[i].classicSpecies;
            if (species == SPECIES_NONE)
            {
                const enum Species islandSpecies[] = {SPECIES_LATIAS, SPECIES_LATIOS,
                    SPECIES_LUGIA, SPECIES_HO_OH, SPECIES_MEW, SPECIES_DEOXYS};
                u32 caught = 0;
                for (u32 j = 0; j < ARRAY_COUNT(islandSpecies); j++)
                    caught += GetSetPokedexFlag(SpeciesToNationalPokedexNum(islandSpecies[j]), FLAG_GET_CAUGHT) != 0;
                ConvertIntToDecimalStringN(gStringVar1, caught, STR_CONV_MODE_LEFT_ALIGN, 1);
                u8 progress[100];
                StringExpandPlaceholders(progress, COMPOUND_STRING("\pYou've found {STR_VAR_1} of those six\ntraveling and island legends."));
                StringAppend(gStringVar4, progress);
                gSpecialVar_Result = TRUE;
                return;
            }
            // The Galarian resident shares Moltres's national Dex entry.
            bool32 caught = species == SPECIES_MOLTRES ? FlagGet(FLAG_EC_CAUGHT_MOLTRES)
                : GetSetPokedexFlag(SpeciesToNationalPokedexNum(species), FLAG_GET_CAUGHT);
            if (species == SPECIES_HEATRAN && GetHeatranDiscoveryState() == 1)
                StringCopy(gStringVar4, COMPOUND_STRING("Heatran is awake in the deepest\nroom of Scorched Slab.\pIf it fled after a battle, leave\nthat room and return to find it."));
            if (caught)
            {
                StringCopy(gStringVar2, GetSpeciesName(species));
                StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("You've already found\n{STR_VAR_2}!\pThat's one local legend you have\nturned into a partner."));
            }
            else if (IsLegendarySignLost(GetLegendarySignIdBySpecies(species)))
            {
                BufferLostLegendaryLead(species);
            }
            gSpecialVar_Result = TRUE;
            return;
        }
        const struct LegendaryGate *gate = &gLegendaryGates[id];
        if (IsLegendarySignCaught(id))
        {
            StringCopy(gStringVar2, GetLegendaryDisplayName(gate->species));
            StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("You've already found\n{STR_VAR_2}!\pThat's one local legend you have\nturned into a partner."));
        }
        else if (IsLegendarySignLost(id))
            BufferLostLegendaryLead(gate->species);
        else if (IsSignDiscoveryLost(id))
            StringAppend(gStringVar4, sText_LegendaryDiscoveryLost);
        // Each lead states its own requirements in prose, the way an NPC
        // would; only the current status is added here.
        else if (IsVisitorAwayInStorms(id))
            StringAppend(gStringVar4, sText_VisitorFollowsStorms);
        else if (!MeetsSignProgression(id) || !MeetsSignDiscovery(id))
            ;
        else if (IsWeatherAnomalyVisitor(id) && IsWeatherAnomalyLive(id))
            StringAppend(gStringVar4, COMPOUND_STRING("\pIts storm is raging there right\nnow. Go and take a look!"));
        else if (gate->kind == LEGENDARY_KIND_WILD || gate->kind == LEGENDARY_KIND_QUEST)
            StringAppend(gStringVar4, COMPOUND_STRING("\pYou're ready! You can search\nfor it there now."));
        else if (gate->kind == LEGENDARY_KIND_STATIC)
            StringAppend(gStringVar4, COMPOUND_STRING("\pYou're ready! Visit its resting\nplace to meet it."));
        else if (gate->kind == LEGENDARY_KIND_GIFT || gate->kind == LEGENDARY_KIND_PRIZE)
            StringAppend(gStringVar4, COMPOUND_STRING("\pYou're ready! You can claim it\nthere now."));
        gSpecialVar_Result = TRUE;
        return;
    }
    gSpecialVar_Result = FALSE;
}

static bool32 ApplyNonMegaGiftSet(struct Pokemon *mon)
{
    enum Species species = GetMonData(mon, MON_DATA_SPECIES);
    u8 selected = 0;
    u32 matches = 0;

    for (u8 choice = 0; choice < GetEmeraldChampionsRawBattleSetCount(species); choice++)
    {
        const struct EmeraldChampionsBattleSet *preset = GetEmeraldChampionsRawBattleSet(species, choice);

        if (preset == NULL
         || preset->requiredItem != ITEM_NONE
         || preset->requiredMove != MOVE_NONE)
            continue;
        if (RandomUniform(RNG_NONE, 0, ++matches - 1) == 0)
            selected = choice;
    }
    if (matches == 0)
        return FALSE;
    return ApplyEmeraldChampionsOpponentSet(mon, selected) != EC_BATTLE_SET_FAILED;
}

u8 GiveLegendarySignReward(enum Species species, u8 level)
{
    struct Pokemon reward;
    u8 giveResult;

    if (!CanAcquireLegendarySignSpecies(species))
        return LEGENDARY_REWARD_UNAVAILABLE;
    CreateMon(&reward, species, level, Random32(), OTID_STRUCT_PLAYER_ID);
    // Level-up moves and real stats remain if no usable preset exists for the species.
    GiveMonInitialMoveset(&reward);
    CalculateMonStats(&reward);
    ApplyNonMegaGiftSet(&reward);
    giveResult = GiveCapturedMonToPlayer(&reward);
    if (giveResult == MON_CANT_GIVE)
        return giveResult;
    HandleSetPokedexFlagFromMon(&reward, FLAG_SET_SEEN);
    HandleSetPokedexFlagFromMon(&reward, FLAG_SET_CAUGHT);
    MarkLegendarySignCaughtBySpecies(species);
    CalculatePlayerPartyCount();
    return giveResult;
}

// Devon's finale reward: the Hall of Fame is the only gate.
void TryGiveArceusLegendarySignMasteryReward(void)
{
    u8 giveResult;

    gSpecialVar_Result = 0;
    if (IsLegendarySignCaught(LEGENDARY_SIGN_ARCEUS))
    {
        gSpecialVar_Result = 4;
        return;
    }
    if (!FlagGet(FLAG_IS_CHAMPION))
        return;

    UnlockLegendarySign(LEGENDARY_SIGN_ARCEUS);
    giveResult = GiveLegendarySignReward(SPECIES_ARCEUS, GetLegendaryEncounterLevel(SPECIES_ARCEUS));
    if (giveResult == LEGENDARY_REWARD_UNAVAILABLE)
        return;
    if (giveResult == MON_CANT_GIVE)
    {
        gSpecialVar_Result = 3;
        return;
    }
    StringCopy(gStringVar1, GetSpeciesName(SPECIES_ARCEUS));
    gSpecialVar_Result = giveResult == MON_GIVEN_TO_PARTY ? 1 : 2;
}
