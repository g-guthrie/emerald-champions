#include "global.h"
#include "test/test.h"
#include "battle.h"
#include "battle_main.h"
#include "battle_message.h"
#include "battle_setup.h"
#include "item.h"
#include "main_menu.h"
#include "malloc.h"
#include "map_name_popup.h"
#include "overworld.h"
#include "party_menu.h"
#include "string_util.h"
#include "text.h"
#include "line_break.h"
#include "constants/abilities.h"
#include "constants/battle.h"
#include "constants/battle_string_ids.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/opponents.h"
#include "../src/data/map_group_count.h"
#include "test/overworld_script.h"

TEST("Move names fit on Pokemon Summary Screen")
{
    u32 i;
    const u32 fontId = FONT_NARROWER, widthPx = 72;
    enum Move move = MOVE_NONE;
    for (i = 1; i < MOVES_COUNT; i++)
    {
        PARAMETRIZE_LABEL("%S", GetMoveName(i)) { move = i; }
    }
    //DebugPrintf("Move %d: %S", GetStringWidth(fontId, GetMoveName(move), 0), GetMoveName(move));
    EXPECT_LE(GetStringWidth(fontId, GetMoveName(move), 0), widthPx);
}

TEST("Move names fit on Battle Screen")
{
    u32 i;
    const u32 fontId = FONT_NARROWER, widthPx = 64;
    enum Move move = MOVE_NONE;
    for (i = 1; i < MOVES_COUNT; i++)
    {
        PARAMETRIZE_LABEL("%S", GetMoveName(i)) { move = i; }
    }
    EXPECT_LE(GetStringWidth(fontId, GetMoveName(move), 0), widthPx);
}

TEST("Move names fit on Move Relearner Screen")
{
    u32 i;
    const u32 fontId = FONT_NARROWER, widthPx = 72;
    enum Move move = MOVE_NONE;
    for (i = 1; i < MOVES_COUNT; i++)
    {
        PARAMETRIZE_LABEL("%S", GetMoveName(i)) { move = i; }
    }
    EXPECT_LE(GetStringWidth(fontId, GetMoveName(move), 0), widthPx);
}

TEST("Move descriptions fit on Pokemon Summary Screen")
{
    u32 i;
    const u32 fontId = FONT_NARROWER, widthPx = 152;
    enum Move move = MOVE_NONE;
    for (i = 1; i < MOVES_COUNT_ALL; i++)
    {
        PARAMETRIZE_LABEL("%S", GetMoveDescription(i)) { move = i; }
    }
    EXPECT_LE(GetStringWidth(fontId, GetMoveDescription(move), 0), widthPx);
}

TEST("Move descriptions fit on battle move info window")
{
    u32 i;
    const u32 fontId = FONT_NARROWER, widthPx = 144;
    enum Move move = MOVE_NONE;
    for (i = 1; i < MOVES_COUNT_ALL; i++)
    {
        PARAMETRIZE_LABEL("%S", GetMoveDescription(i)) { move = i; }
    }
    EXPECT_LE(GetStringWidth(fontId, GetMoveDescription(move), 0), widthPx);
}

TEST("Item names fit on Bag Screen (list)")
{
    u32 i;
    const u32 fontId = FONT_NARROWER;
    const u32 berryWidthPx = 61, restWidthPx = 88;
    enum Item item = ITEM_NONE;
    for (i = 1; i < ITEMS_COUNT; i++)
    {
        PARAMETRIZE_LABEL("%S", gItemsInfo[i].name) { item = i; }
    }
    //DebugPrintf("Item %d: %S", GetStringWidth(fontId, gItemsInfo[item].name, 0), gItemsInfo[item].name);
    if (gItemsInfo[item].pocket == POCKET_BERRIES)
        EXPECT_LE(GetStringWidth(fontId, gItemsInfo[item].name, 0), berryWidthPx);
    else
        EXPECT_LE(GetStringWidth(fontId, gItemsInfo[item].name, 0), restWidthPx);
}

TEST("Item plural names fit on Bag Screen (left box)")
{
    u32 i;
    // -6 for the question mark in FONT_NORMAL.
    const u32 fontId = FONT_NARROWER, widthPx = 101 - 6;
    enum Item item = ITEM_NONE;
    u8 pluralName[ITEM_NAME_PLURAL_LENGTH + 1];
    for (i = 1; i < ITEMS_COUNT; i++)
    {
        PARAMETRIZE_LABEL("%S", gItemsInfo[i].name) { item = i; }
    }
    CopyItemNameHandlePlural(item, pluralName, 2);
    EXPECT_LE(GetStringWidth(fontId, pluralName, 0), widthPx);
}

TEST("Item names fit on PC Storage (list)")
{
    u32 i;
    const u32 fontId = FONT_NARROWER, widthPx = 73;
    enum Item item = ITEM_NONE;
    for (i = 1; i < ITEMS_COUNT; i++)
    {
        PARAMETRIZE_LABEL("%S", gItemsInfo[i].name) { item = i; }
    }
    EXPECT_LE(GetStringWidth(fontId, gItemsInfo[item].name, 0), widthPx);
}

TEST("Item plural names fit on PC storage (left box)")
{
    u32 i;
    // -6 for the question mark in FONT_NORMAL.
    const u32 fontId = FONT_NARROWER, widthPx = 104 - 6;
    enum Item item = ITEM_NONE;
    u8 pluralName[ITEM_NAME_PLURAL_LENGTH + 1];
    for (i = 1; i < ITEMS_COUNT; i++)
    {
        PARAMETRIZE_LABEL("%S", gItemsInfo[i].name) { item = i; }
    }
    CopyItemNameHandlePlural(item, pluralName, 2);
    EXPECT_LE(GetStringWidth(fontId, pluralName, 0), widthPx);
}

