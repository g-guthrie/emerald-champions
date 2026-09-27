#include "global.h"
#include "test/test.h"
#include "emerald_champions_battle_sets.h"
#include "pokemon.h"
#include "regions.h"
#include "rtc.h"
#include "move.h"
#include "overworld.h"
#include "constants/region_map_sections.h"
#include "constants/moves.h"
#include "constants/rtc.h"

// Every evolution a player can reach in this game has to go through something
// the game actually runs: the Leveler (EVO_MODE_NORMAL), a won battle
// (EVO_LEVEL_BATTLE_ONLY), an item from the Bag, or Shedinja's split. No script
// calls tryspecialevo and there is no spin detection, so script triggers and
// spins never fire; a link trade is never required, so a trade evolution needs
// a solo route to the same target. Conditions must be ones the player can meet
// in Hoenn with this game's services (Bonding, Nature, EV editor, tutor, time).

static const u16 sEvolutionShopStock[] =
{
#include "../src/data/emerald_champions_paid_evolution_items.h"
    ITEM_NONE,
};

static bool32 IsSoldByEvolutionSpecialist(enum Item item)
{
    for (u32 i = 0; sEvolutionShopStock[i] != ITEM_NONE; i++)
    {
        if (sEvolutionShopStock[i] == item)
            return TRUE;
    }
    return FALSE;
}

// The Center tutor teaches every legal move; the relearner adds level-up moves.
static bool32 CanPlayerTeachMove(enum Species species, enum Move move)
{
    if (CanSpeciesUseEmeraldChampionsPreparationMove(species, move))
        return TRUE;
    for (enum Species s = species; s != SPECIES_NONE; s = GetSpeciesPreEvolution(s))
    {
        const struct LevelUpMove *learnset = GetSpeciesLevelUpLearnset(s);
        for (u32 i = 0; learnset[i].move != LEVEL_UP_MOVE_END; i++)
        {
            if (learnset[i].move == move)
                return TRUE;
        }
    }
    return FALSE;
}

static bool32 CanPlayerTeachMoveOfType(enum Species species, enum Type type)
{
    for (enum Move move = MOVE_NONE + 1; move < MOVES_COUNT; move++)
    {
        if (GetMoveType(move) == type && CanPlayerTeachMove(species, move))
            return TRUE;
    }
    return FALSE;
}

static bool32 IsConditionMeetable(enum Species species, const struct EvolutionParam *param)
{
    switch ((enum EvolutionConditions)param->condition)
    {
    case IF_GENDER:
    case IF_MIN_FRIENDSHIP:      // Bonding service sets it.
    case IF_ATK_GT_DEF:          // Center EV editor moves it.
    case IF_ATK_EQ_DEF:
    case IF_ATK_LT_DEF:
    case IF_PID_UPPER_MODULO_10_GT:
    case IF_PID_UPPER_MODULO_10_EQ:
    case IF_PID_UPPER_MODULO_10_LT:
    case IF_PID_MODULO_100_GT:
    case IF_PID_MODULO_100_EQ:
    case IF_PID_MODULO_100_LT:
    case IF_NATURE:              // Nature service changes it.
    case IF_AMPED_NATURE:
    case IF_LOW_KEY_NATURE:
    case IF_IN_MAP:
    case IF_IN_MAPSEC:
    case IF_WEATHER:
    case IF_BAG_ITEM_COUNT:
        return TRUE;
    case IF_TIME:
    case IF_NOT_TIME:            // The real-time clock drives time of day.
        return param->arg1 < TIMES_OF_DAY_COUNT;
    case IF_HOLD_ITEM:
        return IsSoldByEvolutionSpecialist(param->arg1);
    case IF_SPECIES_IN_PARTY:
        return IsSpeciesEnabled(param->arg1);
    case IF_TYPE_IN_PARTY:
        return param->arg1 < NUMBER_OF_MON_TYPES;
    case IF_KNOWS_MOVE:
        return CanPlayerTeachMove(species, param->arg1);
    case IF_KNOWS_MOVE_TYPE:
        return CanPlayerTeachMoveOfType(species, param->arg1);
    case IF_REGION:              // The campaign never leaves Hoenn.
        return param->arg1 == REGION_HOENN;
    case IF_NOT_REGION:
        return param->arg1 != REGION_HOENN;
    // Link trades, battle counters, contest stats and follower steps: nothing
    // in the campaign feeds these any more.
    case IF_TRADE_PARTNER_SPECIES:
    case IF_RECOIL_DAMAGE_GE:
    case IF_CURRENT_DAMAGE_GE:
    case IF_CRITICAL_HITS_GE:
    case IF_USED_MOVE_X_TIMES:
    case IF_DEFEAT_X_WITH_ITEMS:
    case IF_MIN_OVERWORLD_STEPS:
    case IF_MIN_BEAUTY:
    case IF_MIN_COOLNESS:
    case IF_MIN_SMARTNESS:
    case IF_MIN_TOUGHNESS:
    case IF_MIN_CUTENESS:
    default:
        return FALSE;
    }
}

