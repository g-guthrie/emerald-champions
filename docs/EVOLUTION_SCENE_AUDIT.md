# Evolution scene pass — September 19, 2026

Scoped review: normal/trade state transitions, completion/cancellation cleanup,
shared species/stat/Dex commit, and disabled move-learning branch. Normal B-button
cancellation is reachable. Trade cancel states currently have no production
transition into them; their cleanup bug is latent, not a demonstrated live crash.

Fixed dangling if in trade END: cancellation previously skipped DestroyTask while
still freeing the scene allocation. Native wrapper executes that actual END state
for canceled/completed cases; before fails cancellation, after passes both. The
wrapper does not claim an end-to-end trade-cancellation UI test.

Consolidated duplicate field/trade evolution commit into CommitEvolution: species,
tracker reset, stat recalculation, player cap, name update, seen/caught Dex and
statistics. Presentation/music/link timing remain scene-specific. Native commit
test verifies species/tracker/Dex changes and held-item/move preservation.

No active move-learning cancellation bug claimed: P_LEVEL_UP_MOVE_LEARNING is
FALSE. No additional cleanup mismatch found; trade leaves its surrounding screen
resources to its owner as before. Evidence: work/evolution-audit-20260919/.
Full trade protocol, every evolution method, chained cancellation, graphics and
all party/PC handoffs remain to verify. AI and save layout unchanged.
