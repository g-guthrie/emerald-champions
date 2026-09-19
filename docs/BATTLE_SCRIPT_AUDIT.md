# Battle script audit — September19

Scope: battle_script_commands.c reward/capture/state-machine connections. This is
one file-level review, not completed campaign or story acceptance. No subagents.
Authored AI/team sources remain untouched.

## Corrections and cleanup

- Reproduced Heavy Ball negative-rate wraparound: against the light, low-catch-rate
  Azelf test board, ordinary ball odds were1 while Heavy Ball odds became17895681.
  A negative additive rate crossed into unsigned HP arithmetic. Clamp the adjusted
  rate to at least1 rather than applying the penalty twice.
- Reproduced zero odds from small multipliers (Beast Ball on the same board). Keep
  the final integer odds at least1 so the shake formula cannot divide by zero.
  Master Ball's guaranteed sentinel still bypasses the calculation. Existing
  125-percent campaign catch modifier and ball bonuses are unchanged above the floor.
- Separated per-recipient XP calculation from command/message/controller state.
  Reset recipient rewards before processing and handle the end-of-list sentinel
  before indexing the party. Modern full/half shares, species caps, EV awards and
  silent zero-EXP processing remain intact.
- Consolidated identical trainer-money branches while retaining existing rates,
  two-trainer handling and the empty-party fallback. Added a zero-size guard before
  indexing the final trainer mon. Removed a duplicate test-header include.

## Evidence

Deus MCP complexity/connection receipts are in work/battle-script-audit-20260919.
Capture trace identifies5 callers and15 downstream links; source inspection
cross-checked those links and the script/controller state transitions. Cached graph
line numbers describe the pre-refactor source. Script DSL coverage is incomplete.

The capture arithmetic predates the Inclement integration (git blame231531c6a3e)
and was also present in the inspected upstream source:
https://raw.githubusercontent.com/rh-hideout/pokeemerald-expansion/master/src/battle_script_commands.c
This is not evidence that integration caused it or that the authored AI needs work.

before-capture.log: both new capture regressions fail for the demonstrated reasons.
after-tests.log:3 capture tests,3 existing XP battle tests and the Master Ball/Ultra
Beast regression PASS. extended-tests.log:2 additional actual engine battle tests
PASS (large award stops at cap; Mewtwo's lower individual cap stays silent).
Normal build and deterministic release gates PASS.9 focused test groups total.

## Coherence reviewed / scope limit

Reviewed successful capture bookkeeping, party/PC handoffs, follower-party save
boundaries, original held-item restoration and XP/reward eligibility. No changes
to story flags or reward prices were inferred merely from complexity ratings.
The map-by-map progression/dialogue/reward audit is still unfinished. Battle move
commands elsewhere in this large file were surveyed, not exhaustively replayed.
No claim is made that all battle mechanics or the whole game are bug-free.