static bool32 AreConditionsMeetable(enum Species species, const struct EvolutionParam *params)
{
    for (u32 i = 0; params != NULL && params[i].condition != CONDITIONS_END; i++)
    {
        if (!IsConditionMeetable(species, &params[i]))
            return FALSE;
    }
    return TRUE;
}

static bool32 IsSoloEvolution(enum Species species, const struct Evolution *evolution)
{
    switch (evolution->method)
    {
    case EVO_LEVEL:
    case EVO_LEVEL_BATTLE_ONLY:
    case EVO_SPLIT_FROM_EVO:
        return AreConditionsMeetable(species, evolution->params);
    case EVO_ITEM:
        return IsSoldByEvolutionSpecialist(evolution->param)
            && AreConditionsMeetable(species, evolution->params);
    default:
        return FALSE;
    }
}

TEST("Evolutions: every evolution uses a method the campaign can execute")
{
    u32 failures = 0;

    for (enum Species species = SPECIES_NONE + 1; species < NUM_SPECIES; species++)
    {
        const struct Evolution *evolutions;

        if (!IsSpeciesEnabled(species))
            continue;
        evolutions = GetSpeciesEvolutions(species);
        if (evolutions == NULL)
            continue;
        for (u32 i = 0; evolutions[i].method != EVOLUTIONS_END; i++)
        {
            const struct Evolution *evolution = &evolutions[i];
            bool32 reachable = FALSE;

            if (evolution->method == EVO_NONE || SanitizeSpeciesId(evolution->targetSpecies) == SPECIES_NONE)
                continue;
            if (evolution->method == EVO_TRADE)
            {
                // A link trade may still evolve it, but a solo route must exist.
                for (u32 j = 0; evolutions[j].method != EVOLUTIONS_END; j++)
                {
                    if (evolutions[j].targetSpecies == evolution->targetSpecies
                     && IsSoloEvolution(species, &evolutions[j]))
                        reachable = TRUE;
                }
            }
            else
            {
                reachable = IsSoloEvolution(species, evolution);
            }
            if (!reachable)
            {
                Test_MgbaPrintf("unreachable evolution species=%d target=%d method=%d param=%d",
                    species, evolution->targetSpecies, evolution->method, evolution->param);
                failures++;
            }
        }
    }
    EXPECT_EQ(failures, 0);
}

TEST("Evolutions: Hoenn locations select regional branches and keep ordinary alternatives")
{
    static const struct { enum Species from, regional, ordinary; enum Item item; u16 mapsec; } stones[] = {
        {SPECIES_PIKACHU, SPECIES_RAICHU_ALOLA, SPECIES_RAICHU, ITEM_THUNDER_STONE, MAPSEC_MOSSDEEP_CITY},
        {SPECIES_EXEGGCUTE, SPECIES_EXEGGUTOR_ALOLA, SPECIES_EXEGGUTOR, ITEM_LEAF_STONE, MAPSEC_DEWFORD_TOWN},
    };
    u16 savedMapsec = gMapHeader.regionMapSectionId;
    struct Pokemon mon;
    for (u32 i = 0; i < ARRAY_COUNT(stones); i++)
    {
        CreateMon(&mon, stones[i].from, 40, 0, OTID_STRUCT_PLAYER_ID);
        gMapHeader.regionMapSectionId = stones[i].mapsec;
        EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_ITEM_CHECK, stones[i].item, NULL, NULL, CHECK_EVO), stones[i].regional);
        gMapHeader.regionMapSectionId = MAPSEC_LITTLEROOT_TOWN;
        EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_ITEM_CHECK, stones[i].item, NULL, NULL, CHECK_EVO), stones[i].ordinary);
    }
    CreateMon(&mon, SPECIES_MIME_JR, 40, 0, OTID_STRUCT_PLAYER_ID);
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
        SetMonMoveSlot(&mon, MOVE_NONE, slot);
    gMapHeader.regionMapSectionId = MAPSEC_SHOAL_CAVE;
    EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, NULL, CHECK_EVO), SPECIES_NONE);
    SetMonMoveSlot(&mon, MOVE_MIMIC, 0);
    EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, NULL, CHECK_EVO), SPECIES_MR_MIME_GALAR);
    gMapHeader.regionMapSectionId = MAPSEC_LITTLEROOT_TOWN;
    EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, NULL, CHECK_EVO), SPECIES_MR_MIME);

    CreateMon(&mon, SPECIES_YAMASK_GALAR, 33, 0, OTID_STRUCT_PLAYER_ID);
    gMapHeader.regionMapSectionId = MAPSEC_ROUTE_111;
    EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, NULL, CHECK_EVO), SPECIES_NONE);
    CreateMon(&mon, SPECIES_YAMASK_GALAR, 34, 0, OTID_STRUCT_PLAYER_ID);
    EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, NULL, CHECK_EVO), SPECIES_RUNERIGUS);
    gMapHeader.regionMapSectionId = MAPSEC_LITTLEROOT_TOWN;
    EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_NORMAL, ITEM_NONE, NULL, NULL, CHECK_EVO), SPECIES_NONE);
    gMapHeader.regionMapSectionId = savedMapsec;
}
