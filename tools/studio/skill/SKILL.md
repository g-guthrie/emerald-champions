---
name: emerald-studio
description: Use Emerald Studio to develop and visually test native Emerald Champions scenes, dialogue, maps, parties and battles. Record or replay interactions headlessly, inspect contact sheets, search NPC dialogue and source history, and apply builds to a local sandbox. Use when the user asks to use Emerald Studio or iterate on this game's overworld.
---

# Emerald Studio

Use the actual native game and its visual evidence. The browser and headless worker share libmGBA and the same game bridge.

## Find the running project

Run the installed skill's `locate.py` to print the checkout and Studio URL. Prefer the user's supplied checkout when they name one. Read that checkout's AGENTS.md and docs/CONTINUE.md. Then use `python3 tools/studio/cli.py start` from its root if the service is stopped. The working tree may be newer than the running build: inspect `status` and `builds` before attributing behavior to current source.

The user can keep playing in the Codex browser. Prefer an independent headless scene worker when inspecting or testing would interrupt them. Do not switch the visible session unless the task calls for it. All Studio sessions are experimental; the earned campaign is separate.

## Commands

`python3 tools/studio/cli.py status` gives position, build, party, observed text and actors. Use `command '<JSON>'` for operations. For larger payloads use `--file recipe.json`; do not shell-interpolate dialogue.

- `{"op":"dialogue.search","query":"Briney","map":"Route116"}` searches authored dialogue; omit map for region-wide search.
- `{"op":"dialogue.export"}` produces the combined reading copy, structured ownership index and cohesion-review candidates.
- `{"op":"scene.graph","map":"Route116","index":2}` traces script references, conditions and native-handler boundaries for an object index.
- `{"op":"history","path":"data/maps/Route116/scripts.inc"}` reads Git history. `{"op":"builds"}` lists recorded local source snapshots.
- `{"op":"assets","query":"steven"}` finds actual sprite sheets; `{"op":"assets.sheet","query":"steven"}` assembles an art contact sheet. These are source assets, not native screenshots.
- `{"op":"scene.run","recipe":{...}}` launches an independent worker; `{"op":"scenes"}` returns jobs and results.
- `{"op":"scene.replay","id":"...","mode":"exact"}` uses the recording's original ROM/state. `mode:"latest"` clean-boots its portable save on the latest compatible built ROM.
- `{"op":"scene.compare","before":"...","after":"..."}` produces paired contact sheets. Explicit marker labels align semantic moments; unlabeled comparisons use relative timing and may need interpretation.
- `{"op":"situation.save","name":"Before the gift","tags":["full bag"]}` saves the actual current situation. `situation.load` with its ID restores it; `mode:"latest"` uses a compatible portable save. Names/tags do not create prerequisites.
- `{"op":"build"}` builds and returns to the current idle location or pre-interaction checkpoint. Check the eventual status; a queued request is not success.

See `tools/studio/scenarios/*.json` for working headless recipes. Example:

```json
{"op":"scene.run","recipe":{"name":"Vial conversation","start":{"chapter":3},"warp":{"map":"OldaleTown_PokemonCenter_1F","x":12,"y":7,"facing":2},"steps":[{"press":"A","frames":120,"label":"Greeting"},{"tap":"A","every":90,"frames":1200,"until":"idle","label":"Controls returned"}],"expect":{"ready":true,"map":"OldaleTown_PokemonCenter_1F"}}}
```

`start:{"live":true}` forks the current live state. Recipe steps support held buttons, a one-frame press, periodic taps, exact frame durations, labels and bounded waits for idle/dialogue/battle. Final expectations can check map, coordinates, readiness, battle state and displayed-text substring. A failed expectation still produces evidence. CLI-only use: `.venv-studio/bin/python tools/studio/run_scene.py recipe.json --out work/studio/scenes/NAME` (use a fresh output directory).

## Visual iteration

For choreography, run the approach, movement, dialogue and exit together. Open the resulting contact sheets with the image tool; do not claim visual validation from JSON or command success. Inspect facing, overlap, paths, camera, palette, timing, dialogue fit and returned controls. Use motion.webp or original frames for a specific timing/detail question. Contact sheets preserve original pixels with labels outside the image and hash sidecars. Source-map previews and sprite atlases support planning; they cannot certify the native scene.

