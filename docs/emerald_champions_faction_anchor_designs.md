# Emerald Champions paired Magma and Aqua anchor designs

Progress: 6/6 paired rounds are design-complete; source status is reported per anchor.

## MAGMA_HIDEOUT_FINAL_MAXIE

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: double, cap 60, offsets [1, 1, 2, 2, 3, 4].
- Primary question: Can the player break Maxie's land-support geometry before Flower Gift, Gravity, Telepathy, or Instruct turns Primal Groudon and Great Tusk into repeated board-wide pressure, while preserving a plan for sun-boosted Hydro Steam and Mega Camerupt?
- Strongest part: Gravity, Telepathy, Instruct, Flower Gift, and Hydro Steam all express Maxie's ideology through battle mechanics; this is land control, not just six sun-abusing attackers.
- Weakest link: Primal Groudon plus support can become oppressive before the player sees the full lesson. Cherrim's fragility, conditional Oranguru logic, absent speed mode, and broad Wide Guard, Taunt, weather, Intimidate, burn, and support-removal counterplay must survive implementation.
- First-loss lesson: The land itself is the combo. Remove Cherrim if physical amplification is killing you, remove Oranguru if geometry is, never assume Water is safe into Walking Wake, and arrive at Mega Camerupt with a way to reduce Eruption before it moves.
- References: `showdown:gen8randomdoublesbattle:012`, `vgc:naic-2022`, `vgc:regional-melbourne-2025`, `showdown:gen9randomdoublesbattle:012`, `showdown:gen7randomdoublesbattle:004`, `showdown:gen9championsrandomdoublesbattle:025`
- Team:
  - `SPECIES_GROUDON` +1 — `ITEM_RED_ORB`, `ABILITY_DROUGHT`; Primal; `MOVE_PRECIPICE_BLADES`, `MOVE_HEAT_CRASH`, `MOVE_SWORDS_DANCE`, `MOVE_PROTECT`
  - `SPECIES_CHERRIM` +1 — `ITEM_FOCUS_SASH`, `ABILITY_FLOWER_GIFT`; `MOVE_HELPING_HAND`, `MOVE_SOLAR_BEAM`, `MOVE_WEATHER_BALL`, `MOVE_PROTECT`
  - `SPECIES_ORANGURU` +2 — `ITEM_MENTAL_HERB`, `ABILITY_TELEPATHY`; `MOVE_GRAVITY`, `MOVE_INSTRUCT`, `MOVE_PSYCHIC`, `MOVE_PROTECT`
  - `SPECIES_GREAT_TUSK` +2 — `ITEM_ASSAULT_VEST`, `ABILITY_PROTOSYNTHESIS`; `MOVE_HEADLONG_RUSH`, `MOVE_CLOSE_COMBAT`, `MOVE_ROCK_SLIDE`, `MOVE_KNOCK_OFF`
  - `SPECIES_WALKING_WAKE` +3 — `ITEM_LIFE_ORB`, `ABILITY_PROTOSYNTHESIS`; `MOVE_HYDRO_STEAM`, `MOVE_DRAGON_PULSE`, `MOVE_FLAMETHROWER`, `MOVE_PROTECT`
  - `SPECIES_CAMERUPT` +4 — `ITEM_CAMERUPTITE`, `ABILITY_ANGER_POINT`; Mega; `MOVE_ERUPTION`, `MOVE_EARTH_POWER`, `MOVE_HEAT_WAVE`, `MOVE_PROTECT`

AI must execute: Score Groudon and Cherrim jointly: Helping Hand requires meaningful same-turn damage and Cherrim should attack when support is redundant. Use Gravity only when accuracy or grounding changes a visible target interaction; do not ground Maxie's own vulnerable partner into allied spread damage without superior payoff. Use Instruct only after a legal partner has a valuable repeatable last move and can act; never target Protect, setup, an incapacitated ally, or a move that would self-sabotage the board. Account for Telepathy before choosing allied spread Ground moves and use independent attacks when the intended immune partner is absent. Evaluate Eruption from current HP and trigger Primal Groudon and Mega Camerupt through normal transformation timing.

