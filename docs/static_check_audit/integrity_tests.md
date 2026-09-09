# Static-check audit: integrity/host tests

Initial read-only audit followed by explicitly approved test reduction, September 8, 2026. Trainer authoring was paused. E0076–E0105 had only been inspected: **no canonical edits or review record were made for that batch**. Earlier completed E0022–E0033 work remains separate. Finding line numbers describe the audited pre-deletion snapshot.

## Scope and actual execution

This report covers the 36 `tests/test_*.py` modules listed below. Production C, current script entry points, constants and selected build consumers were inspected to adjudicate fixture assumptions. Corresponding top-level verification scripts are the root agent's separate audit; campaign/map/evidence and trainer/preset/Circuit suites have other owners.

Baseline invocation used `PYTHONPATH=tests python3 -m unittest` followed by all 36 module stems below. Result: **133 tests ran in 67.893 seconds; 1 failure and 3 errors**. The restart class failed setup, so its seven test methods were not executed. The other two errors are method-level fixture compilation failures. This is a host/Python test result, not a native ROM result.

Six additional mutation probes ran by patching `Path.read_text` or the imported verifier **in memory**, constructing temporary host harnesses through the existing tests. No game source, generated data or trainer data was changed. Subsequent approved test deletions and the narrow save-test extension are recorded below.

## Confirmed findings

### I-01 — P2: save corruption coverage accepts removed checksum validation

Evidence: `tests/test_save_integrity.py:153` builds valid fixture sectors using the production `CalculateChecksum`; `:167`–`:192` tests erased sectors and invalid IDs, but never valid IDs with a corrupt checksum/payload. The production comparisons are in `src/save.c:510`, `:545`, and `:579`.

Reproduction: in the source read for `src/save.c`, replace every `gReadWriteSector->checksum == checksum` with `TRUE`; run `test_save_integrity`. **All 3 tests still pass.** In the real consumer, those comparisons are what reject corrupt payloads and prevent copying them into save blocks. The mutant is not a correct save loader.

Conclusion: the test does exercise useful status transitions, invalid-ID bounds and redundant-slot recovery, but cannot substantiate checksum corruption detection. This is a test coverage defect, not evidence that current production checksum checks are broken. Minimal correction: add a damaged payload or checksum after seeding an otherwise valid sector, test no invalid copy, and test recovery from the other intact slot. Include an independent checksum vector if checksum arithmetic itself is claimed.

### I-02 — P2: new live flag branches can bypass story preflight while the check stays green

Evidence: `tests/test_story_double_entry_integrity.py:23`–`:26` describes resolved-path handling, but `:60`–`:61` ignores **every** pre-check `goto_if_set` and `goto_if_not_defeated`, without checking flag, trainer or destination.

Reproduction: before each Petalburg Woods `call EventScript_CheckStoryDoubleBattleParty`, insert `goto_if_set FLAG_SYS_POKEDEX_GET, PetalburgWoods_EventScript_DevonResearcherIntro`. The flag is a real current constant (`include/constants/flags.h:1351`); the destination is the existing scene that starts actor movement. **All 4 tests still pass.** Actual script execution with the Pokedex flag set takes the branch before party validation; the fixture instead ignores it and executes validation.

Conclusion: “every other command fails closed” is false for new branches of these two command classes. Minimal correction: constrain known first-encounter branch assumptions to exact identities/targets, or model those flag/trainer states and follow reachable paths. Do not broaden ignored commands to cure the mismatch.

### I-03 — P2: Leaf reward identity and quantity are discarded by the script fixture

Evidence: `tests/test_leaf_scene_integrity.py:99`–`:105` models any `checkitem` as one generic possession boolean and any `giveitem` as one generic successful gift. Arguments are never validated. Actual source grants `ITEM_MEWTWONITE_X` at `data/maps/AlteringCave_B1F/scripts.inc:28`.

Reproduction: change only the in-memory `giveitem ITEM_MEWTWONITE_X` to `giveitem ITEM_POTION, 99`. **All 5 tests pass**, including first reward, storage retry and duplicate prevention.

