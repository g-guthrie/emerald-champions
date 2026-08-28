# Emerald Champions backward Gym anchor designs

Progress: 6/6 design-complete; Flannery source-closed and five later Gym anchors protected.

## SOOTOPOLIS_GYM_JUAN

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: double, cap 70, offsets [1, 1, 2, 2, 3, 4].
- Primary question: Can the player disrupt Juan's Surf-safe pairings and changing speed before ally Water Absorb or Mega Tatsugiri's Storm Drain converts spread pressure into recovery or a sweep?
- Strongest part: The same visible Surf can be ordinary spread pressure, heal Volcanion, or boost Mega Tatsugiri, making Juan feel like a conductor without stealing rain or Perish from later bosses.
- Weakest link: The identity depends on joint action and reserve scoring that the current generic AI does not fully provide; if runtime pairing is poor, six individually excellent Water sets could look less coordinated than the dossier promises.
- First-loss lesson: Do not treat six Water Pokemon as one matchup. Break the lead's speed control, identify whether Surf will damage, heal, or boost the partner, preserve a physical answer for Araquanid, and deny the surviving Surf user before Mega Tatsugiri takes the baton.
- References: `showdown:gen6randomdoublesbattle:003`, `showdown:gen8randomdoublesbattle:020`, `showdown:gen9championsrandomdoublesbattle:011`, `vgc:worlds-2009`, `vgc:regional-hong-kong-2017`, `vgc:regional-merida-2025`
- Team:
  - `SPECIES_SUICUNE` +1 — `ITEM_MENTAL_HERB`, `ABILITY_INNER_FOCUS`; `MOVE_TAILWIND`, `MOVE_SURF`, `MOVE_SNARL`, `MOVE_PROTECT`
  - `SPECIES_EMPOLEON` +1 — `ITEM_SHUCA_BERRY`, `ABILITY_COMPETITIVE`; `MOVE_SURF`, `MOVE_FLASH_CANNON`, `MOVE_ICY_WIND`, `MOVE_PROTECT`
  - `SPECIES_VOLCANION` +2 — `ITEM_ASSAULT_VEST`, `ABILITY_WATER_ABSORB`; `MOVE_STEAM_ERUPTION`, `MOVE_HEAT_WAVE`, `MOVE_EARTH_POWER`, `MOVE_SLUDGE_BOMB`
  - `SPECIES_ARAQUANID` +2 — `ITEM_LIFE_ORB`, `ABILITY_WATER_BUBBLE`; `MOVE_LIQUIDATION`, `MOVE_LEECH_LIFE`, `MOVE_WIDE_GUARD`, `MOVE_PROTECT`
  - `SPECIES_TAPU_FINI` +3 — `ITEM_CHOICE_SCARF`, `ABILITY_MISTY_SURGE`; `MOVE_MUDDY_WATER`, `MOVE_MOONBLAST`, `MOVE_NATURES_MADNESS`, `MOVE_TRICK`
  - `SPECIES_TATSUGIRI_DROOPY` +4 — `ITEM_TATSUGIRINITE`, `ABILITY_STORM_DRAIN`; Mega; `MOVE_MUDDY_WATER`, `MOVE_DRAGON_PULSE`, `MOVE_ICY_WIND`, `MOVE_PROTECT`

AI must execute: Extend ally Surf scoring to Storm Drain as well as Water Absorb, while rejecting collateral into non-absorbent partners unless the visible knockout and survival value is superior. Score Suicune and Empoleon lead actions jointly so they do not redundantly set speed or use Surf into each other without a compelling visible result. Use a Juan-specific board-state reserve selector that values absorbent Surf partners, Araquanid's physical and Wide Guard role, Tapu Fini's visible Trick target, and the final Mega without forcing scripted pairs. Mega Evolve Tatsugiri when active unless a source-legal form or state prevents it; value an ally Surf boost only when Tatsugiri survives the predicted opposing actions. Use Tapu Fini Trick only against a visible high-value lock target and do not repeatedly choose Icy Wind or Tailwind when the current speed state is already favorable.

