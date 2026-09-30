// Mauville's iconic move tutor (MOVE_RELEARNER_ICONIC_MOVES): signature moves
// iconic Pokemon can never learn anywhere else. The Center tutor never offers
// these. Species match on the base species, so regional forms share a row.
// `badges` is the badge count the player needs before the tutor offers it.
struct EmeraldChampionsIconicMove
{
    u16 species;
    u16 move;
    u16 family; // Egg species owning these receipt bits, including evolved lessons.
    u8 badges;
    u8 receiptBit; // Stable within an evolution family. Never renumber.
};

static const struct EmeraldChampionsIconicMove sEmeraldChampionsIconicMoves[] =
{
    // Flying Pikachu and the Let's Go partner moves.
    {SPECIES_PICHU,       MOVE_FLY,               SPECIES_PICHU,      2, 0},
    {SPECIES_PIKACHU,     MOVE_FLY,               SPECIES_PICHU,      2, 0},
    {SPECIES_RAICHU,      MOVE_FLY,               SPECIES_PICHU,      2, 0},
    {SPECIES_PIKACHU,     MOVE_PIKA_PAPOW,        SPECIES_PICHU,      2, 1},
    {SPECIES_PIKACHU,     MOVE_ZIPPY_ZAP,         SPECIES_PICHU,      3, 2},
    {SPECIES_PIKACHU,     MOVE_SPLISHY_SPLASH,    SPECIES_PICHU,      3, 3},
    {SPECIES_PIKACHU,     MOVE_FLOATY_FALL,       SPECIES_PICHU,      3, 4},

    // Partner Eevee knows every one; each evolution keeps its own.
    {SPECIES_EEVEE,       MOVE_VEEVEE_VOLLEY,     SPECIES_EEVEE,      2, 0},
    {SPECIES_EEVEE,       MOVE_SIZZLY_SLIDE,      SPECIES_EEVEE,      2, 1},
    {SPECIES_EEVEE,       MOVE_BOUNCY_BUBBLE,     SPECIES_EEVEE,      2, 2},
    {SPECIES_EEVEE,       MOVE_BUZZY_BUZZ,        SPECIES_EEVEE,      2, 3},
    {SPECIES_EEVEE,       MOVE_GLITZY_GLOW,       SPECIES_EEVEE,      3, 4},
    {SPECIES_EEVEE,       MOVE_BADDY_BAD,         SPECIES_EEVEE,      3, 5},
    {SPECIES_EEVEE,       MOVE_SAPPY_SEED,        SPECIES_EEVEE,      4, 6},
    {SPECIES_EEVEE,       MOVE_FREEZY_FROST,      SPECIES_EEVEE,      4, 7},
    {SPECIES_EEVEE,       MOVE_SPARKLY_SWIRL,     SPECIES_EEVEE,      5, 8},
    {SPECIES_FLAREON,     MOVE_SIZZLY_SLIDE,      SPECIES_EEVEE,      2, 1},
    {SPECIES_VAPOREON,    MOVE_BOUNCY_BUBBLE,     SPECIES_EEVEE,      2, 2},
    {SPECIES_JOLTEON,     MOVE_BUZZY_BUZZ,        SPECIES_EEVEE,      2, 3},
    {SPECIES_ESPEON,      MOVE_GLITZY_GLOW,       SPECIES_EEVEE,      3, 4},
    {SPECIES_UMBREON,     MOVE_BADDY_BAD,         SPECIES_EEVEE,      3, 5},
    {SPECIES_LEAFEON,     MOVE_SAPPY_SEED,        SPECIES_EEVEE,      4, 6},
    {SPECIES_GLACEON,     MOVE_FREEZY_FROST,      SPECIES_EEVEE,      4, 7},
    {SPECIES_SYLVEON,     MOVE_SPARKLY_SWIRL,     SPECIES_EEVEE,      5, 8},

    // Kanto and Johto icons.
    {SPECIES_MAGIKARP,    MOVE_DRAGON_RAGE,       SPECIES_MAGIKARP,   2, 0},
    {SPECIES_IGGLYBUFF,   MOVE_SPARKLING_ARIA,    SPECIES_IGGLYBUFF,  2, 0},
    {SPECIES_JIGGLYPUFF,  MOVE_SPARKLING_ARIA,    SPECIES_IGGLYBUFF,  2, 0},
    {SPECIES_WIGGLYTUFF,  MOVE_SPARKLING_ARIA,    SPECIES_IGGLYBUFF,  2, 0},
    {SPECIES_NINETALES,   MOVE_BITTER_MALICE,     SPECIES_VULPIX,     3, 0},
    {SPECIES_GENGAR,      MOVE_SPIRIT_SHACKLE,    SPECIES_GASTLY,     5, 0},
    {SPECIES_ARCANINE,    MOVE_SACRED_FIRE,       SPECIES_GROWLITHE,  5, 0},
    {SPECIES_CHARIZARD,   MOVE_BLUE_FLARE,        SPECIES_CHARMANDER, 6, 0}, // Mega Charizard X's blue fire
    {SPECIES_GYARADOS,    MOVE_DRAGON_ASCENT,     SPECIES_MAGIKARP,   7, 1},

    // Hoenn icons.
    {SPECIES_SCEPTILE,    MOVE_DRAGON_HAMMER,     SPECIES_TREECKO,    4, 0},
    {SPECIES_BLAZIKEN,    MOVE_PYRO_BALL,         SPECIES_TORCHIC,    4, 0},
    {SPECIES_SWAMPERT,    MOVE_WAVE_CRASH,        SPECIES_MUDKIP,     4, 0},
    {SPECIES_SPINDA,      MOVE_REVELATION_DANCE,  SPECIES_SPINDA,     2, 0},
    {SPECIES_MAWILE,      MOVE_JAW_LOCK,          SPECIES_MAWILE,     2, 0},
    {SPECIES_TROPIUS,     MOVE_APPLE_ACID,        SPECIES_TROPIUS,    3, 0},
    {SPECIES_TROPIUS,     MOVE_GRAV_APPLE,        SPECIES_TROPIUS,    3, 1},
    {SPECIES_LUDICOLO,    MOVE_AQUA_STEP,         SPECIES_LOTAD,      3, 0},
    {SPECIES_NINJASK,     MOVE_FIRST_IMPRESSION,  SPECIES_NINCADA,    3, 0},
    {SPECIES_KECLEON,     MOVE_SPECTRAL_THIEF,    SPECIES_KECLEON,    3, 0},
    {SPECIES_ABSOL,       MOVE_DOOM_DESIRE,       SPECIES_ABSOL,      4, 0},
    {SPECIES_MILOTIC,     MOVE_SPARKLING_ARIA,    SPECIES_FEEBAS,     4, 0},
    {SPECIES_SHARPEDO,    MOVE_WAVE_CRASH,        SPECIES_CARVANHA,   4, 0},
    {SPECIES_BANETTE,     MOVE_RAGE_FIST,         SPECIES_SHUPPET,    4, 0},
    {SPECIES_SHIFTRY,     MOVE_BLEAKWIND_STORM,   SPECIES_SEEDOT,     5, 0},
    {SPECIES_TORKOAL,     MOVE_STEAM_ERUPTION,    SPECIES_TORKOAL,    5, 0},
    {SPECIES_CAMERUPT,    MOVE_MAGMA_STORM,       SPECIES_NUMEL,      5, 0},
    {SPECIES_ALTARIA,     MOVE_BOOMBURST,         SPECIES_SWABLU,     5, 0},
    {SPECIES_FLYGON,      MOVE_CLANGING_SCALES,   SPECIES_TRAPINCH,   5, 0},
    {SPECIES_WHISCASH,    MOVE_PRECIPICE_BLADES,  SPECIES_BARBOACH,   6, 0},
    {SPECIES_GARDEVOIR,   MOVE_HYPERSPACE_HOLE,   SPECIES_RALTS,      6, 0},
    {SPECIES_DUSKNOIR,    MOVE_SHADOW_FORCE,      SPECIES_DUSKULL,    6, 0},
    {SPECIES_SALAMENCE,   MOVE_DRAGON_ASCENT,     SPECIES_BAGON,      7, 0},
    {SPECIES_LATIAS,      MOVE_LUSTER_PURGE,      SPECIES_LATIAS,     0, 0},
    {SPECIES_LATIOS,      MOVE_MIST_BALL,         SPECIES_LATIOS,     0, 0},
};
