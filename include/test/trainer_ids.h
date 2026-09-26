#ifndef GUARD_TEST_TRAINER_IDS_H
#define GUARD_TEST_TRAINER_IDS_H

#if !TESTING
#error Synthetic trainer IDs are only available in native test builds.
#endif

// Use two unoccupied trainer slots without moving campaign flags or partners.
#define TEST_TRAINER_IDS_START            TRAINERS_COUNT_EMERALD
#define TRAINER_RED_TEST                  (TEST_TRAINER_IDS_START + 0)
#define TRAINER_LEAF_TEST                 (TEST_TRAINER_IDS_START + 1)
#define TEST_TRAINERS_COUNT               (TEST_TRAINER_IDS_START + 2)

#if TEST_TRAINERS_COUNT > MAX_TRAINERS_COUNT
#error Synthetic trainers no longer fit before the reserved partner IDs.
#endif

#endif // GUARD_TEST_TRAINER_IDS_H
