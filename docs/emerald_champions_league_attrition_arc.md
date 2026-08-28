# Emerald Champions main-story League attrition arc

Status: design `design-complete`, source `unimplemented`, runtime `unplayed`.

## Confirmed native rules

- Strict cap: 80 on Hard.
- Live difficulty: Medium -2, Easy -4 opposing levels only.
- No automatic healing between League members.
- Manual Bag, Pokémon, held-item, and Save access returns between rooms.
- Bag items cannot be used during trainer battles.
- Party composition is locked after League entry.
- Mega Evolution and intentional Primal Reversion are supported; Tera, Z-Moves, Dynamax, and Gigantamax are not.

## Arc question

Can one fixed six-Pokemon party survive five distinct near-impossible questions by spending carried recovery deliberately and preserving the right strategic classes for later rooms?

## Five-stage shape

1. Sidney steals position and turns visible losses into one final king.
2. Phoebe changes the resource from tempo to turns, exits, sleep, burn, and survival.
3. Glacia punishes static play with a readable detonation, trapping, item loss, and a late snowball.
4. Drake changes pace to a singles preservation duel with hazards, one Choice lock, and a visible Mega handoff.
5. Wallace cashes out the surviving field-control and physical answers through fast rain, one denyable reversal, and Mega Milotic.

## 1. ELITE_FOUR_SIDNEY

- Format: double; cap 80; offsets [0, 0, 1, 1, 1, 2].
- Primary question: Can the player solve the Incineroar and Urshifu target dilemma, contest Yveltal tempo, and preserve a Fighting, Ground, or boost-control answer for the final Supreme Overlord Kingambit?
- Strongest part: The Eject Button transition can visibly turn an attack on Incineroar into Dark Aura pressure beside Urshifu, while the same early knockouts build toward a readable Supreme Overlord final king.
- Weakest link: The joint opening scorer and total visible-state reserve selector are substantial unimplemented AI work; the design is complete, but its target 10 remains unobserved until those behaviors and broad matchup families run in the ROM.
- First-loss lesson: Do not treat Protect as universal against the opener. Exploit Urshifu's lock, contest Tailwind, deny Mega Absol its visible disruption job, vary revealed moves so Darkrai cannot Disable the whole plan, and preserve Fighting, Ground, Haze, or phazing for the visibly empowered Iron Head Kingambit.
- Resource tax: The roster applies direct HP and faint pressure plus Will-O-Wisp burn from Mega Absol. Nothing is restored automatically after victory; the player may spend carried healing, status recovery, and revival stock before Phoebe.
- References: `elite:marco-silva:turin-2026`, `vgc:regional-melbourne-2022`, `showdown:gen9randomdoublesbattle:016`, `showdown:gen6randomdoublesbattle:022`
- Exact team:
  - `SPECIES_INCINEROAR` — `ITEM_EJECT_BUTTON`, `ABILITY_INTIMIDATE`; `MOVE_FAKE_OUT`, `MOVE_PARTING_SHOT`, `MOVE_FLARE_BLITZ`, `MOVE_KNOCK_OFF`
  - `SPECIES_URSHIFU` — `ITEM_CHOICE_BAND`, `ABILITY_UNSEEN_FIST`; `MOVE_WICKED_BLOW`, `MOVE_CLOSE_COMBAT`, `MOVE_IRON_HEAD`, `MOVE_U_TURN`
  - `SPECIES_YVELTAL` — `ITEM_ROSELI_BERRY`, `ABILITY_DARK_AURA`; `MOVE_TAILWIND`, `MOVE_OBLIVION_WING`, `MOVE_SNARL`, `MOVE_PROTECT`
  - `SPECIES_ABSOL` — `ITEM_ABSOLITE`, `ABILITY_SUPER_LUCK`; Mega; `MOVE_KNOCK_OFF`, `MOVE_PLAY_ROUGH`, `MOVE_WILL_O_WISP`, `MOVE_PROTECT`
  - `SPECIES_DARKRAI` — `ITEM_LUM_BERRY`, `ABILITY_BAD_DREAMS`; `MOVE_DARK_PULSE`, `MOVE_ICY_WIND`, `MOVE_TAUNT`, `MOVE_DISABLE`
  - `SPECIES_KINGAMBIT` — `ITEM_BLACK_GLASSES`, `ABILITY_SUPREME_OVERLORD`; `MOVE_KOWTOW_CLEAVE`, `MOVE_IRON_HEAD`, `MOVE_SWORDS_DANCE`, `MOVE_SUCKER_PUNCH`

## 2. ELITE_FOUR_PHOEBE

