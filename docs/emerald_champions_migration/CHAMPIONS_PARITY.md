# Pokemon Champions battle parity checkpoint

This checkpoint covers the battle-rule foundation only. It is not a claim of
campaign feature parity with the preserved Emerald Champions ROM.

## Verified source state

- Expansion foundation: `a994c5e8613bf54479a24b5d9710844ab63a8ef0`
- Emerald Champions foundation commit: `4397fd4fe80192500667b6592eca98bdd08c1037`
- Pokemon Showdown oracle: `bb179fbf8449e3c31632bd56f671ffb4404fa6e7`
- Ruleset generation: `GEN_CHAMPIONS`

## Closed parity gaps

- Champions Stat Points use a perfect-IV contribution without rewriting the
  Pokemon's stored IVs.
- Champions PP caps base PP at 20, ignores PP Ups, and initializes newly
  created, taught, swapped, and trainer-authored moves at the correct maximum.
- Healer uses its Champions 50 percent activation rate.
- Disguise treats its absorbed hit as neutral unless the target is immune.
- Anger Shell, Berserk, Color Change, and Pickpocket execute through their
  Champions interaction with Sheer Force while suppressed item effects remain
  suppressed.
- A Mega Evolution survives fainting and revival during the same Champions
  battle. Battle cleanup still restores the base species; Mega state is never
  persisted to the overworld or save.
- Strict campaign level caps and native instant text are enabled as foundation
  behavior.

## Regression result

The current source was rebuilt into a fresh test ELF and run through the host
mGBA test hydra with the `*Champions` filter on 2026-08-28:

- Passed: 40
- Failed: 0
- Known failing: 0

One non-applicable parameter case in the combined Make It Rain test reported
an `ASSUME` skip; it is not one of the 40 selected Champions test cases.

The four Champions tests inherited as known failures in the pinned expansion
(Anger Shell, Berserk, Color Change, and Pickpocket with Sheer Force) are now
ordinary passing regressions.

## Production build

- Padded ROM SHA-256: `6bbbc46105ceb060e7036499bd113f66c35af13286b68c36c193584955fe060f`
- Linked ROM used: 26,723,852 bytes (79.64 percent)
- EWRAM used: 226,424 / 262,144 bytes (86.37 percent)
- IWRAM used: 28,388 / 32,768 bytes (86.63 percent)

The parity patch does not justify species or asset pruning.

## Not yet claimed

- Full Showdown differential coverage for every Champions move, Ability, item,
  condition, and ordering edge case.
- Resolution of the inherited Pickup behavior observed in the broad Gen 9
  control run.
- Campaign services, trainer data, wild sets, tutors, vendors, Legendary
  Signs, Champions Circuit, story scripts, maps, dialogue, saves, or UI from
  the preserved Emerald Champions checkpoint.
- Full-game emulator and playthrough readiness.

Those items remain explicit gates in the migration ledger.
