# Campaign, map, and agent evidence check audit

Date: 2026-09-08. Source audit plus host/Python negative probes, followed by explicitly approved test/gate deletion. No gameplay, trainer, or generated game-output changes. E0106–E0135 trainer authoring was paused before any edits. Recommendations below follow the user's instruction to delete unnecessary tests rather than preserve historical design locks. Finding line numbers refer to the pre-cleanup files examined, including subsequently deleted files.

## Approved cleanup performed

Deleted nine complete files: `tests/campaign/test_battle_checkpoint_lab.py`, `tools/agent_player/test_agent_player.py`, `tests/test_map_dynamic_inventory.py`, `scripts/audit/reach.py`, `scripts/audit/show_label.py`, `scripts/audit/ec_baseline_diff.py`, `scripts/verify_emerald_champions_campaign_interactive_scope.py`, `tests/campaign/interactive_flow_scope.json`, and `scripts/verify_emerald_champions_campaign_run.py`. Root verified that all nine were tracked and remain recoverable through Git.

Pruned baseline-comparison classes/imports/fixtures, legacy item-report/source-text tests, extraction-environment scaffolding, exact Takao ID, eleven benchmark-accounting tests, four exact map-census tests, and three broad map/dynamic-allowance assertions. **57 test methods removed from this owned inventory; 66 retained** (34 campaign, 16 agent integrity, 16 map integrity/render integrity). The actual save/parent-ancestry/import, ROM/ELF admission, no-budget-refund restore, invalid-observation and unintended-step checks remain.

The campaign runner's capture-token audit is advisory rather than fatal; capture/prerequisite helpers remain because the existing diagnostic output consumes their reports. The appendix generator retains explicit `--write` export but no `--check` freshness certification. No report was regenerated. README no longer directs historical baseline comparisons. Root separately narrowed the mandatory map inventory to `include_dynamic=False`; its shared helper remains available, without claiming dynamic execution proof.

Post-cleanup verification: campaign 34/34, agent integrity 16/16, and map integrity/render integrity 16/16 passed. Campaign `--validate-only` passed 227 segment symbol/structure checks (not gameplay); generator `--help` exposes only `--write`; `git diff --check` passed. No live consumer import references the deleted comparator or interactive-scope module. Historical copies inside `Cohesion Book/baseline/source/` were left untouched.

## Findings

### P1: map animation allowance ignores part of the actual destination

`scripts/audit/map_dynamic_inventory.py:241–248` searches for a `TILE_OFFSET_4BPP(...)` substring and derives the allowed range solely from that substring. It does not require the complete destination expression to match the supported form. Its result is consumed as an animation allowance by `map_integrity.py`.

Executed in-memory mutation: replace `TILE_OFFSET_4BPP(508)` in `src/tileset_anims.c` with `(TILE_OFFSET_4BPP(508) + 0x20000)`, then call `animation_inventory(ROOT, read_constants(ROOT))`. Both modified flower transfers remained `status: resolved`, range `[508, 512]`, although their destination is now beyond GBA VRAM. The existing inside-the-macro out-of-range mutation test passes but misses this case. This is a checker false pass, not an observed current game transfer defect.

Recommendation: remove this speculative dynamic-allowance certification and its large parser suite from mandatory development gates. If a small structural map check is retained, do not treat these ranges as proof that unresolved tile art is supplied.

### P1: battle checkpoint construction bypasses the existing parent provenance boundary

`scripts/battle_checkpoint_lab.py:468–490` loads a parent sidecar, takes its ROM/ELF paths, copies the state, and runs it without calling `campaign.validate_state_input`, validating the recorded parent-state hash/ancestry, or verifying that ROM/ELF pair. It later hashes the bytes it happened to use (`:545–546`); that does not prove those bytes were the earned parent checkpoint. The campaign runner's normal resume path has these checks at `run_emerald_champions_campaign.py:424–468`.

Source-path finding; no emulator corruption experiment was run. A stable same-ROM replacement state is not excluded by this construction path. Do not infer this lab is protected by the campaign provenance tests merely because it imports that module.

Recommendation: retire the duplicate checkpoint-lab construction/index/arsenal framework rather than expand it. Preserve the campaign runner's actual state/sidecar/ancestry boundary and its small regression tests.