## SEAFLOOR_CAVERN_FINAL_ARCHIE

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: double, cap 70, offsets [1, 1, 2, 2, 3, 4].
- Primary question: Can the player reduce Primal Kyogre's Water Spout before Tsareena denies priority, then survive an accelerating chain of rain-charged Archaludon, forced Palafin and Urshifu pivots, Protect punishment, and Mega Sharpedo speed?
- Strongest part: Every reserve advances the same tidal verb in a different way—charge, leave, break shelter, accelerate—so the battle feels like Aqua without defaulting to six Swift Swim attackers.
- Weakest link: Kyogre, Palafin, Urshifu, and Mega Sharpedo stack enormous Water physical pressure. Tsareena removal, Water Spout HP dependence, Palafin's required exit, Urshifu's Choice lock, Sharpedo's frailty, and the absence of Tailwind, redirection, and healing must keep the puzzle broad rather than automatic.
- First-loss lesson: This fight accelerates if you let it flow. Cut Kyogre's HP before Water Spout, remove Tsareena before depending on priority, punish Palafin's first exit, exploit Urshifu's lock, and do not give Mega Sharpedo a free protected speed turn.
- References: `showdown:gen9randomdoublesbattle:025`, `elite:wolfe:toronto-2024`, `showdown:gen9randomdoublesbattle:019`, `elite:paul-chua:euic-2026`, `showdown:gen6randomdoublesbattle:004`
- Team:
  - `SPECIES_KYOGRE` +1 — `ITEM_BLUE_ORB`, `ABILITY_DRIZZLE`; Primal; `MOVE_WATER_SPOUT`, `MOVE_ORIGIN_PULSE`, `MOVE_THUNDER`, `MOVE_PROTECT`
  - `SPECIES_TSAREENA` +1 — `ITEM_FOCUS_SASH`, `ABILITY_QUEENLY_MAJESTY`; `MOVE_POWER_WHIP`, `MOVE_TRIPLE_AXEL`, `MOVE_U_TURN`, `MOVE_PROTECT`
  - `SPECIES_ARCHALUDON` +2 — `ITEM_ASSAULT_VEST`, `ABILITY_STAMINA`; `MOVE_ELECTRO_SHOT`, `MOVE_FLASH_CANNON`, `MOVE_DRAGON_PULSE`, `MOVE_BODY_PRESS`
  - `SPECIES_PALAFIN` +2 — `ITEM_LIFE_ORB`, `ABILITY_ZERO_TO_HERO`; `MOVE_FLIP_TURN`, `MOVE_WAVE_CRASH`, `MOVE_ICE_PUNCH`, `MOVE_JET_PUNCH`
  - `SPECIES_URSHIFU_RAPID_STRIKE_STYLE` +3 — `ITEM_CHOICE_BAND`, `ABILITY_UNSEEN_FIST`; `MOVE_SURGING_STRIKES`, `MOVE_CLOSE_COMBAT`, `MOVE_AQUA_JET`, `MOVE_U_TURN`
  - `SPECIES_SHARPEDO` +4 — `ITEM_SHARPEDONITE`, `ABILITY_SPEED_BOOST`; Mega; `MOVE_LIQUIDATION`, `MOVE_CRUNCH`, `MOVE_PSYCHIC_FANGS`, `MOVE_PROTECT`

AI must execute: Evaluate Water Spout from Kyogre's current HP and prefer Origin Pulse, Thunder, or Protect when the visible damage line is stronger. Account for Queenly Majesty when scoring priority from either side without reading hidden player move selection. Give base Palafin one high-priority legal Flip Turn cycle with a valid reserve, then recognize Hero form and stop sacrificing damage to redundant pivots. Score Urshifu's Unseen Fist and Choice lock from public state; U-turn only when the matchup and reserve improve. Use a current-based reserve selector and Mega Evolve Sharpedo normally; Protect for Speed Boost only when survival and next-turn value justify it.

## MAGMA_HIDEOUT_COURTNEY

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: double, cap 60, offsets [1, 1, 2, 2, 3, 4].
- Primary question: Can the player disrupt Courtney's calibrated lead and recognize when Glimmora's Sludge Wave is safe beside Steelix, while exploiting Chi-Yu's Choice commitment before Mega Houndoom solves the final coverage equation?
- Strongest part: Victory Star and Glimmora-Steelix safe-zone geometry make Courtney feel exact and personal rather than a smaller Maxie.
- Weakest link: Four Fire attackers can still read as generic sun. The actual play must emphasize accuracy calibration, Choice commitment, hazards, and ally-safe spread geometry; if runtime AI cannot execute those, the roster must be revised before source closure.
- First-loss lesson: Courtney's danger comes from precision, not randomness. Break the calibrated lead, identify the Choice move, never donate a safe Glimmora-Steelix spread turn, pop the Balloon deliberately, and keep a fast answer for Houndoom.
- References: `showdown:gen5randomdoublesbattle:025`, `elite:giovanni-cischke:worlds-2025`, `showdown:gen9randomdoublesbattle:026`, `elite:wolfe:indianapolis-2026`, `showdown:gen9championsrandomdoublesbattle:011`
- Team:
  - `SPECIES_NINETALES` +1 — `ITEM_HEAT_ROCK`, `ABILITY_DROUGHT`; `MOVE_HEAT_WAVE`, `MOVE_WILL_O_WISP`, `MOVE_ENCORE`, `MOVE_PROTECT`
  - `SPECIES_VICTINI` +1 — `ITEM_ASSAULT_VEST`, `ABILITY_VICTORY_STAR`; `MOVE_V_CREATE`, `MOVE_BLUE_FLARE`, `MOVE_BOLT_STRIKE`, `MOVE_U_TURN`
  - `SPECIES_CHI_YU` +2 — `ITEM_CHOICE_SCARF`, `ABILITY_BEADS_OF_RUIN`; `MOVE_FLAMETHROWER`, `MOVE_OVERHEAT`, `MOVE_DARK_PULSE`, `MOVE_FIRE_BLAST`
  - `SPECIES_GLIMMORA` +2 — `ITEM_FOCUS_SASH`, `ABILITY_TOXIC_DEBRIS`; `MOVE_STEALTH_ROCK`, `MOVE_SLUDGE_WAVE`, `MOVE_POWER_GEM`, `MOVE_PROTECT`
  - `SPECIES_STEELIX` +3 — `ITEM_AIR_BALLOON`, `ABILITY_SHEER_FORCE`; `MOVE_HEAVY_SLAM`, `MOVE_HIGH_HORSEPOWER`, `MOVE_ROCK_SLIDE`, `MOVE_PROTECT`
  - `SPECIES_HOUNDOOM` +4 — `ITEM_HOUNDOOMINITE`, `ABILITY_FLASH_FIRE`; Mega; `MOVE_DARK_PULSE`, `MOVE_HEAT_WAVE`, `MOVE_SOLAR_BEAM`, `MOVE_SLUDGE_BOMB`

