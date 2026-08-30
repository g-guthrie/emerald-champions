# Fresh engine, UI, and CI second pass

Date: 2026-08-29
Branch inspected: `release/full-source-audit`
Starting HEAD: `709b73fbb` (`Force instant text and Set-style battles`)

## Verdict

The engine/UI/CI slice is materially stronger and its focused executable suite is green with zero accepted test debt. After the integration-owned save fixture was added, the final curated manifest contains 19 filters, 43 source files, and a minimum of 256 live tests. The frozen one-shot rerun covers all 256 tests with 256 passes, zero failures, zero assumption failures, zero known-failing tests, and zero TODO tests.

This is not a release-ready or playtested claim. No production ROM was built in this pass. The save-family migration was escalated as a P0 integration boundary after historical source review showed that `FLAG_EC_BESPOKE_TRAINER_FLAGS_MIGRATED` alone is not a safe discriminator; integration owns that migration and its final rerun. The whole runtime manifest must be rerun once that source freezes.

## Scope

Audited directly:

- battle-engine state, form behavior, switch ordering, and doubles AI;
- Champions Circuit entry, counter, restoration, reward, and retry transactions;
- prepared capture/gift transactions and held-item restoration;
- difficulty, level-cap, instant-text, Set-style, and no-Bag engine contracts;
- native role/Ability menu widths and title-screen source branding;
- Poke Vial, Leveler, battle-role service, and retired EV-special wiring;
- static release gates, runtime source selection, test-link memory layout, and GitHub workflows.

Excluded as design/content review:

- trainer party design and campaign battle authorship;
- story prose, map narrative, and world-routing quality;
- wild-species distribution design.

Those excluded domains remain in the release verifier because they are required integration inputs. They were not re-judged here.

## Findings and fixes

### 1. Curated battle-test CI was giving false assurance

The retained curated ELF contained only 34 `*Champions` function tests, despite a higher manifest minimum. Rebuilding from the actual allowlist exposed a second problem: every battle test crashed while function tests passed.

The crash was linkage-dependent, not a battle-engine regression. Two local-static `struct BattleStruct` fixtures in `test/emerald_champions.c` consumed `0x950` bytes of IWRAM and reduced the GBA/test IRQ stack below a safe margin. The same independent priority-order battle passed in a minimal link, crashed when `test/emerald_champions.c` was added, and passed again after the fixtures moved to one shared EWRAM buffer.

Fixes:

- moved the large shared test fixture to EWRAM;
- added a linker assertion requiring 4 KiB of test IWRAM stack headroom;
- removed `test/test_test_runner.c` from unconditional support sources because it is an intentional runner self-test corpus, not infrastructure;
- retained only the three real runner support sources;
- required source selection to resolve every runtime filter and fail if coverage falls below its live minimum.

Measured proof:

- support + priority battle: pass;
- support + priority battle + `test/emerald_champions.c` before fix: crash;
- the identical combined link after fix: pass in 0.35 seconds;
- corrected curated ELF: 43 selected sources from 968 total C test sources.

### 2. Instruct AI did not model action order or legal repeats correctly

The former Instruct TODO was converted into an executable preference test. The first implementation either chose ordinary Psychic or targeted a foe because partner validity and move timing were not modeled consistently.

The generic AI correction now:

- distinguishes an ally that will act before Instruct from a prior-turn last move;
- rejects no move, unavailable/banned moves, recharge, Z/Max, Dynamax, two-turn, Bide, Sky Drop, multi-turn, disabled, unknown, and zero-PP repeats;
- validates Instruct even when the selected target is an ally;
- values legal support repeats, single-target damage repeats, and spread-damage repeats, with spread damage receiving the strongest preference.

Runtime proof:

- Oranguru prefers Instructing allied Torkoal's Heat Wave over its still-legal Psychic;
- Oranguru avoids Instruct when the faster ally has no last move;
- Oranguru avoids Instruct when the ally's pending move is Instruct-banned;
- all 65 tests in `test/battle/ai/ai_doubles.c` pass with zero debt.

