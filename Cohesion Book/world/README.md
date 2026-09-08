# World cohesion volume

This volume specifies the final world around the existing authored battles. It preserves the current game where it is coherent, names exact repairs where it is not, and leaves the live game and frozen baseline untouched.

The final charter is expert doubles from the beginning, broad convenient preparation, no native trainer singles, standard opposing parties of at least four, and DIFF-01's Medium opponent floor at the live player cap minus two. Ordinary wild and legendary capture encounters remain singles. The specifically requested Birch rescue is a scripted doubles exception with two chosen starters, owned by INTRO-01.

## Read the experience

1. [Opening and western woodland](01-opening.md)
2. [Dewford and Slateport](02-dewford-and-slateport.md)
3. [Mauville and the meadows](03-mauville.md)
4. [Volcano, ash and desert](04-volcano-and-desert.md)
5. [Rainforest and Mt. Pyre](05-rainforest.md)
6. [Eastern sea and story crisis](06-sea-and-crisis.md)
7. [Victory Road and League](07-league.md)
8. [Frontier and exceptional revisits](08-frontier.md)
9. [Side activities](09-side-activities.md)
10. [Support and inert content](10-support.md)

## Review and implement

- [Exact world changes](changes.md): complete dialogue replacements, interaction behavior, placement, prerequisites and acceptance.
- [Every registered map](map-index.md): 540 individual map pages, each accounting for all of its physical events and callbacks.
- [Shared contracts](common-contracts.md): reusable behavior contracts with the exact shared-root instance inventory.
- [Machine event ledger](event-ledger.json): all 6,322 map event/callback records, keyed by map and event ID.
- [Dialogue ledger](dialogue-ledger.json): current resolved text labels and their event/proposal references.
- [Unrooted entrypoints](unrooted-entrypoints.json):132 candidate inactive roots, explicitly distinguished from currently accessible NPCs.
- [Proposed edits manifest](proposed-edits.json), [coverage evidence](coverage.json), and [opening geometry](opening-geometry.json).

## What the evidence means

The source snapshot contains 540 registered Hoenn maps,3,052 objects,421 coordinate events,723 background events,1,402 warps,152 connections and 572 map callbacks. The map scripts total86,041 lines; with shared assembled campaign scripts the structure gate examined106,569 lines. It passed map/label/reference geometry and value-returning-special contracts.

The review used per-map dialogue and state digests, complete event records, common engine contracts, and focused source traces for every proposed repair. Shared trees, berry plots, nurses and support rooms receive the same examined contract rather than invented per-instance mechanics. Map-specific coordinates, flags, neighbors and final dependencies remain explicit.

This is not an instruction-by-instruction runtime execution of every script. No ROM was built, no gameplay state was changed, and no fresh-save traversal or battle win is claimed. Full implementations still need the stated field, menu, transaction, scene and combat evidence. Catalogue coverage and an exact specification are useful preparation for that work; they are not a substitute for it.

Some prior suspected defects were rejected after checking current consumers: the Vial upgrade already prevents downgrades; Heatran's misleadingly named flag controls physical presence; Meltan, Ursaluna, Kingambit, Runerigus and several regional forms already received source fixes. The book preserves those corrections.

Persistent receipt IDs use the [central allocation appendix](../appendices/state-allocation.md); the book does not change live headers or save structures.
