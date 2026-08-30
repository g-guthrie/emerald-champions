# Emerald Champions native UI and visual decision ledger

## Purpose and authority

This is the accountability record for the native-presentation pass prompted by
the first live playtest of the migrated engine. It is deliberately a decision
ledger, not a blanket instruction to restore every old byte.

The comparison reference is pre-migration Verdant commit `81e288b51`. The new
baseline is `bf6ca78effa59ebe05dab488c14522e80c566134` on
`fix/native-ui-parity`, plus the uncommitted working-tree repairs present when
this ledger was written on 2026-08-30. Executable source, map JSON, layout
blockdata, config, and directly referenced graphics are the ground truth.
Design documents are context only.

The governing rule is:

> Prefer Verdant's proven Emerald-native presentation, preserve the expansion
> engine wherever it supplies necessary modern mechanics, and combine them
> only at an explicit seam with bounded geometry, predictable Back behavior,
> and runtime proof.

This documentation pass did not edit code, maps, fixtures, renderer, or visual
assets. It updates this ledger and regenerates the canonical visual-byte
manifest. Other agents were modifying the shared working tree; statuses below
distinguish the checked-in baseline from repairs already present in that tree.

Static source proof does not establish that a screen looks good in mGBA or on
hardware. Every item marked `RUNTIME REQUIRED` remains open until its listed
scenario has been rendered and operated.

## Status vocabulary

- `KEEP OLD`: the Verdant behavior or art is the winner.
- `KEEP NEW`: the expansion behavior is required and already fits the game.
- `HYBRID`: retain the new capability behind the old visual or navigation
  language.
- `FIXED IN WIP`: the working tree contains the recommended repair, but it is
  not release proof until build and runtime verification pass.
- `RUNTIME REQUIRED`: source geometry is coherent, but the visual result or
  complete controller path has not been observed.
- `NO ISSUE`: old and new are identical or the difference is inactive.

## Canonical deterministic render evidence

### Manifest authority and boundary

The canonical evidence set is
`work/visual-audit/rendered/current/manifest.json`: **91 scenarios** rendered
at 240x160 by native libmGBA from ROM SHA-256
`dc8d840fac30b18008bac7c73c3cb5b4fa1886bc79709ab7c362400743ee2af6`.
All 91 current frames were visually inspected. The manifest records **39**
runtime-state-verified scenarios: the wild-action/move-details seams, five
battle-presentation states, and all 32 physical one-off encounter placements.

The manifest, rather than loose historical PNG aliases in the same directory,
is the authoritative scenario membership and hash record. A clean standing
frame proves composed geometry, palette, scale, and text fit for that state. It
does not by itself prove Back behavior, cursor retention, scrolling, repeated
allocation/deallocation, animation, audio, collision, save migration, touch/web
controls, or every alternate data value. Those limits remain explicit below.

### Complete 91-scenario coverage

