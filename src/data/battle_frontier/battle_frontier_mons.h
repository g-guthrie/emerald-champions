const struct TrainerMon gBattleFrontierMons[NUM_FRONTIER_MONS] =
{
    [FRONTIER_MON_SUNKERN] = {
        .species = SPECIES_SUNKERN,
        .moves = {MOVE_ENDEAVOR, MOVE_LEAF_STORM, MOVE_EARTH_POWER, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_QUIET,
        .ability = ABILITY_EARLY_BIRD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_AZURILL] = {
        .species = SPECIES_AZURILL,
        .moves = {MOVE_BELLY_DRUM, MOVE_AQUA_JET, MOVE_PLAY_ROUGH, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_HUGE_POWER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CATERPIE] = {
        .species = SPECIES_CATERPIE,
        .moves = {MOVE_ELECTROWEB, MOVE_STRING_SHOT, MOVE_BUG_BITE, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SHIELD_DUST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WEEDLE] = {
        .species = SPECIES_WEEDLE,
        .moves = {MOVE_ELECTROWEB, MOVE_STRING_SHOT, MOVE_BUG_BITE, MOVE_POISON_STING},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SHIELD_DUST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WURMPLE] = {
        .species = SPECIES_WURMPLE,
        .moves = {MOVE_STRING_SHOT, MOVE_BUG_BITE, MOVE_POISON_STING, MOVE_TACKLE},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_SHIELD_DUST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RALTS] = {
        .species = SPECIES_RALTS,
        .moves = {MOVE_DAZZLING_GLEAM, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_BOLD,
        .ability = ABILITY_TELEPATHY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MAGIKARP] = {
        .species = SPECIES_MAGIKARP,
        .moves = {MOVE_HYDRO_PUMP, MOVE_BOUNCE, MOVE_FLAIL, MOVE_FACADE},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FEEBAS] = {
        .species = SPECIES_FEEBAS,
        .moves = {MOVE_HAZE, MOVE_ICY_WIND, MOVE_HYPNOSIS, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 0, 32),
        .nature = NATURE_TIMID,
        .ability = ABILITY_OBLIVIOUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_METAPOD] = {
        .species = SPECIES_METAPOD,
        .moves = {MOVE_ELECTROWEB, MOVE_STRING_SHOT, MOVE_BUG_BITE, MOVE_IRON_DEFENSE},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_SHED_SKIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KAKUNA] = {
        .species = SPECIES_KAKUNA,
        .moves = {MOVE_ELECTROWEB, MOVE_STRING_SHOT, MOVE_SKITTER_SMACK, MOVE_IRON_DEFENSE},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_SHED_SKIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PICHU] = {
        .species = SPECIES_PICHU,
        .moves = {MOVE_FAKE_OUT, MOVE_ELECTROWEB, MOVE_CHARM, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LIGHTNING_ROD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SILCOON] = {
        .species = SPECIES_SILCOON,
        .moves = {MOVE_HARDEN, MOVE_STRING_SHOT, MOVE_BUG_BITE, MOVE_POISON_STING},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_SHED_SKIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CASCOON] = {
        .species = SPECIES_CASCOON,
        .moves = {MOVE_HARDEN, MOVE_STRING_SHOT, MOVE_BUG_BITE, MOVE_POISON_STING},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_SHED_SKIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_IGGLYBUFF] = {
        .species = SPECIES_IGGLYBUFF,
        .moves = {MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_HEAL_PULSE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_FRIEND_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WOOPER] = {
        .species = SPECIES_WOOPER,
        .moves = {MOVE_SCALD, MOVE_ENCORE, MOVE_YAWN, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_SASSY,
        .ability = ABILITY_WATER_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TYROGUE] = {
        .species = SPECIES_TYROGUE,
        .moves = {MOVE_FAKE_OUT, MOVE_HELPING_HAND, MOVE_FEINT, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_VITAL_SPIRIT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SENTRET] = {
        .species = SPECIES_SENTRET,
        .moves = {MOVE_FOLLOW_ME, MOVE_HELPING_HAND, MOVE_SUPER_FANG, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_FRISK,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CLEFFA] = {
        .species = SPECIES_CLEFFA,
        .moves = {MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_HEAL_PULSE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_FRIEND_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SEEDOT] = {
        .species = SPECIES_SEEDOT,
        .moves = {MOVE_FOUL_PLAY, MOVE_LEECH_SEED, MOVE_EXPLOSION, MOVE_PROTECT},
        .heldItem = ITEM_NONE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_PICKPOCKET,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LOTAD] = {
        .species = SPECIES_LOTAD,
        .moves = {MOVE_MUDDY_WATER, MOVE_GIGA_DRAIN, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_POOCHYENA] = {
        .species = SPECIES_POOCHYENA,
        .moves = {MOVE_PROTECT, MOVE_FACADE, MOVE_CRUNCH, MOVE_PLAY_ROUGH},
        .heldItem = ITEM_TOXIC_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_QUICK_FEET,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SHEDINJA] = {
        .species = SPECIES_SHEDINJA,
        .moves = {MOVE_POLTERGEIST, MOVE_X_SCISSOR, MOVE_SHADOW_SNEAK, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(0, 32, 2, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_WONDER_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MAKUHITA] = {
        .species = SPECIES_MAKUHITA,
        .moves = {MOVE_FAKE_OUT, MOVE_CLOSE_COMBAT, MOVE_KNOCK_OFF, MOVE_BULLET_PUNCH},
        .heldItem = ITEM_FLAME_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WHISMUR] = {
        .species = SPECIES_WHISMUR,
        .moves = {MOVE_ICY_WIND, MOVE_FAKE_TEARS, MOVE_ENDEAVOR, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_RATTLED,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ZIGZAGOON] = {
        .species = SPECIES_ZIGZAGOON,
        .moves = {MOVE_BELLY_DRUM, MOVE_EXTREME_SPEED, MOVE_SEED_BOMB, MOVE_PROTECT},
        .heldItem = ITEM_FIGY_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_GLUTTONY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ZUBAT] = {
        .species = SPECIES_ZUBAT,
        .moves = {MOVE_TAILWIND, MOVE_SUPER_FANG, MOVE_TAUNT, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 0, 2),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TOGEPI] = {
        .species = SPECIES_TOGEPI,
        .moves = {MOVE_FOLLOW_ME, MOVE_HELPING_HAND, MOVE_LIFE_DEW, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_SERENE_GRACE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SPINARAK] = {
        .species = SPECIES_SPINARAK,
        .moves = {MOVE_RAGE_POWDER, MOVE_ELECTROWEB, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_INSOMNIA,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MARILL] = {
        .species = SPECIES_MARILL,
        .moves = {MOVE_BELLY_DRUM, MOVE_AQUA_JET, MOVE_PLAY_ROUGH, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_HUGE_POWER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HOPPIP] = {
        .species = SPECIES_HOPPIP,
        .moves = {MOVE_SLEEP_POWDER, MOVE_TAILWIND, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 0, 2),
        .nature = NATURE_TIMID,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SLUGMA] = {
        .species = SPECIES_SLUGMA,
        .moves = {MOVE_HEAT_WAVE, MOVE_CLEAR_SMOG, MOVE_YAWN, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_FLAME_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SWINUB] = {
        .species = SPECIES_SWINUB,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_ICE_SHARD, MOVE_ENDEAVOR, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_OBLIVIOUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SMEARGLE] = {
        .species = SPECIES_SMEARGLE,
        .moves = {MOVE_SPORE, MOVE_FOLLOW_ME, MOVE_FAKE_OUT, MOVE_WIDE_GUARD},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 0, 2),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_MOODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PIDGEY] = {
        .species = SPECIES_PIDGEY,
        .moves = {MOVE_TAILWIND, MOVE_FEATHER_DANCE, MOVE_U_TURN, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_BIG_PECKS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RATTATA] = {
        .species = SPECIES_RATTATA,
        .moves = {MOVE_FACADE, MOVE_SUCKER_PUNCH, MOVE_U_TURN, MOVE_PROTECT},
        .heldItem = ITEM_FLAME_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WYNAUT] = {
        .species = SPECIES_WYNAUT,
        .moves = {MOVE_ENCORE, MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_SAFEGUARD},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 0, 32, 0, 0, 32),
        .nature = NATURE_BOLD,
        .ability = ABILITY_SHADOW_TAG,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SKITTY] = {
        .species = SPECIES_SKITTY,
        .moves = {MOVE_FAKE_OUT, MOVE_ICY_WIND, MOVE_SIMPLE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_WONDER_SKIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SPEAROW] = {
        .species = SPECIES_SPEAROW,
        .moves = {MOVE_DRILL_PECK, MOVE_DRILL_RUN, MOVE_U_TURN, MOVE_QUICK_ATTACK},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SNIPER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HOOTHOOT] = {
        .species = SPECIES_HOOTHOOT,
        .moves = {MOVE_TAILWIND, MOVE_FEATHER_DANCE, MOVE_NIGHT_SHADE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_INSOMNIA,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DIGLETT] = {
        .species = SPECIES_DIGLETT,
        .moves = {MOVE_STOMPING_TANTRUM, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_ARENA_TRAP,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LEDYBA] = {
        .species = SPECIES_LEDYBA,
        .moves = {MOVE_TAILWIND, MOVE_ENCORE, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_RATTLED,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NINCADA] = {
        .species = SPECIES_NINCADA,
        .moves = {MOVE_TOXIC, MOVE_STRING_SHOT, MOVE_SKITTER_SMACK, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_COMPOUND_EYES,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SURSKIT] = {
        .species = SPECIES_SURSKIT,
        .moves = {MOVE_HYDRO_PUMP, MOVE_BUG_BUZZ, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_JIGGLYPUFF] = {
        .species = SPECIES_JIGGLYPUFF,
        .moves = {MOVE_FAKE_OUT, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_FRIEND_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TAILLOW] = {
        .species = SPECIES_TAILLOW,
        .moves = {MOVE_FACADE, MOVE_BRAVE_BIRD, MOVE_QUICK_ATTACK, MOVE_PROTECT},
        .heldItem = ITEM_FLAME_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WINGULL] = {
        .species = SPECIES_WINGULL,
        .moves = {MOVE_HURRICANE, MOVE_MUDDY_WATER, MOVE_TAILWIND, MOVE_REST},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_HYDRATION,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NIDORAN_M] = {
        .species = SPECIES_NIDORAN_M,
        .moves = {MOVE_HONE_CLAWS, MOVE_POISON_JAB, MOVE_DRILL_RUN, MOVE_SUCKER_PUNCH},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_HUSTLE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NIDORAN_F] = {
        .species = SPECIES_NIDORAN_F,
        .moves = {MOVE_SUPER_FANG, MOVE_HELPING_HAND, MOVE_CHARM, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_POISON_POINT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KIRLIA] = {
        .species = SPECIES_KIRLIA,
        .moves = {MOVE_TRICK_ROOM, MOVE_IMPRISON, MOVE_DAZZLING_GLEAM, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_TRACE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MAREEP] = {
        .species = SPECIES_MAREEP,
        .moves = {MOVE_ELECTROWEB, MOVE_EERIE_IMPULSE, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_STATIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MEDITITE] = {
        .species = SPECIES_MEDITITE,
        .moves = {MOVE_FAKE_OUT, MOVE_CLOSE_COMBAT, MOVE_ZEN_HEADBUTT, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_PURE_POWER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SLAKOTH] = {
        .species = SPECIES_SLAKOTH,
        .moves = {MOVE_DOUBLE_EDGE, MOVE_FIRE_PUNCH, MOVE_ICE_PUNCH, MOVE_THROAT_CHOP},
        .heldItem = ITEM_CHOICE_BAND,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_TRUANT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PARAS] = {
        .species = SPECIES_PARAS,
        .moves = {MOVE_SPORE, MOVE_RAGE_POWDER, MOVE_SEED_BOMB, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_SASSY,
        .ability = ABILITY_DRY_SKIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_EKANS] = {
        .species = SPECIES_EKANS,
        .moves = {MOVE_COIL, MOVE_GUNK_SHOT, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DITTO] = {
        .species = SPECIES_DITTO,
        .moves = {MOVE_TRANSFORM},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_RELAXED,
        .ability = ABILITY_IMPOSTER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BARBOACH] = {
        .species = SPECIES_BARBOACH,
        .moves = {MOVE_DRAGON_DANCE, MOVE_HIGH_HORSEPOWER, MOVE_LIQUIDATION, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_OBLIVIOUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MEOWTH] = {
        .species = SPECIES_MEOWTH,
        .moves = {MOVE_FAKE_OUT, MOVE_FEINT, MOVE_DOUBLE_EDGE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_TECHNICIAN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PINECO] = {
        .species = SPECIES_PINECO,
        .moves = {MOVE_LUNGE, MOVE_ROCK_TOMB, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_RELAXED,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TRAPINCH] = {
        .species = SPECIES_TRAPINCH,
        .moves = {MOVE_EARTHQUAKE, MOVE_STONE_EDGE, MOVE_FIRST_IMPRESSION, MOVE_FEINT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 17, 17, 0, 0, 0),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_ARENA_TRAP,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SPHEAL] = {
        .species = SPECIES_SPHEAL,
        .moves = {MOVE_ICY_WIND, MOVE_SUPER_FANG, MOVE_YAWN, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_THICK_FAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HORSEA] = {
        .species = SPECIES_HORSEA,
        .moves = {MOVE_MUDDY_WATER, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SHROOMISH] = {
        .species = SPECIES_SHROOMISH,
        .moves = {MOVE_SPORE, MOVE_SEED_BOMB, MOVE_DRAIN_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_EFFECT_SPORE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SHUPPET] = {
        .species = SPECIES_SHUPPET,
        .moves = {MOVE_TRICK_ROOM, MOVE_DESTINY_BOND, MOVE_WILL_O_WISP, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_RELAXED,
        .ability = ABILITY_INSOMNIA,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DUSKULL] = {
        .species = SPECIES_DUSKULL,
        .moves = {MOVE_TRICK_ROOM, MOVE_NIGHT_SHADE, MOVE_WILL_O_WISP, MOVE_PAIN_SPLIT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_SASSY,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ELECTRIKE] = {
        .species = SPECIES_ELECTRIKE,
        .moves = {MOVE_THUNDER_WAVE, MOVE_THUNDERBOLT, MOVE_FLAMETHROWER, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LIGHTNING_ROD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VULPIX] = {
        .species = SPECIES_VULPIX,
        .moves = {MOVE_HEAT_WAVE, MOVE_ENERGY_BALL, MOVE_WILL_O_WISP, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_DROUGHT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PIKACHU] = {
        .species = SPECIES_PIKACHU,
        .moves = {MOVE_FAKE_OUT, MOVE_THUNDERBOLT, MOVE_GRASS_KNOT, MOVE_PROTECT},
        .heldItem = ITEM_LIGHT_BALL,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LIGHTNING_ROD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SANDSHREW] = {
        .species = SPECIES_SANDSHREW,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SAND_RUSH,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_POLIWAG] = {
        .species = SPECIES_POLIWAG,
        .moves = {MOVE_HYDRO_PUMP, MOVE_MUDDY_WATER, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BELLSPROUT] = {
        .species = SPECIES_BELLSPROUT,
        .moves = {MOVE_GROWTH, MOVE_SOLAR_BEAM, MOVE_SLUDGE_BOMB, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GEODUDE] = {
        .species = SPECIES_GEODUDE,
        .moves = {MOVE_STEALTH_ROCK, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DRATINI] = {
        .species = SPECIES_DRATINI,
        .moves = {MOVE_DRAGON_DANCE, MOVE_SCALE_SHOT, MOVE_IRON_HEAD, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SHED_SKIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SNUBBULL] = {
        .species = SPECIES_SNUBBULL,
        .moves = {MOVE_PLAY_ROUGH, MOVE_SUPER_FANG, MOVE_THUNDER_WAVE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REMORAID] = {
        .species = SPECIES_REMORAID,
        .moves = {MOVE_FOCUS_ENERGY, MOVE_WATER_SPOUT, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_SCOPE_LENS,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_SNIPER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LARVITAR] = {
        .species = SPECIES_LARVITAR,
        .moves = {MOVE_DRAGON_DANCE, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BALTOY] = {
        .species = SPECIES_BALTOY,
        .moves = {MOVE_TRICK_ROOM, MOVE_ALLY_SWITCH, MOVE_EARTH_POWER, MOVE_PROTECT},
        .heldItem = ITEM_MENTAL_HERB,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_RELAXED,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SNORUNT] = {
        .species = SPECIES_SNORUNT,
        .moves = {MOVE_SPIKES, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 0, 32),
        .nature = NATURE_TIMID,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BAGON] = {
        .species = SPECIES_BAGON,
        .moves = {MOVE_DRAGON_DANCE, MOVE_DRAGON_CLAW, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BELDUM] = {
        .species = SPECIES_BELDUM,
        .moves = {MOVE_IRON_HEAD, MOVE_ZEN_HEADBUTT, MOVE_AGILITY, MOVE_IRON_DEFENSE},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GULPIN] = {
        .species = SPECIES_GULPIN,
        .moves = {MOVE_ENCORE, MOVE_YAWN, MOVE_ACID_SPRAY, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_STICKY_HOLD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VENONAT] = {
        .species = SPECIES_VENONAT,
        .moves = {MOVE_SLEEP_POWDER, MOVE_RAGE_POWDER, MOVE_ACID_SPRAY, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_BOLD,
        .ability = ABILITY_COMPOUND_EYES,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MANKEY] = {
        .species = SPECIES_MANKEY,
        .moves = {MOVE_CLOSE_COMBAT, MOVE_U_TURN, MOVE_GUNK_SHOT, MOVE_ICE_PUNCH},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_DEFIANT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MACHOP] = {
        .species = SPECIES_MACHOP,
        .moves = {MOVE_DYNAMIC_PUNCH, MOVE_ROCK_SLIDE, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_NO_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SHELLDER] = {
        .species = SPECIES_SHELLDER,
        .moves = {MOVE_SHELL_SMASH, MOVE_ICICLE_SPEAR, MOVE_ROCK_BLAST, MOVE_PROTECT},
        .heldItem = ITEM_WHITE_HERB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SKILL_LINK,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SMOOCHUM] = {
        .species = SPECIES_SMOOCHUM,
        .moves = {MOVE_FAKE_OUT, MOVE_FAKE_TEARS, MOVE_ICY_WIND, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_OBLIVIOUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NUMEL] = {
        .species = SPECIES_NUMEL,
        .moves = {MOVE_GROWTH, MOVE_FLAME_CHARGE, MOVE_EARTH_POWER, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SIMPLE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CARVANHA] = {
        .species = SPECIES_CARVANHA,
        .moves = {MOVE_PROTECT, MOVE_LIQUIDATION, MOVE_CRUNCH, MOVE_PSYCHIC_FANGS},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SPEED_BOOST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CORPHISH] = {
        .species = SPECIES_CORPHISH,
        .moves = {MOVE_DRAGON_DANCE, MOVE_CRABHAMMER, MOVE_AQUA_JET, MOVE_KNOCK_OFF},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_ADAPTABILITY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CHARMANDER] = {
        .species = SPECIES_CHARMANDER,
        .moves = {MOVE_HEAT_WAVE, MOVE_WEATHER_BALL, MOVE_DRAGON_PULSE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_SOLAR_POWER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CYNDAQUIL] = {
        .species = SPECIES_CYNDAQUIL,
        .moves = {MOVE_ERUPTION, MOVE_HEAT_WAVE, MOVE_EXTRASENSORY, MOVE_NATURE_POWER},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FLASH_FIRE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ABRA] = {
        .species = SPECIES_ABRA,
        .moves = {MOVE_PSYCHIC, MOVE_DAZZLING_GLEAM, MOVE_COUNTER, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_MAGIC_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DODUO] = {
        .species = SPECIES_DODUO,
        .moves = {MOVE_BRAVE_BIRD, MOVE_JUMP_KICK, MOVE_KNOCK_OFF, MOVE_QUICK_ATTACK},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_EARLY_BIRD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GASTLY] = {
        .species = SPECIES_GASTLY,
        .moves = {MOVE_SHADOW_BALL, MOVE_SLUDGE_BOMB, MOVE_ICY_WIND, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SWABLU] = {
        .species = SPECIES_SWABLU,
        .moves = {MOVE_TAILWIND, MOVE_HELPING_HAND, MOVE_FEATHER_DANCE, MOVE_ROOST},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_NATURAL_CURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TREECKO] = {
        .species = SPECIES_TREECKO,
        .moves = {MOVE_SWORDS_DANCE, MOVE_BULLET_SEED, MOVE_DRAIN_PUNCH, MOVE_ACROBATICS},
        .heldItem = ITEM_BERRY_JUICE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_UNBURDEN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TORCHIC] = {
        .species = SPECIES_TORCHIC,
        .moves = {MOVE_PROTECT, MOVE_HELPING_HAND, MOVE_FEATHER_DANCE, MOVE_WILL_O_WISP},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_SPEED_BOOST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MUDKIP] = {
        .species = SPECIES_MUDKIP,
        .moves = {MOVE_WIDE_GUARD, MOVE_LIQUIDATION, MOVE_ICY_WIND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_DAMP,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SQUIRTLE] = {
        .species = SPECIES_SQUIRTLE,
        .moves = {MOVE_FOLLOW_ME, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_TORRENT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TOTODILE] = {
        .species = SPECIES_TOTODILE,
        .moves = {MOVE_LIQUIDATION, MOVE_ICE_PUNCH, MOVE_CRUNCH, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SLOWPOKE] = {
        .species = SPECIES_SLOWPOKE,
        .moves = {MOVE_TRICK_ROOM, MOVE_SCALD, MOVE_HEAL_PULSE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_RELAXED,
        .ability = ABILITY_OBLIVIOUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BULBASAUR] = {
        .species = SPECIES_BULBASAUR,
        .moves = {MOVE_GROWTH, MOVE_SLEEP_POWDER, MOVE_GIGA_DRAIN, MOVE_SLUDGE_BOMB},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CHIKORITA] = {
        .species = SPECIES_CHIKORITA,
        .moves = {MOVE_HEAL_PULSE, MOVE_HELPING_HAND, MOVE_LIGHT_SCREEN, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_OVERGROW,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ODDISH] = {
        .species = SPECIES_ODDISH,
        .moves = {MOVE_GROWTH, MOVE_SOLAR_BEAM, MOVE_SLUDGE_BOMB, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PSYDUCK] = {
        .species = SPECIES_PSYDUCK,
        .moves = {MOVE_HYDRO_PUMP, MOVE_MUDDY_WATER, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CUBONE] = {
        .species = SPECIES_CUBONE,
        .moves = {MOVE_BONEMERANG, MOVE_ROCK_SLIDE, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_THICK_CLUB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_LIGHTNING_ROD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GOLDEEN] = {
        .species = SPECIES_GOLDEEN,
        .moves = {MOVE_WATERFALL, MOVE_ICY_WIND, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_LIGHTNING_ROD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NATU] = {
        .species = SPECIES_NATU,
        .moves = {MOVE_PSYCHIC, MOVE_HEAT_WAVE, MOVE_TAILWIND, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_MAGIC_BOUNCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CLEFAIRY] = {
        .species = SPECIES_CLEFAIRY,
        .moves = {MOVE_FOLLOW_ME, MOVE_HELPING_HAND, MOVE_ICY_WIND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_FRIEND_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MAGNEMITE] = {
        .species = SPECIES_MAGNEMITE,
        .moves = {MOVE_ELECTROWEB, MOVE_THUNDERBOLT, MOVE_FLASH_CANNON, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SEEL] = {
        .species = SPECIES_SEEL,
        .moves = {MOVE_FAKE_OUT, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_THICK_FAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GRIMER] = {
        .species = SPECIES_GRIMER,
        .moves = {MOVE_POISON_JAB, MOVE_KNOCK_OFF, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 0, 0, 0, 2),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_POISON_TOUCH,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KRABBY] = {
        .species = SPECIES_KRABBY,
        .moves = {MOVE_LIQUIDATION, MOVE_KNOCK_OFF, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_EXEGGCUTE] = {
        .species = SPECIES_EXEGGCUTE,
        .moves = {MOVE_SOLAR_BEAM, MOVE_PSYCHIC, MOVE_SLEEP_POWDER, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_EEVEE] = {
        .species = SPECIES_EEVEE,
        .moves = {MOVE_HELPING_HAND, MOVE_YAWN, MOVE_WISH, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_ANTICIPATION,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DROWZEE] = {
        .species = SPECIES_DROWZEE,
        .moves = {MOVE_TRICK_ROOM, MOVE_HELPING_HAND, MOVE_NIGHT_SHADE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_SASSY,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VOLTORB] = {
        .species = SPECIES_VOLTORB,
        .moves = {MOVE_REFLECT, MOVE_LIGHT_SCREEN, MOVE_VOLT_SWITCH, MOVE_TAUNT},
        .heldItem = ITEM_LIGHT_CLAY,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_AFTERMATH,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CHINCHOU] = {
        .species = SPECIES_CHINCHOU,
        .moves = {MOVE_SCALD, MOVE_ELECTROWEB, MOVE_HEAL_BELL, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_VOLT_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TEDDIURSA] = {
        .species = SPECIES_TEDDIURSA,
        .moves = {MOVE_FACADE, MOVE_CLOSE_COMBAT, MOVE_CRUNCH, MOVE_PROTECT},
        .heldItem = ITEM_TOXIC_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_QUICK_FEET,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DELIBIRD] = {
        .species = SPECIES_DELIBIRD,
        .moves = {MOVE_FAKE_OUT, MOVE_TAILWIND, MOVE_ICY_WIND, MOVE_DESTINY_BOND},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_INSOMNIA,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HOUNDOUR] = {
        .species = SPECIES_HOUNDOUR,
        .moves = {MOVE_HEAT_WAVE, MOVE_DARK_PULSE, MOVE_SNARL, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FLASH_FIRE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PHANPY] = {
        .species = SPECIES_PHANPY,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_KNOCK_OFF, MOVE_ICE_SHARD, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 17, 17, 0, 0, 0),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_PICKUP,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SPOINK] = {
        .species = SPECIES_SPOINK,
        .moves = {MOVE_REFLECT, MOVE_LIGHT_SCREEN, MOVE_ICY_WIND, MOVE_PSYCHIC},
        .heldItem = ITEM_LIGHT_CLAY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_THICK_FAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ARON] = {
        .species = SPECIES_ARON,
        .moves = {MOVE_METAL_BURST, MOVE_ENDEAVOR, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_CUSTAP_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LUVDISC] = {
        .species = SPECIES_LUVDISC,
        .moves = {MOVE_ICY_WIND, MOVE_SCALD, MOVE_DRAINING_KISS, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_SOUL_HEART,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TENTACOOL] = {
        .species = SPECIES_TENTACOOL,
        .moves = {MOVE_ACID_SPRAY, MOVE_ICY_WIND, MOVE_SCALD, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CACNEA] = {
        .species = SPECIES_CACNEA,
        .moves = {MOVE_SWORDS_DANCE, MOVE_SUCKER_PUNCH, MOVE_SEED_BOMB, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_WATER_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_UNOWN] = {
        .species = SPECIES_UNOWN,
        .moves = {MOVE_HIDDEN_POWER, MOVE_STORED_POWER},
        .heldItem = ITEM_CHOICE_SPECS,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KOFFING] = {
        .species = SPECIES_KOFFING,
        .moves = {MOVE_SLUDGE_BOMB, MOVE_WILL_O_WISP, MOVE_HAZE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 2, 0),
        .nature = NATURE_BOLD,
        .ability = ABILITY_NEUTRALIZING_GAS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_STARYU] = {
        .species = SPECIES_STARYU,
        .moves = {MOVE_SCALD, MOVE_ICY_WIND, MOVE_RECOVER, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_BOLD,
        .ability = ABILITY_NATURAL_CURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SKIPLOOM] = {
        .species = SPECIES_SKIPLOOM,
        .moves = {MOVE_SLEEP_POWDER, MOVE_TAILWIND, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 0, 2),
        .nature = NATURE_TIMID,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NUZLEAF] = {
        .species = SPECIES_NUZLEAF,
        .moves = {MOVE_FAKE_OUT, MOVE_KNOCK_OFF, MOVE_SNARL, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 2, 0, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_EARLY_BIRD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LOMBRE] = {
        .species = SPECIES_LOMBRE,
        .moves = {MOVE_FAKE_OUT, MOVE_HYDRO_PUMP, MOVE_GIGA_DRAIN, MOVE_ICE_BEAM},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VIBRAVA] = {
        .species = SPECIES_VIBRAVA,
        .moves = {MOVE_TAILWIND, MOVE_EARTH_POWER, MOVE_STRUGGLE_BUG, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RHYHORN] = {
        .species = SPECIES_RHYHORN,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_ICE_FANG, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_LIGHTNING_ROD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CLAMPERL] = {
        .species = SPECIES_CLAMPERL,
        .moves = {MOVE_SHELL_SMASH, MOVE_MUDDY_WATER, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_WHITE_HERB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SHELL_ARMOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PIDGEOTTO] = {
        .species = SPECIES_PIDGEOTTO,
        .moves = {MOVE_BRAVE_BIRD, MOVE_TAILWIND, MOVE_FEATHER_DANCE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_BIG_PECKS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GROWLITHE] = {
        .species = SPECIES_GROWLITHE,
        .moves = {MOVE_SNARL, MOVE_WILL_O_WISP, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FARFETCHD] = {
        .species = SPECIES_FARFETCHD,
        .moves = {MOVE_TAILWIND, MOVE_HELPING_HAND, MOVE_BRAVE_BIRD, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_OMANYTE] = {
        .species = SPECIES_OMANYTE,
        .moves = {MOVE_MUDDY_WATER, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KABUTO] = {
        .species = SPECIES_KABUTO,
        .moves = {MOVE_WATERFALL, MOVE_ROCK_SLIDE, MOVE_AQUA_JET, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LILEEP] = {
        .species = SPECIES_LILEEP,
        .moves = {MOVE_GIGA_DRAIN, MOVE_ANCIENT_POWER, MOVE_RECOVER, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_STORM_DRAIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ANORITH] = {
        .species = SPECIES_ANORITH,
        .moves = {MOVE_X_SCISSOR, MOVE_ROCK_SLIDE, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_AIPOM] = {
        .species = SPECIES_AIPOM,
        .moves = {MOVE_FAKE_OUT, MOVE_TAIL_SLAP, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SKILL_LINK,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ELEKID] = {
        .species = SPECIES_ELEKID,
        .moves = {MOVE_FOLLOW_ME, MOVE_ELECTROWEB, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_BOLD,
        .ability = ABILITY_STATIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LOUDRED] = {
        .species = SPECIES_LOUDRED,
        .moves = {MOVE_HYPER_VOICE, MOVE_FIRE_BLAST, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SCRAPPY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SPINDA] = {
        .species = SPECIES_SPINDA,
        .moves = {MOVE_SUPERPOWER, MOVE_FAKE_OUT, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_CONTRARY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NIDORINA] = {
        .species = SPECIES_NIDORINA,
        .moves = {MOVE_SUPER_FANG, MOVE_DISABLE, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_POISON_POINT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NIDORINO] = {
        .species = SPECIES_NIDORINO,
        .moves = {MOVE_HONE_CLAWS, MOVE_POISON_JAB, MOVE_DRILL_RUN, MOVE_SUCKER_PUNCH},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_HUSTLE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FLAAFFY] = {
        .species = SPECIES_FLAAFFY,
        .moves = {MOVE_ELECTROWEB, MOVE_EERIE_IMPULSE, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_STATIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MAGBY] = {
        .species = SPECIES_MAGBY,
        .moves = {MOVE_FOLLOW_ME, MOVE_HELPING_HAND, MOVE_WILL_O_WISP, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_FLAME_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NOSEPASS] = {
        .species = SPECIES_NOSEPASS,
        .moves = {MOVE_BODY_PRESS, MOVE_THUNDER_WAVE, MOVE_WIDE_GUARD, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CORSOLA] = {
        .species = SPECIES_CORSOLA,
        .moves = {MOVE_SCALD, MOVE_ICY_WIND, MOVE_LIFE_DEW, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_REGENERATOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MAWILE] = {
        .species = SPECIES_MAWILE,
        .moves = {MOVE_PLAY_ROUGH, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BUTTERFREE] = {
        .species = SPECIES_BUTTERFREE,
        .moves = {MOVE_SLEEP_POWDER, MOVE_RAGE_POWDER, MOVE_TAILWIND, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_COMPOUND_EYES,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BEEDRILL] = {
        .species = SPECIES_BEEDRILL,
        .moves = {MOVE_U_TURN, MOVE_POISON_JAB, MOVE_DRILL_RUN, MOVE_KNOCK_OFF},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SWARM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_POLIWHIRL] = {
        .species = SPECIES_POLIWHIRL,
        .moves = {MOVE_MUDDY_WATER, MOVE_ICE_BEAM, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ONIX] = {
        .species = SPECIES_ONIX,
        .moves = {MOVE_ROCK_SLIDE, MOVE_EARTHQUAKE, MOVE_WIDE_GUARD, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BEAUTIFLY] = {
        .species = SPECIES_BEAUTIFLY,
        .moves = {MOVE_QUIVER_DANCE, MOVE_BUG_BUZZ, MOVE_AIR_CUTTER, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_BERSERK,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DUSTOX] = {
        .species = SPECIES_DUSTOX,
        .moves = {MOVE_QUIVER_DANCE, MOVE_BUG_BUZZ, MOVE_SLUDGE_BOMB, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_UNAWARE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LEDIAN] = {
        .species = SPECIES_LEDIAN,
        .moves = {MOVE_TAILWIND, MOVE_ENCORE, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_COVERT_CLOAK,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_EARLY_BIRD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ARIADOS] = {
        .species = SPECIES_ARIADOS,
        .moves = {MOVE_RAGE_POWDER, MOVE_ELECTROWEB, MOVE_LUNGE, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_INSOMNIA,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_YANMA] = {
        .species = SPECIES_YANMA,
        .moves = {MOVE_HYPNOSIS, MOVE_BUG_BUZZ, MOVE_AIR_SLASH, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_COMPOUND_EYES,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DELCATTY_1] = {
        .species = SPECIES_DELCATTY,
        .moves = {MOVE_FAKE_OUT, MOVE_THUNDER_WAVE, MOVE_SUCKER_PUNCH, MOVE_DOUBLE_EDGE},
        .heldItem = ITEM_SILK_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_NORMALIZE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SABLEYE_1] = {
        .species = SPECIES_SABLEYE,
        .moves = {MOVE_WILL_O_WISP, MOVE_QUASH, MOVE_FAKE_OUT, MOVE_FOUL_PLAY},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 2, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_PRANKSTER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LICKITUNG_1] = {
        .species = SPECIES_LICKITUNG,
        .moves = {MOVE_BODY_SLAM, MOVE_KNOCK_OFF, MOVE_DISABLE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_CLOUD_NINE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WEEPINBELL_1] = {
        .species = SPECIES_WEEPINBELL,
        .moves = {MOVE_GROWTH, MOVE_GIGA_DRAIN, MOVE_SLUDGE_BOMB, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GRAVELER_1] = {
        .species = SPECIES_GRAVELER,
        .moves = {MOVE_WIDE_GUARD, MOVE_ROCK_SLIDE, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GLOOM_1] = {
        .species = SPECIES_GLOOM,
        .moves = {MOVE_GROWTH, MOVE_SOLAR_BEAM, MOVE_SLUDGE_BOMB, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PORYGON_1] = {
        .species = SPECIES_PORYGON,
        .moves = {MOVE_TRICK_ROOM, MOVE_TRI_ATTACK, MOVE_ICE_BEAM, MOVE_RECOVER},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_QUIET,
        .ability = ABILITY_DOWNLOAD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KADABRA_1] = {
        .species = SPECIES_KADABRA,
        .moves = {MOVE_PSYCHIC, MOVE_DAZZLING_GLEAM, MOVE_COUNTER, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_MAGIC_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WAILMER_1] = {
        .species = SPECIES_WAILMER,
        .moves = {MOVE_WATER_SPOUT, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_ICY_WIND},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_WATER_VEIL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ROSELIA_1] = {
        .species = SPECIES_ROSELIA,
        .moves = {MOVE_SPIKES, MOVE_TOXIC_SPIKES, MOVE_GIGA_DRAIN, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_NATURAL_CURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VOLBEAT_1] = {
        .species = SPECIES_VOLBEAT,
        .moves = {MOVE_TAILWIND, MOVE_ENCORE, MOVE_THUNDER_WAVE, MOVE_PROTECT},
        .heldItem = ITEM_COVERT_CLOAK,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_PRANKSTER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ILLUMISE_1] = {
        .species = SPECIES_ILLUMISE,
        .moves = {MOVE_TAILWIND, MOVE_FAKE_TEARS, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_COVERT_CLOAK,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_PRANKSTER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_IVYSAUR_1] = {
        .species = SPECIES_IVYSAUR,
        .moves = {MOVE_GROWTH, MOVE_SOLAR_BEAM, MOVE_SLUDGE_BOMB, MOVE_WEATHER_BALL},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CHARMELEON_1] = {
        .species = SPECIES_CHARMELEON,
        .moves = {MOVE_HEAT_WAVE, MOVE_WEATHER_BALL, MOVE_ANCIENT_POWER, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_SOLAR_POWER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WARTORTLE_1] = {
        .species = SPECIES_WARTORTLE,
        .moves = {MOVE_SHELL_SMASH, MOVE_MUDDY_WATER, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_TORRENT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PARASECT_1] = {
        .species = SPECIES_PARASECT,
        .moves = {MOVE_SPORE, MOVE_RAGE_POWDER, MOVE_SEED_BOMB, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_SASSY,
        .ability = ABILITY_DRY_SKIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MACHOKE_1] = {
        .species = SPECIES_MACHOKE,
        .moves = {MOVE_DYNAMIC_PUNCH, MOVE_ROCK_SLIDE, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_NO_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HAUNTER_1] = {
        .species = SPECIES_HAUNTER,
        .moves = {MOVE_SHADOW_BALL, MOVE_SLUDGE_BOMB, MOVE_DAZZLING_GLEAM, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BAYLEEF_1] = {
        .species = SPECIES_BAYLEEF,
        .moves = {MOVE_HEAL_PULSE, MOVE_HELPING_HAND, MOVE_LIGHT_SCREEN, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_OVERGROW,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_QUILAVA_1] = {
        .species = SPECIES_QUILAVA,
        .moves = {MOVE_ERUPTION, MOVE_HEAT_WAVE, MOVE_EXTRASENSORY, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FLASH_FIRE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CROCONAW_1] = {
        .species = SPECIES_CROCONAW,
        .moves = {MOVE_DRAGON_DANCE, MOVE_LIQUIDATION, MOVE_ICE_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TOGETIC_1] = {
        .species = SPECIES_TOGETIC,
        .moves = {MOVE_FOLLOW_ME, MOVE_AIR_SLASH, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_SERENE_GRACE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MURKROW_1] = {
        .species = SPECIES_MURKROW,
        .moves = {MOVE_TAILWIND, MOVE_HAZE, MOVE_TAUNT, MOVE_FOUL_PLAY},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_PRANKSTER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WOBBUFFET_1] = {
        .species = SPECIES_WOBBUFFET,
        .moves = {MOVE_ENCORE, MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_SAFEGUARD},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(2, 0, 32, 0, 0, 32),
        .nature = NATURE_BOLD,
        .ability = ABILITY_SHADOW_TAG,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PLUSLE_1] = {
        .species = SPECIES_PLUSLE,
        .moves = {MOVE_THUNDERBOLT, MOVE_ALLURING_VOICE, MOVE_ELECTROWEB, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_PLUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MINUN_1] = {
        .species = SPECIES_MINUN,
        .moves = {MOVE_NASTY_PLOT, MOVE_THUNDERBOLT, MOVE_GRASS_KNOT, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_MINUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GROVYLE_1] = {
        .species = SPECIES_GROVYLE,
        .moves = {MOVE_LEAF_STORM, MOVE_DRAGON_PULSE, MOVE_VACUUM_WAVE, MOVE_PROTECT},
        .heldItem = ITEM_WHITE_HERB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_UNBURDEN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_COMBUSKEN_1] = {
        .species = SPECIES_COMBUSKEN,
        .moves = {MOVE_HEAT_WAVE, MOVE_FOCUS_BLAST, MOVE_FEINT, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_SPEED_BOOST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MARSHTOMP_1] = {
        .species = SPECIES_MARSHTOMP,
        .moves = {MOVE_MUDDY_WATER, MOVE_EARTH_POWER, MOVE_ICY_WIND, MOVE_WIDE_GUARD},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_QUIET,
        .ability = ABILITY_DAMP,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PONYTA_1] = {
        .species = SPECIES_PONYTA,
        .moves = {MOVE_FLARE_BLITZ, MOVE_WILD_CHARGE, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .heldItem = ITEM_BERRY_JUICE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_FLAME_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_AZUMARILL_1] = {
        .species = SPECIES_AZUMARILL,
        .moves = {MOVE_BELLY_DRUM, MOVE_AQUA_JET, MOVE_PLAY_ROUGH, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_HUGE_POWER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SUDOWOODO_1] = {
        .species = SPECIES_SUDOWOODO,
        .moves = {MOVE_HEAD_SMASH, MOVE_WOOD_HAMMER, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_ROCK_HEAD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MAGCARGO_1] = {
        .species = SPECIES_MAGCARGO,
        .moves = {MOVE_SHELL_SMASH, MOVE_HEAT_WAVE, MOVE_POWER_GEM, MOVE_PROTECT},
        .heldItem = ITEM_WHITE_HERB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SIMPLE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PUPITAR_1] = {
        .species = SPECIES_PUPITAR,
        .moves = {MOVE_DRAGON_DANCE, MOVE_ROCK_SLIDE, MOVE_STOMPING_TANTRUM, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SHED_SKIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SEALEO_1] = {
        .species = SPECIES_SEALEO,
        .moves = {MOVE_WATER_PULSE, MOVE_ICE_BEAM, MOVE_ICY_WIND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_THICK_FAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RATICATE_1] = {
        .species = SPECIES_RATICATE,
        .moves = {MOVE_FACADE, MOVE_SUCKER_PUNCH, MOVE_U_TURN, MOVE_PROTECT},
        .heldItem = ITEM_FLAME_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MASQUERAIN_1] = {
        .species = SPECIES_MASQUERAIN,
        .moves = {MOVE_QUIVER_DANCE, MOVE_BUG_BUZZ, MOVE_HURRICANE, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FURRET_1] = {
        .species = SPECIES_FURRET,
        .moves = {MOVE_FOLLOW_ME, MOVE_HELPING_HAND, MOVE_SUPER_FANG, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_FRISK,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DUNSPARCE_1] = {
        .species = SPECIES_DUNSPARCE,
        .moves = {MOVE_GLARE, MOVE_HEADBUTT, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_SERENE_GRACE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DRAGONAIR_1] = {
        .species = SPECIES_DRAGONAIR,
        .moves = {MOVE_DRAGON_DANCE, MOVE_SCALE_SHOT, MOVE_IRON_HEAD, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SHED_SKIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MIGHTYENA_1] = {
        .species = SPECIES_MIGHTYENA,
        .moves = {MOVE_HOWL, MOVE_SNARL, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LINOONE_1] = {
        .species = SPECIES_LINOONE,
        .moves = {MOVE_BELLY_DRUM, MOVE_EXTREME_SPEED, MOVE_SEED_BOMB, MOVE_PROTECT},
        .heldItem = ITEM_FIGY_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_GLUTTONY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CASTFORM_1] = {
        .species = SPECIES_CASTFORM_NORMAL,
        .moves = {MOVE_SUNNY_DAY, MOVE_WEATHER_BALL, MOVE_SOLAR_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_FORECAST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SHELGON_1] = {
        .species = SPECIES_SHELGON,
        .moves = {MOVE_DRAGON_DANCE, MOVE_DRAGON_CLAW, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_OVERCOAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_METANG_1] = {
        .species = SPECIES_METANG,
        .moves = {MOVE_METEOR_MASH, MOVE_ZEN_HEADBUTT, MOVE_BULLET_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_WEAKNESS_POLICY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WIGGLYTUFF_1] = {
        .species = SPECIES_WIGGLYTUFF,
        .moves = {MOVE_HYPER_VOICE, MOVE_DAZZLING_GLEAM, MOVE_FIRE_BLAST, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(0, 0, 32, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_COMPETITIVE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SUNFLORA_1] = {
        .species = SPECIES_SUNFLORA,
        .moves = {MOVE_LEAF_STORM, MOVE_EARTH_POWER, MOVE_OVERHEAT, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_QUIET,
        .ability = ABILITY_EARLY_BIRD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CHIMECHO_1] = {
        .species = SPECIES_CHIMECHO,
        .moves = {MOVE_TRICK_ROOM, MOVE_HELPING_HAND, MOVE_PSYCHIC, MOVE_HEALING_WISH},
        .heldItem = ITEM_MENTAL_HERB,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_QUIET,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GLIGAR_1] = {
        .species = SPECIES_GLIGAR,
        .moves = {MOVE_TAILWIND, MOVE_HIGH_HORSEPOWER, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_IMMUNITY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_QWILFISH_1] = {
        .species = SPECIES_QWILFISH,
        .moves = {MOVE_WATERFALL, MOVE_THUNDER_WAVE, MOVE_HAZE, MOVE_PROTECT},
        .heldItem = ITEM_ROCKY_HELMET,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SNEASEL_1] = {
        .species = SPECIES_SNEASEL,
        .moves = {MOVE_FAKE_OUT, MOVE_KNOCK_OFF, MOVE_ICY_WIND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 2, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PELIPPER_1] = {
        .species = SPECIES_PELIPPER,
        .moves = {MOVE_WEATHER_BALL, MOVE_HURRICANE, MOVE_WIDE_GUARD, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_DRIZZLE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SWELLOW_1] = {
        .species = SPECIES_SWELLOW,
        .moves = {MOVE_FACADE, MOVE_BRAVE_BIRD, MOVE_QUICK_ATTACK, MOVE_PROTECT},
        .heldItem = ITEM_FLAME_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LAIRON_1] = {
        .species = SPECIES_LAIRON,
        .moves = {MOVE_HEAD_SMASH, MOVE_IRON_HEAD, MOVE_STOMPING_TANTRUM, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_ROCK_HEAD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TANGELA_1] = {
        .species = SPECIES_TANGELA,
        .moves = {MOVE_RAGE_POWDER, MOVE_SLEEP_POWDER, MOVE_GIGA_DRAIN, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 2, 0),
        .nature = NATURE_BOLD,
        .ability = ABILITY_REGENERATOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ARBOK_1] = {
        .species = SPECIES_ARBOK,
        .moves = {MOVE_COIL, MOVE_GUNK_SHOT, MOVE_STOMPING_TANTRUM, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PERSIAN_1] = {
        .species = SPECIES_PERSIAN,
        .moves = {MOVE_FAKE_OUT, MOVE_U_TURN, MOVE_TAUNT, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_TECHNICIAN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SEADRA_1] = {
        .species = SPECIES_SEADRA,
        .moves = {MOVE_FOCUS_ENERGY, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_SCOPE_LENS,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_SNIPER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KECLEON_1] = {
        .species = SPECIES_KECLEON,
        .moves = {MOVE_FAKE_OUT, MOVE_THUNDER_WAVE, MOVE_ICY_WIND, MOVE_RECOVER},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_COLOR_CHANGE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VIGOROTH_1] = {
        .species = SPECIES_VIGOROTH,
        .moves = {MOVE_AFTER_YOU, MOVE_HELPING_HAND, MOVE_ENCORE, MOVE_SLACK_OFF},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_VITAL_SPIRIT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LUNATONE_1] = {
        .species = SPECIES_LUNATONE,
        .moves = {MOVE_TRICK_ROOM, MOVE_PSYCHIC, MOVE_POWER_GEM, MOVE_HELPING_HAND},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_QUIET,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SOLROCK_1] = {
        .species = SPECIES_SOLROCK,
        .moves = {MOVE_TRICK_ROOM, MOVE_ROCK_SLIDE, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_MENTAL_HERB,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_SASSY,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NOCTOWL_1] = {
        .species = SPECIES_NOCTOWL,
        .moves = {MOVE_TAILWIND, MOVE_FEATHER_DANCE, MOVE_HYPER_VOICE, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_INSOMNIA,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SANDSLASH_1] = {
        .species = SPECIES_SANDSLASH,
        .moves = {MOVE_SWORDS_DANCE, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SAND_RUSH,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VENOMOTH_1] = {
        .species = SPECIES_VENOMOTH,
        .moves = {MOVE_RAGE_POWDER, MOVE_SLEEP_POWDER, MOVE_ACID_SPRAY, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 0, 2),
        .nature = NATURE_TIMID,
        .ability = ABILITY_SHIELD_DUST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CHANSEY_1] = {
        .species = SPECIES_CHANSEY,
        .moves = {MOVE_HEAL_PULSE, MOVE_LIFE_DEW, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 0, 32, 0, 0, 32),
        .nature = NATURE_BOLD,
        .ability = ABILITY_HEALER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SEAKING_1] = {
        .species = SPECIES_SEAKING,
        .moves = {MOVE_WATERFALL, MOVE_MEGAHORN, MOVE_DRILL_RUN, MOVE_FLIP_TURN},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_LIGHTNING_ROD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_JUMPLUFF_1] = {
        .species = SPECIES_JUMPLUFF,
        .moves = {MOVE_SLEEP_POWDER, MOVE_TAILWIND, MOVE_RAGE_POWDER, MOVE_PROTECT},
        .heldItem = ITEM_COVERT_CLOAK,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 0, 2),
        .nature = NATURE_TIMID,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PILOSWINE_1] = {
        .species = SPECIES_PILOSWINE,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_ICICLE_CRASH, MOVE_ICE_SHARD, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_THICK_FAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GOLBAT_1] = {
        .species = SPECIES_GOLBAT,
        .moves = {MOVE_TAILWIND, MOVE_SUPER_FANG, MOVE_TAUNT, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 0, 2),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PRIMEAPE_1] = {
        .species = SPECIES_PRIMEAPE,
        .moves = {MOVE_BULK_UP, MOVE_DRAIN_PUNCH, MOVE_RAGE_FIST, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 2, 32, 0, 0, 0),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_DEFIANT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HITMONLEE_1] = {
        .species = SPECIES_HITMONLEE,
        .moves = {MOVE_HIGH_JUMP_KICK, MOVE_KNOCK_OFF, MOVE_WIDE_GUARD, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_RECKLESS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HITMONCHAN_1] = {
        .species = SPECIES_HITMONCHAN,
        .moves = {MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_THUNDER_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_BLITZ_BOXER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GIRAFARIG_1] = {
        .species = SPECIES_GIRAFARIG,
        .moves = {MOVE_PSYCHIC_FANGS, MOVE_CRUNCH, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_STRONG_JAW,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HITMONTOP_1] = {
        .species = SPECIES_HITMONTOP,
        .moves = {MOVE_FAKE_OUT, MOVE_CLOSE_COMBAT, MOVE_WIDE_GUARD, MOVE_FEINT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 17, 8, 0, 0, 9),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BANETTE_1] = {
        .species = SPECIES_BANETTE,
        .moves = {MOVE_POLTERGEIST, MOVE_SHADOW_SNEAK, MOVE_GUNK_SHOT, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_FRISK,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NINJASK_1] = {
        .species = SPECIES_NINJASK,
        .moves = {MOVE_SWORDS_DANCE, MOVE_BATON_PASS, MOVE_X_SCISSOR, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_SPEED_BOOST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SEVIPER_1] = {
        .species = SPECIES_SEVIPER,
        .moves = {MOVE_GLARE, MOVE_SLUDGE_BOMB, MOVE_FLAMETHROWER, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_INFILTRATOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ZANGOOSE_1] = {
        .species = SPECIES_ZANGOOSE,
        .moves = {MOVE_KNOCK_OFF, MOVE_FACADE, MOVE_CLOSE_COMBAT, MOVE_PROTECT},
        .heldItem = ITEM_TOXIC_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_TOXIC_BOOST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CAMERUPT_1] = {
        .species = SPECIES_CAMERUPT,
        .moves = {MOVE_HEAT_WAVE, MOVE_EARTH_POWER, MOVE_YAWN, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_QUIET,
        .ability = ABILITY_SOLID_ROCK,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SHARPEDO_1] = {
        .species = SPECIES_SHARPEDO,
        .moves = {MOVE_PROTECT, MOVE_LIQUIDATION, MOVE_CRUNCH, MOVE_ICE_FANG},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SPEED_BOOST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TROPIUS_1] = {
        .species = SPECIES_TROPIUS,
        .moves = {MOVE_DOUBLE_EDGE, MOVE_TAILWIND, MOVE_WIDE_GUARD, MOVE_ROOST},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 32, 0, 2, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_AERILATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MAGNETON_1] = {
        .species = SPECIES_MAGNETON,
        .moves = {MOVE_ELECTROWEB, MOVE_THUNDERBOLT, MOVE_FLASH_CANNON, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MANTINE_1] = {
        .species = SPECIES_MANTINE,
        .moves = {MOVE_WIDE_GUARD, MOVE_TAILWIND, MOVE_SCALD, MOVE_HELPING_HAND},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_WATER_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_STANTLER_1] = {
        .species = SPECIES_STANTLER,
        .moves = {MOVE_PSYSHIELD_BASH, MOVE_THUNDER_WAVE, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ABSOL_1] = {
        .species = SPECIES_ABSOL,
        .moves = {MOVE_KNOCK_OFF, MOVE_PLAY_ROUGH, MOVE_CLOSE_COMBAT, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_JUSTIFIED,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SWALOT_1] = {
        .species = SPECIES_SWALOT,
        .moves = {MOVE_GASTRO_ACID, MOVE_ACID_SPRAY, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_SHUCA_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_LIQUID_OOZE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CRAWDAUNT_1] = {
        .species = SPECIES_CRAWDAUNT,
        .moves = {MOVE_SWORDS_DANCE, MOVE_CRABHAMMER, MOVE_KNOCK_OFF, MOVE_AQUA_JET},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_ADAPTABILITY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PIDGEOT_1] = {
        .species = SPECIES_PIDGEOT,
        .moves = {MOVE_HURRICANE, MOVE_HEAT_WAVE, MOVE_TAILWIND, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_NO_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GRUMPIG_1] = {
        .species = SPECIES_GRUMPIG,
        .moves = {MOVE_TRICK_ROOM, MOVE_PSYCHIC, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_QUIET,
        .ability = ABILITY_THICK_FAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TORKOAL_1] = {
        .species = SPECIES_TORKOAL,
        .moves = {MOVE_ERUPTION, MOVE_HEAT_WAVE, MOVE_EARTH_POWER, MOVE_PROTECT},
        .heldItem = ITEM_CHARCOAL,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_QUIET,
        .ability = ABILITY_DROUGHT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KINGLER_1] = {
        .species = SPECIES_KINGLER,
        .moves = {MOVE_CRABHAMMER, MOVE_KNOCK_OFF, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CACTURNE_1] = {
        .species = SPECIES_CACTURNE,
        .moves = {MOVE_SUCKER_PUNCH, MOVE_SEED_BOMB, MOVE_LOW_KICK, MOVE_SPIKY_SHIELD},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_WATER_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BELLOSSOM_1] = {
        .species = SPECIES_BELLOSSOM,
        .moves = {MOVE_GROWTH, MOVE_SOLAR_BEAM, MOVE_MOONBLAST, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_OCTILLERY_1] = {
        .species = SPECIES_OCTILLERY,
        .moves = {MOVE_FOCUS_ENERGY, MOVE_WATER_SPOUT, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_SCOPE_LENS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SNIPER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HUNTAIL_1] = {
        .species = SPECIES_HUNTAIL,
        .moves = {MOVE_SHELL_SMASH, MOVE_WATERFALL, MOVE_CRUNCH, MOVE_PROTECT},
        .heldItem = ITEM_WHITE_HERB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_WATER_VEIL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GOREBYSS_1] = {
        .species = SPECIES_GOREBYSS,
        .moves = {MOVE_SHELL_SMASH, MOVE_MUDDY_WATER, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_WHITE_HERB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RELICANTH_1] = {
        .species = SPECIES_RELICANTH,
        .moves = {MOVE_HEAD_SMASH, MOVE_LIQUIDATION, MOVE_STOMPING_TANTRUM, MOVE_PROTECT},
        .heldItem = ITEM_CLEAR_AMULET,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_ROCK_HEAD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_OMASTAR_1] = {
        .species = SPECIES_OMASTAR,
        .moves = {MOVE_MUDDY_WATER, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KABUTOPS_1] = {
        .species = SPECIES_KABUTOPS,
        .moves = {MOVE_SWORDS_DANCE, MOVE_WATERFALL, MOVE_STONE_EDGE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_POLIWRATH_1] = {
        .species = SPECIES_POLIWRATH,
        .moves = {MOVE_LIQUIDATION, MOVE_CLOSE_COMBAT, MOVE_ICE_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SCYTHER_1] = {
        .species = SPECIES_SCYTHER,
        .moves = {MOVE_TAILWIND, MOVE_DUAL_WINGBEAT, MOVE_U_TURN, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_TECHNICIAN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PINSIR_1] = {
        .species = SPECIES_PINSIR,
        .moves = {MOVE_SWORDS_DANCE, MOVE_X_SCISSOR, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_HYPER_CUTTER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_POLITOED_1] = {
        .species = SPECIES_POLITOED,
        .moves = {MOVE_WEATHER_BALL, MOVE_PERISH_SONG, MOVE_ENCORE, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(31, 0, 23, 0, 0, 12),
        .nature = NATURE_CALM,
        .ability = ABILITY_DRIZZLE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CLOYSTER_1] = {
        .species = SPECIES_CLOYSTER,
        .moves = {MOVE_SHELL_SMASH, MOVE_ICICLE_SPEAR, MOVE_ROCK_BLAST, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SKILL_LINK,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DELCATTY_2] = {
        .species = SPECIES_DELCATTY,
        .moves = {MOVE_FAKE_OUT, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_HEAL_BELL},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_WONDER_SKIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SABLEYE_2] = {
        .species = SPECIES_SABLEYE,
        .moves = {MOVE_PARTING_SHOT, MOVE_ENCORE, MOVE_HELPING_HAND, MOVE_FOUL_PLAY},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_PRANKSTER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LICKITUNG_2] = {
        .species = SPECIES_LICKITUNG,
        .moves = {MOVE_HELPING_HAND, MOVE_ICY_WIND, MOVE_SEISMIC_TOSS, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_OBLIVIOUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WEEPINBELL_2] = {
        .species = SPECIES_WEEPINBELL,
        .moves = {MOVE_SWORDS_DANCE, MOVE_POWER_WHIP, MOVE_POISON_JAB, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GRAVELER_2] = {
        .species = SPECIES_GRAVELER,
        .moves = {MOVE_ROCK_SLIDE, MOVE_HIGH_HORSEPOWER, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GLOOM_2] = {
        .species = SPECIES_GLOOM,
        .moves = {MOVE_GIGA_DRAIN, MOVE_SLUDGE_BOMB, MOVE_STRENGTH_SAP, MOVE_SLEEP_POWDER},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_STENCH,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PORYGON_2] = {
        .species = SPECIES_PORYGON,
        .moves = {MOVE_EERIE_IMPULSE, MOVE_ICY_WIND, MOVE_RECOVER, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_TRACE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KADABRA_2] = {
        .species = SPECIES_KADABRA,
        .moves = {MOVE_EXPANDING_FORCE, MOVE_DAZZLING_GLEAM, MOVE_SHADOW_BALL, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_MAGIC_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WAILMER_2] = {
        .species = SPECIES_WAILMER,
        .moves = {MOVE_SCALD, MOVE_ICY_WIND, MOVE_CLEAR_SMOG, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_OBLIVIOUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ROSELIA_2] = {
        .species = SPECIES_ROSELIA,
        .moves = {MOVE_SLEEP_POWDER, MOVE_LEAF_STORM, MOVE_SLUDGE_BOMB, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_NATURAL_CURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VOLBEAT_2] = {
        .species = SPECIES_VOLBEAT,
        .moves = {MOVE_TAIL_GLOW, MOVE_BATON_PASS, MOVE_ENCORE, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_PRANKSTER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ILLUMISE_2] = {
        .species = SPECIES_ILLUMISE,
        .moves = {MOVE_WISH, MOVE_AROMATHERAPY, MOVE_ENCORE, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_PRANKSTER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_IVYSAUR_2] = {
        .species = SPECIES_IVYSAUR,
        .moves = {MOVE_SLEEP_POWDER, MOVE_KNOCK_OFF, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_OVERGROW,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CHARMELEON_2] = {
        .species = SPECIES_CHARMELEON,
        .moves = {MOVE_FIRE_BLAST, MOVE_WEATHER_BALL, MOVE_ANCIENT_POWER, MOVE_OVERHEAT},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SOLAR_POWER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WARTORTLE_2] = {
        .species = SPECIES_WARTORTLE,
        .moves = {MOVE_FAKE_OUT, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_HAZE},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_TORRENT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PARASECT_2] = {
        .species = SPECIES_PARASECT,
        .moves = {MOVE_SPORE, MOVE_WIDE_GUARD, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_MENTAL_HERB,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_SASSY,
        .ability = ABILITY_DAMP,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MACHOKE_2] = {
        .species = SPECIES_MACHOKE,
        .moves = {MOVE_CLOSE_COMBAT, MOVE_FACADE, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_FLAME_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HAUNTER_2] = {
        .species = SPECIES_HAUNTER,
        .moves = {MOVE_SHADOW_BALL, MOVE_SLUDGE_BOMB, MOVE_ICY_WIND, MOVE_TRICK},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BAYLEEF_2] = {
        .species = SPECIES_BAYLEEF,
        .moves = {MOVE_REFLECT, MOVE_LIGHT_SCREEN, MOVE_GIGA_DRAIN, MOVE_PROTECT},
        .heldItem = ITEM_LIGHT_CLAY,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 2, 0),
        .nature = NATURE_BOLD,
        .ability = ABILITY_OVERGROW,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_QUILAVA_2] = {
        .species = SPECIES_QUILAVA,
        .moves = {MOVE_ENDURE, MOVE_HEAT_WAVE, MOVE_EXTRASENSORY, MOVE_PROTECT},
        .heldItem = ITEM_PETAYA_BERRY,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_BLAZE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CROCONAW_2] = {
        .species = SPECIES_CROCONAW,
        .moves = {MOVE_SWORDS_DANCE, MOVE_LIQUIDATION, MOVE_AQUA_JET, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TOGETIC_2] = {
        .species = SPECIES_TOGETIC,
        .moves = {MOVE_FOLLOW_ME, MOVE_TAILWIND, MOVE_AIR_SLASH, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_SERENE_GRACE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MURKROW_2] = {
        .species = SPECIES_MURKROW,
        .moves = {MOVE_QUASH, MOVE_THUNDER_WAVE, MOVE_SNARL, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_PRANKSTER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WOBBUFFET_2] = {
        .species = SPECIES_WOBBUFFET,
        .moves = {MOVE_ENCORE, MOVE_COUNTER, MOVE_MIRROR_COAT, MOVE_DESTINY_BOND},
        .heldItem = ITEM_CUSTAP_BERRY,
        .ev = TRAINER_PARTY_EVS(2, 0, 32, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_SHADOW_TAG,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PLUSLE_2] = {
        .species = SPECIES_PLUSLE,
        .moves = {MOVE_NUZZLE, MOVE_ENCORE, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LIGHTNING_ROD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MINUN_2] = {
        .species = SPECIES_MINUN,
        .moves = {MOVE_NUZZLE, MOVE_ELECTROWEB, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 0, 2),
        .nature = NATURE_TIMID,
        .ability = ABILITY_VOLT_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GROVYLE_2] = {
        .species = SPECIES_GROVYLE,
        .moves = {MOVE_BREAKING_SWIPE, MOVE_QUICK_GUARD, MOVE_HELPING_HAND, MOVE_GIGA_DRAIN},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_OVERGROW,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_COMBUSKEN_2] = {
        .species = SPECIES_COMBUSKEN,
        .moves = {MOVE_COACHING, MOVE_HELPING_HAND, MOVE_CLOSE_COMBAT, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 2, 0, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_SPEED_BOOST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MARSHTOMP_2] = {
        .species = SPECIES_MARSHTOMP,
        .moves = {MOVE_CURSE, MOVE_LIQUIDATION, MOVE_EARTHQUAKE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_TORRENT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PONYTA_2] = {
        .species = SPECIES_PONYTA,
        .moves = {MOVE_WILL_O_WISP, MOVE_MYSTICAL_FIRE, MOVE_ALLY_SWITCH, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FLAME_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_AZUMARILL_2] = {
        .species = SPECIES_AZUMARILL,
        .moves = {MOVE_LIQUIDATION, MOVE_PLAY_ROUGH, MOVE_AQUA_JET, MOVE_KNOCK_OFF},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_HUGE_POWER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SUDOWOODO_2] = {
        .species = SPECIES_SUDOWOODO,
        .moves = {MOVE_ROCK_SLIDE, MOVE_HIGH_HORSEPOWER, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_WEAKNESS_POLICY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MAGCARGO_2] = {
        .species = SPECIES_MAGCARGO,
        .moves = {MOVE_AMNESIA, MOVE_RECOVER, MOVE_LAVA_PLUME, MOVE_YAWN},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_SIMPLE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PUPITAR_2] = {
        .species = SPECIES_PUPITAR,
        .moves = {MOVE_IRON_DEFENSE, MOVE_ROCK_SLIDE, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 17, 17, 0, 0, 0),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_SHED_SKIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SEALEO_2] = {
        .species = SPECIES_SEALEO,
        .moves = {MOVE_ENCORE, MOVE_SUPER_FANG, MOVE_YAWN, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_OBLIVIOUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RATICATE_2] = {
        .species = SPECIES_RATICATE,
        .moves = {MOVE_SWORDS_DANCE, MOVE_FACADE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_FLAME_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MASQUERAIN_2] = {
        .species = SPECIES_MASQUERAIN,
        .moves = {MOVE_QUIVER_DANCE, MOVE_BATON_PASS, MOVE_ICY_WIND, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FURRET_2] = {
        .species = SPECIES_FURRET,
        .moves = {MOVE_TIDY_UP, MOVE_DOUBLE_EDGE, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_FRISK,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DUNSPARCE_2] = {
        .species = SPECIES_DUNSPARCE,
        .moves = {MOVE_HELPING_HAND, MOVE_GLARE, MOVE_BREAKING_SWIPE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_SERENE_GRACE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DRAGONAIR_2] = {
        .species = SPECIES_DRAGONAIR,
        .moves = {MOVE_BREAKING_SWIPE, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_SHED_SKIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MIGHTYENA_2] = {
        .species = SPECIES_MIGHTYENA,
        .moves = {MOVE_CRUNCH, MOVE_PLAY_ROUGH, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_MOXIE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LINOONE_2] = {
        .species = SPECIES_LINOONE,
        .moves = {MOVE_FACADE, MOVE_THROAT_CHOP, MOVE_STOMPING_TANTRUM, MOVE_PROTECT},
        .heldItem = ITEM_TOXIC_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_QUICK_FEET,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CASTFORM_2] = {
        .species = SPECIES_CASTFORM_NORMAL,
        .moves = {MOVE_RAIN_DANCE, MOVE_WEATHER_BALL, MOVE_THUNDER, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_FORECAST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SHELGON_2] = {
        .species = SPECIES_SHELGON,
        .moves = {MOVE_DOUBLE_EDGE, MOVE_DRAGON_CLAW, MOVE_IRON_HEAD, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_ROCK_HEAD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_METANG_2] = {
        .species = SPECIES_METANG,
        .moves = {MOVE_ALLY_SWITCH, MOVE_ICY_WIND, MOVE_LIGHT_SCREEN, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WIGGLYTUFF_2] = {
        .species = SPECIES_WIGGLYTUFF,
        .moves = {MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_HEAL_PULSE, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(0, 0, 32, 2, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_FRISK,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SUNFLORA_2] = {
        .species = SPECIES_SUNFLORA,
        .moves = {MOVE_SOLAR_BEAM, MOVE_EARTH_POWER, MOVE_SLUDGE_BOMB, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SOLAR_POWER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CHIMECHO_2] = {
        .species = SPECIES_CHIMECHO,
        .moves = {MOVE_HEAL_PULSE, MOVE_HELPING_HAND, MOVE_ICY_WIND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GLIGAR_2] = {
        .species = SPECIES_GLIGAR,
        .moves = {MOVE_SWORDS_DANCE, MOVE_HIGH_HORSEPOWER, MOVE_DUAL_WINGBEAT, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_HYPER_CUTTER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_QWILFISH_2] = {
        .species = SPECIES_QWILFISH,
        .moves = {MOVE_SPIKES, MOVE_TAUNT, MOVE_BARB_BARRAGE, MOVE_PROTECT},
        .heldItem = ITEM_BLACK_SLUDGE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 17, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SNEASEL_2] = {
        .species = SPECIES_SNEASEL,
        .moves = {MOVE_BEAT_UP, MOVE_HELPING_HAND, MOVE_TAUNT, MOVE_ICE_SHARD},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PELIPPER_2] = {
        .species = SPECIES_PELIPPER,
        .moves = {MOVE_HURRICANE, MOVE_TAILWIND, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_DAMP_ROCK,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_DRIZZLE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SWELLOW_2] = {
        .species = SPECIES_SWELLOW,
        .moves = {MOVE_BOOMBURST, MOVE_HURRICANE, MOVE_AIR_SLASH, MOVE_PROTECT},
        .heldItem = ITEM_THROAT_SPRAY,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_SCRAPPY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LAIRON_2] = {
        .species = SPECIES_LAIRON,
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_HEAVY_SLAM, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_HEAVY_METAL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TANGELA_2] = {
        .species = SPECIES_TANGELA,
        .moves = {MOVE_SLEEP_POWDER, MOVE_STUN_SPORE, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 2, 0),
        .nature = NATURE_BOLD,
        .ability = ABILITY_REGENERATOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ARBOK_2] = {
        .species = SPECIES_ARBOK,
        .moves = {MOVE_GLARE, MOVE_KNOCK_OFF, MOVE_HAZE, MOVE_PROTECT},
        .heldItem = ITEM_BLACK_SLUDGE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PERSIAN_2] = {
        .species = SPECIES_PERSIAN,
        .moves = {MOVE_FAKE_OUT, MOVE_ICY_WIND, MOVE_PARTING_SHOT, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_TECHNICIAN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SEADRA_2] = {
        .species = SPECIES_SEADRA,
        .moves = {MOVE_ICY_WIND, MOVE_CLEAR_SMOG, MOVE_SCALD, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_DAMP,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KECLEON_2] = {
        .species = SPECIES_KECLEON,
        .moves = {MOVE_DRAIN_PUNCH, MOVE_SHADOW_SNEAK, MOVE_KNOCK_OFF, MOVE_ROCK_SLIDE},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_PROTEAN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VIGOROTH_2] = {
        .species = SPECIES_VIGOROTH,
        .moves = {MOVE_BULK_UP, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 17, 0, 17, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_VITAL_SPIRIT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LUNATONE_2] = {
        .species = SPECIES_LUNATONE,
        .moves = {MOVE_METEOR_BEAM, MOVE_PSYSHOCK, MOVE_EARTH_POWER, MOVE_PROTECT},
        .heldItem = ITEM_POWER_HERB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SOLROCK_2] = {
        .species = SPECIES_SOLROCK,
        .moves = {MOVE_ROCK_SLIDE, MOVE_ZEN_HEADBUTT, MOVE_WILL_O_WISP, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NOCTOWL_2] = {
        .species = SPECIES_NOCTOWL,
        .moves = {MOVE_NASTY_PLOT, MOVE_AIR_SLASH, MOVE_HYPER_VOICE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_TINTED_LENS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SANDSLASH_2] = {
        .species = SPECIES_SANDSLASH,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_KNOCK_OFF, MOVE_LEECH_LIFE},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SAND_RUSH,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VENOMOTH_2] = {
        .species = SPECIES_VENOMOTH,
        .moves = {MOVE_QUIVER_DANCE, MOVE_BUG_BUZZ, MOVE_SLUDGE_BOMB, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_TINTED_LENS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CHANSEY_2] = {
        .species = SPECIES_CHANSEY,
        .moves = {MOVE_ICY_WIND, MOVE_SEISMIC_TOSS, MOVE_THUNDER_WAVE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 0, 32, 0, 0, 32),
        .nature = NATURE_BOLD,
        .ability = ABILITY_NATURAL_CURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SEAKING_2] = {
        .species = SPECIES_SEAKING,
        .moves = {MOVE_SWORDS_DANCE, MOVE_WATERFALL, MOVE_DRILL_RUN, MOVE_PROTECT},
        .heldItem = ITEM_CLEAR_AMULET,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_WATER_VEIL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_JUMPLUFF_2] = {
        .species = SPECIES_JUMPLUFF,
        .moves = {MOVE_SLEEP_POWDER, MOVE_ENCORE, MOVE_STRENGTH_SAP, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 0, 2),
        .nature = NATURE_TIMID,
        .ability = ABILITY_INFILTRATOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PILOSWINE_2] = {
        .species = SPECIES_PILOSWINE,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_ICY_WIND, MOVE_HAZE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_OBLIVIOUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GOLBAT_2] = {
        .species = SPECIES_GOLBAT,
        .moves = {MOVE_QUICK_GUARD, MOVE_BRAVE_BIRD, MOVE_HAZE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(18, 16, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PRIMEAPE_2] = {
        .species = SPECIES_PRIMEAPE,
        .moves = {MOVE_RAGE_FIST, MOVE_HELPING_HAND, MOVE_ENCORE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_DEFIANT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HITMONLEE_2] = {
        .species = SPECIES_HITMONLEE,
        .moves = {MOVE_CURSE, MOVE_CLOSE_COMBAT, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_WHITE_HERB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_UNBURDEN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HITMONCHAN_2] = {
        .species = SPECIES_HITMONCHAN,
        .moves = {MOVE_DRAIN_PUNCH, MOVE_MACH_PUNCH, MOVE_ICE_PUNCH, MOVE_THUNDER_PUNCH},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 0, 0, 0, 2),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_IRON_FIST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GIRAFARIG_2] = {
        .species = SPECIES_GIRAFARIG,
        .moves = {MOVE_NASTY_PLOT, MOVE_PSYCHIC, MOVE_HYPER_VOICE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_SAP_SIPPER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HITMONTOP_2] = {
        .species = SPECIES_HITMONTOP,
        .moves = {MOVE_FAKE_OUT, MOVE_TRIPLE_KICK, MOVE_MACH_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_WIDE_LENS,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_TECHNICIAN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BANETTE_2] = {
        .species = SPECIES_BANETTE,
        .moves = {MOVE_TRICK_ROOM, MOVE_POLTERGEIST, MOVE_WILL_O_WISP, MOVE_PROTECT},
        .heldItem = ITEM_MENTAL_HERB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_INSOMNIA,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NINJASK_2] = {
        .species = SPECIES_NINJASK,
        .moves = {MOVE_DUAL_WINGBEAT, MOVE_LEECH_LIFE, MOVE_NIGHT_SLASH, MOVE_U_TURN},
        .heldItem = ITEM_CHOICE_BAND,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_INFILTRATOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SEVIPER_2] = {
        .species = SPECIES_SEVIPER,
        .moves = {MOVE_COIL, MOVE_GUNK_SHOT, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SHED_SKIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ZANGOOSE_2] = {
        .species = SPECIES_ZANGOOSE,
        .moves = {MOVE_FAKE_OUT, MOVE_FACADE, MOVE_QUICK_ATTACK, MOVE_PROTECT},
        .heldItem = ITEM_TOXIC_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_TOXIC_BOOST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CAMERUPT_2] = {
        .species = SPECIES_CAMERUPT,
        .moves = {MOVE_ROCK_POLISH, MOVE_HEAT_WAVE, MOVE_EARTH_POWER, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SOLID_ROCK,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SHARPEDO_2] = {
        .species = SPECIES_SHARPEDO,
        .moves = {MOVE_PROTECT, MOVE_HYDRO_PUMP, MOVE_DARK_PULSE, MOVE_ICE_BEAM},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SPEED_BOOST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TROPIUS_2] = {
        .species = SPECIES_TROPIUS,
        .moves = {MOVE_LEECH_SEED, MOVE_AIR_SLASH, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_HARVEST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MAGNETON_2] = {
        .species = SPECIES_MAGNETON,
        .moves = {MOVE_THUNDERBOLT, MOVE_FLASH_CANNON, MOVE_METAL_SOUND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_MAGNET_PULL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MANTINE_2] = {
        .species = SPECIES_MANTINE,
        .moves = {MOVE_ICY_WIND, MOVE_SCALD, MOVE_HAZE, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_WATER_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_STANTLER_2] = {
        .species = SPECIES_STANTLER,
        .moves = {MOVE_DOUBLE_EDGE, MOVE_ZEN_HEADBUTT, MOVE_EARTHQUAKE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SAP_SIPPER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ABSOL_2] = {
        .species = SPECIES_ABSOL,
        .moves = {MOVE_NIGHT_SLASH, MOVE_PSYCHO_CUT, MOVE_STONE_EDGE, MOVE_PROTECT},
        .heldItem = ITEM_SCOPE_LENS,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SUPER_LUCK,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SWALOT_2] = {
        .species = SPECIES_SWALOT,
        .moves = {MOVE_ACID_ARMOR, MOVE_BODY_PRESS, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_STICKY_HOLD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CRAWDAUNT_2] = {
        .species = SPECIES_CRAWDAUNT,
        .moves = {MOVE_CRABHAMMER, MOVE_KNOCK_OFF, MOVE_AQUA_JET, MOVE_CLOSE_COMBAT},
        .heldItem = ITEM_CHOICE_BAND,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_ADAPTABILITY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PIDGEOT_2] = {
        .species = SPECIES_PIDGEOT,
        .moves = {MOVE_HURRICANE, MOVE_FEATHER_DANCE, MOVE_ROOST, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_NO_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GRUMPIG_2] = {
        .species = SPECIES_GRUMPIG,
        .moves = {MOVE_SIMPLE_BEAM, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_OWN_TEMPO,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TORKOAL_2] = {
        .species = SPECIES_TORKOAL,
        .moves = {MOVE_HEAT_WAVE, MOVE_SOLAR_BEAM, MOVE_EARTH_POWER, MOVE_PROTECT},
        .heldItem = ITEM_CHARCOAL,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_DROUGHT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KINGLER_2] = {
        .species = SPECIES_KINGLER,
        .moves = {MOVE_WIDE_GUARD, MOVE_KNOCK_OFF, MOVE_CRABHAMMER, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 16, 0, 0, 18),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CACTURNE_2] = {
        .species = SPECIES_CACTURNE,
        .moves = {MOVE_LEAF_STORM, MOVE_DARK_PULSE, MOVE_FOCUS_BLAST, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_WATER_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BELLOSSOM_2] = {
        .species = SPECIES_BELLOSSOM,
        .moves = {MOVE_SWORDS_DANCE, MOVE_SOLAR_BLADE, MOVE_TRIPLE_AXEL, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_OCTILLERY_2] = {
        .species = SPECIES_OCTILLERY,
        .moves = {MOVE_SUBSTITUTE, MOVE_PROTECT, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_MOODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HUNTAIL_2] = {
        .species = SPECIES_HUNTAIL,
        .moves = {MOVE_COIL, MOVE_WATERFALL, MOVE_CRUNCH, MOVE_SUCKER_PUNCH},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_WATER_VEIL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GOREBYSS_2] = {
        .species = SPECIES_GOREBYSS,
        .moves = {MOVE_SHELL_SMASH, MOVE_BATON_PASS, MOVE_SUBSTITUTE, MOVE_PROTECT},
        .heldItem = ITEM_WHITE_HERB,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 2, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RELICANTH_2] = {
        .species = SPECIES_RELICANTH,
        .moves = {MOVE_LIQUIDATION, MOVE_STONE_EDGE, MOVE_STOMPING_TANTRUM, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_OMASTAR_2] = {
        .species = SPECIES_OMASTAR,
        .moves = {MOVE_SHELL_SMASH, MOVE_HYDRO_PUMP, MOVE_EARTH_POWER, MOVE_PROTECT},
        .heldItem = ITEM_WHITE_HERB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SHELL_ARMOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KABUTOPS_2] = {
        .species = SPECIES_KABUTOPS,
        .moves = {MOVE_SWORDS_DANCE, MOVE_LIQUIDATION, MOVE_STONE_EDGE, MOVE_AQUA_JET},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_BATTLE_ARMOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_POLIWRATH_2] = {
        .species = SPECIES_POLIWRATH,
        .moves = {MOVE_BELLY_DRUM, MOVE_LIQUIDATION, MOVE_DRAIN_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SCYTHER_2] = {
        .species = SPECIES_SCYTHER,
        .moves = {MOVE_FEINT, MOVE_DUAL_WINGBEAT, MOVE_CLOSE_COMBAT, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_TECHNICIAN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PINSIR_2] = {
        .species = SPECIES_PINSIR,
        .moves = {MOVE_X_SCISSOR, MOVE_HIGH_HORSEPOWER, MOVE_QUICK_ATTACK, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_MOXIE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_POLITOED_2] = {
        .species = SPECIES_POLITOED,
        .moves = {MOVE_SCALD, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EJECT_BUTTON,
        .ev = TRAINER_PARTY_EVS(32, 0, 23, 0, 0, 11),
        .nature = NATURE_BOLD,
        .ability = ABILITY_DRIZZLE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CLOYSTER_2] = {
        .species = SPECIES_CLOYSTER,
        .moves = {MOVE_SHELL_SMASH, MOVE_ICICLE_SPEAR, MOVE_LIQUIDATION, MOVE_PROTECT},
        .heldItem = ITEM_WHITE_HERB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SKILL_LINK,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DUGTRIO_1] = {
        .species = SPECIES_DUGTRIO,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_ARENA_TRAP,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MEDICHAM_1] = {
        .species = SPECIES_MEDICHAM,
        .moves = {MOVE_FAKE_OUT, MOVE_CLOSE_COMBAT, MOVE_PSYCHO_CUT, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_PURE_POWER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MISDREAVUS_1] = {
        .species = SPECIES_MISDREAVUS,
        .moves = {MOVE_WILL_O_WISP, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FEAROW_1] = {
        .species = SPECIES_FEAROW,
        .moves = {MOVE_DRILL_PECK, MOVE_DRILL_RUN, MOVE_U_TURN, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GRANBULL_1] = {
        .species = SPECIES_GRANBULL,
        .moves = {MOVE_PLAY_ROUGH, MOVE_SUPER_FANG, MOVE_THUNDER_WAVE, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_JYNX_1] = {
        .species = SPECIES_JYNX,
        .moves = {MOVE_FAKE_OUT, MOVE_LOVELY_KISS, MOVE_ICY_WIND, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_OBLIVIOUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DUSCLOPS_1] = {
        .species = SPECIES_DUSCLOPS,
        .moves = {MOVE_TRICK_ROOM, MOVE_NIGHT_SHADE, MOVE_WILL_O_WISP, MOVE_PAIN_SPLIT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_SASSY,
        .ability = ABILITY_FRISK,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DODRIO_1] = {
        .species = SPECIES_DODRIO,
        .moves = {MOVE_BRAVE_BIRD, MOVE_DOUBLE_EDGE, MOVE_JUMP_KICK, MOVE_PROTECT},
        .heldItem = ITEM_SHARP_BEAK,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_MOXIE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MR_MIME_1] = {
        .species = SPECIES_MR_MIME,
        .moves = {MOVE_FAKE_OUT, MOVE_WIDE_GUARD, MOVE_ICY_WIND, MOVE_DAZZLING_GLEAM},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FILTER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LANTURN_1] = {
        .species = SPECIES_LANTURN,
        .moves = {MOVE_SCALD, MOVE_ICY_WIND, MOVE_EERIE_IMPULSE, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_VOLT_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BRELOOM_1] = {
        .species = SPECIES_BRELOOM,
        .moves = {MOVE_SPORE, MOVE_MACH_PUNCH, MOVE_BULLET_SEED, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_TECHNICIAN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FORRETRESS_1] = {
        .species = SPECIES_FORRETRESS,
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_GYRO_BALL, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_RELAXED,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WHISCASH_1] = {
        .species = SPECIES_WHISCASH,
        .moves = {MOVE_DRAGON_DANCE, MOVE_HIGH_HORSEPOWER, MOVE_LIQUIDATION, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_OBLIVIOUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_XATU_1] = {
        .species = SPECIES_XATU,
        .moves = {MOVE_PSYCHIC, MOVE_HEAT_WAVE, MOVE_TAILWIND, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_MAGIC_BOUNCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SKARMORY_1] = {
        .species = SPECIES_SKARMORY,
        .moves = {MOVE_TAILWIND, MOVE_BODY_PRESS, MOVE_BRAVE_BIRD, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MAROWAK_1] = {
        .species = SPECIES_MAROWAK,
        .moves = {MOVE_BONEMERANG, MOVE_ROCK_SLIDE, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_THICK_CLUB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_LIGHTNING_ROD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_QUAGSIRE_1] = {
        .species = SPECIES_QUAGSIRE,
        .moves = {MOVE_HAZE, MOVE_CHILLING_WATER, MOVE_HELPING_HAND, MOVE_RECOVER},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_UNAWARE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CLEFABLE_1] = {
        .species = SPECIES_CLEFABLE,
        .moves = {MOVE_FOLLOW_ME, MOVE_MOONBLAST, MOVE_LIFE_DEW, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_UNAWARE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HARIYAMA_1] = {
        .species = SPECIES_HARIYAMA,
        .moves = {MOVE_FAKE_OUT, MOVE_CLOSE_COMBAT, MOVE_KNOCK_OFF, MOVE_WIDE_GUARD},
        .heldItem = ITEM_FLAME_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RAICHU_1] = {
        .species = SPECIES_RAICHU,
        .moves = {MOVE_FAKE_OUT, MOVE_ENCORE, MOVE_THUNDERBOLT, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LIGHTNING_ROD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DEWGONG_1] = {
        .species = SPECIES_DEWGONG,
        .moves = {MOVE_FAKE_OUT, MOVE_ICE_SPINNER, MOVE_LIQUIDATION, MOVE_FLIP_TURN},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_FUR_COAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MANECTRIC_1] = {
        .species = SPECIES_MANECTRIC,
        .moves = {MOVE_VOLT_SWITCH, MOVE_SNARL, MOVE_OVERHEAT, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LIGHTNING_ROD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VILEPLUME_1] = {
        .species = SPECIES_VILEPLUME,
        .moves = {MOVE_GROWTH, MOVE_SOLAR_BEAM, MOVE_SLUDGE_BOMB, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VICTREEBEL_1] = {
        .species = SPECIES_VICTREEBEL,
        .moves = {MOVE_GROWTH, MOVE_GIGA_DRAIN, MOVE_SLUDGE_BOMB, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ELECTRODE_1] = {
        .species = SPECIES_ELECTRODE,
        .moves = {MOVE_THUNDERBOLT, MOVE_VOLT_SWITCH, MOVE_TAUNT, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_ELECTRIC_SURGE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_EXPLOUD_1] = {
        .species = SPECIES_EXPLOUD,
        .moves = {MOVE_BOOMBURST, MOVE_FOCUS_BLAST, MOVE_FIRE_BLAST, MOVE_PROTECT},
        .heldItem = ITEM_THROAT_SPRAY,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SCRAPPY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SHIFTRY_1] = {
        .species = SPECIES_SHIFTRY,
        .moves = {MOVE_FAKE_OUT, MOVE_TAILWIND, MOVE_KNOCK_OFF, MOVE_LEAF_BLADE},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_WIND_RIDER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GLALIE_1] = {
        .species = SPECIES_GLALIE,
        .moves = {MOVE_ICY_WIND, MOVE_DISABLE, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_COVERT_CLOAK,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LUDICOLO_1] = {
        .species = SPECIES_LUDICOLO,
        .moves = {MOVE_FAKE_OUT, MOVE_HYDRO_PUMP, MOVE_GIGA_DRAIN, MOVE_ICE_BEAM},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HYPNO_1] = {
        .species = SPECIES_HYPNO,
        .moves = {MOVE_TRICK_ROOM, MOVE_HELPING_HAND, MOVE_PSYCHIC_NOISE, MOVE_PROTECT},
        .heldItem = ITEM_MENTAL_HERB,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_SASSY,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GOLEM_1] = {
        .species = SPECIES_GOLEM,
        .moves = {MOVE_ROCK_SLIDE, MOVE_HIGH_HORSEPOWER, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_WEAKNESS_POLICY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RHYDON_1] = {
        .species = SPECIES_RHYDON,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_ICE_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_LIGHTNING_ROD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ALAKAZAM_1] = {
        .species = SPECIES_ALAKAZAM,
        .moves = {MOVE_PSYCHIC, MOVE_FOCUS_BLAST, MOVE_SHADOW_BALL, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_MAGIC_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WEEZING_1] = {
        .species = SPECIES_WEEZING,
        .moves = {MOVE_SLUDGE_BOMB, MOVE_WILL_O_WISP, MOVE_HAZE, MOVE_PROTECT},
        .heldItem = ITEM_COVERT_CLOAK,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 2, 0),
        .nature = NATURE_BOLD,
        .ability = ABILITY_NEUTRALIZING_GAS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KANGASKHAN_1] = {
        .species = SPECIES_KANGASKHAN,
        .moves = {MOVE_FAKE_OUT, MOVE_DOUBLE_EDGE, MOVE_LOW_KICK, MOVE_PROTECT},
        .heldItem = ITEM_CHOPLE_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 0, 0, 0, 2),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SCRAPPY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ELECTABUZZ_1] = {
        .species = SPECIES_ELECTABUZZ,
        .moves = {MOVE_FOLLOW_ME, MOVE_PROTECT, MOVE_HELPING_HAND, MOVE_THUNDER_WAVE},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_BOLD,
        .ability = ABILITY_STATIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TAUROS_1] = {
        .species = SPECIES_TAUROS,
        .moves = {MOVE_BODY_SLAM, MOVE_HIGH_HORSEPOWER, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SLOWBRO_1] = {
        .species = SPECIES_SLOWBRO,
        .moves = {MOVE_TRICK_ROOM, MOVE_SCALD, MOVE_PSYCHIC, MOVE_SLACK_OFF},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_RELAXED,
        .ability = ABILITY_REGENERATOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SLOWKING_1] = {
        .species = SPECIES_SLOWKING,
        .moves = {MOVE_TRICK_ROOM, MOVE_SCALD, MOVE_HEAL_PULSE, MOVE_SLACK_OFF},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_SASSY,
        .ability = ABILITY_OBLIVIOUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MILTANK_1] = {
        .species = SPECIES_MILTANK,
        .moves = {MOVE_BODY_SLAM, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_MILK_DRINK},
        .heldItem = ITEM_COVERT_CLOAK,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SAP_SIPPER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ALTARIA_1] = {
        .species = SPECIES_ALTARIA,
        .moves = {MOVE_TAILWIND, MOVE_BREAKING_SWIPE, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_CLOUD_NINE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NIDOQUEEN_1] = {
        .species = SPECIES_NIDOQUEEN,
        .moves = {MOVE_EARTH_POWER, MOVE_SLUDGE_BOMB, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NIDOKING_1] = {
        .species = SPECIES_NIDOKING,
        .moves = {MOVE_SLUDGE_BOMB, MOVE_EARTH_POWER, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MAGMAR_1] = {
        .species = SPECIES_MAGMAR,
        .moves = {MOVE_FOLLOW_ME, MOVE_HELPING_HAND, MOVE_WILL_O_WISP, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_FLAME_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CRADILY_1] = {
        .species = SPECIES_CRADILY,
        .moves = {MOVE_GIGA_DRAIN, MOVE_POWER_GEM, MOVE_RECOVER, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_STORM_DRAIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ARMALDO_1] = {
        .species = SPECIES_ARMALDO,
        .moves = {MOVE_X_SCISSOR, MOVE_ROCK_SLIDE, MOVE_LIQUIDATION, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GOLDUCK_1] = {
        .species = SPECIES_GOLDUCK,
        .moves = {MOVE_SCALD, MOVE_ICY_WIND, MOVE_ENCORE, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_CLOUD_NINE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RAPIDASH_1] = {
        .species = SPECIES_RAPIDASH,
        .moves = {MOVE_FLARE_BLITZ, MOVE_WILD_CHARGE, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .heldItem = ITEM_CHARCOAL,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_RECKLESS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MUK_1] = {
        .species = SPECIES_MUK,
        .moves = {MOVE_POISON_JAB, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF, MOVE_SHADOW_SNEAK},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 0, 0, 0, 2),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_POISON_TOUCH,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GENGAR_1] = {
        .species = SPECIES_GENGAR,
        .moves = {MOVE_SHADOW_BALL, MOVE_ICY_WIND, MOVE_SLUDGE_WAVE, MOVE_DESTINY_BOND},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_CURSED_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_AMPHAROS_1] = {
        .species = SPECIES_AMPHAROS,
        .moves = {MOVE_THUNDERBOLT, MOVE_ELECTROWEB, MOVE_HEAL_BELL, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_STATIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SCIZOR_1] = {
        .species = SPECIES_SCIZOR,
        .moves = {MOVE_BULLET_PUNCH, MOVE_BUG_BITE, MOVE_SWORDS_DANCE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_TECHNICIAN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HERACROSS_1] = {
        .species = SPECIES_HERACROSS,
        .moves = {MOVE_CLOSE_COMBAT, MOVE_FACADE, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_FLAME_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_URSARING_1] = {
        .species = SPECIES_URSARING,
        .moves = {MOVE_FACADE, MOVE_CLOSE_COMBAT, MOVE_CRUNCH, MOVE_PROTECT},
        .heldItem = ITEM_FLAME_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HOUNDOOM_1] = {
        .species = SPECIES_HOUNDOOM,
        .moves = {MOVE_HEAT_WAVE, MOVE_DARK_PULSE, MOVE_SNARL, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FLASH_FIRE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DONPHAN_1] = {
        .species = SPECIES_DONPHAN,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_RAPID_SPIN, MOVE_KNOCK_OFF, MOVE_ICE_SHARD},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 0, 0, 0, 2),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CLAYDOL_1] = {
        .species = SPECIES_CLAYDOL,
        .moves = {MOVE_TRICK_ROOM, MOVE_ALLY_SWITCH, MOVE_EARTH_POWER, MOVE_PROTECT},
        .heldItem = ITEM_MENTAL_HERB,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_RELAXED,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WAILORD_1] = {
        .species = SPECIES_WAILORD,
        .moves = {MOVE_WATER_SPOUT, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_ICY_WIND},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_DRIZZLE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NINETALES_1] = {
        .species = SPECIES_NINETALES,
        .moves = {MOVE_HEAT_WAVE, MOVE_SOLAR_BEAM, MOVE_WILL_O_WISP, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_DROUGHT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MACHAMP_1] = {
        .species = SPECIES_MACHAMP,
        .moves = {MOVE_DYNAMIC_PUNCH, MOVE_KNOCK_OFF, MOVE_STONE_EDGE, MOVE_BULLET_PUNCH},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_NO_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SHUCKLE_1] = {
        .species = SPECIES_SHUCKLE,
        .moves = {MOVE_POWER_SPLIT, MOVE_HELPING_HAND, MOVE_TOXIC, MOVE_PROTECT},
        .heldItem = ITEM_MENTAL_HERB,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_STEELIX_1] = {
        .species = SPECIES_STEELIX,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_HEAVY_SLAM, MOVE_WIDE_GUARD, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TENTACRUEL_1] = {
        .species = SPECIES_TENTACRUEL,
        .moves = {MOVE_ACID_SPRAY, MOVE_ICY_WIND, MOVE_MUDDY_WATER, MOVE_PROTECT},
        .heldItem = ITEM_BLACK_SLUDGE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_AERODACTYL_1] = {
        .species = SPECIES_AERODACTYL,
        .moves = {MOVE_ROCK_SLIDE, MOVE_TAILWIND, MOVE_DUAL_WINGBEAT, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_UNNERVE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PORYGON2_1] = {
        .species = SPECIES_PORYGON2,
        .moves = {MOVE_TRICK_ROOM, MOVE_ICE_BEAM, MOVE_TRI_ATTACK, MOVE_RECOVER},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_QUIET,
        .ability = ABILITY_DOWNLOAD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GARDEVOIR_1] = {
        .species = SPECIES_GARDEVOIR,
        .moves = {MOVE_MOONBLAST, MOVE_PSYCHIC, MOVE_ICY_WIND, MOVE_TRICK},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_TRACE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_EXEGGUTOR_1] = {
        .species = SPECIES_EXEGGUTOR,
        .moves = {MOVE_SOLAR_BEAM, MOVE_PSYCHIC, MOVE_SLEEP_POWDER, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_STARMIE_1] = {
        .species = SPECIES_STARMIE,
        .moves = {MOVE_HYDRO_PUMP, MOVE_PSYCHIC, MOVE_THUNDERBOLT, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_ANALYTIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FLYGON_1] = {
        .species = SPECIES_FLYGON,
        .moves = {MOVE_TAILWIND, MOVE_DRAGON_CLAW, MOVE_EARTHQUAKE, MOVE_PROTECT},
        .heldItem = ITEM_CLEAR_AMULET,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VENUSAUR_1] = {
        .species = SPECIES_VENUSAUR,
        .moves = {MOVE_LEAF_STORM, MOVE_EARTH_POWER, MOVE_SLEEP_POWDER, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VAPOREON_1] = {
        .species = SPECIES_VAPOREON,
        .moves = {MOVE_MUDDY_WATER, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_WATER_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_JOLTEON_1] = {
        .species = SPECIES_JOLTEON,
        .moves = {MOVE_THUNDERBOLT, MOVE_ELECTROWEB, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_VOLT_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FLAREON_1] = {
        .species = SPECIES_FLAREON,
        .moves = {MOVE_FLARE_BLITZ, MOVE_FACADE, MOVE_SUPERPOWER, MOVE_PROTECT},
        .heldItem = ITEM_TOXIC_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MEGANIUM_1] = {
        .species = SPECIES_MEGANIUM,
        .moves = {MOVE_HEAL_PULSE, MOVE_GIGA_DRAIN, MOVE_LIGHT_SCREEN, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 2, 0),
        .nature = NATURE_BOLD,
        .ability = ABILITY_TRIAGE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ESPEON_1] = {
        .species = SPECIES_ESPEON,
        .moves = {MOVE_PSYCHIC, MOVE_SHADOW_BALL, MOVE_DAZZLING_GLEAM, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_MAGIC_BOUNCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_UMBREON_1] = {
        .species = SPECIES_UMBREON,
        .moves = {MOVE_FOUL_PLAY, MOVE_SNARL, MOVE_HELPING_HAND, MOVE_MOONLIGHT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BLASTOISE_1] = {
        .species = SPECIES_BLASTOISE,
        .moves = {MOVE_FAKE_OUT, MOVE_FOLLOW_ME, MOVE_FLIP_TURN, MOVE_YAWN},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_TORRENT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FERALIGATR_1] = {
        .species = SPECIES_FERALIGATR,
        .moves = {MOVE_DRAGON_DANCE, MOVE_LIQUIDATION, MOVE_ICE_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_AGGRON_1] = {
        .species = SPECIES_AGGRON,
        .moves = {MOVE_HEAD_SMASH, MOVE_HEAVY_SLAM, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .heldItem = ITEM_CLEAR_AMULET,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_ROCK_HEAD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BLAZIKEN_1] = {
        .species = SPECIES_BLAZIKEN,
        .moves = {MOVE_HEAT_WAVE, MOVE_CLOSE_COMBAT, MOVE_THUNDER_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 16, 0, 32, 16, 0),
        .nature = NATURE_HASTY,
        .ability = ABILITY_SPEED_BOOST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WALREIN_1] = {
        .species = SPECIES_WALREIN,
        .moves = {MOVE_HYDRO_PUMP, MOVE_ICY_WIND, MOVE_SUPER_FANG, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_THICK_FAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SCEPTILE_1] = {
        .species = SPECIES_SCEPTILE,
        .moves = {MOVE_LEAF_STORM, MOVE_DRAGON_PULSE, MOVE_FOCUS_BLAST, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_OVERGROW,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CHARIZARD_1] = {
        .species = SPECIES_CHARIZARD,
        .moves = {MOVE_HEAT_WAVE, MOVE_AIR_SLASH, MOVE_TAILWIND, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_BLAZE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TYPHLOSION_1] = {
        .species = SPECIES_TYPHLOSION,
        .moves = {MOVE_ERUPTION, MOVE_HEAT_WAVE, MOVE_FOCUS_BLAST, MOVE_SCORCHING_SANDS},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FLASH_FIRE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LAPRAS_1] = {
        .species = SPECIES_LAPRAS,
        .moves = {MOVE_FREEZE_DRY, MOVE_HYDRO_PUMP, MOVE_ICY_WIND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_WATER_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CROBAT_1] = {
        .species = SPECIES_CROBAT,
        .moves = {MOVE_TAILWIND, MOVE_BRAVE_BIRD, MOVE_TAUNT, MOVE_PROTECT},
        .heldItem = ITEM_SAFETY_GOGGLES,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SWAMPERT_1] = {
        .species = SPECIES_SWAMPERT,
        .moves = {MOVE_MUDDY_WATER, MOVE_EARTH_POWER, MOVE_ICE_BEAM, MOVE_WIDE_GUARD},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_QUIET,
        .ability = ABILITY_DAMP,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GYARADOS_1] = {
        .species = SPECIES_GYARADOS,
        .moves = {MOVE_WATERFALL, MOVE_POWER_WHIP, MOVE_CRUNCH, MOVE_STONE_EDGE},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SNORLAX_1] = {
        .species = SPECIES_SNORLAX,
        .moves = {MOVE_BELLY_DRUM, MOVE_RETURN, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .heldItem = ITEM_IAPAPA_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_GLUTTONY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KINGDRA_1] = {
        .species = SPECIES_KINGDRA,
        .moves = {MOVE_MUDDY_WATER, MOVE_DRACO_METEOR, MOVE_HURRICANE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BLISSEY_1] = {
        .species = SPECIES_BLISSEY,
        .moves = {MOVE_HEAL_PULSE, MOVE_LIFE_DEW, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(2, 0, 32, 0, 0, 32),
        .nature = NATURE_BOLD,
        .ability = ABILITY_HEALER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MILOTIC_1] = {
        .species = SPECIES_MILOTIC,
        .moves = {MOVE_SCALD, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_BOLD,
        .ability = ABILITY_COMPETITIVE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ARCANINE_1] = {
        .species = SPECIES_ARCANINE,
        .moves = {MOVE_FLAMETHROWER, MOVE_SNARL, MOVE_WILL_O_WISP, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SALAMENCE_1] = {
        .species = SPECIES_SALAMENCE,
        .moves = {MOVE_TAILWIND, MOVE_BREAKING_SWIPE, MOVE_HEAT_WAVE, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 2, 0, 32, 0, 0),
        .nature = NATURE_NAIVE,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_METAGROSS_1] = {
        .species = SPECIES_METAGROSS,
        .moves = {MOVE_METEOR_MASH, MOVE_PSYCHIC_FANGS, MOVE_HAMMER_ARM, MOVE_BULLET_PUNCH},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 0, 0, 0, 2),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SLAKING_1] = {
        .species = SPECIES_SLAKING,
        .moves = {MOVE_DOUBLE_EDGE, MOVE_HIGH_HORSEPOWER, MOVE_ICE_PUNCH, MOVE_KNOCK_OFF},
        .heldItem = ITEM_CHOICE_BAND,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_TRUANT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DUGTRIO_2] = {
        .species = SPECIES_DUGTRIO,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_NIGHT_SLASH, MOVE_AERIAL_ACE},
        .heldItem = ITEM_CHOICE_BAND,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_ARENA_TRAP,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MEDICHAM_2] = {
        .species = SPECIES_MEDICHAM,
        .moves = {MOVE_HIGH_JUMP_KICK, MOVE_ICE_PUNCH, MOVE_FEINT, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_PURE_POWER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MAROWAK_2] = {
        .species = SPECIES_MAROWAK,
        .moves = {MOVE_BONEMERANG, MOVE_STONE_EDGE, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_THICK_CLUB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_BATTLE_ARMOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_QUAGSIRE_2] = {
        .species = SPECIES_QUAGSIRE,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_LIQUIDATION, MOVE_ICE_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_WATER_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MISDREAVUS_2] = {
        .species = SPECIES_MISDREAVUS,
        .moves = {MOVE_IMPRISON, MOVE_TRICK_ROOM, MOVE_SHADOW_BALL, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FEAROW_2] = {
        .species = SPECIES_FEAROW,
        .moves = {MOVE_TAILWIND, MOVE_FEATHER_DANCE, MOVE_DRILL_PECK, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GRANBULL_2] = {
        .species = SPECIES_GRANBULL,
        .moves = {MOVE_PLAY_ROUGH, MOVE_CLOSE_COMBAT, MOVE_SNARL, MOVE_ROCK_SLIDE},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 0, 0, 0, 2),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_JYNX_2] = {
        .species = SPECIES_JYNX,
        .moves = {MOVE_ICE_BEAM, MOVE_PSYSHOCK, MOVE_FOCUS_BLAST, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_DRY_SKIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DUSCLOPS_2] = {
        .species = SPECIES_DUSCLOPS,
        .moves = {MOVE_HAZE, MOVE_HELPING_HAND, MOVE_ALLY_SWITCH, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_SASSY,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DODRIO_2] = {
        .species = SPECIES_DODRIO,
        .moves = {MOVE_BRAVE_BIRD, MOVE_DOUBLE_EDGE, MOVE_KNOCK_OFF, MOVE_DRILL_RUN},
        .heldItem = ITEM_CHOICE_BAND,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_MOXIE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MR_MIME_2] = {
        .species = SPECIES_MR_MIME,
        .moves = {MOVE_REFLECT, MOVE_LIGHT_SCREEN, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_LIGHT_CLAY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FILTER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LANTURN_2] = {
        .species = SPECIES_LANTURN,
        .moves = {MOVE_SOAK, MOVE_THUNDERBOLT, MOVE_HEAL_BELL, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_WATER_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BRELOOM_2] = {
        .species = SPECIES_BRELOOM,
        .moves = {MOVE_BULLET_SEED, MOVE_MACH_PUNCH, MOVE_ROCK_TOMB, MOVE_CLOSE_COMBAT},
        .heldItem = ITEM_LOADED_DICE,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_TECHNICIAN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FORRETRESS_2] = {
        .species = SPECIES_FORRETRESS,
        .moves = {MOVE_LUNGE, MOVE_VOLT_SWITCH, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_SASSY,
        .ability = ABILITY_OVERCOAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SKARMORY_2] = {
        .species = SPECIES_SKARMORY,
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_ROOST, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WHISCASH_2] = {
        .species = SPECIES_WHISCASH,
        .moves = {MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_MUDDY_WATER, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_OBLIVIOUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_XATU_2] = {
        .species = SPECIES_XATU,
        .moves = {MOVE_REFLECT, MOVE_LIGHT_SCREEN, MOVE_TRICK_ROOM, MOVE_PROTECT},
        .heldItem = ITEM_LIGHT_CLAY,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_MAGIC_BOUNCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CLEFABLE_2] = {
        .species = SPECIES_CLEFABLE,
        .moves = {MOVE_FOLLOW_ME, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_ROCKY_HELMET,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_UNAWARE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HARIYAMA_2] = {
        .species = SPECIES_HARIYAMA,
        .moves = {MOVE_FAKE_OUT, MOVE_DRAIN_PUNCH, MOVE_KNOCK_OFF, MOVE_HEAVY_SLAM},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 0, 0, 0, 2),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_THICK_FAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RAICHU_2] = {
        .species = SPECIES_RAICHU,
        .moves = {MOVE_FAKE_OUT, MOVE_NUZZLE, MOVE_VOLT_SWITCH, MOVE_PROTECT},
        .heldItem = ITEM_AIR_BALLOON,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LIGHTNING_ROD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DEWGONG_2] = {
        .species = SPECIES_DEWGONG,
        .moves = {MOVE_FAKE_OUT, MOVE_PERISH_SONG, MOVE_DISABLE, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FUR_COAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MANECTRIC_2] = {
        .species = SPECIES_MANECTRIC,
        .moves = {MOVE_EERIE_IMPULSE, MOVE_SNARL, MOVE_THUNDERBOLT, MOVE_PROTECT},
        .heldItem = ITEM_COVERT_CLOAK,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_STATIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VILEPLUME_2] = {
        .species = SPECIES_VILEPLUME,
        .moves = {MOVE_GIGA_DRAIN, MOVE_SLUDGE_BOMB, MOVE_STRENGTH_SAP, MOVE_PROTECT},
        .heldItem = ITEM_ROCKY_HELMET,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_EFFECT_SPORE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VICTREEBEL_2] = {
        .species = SPECIES_VICTREEBEL,
        .moves = {MOVE_SWORDS_DANCE, MOVE_POWER_WHIP, MOVE_POISON_JAB, MOVE_PROTECT},
        .heldItem = ITEM_LUM_BERRY,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ELECTRODE_2] = {
        .species = SPECIES_ELECTRODE,
        .moves = {MOVE_REFLECT, MOVE_LIGHT_SCREEN, MOVE_DISCHARGE, MOVE_PROTECT},
        .heldItem = ITEM_LIGHT_CLAY,
        .ev = TRAINER_PARTY_EVS(32, 0, 16, 0, 0, 18),
        .nature = NATURE_BOLD,
        .ability = ABILITY_ELECTRIC_SURGE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_EXPLOUD_2] = {
        .species = SPECIES_EXPLOUD,
        .moves = {MOVE_HYPER_VOICE, MOVE_ICY_WIND, MOVE_FIRE_BLAST, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SCRAPPY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SHIFTRY_2] = {
        .species = SPECIES_SHIFTRY,
        .moves = {MOVE_TAILWIND, MOVE_KNOCK_OFF, MOVE_LEAF_BLADE, MOVE_EXPLOSION},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_WIND_RIDER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GLALIE_2] = {
        .species = SPECIES_GLALIE,
        .moves = {MOVE_SUBSTITUTE, MOVE_PROTECT, MOVE_FREEZE_DRY, MOVE_DISABLE},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_MOODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LUDICOLO_2] = {
        .species = SPECIES_LUDICOLO,
        .moves = {MOVE_FAKE_OUT, MOVE_ICY_WIND, MOVE_ENCORE, MOVE_GIGA_DRAIN},
        .heldItem = ITEM_COVERT_CLOAK,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 2, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_OWN_TEMPO,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HYPNO_2] = {
        .species = SPECIES_HYPNO,
        .moves = {MOVE_REFLECT, MOVE_LIGHT_SCREEN, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_LIGHT_CLAY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_INSOMNIA,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GOLEM_2] = {
        .species = SPECIES_GOLEM,
        .moves = {MOVE_WIDE_GUARD, MOVE_ROCK_SLIDE, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RHYDON_2] = {
        .species = SPECIES_RHYDON,
        .moves = {MOVE_SWORDS_DANCE, MOVE_HIGH_HORSEPOWER, MOVE_STONE_EDGE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_ROCK_HEAD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ALAKAZAM_2] = {
        .species = SPECIES_ALAKAZAM,
        .moves = {MOVE_EXPANDING_FORCE, MOVE_NASTY_PLOT, MOVE_FOCUS_BLAST, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_MAGIC_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WEEZING_2] = {
        .species = SPECIES_WEEZING,
        .moves = {MOVE_SLUDGE_BOMB, MOVE_WILL_O_WISP, MOVE_TAUNT, MOVE_PROTECT},
        .heldItem = ITEM_BLACK_SLUDGE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 2, 0),
        .nature = NATURE_BOLD,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KANGASKHAN_2] = {
        .species = SPECIES_KANGASKHAN,
        .moves = {MOVE_FAKE_OUT, MOVE_BODY_SLAM, MOVE_ICE_PUNCH, MOVE_DRAIN_PUNCH},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 16, 0, 0, 0, 18),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ELECTABUZZ_2] = {
        .species = SPECIES_ELECTABUZZ,
        .moves = {MOVE_FOLLOW_ME, MOVE_VOLT_SWITCH, MOVE_TAUNT, MOVE_FEINT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_BOLD,
        .ability = ABILITY_STATIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TAUROS_2] = {
        .species = SPECIES_TAUROS,
        .moves = {MOVE_BODY_SLAM, MOVE_ROCK_SLIDE, MOVE_IRON_HEAD, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SLOWBRO_2] = {
        .species = SPECIES_SLOWBRO,
        .moves = {MOVE_SCALD, MOVE_HEAL_PULSE, MOVE_ICY_WIND, MOVE_PROTECT},
        .heldItem = ITEM_ROCKY_HELMET,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_REGENERATOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SLOWKING_2] = {
        .species = SPECIES_SLOWKING,
        .moves = {MOVE_FUTURE_SIGHT, MOVE_CHILLY_RECEPTION, MOVE_SCALD, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_SASSY,
        .ability = ABILITY_REGENERATOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MILTANK_2] = {
        .species = SPECIES_MILTANK,
        .moves = {MOVE_BODY_PRESS, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_MILK_DRINK},
        .heldItem = ITEM_ROCKY_HELMET,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_THICK_FAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ALTARIA_2] = {
        .species = SPECIES_ALTARIA,
        .moves = {MOVE_DRACO_METEOR, MOVE_HEAT_WAVE, MOVE_MOONBLAST, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_NATURAL_CURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NIDOQUEEN_2] = {
        .species = SPECIES_NIDOQUEEN,
        .moves = {MOVE_EARTH_POWER, MOVE_SLUDGE_BOMB, MOVE_ICE_BEAM, MOVE_FIRE_BLAST},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NIDOKING_2] = {
        .species = SPECIES_NIDOKING,
        .moves = {MOVE_SLUDGE_BOMB, MOVE_EARTH_POWER, MOVE_ICE_BEAM, MOVE_THUNDERBOLT},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MAGMAR_2] = {
        .species = SPECIES_MAGMAR,
        .moves = {MOVE_HEAT_WAVE, MOVE_SCORCHING_SANDS, MOVE_FOCUS_BLAST, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_VITAL_SPIRIT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CRADILY_2] = {
        .species = SPECIES_CRADILY,
        .moves = {MOVE_STOCKPILE, MOVE_RECOVER, MOVE_LEECH_SEED, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_SUCTION_CUPS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ARMALDO_2] = {
        .species = SPECIES_ARMALDO,
        .moves = {MOVE_X_SCISSOR, MOVE_ROCK_SLIDE, MOVE_EARTHQUAKE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_BATTLE_ARMOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GOLDUCK_2] = {
        .species = SPECIES_GOLDUCK,
        .moves = {MOVE_SOAK, MOVE_CLEAR_SMOG, MOVE_ICY_WIND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 0, 2),
        .nature = NATURE_TIMID,
        .ability = ABILITY_CLOUD_NINE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RAPIDASH_2] = {
        .species = SPECIES_RAPIDASH,
        .moves = {MOVE_FLARE_BLITZ, MOVE_WILL_O_WISP, MOVE_MORNING_SUN, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 16, 0, 18, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_FLASH_FIRE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MUK_2] = {
        .species = SPECIES_MUK,
        .moves = {MOVE_POISON_JAB, MOVE_DRAIN_PUNCH, MOVE_HAZE, MOVE_PROTECT},
        .heldItem = ITEM_BLACK_SLUDGE,
        .ev = TRAINER_PARTY_EVS(32, 2, 0, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_POISON_TOUCH,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GENGAR_2] = {
        .species = SPECIES_GENGAR,
        .moves = {MOVE_SHADOW_BALL, MOVE_SLUDGE_BOMB, MOVE_FOCUS_BLAST, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_AMPHAROS_2] = {
        .species = SPECIES_AMPHAROS,
        .moves = {MOVE_DISCHARGE, MOVE_POWER_GEM, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_PLUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SCIZOR_2] = {
        .species = SPECIES_SCIZOR,
        .moves = {MOVE_BULLET_PUNCH, MOVE_U_TURN, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_TECHNICIAN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HERACROSS_2] = {
        .species = SPECIES_HERACROSS,
        .moves = {MOVE_CLOSE_COMBAT, MOVE_MEGAHORN, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_MOXIE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_URSARING_2] = {
        .species = SPECIES_URSARING,
        .moves = {MOVE_FACADE, MOVE_CLOSE_COMBAT, MOVE_CRUNCH, MOVE_PROTECT},
        .heldItem = ITEM_TOXIC_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_QUICK_FEET,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HOUNDOOM_2] = {
        .species = SPECIES_HOUNDOOM,
        .moves = {MOVE_HEAT_WAVE, MOVE_DARK_PULSE, MOVE_TAUNT, MOVE_DESTINY_BOND},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_UNNERVE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DONPHAN_2] = {
        .species = SPECIES_DONPHAN,
        .moves = {MOVE_ENDEAVOR, MOVE_ICE_SHARD, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .heldItem = ITEM_CUSTAP_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CLAYDOL_2] = {
        .species = SPECIES_CLAYDOL,
        .moves = {MOVE_REFLECT, MOVE_LIGHT_SCREEN, MOVE_EERIE_IMPULSE, MOVE_PSYCHIC},
        .heldItem = ITEM_LIGHT_CLAY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WAILORD_2] = {
        .species = SPECIES_WAILORD,
        .moves = {MOVE_SCALD, MOVE_ICY_WIND, MOVE_NOBLE_ROAR, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_DRIZZLE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NINETALES_2] = {
        .species = SPECIES_NINETALES,
        .moves = {MOVE_WEATHER_BALL, MOVE_ENCORE, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_HEAT_ROCK,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_DROUGHT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MACHAMP_2] = {
        .species = SPECIES_MACHAMP,
        .moves = {MOVE_DYNAMIC_PUNCH, MOVE_ROCK_SLIDE, MOVE_WIDE_GUARD, MOVE_QUICK_GUARD},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_NO_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SHUCKLE_2] = {
        .species = SPECIES_SHUCKLE,
        .moves = {MOVE_GUARD_SPLIT, MOVE_HELPING_HAND, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_MENTAL_HERB,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_RELAXED,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_STEELIX_2] = {
        .species = SPECIES_STEELIX,
        .moves = {MOVE_WIDE_GUARD, MOVE_BODY_PRESS, MOVE_HEAVY_SLAM, MOVE_ROCK_SLIDE},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TENTACRUEL_2] = {
        .species = SPECIES_TENTACRUEL,
        .moves = {MOVE_SCALD, MOVE_HAZE, MOVE_KNOCK_OFF, MOVE_FLIP_TURN},
        .heldItem = ITEM_BLACK_SLUDGE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_LIQUID_OOZE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_AERODACTYL_2] = {
        .species = SPECIES_AERODACTYL,
        .moves = {MOVE_ROCK_SLIDE, MOVE_TAILWIND, MOVE_WIDE_GUARD, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_UNNERVE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PORYGON2_2] = {
        .species = SPECIES_PORYGON2,
        .moves = {MOVE_EERIE_IMPULSE, MOVE_ICY_WIND, MOVE_RECOVER, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_TRACE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GARDEVOIR_2] = {
        .species = SPECIES_GARDEVOIR,
        .moves = {MOVE_DAZZLING_GLEAM, MOVE_HEAL_PULSE, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_TELEPATHY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_EXEGGUTOR_2] = {
        .species = SPECIES_EXEGGUTOR,
        .moves = {MOVE_TRICK_ROOM, MOVE_LEAF_STORM, MOVE_PSYSHOCK, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_QUIET,
        .ability = ABILITY_HARVEST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_STARMIE_2] = {
        .species = SPECIES_STARMIE,
        .moves = {MOVE_SCALD, MOVE_ICY_WIND, MOVE_RECOVER, MOVE_PROTECT},
        .heldItem = ITEM_COLBUR_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_BOLD,
        .ability = ABILITY_NATURAL_CURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FLYGON_2] = {
        .species = SPECIES_FLYGON,
        .moves = {MOVE_DRACO_METEOR, MOVE_EARTH_POWER, MOVE_HEAT_WAVE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_TINTED_LENS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VENUSAUR_2] = {
        .species = SPECIES_VENUSAUR,
        .moves = {MOVE_GROWTH, MOVE_GIGA_DRAIN, MOVE_SLUDGE_BOMB, MOVE_WEATHER_BALL},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VAPOREON_2] = {
        .species = SPECIES_VAPOREON,
        .moves = {MOVE_WISH, MOVE_FLIP_TURN, MOVE_HAZE, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_WATER_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_JOLTEON_2] = {
        .species = SPECIES_JOLTEON,
        .moves = {MOVE_DISCHARGE, MOVE_SHADOW_BALL, MOVE_ALLURING_VOICE, MOVE_PROTECT},
        .heldItem = ITEM_AIR_BALLOON,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_VOLT_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FLAREON_2] = {
        .species = SPECIES_FLAREON,
        .moves = {MOVE_HEAT_WAVE, MOVE_WILL_O_WISP, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 2, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_FLASH_FIRE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MEGANIUM_2] = {
        .species = SPECIES_MEGANIUM,
        .moves = {MOVE_HEAL_PULSE, MOVE_HELPING_HAND, MOVE_REFLECT, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_FLOWER_VEIL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ESPEON_2] = {
        .species = SPECIES_ESPEON,
        .moves = {MOVE_REFLECT, MOVE_LIGHT_SCREEN, MOVE_PSYCHIC, MOVE_PROTECT},
        .heldItem = ITEM_LIGHT_CLAY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_MAGIC_BOUNCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_UMBREON_2] = {
        .species = SPECIES_UMBREON,
        .moves = {MOVE_WISH, MOVE_FOUL_PLAY, MOVE_YAWN, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BLASTOISE_2] = {
        .species = SPECIES_BLASTOISE,
        .moves = {MOVE_SHELL_SMASH, MOVE_MUDDY_WATER, MOVE_AURA_SPHERE, MOVE_PROTECT},
        .heldItem = ITEM_WHITE_HERB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_MEGA_LAUNCHER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FERALIGATR_2] = {
        .species = SPECIES_FERALIGATR,
        .moves = {MOVE_LIQUIDATION, MOVE_CRUNCH, MOVE_ICE_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_AGGRON_2] = {
        .species = SPECIES_AGGRON,
        .moves = {MOVE_HEAVY_SLAM, MOVE_BODY_PRESS, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_HEAVY_METAL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BLAZIKEN_2] = {
        .species = SPECIES_BLAZIKEN,
        .moves = {MOVE_COACHING, MOVE_HELPING_HAND, MOVE_CLOSE_COMBAT, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_BLAZE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WALREIN_2] = {
        .species = SPECIES_WALREIN,
        .moves = {MOVE_SWORDS_DANCE, MOVE_LIQUIDATION, MOVE_ICICLE_SPEAR, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_OBLIVIOUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SCEPTILE_2] = {
        .species = SPECIES_SCEPTILE,
        .moves = {MOVE_BREAKING_SWIPE, MOVE_QUICK_GUARD, MOVE_HELPING_HAND, MOVE_GIGA_DRAIN},
        .heldItem = ITEM_COVERT_CLOAK,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_OVERGROW,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CHARIZARD_2] = {
        .species = SPECIES_CHARIZARD,
        .moves = {MOVE_HEAT_WAVE, MOVE_SOLAR_BEAM, MOVE_SCORCHING_SANDS, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_SOLAR_POWER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TYPHLOSION_2] = {
        .species = SPECIES_TYPHLOSION,
        .moves = {MOVE_ERUPTION, MOVE_HEAT_WAVE, MOVE_SCORCHING_SANDS, MOVE_PROTECT},
        .heldItem = ITEM_CHARCOAL,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FLASH_FIRE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LAPRAS_2] = {
        .species = SPECIES_LAPRAS,
        .moves = {MOVE_PERISH_SONG, MOVE_FREEZE_DRY, MOVE_HYDRO_PUMP, MOVE_PROTECT},
        .heldItem = ITEM_ROCKY_HELMET,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_SHELL_ARMOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CROBAT_2] = {
        .species = SPECIES_CROBAT,
        .moves = {MOVE_QUICK_GUARD, MOVE_BRAVE_BIRD, MOVE_HAZE, MOVE_PROTECT},
        .heldItem = ITEM_BLACK_SLUDGE,
        .ev = TRAINER_PARTY_EVS(18, 16, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SWAMPERT_2] = {
        .species = SPECIES_SWAMPERT,
        .moves = {MOVE_LIQUIDATION, MOVE_HIGH_HORSEPOWER, MOVE_ICE_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_CLEAR_AMULET,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_TORRENT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GYARADOS_2] = {
        .species = SPECIES_GYARADOS,
        .moves = {MOVE_DRAGON_DANCE, MOVE_WATERFALL, MOVE_POWER_WHIP, MOVE_PROTECT},
        .heldItem = ITEM_CLEAR_AMULET,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_MOXIE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SNORLAX_2] = {
        .species = SPECIES_SNORLAX,
        .moves = {MOVE_CURSE, MOVE_BODY_SLAM, MOVE_HIGH_HORSEPOWER, MOVE_RECYCLE},
        .heldItem = ITEM_FIGY_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_GLUTTONY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KINGDRA_2] = {
        .species = SPECIES_KINGDRA,
        .moves = {MOVE_FOCUS_ENERGY, MOVE_DRACO_METEOR, MOVE_HYDRO_PUMP, MOVE_PROTECT},
        .heldItem = ITEM_SCOPE_LENS,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SNIPER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BLISSEY_2] = {
        .species = SPECIES_BLISSEY,
        .moves = {MOVE_GRAVITY, MOVE_SING, MOVE_HELPING_HAND, MOVE_SOFT_BOILED},
        .heldItem = ITEM_MENTAL_HERB,
        .ev = TRAINER_PARTY_EVS(2, 0, 32, 0, 0, 32),
        .nature = NATURE_BOLD,
        .ability = ABILITY_HEALER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MILOTIC_2] = {
        .species = SPECIES_MILOTIC,
        .moves = {MOVE_MUDDY_WATER, MOVE_ICE_BEAM, MOVE_FLIP_TURN, MOVE_RECOVER},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_COMPETITIVE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ARCANINE_2] = {
        .species = SPECIES_ARCANINE,
        .moves = {MOVE_FLARE_BLITZ, MOVE_EXTREME_SPEED, MOVE_CLOSE_COMBAT, MOVE_PROTECT},
        .heldItem = ITEM_CLEAR_AMULET,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SALAMENCE_2] = {
        .species = SPECIES_SALAMENCE,
        .moves = {MOVE_DRAGON_DANCE, MOVE_DRAGON_CLAW, MOVE_DUAL_WINGBEAT, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_METAGROSS_2] = {
        .species = SPECIES_METAGROSS,
        .moves = {MOVE_ROCK_POLISH, MOVE_METEOR_MASH, MOVE_PSYCHIC_FANGS, MOVE_PROTECT},
        .heldItem = ITEM_WEAKNESS_POLICY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SLAKING_2] = {
        .species = SPECIES_SLAKING,
        .moves = {MOVE_RETALIATE, MOVE_DOUBLE_EDGE, MOVE_HIGH_HORSEPOWER, MOVE_GUNK_SHOT},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_TRUANT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DUGTRIO_3] = {
        .species = SPECIES_DUGTRIO,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_STONE_EDGE, MOVE_NIGHT_SLASH, MOVE_MEMENTO},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_ARENA_TRAP,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MEDICHAM_3] = {
        .species = SPECIES_MEDICHAM,
        .moves = {MOVE_FAKE_OUT, MOVE_HELPING_HAND, MOVE_QUICK_GUARD, MOVE_RECOVER},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_TELEPATHY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MISDREAVUS_3] = {
        .species = SPECIES_MISDREAVUS,
        .moves = {MOVE_PERISH_SONG, MOVE_MEAN_LOOK, MOVE_PAIN_SPLIT, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FEAROW_3] = {
        .species = SPECIES_FEAROW,
        .moves = {MOVE_FOCUS_ENERGY, MOVE_DRILL_PECK, MOVE_DRILL_RUN, MOVE_PROTECT},
        .heldItem = ITEM_SCOPE_LENS,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SNIPER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GRANBULL_3] = {
        .species = SPECIES_GRANBULL,
        .moves = {MOVE_FACADE, MOVE_PLAY_ROUGH, MOVE_CLOSE_COMBAT, MOVE_PROTECT},
        .heldItem = ITEM_TOXIC_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_QUICK_FEET,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_JYNX_3] = {
        .species = SPECIES_JYNX,
        .moves = {MOVE_NASTY_PLOT, MOVE_LOVELY_KISS, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_DRY_SKIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DUSCLOPS_3] = {
        .species = SPECIES_DUSCLOPS,
        .moves = {MOVE_TRICK_ROOM, MOVE_IMPRISON, MOVE_NIGHT_SHADE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_RELAXED,
        .ability = ABILITY_FRISK,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DODRIO_3] = {
        .species = SPECIES_DODRIO,
        .moves = {MOVE_SWORDS_DANCE, MOVE_BRAVE_BIRD, MOVE_JUMP_KICK, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_MOXIE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MR_MIME_3] = {
        .species = SPECIES_MR_MIME,
        .moves = {MOVE_PSYCHIC, MOVE_DAZZLING_GLEAM, MOVE_FOCUS_BLAST, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_SOUNDPROOF,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LANTURN_3] = {
        .species = SPECIES_LANTURN,
        .moves = {MOVE_MUDDY_WATER, MOVE_THUNDER, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_VOLT_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BRELOOM_3] = {
        .species = SPECIES_BRELOOM,
        .moves = {MOVE_SPORE, MOVE_DRAIN_PUNCH, MOVE_LEECH_SEED, MOVE_PROTECT},
        .heldItem = ITEM_TOXIC_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 0, 0, 0, 2),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_POISON_HEAL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FORRETRESS_3] = {
        .species = SPECIES_FORRETRESS,
        .moves = {MOVE_GYRO_BALL, MOVE_EARTHQUAKE, MOVE_EXPLOSION, MOVE_PROTECT},
        .heldItem = ITEM_CUSTAP_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WHISCASH_3] = {
        .species = SPECIES_WHISCASH,
        .moves = {MOVE_REST, MOVE_MUDDY_WATER, MOVE_EARTH_POWER, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_HYDRATION,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_XATU_3] = {
        .species = SPECIES_XATU,
        .moves = {MOVE_PSYCHIC, MOVE_HEAT_WAVE, MOVE_U_TURN, MOVE_ROOST},
        .heldItem = ITEM_ROCKY_HELMET,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 2, 0),
        .nature = NATURE_BOLD,
        .ability = ABILITY_MAGIC_BOUNCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SKARMORY_3] = {
        .species = SPECIES_SKARMORY,
        .moves = {MOVE_ICY_WIND, MOVE_BRAVE_BIRD, MOVE_TAUNT, MOVE_PROTECT},
        .heldItem = ITEM_MENTAL_HERB,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MAROWAK_3] = {
        .species = SPECIES_MAROWAK,
        .moves = {MOVE_DOUBLE_EDGE, MOVE_BONEMERANG, MOVE_FIRE_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_THICK_CLUB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_ROCK_HEAD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_QUAGSIRE_3] = {
        .species = SPECIES_QUAGSIRE,
        .moves = {MOVE_ICY_WIND, MOVE_SCALD, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_DAMP,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CLEFABLE_3] = {
        .species = SPECIES_CLEFABLE,
        .moves = {MOVE_DAZZLING_GLEAM, MOVE_FLAMETHROWER, MOVE_THUNDERBOLT, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_MAGIC_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HARIYAMA_3] = {
        .species = SPECIES_HARIYAMA,
        .moves = {MOVE_FAKE_OUT, MOVE_COACHING, MOVE_WIDE_GUARD, MOVE_DRAIN_PUNCH},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 2, 0, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_THICK_FAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RAICHU_3] = {
        .species = SPECIES_RAICHU,
        .moves = {MOVE_NASTY_PLOT, MOVE_THUNDERBOLT, MOVE_FOCUS_BLAST, MOVE_SURF},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LIGHTNING_ROD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DEWGONG_3] = {
        .species = SPECIES_DEWGONG,
        .moves = {MOVE_SURF, MOVE_FREEZE_DRY, MOVE_REST, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_HYDRATION,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MANECTRIC_3] = {
        .species = SPECIES_MANECTRIC,
        .moves = {MOVE_THUNDERBOLT, MOVE_OVERHEAT, MOVE_SIGNAL_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_MINUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VILEPLUME_3] = {
        .species = SPECIES_VILEPLUME,
        .moves = {MOVE_POLLEN_PUFF, MOVE_STRENGTH_SAP, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_EFFECT_SPORE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VICTREEBEL_3] = {
        .species = SPECIES_VICTREEBEL,
        .moves = {MOVE_STRENGTH_SAP, MOVE_KNOCK_OFF, MOVE_ENCORE, MOVE_PROTECT},
        .heldItem = ITEM_ROCKY_HELMET,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_GLUTTONY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ELECTRODE_3] = {
        .species = SPECIES_ELECTRODE,
        .moves = {MOVE_THUNDER_WAVE, MOVE_TAUNT, MOVE_EXPLOSION, MOVE_THUNDERBOLT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_SASSY,
        .ability = ABILITY_SOUNDPROOF,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_EXPLOUD_3] = {
        .species = SPECIES_EXPLOUD,
        .moves = {MOVE_HYPER_VOICE, MOVE_ICY_WIND, MOVE_FAKE_TEARS, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_SOUNDPROOF,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SHIFTRY_3] = {
        .species = SPECIES_SHIFTRY,
        .moves = {MOVE_FAKE_OUT, MOVE_KNOCK_OFF, MOVE_ICY_WIND, MOVE_PROTECT},
        .heldItem = ITEM_NONE,
        .ev = TRAINER_PARTY_EVS(32, 2, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_PICKPOCKET,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GLALIE_3] = {
        .species = SPECIES_GLALIE,
        .moves = {MOVE_SNOWSCAPE, MOVE_BLIZZARD, MOVE_FREEZE_DRY, MOVE_PROTECT},
        .heldItem = ITEM_ICY_ROCK,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_ICE_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LUDICOLO_3] = {
        .species = SPECIES_LUDICOLO,
        .moves = {MOVE_LEECH_SEED, MOVE_SCALD, MOVE_SUBSTITUTE, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_RAIN_DISH,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HYPNO_3] = {
        .species = SPECIES_HYPNO,
        .moves = {MOVE_HYPNOSIS, MOVE_THUNDER_WAVE, MOVE_PSYCHIC_NOISE, MOVE_PROTECT},
        .heldItem = ITEM_WIDE_LENS,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_INSOMNIA,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GOLEM_3] = {
        .species = SPECIES_GOLEM,
        .moves = {MOVE_STEALTH_ROCK, MOVE_EARTHQUAKE, MOVE_ROCK_SLIDE, MOVE_EXPLOSION},
        .heldItem = ITEM_CUSTAP_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RHYDON_3] = {
        .species = SPECIES_RHYDON,
        .moves = {MOVE_HELPING_HAND, MOVE_BODY_PRESS, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_LIGHTNING_ROD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ALAKAZAM_3] = {
        .species = SPECIES_ALAKAZAM,
        .moves = {MOVE_IMPRISON, MOVE_TRICK_ROOM, MOVE_PSYCHIC, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WEEZING_3] = {
        .species = SPECIES_WEEZING,
        .moves = {MOVE_SLUDGE_BOMB, MOVE_HEAT_WAVE, MOVE_THUNDERBOLT, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KANGASKHAN_3] = {
        .species = SPECIES_KANGASKHAN,
        .moves = {MOVE_FAKE_OUT, MOVE_HELPING_HAND, MOVE_SEISMIC_TOSS, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_SCRAPPY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ELECTABUZZ_3] = {
        .species = SPECIES_ELECTABUZZ,
        .moves = {MOVE_THUNDERBOLT, MOVE_FOCUS_BLAST, MOVE_PSYCHIC, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_VITAL_SPIRIT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TAUROS_3] = {
        .species = SPECIES_TAUROS,
        .moves = {MOVE_RAGING_BULL, MOVE_ROCK_SLIDE, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_ANGER_POINT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SLOWBRO_3] = {
        .species = SPECIES_SLOWBRO,
        .moves = {MOVE_FUTURE_SIGHT, MOVE_SCALD, MOVE_YAWN, MOVE_PROTECT},
        .heldItem = ITEM_COLBUR_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_REGENERATOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SLOWKING_3] = {
        .species = SPECIES_SLOWKING,
        .moves = {MOVE_SCALD, MOVE_PSYCHIC, MOVE_ICY_WIND, MOVE_FLAMETHROWER},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_REGENERATOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MILTANK_3] = {
        .species = SPECIES_MILTANK,
        .moves = {MOVE_DOUBLE_EDGE, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SCRAPPY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ALTARIA_3] = {
        .species = SPECIES_ALTARIA,
        .moves = {MOVE_DRAGON_DANCE, MOVE_DRAGON_CLAW, MOVE_BRAVE_BIRD, MOVE_PROTECT},
        .heldItem = ITEM_LUM_BERRY,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_NATURAL_CURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NIDOQUEEN_3] = {
        .species = SPECIES_NIDOQUEEN,
        .moves = {MOVE_HIGH_HORSEPOWER, MOVE_POISON_JAB, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NIDOKING_3] = {
        .species = SPECIES_NIDOKING,
        .moves = {MOVE_POISON_JAB, MOVE_DRILL_RUN, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MAGMAR_3] = {
        .species = SPECIES_MAGMAR,
        .moves = {MOVE_BELLY_DRUM, MOVE_FIRE_PUNCH, MOVE_THUNDER_PUNCH, MOVE_MACH_PUNCH},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_FLAME_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CRADILY_3] = {
        .species = SPECIES_CRADILY,
        .moves = {MOVE_METEOR_BEAM, MOVE_ENERGY_BALL, MOVE_EARTH_POWER, MOVE_PROTECT},
        .heldItem = ITEM_POWER_HERB,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_QUIET,
        .ability = ABILITY_STORM_DRAIN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ARMALDO_3] = {
        .species = SPECIES_ARMALDO,
        .moves = {MOVE_SWORDS_DANCE, MOVE_X_SCISSOR, MOVE_ROCK_SLIDE, MOVE_AQUA_JET},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_BATTLE_ARMOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GOLDUCK_3] = {
        .species = SPECIES_GOLDUCK,
        .moves = {MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_GRASS_KNOT, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SWIFT_SWIM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RAPIDASH_3] = {
        .species = SPECIES_RAPIDASH,
        .moves = {MOVE_SWORDS_DANCE, MOVE_FLARE_BLITZ, MOVE_SOLAR_BLADE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_RECKLESS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MUK_3] = {
        .species = SPECIES_MUK,
        .moves = {MOVE_CURSE, MOVE_POISON_JAB, MOVE_DRAIN_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_BLACK_SLUDGE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_STICKY_HOLD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GENGAR_3] = {
        .species = SPECIES_GENGAR,
        .moves = {MOVE_SHADOW_BALL, MOVE_WILL_O_WISP, MOVE_TAUNT, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_CURSED_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_AMPHAROS_3] = {
        .species = SPECIES_AMPHAROS,
        .moves = {MOVE_COTTON_GUARD, MOVE_THUNDERBOLT, MOVE_EERIE_IMPULSE, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_STATIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SCIZOR_3] = {
        .species = SPECIES_SCIZOR,
        .moves = {MOVE_FEINT, MOVE_BULLET_PUNCH, MOVE_CLOSE_COMBAT, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_TECHNICIAN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HERACROSS_3] = {
        .species = SPECIES_HERACROSS,
        .moves = {MOVE_CLOSE_COMBAT, MOVE_MEGAHORN, MOVE_KNOCK_OFF, MOVE_ROCK_SLIDE},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_MOXIE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_URSARING_3] = {
        .species = SPECIES_URSARING,
        .moves = {MOVE_FACADE, MOVE_HIGH_HORSEPOWER, MOVE_ICE_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HOUNDOOM_3] = {
        .species = SPECIES_HOUNDOOM,
        .moves = {MOVE_NASTY_PLOT, MOVE_HEAT_WAVE, MOVE_DARK_PULSE, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FLASH_FIRE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DONPHAN_3] = {
        .species = SPECIES_DONPHAN,
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CLAYDOL_3] = {
        .species = SPECIES_CLAYDOL,
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_EARTH_POWER, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WAILORD_3] = {
        .species = SPECIES_WAILORD,
        .moves = {MOVE_CURSE, MOVE_HEAVY_SLAM, MOVE_LIQUIDATION, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 2, 0, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_WATER_VEIL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NINETALES_3] = {
        .species = SPECIES_NINETALES,
        .moves = {MOVE_WILL_O_WISP, MOVE_SNARL, MOVE_ENCORE, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FLASH_FIRE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MACHAMP_3] = {
        .species = SPECIES_MACHAMP,
        .moves = {MOVE_CLOSE_COMBAT, MOVE_FACADE, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_FLAME_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SHUCKLE_3] = {
        .species = SPECIES_SHUCKLE,
        .moves = {MOVE_POWER_TRICK, MOVE_ROCK_SLIDE, MOVE_EARTHQUAKE, MOVE_PROTECT},
        .heldItem = ITEM_HARD_STONE,
        .ev = TRAINER_PARTY_EVS(2, 0, 32, 0, 0, 32),
        .nature = NATURE_RELAXED,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_STEELIX_3] = {
        .species = SPECIES_STEELIX,
        .moves = {MOVE_EARTHQUAKE, MOVE_BODY_PRESS, MOVE_STEALTH_ROCK, MOVE_DRAGON_TAIL},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TENTACRUEL_3] = {
        .species = SPECIES_TENTACRUEL,
        .moves = {MOVE_SCALD, MOVE_SLUDGE_BOMB, MOVE_ICY_WIND, MOVE_MIRROR_COAT},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 2, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_LIQUID_OOZE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_AERODACTYL_3] = {
        .species = SPECIES_AERODACTYL,
        .moves = {MOVE_ROCK_SLIDE, MOVE_SKY_DROP, MOVE_TAUNT, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_UNNERVE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PORYGON2_3] = {
        .species = SPECIES_PORYGON2,
        .moves = {MOVE_TRI_ATTACK, MOVE_THUNDERBOLT, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_DOWNLOAD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GARDEVOIR_3] = {
        .species = SPECIES_GARDEVOIR,
        .moves = {MOVE_TRICK_ROOM, MOVE_MOONBLAST, MOVE_MYSTICAL_FIRE, MOVE_PROTECT},
        .heldItem = ITEM_MENTAL_HERB,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_QUIET,
        .ability = ABILITY_TRACE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_EXEGGUTOR_3] = {
        .species = SPECIES_EXEGGUTOR,
        .moves = {MOVE_SLEEP_POWDER, MOVE_HELPING_HAND, MOVE_SKILL_SWAP, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_SASSY,
        .ability = ABILITY_HARVEST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_STARMIE_3] = {
        .species = SPECIES_STARMIE,
        .moves = {MOVE_GRAVITY, MOVE_HYDRO_PUMP, MOVE_THUNDER, MOVE_BLIZZARD},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_ANALYTIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FLYGON_3] = {
        .species = SPECIES_FLYGON,
        .moves = {MOVE_TAILWIND, MOVE_BREAKING_SWIPE, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_COVERT_CLOAK,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VENUSAUR_3] = {
        .species = SPECIES_VENUSAUR,
        .moves = {MOVE_GIGA_DRAIN, MOVE_KNOCK_OFF, MOVE_SLEEP_POWDER, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 16, 0, 0, 18),
        .nature = NATURE_BOLD,
        .ability = ABILITY_THICK_FAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VAPOREON_3] = {
        .species = SPECIES_VAPOREON,
        .moves = {MOVE_ACID_ARMOR, MOVE_CALM_MIND, MOVE_STORED_POWER, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 2, 0),
        .nature = NATURE_BOLD,
        .ability = ABILITY_WATER_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_JOLTEON_3] = {
        .species = SPECIES_JOLTEON,
        .moves = {MOVE_YAWN, MOVE_FAKE_TEARS, MOVE_VOLT_SWITCH, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_VOLT_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FLAREON_3] = {
        .species = SPECIES_FLAREON,
        .moves = {MOVE_FLARE_BLITZ, MOVE_FACADE, MOVE_QUICK_ATTACK, MOVE_PROTECT},
        .heldItem = ITEM_TOXIC_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MEGANIUM_3] = {
        .species = SPECIES_MEGANIUM,
        .moves = {MOVE_GIGA_DRAIN, MOVE_DAZZLING_GLEAM, MOVE_EARTH_POWER, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_TRIAGE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ESPEON_3] = {
        .species = SPECIES_ESPEON,
        .moves = {MOVE_TRICK_ROOM, MOVE_PSYCHIC, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_QUIET,
        .ability = ABILITY_MAGIC_BOUNCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_UMBREON_3] = {
        .species = SPECIES_UMBREON,
        .moves = {MOVE_SNARL, MOVE_TAUNT, MOVE_THUNDER_WAVE, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_SYNCHRONIZE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BLASTOISE_3] = {
        .species = SPECIES_BLASTOISE,
        .moves = {MOVE_SHELL_SMASH, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_DARK_PULSE},
        .heldItem = ITEM_WHITE_HERB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_MEGA_LAUNCHER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FERALIGATR_3] = {
        .species = SPECIES_FERALIGATR,
        .moves = {MOVE_HELPING_HAND, MOVE_ICY_WIND, MOVE_FLIP_TURN, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_TORRENT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_AGGRON_3] = {
        .species = SPECIES_AGGRON,
        .moves = {MOVE_METAL_BURST, MOVE_HEAVY_SLAM, MOVE_THUNDER_WAVE, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 17, 0, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BLAZIKEN_3] = {
        .species = SPECIES_BLAZIKEN,
        .moves = {MOVE_SWORDS_DANCE, MOVE_FLARE_BLITZ, MOVE_CLOSE_COMBAT, MOVE_PROTECT},
        .heldItem = ITEM_CLEAR_AMULET,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SPEED_BOOST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WALREIN_3] = {
        .species = SPECIES_WALREIN,
        .moves = {MOVE_STOCKPILE, MOVE_BODY_PRESS, MOVE_SUPER_FANG, MOVE_REST},
        .heldItem = ITEM_CHESTO_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_THICK_FAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SCEPTILE_3] = {
        .species = SPECIES_SCEPTILE,
        .moves = {MOVE_LEAF_STORM, MOVE_GIGA_DRAIN, MOVE_FOCUS_BLAST, MOVE_PROTECT},
        .heldItem = ITEM_WHITE_HERB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_UNBURDEN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CHARIZARD_3] = {
        .species = SPECIES_CHARIZARD,
        .moves = {MOVE_FLAMETHROWER, MOVE_HURRICANE, MOVE_ROOST, MOVE_WILL_O_WISP},
        .heldItem = ITEM_HEAVY_DUTY_BOOTS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_BLAZE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TYPHLOSION_3] = {
        .species = SPECIES_TYPHLOSION,
        .moves = {MOVE_HEAT_WAVE, MOVE_WILL_O_WISP, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_TIMID,
        .ability = ABILITY_BLAZE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LAPRAS_3] = {
        .species = SPECIES_LAPRAS,
        .moves = {MOVE_LIFE_DEW, MOVE_HELPING_HAND, MOVE_FREEZE_DRY, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_WATER_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CROBAT_3] = {
        .species = SPECIES_CROBAT,
        .moves = {MOVE_BRAVE_BIRD, MOVE_U_TURN, MOVE_SUPER_FANG, MOVE_PROTECT},
        .heldItem = ITEM_SHARP_BEAK,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SWAMPERT_3] = {
        .species = SPECIES_SWAMPERT,
        .moves = {MOVE_CURSE, MOVE_LIQUIDATION, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_TORRENT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GYARADOS_3] = {
        .species = SPECIES_GYARADOS,
        .moves = {MOVE_WATERFALL, MOVE_ICY_WIND, MOVE_THUNDER_WAVE, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SNORLAX_3] = {
        .species = SPECIES_SNORLAX,
        .moves = {MOVE_BODY_SLAM, MOVE_HIGH_HORSEPOWER, MOVE_HEAVY_SLAM, MOVE_FIRE_PUNCH},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_THICK_FAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KINGDRA_3] = {
        .species = SPECIES_KINGDRA,
        .moves = {MOVE_ICY_WIND, MOVE_CLEAR_SMOG, MOVE_SCALD, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_DAMP,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BLISSEY_3] = {
        .species = SPECIES_BLISSEY,
        .moves = {MOVE_ICY_WIND, MOVE_FLAMETHROWER, MOVE_HEAL_PULSE, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(2, 0, 32, 0, 32, 0),
        .nature = NATURE_BOLD,
        .ability = ABILITY_SERENE_GRACE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MILOTIC_3] = {
        .species = SPECIES_MILOTIC,
        .moves = {MOVE_SCALD, MOVE_HAZE, MOVE_RECOVER, MOVE_PROTECT},
        .heldItem = ITEM_FLAME_ORB,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_MARVEL_SCALE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ARCANINE_3] = {
        .species = SPECIES_ARCANINE,
        .moves = {MOVE_FLARE_BLITZ, MOVE_EXTREME_SPEED, MOVE_HOWL, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_JUSTIFIED,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SALAMENCE_3] = {
        .species = SPECIES_SALAMENCE,
        .moves = {MOVE_DRAGON_CLAW, MOVE_DUAL_WINGBEAT, MOVE_FIRE_FANG, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_MOXIE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_METAGROSS_3] = {
        .species = SPECIES_METAGROSS,
        .moves = {MOVE_METEOR_MASH, MOVE_PSYCHIC_FANGS, MOVE_EXPLOSION, MOVE_PROTECT},
        .heldItem = ITEM_NORMAL_GEM,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SLAKING_3] = {
        .species = SPECIES_SLAKING,
        .moves = {MOVE_BODY_SLAM, MOVE_HIGH_HORSEPOWER, MOVE_KNOCK_OFF, MOVE_HEAVY_SLAM},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 0, 0, 0, 2),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_TRUANT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DUGTRIO_4] = {
        .species = SPECIES_DUGTRIO,
        .moves = {MOVE_HELPING_HAND, MOVE_STEALTH_ROCK, MOVE_MEMENTO, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_ARENA_TRAP,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MEDICHAM_4] = {
        .species = SPECIES_MEDICHAM,
        .moves = {MOVE_HIGH_JUMP_KICK, MOVE_PSYCHO_CUT, MOVE_ICE_PUNCH, MOVE_TRICK},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_PURE_POWER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MISDREAVUS_4] = {
        .species = SPECIES_MISDREAVUS,
        .moves = {MOVE_NASTY_PLOT, MOVE_SHADOW_BALL, MOVE_DAZZLING_GLEAM, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FEAROW_4] = {
        .species = SPECIES_FEAROW,
        .moves = {MOVE_DRILL_PECK, MOVE_DOUBLE_EDGE, MOVE_DRILL_RUN, MOVE_U_TURN},
        .heldItem = ITEM_CHOICE_BAND,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SNIPER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GRANBULL_4] = {
        .species = SPECIES_GRANBULL,
        .moves = {MOVE_PLAY_ROUGH, MOVE_LASH_OUT, MOVE_STOMPING_TANTRUM, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_RATTLED,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_JYNX_4] = {
        .species = SPECIES_JYNX,
        .moves = {MOVE_TRICK_ROOM, MOVE_FAKE_OUT, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_OBLIVIOUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DUSCLOPS_4] = {
        .species = SPECIES_DUSCLOPS,
        .moves = {MOVE_WILL_O_WISP, MOVE_NIGHT_SHADE, MOVE_PAIN_SPLIT, MOVE_HAZE},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_SASSY,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DODRIO_4] = {
        .species = SPECIES_DODRIO,
        .moves = {MOVE_TAILWIND, MOVE_BRAVE_BIRD, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 32, 0, 2, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_EARLY_BIRD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MR_MIME_4] = {
        .species = SPECIES_MR_MIME,
        .moves = {MOVE_TRICK_ROOM, MOVE_FAKE_OUT, MOVE_DAZZLING_GLEAM, MOVE_PROTECT},
        .heldItem = ITEM_MENTAL_HERB,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FILTER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LANTURN_4] = {
        .species = SPECIES_LANTURN,
        .moves = {MOVE_VOLT_SWITCH, MOVE_SCALD, MOVE_ICE_BEAM, MOVE_THUNDERBOLT},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_VOLT_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BRELOOM_4] = {
        .species = SPECIES_BRELOOM,
        .moves = {MOVE_SPORE, MOVE_POUNCE, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_ROCKY_HELMET,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_EFFECT_SPORE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FORRETRESS_4] = {
        .species = SPECIES_FORRETRESS,
        .moves = {MOVE_REFLECT, MOVE_LIGHT_SCREEN, MOVE_VOLT_SWITCH, MOVE_PROTECT},
        .heldItem = ITEM_LIGHT_CLAY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_SASSY,
        .ability = ABILITY_OVERCOAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WHISCASH_4] = {
        .species = SPECIES_WHISCASH,
        .moves = {MOVE_DRAGON_DANCE, MOVE_EARTHQUAKE, MOVE_LIQUIDATION, MOVE_STONE_EDGE},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_OBLIVIOUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_XATU_4] = {
        .species = SPECIES_XATU,
        .moves = {MOVE_TRICK_ROOM, MOVE_IMPRISON, MOVE_PSYCHIC, MOVE_PROTECT},
        .heldItem = ITEM_COVERT_CLOAK,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_MAGIC_BOUNCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SKARMORY_4] = {
        .species = SPECIES_SKARMORY,
        .moves = {MOVE_BRAVE_BIRD, MOVE_IRON_HEAD, MOVE_DRILL_RUN, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_WEAK_ARMOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MAROWAK_4] = {
        .species = SPECIES_MAROWAK,
        .moves = {MOVE_SWORDS_DANCE, MOVE_BONEMERANG, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_THICK_CLUB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_BATTLE_ARMOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_QUAGSIRE_4] = {
        .species = SPECIES_QUAGSIRE,
        .moves = {MOVE_CURSE, MOVE_LIQUIDATION, MOVE_HIGH_HORSEPOWER, MOVE_RECOVER},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_WATER_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CLEFABLE_4] = {
        .species = SPECIES_CLEFABLE,
        .moves = {MOVE_CALM_MIND, MOVE_MOONBLAST, MOVE_FLAMETHROWER, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_MAGIC_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HARIYAMA_4] = {
        .species = SPECIES_HARIYAMA,
        .moves = {MOVE_FACADE, MOVE_CLOSE_COMBAT, MOVE_KNOCK_OFF, MOVE_BULLET_PUNCH},
        .heldItem = ITEM_FLAME_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RAICHU_4] = {
        .species = SPECIES_RAICHU,
        .moves = {MOVE_THUNDERBOLT, MOVE_SURF, MOVE_FOCUS_BLAST, MOVE_VOLT_SWITCH},
        .heldItem = ITEM_CHOICE_SPECS,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LIGHTNING_ROD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DEWGONG_4] = {
        .species = SPECIES_DEWGONG,
        .moves = {MOVE_FAKE_OUT, MOVE_ICY_WIND, MOVE_ENCORE, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FUR_COAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MANECTRIC_4] = {
        .species = SPECIES_MANECTRIC,
        .moves = {MOVE_ELECTRIC_TERRAIN, MOVE_RISING_VOLTAGE, MOVE_OVERHEAT, MOVE_PROTECT},
        .heldItem = ITEM_TERRAIN_EXTENDER,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LIGHTNING_ROD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VILEPLUME_4] = {
        .species = SPECIES_VILEPLUME,
        .moves = {MOVE_ACID_SPRAY, MOVE_SLEEP_POWDER, MOVE_CORROSIVE_GAS, MOVE_PROTECT},
        .heldItem = ITEM_BLACK_SLUDGE,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_EFFECT_SPORE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VICTREEBEL_4] = {
        .species = SPECIES_VICTREEBEL,
        .moves = {MOVE_SWORDS_DANCE, MOVE_POWER_WHIP, MOVE_SUCKER_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_LIECHI_BERRY,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_GLUTTONY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ELECTRODE_4] = {
        .species = SPECIES_ELECTRODE,
        .moves = {MOVE_THUNDERBOLT, MOVE_TERA_BLAST, MOVE_TAUNT, MOVE_VOLT_SWITCH},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_QUIET,
        .ability = ABILITY_SOUNDPROOF,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_EXPLOUD_4] = {
        .species = SPECIES_EXPLOUD,
        .moves = {MOVE_BOOMBURST, MOVE_OVERHEAT, MOVE_HYDRO_PUMP, MOVE_FOCUS_BLAST},
        .heldItem = ITEM_CHOICE_SPECS,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_QUIET,
        .ability = ABILITY_SCRAPPY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SHIFTRY_4] = {
        .species = SPECIES_SHIFTRY,
        .moves = {MOVE_FAKE_OUT, MOVE_LEAF_STORM, MOVE_KNOCK_OFF, MOVE_SUCKER_PUNCH},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 16, 0, 32, 16, 0),
        .nature = NATURE_HASTY,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GLALIE_4] = {
        .species = SPECIES_GLALIE,
        .moves = {MOVE_SPIKES, MOVE_TAUNT, MOVE_FREEZE_DRY, MOVE_EXPLOSION},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 16, 0, 32, 16, 0),
        .nature = NATURE_NAIVE,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LUDICOLO_4] = {
        .species = SPECIES_LUDICOLO,
        .moves = {MOVE_LEAF_STORM, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_FOCUS_BLAST},
        .heldItem = ITEM_CHOICE_SPECS,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_OWN_TEMPO,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HYPNO_4] = {
        .species = SPECIES_HYPNO,
        .moves = {MOVE_BARRIER, MOVE_BODY_PRESS, MOVE_TRICK_ROOM, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GOLEM_4] = {
        .species = SPECIES_GOLEM,
        .moves = {MOVE_DOUBLE_EDGE, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_ROCK_HEAD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RHYDON_4] = {
        .species = SPECIES_RHYDON,
        .moves = {MOVE_DOUBLE_EDGE, MOVE_HIGH_HORSEPOWER, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_RECKLESS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ALAKAZAM_4] = {
        .species = SPECIES_ALAKAZAM,
        .moves = {MOVE_PSYCHIC, MOVE_DAZZLING_GLEAM, MOVE_COUNTER, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_MAGIC_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WEEZING_4] = {
        .species = SPECIES_WEEZING,
        .moves = {MOVE_SLUDGE_BOMB, MOVE_WILL_O_WISP, MOVE_MEMENTO, MOVE_PROTECT},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 2, 0),
        .nature = NATURE_BOLD,
        .ability = ABILITY_NEUTRALIZING_GAS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KANGASKHAN_4] = {
        .species = SPECIES_KANGASKHAN,
        .moves = {MOVE_FAKE_OUT, MOVE_DOUBLE_EDGE, MOVE_EARTHQUAKE, MOVE_SUCKER_PUNCH},
        .heldItem = ITEM_SILK_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SCRAPPY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ELECTABUZZ_4] = {
        .species = SPECIES_ELECTABUZZ,
        .moves = {MOVE_ELECTRIC_TERRAIN, MOVE_RISING_VOLTAGE, MOVE_FOCUS_BLAST, MOVE_PROTECT},
        .heldItem = ITEM_TERRAIN_EXTENDER,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_VITAL_SPIRIT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TAUROS_4] = {
        .species = SPECIES_TAUROS,
        .moves = {MOVE_DOUBLE_EDGE, MOVE_CLOSE_COMBAT, MOVE_HIGH_HORSEPOWER, MOVE_THROAT_CHOP},
        .heldItem = ITEM_CHOICE_BAND,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SLOWBRO_4] = {
        .species = SPECIES_SLOWBRO,
        .moves = {MOVE_CALM_MIND, MOVE_SCALD, MOVE_PSYCHIC_NOISE, MOVE_SLACK_OFF},
        .heldItem = ITEM_ROCKY_HELMET,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_REGENERATOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SLOWKING_4] = {
        .species = SPECIES_SLOWKING,
        .moves = {MOVE_FUTURE_SIGHT, MOVE_CHILLY_RECEPTION, MOVE_SURF, MOVE_SLACK_OFF},
        .heldItem = ITEM_HEAVY_DUTY_BOOTS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_SASSY,
        .ability = ABILITY_REGENERATOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MILTANK_4] = {
        .species = SPECIES_MILTANK,
        .moves = {MOVE_AFTER_YOU, MOVE_BODY_SLAM, MOVE_HELPING_HAND, MOVE_MILK_DRINK},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SAP_SIPPER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ALTARIA_4] = {
        .species = SPECIES_ALTARIA,
        .moves = {MOVE_PERISH_SONG, MOVE_HAZE, MOVE_ROOST, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_CLOUD_NINE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NIDOQUEEN_4] = {
        .species = SPECIES_NIDOQUEEN,
        .moves = {MOVE_BODY_PRESS, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_POISON_POINT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NIDOKING_4] = {
        .species = SPECIES_NIDOKING,
        .moves = {MOVE_SLUDGE_WAVE, MOVE_EARTH_POWER, MOVE_ICE_BEAM, MOVE_FLAMETHROWER},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MAGMAR_4] = {
        .species = SPECIES_MAGMAR,
        .moves = {MOVE_HEAT_WAVE, MOVE_ACID_SPRAY, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_VITAL_SPIRIT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CRADILY_4] = {
        .species = SPECIES_CRADILY,
        .moves = {MOVE_SWORDS_DANCE, MOVE_ROCK_SLIDE, MOVE_POWER_WHIP, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_SUCTION_CUPS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ARMALDO_4] = {
        .species = SPECIES_ARMALDO,
        .moves = {MOVE_ROCK_SLIDE, MOVE_KNOCK_OFF, MOVE_RAPID_SPIN, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_BATTLE_ARMOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GOLDUCK_4] = {
        .species = SPECIES_GOLDUCK,
        .moves = {MOVE_NASTY_PLOT, MOVE_HYDRO_PUMP, MOVE_PSYCHIC, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_CLOUD_NINE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RAPIDASH_4] = {
        .species = SPECIES_RAPIDASH,
        .moves = {MOVE_SWORDS_DANCE, MOVE_FLARE_BLITZ, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .heldItem = ITEM_CLEAR_AMULET,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_RECKLESS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MUK_4] = {
        .species = SPECIES_MUK,
        .moves = {MOVE_GUNK_SHOT, MOVE_DRAIN_PUNCH, MOVE_ICE_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_STENCH,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GENGAR_4] = {
        .species = SPECIES_GENGAR,
        .moves = {MOVE_NASTY_PLOT, MOVE_SHADOW_BALL, MOVE_SLUDGE_BOMB, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_AMPHAROS_4] = {
        .species = SPECIES_AMPHAROS,
        .moves = {MOVE_THUNDERBOLT, MOVE_FOCUS_BLAST, MOVE_POWER_GEM, MOVE_HEAL_BELL},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_STATIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SCIZOR_4] = {
        .species = SPECIES_SCIZOR,
        .moves = {MOVE_BULLET_PUNCH, MOVE_TAILWIND, MOVE_U_TURN, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_TECHNICIAN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HERACROSS_4] = {
        .species = SPECIES_HERACROSS,
        .moves = {MOVE_SWORDS_DANCE, MOVE_CLOSE_COMBAT, MOVE_KNOCK_OFF, MOVE_FACADE},
        .heldItem = ITEM_FLAME_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_URSARING_4] = {
        .species = SPECIES_URSARING,
        .moves = {MOVE_HELPING_HAND, MOVE_YAWN, MOVE_CLOSE_COMBAT, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_UNNERVE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_HOUNDOOM_4] = {
        .species = SPECIES_HOUNDOOM,
        .moves = {MOVE_SNARL, MOVE_WILL_O_WISP, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_COVERT_CLOAK,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 2, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FLASH_FIRE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DONPHAN_4] = {
        .species = SPECIES_DONPHAN,
        .moves = {MOVE_RAPID_SPIN, MOVE_HIGH_HORSEPOWER, MOVE_KNOCK_OFF, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CLAYDOL_4] = {
        .species = SPECIES_CLAYDOL,
        .moves = {MOVE_NASTY_PLOT, MOVE_EARTH_POWER, MOVE_PSYCHIC, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WAILORD_4] = {
        .species = SPECIES_WAILORD,
        .moves = {MOVE_WATER_SPOUT, MOVE_HYDRO_PUMP, MOVE_ICE_BEAM, MOVE_CLEAR_SMOG},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_DRIZZLE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_NINETALES_4] = {
        .species = SPECIES_NINETALES,
        .moves = {MOVE_NASTY_PLOT, MOVE_FLAMETHROWER, MOVE_ENERGY_BALL, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FLASH_FIRE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MACHAMP_4] = {
        .species = SPECIES_MACHAMP,
        .moves = {MOVE_COACHING, MOVE_HELPING_HAND, MOVE_WIDE_GUARD, MOVE_DRAIN_PUNCH},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SHUCKLE_4] = {
        .species = SPECIES_SHUCKLE,
        .moves = {MOVE_STICKY_WEB, MOVE_TOXIC, MOVE_KNOCK_OFF, MOVE_FINAL_GAMBIT},
        .heldItem = ITEM_MENTAL_HERB,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_STEELIX_4] = {
        .species = SPECIES_STEELIX,
        .moves = {MOVE_IRON_DEFENSE, MOVE_REST, MOVE_BODY_PRESS, MOVE_HEAVY_SLAM},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TENTACRUEL_4] = {
        .species = SPECIES_TENTACRUEL,
        .moves = {MOVE_MUDDY_WATER, MOVE_SLUDGE_BOMB, MOVE_ICY_WIND, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_RAIN_DISH,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_AERODACTYL_4] = {
        .species = SPECIES_AERODACTYL,
        .moves = {MOVE_DOUBLE_EDGE, MOVE_ROCK_SLIDE, MOVE_EARTHQUAKE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_ROCK_HEAD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_PORYGON2_4] = {
        .species = SPECIES_PORYGON2,
        .moves = {MOVE_TRI_ATTACK, MOVE_SHADOW_BALL, MOVE_RECOVER, MOVE_PROTECT},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_ANALYTIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GARDEVOIR_4] = {
        .species = SPECIES_GARDEVOIR,
        .moves = {MOVE_CALM_MIND, MOVE_DRAINING_KISS, MOVE_STORED_POWER, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 2, 0),
        .nature = NATURE_BOLD,
        .ability = ABILITY_TRACE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_EXEGGUTOR_4] = {
        .species = SPECIES_EXEGGUTOR,
        .moves = {MOVE_PSYCHIC_TERRAIN, MOVE_EXPANDING_FORCE, MOVE_SLEEP_POWDER, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_STARMIE_4] = {
        .species = SPECIES_STARMIE,
        .moves = {MOVE_RAPID_SPIN, MOVE_HYDRO_PUMP, MOVE_PSYCHIC, MOVE_ICE_BEAM},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_ANALYTIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FLYGON_4] = {
        .species = SPECIES_FLYGON,
        .moves = {MOVE_DRAGON_DANCE, MOVE_DRAGON_CLAW, MOVE_EARTHQUAKE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_TINTED_LENS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VENUSAUR_4] = {
        .species = SPECIES_VENUSAUR,
        .moves = {MOVE_GROWTH, MOVE_GIGA_DRAIN, MOVE_SLUDGE_BOMB, MOVE_WEATHER_BALL},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_CHLOROPHYLL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_VAPOREON_4] = {
        .species = SPECIES_VAPOREON,
        .moves = {MOVE_SURF, MOVE_ICE_BEAM, MOVE_REST, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_HYDRATION,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_JOLTEON_4] = {
        .species = SPECIES_JOLTEON,
        .moves = {MOVE_THUNDERBOLT, MOVE_VOLT_SWITCH, MOVE_SHADOW_BALL, MOVE_ALLURING_VOICE},
        .heldItem = ITEM_CHOICE_SPECS,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_VOLT_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FLAREON_4] = {
        .species = SPECIES_FLAREON,
        .moves = {MOVE_FLARE_BLITZ, MOVE_FACADE, MOVE_SUPERPOWER, MOVE_QUICK_ATTACK},
        .heldItem = ITEM_TOXIC_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MEGANIUM_4] = {
        .species = SPECIES_MEGANIUM,
        .moves = {MOVE_SWORDS_DANCE, MOVE_LEAF_BLADE, MOVE_EARTHQUAKE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_FLOWER_VEIL,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ESPEON_4] = {
        .species = SPECIES_ESPEON,
        .moves = {MOVE_CALM_MIND, MOVE_PSYCHIC, MOVE_DAZZLING_GLEAM, MOVE_MORNING_SUN},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_MAGIC_BOUNCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_UMBREON_4] = {
        .species = SPECIES_UMBREON,
        .moves = {MOVE_FOUL_PLAY, MOVE_WISH, MOVE_PROTECT, MOVE_TOXIC},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BLASTOISE_4] = {
        .species = SPECIES_BLASTOISE,
        .moves = {MOVE_RAPID_SPIN, MOVE_FLIP_TURN, MOVE_SCALD, MOVE_TOXIC},
        .heldItem = ITEM_HEAVY_DUTY_BOOTS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_TORRENT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_FERALIGATR_4] = {
        .species = SPECIES_FERALIGATR,
        .moves = {MOVE_SWORDS_DANCE, MOVE_LIQUIDATION, MOVE_AQUA_JET, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SHEER_FORCE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_AGGRON_4] = {
        .species = SPECIES_AGGRON,
        .moves = {MOVE_HEAD_SMASH, MOVE_HEAVY_SLAM, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_ROCK_HEAD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BLAZIKEN_4] = {
        .species = SPECIES_BLAZIKEN,
        .moves = {MOVE_SWORDS_DANCE, MOVE_CLOSE_COMBAT, MOVE_FLARE_BLITZ, MOVE_KNOCK_OFF},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SPEED_BOOST,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_WALREIN_4] = {
        .species = SPECIES_WALREIN,
        .moves = {MOVE_BLIZZARD, MOVE_SUPER_FANG, MOVE_AQUA_RING, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_ICE_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SCEPTILE_4] = {
        .species = SPECIES_SCEPTILE,
        .moves = {MOVE_SWORDS_DANCE, MOVE_LEAF_BLADE, MOVE_ACROBATICS, MOVE_DRAIN_PUNCH},
        .heldItem = ITEM_GRASSY_SEED,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_UNBURDEN,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CHARIZARD_4] = {
        .species = SPECIES_CHARIZARD,
        .moves = {MOVE_FIRE_BLAST, MOVE_SOLAR_BEAM, MOVE_SCORCHING_SANDS, MOVE_ROOST},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_SOLAR_POWER,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TYPHLOSION_4] = {
        .species = SPECIES_TYPHLOSION,
        .moves = {MOVE_ERUPTION, MOVE_FIRE_BLAST, MOVE_FOCUS_BLAST, MOVE_SCORCHING_SANDS},
        .heldItem = ITEM_CHOICE_SPECS,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FLASH_FIRE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LAPRAS_4] = {
        .species = SPECIES_LAPRAS,
        .moves = {MOVE_RAIN_DANCE, MOVE_REST, MOVE_FREEZE_DRY, MOVE_HYDRO_PUMP},
        .heldItem = ITEM_DAMP_ROCK,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_HYDRATION,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_CROBAT_4] = {
        .species = SPECIES_CROBAT,
        .moves = {MOVE_NASTY_PLOT, MOVE_AIR_SLASH, MOVE_HEAT_WAVE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_INFILTRATOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SWAMPERT_4] = {
        .species = SPECIES_SWAMPERT,
        .moves = {MOVE_LIQUIDATION, MOVE_HIGH_HORSEPOWER, MOVE_SUPERPOWER, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_TORRENT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GYARADOS_4] = {
        .species = SPECIES_GYARADOS,
        .moves = {MOVE_WATERFALL, MOVE_TAUNT, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SNORLAX_4] = {
        .species = SPECIES_SNORLAX,
        .moves = {MOVE_YAWN, MOVE_HELPING_HAND, MOVE_BODY_SLAM, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_THICK_FAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_KINGDRA_4] = {
        .species = SPECIES_KINGDRA,
        .moves = {MOVE_DRAGON_DANCE, MOVE_WATERFALL, MOVE_BREAKING_SWIPE, MOVE_PROTECT},
        .heldItem = ITEM_LUM_BERRY,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_DAMP,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_BLISSEY_4] = {
        .species = SPECIES_BLISSEY,
        .moves = {MOVE_LIGHT_SCREEN, MOVE_CHARM, MOVE_THUNDER_WAVE, MOVE_SOFT_BOILED},
        .heldItem = ITEM_LIGHT_CLAY,
        .ev = TRAINER_PARTY_EVS(2, 0, 32, 0, 0, 32),
        .nature = NATURE_BOLD,
        .ability = ABILITY_NATURAL_CURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MILOTIC_4] = {
        .species = SPECIES_MILOTIC,
        .moves = {MOVE_COIL, MOVE_HYPNOSIS, MOVE_SCALD, MOVE_RECOVER},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_MARVEL_SCALE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ARCANINE_4] = {
        .species = SPECIES_ARCANINE,
        .moves = {MOVE_HEAT_WAVE, MOVE_SCORCHING_SANDS, MOVE_DRAGON_PULSE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FLASH_FIRE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SALAMENCE_4] = {
        .species = SPECIES_SALAMENCE,
        .moves = {MOVE_DRACO_METEOR, MOVE_HEAT_WAVE, MOVE_HYDRO_PUMP, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_NAIVE,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_METAGROSS_4] = {
        .species = SPECIES_METAGROSS,
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_METEOR_MASH, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SLAKING_4] = {
        .species = SPECIES_SLAKING,
        .moves = {MOVE_GIGA_IMPACT, MOVE_HIGH_HORSEPOWER, MOVE_ICE_PUNCH, MOVE_FIRE_PUNCH},
        .heldItem = ITEM_CHOICE_BAND,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_TRUANT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ARTICUNO_1] = {
        .species = SPECIES_ARTICUNO,
        .moves = {MOVE_TAILWIND, MOVE_FREEZE_DRY, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ZAPDOS_1] = {
        .species = SPECIES_ZAPDOS,
        .moves = {MOVE_VOLT_SWITCH, MOVE_HEAT_WAVE, MOVE_TAILWIND, MOVE_PROTECT},
        .heldItem = ITEM_HEAVY_DUTY_BOOTS,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_STATIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MOLTRES_1] = {
        .species = SPECIES_MOLTRES,
        .moves = {MOVE_HEAT_WAVE, MOVE_HURRICANE, MOVE_TAILWIND, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FLAME_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RAIKOU_1] = {
        .species = SPECIES_RAIKOU,
        .moves = {MOVE_THUNDERBOLT, MOVE_SNARL, MOVE_THUNDER_WAVE, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(17, 0, 0, 32, 17, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ENTEI_1] = {
        .species = SPECIES_ENTEI,
        .moves = {MOVE_SACRED_FIRE, MOVE_EXTREME_SPEED, MOVE_STOMPING_TANTRUM, MOVE_SNARL},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 0, 0, 0, 2),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SUICUNE_1] = {
        .species = SPECIES_SUICUNE,
        .moves = {MOVE_SCALD, MOVE_ICE_BEAM, MOVE_TAILWIND, MOVE_ROAR},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 9, 0, 17, 8),
        .nature = NATURE_CALM,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REGIROCK_1] = {
        .species = SPECIES_REGIROCK,
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_ROCK_SLIDE, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REGICE_1] = {
        .species = SPECIES_REGICE,
        .moves = {MOVE_ICE_BEAM, MOVE_THUNDERBOLT, MOVE_FOCUS_BLAST, MOVE_PROTECT},
        .heldItem = ITEM_WEAKNESS_POLICY,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 2, 0),
        .nature = NATURE_BOLD,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REGISTEEL_1] = {
        .species = SPECIES_REGISTEEL,
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_HEAVY_SLAM, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LATIAS_1] = {
        .species = SPECIES_LATIAS,
        .moves = {MOVE_TAILWIND, MOVE_DRACO_METEOR, MOVE_MIST_BALL, MOVE_PROTECT},
        .heldItem = ITEM_SOUL_DEW,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LATIOS_1] = {
        .species = SPECIES_LATIOS,
        .moves = {MOVE_LUSTER_PURGE, MOVE_DRACO_METEOR, MOVE_AURA_SPHERE, MOVE_PROTECT},
        .heldItem = ITEM_SOUL_DEW,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ARTICUNO_2] = {
        .species = SPECIES_ARTICUNO,
        .moves = {MOVE_ICY_WIND, MOVE_FREEZE_DRY, MOVE_HAZE, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ZAPDOS_2] = {
        .species = SPECIES_ZAPDOS,
        .moves = {MOVE_THUNDERBOLT, MOVE_TAILWIND, MOVE_EERIE_IMPULSE, MOVE_ROOST},
        .heldItem = ITEM_ROCKY_HELMET,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MOLTRES_2] = {
        .species = SPECIES_MOLTRES,
        .moves = {MOVE_FLAMETHROWER, MOVE_WILL_O_WISP, MOVE_HELPING_HAND, MOVE_ROOST},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 2, 0),
        .nature = NATURE_BOLD,
        .ability = ABILITY_FLAME_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RAIKOU_2] = {
        .species = SPECIES_RAIKOU,
        .moves = {MOVE_THUNDERBOLT, MOVE_ELECTROWEB, MOVE_SNARL, MOVE_VOLT_SWITCH},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ENTEI_2] = {
        .species = SPECIES_ENTEI,
        .moves = {MOVE_SACRED_FIRE, MOVE_EXTREME_SPEED, MOVE_STONE_EDGE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SUICUNE_2] = {
        .species = SPECIES_SUICUNE,
        .moves = {MOVE_SCALD, MOVE_ICY_WIND, MOVE_SNARL, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_BOLD,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REGIROCK_2] = {
        .species = SPECIES_REGIROCK,
        .moves = {MOVE_SANDSTORM, MOVE_ROCK_SLIDE, MOVE_THUNDER_WAVE, MOVE_PROTECT},
        .heldItem = ITEM_SMOOTH_ROCK,
        .ev = TRAINER_PARTY_EVS(32, 17, 0, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REGICE_2] = {
        .species = SPECIES_REGICE,
        .moves = {MOVE_ICY_WIND, MOVE_THUNDER_WAVE, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REGISTEEL_2] = {
        .species = SPECIES_REGISTEEL,
        .moves = {MOVE_IRON_DEFENSE, MOVE_AMNESIA, MOVE_BODY_PRESS, MOVE_REST},
        .heldItem = ITEM_CHESTO_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LATIAS_2] = {
        .species = SPECIES_LATIAS,
        .moves = {MOVE_MIST_BALL, MOVE_MYSTICAL_FIRE, MOVE_ICY_WIND, MOVE_PROTECT},
        .heldItem = ITEM_COVERT_CLOAK,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LATIOS_2] = {
        .species = SPECIES_LATIOS,
        .moves = {MOVE_TAILWIND, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_LUSTER_PURGE},
        .heldItem = ITEM_COVERT_CLOAK,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 2, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ARTICUNO_3] = {
        .species = SPECIES_ARTICUNO,
        .moves = {MOVE_AURORA_VEIL, MOVE_BLIZZARD, MOVE_FREEZE_DRY, MOVE_PROTECT},
        .heldItem = ITEM_LIGHT_CLAY,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_SNOW_CLOAK,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ZAPDOS_3] = {
        .species = SPECIES_ZAPDOS,
        .moves = {MOVE_THUNDER, MOVE_HURRICANE, MOVE_WEATHER_BALL, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_STATIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MOLTRES_3] = {
        .species = SPECIES_MOLTRES,
        .moves = {MOVE_FLAMETHROWER, MOVE_WILL_O_WISP, MOVE_ROOST, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 2, 0),
        .nature = NATURE_BOLD,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RAIKOU_3] = {
        .species = SPECIES_RAIKOU,
        .moves = {MOVE_THUNDERBOLT, MOVE_SCALD, MOVE_AURA_SPHERE, MOVE_PROTECT},
        .heldItem = ITEM_SHUCA_BERRY,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ENTEI_3] = {
        .species = SPECIES_ENTEI,
        .moves = {MOVE_SACRED_FIRE, MOVE_SNARL, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 2, 0, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SUICUNE_3] = {
        .species = SPECIES_SUICUNE,
        .moves = {MOVE_CALM_MIND, MOVE_SCALD, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REGIROCK_3] = {
        .species = SPECIES_REGIROCK,
        .moves = {MOVE_ROCK_SLIDE, MOVE_STOMPING_TANTRUM, MOVE_ICE_PUNCH, MOVE_PROTECT},
        .heldItem = ITEM_WEAKNESS_POLICY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REGICE_3] = {
        .species = SPECIES_REGICE,
        .moves = {MOVE_BLIZZARD, MOVE_AURORA_VEIL, MOVE_THUNDERBOLT, MOVE_PROTECT},
        .heldItem = ITEM_LIGHT_CLAY,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_ICE_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REGISTEEL_3] = {
        .species = SPECIES_REGISTEEL,
        .moves = {MOVE_THUNDER_WAVE, MOVE_HEAVY_SLAM, MOVE_SEISMIC_TOSS, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LATIAS_3] = {
        .species = SPECIES_LATIAS,
        .moves = {MOVE_HELPING_HAND, MOVE_HEAL_PULSE, MOVE_ICY_WIND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LATIOS_3] = {
        .species = SPECIES_LATIOS,
        .moves = {MOVE_HEAL_PULSE, MOVE_HELPING_HAND, MOVE_MYSTICAL_FIRE, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ARTICUNO_4] = {
        .species = SPECIES_ARTICUNO,
        .moves = {MOVE_HURRICANE, MOVE_FREEZE_DRY, MOVE_TAILWIND, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ZAPDOS_4] = {
        .species = SPECIES_ZAPDOS,
        .moves = {MOVE_THUNDERBOLT, MOVE_HURRICANE, MOVE_HEAT_WAVE, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_STATIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MOLTRES_4] = {
        .species = SPECIES_MOLTRES,
        .moves = {MOVE_FIRE_BLAST, MOVE_SOLAR_BEAM, MOVE_SCORCHING_SANDS, MOVE_PROTECT},
        .heldItem = ITEM_POWER_HERB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_FLAME_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RAIKOU_4] = {
        .species = SPECIES_RAIKOU,
        .moves = {MOVE_SUBSTITUTE, MOVE_CALM_MIND, MOVE_THUNDERBOLT, MOVE_SCALD},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ENTEI_4] = {
        .species = SPECIES_ENTEI,
        .moves = {MOVE_SACRED_FIRE, MOVE_FLARE_BLITZ, MOVE_STONE_EDGE, MOVE_EXTREME_SPEED},
        .heldItem = ITEM_CHOICE_BAND,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SUICUNE_4] = {
        .species = SPECIES_SUICUNE,
        .moves = {MOVE_CALM_MIND, MOVE_SCALD, MOVE_REST, MOVE_SLEEP_TALK},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REGIROCK_4] = {
        .species = SPECIES_REGIROCK,
        .moves = {MOVE_ROCK_SLIDE, MOVE_STOMPING_TANTRUM, MOVE_HAMMER_ARM, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REGICE_4] = {
        .species = SPECIES_REGICE,
        .moves = {MOVE_ICE_BEAM, MOVE_THUNDERBOLT, MOVE_FOCUS_BLAST, MOVE_FLASH_CANNON},
        .heldItem = ITEM_CHOICE_SPECS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 32, 0),
        .nature = NATURE_QUIET,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REGISTEEL_4] = {
        .species = SPECIES_REGISTEEL,
        .moves = {MOVE_ROCK_POLISH, MOVE_HEAVY_SLAM, MOVE_STOMPING_TANTRUM, MOVE_PROTECT},
        .heldItem = ITEM_WEAKNESS_POLICY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LATIAS_4] = {
        .species = SPECIES_LATIAS,
        .moves = {MOVE_CALM_MIND, MOVE_STORED_POWER, MOVE_AURA_SPHERE, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LATIOS_4] = {
        .species = SPECIES_LATIOS,
        .moves = {MOVE_CALM_MIND, MOVE_LUSTER_PURGE, MOVE_AURA_SPHERE, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 2, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GENGAR_5] = {
        .species = SPECIES_GENGAR,
        .moves = {MOVE_SHADOW_BALL, MOVE_SLUDGE_WAVE, MOVE_FOCUS_BLAST, MOVE_TAUNT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GENGAR_6] = {
        .species = SPECIES_GENGAR,
        .moves = {MOVE_NASTY_PLOT, MOVE_SHADOW_BALL, MOVE_SLUDGE_BOMB, MOVE_FOCUS_BLAST},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_CURSED_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GENGAR_7] = {
        .species = SPECIES_GENGAR,
        .moves = {MOVE_SHADOW_BALL, MOVE_ICY_WIND, MOVE_SLUDGE_WAVE, MOVE_DESTINY_BOND},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_CURSED_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GENGAR_8] = {
        .species = SPECIES_GENGAR,
        .moves = {MOVE_SHADOW_BALL, MOVE_SLUDGE_BOMB, MOVE_FOCUS_BLAST, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_URSARING_5] = {
        .species = SPECIES_URSARING,
        .moves = {MOVE_SWORDS_DANCE, MOVE_FACADE, MOVE_CLOSE_COMBAT, MOVE_CRUNCH},
        .heldItem = ITEM_TOXIC_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_QUICK_FEET,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_URSARING_6] = {
        .species = SPECIES_URSARING,
        .moves = {MOVE_FACADE, MOVE_CLOSE_COMBAT, MOVE_CRUNCH, MOVE_SWORDS_DANCE},
        .heldItem = ITEM_EVIOLITE,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_URSARING_7] = {
        .species = SPECIES_URSARING,
        .moves = {MOVE_FACADE, MOVE_CLOSE_COMBAT, MOVE_CRUNCH, MOVE_PROTECT},
        .heldItem = ITEM_FLAME_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_URSARING_8] = {
        .species = SPECIES_URSARING,
        .moves = {MOVE_FACADE, MOVE_CLOSE_COMBAT, MOVE_CRUNCH, MOVE_PROTECT},
        .heldItem = ITEM_TOXIC_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_QUICK_FEET,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MACHAMP_5] = {
        .species = SPECIES_MACHAMP,
        .moves = {MOVE_DYNAMIC_PUNCH, MOVE_KNOCK_OFF, MOVE_STONE_EDGE, MOVE_BULLET_PUNCH},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_NO_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MACHAMP_6] = {
        .species = SPECIES_MACHAMP,
        .moves = {MOVE_CLOSE_COMBAT, MOVE_FACADE, MOVE_KNOCK_OFF, MOVE_BULLET_PUNCH},
        .heldItem = ITEM_FLAME_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_GUTS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MACHAMP_7] = {
        .species = SPECIES_MACHAMP,
        .moves = {MOVE_DYNAMIC_PUNCH, MOVE_KNOCK_OFF, MOVE_STONE_EDGE, MOVE_BULLET_PUNCH},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_NO_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MACHAMP_8] = {
        .species = SPECIES_MACHAMP,
        .moves = {MOVE_DYNAMIC_PUNCH, MOVE_ROCK_SLIDE, MOVE_WIDE_GUARD, MOVE_QUICK_GUARD},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_NO_GUARD,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GARDEVOIR_5] = {
        .species = SPECIES_GARDEVOIR,
        .moves = {MOVE_MOONBLAST, MOVE_PSYCHIC, MOVE_MYSTICAL_FIRE, MOVE_TRICK},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_TRACE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GARDEVOIR_6] = {
        .species = SPECIES_GARDEVOIR,
        .moves = {MOVE_CALM_MIND, MOVE_MOONBLAST, MOVE_WISH, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 2, 0),
        .nature = NATURE_BOLD,
        .ability = ABILITY_TRACE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GARDEVOIR_7] = {
        .species = SPECIES_GARDEVOIR,
        .moves = {MOVE_MOONBLAST, MOVE_PSYCHIC, MOVE_ICY_WIND, MOVE_TRICK},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_TRACE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_GARDEVOIR_8] = {
        .species = SPECIES_GARDEVOIR,
        .moves = {MOVE_DAZZLING_GLEAM, MOVE_HEAL_PULSE, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_TELEPATHY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_STARMIE_5] = {
        .species = SPECIES_STARMIE,
        .moves = {MOVE_COSMIC_POWER, MOVE_RECOVER, MOVE_SCALD, MOVE_PSYSHOCK},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_BOLD,
        .ability = ABILITY_NATURAL_CURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_STARMIE_6] = {
        .species = SPECIES_STARMIE,
        .moves = {MOVE_HYDRO_PUMP, MOVE_PSYCHIC, MOVE_THUNDERBOLT, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_ANALYTIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_STARMIE_7] = {
        .species = SPECIES_STARMIE,
        .moves = {MOVE_SCALD, MOVE_ICY_WIND, MOVE_RECOVER, MOVE_PROTECT},
        .heldItem = ITEM_COLBUR_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_BOLD,
        .ability = ABILITY_NATURAL_CURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_STARMIE_8] = {
        .species = SPECIES_STARMIE,
        .moves = {MOVE_GRAVITY, MOVE_HYDRO_PUMP, MOVE_THUNDER, MOVE_BLIZZARD},
        .heldItem = ITEM_FOCUS_SASH,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_ANALYTIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LAPRAS_5] = {
        .species = SPECIES_LAPRAS,
        .moves = {MOVE_WHIRLPOOL, MOVE_PERISH_SONG, MOVE_FREEZE_DRY, MOVE_REST},
        .heldItem = ITEM_HEAVY_DUTY_BOOTS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_WATER_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LAPRAS_6] = {
        .species = SPECIES_LAPRAS,
        .moves = {MOVE_HYDRO_PUMP, MOVE_FREEZE_DRY, MOVE_ICE_BEAM, MOVE_PSYCHIC_NOISE},
        .heldItem = ITEM_CHOICE_SPECS,
        .ev = TRAINER_PARTY_EVS(18, 0, 0, 16, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_WATER_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LAPRAS_7] = {
        .species = SPECIES_LAPRAS,
        .moves = {MOVE_FREEZE_DRY, MOVE_HYDRO_PUMP, MOVE_ICY_WIND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 0, 32, 2),
        .nature = NATURE_MODEST,
        .ability = ABILITY_WATER_ABSORB,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LAPRAS_8] = {
        .species = SPECIES_LAPRAS,
        .moves = {MOVE_PERISH_SONG, MOVE_FREEZE_DRY, MOVE_HYDRO_PUMP, MOVE_PROTECT},
        .heldItem = ITEM_ROCKY_HELMET,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CALM,
        .ability = ABILITY_SHELL_ARMOR,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SNORLAX_5] = {
        .species = SPECIES_SNORLAX,
        .moves = {MOVE_CURSE, MOVE_BODY_SLAM, MOVE_EARTHQUAKE, MOVE_REST},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_THICK_FAT,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SNORLAX_6] = {
        .species = SPECIES_SNORLAX,
        .moves = {MOVE_CURSE, MOVE_FACADE, MOVE_EARTHQUAKE, MOVE_RECYCLE},
        .heldItem = ITEM_FIGY_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_GLUTTONY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SNORLAX_7] = {
        .species = SPECIES_SNORLAX,
        .moves = {MOVE_BELLY_DRUM, MOVE_RETURN, MOVE_HIGH_HORSEPOWER, MOVE_PROTECT},
        .heldItem = ITEM_IAPAPA_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_GLUTTONY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SNORLAX_8] = {
        .species = SPECIES_SNORLAX,
        .moves = {MOVE_CURSE, MOVE_BODY_SLAM, MOVE_HIGH_HORSEPOWER, MOVE_RECYCLE},
        .heldItem = ITEM_FIGY_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_GLUTTONY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SALAMENCE_5] = {
        .species = SPECIES_SALAMENCE,
        .moves = {MOVE_DRAGON_DANCE, MOVE_DUAL_WINGBEAT, MOVE_EARTHQUAKE, MOVE_ROOST},
        .heldItem = ITEM_HEAVY_DUTY_BOOTS,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_MOXIE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SALAMENCE_6] = {
        .species = SPECIES_SALAMENCE,
        .moves = {MOVE_DRACO_METEOR, MOVE_HURRICANE, MOVE_FIRE_BLAST, MOVE_ROOST},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_NAIVE,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SALAMENCE_7] = {
        .species = SPECIES_SALAMENCE,
        .moves = {MOVE_TAILWIND, MOVE_BREAKING_SWIPE, MOVE_HEAT_WAVE, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 2, 0, 32, 0, 0),
        .nature = NATURE_NAIVE,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SALAMENCE_8] = {
        .species = SPECIES_SALAMENCE,
        .moves = {MOVE_DRAGON_DANCE, MOVE_DRAGON_CLAW, MOVE_DUAL_WINGBEAT, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_INTIMIDATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_METAGROSS_5] = {
        .species = SPECIES_METAGROSS,
        .moves = {MOVE_HEAVY_SLAM, MOVE_EARTHQUAKE, MOVE_KNOCK_OFF, MOVE_BULLET_PUNCH},
        .heldItem = ITEM_CHOICE_BAND,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_METAGROSS_6] = {
        .species = SPECIES_METAGROSS,
        .moves = {MOVE_ROCK_POLISH, MOVE_METEOR_MASH, MOVE_PSYCHIC_FANGS, MOVE_EARTHQUAKE},
        .heldItem = ITEM_WEAKNESS_POLICY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_METAGROSS_7] = {
        .species = SPECIES_METAGROSS,
        .moves = {MOVE_METEOR_MASH, MOVE_PSYCHIC_FANGS, MOVE_HAMMER_ARM, MOVE_BULLET_PUNCH},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 0, 0, 0, 2),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_METAGROSS_8] = {
        .species = SPECIES_METAGROSS,
        .moves = {MOVE_ROCK_POLISH, MOVE_METEOR_MASH, MOVE_PSYCHIC_FANGS, MOVE_PROTECT},
        .heldItem = ITEM_WEAKNESS_POLICY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REGIROCK_5] = {
        .species = SPECIES_REGIROCK,
        .moves = {MOVE_IRON_DEFENSE, MOVE_BODY_PRESS, MOVE_STONE_EDGE, MOVE_STEALTH_ROCK},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_STURDY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REGIROCK_6] = {
        .species = SPECIES_REGIROCK,
        .moves = {MOVE_STONE_EDGE, MOVE_EARTHQUAKE, MOVE_HAMMER_ARM, MOVE_ICE_PUNCH},
        .heldItem = ITEM_CHOICE_BAND,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REGICE_5] = {
        .species = SPECIES_REGICE,
        .moves = {MOVE_ICE_BEAM, MOVE_THUNDERBOLT, MOVE_REST, MOVE_SLEEP_TALK},
        .heldItem = ITEM_HEAVY_DUTY_BOOTS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REGICE_6] = {
        .species = SPECIES_REGICE,
        .moves = {MOVE_ROCK_POLISH, MOVE_ICE_BEAM, MOVE_THUNDERBOLT, MOVE_FOCUS_BLAST},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REGISTEEL_5] = {
        .species = SPECIES_REGISTEEL,
        .moves = {MOVE_IRON_DEFENSE, MOVE_AMNESIA, MOVE_BODY_PRESS, MOVE_REST},
        .heldItem = ITEM_CHESTO_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_REGISTEEL_6] = {
        .species = SPECIES_REGISTEEL,
        .moves = {MOVE_STEALTH_ROCK, MOVE_HEAVY_SLAM, MOVE_THUNDER_WAVE, MOVE_SEISMIC_TOSS},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_CLEAR_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LATIAS_5] = {
        .species = SPECIES_LATIAS,
        .moves = {MOVE_DRACO_METEOR, MOVE_MIST_BALL, MOVE_MYSTICAL_FIRE, MOVE_RECOVER},
        .heldItem = ITEM_SOUL_DEW,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LATIAS_6] = {
        .species = SPECIES_LATIAS,
        .moves = {MOVE_CALM_MIND, MOVE_STORED_POWER, MOVE_AURA_SPHERE, MOVE_RECOVER},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 32, 0, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LATIAS_7] = {
        .species = SPECIES_LATIAS,
        .moves = {MOVE_TAILWIND, MOVE_DRACO_METEOR, MOVE_MIST_BALL, MOVE_PROTECT},
        .heldItem = ITEM_SOUL_DEW,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LATIAS_8] = {
        .species = SPECIES_LATIAS,
        .moves = {MOVE_MIST_BALL, MOVE_MYSTICAL_FIRE, MOVE_ICY_WIND, MOVE_PROTECT},
        .heldItem = ITEM_COVERT_CLOAK,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LATIOS_5] = {
        .species = SPECIES_LATIOS,
        .moves = {MOVE_LUSTER_PURGE, MOVE_DRACO_METEOR, MOVE_AURA_SPHERE, MOVE_RECOVER},
        .heldItem = ITEM_SOUL_DEW,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LATIOS_6] = {
        .species = SPECIES_LATIOS,
        .moves = {MOVE_CALM_MIND, MOVE_LUSTER_PURGE, MOVE_AURA_SPHERE, MOVE_RECOVER},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 2, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LATIOS_7] = {
        .species = SPECIES_LATIOS,
        .moves = {MOVE_LUSTER_PURGE, MOVE_DRACO_METEOR, MOVE_AURA_SPHERE, MOVE_PROTECT},
        .heldItem = ITEM_SOUL_DEW,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_LATIOS_8] = {
        .species = SPECIES_LATIOS,
        .moves = {MOVE_TAILWIND, MOVE_ICY_WIND, MOVE_HELPING_HAND, MOVE_LUSTER_PURGE},
        .heldItem = ITEM_COVERT_CLOAK,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 2, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_LEVITATE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DRAGONITE_1] = {
        .species = SPECIES_DRAGONITE,
        .moves = {MOVE_EXTREME_SPEED, MOVE_DRAGON_CLAW, MOVE_STOMPING_TANTRUM, MOVE_PROTECT},
        .heldItem = ITEM_CLEAR_AMULET,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DRAGONITE_2] = {
        .species = SPECIES_DRAGONITE,
        .moves = {MOVE_DRAGON_DANCE, MOVE_DRAGON_CLAW, MOVE_EXTREME_SPEED, MOVE_PROTECT},
        .heldItem = ITEM_LUM_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_MULTISCALE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DRAGONITE_3] = {
        .species = SPECIES_DRAGONITE,
        .moves = {MOVE_TAILWIND, MOVE_BREAKING_SWIPE, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_MULTISCALE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DRAGONITE_4] = {
        .species = SPECIES_DRAGONITE,
        .moves = {MOVE_HURRICANE, MOVE_THUNDER, MOVE_DRACO_METEOR, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_MULTISCALE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DRAGONITE_5] = {
        .species = SPECIES_DRAGONITE,
        .moves = {MOVE_DRAGON_DANCE, MOVE_EARTHQUAKE, MOVE_DUAL_WINGBEAT, MOVE_ROOST},
        .heldItem = ITEM_HEAVY_DUTY_BOOTS,
        .ev = TRAINER_PARTY_EVS(32, 17, 0, 17, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_MULTISCALE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DRAGONITE_6] = {
        .species = SPECIES_DRAGONITE,
        .moves = {MOVE_DRAGON_TAIL, MOVE_EXTREME_SPEED, MOVE_EARTHQUAKE, MOVE_ROOST},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_MULTISCALE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DRAGONITE_7] = {
        .species = SPECIES_DRAGONITE,
        .moves = {MOVE_EXTREME_SPEED, MOVE_DRAGON_CLAW, MOVE_STOMPING_TANTRUM, MOVE_PROTECT},
        .heldItem = ITEM_CLEAR_AMULET,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DRAGONITE_8] = {
        .species = SPECIES_DRAGONITE,
        .moves = {MOVE_DRAGON_DANCE, MOVE_DRAGON_CLAW, MOVE_EXTREME_SPEED, MOVE_PROTECT},
        .heldItem = ITEM_LUM_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_MULTISCALE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DRAGONITE_9] = {
        .species = SPECIES_DRAGONITE,
        .moves = {MOVE_TAILWIND, MOVE_BREAKING_SWIPE, MOVE_HELPING_HAND, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 17, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_MULTISCALE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_DRAGONITE_10] = {
        .species = SPECIES_DRAGONITE,
        .moves = {MOVE_HURRICANE, MOVE_THUNDER, MOVE_DRACO_METEOR, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_MODEST,
        .ability = ABILITY_MULTISCALE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TYRANITAR_1] = {
        .species = SPECIES_TYRANITAR,
        .moves = {MOVE_ROCK_SLIDE, MOVE_CRUNCH, MOVE_STOMPING_TANTRUM, MOVE_ICE_PUNCH},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SAND_STREAM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TYRANITAR_2] = {
        .species = SPECIES_TYRANITAR,
        .moves = {MOVE_DRAGON_DANCE, MOVE_ROCK_SLIDE, MOVE_CRUNCH, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SAND_STREAM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TYRANITAR_3] = {
        .species = SPECIES_TYRANITAR,
        .moves = {MOVE_ROCK_SLIDE, MOVE_CRUNCH, MOVE_ICE_PUNCH, MOVE_SUPERPOWER},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_UNNERVE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TYRANITAR_4] = {
        .species = SPECIES_TYRANITAR,
        .moves = {MOVE_ROCK_SLIDE, MOVE_CRUNCH, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 17, 0, 0, 17, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_UNNERVE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TYRANITAR_5] = {
        .species = SPECIES_TYRANITAR,
        .moves = {MOVE_STONE_EDGE, MOVE_KNOCK_OFF, MOVE_STEALTH_ROCK, MOVE_THUNDER_WAVE},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 17, 0, 0, 0, 17),
        .nature = NATURE_CAREFUL,
        .ability = ABILITY_SAND_STREAM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TYRANITAR_6] = {
        .species = SPECIES_TYRANITAR,
        .moves = {MOVE_DRAGON_DANCE, MOVE_STONE_EDGE, MOVE_CRUNCH, MOVE_EARTHQUAKE},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_UNNERVE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TYRANITAR_7] = {
        .species = SPECIES_TYRANITAR,
        .moves = {MOVE_ROCK_SLIDE, MOVE_CRUNCH, MOVE_STOMPING_TANTRUM, MOVE_ICE_PUNCH},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 2, 0, 0, 0),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_SAND_STREAM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TYRANITAR_8] = {
        .species = SPECIES_TYRANITAR,
        .moves = {MOVE_DRAGON_DANCE, MOVE_ROCK_SLIDE, MOVE_CRUNCH, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_SAND_STREAM,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TYRANITAR_9] = {
        .species = SPECIES_TYRANITAR,
        .moves = {MOVE_ROCK_SLIDE, MOVE_CRUNCH, MOVE_ICE_PUNCH, MOVE_SUPERPOWER},
        .heldItem = ITEM_CHOICE_SCARF,
        .ev = TRAINER_PARTY_EVS(2, 32, 0, 32, 0, 0),
        .nature = NATURE_JOLLY,
        .ability = ABILITY_UNNERVE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_TYRANITAR_10] = {
        .species = SPECIES_TYRANITAR,
        .moves = {MOVE_ROCK_SLIDE, MOVE_CRUNCH, MOVE_ICE_BEAM, MOVE_PROTECT},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(32, 17, 0, 0, 17, 0),
        .nature = NATURE_BRAVE,
        .ability = ABILITY_UNNERVE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ARTICUNO_5] = {
        .species = SPECIES_ARTICUNO,
        .moves = {MOVE_DEFOG, MOVE_ROOST, MOVE_FREEZE_DRY, MOVE_HAZE},
        .heldItem = ITEM_HEAVY_DUTY_BOOTS,
        .ev = TRAINER_PARTY_EVS(32, 0, 2, 0, 0, 32),
        .nature = NATURE_CALM,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ARTICUNO_6] = {
        .species = SPECIES_ARTICUNO,
        .moves = {MOVE_SUBSTITUTE, MOVE_ROOST, MOVE_TOXIC, MOVE_FREEZE_DRY},
        .heldItem = ITEM_HEAVY_DUTY_BOOTS,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 0, 2),
        .nature = NATURE_TIMID,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ZAPDOS_5] = {
        .species = SPECIES_ZAPDOS,
        .moves = {MOVE_VOLT_SWITCH, MOVE_HURRICANE, MOVE_HEAT_WAVE, MOVE_ROOST},
        .heldItem = ITEM_HEAVY_DUTY_BOOTS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_BOLD,
        .ability = ABILITY_STATIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ZAPDOS_6] = {
        .species = SPECIES_ZAPDOS,
        .moves = {MOVE_THUNDERBOLT, MOVE_HURRICANE, MOVE_HEAT_WAVE, MOVE_ROOST},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_STATIC,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MOLTRES_5] = {
        .species = SPECIES_MOLTRES,
        .moves = {MOVE_FLAMETHROWER, MOVE_WILL_O_WISP, MOVE_ROOST, MOVE_U_TURN},
        .heldItem = ITEM_HEAVY_DUTY_BOOTS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 2, 0),
        .nature = NATURE_BOLD,
        .ability = ABILITY_FLAME_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_MOLTRES_6] = {
        .species = SPECIES_MOLTRES,
        .moves = {MOVE_HURRICANE, MOVE_FIRE_BLAST, MOVE_ROOST, MOVE_U_TURN},
        .heldItem = ITEM_LIFE_ORB,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_FLAME_BODY,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RAIKOU_5] = {
        .species = SPECIES_RAIKOU,
        .moves = {MOVE_THUNDERBOLT, MOVE_VOLT_SWITCH, MOVE_SCALD, MOVE_AURA_SPHERE},
        .heldItem = ITEM_CHOICE_SPECS,
        .ev = TRAINER_PARTY_EVS(2, 0, 0, 32, 32, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_RAIKOU_6] = {
        .species = SPECIES_RAIKOU,
        .moves = {MOVE_THUNDERBOLT, MOVE_SNARL, MOVE_THUNDER_WAVE, MOVE_PROTECT},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(17, 0, 0, 32, 17, 0),
        .nature = NATURE_TIMID,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ENTEI_5] = {
        .species = SPECIES_ENTEI,
        .moves = {MOVE_SACRED_FIRE, MOVE_WILL_O_WISP, MOVE_ROAR, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 32, 0, 0, 2),
        .nature = NATURE_IMPISH,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_ENTEI_6] = {
        .species = SPECIES_ENTEI,
        .moves = {MOVE_SACRED_FIRE, MOVE_EXTREME_SPEED, MOVE_STOMPING_TANTRUM, MOVE_SNARL},
        .heldItem = ITEM_ASSAULT_VEST,
        .ev = TRAINER_PARTY_EVS(32, 32, 0, 0, 0, 2),
        .nature = NATURE_ADAMANT,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SUICUNE_5] = {
        .species = SPECIES_SUICUNE,
        .moves = {MOVE_CALM_MIND, MOVE_SCALD, MOVE_SUBSTITUTE, MOVE_PROTECT},
        .heldItem = ITEM_LEFTOVERS,
        .ev = TRAINER_PARTY_EVS(32, 0, 0, 32, 0, 2),
        .nature = NATURE_TIMID,
        .ability = ABILITY_PRESSURE,
        .ball = BALL_POKE
    },
    [FRONTIER_MON_SUICUNE_6] = {
        .species = SPECIES_SUICUNE,
        .moves = {MOVE_SCALD, MOVE_ICE_BEAM, MOVE_TAILWIND, MOVE_ROAR},
        .heldItem = ITEM_SITRUS_BERRY,
        .ev = TRAINER_PARTY_EVS(32, 0, 9, 0, 17, 8),
        .nature = NATURE_CALM,
        .ability = ABILITY_INNER_FOCUS,
        .ball = BALL_POKE
    }
};
