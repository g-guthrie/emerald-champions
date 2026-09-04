#ifndef GUARD_LEGENDARY_SIGNS_H
#define GUARD_LEGENDARY_SIGNS_H

#include "global.h"
#include "wild_encounter.h"
#include "constants/species.h"

enum LegendarySignId
{
    LEGENDARY_SIGN_ARCEUS,
    LEGENDARY_SIGN_AZELF,
    LEGENDARY_SIGN_BLACEPHALON,
    LEGENDARY_SIGN_BUZZWOLE,
    LEGENDARY_SIGN_CALYREX,
    LEGENDARY_SIGN_CELEBI,
    LEGENDARY_SIGN_CELESTEELA,
    LEGENDARY_SIGN_COBALION,
    LEGENDARY_SIGN_CRESSELIA,
    LEGENDARY_SIGN_DARKRAI,
    LEGENDARY_SIGN_DIALGA,
    LEGENDARY_SIGN_ENTEI,
    LEGENDARY_SIGN_ETERNATUS,
    LEGENDARY_SIGN_GENESECT,
    LEGENDARY_SIGN_GIRATINA,
    LEGENDARY_SIGN_GLASTRIER,
    LEGENDARY_SIGN_GUZZLORD,
    LEGENDARY_SIGN_HOOPA,
    LEGENDARY_SIGN_KARTANA,
    LEGENDARY_SIGN_KYUREM,
    LEGENDARY_SIGN_LANDORUS,
    LEGENDARY_SIGN_MARSHADOW,
    LEGENDARY_SIGN_MESPRIT,
    LEGENDARY_SIGN_NECROZMA,
    LEGENDARY_SIGN_NIHILEGO,
    LEGENDARY_SIGN_PALKIA,
    LEGENDARY_SIGN_PHEROMOSA,
    LEGENDARY_SIGN_PHIONE,
    LEGENDARY_SIGN_POIPOLE,
    LEGENDARY_SIGN_RAIKOU,
    LEGENDARY_SIGN_REGIDRAGO,
    LEGENDARY_SIGN_REGIELEKI,
    LEGENDARY_SIGN_RESHIRAM,
    LEGENDARY_SIGN_SHAYMIN,
    LEGENDARY_SIGN_SPECTRIER,
    LEGENDARY_SIGN_STAKATAKA,
    LEGENDARY_SIGN_TAPU_BULU,
    LEGENDARY_SIGN_TAPU_KOKO,
    LEGENDARY_SIGN_TAPU_LELE,
    LEGENDARY_SIGN_THUNDURUS,
    LEGENDARY_SIGN_TORNADUS,
    LEGENDARY_SIGN_UXIE,
    LEGENDARY_SIGN_VICTINI,
    LEGENDARY_SIGN_VIRIZION,
    LEGENDARY_SIGN_XERNEAS,
    LEGENDARY_SIGN_XURKITREE,
    LEGENDARY_SIGN_YVELTAL,
    LEGENDARY_SIGN_ZACIAN,
    LEGENDARY_SIGN_ZAMAZENTA,
    LEGENDARY_SIGN_ZARUDE,
    LEGENDARY_SIGN_ZEKROM,
    LEGENDARY_SIGN_ZERAORA,
    LEGENDARY_SIGN_ZYGARDE,
    // Append only: existing IDs are persisted in save variables.
    LEGENDARY_SIGN_ARTICUNO,
    LEGENDARY_SIGN_COSMOG,
    LEGENDARY_SIGN_ENAMORUS,
    LEGENDARY_SIGN_FEZANDIPITI,
    LEGENDARY_SIGN_KORAIDON,
    LEGENDARY_SIGN_MAGEARNA,
    LEGENDARY_SIGN_MELOETTA,
    LEGENDARY_SIGN_MELTAN,
    LEGENDARY_SIGN_MEWTWO,
    LEGENDARY_SIGN_MIRAIDON,
    LEGENDARY_SIGN_MUNKIDORI,
    LEGENDARY_SIGN_OKIDOGI,
    LEGENDARY_SIGN_PECHARUNT,
    LEGENDARY_SIGN_REGIGIGAS,
    LEGENDARY_SIGN_TERAPAGOS,
    LEGENDARY_SIGN_WO_CHIEN,
    LEGENDARY_SIGN_ZAPDOS,
    LEGENDARY_SIGN_CHIEN_PAO,
    LEGENDARY_SIGN_CHI_YU,
    LEGENDARY_SIGN_KUBFU,
    LEGENDARY_SIGN_MANAPHY,
    LEGENDARY_SIGN_SUICUNE,
    LEGENDARY_SIGN_TAPU_FINI,
    LEGENDARY_SIGN_TERRAKION,
    LEGENDARY_SIGN_TING_LU,
    LEGENDARY_SIGN_TYPE_NULL,
    LEGENDARY_SIGN_VOLCANION,
    LEGENDARY_SIGN_KELDEO,
    LEGENDARY_SIGN_OGERPON,
    LEGENDARY_SIGN_COUNT,
};

enum LegendarySignSource
{
    LEGENDARY_SOURCE_CONDITIONAL_WILD,
    LEGENDARY_SOURCE_VISIBLE,
    LEGENDARY_SOURCE_BREEDING,
    LEGENDARY_SOURCE_GAME_CORNER,
    LEGENDARY_SOURCE_CIRCUIT,
    LEGENDARY_SOURCE_MASTERY,
    LEGENDARY_SOURCE_ORDINARY_WILD,
};

struct LegendarySignDefinition
{
    enum Species species;
    u16 mapId;
    enum Species requiredSpecies;
    u16 requiredFlag;
    enum LegendarySignSource source;
    enum WildPokemonArea area;
    u8 chance;
    u8 minimumBadges;
    s8 levelOffset;
};

extern const struct LegendarySignDefinition gLegendarySignDefinitions[LEGENDARY_SIGN_COUNT];

bool32 IsLegendarySignUnlocked(enum LegendarySignId signId);
bool32 IsLegendarySignCaught(enum LegendarySignId signId);
void UnlockLegendarySign(enum LegendarySignId signId);
void InitializeLegendaryRelicDeliveryState(void);
void RetryPendingLegendaryRelics(void);
void MarkLegendarySignCaughtBySpecies(enum Species species);
enum LegendarySignId GetLegendarySignIdBySpecies(enum Species species);
bool32 TryGetLegendarySignWildOverride(enum WildPokemonArea area, enum Species *species, u8 *level);
bool32 PlayerPartyHasSpeciesFamily(enum Species species);
bool32 IsLegendarySignOrdinaryWildSpecies(enum Species species);
bool32 IsLegendarySignConditionalWildSpecies(enum Species species);
void TryUnlockEligibleVisibleLegendarySignsForCurrentMap(void);
void DoesPlayerPartyHaveSelectedSpeciesFamily(void);
void TryUnlockSelectedLegendarySign(void);
u16 GetSelectedLegendarySignState(void);
u16 ShouldShowSelectedLegendarySignObject(void);
u16 GetSelectedLegendarySignLevel(void);
void CreateSelectedLegendarySignEncounter(void);
void TryGiveSelectedLegendarySignReward(void);
void CreateEmeraldChampionsStaticLegendaryEncounter(void);
void TryUnlockDarkraiLegendarySign(void);
void TryDiscoverEligibleLegendarySign(void);
void TryGiveArceusLegendarySignMasteryReward(void);
u8 GiveLegendarySignReward(enum Species species, u8 level);

#endif // GUARD_LEGENDARY_SIGNS_H