AI must execute: Account for Victory Star's real accuracy modifier without treating Will-O-Wisp, Blue Flare, Bolt Strike, or Rock Slide as guaranteed. Score Ninetales and Victini jointly and use U-turn when the visible reserve genuinely improves the board. Respect Chi-Yu's public Choice lock and avoid selecting it into a disclosed immunity or forced failure. Use Sludge Wave only beside Steelix, a protected ally, or a board where ally damage is outweighed by a real knockout; otherwise choose single-target STAB or Protect. Mega Evolve Houndoom normally and use immediate coverage rather than inventing a safe setup turn.

## SEAFLOOR_CAVERN_SHELLY

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: double, cap 70, offsets [1, 1, 2, 2, 3, 4].
- Primary question: Can the player deny snow or Aurora Veil before Shelly's two Slush Rush attackers exploit it, then navigate Freeze-Dry and Choice pivots while preserving weather control for Ice Body Mega Lapras?
- Strongest part: Snow, screens, Slush Rush, Freeze-Dry, Liquid Voice, and Ice Body turn water's phase change into one coherent Aqua-admin puzzle without stealing Archie's rain.
- Weakest link: Two Slush Rush attackers can feel repetitive if their roles collapse into fast physical damage. Sandslash must remain the precise Steel blade, Arctozolt the Bolt Beak order test, and the reserve selector must not deploy them as interchangeable modules.
- First-loss lesson: Shelly's screen and weather are the engine. Deny one before attacking into both, break Bolt Beak's move order, exploit Rotom's lock, and end snow before trying to grind through Mega Lapras.
- References: `showdown:gen9championsrandomdoublesbattle:024`, `showdown:gen8randomdoublesbattle:002`, `showdown:gen6randomdoublesbattle:002`, `showdown:gen8randomdoublesbattle:003`, `showdown:gen7randomdoublesbattle:017`
- Team:
  - `SPECIES_NINETALES_ALOLAN` +1 — `ITEM_LIGHT_CLAY`, `ABILITY_SNOW_WARNING`; `MOVE_AURORA_VEIL`, `MOVE_BLIZZARD`, `MOVE_FREEZE_DRY`, `MOVE_PROTECT`
  - `SPECIES_SANDSLASH_ALOLAN` +1 — `ITEM_LIFE_ORB`, `ABILITY_SLUSH_RUSH`; `MOVE_ICICLE_CRASH`, `MOVE_IRON_HEAD`, `MOVE_DRILL_RUN`, `MOVE_PROTECT`
  - `SPECIES_ARCTOZOLT` +2 — `ITEM_ASSAULT_VEST`, `ABILITY_SLUSH_RUSH`; `MOVE_BOLT_BEAK`, `MOVE_ICICLE_CRASH`, `MOVE_STOMPING_TANTRUM`, `MOVE_ROCK_SLIDE`
  - `SPECIES_ROTOM_FROST` +2 — `ITEM_CHOICE_SCARF`, `ABILITY_LEVITATE`; `MOVE_FREEZE_DRY`, `MOVE_THUNDERBOLT`, `MOVE_VOLT_SWITCH`, `MOVE_TRICK`
  - `SPECIES_PRIMARINA` +3 — `ITEM_THROAT_SPRAY`, `ABILITY_LIQUID_VOICE`; `MOVE_HYPER_VOICE`, `MOVE_MOONBLAST`, `MOVE_ICE_BEAM`, `MOVE_PROTECT`
  - `SPECIES_LAPRAS` +4 — `ITEM_LAPRASITE`, `ABILITY_WATER_ABSORB`; Mega; `MOVE_FREEZE_DRY`, `MOVE_HYDRO_PUMP`, `MOVE_THUNDER`, `MOVE_PROTECT`

AI must execute: Score Ninetales and Sandslash jointly: Aurora Veil requires active snow and meaningful remaining duration; Ninetales attacks when the screen is active or denial is likely. Evaluate Slush Rush and Bolt Beak from actual move order rather than assuming snow guarantees maximum power. Treat Rotom-Frost's Freeze-Dry as the local appliance form move, respect Choice lock, and use Trick or Volt Switch only when the visible board improves. Recognize Liquid Voice spread targeting and one Throat Spray activation without farming support turns. Mega Evolve Lapras normally and value Ice Body only while snow remains; do not Protect-loop for passive healing.

