const struct AbilityInfo gAbilitiesInfo[ABILITIES_COUNT] =
{
    [ABILITY_NONE] =
    {
        .name = _("-------"),
        .description = COMPOUND_STRING("No special ability."),
        .aiRating = 0,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
    },

    [ABILITY_STENCH] =
    {
        .name = _("STENCH"),
        .description = COMPOUND_STRING("Helps repel wild POKéMON."),
        .aiRating = 1,
    },

    [ABILITY_DRIZZLE] =
    {
        .name = _("DRIZZLE"),
        .description = COMPOUND_STRING("Summons rain in battle."),
        .aiRating = 9,
    },

    [ABILITY_SPEED_BOOST] =
    {
        .name = _("SPEED BOOST"),
        .description = COMPOUND_STRING("Gradually boosts SPEED."),
        .aiRating = 9,
    },

    [ABILITY_BATTLE_ARMOR] =
    {
        .name = _("BATTLE ARMOR"),
        .description = COMPOUND_STRING("Blocks critical hits."),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_STURDY] =
    {
        .name = _("STURDY"),
        .description = COMPOUND_STRING("Negates 1-hit KO attacks."),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_DAMP] =
    {
        .name = _("DAMP"),
        .description = COMPOUND_STRING("Prevents self-destruction."),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_LIMBER] =
    {
        .name = _("LIMBER"),
        .description = COMPOUND_STRING("Prevents paralysis."),
        .aiRating = 3,
        .breakable = TRUE,
    },

    [ABILITY_SAND_VEIL] =
    {
        .name = _("SAND VEIL"),
        .description = COMPOUND_STRING("Ups evasion in a sandstorm."),
        .aiRating = 3,
        .breakable = TRUE,
    },

    [ABILITY_STATIC] =
    {
        .name = _("STATIC"),
        .description = COMPOUND_STRING("Paralyzes on contact."),
        .aiRating = 4,
    },

    [ABILITY_VOLT_ABSORB] =
    {
        .name = _("VOLT ABSORB"),
        .description = COMPOUND_STRING("Turns electricity into HP."),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_WATER_ABSORB] =
    {
        .name = _("WATER ABSORB"),
        .description = COMPOUND_STRING("Changes water into HP."),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_OBLIVIOUS] =
    {
        .name = _("OBLIVIOUS"),
        .description = COMPOUND_STRING("Prevents attraction."),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_CLOUD_NINE] =
    {
        .name = _("CLOUD NINE"),
        .description = COMPOUND_STRING("Negates weather effects."),
        .aiRating = 5,
    },

    [ABILITY_COMPOUND_EYES] =
    {
#if TESTING
        .name = _("COMPOUND EYES"),
#else
        .name = _("COMPOUNDEYES"),
#endif
        .description = COMPOUND_STRING("Raises accuracy."),
        .aiRating = 7,
    },

    [ABILITY_INSOMNIA] =
    {
        .name = _("INSOMNIA"),
        .description = COMPOUND_STRING("Prevents sleep."),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_COLOR_CHANGE] =
    {
        .name = _("COLOR CHANGE"),
        .description = COMPOUND_STRING("Changes type to foe's move."),
        .aiRating = 2,
    },

    [ABILITY_IMMUNITY] =
    {
        .name = _("IMMUNITY"),
        .description = COMPOUND_STRING("Prevents poisoning."),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_FLASH_FIRE] =
    {
        .name = _("FLASH FIRE"),
        .description = COMPOUND_STRING("Powers up if hit by fire."),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_SHIELD_DUST] =
    {
        .name = _("SHIELD DUST"),
        .description = COMPOUND_STRING("Prevents added effects."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_OWN_TEMPO] =
    {
        .name = _("OWN TEMPO"),
        .description = COMPOUND_STRING("Prevents confusion."),
        .aiRating = 3,
        .breakable = TRUE,
    },

    [ABILITY_SUCTION_CUPS] =
    {
        .name = _("SUCTION CUPS"),
        .description = COMPOUND_STRING("Firmly anchors the body."),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_INTIMIDATE] =
    {
        .name = _("INTIMIDATE"),
        .description = COMPOUND_STRING("Lowers the foe's ATTACK."),
        .aiRating = 7,
    },

    [ABILITY_SHADOW_TAG] =
    {
        .name = _("SHADOW TAG"),
        .description = COMPOUND_STRING("Prevents the foe's escape."),
        .aiRating = 10,
    },

    [ABILITY_ROUGH_SKIN] =
    {
        .name = _("ROUGH SKIN"),
        .description = COMPOUND_STRING("Hurts to touch."),
        .aiRating = 6,
    },

    [ABILITY_WONDER_GUARD] =
    {
        .name = _("WONDER GUARD"),
        .description = COMPOUND_STRING("“Super effective” hits."),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .breakable = TRUE,
    },

    [ABILITY_LEVITATE] =
    {
        .name = _("LEVITATE"),
        .description = COMPOUND_STRING("Not hit by GROUND attacks."),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_EFFECT_SPORE] =
    {
        .name = _("EFFECT SPORE"),
        .description = COMPOUND_STRING("Leaves spores on contact."),
        .aiRating = 4,
    },

    [ABILITY_SYNCHRONIZE] =
    {
        .name = _("SYNCHRONIZE"),
        .description = COMPOUND_STRING("Passes on status problems."),
        .aiRating = 4,
    },

    [ABILITY_CLEAR_BODY] =
    {
        .name = _("CLEAR BODY"),
        .description = COMPOUND_STRING("Prevents ability reduction."),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_NATURAL_CURE] =
    {
        .name = _("NATURAL CURE"),
        .description = COMPOUND_STRING("Heals upon switching out."),
        .aiRating = 7,
    },

    [ABILITY_LIGHTNING_ROD] =
    {
#if TESTING
        .name = _("LIGHTNING ROD"),
#else
        .name = _("LIGHTNINGROD"),
#endif
        .description = COMPOUND_STRING("Draws electrical moves."),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_SERENE_GRACE] =
    {
        .name = _("SERENE GRACE"),
        .description = COMPOUND_STRING("Promotes added effects."),
        .aiRating = 8,
    },

    [ABILITY_SWIFT_SWIM] =
    {
        .name = _("SWIFT SWIM"),
        .description = COMPOUND_STRING("Raises SPEED in rain."),
        .aiRating = 6,
    },

    [ABILITY_CHLOROPHYLL] =
    {
        .name = _("CHLOROPHYLL"),
        .description = COMPOUND_STRING("Raises SPEED in sunshine."),
        .aiRating = 6,
    },

    [ABILITY_ILLUMINATE] =
    {
        .name = _("ILLUMINATE"),
        .description = COMPOUND_STRING("Encounter rate increases."),
        .aiRating = 0,
        .breakable = TRUE,
    },

    [ABILITY_TRACE] =
    {
        .name = _("TRACE"),
        .description = COMPOUND_STRING("Copies special ability."),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeTraced = TRUE, //B_UPDATED_ABILITY_DATA >= GEN_4
    },

    [ABILITY_HUGE_POWER] =
    {
        .name = _("HUGE POWER"),
        .description = COMPOUND_STRING("Raises ATTACK."),
        .aiRating = 10,
    },

    [ABILITY_POISON_POINT] =
    {
        .name = _("POISON POINT"),
        .description = COMPOUND_STRING("Poisons foe on contact."),
        .aiRating = 4,
    },

    [ABILITY_INNER_FOCUS] =
    {
        .name = _("INNER FOCUS"),
        .description = COMPOUND_STRING("Prevents flinching."),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_MAGMA_ARMOR] =
    {
        .name = _("MAGMA ARMOR"),
        .description = COMPOUND_STRING("Prevents freezing."),
        .aiRating = 1,
        .breakable = TRUE,
    },

    [ABILITY_WATER_VEIL] =
    {
        .name = _("WATER VEIL"),
        .description = COMPOUND_STRING("Prevents burns."),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_MAGNET_PULL] =
    {
        .name = _("MAGNET PULL"),
        .description = COMPOUND_STRING("Traps STEEL-type POKéMON."),
        .aiRating = 9,
    },

    [ABILITY_SOUNDPROOF] =
    {
        .name = _("SOUNDPROOF"),
        .description = COMPOUND_STRING("Avoids sound-based moves."),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_RAIN_DISH] =
    {
        .name = _("RAIN DISH"),
        .description = COMPOUND_STRING("Slight HP recovery in rain."),
        .aiRating = 3,
    },

    [ABILITY_SAND_STREAM] =
    {
        .name = _("SAND STREAM"),
        .description = COMPOUND_STRING("Summons a sandstorm."),
        .aiRating = 9,
    },

    [ABILITY_PRESSURE] =
    {
        .name = _("PRESSURE"),
        .description = COMPOUND_STRING("Raises foe's PP usage."),
        .aiRating = 5,
    },

    [ABILITY_THICK_FAT] =
    {
        .name = _("THICK FAT"),
        .description = COMPOUND_STRING("Heat-and-cold protection."),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_EARLY_BIRD] =
    {
        .name = _("EARLY BIRD"),
        .description = COMPOUND_STRING("Awakens quickly from sleep."),
        .aiRating = 4,
    },

    [ABILITY_FLAME_BODY] =
    {
        .name = _("FLAME BODY"),
        .description = COMPOUND_STRING("Burns the foe on contact."),
        .aiRating = 4,
    },

    [ABILITY_RUN_AWAY] =
    {
        .name = _("RUN AWAY"),
        .description = COMPOUND_STRING("Makes escaping easier."),
        .aiRating = 0,
    },

    [ABILITY_KEEN_EYE] =
    {
        .name = _("KEEN EYE"),
        .description = COMPOUND_STRING("Prevents loss of accuracy."),
        .aiRating = 1,
        .breakable = TRUE,
    },

    [ABILITY_HYPER_CUTTER] =
    {
        .name = _("HYPER CUTTER"),
        .description = COMPOUND_STRING("Prevents ATTACK reduction."),
        .aiRating = 3,
        .breakable = TRUE,
    },

    [ABILITY_PICKUP] =
    {
        .name = _("PICKUP"),
        .description = COMPOUND_STRING("May pick up items."),
        .aiRating = 1,
    },

    [ABILITY_TRUANT] =
    {
        .name = _("TRUANT"),
        .description = COMPOUND_STRING("Moves only every two turns."),
        .aiRating = -2,
        .cantBeOverwritten = TRUE,
    },

    [ABILITY_HUSTLE] =
    {
        .name = _("HUSTLE"),
        .description = COMPOUND_STRING("Trades accuracy for power."),
        .aiRating = 7,
    },

    [ABILITY_CUTE_CHARM] =
    {
        .name = _("CUTE CHARM"),
        .description = COMPOUND_STRING("Infatuates on contact."),
        .aiRating = 2,
    },

    [ABILITY_PLUS] =
    {
        .name = _("PLUS"),
        .description = COMPOUND_STRING("Powers up with MINUS."),
        .aiRating = 0,
    },

    [ABILITY_MINUS] =
    {
        .name = _("MINUS"),
        .description = COMPOUND_STRING("Powers up with PLUS."),
        .aiRating = 0,
    },

    [ABILITY_FORECAST] =
    {
        .name = _("FORECAST"),
        .description = COMPOUND_STRING("Changes with the weather."),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeTraced = B_UPDATED_ABILITY_DATA >= GEN_4,
        .failsOnImposter = B_UPDATED_ABILITY_DATA >= GEN_5,
    },

    [ABILITY_STICKY_HOLD] =
    {
        .name = _("STICKY HOLD"),
        .description = COMPOUND_STRING("Prevents item theft."),
        .aiRating = 3,
        .breakable = TRUE,
    },

    [ABILITY_SHED_SKIN] =
    {
        .name = _("SHED SKIN"),
        .description = COMPOUND_STRING("Heals the body by shedding."),
        .aiRating = 7,
    },

    [ABILITY_GUTS] =
    {
        .name = _("GUTS"),
        .description = COMPOUND_STRING("Ups ATTACK if suffering."),
        .aiRating = 6,
    },

    [ABILITY_MARVEL_SCALE] =
    {
        .name = _("MARVEL SCALE"),
        .description = COMPOUND_STRING("Ups DEFENSE if suffering."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_LIQUID_OOZE] =
    {
        .name = _("LIQUID OOZE"),
        .description = COMPOUND_STRING("Draining causes injury."),
        .aiRating = 3,
    },

    [ABILITY_OVERGROW] =
    {
        .name = _("OVERGROW"),
        .description = COMPOUND_STRING("Ups GRASS moves in a pinch."),
        .aiRating = 5,
    },

    [ABILITY_BLAZE] =
    {
        .name = _("BLAZE"),
        .description = COMPOUND_STRING("Ups FIRE moves in a pinch."),
        .aiRating = 5,
    },

    [ABILITY_TORRENT] =
    {
        .name = _("TORRENT"),
        .description = COMPOUND_STRING("Ups WATER moves in a pinch."),
        .aiRating = 5,
    },

    [ABILITY_SWARM] =
    {
        .name = _("SWARM"),
        .description = COMPOUND_STRING("Ups BUG moves in a pinch."),
        .aiRating = 5,
    },

    [ABILITY_ROCK_HEAD] =
    {
        .name = _("ROCK HEAD"),
        .description = COMPOUND_STRING("Prevents recoil damage."),
        .aiRating = 5,
    },

    [ABILITY_DROUGHT] =
    {
        .name = _("DROUGHT"),
        .description = COMPOUND_STRING("Summons sunlight in battle."),
        .aiRating = 9,
    },

    [ABILITY_ARENA_TRAP] =
    {
        .name = _("ARENA TRAP"),
        .description = COMPOUND_STRING("Prevents fleeing."),
        .aiRating = 9,
    },

    [ABILITY_VITAL_SPIRIT] =
    {
        .name = _("VITAL SPIRIT"),
        .description = COMPOUND_STRING("Prevents sleep."),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_WHITE_SMOKE] =
    {
        .name = _("WHITE SMOKE"),
        .description = COMPOUND_STRING("Prevents ability reduction."),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_PURE_POWER] =
    {
        .name = _("PURE POWER"),
        .description = COMPOUND_STRING("Raises ATTACK."),
        .aiRating = 10,
    },

    [ABILITY_SHELL_ARMOR] =
    {
        .name = _("SHELL ARMOR"),
        .description = COMPOUND_STRING("Blocks critical hits."),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_AIR_LOCK] =
    {
        .name = _("AIR LOCK"),
        .description = COMPOUND_STRING("Negates weather effects."),
        .aiRating = 5,
    },

    [ABILITY_TANGLED_FEET] =
    {
        .name = _("TANGLED FEET"),
        .description = COMPOUND_STRING("Boosts evasion if confused."),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_MOTOR_DRIVE] =
    {
        .name = _("MOTOR DRIVE"),
        .description = COMPOUND_STRING("Ups SPEED if hit by ELECTRIC."),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_RIVALRY] =
    {
        .name = _("RIVALRY"),
        .description = COMPOUND_STRING("Strong vs. opposite gender."),
        .aiRating = 1,
    },

    [ABILITY_STEADFAST] =
    {
        .name = _("STEADFAST"),
        .description = COMPOUND_STRING("Flinching raises SPEED."),
        .aiRating = 2,
    },

    [ABILITY_SNOW_CLOAK] =
    {
        .name = _("SNOW CLOAK"),
        .description = COMPOUND_STRING("Ups evasion in hail or snow."),
        .aiRating = 3,
        .breakable = TRUE,
    },

    [ABILITY_GLUTTONY] =
    {
        .name = _("GLUTTONY"),
        .description = COMPOUND_STRING("Eats BERRIES at half HP."),
        .aiRating = 3,
    },

    [ABILITY_ANGER_POINT] =
    {
        .name = _("ANGER POINT"),
        .description = COMPOUND_STRING("Maxes ATTACK when crit."),
        .aiRating = 4,
    },

    [ABILITY_UNBURDEN] =
    {
        .name = _("UNBURDEN"),
        .description = COMPOUND_STRING("Item loss boosts SPEED."),
        .aiRating = 7,
    },

    [ABILITY_HEATPROOF] =
    {
        .name = _("HEATPROOF"),
        .description = COMPOUND_STRING("Halves FIRE-type damage."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_SIMPLE] =
    {
        .name = _("SIMPLE"),
        .description = COMPOUND_STRING("Doubles stat changes."),
        .aiRating = 8,
        .breakable = TRUE,
    },

    [ABILITY_DRY_SKIN] =
    {
        .name = _("DRY SKIN"),
        .description = COMPOUND_STRING("Heat hurts, WATER restores."),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_DOWNLOAD] =
    {
        .name = _("DOWNLOAD"),
        .description = COMPOUND_STRING("Ups offense based on foe."),
        .aiRating = 7,
    },

    [ABILITY_IRON_FIST] =
    {
        .name = _("IRON FIST"),
        .description = COMPOUND_STRING("Powers up punching moves."),
        .aiRating = 6,
    },

    [ABILITY_POISON_HEAL] =
    {
        .name = _("POISON HEAL"),
        .description = COMPOUND_STRING("Restores HP if poisoned."),
        .aiRating = 8,
    },

    [ABILITY_ADAPTABILITY] =
    {
        .name = _("ADAPTABILITY"),
        .description = COMPOUND_STRING("Powers up same-type moves."),
        .aiRating = 8,
    },

    [ABILITY_SKILL_LINK] =
    {
        .name = _("SKILL LINK"),
        .description = COMPOUND_STRING("Maxes multistrike hits."),
        .aiRating = 7,
    },

    [ABILITY_HYDRATION] =
    {
        .name = _("HYDRATION"),
        .description = COMPOUND_STRING("Cures status in rain."),
        .aiRating = 4,
    },

    [ABILITY_SOLAR_POWER] =
    {
        .name = _("SOLAR POWER"),
        .description = COMPOUND_STRING("Sunlight boosts, but hurts."),
        .aiRating = 3,
    },

    [ABILITY_QUICK_FEET] =
    {
        .name = _("QUICK FEET"),
        .description = COMPOUND_STRING("Boosts SPEED if statused."),
        .aiRating = 5,
    },

    [ABILITY_NORMALIZE] =
    {
        .name = _("NORMALIZE"),
        .description = COMPOUND_STRING("Moves become NORMAL-type."),
        .aiRating = -1,
    },

    [ABILITY_SNIPER] =
    {
        .name = _("SNIPER"),
        .description = COMPOUND_STRING("Powers up critical hits."),
        .aiRating = 3,
    },

    [ABILITY_MAGIC_GUARD] =
    {
        .name = _("MAGIC GUARD"),
        .description = COMPOUND_STRING("Only damaged by attacks."),
        .aiRating = 9,
    },

    [ABILITY_NO_GUARD] =
    {
        .name = _("NO GUARD"),
        .description = COMPOUND_STRING("Ensures all attacks land."),
        .aiRating = 8,
    },

    [ABILITY_STALL] =
    {
        .name = _("STALL"),
        .description = COMPOUND_STRING("Always last to use moves."),
        .aiRating = -1,
    },

    [ABILITY_TECHNICIAN] =
    {
        .name = _("TECHNICIAN"),
        .description = COMPOUND_STRING("Powers up weak moves."),
        .aiRating = 8,
    },

    [ABILITY_LEAF_GUARD] =
    {
        .name = _("LEAF GUARD"),
        .description = COMPOUND_STRING("Blocks status in sunlight."),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_KLUTZ] =
    {
        .name = _("KLUTZ"),
        .description = COMPOUND_STRING("Can't use held items."),
        .aiRating = -1,
    },

    [ABILITY_MOLD_BREAKER] =
    {
        .name = _("MOLD BREAKER"),
        .description = COMPOUND_STRING("Moves ignore ABILITIES."),
        .aiRating = 7,
    },

    [ABILITY_SUPER_LUCK] =
    {
        .name = _("SUPER LUCK"),
        .description = COMPOUND_STRING("Raises critical-hit ratio."),
        .aiRating = 3,
    },

    [ABILITY_AFTERMATH] =
    {
        .name = _("AFTERMATH"),
        .description = COMPOUND_STRING("Fainting damages the foe."),
        .aiRating = 5,
    },

    [ABILITY_ANTICIPATION] =
    {
        .name = _("ANTICIPATION"),
        .description = COMPOUND_STRING("Senses dangerous moves."),
        .aiRating = 2,
    },

    [ABILITY_FOREWARN] =
    {
        .name = _("FOREWARN"),
        .description = COMPOUND_STRING("Determines a foe's move."),
        .aiRating = 2,
    },

    [ABILITY_UNAWARE] =
    {
        .name = _("UNAWARE"),
        .description = COMPOUND_STRING("Ignores foes' stat changes."),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_TINTED_LENS] =
    {
        .name = _("TINTED LENS"),
        .description = COMPOUND_STRING("Ups “not very effective”."),
        .aiRating = 7,
    },

    [ABILITY_FILTER] =
    {
        .name = _("FILTER"),
        .description = COMPOUND_STRING("Weakens “supereffective”."),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_SLOW_START] =
    {
        .name = _("SLOW START"),
        .description = COMPOUND_STRING("Takes 5 turns to get going."),
        .aiRating = -2,
    },

    [ABILITY_SCRAPPY] =
    {
        .name = _("SCRAPPY"),
        .description = COMPOUND_STRING("NORMAL & FIGHT. hit GHOSTS."),
        .aiRating = 6,
    },

    [ABILITY_STORM_DRAIN] =
    {
        .name = _("STORM DRAIN"),
        .description = COMPOUND_STRING("Draws WATER to up SP. ATK."),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_ICE_BODY] =
    {
        .name = _("ICE BODY"),
        .description = COMPOUND_STRING("HP recovery in hail or snow."),
        .aiRating = 3,
    },

    [ABILITY_SOLID_ROCK] =
    {
        .name = _("SOLID ROCK"),
        .description = COMPOUND_STRING("Weakens “supereffective”."),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_SNOW_WARNING] =
    {
        .name = _("SNOW WARNING"),
    #if B_SNOW_WARNING >= GEN_9
        .description = COMPOUND_STRING("Summons snow in battle."),
    #else
        .description = COMPOUND_STRING("Summons hail in battle."),
    #endif
        .aiRating = 8,
    },

    [ABILITY_HONEY_GATHER] =
    {
        .name = _("HONEY GATHER"),
        .description = COMPOUND_STRING("May gather HONEY."),
        .aiRating = 0,
    },

    [ABILITY_FRISK] =
    {
        .name = _("FRISK"),
        .description = COMPOUND_STRING("Checks a foe's held item."),
        .aiRating = 3,
    },

    [ABILITY_RECKLESS] =
    {
        .name = _("RECKLESS"),
        .description = COMPOUND_STRING("Powers up recoil moves."),
        .aiRating = 6,
    },

    [ABILITY_MULTITYPE] =
    {
        .name = _("MULTITYPE"),
        .description = COMPOUND_STRING("Matches PLATE or Z-CRYSTAL."),
        .aiRating = 8,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
        .failsOnImposter = B_UPDATED_ABILITY_DATA >= GEN_5,
    },

    [ABILITY_FLOWER_GIFT] =
    {
        .name = _("FLOWER GIFT"),
        .description = COMPOUND_STRING("Sun ups allies' ATK & SP. DEF."),
        .aiRating = 4,
        .cantBeCopied = TRUE,
        .cantBeTraced = B_UPDATED_ABILITY_DATA >= GEN_5,
        .breakable = TRUE,
    },

    [ABILITY_BAD_DREAMS] =
    {
        .name = _("BAD DREAMS"),
        .description = COMPOUND_STRING("Damages sleeping foes."),
        .aiRating = 4,
    },

    [ABILITY_PICKPOCKET] =
    {
        .name = _("PICKPOCKET"),
        .description = COMPOUND_STRING("Steals foe's item if hit."),
        .aiRating = 3,
    },

    [ABILITY_SHEER_FORCE] =
    {
        .name = _("SHEER FORCE"),
        .description = COMPOUND_STRING("Trades effects for power."),
        .aiRating = 8,
    },

    [ABILITY_CONTRARY] =
    {
        .name = _("CONTRARY"),
        .description = COMPOUND_STRING("Reverses stat changes."),
        .aiRating = 8,
        .breakable = TRUE,
    },

    [ABILITY_UNNERVE] =
    {
        .name = _("UNNERVE"),
        .description = COMPOUND_STRING("Foes can't eat BERRIES."),
        .aiRating = 3,
    },

    [ABILITY_DEFIANT] =
    {
        .name = _("DEFIANT"),
        .description = COMPOUND_STRING("Stat-lowering ups ATTACK."),
        .aiRating = 5,
    },

    [ABILITY_DEFEATIST] =
    {
        .name = _("DEFEATIST"),
        .description = COMPOUND_STRING("Halves offenses at half HP."),
        .aiRating = -1,
    },

    [ABILITY_CURSED_BODY] =
    {
        .name = _("CURSED BODY"),
        .description = COMPOUND_STRING("Contact may disable move."),
        .aiRating = 4,
    },

    [ABILITY_HEALER] =
    {
        .name = _("HEALER"),
        .description = COMPOUND_STRING("May cure ally's status."),
        .aiRating = 0,
    },

    [ABILITY_FRIEND_GUARD] =
    {
        .name = _("FRIEND GUARD"),
        .description = COMPOUND_STRING("Reduces damage to allies."),
        .aiRating = 0,
        .breakable = TRUE,
    },

    [ABILITY_WEAK_ARMOR] =
    {
        .name = _("WEAK ARMOR"),
        .description = COMPOUND_STRING("Hits change DEF and SPEED."),
        .aiRating = 2,
    },

    [ABILITY_HEAVY_METAL] =
    {
        .name = _("HEAVY METAL"),
        .description = COMPOUND_STRING("Doubles weight."),
        .aiRating = -1,
        .breakable = TRUE,
    },

    [ABILITY_LIGHT_METAL] =
    {
        .name = _("LIGHT METAL"),
        .description = COMPOUND_STRING("Halves weight."),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_MULTISCALE] =
    {
        .name = _("MULTISCALE"),
        .description = COMPOUND_STRING("Reduces damage at full HP."),
        .aiRating = 8,
        .breakable = TRUE,
    },

    [ABILITY_TOXIC_BOOST] =
    {
        .name = _("TOXIC BOOST"),
        .description = COMPOUND_STRING("Boosts ATTACK if poisoned."),
        .aiRating = 6,
    },

    [ABILITY_FLARE_BOOST] =
    {
        .name = _("FLARE BOOST"),
        .description = COMPOUND_STRING("Boosts SP. ATK if burned."),
        .aiRating = 5,
    },

    [ABILITY_HARVEST] =
    {
        .name = _("HARVEST"),
        .description = COMPOUND_STRING("May create another BERRY."),
        .aiRating = 5,
    },

    [ABILITY_TELEPATHY] =
    {
        .name = _("TELEPATHY"),
        .description = COMPOUND_STRING("Dodges ally's attacks."),
        .aiRating = 0,
        .breakable = TRUE,
    },

    [ABILITY_MOODY] =
    {
        .name = _("MOODY"),
        .description = COMPOUND_STRING("Stats change every turn."),
        .aiRating = 10,
    },

    [ABILITY_OVERCOAT] =
    {
        .name = _("OVERCOAT"),
        .description = COMPOUND_STRING("Blocks weather and powder."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_POISON_TOUCH] =
    {
        .name = _("POISON TOUCH"),
        .description = COMPOUND_STRING("Contact moves may poison."),
        .aiRating = 4,
    },

    [ABILITY_REGENERATOR] =
    {
        .name = _("REGENERATOR"),
        .description = COMPOUND_STRING("Restores HP upon switch."),
        .aiRating = 8,
    },

    [ABILITY_BIG_PECKS] =
    {
        .name = _("BIG PECKS"),
        .description = COMPOUND_STRING("DEFENSE can't be lowered."),
        .aiRating = 1,
        .breakable = TRUE,
    },

    [ABILITY_SAND_RUSH] =
    {
        .name = _("SAND RUSH"),
        .description = COMPOUND_STRING("Ups SPEED in a sandstorm."),
        .aiRating = 6,
    },

    [ABILITY_WONDER_SKIN] =
    {
        .name = _("WONDER SKIN"),
        .description = COMPOUND_STRING("May avoid status moves."),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_ANALYTIC] =
    {
        .name = _("ANALYTIC"),
        .description = COMPOUND_STRING("Boosts power if last to act."),
        .aiRating = 5,
    },

    [ABILITY_ILLUSION] =
    {
        .name = _("ILLUSION"),
        .description = COMPOUND_STRING("Disguises as a partner."),
        .aiRating = 8,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
    },

    [ABILITY_IMPOSTER] =
    {
        .name = _("IMPOSTER"),
        .description = COMPOUND_STRING("Transforms into the foe."),
        .aiRating = 9,
        .cantBeCopied = TRUE,
        .cantBeTraced = TRUE,
    },

    [ABILITY_INFILTRATOR] =
    {
        .name = _("INFILTRATOR"),
        .description = COMPOUND_STRING("Passes through barriers."),
        .aiRating = 6,
    },

    [ABILITY_MUMMY] =
    {
        .name = _("MUMMY"),
        .description = COMPOUND_STRING("ABILITY spreads on contact."),
        .aiRating = 5,
    },

    [ABILITY_MOXIE] =
    {
        .name = _("MOXIE"),
        .description = COMPOUND_STRING("Raises ATTACK after KOs."),
        .aiRating = 7,
    },

    [ABILITY_JUSTIFIED] =
    {
        .name = _("JUSTIFIED"),
        .description = COMPOUND_STRING("Ups ATTACK if hit by DARK."),
        .aiRating = 4,
    },

    [ABILITY_RATTLED] =
    {
        .name = _("RATTLED"),
        .description = COMPOUND_STRING("Raises SPEED when scared."),
        .aiRating = 3,
    },

    [ABILITY_MAGIC_BOUNCE] =
    {
        .name = _("MAGIC BOUNCE"),
        .description = COMPOUND_STRING("Reflects status moves."),
        .aiRating = 9,
        .breakable = TRUE,
    },

    [ABILITY_SAP_SIPPER] =
    {
        .name = _("SAP SIPPER"),
        .description = COMPOUND_STRING("Ups ATTACK if hit by GRASS."),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_PRANKSTER] =
    {
        .name = _("PRANKSTER"),
        .description = COMPOUND_STRING("Status moves have priority."),
        .aiRating = 8,
    },

    [ABILITY_SAND_FORCE] =
    {
        .name = _("SAND FORCE"),
        .description = COMPOUND_STRING("Powers up in a sandstorm."),
        .aiRating = 4,
    },

    [ABILITY_IRON_BARBS] =
    {
        .name = _("IRON BARBS"),
        .description = COMPOUND_STRING("Damages foe on contact."),
        .aiRating = 6,
    },

    [ABILITY_ZEN_MODE] =
    {
        .name = _("ZEN MODE"),
        .description = COMPOUND_STRING("Changes shape at half HP."),
        .aiRating = -1,
        .cantBeCopied = TRUE,
        .cantBeSwapped = B_UPDATED_ABILITY_DATA >= GEN_7,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = B_UPDATED_ABILITY_DATA >= GEN_7,
        .cantBeOverwritten = B_UPDATED_ABILITY_DATA >= GEN_7,
        .failsOnImposter = TRUE,
    },

    [ABILITY_VICTORY_STAR] =
    {
        .name = _("VICTORY STAR"),
        .description = COMPOUND_STRING("Boosts allies' accuracy."),
        .aiRating = 6,
    },

    [ABILITY_TURBOBLAZE] =
    {
        .name = _("TURBOBLAZE"),
        .description = COMPOUND_STRING("Moves ignore ABILITIES."),
        .aiRating = 7,
    },

    [ABILITY_TERAVOLT] =
    {
        .name = _("TERAVOLT"),
        .description = COMPOUND_STRING("Moves ignore ABILITIES."),
        .aiRating = 7,
    },

    [ABILITY_AROMA_VEIL] =
    {
        .name = _("AROMA VEIL"),
        .description = COMPOUND_STRING("Allies can't be move-bound."),
        .aiRating = 3,
        .breakable = TRUE,
    },

    [ABILITY_FLOWER_VEIL] =
    {
        .name = _("FLOWER VEIL"),
        .description = COMPOUND_STRING("Mist for GRASS-type allies."),
        .aiRating = 0,
        .breakable = TRUE,
    },

    [ABILITY_CHEEK_POUCH] =
    {
        .name = _("CHEEK POUCH"),
        .description = COMPOUND_STRING("Eating a BERRY restores HP."),
        .aiRating = 4,
    },

    [ABILITY_PROTEAN] =
    {
        .name = _("PROTEAN"),
        .description = COMPOUND_STRING("Changes type to used move."),
        .aiRating = 8,
    },

    [ABILITY_FUR_COAT] =
    {
        .name = _("FUR COAT"),
        .description = COMPOUND_STRING("Halves physical damage."),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_MAGICIAN] =
    {
        .name = _("MAGICIAN"),
        .description = COMPOUND_STRING("Steals the foe's held item."),
        .aiRating = 3,
    },

    [ABILITY_BULLETPROOF] =
    {
        .name = _("BULLETPROOF"),
        .description = COMPOUND_STRING("Blocks ball and bomb moves."),
        .breakable = TRUE,
        .aiRating = 7,
    },

    [ABILITY_COMPETITIVE] =
    {
        .name = _("COMPETITIVE"),
        .description = COMPOUND_STRING("Stat-lowering ups SP. ATK."),
        .aiRating = 5,
    },

    [ABILITY_STRONG_JAW] =
    {
        .name = _("STRONG JAW"),
        .description = COMPOUND_STRING("Boosts biting moves."),
        .aiRating = 6,
    },

    [ABILITY_REFRIGERATE] =
    {
        .name = _("REFRIGERATE"),
        .description = COMPOUND_STRING("NORMAL moves become ICE."),
        .aiRating = 8,
    },

    [ABILITY_SWEET_VEIL] =
    {
        .name = _("SWEET VEIL"),
        .description = COMPOUND_STRING("Protects allies from sleep."),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_STANCE_CHANGE] =
    {
        .name = _("STANCE CHANGE"),
        .description = COMPOUND_STRING("Changes form as it battles."),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_GALE_WINGS] =
    {
        .name = _("GALE WINGS"),
        .description = COMPOUND_STRING("FLYING moves have priority."),
        .aiRating = 6,
    },

    [ABILITY_MEGA_LAUNCHER] =
    {
        .name = _("MEGA LAUNCHER"),
        .description = COMPOUND_STRING("Powers up pulse moves."),
        .aiRating = 7,
    },

    [ABILITY_GRASS_PELT] =
    {
        .name = _("GRASS PELT"),
        .description = COMPOUND_STRING("GRASSY TERRAIN ups DEFENSE."),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_SYMBIOSIS] =
    {
        .name = _("SYMBIOSIS"),
        .description = COMPOUND_STRING("Passes held item to ally."),
        .aiRating = 0,
    },

    [ABILITY_TOUGH_CLAWS] =
    {
        .name = _("TOUGH CLAWS"),
        .description = COMPOUND_STRING("Powers up contact moves."),
        .aiRating = 7,
    },

    [ABILITY_PIXILATE] =
    {
        .name = _("PIXILATE"),
        .description = COMPOUND_STRING("NORMAL moves become FAIRY."),
        .aiRating = 8,
    },

    [ABILITY_GOOEY] =
    {
        .name = _("GOOEY"),
        .description = COMPOUND_STRING("Contact lowers foe's SPEED."),
        .aiRating = 5,
    },

    [ABILITY_AERILATE] =
    {
        .name = _("AERILATE"),
        .description = COMPOUND_STRING("NORMAL moves become FLYING."),
        .aiRating = 8,
    },

    [ABILITY_PARENTAL_BOND] =
    {
        .name = _("PARENTAL BOND"),
        .description = COMPOUND_STRING("Parent & child each attack."),
        .aiRating = 10,
    },

    [ABILITY_DARK_AURA] =
    {
        .name = _("DARK AURA"),
        .description = COMPOUND_STRING("Ups all DARK-type moves."),
        .aiRating = 6,
        .breakable = B_UPDATED_ABILITY_DATA < GEN_8,
    },

    [ABILITY_FAIRY_AURA] =
    {
        .name = _("FAIRY AURA"),
        .description = COMPOUND_STRING("Ups all FAIRY-type moves."),
        .aiRating = 6,
        .breakable = B_UPDATED_ABILITY_DATA < GEN_8,
    },

    [ABILITY_AURA_BREAK] =
    {
        .name = _("AURA BREAK"),
        .description = COMPOUND_STRING("Reverses AURA ABILITIES."),
        .aiRating = 3,
        .breakable = TRUE,
    },

    [ABILITY_PRIMORDIAL_SEA] =
    {
        .name = _("PRIMORDIAL SEA"),
        .description = COMPOUND_STRING("Summons heavy rain."),
        .aiRating = 10,
    },

    [ABILITY_DESOLATE_LAND] =
    {
        .name = _("DESOLATE LAND"),
        .description = COMPOUND_STRING("Summons extreme sunlight."),
        .aiRating = 10,
    },

    [ABILITY_DELTA_STREAM] =
    {
        .name = _("DELTA STREAM"),
        .description = COMPOUND_STRING("Summons strong winds."),
        .aiRating = 10,
    },

    [ABILITY_STAMINA] =
    {
        .name = _("STAMINA"),
        .description = COMPOUND_STRING("Raises DEFENSE when hit."),
        .aiRating = 6,
    },

    [ABILITY_WIMP_OUT] =
    {
        .name = _("WIMP OUT"),
        .description = COMPOUND_STRING("Switches out at half HP."),
        .aiRating = 3,
    },

    [ABILITY_EMERGENCY_EXIT] =
    {
        .name = _("EMERGENCY EXIT"),
        .description = COMPOUND_STRING("Switches out at half HP."),
        .aiRating = 3,
    },

    [ABILITY_WATER_COMPACTION] =
    {
        .name = _("WATER COMPACTION"),
        .description = COMPOUND_STRING("Ups DEFENSE if hit by WATER."),
        .aiRating = 4,
    },

    [ABILITY_MERCILESS] =
    {
        .name = _("MERCILESS"),
        .description = COMPOUND_STRING("Critical hits poisoned foes."),
        .aiRating = 4,
    },

    [ABILITY_SHIELDS_DOWN] =
    {
        .name = _("SHIELDS DOWN"),
        .description = COMPOUND_STRING("Shell breaks at half HP."),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_STAKEOUT] =
    {
        .name = _("STAKEOUT"),
        .description = COMPOUND_STRING("Stronger on foe's switch in."),
        .aiRating = 6,
    },

    [ABILITY_WATER_BUBBLE] =
    {
        .name = _("WATER BUBBLE"),
        .description = COMPOUND_STRING("Guards from FIRE and burns."),
        .aiRating = 8,
        .breakable = TRUE,
    },

    [ABILITY_STEELWORKER] =
    {
        .name = _("STEELWORKER"),
        .description = COMPOUND_STRING("Ups STEEL-type moves."),
        .aiRating = 6,
    },

    [ABILITY_BERSERK] =
    {
        .name = _("BERSERK"),
        .description = COMPOUND_STRING("Boosts SP. ATK at half HP."),
        .aiRating = 5,
    },

    [ABILITY_SLUSH_RUSH] =
    {
        .name = _("SLUSH RUSH"),
        .description = COMPOUND_STRING("Ups SPEED in hail or snow."),
        .aiRating = 5,
    },

    [ABILITY_LONG_REACH] =
    {
        .name = _("LONG REACH"),
        .description = COMPOUND_STRING("Moves don't make contact."),
        .aiRating = 3,
    },

    [ABILITY_LIQUID_VOICE] =
    {
        .name = _("LIQUID VOICE"),
        .description = COMPOUND_STRING("Sound moves become WATER."),
        .aiRating = 5,
    },

    [ABILITY_TRIAGE] =
    {
        .name = _("TRIAGE"),
        .description = COMPOUND_STRING("Healing moves have priority."),
        .aiRating = 7,
    },

    [ABILITY_GALVANIZE] =
    {
        .name = _("GALVANIZE"),
        .description = COMPOUND_STRING("NORMAL moves turn ELECTRIC."),
        .aiRating = 8,
    },

    [ABILITY_SURGE_SURFER] =
    {
        .name = _("SURGE SURFER"),
        .description = COMPOUND_STRING("ELECTRIC TERRAIN ups SPEED."),
        .aiRating = 4,
    },

    [ABILITY_SCHOOLING] =
    {
        .name = _("SCHOOLING"),
        .description = COMPOUND_STRING("Forms a school at high HP."),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_DISGUISE] =
    {
        .name = _("DISGUISE"),
        .description = COMPOUND_STRING("Shroud protects once."),
        .aiRating = 8,
        .breakable = TRUE,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_BATTLE_BOND] =
    {
        .name = _("BATTLE BOND"),
        .description = COMPOUND_STRING("Changes form after a KO."),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_POWER_CONSTRUCT] =
    {
        .name = _("POWER CONSTRUCT"),
        .description = COMPOUND_STRING("Changes form at half HP."),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_CORROSION] =
    {
        .name = _("CORROSION"),
        .description = COMPOUND_STRING("Can poison STEEL & POISON."),
        .aiRating = 5,
    },

    [ABILITY_COMATOSE] =
    {
        .name = _("COMATOSE"),
        .description = COMPOUND_STRING("Always drowsing."),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
    },

    [ABILITY_QUEENLY_MAJESTY] =
    {
        .name = _("QUEENLY MAJESTY"),
        .description = COMPOUND_STRING("Allies block priority moves."),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_INNARDS_OUT] =
    {
        .name = _("INNARDS OUT"),
        .description = COMPOUND_STRING("Damages the foe if KO'd."),
        .aiRating = 5,
    },

    [ABILITY_DANCER] =
    {
        .name = _("DANCER"),
        .description = COMPOUND_STRING("Dances along with others."),
        .aiRating = 5,
    },

    [ABILITY_BATTERY] =
    {
        .name = _("BATTERY"),
        .description = COMPOUND_STRING("Boosts ally's SP. ATK."),
        .aiRating = 0,
    },

    [ABILITY_FLUFFY] =
    {
        .name = _("FLUFFY"),
        .description = COMPOUND_STRING("Halves contact, heat hurts."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_DAZZLING] =
    {
        .name = _("DAZZLING"),
        .description = COMPOUND_STRING("Allies block priority moves."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_SOUL_HEART] =
    {
        .name = _("SOUL-HEART"),
        .description = COMPOUND_STRING("Others fainting ups SP. ATK."),
        .aiRating = 7,
    },

    [ABILITY_TANGLING_HAIR] =
    {
        .name = _("TANGLING HAIR"),
        .description = COMPOUND_STRING("Contact lowers foe's SPEED."),
        .aiRating = 5,
    },

    [ABILITY_RECEIVER] =
    {
        .name = _("RECEIVER"),
        .description = COMPOUND_STRING("Copies fainted ally ABILITY."),
        .aiRating = 0,
        .cantBeCopied = TRUE,
        .cantBeTraced = TRUE,
    },

    [ABILITY_POWER_OF_ALCHEMY] =
    {
        .name = _("POWER OF ALCHEMY"),
        .description = COMPOUND_STRING("Copies fainted ally ABILITY."),
        .aiRating = 0,
        .cantBeCopied = TRUE,
        .cantBeTraced = TRUE,
    },

    [ABILITY_BEAST_BOOST] =
    {
        .name = _("BEAST BOOST"),
        .description = COMPOUND_STRING("Raises best stat after KOs."),
        .aiRating = 7,
    },

    [ABILITY_RKS_SYSTEM] =
    {
        .name = _("RKS SYSTEM"),
        .description = COMPOUND_STRING("Matches held MEMORY's type."),
        .aiRating = 8,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_ELECTRIC_SURGE] =
    {
        .name = _("ELECTRIC SURGE"),
        .description = COMPOUND_STRING("Makes the field electrified."),
        .aiRating = 8,
    },

    [ABILITY_PSYCHIC_SURGE] =
    {
        .name = _("PSYCHIC SURGE"),
        .description = COMPOUND_STRING("Makes the field weird."),
        .aiRating = 8,
    },

    [ABILITY_MISTY_SURGE] =
    {
        .name = _("MISTY SURGE"),
        .description = COMPOUND_STRING("Makes the field misty."),
        .aiRating = 8,
    },

    [ABILITY_GRASSY_SURGE] =
    {
        .name = _("GRASSY SURGE"),
        .description = COMPOUND_STRING("Makes the field grassy."),
        .aiRating = 8,
    },

    [ABILITY_FULL_METAL_BODY] =
    {
        .name = _("FULL METAL BODY"),
        .description = COMPOUND_STRING("Stats can't be lowered."),
        .aiRating = 4,
    },

    [ABILITY_SHADOW_SHIELD] =
    {
        .name = _("SHADOW SHIELD"),
        .description = COMPOUND_STRING("Reduces damage at full HP."),
        .aiRating = 8,
    },

    [ABILITY_PRISM_ARMOR] =
    {
        .name = _("PRISM ARMOR"),
        .description = COMPOUND_STRING("Weakens “supereffective”."),
        .aiRating = 6,
    },

    [ABILITY_NEUROFORCE] =
    {
        .name = _("NEUROFORCE"),
        .description = COMPOUND_STRING("Ups “supereffective” hits."),
        .aiRating = 6,
    },

    [ABILITY_INTREPID_SWORD] =
    {
        .name = _("INTREPID SWORD"),
        .description = COMPOUND_STRING("Ups ATTACK on entry once."),
        .aiRating = 3,
    },

    [ABILITY_DAUNTLESS_SHIELD] =
    {
        .name = _("DAUNTLESS SHIELD"),
        .description = COMPOUND_STRING("Ups DEFENSE on entry once."),
        .aiRating = 3,
    },

    [ABILITY_LIBERO] =
    {
        .name = _("LIBERO"),
        .description = COMPOUND_STRING("Changes type to used move."),
    },

    [ABILITY_BALL_FETCH] =
    {
        .name = _("BALL FETCH"),
        .description = COMPOUND_STRING("Fetches first failed BALL."),
        .aiRating = 0,
    },

    [ABILITY_COTTON_DOWN] =
    {
        .name = _("COTTON DOWN"),
        .description = COMPOUND_STRING("Lowers others' SPEED if hit."),
        .aiRating = 3,
    },

    [ABILITY_PROPELLER_TAIL] =
    {
        .name = _("PROPELLER TAIL"),
        .description = COMPOUND_STRING("Ignores foes' move-drawing."),
        .aiRating = 2,
    },

    [ABILITY_MIRROR_ARMOR] =
    {
        .name = _("MIRROR ARMOR"),
        .description = COMPOUND_STRING("Bounces stat-lowering."),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_GULP_MISSILE] =
    {
        .name = _("GULP MISSILE"),
        .description = COMPOUND_STRING("Spits when hit after DIVE."),
        .aiRating = 3,
        .cantBeSwapped = B_UPDATED_ABILITY_DATA < GEN_9,
        .cantBeCopied = B_UPDATED_ABILITY_DATA < GEN_9,
        .cantBeTraced = B_UPDATED_ABILITY_DATA < GEN_9,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_STALWART] =
    {
        .name = _("STALWART"),
        .description = COMPOUND_STRING("Ignores foes' move-drawing."),
        .aiRating = 2,
    },

    [ABILITY_STEAM_ENGINE] =
    {
        .name = _("STEAM ENGINE"),
        .description = COMPOUND_STRING("FIRE & WATER hits up SPEED."),
        .aiRating = 3,
    },

    [ABILITY_PUNK_ROCK] =
    {
        .name = _("PUNK ROCK"),
        .description = COMPOUND_STRING("Boosts and resists sound."),
        .aiRating = 2,
        .breakable = TRUE,
    },

    [ABILITY_SAND_SPIT] =
    {
        .name = _("SAND SPIT"),
        .description = COMPOUND_STRING("Summons a sandstorm if hit."),
        .aiRating = 5,
    },

    [ABILITY_ICE_SCALES] =
    {
        .name = _("ICE SCALES"),
        .description = COMPOUND_STRING("Halves special damage."),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_RIPEN] =
    {
        .name = _("RIPEN"),
        .description = COMPOUND_STRING("Doubles BERRY effects."),
        .aiRating = 4,
    },

    [ABILITY_ICE_FACE] =
    {
        .name = _("ICE FACE"),
        .description = COMPOUND_STRING("Hail & snow renew free hit."),
        .aiRating = 4,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
        .breakable = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_POWER_SPOT] =
    {
        .name = _("POWER SPOT"),
        .description = COMPOUND_STRING("Powers up ally's moves."),
        .aiRating = 2,
    },

    [ABILITY_MIMICRY] =
    {
        .name = _("MIMICRY"),
        .description = COMPOUND_STRING("Changes type on terrain."),
        .aiRating = 2,
    },

    [ABILITY_SCREEN_CLEANER] =
    {
        .name = _("SCREEN CLEANER"),
        .description = COMPOUND_STRING("Nullifies barriers on entry."),
        .aiRating = 3,
    },

    [ABILITY_STEELY_SPIRIT] =
    {
        .name = _("STEELY SPIRIT"),
        .description = COMPOUND_STRING("Ups allies' STEEL moves."),
        .aiRating = 2,
    },

    [ABILITY_PERISH_BODY] =
    {
        .name = _("PERISH BODY"),
        .description = COMPOUND_STRING("Foe faints in 3 turns if hit."),
        .aiRating = -1,
    },

    [ABILITY_WANDERING_SPIRIT] =
    {
        .name = _("WANDERING SPIRIT"),
        .description = COMPOUND_STRING("Swaps ABILITIES on contact."),
        .aiRating = 2,
    },

    [ABILITY_GORILLA_TACTICS] =
    {
        .name = _("GORILLA TACTICS"),
        .description = COMPOUND_STRING("Ups ATTACK and locks move."),
        .aiRating = 4,
    },

    [ABILITY_NEUTRALIZING_GAS] =
    {
        .name = _("NEUTRALIZING GAS"),
        .description = COMPOUND_STRING("Nullifies others' ABILITIES."),
        .aiRating = 5,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_PASTEL_VEIL] =
    {
        .name = _("PASTEL VEIL"),
        .description = COMPOUND_STRING("Allies can't be poisoned."),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_HUNGER_SWITCH] =
    {
        .name = _("HUNGER SWITCH"),
        .description = COMPOUND_STRING("Changes form every turn."),
        .aiRating = 2,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_QUICK_DRAW] =
    {
        .name = _("QUICK DRAW"),
        .description = COMPOUND_STRING("Occasionally moves first."),
        .aiRating = 4,
    },

    [ABILITY_UNSEEN_FIST] =
    {
        .name = _("UNSEEN FIST"),
        .description = COMPOUND_STRING("Contact evades protection."),
        .aiRating = 6,
    },

    [ABILITY_CURIOUS_MEDICINE] =
    {
        .name = _("CURIOUS MEDICINE"),
        .description = COMPOUND_STRING("Removes ally stat changes."),
        .aiRating = 3,
    },

    [ABILITY_TRANSISTOR] =
    {
        .name = _("TRANSISTOR"),
        .description = COMPOUND_STRING("Ups ELECTRIC-type moves."),
        .aiRating = 6,
    },

    [ABILITY_DRAGONS_MAW] =
    {
        .name = _("DRAGON'S MAW"),
        .description = COMPOUND_STRING("Ups DRAGON-type moves."),
        .aiRating = 6,
    },

    [ABILITY_CHILLING_NEIGH] =
    {
        .name = _("CHILLING NEIGH"),
        .description = COMPOUND_STRING("Raises ATTACK after KOs."),
        .aiRating = 7,
    },

    [ABILITY_GRIM_NEIGH] =
    {
        .name = _("GRIM NEIGH"),
        .description = COMPOUND_STRING("Raises SP. ATK after KOs."),
        .aiRating = 7,
    },

    [ABILITY_AS_ONE_ICE_RIDER] =
    {
        .name = _("AS ONE"),
        .description = COMPOUND_STRING("UNNERVE and CHILLING NEIGH."),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
    },

    [ABILITY_AS_ONE_SHADOW_RIDER] =
    {
        .name = _("AS ONE"),
        .description = COMPOUND_STRING("UNNERVE and GRIM NEIGH."),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
    },

    [ABILITY_LINGERING_AROMA] =
    {
        .name = _("LINGERING AROMA"),
        .description = COMPOUND_STRING("ABILITY spreads on contact."),
        .aiRating = 5,
    },

    [ABILITY_SEED_SOWER] =
    {
        .name = _("SEED SOWER"),
        .description = COMPOUND_STRING("Makes field grassy if hit."),
        .aiRating = 5,
    },

    [ABILITY_THERMAL_EXCHANGE] =
    {
        .name = _("THERMAL EXCHANGE"),
        .description = COMPOUND_STRING("Ups ATTACK if hit by FIRE."),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_ANGER_SHELL] =
    {
        .name = _("ANGER SHELL"),
        .description = COMPOUND_STRING("Changes stats at half HP."),
        .aiRating = 3,
    },

    [ABILITY_PURIFYING_SALT] =
    {
        .name = _("PURIFYING SALT"),
        .description = COMPOUND_STRING("Guards from status & GHOST."),
        .aiRating = 6,
        .breakable = TRUE,
    },

    [ABILITY_WELL_BAKED_BODY] =
    {
        .name = _("WELL-BAKED BODY"),
        .description = COMPOUND_STRING("Ups DEFENSE if hit by FIRE."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_WIND_RIDER] =
    {
        .name = _("WIND RIDER"),
        .description = COMPOUND_STRING("Wind moves raise ATTACK."),
        .aiRating = 4,
        .breakable = TRUE,
    },

    [ABILITY_GUARD_DOG] =
    {
        .name = _("GUARD DOG"),
        .description = COMPOUND_STRING("Raises ATK if intimidated."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_ROCKY_PAYLOAD] =
    {
        .name = _("ROCKY PAYLOAD"),
        .description = COMPOUND_STRING("Powers up ROCK-type moves."),
        .aiRating = 6,
    },

    [ABILITY_WIND_POWER] =
    {
        .name = _("WIND POWER"),
        .description = COMPOUND_STRING("Gets charged if hit by wind."),
        .aiRating = 4,
    },

    [ABILITY_ZERO_TO_HERO] =
    {
        .name = _("ZERO TO HERO"),
        .description = COMPOUND_STRING("Changes form upon switch."),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_COMMANDER] =
    {
        .name = _("COMMANDER"),
        .description = COMPOUND_STRING("Commands from ally DONDOZO."),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
    },

    [ABILITY_ELECTROMORPHOSIS] =
    {
        .name = _("ELECTROMORPHOSIS"),
        .description = COMPOUND_STRING("Becomes charged when hit."),
        .aiRating = 5,
    },

    [ABILITY_PROTOSYNTHESIS] =
    {
        .name = _("PROTOSYNTHESIS"),
        .description = COMPOUND_STRING("Sunlight boosts best stat."),
        .aiRating = 7,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_QUARK_DRIVE] =
    {
        .name = _("QUARK DRIVE"),
        .description = COMPOUND_STRING("ELECTRIC TERRAIN ups best stat."),
        .aiRating = 7,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_GOOD_AS_GOLD] =
    {
        .name = _("GOOD AS GOLD"),
        .description = COMPOUND_STRING("Immunity to status moves."),
        .aiRating = 8,
        .breakable = TRUE,
    },

    [ABILITY_VESSEL_OF_RUIN] =
    {
        .name = _("VESSEL OF RUIN"),
        .description = COMPOUND_STRING("Reduces others' SP. ATK."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_SWORD_OF_RUIN] =
    {
        .name = _("SWORD OF RUIN"),
        .description = COMPOUND_STRING("Reduces others' DEFENSE."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_TABLETS_OF_RUIN] =
    {
        .name = _("TABLETS OF RUIN"),
        .description = COMPOUND_STRING("Reduces others' ATTACK."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_BEADS_OF_RUIN] =
    {
        .name = _("BEADS OF RUIN"),
        .description = COMPOUND_STRING("Reduces others' SP. DEF."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_ORICHALCUM_PULSE] =
    {
        .name = _("ORICHALCUM PULSE"),
        .description = COMPOUND_STRING("Summons sun and ups ATK."),
        .aiRating = 8,
    },

    [ABILITY_HADRON_ENGINE] =
    {
        .name = _("HADRON ENGINE"),
        .description = COMPOUND_STRING("ELECTRIC TERRAIN & ups SP. ATK."),
        .aiRating = 8,
    },

    [ABILITY_OPPORTUNIST] =
    {
        .name = _("OPPORTUNIST"),
        .description = COMPOUND_STRING("Copies a foe's stat boost."),
        .aiRating = 5,
    },

    [ABILITY_CUD_CHEW] =
    {
        .name = _("CUD CHEW"),
        .description = COMPOUND_STRING("Eats BERRY again next turn."),
        .aiRating = 4,
    },

    [ABILITY_SHARPNESS] =
    {
        .name = _("SHARPNESS"),
        .description = COMPOUND_STRING("Powers up slicing moves."),
        .aiRating = 7,
    },

    [ABILITY_SUPREME_OVERLORD] =
    {
        .name = _("SUPREME OVERLORD"),
        .description = COMPOUND_STRING("Fainted partners up power."),
        .aiRating = 6,
    },

    [ABILITY_COSTAR] =
    {
        .name = _("COSTAR"),
        .description = COMPOUND_STRING("Copies ally's stat changes."),
        .aiRating = 5,
    },

    [ABILITY_TOXIC_DEBRIS] =
    {
        .name = _("TOXIC DEBRIS"),
        .description = COMPOUND_STRING("Lays poison spikes if hit."),
        .aiRating = 4,
    },

    [ABILITY_ARMOR_TAIL] =
    {
        .name = _("ARMOR TAIL"),
        .description = COMPOUND_STRING("Allies block priority moves."),
        .aiRating = 5,
        .breakable = TRUE,
    },

    [ABILITY_EARTH_EATER] =
    {
        .name = _("EARTH EATER"),
        .description = COMPOUND_STRING("Restores if hit by GROUND."),
        .aiRating = 7,
        .breakable = TRUE,
    },

    [ABILITY_MYCELIUM_MIGHT] =
    {
        .name = _("MYCELIUM MIGHT"),
        .description = COMPOUND_STRING("Status moves can't fail."),
        .aiRating = 2,
    },

    [ABILITY_HOSPITALITY] =
    {
        .name = _("HOSPITALITY"),
        .description = COMPOUND_STRING("Restores ally's HP on entry."),
        .aiRating = 5,
    },

    [ABILITY_MINDS_EYE] =
    {
        .name = _("MIND'S EYE"),
        .description = COMPOUND_STRING("Grants FORESIGHT."),
        .aiRating = 8,
        .breakable = TRUE,
    },

    [ABILITY_EMBODY_ASPECT_TEAL_MASK] =
    {
        .name = _("EMBODY ASPECT"),
        .description = COMPOUND_STRING("TERASTAL raises SPEED."),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_EMBODY_ASPECT_HEARTHFLAME_MASK] =
    {
        .name = _("EMBODY ASPECT"),
        .description = COMPOUND_STRING("TERASTAL raises ATTACK."),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_EMBODY_ASPECT_WELLSPRING_MASK] =
    {
        .name = _("EMBODY ASPECT"),
        .description = COMPOUND_STRING("TERASTAL raises SP. DEF."),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_EMBODY_ASPECT_CORNERSTONE_MASK] =
    {
        .name = _("EMBODY ASPECT"),
        .description = COMPOUND_STRING("TERASTAL raises DEFENSE."),
        .aiRating = 6,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_TOXIC_CHAIN] =
    {
        .name = _("TOXIC CHAIN"),
        .description = COMPOUND_STRING("Attacks may badly poison."),
        .aiRating = 8,
    },

    [ABILITY_SUPERSWEET_SYRUP] =
    {
        .name = _("SUPERSWEET SYRUP"),
        .description = COMPOUND_STRING("Lowers foes' evasion once."),
        .aiRating = 5,
    },

    [ABILITY_TERA_SHIFT] =
    {
        .name = _("TERA SHIFT"),
        .description = COMPOUND_STRING("Changes form on entry."),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .cantBeSuppressed = TRUE,
        .cantBeOverwritten = TRUE,
        .failsOnImposter = TRUE,
    },

    [ABILITY_TERA_SHELL] =
    {
        .name = _("TERA SHELL"),
        .description = COMPOUND_STRING("Resists all types at full HP."),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
        .breakable = TRUE,
    },

    [ABILITY_TERAFORM_ZERO] =
    {
        .name = _("TERAFORM ZERO"),
        .description = COMPOUND_STRING("Zeroes weather and terrain."),
        .aiRating = 10,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
    },

    [ABILITY_POISON_PUPPETEER] =
    {
        .name = _("POISON PUPPETEER"),
        .description = COMPOUND_STRING("Poisoning also confuses."),
        .aiRating = 8,
        .cantBeCopied = TRUE,
        .cantBeSwapped = TRUE,
        .cantBeTraced = TRUE,
    },

    [ABILITY_PIERCING_DRILL] =
    {
        .name = _("PIERCING DRILL"),
        .description = COMPOUND_STRING("Contact evades protection."),
    },

    [ABILITY_DRAGONIZE] =
    {
        .name = _("DRAGONIZE"),
        .description = COMPOUND_STRING("NORMAL moves turn DRAGON."),
    },

    [ABILITY_EELEVATE] =
    {
        .name = _("EELEVATE"),
        .description = COMPOUND_STRING("Levitate and BEAST BOOST."),
    },

    [ABILITY_314] =
    {
        .name = _("-------"),
        .description = COMPOUND_STRING("No special ability."),
    },

    [ABILITY_MEGA_SOL] =
    {
        .name = _("MEGA SOL"),
        .description = COMPOUND_STRING("Acts like under sun."),
    },

    [ABILITY_FIRE_MANE] =
    {
        .name = _("FIRE MANE"),
        .description = COMPOUND_STRING("Ups FIRE-type moves."),
    },

    [ABILITY_317] =
    {
        .name = _("-------"),
        .description = COMPOUND_STRING("No special ability."),
    },

    [ABILITY_SPICY_SPRAY] =
    {
        .name = _("SPICY SPRAY"),
        .description = COMPOUND_STRING("Burns the foe when damaged."),
    },

    [ABILITY_AURA_GUARD] =
    {
        .name = _("AURA GUARD"),
        .description = COMPOUND_STRING("Unimplemented."),
    },
};
