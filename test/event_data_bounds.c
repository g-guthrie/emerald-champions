#include "global.h"
#include "event_data.h"
#include "test/test.h"

TEST("Event state: invalid flag and variable gaps cannot address save memory")
{
    static const u16 invalidVars[] = {VARS_END + 1, SPECIAL_VARS_START - 1,
        SPECIAL_VARS_END + 1, TESTING_VARS_START - 1, TESTING_VARS_START + 8, 0xFFFF};
    static const u16 invalidFlags[] = {0, FLAGS_COUNT, SPECIAL_FLAGS_START - 1,
        SPECIAL_FLAGS_END + 1, TESTING_FLAGS_START - 1, TESTING_FLAGS_START + 8, 0xFFFF};
    for (u32 i = 0; i < ARRAY_COUNT(invalidVars); i++)
        EXPECT_EQ(GetVarPointer(invalidVars[i]), NULL);
    for (u32 i = 0; i < ARRAY_COUNT(invalidFlags); i++)
        EXPECT_EQ(GetFlagPointer(invalidFlags[i]), NULL);
}

TEST("Event state: saved, special and testing boundaries retain independent storage")
{
    static const u16 vars[] = {VARS_START, VARS_END, SPECIAL_VARS_START, SPECIAL_VARS_END,
        TESTING_VARS_START, TESTING_VARS_START + 7};
    static const u16 flags[] = {1, FLAGS_COUNT - 1, SPECIAL_FLAGS_START, SPECIAL_FLAGS_END,
        TESTING_FLAGS_START, TESTING_FLAGS_START + 7};
    for (u32 i = 0; i < ARRAY_COUNT(vars); i++)
        EXPECT(VarSet(vars[i], 100 + i));
    for (u32 i = 0; i < ARRAY_COUNT(vars); i++)
        EXPECT_EQ(VarGet(vars[i]), 100 + i);
    for (u32 i = 0; i < ARRAY_COUNT(flags); i++)
        FlagSet(flags[i]);
    for (u32 i = 0; i < ARRAY_COUNT(flags); i++)
    {
        EXPECT(FlagGet(flags[i]));
        FlagToggle(flags[i]);
        EXPECT(!FlagGet(flags[i]));
    }
    for (u32 i = 0; i < ARRAY_COUNT(vars); i++)
        VarSet(vars[i], 0);
    EXPECT_EQ(VarGet(123), 123);
    EXPECT(!VarSet(123, 456));
}