Conclusion: movement/control-flow tests remain useful, but they do not verify delivery of the actual earned item. Minimal correction: inventory keyed by the actual item and quantity, or a narrow assertion of supported gift/check arguments; retain the full-bag and reentry cases. A separate global reward verifier may catch this independently; this result concerns the named fixture, not all gates collectively.

### I-04 — P2: Rydel fixture ignores native item quantities

Evidence: `tests/test_rydel_gift_integrity.py:67`–`:77` increments/decrements one item regardless of script quantity, and `checkitem` tests only nonzero possession. Actual bike script entry points use native `giveitem`/`removeitem` macros whose quantity arguments affect the bag operation.

Reproduction: append `, 99` to all Mach/Acro Bike `giveitem` commands in the in-memory Bike Shop script. **All 5 tests pass**, reporting a single successful bike delivery and correct exchange state. Actual bag delivery does not silently reinterpret the requested quantity as one.

Conclusion: current default-one commands are supported, but malformed explicit quantities evade the test. Minimal correction: parse the optional quantity with its actual default and model/check it, including the same restriction for remove/check operations. The existing full-pocket retry and exchange ordering assertions should remain.

### I-05 — P2: daycare negative controls pass an always-rejecting verifier

Evidence: `tests/test_daycare_discoveries.py:14`–`:22` accepts any `AssertionError` after each mutation. It neither verifies the unmodified source first nor matches the intended failure reason; every test in the module is a rejection case.

Reproduction: patch `gate.verify` to always raise `AssertionError('unrelated permanent verifier error')`. **All 5 tests pass.** A verifier that rejects every valid source is not correct.

Conclusion: a separate direct verifier run can still expose the problem, but this module cannot distinguish its intended negative controls from an unrelated permanent failure. Minimal correction: establish an accepted baseline and ensure the requested mutation actually changed the targeted input, then assert the relevant diagnostic/contract. Do not merely broaden the exception allowance.

### I-06 — P2: “non-grindy distribution” test proves selector arithmetic, not minimum encounter chances or compiled thresholds

Evidence: `tests/test_dive_encounter_integrity.py:18`–`:20` creates C threshold macros directly from JSON; `:39` and `:56` derive expected frequencies from that same JSON. It does not inspect actual species, enforce the approved minimum chance, or consume the generated thresholds included by `src/wild_encounter.c:73`.

Reproduction: change the in-memory land weights to `[89, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]`. **The test passes.** With current Route101's distinct species, this makes Mienfoo and ten other ordinary species 1% encounters, contrary to the user's 5% ordinary-species direction. No species change is required for the counterexample.

Conclusion: the test correctly checks mapping of 0–99 rolls, reversed order and the absence of a special underwater distribution under its constructed constants. Its name overstates the guarantee, and a stale generated threshold file is outside this fixture. Minimal correction: describe/retain the arithmetic test honestly; bind compiled-threshold freshness and aggregate species minimums to their actual owners. Do not impose a per-slot 5% rule where duplicate slots legitimately aggregate to a compliant species chance.

### I-07 — P2: restart fixture no longer compiles against the repaired rescue path

Evidence: `tests/test_restart_integrity.py:38`–`:59` extracts current restart/eligibility code but supplies no `IsEmeraldChampionsBirchRescueBattle` dependency. The dependency is now used by `src/battle_main.c:487` and eligibility code; its real definition is `src/emerald_champions_opening.c:110` and checks first-battle flags plus the opening state.

Reproduction: `PYTHONPATH=tests python3 -m unittest test_restart_integrity`. Class setup reports two undeclared-function errors and executes none of the seven scenarios.

Conclusion: this is a stale host fixture, not a production compile defect. Simply supplying a permanently false stub would also conceal the intended rescue exception. Minimal correction: include the real bounded predicate/context and distinguish ordinary first-battle exclusion from the actual current rescue state, while preserving exact party restoration and bag-key checks.

### I-08 — P2: legendary visibility fixture omits the regional-form distinction now read by production

Evidence: `tests/test_legendary_visibility_integrity.py:21`–`:35` imports the current species resolver but substitutes identity sanitization/base-form macros and provides no `gSpeciesInfo`. `src/legendary_signs.c:318` checks the four regional-form booleans to avoid conflating independent discoveries.

