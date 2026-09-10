#line 60000 "work/speed_scaling_inventory.inc"
#include "caps.h"
#include "debug.h"
#include "random.h"

// Temporary observation only: current native table parties at zero badges.
// The six rival aliases describe their compiled Hoenn starters, not every
// regional-starter substitution performed by the outer field-battle wrapper.
// Speed excludes battle stages, status, weather and held-item/ability effects.
// oldFlat is a counterfactual calculation, never written into the Pokemon.
TEST("EC speed scaling inventory: completed native parties at zero badges across difficulties")
{
    static const struct {
        u16 encounter;
        u16 trainer;
    } entries[] = {
        {1, TRAINER_BRENDAN_ROUTE_103_MUDKIP},
        {1, TRAINER_BRENDAN_ROUTE_103_TORCHIC},
        {1, TRAINER_BRENDAN_ROUTE_103_TREECKO},
        {1, TRAINER_MAY_ROUTE_103_MUDKIP},
        {1, TRAINER_MAY_ROUTE_103_TORCHIC},
        {1, TRAINER_MAY_ROUTE_103_TREECKO},
        {2, TRAINER_CALVIN_1},
        {3, TRAINER_RICK},
        {4, TRAINER_ALLEN},
        {5, TRAINER_TIANA},
        {6, TRAINER_BILLY},
        {7, TRAINER_DARIAN},
        {8, TRAINER_CINDY_1},
        {9, TRAINER_LYLE},
        {10, TRAINER_GRUNT_PETALBURG_WOODS},
        {11, TRAINER_JAMES_1},
        {12, TRAINER_WINSTON_1},
        {13, TRAINER_HALEY_1},
        {14, TRAINER_GINA_AND_MIA_1},
        {15, TRAINER_IVAN},
        {17, TRAINER_CYNDY_1},
        {18, TRAINER_HECTOR},
        {19, TRAINER_MARLENE},
        {20, TRAINER_JOEY},
        {21, TRAINER_JOSE},
        {23, TRAINER_CLARK},
    };
    static const enum DifficultyLevel difficulties[] = {
        DIFFICULTY_EASY, DIFFICULTY_NORMAL, DIFFICULTY_HARD,
    };
    struct Pokemon party[PARTY_SIZE];
    u32 savedFlags = gBattleTypeFlags;
    u16 savedDifficulty = B_VAR_DIFFICULTY ? VarGet(B_VAR_DIFFICULTY) : 0;
    bool8 savedDebug = gIsDebugBattle;
    bool8 savedBadges[8];
    rng_value_t savedRng = gRngValue;
    rng_value_t savedRng2 = gRng2Value;

    for (u32 badge = 0; badge < ARRAY_COUNT(savedBadges); badge++)
    {
        savedBadges[badge] = FlagGet(FLAG_BADGE01_GET + badge);
        FlagClear(FLAG_BADGE01_GET + badge);
    }
    gIsDebugBattle = FALSE;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE;

    for (u32 difficulty = 0; difficulty < ARRAY_COUNT(difficulties); difficulty++)
    {
        SetCurrentDifficultyLevel(difficulties[difficulty]);
        Test_MgbaPrintf("SPEED_INVENTORY difficulty=%d cap=%d reduction=%d aliases=%d",
            GetCurrentDifficultyLevel(), GetCurrentLevelCap(), GetTrainerLevelReduction(), ARRAY_COUNT(entries));
        for (u32 entry = 0; entry < ARRAY_COUNT(entries); entry++)
        {
            // Native accessor resolves missing Easy/Hard table fallbacks.
            const struct Trainer *trainer = GetTrainerStructFromId(entries[entry].trainer);
            CreateNPCTrainerPartyFromTrainer(party, trainer);
            ApplyTrainerLevelDifficulty(party);
            for (u32 slot = 0; slot < trainer->partySize; slot++)
            {
                struct Pokemon *mon = &party[slot];
                enum Species species = GetMonData(mon, MON_DATA_SPECIES);
                u32 level = GetMonData(mon, MON_DATA_LEVEL);
                u32 baseSpeed = GetSpeciesBaseSpeed(species);
                u32 points = GetMonData(mon, MON_DATA_SPEED_EV);
                u32 nature = GetMonData(mon, MON_DATA_HIDDEN_NATURE);
                u32 oldFlat = ((2 * baseSpeed + MAX_PER_STAT_IVS) * level) / 100 + 5 + points;
                oldFlat = ModifyStatByNature(nature, oldFlat, STAT_SPEED);
                Test_MgbaPrintf("SPEED_MON e=%d id=%d d=%d slot=%d species=%d L=%d base=%d points=%d nature=%d native=%d oldFlat=%d",
                    entries[entry].encounter, entries[entry].trainer, difficulties[difficulty], slot,
                    species, level, baseSpeed, points, nature, GetMonData(mon, MON_DATA_SPEED), oldFlat);
                Test_MgbaPrintf("SPEED_SET id=%d d=%d slot=%d item=%d ability=%d moves=%d/%d/%d/%d HP=%d",
                    entries[entry].trainer, difficulties[difficulty], slot, GetMonData(mon, MON_DATA_HELD_ITEM),
                    GetMonAbility(mon), GetMonData(mon, MON_DATA_MOVE1), GetMonData(mon, MON_DATA_MOVE2),
                    GetMonData(mon, MON_DATA_MOVE3), GetMonData(mon, MON_DATA_MOVE4), GetMonData(mon, MON_DATA_MAX_HP));
            }
        }
    }

    gBattleTypeFlags = savedFlags;
    gIsDebugBattle = savedDebug;
    if (B_VAR_DIFFICULTY)
        VarSet(B_VAR_DIFFICULTY, savedDifficulty);
    for (u32 badge = 0; badge < ARRAY_COUNT(savedBadges); badge++)
        if (savedBadges[badge])
            FlagSet(FLAG_BADGE01_GET + badge);
        else
            FlagClear(FLAG_BADGE01_GET + badge);
    gRngValue = savedRng;
    gRng2Value = savedRng2;
}