### P1: battle-lab automation-disable check trusts a symbol label, not its address

`tools/agent_player/agent_player.py:215–216` accepts any initialization record named `gEcHeadlessFixtureActiveScenario` with value zero. `_resolve_records` at `:266–281` leaves a supplied numeric address untouched, including when a symbol is also supplied. The write verification at `:407–412` reads back that supplied address, not the named symbol.

Executed `_resolve_records` on a session with no ELF and `{symbol: gEcHeadlessFixtureActiveScenario, address: 0x02030000, value: 0}`. It accepted that unverified address, retained the symbol label, and produced a record satisfying the battle-lab admission predicate. Therefore this predicate cannot establish automation is disabled. No native auto-win was executed in this audit; this is a verified configuration-validation bypass.

Recommendation: do not certify native battle evidence from this label check. Retain the actual artifact/restore integrity checks only; avoid adding another broad benchmark validation suite.

### P2: the legal arsenal still grants every Mega Stone upon owning the Mega Ring

`scripts/battle_checkpoint_lab.py:201–203` treats all item tokens in `emerald_champions_mega_stones.h` as available when the ring is owned. They enter preset legality at `:243–246`. That contradicts the current world-pickup/gift/exchange stone acquisition model: the ring permits Mega Evolution, not ownership of every stone.

Executed `materialize_legal_arsenal([], 14, [{species_id: 4, slot: 0}], [], [{item_id: ITEM_MEGA_RING numeric value}])`: it granted **99 Mega Stones**, starting with Abomasite, Absolite, and Absolite Z, although only the ring was supplied. This is actual output from the consumer, not a stale document inference.

Additional boundaries: map-local gift/static species are harvested from any matching script text (`:192`), evolution edges ignore additional conditions (`:219–225`), and move/ability legality is drawn from imported Showdown JSON/raw unpreprocessed species text (`:248–259`). Those are not physical-access or compiled-legality proofs. The current Calvin probe had no empty move lists; no blanket claim of broken current learnsets is made.

Recommendation: delete this approximation as a legality authority; do not preserve it by rewriting gameplay or adding availability quotas.

### P2: “validated” checkpoint status does not mean validated

`scripts/battle_checkpoint_lab.py:437` sets index status to `validated` solely when a file exists. Executed `checkpoint_index` with file existence mocked true and writes intercepted: the nonexistent virtual `route102-calvin/checkpoint.json` received `validated` without being read.

Separately, `validate_checkpoint` at `:567–590` does not enforce the published nested schema. With matching artifact hashes mocked, a payload containing empty `identity`, `encounter`, `campaign`, and `preparation_adapter`, and `pokemon: [{}]`, was returned as `validation_status: validated`. This experiment isolates validation logic, not real artifact bytes.

`tests/campaign/test_battle_checkpoint_lab.py:40–55` does not test its named auto-win rejection: its fixture omits three required top-level fields, so the executed rejection is `checkpoint has invalid top-level schema` at `:572`, before the auto-win check. Removing the auto-win check would leave this test green.

Recommendation: delete this test file and the duplicate lab certification/index rather than maintain misleading status machinery.

### P2: campaign comparison silently drops incorrect/missing flag and variable expectations

`scripts/verify_emerald_champions_campaign_run.py:139–169` cross-checks expected item assertion coverage and values, but does not do the equivalent for flags or variables. It drops the original expected contract from normalized output.

Executed using `tests/test_campaign_evidence_integrity.py:35`'s valid producer fixture: set `row.expected` to `flags: {FLAG_EXAMPLE: 0}, vars: {VAR_MISSING: 1}` while preserving the existing successful FLAG_EXAMPLE=1 assertion and no variable assertions. `normalize_run` accepted it and returned an object exactly equal to the original normalization. The 48 campaign tests still pass.

This does not show that the normal runtime producer skips assertions; `assert_campaign_state` performs native queries. It shows the archived-report comparator cannot independently substantiate its persisted expected contract. Its state/screenshot hashes also validate declaration format, not absent archived file contents.