## MAGMA_HIDEOUT_TABITHA

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: double, cap 60, offsets [1, 1, 2, 2, 3, 4].
- Primary question: Can the player interrupt or survive one legal Inteleon-to-Coalossal self-activation, then distinguish Gastrodon's hydraulic defense from Stakataka's spread shield before Mega Excadrill drills through the remaining answers?
- Strongest part: The famous self-activation core is not merely copied; every reserve is another visible component of one Magma machine, and the combo remains finite and targetable.
- Weakest link: Coalossal activation can dominate the entire fight if the AI forces it regardless of board state. The single-use predicates and direct-attack fallbacks are mandatory, not polish.
- First-loss lesson: Treat Surf as an ignition wire. Cut it, clear the boosts, or survive it once—then stop feeding Gastrodon, vary spread and single-target pressure around Stakataka, and keep physical control for the drill head.
- References: `elite:wolfe:players-cup-ii-2020`, `showdown:gen8randomdoublesbattle:005`, `showdown:gen7randomdoublesbattle:020`, `showdown:gen8randomdoublesbattle:011`, `showdown:gen9championsrandomdoublesbattle:019`
- Team:
  - `SPECIES_INTELEON` +1 — `ITEM_FOCUS_SASH`, `ABILITY_SNIPER`; `MOVE_SURF`, `MOVE_SNIPE_SHOT`, `MOVE_ICE_BEAM`, `MOVE_PROTECT`
  - `SPECIES_COALOSSAL` +1 — `ITEM_WEAKNESS_POLICY`, `ABILITY_STEAM_ENGINE`; `MOVE_HEAT_WAVE`, `MOVE_POWER_GEM`, `MOVE_BODY_PRESS`, `MOVE_PROTECT`
  - `SPECIES_GASTRODON` +2 — `ITEM_LEFTOVERS`, `ABILITY_STORM_DRAIN`; `MOVE_EARTH_POWER`, `MOVE_ICE_BEAM`, `MOVE_CLEAR_SMOG`, `MOVE_PROTECT`
  - `SPECIES_STAKATAKA` +2 — `ITEM_MENTAL_HERB`, `ABILITY_BEAST_BOOST`; `MOVE_WIDE_GUARD`, `MOVE_ROCK_SLIDE`, `MOVE_GYRO_BALL`, `MOVE_BODY_PRESS`
  - `SPECIES_DARMANITAN` +3 — `ITEM_LIFE_ORB`, `ABILITY_SHEER_FORCE`; `MOVE_FLARE_BLITZ`, `MOVE_ZEN_HEADBUTT`, `MOVE_ROCK_SLIDE`, `MOVE_SUPERPOWER`
  - `SPECIES_EXCADRILL` +4 — `ITEM_EXCADRITE`, `ABILITY_MOLD_BREAKER`; Mega; `MOVE_HIGH_HORSEPOWER`, `MOVE_IRON_HEAD`, `MOVE_ROCK_SLIDE`, `MOVE_PROTECT`

AI must execute: Use Inteleon Surf only when Coalossal is unactivated and survives the allied hit with meaningful same-turn or next-turn value, or when Gastrodon gains meaningful Storm Drain value. Stop self-activating after Steam Engine or Weakness Policy has fired and prefer direct attacks, burn, or Protect when Surf becomes wasteful. Account for Storm Drain and allied spread targeting without reading hidden player choices. Use Wide Guard only against disclosed spread pressure and let Stakataka attack otherwise. Mega Evolve Excadrill normally and use board-correct direct pressure rather than waiting for sand or setup that does not exist.

## AQUA_HIDEOUT_MATT

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: double, cap 60, offsets [1, 1, 2, 2, 3, 4].
- Primary question: Can the player escape Dhelmise's anchor and contest Pelipper's Tailwind/Wide Guard while distinguishing two Dry Skin sustain threats, then exploit Seismitoad's Choice lock before Mega Gyarados finds a Dragon Dance?
- Strongest part: Matt's existing roster becomes one vivid boarding party: storm, anchor, wet-deck fighters, deckbreaker, captain. It is factional without repeating either Aqua strategist.
- Weakest link: Dry Skin plus rain can become tedious if the AI stalls. Neither user has Protect or a recovery move, and the AI must keep attacking; that constraint is essential.
- First-loss lesson: Matt wins by holding you on a wet deck. Break Pelipper or vary your targeting around Wide Guard, escape the anchor, choose the correct Dry Skin target, exploit Seismitoad's commitment, and never donate a free dance to Gyarados.
- References: `vgc:regional-baltimore-2025`, `showdown:gen8randomdoublesbattle:029`, `showdown:gen9championsrandomdoublesbattle:016`, `showdown:gen5randomdoublesbattle:018`, `showdown:gen9championsrandomdoublesbattle:001`
- Team:
  - `SPECIES_PELIPPER` +1 — `ITEM_DAMP_ROCK`, `ABILITY_DRIZZLE`; `MOVE_TAILWIND`, `MOVE_HURRICANE`, `MOVE_WEATHER_BALL`, `MOVE_WIDE_GUARD`
  - `SPECIES_DHELMISE` +1 — `ITEM_SPELL_TAG`, `ABILITY_STEELWORKER`; `MOVE_ANCHOR_SHOT`, `MOVE_POWER_WHIP`, `MOVE_POLTERGEIST`, `MOVE_ROCK_SLIDE`
  - `SPECIES_HELIOLISK` +2 — `ITEM_LIFE_ORB`, `ABILITY_DRY_SKIN`; `MOVE_THUNDER`, `MOVE_WEATHER_BALL`, `MOVE_GRASS_KNOT`, `MOVE_PROTECT`
  - `SPECIES_TOXICROAK` +2 — `ITEM_BLACK_SLUDGE`, `ABILITY_DRY_SKIN`; `MOVE_FAKE_OUT`, `MOVE_DRAIN_PUNCH`, `MOVE_GUNK_SHOT`, `MOVE_SUCKER_PUNCH`
  - `SPECIES_SEISMITOAD` +3 — `ITEM_CHOICE_BAND`, `ABILITY_SWIFT_SWIM`; `MOVE_LIQUIDATION`, `MOVE_DRAIN_PUNCH`, `MOVE_POWER_WHIP`, `MOVE_ICE_PUNCH`
  - `SPECIES_GYARADOS` +4 — `ITEM_GYARADOSITE`, `ABILITY_INTIMIDATE`; Mega; `MOVE_DRAGON_DANCE`, `MOVE_WATERFALL`, `MOVE_CRUNCH`, `MOVE_POWER_WHIP`

