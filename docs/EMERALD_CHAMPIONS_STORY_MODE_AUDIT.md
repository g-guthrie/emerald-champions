# Emerald Champions Story-Mode Audit

This is the source-audit ledger for the Hoenn campaign. It records the
deterministic checks used to follow the story from New Game through the
Pokémon League and postgame, including optional trainers, NPCs, signs, map
transitions, one-time rewards, legendary encounters, and the Battle Frontier.
It is a description of checked source contracts, not a fresh-save playthrough
or a release-ready declaration.

## Audited surface

- 540 Hoenn maps
- 4,086 NPC, trigger, and sign events
- 1,402 map warps
- 17,938 script control-flow, dialogue, and movement references across 104,701
  assembled script lines
- 391 value-returning `specialvar` calls checked against non-void C functions
- 10,846 story-dialogue visual lines and 47,623 literal Hoenn dialogue lines
- 513 physical trainer encounters containing 561 reachable battle branches
- Every badge, HM license, Mega Ring, League, fossil, legendary-sign, and
  critical story-item progression contract

The automated source-and-artifact gate is
`scripts/verify_emerald_champions_release.py`. It runs 25 named source gates,
checks materialized trainer data and state IDs, rejects stale build artifacts,
and validates ROM identity and memory layout. It does not run the curated
runtime suite itself. GitHub CI separately builds `pokeemerald-release.gba`,
runs that verifier, performs a first-VBlank smoke, and invokes
`scripts/run_emerald_champions_runtime_gates.py`.

The current runtime manifest names 19 filters with a summed minimum of 256
selected tests. The frozen integration tree completed 256/256 in one
invocation, with zero known failures, TODOs, ordinary failures, or assumption
failures. That result proves the named mechanics, not campaign playthrough.

## Progression defects corrected

- Briney's Slateport service now requires Brawly and the actual Mega Ring.
- The Devon Mega reward checks the actual Mega Ring rather than a proxy flag.
- The National Dex is available from the initial Pokédex; the postgame scene
  is now a research sync instead of a contradictory unlock.
- The Sootopolis expert no longer blocks Cave of Origin after Juan.
- Pecharunt enforces both the Champion and Okidogi party requirements.
- Letter, Devon Parts, S.S. Ticket, Dowsing Machine, Powder Jar, Go-Goggles,
  Fly, Devon Scope, and Magma Emblem delivery is retry-safe when the Bag is
  full, using the PC where the native item contract permits it.
- Save migration is versioned through `VAR_EMERALD_CHAMPIONS_SAVE_VERSION`.
  Version-1 and native unversioned e7 saves preserve current state; raw 81e
  saves without the colliding Zygarde marker migrate their stable Sign prefix,
  lifetime Circuit wins, and difficulty while clearing colliding reward and
  trainer bits and rebuilding physical Sign flags. The byte-identical
  81e-Zygarde/e7-upgrade overlap and other ambiguous layouts fail safe to a
  playable Hard state instead of guessing. `SAVE_COMPATIBILITY.md` records the
  bounded contract.
- Legacy Linking Cord stacks move from the Items pocket into one reusable Key
  Item without risking loss when the Key Item pocket is full.
- Field HMs require the matching badge and the actual campaign HM license;
  early move-tutor access no longer bypasses story progression.
- The Pokémon League entrance explicitly checks all eight badge flags.
- Steven now holds Route 120's one-tile bridge until Winona is defeated, with
  Tate and Liza and Juan carrying defense-in-depth Feather Badge checks. This
  closes the native sequence break that previously allowed 200 late encounters
  to be reached while the first-unset-badge cap remained 55.
- Birch's redundant repeat-Johto-starter prize is now a retry-safe Legendary
  Form Research Kit for postgame Kyurem, Zygarde, Necrozma, and Calyrex tools.

## Battle-flow defects corrected

- Every Elite Four room and Wallace now has a native, explicit retirement
  route when fewer than two usable Pokémon remain. It preserves the intended
  attrition rules: no automatic healing, manual Bag healing between rooms,
  locked progression doors, and no Bag use during competitive battles.
- The Museum and Space Center chained fights can continue correctly when the
  player reaches a doubles battle with only one usable Pokémon.
- The Winstrate family no longer creates an unintended doubles attrition lock.
- Match Call remains useful for contact and flavor but never promises disabled
  rematches. Repurposed trainer IDs cannot leak into rematch reporting.
- Norman's accidentally reachable postgame rematch state is migrated back to
  the normal post-battle state; every retained legacy rematch block now has no
  physical entry and remains behind the disabled runtime guard.
- Live Hard/Medium/Easy difficulty also applies to Champions Circuit teams.
- Regional rival replacements respect preset legality and Item Clause.
- Battle-ready wild catches retain their authored held item when entering the
  active party. A party member boxed during catch-and-swap is restored to its
  battle-start loadout before the PC copy is made.
- Random-table Ultra Beasts and Paradox Pokémon receive competitive presets
  just like other ordinary wild encounters; true one-off legendary and
  temporary battle forms remain excluded unless their quest opts in.
- Conditional wild Legendary Signs awaken in their own habitat when the same
  badge, story, and partner requirements are met. Devon remains an optional
  remote oracle rather than mandatory backtracking.
- Arceus mastery requires all 81 other finite Sign definitions, including
  ordinary wilds, Game Corner prizes, Phione breeding, Circuit rewards, and
  the Eternatus mastery milestone.

