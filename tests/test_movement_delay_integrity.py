"""Host production dispatch coverage for NPC facing completion delays.

EC_MOVEMENT_SOURCE_ROOT selects an original source snapshot for differential
verification. Normal runs require the canonical owners and all eleven routes.
"""
import os
import re
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

SNAPSHOT = os.environ.get('EC_MOVEMENT_SOURCE_ROOT')
ROOT = Path(SNAPSHOT) if SNAPSHOT else Path(__file__).resolve().parents[1]
GROUPS = {
    'Medium': ['LookAround', 'FaceDownAndUp', 'FaceLeftAndRight'],
    'Short': ['FaceUpAndLeft', 'FaceUpAndRight', 'FaceDownAndLeft',
              'FaceDownAndRight', 'FaceDownUpAndLeft', 'FaceDownUpAndRight',
              'FaceUpLeftAndRight', 'FaceDownLeftAndRight'],
}


def extract_function(source, name):
    match = re.search(r'^(?:static )?(?:bool8|void) ' + re.escape(name) + r'\([^;]*?\)\n\{', source, re.M)
    if not match:
        raise AssertionError(f'Missing production definition: {name}')
    return source[match.start():source.index('\n}', match.end()) + 3] + '\n'


class MovementDelayIntegrity(unittest.TestCase):
    def test_all_facing_dispatches_preserve_rng_order_and_state(self):
        source = (ROOT / 'src/event_object_movement.c').read_text()
        tables = (ROOT / 'src/data/object_events/movement_type_func_tables.h').read_text()
        entries = []
        bodies = {}
        arrays = []
        for delay, groups in GROUPS.items():
            canonical = f'MovementType_Face_Set{delay}Delay'
            if not SNAPSHOT:
                extract_function(source, canonical)
            array = re.search(r'static const s16 sMovementDelays' + delay + r'\[\]\s*=\s*\{[^}]+\};', source)
            self.assertIsNotNone(array, f'Missing {delay} delay array')
            arrays.append(array.group())
            for group in groups:
                table = re.search(r'\bgMovementTypeFuncs_' + group + r'\[\]\)\([^;]*?=\s*\{(.*?)\};', tables, re.S)
                self.assertIsNotNone(table, f'Missing {group} dispatch table')
                slots = [part.strip() for part in re.sub(r'//[^\n]*', '', table[1]).split(',') if part.strip()]
                self.assertEqual(len(slots), 5, group)
                selected = slots[2]
                self.assertEqual(selected, canonical if not SNAPSHOT else f'MovementType_{group}_Step2')
                bodies[selected] = extract_function(source, selected)
                entries.append((selected, delay))
        self.assertEqual(len(entries), 11)
        if not SNAPSHOT:
            self.assertEqual(len(bodies), 2)
        code = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
typedef uint8_t bool8;
typedef int16_t s16;
#define FALSE 0
#define TRUE 1
#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
struct ObjectEvent { unsigned sentinel; bool8 singleMovementActive; };
struct Sprite { s16 data[8]; unsigned sentinel; };
#define sTypeFuncId data[1]
static struct ObjectEvent *expectedObject;
static struct Sprite *expectedSprite;
static int complete, trace[3], count, delayWritten, initialActive, initialType;
static unsigned randomValue, randomCalls;
static bool8 ObjectEventExecSingleMovementAction(struct ObjectEvent *o, struct Sprite *s)
{
 assert(o == expectedObject && s == expectedSprite && count == 0);
 assert(o->singleMovementActive == initialActive && s->sTypeFuncId == initialType);
 trace[count++] = 1;
 return complete;
}
static uint16_t Random(void)
{
 assert(count == 1);
 assert(expectedObject->singleMovementActive == initialActive && expectedSprite->sTypeFuncId == initialType);
 trace[count++] = 2; randomCalls++;
 return randomValue;
}
static void ProductionSetMovementDelay(struct Sprite *sprite, s16 timer);
static void SetMovementDelay(struct Sprite *s, s16 delay)
{
 assert(s == expectedSprite && count == 2);
 assert(expectedObject->singleMovementActive == initialActive && s->sTypeFuncId == initialType);
 trace[count++] = 3; delayWritten = delay;
 ProductionSetMovementDelay(s, delay);
}
'''
        code += extract_function(source, 'SetMovementDelay').replace('SetMovementDelay(', 'ProductionSetMovementDelay(')
        code += '\n'.join(arrays) + '\n' + '\n'.join(bodies.values())
        code += 'int main(void) {\nstruct { bool8 (*step)(struct ObjectEvent *, struct Sprite *); const s16 *delays; } entries[] = {\n'
        code += ',\n'.join('{%s, sMovementDelays%s}' % entry for entry in entries) + '};\n'
        code += r'''