## MOSSDEEP_GYM_TATE_AND_LIZA

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: double, cap 60, offsets [1, 1, 2, 2, 3, 4].
- Primary question: Can the player break Psychic Terrain offense, deny the one justified Trick Room reversal, and still preserve Dark or Ghost pressure to crack the final Solgaleo-Lunala eclipse?
- Strongest part: The battle tells one escalating twin story—fast shared thought, reversed time, then a sun-moon eclipse—while the final pair's common Dark and Ghost weakness rewards preservation rather than a secret answer.
- Weakest link: Three formations can feel over-authored if reserve selection forces them regardless of board state; the design succeeds only if every pair is preferred but never illegally scripted and Cresselia declines Trick Room when fast mode is winning.
- First-loss lesson: Do not spend the Dark or Ghost answer merely surviving the Terrain lead. Remove or outlast Mega Alakazam, deny Cresselia's one reversal, then break Shadow Shield and attack the final sun-moon pair through their shared weakness before their mixed pressure separates your defenses.
- References: `elite:wolfe:worlds-2025`, `elite:federico-camporesi:naic-2025`, `elite:shoma-honami:worlds-2015`, `elite:ray-rizzo:worlds-2012`, `showdown:gen7randomdoublesbattle:026`, `showdown:gen9randomdoublesbattle:016`, `vgc:naic-2018`
- Team:
  - `SPECIES_TAPU_LELE` +1 — `ITEM_TERRAIN_EXTENDER`, `ABILITY_PSYCHIC_SURGE`; `MOVE_PSYCHIC`, `MOVE_MOONBLAST`, `MOVE_DAZZLING_GLEAM`, `MOVE_PROTECT`
  - `SPECIES_ALAKAZAM` +1 — `ITEM_ALAKAZITE`, `ABILITY_MAGIC_GUARD`; Mega; `MOVE_EXPANDING_FORCE`, `MOVE_DAZZLING_GLEAM`, `MOVE_ENCORE`, `MOVE_PROTECT`
  - `SPECIES_CRESSELIA` +2 — `ITEM_MENTAL_HERB`, `ABILITY_LEVITATE`; `MOVE_TRICK_ROOM`, `MOVE_HELPING_HAND`, `MOVE_ICE_BEAM`, `MOVE_MOONLIGHT`
  - `SPECIES_CALYREX_ICE_RIDER` +2 — `ITEM_LIFE_ORB`, `ABILITY_AS_ONE_ICE_RIDER`; `MOVE_GLACIAL_LANCE`, `MOVE_STOMPING_TANTRUM`, `MOVE_ZEN_HEADBUTT`, `MOVE_PROTECT`
  - `SPECIES_SOLGALEO` +3 — `ITEM_WEAKNESS_POLICY`, `ABILITY_FULL_METAL_BODY`; `MOVE_SUNSTEEL_STRIKE`, `MOVE_PSYCHIC_FANGS`, `MOVE_ROCK_SLIDE`, `MOVE_PROTECT`
  - `SPECIES_LUNALA` +4 — `ITEM_POWER_HERB`, `ABILITY_SHADOW_SHIELD`; `MOVE_MOONGEIST_BEAM`, `MOVE_METEOR_BEAM`, `MOVE_DAZZLING_GLEAM`, `MOVE_WIDE_GUARD`

AI must execute: Score Tapu Lele and Mega Alakazam actions jointly so Terrain Expanding Force, Fairy coverage, Encore, and Protect attack distinct visible plans instead of redundantly targeting one slot without knockout value. Use Trick Room only when the opposing effective board is faster or Calyrex and the cosmic reserves materially dominate the slow order; do not reverse the twins' own winning fast mode. Use a twin-specific reserve selector that recognizes the fast pair, slow pair, and cosmic pair but always permits healthy board-state fallbacks and simultaneous-faint replacements. Value Lunala Wide Guard only against revealed spread pressure and preserve Shadow Shield when a safer direct line exists. Never self-activate Solgaleo's Weakness Policy through scripted ally damage or hidden player information.

