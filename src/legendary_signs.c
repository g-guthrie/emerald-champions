#include "global.h"
#include "event_data.h"
#include "battle_setup.h"
#include "data.h"
#include "legendary_signs.h"
#include "pokedex.h"
#include "pokemon.h"
#include "random.h"
#include "string_util.h"
#include "verdant_battle_sets.h"
#include "constants/flags.h"
#include "constants/maps.h"
#include "constants/opponents.h"
#include "constants/species.h"
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
static const u8 sSignLocationMirageTower[] = _("Mirage Tower's summit");
static const u8 sSignLocationRoute111[] = _("Route 111");
static const u8 sSignLocationRoute120[] = _("Route 120's water");
static const u8 sSignLocationSeafloorRoom6[] = _("Seafloor Cavern Room 6");
static const u8 sSignLocationRoute110[] = _("Route 110");
static const u8 sSignLocationMeteor1F2R[] = _("Meteor Falls' rear cave");
static const u8 sSignLocationNewMauville[] = _("New Mauville");
static const u8 sSignLocationScorchedB2F[] = _("Scorched Slab B2F");
static const u8 sSignLocationRoute117[] = _("Route 117");
static const u8 sSignLocationSafariNorth[] = _("the north Safari Zone");
static const u8 sSignLocationMtPyreExterior[] = _("Mt. Pyre's exterior");
static const u8 sSignLocationRoute119Land[] = _("Route 119's grass");
static const u8 sSignLocationRoute119Water[] = _("Route 119's water");
static const u8 sSignLocationMeteorB1F1R[] = _("Meteor Falls B1F");
static const u8 sSignLocationVictoryRoad1F[] = _("Victory Road 1F");
static const u8 sSignLocationPetalburgWoods2[] = _("deep Petalburg Woods");
static const u8 sSignLocationDesertUnderpass[] = _("Desert Underpass");
static const u8 sSignLocationUnknown[] = _("an unknown place");

static const u8 *GetLegendarySignLocationName(u8 signId)
{
    switch (signId)
    {
    case LEGENDARY_SIGN_AZELF:
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
    case LEGENDARY_SIGN_HOOPA:
        return sSignLocationMirageTower;
    case LEGENDARY_SIGN_LANDORUS:
        return sSignLocationRoute111;
    case LEGENDARY_SIGN_MESPRIT:
        return sSignLocationRoute120;
    case LEGENDARY_SIGN_PALKIA:
        return sSignLocationSeafloorRoom6;
    case LEGENDARY_SIGN_RAIKOU:
    case LEGENDARY_SIGN_TAPU_KOKO:
        return sSignLocationRoute110;
    case LEGENDARY_SIGN_REGIDRAGO:
        return sSignLocationMeteor1F2R;
    case LEGENDARY_SIGN_REGIELEKI:
    case LEGENDARY_SIGN_ZEKROM:
    case LEGENDARY_SIGN_ZERAORA:
        return sSignLocationNewMauville;
    case LEGENDARY_SIGN_RESHIRAM:
        return sSignLocationScorchedB2F;
    case LEGENDARY_SIGN_SHAYMIN:
        return sSignLocationRoute117;
    case LEGENDARY_SIGN_TAPU_BULU:
    case LEGENDARY_SIGN_XERNEAS:
        return sSignLocationSafariNorth;
    case LEGENDARY_SIGN_TAPU_LELE:
    case LEGENDARY_SIGN_YVELTAL:
        return sSignLocationMtPyreExterior;
    case LEGENDARY_SIGN_THUNDURUS:
        return sSignLocationRoute119Water;
    case LEGENDARY_SIGN_TORNADUS:
        return sSignLocationRoute119Land;
    case LEGENDARY_SIGN_UXIE:
        return sSignLocationMeteorB1F1R;
    case LEGENDARY_SIGN_VICTINI:
        return sSignLocationVictoryRoad1F;
    case LEGENDARY_SIGN_VIRIZION:
        return sSignLocationPetalburgWoods2;
    case LEGENDARY_SIGN_ZYGARDE:
        return sSignLocationDesertUnderpass;
    default:
        return sSignLocationUnknown;
    }
}

