# Verdant marquee battle design report

Current phase: `pokemon_league_main_story`

These are design dossiers, not source implementation or observed ROM results. Target difficulty is editorial until runtime playtesting.

## Current League mechanics baseline

- Strict Cap: `80`
- Level Semantics: `Trainer levels are offsets from the active strict cap.`
- Authored Difficulty Setting: `Hard`
- Medium Trainer Level Delta: `-2`
- Easy Trainer Level Delta: `-4`
- Difficulty Changes Only Opposing Trainer Levels: `True`
- Allowed Battle Transformations: `['Mega Evolution', 'Primal Reversion']`
- Forbidden Battle Transformations: `['Terastallization', 'Z-Moves', 'Dynamax', 'Gigantamax']`
- Automatic Healing Between Members: `False`
- Manual Overworld Bag Between Members: `True`
- Pokemon Menu Between Members: `True`
- Save Menu Between Members: `True`
- Items During Trainer Battles: `False`
- Party Composition Locked After Entry: `True`
- Drake Releases Player Control After Victory: `True`
- Hall4 Allows Normal Overworld Menu: `True`
- Champion Room Forces Approach After Entry: `True`
- Mechanics Proposal: `None`

## ELITE_FOUR_SIDNEY

- Status: `design-complete` / source `unimplemented` / runtime `unplayed`
- Format: `double`
- Strict cap: 80
- Target / observed difficulty: **10 / None**
- Memory hook: Sidney steals the opening, darkens the tempo, sends Mega Absol in to ruin the ordinary board, and lets the fallen empower one final king.

### Exact proposed team

| # | Pokémon | Offset | Item | Ability | Moves | Role |
| ---: | --- | ---: | --- | --- | --- | --- |
| 1 | `SPECIES_INCINEROAR` | +0 | `ITEM_EJECT_BUTTON` | `ABILITY_INTIMIDATE` | `MOVE_FAKE_OUT`, `MOVE_PARTING_SHOT`, `MOVE_FLARE_BLITZ`, `MOVE_KNOCK_OFF` | Opening information pivot; Fake Out creates the target dilemma while Parting Shot or Eject Button opens the visible-state reserve selector. |
| 2 | `SPECIES_URSHIFU` | +0 | `ITEM_CHOICE_BAND` | `ABILITY_UNSEEN_FIST` | `MOVE_WICKED_BLOW`, `MOVE_CLOSE_COMBAT`, `MOVE_IRON_HEAD`, `MOVE_U_TURN` | Choice-locked Protect breaker and second opening pivot; its first attack makes the commitment legible. |
| 3 | `SPECIES_YVELTAL` | +1 | `ITEM_ROSELI_BERRY` | `ABILITY_DARK_AURA` | `MOVE_TAILWIND`, `MOVE_OBLIVION_WING`, `MOVE_SNARL`, `MOVE_PROTECT` | First tempo and anti-reversal reserve; Roseli Berry plus special bulk lets Dark Aura, Snarl, Protect, and Oblivion Wing survive Fairy-centered Trick Room while Tailwind remains reserved for ordinary speed states. |
| 4 | `SPECIES_ABSOL` | +1 | `ITEM_ABSOLITE` | `ABILITY_SUPER_LUCK` | `MOVE_KNOCK_OFF`, `MOVE_PLAY_ROUGH`, `MOVE_WILL_O_WISP`, `MOVE_PROTECT` | Only Mega and stable physical-disruption phase; removes items, burns a visible physical threat, reflects status, and protects its frail body without competing for the climax. |
| 5 | `SPECIES_DARKRAI` | +1 | `ITEM_LUM_BERRY` | `ABILITY_BAD_DREAMS` | `MOVE_DARK_PULSE`, `MOVE_ICY_WIND`, `MOVE_TAUNT`, `MOVE_DISABLE` | Mythical control bridge that slows, denies setup, and disables one revealed key move without sleep or setup roulette. |
| 6 | `SPECIES_KINGAMBIT` | +2 | `ITEM_BLACK_GLASSES` | `ABILITY_SUPREME_OVERLORD` | `MOVE_KOWTOW_CLEAVE`, `MOVE_IRON_HEAD`, `MOVE_SWORDS_DANCE`, `MOVE_SUCKER_PUNCH` | Sole final setup climax; Supreme Overlord converts the visible allied faint count into a slow, answerable last stand. |

### Why this battle exists

- Primary question: Can the player solve the Incineroar and Urshifu target dilemma, contest Yveltal tempo, and preserve a Fighting, Ground, or boost-control answer for the final Supreme Overlord Kingambit?
- Primary mode: Fake Out plus Unseen Fist positioning offense that pivots into Yveltal Dark Aura and Tailwind.
- Secondary mode: Mega Absol removes items, burns a visible physical threat, and reflects status as one stable phase; Lum Darkrai always has Icy Wind or Dark Pulse pressure and may Taunt or Disable revealed counterplay, while Kingambit alone owns the setup climax and carries Iron Head for surviving Fairy pressure.
- Difficulty rationale: The player cap remains 80 while Sidney uses offsets 0, 0, +1, +1, +1, and +2. Difficulty comes from the opening target dilemma, interactable Tailwind, proactive Mega Absol disruption, Darkrai's revealed-move control bridge, and an Iron Head Supreme Overlord climax rather than a +4 Mega, generic coverage cannon, or three setup sweepers.
- First-loss lesson: Do not treat Protect as universal against the opener. Exploit Urshifu's lock, contest Tailwind, deny Mega Absol its visible disruption job, vary revealed moves so Darkrai cannot Disable the whole plan, and preserve Fighting, Ground, Haze, or phazing for the visibly empowered Iron Head Kingambit.
- Intentional weakness: Sidney has no redirection or screens. Only Urshifu is Choice-locked, Tailwind is interactable, Mega Absol is frail and selected only into one narrow visible job, Darkrai's strongest control depends on revealed moves and visible support, and Kingambit remains 4x weak to Fighting and must spend a visible turn to Swords Dance.

