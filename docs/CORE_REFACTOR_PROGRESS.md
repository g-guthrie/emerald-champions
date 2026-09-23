# Core refactor progress

Scope: audit the original 49 core files (now 47 after story and Perish-plan consolidation), their supporting code/data and campaign interactions; debug, simplify and consolidate where responsibility overlaps. Do not merge unrelated modules just to lower a count. Preserve authored AI and team difficulty. Completion requires scoped source and behavior evidence for every area, integrated release checks, and the unfinished campaign/finale integration.

Statuses below are coverage records, not claims of perfection. Existing audit documents describe partial passes only.

| File | Coverage | Evidence |
|---|---|---|
| `battle_main.c` | Partial: restart snapshot/restore, battle-end evolution dispatch and cleanup ownership traced; completed-fade cleanup fixed; selected dirty-state initialization tested | CONTINUE.md |
| `battle_util.c` | Partial: lifetime, hazards, Commander attachment, Power Trick form refresh | BATTLE_UTIL_AUDIT.md; ABILITY_MECHANICS_AUDIT.md |
| `battle_script_commands.c` | Partial: EXP/EV recipient eligibility at max level | BATTLE_SCRIPT_AUDIT.md; PARTY_MENU_AUDIT.md |
| `battle_move_resolution.c` | Partial: restrictions, Encore and Pressure PP | MOVE_RESOLUTION_AUDIT.md |
| `battle_controllers.c` | Partial: message snapshots/transfers/raw access and animation-wait ownership tested | CONTINUE.md; test/battle_controller_messages.c |
| `battle_setup.c` | Partial: entry/end callbacks, owner flags, prize setup, generator lifetime, environment/transition selection and rematch selectors; fixed absent-owner flag corruption; 86 campaign groups pass before later behavior-preserving transition cleanup | CONTINUE.md; test/emerald_champions.c |
| `battle_gimmick.c` | Source pass; 18 native Mega groups/104 form cases | MEGA_MECHANICS_AUDIT.md |
| `battle_stat_change.c` | Source pass; partial behavior: bounds, preview purity, queue resets and reactive abilities | STAT_CHANGE_AUDIT.md |
| `battle_hold_effects.c` | Partial: Berry/PP ownership | HELD_ITEM_MECHANICS_AUDIT.md |
| `battle_message.c` | Partial: bounded expansion/encoded records, status tokens, trainer scratch; full current text suite passed | CONTINUE.md; test/text.c |
| `battle_ai_main.c` | Partial: trade/support scoring, ally-target legality, Fake Out partner-move ownership | ABILITY_MECHANICS_AUDIT.md |
| `battle_ai_pair.c` | Partial: sleep simulation and priority-order predicate; 1 broader test failure remains | ABILITY_MECHANICS_AUDIT.md |
| `battle_ai_util.c` | Partial: Protect predicted partner-slot ownership | ABILITY_MECHANICS_AUDIT.md |
| `battle_ai_switch.c` | Partial: optional switch legality now matches engine escape rules; stale active/fainted/partner reserve rejected before action emission; broader strategy audit pending | ABILITY_MECHANICS_AUDIT.md; work/ai-switch-legality-20260922/ |
| `battle_ai_record.c` | Source pass: per-owner records, temporary abilities/moves and switch history; six native knowledge groups pass | test/battle/ai/party_knowledge.c |
| `emerald_champions_battle_plan.c` | Source pass including unchanged consolidated Perish functions; 62-group suite initially57/62; level, Takao and Aisha fixture corrections pass focused reruns; Cristian tactical case remains | CONTINUE.md; work/battle-plans-20260921/; work/authored-fixtures-20260921/; work/takao-fixture-20260921/; work/aisha-fixture-20260921/ |
| `pokemon.c` | Partial: nickname initialization, shared battle EV gain calculation and regional Dex conversion bounds/round-trips | WILD_ENCOUNTER_AUDIT.md; PARTY_MENU_AUDIT.md; CONTINUE.md |
| `party_menu.c` | Partial: allocation-free panels/order cycles; optional setup cancellation/order recovery tested; mandatory failure policy preserved | PARTY_MENU_AUDIT.md; CONTINUE.md |
| `pokemon_storage_system.c` | Partial: setup-failure dispatch, repeat cleanup and carried-mon/item recovery tested; complete UI failure walkthrough pending | STORAGE_SYSTEM_AUDIT.md; CONTINUE.md |
| `pokemon_summary_screen.c` | Partial: sliding tilemaps allocation-free with full-buffer parity; entry/exit and moves source trace started | CONTINUE.md |
| `evolution_scene.c` | Partial: commit/cleanup state paths | EVOLUTION_SCENE_AUDIT.md |
| `daycare.c` | Partial | DAYCARE_AUDIT.md |
| `trade.c` | Partial: shared NPC transaction/handoff | TRADE_AUDIT.md |
| `overworld.c` | Partial: travel/whiteout resets, warp/fixed-Dive fallback, field-return callbacks and background ownership; null connection-header guard fixed and four native groups pass | CONTINUE.md; work/overworld-cleanup-20260921/; work/map-connections-20260921/ |
| `event_object_movement.c` | Partial | OBJECT_MOVEMENT_AUDIT.md |
| `field_control_avatar.c` | Partial: input/interaction/coordinate routing | FIELD_ROUTING_AUDIT.md |
| `field_player_avatar.c` | Partial: movement-state/graphics/coordinate helpers | PLAYER_AVATAR_AUDIT.md |
| `field_move.c` | Source pass; HM and selected puzzle tests | FIELD_MOVE_AUDIT.md |
| `field_effect.c` | Partial: palette allocation/script lifecycle | FIELD_EFFECT_AUDIT.md |
| `field_specials.c` | Partial: party query/form-mutation bounds and distinct Regi requirements | STORY_GATE_AUDIT.md |
| `script.c` | Source pass; selected native coverage | SCRIPT_ENGINE_AUDIT.md |
| `scrcmd.c` | Partial: context/compare/native-wait seams | SCRIPT_ENGINE_AUDIT.md |
| `event_data.c` | Source pass; bounds and campaign-gate regression | EVENT_DATA_AUDIT.md |
| `wild_encounter.c` | Partial: odds/caps, outbreak identity, roster bounds, local-query contract | WILD_ENCOUNTER_AUDIT.md |
| `item.c` | Partial: pocket/PC transactions | ITEM_TRANSACTIONS_AUDIT.md |
| `item_use.c` | Partial: Flight Beacon paths | FLIGHT_BEACON_AUDIT.md |
| `item_menu.c` | Partial: sale/deposit/toss trace; shared removal refresh/tutorial entrypoints; native allocation-failure callback recovery | CONTINUE.md; test/bag.c |
| `shop.c` | Partial: transaction source trace, quantity clamp, native TV purchase-history saturation/reset and Oldale exit capture | CONTINUE.md; test/shop_history.c |
| `start_menu.c` | Partial source trace: reload/navigation callbacks | POKENAV_DISPATCH_AUDIT.md |
| `region_map.c` | Partial: Fly selection/return and special destinations | FLIGHT_BEACON_AUDIT.md |
| `pokenav.c` | Partial: dispatch/transitions and invalid-ID guard | POKENAV_DISPATCH_AUDIT.md |
| `main.c` | Source pass: startup, frame/input/callback/interrupt/reset; L=A repeat natively tested, hardware timing unverified | CONTINUE.md; test/new_game_state.c |
| `new_game.c` | Source pass; dirty-state native initialization | NEW_GAME_AUDIT.md |
| `save.c` | Partial: shared slot validation and native recovery | SAVE_LOAD_AUDIT.md |
| `load_save.c` | Partial source review: persistence/rekey seams | SAVE_LOAD_AUDIT.md |
| `emerald_champions_story.c` | Opening/finale modules consolidated without behavior changes; comprehensive audit pending | STORY_GATE_AUDIT.md |
| `champions_circuit.c` | Source pass: bounded pools/retries, move/ability/item/EV heuristics, lead ordering, rewards and party lifecycle; 12 native groups incl2048 seeds pass; projection756 variants/1322 templates checked; facility traversal unverified | CONTINUE.md; work/circuit-cleanup-20260921/ |

