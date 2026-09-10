#include "global.h"
#include "battle_setup.h"
#include "caps.h"
#include "data.h"
#include "daycare.h"
#include "emerald_champions_battle_sets.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "item.h"
#include "legendary_signs.h"
#include "pokedex.h"
#include "pokemon.h"
#include "random.h"
#include "roamer.h"
#include "battle.h"
#include "script_pokemon_util.h"
#include "string_util.h"
#include "constants/characters.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/maps.h"
#include "constants/region_map_sections.h"
#include "overworld.h"
#include "constants/moves.h"
#include "constants/opponents.h"
#include "constants/vars.h"

const struct LegendarySignDefinition gLegendarySignDefinitions[LEGENDARY_SIGN_COUNT] =
{
#include "data/pokemon/legendary_signs.h"
};

static const u8 sSignLocationShoalIce[] = _("Shoal Cave's ice room");
static const u8 sSignLocationGraniteB2F[] = _("Granite Cave B2F");
static const u8 sSignLocationFieryPath[] = _("Fiery Path");
static const u8 sSignLocationMtPyre6F[] = _("Mt. Pyre's sixth floor");
static const u8 sSignLocationRoute111Desert[] = _("Route 111's desert");
static const u8 sSignLocationRoute111Ruins[] = _("the ruins above Route 111");
static const u8 sSignLocationRoute120[] = _("Route 120");
static const u8 sSignLocationUnderwaterSeafloor[] = _("the Seafloor Cavern seabed");
static const u8 sSignLocationRoute110[] = _("Route 110");
static const u8 sSignLocationMeteor1F2R[] = _("Meteor Falls' rear cave");
static const u8 sSignLocationDewfordMeadow[] = _("Dewford Meadow");
static const u8 sSignLocationAlteringCave[] = _("Altering Cave");
static const u8 sSignLocationCaveOfOriginB1F[] = _("Cave of Origin B1F");
static const u8 sSignLocationNewMauville[] = _("New Mauville");
static const u8 sSignLocationScorchedB2F[] = _("Scorched Slab B2F");
static const u8 sSignLocationRoute102[] = _("Route 102's grass");
static const u8 sSignLocationRoute123[] = _("Route 123");
static const u8 sSignLocationMtPyreExterior[] = _("Mt. Pyre's exterior");
static const u8 sSignLocationRoute119Land[] = _("Route 119's grass");
static const u8 sSignLocationMeteorB1F1R[] = _("Meteor Falls B1F");
static const u8 sSignLocationVictoryRoad1F[] = _("Victory Road 1F");
static const u8 sSignLocationPetalburgWoods2[] = _("Petalburg Woods, second area");
static const u8 sSignLocationPetalburgWoods3[] = _("Petalburg Woods, third area");
static const u8 sSignLocationMeteorB1F2R[] = _("Meteor Falls B1F, rear room");
static const u8 sSignLocationDesertUnderpass[] = _("Desert Underpass");
static const u8 sSignLocationSandstrewnB1F[] = _("Sandstrewn Ruins B1F");
static const u8 sSignLocationAshenWoods[] = _("Ashen Woods");
static const u8 sSignLocationVerdanturfMeadow[] = _("Verdanturf Meadow");
static const u8 sSignLocationRoute112[] = _("Route 112");
static const u8 sSignLocationRoute117[] = _("Route 117");
static const u8 sSignLocationRoute125[] = _("Route 125");
static const u8 sSignLocationRoute126[] = _("Route 126");
static const u8 sSignLocationRoute127[] = _("Route 127");
static const u8 sSignLocationVictoryRoadB1F[] = _("Victory Road B1F");
static const u8 sSignLocationMagmaHideout4F[] = _("Magma Hideout 4F");
static const u8 sSignLocationDevon[] = _("Devon Corp. 2F");
static const u8 sSignLocationCircuit[] = _("the Champions Circuit");
static const u8 sSignLocationGameCorner[] = _("Mauville's Game Corner");
static const u8 sSignLocationDayCare[] = _("Route 117's Day Care");
static const u8 sSignLocationMtPyreSummit[] = _("Mt. Pyre's summit");
static const u8 sSignLocationSealedChamber[] = _("the Sealed Chamber");
static const u8 sSignLocationAlteringB1F[] = _("Altering Cave B1F");
static const u8 sSignLocationEmberPath[] = _("Ember Path");
static EWRAM_DATA u8 sRestingSigns[(LEGENDARY_SIGN_COUNT + 7) / 8];
static EWRAM_DATA u16 sEncounterSignPlusOne;

