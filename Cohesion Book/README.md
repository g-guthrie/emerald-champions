# Emerald Champions — Cohesion and Rearchitecture Book

> Historical specification. The user's September 8 repair audit supersedes this
> book wherever it conflicts with current instructions. Its source inventories
> can aid investigation; its prescriptions and acceptance prose do not establish
> correct gameplay. See the repository `AGENTS.md` and current audit reports.

**Complete specification, ready for review. September 8, 2026.** This book proposes the next revision; it has not been implemented or playtested.

This book examines the existing authored game and specifies a cohesive revision while preserving successful work. The live game, ROM and save are outside the writing scope. The source snapshot under `baseline/source/` includes uncommitted authoring captured on September 8, 2026; `baseline/manifest.json` identifies every copied input.

The governing design is expert-level competitive doubles from the opening. Strategic sophistication is not a progression reward. Species capability, opponent levels and party size establish the difficulty hierarchy. Broad player choice, convenient preparation, habitat identity, exploration and iconic trainer Pokémon support that competition.

## Start here

- [Read the book in one Markdown file](BOOK.md): the decisions and all ten main chapters, with links into the detailed catalogues.
- [Decision index](DECISIONS.md): the proposed changes, preserved design, and source of authority for implementation.
- [Implementation sequence](chapters/08-implementation-sequence.md): nine coherent integration units and their acceptance evidence.

## Main chapters

|Chapter|Subject|
|---|---|
|[00 — Design charter](chapters/00-design-charter.md)|Final user requirements, preservation, scope and evidence.|
|[01 — Opening and starters](chapters/01-opening-and-starters.md)|Two permanent starters, doubles rescue, catching before the four-member rival, all regional choices and old-save migration.|
|[02 — Competition rules](chapters/02-competition-rules.md)|Party sizes, universal level floor, exact Champions Points, Tents, Trainer Hill, Circuit and link entrypoints.|
|[03 — Wild distribution and biomes](chapters/03-wild-distribution-and-biomes.md)|Habitat-specific assessments, complete final encounter tables, collection gaps and encounter-engine repairs.|
|[04 — Acquisition, evolutions and forms](chapters/04-acquisition-evolutions-forms.md)|Every configured species/form, conditional routes, gifts, form tools and irreversible branches.|
|[05 — Mega Stones and legendary rewards](chapters/05-mega-stones-and-legendary-rewards.md)|Every stone, native transformation prerequisites, world placement and safe reward delivery.|
|[06 — Shared AI](chapters/06-shared-ai.md)|Twelve concrete changes to common expert decisions, forecasting, switches, Mega use and paired actions.|
|[07 — Preparation and evolution convenience](chapters/07-preparation-and-evolution-convenience.md)|Ready evolutions at the cap, expanded moves, optional friendship preparation and field compatibility.|
|[08 — Implementation sequence](chapters/08-implementation-sequence.md)|Integration order, shared ownership, migration and completion criteria.|
|[09 — Tests and evidence](chapters/09-tests-and-evidence.md)|Which checks to keep, update or retire; what source analysis and later runtime tests can establish.|

## Detailed volumes

|Volume|Complete coverage|
|---|---|
|[Early battles](battles/early/README.md) and [late battles](battles/late/README.md)|516 authored trainer branches in 468 encounter groups. Every branch has its own assessment and complete baseline/final team.|
|[World](world/README.md) and [map index](world/map-index.md)|540 registered maps and 6,322 object, coordinate, background, warp, connection and callback records. Includes live/shared/inactive entrypoint distinctions.|
|[Wild encounter catalogue](appendices/wild-encounter-catalog.md)|138 maps, 146 header rows and 380 encounter-method records, with complete probabilities and individual habitat assessments.|
|[Species and forms](appendices/species-and-forms.md)|1,578 actual configured species/forms representing 1,025 National Dex identities; active routes and excluded legacy gimmicks are distinguished.|
|[Mega Stone catalogue](appendices/mega-stone-catalog.md)|All 99 stones, 103 item-triggered Mega bindings and 117 raw world source routes grouped into 109 entitlements. Rayquaza's separate move-based Mega is also covered.|
|[Persistent state allocation](appendices/state-allocation.md)|One coordinated save migration, three reclaimed variables, ten receipt flags and preserved existing reward bits.|

The specification preserves 501 complete authored trainer loadouts and changes 15 branches, including six opening aliases. Shared AI and level changes still apply to preserved teams. Wild revisions change 29 slots across 23 maps; the other 115 wild map rosters remain intact. World proposals identify 213 exact targets in 27 changes, with 40 additional exact gym dialogue edits in the battle volume. These are coverage counts, not future content quotas.

## Evidence and implementation records

- [Book validation](review/book-validation.json), [cross-volume review](review/cross-review.md), and [final integration audit](review/integration-audit.md).
- [All trainer reviews](review/all-trainers.json), [final wild distribution](review/wild-distribution.json), [forms](review/acquisition-forms.json), [Mega rewards](review/mega-rewards.json), and [final shared AI specification](review/shared-ai-final.json).
- [Exact world edits](world/changes.md), [world edit manifest](world/proposed-edits.json), [gym dialogue edits](battles/early/gym-dialogue.json), and [opening state/choice specification](review/opening.json).
- [Snapshot manifest](baseline/manifest.json), [captured working-tree status](baseline/git-status.txt), and [live-source comparison at handoff](review/source-drift.json).

The book is bound to 10,703 frozen source inputs captured at **2026-09-08 16:26:43 UTC**, on `main` at `662cd5a3d69bf66b491d557427316c926831458b`, including then-uncommitted work. The manifest, rather than the commit alone, identifies the reviewed game. The snapshot is evidence, not a complete replacement checkout or build distribution.

All work in this task stayed inside this separate book package. No game implementation, ROM build, save change, fresh-save traversal or native battle validation is claimed. The book contains source-backed design and precise implementation instructions; the stated runtime and visual acceptance remains work for the implementation phase.
