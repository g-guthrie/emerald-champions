// Full Ability descriptions for the Pokédex Abilities page, read through
// GetAbilityFullDescription (src/ability_text.c). Each entry describes what
// the Ability does in this game's code, Champions rules and custom Abilities
// included, and is pre-wrapped with \n into at most six lines of at most
// 134 px in FONT_NARROW; test/ability_text.c measures every entry with the
// real font tables. Abilities without an entry use gAbilitiesInfo[].description.

    [ABILITY_STEAM_ENGINE] = COMPOUND_STRING(
        "When a Fire or Water move\n"
        "hits it, its Speed rises to\n"
        "the max. Water moves are\n"
        "never super effective on it.\n"
        "In the party, eggs hatch\n"
        "twice as fast."),
