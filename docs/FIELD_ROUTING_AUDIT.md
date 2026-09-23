# Field routing pass — September 20, 2026

Partial field_control_avatar.c review: frame input creation/reset, interaction
priority, steps/encounters, Dive handoff, coordinate-script lookup and duplicate
tiles. No source-proven live gating defect found in those paths this pass.
Simplified FieldClearPlayerInput to whole-structure zeroing, including four
currently unused bits; removed a forcedMove check already guaranteed by its
outer condition. Preserves active input priority and valid control behavior.

Twelve shared-coordinate groups occur in non-FRLG map definitions. Native test
uses actual compiled map events for Littleroot's first-exit/Birch rescue tile,
Rustboro's stolen/returned goods tile, and New Mauville's two Rotom stages.
Each selects the expected first or later event as its variable changes and
returns NULL when neither condition matches. Input reset test clears a dirty
structure. Both groups pass: work/field-routing-20260920/tests.log.

No story gate, encounter rate, HM requirement or AI changes. Remaining: full
movement/collision/warp choreography and all script-control transfers, including
other shared tiles. This is native event selection, not a visual map traversal
or complete field-player-avatar audit. Release logs share the work directory.