Visible legendary script integration: consolidated four duplicate result blocks
(Regigigas/Articuno/Zapdos/Mewtwo) and connected object flags to the existing
capture ledger. Thirteen native party/legendary groups pass; see STORY_GATE_AUDIT.
This extends campaign-script coverage, not the entire battle_setup.c audit.

## September 22 — Claude continuation (game-specific code audits)

Full suite baseline this session: 1639 tests, 55 failing; now 1636 tests (three
tests of deleted dead code removed), failures down to the upstream AI group,
Cristian and intermittent save-recovery timeouts (triage in progress).

Fixed (each built; suite rerun, no new failures):
- Harvest pouch: a berry type at 255 blocked picking that tree forever (most
  types are never spent by a trade). Count now saturates; picking never
  refused; dead "pouch full" script/text removed; tests updated.
- Pikachu costume to Plain could leave it moveless; shared helper gives
  Thunder Shock (ORAS) and the preview shows the real result.
- Battle-item unlock bits bounded to the save array (stock beyond stays open).
- Rival starter index underflowed without a second starter (legacy saves).
- Script-given template IVs were discarded (ResolveIVs removed in 7.0);
  explicit IVs are honored, unspecified keep the universal 31 (no live script
  passes IVs). `giveegg` eggs now follow the all-31 egg rule like breeding.
