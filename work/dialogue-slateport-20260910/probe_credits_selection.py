from pathlib import Path
import re,subprocess
out=Path('work/dialogue-slateport-20260910')
preamble='''#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef uint16_t u16;
#define NATIONAL_DEX_COUNT 1025
#define NUM_MON_SLIDES 71
#define VAR_STARTER_MON 0
#define FLAG_GET_CAUGHT 0
enum NationalDexOrder { NATIONAL_DEX_NONE = 0 };
static unsigned caught[1026],rng;
static unsigned VarGet(unsigned x){return 0;}
static unsigned GetStarterPokemon(unsigned x){return 252;}
static unsigned SpeciesToNationalPokedexNum(unsigned x){return x;}
static unsigned GetSetPokedexFlag(unsigned x,unsigned flag){return caught[x];}
static unsigned Random(void){rng=rng*1664525u+1013904223u;return rng>>16;}
'''
main='''int main(int argc,char **argv){
 int test=atoi(argv[1]); unsigned i,seen=0;
 if(test==0){caught[252]=caught[1025]=1;}
 if(test==1){for(i=1;i<=200;i++)caught[i]=1;}
 if(test==2){for(i=1;i<=1025;i++)caught[i]=1;}
 if(test==3){caught[1]=caught[4]=caught[252]=1;}
 DeterminePokemonToShow();
 for(i=0;i<NUM_MON_SLIDES;i++){
  unsigned n=sCreditsData->monToShow[i];
  if(!n||n>1025||(!caught[n]&&n!=252))return 2;
  if(n==1025)seen=1;
 }
 if(test==0&&!seen){puts("FAIL: final Pokedex entry excluded");return 3;}
 if(sCreditsData->numMonToShow!=71||sCreditsData->monToShow[70]!=252)return 4;
 puts("PASS: eligible slides and final starter");return 0;
}
'''
for variant,path in [('before',out/'credits-before-league-audit.c'),('after',Path('src/credits.c'))]:
 s=path.read_text();struct=re.search(r'struct CreditsData\n\{.*?\n\};',s,re.S)[0];fn=s[s.index('static void DeterminePokemonToShow(void)\n{'):s.index('\n#endif // !IS_FRLG',s.index('static void DeterminePokemonToShow(void)\n{'))]
 p=out/('credits-selection-'+variant+'.c');p.write_text(preamble+struct+'\nstatic struct CreditsData data,*sCreditsData=&data;\n'+fn+'\n'+main)
 binary=p.with_suffix('');subprocess.run(['cc','-std=c11','-O1','-g','-fsanitize=undefined','-fno-sanitize-recover=all',str(p),'-o',str(binary)],check=True)
 for test in range(4):
  r=subprocess.run([str(binary),str(test)],capture_output=True,text=True);print(variant,test,r.returncode,r.stdout,r.stderr,flush=True)
  if variant=='after' and r.returncode:raise SystemExit(r.returncode)
