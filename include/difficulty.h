#ifndef GUARD_DIFFICULTY_H
#define GUARD_DIFFICULTY_H

#include "constants/difficulty.h"
#include "script.h"

struct Pokemon;

enum DifficultyLevel GetCurrentDifficultyLevel(void);
// Trusted development/fixture override; ordinary play chooses at New Game.
void SetCurrentDifficultyLevel(enum DifficultyLevel);
void SetNewGameDifficultyLevel(enum DifficultyLevel difficulty);
enum DifficultyLevel ConsumeNewGameDifficultyLevel(void);
u8 GetTrainerLevelReduction(void);
u8 GetTrainerLevelReductionFor(enum DifficultyLevel difficulty);
u8 GetTrainerLevelLeadPercentFor(enum DifficultyLevel difficulty);
u8 GetCampaignTrainerLevelFor(enum DifficultyLevel difficulty, s16 offset);
u8 GetCampaignTrainerLevel(s16 offset);

enum DifficultyLevel GetBattlePartnerDifficultyLevel(u16);
enum DifficultyLevel GetTrainerDifficultyLevel(u16);
void Script_IncreaseDifficulty(void);
void Script_DecreaseDifficulty(void);
void Script_GetDifficulty(void);
void Script_SetDifficulty(struct ScriptContext *);

#endif // GUARD_DIFFICULTY_H