### AI and evidence

- State machine: [{'state': 'steal-position', 'condition': 'Opening Incineroar plus Urshifu board', 'behavior': 'Enumerate joint legal action and target pairs for Incineroar and Urshifu. Prefer pressure on two distinct opposing actions, allow a shared target for a knockout or superior visible board, and choose from Fake Out, Knock Off, Wicked Blow, Close Combat, Iron Head, U-turn, Flare Blitz, or Parting Shot after resolving redirection, immunity, Choice lock, Fairy knockout value, and Eject timing.'}, {'state': 'aura-tempo', 'condition': 'A reserve is required and healthy Yveltal either supplies absent-Tailwind tempo or the active-Trick-Room survival branch', 'behavior': 'Under ordinary speed state establish Tailwind when Yveltal survives. Under active Trick Room suppress Tailwind, use Roseli-backed Snarl, Protect, or Oblivion Wing to consume room turns and blunt Fairy pressure, then shape the next visible reserve decision.'}, {'state': 'mega-disruption', 'condition': 'Tailwind has been established or Yveltal has left, Absol survives the visible board, and a burnable physical threat or direct Knock Off or Play Rough knockout gives the stable phase value', 'behavior': 'Select Absol, Mega Evolve automatically, and choose Will-O-Wisp, Knock Off, Play Rough, or a genuinely defensive Protect.'}, {'state': 'revealed-control-bridge', 'condition': 'A revealed move merits Disable, visible support merits Taunt, Icy Wind changes move order, or no other non-Kingambit reserve has a qualifying visible role', 'behavior': 'Select Darkrai for revealed-information control or the proactive Icy Wind into Dark Pulse fallback; never invoke sleep or hidden coverage logic.'}, {'state': 'overlord-climax', 'condition': 'At least four Sidney allies have visibly fainted and no other healthy reserve exists, or Kingambit is the only legal replacement in an unusual simultaneous-faint or forced-replacement state', 'behavior': 'Select the final queued Kingambit and use the actual Supreme Overlord multiplier. Set Swords Dance only after full two-slot survival and revealed-disruption checks. Use Sucker Punch only for a visible priority knockout against an established damaging pattern and never repeat after failure; otherwise finish through Kowtow Cleave or Iron Head.'}]
- Selected references: `elite:marco-silva:turin-2026`, `vgc:regional-melbourne-2022`, `showdown:gen9randomdoublesbattle:016`, `showdown:gen6randomdoublesbattle:022`
- Required source work: Preserve the Incineroar plus Urshifu opening and enumerate every legal joint action and target pair, including Close Combat. Resolve terrain, Ghost immunity, redirection, Fake Out eligibility, Choice lock, knockout value, Iron Head Fairy pressure, and Eject Button timing before scoring; reward distinct pressure but let an Iron Head knockout or superior shared-target board override that bonus.; Implement the visible-state healthy-reserve selector exactly as recorded in ordering; raw array order is never sufficient.; Define survival against both opposing slots from revealed attacks, observed damage, visible field modifiers, and conservative public-species ranges. Prefer Yveltal while Tailwind is absent or Trick Room is active and this two-slot survival test passes. Never select Tailwind while Tailwind or Trick Room is active; under Trick Room score Snarl, Protect, and Oblivion Wing from summed visible damage and survival value.; Mega Absol must pass the same two-slot survival test and have a burnable physical threat established by a revealed physical action or a direct Knock Off or Play Rough knockout; Protect is defensive, never a selector excuse.; Darkrai must use Disable only against the target's revealed last move and Taunt only into visible setup, recovery, or support value. Icy Wind uses observed turn order and conservative public speed ranges and is suppressed under active Trick Room; direct Dark Pulse is the legal forced-fallback action. No sleep move, hidden speed read, or generic coverage lock exists.; Include the actual Supreme Overlord faint multiplier in damage estimates and withhold Kingambit until at least four visible allied faints with no other healthy reserve. Only an empty legal replacement set may trigger the emergency exception.; Evaluate Kingambit Swords Dance only when it survives the full visible action set of both opposing slots and no revealed Taunt, Encore, Haze, Clear Smog, phazing, or burn line invalidates the turn. Use Sucker Punch only for a visible priority knockout against an established damaging pattern; never read the chosen action or repeat it after failure. Prefer Iron Head or Kowtow Cleave against visible support or non-attacking patterns.; Regression-test Eject Button replacement timing and verify that a Yveltal switch-in applies Dark Aura to Urshifu's same-turn Wicked Blow without scripting the outcome.; Continue using automatic opponent Mega Evolution; Absol is the only Mega candidate.

### Author self-check

- Strongest part: The Eject Button transition can visibly turn an attack on Incineroar into Dark Aura pressure beside Urshifu, while the same early knockouts build toward a readable Supreme Overlord final king.
- Weakest link: The joint opening scorer and total visible-state reserve selector are substantial unimplemented AI work; the design is complete, but its target 10 remains unobserved until those behaviors and broad matchup families run in the ROM.

### Campaign reservations

- Spends: The main story marquee Yveltal and Darkrai appearances; Mega Absol as Sidney only Mega and proactive disruption phase; Kingambit late-game Supreme Overlord reveal as the sole climax; One justified League use of Incineroar and Urshifu as a championship positioning core
- Preserves: Dedicated Shadow Tag Perish and Mega Gengar for Phoebe; Snow, toxic or trapping attrition, and major Wide Guard pressure for Glacia; Dragon Mega and legendary-scale singles closer for Drake; Grand rain, dual-speed restricted balance, and Mega Milotic for Wallace
- Releases: Darkrai is released as a revealed-information control bridge after being withheld from the earliest campaign ordinary encounters.; The campaign may use a tournament-authentic Incineroar core here because this is one high-status League appearance, not a global allocator default.; Kingambit full Supreme Overlord identity is saved for the single final phase now that the player owns every broad counterplay class.