Reproduction: `PYTHONPATH=tests python3 -m unittest test_legendary_visibility_integrity`. Compilation fails on four undeclared `gSpeciesInfo` references; no state-transition assertion executes.

Conclusion: stale fixture environment. Minimal correction: retain current resolver semantics with a small explicit form table or configured source data, and cover an interchangeable battle form versus a distinct regional discovery. Do not remove the production distinction to satisfy the old synthetic base-ID model. Existing header documentation correctly limits the fixture's map-association claims.

### I-09 — P2: Steven handoff fixture and source assertions target the retired two-stone API

Evidence: `tests/test_story_handoff_integrity.py:15` extracts a text region now containing `GiveEmeraldChampionsStarterMegaStoneAtIndex`, but its harness still calls removed `GiveEmeraldChampionsStarterMegaStones` and omits `gSpecialVar_0x8008`, `GetEmeraldChampionsStarterStoneAtIndex` and `OwnsEmeraldChampionsMegaStone`. Its `:107`–`:109` assertions require old two-result variables/receipt labels.

Actual consumer: `src/field_specials.c:625` exposes indexed delivery and durable per-index bits. `data/maps/GraniteCave_StevensRoom/scripts.inc:26`–`:48` loops over four possible slots, tracks the receipt channel, and retries storage failure; `:131` uses `VAR_0x800B` to select a PC receipt.

Reproduction: `PYTHONPATH=tests python3 -m unittest test_story_handoff_integrity`: one host compilation error and one failed old-string assertion, while the separate item-consumption ordering method passes.

Conclusion: both environment and asserted API contract are stale. Minimal correction: exercise the actual indexed loop/receipt interface and preserve partial delivery, Bag/PC, ownership, duplicate-stone, durable-bit and full-storage behavior. Do not restore the obsolete two-stone API or force old variable names merely to make these checks green.

## Complete reviewed-file inventory

Every path below is relative to `tests/`. “No demonstrated defect” means no counterexample was established in this bounded audit, not complete verification of the owning subsystem.

