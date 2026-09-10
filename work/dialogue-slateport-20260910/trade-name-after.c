
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
static void BufferInGameTradeMonName(void)
{
    u8 nickname[max(32, POKEMON_NAME_BUFFER_SIZE)];
    const struct InGameTrade *inGameTrade = &sIngameTrades[gSpecialVar_0x8005];
    struct Pokemon *mon = gSpecialVar_0x8004 == PC_MON_CHOSEN
        ? &gParties[B_TRAINER_OPPONENT_A][TRADEMON_FROM_PC]
        : &gParties[B_TRAINER_PLAYER][gSpecialVar_0x8004];
    GetMonData(mon, MON_DATA_NICKNAME, nickname);
    StringCopy_Nickname(gStringVar1, nickname);
    StringCopy(gStringVar2, GetSpeciesName(inGameTrade->species));
}

int main(void) {
 int slots[]={4,PC_MON_CHOSEN,0}, trades[]={2,2,13};
 struct Pokemon *targets[]={&gParties[0][4],&gParties[1][1],&gParties[0][0]};
 for(int i=0;i<3;i++) {gSpecialVar_0x8004=slots[i];gSpecialVar_0x8005=trades[i];expected=targets[i];BufferInGameTradeMonName();printf("case%d nickname=%s species=%s\n",i,gStringVar1,gStringVar2);}
 printf("wrong Pokémon reads=%d\n",wrong); return wrong?1:0;
}
