#include "global.h"
#include "battle_setup.h"
#include "caps.h"
#include "data.h"
#include "daycare.h"
#include "emerald_champions_battle_sets.h"
#include "event_data.h"
#include "item.h"
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
static const u8 sSignLocationPetalburgWoods[] = _("Petalburg Woods");
static const u8 sSignLocationGraniteB2F[] = _("Granite Cave B2F");
static const u8 sSignLocationFieryPath[] = _("Fiery Path");
static const u8 sSignLocationMtPyre6F[] = _("Mt. Pyre's sixth floor");
static const u8 sSignLocationRoute111Desert[] = _("Route 111's desert");
static const u8 sSignLocationRoute111[] = _("Route 111");
static const u8 sSignLocationRoute120[] = _("Route 120");
static const u8 sSignLocationSeafloorRoom6[] = _("Seafloor Cavern Room 6");
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
static const u8 sSignLocationSeafloorApproach[] = _("Seafloor Cavern");
static const u8 sSignLocationRoute125[] = _("Route 125");
static const u8 sSignLocationRoute126[] = _("Route 126");
static const u8 sSignLocationRoute127[] = _("Route 127");
static const u8 sSignLocationVictoryRoadB1F[] = _("Victory Road B1F");
static const u8 sSignLocationMagmaHideout4F[] = _("Magma Hideout 4F");
static const u8 sSignLocationUnknown[] = _("an unknown place");