- Format: double; cap 80; offsets [1, 1, 1, 2, 3, 4].
- Primary question: Can the player preserve an escape route or remove Mega Gengar before the shared Perish clock converts two active Pokemon into unavoidable resource loss?
- Strongest part: One Mega Gengar supports two staged Perish circles, yet removing that single visible trapper collapses the hard clock and forces Phoebe into a materially different direct-cleanup phase.
- Weakest link: Coordinated count-one exits, the returning-Gengar second circle, and the escape-aware fallback can become slow or brittle unless the source state machine is implemented and exercised against several visible escape plans.
- First-loss lesson: Count both sides, not just the player's clock. Preserve one escape class or double-target Mega Gengar on a non-Protect turn; once the single trapper is gone, Phoebe becomes a hard but conventional Ghost offense.
- Resource tax: Perish Song itself ends with battle, but any fainted Pokemon remain fainted, Spore sleep and Will-O-Wisp burn persist, and direct damage carries forward. Control returns after victory, so the player may spend carried Awakenings, Full Heals, recovery, and Revives before Glacia; no automatic repair occurs.
- References: `elite:wolfe:orlando-2023`, `elite:marco-silva:turin-2026`, `showdown:gen7randomdoublesbattle:015`, `showdown:gen7randomdoublesbattle:018`, `showdown:gen8randomdoublesbattle:002`
- Exact team:
  - `SPECIES_GENGAR` — `ITEM_GENGARITE`, `ABILITY_CURSED_BODY`; Mega; `MOVE_PERISH_SONG`, `MOVE_SHADOW_BALL`, `MOVE_DISABLE`, `MOVE_PROTECT`
  - `SPECIES_AMOONGUSS` — `ITEM_MENTAL_HERB`, `ABILITY_REGENERATOR`; `MOVE_RAGE_POWDER`, `MOVE_SPORE`, `MOVE_SYNTHESIS`, `MOVE_PROTECT`
  - `SPECIES_FLUTTER_MANE` — `ITEM_FOCUS_SASH`, `ABILITY_PROTOSYNTHESIS`; `MOVE_PERISH_SONG`, `MOVE_MOONBLAST`, `MOVE_SHADOW_BALL`, `MOVE_PROTECT`
  - `SPECIES_AEGISLASH` — `ITEM_LEFTOVERS`, `ABILITY_STANCE_CHANGE`; `MOVE_KINGS_SHIELD`, `MOVE_SUBSTITUTE`, `MOVE_SHADOW_BALL`, `MOVE_SACRED_SWORD`
  - `SPECIES_MIMIKYU` — `ITEM_SITRUS_BERRY`, `ABILITY_DISGUISE`; `MOVE_WILL_O_WISP`, `MOVE_TAUNT`, `MOVE_PLAY_ROUGH`, `MOVE_SHADOW_CLAW`
  - `SPECIES_MARSHADOW` — `ITEM_LIFE_ORB`, `ABILITY_TECHNICIAN`; `MOVE_SPECTRAL_THIEF`, `MOVE_SHADOW_SNEAK`, `MOVE_ICE_PUNCH`, `MOVE_CLOSE_COMBAT`

## 3. ELITE_FOUR_GLACIA

- Format: double; cap 80; offsets [2, 1, 2, 2, 2, 3].
- Primary question: Can the player act on Glalie's explicit pre-selection crack warning, break Toxapex's trap-and-item transition, and still preserve an answer for final Chilling Neigh Glastrier?
- Strongest part: The one-time crack cue makes Mega Glalie's later Refrigerate detonation actionable before move selection, and final Glastrier gives the shattered board one distinct late snowball rather than another interchangeable fast Ice attacker.
- Weakest link: The battle depends on a bespoke cue and no-turn-zero detonation gate; the middle Chien-Pao, Chandelure, and Iron Bundle selection must prove distinct and Glastrier must prove climactic rather than merely slow in runtime.
- First-loss lesson: Use the guaranteed first action to identify Glalie and Toxapex choices, then watch Baneful Bunker as the detonation tell. Force Toxapex to transition without value, control Iron Bundle speed, and keep burn, Intimidate, or a Fighting or Steel answer for Glastrier.
- Resource tax: Glacia enters third without automatic healing. The readable lead taxes Protect and disruption rather than surprise knowledge; trapping, poison, Knock Off, speed control, priority, and final Glastrier pressure tax HP, status cures, held items, and the Ice or Fighting answer the player must preserve before Drake.
- References: `elite:wolfe:naic-2026`, `showdown:gen4randomdoublesbattle:008`, `showdown:gen9randomdoublesbattle:014`, `showdown:gen9randomdoublesbattle:002`, `showdown:gen6randomdoublesbattle:026`, `elite:shohei-kimura:worlds-2023`
- Exact team:
  - `SPECIES_GLALIE` — `ITEM_GLALITITE`, `ABILITY_REFRIGERATE`; Mega; `MOVE_DOUBLE_EDGE`, `MOVE_EXPLOSION`, `MOVE_ICE_SHARD`, `MOVE_PROTECT`
  - `SPECIES_TOXAPEX` — `ITEM_LEFTOVERS`, `ABILITY_REGENERATOR`; `MOVE_BANEFUL_BUNKER`, `MOVE_INFESTATION`, `MOVE_TOXIC`, `MOVE_KNOCK_OFF`
  - `SPECIES_CHIEN_PAO` — `ITEM_LIFE_ORB`, `ABILITY_SWORD_OF_RUIN`; `MOVE_ICICLE_CRASH`, `MOVE_SACRED_SWORD`, `MOVE_SUCKER_PUNCH`, `MOVE_PROTECT`
  - `SPECIES_CHANDELURE` — `ITEM_CHOICE_SCARF`, `ABILITY_FLASH_FIRE`; `MOVE_HEAT_WAVE`, `MOVE_SHADOW_BALL`, `MOVE_ENERGY_BALL`, `MOVE_TRICK`
  - `SPECIES_IRON_BUNDLE` — `ITEM_BOOSTER_ENERGY`, `ABILITY_QUARK_DRIVE`; `MOVE_FREEZE_DRY`, `MOVE_HYDRO_PUMP`, `MOVE_ICY_WIND`, `MOVE_PROTECT`
  - `SPECIES_GLASTRIER` — `ITEM_ASSAULT_VEST`, `ABILITY_CHILLING_NEIGH`; `MOVE_ICICLE_CRASH`, `MOVE_HIGH_HORSEPOWER`, `MOVE_CLOSE_COMBAT`, `MOVE_CRUNCH`

