#include "global.h"
#include "battle_setup.h"
#include "caps.h"
#include "data.h"
#include "daycare.h"
#include "emerald_champions_battle_sets.h"
#include "event_data.h"
#include "item.h"
#include "malloc.h"
#include "script_menu.h"
#include "legendary_signs.h"
#include "pokedex.h"
#include "pokemon.h"
#include "random.h"
#include "script_pokemon_util.h"
#include "string_util.h"
#include "constants/characters.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/maps.h"
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
static const u8 sSignLocationRoute117[] = _("Route 117");
static const u8 sSignLocationRoute123[] = _("Route 123");
static const u8 sSignLocationMtPyreExterior[] = _("Mt. Pyre's exterior");
static const u8 sSignLocationRoute119Land[] = _("Route 119's grass");
static const u8 sSignLocationMeteorB1F1R[] = _("Meteor Falls B1F");
static const u8 sSignLocationVictoryRoad1F[] = _("Victory Road 1F");
static const u8 sSignLocationPetalburgWoods2[] = _("deep Petalburg Woods");
static const u8 sSignLocationDesertUnderpass[] = _("Desert Underpass");
static const u8 sSignLocationSandstrewnB1F[] = _("Sandstrewn Ruins B1F");
static const u8 sSignLocationAshenWoods[] = _("Ashen Woods");
static const u8 sSignLocationVerdanturfMeadow[] = _("Verdanturf Meadow");
static const u8 sSignLocationRoute112[] = _("Route 112");
static const u8 sSignLocationRoute118[] = _("Route 118");
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
        return sSignLocationPetalburgWoods2;
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
    case LEGENDARY_SIGN_COSMOG:
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
        return sSignLocationRoute117;
    case LEGENDARY_SIGN_TAPU_BULU:
        return sSignLocationRoute123;
    case LEGENDARY_SIGN_TAPU_LELE:
    case LEGENDARY_SIGN_YVELTAL:
        return sSignLocationMtPyreExterior;
    case LEGENDARY_SIGN_TORNADUS:
        return sSignLocationRoute119Land;
    case LEGENDARY_SIGN_UXIE:
    case LEGENDARY_SIGN_CRESSELIA:
    case LEGENDARY_SIGN_DIALGA:
        return sSignLocationMeteorB1F1R;
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
        return sSignLocationRoute118;
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

#define SIGN_LEDGER_MAX_ENTRIES 9

static const u8 sText_LedgerAt[] = _(" waits at\n");
static const u8 sText_LedgerPage[] = _("\p");
static const u8 sText_LedgerMore[] = _("…and more SIGNS are awake elsewhere.");
static const u8 sText_LedgerEnd[] = _(".");

