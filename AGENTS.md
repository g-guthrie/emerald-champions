# Working on Emerald Champions

## Executable-AI restart — latest user instruction

The user explicitly said to start over and fix the AI for each battle as it is
handled. Restart at E0001; pause later trainer-authoring batches and preserve
their existing edits. Previous design/source reviews do not count as encounter
AI completion. Run relevant native decision scenarios using actual compiled
trainer parties and native stat generation, diagnose/fix executable failures,
rerun the scenarios, and report each completed battle separately in the main
chat with team changes, AI changes, observed decisions, rating and remaining
limits before advancing. Human-readable plan/crack prose is not executable AI.
Do not force a move just to match a hoped-for script or read the human's pending
commands. Keep the AI bounded and measure latency as shared logic changes.
Temporary diagnostic scenarios are not a new permanent per-battle test suite;
retain only distinct necessary regressions under the user's test-minimization
policy. A diagnostic PASS means execution, not tactical correctness.

## Static-check audit prerequisite — latest user instruction

The user explicitly requests deleting tests unless they are absolutely necessary.
Remove stale design locks, brittle source-text checks, duplicated scaffolding and
checks without distinct observable value. Preserve only a small, justified set
for current save/artifact integrity and demonstrated serious regressions; do not
rebuild a large suite or change gameplay to make old expectations pass.

Pause further trainer authoring until all current static checks have been audited
against their actual consumers for stale inputs, wrong contracts, weak assertions,
false passes and false rejections. Audit findings are not authorization to change
gameplay to satisfy a checker. Track the reviewed check inventory and distinguish
source review, negative probes and executed tests. Every subsequently completed
battle must receive its own confirmation and intention rating in the main chat;
subagent messages and review documents do not substitute for that confirmation.

## Complete trainer authoring pass — latest user instruction

The user explicitly requested subagents to hand-author and optimize every current
campaign trainer battle, authorizing changes to team composition, moves, items,
abilities and Stat Points where they improve the encounter. Account for the
campaign-wide distribution of trainer species and strategies, not just isolated
team strength. This supersedes the earlier five-encounter scope and any blanket
loadout/stat preservation rule. Preserve progression, live caps/difficulty
offsets, gym identities, current party sizes, the repaired level-3 rescue, wild
habitat work and unrelated changes unless a concrete conflict requires discussion.

Maintain individual branch coverage and report each completed battle in chat
with an honest intention-based 1–10 rating; below 10, explain how it could be
better. Ratings are design judgments, not measured win rates. Do not label an
inventory, unchanged prose, or generic AI test as individual battle completion.
Track intended lead, partner payoff, reserve transitions, counterplay, investment
and speed logic, native mechanic constraints, and species/strategy repetition.
Existing good choices may be retained with explicit reasons; no change quota.
One owner runs canonical materialization; parallel authors may edit only their
assigned encounter blocks and individual review records. Preserve concurrent
edits. Record native testing separately from static/design completion.

## September 8 repair audit — current instruction

The user rejected the 5.1 redesign as reliable authority and requested a game-wide
audit and fixes, with proactive subagents. Current explicit changes: the Birch
rescue opponents are level 3; choosing two starters leads straight into the rescue
without pre-fight team editing; the second wild overworld actor animates; the
first Center tool handout transitions naturally into an optional healing offer.
Repair synchronous battle stalls and faulty strategy with fast, coordinated,
per-battle instructions, including weather, redirection and Trick Room. Audit
actual wild distribution and trainer quality. The Cohesion Book is historical
design input, not an immutable specification or proof of correct gameplay.
These instructions supersede conflicting book implementation requirements below.

The user subsequently approved a habitat/discovery revision and explicitly
requested stronger early choices, especially Fighting options such as Timburr.
Early rosters may change to provide useful competitive tools. Preserve early
powerful discoveries (including Pheromosa and Kartana), babies, and demanding
trainer/gym difficulty; do not weaken their stats or move them later to create
discovery pacing. The implemented opening adds Mienfoo on Route101, Timburr and
Pachirisu on Route102, with existing free preparation. Land duplicate slots now
deliberately emphasize habitat residents; do not refill them with generic
species. Exact Surf/rod species live in `wild_route_sheet.json`; the generator
copies them without changing land, levels, or encounter rates. See
`docs/WILD_HABITAT_REVISION_2026_09_08.md` for the current changes and evidence.