| Surface group | Canonical scenarios and observed result | Decision closed by render | Remaining honest limit |
| --- | --- | --- | --- |
| Center shells | `center-oldale`, `center-lavaridge` | Both restored Center compositions are clean; prior bottom-edge and crowding defects are absent. `KEEP OLD`. | Walk every collision, facing, warp, and service path. |
| Party and global options | `ability-menu`, `party-overview`, `party-action-menu`, `options`, `leveler-complete` | Verdant palette is clean; direct Ability list and party action geometry fit; Difficulty replaces Text Speed cleanly; Leveler uses one final message. `HYBRID`. | Ability mutation/B paths, six-member/status/form/catch-swap variants, setting persistence, level-offset behavior, evolution chains. |
| Battle vendor | `battle-vendor`, `battle-vendor-shop` | Five-row category window and native FREE shop list do not overlap the textbox. `HYBRID`. | Scroll/B/cursor return, every category, quantity/purchase/full Bag, badge-eight archives. |
| Move specialist and battle sets | `move-specialist-root`, `move-specialist-party-prompt`, `battle-set-list`, `battle-set-current`, `all-legal-moves`, `all-legal-moves-direct` | Five-service root, party-first selection, dynamic set list, matched-set disclosure, and unified legal-move UI all fit native windows. `HYBRID`. | Operate every branch, 4+/scrolling set lists, Egg/no-set/no-move, replacement, Mega requirements, protected items, B loops. |
| Mew 372-row legal list | `all-legal-moves-mew`, `all-legal-moves-mew-middle`, `all-legal-moves-mew-final` | First, middle, and final positions are clean; the final frame explicitly shows `CANCEL`. List, description, arrows, and message window remain separated across the full 372-row extent. | Select/replacement/B behavior and every individual move description are not implied by three samples. |
| Game Corner | `game-corner-prizes`, `game-corner-regions`, `game-corner-region-list` | Coin box, top prize list, five-row region selector, and regional species list are clean and separated. `KEEP NEW` archive in native chrome. | Purchase failures, claimed-state cue, all nine regions, repeated windows, B/cursor persistence. |
| Champions Circuit | `circuit-lobby`, `circuit-welcome`, `circuit-room` | Lobby desk, compact first welcome, and native Tower battle room compose cleanly. `HYBRID`. | Full corridor/opponent/battle/continue/loss/reward/retire state machine. |
| Core battle controls | `wild-action-menu`, `move-details` — both runtime-state verified | R-ball rail, native action menu, and L move-info window are clean; sentinel-sprite seam is exercised without corruption. `HYBRID`. | Throw/cycle/B, every move slot, targets, trainer/Frontier/prohibited-catch states, all Button Modes and touch parity. |
| Naming | `naming` | Keyboard, cursor, entry field, Back, OK, and native chrome are clean. `HYBRID`. | Typing/pages/long names/B/cancel/OT restriction and Center return. |
| Deep Storage | `storage-root`, `storage-boxes`, `storage-box-popup`, `storage-move-items` | Root, box field, Jump-to-Box popup, party panel, held-item move panel, Close Box and Cancel states all compose cleanly. Modern utility remains inside the native PC shell. `HYBRID`. | Deposit/withdraw/move/item transfer/summary/selection and repeated cancel loops still require operation. |
| Initial starter regions | `starter-regions` | Five-row nine-region selector and bottom message box are clean; no geometry overlap. `KEEP NEW` roster. | All nine choices, B-loop, remembered row, three-ball scenes, cries, rival counterpart. |
| Pokédex | `pokedex` | Native Emerald list/count/search shell is clean; inactive HGSS/DexNav assets do not leak. `KEEP OLD` shell. | Info/area/search, empty/seen/caught states, long forms and #999/#1000 boundary. |
| Summary and Fairy | `summary-info`, `summary-skills`, `summary-moves`, `summary-move-detail`, `summary-party-roundtrip`, `fairy-summary-info`, `fairy-summary-moves` | Native tabs, Cancel, Info expansion, stats, nature/Ability, return to party, Fairy type badge, and Fairy move rows all fit. Blank description in collapsed Moves view is intentional until A-Info. `HYBRID`. | Contest Moves, long edge strings, real-save OT/Dex values, alternate move rows, close/return and repeated page loops. |
| Bag | `bag` | Canonical Emerald green bag, cream list, description pane, and magenta/cyan stripes are clean. `KEEP OLD` shell. | Other pockets, use/give/toss/register/close and long edge strings. |
| Frontier Pass | `frontier-pass`, `frontier-pass-map` | Both book overview and full Battle Frontier map/list page render cleanly despite changed source sheets. `HYBRID`. | Cursor navigation, Symbol/record variations, Cancel/return and alternate values. |
| Warden/world delivery | `ember-path-warden` | Storyteller, particles, boulder, cliff, and player compose naturally without custom quest UI. `KEEP NEW`. | Dialogue branches, collision/facing, popup and return route. |
| Doubles status and Ability popup | `double-status-ability` — runtime-state verified | Four status labels, doubles healthboxes, Arcanine Intimidate popup, sprites, and message box coexist without overlap. `KEEP NEW` modern information. | Other popup lengths, simultaneous Abilities, status combinations, target/action transitions. |
| Mega presentation | `mega-ready`, `mega-active` — runtime-state verified | Ready and active Charizard states, Mega indicator/state change, move list, PP panel, healthboxes, and L-info rail are clean. `KEEP NEW`. | Trigger input/timing, opponent Mega, doubles, switch/faint/persistence and every Mega sprite. |
| Primal presentation | `opposing-primals` — runtime-state verified | Groudon/Kyogre sprites, Primal indicators, healthboxes, status, action box, and native battle chrome compose cleanly. `KEEP NEW`. | Activation sequence, weather messages/effects, doubles ordering, switch/faint persistence. |
| Safari battle | `safari-action` — runtime-state verified | Safari Balls counter, Ball/Pokéblock/Go Near/Run box, trainer sprite and encounter healthbox fit native geometry. `KEEP OLD` presentation. | Operate all four actions, counters, flee/catch/result paths. |
| Title and Birch | `title-live`, `birch-introduction` | Emerald Champions title animation frame and Birch introduction/message frame are clean and native. Title branding is `KEEP NEW`; Birch presentation is retained Emerald. | Full title animation, New Game/Continue, gender/name sequence, audio and old-save migration. |
| Pokéblock Condition | `pokeblock-condition` | Milotic portrait, condition graph, labels, nature, sheen rail and striped native background compose cleanly. `KEEP NEW` engine behavior in Emerald shell. | Feed/result animation, other graph extremes/species/natures, B/return. |
| Gold Trainer Card | `trainer-card-gold` | Gold palette, stars, portrait, ID, money, time, badges and border are clean. `KEEP NEW` data support with native card. | Flip/back page, long values, badge states, stars and return. |
| Battle Dome card | `battle-dome-info-card` | Triathlete portrait, three-Pokémon team, header and three strategy lines fit the native tournament card. `KEEP NEW` data in Frontier chrome. | Other trainer names/classes/teams/long descriptions and bracket navigation. |
| Contest Results | `contest-results` | Master Rank header, contestants, gauges, hearts/stars, Milotic and win message compose cleanly. `KEEP NEW` engine fixes in native contest art. | Other ranks/results/ties, animation, transitions, awards and exit. |
| Slot Machine | `slot-machine` | Reels, replay tiles, credit/payout, lights, Select Info and B Quit fit the canonical machine. `KEEP OLD` presentation. | Spin/replay/win/loss/help/quit animations and coin edge values. |
| All 32 physical one-off encounters | `encounter-01-hoopa` through `encounter-32-fezandipiti` — all runtime-state verified; supplemental earlier placement frames `thundurus`, `tornadus`, and `landorus` are also present | **32/32 clean** on their live maps, including complete 64px Dialga, Zygarde, Reshiram, Regigigas, Palkia and Enamorus. Exact coordinates, collision, approaches, margins, nearby objects/signs and hashes are in [OVERWORLD_ENCOUNTER_VISUAL_AUDIT.md](OVERWORLD_ENCOUNTER_VISUAL_AUDIT.md). `KEEP NEW`. | Standing frames do not prove dormant/partner/awakening/defeat/catch/full-party/full-PC state machines or every animation frame. |

### Accessibility and evidence limitations

- Native 240x160 frames prove target-resolution fit, not readability after
  web/mobile scaling or on dim/ghosting LCDs.
- The bitmap font has no screen-reader or scalable-text path. Color-only
  nature/type/status distinctions still merit color-vision review wherever
  labels do not duplicate meaning.
- Still frames cannot reveal flicker, palette cycling, held-button repetition,
  audio-only feedback, resource leaks after repeated navigation, or focus loss.
- Only 39 scenarios carry runtime-state sentinels; the remaining 52 are
  deterministic final-frame geometry evidence, not proof that every ordinary
  story path reaches the state.
- The 32 encounter frames prove live standing composition and fixture state,
  not their quest/capture persistence logic or every animation frame.

## Decision ledger

