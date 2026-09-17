#ifndef GUARD_CONSTANTS_BATTLE_ARENA_H
#define GUARD_CONSTANTS_BATTLE_ARENA_H

// Emerald Champions: the Battle Arena facility is retired. The battle engine
// and data/battle_scripts_1.s still read the judgment result, so only these
// values survive; see src/retired_frontier.c.
#define ARENA_RESULT_RUNNING     0
#define ARENA_RESULT_STEP_DONE   1
#define ARENA_RESULT_PLAYER_WON  2
#define ARENA_RESULT_PLAYER_LOST 3
#define ARENA_RESULT_TIE         4

#endif //GUARD_CONSTANTS_BATTLE_ARENA_H
