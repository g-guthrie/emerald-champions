#include "global.h"
#include "data.h"
#include "event_data.h"
#include "pokemon.h"
#include "script.h"
#include "constants/battle.h"

enum DifficultyLevel GetCurrentDifficultyLevel(void)
{
    if (!B_VAR_DIFFICULTY)
        return DIFFICULTY_NORMAL;

    return VarGet(B_VAR_DIFFICULTY);
}

void SetCurrentDifficultyLevel(enum DifficultyLevel desiredDifficulty)
{
    if (!B_VAR_DIFFICULTY)
        return;

    if (desiredDifficulty > DIFFICULTY_MAX)
        desiredDifficulty = DIFFICULTY_MAX;

    VarSet(B_VAR_DIFFICULTY, desiredDifficulty);
}

// Difficulty only staggers opponent levels (Easy -4, Normal -2, Hard 0).
// It never changes trainer AI: the player is meant to experiment with the
// same authored teams and the same opponents at different level gaps.
u8 GetTrainerLevelReduction(void)
{
    switch (GetCurrentDifficultyLevel())
    {
    case DIFFICULTY_EASY:
        return 4;
    case DIFFICULTY_NORMAL:
        return 2;
    case DIFFICULTY_HARD:
    default:
        return 0;
    }
}

void ApplyTrainerLevelDifficulty(struct Pokemon *party)
{
    u8 reduction = GetTrainerLevelReduction();

    if (reduction == 0)
        return;

    for (u32 i = 0; i < PARTY_SIZE; i++)
    {
        enum Species species = GetMonData(&party[i], MON_DATA_SPECIES);
        u32 experience;
        u16 maxHp;
        u8 level;

        if (species == SPECIES_NONE || species == SPECIES_EGG)
            continue;

        level = GetMonData(&party[i], MON_DATA_LEVEL);
        level = level > reduction ? level - reduction : 1;
        experience = gExperienceTables[gSpeciesInfo[species].growthRate][level];
        SetMonData(&party[i], MON_DATA_EXP, &experience);
        SetMonData(&party[i], MON_DATA_LEVEL, &level);
        CalculateMonStats(&party[i]);
        maxHp = GetMonData(&party[i], MON_DATA_MAX_HP);
        SetMonData(&party[i], MON_DATA_HP, &maxHp);
    }
}

enum DifficultyLevel GetBattlePartnerDifficultyLevel(u16 partnerId)
{
    enum DifficultyLevel difficulty = GetCurrentDifficultyLevel();

    if (partnerId > TRAINER_PARTNER(PARTNER_NONE))
        partnerId -= TRAINER_PARTNER(PARTNER_NONE);

    if (difficulty == DIFFICULTY_NORMAL)
        return DIFFICULTY_NORMAL;

    if (gBattlePartners[difficulty][partnerId].party == NULL)
        return DIFFICULTY_NORMAL;

    return difficulty;
}

enum DifficultyLevel GetTrainerDifficultyLevel(u16 trainerId)
{
    enum DifficultyLevel difficulty = GetCurrentDifficultyLevel();

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

    if (currentDifficulty++ > DIFFICULTY_MAX)
        return;

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