TEST("Item names fit on Pokemon Storage System")
{
    u32 i;
    const u32 fontId = FONT_SMALL_NARROWER, widthPx = 66;
    enum Item item = ITEM_NONE;
    for (i = 1; i < ITEMS_COUNT; i++)
    {
        if (gItemsInfo[i].importance) continue;
        PARAMETRIZE_LABEL("%S", gItemsInfo[i].name) { item = i; }
    }
    // All items explicitly listed here are too big to fit.
    switch (item)
    {
    case ITEM_UNREMARKABLE_TEACUP:
    case ITEM_MASTERPIECE_TEACUP:
    case ITEM_TWICE_SPICED_RADISH:
        EXPECT_GT(GetStringWidth(fontId, gItemsInfo[item].name, 0), widthPx);
        break;
    default:
        EXPECT_LE(GetStringWidth(fontId, gItemsInfo[item].name, 0), widthPx);
        break;
    }
}

TEST("Item names fit on Pokemon Summary Screen")
{
    u32 i;
    const u32 fontId = FONT_NARROWER, widthPx = 72;
    enum Item item = ITEM_NONE;
    for (i = 1; i < ITEMS_COUNT; i++)
    {
        if (gItemsInfo[i].importance) continue;
        PARAMETRIZE_LABEL("%S", gItemsInfo[i].name) { item = i; }
    }
    // All items explicitly listed here are too big to fit.
    switch (item)
    {
    case ITEM_UNREMARKABLE_TEACUP:
        EXPECT_GT(GetStringWidth(fontId, gItemsInfo[item].name, 0), widthPx);
        break;
    default:
        EXPECT_LE(GetStringWidth(fontId, gItemsInfo[item].name, 0), widthPx);
        break;
    }
}

TEST("Item names fit on Shop Screen")
{
    u32 i;
    const u32 fontId = FONT_NARROWER, widthPx = 84;
    enum Item item = ITEM_NONE;
    for (i = 1; i < ITEMS_COUNT; i++)
    {
        PARAMETRIZE_LABEL("%S", gItemsInfo[i].name) { item = i; }
    }
    EXPECT_LE(GetStringWidth(fontId, gItemsInfo[item].name, 0), widthPx);
}

TEST("Item descriptions fit on Bag and Shop Screen")
{
    u32 i;
    // Both description windows are 14 tiles wide and print at x = 3 with
    // FONT_NORMAL and zero letter spacing (item_menu.c and shop.c).
    const u32 fontId = FONT_NORMAL, widthPx = 14 * 8 - 3;
    enum Item item = ITEM_NONE;
    for (i = 1; i < ITEMS_COUNT; i++)
    {
        PARAMETRIZE_LABEL("%S", gItemsInfo[i].description) { item = i; }
    }
    EXPECT_LE(GetStringWidth(fontId, gItemsInfo[item].description, 0), widthPx);
}

TEST("Species names fit on Battle Screen HP box")
{
    enum Species i;
    u32 genderWidthPx;
    const u32 fontId = FONT_SMALL_NARROWER, widthPx = 55;
    enum Species species = SPECIES_NONE;
    genderWidthPx = GetStringWidth(fontId, COMPOUND_STRING("♂"), 0);
    for (i = SPECIES_NONE + 1; i < NUM_SPECIES; i++)
    {
        if (IsSpeciesEnabled(i))
        {
            PARAMETRIZE_LABEL("%S", gSpeciesInfo[i].speciesName) { species = i; }
        }
    }
    if (gSpeciesInfo[i].genderRatio != MON_GENDERLESS)
        EXPECT_LE(GetStringWidth(fontId, gSpeciesInfo[species].speciesName, 0) - genderWidthPx, widthPx);
    else
        EXPECT_LE(GetStringWidth(fontId, gSpeciesInfo[species].speciesName, 0), widthPx);
}

TEST("Species names fit on Party Screen")
{
    enum Species i;
    const u32 fontId = FONT_SMALL_NARROWER, widthPx = 50;
    enum Species species = SPECIES_NONE;
    for (i = SPECIES_NONE + 1; i < NUM_SPECIES; i++)
    {
        if (IsSpeciesEnabled(i))
        {
            PARAMETRIZE_LABEL("%S", gSpeciesInfo[i].speciesName) { species = i; }
        }
    }
    EXPECT_LE(GetStringWidth(fontId, gSpeciesInfo[species].speciesName, 0), widthPx);
}

TEST("Species names fit on Pokemon Summary Screen")
{
    enum Species i;
    const u32 fontId = FONT_NARROWER, widthPx = 63;
    enum Species species = SPECIES_NONE;
    for (i = SPECIES_NONE + 1; i < NUM_SPECIES; i++)
    {
        if (IsSpeciesEnabled(i))
        {
            PARAMETRIZE_LABEL("%S", gSpeciesInfo[i].speciesName) { species = i; }
        }
    }
    EXPECT_LE(GetStringWidth(fontId, gSpeciesInfo[species].speciesName, 0), widthPx);
}

TEST("Species names fit on Pokedex Screen")
{
    enum Species i;
    const u32 fontId = FONT_NARROWER, widthPx = 50;
    enum Species species = SPECIES_NONE;
    for (i = SPECIES_NONE + 1; i < NUM_SPECIES; i++)
    {
        if (IsSpeciesEnabled(i))
        {
            PARAMETRIZE_LABEL("%S", gSpeciesInfo[i].speciesName) { species = i; }
        }
    }
    EXPECT_LE(GetStringWidth(fontId, gSpeciesInfo[species].speciesName, 0), widthPx);
}

TEST("Species names fit on Pokedex Screen - Cries")
{
    enum Species i;
    const u32 fontId = FONT_NARROWER, widthPx = 60;
    enum Species species = SPECIES_NONE;
    for (i = SPECIES_NONE + 1; i < NUM_SPECIES; i++)
    {
        if (IsSpeciesEnabled(i))
        {
            PARAMETRIZE_LABEL("%S", gSpeciesInfo[i].speciesName) { species = i; }
        }
    }
    EXPECT_LE(GetStringWidth(fontId, gSpeciesInfo[species].speciesName, 0), widthPx);
}