## FORTREE_GYM_WINONA

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: double, cap 55, offsets [1, 1, 2, 2, 3, 4].
- Primary question: Can the player disrupt Winona's partner-safe spread lanes and changing speed before Feint removes Protect or No Guard Mega Pidgeot turns the final special exchange into guaranteed hits?
- Strongest part: Feint beside Earthquake and Discharge beside Volt Absorb turn Flying immunity into two different readable partner lanes, while No Guard Mega Pidgeot provides a clean final payoff instead of another setup ace.
- Weakest link: Three legendary genies or birds can look like raw-stat inflation, and the design depends on safe spread scoring; the common Rock/Ice weakness, no setup, no redirection, modest offsets, and strict partner checks must remain intact.
- First-loss lesson: Read the flight lanes. Break Talonflame before Feint and Tailwind coordinate with Earthquake, prevent Zapdos and Thundurus from sharing the field, exploit the switches with Rock or Ice pressure, and keep a specially sturdy answer for accurate Mega Pidgeot.
- References: `elite:wolfe:indianapolis-2026`, `showdown:gen9randomdoublesbattle:011`, `showdown:gen9randomdoublesbattle:009`, `showdown:gen8randomdoublesbattle:015`, `showdown:gen8randomdoublesbattle:013`, `showdown:gen5randomdoublesbattle:019`
- Team:
  - `SPECIES_TALONFLAME` +1 — `ITEM_FOCUS_SASH`, `ABILITY_GALE_WINGS`; `MOVE_TAILWIND`, `MOVE_FEINT`, `MOVE_BRAVE_BIRD`, `MOVE_QUICK_GUARD`
  - `SPECIES_LANDORUS_THERIAN` +1 — `ITEM_ASSAULT_VEST`, `ABILITY_INTIMIDATE`; `MOVE_EARTHQUAKE`, `MOVE_ROCK_SLIDE`, `MOVE_U_TURN`, `MOVE_KNOCK_OFF`
  - `SPECIES_ZAPDOS` +2 — `ITEM_SITRUS_BERRY`, `ABILITY_STATIC`; `MOVE_DISCHARGE`, `MOVE_HURRICANE`, `MOVE_HEAT_WAVE`, `MOVE_ROOST`
  - `SPECIES_THUNDURUS_THERIAN` +2 — `ITEM_LIFE_ORB`, `ABILITY_VOLT_ABSORB`; `MOVE_THUNDERBOLT`, `MOVE_ELECTROWEB`, `MOVE_GRASS_KNOT`, `MOVE_PROTECT`
  - `SPECIES_MANTINE` +3 — `ITEM_LEFTOVERS`, `ABILITY_WATER_ABSORB`; `MOVE_WIDE_GUARD`, `MOVE_AIR_SLASH`, `MOVE_SCALD`, `MOVE_ROOST`
  - `SPECIES_PIDGEOT` +4 — `ITEM_PIDGEOTITE`, `ABILITY_NO_GUARD`; Mega; `MOVE_HURRICANE`, `MOVE_HEAT_WAVE`, `MOVE_TAILWIND`, `MOVE_PROTECT`

AI must execute: Score Talonflame and Landorus actions jointly so Feint is chosen only when Landorus or the board can exploit the broken protection, and Tailwind or Quick Guard is not selected redundantly. Treat Earthquake as partner-safe for every Flying ally while still comparing direct attacks, Wide Guard, immunities, and visible knockout value. Prefer Zapdos Discharge beside Volt Absorb Thundurus only when both opponents and partner survival make it superior; never discharge freely beside another ally. Use a Winona reserve selector that recognizes the Ground-safe, Electric-safe, Wide Guard, and Mega roles without forcing unavailable pairs. Mega Evolve Pidgeot when active and use No Guard accuracy in damage estimates; do not use Tailwind again while a favorable Tailwind is active.