AI must execute: Score Pelipper's Tailwind and Wide Guard from actual speed and disclosed spread pressure; attack when support is redundant. Treat Anchor Shot as an ordinary trap with legal switches and fainting behavior, not a permanent scripted lock. Recognize Dry Skin healing and Fire vulnerability under active weather without stalling solely for recovery. Respect Seismitoad's Choice lock and choose an ally-safe move for the current partner. Use Dragon Dance only when Mega Gyarados survives the visible turn and the boost improves a real next-turn line; otherwise attack immediately.

## MT_CHIMNEY_MAXIE

- Status: design `design-complete`, source `source-closed`, runtime `unplayed`.
- Format/cap: double, cap 40, offsets [1, 1, 2, 2, 3, 5].
- Primary question: Can the player contest Crobat's ridge control, manage base Groudon's spread pressure, and sequence around one physical Fake Out plus special Encore/Helping Hand before preserving a safe formation for Mega Flygon?
- Strongest part: The battle introduces Groudon without spending Primal and turns the summit itself—air lane, tempo, and safe Earthquake positioning—into the puzzle.
- Weakest link: Sun plus Tailwind plus Fake Out can feel familiar. One Fake Out, special Encore support, Stone Edge substitutions, no sleep, and the level-legal positioning ace keep the parts distinct.
- First-loss lesson: This is a fight for the ridge. Break Crobat's control, distinguish physical Fake Out from special Encore support, remember Groudon is not Primal, and enter Flygon with positioning intact.
- References: `vgc:naic-2022`, `showdown:gen4randomdoublesbattle:007`, `showdown:gen9championsrandomdoublesbattle:003`, `vgc:regional-portland-2024`, `showdown:gen5randomdoublesbattle:002`
- Team:
  - `SPECIES_GROUDON` +1 — `ITEM_HEAT_ROCK`, `ABILITY_DROUGHT`; `MOVE_PRECIPICE_BLADES`, `MOVE_FIRE_PUNCH`, `MOVE_STONE_EDGE`, `MOVE_PROTECT`
  - `SPECIES_CROBAT` +1 — `ITEM_SITRUS_BERRY`, `ABILITY_INFILTRATOR`; `MOVE_TAILWIND`, `MOVE_SUPER_FANG`, `MOVE_TAUNT`, `MOVE_QUICK_GUARD`
  - `SPECIES_SHIFTRY` +2 — `ITEM_FOCUS_SASH`, `ABILITY_CHLOROPHYLL`; `MOVE_FAKE_OUT`, `MOVE_LEAF_BLADE`, `MOVE_KNOCK_OFF`, `MOVE_PROTECT`
  - `SPECIES_SALAZZLE` +2 — `ITEM_LIFE_ORB`, `ABILITY_CORROSION`; `MOVE_HEAT_WAVE`, `MOVE_SLUDGE_BOMB`, `MOVE_ENCORE`, `MOVE_HELPING_HAND`
  - `SPECIES_ENTEI` +3 — `ITEM_ASSAULT_VEST`, `ABILITY_INNER_FOCUS`; `MOVE_SACRED_FIRE`, `MOVE_EXTREME_SPEED`, `MOVE_STOMPING_TANTRUM`, `MOVE_SNARL`
  - `SPECIES_FLYGON` +5 — `ITEM_FLYGONITE`, `ABILITY_LEVITATE`; Mega; `MOVE_EARTHQUAKE`, `MOVE_DRAGON_CLAW`, `MOVE_STONE_EDGE`, `MOVE_PROTECT`

AI must execute: Score Crobat's Tailwind, Taunt, Quick Guard, and Super Fang from actual board state. Use Shiftry Fake Out only with visible tempo and Salazzle Encore/Helping Hand from the partner's selected action. Use Groudon spread pressure without assuming Primal weather or Gravity. Use Mega Flygon Earthquake only beside Crobat, a protected ally, or a board where payoff outweighs ally damage. Mega Evolve Flygon normally and keep Groudon base form throughout.

## MT_PYRE_MATT

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: double, cap 60, offsets [1, 1, 2, 2, 3, 4].
- Primary question: Can the player escape Dhelmise's grave anchor while controlling Politoed's speed and Helping Hand, then withstand Hoopa's shelter-breaking pressure and damage Jellicent before its Water Spout, burn, and Mega Feraligatr endgame compound?
- Strongest part: The map matters: three spirits, cold rain, and an old anchor turn Matt into a Mt. Pyre boss rather than an early copy of his hideout team.
- Weakest link: Hoopa is extraordinarily strong and can feel arbitrary. Its map fit, Sash dependence, confined form, absent setup, Focus Blast variance, and broad Dark/Ghost counterplay must be tested honestly.
- First-loss lesson: Mt. Pyre punishes passive shelter and slow damage. Break the conductor or anchor, do not expect Protect to solve Hoopa, cut Jellicent's HP early, and never hand Feraligatr a free dance.
- References: `vgc:korean-spring-league-2018`, `showdown:gen8randomdoublesbattle:002`, `showdown:gen9randomdoublesbattle:008`, `vgc:regional-dallas-tx-2020`, `showdown:gen9championsrandomdoublesbattle:023`
- Team:
  - `SPECIES_POLITOED` +1 — `ITEM_DAMP_ROCK`, `ABILITY_DRIZZLE`; `MOVE_ICY_WIND`, `MOVE_HELPING_HAND`, `MOVE_WEATHER_BALL`, `MOVE_PROTECT`
  - `SPECIES_DHELMISE` +1 — `ITEM_ASSAULT_VEST`, `ABILITY_STEELWORKER`; `MOVE_ANCHOR_SHOT`, `MOVE_POWER_WHIP`, `MOVE_POLTERGEIST`, `MOVE_PROTECT`
  - `SPECIES_HOOPA` +2 — `ITEM_FOCUS_SASH`, `ABILITY_MAGICIAN`; `MOVE_HYPERSPACE_HOLE`, `MOVE_SHADOW_BALL`, `MOVE_FOCUS_BLAST`, `MOVE_PROTECT`
  - `SPECIES_JELLICENT` +2 — `ITEM_SITRUS_BERRY`, `ABILITY_WATER_ABSORB`; `MOVE_WATER_SPOUT`, `MOVE_SHADOW_BALL`, `MOVE_WILL_O_WISP`, `MOVE_PROTECT`
  - `SPECIES_CRAMORANT` +3 — `ITEM_LIFE_ORB`, `ABILITY_GULP_MISSILE`; `MOVE_HYDRO_PUMP`, `MOVE_AIR_SLASH`, `MOVE_ICY_WIND`, `MOVE_PROTECT`
  - `SPECIES_FERALIGATR` +4 — `ITEM_FERALIGITE`, `ABILITY_STRONG_JAW`; Mega; `MOVE_DRAGON_DANCE`, `MOVE_WATERFALL`, `MOVE_CRUNCH`, `MOVE_ICE_PUNCH`