## ELITE_FOUR_PHOEBE

- Status: `design-complete` / source `unimplemented` / runtime `unplayed`
- Format: `double`
- Strict cap: 80
- Target / observed difficulty: **10 / None**
- Memory hook: Phoebe's spirits sing one shared countdown, hide the singer behind a living circle, and leave the player deciding who can still escape.

### Exact proposed team

| # | Pokémon | Offset | Item | Ability | Moves | Role |
| ---: | --- | ---: | --- | --- | --- | --- |
| 1 | `SPECIES_GENGAR` | +1 | `ITEM_GENGARITE` | `ABILITY_CURSED_BODY` | `MOVE_PERISH_SONG`, `MOVE_SHADOW_BALL`, `MOVE_DISABLE`, `MOVE_PROTECT` | Only Mega and only Shadow Tag user; starts, preserves, and later renews the clock. |
| 2 | `SPECIES_AMOONGUSS` | +1 | `ITEM_MENTAL_HERB` | `ABILITY_REGENERATOR` | `MOVE_RAGE_POWDER`, `MOVE_SPORE`, `MOVE_SYNTHESIS`, `MOVE_PROTECT` | The one off-type ritual guardian; leads beside the sole trapper so Rage Powder, Spore, and Protect turn the first clock into a targeting puzzle instead of a generic Gengar double-target. |
| 3 | `SPECIES_FLUTTER_MANE` | +1 | `ITEM_FOCUS_SASH` | `ABILITY_PROTOSYNTHESIS` | `MOVE_PERISH_SONG`, `MOVE_MOONBLAST`, `MOVE_SHADOW_BALL`, `MOVE_PROTECT` | Preserved second singer and immediate special pressure; enters through the synchronized first-cycle exit so the second clock is staged rather than merely promised. |
| 4 | `SPECIES_AEGISLASH` | +2 | `ITEM_LEFTOVERS` | `ABILITY_STANCE_CHANGE` | `MOVE_KINGS_SHIELD`, `MOVE_SUBSTITUTE`, `MOVE_SHADOW_BALL`, `MOVE_SACRED_SWORD` | Finite clock survival and mixed direct coverage without spending Glacia's later Wide Guard showcase. |
| 5 | `SPECIES_MIMIKYU` | +3 | `ITEM_SITRUS_BERRY` | `ABILITY_DISGUISE` | `MOVE_WILL_O_WISP`, `MOVE_TAUNT`, `MOVE_PLAY_ROUGH`, `MOVE_SHADOW_CLAW` | Disguise-backed disruption, persistent burn attrition, and direct physical pressure that marks the transition away from Protect cycling. |
| 6 | `SPECIES_MARSHADOW` | +4 | `ITEM_LIFE_ORB` | `ABILITY_TECHNICIAN` | `MOVE_SPECTRAL_THIEF`, `MOVE_SHADOW_SNEAK`, `MOVE_ICE_PUNCH`, `MOVE_CLOSE_COMBAT` | Four-attack spectral mythical cleanup that steals setup boosts and never retreats into the survival phase's Protect rhythm. |

### Why this battle exists

- Primary question: Can the player preserve an escape route or remove Mega Gengar before the shared Perish clock converts two active Pokemon into unavoidable resource loss?
- Primary mode: Mega Gengar Shadow Tag plus two Perish singers, finite redirection, and coordinated count-one exits.
- Secondary mode: Direct Ghost and Fairy pressure culminating in Marshadow's Spectral Thief and priority cleanup when the trap is broken.
- Difficulty rationale: The cap-80 party faces levels 81 through 84, a fast Mega Shadow Tagger, two legal Perish singers, Focus Sash, redirection, sleep, self-healing, Substitute and King's Shield survival, Disguise, burn, and a level-84 Marshadow that steals setup. The team is nearly overwhelming blind, but the entire clock has a deliberately attackable single point of failure.
- First-loss lesson: Count both sides, not just the player's clock. Preserve one escape class or double-target Mega Gengar on a non-Protect turn; once the single trapper is gone, Phoebe becomes a hard but conventional Ghost offense.
- Intentional weakness: Mega Gengar is the only Shadow Tag user. Removing it collapses the hard trap; Ghost-types can switch through Shadow Tag under the current Gen 7 rule, pivot moves and escape items preserve exits, and spread attacks bypass Rage Powder.

### AI and evidence