TEST("Species names fit on Pokemon Storage System")
{
    enum Species i;
    enum Species species = SPECIES_NONE;
    for (i = SPECIES_NONE + 1; i < NUM_SPECIES; i++)
    {
        if (IsSpeciesEnabled(i))
        {
            PARAMETRIZE_LABEL("%S", gSpeciesInfo[i].speciesName) { species = i; }
        }
    }
    EXPECT_LE(GetStringWidth(FONT_NARROWER, gSpeciesInfo[species].speciesName, 0), 66);
    EXPECT_LE(GetStringWidth(FONT_SHORT_NARROWER, gSpeciesInfo[species].speciesName, 0), 60);
}

TEST("Species names fit on Hall of Fame")
{
    enum Species i;
    const u32 fontId = FONT_NARROWER, widthPx = 66;
    enum Species species = SPECIES_NONE;
    for (i = SPECIES_NONE + 1; i < NUM_SPECIES; i++)
    {
        if (IsSpeciesEnabled(i))
        {
            PARAMETRIZE_LABEL("%S", gSpeciesInfo[i].speciesName) { species = i; }
        }
    }
    EXPECT_LE(GetStringWidth(fontId, gSpeciesInfo[species].speciesName, 0), widthPx);
}

TEST("Species names fit on Naming Screen")
{
    enum Species i;
    const u32 fontId = FONT_NARROWER, widthPx = 64;
    enum Species species = SPECIES_NONE;
    for (i = SPECIES_NONE + 1; i < NUM_SPECIES; i++)
    {
        if (IsSpeciesEnabled(i))
        {
            PARAMETRIZE_LABEL("%S", gSpeciesInfo[i].speciesName) { species = i; }
        }
    }
    EXPECT_LE(GetStringWidth(fontId, gSpeciesInfo[species].speciesName, 0), widthPx);
}

TEST("Species names fit on PokeNav Condition Search Screen")
{
    enum Species i;
    const u32 fontId = FONT_NARROWER, widthPx = 60;
    enum Species species = SPECIES_NONE;
    for (i = SPECIES_NONE + 1; i < NUM_SPECIES; i++)
    {
        if (IsSpeciesEnabled(i))
        {
            PARAMETRIZE_LABEL("%S", gSpeciesInfo[i].speciesName) { species = i; }
        }
    }
    EXPECT_LE(GetStringWidth(fontId, gSpeciesInfo[species].speciesName, 0), widthPx);
}

TEST("Species names fit on PokeNav Ribbon Screen")
{
    enum Species i;
    const u32 fontId = FONT_NARROWER, widthPx = 60;
    enum Species species = SPECIES_NONE;
    for (i = SPECIES_NONE + 1; i < NUM_SPECIES; i++)
    {
        if (IsSpeciesEnabled(i))
        {
            PARAMETRIZE_LABEL("%S", gSpeciesInfo[i].speciesName) { species = i; }
        }
    }
    EXPECT_LE(GetStringWidth(fontId, gSpeciesInfo[species].speciesName, 0), widthPx);
}

TEST("Species names fit on PokeNav Ribbon List Screen")
{
    enum Species i;
    const u32 fontId = FONT_NARROWER, widthPx = 60;
    enum Species species = SPECIES_NONE;
    for (i = SPECIES_NONE + 1; i < NUM_SPECIES; i++)
    {
        if (IsSpeciesEnabled(i))
        {
            PARAMETRIZE_LABEL("%S", gSpeciesInfo[i].speciesName) { species = i; }
        }
    }
    EXPECT_LE(GetStringWidth(fontId, gSpeciesInfo[species].speciesName, 0), widthPx);
}

TEST("Species names fit on Battle Screen HP box for vanilla mons with the default font")
{
    enum Species i;
    u32 genderWidthPx;
    const u32 fontId = FONT_SMALL, widthPx = 55;
    enum Species species = SPECIES_NONE;
    genderWidthPx = GetStringWidth(fontId, COMPOUND_STRING("♂"), 0);
    for (i = SPECIES_NONE + 1; i < SPECIES_TURTWIG; i++)
    {
        if (IsSpeciesEnabled(i))
        {
            PARAMETRIZE_LABEL("%S", gSpeciesInfo[i].speciesName) { species = i; }
        }
    }
    if (gSpeciesInfo[i].genderRatio != MON_GENDERLESS)
        EXPECT_LE(GetStringWidth(fontId, gSpeciesInfo[species].speciesName, 0) - genderWidthPx, widthPx);
    else
        EXPECT_LE(GetStringWidth(fontId, gSpeciesInfo[species].speciesName, 0), widthPx);
}

TEST("Species dex entries fit on Pokedex Screen")
{
    enum Species i;
    const u32 fontId = FONT_NORMAL, widthPx = 224;
    enum Species species = SPECIES_NONE;
    for (i = SPECIES_NONE + 1; i < NUM_SPECIES; i++)
    {
        if (IsSpeciesEnabled(i))
        {
            PARAMETRIZE_LABEL("%S", gSpeciesInfo[i].description) { species = i; }
        }
    }
    EXPECT_LE(GetStringWidth(fontId, gSpeciesInfo[species].description, 0), widthPx);
}

TEST("Ability names fit on Pokemon Summary Screen")
{
    u32 i;
    const u32 fontId = FONT_NORMAL, widthPx = 144;
    enum Ability ability = ABILITY_NONE;
    for (i = 1; i < ABILITIES_COUNT; i++)
    {
        PARAMETRIZE_LABEL("%S", gAbilitiesInfo[i].name) { ability = i; }
    }
    EXPECT_LE(GetStringWidth(fontId, gAbilitiesInfo[ability].name, 0), widthPx);
}

TEST("Ability names fit on Ability Pop-Up")
{
    u32 i;
    const u32 fontId = FONT_SMALL_NARROWER, widthPx = 76;
    enum Ability ability = ABILITY_NONE;
    for (i = 1; i < ABILITIES_COUNT; i++)
    {
        PARAMETRIZE_LABEL("%S", gAbilitiesInfo[i].name) { ability = i; }
    }
    EXPECT_LE(GetStringWidth(fontId, gAbilitiesInfo[ability].name, 0), widthPx);
}