static const u8 *GetLegendarySignLocationName(enum LegendarySignId signId)
{
    switch (signId)
    {
    case LEGENDARY_SIGN_AZELF:
    case LEGENDARY_SIGN_CHIEN_PAO:
    case LEGENDARY_SIGN_KYUREM:
        return sSignLocationShoalIce;
    case LEGENDARY_SIGN_CELEBI:
        return sSignLocationPetalburgWoods;
    case LEGENDARY_SIGN_COBALION:
        return sSignLocationGraniteB2F;
    case LEGENDARY_SIGN_ENTEI:
        return sSignLocationFieryPath;
    case LEGENDARY_SIGN_GIRATINA:
        return sSignLocationMtPyre6F;
    case LEGENDARY_SIGN_OKIDOGI:
    case LEGENDARY_SIGN_CHI_YU:
        return sSignLocationAshenWoods;
    case LEGENDARY_SIGN_MUNKIDORI:
    case LEGENDARY_SIGN_MELOETTA:
        return sSignLocationDewfordMeadow;
    case LEGENDARY_SIGN_HOOPA:
        return sSignLocationAlteringCave;
    case LEGENDARY_SIGN_LANDORUS:
        return sSignLocationRoute111;
    case LEGENDARY_SIGN_MESPRIT:
    case LEGENDARY_SIGN_XERNEAS:
        return sSignLocationRoute120;
    case LEGENDARY_SIGN_PALKIA:
        return sSignLocationSeafloorRoom6;
    case LEGENDARY_SIGN_RAIKOU:
    case LEGENDARY_SIGN_TAPU_KOKO:
    case LEGENDARY_SIGN_THUNDURUS:
        return sSignLocationRoute110;
    case LEGENDARY_SIGN_REGIDRAGO:
    case LEGENDARY_SIGN_COSMOG:
        return sSignLocationMeteor1F2R;
    case LEGENDARY_SIGN_REGIELEKI:
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
        return sSignLocationSeafloorApproach;
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
        if (!IsLegendarySignUnlocked(signId) || IsLegendarySignCaught(signId))
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

void UnlockLegendarySign(enum LegendarySignId signId)
{
    SetLegendaryStateBit(VAR_LEGENDARY_SIGNS_UNLOCKED_0, signId);
    switch (signId)
    {
    case LEGENDARY_SIGN_ARTICUNO:
        FlagClear(FLAG_EC_CAUGHT_ARTICUNO);
        break;
    case LEGENDARY_SIGN_CELEBI:
        FlagClear(FLAG_EC_CAUGHT_CELEBI);
        break;
    case LEGENDARY_SIGN_DARKRAI:
        FlagClear(FLAG_HIDE_LEGENDARY_SIGN_DARKRAI);
        break;
    case LEGENDARY_SIGN_CRESSELIA:
        FlagClear(FLAG_HIDE_LEGENDARY_SIGN_CRESSELIA);
        break;
    case LEGENDARY_SIGN_DIALGA:
        FlagClear(FLAG_HIDE_LEGENDARY_SIGN_DIALGA);
        break;
    case LEGENDARY_SIGN_HOOPA:
        FlagClear(FLAG_EC_CAUGHT_HOOPA);
        break;
    case LEGENDARY_SIGN_MELOETTA:
        FlagClear(FLAG_EC_CAUGHT_MELOETTA);
        break;
    case LEGENDARY_SIGN_MEWTWO:
        FlagClear(FLAG_EC_CAUGHT_MEWTWO);
        break;
    case LEGENDARY_SIGN_PALKIA:
        FlagClear(FLAG_EC_CAUGHT_PALKIA);
        break;
    case LEGENDARY_SIGN_PECHARUNT:
        FlagClear(FLAG_EC_CAUGHT_PECHARUNT);
        break;
    case LEGENDARY_SIGN_RESHIRAM:
        FlagClear(FLAG_EC_CAUGHT_RESHIRAM);
        break;
    case LEGENDARY_SIGN_SHAYMIN:
        FlagClear(FLAG_EC_CAUGHT_SHAYMIN);
        break;
    case LEGENDARY_SIGN_TERAPAGOS:
        FlagClear(FLAG_EC_CAUGHT_TERAPAGOS);
        break;
    case LEGENDARY_SIGN_ZAPDOS:
        FlagClear(FLAG_EC_CAUGHT_ZAPDOS);
        break;
    default:
        break;
    }
}

enum LegendarySignId GetLegendarySignIdBySpecies(enum Species species)
{
    for (enum LegendarySignId signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
        if (gLegendarySignDefinitions[signId].species == species)
            return signId;
    return LEGENDARY_SIGN_COUNT;
}

bool32 IsLegendarySignOrdinaryWildSpecies(enum Species species)
{
    enum LegendarySignId signId = GetLegendarySignIdBySpecies(species);

    return signId < LEGENDARY_SIGN_COUNT
        && gLegendarySignDefinitions[signId].source == LEGENDARY_SOURCE_ORDINARY_WILD;
}

bool32 IsLegendarySignConditionalWildSpecies(enum Species species)
{
    enum LegendarySignId signId = GetLegendarySignIdBySpecies(species);

    return signId < LEGENDARY_SIGN_COUNT
        && gLegendarySignDefinitions[signId].source == LEGENDARY_SOURCE_CONDITIONAL_WILD;
}

static void GiveLegendaryRelicItem(enum Item item)
{
    if (CheckBagHasItem(item, 1) || CheckPCHasItem(item, 1))
        return;
    if (!AddBagItem(item, 1))
        AddPCItem(item, 1);
}

static void GiveLegendaryRelicsForSpecies(enum Species species)
{
    static const enum Item sArceusPlates[] =
    {
        ITEM_FLAME_PLATE,
        ITEM_SPLASH_PLATE,
        ITEM_ZAP_PLATE,
        ITEM_MEADOW_PLATE,
        ITEM_ICICLE_PLATE,
        ITEM_FIST_PLATE,
        ITEM_TOXIC_PLATE,
        ITEM_EARTH_PLATE,
        ITEM_SKY_PLATE,
        ITEM_MIND_PLATE,
        ITEM_INSECT_PLATE,
        ITEM_STONE_PLATE,
        ITEM_SPOOKY_PLATE,
        ITEM_DRACO_PLATE,
        ITEM_DREAD_PLATE,
        ITEM_IRON_PLATE,
        ITEM_PIXIE_PLATE,
    };

    switch (species)
    {
    case SPECIES_GROUDON:
        GiveLegendaryRelicItem(ITEM_RED_ORB);
        break;
    case SPECIES_KYOGRE:
        GiveLegendaryRelicItem(ITEM_BLUE_ORB);
        break;
    case SPECIES_ZACIAN:
        GiveLegendaryRelicItem(ITEM_RUSTED_SWORD);
        break;
    case SPECIES_ZAMAZENTA:
        GiveLegendaryRelicItem(ITEM_RUSTED_SHIELD);
        break;
    case SPECIES_OGERPON_TEAL:
        GiveLegendaryRelicItem(ITEM_WELLSPRING_MASK);
        GiveLegendaryRelicItem(ITEM_HEARTHFLAME_MASK);
        GiveLegendaryRelicItem(ITEM_CORNERSTONE_MASK);
        break;
    case SPECIES_ARCEUS:
        for (u32 i = 0; i < ARRAY_COUNT(sArceusPlates); i++)
            GiveLegendaryRelicItem(sArceusPlates[i]);
        break;
    default:
        break;
    }
}

void TryUnlockEligibleVisibleLegendarySignsForCurrentMap(void)
{
    u16 currentMap = ((u8)gSaveBlock1Ptr->location.mapGroup << 8) | (u8)gSaveBlock1Ptr->location.mapNum;

    for (enum LegendarySignId signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
    {
        const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[signId];

        if (sign->source != LEGENDARY_SOURCE_VISIBLE
         || sign->mapId != currentMap
         || IsLegendarySignUnlocked(signId)
         || IsLegendarySignCaught(signId)
         || signId == LEGENDARY_SIGN_PECHARUNT
         || signId == LEGENDARY_SIGN_REGIGIGAS)
            continue;
        if (GetBadgeCountForLegendarySigns() < sign->minimumBadges
         || (sign->requiredFlag != 0 && !FlagGet(sign->requiredFlag))
         || !PlayerPartyHasSpeciesFamily(sign->requiredSpecies))
            continue;
        UnlockLegendarySign(signId);
    }
}

void MarkLegendarySignCaughtBySpecies(enum Species species)
{
    enum LegendarySignId signId = GetLegendarySignIdBySpecies(species);

    // Form-defining relics are earned with their Pokémon, never synthesized
    // by the free held-item vendor or a tutor preset.  This call is
    // idempotent across Bag/PC storage and also covers Groudon/Kyogre, whose
    // canonical Emerald encounters are not Legendary Sign rows.
    GiveLegendaryRelicsForSpecies(species);

    if (signId >= LEGENDARY_SIGN_COUNT)
        return;
    UnlockLegendarySign(signId);
    SetLegendaryStateBit(VAR_LEGENDARY_SIGNS_CAUGHT_0, signId);
    switch (signId)
    {
    case LEGENDARY_SIGN_ARTICUNO:
        FlagSet(FLAG_EC_CAUGHT_ARTICUNO);
        break;
    case LEGENDARY_SIGN_CELEBI:
        FlagSet(FLAG_EC_CAUGHT_CELEBI);
        break;
    case LEGENDARY_SIGN_DARKRAI:
        FlagSet(FLAG_HIDE_LEGENDARY_SIGN_DARKRAI);
        break;
    case LEGENDARY_SIGN_CRESSELIA:
        FlagSet(FLAG_HIDE_LEGENDARY_SIGN_CRESSELIA);
        break;
    case LEGENDARY_SIGN_DIALGA:
        FlagSet(FLAG_HIDE_LEGENDARY_SIGN_DIALGA);
        break;
    case LEGENDARY_SIGN_HOOPA:
        FlagSet(FLAG_EC_CAUGHT_HOOPA);
        break;
    case LEGENDARY_SIGN_MELOETTA:
        FlagSet(FLAG_EC_CAUGHT_MELOETTA);
        break;
    case LEGENDARY_SIGN_MEWTWO:
        FlagSet(FLAG_EC_CAUGHT_MEWTWO);
        break;
    case LEGENDARY_SIGN_PALKIA:
        FlagSet(FLAG_EC_CAUGHT_PALKIA);
        break;
    case LEGENDARY_SIGN_PECHARUNT:
        FlagSet(FLAG_EC_CAUGHT_PECHARUNT);
        break;
    case LEGENDARY_SIGN_RESHIRAM:
        FlagSet(FLAG_EC_CAUGHT_RESHIRAM);
        break;
    case LEGENDARY_SIGN_SHAYMIN:
        FlagSet(FLAG_EC_CAUGHT_SHAYMIN);
        break;
    case LEGENDARY_SIGN_TERAPAGOS:
        FlagSet(FLAG_EC_CAUGHT_TERAPAGOS);
        break;
    case LEGENDARY_SIGN_ZAPDOS:
        FlagSet(FLAG_EC_CAUGHT_ZAPDOS);
        break;
    default:
        break;
    }
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
         || IsLegendarySignCaught(signId))
            continue;

        // Devon can reveal these Signs remotely, but it is not a required hub.
        // Returning to the marked place with the depicted partner wakes the
        // Sign locally under the same story and Badge requirements.
        if (!IsLegendarySignUnlocked(signId))
        {
            if (GetBadgeCountForLegendarySigns() < sign->minimumBadges
             || (sign->requiredFlag != 0 && !FlagGet(sign->requiredFlag))
             || !PlayerPartyHasSpeciesFamily(sign->requiredSpecies))
                continue;
            UnlockLegendarySign(signId);
        }
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
    UnlockLegendarySign(signId);
    gSpecialVar_Result = 2;
}

u16 GetSelectedLegendarySignState(void)
{
    enum LegendarySignId signId = gSpecialVar_0x8004;

    if (signId >= LEGENDARY_SIGN_COUNT || !IsLegendarySignUnlocked(signId))
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
                      && !IsLegendarySignCaught(signId);
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
    if (IsLegendarySignCaught(LEGENDARY_SIGN_DARKRAI))
    {
        gSpecialVar_Result = 4;
        return;
    }
    if (IsLegendarySignUnlocked(LEGENDARY_SIGN_DARKRAI))
    {
        gSpecialVar_Result = 3;
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
    UnlockLegendarySign(LEGENDARY_SIGN_DARKRAI);
    gSpecialVar_Result = 2;
}

void TryDiscoverEligibleLegendarySign(void)
{
    enum LegendarySignId clueId = LEGENDARY_SIGN_COUNT;
    u8 badgeCount = GetBadgeCountForLegendarySigns();

    gSpecialVar_Result = 3;
    for (enum LegendarySignId signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
    {
        const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[signId];

        if (sign->source != LEGENDARY_SOURCE_CONDITIONAL_WILD
         || IsLegendarySignUnlocked(signId)
         || IsLegendarySignCaught(signId))
            continue;
        gSpecialVar_Result = 0;
        if (badgeCount < sign->minimumBadges
         || (sign->requiredFlag != 0 && !FlagGet(sign->requiredFlag)))
            continue;
        if (clueId == LEGENDARY_SIGN_COUNT)
            clueId = signId;
        if (!PlayerPartyHasSpeciesFamily(sign->requiredSpecies))
            continue;

        StringCopy(gStringVar1, GetSpeciesName(sign->requiredSpecies));
        StringCopy(gStringVar2, GetSpeciesName(sign->species));
        StringCopy(gStringVar3, GetLegendarySignLocationName(signId));
        UnlockLegendarySign(signId);
        gSpecialVar_Result = 2;
        return;
    }
    if (clueId < LEGENDARY_SIGN_COUNT)
    {
        const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[clueId];

        StringCopy(gStringVar1, GetSpeciesName(sign->requiredSpecies));
        StringCopy(gStringVar2, GetSpeciesName(sign->species));
        StringCopy(gStringVar3, GetLegendarySignLocationName(clueId));
        gSpecialVar_Result = 1;
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

        if (preset == NULL || preset->requiredItem != ITEM_NONE)
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

    giveResult = GiveLegendarySignReward(SPECIES_ARCEUS, min(MAX_LEVEL, GetCurrentLevelCap()));
    if (giveResult == MON_CANT_GIVE)
    {
        gSpecialVar_Result = 3;
        return;
    }
    StringCopy(gStringVar1, GetSpeciesName(SPECIES_ARCEUS));
    gSpecialVar_Result = giveResult == MON_GIVEN_TO_PARTY ? 1 : 2;
}
