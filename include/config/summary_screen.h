#ifndef GUARD_CONFIG_SUMMARY_SCREEN_H
#define GUARD_CONFIG_SUMMARY_SCREEN_H

// Settings
#define P_SUMMARY_SCREEN_NATURE_COLORS   TRUE   // If TRUE, nature-based stat boosts and reductions will be red and blue in the summary screen.
#define P_SUMMARY_SCREEN_RENAME          FALSE  // Renaming remains a native field service instead of replacing Summary's Cancel action.

// IV/EV settings
#define P_SUMMARY_SCREEN_IV_EV_INFO      FALSE  // If TRUE, will allow player to cycle through the Stats, IVs, and EVs in the summary screen skills page.
#define P_SUMMARY_SCREEN_IV_EV_BOX_ONLY  FALSE  // If TRUE, will allow player to cycle through the Stats, IVs, and EVs in the summary screen skills page, but only in the PC storage box.
#define P_SUMMARY_SCREEN_IV_HYPERTRAIN   TRUE   // If TRUE, stats that have been hyper trained will show as 31/S when viewing them in the summary screen
#define P_SUMMARY_SCREEN_IV_EV_TILESET   FALSE  // If TRUE, loads an alternate tileset to allow changing the "STATS" label in the summary screen skills page. Note: if it's still loading the alternate tileset after changing this and recompiling, you may need a `make clean` before compilation.
#define P_SUMMARY_SCREEN_IV_EV_VALUES    FALSE  // If TRUE, will show the actual IV value instead of the letter grade.
/*
LETTER GRADE GUIDE:

    F = 0
    D = 1 - 15
    C = 16 - 25
    B = 26 - 29
    A = 30
    S = 31

Info taken from https://bulbapedia.bulbagarden.net/wiki/Stats_judge.
*/
#define P_SUMMARY_SCREEN_IV_ONLY         FALSE  // If TRUE, will only show IV info in the summary screen.
#define P_SUMMARY_SCREEN_EV_ONLY         FALSE  // If TRUE, will only show EV info in the summary screen.

// IV/EV flags
#define P_FLAG_SUMMARY_SCREEN_IV_EV_INFO 0      // If this flag is set, will allow player to cycle through the Stats, IVs, and EVs in the summary screen skills page. Note: if P_SUMMARY_SCREEN_IV_EV_INFO is TRUE, this flag does nothing.

// Move Relearner settings
#define P_ENABLE_MOVE_RELEARNERS         TRUE    // Emerald Champions exposes every legal preparation move from the start.
#define P_SORT_MOVES                     TRUE    // The complete lists must remain usable on native GBA menus.

// Level up Relearner
#define P_PRE_EVO_MOVES                  TRUE    // Legally inherited pre-evolution moves remain available.
#define P_ENABLE_ALL_LEVEL_UP_MOVES      TRUE    // Level does not gate preparation at the tutor.

// TM Relearner
#define P_TM_MOVES_RELEARNER             TRUE    // TM moves are part of the complete preparation service.
#define P_ENABLE_ALL_TM_MOVES            TRUE    // Owning the physical TM is never required.

// Relearner flags - Redundant if P_ENABLE_MOVE_RELEARNERS is TRUE, but still added here incase you don't want all relearners unlocked at the same time.
// To use the following features in scripting, replace the 0s with the flag ID you're assigning it to.
// Eg: Replace with FLAG_UNUSED_0x264 so you can use that flag to toggle the feature.
#define P_FLAG_EGG_MOVES                 0       // If this flag is set, enables egg move relearner.
#define P_FLAG_TUTOR_MOVES               0       // If this flag is set, enables tutor move relearner.

// Move Relearner summary screen
#define P_SUMMARY_SCREEN_MOVE_RELEARNER  FALSE  // Complete move preparation belongs to the Center specialist's single, consistent flow.
#define P_SUMMARY_MOVE_RELEARNER_FULL_PP TRUE   // If TRUE, the move relearner in the summary screen restores relearned moves' PP to full.

#endif // GUARD_CONFIG_SUMMARY_SCREEN_H
