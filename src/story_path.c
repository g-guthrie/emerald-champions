#include "global.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "fieldmap.h"
#include "malloc.h"
#include "script.h"
#include "script_movement.h"
#include "story.h"
#include "constants/event_object_movement.h"

// Computed choreography: a scene names who moves and where to; the path is found
// when the scene runs, on the live map, with the game's own collision check
// (GetCollisionAtCoords: tile collision, metatile behavior, elevation, other
// objects). No hand-written step lists, so a map edit can never strand a scene.

enum StoryWalkMode
{
    STORY_WALK_TO_PLAYER, // stop on a tile next to the player, facing them
    STORY_WALK_TO_TILE,   // stop on (x, y)
    STORY_WALK_HOME,      // stop on the object's map position
};

#define STORY_PATH_MAX_STEPS 62
#define STORY_PATH_SLOTS 4

// A movement script must outlive the command that starts it: one buffer per
// object moving at once.
static EWRAM_DATA u8 sStoryPathMoves[STORY_PATH_SLOTS][STORY_PATH_MAX_STEPS + 2] = {0};
static EWRAM_DATA u8 sStoryPathSlot = 0;

// Set when a scene asks for a walk that has no path. The chain test fails on it.
EWRAM_DATA u16 gStoryPathFailures = 0;

static const s8 sDirDx[] = {[DIR_SOUTH] = 0, [DIR_NORTH] = 0, [DIR_WEST] = -1, [DIR_EAST] = 1};
static const s8 sDirDy[] = {[DIR_SOUTH] = 1, [DIR_NORTH] = -1, [DIR_WEST] = 0, [DIR_EAST] = 0};

static bool32 CanStep(struct ObjectEvent *obj, s16 fromX, s16 fromY, enum Direction dir)
{
    struct Coords16 saved = obj->currentCoords;
    enum Collision collision;

    // The collision check reads the mover's current tile (stairs); put it on the
    // tile the step starts from, then put it back.
    obj->currentCoords.x = fromX;
    obj->currentCoords.y = fromY;
    collision = GetCollisionAtCoords(obj, fromX + sDirDx[dir], fromY + sDirDy[dir], dir);
    obj->currentCoords = saved;
    obj->directionOverwrite = DIR_NONE;
    // A scripted walk is not bound by the wander range of the object's map entry.
    return collision == COLLISION_NONE || collision == COLLISION_OUTSIDE_RANGE;
}

static bool32 IsGoal(enum StoryWalkMode mode, s16 x, s16 y, s16 goalX, s16 goalY)
{
    if (mode == STORY_WALK_TO_PLAYER)
        return (x == goalX && (y == goalY - 1 || y == goalY + 1)) || (y == goalY && (x == goalX - 1 || x == goalX + 1));
    return x == goalX && y == goalY;
}

static enum Direction FaceTowards(s16 x, s16 y, s16 targetX, s16 targetY)
{
    if (targetX < x) return DIR_WEST;
    if (targetX > x) return DIR_EAST;
    if (targetY < y) return DIR_NORTH;
    return DIR_SOUTH;
}