static const u8 sSignLocationUnknown[] = _("an unknown place");

static const u8 *GetLegendarySignLocationName(enum LegendarySignId signId)
{
    switch (gLegendarySignDefinitions[signId].source)
    {
    case LEGENDARY_SOURCE_CIRCUIT:
        return sSignLocationCircuit;
    case LEGENDARY_SOURCE_GAME_CORNER:
        return sSignLocationGameCorner;
    case LEGENDARY_SOURCE_BREEDING:
        return sSignLocationDayCare;
    case LEGENDARY_SOURCE_MASTERY:
        return signId == LEGENDARY_SIGN_ARCEUS ? sSignLocationDevon : sSignLocationCircuit;
    default:
        break;
    }
    switch (signId)
    {
    case LEGENDARY_SIGN_MARSHADOW:
        return COMPOUND_STRING("Route 113 ash grass");
    case LEGENDARY_SIGN_ARTICUNO_GALAR:
        return sSignLocationRoute120;
    case LEGENDARY_SIGN_ZAPDOS_GALAR:
        return sSignLocationRoute112;
    case LEGENDARY_SIGN_MOLTRES_GALAR:
        return sSignLocationMtPyreExterior;
    case LEGENDARY_SIGN_DARKRAI:
        return sSignLocationMtPyreSummit;
    case LEGENDARY_SIGN_MAGEARNA:
        return sSignLocationDevon;
    case LEGENDARY_SIGN_REGIGIGAS:
        return sSignLocationSealedChamber;
    case LEGENDARY_SIGN_MEWTWO:
    case LEGENDARY_SIGN_GUZZLORD:
        return sSignLocationAlteringB1F;
    case LEGENDARY_SIGN_BLACEPHALON:
        return sSignLocationEmberPath;
    case LEGENDARY_SIGN_AZELF:
    case LEGENDARY_SIGN_ARTICUNO:
    case LEGENDARY_SIGN_CHIEN_PAO:
    case LEGENDARY_SIGN_KYUREM:
        return sSignLocationShoalIce;
    case LEGENDARY_SIGN_CELEBI:
    case LEGENDARY_SIGN_KARTANA:
        return sSignLocationPetalburgWoods3;
    case LEGENDARY_SIGN_COBALION:
        return sSignLocationGraniteB2F;
    case LEGENDARY_SIGN_ENTEI:
        return sSignLocationFieryPath;
    case LEGENDARY_SIGN_GIRATINA:
    case LEGENDARY_SIGN_PECHARUNT:
        return sSignLocationMtPyre6F;
    case LEGENDARY_SIGN_OKIDOGI:
    case LEGENDARY_SIGN_BUZZWOLE:
    case LEGENDARY_SIGN_CHI_YU:
        return sSignLocationAshenWoods;
    case LEGENDARY_SIGN_MUNKIDORI:
    case LEGENDARY_SIGN_PHEROMOSA:
    case LEGENDARY_SIGN_COSMOG:
    case LEGENDARY_SIGN_MELOETTA:
        return sSignLocationDewfordMeadow;
    case LEGENDARY_SIGN_HOOPA:
        return sSignLocationAlteringCave;
    case LEGENDARY_SIGN_LANDORUS:
        return sSignLocationRoute111Ruins;
    case LEGENDARY_SIGN_MESPRIT:
    case LEGENDARY_SIGN_XERNEAS:
        return sSignLocationRoute120;
    case LEGENDARY_SIGN_PALKIA:
    case LEGENDARY_SIGN_NIHILEGO:
        return sSignLocationUnderwaterSeafloor;
    case LEGENDARY_SIGN_RAIKOU:
    case LEGENDARY_SIGN_TAPU_KOKO:
    case LEGENDARY_SIGN_THUNDURUS:
        return sSignLocationRoute110;
    case LEGENDARY_SIGN_REGIDRAGO:
        return sSignLocationMeteor1F2R;
    case LEGENDARY_SIGN_REGIELEKI:
    case LEGENDARY_SIGN_ZAPDOS:
    case LEGENDARY_SIGN_MELTAN:
    case LEGENDARY_SIGN_ZEKROM:
    case LEGENDARY_SIGN_ZERAORA:
        return sSignLocationNewMauville;
    case LEGENDARY_SIGN_RESHIRAM:
        return sSignLocationScorchedB2F;
    case LEGENDARY_SIGN_SHAYMIN:
        return sSignLocationRoute102;
    case LEGENDARY_SIGN_TAPU_BULU:
        return sSignLocationRoute123;
    case LEGENDARY_SIGN_TAPU_LELE:
    case LEGENDARY_SIGN_YVELTAL:
        return sSignLocationMtPyreExterior;
    case LEGENDARY_SIGN_TORNADUS:
        return sSignLocationRoute119Land;
    case LEGENDARY_SIGN_UXIE:
    case LEGENDARY_SIGN_DIALGA:
        return sSignLocationMeteorB1F1R;
    case LEGENDARY_SIGN_CRESSELIA:
        return sSignLocationMeteorB1F2R;
    case LEGENDARY_SIGN_VICTINI:
        return sSignLocationVictoryRoad1F;
    case LEGENDARY_SIGN_VIRIZION:
    case LEGENDARY_SIGN_WO_CHIEN:
        return sSignLocationPetalburgWoods2;
    case LEGENDARY_SIGN_TING_LU:
        return sSignLocationDesertUnderpass;
    case LEGENDARY_SIGN_ZYGARDE:
    case LEGENDARY_SIGN_STAKATAKA:
        return sSignLocationSandstrewnB1F;
    case LEGENDARY_SIGN_KUBFU:
        return sSignLocationRoute112;
    case LEGENDARY_SIGN_TYPE_NULL:
        return sSignLocationRoute117;
    case LEGENDARY_SIGN_OGERPON:
        return sSignLocationRoute120;
    case LEGENDARY_SIGN_ENAMORUS:
    case LEGENDARY_SIGN_FEZANDIPITI:
        return sSignLocationVerdanturfMeadow;
    case LEGENDARY_SIGN_TERAPAGOS:
        return sSignLocationCaveOfOriginB1F;
    case LEGENDARY_SIGN_MANAPHY:
        return sSignLocationUnderwaterSeafloor;
    case LEGENDARY_SIGN_SUICUNE:
        return sSignLocationRoute125;
    case LEGENDARY_SIGN_TAPU_FINI:
        return sSignLocationRoute126;
    case LEGENDARY_SIGN_KELDEO:
        return sSignLocationRoute127;
    case LEGENDARY_SIGN_TERRAKION:
        return sSignLocationVictoryRoadB1F;
    case LEGENDARY_SIGN_VOLCANION:
        return sSignLocationMagmaHideout4F;
    default:
        return sSignLocationUnknown;
    }
}

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

