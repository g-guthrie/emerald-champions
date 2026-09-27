#ifndef GUARD_GUIDED_TUTORIAL_H
#define GUARD_GUIDED_TUTORIAL_H

#define RIVAL_TUTORIAL_TARGET_X 14
#define RIVAL_TUTORIAL_TARGET_Y 12

bool32 IsRivalDexNavTutorialActive(void);
bool32 RivalTutorialGetSearchOrigin(s16 *x, s16 *y);
void RivalTutorialFieldStep(void);
u16 RivalTutorialDexNavKeys(void);
u16 RivalTutorialSummaryKeys(u32 page);
u32 RivalTutorialTrainerPic(void);
const u8 *RivalTutorialTrainerName(void);
const u8 *RivalTutorialActionPrompt(void);
void PrepareRivalDexNavTutorial(void);
void StartRivalDexNavTutorial(void);
void WarpToRivalTutorialSpot(void);
void RivalTutorialSearchStarted(void);
void RivalTutorialBattleStarted(void);
void ShowRivalPokerusTutorial(void);
void FinishRivalDexNavTutorial(void);
void PrepareRivalTutorialCatch(struct Pokemon *mon);

#endif