| File | Reviewed behavior / result |
|---|---|
| `test_build_integrity.py` | Stamp bytes/schema/membership, real prerequisite Makefile blocks, forbidden release symbols, Hydra child-status decoder. Baseline passed; explicit decoder-only scope is accurate. |
| `test_native_tools_integrity.py` | Source/builder/command/executable cache identity, failed build preservation, nm parsing and mGBA discovery. Passed; compiler/library replacement at unchanged path is not independently exercised here. |
| `test_gba_build_config_integrity.py` | Isolated actual Makefile with fake tools; changed flags/tool bytes/receipts, mode isolation, filter repair, retries. Passed. LTO is explicitly disabled in this fixture, so not LTO pipeline certification. |
| `test_syscall_build_config_integrity.py` | Actual syscall Makefile, archive-member removal, include/tool/recipe changes, partial-tool failure and clean. Passed. Fake assembler checks invocation, not ARM instruction correctness. |
| `test_rom_artifact_integrity.py` | Actual objcopy wrapper with controlled output; byte equality and gbafix-style size/padding. Passed; no demonstrated defect. |
| `test_runtime_gate_integrity.py` | Named result/counter reconciliation, debt identities, process exit, timeout and filter wrapper accounting. Passed; synthetic log protocol coverage, not an actual emulator gate run. |
| `test_restart_integrity.py` | Exact snapshots, partner isolation, key rotation, eligibility. **I-07**, all seven scenarios blocked. |
| `test_save_integrity.py` | Status lifecycle, redundant slots, invalid sector IDs. Baseline passed; **I-01**. |
| `test_scheduler_integrity.py` | Actual scheduler against stable priority model, callback mutation, task-word followups. Passed. Deliberately preserves the existing full-capacity return contract; no new correctness claim about every caller of that contract. |
| `test_task_word_integrity.py` | Signed-halfword roundtrip, byte layout and invalid element bounds under UBSan. Passed; no demonstrated defect. |
| `test_ui_callback_integrity.py` | Cleanup, VBlank ordering, window counts, intro water-drop transitions with controlled APIs. Passed. Optional old entry points and source-root override support differential work, not complete UI integration. |
| `test_util_crc_integrity.py` | Independent bitwise Python oracle, long lengths, null/negative empty case, child timeout. Passed; oracle is independent of the production CRC table. |
| `test_util_word_integrity.py` | Halfword storage/layout, guards, defined old-expression domain, UBSan. Passed; no demonstrated defect. |
| `test_sprite_fill_integrity.py` | Buffer guards and independent single-sprite nibble oracle; spanning output pinned to prior byte-stream hash. Passed. Spanning correctness remains historical parity, not an independent geometry proof. |
| `test_world_memory_integrity.py` | Warp domain, allocation/zero-fill/copy lifetime using bounded stubs. Passed. Synthetic WarpData/task layouts and instantaneous DMA do not certify production header layout or real DMA completion. |
| `test_movement_delay_integrity.py` | Eleven actual dispatch routes, RNG/callback ordering and explicit delay arrays. Passed. Exact helper count/names are refactor ownership assumptions, not a general movement behavior requirement. |
| `test_follow_movement_integrity.py` | Actual copy/follow dispatch with controlled directions/collision/tile behavior. Passed; actual collision engine is outside its stated scope. |
| `test_jump_dispatch_integrity.py` | Actual jump family/direction mapping and completion, controlled animation. Passed. Host UBSan is enabled but not set fail-fast here; scenario assertions remain the principal outcome checks. |
| `test_difficulty_script_integrity.py` | Getter/setter/script effect ordering across value domain and config switches. Passed. Does not exercise campaign trainer level flooring or roster materialization. |
| `test_continue_object_scripts.py` | Bounded script refresh, clone indexing, preserved unrelated fields and explicit historical negative control. Passed; synthetic map layouts are acknowledged. |
| `test_animation_owner_integrity.py` | Actual callback ownership/initialization/steps with controlled trig, translation and destruction. Passed; no rendered-animation claim. |
| `test_sendout_sprite_lifecycle.py` | Actual controller wait/start boundaries under modeled transient sprite pressure. Passed. Player/opponent cases are exercised; other controller predicates are fixed false, not comprehensive controller coverage. |
| `test_stat_stage_owner_integrity.py` | Actual live versus AI context, Growth weather, Simple/Contrary and recording under UBSan. Passed with independent adjustment expectation; synthetic ability enum limits scope. |
| `test_hold_effect_status_integrity.py` | Extracted native status-cure cases, masks, unrelated state and ordered script calls. Passed; only three stated cure families. |
| `test_hold_effect_type_hit_integrity.py` | Native four reaction-item mappings, type/substitute/damage guards and stat-call order. Passed; native stat-cap/consumption script behavior is outside the stubbed boundary. |
| `test_daycare_discoveries.py` | Negative mutations against direct daycare verifier. Baseline passed; **I-05**. |
| `test_dive_encounter_integrity.py` | Roll-to-slot arithmetic against generated-in-test JSON constants. Passed; **I-06**. |
| `test_granite_flash_integrity.py` | Actual default/manual/unlocked Flash helpers plus old-resume negative control; source callback presence. Passed. Callback presence is not a full reachable script proof. |
| `test_leaf_scene_integrity.py` | Bounded scene interpreter, reentry/full bag/party checks. Passed; **I-03**. |
| `test_legendary_relic_integrity.py` | Native relic pending/delivered logic and gift ordering with controlled inventory/acquisition APIs. Passed. Storage, dex, acquisition policy and form resolution are intentionally stubbed, not native-world proof. |
| `test_legendary_visibility_integrity.py` | Intended ownership/unlock isolation with synthetic definitions. **I-08**, fixture did not compile. |
| `test_mega_stone_reward_routes.py` | Accepted native-stone coverage plus detached-NPC and duplicate-trade mutation negatives. Passed. Actual reachability semantics belong to the separately audited consumer. |
| `test_rustboro_guide_integrity.py` | Four gift paths and return position/lock state with explicit Fresh Water quantity assertion. Passed; player movement and native bag implementation are omitted by design. |
| `test_rydel_gift_integrity.py` | First grant, refusal, full pocket, swap and PC-owned cases. Passed; **I-04**. |
| `test_story_double_entry_integrity.py` | Preamble checks, late/missing check negatives, entry-source membership and Museum allow-single calls. Passed; **I-02**. |
| `test_story_handoff_integrity.py` | Native handoff/reward fixture plus source receipt checks. **I-09**, 1 error / 1 failure; item-consumption source-order method passed. |