// Breadth-first search over the loaded map. Returns the number of moves written.
static s32 FindPath(struct ObjectEvent *obj, enum StoryWalkMode mode, s16 goalX, s16 goalY, u8 fast, u8 *out)
{
    s32 width = gBackupMapLayout.width, height = gBackupMapLayout.height;
    s32 count = width * height, head = 0, tail = 0, found = -1, start, i, n;
    u16 *queue;
    u8 *from; // direction taken into each tile, +1 (0 = unvisited)
    u8 dirs[STORY_PATH_MAX_STEPS];

    if (count <= 0)
        return -1;
    queue = AllocZeroed(count * sizeof(u16));
    from = AllocZeroed(count);
    if (queue == NULL || from == NULL)
    {
        Free(queue);
        Free(from);
        return -1;
    }
    start = obj->currentCoords.y * width + obj->currentCoords.x;
    from[start] = 0xFF;
    queue[tail++] = start;
    while (head < tail)
    {
        s32 cur = queue[head++];
        s16 x = cur % width, y = cur / width;
        enum Direction dir;

        if (IsGoal(mode, x, y, goalX, goalY))
        {
            found = cur;
            break;
        }
        for (dir = DIR_SOUTH; dir <= DIR_EAST; dir++)
        {
            s16 nx = x + sDirDx[dir], ny = y + sDirDy[dir];
            s32 next = ny * width + nx;

            if (nx < 0 || ny < 0 || nx >= width || ny >= height || from[next])
                continue;
            if (!CanStep(obj, x, y, dir))
                continue;
            from[next] = dir;
            queue[tail++] = next;
        }
    }
    n = 0;
    if (found >= 0)
    {
        // Walk back from the goal, then write the moves forwards.
        for (i = found; i != start && n < STORY_PATH_MAX_STEPS; n++)
        {
            enum Direction dir = from[i];
            dirs[n] = dir;
            i -= sDirDy[dir] * width + sDirDx[dir];
        }
        if (i != start)
            n = -1;
        for (i = 0; i < n; i++)
            out[i] = (fast ? GetWalkFastMovementAction(dirs[n - 1 - i]) : GetWalkNormalMovementAction(dirs[n - 1 - i]));
        if (n >= 0 && mode == STORY_WALK_TO_PLAYER)
            out[n++] = GetFaceDirectionMovementAction(FaceTowards(found % width, found / width, goalX, goalY));
        if (n >= 0)
            out[n] = MOVEMENT_ACTION_STEP_END;
    }
    else
    {
        n = -1;
    }
    Free(queue);
    Free(from);
    return n;
}

// story_walk OBJ, MODE, X, Y, FAST (asm/macros/event.inc: approach_player, walk_to, walk_home)
void ScrCmd_storywalk(struct ScriptContext *ctx)
{
    u16 localId = VarGet(ScriptReadHalfword(ctx));
    enum StoryWalkMode mode = ScriptReadByte(ctx);
    u8 fast = ScriptReadByte(ctx);
    s16 x = VarGet(ScriptReadHalfword(ctx)) + MAP_OFFSET;
    s16 y = VarGet(ScriptReadHalfword(ctx)) + MAP_OFFSET;
    u8 mapNum = gSaveBlock1Ptr->location.mapNum, mapGroup = gSaveBlock1Ptr->location.mapGroup;
    u8 objectId = GetObjectEventIdByLocalIdAndMap(localId, mapNum, mapGroup);
    struct ObjectEvent *obj;
    u8 *moves;

    Script_RequestEffects(SCREFF_V1 | SCREFF_HARDWARE);
    gSpecialVar_Result = FALSE;
    if (objectId >= OBJECT_EVENTS_COUNT)
    {
        gStoryPathFailures++;
        return;
    }
    obj = &gObjectEvents[objectId];
    if (mode == STORY_WALK_TO_PLAYER)
    {
        struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];
        x = player->currentCoords.x;
        y = player->currentCoords.y;
    }
    else if (mode == STORY_WALK_HOME)
    {
        const struct ObjectEventTemplate *home = GetObjectEventTemplateByLocalIdAndMap(localId, mapNum, mapGroup);
        if (home == NULL)
        {
            gStoryPathFailures++;
            return;
        }
        x = home->x + MAP_OFFSET;
        y = home->y + MAP_OFFSET;
    }
    moves = sStoryPathMoves[sStoryPathSlot];
    sStoryPathSlot = (sStoryPathSlot + 1) % STORY_PATH_SLOTS;
    if (FindPath(obj, mode, x, y, fast, moves) < 0)
    {
        gStoryPathFailures++;
        DebugPrintf("story: no path for object %d", localId);
        return;
    }
    obj->directionOverwrite = DIR_NONE;
    ScriptMovement_StartObjectMovementScript(localId, mapNum, mapGroup, moves);
    gSpecialVar_Result = TRUE;
}

// face_npc OBJ: the player turns to face OBJ (after it has approached).
void ScrCmd_storyface(struct ScriptContext *ctx)
{
    u16 localId = VarGet(ScriptReadHalfword(ctx));
    u8 objectId = GetObjectEventIdByLocalIdAndMap(localId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
    struct ObjectEvent *player = &gObjectEvents[gPlayerAvatar.objectEventId];

    Script_RequestEffects(SCREFF_V1 | SCREFF_HARDWARE);
    if (objectId >= OBJECT_EVENTS_COUNT)
        return;
    ObjectEventTurn(player, FaceTowards(player->currentCoords.x, player->currentCoords.y,
                                        gObjectEvents[objectId].currentCoords.x, gObjectEvents[objectId].currentCoords.y));
}