- State machine: [{'state': 'secure-or-break-escape', 'condition': 'Mega Gengar is not yet active, a foe is visibly Ghost-type, or an escape ability, item, or pivot move has already been revealed', 'behavior': 'Mega Evolve automatically; use Shadow Ball, Disable, Spore, Will-O-Wisp, Taunt, or direct focus fire until the clock can teach a fair actionable lesson. Never inspect an unrevealed player move or item for this transition.'}, {'state': 'start-clock', 'condition': "At least one living foe is unperished, Mega Gengar's Shadow Tag affects the relevant slots, and Gengar does not face two independent visible probable knockout lines", 'behavior': 'On the first cycle Gengar sings while Amoonguss redirects or disrupts; on the staged second cycle Flutter Mane sings as the survival partner switches to preserved Gengar. Never choose two songs or start a clock from a generic move preference alone.'}, {'state': 'survive-three-two', 'condition': 'A useful own-side Perish count is two or three', 'behavior': "Value Protect, Disable, Rage Powder, Spore, Synthesis, Substitute, King's Shield, and safe switching without forcing a deterministic sequence."}, {'state': 'exit-at-one', 'condition': "An AI Pokemon's count is one and a legal reserve exists", 'behavior': 'Coordinate both active slots and reserve destinations. On the first cycle prefer Gengar to Flutter Mane and Amoonguss to Aegislash, using the recorded unreserved fallbacks when damaged or unavailable. If trapped or no reserve exists, choose the highest-value attack or protection rather than pretending survival is possible.'}, {'state': 'second-circle', 'condition': 'The first opposing pair has fainted, active Flutter Mane is uncounted, preserved Mega Gengar is healthy enough to re-enter, and at least one new foe is not a revealed free escape', 'behavior': "Flutter Mane uses Perish Song while Aegislash or the active survival partner switches to Mega Gengar. The opponent's chosen active pair receives the song after Gengar restores Shadow Tag; preserve legal exits for Phoebe's count-one turn."}, {'state': 'direct-cleanup', 'condition': 'The trapper has fainted, revealed escape counterplay remains, or only damaged foes survive', 'behavior': 'Use the trainer-aware reserve selector to transition to non-Protect Mimikyu and four-attack Marshadow, with Flutter Mane or Aegislash attacking as appropriate, instead of wasting turns on an unenforceable clock.'}]
- Selected references: `elite:wolfe:orlando-2023`, `elite:marco-silva:turin-2026`, `showdown:gen7randomdoublesbattle:015`, `showdown:gen7randomdoublesbattle:018`, `showdown:gen8randomdoublesbattle:002`
- Required source work: Recognize Mega Gengar's post-Mega Shadow Tag before move selection and treat Gengar as the first-cycle singer and preserved Flutter Mane as the staged second-cycle singer.; Reject duplicate Perish Song when all living foes already have a count.; Attack, Disable, burn, or sleep visible Ghost-type escape pieces immediately. Treat Soundproof, pivot moves, Shed Shell, and Eject Button as escape information only after the ability, move, or item is revealed in battle; do not inspect hidden player data for the state machine.; Before either clock starts, evaluate Gengar against both opposing slots using only revealed attacks, observed damage, visible modifiers, and conservative public-species ranges. Perish Song is illegal when two independent probable knockout lines remain; use Gengar Protect plus Amoonguss Spore or direct disruption instead.; On the first clock, coordinate Gengar with Amoonguss: Perish Song plus Rage Powder against redirectable pressure, Protect plus Spore against two independent threats, and direct pressure against visible Ghost escape or revealed spread attacks. Rage Powder is never chosen into exclusively spread pressure.; At count one, switch every perishing AI Pokemon when a legal unreserved destination exists. Reserve Flutter Mane for Gengar and Aegislash for Amoonguss first, then use the total damaged-destination fallbacks recorded in ordering; current battle_ai_switch_items.c is only a baseline and does not prove synchronized doubles behavior.; After the first opposing pair falls, stage the second circle exactly: active Flutter Mane uses Perish Song while Aegislash or the current survival partner switches to preserved Mega Gengar. Do not drift into generic damage AI or claim a second cycle without this causal re-entry.; Add a Phoebe-specific phase selector that coordinates both replacement slots, tracks reserved destinations, preserves a singer when defensible, and chooses Mimikyu or Marshadow by visible physical or status pressure, a visible boost Spectral Thief can steal, or an immediate Shadow Sneak knockout when cleanup begins.; Use Spectral Thief preferentially against a boosted target and Shadow Sneak only when priority changes the knockout.

### Author self-check

- Strongest part: One Mega Gengar supports two staged Perish circles, yet removing that single visible trapper collapses the hard clock and forces Phoebe into a materially different direct-cleanup phase.
- Weakest link: Coordinated count-one exits, the returning-Gengar second circle, and the escape-aware fallback can become slow or brittle unless the source state machine is implemented and exercised against several visible escape plans.

### Campaign reservations

- Spends: The campaign's definitive full Shadow Tag Perish battle; Mega Gengar as Phoebe's only Mega; Flutter Mane as the second singer; Marshadow as the spectral mythical and anti-setup final pressure; One strategically justified off-type Amoonguss redirection slot
- Preserves: Sidney's Yveltal, Darkrai, Incineroar, Urshifu, Kingambit, and Mega Absol roster; Glacia's snow, offense-plus-attrition hybrid, and Wide Guard showcase; Drake's Dragon Mega and singles format shift; Wallace's rain, dual-speed, and grand restricted spectacle
- Releases: Battle 11's partial Celebi and Shelmet Perish lesson is allowed to escalate here into the one full League trap; species, setting, counterplay, and AI phase structure are entirely different.; Wolfe Glick's Orlando survival clock is spent only once, at a battle important enough to justify dedicated AI.; Mega Gengar's competitive Perish and Disable identity is finally used at the point where the player owns every broad counterplay class.

## ELITE_FOUR_GLACIA

- Status: `design-complete` / source `unimplemented` / runtime `unplayed`
- Format: `double`
- Strict cap: 80
- Target / observed difficulty: **10 / None**
- Memory hook: Mega Glalie visibly pressures with Double-Edge before its later detonation window, Toxapex traps and strips an item, and Glastrier closes the shattered field as the one late snowball.

### Exact proposed team