## Encounter and reward defects corrected

- Regis, Groudon, Kyogre, Rayquaza, Southern Latias/Latios, Ho-Oh, Lugia, Mew,
  and Deoxys complete only on capture; a knockout cannot permanently consume
  the encounter.
- Static legendary levels now obey the intended cap-relative policy, with the
  fixed level-100 Southern Island encounter retained deliberately.
- All eleven complete fossils use a native scrolling menu, map to the correct
  species, remove the fossil only after successful delivery, and preserve
  older Root/Claw mid-revival saves.
- Finite reward scripts are retry-safe. Infinite or duplicate Mega Stone
  rewards were removed, and the 24 overworld Mega Stone pickups are unique.
- Poké Marts follow campaign progression, and Rare Candy has no acquisition
  path because the reusable Leveler owns all no-grind leveling. Free
  competitive held items remain at the Pokémon Center vendor; berries and
  transformation/progression items remain excluded.
- X-items were removed from Hoenn shops and world pickups. Those slots now
  hold healing, PP, capture, evolution, or one-time progression rewards.
- The Game Corner's obsolete TM reward path was replaced by a transactional
  Starter Archive; every unchosen regional starter is a one-time, preset-ready
  prize, while Genesect, Poipole, and decoration prizes remain intact.

## Narrative and presentation corrections

- Major story dialogue now agrees on the Magma/Aqua plot, Mega access,
  legendary-sign network, Rayquaza resolution, League rules, and Frontier.
- False badge-stat claims, inaccessible New Mauville guidance, obsolete TM
  promises, item-taxonomy errors, and several location/grammar mistakes were
  corrected.
- Gym students and guides describe the mechanics that their current teams
  actually teach.
- Route signs, NPC dialogue, trainer dialogue, and tutor descriptions are
  measured against the native font and window dimensions; overflowing literal
  lines fail the release gate.
- Devon's Pidgeotite reward, save flag, labels, and Match Call state now use
  the same terminology, and the campaign battle master follows live chronology
  through the Elite Four, Wallace, and postgame.
- Forty-one optional-route encounters now use their true earliest reachable
  caps on Routes 115–118, southern/perimeter Route 111, and Mt. Chimney. Teams
  moved earlier use legal evolution phases, reserve Megas until the bracelet,
  and retain all 92 Mega showcases elsewhere in the campaign.

## Inherited upstream defects corrected or contained

- Direct interaction with a reloaded, defeated buried trainer can no longer
  dispatch beyond its four-entry movement-state table.
- Keldeo now enters and leaves Resolute Form consistently when Secret Sword is
  learned or forgotten through party, PC, tutor, deleter, wild, and preset
  paths.
- Unsafe Battle Frontier recording and replay is hidden pending a proven
  upstream repair; save-layout support remains intact.
- Pokémon Storage held-item messages reserve an EOS byte and select a native
  fitting font for every legal item name.
- Pledge coordination and Decorate targeting have explicit doubles-AI rules.
  The curated manifest also selects copied-move targeting, Imposter AI, Sleep
  Clause re-entry, and Billy's Imposter lead with no accepted TODO or
  known-failing debt. These are focused regressions, not exhaustive proof of
  every Transform state.
- Commander preserves swallowed Tatsugiri visibility while sleep/Yawn state
  progresses correctly. Forecast, Flower Gift, Skill Swap reconciliation, and
  simultaneous modern manual-switch order have focused fixes.
- The optional post-KO idle/cry replay is disabled to avoid later affine
  animation corruption.

Known upstream areas deliberately not rewritten without a proven target are
fainted-replacement ordering, full cross-target move-end interleaving, broader
mid-turn AI-cache behavior beyond the two named switch-in reset regressions,
missing authored Gen 8/9 second frames, Terapagos's two-pixel raster defect,
and the broad doubles-animation backlog.

## Battle-master guarantees

The canonical campaign document is
`docs/emerald_champions_master_battle_design.txt`. Its encounter IDs are
contiguous and its trainer branches must materialize exactly into
`src/data/trainers.party`. The audit rejects unknown or illegal moves, invalid
Stat Points, impossible early evolutions, duplicate party species/items,
multiple Mega Stones, pre-bracelet Megas, broken Choice/Assault Vest sets,
dead item-dependent Abilities, nonfunctional Helping Hand in singles, missing
Gym typing, exact duplicate teams, missing campaign trainers, and missing Mega
or legendary showcases.

The master also enforces the 41 optional-route earliest-cap contracts and
milestone chronology: Mt. Pyre precedes the Magma and Aqua hideouts; Space
Center precedes Seafloor Cavern and Sootopolis; Sidney, Phoebe, Glacia, Drake,
and Wallace close the League before postgame begins.

## Evidence boundary

This audit supports source consistency, reachable script wiring, authored
battle materialization, and the existence of focused runtime contracts. ROM
construction, first-VBlank smoke, and the 19-filter zero-debt runtime manifest
must be demonstrated by fresh logs for the exact candidate. It does not claim
that a human has completed a fresh end-to-end playthrough, or that menus,
dialogue, maps, sprites, collision, animations, saving, and controls have been
visually exercised on every intended emulator, device, or hardware-like
renderer. Difficulty ratings remain design targets until playtest observations
replace `UNPLAYED` in the battle master.
