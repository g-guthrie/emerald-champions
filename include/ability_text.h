#ifndef GUARD_ABILITY_TEXT_H
#define GUARD_ABILITY_TEXT_H

#include "constants/abilities.h"

// Full description of an Ability for the Pokédex Abilities page: pre-wrapped
// into at most six lines of at most 134 px in FONT_NARROW. Falls back to the
// one-line gAbilitiesInfo[ability].description when no full text exists.
const u8 *GetAbilityFullDescription(enum Ability ability);

#endif // GUARD_ABILITY_TEXT_H