Compare a preserved before run with the revised run after meaningful changes. Keep proven input sequences, scenario prerequisites and named markers. Review first visit, repeat and relevant reward/storage failures. Only call a branch tested if it actually executed. Text telemetry records a buffer submitted to the printer, not proof every page was read; screenshots and the input trace establish delivery.

Dialogue search follows static references and can include conditional/unreachable branches. Read its source and native-handler boundaries before concluding an NPC says something. Repeated service text is not automatically a defect. The generated reading copy is a derived index; edit native source. Player guides and the Game Book were removed by user direction. Preserve authored trainer ownership.

## State and evidence boundaries

Fresh built-in setups currently cover C03, C04 and C07. Other situations come from actual bookmarks/recordings or explicitly authored recipes; a map warp alone does not establish chapter progression. Party preparation uses native legal presets and the current species cap. Direct trainer tests use the native debug lifecycle and do not award story results; real NPC interactions are needed for postbattle choreography and regional rival substitutions.

Never load raw emulator state into a different ROM. Studio loads the original build for exact rewind, and uses native save/clean boot for compatible updated builds. Arbitrary midbattle code migration and save-layout changes are unsupported. A scene worker is synthetic evidence, not earned campaign advancement. Captures must name their actual ROM/ELF. Local build receipts include the Git base, workspace patch and changed-source archive; do not rewrite current source merely to inspect an old scene.

Keep the UI usable and the live server running when delivering. Report concrete completed checks and open limits. No external codebase indexing is necessary; use local script graphs first, and external MCP only within the user's authorization and current approval boundaries.

For a new scene prerequisite recipe, include an explicit `setup` object with named `flags` (booleans), `vars` (unsigned values), and `items` (quantities to add). These operations are applied atomically with checkpoint rollback on failure, before the recipe's warp. Example: `"setup":{"flags":{"FLAG_BADGE01_GET":true},"items":{"ITEM_OLD_AMBER":1}}`. Use only prerequisites justified by the scene being tested. Record them in the recipe; synthetic prerequisites never count as earned play. Reload the map afterward when its on-load logic depends on the changed flags.


Scene recipes may include `queries` for native readbacks, e.g. `"queries":{"money":{"kind":9},"report":{"kind":1,"id":"FLAG_EC_REPORT_C44_COMPLETE"}}`, and an `expect.queries` object with exact expected values. Query kinds reuse the headless observer (1 flag,2 variable,4 Bag item count,5 PC item count,9 money). Use these to distinguish a rendered reward message from actual inventory/progression. Bounded `until:"idle"` now requires twelve consecutive idle frames, including after destination scripts. Inspect the c30/c44/c45 recipes for examples of real scene prerequisites and native puzzle actions. Imported map-template NPC positions may be moved by transition scripts; trace those before choosing a starting tile.

Independent recipes can select an existing native fixture with `start:{"fixture":"C14_SONG","param":0,"settle_frames":360}`. For postbattle presentation only, `battle_resolution:"fixture_win"` enables the existing headless campaign automation: wild encounters are automatically captured and trainer battles force a win. Recordings label this assistance explicitly; it validates neither combat nor the defeated-without-capture branch. Omit it for ordinary native battles. Check `result.json` → `outcome.passed`, since an expectation failure still emits a recording. Supply all preceding badges when the script counts badges; setting just the latest badge does not establish that progression.

For scoped outcome testing, `battle_resolution:"fixture_defeat"` selects the existing no-capture victory fixture; `"fixture_loss"` selects the existing forced-loss fixture. Both are synthetic battle exits and never combat evidence. `"fixture_win"` retains the automatic-capture behavior for wild battles. All modes are recorded in the outcome.

Use `expect.visited_maps:["SouthernIsland_Exterior"]` to require actual intermediate travel in the native capture trace, including when the scene ends back at its starting harbor. Permission flags alone do not prove a voyage occurred.

Use `expect.forbidden_text:["obsolete instruction"]` to reject stale dialogue anywhere in the native trace, rather than only checking the final text buffer.