| # | Pokémon | Offset | Item | Ability | Moves | Role |
| ---: | --- | ---: | --- | --- | --- | --- |
| 1 | `SPECIES_GLALIE` | +2 | `ITEM_GLALITITE` | `ABILITY_REFRIGERATE` | `MOVE_DOUBLE_EDGE`, `MOVE_EXPLOSION`, `MOVE_ICE_SHARD`, `MOVE_PROTECT` | sole Mega, detonation lead, and priority breaker |
| 2 | `SPECIES_TOXAPEX` | +1 | `ITEM_LEFTOVERS` | `ABILITY_REGENERATOR` | `MOVE_BANEFUL_BUNKER`, `MOVE_INFESTATION`, `MOVE_TOXIC`, `MOVE_KNOCK_OFF` | trap clock, item-removal transition, and detonation shield |
| 3 | `SPECIES_CHIEN_PAO` | +2 | `ITEM_LIFE_ORB` | `ABILITY_SWORD_OF_RUIN` | `MOVE_ICICLE_CRASH`, `MOVE_SACRED_SWORD`, `MOVE_SUCKER_PUNCH`, `MOVE_PROTECT` | major Ice legendary and fast physical amplifier |
| 4 | `SPECIES_CHANDELURE` | +2 | `ITEM_CHOICE_SCARF` | `ABILITY_FLASH_FIRE` | `MOVE_HEAT_WAVE`, `MOVE_SHADOW_BALL`, `MOVE_ENERGY_BALL`, `MOVE_TRICK` | Fire absorption, Steel punishment, and Choice disruption |
| 5 | `SPECIES_IRON_BUNDLE` | +2 | `ITEM_BOOSTER_ENERGY` | `ABILITY_QUARK_DRIVE` | `MOVE_FREEZE_DRY`, `MOVE_HYDRO_PUMP`, `MOVE_ICY_WIND`, `MOVE_PROTECT` | Booster Energy special speed control and pre-climax pressure |
| 6 | `SPECIES_GLASTRIER` | +3 | `ITEM_ASSAULT_VEST` | `ABILITY_CHILLING_NEIGH` | `MOVE_ICICLE_CRASH`, `MOVE_HIGH_HORSEPOWER`, `MOVE_CLOSE_COMBAT`, `MOVE_CRUNCH` | bulky final Chilling Neigh snowball and true late climax |

### Why this battle exists

- Primary question: Can the player act on Glalie's explicit pre-selection crack warning, break Toxapex's trap-and-item transition, and still preserve an answer for final Chilling Neigh Glastrier?
- Primary mode: Mega Glalie and Toxapex open together, but Explosion is forbidden on global turn zero. After Glalie completes its first action, a one-time native battle message announces that its frozen body is cracking before the next move-selection window; only after that actionable cue can the AI consider Explosion beside Baneful Bunker.
- Secondary mode: Lower-offset Chien-Pao and Choice Scarf Chandelure provide distinct physical amplification and special Choice disruption, Booster Iron Bundle supplies active Icy Wind control, and bulky Assault Vest Glastrier is the real late Chilling Neigh climax.
- Difficulty rationale: Six optimized level-81 to level-83 Pokemon create a readable multi-turn detonation, a trap and item-removal bridge, differentiated fast physical and special pressure, active speed control, and one late Chilling Neigh closer. The reduced offsets make the claimed 10 depend on decisions and mode transitions rather than raw levels.
- First-loss lesson: Use the guaranteed first action to identify Glalie and Toxapex choices, then watch Baneful Bunker as the detonation tell. Force Toxapex to transition without value, control Iron Bundle speed, and keep burn, Intimidate, or a Fighting or Steel answer for Glastrier.
- Intentional weakness: Explosion is unavailable on turn zero and later telegraphed by protection; Toxapex has no recovery move and must progress through a legal trap, poison, or Knock Off; the fast reserves have no Tailwind or Trick Room; Chien-Pao exposes its ally; Chandelure is Choice-locked; Iron Bundle consumes one Booster; and final Glastrier is slow, physically oriented, and vulnerable to burn, Intimidate, Fighting, Steel, Fire, Rock, and focused special damage.

### AI and evidence

- State machine: Mode A is a readable lead: turn zero permits Mega Double-Edge, Ice Shard, or Protect but never Explosion, while Toxapex establishes one trap, poison, or Knock Off transition. After that first action, the game shows the one-time crack message before the player chooses the next turn; only then can Baneful Bunker coordinate a safe detonation. Mode B selects Chien-Pao, Scarf Chandelure, or Booster Iron Bundle from the current board for distinct physical, special, or speed-control pressure. Mode C preserves Glastrier as the default final climax and uses Chilling Neigh to convert one earned knockout into the closing snowball.
- Selected references: `elite:wolfe:naic-2026`, `showdown:gen4randomdoublesbattle:008`, `showdown:gen9randomdoublesbattle:014`, `showdown:gen9randomdoublesbattle:002`, `showdown:gen6randomdoublesbattle:026`, `elite:shohei-kimura:worlds-2023`
- Required source work: Forbid Explosion on global turn zero. After Glalie completes its first action, set a one-time trainer state and display a native 'Glalie's frozen body is cracking!' battle message before the next player action-selection window. Explosion remains illegal to the AI until that cue has been shown.; After the first crack cue, reward Explosion only when Toxapex or the current ally has selected protection or is immune and the expected two-target payoff justifies losing Glalie; do not repeat the cue or force detonation.; Select at most one valuable Infestation or Toxic clock, use Knock Off once it removes a meaningful item or opens the next attacker, and recognize Baneful Bunker as a visible safe-detonation signal rather than a forced loop.; Permit Toxapex to switch for Regenerator after it has trapped, poisoned, or removed an item and a reserve can exploit the new board; do not preserve it passively after its transition is complete.; Use Icy Wind when Iron Bundle actually changes move order, and treat Protect as scouting rather than a substitute for useful damage.; Keep healthy Glastrier as the preferred late climax, but release it earlier only when the visible board offers a concrete Chilling Neigh or survival advantage.

### Author self-check

- Strongest part: The one-time crack cue makes Mega Glalie's later Refrigerate detonation actionable before move selection, and final Glastrier gives the shattered board one distinct late snowball rather than another interchangeable fast Ice attacker.
- Weakest link: The battle depends on a bespoke cue and no-turn-zero detonation gate; the middle Chien-Pao, Chandelure, and Iron Bundle selection must prove distinct and Glastrier must prove climactic rather than merely slow in runtime.

### Campaign reservations

- Spends: Mega Glalie as Glacia's signature and only Mega; Chien-Pao as the League's major Ice legendary; the campaign's definitive readable detonation plus trapping-clock battle; Booster Energy Iron Bundle speed control and Glastrier as the late Ice climax
- Preserves: dedicated Perish and Shadow Tag for Phoebe and selected Aqua command fights; Dragon singles lure sequencing and Mega Dragonite for Drake; Wallace's rain and opposite-speed Champion transition; Dark information warfare for Sidney
- Releases: Abomasnow, Ninetales-Alola, Kyurem, Articuno, Glastrier, and Aurora Veil are not locked to Glacia and may be used elsewhere in distinct roles.; The exact Wolfe NAIC roster remains available as a historical reference because Glacia imports its mode discipline rather than copying its six species.; Mamoswine is released from Glacia because its role overlapped Chien-Pao; it remains available for another Wolfe-derived battle.

