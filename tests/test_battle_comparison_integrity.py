"""Exercise production comparisons, typed bytecode decoding and Contrary boundaries.

The whole of src/battle_script_commands.c, src/battle_util.c and
src/battle_stat_change.c is compiled on the host into a library exporting only
the harness entry points. The harness authors the script bytecode it decodes.
"""
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tests'))
import host_c


def harness():
    code = r'''
#include <string.h>
'''
    for name, typ in [('byte', 'u8'), ('halfword', 'u16'), ('word', 'u32')]:
        code += f'''
struct __attribute__((packed)) Code_{name} {{u8 opcode,comparison; const {typ} *ptr; {typ} value; const u8 *jump; u8 next[1];}};
HOST_EXPORT u32 Run_{name}(u8 comparison, {typ} lhs, {typ} rhs) {{
    const u8 target[] = {{123}};
    struct Code_{name} code = {{.comparison=comparison,.ptr=comparison>CMP_BITMASK?0:&lhs,.value=rhs,.jump=target}};
    gBattlescriptCurrInstr=(const u8 *)&code;
    Cmd_jumpif{name}();
    if(gBattlescriptCurrInstr==target) return 1;
    if(gBattlescriptCurrInstr==code.next) return 0;
    return 99;
}}
'''
    code += r'''
struct __attribute__((packed)) ArrayCode {u8 opcode; const u8 *a, *b; u8 size; const u8 *jump; u8 next[1];};
HOST_EXPORT u32 RunArray(u32 unequal, const u8 *a, const u8 *b, u8 size) {
    const u8 target[] = {123};
    struct ArrayCode code = {.a=a,.b=b,.size=size,.jump=target};
    gBattlescriptCurrInstr=(const u8 *)&code;
    if(unequal) Cmd_jumpifarraynotequal(); else Cmd_jumpifarrayequal();
    if(gBattlescriptCurrInstr==target) return 1;
    if(gBattlescriptCurrInstr==code.next) return 0;
    return 99;
}
HOST_EXPORT u32 ReadOrder(void) {
    const u8 target[] = {123};
    struct Code_byte code = {.comparison=CMP_EQUAL,.ptr=(const u8 *)&gBattlescriptCurrInstr,.jump=target};
    const u8 *expected=code.next;
    memcpy(&code.value,&expected,1);
    gBattlescriptCurrInstr=(const u8 *)&code;
    Cmd_jumpifbyte();
    return gBattlescriptCurrInstr==target;
}
HOST_EXPORT u32 RunStat(u32 value,u32 comparison,u32 rhs,u32 contrary) {
    gBattleMons[2].statStages[STAT_ATK]=value;
    return CompareStat(2,STAT_ATK,rhs,comparison,contrary?ABILITY_CONTRARY:ABILITY_NONE);
}
'''
    return code


# battle_main.c boundary: the two globals these commands read and write.
BOUNDARY = r'''
#include "global.h"
#include "battle.h"
''' + host_c.ASSERTS + r'''
const u8 *gBattlescriptCurrInstr;
struct BattlePokemon gBattleMons[MAX_BATTLERS_COUNT];
'''


RUNNER = r'''
import ctypes, operator, random, sys
lib=ctypes.CDLL(sys.argv[1]); old=len(sys.argv)>2
ops=[operator.eq,operator.ne,operator.gt,operator.lt,lambda a,b:bool(a&b),lambda a,b:not(a&b)]
random.seed(907)
count=0
for name,width,typ in [('byte',8,ctypes.c_uint8),('halfword',16,ctypes.c_uint16),('word',32,ctypes.c_uint32)]:
 f=getattr(lib,'Run_'+name);f.argtypes=[ctypes.c_uint8,typ,typ];f.restype=ctypes.c_uint32
 mask=(1<<width)-1
 values=sorted({0,1,2,7,8,15,16,30,31,32,63,127,128,255,mask,mask>>1,1<<(width-1)})
 pairs=[(a&mask,b&mask) for a in values for b in values]+[(random.getrandbits(width),random.getrandbits(width)) for _ in range(2048)]
 for a,b in pairs:
  for cmp in range(8):
   if old and cmp==6 and b>=31: continue # Original signed shift has no defined result here.
   expected=int(ops[cmp](a,b)) if cmp<6 else int(cmp==6 and b<width and bool(a&(1<<b)))
   actual=f(cmp,a,b)
   assert actual==expected,(name,cmp,a,b,actual,expected)
   count+=1
 for cmp in range(7,256): assert f(cmp,0,0)==0 # NULL operand must not be read.
assert lib.ReadOrder()==1
f=lib.RunStat;f.argtypes=[ctypes.c_uint32]*4;f.restype=ctypes.c_uint32
for value in range(13):
 for cmp in range(9):
  for rhs in [0,1,5,6,7,11,12,13,31,0x80000000,0xffffffff]:
   for contrary in range(2):
    boundary=12-rhs if contrary and rhs in (0,12) else rhs
    op=({2:3,3:2}.get(cmp,cmp) if contrary else cmp)
    expected=int(ops[op](value,boundary)) if op<6 else 0
    assert f(value,cmp,rhs,contrary)==expected,(value,cmp,rhs,contrary)
    count+=1
print(count,'comparison and dispatch cases passed')
f=lib.RunArray;f.argtypes=[ctypes.c_uint32,ctypes.c_void_p,ctypes.c_void_p,ctypes.c_uint8];f.restype=ctypes.c_uint32
array_count=0
for length in range(256):
 data=bytes((i*37+11)&255 for i in range(length+4))
 a=ctypes.create_string_buffer(data)
 for mismatch in range(length+1):
  other=bytearray(data);other[mismatch]^=0xff;b=ctypes.create_string_buffer(bytes(other))
  for unequal in range(2):
   expected=int((data[:length]!=bytes(other[:length]))==bool(unequal))
   assert f(unequal,ctypes.addressof(a) if length else None,ctypes.addressof(b) if length else None,length)==expected
   array_count+=1
 for offset in range(4):
  for unequal in range(2):
   expected=int((data[:length]!=data[offset:offset+length])==bool(unequal))
   assert f(unequal,ctypes.addressof(a),ctypes.addressof(a)+offset,length)==expected
   array_count+=1
print(array_count,'array comparison cases passed')
'''


class BattleComparisonIntegrity(unittest.TestCase):
    def test_operations_decoding_and_stat_policy(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)
            host_c.build(path, {
                'battle_script_commands.c': host_c.production('src/battle_script_commands.c') + harness(),
                'battle_util.c': host_c.production('src/battle_util.c'),
                'battle_stat_change.c': host_c.production('src/battle_stat_change.c'),
                'battle_main_boundary.c': BOUNDARY,
            }, shared=True, optimize='-O2')
            result = subprocess.run([sys.executable, '-c', RUNNER, str(path / 'test.so')], capture_output=True, text=True, timeout=20)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertIn('cases passed', result.stdout)


if __name__ == '__main__':
    unittest.main()
