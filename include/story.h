#ifndef GUARD_STORY_H
#define GUARD_STORY_H

#include "constants/story.h"

// One story window from data/progression/story.yaml: open on steps [from, until).
struct StoryWindow
{
    u16 from;
    u16 until;
    u8 player;       // 0 any, 1 male, 2 female
    u16 playerFrom;  // the player restriction applies from this step on
    u8 schedule;     // a schedule segment: when it closes, its object is replaced at once
};

extern const struct StoryWindow gStoryWindows[];
extern const u8 *const gStoryStepNames[];
extern u32 gStoryOrderViolation;

u16 GetStoryStep(void);
bool32 IsStoryWindowId(u16 id);
bool32 IsStoryWindowOpen(u16 id);
bool32 AdvanceStory(u16 step);
void StoryRefreshObjects(void);
void StoryStageAt(u16 step);
void StoryStageAtLeast(u16 step);
void StoryStageBefore(u16 step);

#endif // GUARD_STORY_H
