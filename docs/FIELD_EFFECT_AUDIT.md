# Field-effect palette lifecycle — September 20, 2026

Partial field_effect.c pass: script dispatch/operand advancement, active-list
registration/removal, graphics sharing and faded-palette loading. Active-list
duplicates are intentional for concurrent instances; not deduplicated.

Confirmed native failure: with all16 sprite palette slots occupied, the direct
faded-palette loader receives0xFF, wraps its color-map index into unrelated slot15,
and passes255 to weather buffer calculations. Both direct and script loaders now
skip palette updates on failure; script still advances its pointer by all five
operand bytes. Shared UpdateSpritePaletteWithWeather also rejects slots>=16.
Native before.log reproduces changed mapping; after.log checks direct/script
failure, operand advancement, invalid weather slots and successful reuse after
freeing a palette. Evidence: work/field-effect-audit-20260920/.

Additional source findings remain open: reflection and dynamic follower palette
callers also omit failure handling. The shared weather guard prevents their255
index from escaping palette buffers, but some callers truncate failed allocation
to OAM palette15 and can still show unrelated colors. Next pass must preserve or
hide/retry with valid palettes in field_effect_helpers.c and
 event_object_movement.c. This is not complete allocation-failure recovery.
No battle mechanics/AI/save-format changes. Full goal incomplete.

## Reflection follow-through

Consolidated duplicate creation/refresh pond/ice palette loading into one bool
success helper. Both it and high-bridge loading check allocation before assigning
OAM paletteNum. Failure leaves the prior index unchanged; the caller hides the
reflection and retries through its normal update. High-bridge retry uses the
existing nonzero bridge vertical offset, so allocation failure does not accidentally
switch it to a pond filter. Shared source palettes remain protected by the existing
sprite-usage scan before freeing the reflection's old palette.

Native parameter cases exercise pond, ice and high bridge with all16 palette
slots full; failure preserves the original index and source palette, freeing one
slot enables retry, and repeated load reuses the result. Existing generic loader
regression also passes. Evidence: work/reflection-audit-20260920/. No screenshot
replay was performed; tests verify loading/assignment and source review verifies
visibility/retry dispatch. Follower/dynamic species and generic object-palette
failure propagation still remains outstanding in event_object_movement.c.

## Generic object palette replacement

UpdateSpritePalette now keeps allocation results wide until validated and returns
0xFF on failure without writing it to four-bit OAM. Existing matching palette is
reused immediately, eliminating needless free/reload; shared old palettes remain
protected, and unshared old palettes are released before a new allocation. Sprite
inUse state is restored to its original value. Native test fills all16 slots and
checks shared-palette survival, failure index preservation, subsequent retry,
already-loaded reuse and obsolete-palette release. Reflection regression also
passes. Evidence: work/object-palette-20260920/. Higher-level callers may temporarily
retain old colors; this is not a guarantee that every failed graphics change can
render its desired new colors. Dynamic follower/species creation paths remain
outstanding, including their invalid palette tag lookup and OAM failure propagation.

## Dynamic species palettes

Corrected normal/shiny palette-presence checks and matching female palette
selection. Front-sprite fallback now tags by full graphics identity rather than
bare species, so normal/shiny/female variants cannot share the wrong cached
palette. Creation checks0xFF before palette-tag lookup or OAM assignment, and
preflights before compressed tile allocation. Removed an unused template-output
parameter from the graphics-ID helper. Dynamic refresh uses one shared helper
and retains a shared old OAM palette when replacement cannot allocate.

Native tests choose an enabled fallback species and verify distinct reusable
normal/shiny slots. A full16-slot pool rejects loader and object-graphics creation;
refresh preserves the shared old palette and succeeds when a slot is released.
Generic object and pond/ice/bridge regressions remain green. Evidence:
work/dynamic-palette-20260920/. Source review covers all dynamic call sites.

Recovery limitation remains: map object creation/recreation failures mark the
object inactive for ordinary template spawning to retry after capacity returns.
A moved object's transient position/movement state can reset on this rare path.
That is not full-fidelity recovery and remains a follow-up requirement before
claiming complete exhaustion handling. No fresh visual replay in this pass.
Battle AI, encounter placement and save layout unchanged.

## Restored-object state preservation

Supersedes the template-position recovery limitation above. Return-to-field
palette failure now keeps the existing ObjectEvent active and reconstructs a
hidden sprite without allocating a palette. A runtime16-bit pending mask tracks
these restored slots; it is cleared on object clear/removal and updated on each
recreation. It is not added to the save format. SaveObjectEvents retains the
active object's actual state, and restoration re-evaluates palette availability.

The shared movement update retries loading; logical movement continues to avoid
script deadlocks, while ground effects and visibility are suppressed until the
palette is available. This preserves original coordinates/identity without a new
snapshot/task system. The visibility helper also respects pending state. Native
test exercises actual sprite recreation under a full palette pool, verifies the
object stays active at moved coordinates/facing/localID, then frees a slot and
checks successful visible recovery. Both dynamic-palette groups also pass.
Evidence: work/palette-recovery-20260920/. No fresh visual script/collision replay
claimed; full overworld exhaustion/choreography coverage still remains.

## Pending-placeholder ownership completion

Native removal-while-pending test exposed a gap in the earlier source review:
teardown cleared the pending bit before resource release, so it freed palette0
which the placeholder never owned. The bit now survives resource cleanup and
clears afterward; pending removal skips palette release, not tile cleanup.
Generic and dynamic refresh helpers use a shared sprite-identity lookup to skip
freeing borrowed palettes and clear pending only when a requested palette binds,
including same-slot reuse. Non-dynamic graphics changes now retry their static
palette instead of unconditionally revealing the placeholder.

Extended native recovery cases cover dynamic retry, removal while pending, and
static-graphic recovery, with refresh calls during exhaustion verifying borrowed
palette retention. Six test groups pass together: recovery, dynamic variants and
failure, generic replacement, reflection variants and field-effect loading.
Evidence: work/palette-removal-20260920/. This reinforces that prior source review
was not complete runtime proof. Full overworld visual/script traversal remains
outside these tests; AI and save layout unchanged.
