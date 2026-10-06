# Verified playthrough - working notes

Read this first after any context compaction. It is the only memory that counts.

## Why this exists
The route manifest (artifacts/progression-manifest/route-manifest.txt) was written by reading
source and inferring reachability. That produced errors (labels trusted over finditem, walker
mistakes on water/elevation/ledges, missed side entrances). The owner no longer trusts inference.

## Method (owner approved)
1. One continuous playthrough of the real headless ROM (pokeemerald-headless.gba), route by route,
   in guidebook order. Each leg starts from the save the previous leg ended on (end.sav), so story
   flags, badges and HMs are earned in play, never set by hand. Trainer battles use
   battle_resolution "fixture_win" (synthetic win; says nothing about difficulty).
2. Every pickup is performed in the game: walk to the ball / hidden item / gift NPC, collect it,
   then read the Bag count back from the game (query kind 4). An item enters the manifest only
   when the game confirms it.
3. Anything not reached gets a screenshot of the attempt and a source citation for why.
4. After the PokeNav is unlocked: screenshot the PokeNav Wild Pokemon page for every route walked.
   Before that: wild tables are read from src/data/wild_encounters.json and re-confirmed with the
   PokeNav screenshots once available.
5. Always double-check the code behind each observation (script, flag, condition) and cite it.
6. The manifest entry for a route cites its recording folder, end save and Bag readout. The owner
   approves each route before the next.

## Full sweep (owner, after the pilot): no stone unturned
Every map: photograph every reachable tile (stitched mosaic), talk to every NPC, check every sign
and hidden-item tile, pick up every item, Bag snapshot before/after. Battles are forced wins.
- explore.py MAP SAVE NAME  -> work/studio/explore/NAME/{report.md,report.json,mosaic.png}
- travel.py SAVE TARGET NAME -> one hop (warp or edge) into an adjacent map
- sweep.py SAVE MAP MAP! MAP@x,y ... -> travel+explore in order; MAP! = pass through only;
  appends to artifacts/playthrough/SWEEP.md; stops at the first failure and prints the last save.
- NPC talk mode: A for talk/gifts; B for shops, trades, tutors, multichoice (explore.SHOP_LIKE).
- Oddities found are logged in FINDINGS (below) and raised with the owner at each route report.

## Rules carried over (do not relitigate)
- Route trainer pool = areas walked before the route; route's own new Pokemon/items never count.
  Exception: first rival battle (Route 103) counts Route 103. Gyms/League/Champion: everything
  reachable when walking in.
- No EVs before the Knuckle Badge (cap 30); default spread after: 252 HP / 252 Spe / 4 Def / 2 SpD.
- Wild held items: decision pending (see Open). Honey stays on Combee (owner, this session).
- No new ground or hidden items (AGENTS.md). Commit by explicit path; never git add -A.
- Never report anything unverified. Flag oddities and stop to ask.

