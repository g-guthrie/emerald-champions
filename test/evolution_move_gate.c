#include "global.h"
#include "test/test.h"
#include "pokemon.h"
#include "event_data.h"
#include "emerald_champions_battle_sets.h"
#include "field_specials.h"
#include "battle_tent.h"

TEST("Evolution move gate: tutor offers Ancient Power to Yanma only from level 33")
{
    u32 level;
    PARAMETRIZE { level = 20; }
    PARAMETRIZE { level = 32; }
    PARAMETRIZE { level = 33; }
    struct Pokemon mon;
    u16 moves[MOVES_COUNT_ALL];
    CreateMon(&mon, SPECIES_YANMA, level, 0, OTID_STRUCT_PLAYER_ID);
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
        SetMonMoveSlot(&mon, MOVE_NONE, slot);
    bool32 found = FALSE;
    u32 count = GetEmeraldChampionsPreparationMovesToLearn(&mon.box, moves);
    for (u32 i = 0; i < count; i++)
        found |= moves[i] == MOVE_ANCIENT_POWER;
    EXPECT_EQ(found, level >= 33);
}

TEST("Evolution move gate: trigger without natural learn level waits for badge four")
{
    FlagClear(FLAG_BADGE04_GET);
    EXPECT(IsEmeraldChampionsEvolutionMoveLocked(SPECIES_STANTLER, MOVE_PSYSHIELD_BASH, 100));
    EXPECT(!IsEmeraldChampionsEvolutionMoveLocked(SPECIES_YANMA, MOVE_PROTECT, 1));
    FlagSet(FLAG_BADGE04_GET);
    EXPECT(!IsEmeraldChampionsEvolutionMoveLocked(SPECIES_STANTLER, MOVE_PSYSHIELD_BASH, 1));
    EXPECT(IsEmeraldChampionsEvolutionMoveLocked(SPECIES_YANMA, MOVE_ANCIENT_POWER, 32));
    FlagClear(FLAG_BADGE04_GET);
}

TEST("Thick Club shelf requires badge four even after legitimate acquisition")
{
    FlagClear(FLAG_BADGE04_GET);
    EmeraldChampions_UnlockBattleItem(ITEM_THICK_CLUB);
    EXPECT(!IsEmeraldChampionsBattleItemUnlocked(ITEM_THICK_CLUB));
    FlagSet(FLAG_BADGE04_GET);
    EXPECT(IsEmeraldChampionsBattleItemUnlocked(ITEM_THICK_CLUB));
    FlagClear(FLAG_BADGE04_GET);
}

TEST("Slateport Tent uses held gear before Wattson and evolution prizes afterward")
{
    u32 unlocked;
    PARAMETRIZE { unlocked = FALSE; }
    PARAMETRIZE { unlocked = TRUE; }
    if (unlocked)
        FlagSet(FLAG_BADGE03_GET);
    else
        FlagClear(FLAG_BADGE03_GET);
    for (u32 i = 0; i < 100; i++)
    {
        gSaveBlock2Ptr->frontier.slateportTentPrize = ITEM_NONE;
        SetChampionsTentPrize(0);
        u16 prize = gSaveBlock2Ptr->frontier.slateportTentPrize;
        bool32 early = prize == ITEM_CHARCOAL || prize == ITEM_MYSTIC_WATER
            || prize == ITEM_MIRACLE_SEED || prize == ITEM_MAGNET
            || prize == ITEM_SOFT_SAND || prize == ITEM_HARD_STONE;
        EXPECT_EQ(early, !unlocked);
    }
    gSaveBlock2Ptr->frontier.slateportTentPrize = ITEM_NONE;
    FlagClear(FLAG_BADGE03_GET);
}
