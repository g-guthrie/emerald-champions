# Cap 20 / cap 30 redesign: implementation spec

Approved by the owner. (Copy kept at work/redesign/cap20-30-changes.md, which is gitignored.) Goal: remove the cap-20 and cap-30 power cliffs by
staggering evolutions (stones, trades, move triggers, friendship) and moving
early outliers later.

Rule for tuning: assume every friendship evolution happens as soon as its base
form is catchable. Placement, not the Bonding service, is what keeps them late.

Current working tree (uncommitted, not built):
- Seaspray Cave 1F: Dawn Stone ball is now Never-Melt Ice with flag
  `FLAG_SEASPRAY_CAVE_NEVER_MELT_ICE`; its two Unova Stunfisk slots are now
  Psyduck and Zubat (Galarian Stunfisk stays).
- Evolution-move gate in C (`IsEmeraldChampionsEvolutionMoveLocked` in
  `src/emerald_champions_battle_sets.c`, declared in
  `include/emerald_champions_battle_sets.h`) and the Python helper
  `scripts/evolution_move_gate.py` (not wired in yet).
- A local, unpushed commit "Bonding opens with the first Gym Badge".
  Keep its game parts (script, tutor greeting, Roxanne's line). Revert its
  model parts (see section F).

## A. Wild moves (`src/data/wild_encounters.json`)

When a species moves into a slot it takes that slot's levels. When it leaves,
the filler keeps the old slot's levels.

| # | Species | Leaves (slot, levels) -> filler | Arrives (slot, levels) -> displaced | Effect |
|---|---|---|---|---|
| 1 | Riolu | Route 116 land 4 (8-10) -> Starly | Meteor Falls 1F 1R land 9 (36) -> was Druddigon (two remain) | Lucario at cap 40 |
| 2 | Woobat | Seaspray Cave land 4, 7, 9, 10 (10-12) -> Zubat, Tynamo, Wooper, Psyduck | Meteor Falls 1F 1R land 11 (35) -> was Ferroseed (two remain) | Swoobat at cap 40 |
| 3 | Buneary | Petalburg Woods land 5 (6-8) -> Togepi | Route 118 land 7 (26) -> was Liepard (Purrloin is still on Route 116) | Lopunny at cap 30; Togetic early is fine |
| 4 | Chingling | Rusturf Tunnel land 7 (8-10) -> Noibat | Route 118 land 5 (26) -> was Raticate (Rattata is still in Dewford Manor) | Chimecho at cap 30 |
| 5 | Sewaddle | Route 101 land 4 and honey 3 (3) -> Wurmple | Route 118 land 2 (24) -> was Linoone (Zigzagoon is still wild; Linoone also on Route 123) | Leavanny at cap 30 |
| 6 | Pachirisu | Route 103 land 10 (3) -> Shinx | Route 110 honey 3 (13) -> was Ekans (two Ekans honey slots remain) | Pachirisu at cap 20 |
| 7 | Duraludon | Granite Cave B2F land 8 (13-15) -> Sandshrew | (none: it stays at Cave of Origin, L66+) | Duraludon at cap 70+ |

## B. Stones (cap 20 -> cap 30)

1. `data/maps/SlateportCity_Mart/scripts.inc`: the Mart sells only stones
   (Fire, Water, Leaf, Thunder, Ice, Moon, Sun, Oval). Before
   `FLAG_BADGE02_GET` the clerk explains the stones arrive once you have
   beaten Brawly and the menu does not open. After it, the current list.
   The Black Belt NPC line can foreshadow it.
2. `data/maps/Seaspray_Cave_B1F/map.json` (line ~101): replace the
   `ITEM_ICE_STONE` hidden/ground item with a non-evolution item (e.g.
   `ITEM_ICY_ROCK`). Rename its flag to match the new item in
   `include/constants/flags.h` and the map.json, the same way the 1F
   Dawn Stone ball was handled.

## C. Trade evolutions (cap 30 -> cap 40, after Wattson)

1. `src/battle_tent.c` `sSlateportTentRewards`: every prize is an evolution
   item (King's Rock, Metal Coat, Dragon Scale, Up-Grade, Dubious Disc,
   Protector, Electirizer, Magmarizer, Reaper Cloth, Razor Claw, Razor Fang,
   Prism Scale, Oval Stone, Deep Sea Tooth, Deep Sea Scale, Linking Cord).
   Before `FLAG_BADGE03_GET`, award from a separate non-evolution prize list
   (choose sensible held items already sold elsewhere). After it, the current
   list.
2. `data/maps/MauvilleCity_Mart/scripts.inc`: split by badge:
   - always: Light Ball, Leek, Metal Powder
   - after `FLAG_BADGE03_GET`: add Up-Grade, Deep Sea Tooth, Deep Sea Scale,
     Metal Coat, King's Rock, Prism Scale, Sachet, Whipped Dream
   - after `FLAG_BADGE04_GET`: add Thick Club (see D1)
3. Check the manifest afterwards: Alakazam, Machamp, Gengar, Golem,
   Gigalith, Conkeldurr, Steelix, Weavile, Sneasler, Trevenant, Politoed,
   Slowking, Porygon2 must first appear at cap 40.

## D. Other cap-30 spikes

1. Thick Club to 4 badges: Mauville Mart (C2) and the Center species shelf
   (`src/field_specials.c` `sEmeraldChampionsSpeciesItems`, opened by
   catching the species, around lines 480-500 and 690-706). Add a
   `FLAG_BADGE04_GET` requirement for `ITEM_THICK_CLUB`. Mirror it in
   `scripts/reference_pool.py` (`EQUIPMENT_SPECIES` handling).
2. Game Corner (`data/maps/MauvilleCity_GameCorner/scripts.inc`, around
   lines 298-303): Porygon and Munchlax prizes unavailable until
   `FLAG_BADGE03_GET` (Snorlax and Porygon2 then land at cap 40;
   Porygon-Z needs the Dubious Disc, which is Fortree or the gated tent).
3. Legendaries to 4 badges (`src/data/pokemon/legendary_signs.h`):
   - `GATE(COBALION, 2, FLAG_BADGE02_GET, ...)` -> `4, FLAG_BADGE04_GET`
   - `GATE(MELOETTA, 2, FLAG_BADGE02_GET, ...)` -> `4, FLAG_BADGE04_GET`
     (also check its NPC quest in `MeetsSignDiscovery` for a badge check)
   - Iron Leaves (Verdanturf Meadow land 2, L42, 5%) has no gate row; no
     Paradox does. Either append a new LegendarySignId with a 4-badge gate
     (ids are append-only, never renumber), or move its 5% slot to a map
     that opens at cap 45+ and give Verdanturf slot 2 a regular species.

## E. Evolution-move gate (all caps)

Rule: the tutor does not offer a move that would evolve its learner (an
`IF_KNOWS_MOVE` evolution condition) until the level at which that species
learns the move by level-up. If it never learns it by level-up, it waits for
`FLAG_BADGE04_GET`.

Natural levels found: Yanma Ancient Power 33 (Yanmega cap 40), Aipom Double
Hit 32 (Ambipom cap 40), Steenee Stomp 28 (Tsareena cap 30), Mime Jr. Mimic 32
(Mr. Mime cap 40), Dunsparce Hyper Drill 32 (Dudunsparce cap 40), Primeape
Rage Fist 35, Lickitung Rollout 6, Piloswine Ancient Power 1.

1. C (done, unbuilt): `GetEmeraldChampionsPreparationMovesToLearn` skips
   locked moves. Optional: a tutor line explaining the lock.
2. `scripts/manifest_pokemon_sources.py` `IF_KNOWS_MOVE` (line ~387): also
   require `context.get('evolution_move_ready', lambda s, m: True)(sp, arg)`.
3. `scripts/manifest_world.py` (line ~533): pass
   `evolution_move_ready=lambda s, m: evolution_move_gate.ready(s, m, state['cap'], state['flags'])`
   (flags there are canonical names; check `FLAG_BADGE04_GET` matches).
4. `scripts/reference_pool.py` `evo_ok`: it ignores `IF_KNOWS_MOVE` today.
   Add a callback argument so the caller (around line 1269) can pass
   `lambda move: evolution_move_gate.ready(sp, move, cap, flags)`.
5. `scripts/tuning_pool_check.py`: reject a member that knows a locked
   trigger move for its species at the party's level.
6. Tests: a native test (e.g. a level 20 Yanma is not offered Ancient
   Power; a level 33 one is) and Python tests for `evolution_move_gate`.

## F. Friendship model (tools only)

Keep the in-game Bonding lock. Revert the model parts of the local Bonding
commit so tools assume friendship evolutions as soon as the base is legal:
- `scripts/manifest_world.py`: `friendship_max` back to 255 from the first
  Center (drop the `FLAG_BADGE01_GET` check).
- `scripts/manifest_pokemon_sources.py`: `IF_MIN_FRIENDSHIP` true when
  `friendship_max >= 160` with that 255 (or simply true).
- `scripts/reference_pool.py`: `per_window` back to 255 everywhere,
  `evo_ok(bonding=...)` no longer blocks, the encounter-pool
  `max_this_milestone` back to 255. Keep the doc text saying Bonding opens
  at badge 1 but tuning assumes friendship evolutions.
- One Python test was failing after the model change; it should pass again
  after this revert. Check it.

## G. After implementing

1. `make -j4`, `make -j4 check` (2003 pass, 9 expected failures),
   `python3 -m unittest discover -s tests`.
2. Regenerate the manifest:
   `python3 scripts/manifest_campaign.py --out work/progression-manifest`,
   then `python3 scripts/manifest_text.py work/progression-manifest/progression.pickle --out artifacts/progression-manifest/availability-manifest.txt`,
   copy the pickle to `work/retune/progression.pickle` and rebuild
   `work/retune/order.json`.
3. `python3 scripts/retune_recheck.py`: battles #1 (rival) and #3 (Rick)
   used Pachirisu and need reruns.
4. Screenshot-check new text: Bonding lock and greeting, Roxanne's badge
   line, the Slateport clerk line, and the Calvin, Billy and Tiana intros
   (already committed, never screenshot-checked).
5. Commit and push to `claude/eager-feynman-m216tp`.