## 4. ELITE_FOUR_DRAKE

- Format: single; cap 80; offsets [1, 1, 1, 2, 2, 3].
- Primary question: Can the player exploit one visible Choice lock and shared Dragon weaknesses, then interrupt or reset Latios's Tailwind-or-Memento handoff before the sole four-attack special Mega Dragonite begins trading?
- Strongest part: Latios visibly spending Tailwind or Memento before an unexpected four-attack special Mega Dragonite turns the familiar all-Dragon weakness into a memorable category reversal.
- Weakest link: The Latios handoff and board-state reserve choice remain unimplemented; Memento can be cleared by switching and shared Fairy or Ice pressure may compress the roster, so target 10 depends on measured matchup play rather than the dossier alone.
- First-loss lesson: Identify Hydreigon as the one Choice lock, scout mixed screen-breaking Dragapult and anti-Fairy Dragalge, then treat Latios's Tailwind or Memento as the visible announcement of the sole climax. Switch to erase Memento drops if needed, break Multiscale, and answer Dragonite with priority, speed reversal, Ice or Fairy pressure, or special bulk.
- Resource tax: Drake enters fourth without automatic healing. Stealth Rock, one Scarf lock, mixed coverage, Dragalge's Fairy punishment, and the Latios-to-Mega handoff tax HP, hazard removal, scouting, denial PP, and the special wall or priority answer the player preserves before Wallace.
- References: `elite:giovanni-cischke:worlds-2025`, `smogon:gen8ou:002`, `showdown:gen9randomdoublesbattle:002`, `smogon:gen7uu:001`, `showdown:gen8randombattle:026`, `smogon:gen8ou:003`, `showdown:gen7randombattle:007`
- Exact team:
  - `SPECIES_GARCHOMP` — `ITEM_ROCKY_HELMET`, `ABILITY_ROUGH_SKIN`; `MOVE_STEALTH_ROCK`, `MOVE_EARTHQUAKE`, `MOVE_ROAR`, `MOVE_FIRE_FANG`
  - `SPECIES_DRAGAPULT` — `ITEM_EXPERT_BELT`, `ABILITY_CLEAR_BODY`; `MOVE_DRAGON_DARTS`, `MOVE_SHADOW_BALL`, `MOVE_FLAMETHROWER`, `MOVE_PSYCHIC_FANGS`
  - `SPECIES_HYDREIGON` — `ITEM_CHOICE_SCARF`, `ABILITY_LEVITATE`; `MOVE_DRACO_METEOR`, `MOVE_DARK_PULSE`, `MOVE_FLASH_CANNON`, `MOVE_U_TURN`
  - `SPECIES_LATIOS` — `ITEM_SOUL_DEW`, `ABILITY_LEVITATE`; `MOVE_DRACO_METEOR`, `MOVE_PSYSHOCK`, `MOVE_TAILWIND`, `MOVE_MEMENTO`
  - `SPECIES_DRAGALGE` — `ITEM_ASSAULT_VEST`, `ABILITY_ADAPTABILITY`; `MOVE_DRAGON_PULSE`, `MOVE_SLUDGE_BOMB`, `MOVE_SURF`, `MOVE_THUNDERBOLT`
  - `SPECIES_DRAGONITE` — `ITEM_DRAGONINITE`, `ABILITY_MULTISCALE`; Mega; `MOVE_DRAGON_PULSE`, `MOVE_AIR_SLASH`, `MOVE_FLAMETHROWER`, `MOVE_THUNDERBOLT`