## ELITE_FOUR_DRAKE

- Status: `design-complete` / source `unimplemented` / runtime `unplayed`
- Format: `single`
- Strict cap: 80
- Target / observed difficulty: **10 / None**
- Memory hook: Garchomp marks the field, mixed Dragapult and Scarf Hydreigon force different defenses, anti-Fairy Dragalge clears the lane, and Latios visibly sacrifices its momentum so special Mega Dragonite can become the one true climax.

### Exact proposed team

| # | Pokémon | Offset | Item | Ability | Moves | Role |
| ---: | --- | ---: | --- | --- | --- | --- |
| 1 | `SPECIES_GARCHOMP` | +1 | `ITEM_ROCKY_HELMET` | `ABILITY_ROUGH_SKIN` | `MOVE_STEALTH_ROCK`, `MOVE_EARTHQUAKE`, `MOVE_ROAR`, `MOVE_FIRE_FANG` | hazard lead, contact tax, reliable phazer, and accurate Fire coverage into Steel |
| 2 | `SPECIES_DRAGAPULT` | +1 | `ITEM_EXPERT_BELT` | `ABILITY_CLEAR_BODY` | `MOVE_DRAGON_DARTS`, `MOVE_SHADOW_BALL`, `MOVE_FLAMETHROWER`, `MOVE_PSYCHIC_FANGS` | flexible mixed breaker and proactive screen-breaking commitment test |
| 3 | `SPECIES_HYDREIGON` | +1 | `ITEM_CHOICE_SCARF` | `ABILITY_LEVITATE` | `MOVE_DRACO_METEOR`, `MOVE_DARK_PULSE`, `MOVE_FLASH_CANNON`, `MOVE_U_TURN` | special speed check, Fairy lure, and second pivot |
| 4 | `SPECIES_LATIOS` | +2 | `ITEM_SOUL_DEW` | `ABILITY_LEVITATE` | `MOVE_DRACO_METEOR`, `MOVE_PSYSHOCK`, `MOVE_TAILWIND`, `MOVE_MEMENTO` | visible attacking bridge that can spend itself to create the sole Mega climax |
| 5 | `SPECIES_DRAGALGE` | +2 | `ITEM_ASSAULT_VEST` | `ABILITY_ADAPTABILITY` | `MOVE_DRAGON_PULSE`, `MOVE_SLUDGE_BOMB`, `MOVE_SURF`, `MOVE_THUNDERBOLT` | accurate special sponge and anti-Fairy wallbreaker |
| 6 | `SPECIES_DRAGONITE` | +3 | `ITEM_DRAGONINITE` | `ABILITY_MULTISCALE` | `MOVE_DRAGON_PULSE`, `MOVE_AIR_SLASH`, `MOVE_FLAMETHROWER`, `MOVE_THUNDERBOLT` | sole Champions Mega showcase and four-attack special-category reversal with no setup or recovery loop |

### Why this battle exists

- Primary question: Can the player exploit one visible Choice lock and shared Dragon weaknesses, then interrupt or reset Latios's Tailwind-or-Memento handoff before the sole four-attack special Mega Dragonite begins trading?
- Primary mode: Rough Skin Garchomp establishes Stealth Rock or Roar pressure, mixed Expert Belt Dragapult attacks without a Choice lock, and Choice Scarf Hydreigon supplies the roster's one readable speed commitment.
- Secondary mode: Assault Vest Dragalge punishes Fairy offense, then Soul Dew Latios either attacks or spends a visible Tailwind/Memento handoff into a four-attack Mega Dragonite with no setup or recovery loop.
- Difficulty rationale: Six optimized level-81 to level-83 Dragons create hazards, mixed damage, one exploitable Choice lock, anti-Fairy pressure, and one visible Latios-to-Mega-Dragonite climax. No Power Construct, paralysis chain, setup, recovery loop, or second ace remains; target 10 must come from preservation, denying or resetting the handoff, and answering the four-attack special category reversal.
- First-loss lesson: Identify Hydreigon as the one Choice lock, scout mixed screen-breaking Dragapult and anti-Fairy Dragalge, then treat Latios's Tailwind or Memento as the visible announcement of the sole climax. Switch to erase Memento drops if needed, break Multiscale, and answer Dragonite with priority, speed reversal, Ice or Fairy pressure, or special bulk.
- Intentional weakness: Every member remains a Dragon, only Hydreigon is Choice-locked, Dragapult has no pivot, setup, or defensive item, Garchomp and Dragalge use reliable but lower-ceiling coverage, Latios's Memento can be erased by switching, and special Mega Dragonite has no setup, recovery, priority, or second life after Multiscale breaks.

### AI and evidence