void UnlockLegendarySign(enum LegendarySignId signId)
{
    u16 objectFlag = GetLegendarySignObjectFlag(signId);

    SetLegendaryStateBit(VAR_LEGENDARY_SIGNS_UNLOCKED_0, signId);
    if (objectFlag != 0 && !IsLegendarySignCaught(signId))
        FlagClear(objectFlag);
}

enum LegendarySignId GetLegendarySignIdBySpecies(enum Species species)
{
    species = SanitizeSpeciesId(species);
    for (enum LegendarySignId signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
        if (gLegendarySignDefinitions[signId].species == species)
            return signId;

    // Registered regional discoveries are independent; interchangeable
    // battle and item forms still complete their original discovery.
    if (gSpeciesInfo[species].isAlolanForm || gSpeciesInfo[species].isGalarianForm
     || gSpeciesInfo[species].isHisuianForm || gSpeciesInfo[species].isPaldeanForm)
        return LEGENDARY_SIGN_COUNT;
    species = GET_BASE_SPECIES_ID(species);
    for (enum LegendarySignId signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
        if (gLegendarySignDefinitions[signId].species == species)
            return signId;
    return LEGENDARY_SIGN_COUNT;
}

// Save indices are append-only. Reserve 0-95 for Signs and 96-127 for
// canonical encounters that predate the Sign ledger. Never use caught bits
// for a failed encounter: ownership-dependent quests must still know the difference.
STATIC_ASSERT(LEGENDARY_SIGN_COUNT <= 96, LegendarySignCapacity);

static bool32 HasCaughtSpeciesFamily(enum Species species)
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
    const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[id];
    return GetBadgeCountForLegendarySigns() >= sign->minimumBadges
        && (sign->requiredFlag == 0 || FlagGet(sign->requiredFlag));
}

static bool32 MeetsSignDiscovery(enum LegendarySignId id)
{
    // The statue needs all three partners present, including on retries.
    if (id == LEGENDARY_SIGN_REGIGIGAS)
        return PlayerPartyHasSpeciesFamily(SPECIES_REGIROCK)
            && PlayerPartyHasSpeciesFamily(SPECIES_REGICE)
            && PlayerPartyHasSpeciesFamily(SPECIES_REGISTEEL);
    if (IsLegendarySignUnlocked(id))
        return TRUE;
    if (id == LEGENDARY_SIGN_MELOETTA || id == LEGENDARY_SIGN_LANDORUS || id == LEGENDARY_SIGN_MARSHADOW)
        return FALSE; // Their local NPC completes a small, permanent discovery quest.
    if (id == LEGENDARY_SIGN_KYUREM)
        return HasCaughtSpeciesFamily(SPECIES_RESHIRAM) || HasCaughtSpeciesFamily(SPECIES_ZEKROM);
    if (id == LEGENDARY_SIGN_PECHARUNT)
        return IsLegendarySignCaught(LEGENDARY_SIGN_OKIDOGI)
            && IsLegendarySignCaught(LEGENDARY_SIGN_MUNKIDORI)
            && IsLegendarySignCaught(LEGENDARY_SIGN_FEZANDIPITI);
    return HasCaughtSpeciesFamily(gLegendarySignDefinitions[id].requiredSpecies);
}

void TryUnlockLocalLegendaryDiscovery(void)
{
    enum LegendarySignId id = gSpecialVar_0x8004;
    u16 map = ((u8)gSaveBlock1Ptr->location.mapGroup << 8) | (u8)gSaveBlock1Ptr->location.mapNum;
    bool32 helped = FALSE;

    gSpecialVar_Result = FALSE;
    if (id != LEGENDARY_SIGN_MELOETTA && id != LEGENDARY_SIGN_LANDORUS)
        return;
    if (map != gLegendarySignDefinitions[id].mapId || !MeetsSignProgression(id)
     || IsLegendarySignUnlocked(id) || IsLegendarySignCaught(id))
        return;
    if (id == LEGENDARY_SIGN_LANDORUS)
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

bool32 IsLegendarySignOrdinaryWildSpecies(enum Species species)
{
    enum LegendarySignId signId = GetLegendarySignIdBySpecies(species);

    return signId < LEGENDARY_SIGN_COUNT
        && gLegendarySignDefinitions[signId].source == LEGENDARY_SOURCE_ORDINARY_WILD;
}

bool32 IsNativeWildLegendarySpecies(enum Species species)
{
    enum LegendarySignId id = GetLegendarySignIdBySpecies(species);
    return id < LEGENDARY_SIGN_COUNT
        && gLegendarySignDefinitions[id].source == LEGENDARY_SOURCE_NATIVE_WILD;
}

bool32 CanAcquireLegendarySignSpecies(enum Species species)
{
    enum LegendarySignId id = GetLegendarySignIdBySpecies(species);
    return id >= LEGENDARY_SIGN_COUNT
        || (!IsLegendarySignCaught(id) && MeetsSignProgression(id) && MeetsSignDiscovery(id));
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

void RetryPendingLegendaryRelics(void)
{
    u32 state = GetLegendaryRelicDeliveryState();
    for (u32 item = 0; item < ARRAY_COUNT(sLegendaryRelicItems); item++)
    {
        if ((state & (1u << item)) && GiveLegendaryRelicItem(sLegendaryRelicItems[item]))
            state &= ~(1u << item);
    }
    SetLegendaryRelicDeliveryState(state);
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
            // Only a real acquisition with failed insertion creates debt.
            if (!GiveLegendaryRelicItem(sLegendaryRelicItems[item]))
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

void DoesPlayerPartyHaveSelectedSpeciesFamily(void)
{
    gSpecialVar_Result = PlayerPartyHasSpeciesFamily(gSpecialVar_0x8004);
}

static u8 GetSignLevel(s8 offset)
{
    s32 level = (s32)GetCurrentLevelCap() + offset;

    if (level < 1)
        level = 1;
    if (level > MAX_LEVEL)
        level = MAX_LEVEL;
    return level;
}



void TryUnlockSelectedLegendarySign(void)
{
    enum LegendarySignId id = gSpecialVar_0x8004;
    gSpecialVar_Result = 0;
    if (id >= LEGENDARY_SIGN_COUNT)
        return;
    if (IsLegendarySignCaught(id))
        gSpecialVar_Result = 4;
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
    else if (CanAcquireLegendarySignSpecies(gLegendarySignDefinitions[id].species))
        gSpecialVar_Result = IsSignResting(id) ? 3 : 1;
    else
        gSpecialVar_Result = 0;
    return gSpecialVar_Result;
}

u16 ShouldShowSelectedLegendarySignObject(void)
{
    enum LegendarySignId id = gSpecialVar_0x8004;
    gSpecialVar_Result = id < LEGENDARY_SIGN_COUNT && !IsLegendarySignCaught(id);
    return gSpecialVar_Result;
}

u16 GetSelectedLegendarySignLevel(void)
{
    enum LegendarySignId signId = gSpecialVar_0x8004;
    s8 offset = 2;

    if (signId < LEGENDARY_SIGN_COUNT)
        offset = gLegendarySignDefinitions[signId].levelOffset;
    gSpecialVar_Result = GetSignLevel(offset);
    return gSpecialVar_Result;
}

void CreateSelectedLegendarySignEncounter(void)
{
    enum LegendarySignId id = gSpecialVar_0x8004;
    gSpecialVar_Result = FALSE;
    if (id >= LEGENDARY_SIGN_COUNT || IsSignResting(id)
     || !CanAcquireLegendarySignSpecies(gLegendarySignDefinitions[id].species))
        return;
    u16 map = ((u8)gSaveBlock1Ptr->location.mapGroup << 8) | (u8)gSaveBlock1Ptr->location.mapNum;
    if (gLegendarySignDefinitions[id].mapId != map)
        return;
    sEncounterSignPlusOne = id + 1;
    UnlockLegendarySign(id);
    CreateScriptedWildMon(gLegendarySignDefinitions[id].species,
        GetSignLevel(gLegendarySignDefinitions[id].levelOffset), ITEM_NONE);
    ApplyEmeraldChampionsRandomNonMegaSet(&gParties[B_TRAINER_OPPONENT_A][0]);
    gSpecialVar_Result = TRUE;
}

void CreateEmeraldChampionsStaticLegendaryEncounter(void)
{
    enum Species species = gSpecialVar_0x8004;
    s16 levelOffset = gSpecialVar_0x8005;

    if (species == SPECIES_NONE || species >= NUM_SPECIES)
        return;
    CreateScriptedWildMon(species, GetSignLevel(levelOffset), ITEM_NONE);
    ApplyEmeraldChampionsRandomNonMegaSet(&gParties[B_TRAINER_OPPONENT_A][0]);
    if (species == SPECIES_LATIAS || species == SPECIES_LATIOS)
    {
        // Southern Island keeps its event origin and Soul Dew at the live cap.
        u32 fateful = TRUE;
        u16 item = ITEM_SOUL_DEW;
        SetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_MODERN_FATEFUL_ENCOUNTER, &fateful);
        SetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_HELD_ITEM, &item);
    }
}

void TryGiveSelectedLegendarySignReward(void)
{
    enum LegendarySignId signId = gSpecialVar_0x8004;
    u8 giveResult;

    gSpecialVar_Result = 0;
    if (signId >= LEGENDARY_SIGN_COUNT || IsLegendarySignCaught(signId))
        return;
    giveResult = GiveLegendarySignReward(
        gLegendarySignDefinitions[signId].species,
        GetSignLevel(gLegendarySignDefinitions[signId].levelOffset));
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

// Standard encounters give each native resident 3% and each quest discovery 1%.
// Sweet Scent shares one 25% roll evenly across both eligible groups.
enum Species ChooseRareWildLegendarySpecies(enum WildPokemonArea area, bool32 sweetScent)
{
    u16 map = ((u8)gSaveBlock1Ptr->location.mapGroup << 8) | (u8)gSaveBlock1Ptr->location.mapNum;
    enum WildPokemonArea habitat = WILD_AREA_LAND;
    enum Species native[LEGENDARY_SIGN_COUNT];
    enum Species quests[LEGENDARY_SIGN_COUNT];
    u32 nativeCount = 0, questCount = 0;
    u32 roll;

    if (map == MAP_ROUTE125 || map == MAP_ROUTE126 || map == MAP_ROUTE127)
        habitat = WILD_AREA_WATER;
    if (area != habitat)
        return SPECIES_NONE;
    for (enum LegendarySignId id = 0; id < LEGENDARY_SIGN_COUNT; id++)
    {
        const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[id];
        if (sign->mapId != map || !CanAcquireLegendarySignSpecies(sign->species))
            continue;
        if (sign->source == LEGENDARY_SOURCE_NATIVE_WILD)
            native[nativeCount++] = sign->species;
        else if (sign->source == LEGENDARY_SOURCE_RARE_WILD)
            quests[questCount++] = sign->species;
    }
    if (nativeCount == 0 && questCount == 0)
        return SPECIES_NONE;
    roll = RandomUniform(RNG_NONE, 0, 99);
    if (sweetScent)
    {
        if (roll >= 25)
            return SPECIES_NONE;
        u32 choice = RandomUniform(RNG_WILD_MON_TARGET, 0, nativeCount + questCount - 1);
        return choice < nativeCount ? native[choice] : quests[choice - nativeCount];
    }
    if (roll < nativeCount * 3)
        return native[roll / 3];
    roll -= nativeCount * 3;
    if (roll < questCount)
        return quests[roll];
    return SPECIES_NONE;
}

// A sign advances through local requirements as ordinary text pages.
void BufferNextLocalLegendaryRequirement(void)
{
    u16 map = ((u8)gSaveBlock1Ptr->location.mapGroup << 8) | (u8)gSaveBlock1Ptr->location.mapNum;
    for (u32 id = gSpecialVar_0x8004; id < LEGENDARY_SIGN_COUNT; id++)
    {
        if (gLegendarySignDefinitions[id].source != LEGENDARY_SOURCE_RARE_WILD
         || gLegendarySignDefinitions[id].mapId != map)
            continue;
        gSpecialVar_0x8004 = id;
        ResearchSelectedLegendarySign();
        gSpecialVar_0x8004 = id + 1;
        gSpecialVar_Result = TRUE;
        return;
    }
    gSpecialVar_Result = FALSE;
}

static void AppendLegendaryProgressionRequirements(enum LegendarySignId id)
{
    const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[id];
    if (GetBadgeCountForLegendarySigns() < sign->minimumBadges)
    {
        ConvertIntToDecimalStringN(gStringVar1, sign->minimumBadges, STR_CONV_MODE_LEFT_ALIGN, 1);
        u8 buffer[80];
        StringExpandPlaceholders(buffer, COMPOUND_STRING("\pGym Badges required:\n{STR_VAR_1}."));
        StringAppend(gStringVar4, buffer);
    }
    if (sign->requiredFlag == FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE && !FlagGet(sign->requiredFlag))
        StringAppend(gStringVar4, COMPOUND_STRING("\pFirst, resolve the weather crisis\nin Sootopolis."));
    else if (sign->requiredFlag == FLAG_RECEIVED_WATTSON_ELECTIRIZER && !FlagGet(sign->requiredFlag))
        StringAppend(gStringVar4, COMPOUND_STRING("\pHelp Wattson stop New Mauville's\ngenerator, then report back to him."));
    else if (sign->requiredFlag == FLAG_HIDE_ROUTE_119_TEAM_AQUA && !FlagGet(sign->requiredFlag))
        StringAppend(gStringVar4, COMPOUND_STRING("\pFirst, rescue the Weather Institute\nresearchers from Team Aqua."));
    else if (sign->requiredFlag == FLAG_GROUDON_AWAKENED_MAGMA_HIDEOUT && !FlagGet(sign->requiredFlag))
        StringAppend(gStringVar4, COMPOUND_STRING("\pFirst, confront Maxie deep inside\nthe Magma Hideout."));
    else if (sign->requiredFlag == FLAG_RECEIVED_RED_OR_BLUE_ORB && !FlagGet(sign->requiredFlag))
        StringAppend(gStringVar4, COMPOUND_STRING("\pFirst, help the elders at Mt. Pyre\nand receive the Magma Emblem."));
}

void ResearchSelectedLegendarySign(void)
{
    u16 selection = gSpecialVar_0x8004;
    gSpecialVar_Result = 0;
    if (selection >= LEGENDARY_SIGN_COUNT)
        return;
    enum LegendarySignId id = selection;
    const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[id];
    StringCopy(gStringVar1, GetSpeciesName(sign->requiredSpecies));
    StringCopy(gStringVar2, GetLegendaryDisplayName(sign->species));
    StringCopy(gStringVar3, GetLegendarySignLocationName(id));
    StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("{STR_VAR_2}\n{STR_VAR_3}."));
    if (IsLegendarySignCaught(id))
    {
        StringAppend(gStringVar4, COMPOUND_STRING("\pCaught! This discovery is recorded."));
        gSpecialVar_Result = 4;
        return;
    }
    if (sign->source == LEGENDARY_SOURCE_NATIVE_WILD)
    {
        StringAppend(gStringVar4, COMPOUND_STRING("\pA wild resident: 3% per encounter."));
        StringAppend(gStringVar4, sign->mapId == MAP_ROUTE125
            ? COMPOUND_STRING("\pSearch while SURFING here.")
            : COMPOUND_STRING("\pSearch the wild Pokémon here."));
        StringAppend(gStringVar4, COMPOUND_STRING("\pNo special requirements. Once\ncaught, it stops appearing here.\pSWEET SCENT: a shared 25% chance\namong eligible, uncaught legends."));
        gSpecialVar_Result = 0;
        return;
    }
    if (!MeetsSignProgression(id))
    {
        AppendLegendaryProgressionRequirements(id);
        return;
    }
    if (!MeetsSignDiscovery(id))
    {
        if (id == LEGENDARY_SIGN_MARSHADOW)
            StringAppend(gStringVar4, COMPOUND_STRING("\pCollect 250 soot in total, then\nspeak to Route 113's glassmaker.\lSpending soot keeps your progress."));
        else if (id == LEGENDARY_SIGN_MELOETTA)
            StringAppend(gStringVar4, COMPOUND_STRING("\pHelp Dewford Meadow's warden perform\na song. Bring a Pokémon with SING."));
        else if (id == LEGENDARY_SIGN_LANDORUS)
            StringAppend(gStringVar4, COMPOUND_STRING("\pBring CASTFORM to the archaeologist\nat Route 111's elevated ruins.\lHelp with the local weather survey."));
        else if (id == LEGENDARY_SIGN_KYUREM)
            StringAppend(gStringVar4, COMPOUND_STRING("\pFirst, catch RESHIRAM or ZEKROM.\nEither discovery is enough."));
        else if (id == LEGENDARY_SIGN_REGIGIGAS)
            StringAppend(gStringVar4, COMPOUND_STRING("\pBring REGIROCK, REGICE and REGISTEEL\ntogether to the statue."));
        else if (id == LEGENDARY_SIGN_PECHARUNT)
            StringAppend(gStringVar4, COMPOUND_STRING("\pCatch OKIDOGI, MUNKIDORI and\nFEZANDIPITI, then inspect the shrine."));
        else
        {
            u8 buffer[180];
            StringExpandPlaceholders(buffer, COMPOUND_STRING("\pA clue points to this family:\n{STR_VAR_1}.\pBefriend a member of that family.\nA caught Pokédex entry is enough;\lyour partner can stay in the PC."));
            StringAppend(gStringVar4, buffer);
        }
        gSpecialVar_Result = 1;
        return;
    }
    switch (sign->source)
    {
    case LEGENDARY_SOURCE_GAME_CORNER:
        StringAppend(gStringVar4, COMPOUND_STRING("\pAsk the Pokémon prize counter at\nMauville's Game Corner."));
        return;
    case LEGENDARY_SOURCE_CIRCUIT:
        StringAppend(gStringVar4, COMPOUND_STRING("\pEarn its Champions Circuit milestone\nat the Battle Frontier's Battle Tower.\pThe ferry opens with six badges.\pLifetime wins count. The attendant\nkeeps an earned reward until claimed."));
        return;
    case LEGENDARY_SOURCE_MASTERY:
        StringAppend(gStringVar4, id == LEGENDARY_SIGN_ARCEUS
            ? COMPOUND_STRING("\pResolve the weather crisis and earn\nall eight badges, then visit Devon.\pNo Pokédex completion is needed.")
            : COMPOUND_STRING("\pWin 40 Circuit battles in total and\nclaim its other legendary rewards."));
        return;
    case LEGENDARY_SOURCE_BREEDING:
        StringAppend(gStringVar4, COMPOUND_STRING("\pLeave MANAPHY and DITTO together\nat Route 117's Day Care.\pHatch their Egg to meet PHIONE."));
        return;
    case LEGENDARY_SOURCE_RARE_WILD:
        StringAppend(gStringVar4, COMPOUND_STRING("\pAvailable now!"));
        StringAppend(gStringVar4, (sign->mapId == MAP_ROUTE125 || sign->mapId == MAP_ROUTE126 || sign->mapId == MAP_ROUTE127)
            ? COMPOUND_STRING("\pSearch while SURFING in this area.\nThis Pokémon has a 1% encounter chance.")
            : COMPOUND_STRING("\pSearch the wild Pokémon in this area.\nThis Pokémon has a 1% encounter chance."));
        StringAppend(gStringVar4, COMPOUND_STRING("\pSWEET SCENT gives a 25% chance of\na legendary encounter here.\pEligible local legendaries share\nthat chance equally. Their badge and\lother requirements still apply."));
        StringAppend(gStringVar4, COMPOUND_STRING("\pIf it escapes or faints, keep looking.\nIt can appear again until caught."));
        return;
    case LEGENDARY_SOURCE_ORDINARY_WILD:
        StringAppend(gStringVar4, COMPOUND_STRING("\pSearch the wild Pokémon in this area.\nNo research visit is needed."));
        return;
    default:
        break;
    }
    if (id == LEGENDARY_SIGN_MAGEARNA)
    {
        StringAppend(gStringVar4, COMPOUND_STRING("\pSpeak to Devon's dream researcher\nabout his mechanical prototype."));
        return;
    }
    if (IsSignResting(id))
    {
        StringAppend(gStringVar4, COMPOUND_STRING("\pResting after your last encounter.\nLeave this area and return to retry."));
        gSpecialVar_Result = 5;
        return;
    }
    StringAppend(gStringVar4, COMPOUND_STRING("\pReady! Approach the Pokémon or\ninspect its shrine in this area."));
    gSpecialVar_Result = 2;
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
    return FlagGet(FLAG_EC_CAUGHT_HEATRAN) ? 0 : 1;
}

void BufferNextCenterLegendaryLead(void)
{
    for (u32 i = gSpecialVar_0x8004; i < ARRAY_COUNT(sCenterLegendaryLeads); i++)
    {
        if (sCenterLegendaryLeads[i].city != gMapHeader.regionMapSectionId)
            continue;
        enum LegendarySignId id = sCenterLegendaryLeads[i].id;
        gSpecialVar_0x8004 = i + 1;
        StringCopy(gStringVar4, sCenterLegendaryLeads[i].lead);
        if (id == LEGENDARY_SIGN_SHAYMIN && !FlagGet(FLAG_ADVENTURE_STARTED))
            StringCopy(gStringVar4, COMPOUND_STRING("After you battle the PROF.'s kid,\nreturn to BIRCH for your send-off.\pThen head west to ROUTE 102.\nSHAYMIN lives in its grass."));
        if (id == LEGENDARY_SIGN_COUNT)
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
                StringCopy(gStringVar4, COMPOUND_STRING("HEATRAN is awake in the deepest\nroom of SCORCHED SLAB.\pIf it fled after a battle, leave\nthat room and return to find it."));
            if (caught)
            {
                StringCopy(gStringVar2, GetSpeciesName(species));
                StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("You've already found\n{STR_VAR_2}!\pThat's one local legend you have\nturned into a partner."));
            }
            gSpecialVar_Result = TRUE;
            return;
        }
        const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[id];
        if (IsLegendarySignCaught(id))
        {
            StringCopy(gStringVar2, GetLegendaryDisplayName(sign->species));
            StringCopy(gStringVar3, GetLegendarySignLocationName(id));
            StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("You've already found\n{STR_VAR_2}!\pThe local lead was:\n{STR_VAR_3}."));
        }
        else if (sign->source == LEGENDARY_SOURCE_NATIVE_WILD)
            StringAppend(gStringVar4, COMPOUND_STRING("\pIt has a 3% wild encounter chance.\nSWEET SCENT gives a shared 25%\lchance among uncaught local legends."));
        else if (!MeetsSignProgression(id))
            AppendLegendaryProgressionRequirements(id);
        else if (!MeetsSignDiscovery(id))
            StringAppend(gStringVar4, COMPOUND_STRING("\pThat discovery is still waiting\nfor your help."));
        else if (sign->source == LEGENDARY_SOURCE_RARE_WILD)
            StringAppend(gStringVar4, COMPOUND_STRING("\pYou've met its requirements!\nYou can search for it there now."));
        else if (sign->source == LEGENDARY_SOURCE_VISIBLE && id != LEGENDARY_SIGN_MAGEARNA)
            StringAppend(gStringVar4, COMPOUND_STRING("\pYou've met its requirements.\nVisit its resting place to meet it."));
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

void TryGiveArceusLegendarySignMasteryReward(void)
{
    u8 giveResult;

    gSpecialVar_Result = 0;
    if (IsLegendarySignCaught(LEGENDARY_SIGN_ARCEUS))
    {
        gSpecialVar_Result = 4;
        return;
    }
    if (!FlagGet(FLAG_BADGE08_GET) || !FlagGet(FLAG_SOOTOPOLIS_ARCHIE_MAXIE_LEAVE))
        return;

    // Devon's final campaign discovery is available before the Elite Four.
    UnlockLegendarySign(LEGENDARY_SIGN_ARCEUS);
    giveResult = GiveLegendarySignReward(SPECIES_ARCEUS, min(MAX_LEVEL, GetCurrentLevelCap()));
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
