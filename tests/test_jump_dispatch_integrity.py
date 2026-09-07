"""Production jump initialization, completion and dispatch with controlled animation.

The animation stub changes fields and returns completion on a chosen frame;
physics and rendering remain emulator coverage rather than host claims.
"""
import re
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FAMILIES = ('Jump2', 'Jump', 'JumpInPlace', 'AcroWheelieHopFace', 'AcroWheelieHop', 'AcroWheelieJump')
DIRECTIONS = ('Down', 'Up', 'Left', 'Right')


def jump_harness(source, tables):
    names = [family + direction for family in FAMILIES for direction in DIRECTIONS]
    functions = []
    table_bodies = []
    for name in names:
        table = re.search(r'u8 \(\*const gMovementActionFuncs_' + name + r'\[\]\)\([^;{]+\) = \{.*?\n\};', tables, re.S).group()
        table_bodies.append(table)
        for function in re.findall(r'\bMovementAction_\w+', table):
            if function != 'MovementAction_PauseSpriteAnim' and function not in functions:
                functions.append(function)
    code = r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
typedef uint8_t u8;
typedef uint8_t bool8;
#define TRUE 1
#define FALSE 0
#define sActionFuncId data[2]
enum Direction {DIR_NONE, DIR_SOUTH, DIR_NORTH, DIR_WEST, DIR_EAST};
enum {JUMP_DISTANCE_IN_PLACE, JUMP_DISTANCE_NORMAL, JUMP_DISTANCE_FAR};
enum {JUMP_TYPE_HIGH, JUMP_TYPE_NORMAL, JUMP_TYPE_LOW};
struct ObjectEvent {unsigned noShadow, directionOverwrite, landingJump, animationTicks;};
struct Sprite {unsigned data[8], animationTicks;};
static unsigned initCalls, jumpCalls, finishFrame, initKind, initDirection, initDistance, initType;
static void Init(struct ObjectEvent *obj, struct Sprite *sprite, enum Direction direction, unsigned distance, unsigned type, unsigned kind) {
    initCalls++;initDirection=direction;initDistance=distance;initType=type;initKind=kind;sprite->sActionFuncId=1;
}
static void InitJumpRegular(struct ObjectEvent *obj, struct Sprite *sprite, enum Direction direction, unsigned distance, unsigned type) {Init(obj,sprite,direction,distance,type,0);}
static void InitAcroWheelieJump(struct ObjectEvent *obj, struct Sprite *sprite, enum Direction direction, unsigned distance, unsigned type) {Init(obj,sprite,direction,distance,type,1);}
static bool8 DoJumpAnim(struct ObjectEvent *obj, struct Sprite *sprite) {
    obj->animationTicks+=3;sprite->animationTicks+=2;
    return jumpCalls++ >= finishFrame;
}
static u8 MovementAction_PauseSpriteAnim(struct ObjectEvent *obj, struct Sprite *sprite) {return TRUE;}
'''
    for name in functions:
        code += f'bool8 {name}(struct ObjectEvent *, struct Sprite *);\n'
    for name in functions:
        code += re.search(r'bool8 ' + name + r'\([^;]+?\)\n\{.*?\n\}', source, re.S).group() + '\n'
    code += '\n'.join(table_bodies)
    code += '\nstatic u8 (*const *actions[])(struct ObjectEvent *,struct Sprite *) = {\n'
    code += ',\n'.join('gMovementActionFuncs_' + name for name in names) + '};\n'
    code += r'''
static uint64_t trace=1469598103934665603ULL;
static void Hash(unsigned value){trace^=value;trace*=1099511628211ULL;}
int main(void) {
    const unsigned distances[]={JUMP_DISTANCE_FAR,JUMP_DISTANCE_NORMAL,JUMP_DISTANCE_IN_PLACE,JUMP_DISTANCE_IN_PLACE,JUMP_DISTANCE_NORMAL,JUMP_DISTANCE_FAR};
    const unsigned types[]={JUMP_TYPE_HIGH,JUMP_TYPE_NORMAL,JUMP_TYPE_HIGH,JUMP_TYPE_LOW,JUMP_TYPE_LOW,JUMP_TYPE_HIGH};
    const unsigned finishFrames[]={0,1,2,7};
    unsigned scenarios=0;
    for(unsigned action=0;action<24;action++) for(unsigned shadow=0;shadow<2;shadow++)
    for(unsigned completion=0;completion<4;completion++) for(unsigned overwrite=0;overwrite<=4;overwrite++) {
        struct ObjectEvent obj={shadow,overwrite,7,0};
        struct Sprite sprite={{19,19,0,19,19,19,19,19},0};
        initCalls=jumpCalls=0;finishFrame=finishFrames[completion];
        for(unsigned frame=0;frame<=finishFrame;frame++) {
            unsigned done=actions[action][frame?1:0](&obj,&sprite);
            assert(done==(frame==finishFrame));
            assert(initCalls==1 && jumpCalls==frame+1);
            assert(sprite.sActionFuncId==(done?2:1));
            assert(obj.noShadow==(done?0:shadow));
            assert(obj.directionOverwrite==overwrite && obj.landingJump==7);
            assert(obj.animationTicks==3*(frame+1) && sprite.animationTicks==2*(frame+1));
            for(unsigned i=0;i<8;i++)if(i!=2)assert(sprite.data[i]==19);
            Hash(done);Hash(sprite.sActionFuncId);Hash(obj.noShadow);Hash(obj.animationTicks);Hash(sprite.animationTicks);
        }
        unsigned family=action/4,direction=action%4;
        unsigned expectedDirection=family>=4 && direction>=2 && overwrite ? overwrite : direction+1;
        assert(initKind==(family>=3));
        assert(initDirection==expectedDirection && initDistance==distances[family] && initType==types[family]);
        assert(actions[action][2]==MovementAction_PauseSpriteAnim);
        Hash(initKind);Hash(initDirection);Hash(initDistance);Hash(initType);scenarios++;
    }
    printf("jump scenarios=%u trace=%016llx\n",scenarios,(unsigned long long)trace);
    return 0;
}
'''
    return code


def run_harness(code):
    with tempfile.TemporaryDirectory() as temp:
        path = Path(temp)
        (path / 'test.c').write_text(code)
        subprocess.run(['cc', '-O1', '-std=c11', '-fsanitize=undefined', str(path / 'test.c'), '-o', str(path / 'test')], check=True, timeout=30)
        return subprocess.check_output([str(path / 'test')], text=True, timeout=30)


class JumpDispatchIntegrity(unittest.TestCase):
    def test_every_ordinary_jump_action_preserves_initialization_and_completion(self):
        run_harness(jump_harness((ROOT / 'src/event_object_movement.c').read_text(),
                                (ROOT / 'src/data/object_events/movement_action_func_tables.h').read_text()))


if __name__ == '__main__':
    unittest.main()