const unsigned randomValues[] = {0,1,2,3,4,5,6,7,32767,32768,65532,65533,65534,65535};
const int initialTypes[] = {0,2,3,7};
assert(ARRAY_COUNT(entries) == 11);
assert(ARRAY_COUNT(sMovementDelaysMedium) == 4 && ARRAY_COUNT(sMovementDelaysShort) == 4);
const s16 medium[] = {32,64,96,128}, shortDelays[] = {32,48,64,80};
assert(memcmp(medium,sMovementDelaysMedium,sizeof medium)==0);
assert(memcmp(shortDelays,sMovementDelaysShort,sizeof shortDelays)==0);
unsigned exercised = 0;
for (unsigned e=0;e<ARRAY_COUNT(entries);e++)
 for (complete=0;complete<=1;complete++)
  for (initialActive=0;initialActive<=1;initialActive++)
   for (unsigned t=0;t<ARRAY_COUNT(initialTypes);t++)
    for (unsigned r=0;r<ARRAY_COUNT(randomValues);r++) {
     struct ObjectEvent object = {0}; struct Sprite sprite = {0};
     object.sentinel=0xABCD; object.singleMovementActive=initialActive;
     sprite.sentinel=0x1234;
     for (unsigned i=0;i<ARRAY_COUNT(sprite.data);i++) sprite.data[i]=100+i;
     sprite.sTypeFuncId=initialType=initialTypes[t];
     struct ObjectEvent expectedO=object; struct Sprite expectedS=sprite;
     expectedObject=&object; expectedSprite=&sprite;
     count=0; randomCalls=0; delayWritten=-1; randomValue=randomValues[r];
     assert(entries[e].step(&object,&sprite)==FALSE);
     assert(trace[0]==1);
     if (complete) {
      int delay=entries[e].delays[randomValue%4];
      assert(count==3 && trace[1]==2 && trace[2]==3 && randomCalls==1 && delayWritten==delay);
      expectedO.singleMovementActive=FALSE; expectedS.sTypeFuncId=3; expectedS.data[3]=delay; expectedS.data[7]=delay;
     } else assert(count==1 && randomCalls==0 && delayWritten==-1);
     assert(object.sentinel==expectedO.sentinel && object.singleMovementActive==expectedO.singleMovementActive);
     assert(sprite.sentinel==expectedS.sentinel && memcmp(sprite.data,expectedS.data,sizeof sprite.data)==0);
     exercised++;
    }
assert(exercised==2464);
return 0;
}
'''
        compiler = shutil.which('clang') or shutil.which('cc')
        self.assertIsNotNone(compiler, 'Host C compiler required')
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'delay.c'
            binary = Path(tmp) / 'delay'
            path.write_text(code)
            subprocess.run([compiler, '-std=c11', '-fsanitize=undefined', '-fno-sanitize-recover=all', str(path), '-o', str(binary)], check=True, capture_output=True, text=True, timeout=30)
            subprocess.run([str(binary)], check=True, capture_output=True, text=True, timeout=30)


if __name__ == '__main__':
    unittest.main()
