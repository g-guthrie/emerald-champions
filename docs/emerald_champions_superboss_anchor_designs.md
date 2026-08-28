# Emerald Champions rival and superboss anchor designs

Progress: 7/7 primary anchors are design-complete; source remains untouched.

## STEVEN_METEOR_FALLS_SUPERBOSS

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: double, cap 100.
- Primary question: Can the player decide whether to deny or reverse Magearna's Trick Room while preventing Diancie, Soul-Heart, Beast Boost, Make It Rain, and Mega Metagross from converting each knockout into the next material advantage?
- Strongest part: The team is unmistakably Steven without defaulting to passive Steel walls: every rare material changes battle state in a different, visible, attack-driven way.
- Weakest link: Soul-Heart plus Beast Boost plus level 100 can snowball brutally. Their event correctness and broad Haze/Unaware/Fire/Ground/Fighting counterplay need real-ROM testing before observed difficulty can be claimed.
- First-loss lesson: Steven's treasure compounds only if you let each material hand value to the next. Decide the speed mode, exploit the public costs and locks, clear snowball stats, and preserve the correct Fire/Ground/Fighting line for Metagross.
- References: `showdown:gen9randomdoublesbattle:010`, `showdown:gen9randomdoublesbattle:005`, `showdown:gen7randomdoublesbattle:010`, `elite:wolfe:milwaukee-2025`, `elite:ray-rizzo:worlds-2012`
- Team:
  - `SPECIES_DIANCIE` — `ITEM_SITRUS_BERRY`, `ABILITY_CLEAR_BODY`; `MOVE_DIAMOND_STORM`, `MOVE_BODY_PRESS`, `MOVE_HELPING_HAND`, `MOVE_PROTECT`
  - `SPECIES_MAGEARNA` — `ITEM_MENTAL_HERB`, `ABILITY_SOUL_HEART`; `MOVE_TRICK_ROOM`, `MOVE_FLEUR_CANNON`, `MOVE_FLASH_CANNON`, `MOVE_PROTECT`
  - `SPECIES_MELMETAL` — `ITEM_ASSAULT_VEST`, `ABILITY_IRON_FIST`; `MOVE_DOUBLE_IRON_BASH`, `MOVE_HIGH_HORSEPOWER`, `MOVE_ICE_PUNCH`, `MOVE_THUNDER_PUNCH`
  - `SPECIES_KARTANA` — `ITEM_CHOICE_SCARF`, `ABILITY_BEAST_BOOST`; `MOVE_LEAF_BLADE`, `MOVE_AERIAL_ACE`, `MOVE_SACRED_SWORD`, `MOVE_KNOCK_OFF`
  - `SPECIES_GHOLDENGO` — `ITEM_WHITE_HERB`, `ABILITY_GOOD_AS_GOLD`; `MOVE_MAKE_IT_RAIN`, `MOVE_SHADOW_BALL`, `MOVE_THUNDERBOLT`, `MOVE_PROTECT`
  - `SPECIES_METAGROSS` — `ITEM_METAGROSSITE`, `ABILITY_CLEAR_BODY`; Mega; `MOVE_METEOR_MASH`, `MOVE_ZEN_HEADBUTT`, `MOVE_ICE_PUNCH`, `MOVE_PROTECT`

AI must execute: Set Trick Room only when the opposing effective board is faster and Steven's active or reserve slow materials gain meaningful order; never reverse a winning fast Kartana or Mega Metagross state. Use Helping Hand only for meaningful same-turn damage and let Diancie attack or Protect otherwise. Track Fleur Cannon and Make It Rain stat drops, Soul-Heart and Beast Boost triggers, and White Herb consumption exactly. Respect Kartana's public Choice lock and select Melmetal, Kartana, or Gholdengo from visible speed and damage needs. Mega Evolve Metagross normally and use direct coverage without hidden matchup knowledge.