## PETALBURG_GYM_NORMAN

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: single, cap 45, offsets [1, 1, 2, 2, 3, 4].
- Primary question: Can the player identify each Normal Pokemon's visible category, item commitment, and ability constraint quickly enough to exploit it before Mega Kangaskhan converts one mistake into the sole setup endgame?
- Strongest part: Every Pokemon asks a different visible singles question, and Regigigas into Mega Kangaskhan gives Norman two plausible endgames without hiding identity or importing doubles machinery.
- Weakest link: Choice switching and six distinct information checks can feel like a sequence of modules unless Norman's replacement AI and dialogue unify them as one discipline test; runtime pacing must prove the team feels cohesive rather than encyclopedic.
- First-loss lesson: Read before attacking. Identify each lock and category, avoid contact into Fluffy, do not give Guts Swellow a better status plan, exploit Slow Start rather than rushing Regigigas, and keep Intimidate, burn, Haze, phazing, or immediate Fighting pressure for Mega Kangaskhan.
- References: `showdown:gen4randombattle:013`, `smogon:gen4uu:003`, `showdown:gen8randombattle:011`, `smogon:gen5uu:002`, `showdown:gen6randombattle:020`, `smogon:gen6nu:001`
- Team:
  - `SPECIES_PORYGON_Z` +1 — `ITEM_CHOICE_SPECS`, `ABILITY_ADAPTABILITY`; `MOVE_TRI_ATTACK`, `MOVE_DARK_PULSE`, `MOVE_THUNDERBOLT`, `MOVE_ICE_BEAM`
  - `SPECIES_SWELLOW` +1 — `ITEM_FLAME_ORB`, `ABILITY_GUTS`; `MOVE_FACADE`, `MOVE_BRAVE_BIRD`, `MOVE_U_TURN`, `MOVE_QUICK_ATTACK`
  - `SPECIES_BEWEAR` +2 — `ITEM_ASSAULT_VEST`, `ABILITY_FLUFFY`; `MOVE_DOUBLE_EDGE`, `MOVE_DRAIN_PUNCH`, `MOVE_ICE_PUNCH`, `MOVE_SHADOW_CLAW`
  - `SPECIES_MELOETTA` +2 — `ITEM_CHOICE_SCARF`, `ABILITY_SERENE_GRACE`; `MOVE_HYPER_VOICE`, `MOVE_PSYCHIC`, `MOVE_FOCUS_BLAST`, `MOVE_U_TURN`
  - `SPECIES_REGIGIGAS` +3 — `ITEM_LEFTOVERS`, `ABILITY_SLOW_START`; `MOVE_CRUSH_GRIP`, `MOVE_DRAIN_PUNCH`, `MOVE_KNOCK_OFF`, `MOVE_THUNDER_WAVE`
  - `SPECIES_KANGASKHAN` +4 — `ITEM_KANGASKHANITE`, `ABILITY_SCRAPPY`; Mega; `MOVE_FAKE_OUT`, `MOVE_DOUBLE_EDGE`, `MOVE_SUCKER_PUNCH`, `MOVE_POWER_UP_PUNCH`

AI must execute: Use singles switch scoring that understands each revealed Choice lock, immunity, known speed order, current HP, status, and hazard cost without reading unrevealed player information. Treat Swellow's activated Guts and Bewear's Fluffy as public state when comparing attacks; distinguish contact from non-contact damage. Track Regigigas Slow Start turns accurately, avoid unnecessary clock resets, and value Thunder Wave or Knock Off only when immediate board gain exceeds direct damage. Use Fake Out only on Kangaskhan's legal first active turn and Power-Up Punch only when Kangaskhan survives the visible response and can exploit the boost. Respect both Choice users' locks and use U-turn only when a legal healthier or matchup-correct reserve exists.

## LAVARIDGE_GYM_FLANNERY

