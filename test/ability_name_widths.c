#include "global.h"
#include "test/test.h"
#include "text.h"
#include "constants/abilities.h"

// Every Ability name must fit wherever it is drawn. Widths are the native
// windows' pixel widths; sites that shrink long names use GetFontIdToFit,
// and the check measures the font that call would pick.
TEST("Ability names fit the summary, battle pop-up, Pokédex and DexNav")
{
    for (u32 ability = ABILITY_NONE + 1; ability < ABILITIES_COUNT; ability++)
    {
        const u8 *name = gAbilitiesInfo[ability].name;
        u32 font;

        if (name[0] == EOS)
            continue;
        // Summary Profile page: an 18-tile window, FONT_NORMAL at x 0.
        EXPECT_LE(GetStringWidth(FONT_NORMAL, name, 0), 18 * 8);
        // Battle Ability pop-up: a 10-tile text area, shrinks to fit.
        font = GetFontIdToFit(name, FONT_SMALL, 0, 10 * 8);
        EXPECT_LE(GetStringWidth(font, name, 0), 10 * 8);
        // Pokédex stats page: an 18-tile window, FONT_SMALL at x 5.
        EXPECT_LE(GetStringWidth(FONT_SMALL, name, 0), 18 * 8 - 5);
        // DexNav info column: a 9-tile window, shrinks to fit.
        font = GetFontIdToFit(name, FONT_SMALL, 0, 9 * 8);
        EXPECT_LE(GetStringWidth(font, name, 0), 9 * 8);
    }
}
