# Static-check audit: release and world checks

September 8, 2026. Read-only implementation review and in-memory negative probes;
no checker or gameplay fixes in this audit. Actual sources, not prior prose, are
the authority. A reproduced hole is a defect in the named check, not proof that
the production game currently contains the injected defect.

## Confirmed findings

### R1 — State-ID verification silently omits current IDs (high)

`scripts/verify_emerald_champions_release.py:52–133` collects only FLAG_/VAR_
definitions, then silently skips expressions it cannot resolve at lines 125–128.
It does not collect bases such as SYSTEM_FLAGS, TEMP_FLAGS_START or
TEMP_VARS_START, nor the referenced rematch enum. A trace of the current check
observed **284 skipped definitions** while it printed PASS. These include badge,
system, temporary and registered-trainer flags. This is not a count of collisions.

An in-memory appended valid C definition
`#define FLAG_EC_AUDIT_COLLISION (0x554 / 2)` duplicates the current
`FLAG_EC_HATCHED_DAYCARE_EGG` value 0x2AA and still passes, because division is an
unsupported AST operator. No file was modified. Correct direction: use configured
C values, explicitly classify intentional aliases/range sentinels, and report
unresolved definitions instead of silently certifying them. Merely adding bases
to the existing blanket uniqueness test would falsely reject intentional local
temporary aliases; fixing the contract needs that distinction.

### R2 — Wild integrity accepts non-Pokémon and invalid encounter rates (medium)

`scripts/verify_wild_distribution.py:39–59` validates level comparisons but never
validates land/Rock Smash species against configured encounter-eligible species.
Replacing Route101's first land slot with `SPECIES_NONE` in memory still prints
PASS for all 264 tables. A second probe sets its `encounter_rate` to -1 and also
passes: line 49 treats a negative rate like a disabled table. The native generator
emits that number directly into WildPokemonInfo, whose encounterRate is u8.
This can become 255, not a disabled encounter. Fractional level values also lack
an integer-type check. Water authoring separately rejects NONE/EGG, but does not
close the land or rate holes.

Correct direction: validate species against current configured data and validate
the actual numeric field types/ranges, including explicit zero semantics. Keep
the user-approved 5% species minimum rather than freezing exact species or slot
weights. The current forward/reversed aggregation matches native Sweet Scent/
Lure slot reversal, including the distinct rod groups. Probabilities describe
base slot selection, not every ability attraction, Repel or special encounter.

### R3 — Reward graph counts unreachable instructions as reachable gifts (medium)

`scripts/verify_mega_stone_rewards.py:43–68` scans all tokens/instructions in a
label body without stopping at unconditional end/return/goto, and follows tokens
which merely happen to equal labels. An in-memory `end` immediately before
Route117's Audinite `giveitem` leaves the same claimed reachable Audinite reward.
The current live script does not contain that injected end.

The check roots every registered map's events rather than proving physical or
state-dependent access. `stones()` in `generate_emerald_champions_mega_archive.py`
also scans unpreprocessed form-change text; its catalogue is not automatically
the configured Mega roster. The `root` parameter does not propagate to stones().
Correct direction: describe structural reward candidates honestly, traverse
actual control-flow edges through terminators, and use configured form changes.
Keep native receipt/no-room tests for delivery; a graph alone cannot prove it.

### R4 — Trainer invocation coverage accepts commented-out battles (medium)

`scripts/align_emerald_champions_battle_scripts.py:74–117` searches raw lines
without stripping comments. Baseline `mismatches(formats())` returns no errors.
Commenting out Calvin's actual trainerbattle invocation **in memory** still
returns no errors; the comment satisfies its 'seen' set. It also inventories all
map/script files, not only assembled/reachable ones. The two-trainer parser
special-cases `trainerbattle_double_two_trainers` and `multi_2_vs_2`, but not
`trainerbattle_two_trainers`, so the latter's second trainer is not collected by
that branch. Native macro definitions are in `asm/macros/event.inc:840`.
Correct direction: parse assembled instructions, include both trainer operands,
and keep invocation coverage distinct from reachability or combat behavior.

### R5 — Progression checker rejects harmless comments (low), overstates assembly

`scripts/verify_emerald_champions_progression.py:153–171` does not use its own
`clean_script_line` helper for specialvar checking. Appending only the comment
`@ Do not use: specialvar VAR_RESULT, OpenEmeraldChampionsBattleItemMart` in memory
causes rejection. This is a concrete false failure on non-executable text.

Its label index (`:52–65`) reads all .inc/.s files, including unassembled and
conditional sources, so reference existence is weaker than assembled reference
resolution. `assembled_sources` likewise approximates inclusion from filenames.
The current final reachability disclaimer is appropriate; the assembled-source
claims should also be narrowed or derived from real includes/preprocessing.

### R6 — Daycare source check inherits reachability weakness and locks locations

`scripts/verify_daycare_discoveries.py:9–20` fixes ten exact species/map pairs,
including displaced families as well as babies. These are historical placement
requirements, not a general test of physically accessible early babies. The
Mantyke two-slot slice matches the **current** Old Rod indices but is hard-coded.
The reward checks depend on R3 and raw substring/order matching. They do not
execute item delivery, breeding, hatching or cancellation. Its early species
checks do not reject encounter_rate=0. Keep only explicitly current placements
as placement checks; do not restore old habitats to satisfy stale coordinates.
See the integrity-test audit for the independently reproduced negative-only
test-suite false pass.

### R7 — Optional visual comparator does not bind ROM to ELF/current source