- State machine: Phase A fixes Garchomp as the visible lead and conditionally establishes hazards or phazing. Phase B selects mixed no-pivot Dragapult for flexible damage and screen breaking, Scarf Hydreigon for the sole pivot lock, or Dragalge for Fairy and special pressure. Phase C is the visible handoff: with Dragonite the final healthy reserve, Latios attacks, uses Tailwind, or conditionally sacrifices itself with Memento. Phase D Mega Evolves Dragonite as the sole climax and attacks immediately; the player can switch to erase Memento drops, stall or reverse Tailwind, break Multiscale, use priority, or rely on special bulk.
- Selected references: `elite:giovanni-cischke:worlds-2025`, `smogon:gen8ou:002`, `showdown:gen9randomdoublesbattle:002`, `smogon:gen7uu:001`, `showdown:gen8randombattle:026`, `smogon:gen8ou:003`, `showdown:gen7randombattle:007`
- Required source work: Prefer Stealth Rock on turn zero only when Garchomp survives and hazards have value; use Roar against visible setup or a switch worth taxing, never as an accuracy gamble.; Treat Hydreigon as the one Choice-locked pivot and use U-turn or switching to escape an immunity; Dragapult remains flexible and should choose physical or special coverage from the visible matchup.; Select Dragalge when Fairy pressure or special bulk matters, using accurate Surf and Thunderbolt rather than gambling on Hydro Pump or Focus Blast.; Select Latios as the late bridge only when Dragonite remains the final healthy reserve. Use Tailwind when its remaining turns can matter; use Memento only when Latios is expendable, the active target is not already minimized, and Dragonite can safely exploit the weakened board.; Preserve healthy Mega Dragonite as the sole final climax and Mega Evolve immediately on entry. Its checked-in Mega form is Dragon/Flying with HP 91, Attack 124, Defense 115, Speed 100, Special Attack 145, Special Defense 125, and Multiscale in every slot; choose among four attacks with no setup or recovery.

### Author self-check

- Strongest part: Latios visibly spending Tailwind or Memento before an unexpected four-attack special Mega Dragonite turns the familiar all-Dragon weakness into a memorable category reversal.
- Weakest link: The Latios handoff and board-state reserve choice remain unimplemented; Memento can be cleared by switching and shared Fairy or Ice pressure may compress the roster, so target 10 depends on measured matchup play rather than the dossier alone.

### Campaign reservations

- Spends: Mega Dragonite as Drake's signature, only Mega, and sole climax; Latios as Drake's visible Tailwind-or-Memento handoff legend; the League's deliberate singles format shift; the campaign's definitive all-Dragon preservation-to-handoff test
- Preserves: Wallace's rain and opposite-speed Champion transition; Glacia's Mega detonation and trapping clock; dedicated Perish and Shadow Tag for Phoebe; Mega Salamence, Dialga, Reshiram, Naganadel, and Rayquaza for distinct later roles rather than stacking them here
- Releases: The old five-legendary Drake roster, utility Power Construct, and three simultaneous setup clocks are released.; Mega Salamence is not locked to Drake and may appear elsewhere if its role does not duplicate Mega Dragonite.; Thousand Arrows is not reserved until its player-facing learnset source is made legal and explicitly approved.; Zygarde is fully released from Drake; Mega Dragonite owns the climax alone.

## CHAMPION_WALLACE

- Status: `design-complete` / source `unimplemented` / runtime `unplayed`
- Format: `double`
- Strict cap: 80
- Target / observed difficulty: **10 / None**
- Memory hook: Wallace opens a royal rain assault whose speed depends on an interactable Tailwind, reverses the board behind Ogerpon, then closes with an aggressive Ferrothorn and Mega Milotic pair.

### Exact proposed team

| # | Pokémon | Offset | Item | Ability | Moves | Role |
| ---: | --- | ---: | --- | --- | --- | --- |
| 1 | `SPECIES_KYOGRE` | +2 | `ITEM_MYSTIC_WATER` | `ABILITY_DRIZZLE` | `MOVE_WATER_SPOUT`, `MOVE_ORIGIN_PULSE`, `MOVE_THUNDER`, `MOVE_ICE_BEAM` | rain engine and HP-sensitive spread breaker whose speed is interactable |
| 2 | `SPECIES_TORNADUS_THERIAN` | +2 | `ITEM_FOCUS_SASH` | `ABILITY_REGENERATOR` | `MOVE_TAILWIND`, `MOVE_HURRICANE`, `MOVE_TAUNT`, `MOVE_U_TURN` | rain-perfect Flying pressure, speed control, denial, and pivot |
| 3 | `SPECIES_OGERPON_WELLSPRING` | +3 | `ITEM_WELLSPRING_MASK` | `ABILITY_WATER_ABSORB` | `MOVE_IVY_CUDGEL`, `MOVE_FOLLOW_ME`, `MOVE_HORN_LEECH`, `MOVE_SPIKY_SHIELD` | offensive redirection bridge that protects the mode transition |
| 4 | `SPECIES_PALKIA` | +3 | `ITEM_LUSTROUS_ORB` | `ABILITY_PRESSURE` | `MOVE_TRICK_ROOM`, `MOVE_SPACIAL_REND`, `MOVE_HYDRO_PUMP`, `MOVE_PROTECT` | single slow-mode setter and rain-boosted legendary breaker |
| 5 | `SPECIES_FERROTHORN` | +3 | `ITEM_ROCKY_HELMET` | `ABILITY_IRON_BARBS` | `MOVE_POWER_WHIP`, `MOVE_GYRO_BALL`, `MOVE_KNOCK_OFF`, `MOVE_BODY_PRESS` | slow rain-protected four-attack anchor and physical contact tax |
| 6 | `SPECIES_MILOTIC` | +4 | `ITEM_MILOTICITE` | `ABILITY_COMPETITIVE` | `MOVE_BRINE`, `MOVE_MOONBLAST`, `MOVE_ICY_WIND`, `MOVE_RECOVER` | specially fortified proactive Mega ace with reliable cleanup and one recovery tool |

### Why this battle exists

