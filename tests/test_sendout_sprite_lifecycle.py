"""Execute production send-out boundaries under the captured transient sprite pressure.

The native crash had 30 live particles and failed on the second partner's
controller allocation. The whole of src/battle_controllers.c is compiled on the
host (including the real StartSendOutAnim and intro/switch scheduling); the
harness stubs only other units' sprite, task, graphics and party services, and
checks that waiting performs no partial send-out work.
"""
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tests'))
import host_c

# Reachable only through controller callbacks this fixture never advances to;
# they resolve to trapping placeholders so any call aborts the run.
UNCALLED = (
    'CopyBattleSpriteInvisibility', 'CreateTask', 'DestroySprite', 'FreeSpritePaletteByTag',
    'FreeSpriteTilesByTag', 'GetFirstLiveMon', 'GetPartnerBattler', 'HandleLowHpMusicChange',
    'InitAndLaunchSpecialAnimation', 'IsCryPlayingOrClearCrySongs', 'SetBattlerShadowSpriteCallback',
    'SetHealthboxSpriteVisible', 'SpriteCB_WaitForBattlerBallReleaseAnim', 'SpriteCallbackDummy_2',
    'StartHealthboxSlideIn', 'Task_PlayerController_RestoreBgmAfterCry', 'UpdateHealthboxAttribute',
    'm4aMPlayVolumeControl', 'gMPlayInfo_BGM',
)

# Other units' state and services, with their real declarations.
BOUNDARY = r'''
#include "global.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_controllers.h"
#include "battle_gfx_sfx_util.h"
#include "battle_setup.h"
#include "battle_util.h"
#include "event_object_movement.h"
#include "pokeball.h"
#include "sprite.h"
#include "task.h"
#include "util.h"
''' + host_c.ASSERTS + r'''
#include "host_sendout.h"
struct Task gTasks[NUM_TASKS];
struct Sprite gSprites[MAX_SPRITES + 1];
struct SpriteTemplate gMultiuseSpriteTemplate;
static struct BattleAnimationInfo sAnimation;
static struct BattleHealthboxInfo sHealthboxes[MAX_BATTLERS_COUNT];
static struct BattleSpriteInfo sBattlerSprites[MAX_BATTLERS_COUNT];
static struct BattleSpriteData sSprites = {.battlerData = sBattlerSprites, .healthBoxesData = sHealthboxes, .animationData = &sAnimation};
struct BattleSpriteData *gBattleSpritesDataPtr = &sSprites;
static struct BattleResources sResources;
struct BattleResources *gBattleResources = &sResources;
static struct BattleStruct sBattleStruct;
struct BattleStruct *gBattleStruct = &sBattleStruct;
u32 gBattleTypeFlags;
u8 gBattlersCount = MAX_BATTLERS_COUNT;
u16 gBattlerPartyIndexes[MAX_BATTLERS_COUNT];
u8 gBattlerPositions[MAX_BATTLERS_COUNT];
u8 gBattlerSpriteIds[MAX_BATTLERS_COUNT];
u8 gHealthboxSpriteIds[MAX_BATTLERS_COUNT];
u8 gActionSelectionCursor[MAX_BATTLERS_COUNT];
u8 gMoveSelectionCursor[MAX_BATTLERS_COUNT];
TrainerBattleParameter gTrainerBattleParameter;
struct HostSendout gHost;
static struct Pokemon sMons[MAX_BATTLERS_COUNT];

// Controller identities are compared by address only.
void PlayerBufferExecCompleted(enum BattlerId battler) { gHost.executed = 1; }
void OpponentBufferExecCompleted(enum BattlerId battler) { gHost.executed = 2; }
void PlayerPartnerBufferExecCompleted(enum BattlerId battler) { gHost.executed = 3; }
void RecordedPlayerBufferExecCompleted(enum BattlerId battler) { gHost.executed = 4; }
void RecordedPartnerBufferExecCompleted(enum BattlerId battler) { gHost.executed = 5; }
void LinkPartnerBufferExecCompleted(enum BattlerId battler) { gHost.executed = 6; }
void SpriteCallbackDummy(struct Sprite *sprite) { gHost.executed = 7; }

struct Pokemon *GetBattlerMon(enum BattlerId battler) { return &sMons[battler]; }
u32 GetMonData2(struct Pokemon *mon, s32 field)
{
    // Every battler holds a healthy, non-egg Pokemon.
    if (field == MON_DATA_SPECIES_OR_EGG || field == MON_DATA_SPECIES) return SPECIES_ZIGZAGOON;
    if (field == MON_DATA_HP) return 20;
    assert(field == MON_DATA_IS_EGG);
    return FALSE;
}
struct ObjectEvent *GetFollowerObject(void) { return NULL; }
enum Species GetBattlerVisualSpecies(enum BattlerId battler) { return SPECIES_ZIGZAGOON; }
void BattleLoadMonSpriteGfx(struct Pokemon *mon, enum BattlerId battler) { assert(mon == &sMons[battler]); gHost.gfxLoads++; }
void ClearTemporarySpeciesSpriteData(enum BattlerId battler, bool32 dontClearTransform, bool32 dontClearSubstitute) { gHost.clears++; }
static u8 TakeSpriteSlot(void) { assert(gHost.freeSlots > 0); gHost.freeSlots--; return MAX_SPRITES - 1 - gHost.freeSlots; }
u8 CreateInvisibleSpriteWithCallback(void (*callback)(struct Sprite *)) { return TakeSpriteSlot(); }
u32 CreateSprite(const struct SpriteTemplate *template, s16 x, s16 y, u32 subpriority) { return TakeSpriteSlot(); }
void SetMultiuseSpriteTemplateToPokemon(enum Species speciesTag, enum BattlerPosition battlerPosition) {}
u8 GetBattlerSpriteCoord(enum BattlerId battler, u8 coordType) { return 0; }
u8 GetBattlerSpriteDefault_Y(enum BattlerId battler) { return 0; }
u8 GetBattlerSpriteSubpriority(enum BattlerId battler) { return 0; }
void StartSpriteAnim(struct Sprite *sprite, u8 animNum) {}
u8 DoPokeballSendOutAnimation(enum BattlerId battler, s16 pan, u8 kindOfThrow) { gHost.starts[battler]++; return 0; }
void TryShinyAnimation(enum BattlerId battler, struct Pokemon *mon) {}
// Task words hold a 32-bit GBA pointer; the host sees the truncated value.
u32 GetWordTaskArg(u8 taskId, u8 dataElem) { assert(taskId == 0 && dataElem == 3); return (u32)(uintptr_t)HostDone; }
void DestroyTask(u8 taskId) { assert(taskId == 0); gHost.destroyed++; }
'''