The user subsequently approved every redistribution recommendation in
`docs/WILD_AREA_RATINGS_2026_09_08.md`. Preserve its 30 keep decisions and the
stronger early choices while applying the 108 area revisions. Their latest
correction requires Pineco, Heracross and Ferroseed in accessible grass during
the first Petalburg Woods visit: Rock Smash is not available then and must not
gate those early choices. Rock Smash may offer supplemental catches. This
correction supersedes any conflicting placement in the approved report.
The completed 108-area implementation and production evidence are in
`docs/WILD_AREA_REVISION_2026_09_08.md`; the numerical ratings remain a
pre-change snapshot, not claims of fully playtested campaign balance.
The user then authorized further discretionary wild-distribution refinements.
The follow-up promotes Route123's orchard to mature residents (including
Hydrapple and Arboliva) and adds Crustle at 25% to the main Woods Rock Smash
table. Preserve the first-visit grass Heracross/Pineco/Ferroseed access.
See `docs/WILD_AREA_POLISH_2026_09_08.md` for exact changes and evidence.

The user subsequently approved raising trainer-strategy quality with targeted
improvements, after discussing Perish Trap, Storm Drain, Steam Engine and creative
ability partnerships. That authorizes a focused pass on existing encounters'
teams and executable tactics. Preserve gym identities, current party sizes and
level/cap settings; verify the intended combo and useful counterplay. The earlier
instruction to preserve every loadout is not a veto on these approved upgrades.

Preserve the current game design. Fix demonstrated bugs and refactor without changing behavior. Discuss changes to progression, availability, economy, difficulty tuning, or player options before implementing them. Retry restarts the current battle; Reload restores the last saved game. Both are intentional and must remain separate.

The user-approved campaign direction (September 5, 2026) intentionally demands competitive knowledge from the opening. Easy/Normal/Hard are global opponent-level calibration offsets, not different teams or AI. Prefer coordinated, demanding battles over filler. The route-conversation reduction and gym coordination changes are deliberate; old encounter totals, percentage quotas, and historical prose must not restore retired fights or override the current authoring. This campaign revision does not require compatibility with pre-tuning save files. New design changes can be discussed and revised rather than treating old documents as immutable requirements.

The user subsequently authorized improvements to writing, pacing, choreography, and story during the campaign audit, including revisiting earlier scenes, toward a richer Pokémon odyssey. That authorization covers coherent improvements within the campaign direction above. The audit also includes the complete legendary-Sign pipeline and standard, modern, and imported Inclement custom Mega Evolutions; verify the actual compiled roster and runtime behavior rather than assuming imported content is present.

On September 8 the user superseded the old mandatory-Devon and one-attempt legendary design. Devon is an optional guide. Field Signs use deliberate landmark interactions instead of random encounter hunts, preserve badge/story gates, and use species requirements sparingly. A failed encounter returns after leaving and re-entering its area; captures and earned rewards remain permanent. Keep the full, revisitable legendary guide accurate.

The user also approved keeping Terapagos in Cave of Origin B1F with a deliberate landmark encounter in that otherwise quiet room, Dondozo as a wild encounter near the submerged submarine, and distinctive Dive encounter rosters with rare finds. The September 7 revision raises the minimum ordinary species encounter chance to 5% per encounter method, with Feebas the sole exception. Preserve the flattened distribution to avoid grind. Use the relaxed legendary rules above while preserving these underwater habitats.

## Canonical ownership

- `src/`, `include/`, and game files under `data/`: executable behavior and compiled tables.
- `data/emerald_champions/`: authored trainer/preset data and imported reference datasets used by generators. Preserve authored values; synchronize their materialized game tables through the relevant generator and comparison check.
- `tests/campaign/`: gameplay traversal definitions, checkpoint recipes, and campaign reference results.
- `tests/headless/` and `tests/reference/`: runtime harnesses and explicit reference baselines.
- `scripts/` and `tools/agent_player/`: verification, generation, and the two testing pipelines.
- `docs/`: explanations derived from code. Never use prose as proof or as a build input.

Keep one owner for each rule or dataset. Do not revive an obsolete generator or duplicate a rule in a verifier merely to make a gate pass. Inspect the actual consumer before changing generated data. Allocate state IDs without collisions. This revision targets fresh saves and does not require compatibility migrations.

Work in this order: question the requirement, delete unnecessary code or artifacts, then simplify what remains. Do not preserve an implementation merely because it already exists, or optimize a process that can be removed. Preserve agreed gameplay behavior while changing its implementation.

## Verification

When a check fails, audit the check before treating the failure as a game defect. Verify its intended contract, current inputs, artifact freshness, and assertion logic. Correct stale or invalid tests while preserving valid invariants, and record the reason for each test or reference change. A passing stale binary is not evidence about current source.

