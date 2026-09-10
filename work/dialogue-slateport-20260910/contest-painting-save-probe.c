#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
typedef uint8_t u8; typedef uint32_t u32; typedef int32_t s32; typedef uint8_t bool8;
#define TRUE 1
#define FALSE 0
#include "constants/global.h"
#include "constants/contest.h"
enum Species { SPECIES_NONE };
struct ContestWinner
{
    u32 personality;
    u32 trainerId;
    enum Species species;
    u8 contestCategory;
    u8 monName[VANILLA_POKEMON_NAME_LENGTH + 1];
    u8 trainerName[PLAYER_NAME_LENGTH + 1];
    u8 contestRank:7;
    bool8 isShiny:1;
    //u8 padding;
};
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
bool8 SaveContestWinner(u8 rank)
{
    s32 i;
    u8 captionId = Random() % NUM_PAINTING_CAPTIONS;

    // Get the index of the winner among the contestants
    for (i = 0; i < CONTESTANT_COUNT - 1; i++)
        if (gContestFinalStandings[i] == 0)
            break;

    // Exit if attempting to save a Pokémon other than the player's to the museum
    if (rank == CONTEST_SAVE_FOR_MUSEUM && i != gContestPlayerMonIndex)
        return FALSE;

    // Adjust the random painting caption depending on the category
    captionId += NUM_PAINTING_CAPTIONS * gSpecialVar_ContestCategory;

    if (rank != CONTEST_SAVE_FOR_ARTIST)
    {
        // Save winner in the saveblock
        // Used to save any winner for the Contest Hall or the Museum
        // but excludes the temporary save used by the artist
        u8 id = GetContestWinnerSaveIdx(rank, TRUE);
        gSaveBlock1Ptr->contestWinners[id].personality = gContestMons[i].personality;
        gSaveBlock1Ptr->contestWinners[id].isShiny = gContestMons[i].isShiny;
        gSaveBlock1Ptr->contestWinners[id].species = gContestMons[i].species;
        gSaveBlock1Ptr->contestWinners[id].trainerId = gContestMons[i].otId;
        StringCopyN(gSaveBlock1Ptr->contestWinners[id].monName, gContestMons[i].nickname, VANILLA_POKEMON_NAME_LENGTH);
        StringCopy(gSaveBlock1Ptr->contestWinners[id].trainerName, gContestMons[i].trainerName);
        if (gLinkContestFlags & LINK_CONTEST_FLAG_IS_LINK)
            gSaveBlock1Ptr->contestWinners[id].contestRank = CONTEST_RANK_LINK;
        else
            gSaveBlock1Ptr->contestWinners[id].contestRank = gSpecialVar_ContestRank;

        if (rank != CONTEST_SAVE_FOR_MUSEUM)
            gSaveBlock1Ptr->contestWinners[id].contestCategory = gSpecialVar_ContestCategory;
        else
            gSaveBlock1Ptr->contestWinners[id].contestCategory = captionId;
    }
    else
    {
        // Set the most recent winner so the artist can show the player their painting
        gCurContestWinner.personality = gContestMons[i].personality;
        gCurContestWinner.isShiny = gContestMons[i].isShiny;
        gCurContestWinner.trainerId = gContestMons[i].otId;
        gCurContestWinner.species = gContestMons[i].species;
        StringCopyN(gCurContestWinner.monName, gContestMons[i].nickname, VANILLA_POKEMON_NAME_LENGTH);
        StringCopy(gCurContestWinner.trainerName, gContestMons[i].trainerName);
        gCurContestWinner.contestCategory = captionId;
    }
    return TRUE;
}
u8 GetContestWinnerSaveIdx(u8 rank, bool8 shift)
{
    s32 i;

    switch (rank)
    {
    case CONTEST_RANK_NORMAL:
    case CONTEST_RANK_SUPER:
    case CONTEST_RANK_HYPER:
    case CONTEST_RANK_MASTER:
        if (shift)
        {
            for (i = NUM_CONTEST_HALL_WINNERS - 1; i > 0; i--)
                memcpy(&gSaveBlock1Ptr->contestWinners[i], &gSaveBlock1Ptr->contestWinners[i - 1], sizeof(struct ContestWinner));
        }
        return CONTEST_WINNER_HALL_1 - 1;
    default:
//  case CONTEST_SAVE_FOR_MUSEUM:
//  case CONTEST_SAVE_FOR_ARTIST:
        return MUSEUM_CONTEST_WINNERS_START + gSpecialVar_ContestCategory;
    }
}
void SetContestWinnerForPainting(int contestWinnerId)
{
    u8 *saveIdx = &gCurContestWinnerSaveIdx;
    u8 *isForArtist = &gCurContestWinnerIsForArtist;
    gCurContestWinner = gSaveBlock1Ptr->contestWinners[contestWinnerId - 1];
    *saveIdx = contestWinnerId - 1;
    *isForArtist = FALSE;
}
int main(void) {
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
