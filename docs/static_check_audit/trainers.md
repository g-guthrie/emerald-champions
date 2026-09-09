# Trainer, preparation and Circuit static-check audit

## September 9 native-fixture correction

During executable E0010 review, synthetic `trainer_control.party` IDs1–15 were
found to override campaign records included by the native test runner. The
first Woods grunt diagnostic therefore loaded an unrelated level100 fixture;
that run was discarded. This was a native harness defect, not evidence that
the game's grunt had that team. The completed restart encounters all use IDs
above15 and had matching native input logs.

Only synthetic RED/LEAF have remaining consumers. Removed13 unused identities
(15 difficulty-specific blocks), moved RED/LEAF into spare TESTING-only slots,
removed synthetic NONE and blanket duplicate-initializer warning suppression,
and updated text/slide consumers. Emerald855/856 and FRLG624/625 fit below each
unchanged MAX_TRAINERS_COUNT, enforced at compile time. Production counts,
trainer flags, system flags and reserved partner IDs remain unchanged. The
generated header was rebuilt through Makefile. Corrected native Woods roster
guards and all16 existing shared paired-AI regressions passed afterward.
Deleted fixture definitions are recoverable through Git; no game trainers
were deleted. This finding supplements, rather than retroactively expands,
the original static-check audit below.

September 8, 2026. Initially a read-only audit after the user paused battle
authoring; the subsequently approved check deletions are recorded below. No
trainer or runtime source was changed during this audit. No E0046–75
authoring was started. Earlier E0010–21 edits remain pending the parent task's
integration; their individual review document had not been written when paused.

## Outcome

The current master audit's 15 failures are not evidence of 15 broken battles:
all nine rejected move assignments are present in the actual native preparation
arrays, and the other six enforce an Item Clause that ordinary campaign parties
do not use. Other checks accept impossible or malformed data, and two metadata
readers misread actual C values. A passing exact-projection check proves that
generated output matches its input, not that the input is legal or good.

Sixteen focused tests passed in 2.882 seconds. One test that invokes a generator
CLI was deliberately not run. These were host/synthetic and projection checks,
not native campaign battles. In-memory negative probes below did not write any
source or generated output. Passing tests did not prevent the demonstrated
validator false passes.

## Confirmed findings

### 1. Master move legality uses the wrong access contract — high impact

`scripts/audit_emerald_champions_master_battles.py:397` combines the pinned
Champions learnset and one reviewed-extension category. Lines 765–772 reject
everything outside that set. The actual Center service intentionally uses the
all-generation corpus, plus both formats' presets:
`src/move_relearner.c:1024`, `src/move_relearner.c:1049`, and
`tools/learnset_helpers/make_teachables.py:139`. That corpus is not merely a
stale copy of the pinned Champions pool.

Current master failures confirmed in the direct generated native arrays:

| Trainer assignment | Rejected move | Native direct array |
| --- | --- | --- |
| E0001 Mudkip, Brendan and May | Superpower | Present, both branches |
| E0004 Bunnelby | Strength | Present |
| E0023 Bunnelby | Return | Present |
| E0026 Skrelp | Scald | Present |
| E0027 Shieldon | Wide Guard | Present |
| E0031 Zubat | Tailwind | Present |
| E0033 Finneon | Scald | Present |
| E0045 Luvdisc | Heal Pulse | Present |

Additional in-memory repro: replace Marlene's Slowpoke Hydro Pump with Scald in
the master string and call `audit()` through an object whose `read_text()`
returns that string. The added rejection is exactly `moves outside pinned
Champions/mainline learnset ['MOVE_SCALD']`. Yet
`sEmeraldChampionsPreparationLearnsets[SPECIES_SLOWPOKE]` directly maps to
`sEmeraldChampionsPreparationMoves_SLOWPOKE`, which contains `MOVE_SCALD`.
No form fallback or preset union is needed to prove this mismatch.

