#ifndef GUARD_CONFIG_CAPS_H
#define GUARD_CONFIG_CAPS_H

// Level Cap Constants
#define EXP_CAP_NONE                    0 // Regular behavior, no level caps are applied
#define EXP_CAP_HARD                    1 // Pokémon with a level >= the level cap cannot gain any experience
#define EXP_CAP_SOFT                    2 // Pokémon with a level >= the level cap will gain reduced experience

#define LEVEL_CAP_NONE                  0 // No level cap, only applicable if B_EXP_CAP_TYPE is EXP_CAP_NONE
#define LEVEL_CAP_FLAG_LIST             1 // Level cap is chosen according to the first unset flag in `sLevelCapFlagMap`
#define LEVEL_CAP_VARIABLE              2 // Level cap is chosen according to the contents of the event variable specified by B_LEVEL_CAP_VARIABLE

// Level Cap Configs
#define B_EXP_CAP_TYPE                  EXP_CAP_HARD   // The cap is a ceiling, never passed. EXP_CAP_SOFT would let levels run past it.
#define B_LEVEL_CAP_TYPE                LEVEL_CAP_FLAG_LIST
#define B_LEVEL_CAP_VARIABLE            0 // event variable used to derive level cap if B_LEVEL_CAP_TYPE is set to LEVEL_CAP_VARIABLE

#define B_RARE_CANDY_CAP                TRUE  // Rare Candy and Leveler effects stop exactly at the current cap.
#define B_LEVEL_CAP_EXP_UP              FALSE // If set to true, mons under level cap will receive more experience

// EV Cap Constants
#define EV_CAP_NONE                     0 // Regular behavior, no EV caps are applied
#define EV_CAP_FLAG_LIST                1 // EV cap is chosen according to the first unset flag in `sEVCapFlagMap`
#define EV_CAP_VARIABLE                 2 // EV cap is chosen according to the contents of the event variable specified by B_EV_CAP_VARIABLE
#define EV_CAP_NO_GAIN                  3 // No EVs can be gained

// EV Cap Configs
#define B_EV_CAP_TYPE                   EV_CAP_NO_GAIN // Battles never train EVs. EVs come only from vitamins and Evie in Fallarbor, so a team's stats depend on choices, not grinding.
#define B_EV_CAP_VARIABLE               0 // unused unless B_EV_CAP_TYPE is EV_CAP_VARIABLE

#define B_EV_ITEMS_CAP                  FALSE // Vitamins train to the ordinary 252/510 limits, as they do in Inclement.

#endif /* GUARD_CONFIG_CAPS_H */