## CYNTHIA_MOSSDEEP_SUPERBOSS

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: double, cap 100.
- Primary question: Can the player break Togekiss's redirection and Tailwind without feeding Mega Garchomp free Earthquakes, then navigate mixed immediate coverage, Spiritomb's finite control, and Competitive Milotic without relying on Intimidate autopilot?
- Strongest part: The recognizable roster is preserved while every passive or sleep-heavy set becomes a real doubles action, so nostalgia and difficulty reinforce each other.
- Weakest link: Milotic repeats a protected Champion species. The iconic roster and base Competitive role justify it, but the guide must explicitly contrast Cynthia's closer with Wallace's Mega rain ace.
- First-loss lesson: You already know Cynthia's species; the puzzle is their coordination. Break the opening lane, switch damage categories around Spiritomb, and do not hand Competitive Milotic the exact stat drop it wants.
- References: `elite:sejun-park:worlds-2014`, `showdown:gen7randomdoublesbattle:010`, `showdown:gen7randomdoublesbattle:014`, `showdown:gen9championsrandomdoublesbattle:007`, `elite:ray-rizzo:worlds-2012`
- Team:
  - `SPECIES_TOGEKISS` — `ITEM_SITRUS_BERRY`, `ABILITY_SERENE_GRACE`; `MOVE_FOLLOW_ME`, `MOVE_TAILWIND`, `MOVE_AIR_SLASH`, `MOVE_DAZZLING_GLEAM`
  - `SPECIES_GARCHOMP` — `ITEM_GARCHOMPITE`, `ABILITY_ROUGH_SKIN`; Mega; `MOVE_EARTHQUAKE`, `MOVE_DRAGON_CLAW`, `MOVE_ROCK_SLIDE`, `MOVE_PROTECT`
  - `SPECIES_ROSERADE` — `ITEM_FOCUS_SASH`, `ABILITY_TECHNICIAN`; `MOVE_LEAF_STORM`, `MOVE_SLUDGE_BOMB`, `MOVE_SHADOW_BALL`, `MOVE_PROTECT`
  - `SPECIES_LUCARIO` — `ITEM_LIFE_ORB`, `ABILITY_INNER_FOCUS`; `MOVE_CLOSE_COMBAT`, `MOVE_METEOR_MASH`, `MOVE_EXTREME_SPEED`, `MOVE_PROTECT`
  - `SPECIES_SPIRITOMB` — `ITEM_LEFTOVERS`, `ABILITY_PRESSURE`; `MOVE_SNARL`, `MOVE_WILL_O_WISP`, `MOVE_SUCKER_PUNCH`, `MOVE_PROTECT`
  - `SPECIES_MILOTIC` — `ITEM_ADRENALINE_ORB`, `ABILITY_COMPETITIVE`; `MOVE_SCALD`, `MOVE_ICE_BEAM`, `MOVE_RECOVER`, `MOVE_PROTECT`

AI must execute: Score Togekiss and Garchomp jointly: Follow Me requires real survival or attack value, Tailwind requires speed value, and Garchomp Earthquake must account for its partner. Use Air Slash and Dazzling Gleam when support is redundant rather than locking Togekiss into passive turns. Track Leaf Storm and Close Combat drops and select Roserade or Lucario from visible target defense and coverage. Use Spiritomb Snarl or Will-O-Wisp against the correct visible category and attack otherwise. Recognize Competitive and Adrenaline Orb from actual stat-drop events and use Recover only when survival value exceeds damage.

