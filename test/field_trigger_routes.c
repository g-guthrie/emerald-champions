#include "global.h"
#include "bike.h"
#include "field_player_avatar.h"
#include "event_data.h"
#include "field_control_avatar.h"
#include "fieldmap.h"
#include "overworld.h"
#include "test/test.h"
#include "constants/maps.h"

extern const u8 RustboroCity_EventScript_HelpGetGoodsTrigger0[];
extern const u8 RustboroCity_EventScript_ReturnGoodsTrigger0[];
extern const u8 LittlerootTown_EventScript_NeedPokemonTriggerRight[];
extern const u8 LittlerootTown_EventScript_GoSaveBirchTrigger[];
extern const u8 NewMauville_Inside_EventScript_Rotom[];
extern const u8 NewMauville_Inside_EventScript_Rotom2[];
extern const u8 EverGrandeCity_PokemonLeague_1F_EventScript_CheckLeagueDoor[];

TEST("League entrance: both door tiles check the player's restricted party")
{
    struct MapHeader saved = gMapHeader;
    gMapHeader = *Overworld_GetMapHeaderByGroupAndId(
        MAP_GROUP(MAP_EVER_GRANDE_CITY_POKEMON_LEAGUE_1F),
        MAP_NUM(MAP_EVER_GRANDE_CITY_POKEMON_LEAGUE_1F));
    for (s16 x = 9; x <= 10; x++)
    {
        struct MapPosition pos = {x + MAP_OFFSET, 2 + MAP_OFFSET, 3};
        EXPECT_EQ(GetCoordEventScriptAtMapPosition(&pos),
            EverGrandeCity_PokemonLeague_1F_EventScript_CheckLeagueDoor);
    }
    gMapHeader = saved;
}

TEST("Field triggers: shared tiles select the matching story state instead of the first entry")
{
    static const struct {
        u16 map, var, firstValue, secondValue;
        s16 x, y;
        u8 elevation;
        const u8 *first, *second;
    } cases[] = {
        {MAP_RUSTBORO_CITY, VAR_RUSTBORO_CITY_STATE, 3, 5, 30, 9, 3, RustboroCity_EventScript_HelpGetGoodsTrigger0, RustboroCity_EventScript_ReturnGoodsTrigger0},
        {MAP_LITTLEROOT_TOWN, VAR_LITTLEROOT_TOWN_STATE, 0, 1, 11, 1, 3, LittlerootTown_EventScript_NeedPokemonTriggerRight, LittlerootTown_EventScript_GoSaveBirchTrigger},
        {MAP_NEW_MAUVILLE_INSIDE, VAR_NEW_MAUVILLE_STATE, 2, 4, 32, 6, 0, NewMauville_Inside_EventScript_Rotom, NewMauville_Inside_EventScript_Rotom2},
    };
    struct MapHeader saved = gMapHeader;
    for (u32 i = 0; i < ARRAY_COUNT(cases); i++)
    {
        gMapHeader = *Overworld_GetMapHeaderByGroupAndId(cases[i].map >> 8, cases[i].map & 0xFF);
        struct MapPosition pos = {cases[i].x + MAP_OFFSET, cases[i].y + MAP_OFFSET, cases[i].elevation};
        u16 before = VarGet(cases[i].var);
        VarSet(cases[i].var, cases[i].firstValue);
        EXPECT_EQ(GetCoordEventScriptAtMapPosition(&pos), cases[i].first);
        VarSet(cases[i].var, cases[i].secondValue);
        EXPECT_EQ(GetCoordEventScriptAtMapPosition(&pos), cases[i].second);
        VarSet(cases[i].var, 0xFFFF);
        EXPECT_EQ(GetCoordEventScriptAtMapPosition(&pos), NULL);
        VarSet(cases[i].var, before);
    }
    gMapHeader = saved;
}

TEST("Field input: every frame starts with cleared actions and direction")
{
    struct FieldInput input, empty = {0};
    memset(&input, 0xFF, sizeof(input));
    FieldClearPlayerInput(&input);
    EXPECT_EQ(memcmp(&input, &empty, sizeof(input)), 0);
}

TEST("Bicycle input: idle interactions work while full-speed riding stays locked")
{
    struct PlayerAvatar savedAvatar = gPlayerAvatar;
    struct ObjectEvent savedObject = gObjectEvents[0];
    struct MapHeader savedHeader = gMapHeader;
    struct BackupMapLayout savedLayout = gBackupMapLayout;
    u32 savedButtonMode = gSaveBlock2Ptr->optionsButtonMode;
    u16 floor = 0; // General metatile 0 is not forced movement.
    struct FieldInput input;
    static const struct { u8 running, transition; bool8 allowed, direction; } cases[] = {
        {NOT_MOVING, T_NOT_MOVING, TRUE, TRUE},
        {NOT_MOVING, T_TILE_CENTER, TRUE, TRUE},
        {MOVING, T_TILE_CENTER, FALSE, TRUE},
        {MOVING, T_TILE_TRANSITION, FALSE, FALSE},
    };

    gMapHeader = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(MAP_OLDALE_TOWN), MAP_NUM(MAP_OLDALE_TOWN));
    gBackupMapLayout = (struct BackupMapLayout){.width = 1, .height = 1, .map = &floor};
    gPlayerAvatar = (struct PlayerAvatar){.flags = PLAYER_AVATAR_FLAG_ACRO_BIKE, .objectEventId = 0};
    gObjectEvents[0].currentCoords = (struct Coords16){0, 0};
    gSaveBlock2Ptr->optionsButtonMode = OPTIONS_BUTTON_MODE_NORMAL;
    for (u32 i = 0; i < ARRAY_COUNT(cases); i++)
    {
        gPlayerAvatar.runningState = cases[i].running;
        gPlayerAvatar.tileTransitionState = cases[i].transition;
        EXPECT_EQ(GetPlayerSpeed(), PLAYER_SPEED_FASTEST); // Slope/cracked-floor capability survives.
        FieldClearPlayerInput(&input);
        FieldGetPlayerInput(&input, A_BUTTON | START_BUTTON | SELECT_BUTTON, DPAD_UP);
        EXPECT_EQ((u32)input.pressedAButton, cases[i].allowed);
        EXPECT_EQ((u32)input.pressedStartButton, cases[i].allowed);
        EXPECT_EQ((u32)input.pressedSelectButton, cases[i].allowed);
        // Direction events are emitted at tile centers, not mid-transition.
        EXPECT_EQ((u32)input.heldDirection, cases[i].direction);
        EXPECT_EQ(input.dpadDirection, DIR_NORTH);
    }
    gSaveBlock2Ptr->optionsButtonMode = savedButtonMode;
    gBackupMapLayout = savedLayout;
    gMapHeader = savedHeader;
    gObjectEvents[0] = savedObject;
    gPlayerAvatar = savedAvatar;
}