- Status: design `design-complete`, source `source-closed`, runtime `unplayed`.
- Format/cap: double, cap 40, offsets [1, 1, 2, 2, 3, 4].
- Primary question: Can the player damage or disrupt Flannery's HP-sensitive Eruption engines while navigating After You and one justified Trick Room reversal, then preserve Water or Ground pressure for Air Balloon Heatran and Mega Emboar?
- Strongest part: The same slow Fire archetype can move first through After You or last through Trick Room, making temperature timing the real puzzle before a completely physical Mega Emboar finish.
- Weakest link: After You plus Eruption is a famous core and could feel borrowed rather than bespoke; removing sleep, tying every later formation to Lavaridge's thermal story, and preserving Magma's complete positioning doctrine are necessary distinctions.
- First-loss lesson: Damage Torkoal before After You converts Eruption, decide whether to deny Delphox or reverse its Trick Room, break Heatran's Balloon before committing Ground pressure, and keep a physical Water/Ground or Intimidate answer for Mega Emboar instead of spending everything on the sun lead.
- References: `vgc:regional-vancouver-bc-2023`, `vgc:laic-2017`, `showdown:gen9randomdoublesbattle:019`, `smogon:gen9ou:004`, `showdown:gen6randomdoublesbattle:010`, `showdown:gen9championsrandomdoublesbattle:027`
- Team:
  - `SPECIES_TORKOAL` +1 — `ITEM_EJECT_BUTTON`, `ABILITY_DROUGHT`; `MOVE_ERUPTION`, `MOVE_BODY_PRESS`, `MOVE_YAWN`, `MOVE_PROTECT`
  - `SPECIES_LILLIGANT` +1 — `ITEM_FOCUS_SASH`, `ABILITY_CHLOROPHYLL`; `MOVE_AFTER_YOU`, `MOVE_HELPING_HAND`, `MOVE_SOLAR_BEAM`, `MOVE_PROTECT`
  - `SPECIES_DELPHOX` +2 — `ITEM_MENTAL_HERB`, `ABILITY_PYROMANCY`; `MOVE_TRICK_ROOM`, `MOVE_HEAT_WAVE`, `MOVE_SHADOW_BALL`, `MOVE_WILL_O_WISP`
  - `SPECIES_SKELEDIRGE` +2 — `ITEM_THROAT_SPRAY`, `ABILITY_UNAWARE`; `MOVE_TORCH_SONG`, `MOVE_SHADOW_BALL`, `MOVE_HYPER_VOICE`, `MOVE_PROTECT`
  - `SPECIES_HEATRAN` +3 — `ITEM_AIR_BALLOON`, `ABILITY_FLASH_FIRE`; `MOVE_MAGMA_STORM`, `MOVE_FLASH_CANNON`, `MOVE_EARTH_POWER`, `MOVE_PROTECT`
  - `SPECIES_EMBOAR` +4 — `ITEM_EMBOARITE`, `ABILITY_RECKLESS`; Mega; `MOVE_HEAT_CRASH`, `MOVE_CLOSE_COMBAT`, `MOVE_HIGH_HORSEPOWER`, `MOVE_WILD_CHARGE`

AI must execute: Score Torkoal and Lilligant actions jointly: After You requires a slower healthy partner, meaningful same-turn attack value, and survivable visible board; Helping Hand requires superior damage value without redundant support. Use Eruption damage from current HP and avoid it when Heat Wave, Earth Power, Body Press, or direct coverage creates the stronger visible line. Set Trick Room only when the opposing effective board is faster or slow Fire reserves dominate; never reverse a winning Chlorophyll or After You state. Use a Flannery reserve selector that recognizes fast heat, slow heat, Heatran trap, and Mega Emboar physical roles without forcing missing pairs. Value Magma Storm through real accuracy and trap payoff, and Mega Evolve Emboar when active without manufacturing a safe sweep.