## LEAF_ALTERING_CAVE_SUPERBOSS

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: double, cap 100.
- Primary question: Can the player prevent Mew from creating the exact speed or coaching line Snorlax needs, then survive immediate Mewtwo/Tauros/Moltres pressure while reducing Mega Blastoise's HP before Water Spout?
- Strongest part: The team feels like a Kanto legend without stacking six box legends: every icon has a distinct competitive job and the Mega starter is the emotional finish.
- Weakest link: Mew plus Belly Drum can become binary. It must remain a contestable option rather than a scripted opening, and Leaf must be fully dangerous when the setup is denied.
- First-loss lesson: Kanto's icons are dangerous for different reasons. Break the one setup, exploit public costs and locks, change your defensive axis between legends, and hit Blastoise before its HP becomes your problem.
- References: `showdown:gen4randomdoublesbattle:003`, `showdown:gen9randomdoublesbattle:014`, `showdown:gen9randomdoublesbattle:012`, `showdown:gen9championsrandomdoublesbattle:010`, `vgc:korean-nationals-2018`
- Team:
  - `SPECIES_MEW` — `ITEM_MENTAL_HERB`, `ABILITY_SYNCHRONIZE`; `MOVE_TAILWIND`, `MOVE_HELPING_HAND`, `MOVE_COACHING`, `MOVE_PSYCHIC`
  - `SPECIES_SNORLAX` — `ITEM_FIGY_BERRY`, `ABILITY_GLUTTONY`; `MOVE_BELLY_DRUM`, `MOVE_BODY_SLAM`, `MOVE_HIGH_HORSEPOWER`, `MOVE_PROTECT`
  - `SPECIES_MEWTWO` — `ITEM_LIFE_ORB`, `ABILITY_UNNERVE`; `MOVE_PSYSTRIKE`, `MOVE_ICE_BEAM`, `MOVE_AURA_SPHERE`, `MOVE_PROTECT`
  - `SPECIES_TAUROS` — `ITEM_CHOICE_BAND`, `ABILITY_INTIMIDATE`; `MOVE_DOUBLE_EDGE`, `MOVE_CLOSE_COMBAT`, `MOVE_HIGH_HORSEPOWER`, `MOVE_ROCK_SLIDE`
  - `SPECIES_MOLTRES` — `ITEM_SAFETY_GOGGLES`, `ABILITY_FLAME_BODY`; `MOVE_HEAT_WAVE`, `MOVE_AIR_SLASH`, `MOVE_WILL_O_WISP`, `MOVE_ROOST`
  - `SPECIES_BLASTOISE` — `ITEM_BLASTOISINITE`, `ABILITY_TORRENT`; Mega; `MOVE_WATER_SPOUT`, `MOVE_DARK_PULSE`, `MOVE_AURA_SPHERE`, `MOVE_PROTECT`

AI must execute: Score Mew and Snorlax jointly: Tailwind, Helping Hand, and Coaching require visible value; Belly Drum requires survival, berry state, and a real next-turn line. Stop supporting when Snorlax setup is denied or spent and let Mew attack. Respect Tauros's Choice lock, Double-Edge recoil, Mewtwo Life Orb, and Moltres Roost/burn from public state. Evaluate Water Spout from Blastoise's current HP and prefer pulse coverage or Protect when stronger. Mega Evolve Blastoise normally and do not Shell Smash or add an alternate Mega.

## WALLY_VICTORY_ROAD

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: double, cap 80.
- Primary question: Can the player interrupt Whimsicott's flexible support and Sylveon's finite spread boost, then survive layered priority and a Choice Mold Breaker without spending every answer before Mega Gallade's Wide Guard endgame?
- Strongest part: In a game full of rare monsters, Wally's hardest statement is that six accessible species can be a League-level team through perfect roles and trust.
- Weakest link: Whimsicott plus Sylveon is a recognizable competitive module. The sleep-free priority contrast, no-legend narrative, and Haxorus/Gallade finish must keep the whole fight personal rather than borrowed.
- First-loss lesson: Wally's strength is efficient ordinary Pokémon. Break the first support decision, identify which priority profile is entering, trap Haxorus in the wrong move, and vary targeting so Gallade cannot shield everything.
- References: `elite:wolfe:milwaukee-2025`, `elite:francesco-pio-pero:naic-2026`, `showdown:gen4randomdoublesbattle:008`, `vgc:korean-winter-league-2018`, `showdown:gen9randomdoublesbattle:021`, `showdown:gen5randomdoublesbattle:022`
- Team:
  - `SPECIES_WHIMSICOTT` — `ITEM_MENTAL_HERB`, `ABILITY_PRANKSTER`; `MOVE_TAILWIND`, `MOVE_ENCORE`, `MOVE_HELPING_HAND`, `MOVE_MOONBLAST`
  - `SPECIES_SYLVEON` — `ITEM_THROAT_SPRAY`, `ABILITY_PIXILATE`; `MOVE_HYPER_VOICE`, `MOVE_QUICK_ATTACK`, `MOVE_MYSTICAL_FIRE`, `MOVE_PROTECT`
  - `SPECIES_BRELOOM` — `ITEM_FOCUS_SASH`, `ABILITY_TECHNICIAN`; `MOVE_MACH_PUNCH`, `MOVE_BULLET_SEED`, `MOVE_ROCK_TOMB`, `MOVE_PROTECT`
  - `SPECIES_AZUMARILL` — `ITEM_ASSAULT_VEST`, `ABILITY_HUGE_POWER`; `MOVE_LIQUIDATION`, `MOVE_PLAY_ROUGH`, `MOVE_AQUA_JET`, `MOVE_KNOCK_OFF`
  - `SPECIES_HAXORUS` — `ITEM_CHOICE_SCARF`, `ABILITY_MOLD_BREAKER`; `MOVE_DRAGON_CLAW`, `MOVE_STOMPING_TANTRUM`, `MOVE_POISON_JAB`, `MOVE_ROCK_SLIDE`
  - `SPECIES_GALLADE` — `ITEM_GALLADITE`, `ABILITY_JUSTIFIED`; Mega; `MOVE_PSYCHO_CUT`, `MOVE_CLOSE_COMBAT`, `MOVE_WIDE_GUARD`, `MOVE_PROTECT`