TEST("Ability descriptions fit on Pokemon Summary Screen")
{
    u32 i;
    const u32 fontId = FONT_NARROWER, widthPx = 146;
    enum Ability ability = ABILITY_NONE;
    for (i = 1; i < ABILITIES_COUNT; i++)
    {
        PARAMETRIZE_LABEL("%S", gAbilitiesInfo[i].description) { ability = i; }
    }
    EXPECT_LE(GetStringWidth(fontId, gAbilitiesInfo[ability].description, 0), widthPx);
}

TEST("Type names fit on Battle Screen")
{
    u32 i;
    const u32 fontId = FONT_NARROWER, widthPx = 39;
    enum Type type = TYPE_NORMAL;
    for (i = 0; i < NUMBER_OF_MON_TYPES; i++)
    {
        PARAMETRIZE_LABEL("%S", gTypesInfo[i].name) { type = i; }
    }
    EXPECT_LE(GetStringWidth(fontId, gTypesInfo[type].name, 0), widthPx);
}

TEST("Type names fit on Pokedex Search Screen")
{
    u32 i;
    const u32 fontId = FONT_NARROWER, widthPx = 38;
    enum Type type = TYPE_NORMAL;
    for (i = 0; i < NUMBER_OF_MON_TYPES; i++)
    {
        PARAMETRIZE_LABEL("%S", gTypesInfo[i].name) { type = i; }
    }
    EXPECT_LE(GetStringWidth(fontId, gTypesInfo[type].name, 0), widthPx);
}


TEST("Map names fit in popup")
{
    ASSUME(OW_POPUP_GENERATION == GEN_3);
    const u32 fontId = FONT_NARROWER;
    u32 widthPx = 80;
    s8 mapGroup = 0;
    s8 mapNum = 0;
    u8 mapName[MAP_POPUP_STRING_BUFFER_LENGTH - MAP_POPUP_PREFIX_BUFFER_LENGTH];
    for (u32 i = 0; MAP_GROUP_COUNT[i] != 0; i++)
    {
        for (u32 j = 0; j < MAP_GROUP_COUNT[i]; j++)
        {
            const struct MapHeader *mapHeader = Overworld_GetMapHeaderByGroupAndId(i, j);
            if (mapHeader->showMapName)
                PARAMETRIZE_LABEL("%S", GetPopUpMapName(mapName, mapHeader)) { mapGroup = i; mapNum = j;}
        }
    }
    EXPECT_LE(GetStringWidth(fontId, GetPopUpMapName(mapName, Overworld_GetMapHeaderByGroupAndId(mapGroup, mapNum)), 0), widthPx);
}

