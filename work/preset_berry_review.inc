#line 12000 "work/preset_berry_review.inc"

// Temporary source-selected native evidence probe. This records the common
// scripted-preset application's actual HP/EV result and simple full-HP cost
// arithmetic. It does not claim that a berry strategy succeeds in battle.
#include "emerald_champions_battle_sets.h"
#include "item.h"
#include "constants/abilities.h"
#include "constants/emerald_champions.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/species.h"

struct PresetBerryReviewCase
{
    enum Species species;
    u8 format;
    u8 choice;
};

static bool32 PresetBerryReviewHasMove(const struct EmeraldChampionsBattleSet *preset, enum Move move)
{
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        if (preset->moves[i] == move)
            return TRUE;
    return FALSE;
}

static u32 PresetBerryReviewEvTotal(struct Pokemon *mon)
{
    u32 total = 0;

    for (u32 stat = 0; stat < NUM_STATS; stat++)
        total += GetMonData(mon, MON_DATA_HP_EV + stat);
    return total;
}

static void PrintPresetBerryReview(
    const struct PresetBerryReviewCase *review,
    const u8 *levels,
    u32 levelCount)
{
    const struct EmeraldChampionsBattleSet *preset =
        GetEmeraldChampionsRawBattleSetForFormat(review->species, review->choice, review->format);

    EXPECT(preset != NULL);
    if (preset == NULL)
    {
        Test_MgbaPrintf("BERRY_REVIEW missing species=%S format=%d choice=%d",
            GetSpeciesName(review->species), review->format, review->choice);
        return;
    }

    for (u32 i = 0; i < levelCount; i++)
    {
        struct Pokemon mon;
        u8 level = levels[i];
        u8 result;
        u32 rawHp = CalculateSpeciesStat(
            review->species, preset->nature, STAT_HP, level, preset->evs[STAT_HP]);
        bool32 hasDrum = PresetBerryReviewHasMove(preset, MOVE_BELLY_DRUM);
        bool32 hasSubstitute = PresetBerryReviewHasMove(preset, MOVE_SUBSTITUTE);

        CreateRandomMonWithIVs(&mon, review->species, level, MAX_PER_STAT_IVS);
        result = ApplyEmeraldChampionsScriptedSet(&mon, preset);
        EXPECT_EQ(result, EC_BATTLE_SET_SUCCESS);
        if (result == EC_BATTLE_SET_FAILED)
        {
            Test_MgbaPrintf("BERRY_REVIEW applyFailed species=%S format=%d choice=%d level=%d item=%S ability=%d rawHp=%d rawHpEv=%d",
                GetSpeciesName(review->species), review->format, review->choice, level,
                GetItemName(preset->item), preset->ability, rawHp, preset->evs[STAT_HP]);
            continue;
        }

        u32 maxHp = GetMonData(&mon, MON_DATA_MAX_HP);
        u32 currentHp = GetMonData(&mon, MON_DATA_HP);
        u32 appliedHpEv = GetMonData(&mon, MON_DATA_HP_EV);
        EXPECT_EQ(currentHp, maxHp);
        EXPECT(PresetBerryReviewEvTotal(&mon) <= MAX_TOTAL_EVS);
        u32 subCost = maxHp / 4;
        u32 drumCost = maxHp / 2;
        u32 afterSub = maxHp - subCost;
        u32 afterDrum = maxHp - drumCost;
        u32 afterSubDrum = maxHp - subCost - drumCost;
        bool32 gluttonyOrBerryJuice = preset->ability == ABILITY_GLUTTONY
                                  || preset->item == ITEM_BERRY_JUICE;

        Test_MgbaPrintf("BERRY_REVIEW species=%S format=%d choice=%d level=%d apply=%d item=%S ability=%d drum=%d sub=%d rawHp=%d rawHpEv=%d postHp=%d currentHp=%d postHpEv=%d evTotal=%d odd=%d gluttonyOrJuice=%d",
            GetSpeciesName(review->species), review->format, review->choice, level, result,
            GetItemName(preset->item), preset->ability, hasDrum, hasSubstitute,
            rawHp, preset->evs[STAT_HP], maxHp, currentHp, appliedHpEv,
            PresetBerryReviewEvTotal(&mon), maxHp & 1, gluttonyOrBerryJuice);
        Test_MgbaPrintf("BERRY_REVIEW_ARITH_NOT_BATTLE species=%S format=%d choice=%d level=%d full=%d subCost=%d afterSub=%d drumCost=%d afterDrum=%d afterSubDrum=%d halfDown=%d halfUp=%d quarterDown=%d",
            GetSpeciesName(review->species), review->format, review->choice, level,
            maxHp, subCost, afterSub, drumCost, afterDrum, afterSubDrum,
            maxHp / 2, (maxHp + 1) / 2, maxHp / 4);
    }
}

TEST("EC preset berry review: non-Sitrus Belly Drum application by level")
{
    static const u8 levels[] = {2, 5, 10, 12, 14, 20, 30, 50, 100};
    static const struct PresetBerryReviewCase reviews[] =
    {
        {SPECIES_POLIWAG, EC_BATTLE_FORMAT_DOUBLES, 1},
        {SPECIES_POLIWAG, EC_BATTLE_FORMAT_SINGLES, 0},
        {SPECIES_SNORLAX, EC_BATTLE_FORMAT_DOUBLES, 0},
        {SPECIES_ZIGZAGOON, EC_BATTLE_FORMAT_DOUBLES, 0},
        {SPECIES_ZIGZAGOON, EC_BATTLE_FORMAT_SINGLES, 0},
        {SPECIES_LINOONE, EC_BATTLE_FORMAT_DOUBLES, 0},
        {SPECIES_LINOONE, EC_BATTLE_FORMAT_SINGLES, 0},
        {SPECIES_MUNCHLAX, EC_BATTLE_FORMAT_DOUBLES, 0},
    };

    for (u32 i = 0; i < ARRAY_COUNT(reviews); i++)
        PrintPresetBerryReview(&reviews[i], levels, ARRAY_COUNT(levels));
}

TEST("EC preset berry review: Substitute plus Drum or Sitrus application by level")
{
    static const u8 levels[] = {10, 12, 14, 50, 100};
    static const struct PresetBerryReviewCase reviews[] =
    {
        {SPECIES_EISCUE, EC_BATTLE_FORMAT_SINGLES, 0},
        {SPECIES_HAWLUCHA, EC_BATTLE_FORMAT_SINGLES, 0},
    };

    for (u32 i = 0; i < ARRAY_COUNT(reviews); i++)
        PrintPresetBerryReview(&reviews[i], levels, ARRAY_COUNT(levels));
}
