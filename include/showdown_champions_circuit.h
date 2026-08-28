#ifndef GUARD_SHOWDOWN_CHAMPIONS_CIRCUIT_H
#define GUARD_SHOWDOWN_CHAMPIONS_CIRCUIT_H

#include "global.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/pokemon.h"
#include "constants/species.h"

#define SHOWDOWN_CIRCUIT_VARIANT_COUNT 311
#define SHOWDOWN_CIRCUIT_TEMPLATE_COUNT 444
#define SHOWDOWN_CIRCUIT_MAX_MOVES 8
#define SHOWDOWN_CIRCUIT_MAX_ABILITIES 2

enum ShowdownCircuitRole
{
    SHOWDOWN_ROLE_OFFENSIVE_PROTECT,
    SHOWDOWN_ROLE_SUPPORT,
    SHOWDOWN_ROLE_BULKY_SETUP,
    SHOWDOWN_ROLE_BULKY_ATTACKER,
    SHOWDOWN_ROLE_SETUP_SWEEPER,
    SHOWDOWN_ROLE_CHOICE_ITEM,
    SHOWDOWN_ROLE_WALLBREAKER,
    SHOWDOWN_ROLE_FAST_ATTACKER,
};

#define SHOWDOWN_COMPAT_WEB_SETTER     (1 << 0)
#define SHOWDOWN_COMPAT_SCREEN_SETTER  (1 << 1)
#define SHOWDOWN_COMPAT_SCREEN_CLEANER (1 << 2)
#define SHOWDOWN_COMPAT_DRY_SKIN_SUN   (1 << 3)
#define SHOWDOWN_COMPAT_LIGHTNING_ROD  (1 << 4)
#define SHOWDOWN_COMPAT_SUN_SETTER     (1 << 5)
#define SHOWDOWN_COMPAT_RAIN_SETTER    (1 << 6)
#define SHOWDOWN_COMPAT_SAND_SETTER    (1 << 7)
#define SHOWDOWN_COMPAT_SNOW_SETTER    (1 << 8)

struct ShowdownCircuitVariant
{
    enum Species partySpecies;
    enum Species formSpecies;
    enum Item requiredItem;
    u16 templateOffset;
    u8 templateCount;
    u16 compatibilityFlags;
};

struct ShowdownCircuitTemplate
{
    enum Move moves[SHOWDOWN_CIRCUIT_MAX_MOVES];
    enum Ability abilities[SHOWDOWN_CIRCUIT_MAX_ABILITIES];
    enum Type preferredType;
    enum ShowdownCircuitRole role;
    u8 moveCount;
    u8 abilityCount;
};

extern const struct ShowdownCircuitVariant gShowdownCircuitVariants[SHOWDOWN_CIRCUIT_VARIANT_COUNT];
extern const struct ShowdownCircuitTemplate gShowdownCircuitTemplates[SHOWDOWN_CIRCUIT_TEMPLATE_COUNT];

#endif // GUARD_SHOWDOWN_CHAMPIONS_CIRCUIT_H