extern u16 sBattlerAbilities[MAX_BATTLERS_COUNT];
//*
#define BATTLE_STRING_BUFFER_SIZE 1000
TEST("Battle strings fit on the battle message window")
{
    u32 i, j, strWidth;
    u32 start = STRINGID_TABLE_START + 1;
    u32 end = STRINGID_COUNT - 1;
    const u32 fontId = FONT_NORMAL;
    enum StringID battleStringId = 0;
    u8 *battleString = Alloc(BATTLE_STRING_BUFFER_SIZE);

    s32 sixDigitNines = 999999;                                 // 36 pixels.
    u8 nickname[POKEMON_NAME_LENGTH + 1] = _("MMMMMMMMMMMM");   // 72 pixels.
    enum Move longMoveID = MOVE_NATURES_MADNESS;                // 89 pixels.
    enum Ability longAbilityID = ABILITY_SUPERSWEET_SYRUP;      // 91 pixels.
    enum Stat longStatName = STAT_EVASION;                      // 40 pixels.
    enum Type longTypeName = TYPE_ELECTRIC;                     // 43 pixels.
    enum Species longSpeciesName = SPECIES_SANDY_SHOCKS;        // 47 pixels.
    enum Item longItemName = ITEM_UNREMARKABLE_TEACUP;          // 73 pixels.
    u8 boxName[9] = _("MMMMMMMM");                              // 54 pixels.

    // Set longest default player name, JOHNNY
    NewGameBirchSpeech_SetDefaultPlayerName(10);  // JOHNNY

    RUN_OVERWORLD_SCRIPT(
        givemon SPECIES_WOBBUFFET, 100;
        createmon 1, 0, SPECIES_WOBBUFFET, 100;
    );
    SetMonData(&gParties[B_TRAINER_PLAYER][0], MON_DATA_NICKNAME, nickname);
    SetMonData(&gParties[B_TRAINER_OPPONENT_A][0], MON_DATA_NICKNAME, nickname);

    for (i = start; i <= end; i++)
    {
        PARAMETRIZE_LABEL("%S", gBattleStringsTable[i]) { battleStringId = i; }
    }

    // Clear buffers
    PREPARE_STRING_BUFFER(gBattleTextBuff1, STRINGID_EMPTYSTRING3);
    PREPARE_STRING_BUFFER(gBattleTextBuff2, STRINGID_EMPTYSTRING3);
    PREPARE_STRING_BUFFER(gBattleTextBuff3, STRINGID_EMPTYSTRING3);
    *gStringVar1 = EOS;
    *gStringVar2 = EOS;
    *gStringVar3 = EOS;

    // Set positions
    gBattlerPositions[0] = B_POSITION_PLAYER_LEFT;
    gBattlerPositions[1] = B_POSITION_OPPONENT_LEFT;
    gBattlerPositions[2] = B_POSITION_PLAYER_RIGHT;
    gBattlerPositions[3] = B_POSITION_OPPONENT_RIGHT;

    // Set abilities
    gLastUsedAbility = longAbilityID;
    for (j = 0; j < MAX_BATTLERS_COUNT; j++)
        sBattlerAbilities[j] = longAbilityID;

    // Set Trainers
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_RED_TEST;
    TRAINER_BATTLE_PARAM.opponentB = TRAINER_RED_TEST;

    // Set battler to 1, so "The opposing " is prefixed when refering to battlers.
    gBattleTypeFlags |= BATTLE_TYPE_TRAINER;
    gBattlerAttacker = gBattlerTarget = gBattleScripting.battler = gEffectBattler = 1;

    // Set moves
    gCurrentMove = longMoveID;
    gBattleMsgDataPtr = AllocZeroed(sizeof(struct BattleMsgData));
    gBattleMsgDataPtr->currentMove = longMoveID;

    // Set Items
    gLastUsedItem = longItemName;

    // Buffer specific strings for each Battle String.
    // In cases where a buffer is used with multiple contexts, the widest string is used.
    // Eg. STRINGID_CANACTFASTERTHANKSTO is used for both with abilities and items,
    // so ability is chosen because it's longer.
    switch (battleStringId)
    {
    // Testing Trainer messages is out of the current scope for this test.
    case STRINGID_TRAINER1LOSETEXT:
    case STRINGID_TRAINER2LOSETEXT:
    case STRINGID_TRAINER1WINTEXT:
    case STRINGID_TRAINER2WINTEXT:
        break;
    // Buffer Nickname with prefix to B_BUFF1, " a boosted" to B_BUFF2, "999999" to B_BUFF3
    case STRINGID_PKMNGAINEDEXP:
        PREPARE_MON_NICK_WITH_PREFIX_BUFFER(gBattleTextBuff1, 0, 0);
        PREPARE_STRING_BUFFER(gBattleTextBuff2, STRINGID_ABOOSTED); // 'gained a boosted'
        PREPARE_WORD_NUMBER_BUFFER(gBattleTextBuff3, 6, sixDigitNines);
        break;
    // Buffer Nickname with prefix to B_BUFF1, "100" to B_BUFF2
    case STRINGID_PKMNGREWTOLV:
        PREPARE_MON_NICK_WITH_PREFIX_BUFFER(gBattleTextBuff1, 0, 0);
        PREPARE_BYTE_NUMBER_BUFFER(gBattleTextBuff2, 3, 100);
        break;
    // Buffer Nickname with prefix to B_BUFF1, move name to B_BUFF2
    case STRINGID_PKMNLEARNEDMOVE:
    case STRINGID_TRYTOLEARNMOVE1:
    case STRINGID_TRYTOLEARNMOVE2:
    case STRINGID_TRYTOLEARNMOVE3:
    case STRINGID_PKMNFORGOTMOVE:
    case STRINGID_STOPLEARNINGMOVE:
    case STRINGID_DIDNOTLEARNMOVE:
        PREPARE_MON_NICK_WITH_PREFIX_BUFFER(gBattleTextBuff1, 0, 0);
        PREPARE_MOVE_BUFFER(gBattleTextBuff2, longMoveID);
        break;
    // Buffer Move name to B_BUFF1
    case STRINGID_PKMNLEARNEDMOVE2:
    case STRINGID_PKMNHURTBY:
    case STRINGID_PKMNFREEDFROM:
    case STRINGID_PKMNMOVEWASDISABLED:
    case STRINGID_PKMNSKETCHEDMOVE:
    case STRINGID_PKMNGOTFREE:
    case STRINGID_PKMNLOSTPPGRUDGE:
    case STRINGID_PKMNSITEMRESTOREDPP:
    case STRINGID_PKMNSXWOREOFF:
    case STRINGID_BUFFERENDS:
    case STRINGID_FOREWARNACTIVATES:
    case STRINGID_CURSEDBODYDISABLED:
    case STRINGID_CURRENTMOVECANTSELECT:
    case STRINGID_TARGETISHURTBYSALTCURE:
        PREPARE_MOVE_BUFFER(gBattleTextBuff1, longMoveID);
        break;
    // Buffer "999999" to B_BUFF1
    case STRINGID_PLAYERGOTMONEY:
    case STRINGID_PLAYERWHITEOUT2_TRAINER:
    case STRINGID_PLAYERPICKEDUPMONEY:
    case STRINGID_PLAYERWHITEOUT2_WILD:
        PREPARE_WORD_NUMBER_BUFFER(gBattleTextBuff1, 6, sixDigitNines);
        break;
    // Buffer "99" to B_BUFF1
    case STRINGID_HITXTIMES:
    case STRINGID_MAGNITUDESTRENGTH:
        PREPARE_WORD_NUMBER_BUFFER(gBattleTextBuff1, 2, 99);
        break;
    // Buffer "9" to B_BUFF1
    case STRINGID_PKMNSTOCKPILED:
    case STRINGID_PKMNPERISHCOUNTFELL:
        PREPARE_WORD_NUMBER_BUFFER(gBattleTextBuff1, 1, 9);
        break;
    // Buffer Ability name to B_BUFF1
    case STRINGID_PKMNMADESLEEP:
    case STRINGID_PKMNPOISONEDBY:
    case STRINGID_PKMNBURNEDBY:
    case STRINGID_PKMNFROZENBY:
    case STRINGID_PKMNWASPARALYZEDBY:
    case STRINGID_CANACTFASTERTHANKSTO:
        PREPARE_ABILITY_BUFFER(gBattleTextBuff1, longAbilityID);
        break;
    // Buffer Stat name to B_BUFF1
    case STRINGID_STATSWONTINCREASE:
    case STRINGID_STATSWONTDECREASE:
    case STRINGID_TARGETABILITYSTATRAISE:
    case STRINGID_ATTACKERABILITYSTATRAISE:
    case STRINGID_TARGETABILITYSTATLOWER:
    case STRINGID_SCRIPTINGABILITYSTATRAISE:
    case STRINGID_STATWASHEIGHTENED:
        StringCopy(gBattleTextBuff1, gStatNamesTable[longStatName]);
        break;
    // Buffer Type name to B_BUFF1
    case STRINGID_PKMNCHANGEDTYPE:
    case STRINGID_PKMNCHANGEDTYPEWITH:
    case STRINGID_TARGETCHANGEDTYPE:
    case STRINGID_PROTEANTYPECHANGE:
    case STRINGID_THIRDTYPEADDED:
    case STRINGID_ATTACKERLOSTITSTYPE:
        PREPARE_TYPE_BUFFER(gBattleTextBuff1, longTypeName);
        break;
    // Buffer Species name to B_BUFF1
    case STRINGID_PKMNTRANSFORMEDINTO:
    case STRINGID_WILDPKMNFLED:
    case STRINGID_MEGAEVOEVOLVED:
    case STRINGID_PKMNREVIVEDREADYTOFIGHT:
    case STRINGID_ITEMRESTOREDSPECIESHEALTH: // Should probably use nickname instead?
    case STRINGID_ITEMCUREDSPECIESSTATUS: // Should probably use nickname instead?
    case STRINGID_ITEMRESTOREDSPECIESPP: // Should probably use nickname instead?
        PREPARE_SPECIES_BUFFER(gBattleTextBuff1, longSpeciesName)
        break;
    // Buffer nickname with prefix to B_BUFF1
    case STRINGID_PKMNATTACK:
    case STRINGID_PKMNWISHCAMETRUE:
        PREPARE_MON_NICK_WITH_PREFIX_BUFFER(gBattleTextBuff1, 1, 0);
        break;
    // Buffer nickname with prefix in lower case to B_BUFF1
    case STRINGID_USEDINSTRUCTEDMOVE:
        PREPARE_MON_NICK_WITH_PREFIX_LOWER_BUFFER(gBattleTextBuff1, 1, 0);
        break;
    // Buffer nickname to B_BUFF2
    case STRINGID_ENEMYABOUTTOSWITCHPKMN:
        PREPARE_MON_NICK_BUFFER(gBattleTextBuff2, 1, 0);
        break;
    // Buffer Item name to B_BUFF1
    case STRINGID_PKMNHURTSWITH:
    case STRINGID_PKMNCURIOUSABOUTX:
    case STRINGID_PKMNENTHRALLEDBYX:
    case STRINGID_PKMNIGNOREDX:
    case STRINGID_PKMNOBTAINEDX:
    case STRINGID_ABOUTTOUSEPOLTERGEIST:
        PREPARE_ITEM_BUFFER(gBattleTextBuff1, longItemName);
        break;
    // Buffer Item name to B_BUFF2
    case STRINGID_PKMNOBTAINEDX2:
        PREPARE_ITEM_BUFFER(gBattleTextBuff2, longItemName);
        break;
    // Buffer Item name to B_BUFF1 and B_BUFF2
    case STRINGID_PKMNOBTAINEDXYOBTAINEDZ:
        PREPARE_ITEM_BUFFER(gBattleTextBuff1, longItemName);
        PREPARE_ITEM_BUFFER(gBattleTextBuff2, longItemName);
        break;
    // Buffer nickname with prefix to B_BUFF1, Ability name to B_BUFF2
    case STRINGID_PKMNTRACED:
        PREPARE_MON_NICK_WITH_PREFIX_LOWER_BUFFER(gBattleTextBuff1, 1, 0);
        PREPARE_ABILITY_BUFFER(gBattleTextBuff2, longAbilityID);
        break;
    // Buffer Stat name to B_BUFF1, "drastically rose" to B_BUFF2
    case STRINGID_STATROSE:
    case STRINGID_USINGITEMSTATOFPKMNROSE:
        StringCopy(gBattleTextBuff1, gStatNamesTable[longStatName]);
        StringCopy(gBattleTextBuff2, gText_drastically);
        StringAppend(gBattleTextBuff2, gText_StatRose);
        break;
    // Buffer Stat name to B_BUFF1, "severely fell" to B_BUFF2
    case STRINGID_STATFELL:
        StringCopy(gBattleTextBuff1, gStatNamesTable[longStatName]);
        StringCopy(gBattleTextBuff2, gText_severely);
        StringAppend(gBattleTextBuff2, gText_StatFell);
        break;
    // Buffer Status name to B_BUFF2
    case STRINGID_PKMNSITEMCUREDPROBLEM:
    case STRINGID_PKMNSXCUREDITSYPROBLEM:
        StringCopy(gBattleTextBuff1, gText_Confusion);
        break;
    // Buffer Box name to STR_VAR_1 and STR_VAR_3, Nickname to STR_VAR_2
    case STRINGID_PKMNTRANSFERREDSOMEONESPC:
    case STRINGID_PKMNTRANSFERREDLANETTESPC:
    case STRINGID_PKMNBOXSOMEONESPCFULL:
    case STRINGID_PKMNBOXLANETTESPCFULL:
        StringCopy(gStringVar1, boxName);
        StringCopy(gStringVar2, nickname);
        StringCopy(gStringVar3, boxName);
        break;
    default:
        break;
    }
    EXPECT(gBattleStringsTable[battleStringId] != NULL);
    BattleStringExpandPlaceholders(gBattleStringsTable[battleStringId], battleString, BATTLE_STRING_BUFFER_SIZE);
    DebugPrintf("Battle String ID %d: %S", battleStringId, battleString);
    for (j = 1;; j++)
    {
        strWidth = GetStringLineWidth(fontId, battleString, 0, j, BATTLE_STRING_BUFFER_SIZE);
        if (strWidth == 0)
            break;
        EXPECT_LE(strWidth - 1, BATTLE_MSG_MAX_WIDTH); // -1 because there's a pixel-wide space that doesn't visually look like it's out of frame when using FONT_NORMAL.
    }
    Free(gBattleMsgDataPtr);
    Free(battleString);
}
//*/

