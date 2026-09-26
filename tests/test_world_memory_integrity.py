"""Execute production warp/copy functions with bounded host allocation and DMA stubs.

The whole of src/overworld.c and src/menu.c is compiled on the host; the
harness stubs only other units' save block, heap, decompression, BG and task
services. These checks expose conversion and allocation bugs; emulator renders
remain separate.
"""
from pathlib import Path
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tests'))
import host_c


def run_c(units, inert=()):
    with tempfile.TemporaryDirectory(prefix='ec-world-memory-') as tmp:
        host_c.run(host_c.build(Path(tmp), units, inert=inert))


SAVE_BOUNDARY = r'''
#include "global.h"
''' + host_c.ASSERTS + r'''
static struct SaveBlock1 sSave;
struct SaveBlock1 *gSaveBlock1Ptr = &sSave;
'''

WARPS = r'''
int main(void) {
 s16 values[]={-1,0,127,128,131,255,300};
 for(unsigned i=0;i<sizeof(values)/sizeof(values[0]);i++) {
  s16 v=values[i];
  SetWarpDestination(2,3,-1,v,v); assert(sWarpDestination.x==v && sWarpDestination.y==v);
  gSaveBlock1Ptr->pos.x=gSaveBlock1Ptr->pos.y=v;SetDynamicWarp(0,2,3,-1);assert(gSaveBlock1Ptr->dynamicWarp.x==v && gSaveBlock1Ptr->dynamicWarp.y==v);
  SetDynamicWarpWithCoords(0,2,3,-1,v,v);assert(gSaveBlock1Ptr->dynamicWarp.x==v && gSaveBlock1Ptr->dynamicWarp.y==v);
  SetEscapeWarp(2,3,-1,v,v);assert(gSaveBlock1Ptr->escapeWarp.x==v && gSaveBlock1Ptr->escapeWarp.y==v);
  SetFixedDiveWarp(2,3,-1,v,v);assert(sFixedDiveWarp.x==v && sFixedDiveWarp.y==v);
  SetFixedHoleWarp(2,3,-1,v,v);assert(sFixedHoleWarp.x==v && sFixedHoleWarp.y==v);
  assert(sWarpDestination.mapGroup==2 && sWarpDestination.mapNum==3 && sWarpDestination.warpId==-1);
 }
}
'''

# heap, decompression, BG and task services of other units.
MENU_BOUNDARY = r'''
#include "global.h"
#include "bg.h"
#include "decompress.h"
#include "malloc.h"
#include "task.h"
''' + host_c.ASSERTS + r'''
#include <stdlib.h>
#include <string.h>
#include "host_menu.h"
struct HostMenu gHost;
struct Task gTasks[NUM_TASKS];
void *AllocZeroed_(u32 n, const char *location) {
 gHost.allocations++;if(gHost.failAllocation)return NULL;
 gHost.allocated=n;gHost.allocation=malloc(n);memset(gHost.allocation,0xcc,n);memset(gHost.allocation,0,n);return gHost.allocation;
}
u32 GetDecompressedDataSize(const u32 *src) {return *src;}
void DecompressDataWithHeaderWram(const u32 *src,void *dest) {memset(dest,0x5a,*src);}
u16 LoadBgTiles(u32 bg,const void *ptr,u16 size,u16 offset) {
 assert(ptr==gHost.allocation);assert(size<=gHost.allocated);assert(size<=sizeof(gHost.result));
 memcpy(gHost.result,ptr,size);gHost.copied=size;return 7;
}
u8 CreateTask(TaskFunc fn,u8 priority) {return 0;}
// Task words hold a 32-bit GBA pointer; the host sees the truncated value.
void SetWordTaskArg(u8 task,u8 index,u32 value) {assert(task==0&&index==1);gHost.taskPtr=value;gHost.taskPtrSet=1;}
'''

HOST_MENU = r'''
struct HostMenu {unsigned allocated,copied,allocations,failAllocation,taskPtrSet;void *allocation;u32 taskPtr;unsigned char result[128];};
extern struct HostMenu gHost;
'''

MENU = r'''
#include <stdlib.h>
#include "host_menu.h"
int main(void) {
 u32 payload=32;
 for(unsigned heap=0;heap<2;heap++) for(unsigned requested=0;requested<=64;requested+=16) {
  gHost.copied=0;gHost.taskPtrSet=0;sTempTileDataBufferIdx=0;
  if(heap)DecompressAndLoadBgGfxUsingHeap(2,&payload,requested,9,0);
  else assert(DecompressAndCopyTileDataToVram(2,&payload,requested,9,0)==gHost.allocation);
  unsigned expected=requested?requested:payload;assert(gHost.copied==expected);
  for(unsigned i=0;i<expected;i++) assert(gHost.result[i]==(i<payload?0x5a:0));
  assert(heap?gHost.taskPtrSet&&gHost.taskPtr==(u32)(uintptr_t)gHost.allocation&&gTasks[0].data[0]==7:sTempTileDataBuffer[0]==gHost.allocation);
  free(gHost.allocation);
 }
 gHost.failAllocation=1;gHost.copied=0;sTempTileDataBufferIdx=0;
 assert(!DecompressAndCopyTileDataToVram(2,&payload,64,0,0));assert(!gHost.copied && !sTempTileDataBufferIdx);
 gHost.taskPtrSet=0;DecompressAndLoadBgGfxUsingHeap(2,&payload,64,0,0);assert(!gHost.copied && !gHost.taskPtrSet);
 gHost.failAllocation=0;sTempTileDataBufferIdx=32;unsigned before=gHost.allocations;
 assert(!DecompressAndCopyTileDataToVram(2,&payload,64,0,0));assert(before==gHost.allocations);
}
'''

# The heap task's completion callback is only registered, never run here.
MENU_UNCALLED = ('LoadBgTilemap', 'CheckForSpaceForDma3Request', 'Free', 'GetWordTaskArg', 'DestroyTask')


class WorldMemoryTests(unittest.TestCase):
    def test_warp_coordinate_domain_and_sentinels(self):
        run_c({'overworld.c': host_c.production('src/overworld.c') + WARPS, 'save_boundary.c': SAVE_BOUNDARY})

    def test_decompressed_copies_have_initialized_storage_until_dma_finishes(self):
        with tempfile.TemporaryDirectory(prefix='ec-world-memory-') as tmp:
            path = Path(tmp)
            (path / 'host_menu.h').write_text(HOST_MENU)
            host_c.run(host_c.build(path, {
                'menu.c': host_c.production('src/menu.c') + MENU,
                'menu_boundary.c': MENU_BOUNDARY,
            }, flags=('-iquote', str(path)), inert=MENU_UNCALLED))


if __name__ == '__main__':
    unittest.main()