Correction to my earlier authoring report: Kantonian Grimer's Knock Off is legal
and present in its direct native array. I incorrectly passed bare `GRIMER` into
`pinned_legal_moves()`: official IDs normalize, but reviewed extensions require
the `SPECIES_GRIMER` key. The extension record explicitly retains Knock Off from
Inclement's tutor source. This was my caller error, not stale generated Grimer
data. Drain Punch remains a legal earlier authoring change, but must not be
described as repairing illegal Knock Off. Wooloo's direct array lacks Body Press.

Recommendation: remove this imported-pool veto. Where legality is needed, use
the current native preparation/access owner, with separate handling of moves
that native battle mechanics explicitly forbid for a species. Do not delete
valid trainer moves to silence this audit.

### 2. The master invents a campaign Item Clause — high impact

`scripts/audit_emerald_champions_master_battles.py:869–871` unconditionally rejects
duplicate held items. `include/config/battle.h:457` sets the pool Item Clause to
FALSE. Furthermore, `src/trainer_pools.c:370` applies pool pruning only when a
trainer actually uses a pool; ordinary fixed campaign parties retain their
authored members/items. The six present rejections are Devan, Dawson, Sarah,
Josh, Lilith and Edmond. Item diversity can be a design preference, not a claim
that the current native game makes these parties illegal.

The same master checker contains other design locks rather than universal
mechanical invariants: mandatory Toxic/Flame Orb for particular abilities,
categorical Choice setup/protection rejection, early evolution thresholds,
offset limits, gym-type percentages and exact generated prose. Their presence
must not be mistaken for current user authorization. Remove those vetoes, not
the corresponding creative team options.

### 3. Preset metadata misreads signed priorities and C expressions — high impact

`scripts/generate_emerald_champions_battle_sets.py:972–976` extracts unsigned digit
sequences and chooses their maximum. Repro from `move_metadata()`:

| Move | Metadata priority | Native move declaration |
| --- | ---: | ---: |
| Dragon Tail | +6 | -6 |
| Circle Throw | +6 | -6 |
| Focus Punch | +3 | -3 |

This feeds `ranked_attacks()` at lines 1141–1142, which rewards positive priority,
and singles ranking at line 1572. It also feeds the quality report. Exact output
regeneration can pass while consistently reproducing this error.

`species_build_metadata()` at lines 1007–1009 reads only the first number of a
preprocessed stat expression. It reports Porygon's Sp. Atk as **8**. The actual
configured initializer is `8 + 1 >= 1 ? 85 : 75`, whose C value is **85**.
Preprocessing is not constant-expression evaluation. This distorts offensive
identity, synthesized sets and recommendations. The configured-ability reader
does not use this numeric stat parser; do not conflate the two.

Recommendation: repair these shared readers before trusting synthesis or
quality suggestions. This audit did not modify or regenerate their consumers.

### 4. Any required item bypasses preset species-ability validation

`scripts/generate_emerald_champions_battle_sets.py:2247–2251` assumes any non-NONE
required item or required move is a valid transformation. It does not establish
a native form-change relation before skipping the ordinary ability check.

In-memory repro: copy the Bulbasaur default, set `required_item='ITEM_POTION'`,
`ability='ABILITY_WONDER_GUARD'`, and `moves=['MOVE_HYPERSPACE_FURY']`.
`validate([entry])` returns successfully. All tokens exist, but Potion is not a
Bulbasaur transformation and Hyperspace Fury explicitly fails outside Hoopa
Unbound in `src/battle_move_resolution.c:1208`. The validator does not check
move/species executability; admitting a token to a preset also grants it through
the native preparation preset union, which makes a circular "it is in a preset,
therefore it is valid" argument particularly dangerous.

Recommendation: retain structural output integrity, but validate the actual
form/item/move relation before the transformation exception. Do not market the
current validator as full native set legality.

### 5. Circuit local validation accepts unsupported schema and malformed moves

`scripts/generate_showdown_champions_circuit.py:259–298` validates additional
source hashes only when schema_version equals 2; it never rejects other schema
versions. It checks move count/duplicates/NONE, not move-token existence or
native executability. In-memory repro: set schema_version to 999, remove
source_files, replace the first template move with `MOVE_DOES_NOT_EXIST`.
`validate_manifest()` still succeeds. The invalid identifier would fail a later
C compile, but the supposed structural validator has already accepted it.

