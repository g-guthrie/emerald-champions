# Mega mechanics pass — September 20, 2026

Source review: battle_gimmick.c eligibility dispatch, trainer ownership/usage,
multiple-Mega reservation, activation record and indicator lifecycle; native
Mega test sources and configured Pokemon stat calculation. No production
mechanics/AI changes in this pass.

Executed every one of the 18 declared native test groups in mega_evolution.c
and emerald_champions_megas.c. Coverage includes 104 parameterized added/form
Mega transformations with their actual requirements, authored Steven slot
permissions/native eligibility/form forecasts, switch ownership/held stone,
Gen6 vs Gen7 action order, replaced abilities, two-sided same-turn Mega,
faint/revive and battle-end restoration. All 18 have PASS evidence across
work/mega-audit-20260920/tests.log, after.log and additional.log.

Two upstream turn-order fixtures retained a 205 Speed expectation based on zero
IVs. Current default IVs are31 with conventional stat calculation, yielding236
for level100 neutral Mega Gardevoir base Speed100 and zero EVs. Both scene/order
assertions already passed. Made Level100/SpeedIV31 explicit and expected236;
retained exact speed and ordering assertions. No weakening of mechanics checks.

Limits: TESTING intentionally permits other gimmicks and uses chosen-gimmick
fixtures; these tests do not establish release Mega-only policy by themselves.
Existing production filter was source-reviewed. Native fixture coverage is not
full authored campaign tactical acceptance or UI sprite exhaustion coverage.
Indicator/trigger allocation-failure behavior remains a review item. Full goal
and broad battle mechanics audit remain incomplete. No new normal ROM rebuild
needed because only test fixture data changed.

## Absent-indicator lifecycle repair

Shared GetIndicatorSprite now rejects the existing zero sentinel and non-live
allocation IDs. Visibility, priority and level positioning use it consistently.
Native before.log reproduces modifying sprite0 with no indicator; after.log
preserves sprite0 and the MAX_SPRITES dummy, while a valid indicator still receives
priority and one/two/three-digit level offsets. Evidence: work/indicator-audit-20260920/.

Source reachability: Safari's player healthbox has no indicator; BattleStruct's
zeroed ID is intentional, and shared priority updates still run. A normal
indicator cannot receive0 because its healthbox is allocated first. MAX_SPRITES
has dummy storage and allocation asserts, so this was not claimed as an observed
out-of-bounds allocation crash. Indicators live with healthboxes until battle
sprite reset; no independent destruction change was needed. Mechanics/AI unchanged.
No fresh Safari visual replay in this pass; the test verifies sprite-state writes.
