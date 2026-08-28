# Pre-story-rebuild checkpoint

Date: 2026-08-28

This is the source checkpoint immediately before restoring the 22 missing
Inclement-era map directories and rebuilding the campaign story around them.

## Proven state

- Production ROM builds successfully.
- ROM SHA-256: `4a19d36c58816ceda75957f021df4359e3e3da532fa1072be2f1e3ba46654cf1`
- ROM usage: 26,915,808 bytes (80.22 percent of 32 MiB).
- EWRAM: 227,056 bytes (86.61 percent).
- IWRAM: 28,376 bytes (86.60 percent).
- Canonical campaign master: 509 physical encounters, 557 trainer branches,
  84.92 percent doubles.
- All 557 trainer parties match the master.
- All 543 trainer IDs native to the modern Hoenn source are implemented.
- Fourteen disabled rematch slots hold selected one-time bespoke parties; six
  are wired on retained maps and eight remain explicitly planned.
- The live Champions Circuit uses the pinned Showdown Champions generator:
  311 variants and 444 role templates.
- All 53 Legendary Sign definitions and all 1,309 competitive presets pass
  their static verification gates.

## Deliberately not claimed

- The 22-map restoration and story rewrite have not started.
- Eight selected bespoke encounters are not yet reachable.
- Expanded generation-wide starter selection is not yet restored.
- Full save/reload, all-badge progression, Frontier-facility, and complete
  playthrough emulator gates remain open.