`build()` records `supplement_source_sha256` at line 198, but local validation
does not compare it to the current authored supplement. Likewise, declared
upstream hashes are metadata: local projection does not re-derive templates
from upstream bytes. `project(check=True)` accurately detects changed C output
against the local manifest; it does **not** prove that manifest still represents
the pinned upstream or current supplement. Its help text describes local
projection correctly. The tests covering selected hard-coded imported records
do not close that boundary.

Recommendation: keep a small exact local-projection check, reject unknown schema
and malformed records, and explicitly distinguish upstream import verification
from local runtime-artifact integrity. Remove broad imported-data pinning tests
as routine release gates.

### 6. Teams `--check` is scratch materialization/ability validation, not parity

`scripts/emerald_champions_teams.py:451–466` compares the plan table, creates a
scratch master/party and runs configured ability legality against scratch. It
does not compare the actual master or trainers.party to the canonical teams,
nor validate moves, items, party sizes against the campaign's declared format,
or run the master audit. A move-only or item-only source edit can leave the
actual game party stale while this check passes, provided strategies did not
change. Its final success message is narrow and accurate; the `--check` help
text "every static gate" is stale. Do not interpret it as release parity.

`scripts/implement_emerald_champions_master_battles.py:210–212` separately offers
an actual exact master-to-party comparison. Retain that narrow invariant.

The teams compiler also ignores a parsed branch's encounter identity when
matching master records (`emerald_champions_teams.py:303–311`). In-memory repro:
read current teams, change only the Woods grunt's `branch.encounter` from 10 to
999, then call `compile_master()`. It accepts the branch and writes its team
into E0010 by trainer ID. This can silently mislabel ownership, ratings and cap
intent even though the trainer table remains structurally compilable.

### 7. Coverage inventory misses the actual two-opponent story macro

`scripts/audit_emerald_champions_campaign_coverage.py:42` recognizes only a
trainerbattle-prefixed command whose first argument looks like TRAINER_*.
`data/maps/MossdeepCity_SpaceCenter_2F/scripts.inc:253` uses `multi_2_vs_2` with
Maxie and Courtney; neither invocation enters that trainer inventory. A generic
`trainerbattle TRAINER_BATTLE_DOUBLE, TRAINER_CALVIN_1, ...` instead records the
mode token as if it were a trainer. Both were reproduced directly against
`TRAINER_RE.findall()`.

The inventory also globs shared `.inc` files instead of resolving the actual
compiled include graph. It is a source inventory approximation, not proof that
every listed line is compiled/reachable. The final `complete` means complete
**declared** coverage of its computed inventory, not observed playthrough.
That distinction is correctly stated in its final message and should remain.
Missing invocation forms, however, shrink the denominator and undermine even
the declared trainer-coverage claim.

### 8. Quality and legacy audit reports are not correctness gates

`scripts/audit_battle_set_quality.py:338–347` calls several valid tools
competitively useless and says Choice items cannot select Protect. Its hard-dead
list includes String Shot, Conversion, Recycle, Power Trick and other moves
with native strategic effects. Its `pool = learnable_pool(...) | set(mv)` at
line 226 cannot establish that existing moves are independently legal, and it
inherits finding 3's incorrect metadata. Its opening docstring already admits
heuristics are optional and must not veto authoring. Delete this report rather
than preserving persuasive HIGH labels for unfounded recommendations.

`scripts/audit/cap_order.py:32–63` hard-codes caps, earliest badges and acceptable
class offsets despite claiming compiled-data authority. It reads authored base
levels, not the live-cap floor in `src/difficulty.c:53`. Its geometric reachability
is an approximation and its report always exits successfully. It is not a
reliable progression validator under current design changes.

`scripts/audit/gate_order.py:12–28` derives reachability from authored metadata;
lines 35–43 treat any shared/C setter as globally early, and lines 73–98 ignore
large flag/variable classes including EC state. A setter's mere existence, even
in unreachable code, is not an available route. It also conflates set/unset
tests. This cannot certify story gates and should not remain a gate.

