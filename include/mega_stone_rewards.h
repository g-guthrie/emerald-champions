#ifndef GUARD_MEGA_STONE_REWARDS_H
#define GUARD_MEGA_STONE_REWARDS_H
u16 GetHarvestedBerryCount(u8 berry);
void AddHarvestedBerries(u8 berry, u16 count);
void BuildEmeraldChampionsHarvestChoices(void);
void BufferEmeraldChampionsHarvestRecipe(void);
void TradeEmeraldChampionsGardenBerries(void);
void CheckEmeraldChampionsGardenCelebi(void);
void LoseEmeraldChampionsGardenCelebi(void);
// Mega Evolutions the player has actually watched happen, on either side.
u32 EmeraldChampions_GetMegaArchiveCount(void);
void EmeraldChampions_RecordMegaWitnessed(u32 item);
u32 EmeraldChampions_CountMegasWitnessed(void);
// Norman's starter Mega Stones (callnative from PetalburgCity_Gym).
u16 GetNormanStarterMegaStone(void);
u16 GetNormanPartnerMegaStone(void);
void BufferNormanStarterMegaStone(void);
void BufferNormanPartnerMegaStone(void);
void BufferNormanMegaGiftKind(void);
void MarkStarterMegaStoneReceived(void);
#endif