- Primary question: Can the player dismantle Wallace's fast rain without spending the physical and field-control resources needed for his Trick Room and Mega Milotic endgame?
- Primary mode: Mystic Water Kyogre and Tornadus-Therian create immediate rain, HP-sensitive Water Spout, accurate Hurricane and Thunder, and an interactable Tailwind speed advantage.
- Secondary mode: Trainer-aware reserve selection pairs Wellspring Ogerpon with Palkia for a denyable Trick Room reversal, then keeps Ferrothorn beside a proactive Mega Milotic endgame.
- Difficulty rationale: Six fully authored level-82 to level-84 Pokemon create opposite speed modes, weather amplification, redirection, a trainer-aware two-pair transition, and a physically exploitable but specially fortified Mega ace. Kyogre no longer outruns the player for free, and the endgame attacks proactively instead of relying on a Calm Mind and burn loop.
- First-loss lesson: Do not spend every answer merely surviving rain. Break or replace the fast field, deny Palkia's one reversal turn, and preserve a physical answer for Mega Milotic; Wallace is defeated by managing the transition, not by finding one anti-Water species.
- Intentional weakness: Kyogre and Tornadus expose one fixed lead with no Fake Out or automatic Scarf advantage, Palkia is the only Trick Room setter, spread damage bypasses Ogerpon, weather replacement and Wide Guard sharply reduce the fast mode, and Prism Scales leaves Mega Milotic vulnerable to preserved physical Electric, Grass, and Poison pressure without random Scald burns.

### AI and evidence

- State machine: Mode A opens with rain plus interactable Tailwind, Taunt, Hurricane, and high-HP Water Spout. The reserve selector then forms Ogerpon plus Palkia: Mode B becomes available only when the player wins the speed war or Wallace's slow pieces dominate, and Follow Me protects only a valuable Trick Room turn. The selector forms Ferrothorn plus Milotic for Mode C; both apply direct pressure, Milotic uses Icy Wind only outside Trick Room and Recover only when it survives the board.
- Selected references: `elite:wolfe:toronto-2024`, `elite:wolfe:worlds-2016`, `elite:luca-ceribelli:worlds-2024`, `vgc:regional-houston-tx-2026`
- Required source work: A reusable dual-speed selector must choose Trick Room only when the opposing effective board is faster or Wallace's slow reserves dominate; it must not reverse Wallace's active Tailwind advantage.; A Wallace-specific reserve selector must pair healthy Ogerpon with healthy Palkia for the mode transition and healthy Ferrothorn with healthy Milotic for the climax; generic replacement scoring is insufficient.; Follow Me should gain value when Palkia is choosing Trick Room, without forcing Ogerpon to redirect into obvious spread damage.; Water Spout scoring must fall with Kyogre's HP and prefer Origin Pulse when appropriate.; Milotic must avoid Icy Wind during active Trick Room, use Brine proactively against damaged targets, Recover only on a survivable board, and Mega Evolve as Wallace's sole Mega.

### Author self-check

- Strongest part: The player can finally win the visible Kyogre-Tornadus speed war only to face a denyable Ogerpon-Palkia reversal, then must cash in the physical answer preserved for Mega Milotic.
- Weakest link: The two reserve pairs can feel manufactured and the Ferrothorn-Milotic finish can drag unless replacement choice remains board-aware and both members choose proactive pressure in runtime.

### Campaign reservations

- Spends: Mega Milotic as Wallace's signature and only Mega; Kyogre as the Champion's Hoenn weather legend; Palkia as the Water/Dragon slow-mode legend; Tornadus-Therian and Wellspring Ogerpon as the modern rain-positioning reveal; the campaign's grand dual-speed rain identity
- Preserves: dedicated Perish and Shadow Tag for Phoebe and selected Aqua command fights; Ice stall-offense and toxic trapping for Glacia; Dark information warfare for Sidney; Dragon singles lure sequencing for Drake; Primal Reversion remains reserved for Magma and Aqua bosses; Tera, Z-Moves, and Dynamax remain absent
- Releases: Zapdos, Manaphy, Arceus-Water, Latios, Omastar, and generic rain sweepers are not locked to Wallace and may be used elsewhere if their roles differ.; The exact Wolfe Toronto roster remains available for an Aqua adaptation because Wallace imports its mode discipline rather than its species.

## Cross-dossier collision review

- Hard errors: 0
- Unique species: 30
- Formats: {'double': 4, 'single': 1}
- Advisory `SIGNATURE_MOVE_REPETITION`: `{"MOVE_CLOSE_COMBAT": ["ELITE_FOUR_GLACIA", "ELITE_FOUR_PHOEBE", "ELITE_FOUR_SIDNEY"], "MOVE_ICY_WIND": ["CHAMPION_WALLACE", "ELITE_FOUR_GLACIA", "ELITE_FOUR_SIDNEY"], "MOVE_KNOCK_OFF": ["CHAMPION_WALLACE", "ELITE_FOUR_GLACIA", "ELITE_FOUR_SIDNEY"], "MOVE_SHADOW_BALL": ["ELITE_FOUR_DRAKE", "ELITE_FOUR_GLACIA", "ELITE_FOUR_PHOEBE"], "MOVE_TAILWIND": ["CHAMPION_WALLACE", "ELITE_FOUR_DRAKE", "ELITE_FOUR_SIDNEY"], "MOVE_TAUNT": ["CHAMPION_WALLACE", "ELITE_FOUR_PHOEBE", "ELITE_FOUR_SIDNEY"], "MOVE_U_TURN": ["CHAMPION_WALLACE", "ELITE_FOUR_DRAKE", "ELITE_FOUR_SIDNEY"]}`
- Advisory `HISTORIC_REFERENCE_REUSE`: `{"elite:marco-silva:turin-2026": ["ELITE_FOUR_PHOEBE", "ELITE_FOUR_SIDNEY"], "showdown:gen9randomdoublesbattle:002": ["ELITE_FOUR_DRAKE", "ELITE_FOUR_GLACIA"]}`
- Advisory `MODE_CLUSTER`: `{"choice-pressure": ["ELITE_FOUR_DRAKE", "ELITE_FOUR_GLACIA", "ELITE_FOUR_SIDNEY"], "fast-control": ["CHAMPION_WALLACE", "ELITE_FOUR_DRAKE", "ELITE_FOUR_GLACIA", "ELITE_FOUR_SIDNEY"], "weather-rain": ["CHAMPION_WALLACE", "ELITE_FOUR_GLACIA", "ELITE_FOUR_PHOEBE", "ELITE_FOUR_SIDNEY"]}`

No dossier here authorizes a game-source or campaign-mechanics change.
