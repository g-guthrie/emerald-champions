#include "global.h"
#include "event_data.h"
#include "script.h"
#include "story.h"
#include "constants/vars.h"

// The story machine: VAR_STORY_STEP is the only story state, and every window that
// decides what exists is in data/progression/story.yaml (generated into story.h).
#include "data/story.h"

// Set when a script tries to move the story anywhere but one step forward:
// (current << 16) | requested. The chain test fails on any non-zero value.
EWRAM_DATA u32 gStoryOrderViolation = 0;

u16 GetStoryStep(void)
{
    return VarGet(VAR_STORY_STEP);
}

bool32 IsStoryWindowId(u16 id)
{
    return id >= STORY_WINDOWS_START && id < STORY_WINDOWS_START + STORY_WINDOWS_COUNT;
}

bool32 IsStoryWindowOpen(u16 id)
{
    const struct StoryWindow *window = &gStoryWindows[id - STORY_WINDOWS_START];
    u16 step = GetStoryStep();

    if (step < window->from || step >= window->until)
        return FALSE;
    if (window->player == 1 && gSaveBlock2Ptr->playerGender != MALE)
        return FALSE;
    if (window->player == 2 && gSaveBlock2Ptr->playerGender != FEMALE)
        return FALSE;
    return TRUE;
}

bool32 AdvanceStory(u16 step)
{
    u16 current = GetStoryStep();

    if (step == current)
        return TRUE;
    if (step != current + 1 || step >= STORY_STEP_COUNT)
    {
        gStoryOrderViolation = (current << 16) | step;
        DebugPrintf("story: refused %d -> %d", current, step);
        return FALSE;
    }
    VarSet(VAR_STORY_STEP, step);
    return TRUE;
}

// advance_story STEP (asm/macros/event.inc)
void ScrCmd_advancestory(struct ScriptContext *ctx)
{
    u16 step = ScriptReadHalfword(ctx);

    Script_RequestEffects(SCREFF_V1);
    Script_RequestWriteVar(VAR_STORY_STEP);
    AdvanceStory(step);
}