Recommendation: delete the historical full-run baseline comparison gate and its `BaselineEvidenceTests`. Keep direct producer checkpoint/save integrity checks, not exact historical map/flag/screenshot snapshots.

### P2: two current failures are stale expectations, not game defects

1. `tests/campaign/test_battle_checkpoint_lab.py:78` requires Pidgey in Calvin's early arsenal. Actual current materialization lacks only `SPECIES_PIDGEY` among that assertion's four species. This reflects the authorized habitat revision, not a reason to restore an old wild roster. Executed suite: 2 passed, 1 failed.
2. `scripts/verify_emerald_champions_campaign_interactive_scope.py:50–56` checks source substrings as evidence. Its current `tests/campaign/interactive_flow_scope.json:45` demands the retired `TryUnlockEligibleVisibleLegendarySignsForCurrentMap` symbol; the verifier fails on that exact token after the authorized legendary redesign. Its declared coverage is not an executed interaction test even when the tokens exist.

Recommendation: delete both stale locks, including the scope JSON if no retained traversal UI consumes it; do not put the old symbol or species back.

### P2: campaign appendix --check omits the live rules it describes

`scripts/generate_emerald_champions_campaign_evidence.py:82–94` claims to hash every relevant canonical input, but omits `src/caps.c` and `src/difficulty.c`. `:297–307` hardcodes their rules, including the currently inaccurate “Hard uses authored levels; Medium subtracts 2.” Native `src/difficulty.c:60–79` first applies the live-cap floor, then the offset; the mode is Normal. Team prose comes from the master text, not a compiled-party equality check.

Thus cap/difficulty changes cannot invalidate this hash, and regeneration does not repair the hardcoded prose. Current `--check` fails because the appendix is stale; it was not regenerated during the paused trainer work. That present failure does not establish any gameplay defect.

Recommendation: delete its freshness gate/generated evidence appendix as proof. An optional human-readable export may remain only if wanted; it is not an authority for game rules.

### P2: agent screenshot acceptance validates only a 24-byte header

`tools/agent_player/agent_player.py:400–405` accepts a PNG signature, IHDR label, and 240×160 dimensions without decoding or checking any image data/CRC. A 24-byte header alone satisfies the source predicate; accepted output replaces the prior observation/state at `:415–416`. The campaign path already uses a real screenshot validator; this separate path does not.

Source-path finding, not an executed native malformed screenshot. Existing restore tests mock `_run_step`, so their passing result says nothing about PNG integrity. Retain the shared observation boundary if this harness stays; do not keep a second weaker implementation merely to preserve its tests.

### P3: dialogue/reachability helpers are heuristics, not gates

`scripts/audit/textwidth.py:42–56` measures each assembly `.string` independently, not concatenated runtime lines. Two adjacent strings without a line-break control can each fit while their concatenation overflows. It assigns zero width to string variables and strips cursor/font controls. `reach.py` flood-fills static collisions against a pinned historical tree; it does not model current script gates, elevation transitions, movement state, or dynamic maps.

Recommendation: delete these historical gate/report assumptions. Manual targeted native dialogue review remains useful. Do not establish release correctness from either helper's output.

## Executed checks

| Command / probe | Actual result |
| --- | --- |
| `python3 -m unittest discover -s tests -p 'test_campaign*.py' -v` | 48 passed |
| `python3 -m unittest discover -s tests -p 'test_map*.py' -v` | 42 passed |
| `python3 -m unittest discover -s tests -p 'test_agent_evidence_integrity.py' -v` | 27 passed |
| `python3 -m unittest discover -s tools/agent_player -p 'test_agent_player.py' -v` | 3 passed |
| `python3 -m unittest discover -s tests/campaign -p 'test_battle_checkpoint_lab.py' -v` | 2 passed; stale Pidgey assertion failed |
| Campaign battle-policy verifier | Passed 29 host-compiled classification cases; not native combat |
| Campaign capture-path verifier | Passed token audit: 23 legendary, 7 WON-safe paths, 17 cleanup consumers |
| Campaign prerequisite verifier | Passed inventory: 930 rows; 37 declared native coverage, 893 route setup |
| Campaign interactive-scope verifier | Failed retired legendary function substring |
| Campaign evidence generator `--check` | Failed stale appendix; no regeneration |
| In-memory negative probes | Wrong flags/vars accepted; out-of-VRAM expression resolved; ring grants 99 stones; imaginary checkpoint indexed validated; incomplete nested schema validated; auto-win test rejects unrelated top-level schema; named wrong-address write accepted |

