// Mauville's iconic move tutor (MOVE_RELEARNER_ICONIC_MOVES): signature moves
// iconic Pokemon can never learn anywhere else. The Center tutor never offers
// these. Species match on the base species, so regional forms share a row.
// `badges` is the badge count the player needs before the tutor offers it.
struct EmeraldChampionsIconicMove
{
    u16 species;
    u16 move;
    u8 badges;
};

static const struct EmeraldChampionsIconicMove sEmeraldChampionsIconicMoves[] =
{
    // Flying Pikachu and the Let's Go partner moves.
    {SPECIES_PICHU,      MOVE_FLY,              2},
    {SPECIES_PIKACHU,    MOVE_FLY,              2},
    {SPECIES_RAICHU,     MOVE_FLY,              2},
    {SPECIES_PIKACHU,    MOVE_PIKA_PAPOW,       2},
    {SPECIES_PIKACHU,    MOVE_ZIPPY_ZAP,        3},
    {SPECIES_PIKACHU,    MOVE_SPLISHY_SPLASH,   3},
    {SPECIES_PIKACHU,    MOVE_FLOATY_FALL,      3},

    // Partner Eevee knows every one; each evolution keeps its own.
    {SPECIES_EEVEE,      MOVE_VEEVEE_VOLLEY,    2},
    {SPECIES_EEVEE,      MOVE_SIZZLY_SLIDE,     2},
    {SPECIES_EEVEE,      MOVE_BOUNCY_BUBBLE,    2},
    {SPECIES_EEVEE,      MOVE_BUZZY_BUZZ,       2},
    {SPECIES_EEVEE,      MOVE_GLITZY_GLOW,      3},
    {SPECIES_EEVEE,      MOVE_BADDY_BAD,        3},
    {SPECIES_EEVEE,      MOVE_SAPPY_SEED,       4},
    {SPECIES_EEVEE,      MOVE_FREEZY_FROST,     4},
    {SPECIES_EEVEE,      MOVE_SPARKLY_SWIRL,    5},
    {SPECIES_FLAREON,    MOVE_SIZZLY_SLIDE,     2},
    {SPECIES_VAPOREON,   MOVE_BOUNCY_BUBBLE,    2},
    {SPECIES_JOLTEON,    MOVE_BUZZY_BUZZ,       2},
    {SPECIES_ESPEON,     MOVE_GLITZY_GLOW,      3},
    {SPECIES_UMBREON,    MOVE_BADDY_BAD,        3},
    {SPECIES_LEAFEON,    MOVE_SAPPY_SEED,       4},
    {SPECIES_GLACEON,    MOVE_FREEZY_FROST,     4},
    {SPECIES_SYLVEON,    MOVE_SPARKLY_SWIRL,    5},

    // Kanto and Johto icons.
    {SPECIES_MAGIKARP,   MOVE_DRAGON_RAGE,      2},
    {SPECIES_IGGLYBUFF,  MOVE_SPARKLING_ARIA,   2},
    {SPECIES_JIGGLYPUFF, MOVE_SPARKLING_ARIA,   2},
    {SPECIES_WIGGLYTUFF, MOVE_SPARKLING_ARIA,   2},
    {SPECIES_NINETALES,  MOVE_BITTER_MALICE,    3},
    {SPECIES_GENGAR,     MOVE_SPIRIT_SHACKLE,   5},
    {SPECIES_ARCANINE,   MOVE_SACRED_FIRE,      5},
    {SPECIES_CHARIZARD,  MOVE_BLUE_FLARE,       6}, // Mega Charizard X's blue fire
    {SPECIES_GYARADOS,   MOVE_DRAGON_ASCENT,    7},

    // Hoenn icons.
    {SPECIES_SCEPTILE,   MOVE_DRAGON_HAMMER,    4},
    {SPECIES_BLAZIKEN,   MOVE_PYRO_BALL,        4},
    {SPECIES_SWAMPERT,   MOVE_WAVE_CRASH,       4},
    {SPECIES_SPINDA,     MOVE_REVELATION_DANCE, 2},
    {SPECIES_MAWILE,     MOVE_JAW_LOCK,         2},
    {SPECIES_TROPIUS,    MOVE_APPLE_ACID,       3},
    {SPECIES_TROPIUS,    MOVE_GRAV_APPLE,       3},
    {SPECIES_LUDICOLO,   MOVE_AQUA_STEP,        3},
    {SPECIES_NINJASK,    MOVE_FIRST_IMPRESSION, 3},
    {SPECIES_KECLEON,    MOVE_SPECTRAL_THIEF,   3},
    {SPECIES_ABSOL,      MOVE_DOOM_DESIRE,      4},
    {SPECIES_MILOTIC,    MOVE_SPARKLING_ARIA,   4},
    {SPECIES_SHARPEDO,   MOVE_WAVE_CRASH,       4},
    {SPECIES_BANETTE,    MOVE_RAGE_FIST,        4},
    {SPECIES_SHIFTRY,    MOVE_BLEAKWIND_STORM,  5},
    {SPECIES_TORKOAL,    MOVE_STEAM_ERUPTION,   5},
    {SPECIES_CAMERUPT,   MOVE_MAGMA_STORM,      5},
    {SPECIES_ALTARIA,    MOVE_BOOMBURST,        5},
    {SPECIES_FLYGON,     MOVE_CLANGING_SCALES,  5},
    {SPECIES_WHISCASH,   MOVE_PRECIPICE_BLADES, 6},
    {SPECIES_GARDEVOIR,  MOVE_HYPERSPACE_HOLE,  6},
    {SPECIES_DUSKNOIR,   MOVE_SHADOW_FORCE,     6},
    {SPECIES_SALAMENCE,  MOVE_DRAGON_ASCENT,    7},
    {SPECIES_LATIAS,     MOVE_LUSTER_PURGE,     0},
    {SPECIES_LATIOS,     MOVE_MIST_BALL,        0},
};
