from pathlib import Path
import subprocess
p=Path('src/safari_zone.c');s=p.read_text();body=s[s.index('void GetPokeblockFeederWithinRange(void)'):s.index('struct Pokeblock *SafariZoneGetActivePokeblock(void)')]
prefix='''#include <stdint.h>
#include <assert.h>
#include <stdio.h>
typedef int16_t s16; typedef uint16_t u16;
#define NUM_POKEBLOCK_FEEDERS 10
struct Feeder {s16 x,y; int mapNum,stepCounter;} sPokeblockFeeders[10];
struct Save {struct {int mapNum;} location;} save, *gSaveBlock1Ptr=&save;
u16 gSpecialVar_Result;
void PlayerGetDestCoords(s16 *x,s16 *y) {*x=20; *y=20;}
'''
tail='''int main(void) {
 save.location.mapNum=7;
 sPokeblockFeeders[0]=(struct Feeder){10,10,7,100};
 sPokeblockFeeders[1]=(struct Feeder){19,20,7,100};
 GetPokeblockFeederWithinRange(); assert(gSpecialVar_Result==1);
 sPokeblockFeeders[1]=(struct Feeder){10,10,7,100};
 GetPokeblockFeederWithinRange(); assert(gSpecialVar_Result==65535);
 sPokeblockFeeders[1]=(struct Feeder){20,20,7,0};
 GetPokeblockFeederWithinRange(); assert(gSpecialVar_Result==65535);
 sPokeblockFeeders[1]=(struct Feeder){23,22,7,100};
 GetPokeblockFeederWithinRange(); assert(gSpecialVar_Result==1);
 sPokeblockFeeders[1].x=24;
 GetPokeblockFeederWithinRange(); assert(gSpecialVar_Result==65535);
 puts("PASS: second feeder, distant feeders, expired feeder, radius five/six");
}
'''
out=Path('work/dialogue-slateport-20260910');(out/'feeder-source-probe.c').write_text(prefix+body+tail)
subprocess.run(['cc','-Wall','-Wextra','-o',str(out/'feeder-source-probe'),str(out/'feeder-source-probe.c')],check=True)
subprocess.run([str(out/'feeder-source-probe')],check=True)