`scripts/audit/ec_baseline_diff.py:99–127` explicitly normalizes slide movement
to walking as a "placeholder", drops compare instructions that are not merged
with its recognized modern conditional macros, and treats lock/lockall as
equivalent. In-memory probes show slide_up and walk_faster_up become identical;
`compare VAR_X, 1; goto_if 1, Label` loses its comparison. The native commands
have different state/movement semantics. Its "only real behaviour differences
remain" claim is false. It also ignores maps not present in both trees. Delete
this obsolete baseline comparator; do not interpret an empty diff as parity.

## Per-file disposition

"Retain" below is deliberately narrow. It is not permission to grow a new
framework or restore a broad inherited suite. Production generators may remain
necessary even where their present checker needs repair.

| Reviewed file/check | Recommendation | Necessary invariant or reason to remove |
| --- | --- | --- |
| `scripts/emerald_champions_teams.py` | Retain authoring owner; narrow/repair check | Canonical-to-master/plan identity is necessary. Current scratch-only check does not prove deployed parity; encounter IDs need validation. |
| `scripts/implement_emerald_champions_master_battles.py` | Retain `--verify-only` | Exact master-to-trainer-source materialization; it should not make strategic judgments. |
| `scripts/verify_trainer_ability_legality.py` | Retain | Resolves configured ability arrays and species aliases; protects against silent native ability fallback. It checks trainers.party, not stale trainers.h/ROM artifacts. No current false result demonstrated in its correctly invoked path. |
| `scripts/audit_emerald_champions_master_battles.py` | Remove monolithic gate | Wrong move corpus, nonexistent Item Clause and numerous stale design/prose vetoes. Check imports before deleting the file; other tools reuse helper constants/functions. |
| `scripts/audit_battle_set_quality.py` | Delete | Optional, often false strategy judgments; not an essential invariant. |
| `scripts/audit_emerald_champions_campaign_coverage.py` | Remove closure-gate status; retain only if inventory report is useful | No runtime evidence, missing native trainer macro; must not block or certify battle work. |
| `scripts/generate_emerald_champions_battle_sets.py --check` | Retain exact output comparison; repair validator/readers | Actual preset C must match its declared inputs. Historical-source reconstruction is not strategic approval or move legality. |
| `scripts/generate_showdown_champions_circuit.py --check` | Retain minimal local projection after structural repair | Native tables/counts must match local manifest; not pinned-upstream or current-supplement proof. |
| `scripts/audit/cap_order.py` | Delete | Frozen progression/class assumptions and approximate geometry. |
| `scripts/audit/gate_order.py` | Delete | Text occurrence and authored cap guesses cannot prove gate reachability. |
| `scripts/audit/ec_baseline_diff.py` | Delete | Obsolete baseline and demonstrably behavior-erasing normalization. |
| `tests/test_battle_comparison_integrity.py` | Retain | Executes production bytecode comparisons/dispatch, unsigned boundaries, invalid-op handling and Contrary boundaries; protects foundational control flow rather than design. |
| `tests/test_battle_set_ability_integrity.py` | Retain | Production application and recognition must agree on actual species ability selection. Synthetic storage and early return are explicit; not full preset-application proof. |
| `tests/test_battle_set_visibility_integrity.py` | Retain | Count/name/preset indexing and owned/transformation filtering must agree, including out-of-range choices. Does not test actual inventory APIs or generated data. |
| `tests/test_preparation_move_integrity.py` | Retain the collector invariant | Production collector unions real generated tables, removes known moves and deduplicates without overflowing. Historical expectations reuse the production mapping function, so it cannot independently prove historical/form mapping correctness or input freshness. Fixed Starmie/Smeargle examples are not a full legality oracle. |
| `tests/test_showdown_projection_integrity.py` | Delete as broad imported-data regression; preserve only essential projection logic in the generator check | Pins selected upstream records, policy ownership and license prose; it passes despite finding 5. Exact C/count drift checks are useful but duplicated by local projection. |
| `tests/test_review_count_integrity.py` | Delete | Small derived count assertion already belongs to the generator, not a separate suite. It does not validate action vocabulary: anything except `replace` is retained. |
| `tests/test_circuit_selection_integrity.py` | Delete | Mostly synthetic move/RNG draw/culling call-order expectations; locks an implementation rather than essential game behavior. Explicitly does not execute native culling. |
| `tests/test_circuit_team_coherence_integrity.py` | Delete | Synthetic heuristic expectations and chosen lead policy, not proof of real-team strategy. Hard-coded choice preferences should not freeze redesign. |
| `tests/test_rustboro_trainer_corridor.py` | Delete | Reimplemented BFS/static sight with fixed coordinates, two facing directions and a hard-coded Tommy displacement. Does not execute trainer approach/flags/native collision and locks current layout. |

