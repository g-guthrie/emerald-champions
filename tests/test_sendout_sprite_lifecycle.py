"""Execute production send-out boundaries under the captured transient sprite pressure.

The native crash had 30 live particles and failed on the second partner's
controller allocation. These host tests stub sprite demand, not the scheduling
branches, and check that waiting performs no partial send-out work.
"""
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def function(source, signature):
    start = source.index(signature + '\n{')
    end = source.index('{', start)
    depth = 1
    cursor = end + 1
    while depth:
        depth += (source[cursor] == '{') - (source[cursor] == '}')
        cursor += 1
    return source[start:cursor]


class SendoutSpriteLifecycle(unittest.TestCase):
    def test_intro_and_switch_wait_then_start_once(self):
        source = (ROOT / 'src/battle_controllers.c').read_text()
        task = function(source, 'static void Task_StartSendOutAnim(u8 taskId)')
        switch = function(source, 'void BtlController_HandleSwitchInAnim(enum BattlerId battler)')
        harness = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
typedef uint8_t u8;
typedef int bool32;
#define FALSE 0
#define PARTY_SIZE 6
#define BIT_FLANK 2
enum BattlerId { PLAYER, OPPONENT, PARTNER, OPPONENT_PARTNER };
struct Animation {unsigned numBallParticles;} animation;
struct Sprites {struct Animation *animationData;} sprites={&animation}, *gBattleSpritesDataPtr=&sprites;
struct Task {int data[16];} gTasks[1];
#define tBattlerId data[0]
#define tStartTimer data[1]
#define tFramesToWait data[2]
#define tControllerFunc_1 3
struct Resources {u8 bufferA[4][64];} resources, *gBattleResources=&resources;
struct Battle {unsigned monToSwitchIntoId[4];} battle, *gBattleStruct=&battle;
unsigned gBattlerPartyIndexes[4],gActionSelectionCursor[4],gMoveSelectionCursor[4];
void (*gBattlerControllerFuncs[4])(enum BattlerId);
static unsigned starts[4],gfxLoads,clears,destroyed,freeSlots;
static int paired;
static void done(enum BattlerId b){}
static void BtlController_HandleSwitchInTryShinyAnim(enum BattlerId b){}
static int TwoMonsAtSendOut(enum BattlerId b){return paired;}
static int ShouldDoSlideInAnim(enum BattlerId b){return 0;}
static void *GetBattlerMon(enum BattlerId b){return 0;}
static void BattleLoadMonSpriteGfx(void *mon,enum BattlerId b){gfxLoads++;}
static void ClearTemporarySpeciesSpriteData(enum BattlerId b,int a,int c){clears++;}
static void StartSendOutAnim(enum BattlerId b,int a,int c,int d){assert(freeSlots>=2);freeSlots-=2;starts[b]++;}
static uintptr_t GetWordTaskArg(unsigned task,unsigned field){return (uintptr_t)done;}
static void DestroyTask(unsigned task){destroyed++;}
static int IsControllerPlayer(enum BattlerId b){return b==PLAYER;}
static int IsControllerOpponent(enum BattlerId b){return b==OPPONENT;}
static int IsControllerPlayerPartner(enum BattlerId b){return 0;}
static int IsControllerRecordedPlayer(enum BattlerId b){return 0;}
static int IsControllerRecordedPartner(enum BattlerId b){return 0;}
static int IsControllerLinkPartner(enum BattlerId b){return 0;}
static void reset(void){
 memset(gTasks,0,sizeof(gTasks));memset(&resources,0x55,sizeof(resources));
 memset(starts,0,sizeof(starts));memset(gBattlerControllerFuncs,0,sizeof(gBattlerControllerFuncs));
 gfxLoads=clears=destroyed=0;animation.numBallParticles=30;freeSlots=2;
 for(unsigned i=0;i<4;i++){gBattlerPartyIndexes[i]=i;gActionSelectionCursor[i]=7;gMoveSelectionCursor[i]=8;battle.monToSwitchIntoId[i]=9;}
}
'''
        harness += task + '\n' + switch + r'''
int main(void){
 for(unsigned mode=0;mode<2;mode++){
  unsigned pair=1-mode;
  reset();paired=pair;
  for(unsigned frame=0;frame<8;frame++)Task_StartSendOutAnim(0);
  assert(!starts[0]&&!starts[2]&&!gfxLoads&&!destroyed);
  assert(resources.bufferA[0][1]==0x55&&resources.bufferA[2][1]==0x55);
  assert(!gBattlerControllerFuncs[0]);
  animation.numBallParticles=0;freeSlots=32;
  Task_StartSendOutAnim(0);
  assert(starts[0]==1&&starts[2]==pair&&gfxLoads==pair&&destroyed==1);
  assert(gBattlerControllerFuncs[0]==done);
 }
 reset();paired=1;gTasks[0].tFramesToWait=3;
 for(unsigned frame=0;frame<8;frame++)Task_StartSendOutAnim(0);
 assert(gTasks[0].tStartTimer==3&&!starts[0]&&!starts[2]&&!gfxLoads&&!destroyed);
 animation.numBallParticles=0;freeSlots=32;
 Task_StartSendOutAnim(0);
 assert(starts[0]==1&&starts[2]==1&&destroyed==1&&gTasks[0].tStartTimer==3);
 for(unsigned b=0;b<2;b++){
  reset();gBattlerControllerFuncs[b]=BtlController_HandleSwitchInAnim;
  for(unsigned frame=0;frame<8;frame++)gBattlerControllerFuncs[b](b);
  assert(!starts[b]&&!gfxLoads&&!clears);
  assert(gBattlerPartyIndexes[b]==b&&gActionSelectionCursor[b]==7&&gMoveSelectionCursor[b]==8&&battle.monToSwitchIntoId[b]==9);
  assert(gBattlerControllerFuncs[b]==BtlController_HandleSwitchInAnim);
  animation.numBallParticles=0;freeSlots=32;
  gBattlerControllerFuncs[b](b);
  assert(starts[b]==1&&gBattlerPartyIndexes[b]==0x55);
  assert(gfxLoads==(b==PLAYER)&&clears==(b==PLAYER));
  assert(gBattlerControllerFuncs[b]==BtlController_HandleSwitchInTryShinyAnim);
  gBattlerControllerFuncs[b](b);assert(starts[b]==1);
 }
 return 0;
}
'''
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary)
            (path / 'test.c').write_text(harness)
            subprocess.run([shutil.which('cc'), '-std=c99', str(path / 'test.c'), '-o', str(path / 'test')], check=True)
            subprocess.run([str(path / 'test')], check=True)


if __name__ == '__main__':
    unittest.main()