## Latest user direction: delete unless absolutely necessary

The user subsequently requested aggressive test deletion rather than repairing brittle scaffolds or adding tests. The earlier “minimal correction” notes document the faulty contract; they are **not recommendations to rebuild those suites now**. Root accepted the following classification and coordinated deletion. The only explicitly approved functional test extension was the existing current-save corruption scenario, with no new module or framework.

### Retain: 13 files

| Exact file under `tests/` | Necessary retained purpose |
|---|---|
| `test_build_integrity.py` | Prevent mislabeled/stale release artifacts and hidden prerequisite failures. |
| `test_native_tools_integrity.py` | Prevent stale/corrupted native runner reuse and replacement of a good runner by failed build output. |
| `test_gba_build_config_integrity.py` | Real Makefile cache invalidation and failed-compile retries; stale objects materially falsify a build result. |
| `test_syscall_build_config_integrity.py` | Real syscall archive rebuild/member removal and failed-tool recovery. |
| `test_rom_artifact_integrity.py` | Reject different ROM/ELF bytes even when each has a valid metadata hash. |
| `test_runtime_gate_integrity.py` | Keep crashes, incomplete/mismatched logs and process failures from being reported as successful native evidence. |
| `test_save_integrity.py` | Current-save transaction state, sector-ID bounds and redundant-slot recovery. The approved existing-scenario extension now checks damaged checksums/payloads; this is not exhaustive checksum/flash certification. |
| `test_task_word_integrity.py` | Current callback/task address storage and bounds regression under UBSan, not a generic refactor trace. |
| `test_util_word_integrity.py` | The distinct sprite/util halfword address helper's demonstrated signed-shift corruption regression. |
| `test_util_crc_integrity.py` | Actual CRC arithmetic and known long-buffer counter hang with independent oracle and bounded child execution. |
| `test_world_memory_integrity.py` | Specific wide-coordinate truncation and decompressed-buffer allocation/overread regressions, with the stated host-layout/DMA limits. |
| `test_continue_object_scripts.py` | Specific Continue stale/out-of-bounds script pointer regression; deterministic prior-bug counterexample included. |
| `test_sendout_sprite_lifecycle.py` | Specific reproduced battle freeze/crash under transient sendout sprite pressure, guarding side effects while waiting. |

### Delete: 23 files

| Exact file under `tests/` | Reason to remove rather than repair now |
|---|---|
| `test_restart_integrity.py` | Broken extracted-code scaffold; do not rebuild its environment during the deletion pass. Preserve the shared helper noted below. |
| `test_scheduler_integrity.py` | Broad stable-priority/refactor trace/model contract; retained word-storage regressions cover the identified address corruption more directly. |
| `test_ui_callback_integrity.py` | Extracted UI ordering/parity scaffold rather than a demonstrated current blocker. |
| `test_sprite_fill_integrity.py` | Large historical raw-stream parity lock for spanning geometry; not essential release evidence. |
| `test_movement_delay_integrity.py` | Exact helper names/counts, delays and refactor-order contract. |
| `test_follow_movement_integrity.py` | Large synthetic dispatch/trace scaffold with collision behavior stubbed. |
| `test_jump_dispatch_integrity.py` | Synthetic animation dispatch/parity scaffold, not physical/rendered jump verification. |
| `test_difficulty_script_integrity.py` | Broad script API value/order matrix; does not protect the actually relevant campaign level-floor calculation. |
| `test_animation_owner_integrity.py` | Historical callback/trace parity with translation and rendering stubbed. |
| `test_stat_stage_owner_integrity.py` | Broad synthetic refactor context matrix; focused current native strategy cases belong to the separately classified runtime corpus. |
| `test_hold_effect_status_integrity.py` | Extracted narrow switch-case matrix, not complete native item behavior. |
| `test_hold_effect_type_hit_integrity.py` | Extracted case/ordered-call scaffold with real stat application stubbed. |
| `test_daycare_discoveries.py` | Negative-only wrapper can be false green; do not repair its brittle source-mutation framework. |
| `test_dive_encounter_integrity.py` | Self-derived distribution fixture with overclaimed policy/source guarantees; actual distribution owner is separate. |
| `test_granite_flash_integrity.py` | Small old-resume policy and source-presence scaffold, not an essential memory/save/artifact regression. |
| `test_leaf_scene_integrity.py` | Simplified interpreter with demonstrated reward false green. |
| `test_legendary_relic_integrity.py` | Synthetic acquisition/inventory fixture and exact reward-bit locks; no native delivery/storage proof. |
| `test_legendary_visibility_integrity.py` | Broken synthetic form model and extracted-code scaffold. |
| `test_mega_stone_reward_routes.py` | Wrapper around world-reward scanner/policy checks, classified separately by its owner. |
| `test_rustboro_guide_integrity.py` | Simplified scene interpreter and hardcoded choreography/gift contract. |
| `test_rydel_gift_integrity.py` | Simplified bag/script interpreter with demonstrated quantity false green. |
| `test_story_double_entry_integrity.py` | Demonstrably permissive preflight interpreter and source-shape locks. |
| `test_story_handoff_integrity.py` | Broken retired API fixture plus stale source-string contracts. |