TEST("Battle message: trainer name placeholders use independent scratch text")
{
    u32 flags = gBattleTypeFlags;
    u16 trainerA = TRAINER_BATTLE_PARAM.opponentA;
    u16 trainerB = TRAINER_BATTLE_PARAM.opponentB;
    gBattleTypeFlags = BATTLE_TYPE_TRAINER | BATTLE_TYPE_DOUBLE | BATTLE_TYPE_TWO_OPPONENTS;
    TRAINER_BATTLE_PARAM.opponentA = TRAINER_ROXANNE_1;
    TRAINER_BATTLE_PARAM.opponentB = TRAINER_BRAWLY_1;
    for (u32 i = 0; i < MAX_BATTLERS_COUNT; i++) gBattlerPositions[i] = i;
    u8 first[128], second[128], expected[256], actual[256];
    BattleStringExpandPlaceholders(COMPOUND_STRING("{B_TRAINER1_NAME_WITH_CLASS}"), first, sizeof(first));
    BattleStringExpandPlaceholders(COMPOUND_STRING("{B_TRAINER2_NAME_WITH_CLASS}"), second, sizeof(second));
    StringCopy(expected, first);
    StringAppend(expected, COMPOUND_STRING("\n"));
    StringAppend(expected, second);
    BattleStringExpandPlaceholders(COMPOUND_STRING("{B_TRAINER1_NAME_WITH_CLASS}\n{B_TRAINER2_NAME_WITH_CLASS}"), actual, sizeof(actual));
    EXPECT_EQ(StringCompare(actual, expected), 0);
    gBattleTypeFlags = flags;
    TRAINER_BATTLE_PARAM.opponentA = trainerA;
    TRAINER_BATTLE_PARAM.opponentB = trainerB;
}

