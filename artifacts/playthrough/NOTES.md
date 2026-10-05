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
- Last end save: work/studio/scenes/pt00h/end.sav (Birch's lab, idle, 6,5)
- Next: lab -> Littleroot -> Route 101 -> Oldale (check gifts) -> Route 103 rival (forced win) ->
  back to lab (Pokedex; Littleroot state 3) -> Mom (Old Rod, confirm by Bag query).
- Runner step types: walk, walk_to (+face), until_text, tap/press/hold; door mats need an extra
  hold toward the exit; Yes/No prompts need explicit cursor moves (tapping A picks the default).
- Finding: Mom's Old Rod trigger needs VAR_LITTLEROOT_TOWN_STATE == 3 (set when Birch gives the
  Pokedex, ProfessorBirchsLab/scripts.inc:391). Confirm order in play.

## Known errors in the old manifest (to be redone from evidence)
- Step 5: Petalburg Woods (4,26) is a Nugget (item_ball_scripts.inc:354), not Paralyze Heal.
- Step 6: Rustboro (35,55) Wise Glasses was missed (reachable from Route 104's north edge at x 32-33).
- Step 6: Seaspray "Stone Edge TM" ball is Baxcalibrite; Route 116 "HP Up" ball is Muscle Band.
- Steps 3-6 to be redone with this method after the pilot.

## Open items
- Label drift: item-ball script labels and flags that name a different item than finditem gives
  (e.g. PetalburgWoods_EventScript_ItemParalyzeHeal -> ITEM_NUGGET). Fix labels; add a check so a
  label/flag can never name a different item than the script gives; flag NPC dialogue that names
  an item different from its giveitem.
- Held-item cleanup and vendor badge-floor template: paused until the verified inventory exists.
- Hard battle tuning: paused (only Rival -8, Calvin -6 done).
