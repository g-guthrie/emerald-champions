#include <stdint.h>
#include <assert.h>
#include <stdio.h>
typedef int16_t s16; typedef uint16_t u16;
#define NUM_POKEBLOCK_FEEDERS 10
struct Feeder {s16 x,y; int mapNum,stepCounter;} sPokeblockFeeders[10];
struct Save {struct {int mapNum;} location;} save, *gSaveBlock1Ptr=&save;
u16 gSpecialVar_Result;
void PlayerGetDestCoords(s16 *x,s16 *y) {*x=20; *y=20;}
void GetPokeblockFeederWithinRange(void)
{
    s16 x, y;
    u16 i;

    PlayerGetDestCoords(&x, &y);

    for (i = 0; i < NUM_POKEBLOCK_FEEDERS; i++)
    {
        if (sPokeblockFeeders[i].stepCounter != 0
         && gSaveBlock1Ptr->location.mapNum == sPokeblockFeeders[i].mapNum)
        {
            // Each feeder is measured from the original player position.
            s16 dx = x - sPokeblockFeeders[i].x;
            s16 dy = y - sPokeblockFeeders[i].y;
            if (dx < 0)
                dx *= -1;
            if (dy < 0)
                dy *= -1;
            if ((dx + dy) <= 5)
            {
                gSpecialVar_Result = i;
                return;
            }
        }
    }

    gSpecialVar_Result = -1;
}

int main(void) {
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
