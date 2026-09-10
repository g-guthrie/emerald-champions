# Known issues and evidence limits

This is a bounded issue register, not a complete defect inventory. Entries distinguish demonstrated source defects from behavior questions and missing validation. Test totals and build claims belong with the exact artifacts that produced them, not in this rolling document.

## Fresh-save compatibility boundary

The current campaign targets fresh saves. The 5.1 implementation removed legacy
save migration; older wording here about `MigrateEmeraldChampionsCoreState` and
`ResetAmbiguousEmeraldChampionsState` described code that no longer exists.
Current-run failed relic deliveries remain pending in `legendary_signs.c` and
are retried after Pokémon Center healing. This does not reconstruct rewards
lost in an older game revision.

## Behavior that needs a design decision before alteration

Starting a new game initializes Hard even if Easy was selected in title-screen Options beforehand (`NewGameInitData`, [new_game.c](https://github.com/g-guthrie/emerald-champions/blob/main/src/new_game.c)). The existing behavior should be confirmed before changing how new-game preferences are carried forward.

## Validation still required

The September 8 repair replaces the exhaustive synchronous paired search and
passes the focused native tactics/state suite. It is still a bounded forecast:
same-turn manual terrain effects, delayed Wish, drain/recoil and complex
secondary or multi-turn interactions are not fully replayed for every candidate.
The 18-frame warm full-party probe is one position, not a universal latency
ceiling. See [AI repair evidence](development/ai-repair-2026-09-08.md) and the
[trainer audit](TRAINER_STRATEGY_AUDIT.md) before interpreting plan coverage as
proof of optimal play.

Static map and script checks do not prove state-dependent campaign reachability. Their scope is documented in [verify_emerald_champions_progression.py](https://github.com/g-guthrie/emerald-champions/blob/main/scripts/verify_emerald_champions_progression.py) and [verify_map_reachability.py](https://github.com/g-guthrie/emerald-champions/blob/main/scripts/verify_map_reachability.py). Fresh-save traversal, failure/recovery paths, and native UI interactions need runtime evidence.

Selected AI tests and authored encounters must be evaluated against the current source and resulting ROM. A test fixture's invalid setup, a failed behavioral assertion, and a confirmed gameplay defect are different findings. Difficulty, encounter ordering, doubles strategy, and Nuzlocke feasibility are not established by structural gates or a list of intended teams.

Agent-player `battle_success` events are declarations. [aggregate_results.py](https://github.com/g-guthrie/emerald-champions/blob/main/tools/agent_player/aggregate_results.py) labels their outcomes reported; neither Hard mode nor budget exhaustion is inferred from absent evidence. Distinct RNG-delay settings are counted, but statistical independence is not proved. The campaign automation path and real battle-playing path serve different purposes and must retain separate interpretations.

## Active authoring

Trainer and moveset authoring can change the behavior being assessed. Build identity, compiled parties, available preparation, and runtime evidence must refer to the same revision. An earlier passing run does not validate subsequent authored changes. Keep unresolved balance judgments distinct from reproducible engine defects, and record new findings with their trigger and source or runtime evidence.
