#ifndef GUARD_MOVE_RELEARNER_H
#define GUARD_MOVE_RELEARNER_H

#include "constants/move_relearner.h"

void TeachMoveRelearnerMove(void);
void MoveRelearnerShowHideHearts(s32 move);
void MoveRelearnerShowHideCategoryIcon(s32);
void CB2_InitLearnMove(void);
bool32 CanBoxMonRelearnMoves(struct BoxPokemon *boxMon, enum MoveRelearnerStates state);
bool32 HasMoveToRelearn(struct BoxPokemon *boxMon, enum MoveRelearnerStates state);
const u16 *GetEmeraldChampionsPreparationMoves(enum Species species);
// Existing preparation pool plus both preset formats, excluding known moves.
// Pass NULL to count without writing a list.
u32 GetEmeraldChampionsPreparationMovesToLearn(struct BoxPokemon *mon, u16 *moves);

extern enum MoveRelearnerStates gMoveRelearnerState;
extern enum RelearnMode gRelearnMode;

bool32 CanSpeciesUseEmeraldChampionsPreparationMove(enum Species species, enum Move move);

#endif //GUARD_MOVE_RELEARNER_H