`scripts/verify_emerald_champions_visual_runtime.py` actually runs a renderer; it
is not a static game checker and is not in STATIC_GATES. Its comparator correctly
requires a nonempty scenario set, unique names, pixel hashes and semantic
observer coverage, then compares an explicit baseline. But the renderer at
`scripts/render_emerald_champions_ui.py:1708–1715` resolves write/read addresses
from the supplied ELF without verifying ROM/ELF correspondence or a current
source stamp. The manifest hashes the ROM but not ELF. A matching old pair can
reproduce old pixels and pass without exercising current source. This is a
provenance boundary from code review, not a newly run emulator reproduction.
Use the existing artifact-pair/stamp checks when claiming current-source runtime
evidence. Deliberate visual changes must not be reverted just to match pixels.

## Reviewed implementation inventory and retained value

| File / entry point | Disposition |
| --- | --- |
| verify_emerald_champions_release.py | Nine named gates, fail-fast subprocess handling and explicit artifact checks are useful; state-ID gap R1; aggregate PASS inherits constituent gaps. |
| stamp_release_inputs.py | Content/path/artifact hashing, strict artifact key set and byte recheck are useful. Does not authenticate compilation or stale objects, as its docstring correctly states. Uses a repository-specific .gitignore interpreter and omits ignored generated outputs deliberately. Stamp creation also checks mtimes; copied tree timing can block creation, not falsely prove a fresh build. |
| rom_artifacts.py | Compares actual ELF-derived binary, exact power-of-two size and FF padding; good narrow integrity contract, not gameplay or source-freshness proof. |
| verify_wild_distribution.py | Useful slot lengths, weight sums, reversed species aggregation and authored-water comparison; R2. |
| emerald_champions_rebuild_wild_water.py | In-memory materialization comparison is nonmutating when called by verifier; validates authored water list sizes/constants, not native configured eligibility or rates. No generator was run during this audit. |
| verify_mega_stone_rewards.py | Useful missing-stone and duplicate trade-source inventory, no-free-archive/currency checks; R3. Hard-coded trade location text is not location proof. |
| generate_emerald_champions_mega_archive.py | Read as a verification dependency; unpreprocessed catalogue boundary in R3. No generation run. |
| item_catalog.py | Reads current item-category arrays and validates NONE terminators; useful current-source vendor inventory, not proof a menu is reachable. Raw regex can see inactive/comments. |
| verify_daycare_discoveries.py | Exact source-placement/pattern check, not native behavioral proof; R6. |
| align_emerald_champions_battle_scripts.py | Useful format check after correcting parsing; R4. |
| verify_emerald_champions_progression.py | Current map geometry/reference inventory is useful; R5 and explicit state-reachability boundary. |
| verify_emerald_champions_visual_contracts.py | Actual registered maps/layout dimensions, duplicate IDs, warp indices and connections; reviewed off-map exceptions and dynamic warp skips are explicit. Not physical walkability, object-visibility or runtime scene proof. Coordinates are coerced to int, so fractional source coordinates can be truncated by the checker. |
| verify_emerald_champions_visual_runtime.py | Optional runtime/pixel comparator, R7; not an aesthetic or whole-UI judgment. |
| .github/workflows/build.yml | Build/stamp/upload only; no static or host regression gates run in CI. This is documented and deliberate, not proof that green CI certifies those checks. Push filter omits codex/**; pull_request trigger remains. |

The release memory/header/fixture-symbol checks were read against the actual
artifact pipeline, not rerun on a stale release. Runtime gate and native-tool
consumer tests are covered by the integrity reviewer. Map-integrity and map
dynamic audit helpers are covered by the campaign reviewer. Trainer generator
checks are covered by the trainer reviewer. Vendored upstream tests and native
battle execution are outside this static-check audit.

## Commands/probes actually run

All probes used unittest.mock against Path.read_text in process memory. No bad
game data was written. Current unmodified progression, wild, reward, daycare,
branding, state-ID and trainer-format checks passed their existing assertions.
The specific false passes/rejection above were then reproduced. The progression
baseline reported 540 Hoenn maps, 4,195 physical event entries, 1,402 warps,
18,359 script references and 368 specialvar calls. Reward baseline reported 99
stones. These are checker inventories, not campaign completion or physical
access proofs. No fresh ROM build or battle authoring was performed after pause.

## Implemented reduction after the audit

The subsequent user instruction authorized deleting nonessential tests. Root
removed the standalone daycare source lock and the old native save-layout tests;
coordinated host-fixture deletions are listed in the other reports. The release
command no longer invokes script-reference, trainer-format or reward-graph
scanners, and the misleading partial state-ID and branding scanners were deleted.
Actual ROM header, checksum, fixture-symbol and byte-identity checks remain.

Compiled map/tile integrity now runs with `include_dynamic=False`; speculative
scene/animation parsing cannot supply release certification. The current run
found zero compiled errors, 47 explicitly dormant errors and one unresolved
static ownership case across 540 compiled Hoenn maps. This is not a visual or
walkability approval. The helper API remains for optional inspection.

The retained wild structural check received a small fix for R2 using the existing
configured-species reader plus native integer/u8 bounds, with no new test file.
The current 264 tables pass. In-memory NONE-species, negative-rate and fractional-
level probes now all reject for the intended reason. Current trainer materialization
and 2,339 configured ability checks also passed. These results are not individual
battle completion or strategic quality evidence.

`scripts/audit/mega_stone_timing.py` was additionally read in full: retained only
as an optional candidate-source report. Its use of raw family/text matches,
authored map caps and default selector rows is expressly labeled unknown access/
evolution timing; it is neither a legality nor progression gate. No report run or
gameplay alteration was required for this disposition.