All mutations were in-memory mocks. No temporary experiment altered game source. No emulator, fresh native traversal, ROM build, or complete map render was run for this static audit.

## Reviewed-file inventory and deletion decisions

Every script/test listed below was read in full. “Retain focused subset” is an exact deletion recommendation for the named portions, not approval of the entire inherited suite. Root performs coordinated deletion and dependency cleanup.

### Campaign scripts/tests

| File | Decision and distinct value |
| --- | --- |
| `scripts/verify_emerald_champions_campaign_battle_policy.py` | Retain optional focused host check of the actual production automation classifier; it prevents capture/native flow being auto-won, not a team/design gate. |
| `scripts/verify_emerald_champions_campaign_capture_paths.py` | Delete token-based audit gate; occurrence/substring inventories do not execute dispatch or outcome flow. |
| `scripts/verify_emerald_champions_campaign_prerequisites.py` | Delete heuristic inventory gate; classifications and route-setup declarations are not native prerequisite coverage. |
| `scripts/verify_emerald_champions_campaign_interactive_scope.py` | Delete source-token/declared-coverage lock. |
| `scripts/verify_emerald_champions_campaign_run.py` | Delete historical baseline comparator; retain runtime producer integrity, not snapshots. |
| `scripts/generate_emerald_champions_campaign_evidence.py` | Delete --check gate and generated proof role; optional export only if explicitly useful. |
| `tests/test_campaign_evidence_integrity.py` | Retain `CheckpointEvidenceTests` and `CampaignPairBoundaryTests` for corrupt/foreign state and lineage safety. Retain actual classifier return-value mutation and normal cases in `ProductionPolicyTests`; delete extraction-format/missing-compiler scaffolding tests. Delete `BaselineEvidenceTests` with comparator. Two-argument API compatibility test is dispensable, not current-save safety. |
| `tests/test_campaign_item_evidence.py` | Delete comparator coverage/legacy-report tests with comparator. Keep actual item-ID parsing/manifest validation only while current producer uses them. Delete raw C query substring/order assertions; they are not native bag tests. |
| `tests/test_campaign_observation_integrity.py` | Retain focused corrupt-PNG rejection and the demonstrated facing/accidental-step regression; these protect the actual Python observation/action consumer. They are mocked runner tests, not native avatar proof. |
| `tests/test_campaign_save_import_integrity.py` | Retain: actual battery import metadata boundary, source hash, parent identity, earned flag/item contracts. No old-save compatibility lock needed. |
| `tests/test_campaign_step_integrity.py` | Retain turn-only versus turn-and-step and unexpected displacement/map rejection; distinct demonstrated unintended movement failure. Can merge with observation file without changing assertions. |
| `tests/test_campaign_trainer_flags_integrity.py` | Retain only namespace bounds/collision behavior used by campaign queries; remove exact Takao numeric-ID assertion as a design lock. |
| `tests/campaign/test_battle_checkpoint_lab.py` | Delete all three: stale roster lock, wrong-reason rejection, duplicate index-count scaffolding. |

### Map/audit tools and tests