| Surface | Verdant `81e288b51` | Migrated baseline | Decision | Evidence and reason | Status |
| --- | --- | --- | --- | --- | --- |
| Pokémon Center shell | Classic 16×9 Emerald/Inclement Center with nurse and service counter positions the user had already approved | Expansion Center layout, dimensions, and object placement visibly changed the first room of the game | `KEEP OLD` | The Center is a repeated campaign hub; a visually different room makes the whole game feel like the wrong engine. The working tree restores both old 16×9 blockdata variants and places the nurse, vendor, and tutor on reviewed walkable tiles in all 16 live Centers. Oldale and Lavaridge now render cleanly after their edge/crowding repairs. | `FIXED IN WIP`; native UI gate and two representative renders pass; collision and full-NPC walkthrough still required |
| Center move specialist menu | One native specialist owned battle sets, moves, deletion, and renaming; party-first selection | Only `Build a Battle Set`, `Learn a Move`, and `Exit`; deletion/renaming moved away or leaked into Summary shortcuts | `HYBRID` | Keep the new set data, but restore the old service topology. The WIP exposes Build Set, Learn Move, Forget Move, Rename, Exit. The root and party prompt render cleanly. The first reuse of Lilycove's entry was rejected because its map-local `LOCALID_MOVE_DELETER` would turn a Center object; the WIP now enters the map-neutral body. | `FIXED IN WIP`; static/render proof for root; operate every branch in two Centers and the League lobby |
| Move access hierarchy | One party choice followed by the legal move list; the player did not need to know whether a move was learned by level, egg, TM, or tutor | Baseline used a four-way Level/Egg/TM/Tutor category prompt, then full PC storage because `OW_CHOOSE_FROM_PC_AND_PARTY` was enabled | `HYBRID` | The source category is implementation trivia, not player knowledge. The live WIP enters one `MOVE_RELEARNER_ALL_MOVES` list that unions level, pre-evolution, egg, TM, and tutor legality with duplicate removal and alphabetical sorting. Geodude plus Mew's first, middle, final, and explicit Cancel states prove the 372-row geometry. Party selection is native; boxes remain accessible at the PC. | Composition closed across short/maximum list extents; operate empty/filter/replacement/Back paths |
| Summary actions | Native `Cancel`; preparation lived at the Center | `Rename` replaced Cancel and a move-relearner shortcut was available from Summary anywhere | `KEEP OLD` | Both shortcuts bypassed the service NPC and visibly altered a core Emerald screen. WIP sets `P_SUMMARY_SCREEN_RENAME` and `P_SUMMARY_SCREEN_MOVE_RELEARNER` to `FALSE`. Info, Skills, Moves, A-Info detail, Fairy-type pages, and party return all use native tabs and Cancel. The collapsed blank description is intentional until A-Info. | Composition closed for captured pages; real-data/long-string/Contest/interaction paths remain |
| Party/PC field selectors | Tutors, Day Care, trades, deleter, and Name Rater opened the party screen | `OW_CHOOSE_FROM_PC_AND_PARTY=TRUE` opened the full storage UI from field services | `KEEP OLD` | A full storage screen is a large visual and mental context switch, especially in a Center. WIP restores `FALSE`. | `FIXED IN WIP`; test tutor, Day Care, trade, deleter, Name Rater |
| Party screen skin | Classic cyan/white Verdant skin | Expansion recolor | `HYBRID` | Keep expansion party code, modern forms, and new category functionality, but use the exact approved old skin. The old and new tilemaps are byte-identical. An initial one-bank PNG palette caused cyan/black corruption; the exact 176-color Verdant palette now drives the tilemap. Overview, action, Ability, and Leveler frames are clean. | `FIXED IN WIP`; exact asset and representative render evidence below; item/form/catch-swap/status/multi states remain |
| Party Ability flow | `Ability` in the Pokémon action menu opened a dedicated list, highlighted the current Ability, applied directly, and returned one level | A `Change Ability` confirmation flow auto-changed first, hid the true choices behind another prompt, and could draw under the message box | `KEEP OLD` | Ability swapping is a core repeated action. WIP restores a dedicated bounded list, current-Ability cursor, direct application, and Back to the same Pokémon action menu. The dedicated list now renders cleanly on the repaired palette. | `FIXED IN WIP`; static and rendered-menu proof; controller/touch-equivalent mutation/Back paths still required |
| Party action cursor after Ability | Parent action menu reopened at its first row after leaving the Ability list | Same underlying expansion helper initially behaved differently, then WIP returned to the same action menu but still starts at `SUMMARY` | `KEEP OLD` | This matches the approved Verdant behavior. Preserving the exact parent-row cursor would be a separate improvement, not parity, and is not worth extra state unless playtesting shows friction. | `NO ISSUE` |
| Battle-set chooser geometry | Fixed native list; up to four visible rows | Fixed 8-tile window even when only three rows existed, leaving an empty visual row | `HYBRID` | Preserve scrolling and dynamic set names, but size the window to `2 * visibleRows`. The two-set-plus-Exit and current-set confirmations render cleanly. | `FIXED IN WIP`; four/five-plus rows and scrolling still require operation/render proof |
| Battle-set application loop | Full mutation explanation and one NPC interaction per Pokémon | Same inherited friction after migration | `HYBRID` | The set system is the game's most frequent preparation action. WIP shortens confirmation/result copy and asks `Prepare another Pokémon?`, returning to party choice without retalking. | `FIXED IN WIP`; first-use explanatory copy remains a possible enhancement |
| Current set identification | A single recommended set made identity mostly implicit | Baseline wilds could roll multiple sets, but the tutor neither identified nor preselected the complete current orientation | `KEEP NEW` capability with native disclosure | The WIP derives the matching preset without a save-format field by comparing unordered moves plus nature, effective Ability (including pre-Mega fallback), held/required item, and all six Stat Point fields. It buffers the current set name and opens the scrolling chooser on that row. The matched-set confirmation renders cleanly. Optional one-time capture disclosure is still a separate product decision. | Recognition and disclosure `FIXED IN WIP`; scrolled-row preselection and capture message remain open |
| Battle vendor hierarchy | One long native shop list | Category popup could show seven rows through the bottom message box; menu returned to the wrong level; post-badge archives were mixed into the same hierarchy | `HYBRID` | Categories are necessary for 111 free items, but use a five-row scrolling menu above the textbox, Back one level, and native shop screens for inventory. Both category and shop inventory renders are clean. | `FIXED IN WIP`; operate pre/post-badge, every category, B, quantity, purchase, and return |
| Battle vendor cursor memory | Not applicable to the old flat list | Baseline returned from every category at `Offense` and reset the post-badge archive row | `KEEP NEW` categories with retained local cursor | The WIP uses independent interaction-local state: `VAR_0x8008` for held-item category and `VAR_0x8009` for the post-badge top menu. Both initialize on NPC entry, feed `dynmultistack`'s initial row, update only after a real choice, and reset when the interaction ends. | `FIXED IN WIP`; runtime test each return and B path |
| Standard Poké Marts | Native progression and shop renderer | Expansion shop engine plus project progression | `HYBRID` | Keep current item data and narrow-font safety, but retain native Mart layouts/progression. Rare Candy remains in medicine Marts. No additional custom category UI belongs here. | Source geometry cleared; runtime shop sweep required |
| Bag shell | Native Emerald pocket art, item list, icon, description, and commands | Expansion Bag capabilities behind the retained shell | `KEEP OLD` shell with current item data | The deterministic render matches canonical Emerald, including the green bag and magenta/cyan stripes. Old Verdant menu PNG/palette are byte-identical to their current renamed assets; only three tilemap bytes differ. The stripes are intentional native art, not corruption. | Base frame visually proved; classify the three tilemap bytes and operate all pockets/actions/edge strings |
| Game Corner Pokémon prizes | Right-side scrolling prize list with a separate coin box | Nested Genesect/Poipole/Starter Archive menus began at x=8 while the coin frame occupied x=0..9; every return re-created the coin window without removing the old one | `HYBRID` | Keep the finite starter archive, but use one coin window, top prize menu at x=10, and regional species lists at x=11 for a visible one-tile gap. The old behavior did not need the hierarchy, but its separation of money and menu was correct. All three hierarchy levels render cleanly. | `FIXED IN WIP`; repeated transactions and all B/cursor/claimed-state paths remain |
| Game Corner archive state | Smaller prize list | Baseline left 27 starters visually indistinguishable after claim and reset the region chooser to Kanto on every return | `KEEP NEW` archive with native state cues | The WIP initializes `VAR_0x8005` once when opening prizes, records each real region choice, and `SCROLL_MULTI_STARTER_REGIONS` reconstructs row/offset from it. B from species therefore returns to the same region; B from regions returns to top prizes; B from top exits and removes coins once. A compact claimed marker or omission of claimed entries remains desirable. | Region cursor `FIXED IN WIP`; claimed-state cue remains open |
| Fossil revival | Root/Claw-era native flow | All 11 fossils in a dynamic list with item icon; six visible rows put the lower frame on message row 14 | `KEEP NEW` | Full fossil support is better. Limit to five visible rows and preserve B as cancel. | `FIXED IN WIP`; all five live dynamic-menu geometries pass static gate |
| Initial starter region | Seven-region selector; B silently fell through to Hoenn | Nine-region selector retained the silent Hoenn fallthrough and initially reopened at Kanto | `KEEP NEW` roster, fix old navigation | A Back press must never commit an unstated choice. WIP loops on `MULTI_B_PRESSED`, records the last real region in `VAR_0x8005`, reconstructs its scroll row/offset, and caps the shared selector at five visible rows. The initial selector renders cleanly. | `FIXED IN WIP`; operate all nine choices, later rows, cursor return, and repeated B |
| Starter selection art | Emerald bag/three-ball scene | Expansion-compatible scene extended through Galar and Paldea | `KEEP NEW` | The underlying scene remains Emerald-native and Gen 8/9 support is required. | `RUNTIME REQUIRED` for all nine trios, names, cry, rival counterpart |
| Options screen | Emerald option frame with the text-speed slot repurposed for difficulty | Same concept on the expansion implementation | `HYBRID` | Keep Hard/Medium/Easy in the old screen and force instant text. The rendered native screen is already dense but clean; do not add `-2/-4` prose or another window here. Explain the tuning contract in the guide/help text outside this menu. | Geometry/render cleared; setting persistence and real battle offsets still require runtime proof |
| Battle L/R controls | R throws the last used Ball | L opens move details; R throws last Ball; R+D-pad cycles Balls | `HYBRID`, favor current | Preserve R from Verdant. Keep L details because this is a doubles-first competitive teaching game, and keep R cycling because it removes Bag friction. The left rail is reused rather than showing both widgets simultaneously. L details are suppressed under `L=A`. Wild-action and move-details fixture states render cleanly and report successful state observation. | Two sentinel OOB defects `FIXED IN WIP`; full controller matrix below remains |
| Battle L/R safety | No move-info sprite existed | First wild action screen hid `gSprites[MAX_SPRITES]`; L=A skipped creation but later unconditionally hid the missing sprite | `KEEP NEW` after guards | Both writes were direct out-of-bounds memory access introduced at the seam between old R-ball UI and new L-details UI. WIP guards both sentinel accesses and the native UI gate enforces them. | `FIXED IN WIP`; runtime tests still required |
| Battle Back/cursor behavior | Native action and move cursor persistence | Current move/action cursors persist per battler; B from target selection returns to the same move; closing details preserves move selection | `KEEP NEW` | This is both native and efficient. Do not reset battle cursors as part of cosmetic restoration. | Source cleared; runtime matrix remains |
| Modern battle hints and forms | No L details; status, Ability, Mega, Primal, and Safari affordances were narrower | Move details, target previews, category icons, seen-effectiveness help, modern Mega trigger, Primal indicators, doubles Ability popups | `KEEP NEW` behind Emerald art | Runtime-state fixtures now prove clean wild L-details/R-ball, four-status doubles plus Intimidate popup, Mega-ready and Mega-active states, opposing Primals, and Safari action geometry. The composed assets no longer require a blind old restore. | Presentation states closed; trigger timing, alternate strings/statuses, targets, switching/fainting, weather and full controller matrix remain |
| Pokédex shell | Native Emerald Dex | Native Emerald Dex remains selected; HGSS Dex and DexNav assets/code exist but configs are false | `KEEP OLD` shell | Inactive expansion assets are not a live visual regression. The list page renders natively; four-digit National numbers have explicit source handling. | Base list approved; info/area/search and #999/#1000 boundary remain |
| Route signs | Native wayfinding only | A second native sign message lists live species by Grass, Surf, Rock Smash, rods, Hidden/Honey, and special under-bridge source | `HYBRID` | Keep the native Dex untouched and embed availability in world signs, as requested. Lines are bounded at 200px and `gStringVar4` is 1000 bytes. | Static route-sign gate passes; test short and maximum-page signs |
| Restored-area identity | Unique `MAPSEC_SEASPRAY_CAVE`, `EMBER_PATH`, `ASHEN_WOODS`, `SANDSTREWN_RUINS`, `DEWFORD_MEADOW`, `DEWFORD_MANOR`, `VERDANTURF_MEADOW` with themed popups | Those maps were mislabeled as Route 115, Jagged Pass, Route 111, Dewford Town, or Verdanturf Town | `KEEP OLD` | This was a direct migration loss. It made authored locations announce the wrong names. WIP restores all seven names, old coordinates, Stone/Wood themes, and parent-region links. | `FIXED IN WIP`; generate constants and test every entrance/floor/region map |
| Visible Legendary interactions | Native object interaction, cry, message, battle | Same general pattern with more sprites and conditional states | `KEEP NEW` | No separate quest UI should be added. All 32 physical one-off objects are now full, clean, actual-map, runtime-state-verified compositions, including six 64px sprites and the repaired Magearna/Fezandipiti/Virizion placements. Exact live geometry and evidence are in `OVERWORLD_ENCOUNTER_VISUAL_AUDIT.md`. | Standing composition closed 32/32; dormant/partner/awakening/defeat/catch/full-party/full-PC logic and animation frames remain |
| Form-change Key Items | Most modern forms were unavailable or items were inert | Expansion party popups support Rotom, Zygarde, fusions, and other form tools | `KEEP NEW` under old party skin | These are necessary capabilities and their current bounded side windows fit the party screen. Do not restore the old inert Rotom Catalog. | Source geometry cleared; test every obtainable form item and B path |
| Leveler | New Verdant system, using party/evolution machinery | Baseline retained fanfare plus two stat pages for every under-cap member | `HYBRID` | The WIP raises each eligible member silently inside the native party sequence, interrupts only for real evolution chains, tracks whether anything changed, and prints one final native message with the current cap. The final party/cap message renders cleanly. Rare Candy retains its ordinary per-Pokémon stat flow. | `FIXED IN WIP`; final-frame proof only; operate 0/1/6 eligible members and chained evolutions |
| Poké Vial | New Verdant Key Item with charges and native item message | Same | `KEEP NEW` | No custom full-screen UI is needed. | Source-cleared; test 0/1/max charges, register/use, refill, upgrades |
| Catch swap | Older engine sent full-party catches to PC | Modern native-styled party swap is enabled | `KEEP NEW` | Immediate team iteration is central to the game. | Runtime test full party, B, HM restriction, Egg, PC full |
| Naming screen | Emerald naming screen | Expansion source assets and form support changed several cursor/menu assets | `HYBRID` | Keep expansion correctness and old service entry; do not restore assets by hash alone because several source sheets changed dimensions/layout. The base keyboard, cursor, Back, OK, and entry field render cohesively. | Base render approved; typing, pages, long names, B/cancel, OT restriction, and return remain |
| Storage screen | Emerald PC | Expansion storage adds item swaps and selection hooks; 126 same-path assets are semantically identical, five are genuinely changed | `HYBRID` | Keep modern utility only where it uses the native PC shell. Root, box field, Jump-to-Box popup, party panel, Move Items rail, Close Box, and Cancel states all render cleanly. The Center still must not invoke the whole storage UI for ordinary services. | Deep composition closed; deposit/withdraw/move/item-transfer/summary and repeated cancel loops remain |
| Champions Circuit | No Showdown generator | Generated full-team doubles run loops directly from the lobby desk and repeats the long introduction on each talk | `HYBRID` | Keep the generator and reward logic; present it through Battle Tower corridor/battle-room/return states, and show the long explanation only on first talk. Lobby, compact welcome, and native battle room all render cohesively. | `FIXED IN WIP`; full corridor/battle/continue/loss/reward/retire state machine remains unproved |
| Native Frontier facilities | Standard Emerald facility flows | Retained alongside Circuit | `KEEP OLD` presentation with current engine fixes | These remain the visual benchmark for the Circuit. Frontier Pass overview, full map/list page, and Battle Dome trainer card all render cohesively despite changed source sheets. | Captured compositions closed; navigation/value variants, Dome bracket, Factory and recordings-disabled messaging remain |
| Pokéblock Condition | Native Emerald graph and portrait | Expansion graph/data implementation | `HYBRID` | Milotic, five-axis graph, labels, nature, sheen rail and canonical striped background render cleanly. The changed graph/frame binaries are compatible as composed. | Base composition closed; feeding/results, graph extremes, other species/natures and return remain |
| Contest Results | Native Emerald contest results | Expansion contest fixes and larger species roster | `KEEP NEW` behavior in native chrome | Master Rank header, contestants, gauges, hearts/stars, Milotic and win message all fit cleanly. | Base result composition closed; other ranks/results/ties, animation, awards and exit remain |
| Slot Machine | Native Emerald reels and chrome | One same-path menu sheet changed | `KEEP OLD` presentation | Reels, replay tiles, credit/payout, lights, Select Info and B Quit render as a coherent native machine. | Base composition closed; spin/replay/win/loss/help/quit animations and coin extremes remain |
| Trainer Card | Native card pages and badge language | Gold palette and modern data support | `KEEP NEW` support in native chrome | Gold card, stars, portrait, ID, money, time, badges and border render cleanly. | Front composition closed; flip/back, long values, badge/star states and return remain |
| Birch introduction | Native Emerald professor scene | Expansion-compatible source and current title flow | `KEEP OLD` presentation | Birch sprite, spotlight, message frame and intro copy compose cleanly. | Base frame closed; full gender/name/New Game sequence, audio and save migration remain |
| Ordinary Bag/shop/storage text widths | Native narrow-font fallbacks | Expansion adds longer moves, items, Abilities, species, and forms | `KEEP NEW` fit logic | Existing source uses narrow/narrower fallbacks and text tests. Width safety is not equivalent to beauty, so edge-width rows still need render review. | Static text gates pass; runtime edge strings required |
| Title screen | Verdant/Inclement branding | Emerald Champions art and text | `KEEP NEW` | Branding is intentional, and the live Emerald Champions title frame renders cleanly with native Emerald animation language. | Captured title frame approved; full animation, audio, New Game/Continue and save migration remain |

