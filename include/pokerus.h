#ifndef GUARD_POKERUS_H
#define GUARD_POKERUS_H

u32 GetPokerusSpreadsLeft(struct Pokemon *mon);
void GiveMonPokerus(struct Pokemon *mon, bool32 canSpread);
bool32 IsPokerusNatureBoosted(struct Pokemon *mon, u32 stat);
u32 GetPokerusNatureModifier(struct Pokemon *mon, u32 stat);
bool32 HasHotSpringPokerus(struct Pokemon *mon);
bool32 PreparePartyPokerusInHotSpring(void);
bool32 IsMonReadyForHotSpringTreatment(struct Pokemon *mon);
bool32 RecoverMonPokerusInHotSpring(struct Pokemon *mon);
void BufferHotSpringPokerusPreview(void);
void ApplyHotSpringPokerusTreatment(void);
void RandomlyGivePartyPokerus(void);
bool32 IsPokerusInParty(void);
bool32 CheckMonPokerus(struct Pokemon *mon);
bool32 CheckMonHasHadPokerus(struct Pokemon *mon);
bool32 ShouldPokemonShowActivePokerus(struct Pokemon *mon);
bool32 ShouldPokemonShowCuredPokerus(struct Pokemon *mon);
void PartySpreadPokerus(void);

#endif // GUARD_POKERUS_H