TEST("Battle message: destination capacity includes terminator and rejects partial controls")
{
    static const u8 *const messages[] = {
        COMPOUND_STRING("ABCDEFGHIJ"),
        COMPOUND_STRING("{COLOR RED}ABCDEFGHIJ"),
        COMPOUND_STRING("{B_BUFF1}ABCDEFGHIJ"),
    };
    StringCopy(gBattleTextBuff1, COMPOUND_STRING("PREFIX"));
    for (u32 message = 0; message < ARRAY_COUNT(messages); message++)
    {
        u8 full[128];
        u32 length = BattleStringExpandPlaceholders(messages[message], full, sizeof(full));
        for (u32 capacity = 0; capacity <= length + 1; capacity++)
        {
            u8 output[128];
            memset(output, 0xA5, sizeof(output));
            u32 result = BattleStringExpandPlaceholders(messages[message], output + 1, capacity);
            EXPECT_EQ(output[0], 0xA5);
            EXPECT_EQ(output[capacity + 1], 0xA5);
            if (capacity == 0)
                EXPECT_EQ(result, 0);
            else if (capacity < length)
            {
                EXPECT_EQ(result, 1);
                EXPECT_EQ(output[1], EOS);
            }
            else
            {
                EXPECT_EQ(result, length);
                EXPECT_EQ(StringCompare(output + 1, full), 0);
            }
        }
    }
}

extern const u8 *Test_GetBattleStatusString(u8 *src);

TEST("Battle message: legacy status tokens match bytewise from unaligned input")
{
    ALIGNED(4) u8 input[16];
    for (u32 i = 0; i < ARRAY_COUNT(gStatusConditionStringsTable); i++)
    {
        memset(input, 0xA5, sizeof(input));
        memcpy(input + 1, gStatusConditionStringsTable[i][0], 8);
        EXPECT_EQ(Test_GetBattleStatusString(input + 1), gStatusConditionStringsTable[i][1]);
        StringCopy(gBattleTextBuff1, input + 1);
        u8 output[64];
        BattleStringExpandPlaceholders(COMPOUND_STRING("{B_BUFF1}"), output, sizeof(output));
        EXPECT_EQ(StringCompare(output, gStatusConditionStringsTable[i][1]), 0);
    }
    input[1] = EOS;
    EXPECT_EQ(Test_GetBattleStatusString(input + 1), NULL);
    StringCopy(input + 1, COMPOUND_STRING("UNKNOWN"));
    EXPECT_EQ(Test_GetBattleStatusString(input + 1), NULL);
}

TEST("Automatic wrapping: overwide words do not create uninitialized trailing lines")
{
    u8 text[32];
    StringCopy(text, COMPOUND_STRING("MMMMMMMMMMMM"));
    BreakStringAutomatic(text, 1, 2, FONT_NORMAL, SHOW_SCROLL_PROMPT);
    EXPECT_EQ(StringCompare(text, COMPOUND_STRING("MMMMMMMMMMMM")), 0);
    memset(text, 0xA5, sizeof(text));
    StringCopy(text, COMPOUND_STRING("MMMM NNNN"));
    BreakStringAutomatic(text, 1, 2, FONT_NORMAL, SHOW_SCROLL_PROMPT);
    EXPECT_EQ(StringCompare(text, COMPOUND_STRING("MMMM\nNNNN")), 0);
    EXPECT_EQ(text[10], 0xA5);
    StringCopy(text, COMPOUND_STRING("AA BB CC"));
    BreakStringAutomatic(text, 1, 2, FONT_NORMAL, SHOW_SCROLL_PROMPT);
    EXPECT_EQ(StringCompare(text, COMPOUND_STRING("AA\nBB\lCC")), 0);
}

TEST("Automatic wrapping: leading repeated and trailing spaces preserve word bytes")
{
    u8 text[32];
    memset(text, 0xA5, sizeof(text));
    StringCopy(text, COMPOUND_STRING("  AA   BB  "));
    BreakStringAutomatic(text, 1, 2, FONT_NORMAL, SHOW_SCROLL_PROMPT);
    EXPECT_EQ(StringCompare(text, COMPOUND_STRING("  AA  \nBB  ")), 0);
    EXPECT_EQ(text[12], 0xA5);
    StringCopy(text, COMPOUND_STRING("   "));
    BreakStringAutomatic(text, 1, 2, FONT_NORMAL, SHOW_SCROLL_PROMPT);
    EXPECT_EQ(StringCompare(text, COMPOUND_STRING("   ")), 0);
    StringCopy(text, COMPOUND_STRING(" AA  "));
    BreakStringAutomatic(text, 1, 2, FONT_NORMAL, SHOW_SCROLL_PROMPT);
    EXPECT_EQ(StringCompare(text, COMPOUND_STRING(" AA  ")), 0);
}