AI must execute: Score Whimsicott and Sylveon jointly: Tailwind, Encore, and Helping Hand require visible value; Moonblast is preferred when support is redundant. Track Throat Spray consumption and Pixilate targeting exactly; do not farm Quick Attack or Protect turns. Select Breloom versus Azumarill from actual target bulk, priority need, multi-hit value, and item-removal value. Respect Haxorus's Choice lock and Mold Breaker only against abilities it actually bypasses. Use Wide Guard only against disclosed spread pressure and Mega Evolve Gallade normally.

## LILYCOVE_RIVAL

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: double, cap 60.
- Primary question: Can the player disrupt Mega Butterfree and Mienshao's flexible opening, exploit Dracovish's Choice/move-order commitment, and solve the common core without the answer depending narrowly on which of 21 counter-starters occupies the final slot?
- Strongest part: The final rival is genuinely personalized for every starter choice without becoming 21 narrow puzzles; the branch changes one meaningful emotional slot inside one excellent common team.
- Weakest link: Universal type templates cannot be each starter's perfect bespoke set. That is intentional restraint: branch fairness and legality matter more than pretending 21 different optimized puzzles occupy one encounter.
- First-loss lesson: The starter changes the flavor, not the solution. Break the common opening, exploit the public commitment modules, and adapt one reserve slot after preview instead of rebuilding for a single scripted counter.
- References: `showdown:gen8randomdoublesbattle:011`, `showdown:gen7randomdoublesbattle:016`, `showdown:gen4randomdoublesbattle:029`, `vgc:ocic-2020`, `showdown:gen7randomdoublesbattle:013`
- Team:
  - `SPECIES_BUTTERFREE` — `ITEM_BUTTERFRENITE`, `ABILITY_COMPOUND_EYES`; Mega; `MOVE_TAILWIND`, `MOVE_RAGE_POWDER`, `MOVE_BUG_BUZZ`, `MOVE_HURRICANE`
  - `SPECIES_MIENSHAO` — `ITEM_EJECT_BUTTON`, `ABILITY_INNER_FOCUS`; `MOVE_FAKE_OUT`, `MOVE_FEINT`, `MOVE_CLOSE_COMBAT`, `MOVE_U_TURN`
  - `SPECIES_ESPEON` — `ITEM_LIFE_ORB`, `ABILITY_MAGIC_BOUNCE`; `MOVE_PSYCHIC`, `MOVE_DAZZLING_GLEAM`, `MOVE_SHADOW_BALL`, `MOVE_PROTECT`
  - `SPECIES_DRACOVISH` — `ITEM_CHOICE_SCARF`, `ABILITY_STRONG_JAW`; `MOVE_FISHIOUS_REND`, `MOVE_DRAGON_RUSH`, `MOVE_CRUNCH`, `MOVE_ROCK_SLIDE`
  - `SPECIES_PALOSSAND` — `ITEM_SITRUS_BERRY`, `ABILITY_WATER_COMPACTION`; `MOVE_EARTH_POWER`, `MOVE_SHADOW_BALL`, `MOVE_SHORE_UP`, `MOVE_PROTECT`
  - `SPECIES_BLAZIKEN` — `ITEM_EXPERT_BELT`, `ABILITY_BLAZE`; `MOVE_FIRE_PLEDGE`, `MOVE_HEAT_WAVE`, `MOVE_FLARE_BLITZ`, `MOVE_PROTECT`

AI must execute: Score Butterfree and Mienshao jointly: Tailwind, Rage Powder, Fake Out, and Feint require visible value; both attack when support is redundant. Resolve Eject Button and reserve selection normally and keep May/Brendan branch behavior identical. Evaluate Fishious Rend from actual move order and respect Dracovish's Choice lock. Recognize Water Compaction only from real Water damage and prevent Palossand recovery loops. Load and evaluate the actual dynamic starter species and its type-specific move table rather than the Hoenn placeholder.