## Tooling facts
- Studio: tools/studio/run_scene.py recipe.json --out work/studio/scenes/NAME (venv .venv-studio).
- start {"chapter":100} = TRUE new game via the cartridge's CB2_NewGame (truck), added this session.
  Do NOT use chapter 101 (Birch's bag shortcut): it skips the house intro and leaves
  VAR_LITTLEROOT_TOWN_STATE = 0, so Littleroot's "need a Pokemon" guard blocks the exit north.
- Walking: recipe step {"walk":[dirs]} (added this session) holds each direction until the game
  reports one tile of progress; scripts it triggers are tapped through; no progress = failure.
- route_steps.py proposes walk directions (reach.py movement rules); the game is the judge.
- Saves: the runner boots a private copy of start.save (core writes saves back in place);
  end.sav files are made read-only after a leg passes.
- Chaining: start {"save": "<path to end.sav>"} (added this session) boots a leg from the previous
  leg's end save; every run writes end.sav when it finishes on idle field.
- Max 18,000 frames per scene. Walking = 16 frames per tile.
- PokeNav wild page recipe: tools/studio/scenarios/pokenav-wild-transitions.json.
- Rebuild headless ROM after any source change:
  make -j4 BUILD_NAME=emerald-headless EC_HEADLESS_FIXTURES=1 TEST=0 pokeemerald-headless.gba
  then python3 scripts/stamp_release_inputs.py --stamp pokeemerald-headless.inputs.json

## State
- Legs done (recipes in artifacts/playthrough/recipes, evidence in work/studio/scenes/<leg>):
  pt00 truck (ng) -> pt00b out of truck -> pt00c clock set -> pt00d out of house -> pt00e/f May met
  (region default Hoenn, VAR_STARTER_GEN 3) -> pt00g north trigger -> pt00h rescue: pair Mudkip +
  Treecko (screenshots), forced win, back in Birch's lab (VAR_BIRCH_LAB_STATE 3).
- Legs after: pt01 to Oldale, pt02 Mart employee, pt03 Mart tour, pt04 Route 103 (May needs Leveler),
  pt05 Leveler + May (forced win), pt06 lab Pokedex + Mom Old Rod, pt07 Oldale vendor kit.
  PILOT COMPLETE (Steps 1-2 inputs verified). Next: report to owner; then Step 3 (Route 102 trainers).
- Last end save: work/studio/scenes/pt07/end.sav (Oldale Center, idle, 2,4)
- Evidence table: artifacts/playthrough/EVIDENCE.md (update after every leg).
- Tap B (not A) through tours/shops: A buys items (15 Mental Herbs happened once).
- Next: lab -> Littleroot -> Route 101 -> Oldale (check gifts) -> Route 103 rival (forced win) ->
  back to lab (Pokedex; Littleroot state 3) -> Mom (Old Rod, confirm by Bag query).
- Runner step types: walk, walk_to (+face), until_text, tap/press/hold; door mats need an extra
  hold toward the exit; Yes/No prompts need explicit cursor moves (tapping A picks the default).
- Finding: Mom's Old Rod trigger needs VAR_LITTLEROOT_TOWN_STATE == 3 (set when Birch gives the
  Pokedex, ProfessorBirchsLab/scripts.inc:391). Confirm order in play.

## Standing owner rules (sweep)
- Fix every misnamed label/flag/text found, as found (scripts/item_source_names.py --check guards).
- Renumber battles after each verified step: python3 scripts/renumber_battles.py --plan/--write
  (manifest order is truth; tests/test_manifest_battle_order.py; Deferred: lines for late ones).
- Wild held battle items on Pokemon met before the first Badge -> clerk floor 0 (family-wide);
  berries, Honey, medicine, sell items, Everstone stay; scheduled items (Light Ball) just removed.
- No friendship evolution before the Stone Badge (src/pokemon.c).
- Worthless overworld pickups (Potion, Super Potion, Ether, Repel, status heals...) become battle
  items, preferably the ones pulled off wild holds in that step; others get a clerk badge floor.
- Held-item rule applies after the first Badge too (owner, step 8).
- Report per step: Pokemon, items, evolutions, concerns; then write the manifest step.

## Harness fixes (sweep)
- Planned paths never step on a warp tile unless it is the goal; explore reachability and the
  hidden-actor approach skip warps (Brendan 2F stairs sent the sweep to 1F).
