#include "global.h"
#include "pokemon.h"
#include "showdown_champions_circuit.h"
#include "constants/pokedex.h"
const unsigned circuitAuditLayout[] = {
 sizeof(struct ShowdownCircuitVariant), offsetof(struct ShowdownCircuitVariant, templateOffset), offsetof(struct ShowdownCircuitVariant, templateCount),
 sizeof(struct ShowdownCircuitTemplate), offsetof(struct ShowdownCircuitTemplate, abilities), offsetof(struct ShowdownCircuitTemplate, moveCount), offsetof(struct ShowdownCircuitTemplate, abilityCount), offsetof(struct ShowdownCircuitTemplate, authored), offsetof(struct ShowdownCircuitTemplate, evs),
 sizeof(struct SpeciesInfo), offsetof(struct SpeciesInfo, abilities), offsetof(struct SpeciesInfo, height), NUM_ABILITY_SLOTS, NATIONAL_DEX_COUNT,
 sizeof(enum Species), sizeof(enum Move), sizeof(enum Ability), sizeof(enum Item)
};
unsigned CircuitAuditDex(const struct SpeciesInfo *p) { return p->natDexNum; }