## Review and execution limits

All eleven main audit/check files and all nine test files in the table were
reviewed in their relevant entirety except the 2,500-line battle-set generator:
for that generator, this audit traced its main/check/validation pipeline,
canonical source loading, move/species metadata readers, legality pool,
ranking consumers and review-count rendering; it did not individually audit
every embedded authored override or every synthesis branch. Circuit build,
validation, projection and source provenance paths were traced. No upstream
checkout was fetched or re-imported, and neither generator CLI was executed.

Supporting native consumers read: trainer generation and pool selection,
difficulty/Stat Point calculation, preparation collector/table/form mapping,
move data, Hyperspace Fury failure, and Circuit family/selection consumers.
`make_teachables.py` was read for the preparation data origin, not claimed as a
complete audit of its ordinary TM/tutor branch. Map/reach/story audit helpers and
`test_campaign_trainer_flags_integrity.py` were explicitly handed to the other
audit owner. No claim of reviewing those files is made here.

The 16 passing tests exclude only
`ShowdownProjectionIntegrity.test_local_cli_check_without_upstream_does_not_write`
to honor the no-generator instruction. They wrote disposable host harnesses and
projection fixtures to temporary directories, not project data. No native ROM
build or battle simulation occurred. Findings and line numbers above describe
the pre-deletion sources inspected during the audit.

## Approved cleanup completed

Following the user's deletion authorization and the parent task's explicit
target approval, removed:

- `tests/test_showdown_projection_integrity.py`
- `tests/test_review_count_integrity.py`
- `tests/test_circuit_selection_integrity.py`
- `tests/test_circuit_team_coherence_integrity.py`
- `tests/test_rustboro_trainer_corridor.py`
- `scripts/audit_battle_set_quality.py`
- `scripts/audit/cap_order.py`
- `scripts/audit/gate_order.py`

All eight were tracked files and can be recovered from Git history. The
monolithic `audit()` function and CLI were also removed from
`scripts/audit_emerald_champions_master_battles.py`; existing imported helper
interfaces remain. The authoring compiler's `--check` documentation/help now
describes only the strategy-table comparison and scratch materialization/ability
checks it actually performs. No generator behavior or team data was changed to
satisfy a check.

The four retained test modules passed again after cleanup (4 tests, 1.976
seconds), and helper imports passed through `emerald_champions_teams`, `ec_moves`
and `export_chatgpt_battle_prompt`. At cleanup, the metadata/validator defects in
findings 3–7 remained unresolved; deleting bad gates did not repair those data
readers. The baseline-comparison trio is owned by the parent/map audit task and
was not deleted by this agent. Coverage-inventory semantics were left unchanged.

### Subsequent bounded repair: finding 3

The resumed authoring task repaired the numeric metadata reader without running
generation or changing authored sets. Full numeric initializer expressions are
now evaluated by a host C compiler using the game's configured constants, rather
than unsigned digit extraction or choosing the largest number in a ternary.
An independent typed C projection matched power, accuracy and signed priority
for all 935 current move entries, including all 14 negative-priority moves.
Configured stat spot checks include Porygon Sp. Atk 85, Charizard Sp. Atk 109,
Ninetales Sp. Atk 81 and Alakazam Sp. Def 95. This repairs finding 3's demonstrated
numeric bugs; it is not a claim of complete metadata or generator validation.
Findings 4–7 were not repaired by this task.