| File | Decision and boundary |
| --- | --- |
| `scripts/audit/map_integrity.py` | Remove broad mandatory gate. A small explicit invalid compiled reference/truncated map check may be retained for demonstrated corruption; current full tool is a source inventory, not visuals/reachability proof, and consumes unsound dynamic allowances. |
| `scripts/audit/map_dynamic_inventory.py` | Delete speculative source-parser gate/allowance machinery; findings must not certify execution. |
| `scripts/audit/map_sweep_render.py` | Retain optional native inspection utility only. Its stamp and before/after artifact binding are useful; initial map/position plus a screenshot are not traversal or visual approval. |
| `scripts/audit/map_sweep_names.json` | Inspected shared fixture census; retain only as necessary fixture data for the optional renderer, not a historical exact-count gate. |
| `scripts/audit/leaf_scene_runtime.py` | Retain optional targeted native diagnostic only if reused; not a static test. ROM/ELF pair checked, but no current-source stamp: outputs concern supplied artifacts only. |
| `scripts/audit/rustboro_guide_runtime.py` | Same optional diagnostic boundary; do not use pre-redesign scene assumptions as a release lock. |
| `scripts/audit/story_handoff_runtime.py` | Same optional diagnostic boundary; no current-source freshness claim. |
| `scripts/audit/reach.py` | Delete obsolete pinned-baseline reachability heuristic. |
| `scripts/audit/show_label.py` | Delete historical side-by-side baseline convenience helper with retired baseline machinery; no correctness contract. |
| `scripts/audit/textwidth.py` | Delete heuristic global checker; use targeted native/accurate text review when editing dialogue. |
| `scripts/audit/README.md` | Remove obsolete mandatory-gate and historical-baseline instructions when coordinated deletion occurs. |
| `tests/test_map_dynamic_inventory.py` | Delete parser-shape/coverage scaffolding with speculative inventory. Passing 19 tests does not establish correct destination evaluation. |
| `tests/test_map_sweep_integrity.py` | Retain only ROM/ELF/source-stamp rejection, mid-run artifact/source mutation, interrupted/failing/unverified render rejection if renderer retained. Delete exact census/order/position fixtures as broad design locks. |
| `tests/test_map_tile_integrity.py` | Retain only actual converter nonblank-tail regression and truncated/invalid compiled binary reference checks if retaining that narrow checker. Delete dormant-layout census, exact placements, and dynamic-allowance tests with broad inventory. |

### Agent evidence and supporting consumers

| File | Decision and boundary |
| --- | --- |
| `tools/agent_player/test_agent_player.py` | Delete all three generic smoke tests: atomic JSON round-trip, fixed button-set equality, synthetic metric counting. No distinct serious regression value. |
| `tests/test_agent_evidence_integrity.py` | Retain foreign checkpoint/config rejection, no budget refund on restore, ROM/ELF admission before state mutation, and reported-versus-verified outcome separation. Delete legacy-protocol compatibility and benchmark cohort/seed/metric accounting scaffolding unless this benchmark remains an active user workflow. |
| `tools/agent_player/agent_player.py` | Consumer read in full; preserve actual state/config/artifact boundaries if the optional harness remains. This audit does not authorize deleting the user's runtime tool. See automation-disable and PNG findings. |
| `tools/agent_player/aggregate_results.py` | Consumer read in full; correctly labels reported wins rather than independently verified outcomes. Optional reporting, not gameplay proof or release gate. |
| `tools/agent_player/result.schema.json` | Read in full; output schema does not independently verify declared outcomes. Retain only alongside retained reporting tool. |
| `tools/agent_player/arsenal.schema.example.json` | Read in full; an example, not an enforced JSON Schema or proof of physical availability. Remove if unused rather than preserving a duplicate legality contract. |
| `scripts/battle_checkpoint_lab.py` | Consumer read in full; recommend retiring duplicate checkpoint construction/index and approximate legal-arsenal certification. Do not remove actual campaign saves/checkpoints as part of deleting this code. |
| `tests/campaign/battle_checkpoint.schema.json` | Read in full; not applied by validator. Delete alongside retired duplicate lab; do not introduce another schema framework. |

Supporting source review: selected actual producer paths in `scripts/run_emerald_champions_campaign.py` (artifact snapshots, parent validation/ancestry, expected-state checks, stepping/query actions, battery import), fixture recipes/manifests and interactive scope references, native difficulty/cap and item/archive definitions, species/item bit layouts, font widths/charmap, and current fixture inventory. This is **not** a claim to have audited every line of the 1,890-line campaign runner or every runtime C implementation. Native party held-item field is indeed 10 bits (`include/pokemon.h:138`); no false defect is reported against the lab's matching mask.

Excluded/other owners: `cap_order.py`, `ec_baseline_diff.py`, `gate_order.py`, `mega_stone_timing.py`, `align_battle_scripts`, other story/legendary/reward tests, release/stamp core consumers, and vendored mGBA tests. No findings here certify the campaign as fully played, any individual battle as solved, or this working tree as release-ready.
