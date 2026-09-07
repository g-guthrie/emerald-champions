"""Execute copy/follow dispatch with controlled direction, collision and tile APIs."""
import re
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ACTIONS = ('WALK_FAST', 'WALK_FASTER', 'SLIDE', 'JUMP_IN_PLACE', 'JUMP')


def follow_harness(source, tables):
    entries = []
    for table in ('gCopyPlayerMovementFuncs', 'gFollowPlayerMovementFuncs'):
        body = re.search(r'bool8 \(\*const ' + table + r'\[\]\)\([^;{]+\) = \{.*?\n\};', tables, re.S).group()
        mapping = dict(re.findall(r'\[COPY_MOVE_(\w+)\]\s*=\s*(\w+)', body))
        entries.extend(mapping[action] for action in ACTIONS)
    code = r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
typedef uint8_t u8;
typedef uint8_t bool8;
typedef int16_t s16;
#define TRUE 1
#define FALSE 0
#define sTypeFuncId data[1]
enum Direction {DIR_NONE, DIR_SOUTH, DIR_NORTH, DIR_WEST, DIR_EAST};
struct ObjectEvent {unsigned movementType, directionSequenceIndex, singleMovementActive, noShadow, landingJump;};
struct Sprite {unsigned data[8];};
static const enum Direction gInitialMovementTypeFacingDirections[]={DIR_SOUTH,DIR_NORTH,DIR_WEST,DIR_EAST};
static unsigned initial, sequence, player, direction, collision, accepted, active, selected;
static unsigned trace[16], count;
static void Record(unsigned event){assert(count<16);trace[count++]=event;}
static enum Direction GetCopyDirection(enum Direction a,unsigned b,enum Direction c){assert(a==initial+1 && b==sequence && c==player);Record(1);return direction;}
static void ObjectEventMoveDestCoords(struct ObjectEvent *obj,enum Direction d,s16 *x,s16 *y){assert(d==direction);Record(2);*x=3*(int)d-12;*y=5*(int)sequence-10;}
'''
    for kind, name in enumerate(('WalkFast', 'WalkFaster', 'Slide', 'JumpInPlace', 'Jump', 'FaceDirection')):
        # FaceDirection uses the existing GetFaceDirectionMovementAction name.
        code += f'static unsigned Get{name}MovementAction(enum Direction d) {{assert(d==direction);Record({10+kind});return {16*kind}+d;}}\n'
    code += r'''
static void ObjectEventSetSingleMovement(struct ObjectEvent *obj,struct Sprite *sprite,unsigned action){assert(obj->singleMovementActive==active && sprite->sTypeFuncId==13);Record(3);selected=action;}
static unsigned GetCollisionAtCoords(struct ObjectEvent *obj,s16 x,s16 y,enum Direction d){assert(d==direction && x==3*(int)d-12 && y==5*(int)sequence-10);Record(4);return collision;}
static u8 MapGridGetMetatileBehaviorAt(s16 x,s16 y){assert(x==3*(int)direction-12 && y==5*(int)sequence-10);Record(5);return 123;}
static bool8 Tile(u8 behavior){assert(behavior==123);Record(6);return accepted;}
'''
    for name in dict.fromkeys(entries):
        code += re.search(r'bool8 ' + name + r'\([^;]+?\)\n\{.*?\n\}', source, re.S).group() + '\n'
    code += 'static bool8 (*const entries[])(struct ObjectEvent *,struct Sprite *,enum Direction,bool8(u8))={' + ','.join(entries) + '};\n'
    code += r'''
static uint64_t hash=1469598103934665603ULL;
static void Hash(unsigned value){hash^=value;hash*=1099511628211ULL;}
int main(void){
 unsigned scenarios=0;
 for(unsigned entry=0;entry<10;entry++)for(initial=0;initial<4;initial++)for(sequence=0;sequence<4;sequence++)
 for(player=1;player<=8;player++)for(collision=0;collision<2;collision++)for(unsigned callback=0;callback<3;callback++)for(active=0;active<2;active++){
  direction=(initial+sequence+player)%4+1;accepted=callback==2;
  struct ObjectEvent obj={initial,sequence,active,7,9};struct Sprite sprite={{13,13,13,13,13,13,13,13}};
  count=0;selected=999;assert(entries[entry](&obj,&sprite,player,callback?Tile:NULL));
  unsigned kind=entry%5,at=0;assert(trace[at++]==1);
  if(kind!=3)assert(trace[at++]==2);
  assert(trace[at++]==10+kind);assert(trace[at++]==3);
  unsigned fallback=0;
  if(kind!=3){
   assert(trace[at++]==4);
   if(!collision && callback){assert(trace[at++]==5);assert(trace[at++]==6);}
   fallback=collision || (callback && !accepted);
   if(fallback){assert(trace[at++]==15);assert(trace[at++]==3);}
  }
  assert(count==at && selected==16*(fallback?5:kind)+direction);
  assert(obj.movementType==initial && obj.directionSequenceIndex==sequence && obj.singleMovementActive==TRUE && obj.noShadow==7 && obj.landingJump==9);
  assert(sprite.sTypeFuncId==2);for(unsigned i=0;i<8;i++)if(i!=1)assert(sprite.data[i]==13);
  Hash(selected);Hash(count);for(unsigned i=0;i<count;i++)Hash(trace[i]);Hash(obj.singleMovementActive);Hash(sprite.sTypeFuncId);scenarios++;
 }
 printf("follow scenarios=%u trace=%016llx\n",scenarios,(unsigned long long)hash);return 0;
}
'''
    return code


def run_harness(code):
    with tempfile.TemporaryDirectory() as temp:
        path = Path(temp)
        (path / 'test.c').write_text(code)
        subprocess.run(['cc', '-O1', '-std=c11', '-fsanitize=undefined', '-fno-sanitize-recover=undefined', str(path / 'test.c'), '-o', str(path / 'test')], check=True, timeout=30)
        return subprocess.check_output([str(path / 'test')], text=True, timeout=30)


class FollowMovementIntegrity(unittest.TestCase):
    def test_copy_and_follow_entries_preserve_helper_order_and_state(self):
        run_harness(follow_harness((ROOT / 'src/event_object_movement.c').read_text(),
                                  (ROOT / 'src/data/object_events/movement_type_func_tables.h').read_text()))


if __name__ == '__main__':
    unittest.main()