static u16 GetLegendaryStateVar(u16 firstVar, u8 signId)
{
    return firstVar + signId / 16;
}

static bool8 GetLegendaryStateBit(u16 firstVar, u8 signId)
{
    if (signId >= LEGENDARY_SIGN_COUNT)
        return FALSE;
    return (VarGet(GetLegendaryStateVar(firstVar, signId)) & (1 << (signId % 16))) != 0;
}

static void SetLegendaryStateBit(u16 firstVar, u8 signId)
{
    u16 var;

    if (signId >= LEGENDARY_SIGN_COUNT)
        return;
    var = GetLegendaryStateVar(firstVar, signId);
    VarSet(var, VarGet(var) | (1 << (signId % 16)));
}

static u8 GetBadgeCountForLegendarySigns(void)
{
    u8 badge;
    u8 count = 0;

    for (badge = 0; badge < NUM_BADGES; badge++)
        if (FlagGet(FLAG_BADGE01_GET + badge))
            count++;
    return count;
}

bool8 IsLegendarySignUnlocked(u8 signId)
{
    return GetLegendaryStateBit(VAR_LEGENDARY_SIGNS_UNLOCKED_0, signId);
}

bool8 IsLegendarySignCaught(u8 signId)
{
    return GetLegendaryStateBit(VAR_LEGENDARY_SIGNS_CAUGHT_0, signId);
}

void UnlockLegendarySign(u8 signId)
{
    SetLegendaryStateBit(VAR_LEGENDARY_SIGNS_UNLOCKED_0, signId);
    switch (signId)
    {
    case LEGENDARY_SIGN_DARKRAI:
        FlagClear(FLAG_HIDE_LEGENDARY_SIGN_DARKRAI);
        break;
    case LEGENDARY_SIGN_CRESSELIA:
        FlagClear(FLAG_HIDE_LEGENDARY_SIGN_CRESSELIA);
        break;
    case LEGENDARY_SIGN_DIALGA:
        FlagClear(FLAG_HIDE_LEGENDARY_SIGN_DIALGA);
        break;
    }
}