## Back-navigation and cursor-memory contract

The intended contract is explicit state, not global magic:

1. B backs exactly one conceptual level. It must never commit a default choice.
2. A submenu returns to the same object, Pokémon, category, or region that
   launched it when that state is still valid.
3. Scroll row and offset persist only for the current NPC interaction. They
   reset after `release`, map change, or a new transaction.
4. Applying a mutation may return to the parent action menu; it must not dump
   the player to the overworld unless the flow is complete.
5. Cancel must remove every auxiliary window and sprite exactly once.

### Current cursor results

- Battle action and move cursors: retained correctly.
- Battle target B: returns to the same move correctly.
- Move-details close: same move remains selected.
- Ability list: current Ability is highlighted; B returns one level; parent
  action cursor resets to Summary, matching old Verdant.
- Shop list: row/scroll persists through ordinary purchase loops.
- Battle-set list: a full-orientation matcher now identifies and preselects the
  current set, including a scrolled row beyond the first four entries.
- Battle vendor: category and archive menus retain independent local cursors in
  `VAR_0x8008` and `VAR_0x8009`.
- Game Corner: returning from a starter retains the chosen region; claimed
  entries are still not visually marked.
- Move relearner: the live Center path now opens one unified All Legal Moves
  list, so the obsolete category-cursor reset is no longer player-facing.
