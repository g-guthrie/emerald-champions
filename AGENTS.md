# Working on Emerald Champions

Emerald Champions is a hard, doubles-focused Emerald campaign built on
pokeemerald-expansion. Its core loop removes preparation grind so difficulty
comes from team construction, battle decisions, exploration, and authored
encounters.

## Authority

Executable source and runtime behavior are ground truth. Documents describe
intent or project source into a readable form; they never prove that a feature
works, is balanced, or is fun.

Read these first for factual questions:

- Trainer loadouts and AI flags: `src/data/trainers.party`
- Wild tables: `src/data/wild_encounters.json`
- Species data: `src/data/pokemon/species_info/`
- Moves, Abilities, items, and battle callbacks: `src/data/`, `src/battle_*`,
  and `include/config/`
- Story, NPCs, rewards, and event state: `data/maps/*/scripts.inc`, map JSON,
  and `data/scripts/`
- Frontier generation: the actual Frontier C sources and generated tables
- Build/test behavior: `Makefile`, `.github/workflows/`, `scripts/verify_*`,
  and `test/`

Two files under `docs/` are load-bearing inputs and must stay synchronized
with compiled source:

- `docs/emerald_champions_master_battle_design.txt`
- `docs/emerald_champions_battle_sets.json`

Use their rationale and schemas, but verify every material claim against the
compiled data. `difficulty_observed: UNPLAYED` is not playtest evidence.

## Working rules

- Make minimal, explicit changes. Preserve save IDs and map object IDs unless
  a migration is deliberately designed and tested.
- Do not regenerate data with an old allocator or snapshot merely because a
  generator exists. Confirm that the generator still expresses the accepted
  design first.
- A static gate proves only its named source invariant. It does not prove a
  complete campaign, rendered presentation, difficulty, pacing, or fun.
- Treat warnings, TODOs, known failures, fallbacks, and recovery paths as
  evidence to investigate. Never hide a strategy-changing fallback behind a
  release assertion.
- Reconcile trainer changes in both the load-bearing battle design and
  `src/data/trainers.party`; the ROM consumes the latter.
- Keep ordinary held items non-scarce, berries and special transformation
  items progression-aware, starters out of ordinary wild grass, and fossils
  behind revival unless a new design explicitly replaces those rules.
- Do not call the game release-ready until a fresh build, curated runtime
  suite, scenario saves, visual checks, and a fresh-save campaign playthrough
  support that claim.

## Verification

Run the source/static suite with:

```sh
python3 scripts/verify_emerald_champions_release.py
```

That command also requires fresh release artifacts and a content stamp.
Release ROMs are compiled in the Docker builder from a copied tree, so after
`make release` run, inside that same tree:

```sh
python3 scripts/stamp_release_inputs.py
```

and copy `pokeemerald-release.inputs.json` out beside the ROM. The verifier
compares that stamp to the host tree by content; modification times alone
cannot prove the ROM was built from the current sources. The curated runtime
suite is:

```sh
python3 scripts/run_emerald_champions_runtime_gates.py --jobs "$(sysctl -n hw.ncpu)"
```

The local macOS checkout has a native `mgba-rom-test-mac`, but a complete GBA
cross-toolchain may still require Linux/CI. The weekly full-test workflow
compiles the entire upstream test corpus; required CI runs the smaller,
identity-checked Emerald Champions runtime manifest.

## Git and release discipline

Work on a coherent branch, commit only reviewed files, push, and confirm the
remote head. Do not merge `origin/main` wholesale when it diverges: inspect
each commit because an earlier main commit deleted useful documentation and
carried stale defect counts. Required CI is not enforced until a branch rule
requires the stable `build` job.
