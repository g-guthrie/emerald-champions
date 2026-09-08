# Final integration audit

The main specification, battle volumes, world volume, encounter catalogue, acquisition/form index and Mega catalogue are reconciled. The [book validator](../tools/validate_book.py) checks coverage and internal consistency against this finite frozen source inventory. These checks apply to the document, not as permanent game-content quotas.

## Coverage reconciliation

- The two battle volumes have disjoint review keys and together cover all 516 native trainer branches in 468 encounter groups. Final teams contain 2,339 Pokémon slots; 501 complete loadouts are preserved and 15 branches change. Only explicitly paired Half contributions contain three Pokémon.
- The world ledger's map names match all 540 registered maps. Its 6,322 event/callback records reconcile by kind to 3,052 objects, 421 coordinate events, 723 background events, 1,402 warps, 152 connections and 572 callbacks. Every map has an individual page.
- The 138 wild-map reviews include all 146 source header rows and 380 method records. Every final method sums to 100%; the ordinary per-species floor also holds under the source's reversed-slot encounter behavior, with the deliberate Feebas exception. All 29 changed slots and their displaced-family alternatives are documented.
- The acquisition review keys exactly match all 1,579 configured entries including NONE. The 1,578 actual species/forms represent 1,025 National Dex identities. There are 1,515 specified active paths/presentations and 63 deliberately inactive legacy entries. This is conditional design coverage, not proof of a completed collection playthrough.
- All 99 Mega Stone items have individual records. Native item/form bindings, physical pickups, NPC gifts and berry trades are accounted for; the catalogue distinguishes 117 source routes from 109 actual world entitlements.

## Shared changes and state

The 213 world targets and 40 gym dialogue targets have no collision on the same source-file/text-label pair. Overlapping files still share dependencies, so implementation follows the named owners in [chapter 08](../chapters/08-implementation-sequence.md): opening text/state belongs to INTRO-01; form gifts use its always-present aide priority; second-starter stones use one receipt schema; world geometry and native dialogue targets retain their specified owners.

The independent [cross-volume review](cross-review.md) found seven concrete issues and resolved them in the specification, including old lab OnFrame re-entry, missed legacy early supplies, absent Steven after a prior gift, an ambiguous quiz answer, and unsupported singles entrypoints. These are proposed fixes, not claims of fixed native behavior.

The final migration has three distinct reclaimed variables and ten distinct receipt flags, all declaration-only in the frozen symbolic scan. Version 4 initialization preserves starter-stone bits 0/1, clears the newly assigned bits 2/3 once, and preserves old Sign IDs while initializing new bits 82–84. No save structure is enlarged. Ordinary subsequent loads must not reset these values. The source allocation and finite relic/sign capacities remain implementation checks.

The [reconciled shared AI contracts](shared-ai-final.json) and chapter 06 supersede the earlier working AI audit where they differ, particularly the final SAI-07 action-pair search bound. The single `fishing_species_overrides` field supersedes an earlier Old Rod-specific idea; no independent second encounter database is proposed.

## Source identity and limits

All 10,703 frozen source files retain their captured hashes, and every reviewed canonical counterpart still matches at handoff. HEAD remains `662cd5a3d69bf66b491d557427316c926831458b`. The [live comparison](source-drift.json) also records one Git-status addition outside the reviewed snapshot: `pokeemerald-test.inputs.json` is now modified. That build-input metadata is not a reviewed source change or evidence of a newly validated ROM. Its cause is not attributed by this comparison.

The book contains no implementation claim. No new ROM, fresh-save traversal, rendered-scene pass or native battle validation was produced for the proposed revision. The relevant acceptance in each chapter remains necessary during implementation. Static route coverage cannot prove every story state reachable; a mathematical benchmark cannot prove an autonomous battle decision or game balance.

The [final validator output](book-validation.json) records the actual check time, document/link counts and any errors. Its source hashes and coverage are evidence of an intact, complete specification package. Recompare live files when implementation starts, since later work may change them.
