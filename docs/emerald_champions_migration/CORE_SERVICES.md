# Core preparation and economy

Emerald Champions removes preparation grind while preserving campaign
progression around Mega Stones, evolution items, berries, and legendary
acquisition.

## Source-implemented systems

- Native Difficulty option: Hard uses authored levels, Medium applies -2, and
  Easy applies -4 to every enemy trainer Pokemon. Hard is the default.
- Text speed is forced to the instant setting.
- The first Pokemon Center visit grants the reusable Poke Vial and Leveler.
  Healing and whiteout refill the Vial; its native Chansey quest raises
  capacity to two, and Route 133 raises it to three.
- The Leveler raises the party to the current strict cap. Level-up move prompts
  are disabled, while normal evolution handling remains active.
- Rare Candy has no campaign acquisition. The reusable Leveler is the sole
  no-grind leveling service and stops cleanly at the current cap.
- All sixteen Hoenn Pokemon Centers have the shared move tutor and held-item
  vendor. The tutor exposes level, pre-evolution, egg, TM, and tutor moves.
- The free held-item vendor is split into six native categories containing 111
  ordinary competitive items. Berries, Mega Stones, Orbs, masks, Drives,
  Memories, Plates, and other transformation/progression items are excluded.
- The normal party menu has an Ability action listing the Pokemon's unique
  legal abilities; no external item, currency, or special screen is required.
- Badge eight unlocks complete Mega Stone and evolution-item archives before
  the League. Earlier story rewards remain finite and meaningful.
- Every scripted reward and every former TM map pickup has been audited for
  finite, nonredundant progression value.
- All thirty trade evolutions have single-player methods; every required
  evolution item is obtainable, and the Linking Cord is reusable.
- Held loadouts, including consumed Berries, restore after battle.

## Evidence

- Core-service static verifier: PASS
- Pokemon Centers: 16 / 16
- Medicine marts: 21 / 21
- Free ordinary held items: 111
- Reward scripts audited: 47 plus all map TM pickups
- Poke Vial quest verifier: PASS
- Solo evolution verifier: PASS
- Current runtime contract: `*Champions` selected 90/90 tests with zero
  accepted known-failing or TODO results in the frozen one-shot run

These source checks do not replace rendered menu/vendor checks, save/reload
scenarios, or human playtesting on intended emulator/device targets.