TEST("Automatic wrapping: long word metrics do not wrap at 255 pixels or bytes")
{
    static const u32 lengths[] = {60, 300};
    for (u32 sample = 0; sample < ARRAY_COUNT(lengths); sample++)
    {
        u8 text[320];
        memset(text, 0xA5, sizeof(text));
        u32 length = lengths[sample];
        for (u32 i = 0; i < length; i++) text[i] = CHAR_M;
        text[length] = CHAR_SPACE;
        text[length + 1] = CHAR_B;
        text[length + 2] = EOS;
        BreakStringAutomatic(text, 200, 2, FONT_NORMAL, SHOW_SCROLL_PROMPT);
        for (u32 i = 0; i < length; i++) EXPECT_EQ(text[i], CHAR_M);
        EXPECT_EQ(text[length], CHAR_NEWLINE);
        EXPECT_EQ(text[length + 1], CHAR_B);
        EXPECT_EQ(text[length + 2], EOS);
        EXPECT_EQ(text[length + 3], 0xA5);
    }
}

TEST("Automatic wrapping: text control parameters cannot become spaces or line breaks")
{
    u8 text[] = {EXT_CTRL_CODE_BEGIN, EXT_CTRL_CODE_PLAY_SE, CHAR_NEWLINE, CHAR_PROMPT_CLEAR,
        EXT_CTRL_CODE_BEGIN, EXT_CTRL_CODE_COLOR, CHAR_SPACE, CHAR_A, CHAR_A,
        CHAR_SPACE, CHAR_B, CHAR_B, EOS};
    u8 original[sizeof(text)];
    memcpy(original, text, sizeof(text));
    EXPECT_EQ(CountLineBreaks(text), 0);
    EXPECT(!StringHasManualBreaks(text));
    StripLineBreaks(text);
    EXPECT_EQ(memcmp(text, original, sizeof(text)), 0);
    BreakStringAutomatic(text, 1, 2, FONT_NORMAL, SHOW_SCROLL_PROMPT);
    EXPECT_EQ(memcmp(text, original, 9), 0);
    EXPECT_EQ(text[9], CHAR_NEWLINE);
    EXPECT_EQ(text[10], CHAR_B);
    EXPECT_EQ(text[11], CHAR_B);
    EXPECT_EQ(text[12], EOS);
}

TEST("Battle message: encoded buffers reject unknown tokens invalid indices and missing terminators")
{
    u8 src[TEXT_BUFF_ARRAY_COUNT];
    u8 dst[256];
    static const u8 invalid[][8] = {
        {B_BUFF_PLACEHOLDER_BEGIN, 0xFE, B_BUFF_EOS},
        {B_BUFF_PLACEHOLDER_BEGIN, B_BUFF_NUMBER, 3, 3, 1, 2, 3, B_BUFF_EOS},
        {B_BUFF_PLACEHOLDER_BEGIN, B_BUFF_TYPE, 255, B_BUFF_EOS},
        {B_BUFF_PLACEHOLDER_BEGIN, B_BUFF_ABILITY, 255, 255, B_BUFF_EOS},
        {B_BUFF_PLACEHOLDER_BEGIN, B_BUFF_MON_NICK, MAX_BATTLERS_COUNT, 0, B_BUFF_EOS},
        {B_BUFF_PLACEHOLDER_BEGIN, B_BUFF_MON_NICK, 0, PARTY_SIZE, B_BUFF_EOS},
    };
    for (u32 i = 0; i < ARRAY_COUNT(invalid); i++)
    {
        memset(src, B_BUFF_EOS, sizeof(src));
        memcpy(src, invalid[i], sizeof(invalid[i]));
        dst[0] = CHAR_A;
        ExpandBattleTextBuffPlaceholders(src, dst, sizeof(dst));
        EXPECT_EQ(dst[0], EOS);
    }
    memset(src, 0, sizeof(src));
    src[0] = B_BUFF_PLACEHOLDER_BEGIN;
    for (u32 i = 1; i + 2 < sizeof(src); i += 3)
        src[i] = B_BUFF_MOVE;
    ExpandBattleTextBuffPlaceholders(src, dst, sizeof(dst));
    EXPECT_EQ(dst[0], EOS);
    PREPARE_MOVE_BUFFER(src, MOVE_SURF);
    ExpandBattleTextBuffPlaceholders(src, dst, sizeof(dst));
    EXPECT_EQ(StringCompare(dst, GetMoveName(MOVE_SURF)), 0);
}

TEST("Battle message: encoded concatenation respects each destination capacity")
{
    u8 src[TEXT_BUFF_ARRAY_COUNT] = {
        B_BUFF_PLACEHOLDER_BEGIN, B_BUFF_MOVE, MOVE_SURF & 255, MOVE_SURF >> 8,
        B_BUFF_MOVE, MOVE_PROTECT & 255, MOVE_PROTECT >> 8, B_BUFF_EOS,
    };
    u8 full[128];
    ExpandBattleTextBuffPlaceholders(src, full, sizeof(full));
    u32 length = StringLength(full) + 1;
    EXPECT_GT(length, 1);
    for (u32 capacity = 0; capacity <= length; capacity++)
    {
        u8 output[128];
        memset(output, 0xA5, sizeof(output));
        ExpandBattleTextBuffPlaceholders(src, output + 1, capacity);
        EXPECT_EQ(output[0], 0xA5);
        EXPECT_EQ(output[capacity + 1], 0xA5);
        if (capacity && capacity < length) EXPECT_EQ(output[1], EOS);
        if (capacity == length) EXPECT_EQ(StringCompare(output + 1, full), 0);
    }
}
