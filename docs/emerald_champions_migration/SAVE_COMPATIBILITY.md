# Emerald Champions save compatibility contract

## Deliberate compatibility boundary

The Inclement-derived Emerald Champions ROM at campaign checkpoint
`33202c162ebc34a1dbe2000acd26b0720baa109d` and the modern Champions-engine
ROM are different save families. The migration changes species and item
identity tables, battle data, and campaign state ownership. Loading an old
Inclement-derived save in the new ROM is unsupported and must never be
presented as safe.

The first public modern-engine release establishes the new save baseline.
Subsequent modern builds must preserve native in-game saves unless a future
release explicitly declares and implements a migration.

## Current modern-save additions

This branch does not resize either save block. New state uses previously
unused native event slots:

- `VAR_EMERALD_CHAMPIONS_DIFFICULTY` at `0x40F7`.
- Twelve Legendary Sign bitfield variables: the original eight at `0x40F8`
  through `0x40FF`, plus append-only unlock/caught fields at `0x408B`,
  `0x409D`, `0x40A1`, and `0x40A8`.
- Dedicated Champions Circuit current wins, lifetime wins, and active marker
  at `0x40DB`, `0x40DC`, and `0x40E5`.
- Three visible-encounter hide flags at `0x4ED` through `0x4EF`.
- Two one-time Game Corner reward flags at `0x4EB` and `0x4EC`.
- Poke Vial and Leveler are append-only item identities in the modern item
  table; no existing item identity was renumbered by those additions.

The Circuit intentionally does not use Battle Tower streak or outcome fields.
Its temporary level normalization uses the engine's existing saved-party
buffer and the lobby script does not expose a save-and-quit path mid-run.

## Current evidence

- SaveBlock1, SaveBlock2, SaveBlock3, and aggregate backward-layout tests:
  4 passed, 0 failed.
- Numeric flag and variable assignments are unique.
- The Circuit runtime test proves exact party restoration after a run.
- The modern ROM identity is stable at title `EM CHAMPIONS`, game code `BPEE`,
  maker `01`.

## Remaining deployment gate

Before the modern baseline is called release-ready, automated and emulator
tests must prove:

1. New game, native save, title-screen reload, and power-cycle reload.
2. Save made before a compatible ROM rebuild loads after that rebuild.
3. Difficulty, Legendary Sign, one-time reward, and Circuit lifetime state
   survive reload.
4. Saving outside the Circuit after retirement or defeat retains the original
   party, levels, moves, items, and PC rewards.
5. The web layer keys cloud saves to the stable modern game identity rather
   than the changing ROM hash.

The standalone modern save layout is release-ready. Cross-build browser/cloud
retention remains a deployment concern: the web layer must key saves to this
stable modern game identity rather than a changing ROM hash, and that behavior
must be rechecked whenever the hosted ROM is replaced.