## MAUVILLE_GYM_WATTSON

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: double, cap 30, offsets [1, 1, 2, 2, 3, 4].
- Primary question: Can the player identify whether Wattson's live circuit is fast or reversed, deny the correct controller rather than merely attacking Electric weaknesses, and preserve a Ground or special-bulk answer for No Guard Mega Raichu Y?
- Strongest part: Every member reads as a different part of an electrical circuit, and Mega Raichu Y's No Guard changes Thunder and Focus Blast from risky moves into a visible mechanical climax.
- Weakest link: Fast terrain plus slow reversal comes from a famous modern championship architecture; the five-Electric typing, Emolga routing, Ampharos transformer, single earned Farigiraf exception, and exact Mega payoff must remain visible so the battle belongs to Wattson.
- First-loss lesson: Do not treat the team as six Electric targets. Break the Koko-Emolga routing, decide whether Farigiraf should be denied or allowed to reverse into your own slow answer, exploit Ampharos's lock, and keep the correct Ground or special-bulk piece for Mega Raichu's exact Thunder and Focus Blast.
- References: `elite:luca-ceribelli:worlds-2024`, `elite:wolfe:worlds-2016`, `showdown:gen9championsrandomdoublesbattle:009`, `showdown:gen4randombattle:003`, `vgc:ocic-2017`
- Team:
  - `SPECIES_TAPU_KOKO` +1 — `ITEM_TERRAIN_EXTENDER`, `ABILITY_ELECTRIC_SURGE`; `MOVE_WILD_CHARGE`, `MOVE_NATURES_MADNESS`, `MOVE_U_TURN`, `MOVE_TAUNT`
  - `SPECIES_EMOLGA` +1 — `ITEM_FOCUS_SASH`, `ABILITY_LIGHTNING_ROD`; `MOVE_TAILWIND`, `MOVE_ENCORE`, `MOVE_HELPING_HAND`, `MOVE_THUNDERBOLT`
  - `SPECIES_IRON_HANDS` +2 — `ITEM_ASSAULT_VEST`, `ABILITY_QUARK_DRIVE`; `MOVE_FAKE_OUT`, `MOVE_CLOSE_COMBAT`, `MOVE_WILD_CHARGE`, `MOVE_ICE_PUNCH`
  - `SPECIES_FARIGIRAF` +2 — `ITEM_SITRUS_BERRY`, `ABILITY_ARMOR_TAIL`; `MOVE_TRICK_ROOM`, `MOVE_TWIN_BEAM`, `MOVE_LIGHT_SCREEN`, `MOVE_PROTECT`
  - `SPECIES_AMPHAROS` +3 — `ITEM_CHOICE_SPECS`, `ABILITY_MOLD_BREAKER`; `MOVE_THUNDERBOLT`, `MOVE_DRAGON_PULSE`, `MOVE_FOCUS_BLAST`, `MOVE_POWER_GEM`
  - `SPECIES_RAICHU` +4 — `ITEM_RAICHUNITE_Y`, `ABILITY_STATIC`; Mega; `MOVE_THUNDER`, `MOVE_FOCUS_BLAST`, `MOVE_GRASS_KNOT`, `MOVE_PROTECT`

AI must execute: Score Koko and Emolga jointly: Tailwind, Taunt, Encore, Helping Hand, and U-turn require visible board value and may not stack redundant support when direct damage is superior. Set Trick Room only when the opposing effective board is faster and Iron Hands or Ampharos gains meaningful action order; never reverse a winning Tailwind or fast Mega state. Recognize Armor Tail and Lightning Rod when valuing priority and Electric targets, including ally-protection rather than hidden player intent. Use a Wattson reserve selector for fast grid, heavy relay, reversed transformer, and Mega overvoltage roles with independent missing-partner fallbacks. Mega Evolve Raichu when active and evaluate Thunder and Focus Blast with No Guard only after the transformation is actually selected.

## Six-Gym anchor-board review

- Unique species: 36/36; Gym collisions: 0; League collisions: 0.
- Unique Mega signatures: 6/6.
- Specialty members: `SOOTOPOLIS_GYM_JUAN` 6/6, `MOSSDEEP_GYM_TATE_AND_LIZA` 6/6, `FORTREE_GYM_WINONA` 6/6, `PETALBURG_GYM_NORMAN` 6/6, `LAVARIDGE_GYM_FLANNERY` 5/6, `MAUVILLE_GYM_WATTSON` 5/6.
- Judgment: The board spends six different species rosters, six different Megas, and six different primary questions. Shared competitive references are evidence, not allocations; any reuse remains acceptable only when the imported interaction and battle lesson differ.
