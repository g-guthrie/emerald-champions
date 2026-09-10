#include <stdint.h>
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
struct CreditsData
{
    u16 monToShow[NUM_MON_SLIDES]; // List of Pokémon species ids that will show during the credits
    u16 imgCounter; //how many mon images have been shown
    u16 nextImgPos; //if the next image spawns left/center/right
    u16 currShownMon; //index into monToShow
    u16 numMonToShow; //number of Pokémon to show, always NUM_MON_SLIDES after determine function
    u16 caughtMonIds[NATIONAL_DEX_COUNT]; //temporary location to hold a condensed array of all caught Pokémon
    u16 numCaughtMon; //count of filled spaces in caughtMonIds
    u16 unused[7];
};
static struct CreditsData data,*sCreditsData=&data;
static void DeterminePokemonToShow(void)
{
    enum NationalDexOrder starter = SpeciesToNationalPokedexNum(GetStarterPokemon(VarGet(VAR_STARTER_MON)));
    u16 page;
    u16 dexNum;
    u16 j;

    // Go through the Pokédex, and anything that has gotten caught we put into our massive array.
    // This basically packs all of the caught Pokémon into the front of the array
    for (dexNum = 1, j = 0; dexNum <= NATIONAL_DEX_COUNT; dexNum++)
    {
        if (GetSetPokedexFlag(dexNum, FLAG_GET_CAUGHT))
        {
            sCreditsData->caughtMonIds[j] = dexNum;
            j++;
        }
    }

    // Fill the rest of the array with zeroes
    for (dexNum = j; dexNum < NATIONAL_DEX_COUNT; dexNum++)
        sCreditsData->caughtMonIds[dexNum] = NATIONAL_DEX_NONE;

    // Cap the number of Pokémon we care about to NUM_MON_SLIDES, the max we show in the credits scene (-1 for the starter)
    sCreditsData->numCaughtMon = j;
    if (sCreditsData->numCaughtMon < NUM_MON_SLIDES)
        sCreditsData->numMonToShow = j;
    else
        sCreditsData->numMonToShow = NUM_MON_SLIDES;

    // Loop through our list of caught Pokémon and select randomly from it to fill the images to show
    j = 0;
    do
    {
        // Select a random mon, insert into array
        page = Random() % sCreditsData->numCaughtMon;
        sCreditsData->monToShow[j] = sCreditsData->caughtMonIds[page];

        // Remove the select mon from the array, and condense array entries
        j++;
        sCreditsData->caughtMonIds[page] = 0;
        sCreditsData->numCaughtMon--;
        if (page != sCreditsData->numCaughtMon)
        {
            // Instead of looping through and moving everything down, just take from the end. Order doesn't matter after all.
            sCreditsData->caughtMonIds[page] = sCreditsData->caughtMonIds[sCreditsData->numCaughtMon];
            sCreditsData->caughtMonIds[sCreditsData->numCaughtMon] = 0;
        }
    }
    while (sCreditsData->numCaughtMon != 0 && j < NUM_MON_SLIDES);

    // If we don't have enough Pokémon in the dex to fill everything, copy the selected mon into the end of the array, so it loops
    if (sCreditsData->numMonToShow < NUM_MON_SLIDES)
    {
        for (j = sCreditsData->numMonToShow, page = 0; j < NUM_MON_SLIDES; j++)
        {
            sCreditsData->monToShow[j] = sCreditsData->monToShow[page];

            page++;
            if (page == sCreditsData->numMonToShow)
                page = 0;
        }
        // Ensure the last Pokémon is our starter
        sCreditsData->monToShow[NUM_MON_SLIDES - 1] = starter;
    }
    else
    {
        // Check to see if our starter has already appeared in this list, break if it has
        for (dexNum = 0; dexNum < NUM_MON_SLIDES && sCreditsData->monToShow[dexNum] != starter; dexNum++);

        // If it has, swap it with the last Pokémon, to ensure our starter is the last image
        if (dexNum < sCreditsData->numMonToShow - 1)
        {
            sCreditsData->monToShow[dexNum] = sCreditsData->monToShow[NUM_MON_SLIDES-1];
            sCreditsData->monToShow[NUM_MON_SLIDES - 1] = starter;
        }
        else
        {
            // Ensure the last Pokémon is our starter
            sCreditsData->monToShow[NUM_MON_SLIDES - 1] = starter;
        }
    }
    sCreditsData->numMonToShow = NUM_MON_SLIDES;
}

int main(int argc,char **argv){
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