## 5. CHAMPION_WALLACE

- Format: double; cap 80; offsets [2, 2, 3, 3, 3, 4].
- Primary question: Can the player dismantle Wallace's fast rain without spending the physical and field-control resources needed for his Trick Room and Mega Milotic endgame?
- Strongest part: The player can finally win the visible Kyogre-Tornadus speed war only to face a denyable Ogerpon-Palkia reversal, then must cash in the physical answer preserved for Mega Milotic.
- Weakest link: The two reserve pairs can feel manufactured and the Ferrothorn-Milotic finish can drag unless replacement choice remains board-aware and both members choose proactive pressure in runtime.
- First-loss lesson: Do not spend every answer merely surviving rain. Break or replace the fast field, deny Palkia's one reversal turn, and preserve a physical answer for Mega Milotic; Wallace is defeated by managing the transition, not by finding one anti-Water species.
- Resource tax: The fight taxes weather replacement, Wide Guard or spread mitigation, speed-control PP, physical Grass/Electric coverage, Taunt, item planning, and whatever Revives or healing the player chooses to spend after Drake. It does not erase the manual healing window.
- References: `elite:wolfe:toronto-2024`, `elite:wolfe:worlds-2016`, `elite:luca-ceribelli:worlds-2024`, `vgc:regional-houston-tx-2026`
- Exact team:
  - `SPECIES_KYOGRE` — `ITEM_MYSTIC_WATER`, `ABILITY_DRIZZLE`; `MOVE_WATER_SPOUT`, `MOVE_ORIGIN_PULSE`, `MOVE_THUNDER`, `MOVE_ICE_BEAM`
  - `SPECIES_TORNADUS_THERIAN` — `ITEM_FOCUS_SASH`, `ABILITY_REGENERATOR`; `MOVE_TAILWIND`, `MOVE_HURRICANE`, `MOVE_TAUNT`, `MOVE_U_TURN`
  - `SPECIES_OGERPON_WELLSPRING` — `ITEM_WELLSPRING_MASK`, `ABILITY_WATER_ABSORB`; `MOVE_IVY_CUDGEL`, `MOVE_FOLLOW_ME`, `MOVE_HORN_LEECH`, `MOVE_SPIKY_SHIELD`
  - `SPECIES_PALKIA` — `ITEM_LUSTROUS_ORB`, `ABILITY_PRESSURE`; `MOVE_TRICK_ROOM`, `MOVE_SPACIAL_REND`, `MOVE_HYDRO_PUMP`, `MOVE_PROTECT`
  - `SPECIES_FERROTHORN` — `ITEM_ROCKY_HELMET`, `ABILITY_IRON_BARBS`; `MOVE_POWER_WHIP`, `MOVE_GYRO_BALL`, `MOVE_KNOCK_OFF`, `MOVE_BODY_PRESS`
  - `SPECIES_MILOTIC` — `ITEM_MILOTICITE`, `ABILITY_COMPETITIVE`; Mega; `MOVE_BRINE`, `MOVE_MOONBLAST`, `MOVE_ICY_WIND`, `MOVE_RECOVER`

## Collision dispositions

- **SIGNATURE_MOVE_REPETITION — retain:** Repeated common competitive moves occupy materially different jobs: Tailwind is Sidney's midgame Dark tempo, Drake's visible singles handoff, and Wallace's lead speed mode; Icy Wind is Darkrai control, Iron Bundle control, and a conditional non-Trick-Room Milotic option; Knock Off belongs to position theft, trap transition, and proactive slow-mode pressure. Close Combat, Shadow Ball, Taunt, and U-turn are role-appropriate coverage or utility rather than repeated primary questions.
- **HISTORIC_REFERENCE_REUSE — retain:** The repeated references are used for different evidence. Sidney imports positioning and a single Kingambit endgame from Turin; Phoebe uses only survival-clock lessons. The shared random-doubles record informs Glacia's mode discipline and Drake's differentiated role sequencing, not an exact duplicated roster or core.
- **MODE_CLUSTER — retain-with-implementation-review:** Only Wallace owns active rain. Choice pressure is limited to one readable commitment in Sidney, Glacia, and Drake. Fast control appears in four anchors but at different moments and with different answers: a contestable middle Tailwind, active Icy Wind, a sacrificial singles handoff, and a lead Tailwind that may be reversed by Trick Room. Recheck the final Victory Road previous-ten window before implementation.

## Completion truth

The connected arc and all five exact teams are design-complete and statically legal. Game source, exact final dialogue, guide replacement, AI state machines, previous-ten implementation context, real-ROM behavior, and observed difficulty remain explicitly open until chronological implementation.