- Fossil list: reset is acceptable because revival is one finite transaction.
- Initial starter region: B-loop repair is present; no silent Hoenn selection.

The implemented cursor state remains interaction-local. A global allocator or
global last-menu cursor would create more hidden state than it solves. The
remaining Game Corner claimed-state cue should likewise stay local to that
archive rather than changing generic menus.

## Visual asset census

### Method

The non-Pokémon frontend census covers these source families:

`bag`, `battle_frontier`, `battle_interface`, `birch_speech`, `contest`,
`easy_chat`, `evolution_scene`, `frontier_pass`, `interface`, `map_popup`,
`naming_screen`, `party_menu`, `pokeblock`, `pokedex`, `pokemon_storage`,
`pokenav`, `shop`, `slot_machine`, `starter_choose`, `summary_screen`,
`text_window`, `title_screen`, `trainer_card`, and `types`.

It excludes Pokémon front/back/icon sprites, trainer sprites, object-event
sprites, move-animation art, item icons, and audio. Those are separate content
audits, not frontend chrome.

Comparison is semantic rather than naive byte equality:

- `.pal` files normalize LF/CRLF and trailing newline differences.
- UI `.png` files compare decoded RGBA pixels and dimensions.
- map/tileset indexed PNGs compare dimensions and palette-index planes, while
  their external palettes are compared separately.
