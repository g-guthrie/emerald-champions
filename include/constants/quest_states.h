#ifndef GUARD_CONSTANTS_QUEST_STATES_H
#define GUARD_CONSTANTS_QUEST_STATES_H

// Named states for quests whose progress other systems read, above all the
// Pokémon Center local guide. The quest's own scripts advance these values;
// src/quest_states.c turns them into the stages that both the scripts and the
// guide ask about. The values are save data: never renumber them.

// Blob, the Route 111 nurse's Chansey: where it is (VAR_CHANSEY_NURSE_STATE).
#define CHANSEY_NURSE_NEEDS_HELP            0 // Blob is on Route 111 and the nurse is waiting.
#define CHANSEY_NURSE_BLOB_ON_ROUTE112      1 // Blob fled north to the cable car.
#define CHANSEY_NURSE_BLOB_ON_JAGGED_PASS   2
#define CHANSEY_NURSE_BLOB_IN_ASHEN_WOODS   3 // At the woods' entrance.
#define CHANSEY_NURSE_BLOB_ASHEN_WOODS_WEST 4
#define CHANSEY_NURSE_BLOB_ASHEN_WOODS_EAST 5 // Cornered: talk to it to throw the Heal Ball.
#define CHANSEY_NURSE_BLOB_CAUGHT           6 // In a Heal Ball; the Route 111 nurse owes the reward.
// Blob is off every map. The quest never sets this (the reward is recorded in
// VAR_POKE_VIAL_MAX_CHARGES); headless fixtures use it to clear Blob away.
#define CHANSEY_NURSE_RETIRED               7

// Doses the Poké Vial holds (VAR_POKE_VIAL_MAX_CHARGES). Each upgrade is a
// quest reward, so the capacity also records which rewards were claimed.
#define POKE_VIAL_CAPACITY_NONE     0 // No Poké Vial yet.
#define POKE_VIAL_CAPACITY_BASE     1 // A Pokémon Center nurse's first gift.
#define POKE_VIAL_CAPACITY_BLOB     2 // The Route 111 nurse's thanks for catching Blob.
#define POKE_VIAL_CAPACITY_ROUTE133 3 // The Route 133 nurse's thanks; the maximum.

// GetChanseyQuestStage: what the player should do next for Blob and its nurse.
#define CHANSEY_STAGE_NEEDS_HELP 0 // Meet the Route 111 nurse.
#define CHANSEY_STAGE_CHASE      1 // Blob is loose between Route 112 and Ashen Woods.
#define CHANSEY_STAGE_CAUGHT     2 // Bring Blob back to Route 111 for the second dose.
#define CHANSEY_STAGE_ROUTE133   3 // The nurse waits on Route 133 with the third dose.
#define CHANSEY_STAGE_COMPLETE   4 // Every dose earned.
#define CHANSEY_STAGE_RETIRED    5 // Blob is gone without a reward (CHANSEY_NURSE_RETIRED).

// VAR_TRICK_HOUSE_LEVEL counts the Trick Master's solved puzzles; level N
// means puzzle N + 1 is next.
#define TRICK_HOUSE_PUZZLE_COUNT 8
#define TRICK_HOUSE_ALL_SOLVED   TRICK_HOUSE_PUZZLE_COUNT

// The Sootopolis crisis (VAR_SOOTOPOLIS_CITY_STATE).
#define SOOTOPOLIS_STATE_CALM            0 // Before the Seafloor Cavern.
#define SOOTOPOLIS_STATE_LEGENDS_CLASH   1 // Groudon and Kyogre are fighting; their scene plays on arrival.
#define SOOTOPOLIS_STATE_LEGENDS_SEEN    2 // The player saw them; Wallace is in the Cave of Origin.
#define SOOTOPOLIS_STATE_TO_SKY_PILLAR   3 // Wallace left for Sky Pillar.
#define SOOTOPOLIS_STATE_SKY_PILLAR_OPEN 4 // Wallace opened Sky Pillar and went back to the city.
#define SOOTOPOLIS_STATE_RAYQUAZA_AWAKE  5 // Rayquaza calmed the legends.
#define SOOTOPOLIS_STATE_GYM_BEATEN      6 // Juan's Badge; the city is back to normal.

#endif // GUARD_CONSTANTS_QUEST_STATES_H
