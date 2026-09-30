#include "global.h"
#include "constants/trainer_types.h"
#include "test/test.h"

extern u8 Test_GetSortedTrainerObjects(u8 *trainerObjects);

TEST("Trainer scan: only active trainers are sorted, without touching the next array entry")
{
    u32 count;
    PARAMETRIZE { count = 0; }
    PARAMETRIZE { count = 1; }
    PARAMETRIZE { count = OBJECT_EVENTS_COUNT - 1; }
    PARAMETRIZE { count = OBJECT_EVENTS_COUNT; }
    struct ObjectEvent saved[OBJECT_EVENTS_COUNT];
    u8 objects[OBJECT_EVENTS_COUNT + 1] = {0};
    memcpy(saved, gObjectEvents, sizeof(saved));
    memset(gObjectEvents, 0, sizeof(gObjectEvents));
    // Slot zero is an ordinary actor unless the list is completely full.
    // The spare zero entry is readable, but must not participate in sorting.
    gObjectEvents[0].active = TRUE;
    for (u32 i = 0; i < count; i++)
    {
        u32 slot = (i + 1) % OBJECT_EVENTS_COUNT;
        gObjectEvents[slot].active = TRUE;
        gObjectEvents[slot].trainerType = TRAINER_TYPE_NORMAL + i % 3;
        gObjectEvents[slot].localId = count - i;
    }
    EXPECT_EQ(Test_GetSortedTrainerObjects(objects), count);
    for (u32 i = 0; i < count; i++)
        EXPECT_EQ(objects[i], (count - i) % OBJECT_EVENTS_COUNT);
    EXPECT_EQ(objects[count], 0);
    // Stale trainer metadata must not include an inactive actor.
    for (u32 i = 0; i < OBJECT_EVENTS_COUNT; i++)
        gObjectEvents[i].active = FALSE;
    EXPECT_EQ(Test_GetSortedTrainerObjects(objects), 0);
    memcpy(gObjectEvents, saved, sizeof(saved));
}