- `.bin`/tilemap data compare exact bytes.
- identical content at a renamed path counts as a rename, not a visual change.

### Curated frontend census used for initial triage

The following 24-family census is the **pre-render triage snapshot**, retained
to explain which differences were selected for visual review. It predates the
explicit `graphics/party_menu/bg.pal` addition and is not the canonical current
tree inventory; the regenerated whole-tree manifest immediately below is.

Across that snapshot:

- old assets: **763**
- current assets: **867**
- same-path semantic matches: **458**
- renamed semantic matches: **189**
- genuine same-path semantic changes: **35**
- old assets with no semantic current match: **81**
- new assets with no semantic old match: **186**

Thus **647 of 763 old assets (84.8%)** survive semantically unchanged before
making any design judgment. Raw hashes significantly overstate drift because
the expansion reorganized `graphics/interface` and normalized palette files.

| Family | Same path | Changed path | Renamed identical | Old unmatched | Current unmatched |
| --- | ---: | ---: | ---: | ---: | ---: |
| `bag` | 0 | 0 | 0 | 0 | 5 |
| `battle_frontier` | 28 | 1 | 7 | 4 | 2 |
| `battle_interface` | 21 | 13 | 2 | 7 | 38 |
| `birch_speech` | 5 | 1 | 0 | 0 | 1 |
| `contest` | 27 | 1 | 0 | 2 | 19 |
| `easy_chat` | 14 | 0 | 0 | 0 | 0 |
| `evolution_scene` | 9 | 0 | 0 | 0 | 0 |
| `frontier_pass` | 15 | 2 | 1 | 1 | 0 |
| `interface` | 4 | 0 | 73 | 43 | 9 |
| `map_popup` | 0 | 0 | 0 | 0 | 4 |
| `naming_screen` | 6 | 4 | 3 | 3 | 15 |
| `party_menu` | 0 | 0 | 0 | 0 | 5 |
| `pokeblock` | 22 | 2 | 0 | 0 | 0 |
| `pokedex` | 11 | 1 | 21 | 14 | 42 |
| `pokemon_storage` | 126 | 5 | 3 | 1 | 8 |
| `pokenav` | 59 | 0 | 55 | 1 | 17 |
| `shop` | 0 | 0 | 3 | 0 | 0 |
| `slot_machine` | 46 | 1 | 0 | 3 | 0 |
| `starter_choose` | 0 | 0 | 0 | 0 | 5 |
| `summary_screen` | 0 | 0 | 12 | 0 | 3 |
| `text_window` | 25 | 0 | 0 | 0 | 5 |
| `title_screen` | 7 | 2 | 3 | 0 | 1 |
| `trainer_card` | 7 | 1 | 21 | 2 | 2 |
| `types` | 26 | 1 | 0 | 0 | 5 |

Counts in the rename columns are old-source accounting. One current asset can
legitimately match more than one old duplicate, so the columns are not intended
to sum to the current total.

### Canonical current visual-tree manifest

`work/audits/VERDANT_VISUAL_BYTE_MANIFEST.json` was regenerated after the
render-led repairs and immediately passed a no-write freshness check. It
accounts for every selected reference and current visual source exactly once:

- reference assets: **14,842**
- current assets: **24,573**
- same-path byte-identical: **4,546**
- relocated byte-identical: **791**
- relocated pixel-identical: **89**
- changed: **7,418**
- removed: **1,998**
- added: **11,729**

The canonical UI group is reference **399**, current **466**, with 96
same-path-identical, 171 relocated-byte-identical, one
relocated-pixel-identical, 44 changed, 87 removed, and 154 added assets. These
are inventory classifications, not quality scores; the manifest deliberately
states that it makes no subjective visual judgment.

### Exact 35 semantic same-path changes in the initial curated census

These are the only scoped files whose decoded content differs at the same path:

#### Battle Frontier (1)

- `graphics/battle_frontier/tourney_info_card.png`

#### Battle interface (13)

- `graphics/battle_interface/ability_pop_up.png`
- `graphics/battle_interface/alpha_indicator.png`
- `graphics/battle_interface/healthbox_doubles_opponent.png`
- `graphics/battle_interface/healthbox_doubles_player.png`
- `graphics/battle_interface/healthbox_safari.png`
- `graphics/battle_interface/healthbox_singles_opponent.png`
- `graphics/battle_interface/healthbox_singles_player.png`
- `graphics/battle_interface/mega_indicator.png`
- `graphics/battle_interface/mega_trigger.png`
- `graphics/battle_interface/omega_indicator.png`
- `graphics/battle_interface/status2.png`
- `graphics/battle_interface/status3.png`
- `graphics/battle_interface/status4.png`

Decision: `HYBRID`, render-approved. These assets carry modern status, Mega,
Primal, Safari, Ability-popup, and healthbox capabilities. Runtime-state frames
now prove doubles status/Intimidate, Mega ready/active, opposing Primals,
Safari, L-info, and R-ball compositions. Do not copy old sheets over changed
dimensions. Alternate strings, transitions, and interaction states remain.

#### Intro/contest/Frontier chrome (4)

- `graphics/birch_speech/bg2.pal`
- `graphics/contest/results_screen/tiles.png`
- `graphics/frontier_pass/bg.png`
- `graphics/frontier_pass/map_screen.png`

Decision: `HYBRID`, render-approved. Birch speech, Contest Results, Frontier
Pass overview, and Frontier Pass map/list all render cleanly. None justifies a
blind restore because the new engine changed source-sheet organization and
facility support. Alternate values, animations, and navigation remain.

#### Naming screen (4)

- `graphics/naming_screen/cursor.png`
- `graphics/naming_screen/input_arrow.png`
- `graphics/naming_screen/menu.pal`
- `graphics/naming_screen/underscore.png`

Decision: `HYBRID`. Keep current engine geometry until the full keyboard is
rendered; require Emerald colors, borders, readable cursor, and predictable B.

#### Pokéblock/Pokédex (3)

- `graphics/pokeblock/use_screen/graph_data.bin`
- `graphics/pokeblock/use_screen/mon_frame.bin`
- `graphics/pokedex/bg_hoenn.pal`

Decision: `HYBRID`, render-approved. The Pokédex remains the native shell, and
the complete Pokéblock Condition screen proves the changed graph/frame data
still composes as Emerald. Alternate Dex pages and Pokéblock result extremes
remain.

#### Pokémon Storage (5)

- `graphics/pokemon_storage/box_selection_popup_center.png`
- `graphics/pokemon_storage/box_selection_popup_sides.png`
- `graphics/pokemon_storage/item_info_frame.png`
- `graphics/pokemon_storage/menu.png`
- `graphics/pokemon_storage/scrolling_bg.png`

