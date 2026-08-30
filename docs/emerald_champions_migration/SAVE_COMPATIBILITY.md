# Emerald Champions save compatibility contract

## Deliberate compatibility boundary

Arbitrary Inclement-derived Emerald Champions saves and the modern
Champions-engine ROM are different save families. The one historical exception
is the exact executable state contract at commit
`81e288b51995c59c1dbc640f77907b8120788bc9`; the runtime contains a bounded,
one-time migration for that contract. Earlier, modified, or otherwise
ambiguous save families are not inferred. They fail safe to a playable Hard
state with colliding current progression cleared. Exact historical source IDs
remain recorded in `SOURCE_LOCK.md`; no other hash is a compatibility claim.

The first public modern-engine release establishes the new save baseline.
Subsequent modern builds must preserve native in-game saves unless a future
release explicitly declares and implements a migration.

`VAR_EMERALD_CHAMPIONS_SAVE_VERSION` (`0x40B8`) is the authoritative current
save discriminator. That variable is proven unused in the 81e checkpoint, the
modern `e7df953860be1a250f6720fa4579aebe30436a04` baseline, and the source that
introduced this migration. New games stamp version 1 before gameplay begins.
Once version 1 is present, loading is idempotent and does not rewrite state.

Unversioned saves are classified before the reused `0x4C5` flag is trusted:

- Raw 81e saves carry all three old migration markers: system flags `0x91E`
  and `0x91F`, plus `0x4F9`. The last is now Shaymin's physical caught flag.
  They are migrated only when the current `0x4C5` marker is clear.
- Native e7-and-later modern saves carry
  `FLAG_EC_BESPOKE_TRAINER_FLAGS_MIGRATED` (`0x4C5`) and neither old system
  marker. Their current difficulty, Sign, Circuit, reward, physical-object,
  and Shaymin state is preserved exactly; only the version variable is
  stamped.
- All old markers together with `0x4C5` are intrinsically ambiguous. That byte
  pattern can be either a raw 81e save that defeated Zygarde or an 81e save
  later written by e7, because e7 had no additional layout-version bit. No
  difficulty or Sign word can distinguish those histories reliably. The
  migration therefore fails that overlap safe instead of guessing: current
  colliding progress is cleared to a playable Hard state while party, story,
  badges, inventory, and ordinary campaign progress remain intact.
- Any other partial or contradictory signature uses the same fail-safe path.

The bounded raw-81e migration snapshots every overlapping field before
writing:

- old unlocked Sign words `0x40F7`-`0x40FA` move to current
  `0x40F8`-`0x40FB`;
- old caught Sign words `0x40FB`-`0x40FE` move to current
  `0x40FC`-`0x40FF`;
- only the stable IDs 0-52, Arceus through Zygarde, are retained; the unused
  tail and all appended Sign words are cleared;
- old Circuit lifetime wins at `0x40FF` move to `0x40DC`, while current-run and
  active state reset to zero; and
- old Hard/Medium/Easy values 0/1/2 in `optionsTextSpeed` become current
  Hard/Normal/Easy without changing the modern text-speed policy.

Because 81e reused many of the same bits for unrelated content, its migration
re-enables the complete audited pickup ranges `0x20`-`0x49`, `0x1F4`-`0x2BB`,
and `0x3E9`-`0x492`; clears the current claim/object range `0x4A2`-`0x4EC` and
the changed direct-reward claims; then reconstructs every physical object in
the stable 0-52 Sign prefix from the migrated caught/unlocked bits. This keeps
old Sign and Circuit accomplishments while preventing an old legendary,
pickup, or quest flag from hiding a different current reward or one-off.

## Current modern-save additions

This branch does not resize either save block. New state uses previously
unused native event slots:

- `VAR_EMERALD_CHAMPIONS_SAVE_VERSION` at `0x40B8` (current value 1).
- `VAR_EMERALD_CHAMPIONS_DIFFICULTY` at `0x40F7`.
- Twelve Legendary Sign bitfield variables: the original eight at `0x40F8`
  through `0x40FF`, plus append-only unlock/caught fields at `0x408B`,
  `0x409D`, `0x40A1`, and `0x40A8`.
- Dedicated Champions Circuit current wins, lifetime wins, and active marker
  at `0x40DB`, `0x40DC`, and `0x40E5`.
- Dedicated physical one-off state occupies audited persistent slots. Current
  additions include catch flags at `0x4B6` through `0x4C4`, `0x4D7` through
  `0x4D8`, and `0x4E0` through `0x4E9`, plus the three visible-chain hide flags
  at `0x4ED` through `0x4EF`. Some of the 32 physical one-offs retain native
  event flags rather than consuming a new Emerald Champions slot.
- Two one-time Game Corner reward flags at `0x4EB` and `0x4EC`.
- Twenty-seven one-time Starter Archive flags use persistent slots at
  `0x4A2`–`0x4B0`, `0x4B4`–`0x4B5`, `0x4C7`–`0x4C9`, and `0x4D0`–`0x4D6`.
  Those slots were unused in the modern e7 baseline but live in 81e, so the
  explicit 81e migration clears them before claims begin. The originally
  selected starter is rejected using its existing persistent selection vars.
- Poke Vial and Leveler are append-only item identities in the modern item
  table; no existing item identity was renumbered by those additions.

The Circuit intentionally does not use Battle Tower streak or outcome fields.
Its temporary level normalization uses the engine's existing saved-party
buffer and the lobby script does not expose a save-and-quit path mid-run.

## Current source and runtime contracts

- The curated runtime manifest requires all four SaveBlock1, SaveBlock2,
  SaveBlock3, and aggregate backward-layout tests. It accepts no known-failing
  or TODO debt.
- Numeric flag and variable assignments are checked for uniqueness by the
  release verifier.
- Authored Champions tests cover Circuit counter transitions and exact party
  restoration after a run; they must be rerun for the exact candidate.
- The configured modern build identity is title `EM CHAMPIONS`, game code
  `BPEE`, maker `01`; a fresh ROM header check must confirm the artifact.

## Remaining deployment gate

Before a modern build can support a release-ready claim, automated and emulator
tests must prove:

1. New game, native save, title-screen reload, and power-cycle reload.
2. Save made before a compatible ROM rebuild loads after that rebuild.
3. Difficulty, Legendary Sign, one-time reward, and Circuit lifetime state
   survive reload.
4. Saving outside the Circuit after retirement or defeat retains the original
   party, levels, moves, items, and PC rewards.
5. The web layer keys cloud saves to the stable modern game identity rather
   than the changing ROM hash.

The source defines a modern save-layout contract, but it is not by itself
release-readiness proof. Cross-build browser/cloud retention remains a separate
deployment concern: the web layer must key saves to the stable modern game
identity rather than a changing ROM hash, and that behavior must be rechecked
whenever the hosted ROM is replaced. Power-cycle behavior and retention should
also be exercised on every intended emulator/device family.
