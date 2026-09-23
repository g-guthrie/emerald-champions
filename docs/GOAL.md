# Current goal: coherent campaign, audited route by route

Follow the entire playable campaign in story order, route by route and NPC by
NPC, through the League, postgame and optional/legendary branches. For every
interaction inspect its dialogue, prerequisites, flags, gifts, battles, outcomes,
retries and effect on the next progression gate. Check every item for correct
identity, availability, unintended duplication and useful reward value given our
early item access and convenience systems. Fix concrete bugs as found, simplify
unnecessary/duplicated code, and preserve existing user work.

Main owns the campaign audit and complicated fixes. Keep one reusable Sol Low
worker for bounded analogous-bug and dependency scans after each confirmed bug.
It may implement straightforward fixes only in explicitly assigned files; main
reviews them. Stop other workers; no nested agents or parallel builds. Existing
agent records cannot currently be deleted with the exposed controls.

Use targeted headless screenshots when something visual or choreographed is
questionable. Run focused tests proportional to changed behavior and bind builds
to source. A source review is not a full runtime guarantee. Record area-by-area
coverage, fixes, verification and remaining work in STORY_GATE_AUDIT.md, keeping
handoff concise. Finish with a source-bound testable ROM and honest Git status;
do not declare the game perfect or fully verified while gaps remain.

## Design constraints

Inclement is the story/world baseline. Preserve authored doubles teams and AI
unless a concrete defect is established and explained. Retain native ability
switching, free legal-move tutor/no TMs, native EV/IV services, and HMs unlocked
by story license and badge (compatible party member, no moveslot). The Flight
Beacon may call a Fly-capable Pokémon from the party or PC. Retain the individual
current-cap party Leveler, normal modern XP below cap and silent no-XP at cap,
and restoration of consumed non-berry held items. Consumed Berries require the
Regenerator, gifted by Norman after Badge5, to restore after battle. Before that
they stay consumed. Preserve legendary puzzles.
Knock Off removes held items only for the current battle; Thief and Covet transfer
them permanently in ordinary trainer battles.
Early Eviolite, Choice Band/Specs/Scarf and Focus Sash access must be reflected in
later NPC dialogue and reward incentives. Extra copies can be useful in doubles;
review them rather than automatically removing them. No new overworld item
placements: replace only original Inclement pickups or gifts. No player guides,
Game Book, Center battle presets or Center stat editor.

The active app goal now covers the entire core inventory and supporting seams: debug, simplify, consolidate redundant responsibilities and finish the integrated game. Track all 49 core files in CORE_REFACTOR_PROGRESS.md; existing partial audits are not full-file completion. The broader campaign and finale requirements below remain in scope. Reduce file count only through coherent consolidation, not unrelated concatenation.

## Superseding finale direction

The current priority is restoring authored Wally and building an explicit
post-League campaign: Wally → five S.S. Tidal cabin battles → Steven → Birth
Island's puzzle and Deoxys encounter → Buffel's final trial. League victory is
one milestone; clearing this arc is campaign completion. Defeat or capture counts
for Deoxys, with a permanent completion bit that survives later League runs.
Keep Southern Island available as an additional expedition. Reuse Inclement
maps, existing trainers and item locations. Boat teams and Buffel may be upgraded
under the user's explicit authorization, with truthful native playtest evidence.
Routine route/Gym rematches remain retired; blanket deletion of meaningful
postgame fights is superseded. Preserve exact Wally team and coordination AI,
with the shared authored loadout-knowledge profile intact. The user explicitly permits
knowing available moves; selected actions and pending switches must remain private. Difficulty remains
provisional until tested against strong, explicitly described player parties.

## Explicit scope addition

The user explicitly includes battle mechanics and wild Pokemon distribution. Audit move/ability/item interactions and encounter locations, rates, levels, held items, species availability and timing against campaign progression. Preserve the authored AI information model and teams unless a demonstrated defect requires an explained fix. Full goal remains incomplete.