Decision: `HYBRID`, render-approved. Root, boxes, Jump-to-Box popup, and Move
Items states prove the changed sheets compose inside the native PC shell. The
other 126 same-path storage assets are semantically identical. Operated item
transfer, summary, and repeated navigation remain; this is not a storage
rewrite.

#### Remaining intentional or bounded changes (5)

- `graphics/slot_machine/menu.png`
- `graphics/title_screen/emerald_version.png`
- `graphics/title_screen/press_start.png`
- `graphics/trainer_card/gold.pal`
- `graphics/types/fairy.png`

Decision: render-approved. Title branding and Fairy are `KEEP NEW`; Slot
Machine presentation is `KEEP OLD`; the gold Trainer Card is `KEEP NEW` support
in native chrome. All four composed states are clean.
`emerald_champions_version.png` is the intended live title mark, so the presence
of changed legacy `emerald_version.png` is not itself a release defect.

### Explicit WIP art restorations

Five working-tree assets were intentionally changed or added during this
native pass:

- `graphics/party_menu/bg.png` now exactly matches old
  `graphics/interface/party_menu_bg.png` (`SHA-256 66f37a952847…`).
- `graphics/party_menu/bg.pal` is the exact 176-color, eleven-bank Verdant
  palette used by the restored tilemap (`SHA-256 1b44d706c441…`). Loading the
  one-bank palette embedded in the PNG produced severe cyan/black corruption;
  the party, action, Ability, and Leveler renders now prove the repaired
  composition.
- `graphics/interface/status_icons.png` now exactly matches the old asset.
- `graphics/bag/select_button.png` now exactly matches old
  `graphics/interface/select_button.png`.
- `graphics/interface/category_icons.png` now matches old
  `graphics/interface/split_icons.png`, preserving the old palette language for
  a renamed/current role.

The party tilemap is independently byte-identical old/new
(`SHA-256 a14ed626ff30…`), as are the large and small party Poké Ball assets.
This is why the party decision is a safe skin restoration rather than a layout
port. It also shows why visual review cannot stop at decoded PNG equality: the
external palette-bank contract is part of the composed screen.

### World layout and tileset census

Map geometry was compared separately from UI chrome:

- **926/926** map/border blockdata files common to Verdant and the current tree
  are now byte-identical.
- **0** common map/border binaries remain changed or missing.
- **724** map/border binaries are new-only, principally restored/expansion and
  FRLG content; they cannot be judged against Verdant by hash.
- **28** common `map.bin` files differed at baseline and are explicitly restored
  to their exact old bytes in the WIP:

  `EverGrandeCity_PokemonLeague_1F`, `HouseWithBed`,
  `LavaridgeTown_PokemonCenter_1F`, `LilycoveCity_CoveLilyMotel_2F`,
  `MagmaHideout_4F`, `Mart`, `MauvilleCity`, `MauvilleCity_GameCorner`,
  `MirageTower_2F`, `MirageTower_3F`, `MossdeepCity`,
  `NewMauville_Inside`, `PokemonCenter_1F`, `Route105`, `Route106`,
  `Route111`, `Route119`, `Route124_DivingTreasureHuntersHouse`, `Route131`,
  `Route131_SkyPillar`, `RustboroCity`, `SafariZone_Northeast`,
  `SafariZone_Southeast`, `SlateportCity`, `Underwater_Route124`,
  `Underwater_Route126`, `Underwater_Route127`, and `VictoryRoad_B2F`.

Tileset raw hashes initially reported 1,239 changed common files, but almost all
were false positives caused by CRLF palette serialization or embedded PNG
palette metadata. After comparing external palette colors and indexed tile
planes, the actual common-data review is bounded to:

- `data/tilesets/secondary/cave/metatile_attributes.bin`
- `data/tilesets/secondary/cave/metatiles.bin`
- `data/tilesets/secondary/dewford/metatile_attributes.bin`
- `data/tilesets/secondary/dewford/metatiles.bin`
- `data/tilesets/secondary/dewford/tiles.png`
- `data/tilesets/secondary/pokemon_center/metatile_attributes.bin`
- `data/tilesets/secondary/rusturf_tunnel/metatile_attributes.bin`
- `data/tilesets/secondary/rusturf_tunnel/metatiles.bin`
- `data/tilesets/secondary/underwater/metatile_attributes.bin`

Metatile attributes are behavioral data and must not be reverted for visual
symmetry. The Cave, Dewford, Rusturf, and Underwater composed scenes require
runtime comparison; old blockdata with incompatible modern behavior metadata
would be worse than either coherent version.

## Source defects found during this pass

1. First wild action screen could write outside `gSprites` while creating the
   R-ball widget because the L-info sprite was still `MAX_SPRITES`.
2. L=A mode deliberately skipped L-info sprite creation, but selecting or
   canceling a move unconditionally hid that missing sprite.
3. Game Corner coin and top prize-menu frames overlapped by two tile columns;
   after moving the top menu to x=10, the regional species frames still merged
   against the coin frame until those submenus moved to x=11.
4. Game Corner return loops allocated repeated coin windows without removing
   the earlier windows.
5. Fossil dynamic menu's sixth visible row drew its frame on message row 14.
6. Battle-vendor seven-row category list entered the message box.
7. Initial starter-region B silently selected Hoenn.
8. Center move-deleter reuse initially referenced a foreign map's local object
   id.
9. Summary Rename/Move Relearner and field PC+party selection bypassed the
   approved Center flow.
10. Seven restored areas had lost their names and announced parent routes or
    towns.
11. The first full-orientation matcher declared its Mon Data field lookup array
    with an enum element type that this engine/toolchain does not expose as a
    complete array type. The build caught it; the live source uses a bounded
    `static const s32 statPointFields[NUM_STATS]` containing the six defined
    `MON_DATA_*_EV` fields.
12. The first independent vendor-cursor draft used nonexistent script symbols
    `VAR_0x800C` and `VAR_0x800D`. The assembler caught them; the live script
    now uses defined temporary variables `VAR_0x8008` and `VAR_0x8009`, with
    separate responsibilities and initialization at NPC entry.
13. Restoring the party background PNG without its full external palette loaded
    only one 16-color bank into a tilemap that addresses eleven banks. The
    result was severe cyan/black corruption in party and Ability screens. The
    176-color Verdant `bg.pal` is now loaded explicitly.
14. The shared nine-region selector could show six rows in a 12-tile window,
    colliding with the bottom message box. It now shows five rows in 10 tiles.
15. Oldale's bottom visitor clipped at y=8, while Lavaridge's restored room
    crowded its right-side service counter and bottom visitors. Object
    placement was rebalanced on visible walkable tiles.
16. Tornadus was stacked directly above a Route 119 sign, and Landorus occupied
    a collision-1 boulder tile. Their rendered placements now use a clear
    approach tile and valid ground, respectively.
