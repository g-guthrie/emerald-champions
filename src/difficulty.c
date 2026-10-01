#include "global.h"
#include "battle.h"
#include "caps.h"
#include "data.h"
#include "event_data.h"
#include "pokemon.h"
#include "script.h"
#include "constants/battle.h"

EWRAM_DATA static bool8 sHasNewGameDifficulty = FALSE;
EWRAM_DATA static enum DifficultyLevel sNewGameDifficulty = 0;

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

// A pending New Game choice must not change the loaded save when the player
// cancels the introduction. Consume it only after the new game's reset begins.
void SetNewGameDifficultyLevel(enum DifficultyLevel difficulty)
{
    sNewGameDifficulty = min(difficulty, DIFFICULTY_HARD);
    sHasNewGameDifficulty = TRUE;
}

enum DifficultyLevel ConsumeNewGameDifficultyLevel(void)
{
    enum DifficultyLevel difficulty = sHasNewGameDifficulty
        ? sNewGameDifficulty : GetCurrentDifficultyLevel();
    sHasNewGameDifficulty = FALSE;
    return difficulty;
}

// Facilities (Circuit, Tent, Trainer Hill) lower their own fixed base by a
// flat number of levels. Campaign trainers use the lead scaling below.
u8 GetTrainerLevelReduction(void)
{
    return GetTrainerLevelReductionFor(GetCurrentDifficultyLevel());
}

u8 GetTrainerLevelReductionFor(enum DifficultyLevel difficulty)
{
    switch (difficulty)
    {
    case DIFFICULTY_EASY:
        return 8;
    case DIFFICULTY_NORMAL:
        return 3;
    case DIFFICULTY_HARD:
    default:
        return 0;
    }
}

// Hard fights at the full authored lead over the campaign cap (offset + 2).
// Medium and Easy keep only a share of each opponent's lead, so the gap
// between modes follows the size of the fight's spike: a rival eight levels
// over the cap at cap 14 and an Elite sixteen over at cap 80 both shrink in
// proportion, rather than losing the same flat number of levels. Easy also
// sits a share of the cap lower, so the same relief applies at every stage;
// perfect sets and doubles AI beat an ordinary team even at the cap. Teams,
// strategy and the shared planner never change between modes.
u8 GetTrainerLevelLeadPercentFor(enum DifficultyLevel difficulty)
{
    switch (difficulty)
    {
    case DIFFICULTY_EASY:
        return 25;
    case DIFFICULTY_NORMAL:
        return 60;
    case DIFFICULTY_HARD:
    default:
        return 100;
    }
}

u8 GetTrainerLevelCapDropPercentFor(enum DifficultyLevel difficulty)
{
    return difficulty == DIFFICULTY_EASY ? 15 : 0;
}

u8 GetCampaignTrainerLevelFor(enum DifficultyLevel difficulty, s16 offset)
{
    s32 cap = GetCurrentLevelCap();
    s32 lead = offset + 2;
    if (lead > 0)
        lead = (lead * GetTrainerLevelLeadPercentFor(difficulty) + 50) / 100;
    s32 level = cap + lead - (cap * GetTrainerLevelCapDropPercentFor(difficulty) + 50) / 100;
    return max(1, min(MAX_LEVEL, level));
}

u8 GetCampaignTrainerLevel(s16 offset)
{
    return GetCampaignTrainerLevelFor(GetCurrentDifficultyLevel(), offset);
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
#if TESTING || EC_HEADLESS_FIXTURES
    Script_RequestEffects(SCREFF_V1);
    Script_RequestWriteVar(B_VAR_DIFFICULTY);
    SetCurrentDifficultyLevel(min(GetCurrentDifficultyLevel() + 1, DIFFICULTY_HARD));
#endif
}

void Script_DecreaseDifficulty(void)
{
#if TESTING || EC_HEADLESS_FIXTURES
    enum DifficultyLevel difficulty = GetCurrentDifficultyLevel();
    if (difficulty > DIFFICULTY_EASY)
    {
        Script_RequestEffects(SCREFF_V1);
        Script_RequestWriteVar(B_VAR_DIFFICULTY);
        SetCurrentDifficultyLevel(difficulty - 1);
    }
#endif
}

void Script_GetDifficulty(void)
{
    Script_RequestEffects(SCREFF_V1);
    gSpecialVar_Result = GetCurrentDifficultyLevel();
}

void Script_SetDifficulty(struct ScriptContext *ctx)
{
    enum DifficultyLevel desiredDifficulty = ScriptReadByte(ctx);

    // Release scripts cannot change the difficulty of an existing run. Still
    // consume the operand so legacy script bytecode advances correctly.
#if TESTING || EC_HEADLESS_FIXTURES
    Script_RequestEffects(SCREFF_V1);
    Script_RequestWriteVar(B_VAR_DIFFICULTY);
    SetCurrentDifficultyLevel(desiredDifficulty);
#else
    (void)desiredDifficulty;
#endif
}
