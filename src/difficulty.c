#include "global.h"
#include "battle.h"
#include "caps.h"
#include "data.h"
#include "event_data.h"
#include "pokemon.h"
#include "script.h"
#include "constants/battle.h"

enum DifficultyLevel GetCurrentDifficultyLevel(void)
{
    u16 difficulty;

    if (!B_VAR_DIFFICULTY)
        return DIFFICULTY_NORMAL;

    difficulty = VarGet(B_VAR_DIFFICULTY);
    // Loaded state bypasses the setter. Apply the same bound before callers
    // use this value as an index into the trainer and partner tables.
    if (difficulty > DIFFICULTY_MAX)
        difficulty = DIFFICULTY_MAX;
    return difficulty;
}

void SetCurrentDifficultyLevel(enum DifficultyLevel desiredDifficulty)
{
    if (!B_VAR_DIFFICULTY)
        return;

    if (desiredDifficulty > DIFFICULTY_MAX)
        desiredDifficulty = DIFFICULTY_MAX;

    VarSet(B_VAR_DIFFICULTY, desiredDifficulty);
}

// Hard plays the roster exactly as authored against the cap. Medium and Easy
// stagger it down by one and four levels. Facilities use the same reduction
// against their own base, so the three modes stay in step everywhere.
// It never changes trainer AI: the player is meant to experiment with the
// same authored teams and the same opponents at different level gaps.
u8 GetTrainerLevelReduction(void)
{
    return GetTrainerLevelReductionFor(GetCurrentDifficultyLevel());
}

u8 GetTrainerLevelReductionFor(enum DifficultyLevel difficulty)
{
    switch (difficulty)
    {
    case DIFFICULTY_EASY:
        return 6;
    case DIFFICULTY_NORMAL:
        return 3;
    case DIFFICULTY_HARD:
    default:
        return 2;
    }
}

u8 GetCampaignTrainerLevel(s16 offset)
{
    // Authoring uses Normal. Opponents may exceed level 100; 255 is the
    // existing native u8 battle-level representation, not a difficulty policy.
    s32 level = (s32)GetCurrentLevelCap() + offset + 2 - GetTrainerLevelReduction();
    return max(1, min(255, level));
}

enum DifficultyLevel GetBattlePartnerDifficultyLevel(u16 partnerId)
{
    enum DifficultyLevel difficulty = GetCurrentDifficultyLevel();

    if (partnerId > TRAINER_PARTNER(PARTNER_NONE))
        partnerId -= TRAINER_PARTNER(PARTNER_NONE);

    if (partnerId >= PARTNER_COUNT)
        return DIFFICULTY_NORMAL;

    if (difficulty == DIFFICULTY_NORMAL)
        return DIFFICULTY_NORMAL;

    if (gBattlePartners[difficulty][partnerId].party == NULL)
        return DIFFICULTY_NORMAL;

    return difficulty;
}

enum DifficultyLevel GetTrainerDifficultyLevel(u16 trainerId)
{
    enum DifficultyLevel difficulty = GetCurrentDifficultyLevel();
    trainerId = SanitizeTrainerId(trainerId);

    if (difficulty == DIFFICULTY_NORMAL)
        return DIFFICULTY_NORMAL;

    if (gTrainers[difficulty][trainerId].party == NULL)
        return DIFFICULTY_NORMAL;

    return difficulty;
}

void Script_IncreaseDifficulty(void)
{
    enum DifficultyLevel currentDifficulty;

    if (!B_VAR_DIFFICULTY)
        return;

    currentDifficulty = GetCurrentDifficultyLevel();

    currentDifficulty++;

    Script_RequestEffects(SCREFF_V1);
    Script_RequestWriteVar(B_VAR_DIFFICULTY);

    SetCurrentDifficultyLevel(currentDifficulty);
}

void Script_DecreaseDifficulty(void)
{
    enum DifficultyLevel currentDifficulty;

    if (!B_VAR_DIFFICULTY)
        return;

    currentDifficulty = GetCurrentDifficultyLevel();

    if (!currentDifficulty)
        return;

    Script_RequestEffects(SCREFF_V1);
    Script_RequestWriteVar(B_VAR_DIFFICULTY);

    SetCurrentDifficultyLevel(--currentDifficulty);
}

void Script_GetDifficulty(void)
{
    Script_RequestEffects(SCREFF_V1);
    gSpecialVar_Result = GetCurrentDifficultyLevel();
}

void Script_SetDifficulty(struct ScriptContext *ctx)
{
    enum DifficultyLevel desiredDifficulty = ScriptReadByte(ctx);

    Script_RequestEffects(SCREFF_V1);
    Script_RequestWriteVar(B_VAR_DIFFICULTY);

    SetCurrentDifficultyLevel(desiredDifficulty);
}