17. The Circuit repeated long exposition and bypassed the native Tower
    corridor/battle-room staging. First-talk state and native room transitions
    now provide the intended presentation shell.
All seventeen have working-tree repairs. The Bag's magenta/cyan stripes were
initially suspicious in isolation but direct canonical-Emerald comparison
proved them to be native art, not an eighteenth defect. The two build-only
failures are retained here because they demonstrate why a successful
compile is part of UI proof, even when the intended change is only navigation
state. `summary-moves.png`'s blank description pane is expected because its
fixture never invokes the visibly labeled A-Info action. The current static command
`python3 scripts/verify_emerald_champions_native_ui.py` passes Center hashes and
geometry, League tutor placement, Ability flow, L/R sentinel guards, and all
five live non-debug dynamic-menu/textbox bounds. The rendered register above is
composition proof for its named final frames, not complete interaction proof.

## Unresolved runtime scenario matrix

The register above closes the static-composition question for its captured
frames. The matrix below now means **interaction, alternate state, or
uncaptured screen still outstanding**; it must not be read as saying that none
of these systems has been rendered.

### Opening and global navigation

- New Game and Continue from title, including old-save migration.
- Birch introduction's base composition is approved; gender/name entry, all
  nine starter-region choices, B-loop, three-ball scene, rival counterpart,
  first party open, animation and audio remain.
- Start menu, Save, PokéNav, and return cursors. Options, Pokédex-list, and Bag
  base compositions are approved; the Bag stripes match canonical Emerald.
- Hard/Medium/Easy changes before and after a loss, including persistence and
  actual -2/-4 trainer-level behavior.

### Centers and preparation

- Standard Center and Lavaridge variant collision/walk paths; their standing
  room compositions are approved.
- Nurse first gift, Vial refill, full Bag, and repeat heal.
- Battle vendor pre/post badge eight, every category, scrolling, B, quantity,
  free purchase, and return.
- Build Set with 2/3/4+ choices, current-set match, Mega set, protected item,
  no set, Egg, cancel, apply another, and long names.
- Learn All Moves/filter decision, no move, Egg, four known moves/replacement,
  and B from every layer.
- Forget Move and Rename from at least two Centers; Name Rater OT restriction.

### Party, Summary, forms, and progression tools

- Restored party skin with healthy, status, Egg, held item, six-member party,
  double/multi layouts, and empty slots. The captured one-member fainted base
  and action/Ability states are approved.
- Ability list with 1/2/3 unique Abilities, duplicate slots, Cancel row, B,
  same-Ability A, and changed-Ability return. The captured three-Ability list
  and current-selection presentation are approved.
- Summary Contest Moves, long species/item/Ability names, nature colors,
  #999/#1000 Dex boundary, and no Rename/Relearn shortcut. Info/Skills/Battle
  Moves, A-Info detail, Fairy Info/Moves, and party-return compositions are
  approved; alternate rows and close/return paths still require operation.
- Rotom Catalog, Zygarde Cube, fusion tools, Reveal Glass, Gracidea, and all
  campaign-obtainable form items.
- Leveler on 0/1/6 eligible Pokémon, Eggs, level 100/no-cap sentinel, one and
  chained evolutions, cancel expectations, and return to Bag/field.
- Vial registered/unregistered, 0/1/max charges, upgrades, refill.

### Battle controller and catch flow

- First wild action screen with no prior move-info sprite has a verified clean
  captured state; exercise all exits and later turns.
- R tap throw, R+D-pad cycle both directions, B cancel, one Ball, no Ball,
  trainer battle, Frontier battle, and prohibited catch.
- L details has a verified clean captured state; exercise open/close using
  L/A/B on each move slot.
- Button Mode Normal, LR, and L=A across action/move/target screens.
- Singles/doubles target selection and B returning to the same move.
- Mega-ready/active, four-status doubles with Ability popup, opposing Primals,
  and Safari action compositions are runtime-state approved. Trigger timing,
  alternate popup lengths/statuses, weather sequencing, switching/fainting,
  and every form remain.
- Full-party catch swap, HM restriction, PC full, nickname, and current-set
  disclosure.

### World information and restored content

- Short and maximum-length route signs, each encounter method, Hidden/Honey,
  Route 119 under-bridge Feebas, and maps with no wilds.
- Native map popup and region-map labels for all seven restored MAPSECs; repeat
  floors and parent return.
- All 32 visible one-off standing compositions are clean and runtime-state
  verified on their live maps. Dormant, missing-partner, awakened,
  defeated/remains, caught/hidden, full-party/full-PC paths and all animation
  frames remain.
- Cave, Dewford, Rusturf, Underwater, Center, Ever Grande, Mauville, and
  Frontier animated tiles affected by the nine bounded tileset data changes.

### Game Corner, fossils, storage, and Frontier

- Coin purchase, Doll menu, Genesect/Poipole, all nine starter regions, claimed
  entries, insufficient Coins, party/PC full, set failure, B at every level,
  and repeated transactions without window exhaustion.
- All eleven fossils, 1/5/11 held fossils, item icon, scroll, B, full party/PC,
  and return after revival.
- Name entry and storage Deposit/Withdraw/Move/Item/Summary/box select/cancel;
  naming plus root/boxes/Jump-to-Box/Move-Items compositions are approved, but
  the transactions and repeated return loops remain.
- Frontier Pass interactions, Dome tree, Factory selection, every retained
  native facility, and Circuit entry/continue/loss/reward/full-storage/retire.
  Both Frontier Pass pages, the Dome information card, and Circuit
  lobby/compact-welcome/battle-room compositions are approved; their complete
  state machines remain unproved.
- Pokéblock feeding/results, Trainer Card flip, Contest result transitions,
  Slot Machine spin/help/quit, and alternate values remain; their base
  Condition/Card/Results/Machine compositions are approved.

## Release decision

The correct direction is not to abandon the expansion engine. The failures
were at its presentation seams: old maps paired with new room blockdata, new
menus without message-box budgets, modern shortcuts bypassing the approved
service flow, and shared L/R widgets assuming each other's sprites existed.

The working tree restores the important old presentation contracts while
retaining current mechanics. The reproducible 91-scenario manifest now closes
the principal composition questions: Center shells, party/Ability, preparation
menus, the full 372-row Mew list extent, deep Storage, Frontier pages, modern
battle overlays, title/Birch, Pokéblock, cards, Contest, Slot Machine, Fairy,
and all 32 one-off encounters have clean evidence on one frozen fixture ROM.

It is still not legitimate to call every UI interaction finished: the matrix
above deliberately retains Back/cursor/repetition paths, alternate values,
animations, audio, story reachability, and complete state machines. Claimed
starter markers and optional post-capture set disclosure remain product
decisions. Difficulty's -2/-4 contract belongs in the guide rather than
another crowded Options window.
