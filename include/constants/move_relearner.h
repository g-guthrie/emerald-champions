#ifndef GUARD_CONSTANTS_MOVE_RELEARNER_H
#define GUARD_CONSTANTS_MOVE_RELEARNER_H

#include "constants/moves.h"

// Max number of moves shown by the move relearner. The Center tutor can offer
// every legal move, so this must cover the whole move list.
#define MAX_RELEARNER_MOVES MOVES_COUNT_ALL

// Move Relearner menu change constants
enum MoveRelearnerStates
{
    MOVE_RELEARNER_LEVEL_UP_MOVES,
    MOVE_RELEARNER_EGG_MOVES,
    MOVE_RELEARNER_ALL_MOVES,
    MOVE_RELEARNER_COUNT,
};

enum RelearnMode
{
    RELEARN_MODE_NONE = 0,
    RELEARN_MODE_SCRIPT = 1,                     // Relearning moves through an event script
    // Must stay 2, it is tied to the summary screen pages
    RELEARN_MODE_PSS_PAGE_BATTLE_MOVES = 2,      // Relearning moves through the summary screen's battle moves page
};

#endif // GUARD_CONSTANTS_MOVE_RELEARNER_H
