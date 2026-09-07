"""Production sprite fills: full buffers, guards, and source lookup traces.

The recorded digest is the pre-refactor active-mode output. Original/current
raw streams were also compared byte-for-byte before setting this baseline.
Host VRAM and source pointers avoid truncating real 64-bit host addresses.
"""
import hashlib
import subprocess
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXPECTED_STREAM_SHA256 = '690146be9164f6b391c423b4993bcd04432229069b347132f24017e05e149440'


def fill_harness(source):
    region = source[source.index('#define nextX data[1]'):source.index('static void StorePointerInSpriteData(')]
    code = r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef uint32_t u32;
typedef int bool32;
#define TRUE 1
#define FALSE 0
#define SPRITE_NONE 255
#define TILE_SIZE_4BPP 32
#define GUARD 16
#define STRIDE 528
#define VRAM_WORDS (GUARD + 9 * STRIDE + GUARD)
static u32 vram[VRAM_WORDS], expected[VRAM_WORDS], sourceWords[9][512];
#define OBJ_VRAM0 ((uintptr_t)vram)
struct Sprite {struct {u32 tileNum;} oam;u32 data[8],width,height;};
static struct Sprite gSprites[9], savedSprites[9];
static u32 trace[512], traceCount, gridCount, sourceMode;
static void Record(u32 value){assert(traceCount<512);trace[traceCount++]=value;}
static u32 GetSpriteWidth(struct Sprite *s){assert(s>=gSprites && s<gSprites+gridCount);Record(0x100+(s-gSprites));return s->width;}
static u32 GetSpriteHeight(struct Sprite *s){assert(s>=gSprites && s<gSprites+gridCount);Record(0x200+(s-gSprites));return s->height;}
static u32 *GetSrcPtrFromSprite(struct Sprite *s){assert(sourceMode && s>=gSprites && s<gSprites+gridCount);Record(0x300+(s-gSprites));return sourceWords[s-gSprites];}
'''
    code += region
    code += r'''
static void Emit(const void *data,unsigned size){assert(fwrite(data,1,size,stdout)==size);}
int main(void){
 const unsigned layouts[][4]={{16,16,1,1},{16,16,2,2},{32,32,3,3},{64,32,2,1},{64,64,3,3}};
 unsigned scenarios=0;
 for(unsigned layout=0;layout<5;layout++){
  unsigned w=layouts[layout][0],h=layouts[layout][1],columns=layouts[layout][2],rows=layouts[layout][3];gridCount=columns*rows;
  memset(gSprites,0,sizeof gSprites);
  for(unsigned id=0;id<gridCount;id++){
   gSprites[id].width=w;gSprites[id].height=h;gSprites[id].oam.tileNum=(GUARD+id*STRIDE)/8;
   gSprites[id].nextX=id%columns+1<columns?id+1:SPRITE_NONE;
   gSprites[id].nextY=id/columns+1<rows?id+columns:SPRITE_NONE;
   for(unsigned i=0;i<512;i++)sourceWords[id][i]=0x01234567u^(id*0x11111111u)^(i*2654435761u);
  }
  memcpy(savedSprites,gSprites,sizeof gSprites);
  const unsigned rectangles[][4]={
   {0,0,0,0},{3,5,0,7},{3,5,7,0},{0,0,8,8},{0,0,w,h},
   {0,0,5,8},{3,1,9,7},{2,2,3,4},{w-5,2,13,7},{2,h-5,13,13},
   {w-5,h-5,w+11,h+11},{1,h-3,2*w+9,h+7},{3,5,columns*w+19,rows*h+17},
   {8,5,24,11},{0,5,24,11},{40,8,56,8},{32,16,32,8},{16,5,55,11},{8,5,55,11},{55,19,40,12},
   {0,0,80,16},{w-3,0,w+7,0}
  };
  for(unsigned rect=0;rect<sizeof rectangles/sizeof *rectangles;rect++){
   unsigned left=rectangles[rect][0],top=rectangles[rect][1],width=rectangles[rect][2],height=rectangles[rect][3];
   if(left>=w || top>=h || (left+width>w && columns==1) || (top+height>h && rows==1))continue;
   for(unsigned mode=0;mode<17;mode++){
    sourceMode=mode==16;traceCount=0;
    for(unsigned i=0;i<VRAM_WORDS;i++)vram[i]=0xa5a50000u^i;
    memcpy(expected,vram,sizeof vram);
    if(sourceMode)FillSpriteRectSprite(0,left,top,width,height);else FillSpriteRectColor(0,left,top,width,height,mode);
    assert(trace[0]==0x100 && trace[1]==0x200);
    if(sourceMode){assert(traceCount>=3 && trace[2]==0x300);}else assert(traceCount==2);
    assert(!memcmp(savedSprites,gSprites,sizeof gSprites));
    for(unsigned i=0;i<GUARD;i++)assert(vram[i]==expected[i]);
    for(unsigned id=0;id<9;id++)for(unsigned i=id<gridCount?w*h/8:0;i<STRIDE;i++)assert(vram[GUARD+id*STRIDE+i]==expected[GUARD+id*STRIDE+i]);
    for(unsigned i=GUARD+9*STRIDE;i<VRAM_WORDS;i++)assert(vram[i]==expected[i]);
    if(width==0 || height==0)assert(!memcmp(vram,expected,sizeof vram));
    // Independently verify every affected nibble and untouched bit for
    // single-sprite cases; spanning cases are pinned by the full raw stream.
    if(left+width<=w && top+height<=h){
     for(unsigned y=top;y<top+height;y++)for(unsigned x=left;x<left+width;x++){
      unsigned index=(y/8)*w+(x/8)*8+y%8,shift=(x%8)*4;
      unsigned nibble=sourceMode?(sourceWords[0][index]>>shift)&15:mode;
      expected[GUARD+index]=(expected[GUARD+index]&~(15u<<shift))|(nibble<<shift);
     }
     assert(!memcmp(vram,expected,sizeof vram));
    }
    unsigned identity[]={layout,rect,mode,traceCount};Emit(identity,sizeof identity);Emit(trace,traceCount*sizeof *trace);Emit(vram,sizeof vram);Emit(gSprites,sizeof gSprites);scenarios++;
   }
  }
 }
 Emit(&scenarios,sizeof scenarios);return 0;
}
'''
    return code


def run_harness(source):
    with tempfile.TemporaryDirectory() as temp:
        root = Path(temp)
        (root / 'test.c').write_text(fill_harness(source))
        subprocess.run(['cc', '-O1', '-std=c11', '-fsanitize=undefined', '-fno-sanitize-recover=undefined', str(root / 'test.c'), '-o', str(root / 'test')], check=True, timeout=30)
        return subprocess.check_output([str(root / 'test')], timeout=30)


class SpriteFillIntegrity(unittest.TestCase):
    def test_active_modes_preserve_full_buffers_and_lookup_trace(self):
        result = run_harness((ROOT / 'src/sprite.c').read_text())
        self.assertEqual(hashlib.sha256(result).hexdigest(), EXPECTED_STREAM_SHA256)


if __name__ == '__main__':
    unittest.main()
