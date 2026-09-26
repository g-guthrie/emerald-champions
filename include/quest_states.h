#ifndef GUARD_QUEST_STATES_H
#define GUARD_QUEST_STATES_H

#include "constants/quest_states.h"

// Shared readings of quest progress. The quest scripts reach the u16 ones as
// specials; the Center local guide calls them directly, so its advice and the
// quest can never disagree about the current stage.

// CHANSEY_STAGE_*: the next step for Blob and the Poké Vial upgrades.
u16 GetChanseyQuestStage(void);
// Blob is caught and the Route 111 nurse still owes the second dose.
bool32 IsChanseyVialRewardAvailable(void);
// The Route 111 nurse has paid out and left.
u16 IsChanseyVialRewardClaimed(void);
// The Route 133 nurse is waiting with the third dose.
u16 IsRoute133VialUpgradeAvailable(void);

// Every Trick House puzzle is solved.
bool32 IsTrickHouseComplete(void);

// The Odd Keystone was picked up in the Sandstrewn Ruins and then used up at
// the Abandoned Ship (it is no longer in the Bag or the PC).
bool32 IsOddKeystoneSpent(void);

// Rayquaza has calmed Groudon and Kyogre above Sootopolis.
bool32 HasRayquazaCalmedSootopolis(void);

#endif // GUARD_QUEST_STATES_H
