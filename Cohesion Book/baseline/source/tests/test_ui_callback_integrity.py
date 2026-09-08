"""Execute extracted production UI helpers against state and ordered-call contracts.

EC_UI_SOURCE_ROOT optionally selects a pre-refactor source snapshot. This runs
identical expectations for each old entry point and its shared replacement.
"""
import os
import re
import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(os.environ.get('EC_UI_SOURCE_ROOT', Path(__file__).resolve().parents[1]))


def function(source, name):
    match = re.search(r'^(?:static )?\w+[ *]+' + name + r'\([^;]*?\)\n\{', source, re.M)
    if not match:
        return ''
    return source[match.start():source.index('\n}', match.end()) + 3] + '\n'


class UiCallbackIntegrity(unittest.TestCase):
    def run_c(self, code):
        compiler = shutil.which('clang') or shutil.which('cc')
        self.assertIsNotNone(compiler)
        with tempfile.TemporaryDirectory() as tmp:
            source = Path(tmp) / 'test.c'
            binary = Path(tmp) / 'test'
            source.write_text('#include <assert.h>\n#include <stdint.h>\n#include <stddef.h>\n#include <string.h>\n' + code)
            subprocess.run([compiler, '-std=c11', '-fsanitize=undefined', '-fno-sanitize-recover=all', str(source), '-o', str(binary)], check=True, capture_output=True, text=True, timeout=30)
            subprocess.run([str(binary)], check=True, capture_output=True, text=True, timeout=30)

    def test_pokedex_cleanup_order_and_nullable_buffers(self):
        source = (ROOT / 'src/pokedex.c').read_text()
        names = [n for n in ['FreeInfoScreenWindowAndBgBuffers', 'FreeWindowAndBgBuffers', 'FreeSearchWindowAndBgBuffers'] if function(source, n)]
        self.assertIn('FreeInfoScreenWindowAndBgBuffers', names)
        code = r'''
static int trace[16], count, buffers[4];
static unsigned mask;
static void FreeAllWindowBuffers(void) { trace[count++] = 100; }
static void *GetBgTilemapBuffer(unsigned bg) { trace[count++] = 10 + bg; return mask & (1u << bg) ? &buffers[bg] : NULL; }
static void Free(void *p) { trace[count++] = 20 + ((int *)p - buffers); }
'''
        code += ''.join(function(source, n) for n in names)
        code += 'int main(void) { void (*entries[])(void) = {' + ','.join(names) + '};\n'
        code += r'''
for (unsigned entry = 0; entry < sizeof entries / sizeof *entries; entry++)
 for (mask = 0; mask < 16; mask++) {
  count = 0; entries[entry](); int at = 0;
  assert(trace[at++] == 100);
  for (int bg = 0; bg < 4; bg++) {
   assert(trace[at++] == 10 + bg);
   if (mask & (1u << bg)) assert(trace[at++] == 20 + bg);
  }
  assert(at == count);
 }
return 0; }
'''
        self.run_c(code)

    def test_evolution_vblank_order_and_values(self):
        source = (ROOT / 'src/evolution_scene.c').read_text()
        names = [n for n in ['VBlankCB_EvolutionScene', 'VBlankCB_TradeEvolutionScene'] if function(source, n)]
        self.assertIn('VBlankCB_EvolutionScene', names)
        code = 'static int trace[16], values[16], count;\n'
        for i in range(4):
            for axis, suffix in [('X', 'HOFS'), ('Y', 'VOFS')]:
                index = 2 * i + (axis == 'Y')
                code += f'#define REG_OFFSET_BG{i}{suffix} {index}\nstatic int gBattle_BG{i}_{axis} = {101 + index};\n'
        code += 'static void SetGpuReg(int reg, int value) { trace[count] = reg; values[count++] = value; }\n'
        for i, name in enumerate(['LoadOam', 'ProcessSpriteCopyRequests', 'TransferPlttBuffer', 'ScanlineEffect_InitHBlankDmaTransfer']):
            code += f'static void {name}(void) {{ trace[count++] = {8+i}; }}\n'
        code += ''.join(function(source, n) for n in names)
        code += 'int main(void) { void (*entries[])(void) = {' + ','.join(names) + '};\n'
        code += 'for (unsigned e=0;e<sizeof entries/sizeof *entries;e++) { count=0; entries[e](); assert(count==12); for(int i=0;i<12;i++) assert(trace[i]==i); for(int i=0;i<8;i++) assert(values[i]==101+i); } return 0; }'
        self.run_c(code)

    def test_window_counts_include_every_slot(self):
        source = (ROOT / 'src/window.c').read_text()
        names = [n for n in ['GetNumActiveWindowsOnBg', 'GetNumActiveWindowsOnBg8Bit'] if function(source, n)]
        self.assertIn('GetNumActiveWindowsOnBg', names)
        code = '''typedef unsigned u32; typedef int s32;
#define WINDOWS_MAX 8
static struct { struct { unsigned bg; } window; } gWindows[WINDOWS_MAX];
'''
        code += ''.join(function(source, n) for n in names)
        code += 'int main(void) { u32 (*entries[])(u32) = {' + ','.join(names) + '};\n'
        code += r'''
for (unsigned pattern=0;pattern<65536;pattern++) {
 unsigned n=pattern, expected[5]={0};
 for(int i=0;i<WINDOWS_MAX;i++) { unsigned bg=n%4; n/=4; gWindows[i].window.bg=bg; expected[bg]++; }
 for(unsigned e=0;e<sizeof entries/sizeof *entries;e++)
  for(unsigned bg=0;bg<5;bg++) assert(entries[e](bg)==expected[bg]);
}
for(int i=0;i<WINDOWS_MAX;i++) gWindows[i].window.bg=255;
for(unsigned e=0;e<sizeof entries/sizeof *entries;e++) { assert(entries[e](255)==WINDOWS_MAX); assert(entries[e](0)==0); }
return 0; }
'''
        self.run_c(code)

    def test_water_drop_creation_and_fall_frames(self):
        source = (ROOT / 'src/intro.c').read_text()
        names = [n for n in ['SpriteCB_WaterDrop_Fall', 'SpriteCB_WaterDropShort'] if function(source, n)]
        self.assertIn('SpriteCB_WaterDrop_Fall', names)
        immediate = names[-1]
        code = r'''
typedef uint8_t u8; typedef uint16_t u16; typedef int16_t s16;
#define TRUE 1
#define SPRITE_SHAPE(x) 2
#define SPRITE_SIZE(x) 3
#define ST_OAM_AFFINE_ERASE 0
#define ST_OAM_AFFINE_DOUBLE 3
#define DROP_ANIM_RIPPLE 2
#define DROP_ANIM_REFLECTION 1
#define DROP_ANIM_LOWER_HALF 3
struct Sprite { s16 x,y,x2,y2,data[8]; int invisible; struct {int shape,size,affineMode,matrixNum;} oam; void (*callback)(struct Sprite *); };
static struct Sprite gSprites[3];
static int spriteCount, trace[32], at, sSpriteTemplate_WaterDrop;
static void SpriteCB_WaterDrop_Ripple(struct Sprite *s) { assert(0); }
static void SpriteCB_WaterDrop(struct Sprite *s) { assert(0); }
static void SpriteCB_WaterDropHalf(struct Sprite *s) { assert(0); }
static void StartSpriteAnim(struct Sprite *s, int anim) { trace[at++]=100+anim; }
static void CalcCenterToCornerVec(struct Sprite *s,int shape,int size,int mode) { assert(shape==2 && size==3 && mode==0); trace[at++]=200; }
static u8 CreateSprite(void *t,int x,int y,int priority) { assert(t==&sSpriteTemplate_WaterDrop && priority==1); int id=spriteCount++; assert(id<3); gSprites[id].x=x; gSprites[id].y=y; return id; }
static void SetOamMatrix(int id,int a,int b,int c,int d) { assert(id>=2 && id<=4); assert(a==96 && b==0 && c==0 && d==(id==4?192:96)); trace[at++]=300+id; }
'''
        code += ''.join(function(source, n) for n in names) + function(source, 'CreateWaterDrop')
        code += 'int main(void) { void (*entries[])(struct Sprite *) = {' + ','.join(names) + '};\n'
        code += r'''
for(unsigned e=0;e<sizeof entries/sizeof *entries;e++)
 for(int y=4;y<=12;y++) for(int d=0;d<8;d++) {
  struct Sprite s={.x=5,.y=y,.x2=2,.y2=3,.data={0,d,7,9,0,10,0,0},.callback=entries[e]};
  while(s.y<10) { int prev=s.y; at=0; s.callback(&s); assert(s.y==prev+4 && s.callback==entries[e]); assert(at==0 && s.data[7]==0 && s.x==5 && !s.invisible); }
  int prev=s.y; at=0; s.callback(&s);
  assert(s.y==prev+3 && s.x==7 && s.data[7]==1 && s.invisible);
  assert(s.data[2]==1024 && s.data[3]==8*(d&3));
  assert(s.callback==SpriteCB_WaterDrop_Ripple && s.oam.shape==2 && s.oam.size==3);
  assert(at==2 && trace[0]==102 && trace[1]==200);
 }
for(int immediate=0;immediate<2;immediate++) {
 memset(gSprites,0,sizeof gSprites); spriteCount=0; at=0;
 assert(CreateWaterDrop(5,4,64,2,10,immediate)==0 && spriteCount==3);
'''
        code += f'assert(gSprites[0].callback==(immediate ? {immediate} : SpriteCB_WaterDrop));\n'
        code += r'''
 assert(gSprites[0].data[2]==64 && gSprites[0].data[3]==64 && gSprites[0].data[5]==10 && gSprites[0].data[6]==64);
 for(int i=0;i<3;i++) { assert(gSprites[i].x==5 && gSprites[i].y==4); assert(gSprites[i].data[1]==2+i && gSprites[i].oam.matrixNum==2+i && gSprites[i].oam.affineMode==3); }
 for(int i=1;i<3;i++) assert(gSprites[i].callback==SpriteCB_WaterDropHalf && gSprites[i].data[7]==0);
 int expected[]={200,101,200,103,200,302,303,304};
 assert(at==8); for(int i=0;i<at;i++) assert(trace[i]==expected[i]);
}
return 0; }
'''
        self.run_c(code)


if __name__ == '__main__':
    unittest.main()
