# Continue Emerald Champions development

## Published snapshot: Emerald Champions 6.2

See [6.2 release notes](RELEASE_6.2.md). The checkpoint descriptions below are historical; the v6.2 source snapshot includes the current EV migration and Birch rescue fixes.

The repository root is the live game source. Start with `AGENTS.md`. The user
requested a repair audit after reporting opening, AI and performance defects in
5.1, and rejected the [Cohesion Book](../Cohesion%20Book/README.md) as binding design
authority. Its inventories and `baseline/source` are historical evidence only.
The repair has its own build and runtime evidence below; historical 5.1 results
describe the previous checkpoint, not the repairs.

## Current local checkpoint: wild-area follow-up

`pokeemerald-wild-polish-20260908-release.gba` includes the mature Route123 orchard and optional main-Woods
Rock Smash Crustle at 25%. First-visit grass Heracross 11%, Pineco 8% and
Ferroseed 5% remain unchanged, as do the other stronger early choices and
all three Dive habitats. This follow-up changes 11 species slots in two tables.
Route123 now leads with Hydrapple 13% and Arboliva 12%; see
[WILD_AREA_POLISH_2026_09_08.md](WILD_AREA_POLISH_2026_09_08.md) for all final
chances and the four basic forms now obtained through breeding.
SHA-256: `1a4a366695dc6eefea9d92c232796ee632479867fff9f1b9836168fb473d56d5`.
The final rebuild used a stable set of 28,977 inputs. All release checks
passed, and all 256 default method tables / 2,361 encounter slots across 138
areas match the production ROM. Campaign balance remains to be playtested.
Use a fresh save. Nothing was committed, pushed or uploaded.

## Previous local checkpoint: approved wild-area review

`pokeemerald-wild-review-20260908-release.gba` applies all 108 recommendations from the individual area review,
with the user's later correction: first Petalburg Woods grass contains
Heracross 11%, Pineco 8% and Ferroseed 5%, without a Rock Smash gate.
Route101 now has Mienfoo 13%, Bonsly 12% and Eevee 8%; Route102 retains
Timburr 11% and Pachirisu 8%. All 30 areas rated 10 retain their full tables,
including the three Dive habitats and powerful early meadow discoveries.
This pass changes 469 species slots across 135 native tables, preserving
encounter levels, frequencies, weights, map access, stats, trainers and caps.
SHA-256: `a68d1f296356090ae181fbaf172ccf79b638ee4d7548e108e46aa0d5a3461854`.
The adjacent ELF/input stamp bind 28,977 inputs, unchanged during compilation.
All release checks passed; all 256 default native tables / 2,361 slots across
138 areas match the production ROM. Capture traversal and campaign difficulty
were not playtested in this pass. See [WILD_AREA_REVISION_2026_09_08.md](WILD_AREA_REVISION_2026_09_08.md)
for every final population and the retained evolutionary-family access.
Use a fresh save. Nothing was committed, pushed or uploaded.

## Previous local checkpoint: September 8 habitat revision

`pokeemerald-habitats-20260908-release.gba` includes the repair below plus the
user-approved habitat changes and stronger opening choices: Mienfoo on Route101,
Timburr and Pachirisu on Route102. Early babies, Pheromosa, Kartana and Dive
rosters are preserved. Trainer difficulty and species stats are unchanged.
SHA-256: `55a1471823240783ddd36e1c97afdb2571007da88ad95932ba33b1f75ef9296b`.
The adjacent ELF/input stamp bind 28,977 current inputs; release checks pass.
All 50 changed method tables match the final ROM bytes. New battle balance and
capture traversal were not playtested. See
[WILD_HABITAT_REVISION_2026_09_08.md](WILD_HABITAT_REVISION_2026_09_08.md).
Use a fresh save. Nothing was committed, pushed or uploaded.

## Previous local checkpoint: September 8 repair candidate

`pokeemerald-repair-20260908-release.gba` is the clean integrated local repair
build. SHA-256: `226bd3e025e096254f710c42dec339a00eb6d744bfc8b5b3cdbe12022513ab7c`.
Its adjacent ELF and input stamp bind its original 28,977 build inputs. Its release checks passed.
It has not been committed, pushed or uploaded; the original 5.1 ROM is preserved.
Use a fresh save.

The repair fixes opening levels/choreography/preparation interruption, optional
nurse healing and rescue Retry, replaces the expensive synchronous paired AI,
and adds executable trainer plans with targeted Maura, Parker, Connie,
Kate & Joy and Quincy improvements. The focused native suite passes 20/20
declarations plus 16 weather/terrain opener parameters. Final production UI
checks cover starter selection, level-3 opponents, animation, Retry and natural
loss recovery; a final headless native scenario covers both nurse choices.

