#include "global.h"
#include "pokemon.h"
#include "battle.h"
const unsigned offsets[] = {sizeof(struct BattlePokemon),offsetof(struct BattlePokemon,hp),offsetof(struct BattlePokemon,maxHP),offsetof(struct BattlePokemon,item),offsetof(struct BattlePokemon,statStages),offsetof(struct BattlePokemon,status1)};
