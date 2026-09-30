#include "global.h"
#include "test/test.h"
#include "pokemon.h"
#include "daycare.h"
#include "item.h"
#include "event_data.h"
#include "emerald_champions_battle_sets.h"
#include "constants/items.h"
#include "constants/move_relearner.h"
#include "constants/flags.h"
#include "../src/data/pokemon/emerald_champions_iconic_moves.h"

static void ClearIconicBadges(void)
{
    for (u32 flag = FLAG_BADGE01_GET; flag <= FLAG_BADGE08_GET; flag++)
        FlagClear(flag);
}

static void ForgetMoves(struct BoxPokemon *mon)
{
    for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
        SetBoxMonMoveSlot(mon, MOVE_NONE, slot);
}

TEST("Iconic tutor: authored receipt families agree with breeding ancestry")
{
    for (u32 i = 0; i < ARRAY_COUNT(sEmeraldChampionsIconicMoves); i++)
        EXPECT_EQ(sEmeraldChampionsIconicMoves[i].family, GetEggSpecies(sEmeraldChampionsIconicMoves[i].species));
}

TEST("Iconic tutor: unrelated non-evolving species returns an empty lesson list")
{
    ClearIconicBadges();
    struct Pokemon mon;
    CreateMonWithIVs(&mon, SPECIES_DITTO, 20, 0, OTID_STRUCT_PLAYER_ID, 31);
    u16 moves[MAX_RELEARNER_MOVES];
    EXPECT_EQ(GetEmeraldChampionsIconicMovesToLearn(&mon.box, moves), 0);
    GetEmeraldChampionsPreparationMovesToLearn(&mon.box, NULL);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_ICONIC_MOVES), 0);
    EXPECT(!IsIconicMoveUnlocked(&mon.box, MOVE_FLY));
}

TEST("Iconic tutor: a paid receipt bit zero offers only its own family's lesson")
{
    enum Species species;
    enum Move move;
    PARAMETRIZE { species = SPECIES_PICHU; move = MOVE_FLY; }
    PARAMETRIZE { species = SPECIES_EEVEE; move = MOVE_VEEVEE_VOLLEY; }
    PARAMETRIZE { species = SPECIES_MAGIKARP; move = MOVE_DRAGON_RAGE; }
    PARAMETRIZE { species = SPECIES_IGGLYBUFF; move = MOVE_SPARKLING_ARIA; }
    PARAMETRIZE { species = SPECIES_NINETALES; move = MOVE_BITTER_MALICE; }
    PARAMETRIZE { species = SPECIES_MILOTIC; move = MOVE_SPARKLING_ARIA; }
    PARAMETRIZE { species = SPECIES_SWAMPERT; move = MOVE_WAVE_CRASH; }
    PARAMETRIZE { species = SPECIES_LATIAS; move = MOVE_LUSTER_PURGE; }
    PARAMETRIZE { species = SPECIES_LATIOS; move = MOVE_MIST_BALL; }

    ClearIconicBadges();
    ClearBag();
    struct Pokemon mon;
    CreateMonWithIVs(&mon, species, 20, 0, OTID_STRUCT_PLAYER_ID, 31);
    ForgetMoves(&mon.box);
    SetMonMoveSlot(&mon, move, 0);
    EXPECT(AddBagItem(ITEM_GOLD_BOTTLE_CAP, 1));
    EXPECT(PayForIconicMove(&mon.box, move, ITEM_GOLD_BOTTLE_CAP));
    EXPECT_EQ(GetMonData(&mon, MON_DATA_ICONIC_MOVES), 1);
    ForgetMoves(&mon.box);
    u16 moves[MAX_RELEARNER_MOVES];
    EXPECT_EQ(GetEmeraldChampionsIconicMovesToLearn(&mon.box, moves), 1);
    EXPECT_EQ(moves[0], move);
    EXPECT(IsIconicMoveUnlocked(&mon.box, move));
}