AI must execute: Score Icy Wind, Helping Hand, Weather Ball, and Protect from actual speed, damage, and survival rather than fixed support turns. Treat Anchor Shot as ordinary visible trapping and never as Shadow Tag or a hidden-switch prediction. Use Hyperspace Hole through ordinary move rules and score Focus Blast with its real accuracy rather than as guaranteed coverage. Evaluate Jellicent Water Spout from current HP and switch to Shadow Ball, burn, or Protect when stronger. Use Dragon Dance only when Mega Feraligatr survives and the boost improves a real next-turn line; otherwise attack immediately.

## MT_CHIMNEY_TABITHA

- Status: design `design-complete`, source `source-closed`, runtime `unplayed`.
- Format/cap: double, cap 40, offsets [1, 1, 2, 2, 3, 4].
- Primary question: Can the player stop Klinklang's one Shift Gear, exploit Xurkitree's Choice lock, and contain Electivire's coverage before Rhyperior and Mega Machamp turn the prototype into direct force?
- Strongest part: The same Coalossal tells a clean progression story, while fresh Electivire/Xurkitree avoid recent and protected collisions.
- Weakest link: The machine can still read as six strong parts. Shift Gear, Motor Drive coverage, Choice commitment, Beast Boost, chassis bulk, and direct Mega coverage must remain distinct in AI and guide text.
- First-loss lesson: This is an unfinished machine. Stop the one gear boost, force the power supply into a bad lock, contain Electivire's coverage, crack the chassis specially, and preserve a clean answer for Machamp.
- References: `showdown:gen8randomdoublesbattle:005`, `showdown:gen6randomdoublesbattle:024`, `showdown:gen6randombattle:025`, `showdown:gen8randombattle:014`, `showdown:gen9championsrandomdoublesbattle:015`, `showdown:gen7randomdoublesbattle:012`
- Team:
  - `SPECIES_COALOSSAL` +1 — `ITEM_AIR_BALLOON`, `ABILITY_STEAM_ENGINE`; `MOVE_HEAT_WAVE`, `MOVE_POWER_GEM`, `MOVE_BODY_PRESS`, `MOVE_PROTECT`
  - `SPECIES_KLINKLANG` +1 — `ITEM_WHITE_HERB`, `ABILITY_CLEAR_BODY`; `MOVE_GEAR_GRIND`, `MOVE_WILD_CHARGE`, `MOVE_SHIFT_GEAR`, `MOVE_PROTECT`
  - `SPECIES_ELECTIVIRE` +2 — `ITEM_EXPERT_BELT`, `ABILITY_MOTOR_DRIVE`; `MOVE_WILD_CHARGE`, `MOVE_ICE_PUNCH`, `MOVE_FIRE_PUNCH`, `MOVE_CROSS_CHOP`
  - `SPECIES_XURKITREE` +2 — `ITEM_CHOICE_SPECS`, `ABILITY_BEAST_BOOST`; `MOVE_THUNDERBOLT`, `MOVE_VOLT_SWITCH`, `MOVE_DAZZLING_GLEAM`, `MOVE_ENERGY_BALL`
  - `SPECIES_RHYPERIOR` +3 — `ITEM_ASSAULT_VEST`, `ABILITY_SOLID_ROCK`; `MOVE_HIGH_HORSEPOWER`, `MOVE_STONE_EDGE`, `MOVE_ICE_PUNCH`, `MOVE_FIRE_PUNCH`
  - `SPECIES_MACHAMP` +4 — `ITEM_MACHAMPITE`, `ABILITY_GUTS`; Mega; `MOVE_CLOSE_COMBAT`, `MOVE_ICE_PUNCH`, `MOVE_THUNDER_PUNCH`, `MOVE_BULLET_PUNCH`

AI must execute: Use Shift Gear only when Klinklang survives and the boost creates a real next-turn line. Respect Xurkitree's Choice lock and use Volt Switch only when a reserve improves the board. Score Electivire's Motor Drive immunity, recoil, and four coverage types from the visible matchup. Account for Coalossal's Air Balloon without manufacturing an allied Water activation. Mega Evolve Machamp normally and choose direct coverage without confusion dependence.

