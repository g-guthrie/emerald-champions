#include "global.h"
#include "pokenav.h"
#include "test/test.h"

extern bool32 Test_SetActivePokenavMenu(u32 menuId);

TEST("Pokenav dispatch: retired and invalid IDs cannot enter menu callbacks")
{
    // No Pokenav resources or graphics are allocated. Rejection must happen
    // before dispatch or input/UI initialization touches any menu state.
    EXPECT(!Test_SetActivePokenavMenu(POKENAV_MATCH_CALL));
    EXPECT(!Test_SetActivePokenavMenu(0));
    EXPECT(!Test_SetActivePokenavMenu(POKENAV_MENU_IDS_START - 1));
    EXPECT(!Test_SetActivePokenavMenu(POKENAV_MENU_IDS_START + 100));
    EXPECT(!Test_SetActivePokenavMenu(UINT_MAX));
}