HOST_SENDOUT = r'''
struct HostSendout { unsigned starts[MAX_BATTLERS_COUNT], gfxLoads, clears, destroyed, freeSlots, executed; };
extern struct HostSendout gHost;
void HostDone(enum BattlerId battler);
'''

HARNESS = r'''
#include <string.h>
#include "host_sendout.h"
#define PLAYER B_POSITION_PLAYER_LEFT
#define OPPONENT B_POSITION_OPPONENT_LEFT
#define PARTNER B_POSITION_PLAYER_RIGHT
void HostDone(enum BattlerId battler) {}
#define sDone ((void (*)(enum BattlerId))(uintptr_t)(u32)(uintptr_t)HostDone)
static void reset(u32 battleType){
 memset(gTasks,0,sizeof(struct Task)*NUM_TASKS);memset(gBattleResources,0x55,sizeof(*gBattleResources));
 memset(&gHost,0,sizeof(gHost));memset(gBattlerControllerFuncs,0,sizeof(gBattlerControllerFuncs));
 memset(gBattleSpritesDataPtr->healthBoxesData,0,sizeof(struct BattleHealthboxInfo)*MAX_BATTLERS_COUNT);
 gBattleTypeFlags=battleType;gBattleSpritesDataPtr->animationData->numBallParticles=30;gHost.freeSlots=2;
 for(unsigned i=0;i<MAX_BATTLERS_COUNT;i++){
  gBattlerPositions[i]=i;gBattlerPartyIndexes[i]=i;gActionSelectionCursor[i]=7;gMoveSelectionCursor[i]=8;gBattleStruct->monToSwitchIntoId[i]=9;
  gBattlerControllerEndFuncs[i]=i==PLAYER?PlayerBufferExecCompleted:OpponentBufferExecCompleted;
 }
}
int main(void){
 for(unsigned mode=0;mode<2;mode++){
  unsigned pair=1-mode;
  reset(pair?BATTLE_TYPE_DOUBLE:0);
  assert(TwoMonsAtSendOut(PLAYER)==pair);
  for(unsigned frame=0;frame<8;frame++)Task_StartSendOutAnim(0);
  assert(!gHost.starts[PLAYER]&&!gHost.starts[PARTNER]&&!gHost.gfxLoads&&!gHost.clears&&!gHost.destroyed);
  assert(gBattleResources->bufferA[PLAYER][1]==0x55&&gBattleResources->bufferA[PARTNER][1]==0x55);
  assert(!gBattlerControllerFuncs[PLAYER]);
  gBattleSpritesDataPtr->animationData->numBallParticles=0;gHost.freeSlots=32;
  Task_StartSendOutAnim(0);
  // Player-side send-outs load only the partner's graphics before starting.
  assert(gHost.starts[PLAYER]==1&&gHost.starts[PARTNER]==pair&&gHost.gfxLoads==pair&&gHost.destroyed==1);
  assert(gBattlerControllerFuncs[PLAYER]==sDone);
 }
 reset(BATTLE_TYPE_DOUBLE);gTasks[0].data[2]=3; // tFramesToWait
 for(unsigned frame=0;frame<8;frame++)Task_StartSendOutAnim(0);
 assert(gTasks[0].data[1]==3&&!gHost.starts[PLAYER]&&!gHost.starts[PARTNER]&&!gHost.gfxLoads&&!gHost.destroyed); // tStartTimer
 gBattleSpritesDataPtr->animationData->numBallParticles=0;gHost.freeSlots=32;
 Task_StartSendOutAnim(0);
 assert(gHost.starts[PLAYER]==1&&gHost.starts[PARTNER]==1&&gHost.destroyed==1&&gTasks[0].data[1]==3);
 for(unsigned b=0;b<2;b++){
  reset(0);gBattlerControllerFuncs[b]=BtlController_HandleSwitchInAnim;
  for(unsigned frame=0;frame<8;frame++)gBattlerControllerFuncs[b](b);
  assert(!gHost.starts[b]&&!gHost.gfxLoads&&!gHost.clears);
  assert(gBattlerPartyIndexes[b]==b&&gActionSelectionCursor[b]==7&&gMoveSelectionCursor[b]==8&&gBattleStruct->monToSwitchIntoId[b]==9);
  assert(gBattlerControllerFuncs[b]==BtlController_HandleSwitchInAnim);
  gBattleSpritesDataPtr->animationData->numBallParticles=0;gHost.freeSlots=32;
  gBattlerControllerFuncs[b](b);
  assert(gHost.starts[b]==1&&gBattlerPartyIndexes[b]==0x55);
  // The switch loads/clears the player's sprite itself; StartSendOutAnim
  // clears every battler and loads the opponent's.
  assert(gHost.gfxLoads==1&&gHost.clears==(b==PLAYER)+1);
  assert(b==PLAYER?gActionSelectionCursor[b]==0&&gMoveSelectionCursor[b]==0:gBattleStruct->monToSwitchIntoId[b]==PARTY_SIZE);
  assert(gBattlerControllerFuncs[b]==BtlController_HandleSwitchInTryShinyAnim);
  gBattlerControllerFuncs[b](b);assert(gHost.starts[b]==1);
 }
 return 0;
}
'''


class SendoutSpriteLifecycle(unittest.TestCase):
    def test_intro_and_switch_wait_then_start_once(self):
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary)
            (path / 'host_sendout.h').write_text(HOST_SENDOUT)
            executable = host_c.build(path, {
                'battle_controllers.c': host_c.production('src/battle_controllers.c') + HARNESS,
                'boundary.c': BOUNDARY,
            }, flags=('-iquote', str(path)), inert=tuple(UNCALLED))
            host_c.run(executable)


if __name__ == '__main__':
    unittest.main()
