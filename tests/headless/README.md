# Emerald Champions deterministic visual tests

This directory contains the native libmGBA runner restored from Verdant commit
`b9af47310` and the current test-only fixture bridge contract.

## Truth contract

- Production uses `EC_HEADLESS_FIXTURES=0`; no fixture callback or scenario
  data is reachable in the shipped ROM.
- The dedicated fixture build uses `EC_HEADLESS_FIXTURES=1` and exposes only
  named setup/observation words resolved from its current ELF.
- The host runner supplies fixed RTC, timed keys, memory writes/reads, frame
  count, screenshots, video hashes, and scratch save I/O.
- The renderer rejects missing/dataless input and copies the ROM to a private
  scratch directory. It never opens a user save for writing.
- Battle screenshots must set both `gEcHeadlessFixtureSetupResult` and
  `gEcHeadlessFixtureObservedResult`; a plausible stale frame cannot pass.

## Build

The project’s validated Linux GBA toolchain can build the fixture variant with:

```sh
make -j8 BUILD_NAME=emerald-headless MAP_VERSION=emerald \
  EC_HEADLESS_FIXTURES=1
```

The host runner is compiled automatically against the native Homebrew libmGBA
when a render begins.

## Render

```sh
python3 scripts/render_emerald_champions_ui.py all
python3 scripts/render_emerald_champions_ui.py ability-menu
python3 scripts/render_emerald_champions_ui.py wild-action-menu
python3 scripts/render_emerald_champions_ui.py move-details
python3 scripts/render_emerald_champions_ui.py overworld-encounters
python3 scripts/render_emerald_champions_ui.py double-status-ability
python3 scripts/render_emerald_champions_ui.py mega-active
python3 scripts/render_emerald_champions_ui.py opposing-primals
```

Outputs and their hash manifest live under
`work/visual-audit/rendered/current/`. Rendering `all` owns the authoritative
`manifest.json`; a focused render writes `manifest.<scenario>.json` so it can
never erase the complete evidence manifest.

`scripts/verify_emerald_champions_native_ui.py` is the permanent source gate
for this harness. It requires the production-off compile guard, the complete
reviewed scenario inventory, and runtime setup plus observation proof for both
battle UI scenarios. It also checks the executable map collision/elevation
bits beneath the reviewed Tornadus and Landorus placements.

Current scenarios cover both Pokémon Center variants, party/Ability states,
Options, battle vendor, set tutor, legal-move list, Game Corner, Storage,
Naming, Leveler, Circuit presentation, L/R battle UI, and the visible
Thundurus/Tornadus/Landorus encounters. They also cover the Pokédex, Bag,
Frontier Pass, all three visible Summary pages, a selected move description,
and a Summary-to-party return.
High-risk composed states additionally cover the release-style title and Birch
intro, doubles status healthboxes with an idle native Ability popup, Mega-ready
and settled Mega-active states, opposing settled Primals, Safari UI, Pokéblock
condition graph, Storage box popup and Move Items mode, Frontier Pass map, and
the four-star gold Trainer Card. Runtime sentinels are required for battle
states; the Ability fixture stops only after both popup sprites exist and the
main sprite reaches its native idle x-coordinate.
The 372-row Mew preparation list is captured at its first, middle, and final
scroll positions, including the terminal `CANCEL` row; the native gate also
requires its menu count to remain 16-bit.
The final changed-asset census is closed by native Battle Dome opponent-card,
Contest results, Slot Machine, and Sylveon Fairy-type Summary fixtures. Dome
and Contest use coherent facility state and their public dispatch/display
flows; the fixtures do not draw isolated assets or substitute copied pixels.
The `overworld-encounters` group uses one generic test-only ROM scenario and
the reviewed 32-row table in
`include/emerald_champions_headless_overworld_fixtures.h`. Each row loads the
actual authored map and object rather than staging the species on a surrogate
map, and requires both active-object and visible-sprite runtime proof.
Add scenarios by extending the enum and test-only switch in
`src/emerald_champions_headless.c`, then adding a named row to
`scripts/render_emerald_champions_ui.py`.
