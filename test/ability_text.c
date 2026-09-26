#include "global.h"
#include "test/test.h"
#include "ability_text.h"
#include "text.h"
#include "constants/abilities.h"

// The Pokédex Abilities page prints the full description in FONT_NARROW in a
// box six lines tall and 134 px wide. Entries are pre-wrapped with \n.
#define FULL_DESCRIPTION_WIDTH 134
#define FULL_DESCRIPTION_LINES 6

TEST("Full Ability descriptions fit the Pokédex Abilities box")
{
    for (u32 ability = ABILITY_NONE; ability < ABILITIES_COUNT; ability++)
    {
        const u8 *text = GetAbilityFullDescription(ability);
        u32 lines = 1;

        EXPECT(text != NULL);
        EXPECT_NE(text[0], EOS);
        for (const u8 *c = text; *c != EOS; c++)
        {
            if (*c == CHAR_NEWLINE)
                lines++;
        }
        EXPECT_LE(lines, FULL_DESCRIPTION_LINES);
        EXPECT_LE(GetStringWidth(FONT_NARROW, text, 0), FULL_DESCRIPTION_WIDTH);
    }
}

// Summary Profile page: an 18-tile window prints the one-line description at
// x 0 with FONT_NORMAL, shrinking to FONT_NARROW and then FONT_NARROWER.
TEST("Short Ability descriptions fit the Summary on one line")
{
    for (u32 ability = ABILITY_NONE; ability < ABILITIES_COUNT; ability++)
    {
        const u8 *text = gAbilitiesInfo[ability].description;
        u32 font;

        if (text == NULL)
            continue;
        for (const u8 *c = text; *c != EOS; c++)
            EXPECT_NE(*c, CHAR_NEWLINE);
        font = GetFontIdToFit(text, FONT_NORMAL, 0, 18 * 8);
        EXPECT_LE(GetStringWidth(font, text, 0), 18 * 8);
    }
}

TEST("Abilities without full text fall back to the short description")
{
    EXPECT_EQ(GetAbilityFullDescription(ABILITIES_COUNT), gAbilitiesInfo[ABILITY_NONE].description);
    EXPECT(GetAbilityFullDescription(ABILITY_STEAM_ENGINE) != gAbilitiesInfo[ABILITY_STEAM_ENGINE].description);
}