TEST("Iconic tutor: boxed evolved Eevee keeps partner lessons without badges or caps")
{
    ClearIconicBadges();
    ClearBag();
    struct Pokemon mon;
    CreateMonWithIVs(&mon, SPECIES_EEVEE, 20, 0, OTID_STRUCT_PLAYER_ID, 31);
    SetMonMoveSlot(&mon, MOVE_VEEVEE_VOLLEY, 0);
    EXPECT(AddBagItem(ITEM_GOLD_BOTTLE_CAP, 1));
    EXPECT(PayForIconicMove(&mon.box, MOVE_VEEVEE_VOLLEY, ITEM_GOLD_BOTTLE_CAP));
    struct BoxPokemon boxed = mon.box;
    enum Species evolution = SPECIES_FLAREON;
    SetBoxMonData(&boxed, MON_DATA_SPECIES, &evolution);
    ForgetMoves(&boxed);
    u16 moves[MAX_RELEARNER_MOVES];
    EXPECT_EQ(GetEmeraldChampionsIconicMovesToLearn(&boxed, moves), 1);
    EXPECT_EQ(moves[0], MOVE_VEEVEE_VOLLEY);
    SetBoxMonMoveSlot(&boxed, MOVE_VEEVEE_VOLLEY, 0);
    EXPECT(PayForIconicMove(&boxed, MOVE_VEEVEE_VOLLEY, ITEM_BOTTLE_CAP));
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_BOTTLE_CAP), 0);
}

TEST("Iconic tutor: regional evolution preserves purchased Fly without unlocking other lessons")
{
    ClearIconicBadges();
    ClearBag();
    struct Pokemon mon;
    CreateMonWithIVs(&mon, SPECIES_PIKACHU, 20, 0, OTID_STRUCT_PLAYER_ID, 31);
    SetMonMoveSlot(&mon, MOVE_FLY, 0);
    EXPECT(AddBagItem(ITEM_GOLD_BOTTLE_CAP, 1));
    EXPECT(PayForIconicMove(&mon.box, MOVE_FLY, ITEM_GOLD_BOTTLE_CAP));
    enum Species evolution = SPECIES_RAICHU_ALOLA;
    SetMonData(&mon, MON_DATA_SPECIES, &evolution);
    ForgetMoves(&mon.box);
    u16 moves[MAX_RELEARNER_MOVES];
    EXPECT_EQ(GetEmeraldChampionsIconicMovesToLearn(&mon.box, moves), 1);
    EXPECT_EQ(moves[0], MOVE_FLY);
    EXPECT(!IsIconicMoveUnlocked(&mon.box, MOVE_VEEVEE_VOLLEY));
}

TEST("Iconic tutor: legacy known lessons migrate before forgetting including unlisted relatives")
{
    enum Species species;
    enum Move move;
    bool32 preparation;
    PARAMETRIZE { species = SPECIES_GLACEON; move = MOVE_SPARKLY_SWIRL; preparation = FALSE; }
    PARAMETRIZE { species = SPECIES_GLACEON; move = MOVE_SPARKLY_SWIRL; preparation = TRUE; }
    PARAMETRIZE { species = SPECIES_TREECKO; move = MOVE_DRAGON_HAMMER; preparation = FALSE; }
    PARAMETRIZE { species = SPECIES_SHEDINJA; move = MOVE_FIRST_IMPRESSION; preparation = TRUE; }

    ClearIconicBadges();
    struct Pokemon mon;
    CreateMonWithIVs(&mon, species, 20, 0, OTID_STRUCT_PLAYER_ID, 31);
    ForgetMoves(&mon.box);
    SetMonMoveSlot(&mon, move, 0);
    EXPECT(!IsIconicMoveUnlocked(&mon.box, move));
    if (preparation)
        GetEmeraldChampionsPreparationMovesToLearn(&mon.box, NULL);
    else
        EXPECT_EQ(GetEmeraldChampionsIconicMovesToLearn(&mon.box, NULL), 0);
    EXPECT(IsIconicMoveUnlocked(&mon.box, move));
    ForgetMoves(&mon.box);
    u16 moves[MAX_RELEARNER_MOVES];
    EXPECT_EQ(GetEmeraldChampionsIconicMovesToLearn(&mon.box, moves), 1);
    EXPECT_EQ(moves[0], move);
}