// Builds a readable list of every awakened, uncaught Sign into gStringVar4
// (one "SPECIES waits at\nLOCATION." page per Sign) and returns the count in
// gSpecialVar_Result, so the player can always find out what is pending
// without walking the whole region.
void BufferLegendarySignLedger(void)
{
    u8 *end = gStringVar4;
    u32 count = 0;

    *end = EOS;
    for (enum LegendarySignId signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
    {
        if (!IsLegendarySignUnlocked(signId) || IsLegendarySignCaught(signId)
         || IsLegendaryEncounterLost(gLegendarySignDefinitions[signId].species))
            continue;
        if (count == SIGN_LEDGER_MAX_ENTRIES)
        {
            end = StringAppend(end, sText_LedgerPage);
            StringAppend(end, sText_LedgerMore);
            count++;
            break;
        }
        if (count != 0)
            end = StringAppend(end, sText_LedgerPage);
        end = StringAppend(end, GetSpeciesName(gLegendarySignDefinitions[signId].species));
        end = StringAppend(end, sText_LedgerAt);
        end = StringAppend(end, GetLegendarySignLocationName(signId));
        end = StringAppend(end, sText_LedgerEnd);
        count++;
    }
    gSpecialVar_Result = count;
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
    if (objectFlag != 0 && !IsLegendarySignCaught(signId)
     && !IsLegendaryEncounterLost(gLegendarySignDefinitions[signId].species))
        FlagClear(objectFlag);
}

enum LegendarySignId GetLegendarySignIdBySpecies(enum Species species)
{
    for (enum LegendarySignId signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
        if (gLegendarySignDefinitions[signId].species == species)
            return signId;
    return LEGENDARY_SIGN_COUNT;
}

// Save indices are append-only. Reserve 0-95 for Signs and 96-127 for
// canonical encounters that predate the Sign ledger. Never use caught bits
// for a failed encounter: ownership-dependent quests must still know the difference.
static const enum Species sNativeLegendaryEncounters[] =
{
    SPECIES_GROUDON, SPECIES_KYOGRE, SPECIES_RAYQUAZA,
    SPECIES_REGIROCK, SPECIES_REGICE, SPECIES_REGISTEEL,
    SPECIES_LATIAS, SPECIES_LATIOS, SPECIES_LUGIA, SPECIES_HO_OH,
    SPECIES_MEW, SPECIES_DEOXYS, SPECIES_JIRACHI, SPECIES_DIANCIE,
    SPECIES_HEATRAN, SPECIES_MOLTRES,
};
// These existing event flags control physical presence, not Pokédex capture.
static const u16 sNativeLegendaryDefeatedFlags[] =
{
    FLAG_DEFEATED_GROUDON, FLAG_DEFEATED_KYOGRE, FLAG_DEFEATED_RAYQUAZA,
    FLAG_DEFEATED_REGIROCK, FLAG_DEFEATED_REGICE, FLAG_DEFEATED_REGISTEEL,
    0, 0, FLAG_DEFEATED_LUGIA, FLAG_DEFEATED_HO_OH,
    FLAG_DEFEATED_MEW, FLAG_DEFEATED_DEOXYS,
    FLAG_EC_CAUGHT_JIRACHI, FLAG_EC_CAUGHT_DIANCIE,
    FLAG_EC_CAUGHT_HEATRAN, FLAG_EC_CAUGHT_MOLTRES,
};
static const u16 sNativeLegendaryHideFlags[] =
{
    FLAG_HIDE_TERRA_CAVE_GROUDON, FLAG_HIDE_MARINE_CAVE_KYOGRE,
    FLAG_HIDE_SKY_PILLAR_TOP_RAYQUAZA_STILL,
    FLAG_HIDE_REGIROCK, FLAG_HIDE_REGICE, FLAG_HIDE_REGISTEEL,
    0, 0, FLAG_HIDE_LUGIA, FLAG_HIDE_HO_OH, FLAG_HIDE_MEW, FLAG_HIDE_DEOXYS,
    FLAG_EC_CAUGHT_JIRACHI, FLAG_EC_CAUGHT_DIANCIE,
    FLAG_EC_CAUGHT_HEATRAN, FLAG_EC_CAUGHT_MOLTRES,
};
STATIC_ASSERT(ARRAY_COUNT(sNativeLegendaryDefeatedFlags) == ARRAY_COUNT(sNativeLegendaryEncounters), LegendaryDefeatFlags);
STATIC_ASSERT(ARRAY_COUNT(sNativeLegendaryHideFlags) == ARRAY_COUNT(sNativeLegendaryEncounters), LegendaryHideFlags);
STATIC_ASSERT(LEGENDARY_SIGN_COUNT <= 96, LegendaryDefeatSignCapacity);
STATIC_ASSERT(ARRAY_COUNT(sNativeLegendaryEncounters) <= 32, LegendaryDefeatNativeCapacity);

static u32 GetLegendaryEncounterLossIndex(enum Species species)
{
    enum LegendarySignId signId;

    if (species == SPECIES_NONE || species >= NUM_SPECIES)
        return 128;
    species = GET_BASE_SPECIES_ID(species);
    if (!gSpeciesInfo[species].isRestrictedLegendary
     && !gSpeciesInfo[species].isSubLegendary
     && !gSpeciesInfo[species].isMythical
     && !gSpeciesInfo[species].isUltraBeast)
        return 128;
    species = GetEggSpecies(species);
    signId = GetLegendarySignIdBySpecies(species);
    if (signId < LEGENDARY_SIGN_COUNT)
        return signId;
    for (u32 i = 0; i < ARRAY_COUNT(sNativeLegendaryEncounters); i++)
        if (sNativeLegendaryEncounters[i] == species)
            return 96 + i;
    return 128;
}

bool32 IsLegendaryEncounterLost(enum Species species)
{
    u32 index = GetLegendaryEncounterLossIndex(species);

    return index < 128
        && (gSaveBlock2Ptr->pokedex.lostLegendaryEncounters[index / 8] & (1u << (index % 8)));
}

bool32 IsOneShotLegendarySpecies(enum Species species)
{
    return GetLegendaryEncounterLossIndex(species) < 128;
}

void MarkLegendaryEncounterLost(enum Species species)
{
    u32 index = GetLegendaryEncounterLossIndex(species);

    if (index < 128)
    {
        gSaveBlock2Ptr->pokedex.lostLegendaryEncounters[index / 8] |= 1u << (index % 8);
        if (index < LEGENDARY_SIGN_COUNT)
        {
            u16 objectFlag = GetLegendarySignObjectFlag(index);
            if (objectFlag != 0)
                FlagSet(objectFlag);
        }
        else if (index >= 96)
        {
            u32 nativeIndex = index - 96;
            if (nativeIndex == 0 || nativeIndex == 1)
                VarSet(VAR_SHOULD_END_ABNORMAL_WEATHER, 1);
            if (sNativeLegendaryDefeatedFlags[nativeIndex] != 0)
                FlagSet(sNativeLegendaryDefeatedFlags[nativeIndex]);
            if (sNativeLegendaryHideFlags[nativeIndex] != 0)
                FlagSet(sNativeLegendaryHideFlags[nativeIndex]);
            // The TV choice roams; only the other species owns Southern Island.
            if ((nativeIndex == 6 && VarGet(VAR_ROAMER_POKEMON) != 0)
             || (nativeIndex == 7 && VarGet(VAR_ROAMER_POKEMON) == 0))
            {
                FlagSet(FLAG_DEFEATED_LATIAS_OR_LATIOS);
                FlagSet(FLAG_HIDE_SOUTHERN_ISLAND_UNCHOSEN_EON_DUO_MON);
            }
        }
    }
}

bool32 IsLegendarySignOrdinaryWildSpecies(enum Species species)
{
    enum LegendarySignId signId = GetLegendarySignIdBySpecies(species);

    return signId < LEGENDARY_SIGN_COUNT
        && gLegendarySignDefinitions[signId].source == LEGENDARY_SOURCE_ORDINARY_WILD;
}

bool32 CanAcquireLegendarySignSpecies(enum Species species)
{
    enum LegendarySignId signId = GetLegendarySignIdBySpecies(species);

    if (IsLegendaryEncounterLost(species))
        return FALSE;
    return signId >= LEGENDARY_SIGN_COUNT
        || (IsLegendarySignUnlocked(signId) && !IsLegendarySignCaught(signId));
}

bool32 IsLegendarySignConditionalWildSpecies(enum Species species)
{
    enum LegendarySignId signId = GetLegendarySignIdBySpecies(species);

    return signId < LEGENDARY_SIGN_COUNT
        && gLegendarySignDefinitions[signId].source == LEGENDARY_SOURCE_CONDITIONAL_WILD;
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

// Migration never creates pending rewards from historical ownership. It only
// prevents a repeated caught-marker call from recreating discarded old relics.
void InitializeLegendaryRelicDeliveryState(void)
{
    u32 state = 0;
    for (u32 group = 0; group < ARRAY_COUNT(sLegendaryRelicGrants); group++)
    {
        if (GetSetPokedexFlag(SpeciesToNationalPokedexNum(sLegendaryRelicGrants[group].species), FLAG_GET_CAUGHT))
            state |= 1u << (LEGENDARY_RELIC_EARNED_SHIFT + group);
    }
    SetLegendaryRelicDeliveryState(state);
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
    enum LegendarySignId signId = GetLegendarySignIdBySpecies(species);
    u16 objectFlag;

    // Form-defining relics are earned with their Pokémon, never synthesized
    // by the free held-item vendor or a tutor preset.  This call is
    // earned once and persists any undelivered items, including Groudon/Kyogre, whose
    // canonical Emerald encounters are not Legendary Sign rows.
    GiveLegendaryRelicsForSpecies(species);

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

bool32 TryGetLegendarySignWildOverride(enum WildPokemonArea area, enum Species *species, u8 *level)
{
    u16 currentMap = ((u8)gSaveBlock1Ptr->location.mapGroup << 8) | (u8)gSaveBlock1Ptr->location.mapNum;

    for (enum LegendarySignId signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
    {
        const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[signId];

        if (sign->source != LEGENDARY_SOURCE_CONDITIONAL_WILD
         || sign->mapId != currentMap
         || sign->area != area
         || IsLegendarySignCaught(signId)
         || IsLegendaryEncounterLost(sign->species))
            continue;

        if (!IsLegendarySignUnlocked(signId))
            continue;
        if (RandomUniform(RNG_NONE, 0, 99) >= sign->chance)
            continue;

        *species = sign->species;
        *level = GetSignLevel(sign->levelOffset);
        return TRUE;
    }
    return FALSE;
}

void TryUnlockSelectedLegendarySign(void)
{
    enum LegendarySignId signId = gSpecialVar_0x8004;
    const struct LegendarySignDefinition *sign;

    gSpecialVar_Result = 0;
    if (signId >= LEGENDARY_SIGN_COUNT)
        return;
    sign = &gLegendarySignDefinitions[signId];
    if (IsLegendaryEncounterLost(sign->species))
    {
        gSpecialVar_Result = 5;
        return;
    }
    if (IsLegendarySignCaught(signId))
    {
        gSpecialVar_Result = 4;
        return;
    }
    if (IsLegendarySignUnlocked(signId))
    {
        gSpecialVar_Result = 3;
        return;
    }
    if (GetBadgeCountForLegendarySigns() < sign->minimumBadges
     || (sign->requiredFlag != 0 && !FlagGet(sign->requiredFlag)))
        return;
    if (!PlayerPartyHasSpeciesFamily(sign->requiredSpecies))
    {
        gSpecialVar_Result = 1;
        return;
    }
    gSpecialVar_Result = 6; // Return to Devon to research this individual Sign.
}

u16 GetSelectedLegendarySignState(void)
{
    enum LegendarySignId signId = gSpecialVar_0x8004;

    if (signId < LEGENDARY_SIGN_COUNT
     && IsLegendaryEncounterLost(gLegendarySignDefinitions[signId].species))
        gSpecialVar_Result = 3;
    else if (signId >= LEGENDARY_SIGN_COUNT || !IsLegendarySignUnlocked(signId))
        gSpecialVar_Result = 0;
    else if (IsLegendarySignCaught(signId))
        gSpecialVar_Result = 2;
    else
        gSpecialVar_Result = 1;
    return gSpecialVar_Result;
}

u16 ShouldShowSelectedLegendarySignObject(void)
{
    enum LegendarySignId signId = gSpecialVar_0x8004;

    gSpecialVar_Result = signId < LEGENDARY_SIGN_COUNT
                      && IsLegendarySignUnlocked(signId)
                      && !IsLegendarySignCaught(signId)
                      && !IsLegendaryEncounterLost(gLegendarySignDefinitions[signId].species);
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
    enum LegendarySignId signId = gSpecialVar_0x8004;

    if (signId >= LEGENDARY_SIGN_COUNT)
        return;
    CreateScriptedWildMon(
        gLegendarySignDefinitions[signId].species,
        GetSignLevel(gLegendarySignDefinitions[signId].levelOffset),
        ITEM_NONE);
    ApplyEmeraldChampionsRandomNonMegaSet(&gParties[B_TRAINER_OPPONENT_A][0]);
}

void CreateEmeraldChampionsStaticLegendaryEncounter(void)
{
    enum Species species = gSpecialVar_0x8004;
    s16 levelOffset = gSpecialVar_0x8005;

    if (species == SPECIES_NONE || species >= NUM_SPECIES)
        return;
    CreateScriptedWildMon(species, GetSignLevel(levelOffset), ITEM_NONE);
    ApplyEmeraldChampionsRandomNonMegaSet(&gParties[B_TRAINER_OPPONENT_A][0]);
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
    static const u16 sMtPyreTrainers[] =
    {
        TRAINER_MARK,
        TRAINER_DEZ_AND_LUKE,
        TRAINER_LEAH,
        TRAINER_ZANDER,
        TRAINER_WILLIAM,
        TRAINER_KAYLA,
        TRAINER_GABRIELLE_1,
        TRAINER_ATSUSHI,
        TRAINER_TASHA,
        TRAINER_VALERIE_1,
        TRAINER_CEDRIC,
        TRAINER_GRUNT_MT_PYRE_1,
        TRAINER_GRUNT_MT_PYRE_2,
        TRAINER_GRUNT_MT_PYRE_3,
        TRAINER_GRUNT_MT_PYRE_4,
        TRAINER_MATT_MT_PYRE,
    };

    gSpecialVar_Result = 0;
    if (IsLegendaryEncounterLost(SPECIES_DARKRAI))
    {
        gSpecialVar_Result = 5;
        return;
    }
    if (IsLegendarySignCaught(LEGENDARY_SIGN_DARKRAI))
    {
        gSpecialVar_Result = 4;
        return;
    }
    if (!FlagGet(FLAG_RECEIVED_RED_OR_BLUE_ORB))
        return;
    for (u32 i = 0; i < ARRAY_COUNT(sMtPyreTrainers); i++)
        if (!HasTrainerBeenFought(sMtPyreTrainers[i]))
            return;
    if (!PlayerPartyHasSpeciesFamily(SPECIES_MUSHARNA))
    {
        gSpecialVar_Result = 1;
        return;
    }
    gSpecialVar_Result = IsLegendarySignUnlocked(LEGENDARY_SIGN_DARKRAI) ? 3 : 6;
}

void BuildLegendarySignResearchMenu(void)
{
    u8 badgeCount = GetBadgeCountForLegendarySigns();

    gSpecialVar_Result = 0;
    for (enum LegendarySignId signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
    {
        const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[signId];
        struct ListMenuItem item;
        u8 *name;

        // These two are researched and delivered personally by this scientist.
        if (signId == LEGENDARY_SIGN_MAGEARNA || signId == LEGENDARY_SIGN_ARCEUS
         || IsLegendarySignCaught(signId)
         || IsLegendaryEncounterLost(sign->species)
         || badgeCount < sign->minimumBadges
         || (sign->requiredFlag != 0 && !FlagGet(sign->requiredFlag)))
            continue;
        name = Alloc(StringLength(GetSpeciesName(sign->species)) + 5);
        StringCopy(name, GetSpeciesName(sign->species));
        if (IsLegendarySignUnlocked(signId))
            StringAppend(name, COMPOUND_STRING(" (R)"));
        item.name = name;
        item.id = signId;
        MultichoiceDynamic_PushElement(item);
        gSpecialVar_Result++;
    }
}

void ResearchSelectedLegendarySign(void)
{
    enum LegendarySignId signId = gSpecialVar_0x8004;
    const struct LegendarySignDefinition *sign;

    TryUnlockSelectedLegendarySign();
    if (signId >= LEGENDARY_SIGN_COUNT)
        return;
    sign = &gLegendarySignDefinitions[signId];
    StringCopy(gStringVar1, sign->requiredSpecies == SPECIES_NONE
        ? COMPOUND_STRING("Your research") : GetSpeciesName(sign->requiredSpecies));
    StringCopy(gStringVar2, GetSpeciesName(sign->species));
    StringCopy(gStringVar3, GetLegendarySignLocationName(signId));
    gSpecialVar_0x8005 = sign->source;
    switch (sign->source)
    {
    case LEGENDARY_SOURCE_GAME_CORNER:
        StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("{STR_VAR_2}'s Sign is translated.\pAsk the Pokémon prize counter at\nMauville's Game Corner about it."));
        break;
    case LEGENDARY_SOURCE_CIRCUIT:
    case LEGENDARY_SOURCE_MASTERY:
        StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("{STR_VAR_2}'s Sign is translated.\pEarn its Champions Circuit milestone\nat the Battle Frontier's Battle Tower.\pThe attendant will keep your earned\nreward until you return to claim it."));
        break;
    case LEGENDARY_SOURCE_BREEDING:
        StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("{STR_VAR_2}'s Sign is translated.\pLeave MANAPHY and DITTO at the\nDay Care on Route 117.\pTheir Egg can now carry PHIONE."));
        break;
    default:
        StringExpandPlaceholders(gStringVar4, COMPOUND_STRING("{STR_VAR_2}'s Sign is translated.\pReturn to\n{STR_VAR_3}.\pThe Sign can now answer you there."));
        if (signId == LEGENDARY_SIGN_DARKRAI)
            StringAppend(gStringVar4, COMPOUND_STRING("\pComplete every Trainer battle within\nMt. Pyre, then speak to the old man\lwith MUSHARNA's family in your party."));
        else if (signId == LEGENDARY_SIGN_REGIGIGAS)
            StringAppend(gStringVar4, COMPOUND_STRING("\pTake REGIROCK, REGICE, and REGISTEEL\nto the statue in the inner chamber."));
        else if (signId == LEGENDARY_SIGN_PECHARUNT)
            StringAppend(gStringVar4, COMPOUND_STRING("\pFirst capture OKIDOGI, MUNKIDORI,\nand FEZANDIPITI. Then return to\lthe shrine with OKIDOGI's family."));
        break;
    }
    if (gSpecialVar_Result == 6)
    {
        UnlockLegendarySign(signId);
        gSpecialVar_Result = 2;
    }
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
    for (enum LegendarySignId signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
    {
        if (signId != LEGENDARY_SIGN_ARCEUS && !IsLegendarySignCaught(signId))
            return;
    }

    // This final research is performed personally at Devon, after all other Signs.
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