- Legendary sign rewards get a level-up moveset and real stats before the
  preset, so a species without a usable preset is never moveless at 0 HP.
- Obedience cap for battle-only forms (Crowned, Complete) uses the species
  that entered battle (`changedSpecies`).
- Regenerator policy leaks: Corrosive Gas cleared the origin without recording
  a loss (berry came back free); a foe's Pickup/Recycle of the player's eaten
  Berry cleared its consumption. Both now count as removal until the Berry
  genuinely returns; two regression tests added; all 14 Regenerator tests pass.
- Winstrate gauntlet re-paid prize money every reset (flags cleared on
  re-entry). Three unused, unaliased flags renamed to per-Winstrate "paid"
  markers; eligibility checks them and payout sets them.
- Test debt: givemon tests set the Champion cap; InheritIVs upstream tests
  replaced by one test of the all-31 egg rule; Knock Off/Sticky Hold start HP
  moved above the deliberate half+1 Berry threshold (7.0 "one HP above the
  ordinary cutoff"), which ate the Berry at battle start; berry harvest test
  updated for saturation; center-guide test no longer expects Frontier leads.
- Dead code removed: 15 uncalled specials and their prototypes/rows/tests
  (Castform locator, Shoal Fen trades, meadow song/Castform survey prep,
  opening balls, starter naming helpers, berry pair, harvest credit minting,
  redundant purchase check, sign object/level queries, GetSignLevel,
  AreChosenMonEVsMaxedOut), 19 EC_Require* script blocks, Castform constants.
Kept pending the user's decision: Game Corner Genesect/starter archive code
and the Champions Circuit (both lost their entry in the Inclement rebase).

### Later Sept 22 — field engine, menus, code-graph dead-code sweep

Fixed (full suite after the batch: no new failures; same 40 AI/Cristian set):
- Berry pocket bundle-space check could route into a null overflow array.
- Reload offered for a different save file (`gDifferentSaveFile`).
- Honey reset Dig/Escape Rope warp state; Repel Spray counted steps in the
  Pike/Pyramid; `checkpartyhasspecies` counted Eggs; Repel Spray step count
  set from its constant.
- Object palettes: a failed reload after freeing the old slot left the sprite
  drawing from the freed palette. It now joins the existing pending-palette
  path (hidden, retried by its movement callback).
- Last Resort tests pinned to Gen 9 selectability (Champions config makes the
  move unselectable before the others, which the tests don't exercise).
- Dead code (confirmed by the codebase-memory graph: no CALLS/USAGE edges,
  then a whole-tree token check including scripts/specials): the free
  catalogue mode in `shop.c` (`CreateFreePokemartMenu`, `freeItems`, catalogue
  texts), five unused specials (Route 116 Dusk Stone, Pacifidlog stone day
  pair, Furfrou/species change), `AI_RefreshCandidateFieldEffects`, and the
  duplicate `src/pokenav_call.c`/`include/pokenav_call.h` (the `pokenavcall`
  command runs through the restored inert Match Call in `match_call.c`).
  Upstream/Inclement engine functions without callers are kept on purpose.

Pending the user's decision (in addition to Circuit and Genesect):
- In-game trades: the Sept 2 design had six trades; the Inclement world
  layout keeps four NPCs (Rustboro→Fidough, Dewford→Tropius,
  Verdanturf→Chansey, Fortree/Frontier→Type: Null). The authored
  Bombirdier-for-Volbeat (Roman) and Cyclizar-for-Bagon (Skylar) entries have
  no trader. Remove them, or give them an NPC.
- AI (triage `work/`): singles Ace-last ordering dropped in d17f80288f;
  `CanBattlerKOTargetIgnoringSturdy` can never fire (cached damage is clipped
  to hp-1). Both change AI decisions, so they wait for approval.

### Later Sept 22 — user decisions applied; story and save audits

User decisions (Sept 22): the game has no singles battles; trades should fit
the whole game's species sources.
- Trades: Bombirdier and Cyclizar had no wild, gift or egg source while the
  Dewford (Tropius) and Verdanturf (Chansey) trades duplicated wild species
  (Tropius on Routes 119/120; Chansey in the Safari Zone and via Route 102
  Happiny). The orphaned entries carried Roman's and Skylar's exact OT, otId
  and personality, so Dewford Hall now trades Bombirdier for Croagunk
  (Petalburg Woods, just before Dewford) and Verdanturf trades Cyclizar for
  Igglybuff (Route 117 next door). All four traders now give species with no
  other source (Fidough, Bombirdier, Cyclizar, Type: Null); stale
  Tropius/Happiny entries and the Pawniard/Karrablast aliases removed.
- Singles: every scripted trainer battle is already doubles (315
  `trainerbattle_double`, 68 `trainerbattle_no_intro_double`). Remaining
  singles are the Battle Frontier and Tents; the user chose to convert them
  to doubles (Arena and the Arena Tent close). Wild battles remain singles.

Story/finale audit (no softlocks, wrong flags or lock leaks found). Applied:
- Headless starter-region scenario now drives the live
  `RivalsHouse_EventScript_ChooseStarterRegion`; the divergent copy and its
  texts are deleted.
- Dead: the Deoxys form service (gated on a never-set flag), the milestone
  macros/specials/functions and cap-line script (no callers; comment
  described behavior the code didn't have), the headless-only opening states
  3/4, the Southern Island branch in the static-legendary builder, and the
  C-only `GetHeatranDiscoveryState` special row.
- Steven's early line and the S.S. Tidal sailor now give the shared finale
  directions (the sailor previously said only "enjoy your cruise" when the
  cabins were cleared before Wally).

Save/storage audit. Fixed:
- Link-type saves (every Lilycove contest, Frontier, record mixing, Blender)
  rewrote PC sectors from their old flash contents. With Reload available, a
  withdraw→contest→reload duplicated the Pokémon and a deposit lost it. All
  sectors are now written from RAM, like a normal save (same sector count).
- Releasing a fused form in the PC stranded its partner and blocked that
  fusion slot permanently; release now asks to separate first.
- New Game kept the previous file's fused partners; storage reset clears them.

### Frontier doubles conversion — implementation in progress (September 22)

All facility battle types are set in `DoFacilityTrainerBattleInternal`
(`src/battle_frontier.c` ~73; scripts reach it via `dofacilitytrainerbattle` /
`facilitytrainerbattle`). Pyramid floor approaches use
`BattleSetup_StartTrainerBattle` (`battle_setup.c` ~1471). Tents reuse the
Palace/Factory paths; `data/scripts/champions_tent.inc` is unreferenced.
Implemented so far: Tower, Dome, Palace and Factory start trainer fights as
doubles regardless of an older saved Singles record. Pike lone-trainer rooms
have two opposing Pokémon and Pyramid floor trainers have two per lone trainer;
the Pyramid top battle also carries the Double flag. Tower, Dome, Palace and
Factory Singles counters close and point to the Double counter. Arena and the
Fallarbor Tent close; their room-entry callbacks return to the lobby, and paused
lobby runs restore the party instead of resuming a singles match. Slateport and
Verdanturf Tents now select doubles mode. Wild encounters remain singles.

The release ROM builds and release gates pass (current input stamp
`d30ec5709f06`, rebuilt after the concurrent Littleroot/player-house edits).
Synthetic native scenes on headless ROM `89aebd8917eeb4d8` show all five
closed counters returning controls, both retired battle rooms warping to their
lobbies, and the Tower Double counter reaching the four-Pokémon party chooser.
The Pike's one-usable-Pokémon room exclusion passes a focused native test over
hinted and random selections, with and without healing rooms. Tower Doubles
accepts four distinct party members and visibly presents two opposing and two
player Pokémon. An explicitly assisted-win run advances through at least two
matches to the third opponent's prompt on headless ROM `8c0f362d2afde3c3`;
it verifies result/re-entry scripting, not tactical difficulty. Arena/Fallarbor
paused-save return remains unverified. The active Frontier reception guide and
nearby Arena references now describe the open doubles facilities. The Lounge
gambler offers only seven reachable modes and refunds an old wager on a retired
mode once; this passes a focused native test and an on-screen refund scene.
An earlier scene used stale ROM `a559d8c6cd73507a` and is not evidence
for the current edits. Studio needed the existing x265 4.1 library scoped to its
worker process; the live Studio server is stopped and had a stale resume state.

Remaining checks and decisions:
1. Test party selection, battle startup/outcome/re-entry for Dome, Palace,
   Factory, Pike, Pyramid and Tents, including old paused Singles records.
   Dome opponent selection uses two from three; Tower's four-party entry and
   the first result/re-entry path have scoped native evidence.
2. Review remaining Single wording on resume/results paths and Elevator/Multi
   Corridor variants. New Singles entries already close. Keep old record slots.
3. Check the mandatory Pike Brain with one usable mon against the established
   global rule. Optional Pike trainer rooms require two usable Pokémon. Pyramid
   wild encounters remain singles; verify its trainer floor and top battles.
4. Check Arena/Fallarbor paused-save return and existing pending prize delivery
   on a native save; the dormant Arena battle handler itself remains present.
5. Confirm facility AI flags and trainer teams in native matches. Preserve the
   authored campaign AI and avoid shared planner changes without diagnosis.
6. The latest full native test run currently has 1 failure out of 1,628 groups:
   the preexisting Sturdy setup AI case. 1,618 pass and 9 are expected failures.
   No shared AI strategy was edited for this conversion. Evidence:
   `work/frontier-doubles-20260922/after-concurrent-full-check.log`. The
   current release gates pass in `after-concurrent-release-gates.log`.
Note: Tent corridor/battle-room scripts already carry uncommitted edits.

Handoff research for items 3 and 5 (Claude session; read-only, nothing edited):
- Facility AI: `GetAiFlags` (`src/battle_ai_main.c` ~291) gives facility
  trainers only `CHECK_BAD_MOVE | CHECK_VIABILITY | TRY_TO_FAINT`. Factory's
  `GetAiScriptsInBattleFactory` returns 0 for the Tent and early challenges,
  and the doubles flag is added only when the others are non-zero, so those
  trainers get no doubles AI at all. Campaign trainers, Trainer Hill and the
  Circuit share the ~264 profile branch; routing non-link, non-recorded facility
  battles there would match them. The pair planner already skips facility
  battles (`GetCampaignTrainer`). This changes facility trainer behaviour
  (user approved converting facilities to doubles; call it out if applied).
- USER DECISION (Sept 22): wild battles stay singles everywhere, including
  Pike and Pyramid wild encounters. Only trainer battles are doubles.

### Sept 22 evening — suite green except the pending Sturdy decision

Full `make check`: 1626 tests, 1616 pass, 9 expected-failing, 1 failing
("AI won't setup if it can KO through Sturdy effect" — a real AI bug held for
the user's approval). AI fixture repairs (test-only; no AI code touched):
chip-penalty fixtures reshaped to ≤4HKO or expected values include the −1;
median-roll HP recomputed; Order Up speeds set so +3 overtakes; doubles
switching tests rewritten as pair-planner assertions or removed where the
upstream random/singles path never runs (no singles battles in this game);
Weakness Policy Protect asserts `ProtectChecks` directly; Cristian Beat Up
fixtures corrected (Annihilape isn't Dark-weak; Lucario case was masked).
Also: Last Resort tests pinned to Gen 9 selectability; the Lati half of the
legendary retry test and the "link save preserves the PC" assertion updated
to the fixed behavior; runner self-test premise made AI-independent.
Native: Dewford Hall offers Bombirdier for Croagunk; Verdanturf offers
Cyclizar for Igglybuff (`work/claude-20260922/trades/`).

### Coordination (Sept 22, 17:20) — concurrent sessions in this tree

Two Codex threads and one Claude session are working here. To avoid
overlapping edits, the Claude session is auditing and fixing only:
`evolution_scene.c`, `daycare.c`, `pokemon_summary_screen.c`,
`item_menu.c`, `region_map.c` and `trade.c` (plus their tests/scripts).
The Frontier conversion and the AI files stay with the Codex core-refactor
thread. The Claude session takes `/tmp/emerald-champions-build.lock`
(mkdir; `owner` file) before any make and removes it afterwards.

### Sept 22, 17:40 — baseline after the Codex threads stopped

Codex's last in-flight edit (Sturdy/Focus Sash setup guard: when the cached
damage was clipped to hp−1, recheck with the raw single-hit calculation) is
complete. Headless ROM builds clean; full suite: 1633 tests, 1624 pass,
9 expected-failing, 0 failing (`work/claude-20260922/check-run9.log`).
The Claude session now owns the remaining overworld, campaign and NPC dialogue
work; a four-segment, route-by-route read-only audit is running
(`work/claude-20260922/dialogue-audit/`).

### Sept 22 evening — evolution, daycare, trade, Bag, summary, Fly audits

Fixed:
- Held-item level-up evolutions (Oval Stone, Razor Fang/Claw…) spent the item
  when the scene began but could still be cancelled with B, losing the item.
  They are now uncancellable, like bag-item evolutions. Test added.
- Nidoran♂ and Volbeat were unbreedable: the offspring gender roll read a
  field nothing wrote since 231531c6a3. The roll is restored when an egg is
  triggered. Test added (Ditto with Nidoran♂ / Volbeat yields both species).
- NPC trades never returned the offered Pokémon's held item (the helper was
  only wired to the unused `ingame_trade` macro). All five traders now return
  it first; Mail or a full Bag/PC cancels. Species names are re-buffered so
  post-trade lines don't show the item name.
- Bag allocation failure left a stale `gSpecialVar_ItemId` (battle could reuse
  the last item; Ball Swapper/fossil/planting scripts would act on it).
- An emptied L/R binding swallowed the press and showed the SELECT message.
- Ball Swapper stored the item ID as the ball ID (works only for IDs 1–27);
  now converted with `ItemIdToBallId`.
- Primal Orbs used Mega Stone wording; summary screen no longer resets the
  relearner state when that feature is off; TriggerEvolution assert bound.
- Dead: `FLAG_EC_HATCHED_DAYCARE_EGG` (set, never read), two unused includes.
Kept (Codex's deliberate change): the Daycare now allows depositing down to
one healthy Pokémon.

### Frontier doubles conversion — Claude continuation (Sept 22 evening)

- Facility AI: facility trainers had only CHECK_BAD_MOVE|CHECK_VIABILITY|
  TRY_TO_FAINT (Factory/Tent early streaks: no flags at all, so no doubles
  AI). Non-Palace, non-link, non-recorded facility trainer battles now use the
  shared campaign/Hill/Circuit doubles profile (`GetAiFlags`). Wild Pike and
  Pyramid encounters are excluded; no AI decision code changed. User was told
  this changes Frontier difficulty.
- Frontier Brains now appear in Doubles (never Multis); Scott's shields read
  the Tower Doubles streak and his symbol Berries no longer need the Arena;
  Exchange Corner clerks rebuilt to sell what their menus list.
- Suite 1626/1635 pass (9 expected), release gates PASS.
Still open from the Codex list: native party/outcome/re-entry checks per
facility, Pike Brain one-usable-Pokémon rule, Arena paused-save return, and
the Ranking Hall still reading Singles records (user decision).

### September 23 — current AI Sturdy result

The earlier Sturdy setup failure is resolved in current source. The cached
damage was clipped at HP−1; the existing raw simulator now supplies the
pre-survival attacking roll only for that guard. Sturdy and Focus Sash native
fixtures pass; full suite is 1,627 passed, nine expected failures, zero
unexpected failures out of 1,636. Release gates and input stamp pass for ROM
SHA-256 `089f74a248b8ffca598aa334a2622630dd52f623453262db8bbf3d11a68d8cd5`.
Earlier failing counts above remain historical snapshots. See
`docs/ABILITY_MECHANICS_AUDIT.md` and `work/sturdy-diagnosis-20260922/`.

### September 23 — Dome Doubles native entry and first result

On headless ROM SHA-256
`b669f5c8a0f5cb2b7b73bffd63b1b2d8766501d822b35dc22a3b846c41454c08`,
the Double attendant accepts three Open Level tournament Pokémon. A separate
two-Pokémon selector leads to a native trainer battle with two visible opposing
and two visible player Pokémon. An explicitly assisted first win returns to
the prebattle room and advances its prompt from Round 1 to Round 2. Evidence:
`work/frontier-doubles-20260923/dome-two-leads-visible/` and
`dome-first-win-return/`. The forced win is script evidence, not tactical
validation; the full tournament, paused-save recovery and other facilities
remain open. Later unrelated source edits were integrated into release ROM
SHA-256 `5c3e9bbffa0b69ba949d7eb1fb993f9be76e9a634003e66c340f8f64ea25e636`;
release gates and its input stamp pass for that snapshot. Re-check after any
further source edits.

Pike mode correction: the lobby attendant now sets `FRONTIER_MODE_DOUBLES`;
lobby and paused Three Path entry normalize old Singles-mode saves before
status handling. Pike's streak array is indexed by level only, and both modes
read the same record. Gambler challenge ID 10 remains stable, now matches
Doubles, and still settles an old Singles-mode bet. One- and two-trainer Pike
room type names now describe opponent count, with their saved numeric IDs
unchanged; both launch native double battles. Focused gambler/one-usable-mon
tests pass. A synthetic physical entrance from Outside West on headless ROM
`b7452996bfcf5964cff588b3111f9d97cec5003eefb402bae92bfed009a42440`
converts mode 0 to 1 and facility to Pike before the player talks to the
attendant. The Brain battle and full streak remain unverified. Current release
build must be repeated after concurrent map-script edits.

### September 23 — boxed Legendary gift choice

When a Legendary gift is boxed because the party already contains one, the
transfer dialogue now offers to replace that party Legendary. Accepting swaps
the two Pokémon through the gift's exact PC slot, preserving both and their
held items even if storage is otherwise full; declining leaves the gift boxed.
Mail on the party Legendary prevents replacement and leaves the gift safe in
the PC. The shared gift transfer script and Devon's custom Legendary rewards
use this choice. Captures and in-game trades are unchanged.

The Steven-house Meltan gift was checked in native scenes for the prompt and
both responses (`work/legendary-gift-choice-20260923/`); a follow-up scene
verified the corrected box-name confirmation. Focused C tests cover
the box-slot swap, Mail refusal and scripted gift. The full native suite had
1,639 passes, nine expected failures and one timeout in the existing save
recovery test; that same test passed on immediate retry against the stamped
test binary. Release gates and the input stamp pass for ROM SHA-256
`92da90ea5e2b9cfcae33bca0841667ec3308b6c13f17ce80696e5762464b29aa`.
