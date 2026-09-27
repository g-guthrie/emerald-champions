#ifndef GUARD_POKERUS_H
#define GUARD_POKERUS_H

u32 GetPokerusSpreadsLeft(struct Pokemon *mon);
void GiveMonPokerus(struct Pokemon *mon, bool32 canSpread);
bool32 IsPokerusNatureBoosted(struct Pokemon *mon, u32 stat);
void RandomlyGivePartyPokerus(void);
bool32 IsPokerusInParty(void);
bool32 CheckMonPokerus(struct Pokemon *mon);
bool32 CheckMonHasHadPokerus(struct Pokemon *mon);
bool32 ShouldPokemonShowActivePokerus(struct Pokemon *mon);
bool32 ShouldPokemonShowCuredPokerus(struct Pokemon *mon);
void PartySpreadPokerus(void);

#endif // GUARD_POKERUS_H