Read [GAME_REPAIR_AUDIT.md](GAME_REPAIR_AUDIT.md),
[TRAINER_STRATEGY_AUDIT.md](TRAINER_STRATEGY_AUDIT.md), and
[WILD_DISTRIBUTION_AUDIT.md](WILD_DISTRIBUTION_AUDIT.md). Selected matchups and
static inventories do not certify the whole campaign. Same-turn manual terrain
and several delayed/secondary effects remain approximate; Quincy Gas-exit
eligibility has source review but no committed multi-turn encounter proof.
Next meaningful work is player-facing campaign/matchup playtesting, not restoring
the Cohesion Book as authority or restarting a full generic AI rewrite.

## Historical published checkpoint: 5.1

`pokeemerald-release.gba` is the latest Emerald Champions 5.1 ROM. SHA-256: `900a71942944e9ef07016f3f5dc55b83a8bffce8f5d38a545be174b38268a9d7`. The adjacent input stamp records its original compiled source digest and artifact identities. The ELF is a generated build artifact; rebuild it locally. The same ROM is on [Google Drive](https://drive.google.com/file/d/1eGbIyPYQV4Q0pi6XbWOShrZAZV1-Q2AH/view).

The book's historical wording about old-save migration is superseded by the user's explicit instruction: **fresh saves only; no old-save compatibility**. The major opening, trainer, world, preparation, forms and shared-AI changes are implemented. This is not a claim that every book acceptance case or the complete campaign has been verified.

## Build from a checkout

Clone this repository and run commands from its root. Use a complete ARM GCC/binutils/newlib toolchain; a bare compiler without `string.h`/newlib is insufficient. CI provisioning is in [.github/workflows/build.yml](../.github/workflows/build.yml); general guidance is in [VERIFICATION.md](VERIFICATION.md).

```sh
git clone https://github.com/g-guthrie/emerald-champions.git
cd emerald-champions
make -j4 release DEVKITARM=/absolute/path/to/arm-toolchain
python3 scripts/stamp_release_inputs.py
```

On the original Mac, the complete tested ARM15 toolchain is `/Users/gguthrie/.local/share/arm-gnu-toolchain-15.2-20260718/Payload`. Generated tools, objects, assets, ROMs and emulator state are not prerequisites to bring from another working directory. The Makefile generates its dependencies. Native compilers and the tools listed in CI must be installed.

## Evidence and next work

- Production build and ROM header checks passed. Seven focused native declarations passed: six tactical/state-restoration checks covering13 parameter scenarios, plus a temporary full-party timing probe. Profiling code was removed before the final ROM.
- The demanding full-party probe improved from1803 frames originally, to1428 in the first uploaded checkpoint, to515 (~8.6 seconds) in5.1. A warm explicit decision took374frames. These are instrumented native test measurements, not a universal production latency guarantee. Worst-case AI latency still warrants work.
- An earlier pinned intermediate ROM was traversed from blank flash using normal inputs: dual selection/cancel, preparation, doubles rescue, natural loss/restoration, retry win, both nicknames, Dex and20Balls. Full campaign, all facilities/services, multiplayer and every tactical acceptance case remain unverified.
- A broader retained-file test run passed59 declarations and failed3: After You/Trick Room2/2, weather-for-ally6/10, terrain-for-ally6/8 in `test/battle/ai/ai_doubles.c`. Preliminary inspection found over-specific setup expectations; final adjudication is unfinished. Do not weaken assertions merely to pass them.
- Continue performance work in `src/battle_ai_pair.c`, `src/battle_ai_util.c`, and `src/battle_ai_switch.c`. Current code preserves every legal current-turn move/target, uses native forecasts for the small future reserve-Mega preference, avoids redundant entry-cache fills, caches unchanged speed/reserve metadata, and skips irrelevant residual calculations.

Local `work/` contains large disposable experiments, logs, screenshots and emulator fixtures. It is not required to build or continue source development. Durable status is recorded here; the full specification and its evidence are in the book.

## Retained debugging material

The [manual timing probe](development/cohesion_ai_timing.c), [tactical fixture contracts](development/ai-native-validation.md), and [focused native output](development/ai-check-results.txt) are retained for continuing AI work. The probe is deliberately outside the automatically discovered test tree. To run it, copy it temporarily into `test/`, select that source explicitly with `TEST_SOURCE_ALLOWLIST`, and use the `EC expert pair: timing probe` prefix; remove the temporary copy afterward.

The optional normal-key opening driver and input trace are tracked at `work/runtime-opening/replay_final_opening.py` and `work/runtime-opening/intermediate-build3/normal-key-trace.json`. Build the existing headless runner before use; see `work/runtime-opening/runtime-plan.md`. The trace belongs to the intermediate artifact documented there, so replaying it on changed timing or game logic requires visual review, not automatic success claims.
