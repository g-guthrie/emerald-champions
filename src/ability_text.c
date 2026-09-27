#include "global.h"
#include "ability_text.h"
#include "pokemon.h"
#include "constants/abilities.h"

static const u8 *const sAbilityFullDescriptions[ABILITIES_COUNT] =
{
#include "data/ability_full_descriptions.h"
};

const u8 *GetAbilityFullDescription(enum Ability ability)
{
    if (ability >= ABILITIES_COUNT)
        ability = ABILITY_NONE;
    if (sAbilityFullDescriptions[ability] != NULL)
        return sAbilityFullDescriptions[ability];
    return gAbilitiesInfo[ability].description;
}