- Every explore step carries "map"; the runner fails the chunk if the game is on another map.
- Stand spots for talk/inspect never sit on another actor or a warp (lab: it chose Birch's tile).
- Story gates: a coord trigger live now whose script never changes its own var repeats forever and
  turns the player back (Petalburg Gym escort at x=8, y 10-13 while VAR_PETALBURG_CITY_STATE == 0).
  explore.py queries trigger vars, walks around such tiles, lists what lies behind as "story progress".
  Petalburg: visit the Gym (Norman, Wally) before the west side.
- Absent actors so far are all flag-hidden at this story point (checked against map.json flags).

## Wild Pokemon evidence: DexNav (Start menu, 2nd entry), from the start of the game
- artifacts/playthrough/dexnav.py SAVE NAME -> work/studio/dexnav/NAME/sheet.png (one panel per icon).
  The roster lists only slots that can start a battle now (no Honey table without Honey, no Surf
  table without Surf). Route 101/102/103 land + Old Rod rosters match src/data/wild_encounters.json.
- Each scene boots fresh from a save: Start menu cursor on Pokedex, Bag on Items pocket.
- Repel Spray: sweep token SPRAY (recipes/use-repel-spray.json; Key Items 4th). Renews itself:
  wear-off Yes/No defaults to Yes and scenes are settled with A.
- Waiting on a background run: never `pgrep -f sweep.py` (matches the waiting shell itself).

## Known errors in the old manifest (to be redone from evidence)
- Bonding (friendship evolutions) is locked until the Stone Badge (emerald_champions.inc:482);
  before it friendship only rises by walking. Steps 4-6 wrongly list Roselia, Marill, Togetic and
  Pikachu via Bonding; the manifest header must say Bonding opens with the Stone Badge.
- Step 5: Petalburg Woods (4,26) is a Nugget (item_ball_scripts.inc:354), not Paralyze Heal.
- Step 6: Rustboro (35,55) Wise Glasses was missed (reachable from Route 104's north edge at x 32-33).
- Step 6: Seaspray "Stone Edge TM" ball is Baxcalibrite; Route 116 "HP Up" ball is Muscle Band.
- Steps 3-6 to be redone with this method after the pilot.

## FINDINGS (raise with owner)
- FIXED 5ecf8659db: Move Tutor line now says EV training arrives with the Knuckle Badge (EVs are
  locked until then, caps.c AreEVsUnlocked). Owner told.

- Tatsugirinite (AbandonedShip_HiddenFloorRooms: keys + Dive) and Drampanite (MeteorFalls_B1F_2R:
  Waterfall) are listed as first-wave stones (cap 45/55) in tests/test_cap40_redesign.py, which fails
  on HEAD before any rename: their real homes are far later.
- Wrong-map flags FIXED 882d614995 (Route 103 Pearl was FLAG_ITEM_ROUTE_133_PEARL_STRING, + 5 more);
  check now catches a flag naming another existing map. Owner told.
- Label/flag drift FIXED (scripts/item_source_names.py --write, 241 names, values unchanged); CI test
  tests/test_item_source_names.py keeps it fixed. Save-layout id changed only because flag NAMES are
  hashed (scripts/build_provenance.py ids); flag values and struct layout are unchanged.

## Open items
- (done) Label drift found so far: PetalburgWoods ItemParalyzeHeal -> Nugget; Lab MayGivePokeBalls /
  BrendanGivePokeBalls -> 30 Cherish Balls; Route116 ItemHPUp -> Muscle Band; RustboroCity
  ItemAbilityCapsule -> Wise Glasses; Seaspray ItemStoneEdge -> Baxcalibrite; Seaspray Water_Pulse ->
  Covert Cloak; PetalburgWoods_2 TM80Venoshock -> Toxic Orb; Woods_3 TM86_GrassKnot -> Nugget,
  TM34_SludgeWave -> Victreebelite, Beedrillite -> Luminous Moss; Route104 ItemAudinite -> Shell Bell.
- Label drift: item-ball script labels and flags that name a different item than finditem gives
  (e.g. PetalburgWoods_EventScript_ItemParalyzeHeal -> ITEM_NUGGET). Fix labels; add a check so a
  label/flag can never name a different item than the script gives; flag NPC dialogue that names
  an item different from its giveitem.
- Held-item cleanup and vendor badge-floor template: paused until the verified inventory exists.
- Hard battle tuning: paused (only Rival -8, Calvin -6 done).