TEST("Iconic tutor: unrelated species cannot migrate an iconic lesson or spend a cap on it")
{
    ClearIconicBadges();
    ClearBag();
    struct Pokemon mon;
    CreateMonWithIVs(&mon, SPECIES_EEVEE, 20, 0, OTID_STRUCT_PLAYER_ID, 31);
    ForgetMoves(&mon.box);
    SetMonMoveSlot(&mon, MOVE_FLY, 0);
    EXPECT(AddBagItem(ITEM_GOLD_BOTTLE_CAP, 1));
    EXPECT_EQ(GetEmeraldChampionsIconicMovesToLearn(&mon.box, NULL), 0);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_ICONIC_MOVES), 0);
    EXPECT(!PayForIconicMove(&mon.box, MOVE_FLY, ITEM_GOLD_BOTTLE_CAP));
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_GOLD_BOTTLE_CAP), 1);
}

TEST("Iconic tutor: fresh lessons remain gated by badge count")
{
    ClearIconicBadges();
    struct Pokemon mon;
    CreateMonWithIVs(&mon, SPECIES_PIKACHU, 20, 0, OTID_STRUCT_PLAYER_ID, 31);
    ForgetMoves(&mon.box);
    EXPECT_EQ(GetEmeraldChampionsIconicMovesToLearn(&mon.box, NULL), 0);
    FlagSet(FLAG_BADGE01_GET);
    EXPECT_EQ(GetEmeraldChampionsIconicMovesToLearn(&mon.box, NULL), 0);
    FlagSet(FLAG_BADGE02_GET);
    EXPECT_EQ(GetEmeraldChampionsIconicMovesToLearn(&mon.box, NULL), 2);
    FlagSet(FLAG_BADGE03_GET);
    EXPECT_EQ(GetEmeraldChampionsIconicMovesToLearn(&mon.box, NULL), 5);
    ClearIconicBadges();
}

TEST("Iconic catalogue: every listed lesson agrees with the actual tutor without spending or teaching")
{
    ClearBag();
    EXPECT(AddBagItem(ITEM_GOLD_BOTTLE_CAP, 1));
    for (u32 flag = FLAG_BADGE01_GET; flag <= FLAG_BADGE08_GET; flag++)
        FlagSet(flag);
    u32 count = 0;
    enum Species species;
    while ((species = GetEmeraldChampionsIconicTutorSpecies(count)) != SPECIES_NONE)
    {
        for (u32 i = 0; i < count; i++)
            EXPECT_NE(GetEmeraldChampionsIconicTutorSpecies(i), species);
        struct Pokemon mon;
        CreateMonWithIVs(&mon, species, 20, 0, OTID_STRUCT_PLAYER_ID, 31);
        ForgetMoves(&mon.box);
        struct Pokemon before = mon;
        u16 offered[MAX_RELEARNER_MOVES];
        u32 offeredCount = GetEmeraldChampionsIconicMovesToLearn(&mon.box, offered);
        u32 lesson = 0;
        enum Move move;
        u8 badges;
        while (GetEmeraldChampionsIconicTutorLesson(species, lesson, &move, &badges))
        {
            EXPECT_LT(lesson, offeredCount);
            if (lesson < offeredCount)
                EXPECT_EQ(move, offered[lesson]);
            EXPECT_LE(badges, 8);
            lesson++;
        }
        EXPECT_EQ(lesson, offeredCount);
        EXPECT_EQ(move, MOVE_NONE);
        EXPECT_EQ(memcmp(&mon, &before, sizeof(mon)), 0);
        count++;
    }
    EXPECT_GT(count, 0);
    EXPECT_EQ(CountTotalItemQuantityInBag(ITEM_GOLD_BOTTLE_CAP), 1);
    ClearIconicBadges();
    // Browsing before an unlock still discloses the later lesson.
    enum Move move;
    u8 badges;
    EXPECT(GetEmeraldChampionsIconicTutorLesson(SPECIES_EEVEE, 8, &move, &badges));
    EXPECT_EQ(move, MOVE_SPARKLY_SWIRL);
    EXPECT_EQ(badges, 5);
    EXPECT(GetEmeraldChampionsIconicTutorLesson(SPECIES_RAICHU_ALOLA, 0, &move, &badges));
    EXPECT_EQ(move, MOVE_FLY);
    EXPECT_EQ(badges, 2);
}