## ROUTE_119_RIVAL

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: double, cap 55.
- Primary question: Can the player break Butterfree-Weavile's sleep-free tactical opening, then navigate Rotom's pivot, Starmie's coverage, Passimian's Defiant pressure, and one public fully evolved counter-starter without any branch becoming a hard-counter script?
- Strongest part: This feels like a continuing rival team: two early partners visibly matured, one late signature begins, and the player's starter choice remains emotionally real across all seven generations.
- Weakest link: The common core has several fast utility attackers and could feel toolbox-like. Continuity, exact role separation, and the public starter branch must keep it a character team rather than five random good sets.
- First-loss lesson: This rival has grown with recognizable partners. Break the common tactical opening, exploit each public cost, and adjust one slot after preview for the actual starter instead of expecting a Hoenn-only script.
- References: `showdown:gen8randomdoublesbattle:011`, `showdown:gen7randomdoublesbattle:016`, `showdown:gen7randomdoublesbattle:006`, `showdown:gen4randomdoublesbattle:030`, `showdown:gen9championsrandomdoublesbattle:008`
- Team:
  - `SPECIES_BUTTERFREE` — `ITEM_FOCUS_SASH`, `ABILITY_COMPOUND_EYES`; `MOVE_TAILWIND`, `MOVE_RAGE_POWDER`, `MOVE_BUG_BUZZ`, `MOVE_HURRICANE`
  - `SPECIES_WEAVILE` — `ITEM_LIFE_ORB`, `ABILITY_PRESSURE`; `MOVE_FAKE_OUT`, `MOVE_ICICLE_CRASH`, `MOVE_KNOCK_OFF`, `MOVE_FEINT`
  - `SPECIES_ROTOM_MOW` — `ITEM_SITRUS_BERRY`, `ABILITY_LEVITATE`; `MOVE_LEAF_STORM`, `MOVE_THUNDERBOLT`, `MOVE_VOLT_SWITCH`, `MOVE_WILL_O_WISP`
  - `SPECIES_STARMIE` — `ITEM_EXPERT_BELT`, `ABILITY_ANALYTIC`; `MOVE_HYDRO_PUMP`, `MOVE_PSYSHOCK`, `MOVE_ICE_BEAM`, `MOVE_PROTECT`
  - `SPECIES_PASSIMIAN` — `ITEM_ASSAULT_VEST`, `ABILITY_DEFIANT`; `MOVE_CLOSE_COMBAT`, `MOVE_KNOCK_OFF`, `MOVE_ROCK_SLIDE`, `MOVE_U_TURN`
  - `SPECIES_BLAZIKEN` — `ITEM_LEFTOVERS`, `ABILITY_BLAZE`; `MOVE_FIRE_PLEDGE`, `MOVE_HEAT_WAVE`, `MOVE_FLARE_BLITZ`, `MOVE_PROTECT`

AI must execute: Score Butterfree-Weavile actions jointly: Tailwind, Rage Powder, Fake Out, and Feint require visible value; both attack when support is redundant. Track Leaf Storm drops and use Volt Switch/U-turn only when the reserve improves the board. Recognize Defiant from actual stat drops and never infer Intimidate before it occurs. Evaluate Starmie's real accuracy and target defense without hidden information. Load and evaluate the actual final counter-starter and its type template, with May/Brendan parity.

## STEVEN_MOSSDEEP_ALLY

- Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.
- Format/cap: multi_2_vs_2, cap 70.
- Primary question: Can the player choose three partners that complement Steven's Water shield, screens, Acid Spray, and Mega Aggron while surviving two coordinated enemy trainers with redirection/Wide Guard, Psycho Boost, two Beast Boost paths, and a final Choice commitment?
- Strongest part: The dossier finally treats the encounter as the physical multi battle it is: nine exact deployable Pokemon, two distinct enemy trainers, a useful ally, and party selection as part of the puzzle.
- Weakest link: Fixed three-slot ordering can make the enemy waves feel scripted. Cross-trainer faints naturally desynchronize them, and each slot has independent action fallbacks, but real multi-battle testing is essential.
- First-loss lesson: This battle begins in party selection. Bring what Steven lacks, remove the enemy support that protects the current attacker, and treat the two enemy trainers as desynchronizable three-member lines rather than one six-slot switch engine.
- References: `showdown:gen7randomdoublesbattle:017`, `vgc:regional-collinsville-il-2019`, `showdown:gen4randomdoublesbattle:019`, `vgc:euic-2019`, `showdown:gen7randomdoublesbattle:011`, `showdown:gen4randomdoublesbattle:030`, `showdown:gen4randomdoublesbattle:011`, `showdown:gen5randomdoublesbattle:015`
- Team:
  - `SPECIES_CRADILY` — `ITEM_ASSAULT_VEST`, `ABILITY_STORM_DRAIN`; `MOVE_GIGA_DRAIN`, `MOVE_POWER_GEM`, `MOVE_EARTH_POWER`, `MOVE_ACID_SPRAY`
  - `SPECIES_CLAYDOL` — `ITEM_LIGHT_CLAY`, `ABILITY_LEVITATE`; `MOVE_REFLECT`, `MOVE_LIGHT_SCREEN`, `MOVE_PSYCHIC`, `MOVE_EARTH_POWER`
  - `SPECIES_AGGRON` — `ITEM_AGGRONITE`, `ABILITY_STURDY`; Mega; `MOVE_HEAVY_SLAM`, `MOVE_BODY_PRESS`, `MOVE_ROCK_SLIDE`, `MOVE_PROTECT`
- Maxie opponent team:
  - `SPECIES_RESHIRAM` — `ITEM_LIFE_ORB`, `ABILITY_TURBOBLAZE`; `MOVE_HEAT_WAVE`, `MOVE_DRAGON_PULSE`, `MOVE_EARTH_POWER`, `MOVE_PROTECT`
  - `SPECIES_VOLCARONA` — `ITEM_SITRUS_BERRY`, `ABILITY_FLAME_BODY`; `MOVE_RAGE_POWDER`, `MOVE_HEAT_WAVE`, `MOVE_BUG_BUZZ`, `MOVE_PROTECT`
  - `SPECIES_TURTONATOR` — `ITEM_LEFTOVERS`, `ABILITY_SHELL_ARMOR`; `MOVE_WIDE_GUARD`, `MOVE_HEAT_WAVE`, `MOVE_DRAGON_PULSE`, `MOVE_PROTECT`
- Courtney opponent team:
  - `SPECIES_DEOXYS_ATTACK` — `ITEM_FOCUS_SASH`, `ABILITY_PRESSURE`; `MOVE_PSYCHO_BOOST`, `MOVE_ICE_BEAM`, `MOVE_SHADOW_BALL`, `MOVE_PROTECT`
  - `SPECIES_NIHILEGO` — `ITEM_EXPERT_BELT`, `ABILITY_BEAST_BOOST`; `MOVE_POWER_GEM`, `MOVE_SLUDGE_BOMB`, `MOVE_THUNDERBOLT`, `MOVE_PROTECT`
  - `SPECIES_BLACEPHALON` — `ITEM_CHOICE_SCARF`, `ABILITY_BEAST_BOOST`; `MOVE_HEAT_WAVE`, `MOVE_SHADOW_BALL`, `MOVE_PSYCHIC`, `MOVE_TRICK`

AI must execute: Steven must target enemies only, choose Reflect versus Light Screen from visible enemy categories, avoid redundant screens, and use Acid Spray when the player's visible special attacker benefits. Enemy AI must coordinate Rage Powder and Wide Guard only with real partner value and cannot read the player's hidden selected move. Track Psycho Boost and Choice lock, Beast Boost triggers, and cross-trainer target/replacement state exactly. Apply live difficulty only to gEnemyParty, never Steven's gMultiPartnerParty; add a regression proving ally levels are invariant. Mega Evolve Aggron normally and never expose Mega Metagross or an enemy Mega in this encounter.

## Tranche review

- Distinct species: 43 across 45 slots; unwaived internal collisions: 0; unwaived protected collisions: 0.
- Unique Mega signatures: 6/7.
- Judgment: Steven tests material-state compounding, Cynthia tests iconic doubles coordination, and Leaf tests finite setup across Kanto damage axes. The three are mechanically and culturally distinct.

## Primary anchor board complete

Next: Frontier brains and rematch anchors, followed by campaign-wide collision review.
