#include "global.h"
#include "emerald_champions_battle_sets.h"
#include "item.h"
#include "party_menu.h"
#include "pokemon.h"
#include "random.h"
#include "string_util.h"
#include "test/test.h"
#include "text.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/species.h"

TEST("Keldeo follows Secret Sword form changes in party and PC movesets")
{
    struct Pokemon mon;
    struct BoxPokemon boxMon;

    CreateMon(&mon, SPECIES_KELDEO_ORDINARY, 40, 0, OTID_STRUCT_PLAYER_ID);
    for (u32 i = 0; i < MAX_MON_MOVES; i++)
        SetMonMoveSlot(&mon, MOVE_NONE, i);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_KELDEO_ORDINARY);

    SetMonMoveSlot(&mon, MOVE_SECRET_SWORD, 0);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_KELDEO_RESOLUTE);
    SetMonMoveSlot(&mon, MOVE_NONE, 0);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_KELDEO_ORDINARY);

    CreateBoxMon(&boxMon, SPECIES_KELDEO_ORDINARY, 40, 0, OTID_STRUCT_PLAYER_ID);
    SetBoxMonMoveSlot(&boxMon, MOVE_SECRET_SWORD, 0);
    EXPECT(TryBoxMonFormChangeOnMove(&boxMon, MOVE_SECRET_SWORD));
    EXPECT_EQ(GetBoxMonData(&boxMon, MON_DATA_SPECIES), SPECIES_KELDEO_RESOLUTE);
    SetBoxMonMoveSlot(&boxMon, MOVE_NONE, 0);
    EXPECT(TryBoxMonFormChangeOnMove(&boxMon, MOVE_SECRET_SWORD));
    EXPECT_EQ(GetBoxMonData(&boxMon, MON_DATA_SPECIES), SPECIES_KELDEO_ORDINARY);
}

TEST("Emerald Champions Keldeo presets match their Secret Sword form")
{
    static const enum Species forms[] = {SPECIES_KELDEO_ORDINARY, SPECIES_KELDEO_RESOLUTE};
    struct Pokemon mon;

    for (u32 form = 0; form < ARRAY_COUNT(forms); form++)
    {
        for (u8 format = 0; format < EC_BATTLE_FORMAT_COUNT; format++)
        {
            CreateMon(&mon, forms[form], 40, 0, OTID_STRUCT_PLAYER_ID);
            u8 count = GetEmeraldChampionsBattleSetCountForFormat(&mon, format);
            EXPECT_GT(count, 0);
            for (u8 choice = 0; choice < count; choice++)
            {
                bool32 wantsSecretSword = FALSE;
                CreateMon(&mon, forms[form], 40, 0, OTID_STRUCT_PLAYER_ID);
                const struct EmeraldChampionsBattleSet *preset =
                    GetEmeraldChampionsBattleSetPresetForFormat(&mon, choice, format);
                EXPECT(preset != NULL);
                for (u32 slot = 0; slot < MAX_MON_MOVES; slot++)
                    wantsSecretSword |= preset->moves[slot] == MOVE_SECRET_SWORD;

                EXPECT_NE(ApplyEmeraldChampionsBattleSetChoiceForFormat(&mon, choice, format), EC_BATTLE_SET_FAILED);
                EXPECT_EQ(MonKnowsMove(&mon, MOVE_SECRET_SWORD), wantsSecretSword);
                EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES),
                          wantsSecretSword ? SPECIES_KELDEO_RESOLUTE : SPECIES_KELDEO_ORDINARY);
            }
        }
    }

    // Wild Keldeo's native form must follow the resulting moveset, including
    // Ordinary presets that do not contain Secret Sword.
    SeedRng(0x4B454C44);
    for (u32 i = 0; i < 32; i++)
    {
        CreateMon(&mon, SPECIES_KELDEO_ORDINARY, 40, 0, OTID_STRUCT_PLAYER_ID);
        EXPECT_NE(ApplyEmeraldChampionsRandomWildSet(&mon), EC_BATTLE_SET_FAILED);
        if (MonKnowsMove(&mon, MOVE_SECRET_SWORD))
        {
            EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_KELDEO_RESOLUTE);
        }
        else
        {
            EXPECT_EQ(GetMonData(&mon, MON_DATA_SPECIES), SPECIES_KELDEO_ORDINARY);
        }
    }
}

TEST("Pokemon Storage held-item messages fit every item name")
{
    enum Item item = ITEM_NONE;
    u8 text[ITEM_NAME_LENGTH + 32];

    for (u32 i = 1; i < ITEMS_COUNT; i++)
        PARAMETRIZE_LABEL("%S", gItemsInfo[i].name) { item = i; }

    StringCopy(text, gItemsInfo[item].name);
    StringAppend(text, COMPOUND_STRING(" is now held."));
    u32 fontId = GetFontIdToFit(text, FONT_NORMAL, 0, 18 * TILE_WIDTH);
    EXPECT_LE(GetStringWidth(fontId, text, 0), 18 * TILE_WIDTH);
}

TEST("Unsafe Frontier recorded battles remain player-inaccessible")
{
    EXPECT(!B_RECORDED_BATTLES_ENABLED);
}
