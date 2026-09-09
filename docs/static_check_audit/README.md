# Static-check audit and test reduction

September 8, 2026. The user paused further trainer authoring, requested a review
of all project static checks against actual game code, then explicitly requested
deleting tests unless absolutely necessary. This audit covers project-owned
static/check pipelines and their host regression modules, not vendored mGBA tests
or a complete audit/run of the native battle-test corpus.

## Outcome

**32 test files deleted**, including 29 top-level host modules, the three-test
checkpoint-lab module, generic agent-player smoke tests, and the obsolete native
save-layout compatibility file. The top-level host suite shrank from 55 modules
to 26; additional nonessential test classes were removed from retained files.
All deleted files were tracked and can be recovered from Git. No saves, ROMs,
campaign checkpoints, trainer teams or wild habitats were deleted.

Removed stale heuristic scripts, the monolithic master audit/CLI, the historical
campaign baseline comparator and obsolete interactive-scope data. Shared helper
functions still consumed by authoring/runtime tools were preserved. Source-token
script/reward scanners no longer block release. The partial state-ID scanner
and branding scanner were deleted; real artifact/header checks remain.

The most consequential audit findings were:

- Wrong legality authority: the old master gate rejected nine move assignments
  that the native preparation tables allow, plus six duplicated items despite
  no campaign Item Clause. These were checker failures, not broken trainer teams.
- False-green tests: the save fixture accepted removed checksum comparisons;
  scene fixtures accepted wrong rewards and bypassed preflight; negative-only
  tests passed an always-failing verifier.
- Stale fixtures: restart/legendary/Steven-handoff environments no longer matched
  current production APIs, while another test froze an old wild species placement.
- Incorrect metadata: the preset reader turns negative move priority positive
  and reads Porygon's configured Sp. Atk as 8 instead of evaluating it to 85.
- Overclaimed evidence: approximate reward/coverage/animation graphs, the
  checkpoint lab's all-stones-on-Ring arsenal, and baseline comparison do not
  prove actual access, disabled automation, legality or campaign completion.

## What remains, and why

Retained checks have narrow jobs: current-save transactions/corruption, exact
ROM/ELF/source identity, build-cache/config correctness, critical buffer/word/
object-script regressions, configured trainer abilities, preparation indexing,
compiled map references, and checkpoint/observation integrity. They are not team
quality scores, historical layouts, strategic quotas or proof the game is fixed.

The existing save scenario was strengthened, not replaced by a framework: valid
sector IDs with corrupt data/checksums must not copy the damaged sector, and an
intact alternate slot must recover. The formerly false-green checksum mutant
now fails. A small fix to the existing wild checker rejects NONE/EGG/unconfigured
species and noninteger/out-of-range native numeric fields. No new test file was
created for either correction.

Map release checking now excludes speculative dynamic inventories. The current
structural run has zero compiled errors, 47 dormant errors and one unresolved
static ownership case; none of that establishes visual fidelity or walkability.

## Evidence and limits

Final integrated command: `python3 -m unittest discover -s tests -p 'test_*.py'`
ran **156 tests in 64.942 seconds, all passing**, after the combined deletion and
dependency cleanup. This is the remaining host suite, not native battle coverage.

- Retained integrity subset: **86/86** tests across 13 modules, after save changes.
- Retained trainer/preparation subset: **4/4** tests; shared helper imports work.
- Retained campaign/map/agent subset: **66/66** checks. Historical comparator,
  scope and census locks removed; 227-segment manifest validation also passes,
  which is declaration validation rather than played traversal.
- Current materialized trainer source matches the master; **2,339** requested
  trainer abilities pass configured species/form checks.
- Current wild structure: **264** tables pass; the three in-memory invalid
  species/rate/level probes reject for the intended reasons.
- Git whitespace and Python syntax checks are cleanup evidence only, not game
  behavior. No fresh ROM or campaign playthrough was claimed during this pause.

The audit did **not** fix every exposed generator/harness defect. In particular,
signed-priority/stat parsing, transformation exceptions in preset validation,
Circuit schema/provenance, missing multi-opponent coverage, and optional battle-
lab automation/provenance/resource errors remain documented. Do not use those
paths as authorities while authoring battles. Direct current native tables and
actual battle consumers remain authoritative. A projection PASS is only output
agreement, not sound synthesis or competitive quality.

## Detailed coverage and reproduction records

Follow-up during the encounter-AI restart: the native mGBA coordinator returned
success for an empty scenario filter. `tools/mgba-rom-test-hydra/main.c` now
returns failure when zero results were collected (preserving an existing error).
Rebuilt the coordinator and deliberately selected a nonexistent diagnostic:
it printed `No tests found`, returned failure, and `make check` exited 2.
The valid `EC expert pair:` selection subsequently ran all 16 groups and passed.
This prevents an empty run from masquerading as verification; it adds no test
cases or gameplay requirements.

- [Release/world checks](release_world.md): gate inventory, invalid species/rates,
  silent state-ID omissions, comments/dead-code false results, artifact limits.
- [Trainer checks](trainers.md): generator/check consumers, native legality,
  metadata, exact per-file deletions/retention and reviewer correction.
- [Integrity tests](integrity_tests.md): all 36 assigned host modules, independent
  mutation probes, stale fixture evidence, 23 deletions/13 retained modules.
- [Campaign/map/agent evidence](campaign_evidence.md): complete assigned inventory,
  producer-versus-comparator boundaries and optional harness limitations.

Reports preserve pre-deletion line references as audit evidence; those locations
refer to the inspected version and may now be recoverable only in Git. Historical
reports and the Cohesion Book are not build inputs or current gameplay authority.

## Battle handoff

No additional battle was marked complete during the audit/cleanup. Earlier
authored changes through E0045 remain preserved, and E0046–E0135 assignments
were paused before edits. Every completed battle must be confirmed in the main
chat with its intention-based 1–10 rating and an individual improvement path
below 10. Subagent messages, an inventory, passing checks and these documents
do not substitute for that confirmation or for native battle evidence.
