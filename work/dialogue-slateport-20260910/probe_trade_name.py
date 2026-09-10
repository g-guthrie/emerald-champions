from pathlib import Path
import subprocess
root=Path.cwd();out=root/'work/dialogue-slateport-20260910';s=(root/'src/trade.c').read_text();start=s.index('static void BufferInGameTradeMonName(void)\n{');end=s.index('\nstatic void CreateInGameTradePokemonInternal',start);fn=s[start:end]
pre=fn.replace('''    struct Pokemon *mon = gSpecialVar_0x8004 == PC_MON_CHOSEN
        ? &gParties[B_TRAINER_OPPONENT_A][TRADEMON_FROM_PC]
        : &gParties[B_TRAINER_PLAYER][gSpecialVar_0x8004];
    GetMonData(mon, MON_DATA_NICKNAME, nickname);''','''    GetMonData(&gParties[B_TRAINER_PLAYER][gSpecialVar_0x8005], MON_DATA_NICKNAME, nickname);''')
assert pre!=fn
prefix=r'''
#include <stdio.h>
#include <string.h>
typedef unsigned char u8;
#define max(a,b) ((a)>(b)?(a):(b))
#define POKEMON_NAME_BUFFER_SIZE 13
#define B_TRAINER_PLAYER 0
#define B_TRAINER_OPPONENT_A 1
#define PC_MON_CHOSEN 254
#define TRADEMON_FROM_PC 1
#define MON_DATA_NICKNAME 2
struct Pokemon { int sentinel; } gParties[2][6];
struct InGameTrade { int species; } sIngameTrades[15];
int gSpecialVar_0x8004,gSpecialVar_0x8005;
u8 gStringVar1[32],gStringVar2[32];
struct Pokemon *expected;
int wrong;
static void GetMonData(struct Pokemon *p,int field,u8 *out) { if (p!=expected) {wrong++; strcpy((char*)out,"WRONG");} else strcpy((char*)out,"RECEIVED"); }
static void StringCopy_Nickname(u8 *a,u8 *b) {strcpy((char*)a,(char*)b);}
static void StringCopy(u8 *a,const u8 *b) {strcpy((char*)a,(const char*)b);}
static const u8 *GetSpeciesName(int species) {return (const u8*)"CYCLIZAR";}
'''
suffix=r'''
int main(void) {
 int slots[]={4,PC_MON_CHOSEN,0}, trades[]={2,2,13};
 struct Pokemon *targets[]={&gParties[0][4],&gParties[1][1],&gParties[0][0]};
 for(int i=0;i<3;i++) {gSpecialVar_0x8004=slots[i];gSpecialVar_0x8005=trades[i];expected=targets[i];BufferInGameTradeMonName();printf("case%d nickname=%s species=%s\n",i,gStringVar1,gStringVar2);}
 printf("wrong Pokémon reads=%d\n",wrong); return wrong?1:0;
}
'''
for label,body in [('before',pre),('after',fn)]:
 source=out/('trade-name-'+label+'.c');binary=out/('trade-name-'+label);source.write_text(prefix+body+suffix)
 subprocess.run(['cc','-std=c99','-O0',str(source),'-o',str(binary)],check=True)
 r=subprocess.run([str(binary)],text=True,capture_output=True);print(label,r.returncode,r.stdout,flush=True)
 assert r.returncode==(1 if label=='before' else 0)
