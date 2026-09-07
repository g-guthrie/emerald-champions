"""Execute production difficulty script APIs, including their effect ordering.

EC_DIFFICULTY_SOURCE_ROOT selects a pre-refactor snapshot for the same checks.
"""
import os
import re
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(os.environ.get('EC_DIFFICULTY_SOURCE_ROOT', Path(__file__).resolve().parents[1]))
CONSTANTS = Path(__file__).resolve().parents[1] / 'include/constants/difficulty.h'


class DifficultyScriptIntegrity(unittest.TestCase):
    def test_script_effects_and_capped_writes_in_all_configurations(self):
        source = (ROOT / 'src/difficulty.c').read_text()
        names = ['GetCurrentDifficultyLevel', 'SetCurrentDifficultyLevel',
                 'Script_IncreaseDifficulty', 'Script_DecreaseDifficulty',
                 'Script_GetDifficulty', 'Script_SetDifficulty']
        functions = []
        for name in names:
            match = re.search(r'^(?:enum DifficultyLevel|void) ' + name + r'\([^;]*?\)\n\{', source, re.M)
            self.assertIsNotNone(match, f'Missing production function {name}')
            functions.append(source[match.start():source.index('\n}', match.end()) + 3])
        self.assertEqual(len(functions), 6)
        code = r'''
#include <assert.h>
#include <stdint.h>
typedef uint8_t u8; typedef uint16_t u16;
#define SCREFF_V1 17
struct ScriptContext { unsigned position; u8 value; };
static u16 stored, gSpecialVar_Result;
static unsigned trace[8], arguments[8], count;
static void record(unsigned event, unsigned argument) { assert(count<8); trace[count]=event; arguments[count++]=argument; }
static u16 VarGet(unsigned var) { record(1,var); return stored; }
static void VarSet(unsigned var,unsigned value) { record(4,var); stored=value; }
static void Script_RequestEffects(unsigned effect) { record(2,effect); }
static void Script_RequestWriteVar(unsigned var) { record(3,var); }
static u8 ScriptReadByte(struct ScriptContext *ctx) { record(5,ctx->position); ctx->position++; return ctx->value; }
'''
        code += CONSTANTS.read_text() + '\n' + '\n'.join(functions)
        code += r'''
static void expect(unsigned index,unsigned event,unsigned argument) { assert(index<count && trace[index]==event && arguments[index]==argument); }
int main(void) {
unsigned exercised=0;
for(unsigned raw=0;raw<=65535;raw++) {
 unsigned bounded=raw>DIFFICULTY_MAX?DIFFICULTY_MAX:raw;
 stored=raw; count=0; Script_IncreaseDifficulty();
 if(B_VAR_DIFFICULTY) {
  assert(count==4); expect(0,1,B_VAR_DIFFICULTY); expect(1,2,SCREFF_V1); expect(2,3,B_VAR_DIFFICULTY); expect(3,4,B_VAR_DIFFICULTY);
  assert(stored==(bounded<DIFFICULTY_MAX?bounded+1:DIFFICULTY_MAX));
 } else { assert(count==0 && stored==raw); }
 stored=raw; count=0; Script_DecreaseDifficulty();
 if(B_VAR_DIFFICULTY) {
  expect(0,1,B_VAR_DIFFICULTY);
  if(bounded) { assert(count==4); expect(1,2,SCREFF_V1); expect(2,3,B_VAR_DIFFICULTY); expect(3,4,B_VAR_DIFFICULTY); assert(stored==bounded-1); }
  else assert(count==1 && stored==raw);
 } else assert(count==0 && stored==raw);
 stored=raw; count=0; gSpecialVar_Result=65535; Script_GetDifficulty();
 expect(0,2,SCREFF_V1);
 if(B_VAR_DIFFICULTY) { assert(count==2); expect(1,1,B_VAR_DIFFICULTY); assert(gSpecialVar_Result==bounded); }
 else { assert(count==1 && gSpecialVar_Result==DIFFICULTY_NORMAL); }
 assert(stored==raw); exercised+=3;
}
for(unsigned value=0;value<256;value++) {
 struct ScriptContext ctx={9,value}; stored=54321; count=0; Script_SetDifficulty(&ctx);
 assert(ctx.position==10); expect(0,5,9); expect(1,2,SCREFF_V1); expect(2,3,B_VAR_DIFFICULTY);
 if(B_VAR_DIFFICULTY) { assert(count==4); expect(3,4,B_VAR_DIFFICULTY); assert(stored==(value>DIFFICULTY_MAX?DIFFICULTY_MAX:value)); }
 else assert(count==3 && stored==54321);
 exercised++;
}
assert(exercised==196864);
return 0;
}
'''
        compiler = shutil.which('clang') or shutil.which('cc')
        self.assertIsNotNone(compiler)
        with tempfile.TemporaryDirectory() as tmp:
            fixture = Path(tmp) / 'difficulty.c'
            executable = Path(tmp) / 'difficulty'
            fixture.write_text(code)
            for testing in (0, 1):
                for variable in (0, 0x40F8):
                    with self.subTest(testing=testing, variable=variable):
                        subprocess.run([compiler, '-std=c11', '-fsanitize=undefined', '-fno-sanitize-recover=all', f'-DTESTING={testing}', f'-DB_VAR_DIFFICULTY={variable}', str(fixture), '-o', str(executable)], check=True, capture_output=True, text=True, timeout=30)
                        subprocess.run([str(executable)], check=True, capture_output=True, text=True, timeout=30)


if __name__ == '__main__':
    unittest.main()