## WEATHER_INSTITUTE_SHELLY

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: double, cap 55, offsets [1, 1, 2, 2, 3, 4].
- Primary question: Can the player deny Castform's vulnerable Rain Dance through Ludicolo's Fake Out, then adapt as five different specimens turn the same forecast into speed, accuracy, hydration, Fire shielding, and Mega Water pressure?
- Strongest part: Castform makes the map and research premise mechanically real, while every reserve demonstrates a different weather consequence rather than another generic rain sweeper.
- Weakest link: Manual Rain Dance can make the boss collapse too easily if Castform is removed. That is intentional counterplay, but Hard offsets and every reserve's functional non-rain line must keep the fight above the game's floor.
- First-loss lesson: The entire lab starts with one forecast. Deny or outlast it if you can; if rain begins, identify which single benefit each specimen is using rather than treating them as interchangeable Water attackers.
- References: `showdown:gen4randomdoublesbattle:003`, `vgc:regional-baltimore-2025`, `vgc:regional-portland-2024`, `vgc:regional-dallas-tx-2020`, `showdown:gen7randomdoublesbattle:014`
- Team:
  - `SPECIES_CASTFORM` +1 — `ITEM_DAMP_ROCK`, `ABILITY_FORECAST`; `MOVE_RAIN_DANCE`, `MOVE_WEATHER_BALL`, `MOVE_THUNDER`, `MOVE_PROTECT`
  - `SPECIES_LUDICOLO` +1 — `ITEM_FOCUS_SASH`, `ABILITY_SWIFT_SWIM`; `MOVE_FAKE_OUT`, `MOVE_HYDRO_PUMP`, `MOVE_GIGA_DRAIN`, `MOVE_ICE_BEAM`
  - `SPECIES_RAIKOU` +2 — `ITEM_ASSAULT_VEST`, `ABILITY_INNER_FOCUS`; `MOVE_THUNDER`, `MOVE_AURA_SPHERE`, `MOVE_SNARL`, `MOVE_VOLT_SWITCH`
  - `SPECIES_GOODRA` +2 — `ITEM_LIFE_ORB`, `ABILITY_HYDRATION`; `MOVE_MUDDY_WATER`, `MOVE_DRAGON_PULSE`, `MOVE_THUNDER`, `MOVE_PROTECT`
  - `SPECIES_SCIZOR` +3 — `ITEM_EXPERT_BELT`, `ABILITY_TECHNICIAN`; `MOVE_BULLET_PUNCH`, `MOVE_U_TURN`, `MOVE_SUPERPOWER`, `MOVE_PROTECT`
  - `SPECIES_KINGLER` +4 — `ITEM_KINGLERITE`, `ABILITY_HYPER_CUTTER`; Mega; `MOVE_CRABHAMMER`, `MOVE_HIGH_HORSEPOWER`, `MOVE_X_SCISSOR`, `MOVE_PROTECT`

AI must execute: Use Rain Dance only when rain is absent, Castform survives the visible turn, and at least one active or reserve specimen gains meaningful value. Score Ludicolo Fake Out jointly with Castform and attack when weather is already active or Fake Out is invalid. Recompute Weather Ball type, Thunder accuracy, Swift Swim speed, Hydration, and Fire damage from actual weather each turn. Use Volt Switch and U-turn only when the reserve improves the board, and respect all Choice or item state. Mega Evolve Kingler normally and choose immediate coverage rather than inventing setup.

## METEOR_FALLS_COURTNEY

- Status: design `design-complete`, source `source-closed`, runtime `unplayed`.
- Format/cap: multi_2_vs_2, cap 40, offsets [1, 1, 2, 2, 3, 4].
- Primary question: Can the player read and punish one conditional Solrock detonation, stop Minior's shell break, exploit Jirachi's Choice lock, and preserve enough Rock/Steel/Water/Electric pressure for Celesteela and Mega Aerodactyl?
- Strongest part: Every slot narrates a different stage of a meteor impact, making Meteor Falls mechanically relevant without borrowing Mossdeep's cosmic battle system.
- Weakest link: Explosion can feel cheap even when telegraphed. The low-HP/protected-partner predicate, broad Ghost/Wide Guard/Protect counterplay, and absence of a second detonation are mandatory.
- First-loss lesson: Courtney's impact is timed, not random. Deny or survive the one detonation, stop Minior before it cracks open, force Jirachi into the wrong trajectory, and arrive at Aerodactyl with priority or speed intact.
- References: `showdown:gen5randomdoublesbattle:021`, `showdown:gen8randomdoublesbattle:014`, `showdown:gen9championsrandomdoublesbattle:005`
- Team:
  - `SPECIES_SOLROCK` +1 — `ITEM_FOCUS_SASH`, `ABILITY_LEVITATE`; `MOVE_ROCK_SLIDE`, `MOVE_ZEN_HEADBUTT`, `MOVE_WILL_O_WISP`, `MOVE_EXPLOSION`
  - `SPECIES_LUNATONE` +1 — `ITEM_LIFE_ORB`, `ABILITY_LEVITATE`; `MOVE_POWER_GEM`, `MOVE_PSYCHIC`, `MOVE_ICY_WIND`, `MOVE_PROTECT`
  - `SPECIES_MINIOR` +2 — `ITEM_WHITE_HERB`, `ABILITY_SHIELDS_DOWN`; `MOVE_SHELL_SMASH`, `MOVE_ACROBATICS`, `MOVE_ROCK_SLIDE`, `MOVE_PROTECT`
  - `SPECIES_JIRACHI` +2 — `ITEM_CHOICE_SCARF`, `ABILITY_SERENE_GRACE`; `MOVE_IRON_HEAD`, `MOVE_ICY_WIND`, `MOVE_U_TURN`, `MOVE_TRICK`
  - `SPECIES_CELESTEELA` +3 — `ITEM_ASSAULT_VEST`, `ABILITY_BEAST_BOOST`; `MOVE_HEAVY_SLAM`, `MOVE_FLAMETHROWER`, `MOVE_GIGA_DRAIN`, `MOVE_ROCK_SLIDE`
  - `SPECIES_AERODACTYL` +4 — `ITEM_AERODACTYLITE`, `ABILITY_ROCK_HEAD`; Mega; `MOVE_ROCK_SLIDE`, `MOVE_DUAL_WINGBEAT`, `MOVE_ICE_FANG`, `MOVE_PROTECT`

