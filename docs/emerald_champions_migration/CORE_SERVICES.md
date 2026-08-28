# Core preparation and economy checkpoint

This checkpoint restores the first playable Emerald Champions service slice on
top of the Champions engine. It does not claim preset, wild-distribution,
story, or trainer parity.

## Implemented

- Native Options difficulty row: Hard keeps authored enemy levels, Medium
  subtracts two, and Easy subtracts four. Enemy parties A and B are adjusted
  after creation; player and allied partner parties are not touched.
- Hard is the new-game default. Native instant text remains forced.
- The first Pokemon Center visit grants a Poke Vial and Leveler. Center healing
  and whiteout refill the Vial.
- The Poke Vial restores the active party outside battle without healing the
  PC boxes.
- The reusable Leveler raises every eligible party member to the current cap,
  preserves multi-stage evolution handling, and never consumes itself.
- Rare Candy costs 1,000, raises up to ten levels without crossing the cap,
  and is added to all 20 Hoenn medicine-mart lists.
- Level-up and evolution move prompts are disabled. The complete native move
  relearner exposes level, pre-evolution, egg, TM, and tutor moves without item
  or level ownership gates.
- All 16 Hoenn Pokemon Centers contain the shared move tutor and free battle
  vendor. The vendor excludes Berries and explicit transformation/progression
  items.
- Champions loadouts restore their original held item after battle, including
  consumed Berries. Captured preset Pokemon also retain their original Berry.

## Build and regression evidence

- Production build: clean
- Padded ROM SHA-256:
  `a4c8d9e4656402be677211e1c230654fe4a0eaa556c930d730c154250f069a74`
- Linked ROM used: 26,726,316 bytes (79.65 percent)
- EWRAM used: 226,436 / 262,144 bytes (86.38 percent)
- IWRAM used: 28,388 / 32,768 bytes (86.63 percent)
- Selected Champions plus Emerald Champions regressions: 43 passed, 0 failed

## Still open in this feature family

- Real-emulator interaction testing for the Leveler evolution sequence, Vial,
  difficulty menu, complete move lists, and free shop quantity flow.
- The Route 111 and Route 133 Poke Vial capacity upgrades and their story
  validation.
- Competitive preset selection with moves, nature, Ability, Stat Points, and
  held item.
- Battle-ready wild preset assignment.
- Native vendor category UX and complete item-policy verification.
- Reward/economy rewrites that remove redundant campaign gifts and pickups.