### 3. Beat Up + Justified was assigned the wrong AI flag at the integration boundary

An exact Cottonee/Terrakion fixture proved that `AI_FLAG_ATTACKS_PARTNER` makes the AI treat its ally as an enemy and suppresses the existing support-combo logic. This is the flag's intended generic meaning, so changing engine semantics would have been wrong.

The issue was handed to trainer generation. The generator rule was removed, all three affected flags disappeared from materialized trainers, and a static regression now rejects `AI_FLAG_ATTACKS_PARTNER` on Beat Up + Justified teams. The existing no-flag Justified runtime test passes.

### 4. Forecast behavior had been inverted

Castform had been changed to retain its form after Forecast was suppressed or lost. Current upstream behavior and tests require Castform to return to normal form. The Castform-only early return was removed; the separate Cherrim-Dynamax exception remains.

Proof: all 18 Forecast tests and all 12 Flower Gift tests pass.

Primary references: [upstream Forecast tests](https://raw.githubusercontent.com/rh-hideout/pokeemerald-expansion/master/test/battle/ability/forecast.c), [upstream form-change source](https://raw.githubusercontent.com/rh-hideout/pokeemerald-expansion/master/src/battle_util.c), and [upstream Flower Gift tests](https://raw.githubusercontent.com/rh-hideout/pokeemerald-expansion/master/test/battle/ability/flower_gift.c).

### 5. Doubles ordering and defensive-move debt were closed

- simultaneous manual switches now have four executable ordering tests, including Trick Room reversal and immunity to move-order effects such as Custap, Stall, and Lagging Tail;
- Quick Guard and Wide Guard TODOs were replaced with forward/reverse AI-order tests;
- post-KO switch-in Ability order and spread-target failure order remain explicitly gated.

### 6. Circuit rewards could become unreachable after a full-PC win

Reward entitlement now uses lifetime Circuit wins rather than transient current-run wins. The lobby desk retries pending rewards, including after a full-PC result, without requiring another win.

Runtime coverage proves:

- entry rejects incomplete, fainted, and Egg parties and accepts six healthy Pokémon;
- win/loss transitions preserve counters and restore the exact party;
- rewards go to PC when possible;
- a full-PC reward remains claimable at the same lifetime-win count;
- mastery waits for every finite Circuit reward.

### 7. Retired EV/Effort Ribbon specials were orphaned live symbols

Removed the unused `LeadMonHasEffortRibbon`, `GiveLeadMonEffortRibbon`, and `Special_AreLeadMonEVsMaxedOut` special registrations and implementations. Debug EV inspection helpers and the native Effort Ribbon model remain because they still have live callers. A core static invariant prevents the retired specials from returning.

### 8. UI and branding invariants were too implicit

- runtime width checks now use the native font measurement functions for every role and Ability chooser label;
- the role chooser is statically required to derive its dynamic count, cap rows at four, clamp window width, and clamp cursor state;
- `gTitleScreenEmeraldVersionGfx` and `gTitleScreenEmeraldVersionPal` are required to source `graphics/title_screen/emerald_champions_version.png` exclusively;
- the Game Corner verifier now derives the `*Champions` minimum from live declarations instead of hard-coding a stale count.

Visual navigation, controller input, and full screen composition still require emulator/human checks.

### 9. Save migration is an integration-owned P0 boundary

Historical review showed that old Zygarde-era saves can already have flag `0x4C5`; therefore that flag cannot by itself identify a compatible modern save. A safe solution needs a new append-only save-layout version discriminator, exact modern-save preservation, full old-prefix Sign migration, physical hide-flag reconstruction, and fail-safe cleanup for ambiguous saves.

This audit does not sign off a partial `0x4F9` difficulty recovery or a `0x4C5` early return. Integration owns the implementation and final runtime rerun.

## Coverage matrix

| Domain | Source/static evidence | Runtime evidence | Remaining human-only risk |
|---|---|---|---|
| Battle standard | Champions generation, Set style, no-Bag trainer rule, held-item restoration | 90 `*Champions` tests | Full campaign balance and every battle transition |
| Abilities/forms | Forecast/Flower Gift source parity; Mega-only selection | Forecast 18, Flower Gift 12, Mega end 2, Commander 42 | Rare multi-turn/form combinations outside filters |
| Doubles ordering | manual-switch and post-KO order source | manual switch 4, post-KO 5, spread failures 2 | Long battles with switches, revives, and Trick Room changes |
| Doubles AI | target/move legality, cache reset, Instruct validity/value | AI doubles 65 plus six focused AI/Imposter filters | Campaign-wide tactical quality and random seeds |
| Circuit | generated-template, Ability, variant, reward, and lobby invariants | entry/counter/reward/restore/mastery tests inside `*Champions` | Long streaks, retirement UX, many random team seeds |
| Capture/gifts | transactional party/PC/no-room and item-baseline contracts | prepared capture, Game Corner, story-gift tests | Real catch-swap controller path and evolution prompts |
| Difficulty/caps | Hard/Medium/Easy, strict cap, instant text, Set style | focused function tests | Save migration final integration and real reload |
| Service UI | role/Ability widths, dynamic count, bounded window, Poke Vial exact mutation | native width and service function tests | Menu navigation, Poke Vial charge UX, Leveler multi-evolution flow |
| Title/identity | exclusive Champions title asset and ROM identity configuration | none in this pass | Rendered title screen and final ROM header |
| Save layout | four upstream save-layout tests | `test/save.c`: 4 pass | cross-build, power-cycle, browser/cloud retention; migration P0 |
| CI harness | filter-to-source resolution, count/debt checks, IWRAM assertion | 256 focused tests in the frozen one-shot run | production CI and human campaign evidence |

## Static release-gate decisions

All 25 gates should remain. Some overlap deliberately: producer freshness, generated consumer state, and runtime-facing contracts fail for different reasons.

| Gate | Decision | Reason |
|---|---|---|
| core services | Keep | Central engine, UI, item, difficulty, title, and retired-symbol invariants |
| finite rewards | Keep | Canonical finite-reward rewrite/freshness contract |
| reward economy | Keep | Prevents duplicate, missing, or progression-breaking reward state |
| wild distribution | Keep | Required integration input even though distribution design was excluded here |
| route signs | Keep | Generated routing/source freshness |
| competitive presets | Keep | Legal materialization and authored preset contract |
| species stat rebalances | Keep | Detects silent species-data drift |
| upstream critical fixes | Keep | Guards selected engine repairs and inaccessible unsafe features |
| campaign roster | Keep | Required campaign consumer coverage |
| Game Corner starter archive | Keep | Transactionality, flags, menus, presets, and live runtime count |
| trainer Ability legality | Keep | Detects impossible authored Abilities |
| trainer runtime coherence | Keep | Cross-checks flags, sets, and engine-consumable trainer state |
| story and dialogue | Keep | Required source/story integration, not play proof |
| rematch-free Match Call | Keep | Protects the deliberately removed rematch model |
| whole-campaign progression graph | Keep | Detects unreachable or circular progression |
| legendary availability | Keep | Ensures every finite legendary source is reachable in data |
| legendary signs and Circuit | Keep | Generated hashes, reward reachability, and Circuit transaction invariants |
| regional starters | Keep | Starter-source and acquisition completeness |
| restored world | Keep | Map/object/script materialization contract |
| single-player evolutions | Keep | Ensures evolution accessibility without link services |
| fossil revival | Keep | Transactional fossil delivery and retry behavior |
| Poke Vial quest | Keep | Quest wiring, exact healing mutation, and charge semantics |
| campaign battle master | Keep | Canonical battle schema and legality source |
| campaign evidence freshness | Keep | Prevents published evidence from lagging canonical inputs |
| battle script formats | Keep | Ensures authored format and script materialization agree |

## Runtime manifest

The final manifest accepts no known-failing or TODO debt.

| Filter | Minimum / observed | Elapsed evidence |
|---|---:|---:|
| `*Champions` | 90 / 90 | 6.91 s |
| item descriptions | 1 / 1 | 0.32 s |
| Daycare inheritance | 1 / 1 | 0.20 s |
| upstream critical fixes | 4 / 4 | 0.33 s |
| Commander | 42 / 42 | 7.35 s |
| Forecast | 18 / 18 | 4.71 s |
| Flower Gift | 12 / 12 | 2.17 s |
| Mega end-of-battle reversion | 2 / 2 | 0.36 s |
| simultaneous manual switches | 4 / 4 | 0.84 s |
| post-KO switch-in Ability order | 5 / 5 | 4.93 s |
| spread Earthquake failure order | 2 / 2 | 0.37 s |
| smart-switch move-data isolation | 1 / 1 | 0.31 s |
| smart-switch recalculation reset | 1 / 1 | 0.26 s |
| Imposter copied move slot/target | 1 / 1 | 0.26 s |
| Imposter copied Spore target | 1 / 1 | 0.27 s |
| Sleep Clause after transformation | 1 / 1 | 0.63 s |
| Billy Imposter lead target | 1 / 1 | 0.31 s |
| save layout | 4 / 4 | 0.18 s |
| doubles AI corpus | 65 / 65 | 59.87 s |
| **Total** | **256 / 256** | **84.58 s** |

The final timing is the one-shot 19-filter invocation after the save migration froze; orchestration wall time was 85.10 seconds.

## Workflow decisions

| Workflow | Decision | Truth boundary |
|---|---|---|
| `.github/workflows/build.yml` | Keep and require `build` in branch protection | Fresh release build, 25 static gates, ROM checks, smoke boot, and curated runtime suite |
| `.github/workflows/docs.yml` | Keep non-release | Builds/deploys docs only when Pages is enabled; cannot imply game readiness |
| `.github/workflows/full-test-compile.yml` | Keep weekly/manual | Compiles all 968 test C sources; valuable because required CI intentionally runs a 43-source focused suite |

Branch protection is repository-host configuration and must be confirmed by a maintainer; source comments cannot enable it.

## Commands and evidence

Passed in this pass:

```text
python3 scripts/verify_emerald_champions_core.py
python3 scripts/verify_game_corner_starter_archive.py
python3 scripts/verify_legendary_signs_and_circuit.py
python3 -m py_compile <changed verification/runtime scripts>
git diff --check <engine/UI/CI-owned files>
docker ... python3 scripts/run_emerald_champions_runtime_gates.py --build-only --jobs 12
  curated_test_sources=43 full_test_sources=968
  final incremental build=22.10 s (subsequent test-only relink=18.47 s)
focused independent priority battle after IWRAM fix: 1/1 pass, 0.35 s
focused Instruct positive/negative tests: 3/3 pass
test/battle/ai/ai_doubles.c: 65/65 pass, 59.87 s
```

A full 968-source ELF compiled successfully earlier during diagnosis, but before the final Instruct and migration changes. It is diagnostic evidence, not final-candidate proof; the weekly workflow should rebuild it from the frozen source.

## Human-only release risks

- complete new-game-to-postgame playthrough with encounter-by-encounter pacing notes;
- native Battle Frontier entry, retirement, defeat, save/reload, and long Circuit streaks;
- controller/touch navigation for role, Ability, Poke Vial, Leveler, and catch-swap menus;
- multi-evolution Leveler flows and cancellation/learn-move prompts;
- real full-party/full-PC capture and gift UX;
- save-family migration fixtures, title-screen reload, power-cycle reload, and cross-build retention;
- browser/cloud save key stability;
- rendered title branding and final clean production ROM build/header/smoke;
- long randomized AI/Circuit seed campaigns beyond the focused deterministic corpus.

## Release handoff

Before any release-ready claim:

1. integration must close the save-layout-version migration with historical fixtures;
2. rebuild from the frozen source;
3. run all 25 static gates, all 19 runtime filters in one invocation, and the full-test compile workflow;
4. perform the human scenarios above;
5. verify required branch protection and remote CI status.
