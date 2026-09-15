// Authored competitive sets for the seven mandatory legendary story scenes.
// Applied via ApplyEmeraldChampionsScriptedSet at the live cap level, in
// place of ApplyEmeraldChampionsRandomNonMegaSet, by
// CreateSelectedLegendarySignEncounter and
// CreateEmeraldChampionsStaticLegendaryEncounter in src/legendary_signs.c.
//
// evs[] is in display order (HP, Atk, Def, SpA, SpD, Spe), matching
// gEmeraldChampionsEvOrder. IVs are always maxed by ApplyEmeraldChampionsScriptedSet
// (PRESET_SCRIPTED), so every mon here is 31/31/31/31/31/31.
//
// Move legality is pinned against data/emerald_champions/showdown_champions_learnsets.json
// (see scripts/ec_moves.py). Two design-brief moves were illegal and were
// substituted:
//   - MEW: SOFT_BOILED is not a legal Mew move in this project's pinned
//     learnset -> substituted with REST, the closest legal full-recovery move.
//   - JIRACHI: all four brief moves (IRON_HEAD, ZEN_HEADBUTT, THUNDER_WAVE,
//     WISH) are legal as authored; no substitution needed.
    {
        .species = SPECIES_MOLTRES,
        .set = {
            .moves = {MOVE_FLAMETHROWER, MOVE_HURRICANE, MOVE_ROOST, MOVE_WILL_O_WISP},
            .item = ITEM_SITRUS_BERRY,
            .requiredItem = ITEM_NONE,
            .requiredMove = MOVE_NONE,
            .nature = NATURE_TIMID,
            .ability = ABILITY_FLAME_BODY,
            .evs = {4, 0, 0, 252, 0, 252},
        },
    },
    {
        .species = SPECIES_LANDORUS,
        .set = {
            .moves = {MOVE_EARTH_POWER, MOVE_SLUDGE_WAVE, MOVE_ROCK_SLIDE, MOVE_CALM_MIND},
            .item = ITEM_LIFE_ORB,
            .requiredItem = ITEM_NONE,
            .requiredMove = MOVE_NONE,
            .nature = NATURE_TIMID,
            .ability = ABILITY_SHEER_FORCE,
            .evs = {4, 0, 0, 252, 0, 252},
        },
    },
    {
        .species = SPECIES_LATIAS,
        .set = {
            .moves = {MOVE_DRACO_METEOR, MOVE_PSYCHIC, MOVE_MIST_BALL, MOVE_RECOVER},
            .item = ITEM_SOUL_DEW,
            .requiredItem = ITEM_NONE,
            .requiredMove = MOVE_NONE,
            .nature = NATURE_TIMID,
            .ability = ABILITY_LEVITATE,
            .evs = {4, 0, 0, 252, 0, 252},
        },
    },
    {
        .species = SPECIES_LATIOS,
        .set = {
            .moves = {MOVE_DRACO_METEOR, MOVE_PSYSHOCK, MOVE_LUSTER_PURGE, MOVE_RECOVER},
            .item = ITEM_SOUL_DEW,
            .requiredItem = ITEM_NONE,
            .requiredMove = MOVE_NONE,
            .nature = NATURE_TIMID,
            .ability = ABILITY_LEVITATE,
            .evs = {4, 0, 0, 252, 0, 252},
        },
    },
    {
        .species = SPECIES_MEW,
        .set = {
            // Design brief asked for SOFT_BOILED; it is not a legal Mew move
            // in this project's pinned learnset, so REST substitutes as the
            // closest legal full-recovery option.
            .moves = {MOVE_PSYCHIC, MOVE_ICE_BEAM, MOVE_NASTY_PLOT, MOVE_REST},
            .item = ITEM_LEFTOVERS,
            .requiredItem = ITEM_NONE,
            .requiredMove = MOVE_NONE,
            .nature = NATURE_TIMID,
            .ability = ABILITY_SYNCHRONIZE,
            .evs = {4, 0, 0, 252, 0, 252},
        },
    },
    {
        .species = SPECIES_HEATRAN,
        .set = {
            .moves = {MOVE_MAGMA_STORM, MOVE_EARTH_POWER, MOVE_FLASH_CANNON, MOVE_TAUNT},
            .item = ITEM_AIR_BALLOON,
            .requiredItem = ITEM_NONE,
            .requiredMove = MOVE_NONE,
            .nature = NATURE_TIMID,
            .ability = ABILITY_FLASH_FIRE,
            .evs = {4, 0, 0, 252, 0, 252},
        },
    },
    {
        .species = SPECIES_DIANCIE,
        .set = {
            .moves = {MOVE_MOONBLAST, MOVE_DIAMOND_STORM, MOVE_EARTH_POWER, MOVE_CALM_MIND},
            .item = ITEM_LEFTOVERS,
            .requiredItem = ITEM_NONE,
            .requiredMove = MOVE_NONE,
            .nature = NATURE_MODEST,
            .ability = ABILITY_CLEAR_BODY,
            .evs = {252, 0, 4, 252, 0, 0},
        },
    },
    {
        .species = SPECIES_JIRACHI,
        .set = {
            .moves = {MOVE_IRON_HEAD, MOVE_ZEN_HEADBUTT, MOVE_THUNDER_WAVE, MOVE_WISH},
            .item = ITEM_LEFTOVERS,
            .requiredItem = ITEM_NONE,
            .requiredMove = MOVE_NONE,
            .nature = NATURE_JOLLY,
            .ability = ABILITY_SERENE_GRACE,
            .evs = {252, 4, 0, 0, 0, 252},
        },
    },
