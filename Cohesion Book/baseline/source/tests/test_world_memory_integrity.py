"""Execute production warp/copy functions with bounded host allocation and DMA stubs.

These checks expose conversion and allocation bugs; emulator renders remain separate.
"""
from pathlib import Path
import re
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


def function(source, name):
    match = re.search(r"^[\w *]+\b" + name + r"\([^;]*?\)\n\{", source, re.M)
    if not match:
        raise AssertionError(name)
    end = source.index('{', match.start())
    depth = 1
    while depth:
        end += 1
        depth += (source[end] == '{') - (source[end] == '}')
    return source[match.start():end + 1] + '\n'


def run_c(source):
    with tempfile.TemporaryDirectory(prefix='ec-world-memory-') as tmp:
        path = Path(tmp) / 'probe.c'
        path.write_text(source)
        binary = Path(tmp) / 'probe'
        subprocess.run(['cc', '-std=c99', '-Werror=implicit-function-declaration', str(path), '-o', str(binary)], check=True, capture_output=True)
        result = subprocess.run([str(binary)], capture_output=True, text=True)
        if result.returncode:
            raise AssertionError(result.stderr)


class WorldMemoryTests(unittest.TestCase):
    def test_warp_coordinate_domain_and_sentinels(self):
        source = (ROOT / 'src/overworld.c').read_text()
        names = ['SetWarpData', 'SetWarpDestination', 'SetDynamicWarp', 'SetDynamicWarpWithCoords', 'SetEscapeWarp', 'SetFixedDiveWarp', 'SetFixedHoleWarp']
        harness = '''
#include <assert.h>
#include <stdint.h>
typedef int8_t s8; typedef int16_t s16; typedef int32_t s32;
struct WarpData { s8 mapGroup, mapNum, warpId; s16 x,y; };
static struct WarpData sWarpDestination,sFixedDiveWarp,sFixedHoleWarp;
static struct Save {struct WarpData dynamicWarp,escapeWarp; struct {s16 x,y;} pos;} save;
static struct Save *gSaveBlock1Ptr=&save;
'''
        harness += ''.join(function(source, name) for name in names)
        harness += '''
int main(void) {
 s16 values[]={-1,0,127,128,131,255,300};
 for(unsigned i=0;i<sizeof(values)/sizeof(values[0]);i++) {
  s16 v=values[i];
  SetWarpDestination(2,3,-1,v,v); assert(sWarpDestination.x==v && sWarpDestination.y==v);
  save.pos.x=save.pos.y=v;SetDynamicWarp(0,2,3,-1);assert(save.dynamicWarp.x==v && save.dynamicWarp.y==v);
  SetDynamicWarpWithCoords(0,2,3,-1,v,v);assert(save.dynamicWarp.x==v && save.dynamicWarp.y==v);
  SetEscapeWarp(2,3,-1,v,v);assert(save.escapeWarp.x==v && save.escapeWarp.y==v);
  SetFixedDiveWarp(2,3,-1,v,v);assert(sFixedDiveWarp.x==v && sFixedDiveWarp.y==v);
  SetFixedHoleWarp(2,3,-1,v,v);assert(sFixedHoleWarp.x==v && sFixedHoleWarp.y==v);
  assert(sWarpDestination.mapGroup==2 && sWarpDestination.mapNum==3 && sWarpDestination.warpId==-1);
 }
}
'''
        run_c(harness)

    def test_decompressed_copies_have_initialized_storage_until_dma_finishes(self):
        source = (ROOT / 'src/menu.c').read_text()
        harness = '''
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
typedef uint8_t u8;typedef uint16_t u16;typedef uintptr_t u32;
#define ARRAY_COUNT(a) (sizeof(a)/sizeof((a)[0]))
#define MAX(a,b) ((a)>(b)?(a):(b))
static void *sTempTileDataBuffer[32];static u16 sTempTileDataBufferIdx;
static struct Task {int data[16];} gTasks[1];
static unsigned allocated,copied,allocations,failAllocation;static void *allocation,*taskPtr;
static unsigned char result[128];
static void *Alloc(u32 n) {allocations++;if(failAllocation)return NULL;allocated=n;allocation=malloc(n);memset(allocation,0xcc,n);return allocation;}
static void *AllocZeroed(u32 n) {void *p=Alloc(n);if(p)memset(p,0,n);return p;}
static u32 GetDecompressedDataSize(const void *src) {return *(const u32*)src;}
static void DecompressDataWithHeaderWram(const void *src,void *dest) {memset(dest,0x5a,*(const u32*)src);}
static u16 copy_decompressed_tile_data_to_vram(u8 bg,const void *ptr,u16 size,u16 offset,u8 mode) {
 assert(ptr==allocation);assert(size<=allocated);assert(size<=sizeof(result));
 memcpy(result,ptr,size);copied=size;return 7;
}
static void task_free_buf_after_copying_tile_data_to_vram(u8 task) {}
static u8 CreateTask(void (*fn)(u8),u8 priority) {return 0;}
static void SetWordTaskArg(u8 task,u8 index,uintptr_t ptr) {taskPtr=(void*)ptr;}
'''
        harness += function(source, 'malloc_and_decompress')
        if 'static void *AllocDecompressedTileData(' in source:
            harness += function(source, 'AllocDecompressedTileData')
        harness += function(source, 'DecompressAndCopyTileDataToVram')
        harness += function(source, 'DecompressAndLoadBgGfxUsingHeap')
        harness += '''
int main(void) {
 u32 payload=32;
 for(unsigned heap=0;heap<2;heap++) for(unsigned requested=0;requested<=64;requested+=16) {
  copied=0;taskPtr=NULL;sTempTileDataBufferIdx=0;
  if(heap)DecompressAndLoadBgGfxUsingHeap(2,&payload,requested,9,0);
  else assert(DecompressAndCopyTileDataToVram(2,&payload,requested,9,0)==allocation);
  unsigned expected=requested?requested:payload;assert(copied==expected);
  for(unsigned i=0;i<expected;i++) assert(result[i]==(i<payload?0x5a:0));
  assert(heap?taskPtr==allocation:sTempTileDataBuffer[0]==allocation);
  free(allocation);
 }
 failAllocation=1;copied=0;sTempTileDataBufferIdx=0;
 assert(!DecompressAndCopyTileDataToVram(2,&payload,64,0,0));assert(!copied && !sTempTileDataBufferIdx);
 taskPtr=NULL;DecompressAndLoadBgGfxUsingHeap(2,&payload,64,0,0);assert(!copied && !taskPtr);
 failAllocation=0;sTempTileDataBufferIdx=32;unsigned before=allocations;
 assert(!DecompressAndCopyTileDataToVram(2,&payload,64,0,0));assert(before==allocations);
}
'''
        run_c(harness)


if __name__ == '__main__':
    unittest.main()
