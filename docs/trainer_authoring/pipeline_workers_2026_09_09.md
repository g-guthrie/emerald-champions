# Explicit emulator worker control — September 9, 2026

The runtime gate script previously passed `--jobs` only to its make child.
Hydra reads its own MAKEFLAGS, which a directly invoked Python parent does not
receive back from that child. Such a run could build in parallel and simulate
with one worker.

`run_emerald_champions_runtime_gates.py --workers N` now forwards `HYDRA_JOBS=N`
explicitly to Hydra. The default is `--jobs`, capped at 32. Values outside 1–32
fail before running gates. Hydra validates the same explicit environment budget
and reports its effective count. Without an explicit budget, existing make -j
behavior remains supported. Gate filters remain sequential; no nested worker
pools or concurrent writes to the shared filtered ELF were introduced.

Evidence:

- Host Hydra compilation succeeded.
- All 24 existing runtime-gate integrity tests passed; no new permanent tests.
- Explicit values 0, -1, 33, garbage, 2x and a huge integer were rejected by
  Hydra with exit 2. Python rejected --workers 0 and 33 before execution.
- `HYDRA_JOBS=2 make -j6 check TESTS='EC preset EV review'` reported two workers
  and completed two native groups on workers 0 and 1, each exactly once.

This proves explicit worker selection and a small concurrent run, **not** the
optimal worker count or representative battle throughput. Benchmark 2/4/6/8
workers on the same immutable representative battle workload before selecting
the sustained CPU budget. Host throughput is separate from GBA decision latency.
The unsuccessful Protect-scoring experiment was subsequently withdrawn (see the
Gen 9 follow-up report); this pipeline change certifies no trainer completion.

## September 10: verify executed pressure, not issued commands

Coby's proposed Snarl sample initially delivered no Snarl: native Thunderbolt
paralyzed Umbreon before it moved. The runner's default weighted outcome chooses
the last nonzero entry, so every subsequent paralysis roll blocked its action.
Default secondary effects also succeed. This is deterministic diagnostic RNG,
not a representative statistical distribution of game play.

Use actual player status, last executed move and PP when interpreting pressure.
The temporary reusable `work/ec_review_common.inc` now prints `REVIEW_PLAYER`
alongside trainer state. A legal Lum counter-scenario let the first Snarl land
and proved Competitive activation/payoff; its earlier paralysis-only trace did
not. Do not change runtime mechanics or declare an ability broken because a
scripted command never executed. Temporary text tracing was removed; no new
retained test or runner behavior change. See Coby's individual Gen9 review for
the exact native trace, counter-scenario and remaining limits.

## September 10: hidden Commander actors and pair scores

Nolan exposed a diagnostic-only turn boundary issue: a swallowed Tatsugiri skips
decisions, leaving automatic AI PASS expectations queued. When Dondozo faints,
the runner can count Tatsugiri's next, unexecuted choice as an extra played turn.
The temporary Nolan probe removes only those implicit right-side PASS entries,
asserting they contain no explicit expectations. Player records still define
the requested turn count; game behavior and explicit assertions are not changed.
Do not apply this workaround to a test with actual expected AI actions without
reviewing its contract. The permanent runner was not modified for this issue.

The doubles pair evaluator scores through AI_ScoreMoveAgainstTarget; a later
read of finalScore may return zeros and is not its per-action evidence. Nolan's
native rescoring shows that no-bench Flip Turn already lacks the Hero bonus
and has a lower independent score than Ice Punch. Its weak selection remains
an unresolved pair-decision limitation, not a diagnosed bonus bug. See the
individual Nolan review and retained temporary logs for exact evidence.