u8 GetLegendarySignIdBySpecies(u16 species)
{
    u8 signId;

    for (signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
        if (gLegendarySignDefinitions[signId].species == species)
            return signId;
    return LEGENDARY_SIGN_COUNT;
}

void MarkLegendarySignCaughtBySpecies(u16 species)
{
    u8 signId = GetLegendarySignIdBySpecies(species);

    if (signId < LEGENDARY_SIGN_COUNT)
    {
        UnlockLegendarySign(signId);
        SetLegendaryStateBit(VAR_LEGENDARY_SIGNS_CAUGHT_0, signId);
        switch (signId)
        {
        case LEGENDARY_SIGN_DARKRAI:
            FlagSet(FLAG_HIDE_LEGENDARY_SIGN_DARKRAI);
            break;
        case LEGENDARY_SIGN_CRESSELIA:
            FlagSet(FLAG_HIDE_LEGENDARY_SIGN_CRESSELIA);
            break;
        case LEGENDARY_SIGN_DIALGA:
            FlagSet(FLAG_HIDE_LEGENDARY_SIGN_DIALGA);
            break;
        }
    }
}

bool8 PlayerPartyHasSpeciesFamily(u16 species)
{
    u16 requestedDex = SpeciesToNationalPokedexNum(species);
    u8 slot;

    if (species == SPECIES_NONE)
        return TRUE;
    for (slot = 0; slot < PARTY_SIZE; slot++)
    {
        u16 partySpecies = GetMonData(&gPlayerParty[slot], MON_DATA_SPECIES2, NULL);

        if (partySpecies != SPECIES_NONE
         && partySpecies != SPECIES_EGG
         && SpeciesToNationalPokedexNum(partySpecies) == requestedDex)
            return TRUE;
    }
    return FALSE;
}

bool8 TryGetLegendarySignWildOverride(u8 area, u16 *species, u8 *level)
{
    u16 currentMap = ((u8)gSaveBlock1Ptr->location.mapGroup << 8)
                   | (u8)gSaveBlock1Ptr->location.mapNum;
    u8 signId;

    for (signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
    {
        const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[signId];

        if (sign->source != LEGENDARY_SOURCE_CONDITIONAL_WILD
         || sign->mapId != currentMap
         || sign->area != area
         || !IsLegendarySignUnlocked(signId)
         || IsLegendarySignCaught(signId)
         || Random() % 100 >= sign->chance)
            continue;

        *species = sign->species;
        *level = min(MAX_LEVEL, GetLevelCap() + sign->levelOffset);
        return TRUE;
    }
    return FALSE;
}

void TryUnlockSelectedLegendarySign(void)
{
    u8 signId = gSpecialVar_0x8004;
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
    u8 signId = gSpecialVar_0x8004;

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
    u8 signId = gSpecialVar_0x8004;

    gSpecialVar_Result = signId < LEGENDARY_SIGN_COUNT
                      && IsLegendarySignUnlocked(signId)
                      && !IsLegendarySignCaught(signId);
    return gSpecialVar_Result;
}

u16 GetSelectedLegendarySignLevel(void)
{
    u8 signId = gSpecialVar_0x8004;
    s16 offset = 2;

    if (signId < LEGENDARY_SIGN_COUNT)
        offset = gLegendarySignDefinitions[signId].levelOffset;
    gSpecialVar_Result = min(MAX_LEVEL, GetLevelCap() + offset);
    return gSpecialVar_Result;
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
    u8 i;

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
    for (i = 0; i < ARRAY_COUNT(sMtPyreTrainers); i++)
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
    u8 signId;
    u8 clueId = LEGENDARY_SIGN_COUNT;
    u8 badgeCount = GetBadgeCountForLegendarySigns();

    gSpecialVar_Result = 3;
    for (signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
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

        StringCopy(gStringVar1, gSpeciesNames[sign->requiredSpecies]);
        StringCopy(gStringVar2, gSpeciesNames[sign->species]);
        StringCopy(gStringVar3, GetLegendarySignLocationName(signId));
        UnlockLegendarySign(signId);
        gSpecialVar_Result = 2;
        return;
    }
    if (clueId < LEGENDARY_SIGN_COUNT)
    {
        const struct LegendarySignDefinition *sign = &gLegendarySignDefinitions[clueId];

        StringCopy(gStringVar1, gSpeciesNames[sign->requiredSpecies]);
        StringCopy(gStringVar2, gSpeciesNames[sign->species]);
        StringCopy(gStringVar3, GetLegendarySignLocationName(clueId));
        gSpecialVar_Result = 1;
    }
}

void TryGiveArceusLegendarySignMasteryReward(void)
{
    struct Pokemon reward;
    u8 signId;
    u8 level;
    u8 giveResult;

    gSpecialVar_Result = 0;
    if (IsLegendarySignCaught(LEGENDARY_SIGN_ARCEUS))
    {
        gSpecialVar_Result = 4;
        return;
    }
    for (signId = 0; signId < LEGENDARY_SIGN_COUNT; signId++)
    {
        u8 source = gLegendarySignDefinitions[signId].source;

        if ((source == LEGENDARY_SOURCE_CONDITIONAL_WILD
          || source == LEGENDARY_SOURCE_VISIBLE)
         && !IsLegendarySignCaught(signId))
            return;
    }

    level = min(MAX_LEVEL, GetLevelCap());
    CreateMon(&reward, SPECIES_ARCEUS, level, MAX_PER_STAT_IVS, TRUE,
              Random32(), OT_ID_PLAYER_ID, 0);
    if (GetVerdantBattleSetRawCount(SPECIES_ARCEUS) != 0)
        ApplyVerdantGiftBattleSet(&reward, 0);
    giveResult = GiveMonToPlayer(&reward);
    if (giveResult == MON_CANT_GIVE)
    {
        gSpecialVar_Result = 3;
        return;
    }
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_ARCEUS), FLAG_SET_SEEN);
    GetSetPokedexFlag(SpeciesToNationalPokedexNum(SPECIES_ARCEUS), FLAG_SET_CAUGHT);
    MarkLegendarySignCaughtBySpecies(SPECIES_ARCEUS);
    StringCopy(gStringVar1, gSpeciesNames[SPECIES_ARCEUS]);
    gSpecialVar_Result = giveResult == MON_GIVEN_TO_PARTY ? 1 : 2;
}