AI must execute: Use Explosion only when Solrock is low enough or the damage wins a real board, and only when Lunatone or the active ally is protected, immune, or expendable for superior payoff. Use Lunatone Protect in coordination with a justified detonation, but attack or control speed when Explosion is wrong. Use Shell Smash only when Minior survives the visible turn and the boost improves a real next-turn line. Respect Jirachi's Choice lock and use Trick or U-turn only when the visible board improves. Mega Evolve Aerodactyl normally and use immediate coverage rather than inventing setup.

## SLATEPORT_ARCHIE

- Status: design `design-complete`, source `source-closed`, runtime `unplayed`.
- Format/cap: double, cap 30, offsets [1, 1, 2, 2, 3, 4].
- Primary question: Can the player deny Prankster manual rain through layered Fake Out, Wide Guard, Feint, and Encore, then survive Manaphy and Kingdra while preventing Intimidate control from feeding Mega Malamar's Contrary Superpower endgame?
- Strongest part: Removing premature Primal Kyogre fixes the story and actually makes Archie more impressive: the first battle is won through a tactical crew, while the final battle earns the legendary flood.
- Weakest link: Manual rain also appears later at the Weather Institute. The support-dense heist, early cap, mythical prize, and Contrary mastermind must keep this encounter tactically distinct from Shelly's specimen experiment.
- First-loss lesson: This is a planned robbery, not a legendary flood. Break the coordinator, read support from the board instead of guessing, outlast the finite getaway rain, and do not feed Contrary before focusing Malamar.
- References: `showdown:gen6randomdoublesbattle:018`, `elite:wolfe:worlds-2016`, `showdown:gen6randomdoublesbattle:009`, `elite:wolfe:toronto-2024`, `showdown:gen9championsrandomdoublesbattle:011`, `showdown:gen9championsrandomdoublesbattle:002`
- Team:
  - `SPECIES_LIEPARD` +1 — `ITEM_DAMP_ROCK`, `ABILITY_PRANKSTER`; `MOVE_RAIN_DANCE`, `MOVE_FAKE_OUT`, `MOVE_ENCORE`, `MOVE_FOUL_PLAY`
  - `SPECIES_HITMONTOP` +1 — `ITEM_EJECT_BUTTON`, `ABILITY_INTIMIDATE`; `MOVE_FAKE_OUT`, `MOVE_WIDE_GUARD`, `MOVE_FEINT`, `MOVE_CLOSE_COMBAT`
  - `SPECIES_MANAPHY` +2 — `ITEM_LIFE_ORB`, `ABILITY_HYDRATION`; `MOVE_SCALD`, `MOVE_ICE_BEAM`, `MOVE_ENERGY_BALL`, `MOVE_PROTECT`
  - `SPECIES_KINGDRA` +2 — `ITEM_MYSTIC_WATER`, `ABILITY_SWIFT_SWIM`; `MOVE_SCALD`, `MOVE_DRACO_METEOR`, `MOVE_ICY_WIND`, `MOVE_PROTECT`
  - `SPECIES_QWILFISH` +3 — `ITEM_BLACK_SLUDGE`, `ABILITY_INTIMIDATE`; `MOVE_WATERFALL`, `MOVE_POISON_JAB`, `MOVE_THUNDER_WAVE`, `MOVE_PROTECT`
  - `SPECIES_MALAMAR` +4 — `ITEM_MALAMARITE`, `ABILITY_CONTRARY`; Mega; `MOVE_SUPERPOWER`, `MOVE_PSYCHO_CUT`, `MOVE_KNOCK_OFF`, `MOVE_PROTECT`

AI must execute: Score Liepard and Hitmontop actions jointly: Rain Dance, either Fake Out, Wide Guard, Feint, and Encore require visible value and may not stack redundant support. Use Rain Dance only when rain is absent, Liepard survives, and active or reserve attackers gain meaningful value. Resolve Eject Button and reserve selection normally, with a legal fallback when Hitmontop leaves earlier than expected. Respect Draco Meteor drops, Icy Wind speed value, Thunder Wave status, both Intimidates, and Contrary without reading hidden player choices. Mega Evolve Malamar normally and avoid Protect or Superpower loops when direct coverage or survival says otherwise.

## Pair review

- Distinct species: 69 across 72 slots; unwaived faction collisions: 0; unwaived protected-anchor collisions: 0.
- Intentional recurring faction signatures: 3.
- Intentional protected reprises: 1 (Archie's Primal Kyogre versus Wallace's later base Kyogre).
- Judgment: Maxie controls stable geometry and repeated land pressure; Archie creates unstable momentum and forced handoffs. Weather is shared fiction, not a shared puzzle.

## Backward faction board complete

Next: rival milestones, Steven, rematches, and superbosses, followed by the campaign-wide anchor collision review.