**Deletion dependency resolved:** the original `test_save_integrity.py:12` imported `declaration` from `test_restart_integrity.py`. The exact existing brace-balanced extraction helper was moved into `test_save_integrity.py`, with `import re`; no replacement framework was created. Repository search found no other test/script imports of the deleted modules.

These recommendations do not classify/delete the native `test/` runtime corpus. The root's separately identified legacy `test/save.c` layout/backward-compatibility locks are not the transaction/sector-ID protection retained here.

## Implementation and post-deletion evidence

Root removed seven owned modules first: daycare discoveries, Dive encounter, Leaf scene, Rydel gift, story double-entry, story handoff and legendary visibility. This agent then removed the remaining **16** using `apply_patch`: restart, scheduler, UI callback, sprite fill, movement delay, follow movement, jump dispatch, difficulty script, animation owner, stat-stage owner, both hold-effect suites, Granite Flash, legendary relic, Mega Stone reward routes and Rustboro guide. All 16 are tracked working-tree deletions recoverable from Git; no commit or push was made. No game source was changed.

The first retained 13-module baseline passed **86 tests in 59.377 seconds**. Root then explicitly approved closing I-01 in the existing save corruption scenario, retaining the same three test methods. For each flash slot it now mutates either checksum or payload **after** producing a valid sector, requires overall corruption rejection and no copy of that damaged sector, then verifies recovery from the intact alternate slot with fresh destination sentinels.

An initial candidate assertion that no sectors could copy on an overall corrupt save was rejected as too strong: `TryLoadSaveSlot` intentionally calls the per-sector copier even after detecting an incomplete save, and that copier can preserve individually valid sectors. The final assertion protects only the damaged sector before checking full alternate-slot recovery. This was a correction of the test contract, not a production save change.

After the extension, the three current-save tests passed. Repeating I-01's in-memory mutant (all three checksum comparisons replaced by `TRUE`) now produces **one assertion failure, zero compilation errors** in the existing corruption test. Thus the original false-green counterexample is now detected. The fixture still does not certify real flash timing, every arithmetic checksum vector, power-loss behavior or complete save reload callbacks.

The final full retained-suite rerun, including the changed save fixture, passed **86 tests across all 13 modules in 60.388 seconds**. Scoped `git diff --check` also passed. No game source or trainer data was changed during this reduction.

## Boundaries

This audit established nine actionable test defects/overclaims, including six reproduced false-pass mutations and three stale fixture families. It did not prove nine current game bugs. The approved response removed the nonessential defective scaffolds and extended only the retained current-save scenario. No game-source fix, generator run, new framework or replacement test suite was performed; production behavior was not rolled back to satisfy obsolete fixtures.

Not all conceivable mutations were tried. Host-specific structure stubs, limited ABI coverage, historical output hashes, exact helper-name assertions, and constant scenario counts are documented above as limits, not automatically promoted into bugs. Full game timing, rendering, real save-flash behavior, all dynamic regional trainer variants, and whole-campaign playability remain separate native/runtime evidence.
