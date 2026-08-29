#ifndef GUARD_STARTER_CHOOSE_H
#define GUARD_STARTER_CHOOSE_H

#include "constants/species.h"

extern const u16 gBirchBagGrass_Pal[];
extern const u32 gBirchBagTilemap[];
extern const u32 gBirchGrassTilemap[];
extern const u32 gBirchBagGrass_Gfx[];
extern const u32 gPokeballSelection_Gfx[];

u16 GetStarterPokemon(u16 chosenStarterId);
enum Species GetStarterPokemonForGeneration(u16 chosenStarterId, u16 starterGeneration);
enum Species GetMiddleEvolutionForStarter(enum Species species);
enum Species GetFinalEvolutionForStarter(enum Species species);
void CB2_ChooseStarter(void);

#endif // GUARD_STARTER_CHOOSE_H
