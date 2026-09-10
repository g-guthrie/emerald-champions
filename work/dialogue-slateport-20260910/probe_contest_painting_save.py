"""Host-execute the actual painting save/load functions; not an emulator traversal."""
from pathlib import Path
import subprocess,re
root=Path(__file__).resolve().parents[2]
out=root/'work/dialogue-slateport-20260910'
contest=(root/'src/contest.c').read_text()
painting=(root/'src/contest_painting.c').read_text()
def function(source,signature):
    start=source.index(signature+'\n{')
    return source[start:source.index('\n}',start)+2]+'\n'
struct=re.search(r'struct ContestWinner\n\{.*?\n\};',(root/'include/global.h').read_text(),re.S)[0]
prefix='''#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
typedef uint8_t u8; typedef uint32_t u32; typedef int32_t s32; typedef uint8_t bool8;
#define TRUE 1
#define FALSE 0
#include "constants/global.h"
#include "constants/contest.h"
enum Species { SPECIES_NONE };
'''+struct+'''
struct Save {struct ContestWinner contestWinners[NUM_CONTEST_WINNERS];} save, *gSaveBlock1Ptr=&save;
struct Entrant {u32 personality, otId; enum Species species; u8 isShiny; u8 nickname[11],trainerName[8];} gContestMons[4];
struct ContestWinner gCurContestWinner;
u8 gContestFinalStandings[4]={1,2,3,0},gContestPlayerMonIndex=3;
u8 gSpecialVar_ContestCategory,gSpecialVar_ContestRank=CONTEST_RANK_MASTER,gLinkContestFlags;
u8 gCurContestWinnerSaveIdx,gCurContestWinnerIsForArtist;
u8 GetContestWinnerSaveIdx(u8,bool8);
unsigned Random(void) {return 1;}
void StringCopyN(u8 *d,const u8 *s,unsigned n) {memcpy(d,s,n);}
void StringCopy(u8 *d,const u8 *s) {do {*d++=*s;} while (*s++!=255);}
'''
body=function(contest,'bool8 SaveContestWinner(u8 rank)')+function(contest,'u8 GetContestWinnerSaveIdx(u8 rank, bool8 shift)')+function(painting,'void SetContestWinnerForPainting(int contestWinnerId)')
tail='''int main(void) {
 memset(gContestMons[3].nickname,255,11); memset(gContestMons[3].trainerName,255,8);
 gContestMons[3].species=25; gContestMons[3].isShiny=1;
 assert(SaveContestWinner(CONTEST_SAVE_FOR_MUSEUM));
 SetContestWinnerForPainting(CONTEST_WINNER_MUSEUM_COOL);
 assert(gCurContestWinner.isShiny && gCurContestWinner.species==25);
 assert(SaveContestWinner(CONTEST_RANK_MASTER));
 SetContestWinnerForPainting(CONTEST_WINNER_HALL_1); assert(gCurContestWinner.isShiny);
 gContestMons[3].isShiny=0;
 assert(SaveContestWinner(CONTEST_SAVE_FOR_MUSEUM));
 SetContestWinnerForPainting(CONTEST_WINNER_MUSEUM_COOL); assert(!gCurContestWinner.isShiny);
 assert(SaveContestWinner(CONTEST_RANK_MASTER));
 SetContestWinnerForPainting(CONTEST_WINNER_HALL_1); assert(!gCurContestWinner.isShiny);
 SetContestWinnerForPainting(CONTEST_WINNER_HALL_2); assert(gCurContestWinner.isShiny);
 puts("PASS: shiny museum/hall save+load, ordinary replacement, prior hall record retained");
}
'''
cpath=out/'contest-painting-save-probe.c';binary=out/'contest-painting-save-probe'
cpath.write_text(prefix+body+tail)
subprocess.run(['cc','-Wall','-Wextra','-I',str(root/'include'),'-o',str(binary),str(cpath)],check=True)
subprocess.run([str(binary)],check=True)
