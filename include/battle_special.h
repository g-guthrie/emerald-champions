#ifndef GUARD_BATTLE_SPECIAL_H
#define GUARD_BATTLE_SPECIAL_H

void DoSpecialTrainerBattle(void); 
u8 GetEreaderTrainerFrontSpriteId(void);
enum TrainerClassID GetEreaderTrainerClassId(void);
void GetEreaderTrainerName(u8 *dst);
void ValidateEReaderTrainer(void);
void ClearEReaderTrainer(struct BattleTowerEReaderTrainer *ereaderTrainer);

#endif // GUARD_BATTLE_SPECIAL_H