Build and toolchain commands are documented in `docs/VERIFICATION.md` and implemented in the Makefile and CI workflows. After a successful release build, bind both artifacts to their inputs:

```sh
python3 scripts/stamp_release_inputs.py
python3 scripts/verify_emerald_champions_release.py
```

For a demonstrated battle issue, the optional runtime tool can run selected native groups:

```sh
python3 scripts/run_emerald_champions_runtime_gates.py --jobs 4
```

For a Docker-built test ELF, create its stamp inside the same built tree and copy both files together; `--run-only` must reject different source inputs. A content stamp verifies declared input and artifact identity, not that every cached object was rebuilt.

The gameplay pipeline uses automatic battle resolution to test traversal. The battle-testing pipeline disables that automation. Preserve both. A traversal result does not establish combat difficulty; a reported battle win is not independently verified outcome evidence.

Report exactly what was exercised. Static checks, host C regressions, actual battle tests, visual scenarios, and fresh-save traversal are different evidence. Do not turn known failures into success by broadening allowances. Do not claim complete campaign coverage or release readiness without corresponding runtime evidence.

## Shared work

Preserve concurrent edits and check the current file before integrating a patch. Do not overwrite another thread's generated or authored work with an older snapshot. Commit or publish only within the user's requested scope.

## September 7 exploration and encounter direction

Wild distribution changes require a game-wide appraisal of habitat identity, useful early unevolved/fast-evolving options, later discoveries, physical access and Mega-capable evolution timing. Preserve Magnemite's power-station association and the Duskull line's ghost-tower identity. Mega Stones now come from world pickups, meaningful NPC gifts and three one-time Berry Master exchanges (20 garden berries each); the Mega Ring enables use but does not unlock a free stone archive. Keep free battle supplies, the evolution-item archive and accessible Poké Balls. See `docs/REGIONAL_ENCOUNTERS_AND_REWARDS.md` for the current regional pass and source catalogue.

On September 8 the user revised the Daycare plan: babies should be available during the low-cap opening, not locked behind breeding. Munchlax and Mime Jr. belong before Roxanne, Chingling in Rusturf Tunnel, Happiny in the opening meadow roster, and Mantyke on Dewford's Old Rod. Preserve displaced families elsewhere. The Daycare rewards one egg produced by two deposited compatible parents and then hatched with Kangaskhanite (gift eggs, including Togepi, do not qualify), and Audinite becomes an ordinary Route117 NPC gift. Keep short breeding/hatching waits and free stat/move editing. Do not introduce the proposed Hitmon breeding restriction. The user asked for an explanation of current generation rules and Mega Kangaskhan nerfs, not a battle-rules rebalance.

## September 8 test-suite reduction

The user requested an aggressive deletion of tests and gates that distract from development or freeze old design. Keep a small suite protecting current behavior and demonstrated failure modes. Do not restore the retired inherited corpus, historical parity gates, prose locks, strategy quotas or test-count floors. A new test needs distinct observable regression value; update or delete superseded expectations with the gameplay change. Preserve save/artifact integrity and meaningful transaction/battle regressions, without claiming the focused suite certifies the whole game.

The September 8 trainer hierarchy discussion preserves demanding four-Pokémon ordinary doubles and the badge-level floor. Difficulty offsets are the user's global playtesting calibration tool. Trainer-roster hierarchy and stat-point scaling remain discussion-only in the legendary-overhaul task.

## September 8 Cohesion Book implementation

Latest implementation override: old saves do not need compatibility. Build for fresh saves; remove compatibility-only migrations, legacy gift reconciliation, and retired-facility recovery paths. Preserve current-run save integrity, item delivery retries, and normal temporary-party restoration.

The user authorized implementing the completed separate `Cohesion Book` and building the game. Its reviewed source snapshot includes the current uncommitted authoring. The latest instructions supersede the earlier opening exception: choose and retain two starters, use a doubles Birch rescue and a four-member doubles Route103 rival, and keep ordinary wild/legendary captures singles. Medium campaign opponents, including gyms, use at least the live cap minus two. Apply the book's exact authoring and shared mechanics while preserving unrelated work.

The user explicitly requested deleting regression tests and gates as implementation proceeds, direct code without speculative defensive layers, and no new tests unless absolutely necessary. Remove superseded expectations and obsolete blockers with their changes. Keep actual gameplay requirements such as reward delivery and cancellation correct; do not add generic validation frameworks. Build the final integrated game, and report the actual evidence obtained without equating a successful build with a fully played campaign.
