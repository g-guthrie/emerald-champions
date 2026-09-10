# Emerald Champions 7.0

Built September 10, 2026, from the current integrated project using a fresh release object directory.

Open `Emerald Champions 7.0.gba` in a GBA emulator. This campaign revision targets a fresh save. Keep previous saves backed up separately.

The ROM includes the current campaign, dialogue, legendary encounter, economy, preparation, and trainer changes present at this checkpoint. No new gameplay edits were made during packaging.

Verification: full release compilation succeeded; build inputs were unchanged during compilation; all required release gates passed; the actual release ROM booted to its title screen in mGBA without fixture writes. The full verification output is included. These checks do not certify every gameplay path: the segment-by-segment full-game audit is paused at the user's request, with remaining coverage recorded in `Cohesion Book/review/2026-09-10-full-game-audit-progress.md`.

Build command: `make -j4 RELEASE=1 BUILD_NAME=champions-7.0-20260910 FILE_NAME=pokeemerald-champions-7.0-release` using the bundled ARM toolchain.

The root `pokeemerald-release.gba` is an identical copy of this ROM. Root ROM/ELF input stamps record artifact and build-input hashes.
