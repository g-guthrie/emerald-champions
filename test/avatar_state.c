#include "global.h"
#include "field_player_avatar.h"
#include "test/test.h"

TEST("Avatar state: changing movement mode preserves control flags and replaces the old mode")
{
    struct PlayerAvatar saved = gPlayerAvatar;
    static const u8 modes[] = {PLAYER_AVATAR_FLAG_ON_FOOT, PLAYER_AVATAR_FLAG_MACH_BIKE,
        PLAYER_AVATAR_FLAG_ACRO_BIKE, PLAYER_AVATAR_FLAG_SURFING, PLAYER_AVATAR_FLAG_UNDERWATER};
    u8 control = PLAYER_AVATAR_FLAG_DASH | PLAYER_AVATAR_FLAG_FORCED_MOVE | PLAYER_AVATAR_FLAG_CONTROLLABLE;
    for (u32 old = 0; old < ARRAY_COUNT(modes); old++)
    for (u32 next = 0; next < ARRAY_COUNT(modes); next++)
    {
        gPlayerAvatar.flags = modes[old] | control;
        SetPlayerAvatarStateMask(modes[next]);
        EXPECT_EQ(gPlayerAvatar.flags, modes[next] | control);
    }
    gPlayerAvatar = saved;
}

TEST("Avatar graphics: each movement presentation retains player gender")
{
    for (u32 state = PLAYER_AVATAR_STATE_NORMAL; state <= PLAYER_AVATAR_STATE_VSSEEKER; state++)
    for (enum Gender gender = MALE; gender <= FEMALE; gender++)
        EXPECT_EQ(GetPlayerAvatarGenderByGraphicsId(GetPlayerAvatarGraphicsIdByStateIdAndGender(state, gender)), gender);
}

TEST("Avatar coordinates: facing queries offset a copy without changing the player's position")
{
    struct PlayerAvatar saved = gPlayerAvatar;
    struct ObjectEvent object = gObjectEvents[0];
    gPlayerAvatar.objectEventId = 0;
    gObjectEvents[0].currentCoords.x = 20;
    gObjectEvents[0].currentCoords.y = 30;
    static const s8 offsets[][2] = {{0, 1}, {0, -1}, {-1, 0}, {1, 0}};
    for (enum Direction dir = DIR_SOUTH; dir <= DIR_EAST; dir++)
    {
        gObjectEvents[0].facingDirection = dir;
        s16 x, y;
        GetXYCoordsOneStepInFrontOfPlayer(&x, &y);
        EXPECT_EQ(x, 20 + offsets[dir - DIR_SOUTH][0]);
        EXPECT_EQ(y, 30 + offsets[dir - DIR_SOUTH][1]);
        PlayerGetDestCoords(&x, &y);
        EXPECT_EQ(x, 20);
        EXPECT_EQ(y, 30);
    }
    gObjectEvents[0] = object;
    gPlayerAvatar = saved;
}
