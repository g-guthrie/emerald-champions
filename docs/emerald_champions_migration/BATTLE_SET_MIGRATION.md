# Competitive battle-set migration checkpoint

The preserved doubles corpus is now executable on the modern Champions data
model instead of being treated as prose or copied Inclement-era slots.

## Runtime corpus

- Authored default sets: 1,143
- Authored alternatives: 166
- Total executable sets: 1,309
- Current wild-table species: 222
- Current wild-table species with a default: 222
- Each set carries exact moves, item policy, nature, Ability identity, and six
  Stat Point values totaling 66 with a 32-per-stat maximum.

Legacy regional/form constant names are normalized to the modern expansion.
Six removed custom-Ability references have deliberate canonical adaptations.
If any other old Ability is no longer legal for its species, runtime resolution
uses the current default Ability when legal, then the current Hidden Ability,
then an ordinary Ability. The final all-species editorial audit must replace
every such fallback with an explicit judgment.

## Player and wild behavior

- Every serviced Pokemon Center now offers both `Build a Battle Set` and the
  complete legal move relearner.
- Applying a set replaces moves, effective nature, Ability, Stat Points, and
  the ordinary held item.
- A protected progression item already held by the Pokemon is never discarded.
- Mega presets remain hidden until the player owns the Mega Ring. Choosing one
  never grants its stone; the dialogue names the required stone.
- Opponent presets do receive their authored Mega Stone.
- Ordinary generated wild Pokemon choose uniformly from their currently visible
  set choices before the native wild-held-item roll, so the preset item wins.
- Legendary, Mythical, Ultra Beast, Paradox, Mega, Primal, Ultra, Gigantamax,
  and Tera-form encounters are excluded from ordinary random preset assignment.

## Validation

- All 1,309 imported sets were instantiated and applied inside mGBA against the
  current species, move, item, Ability, and nature data.
- Every applied set retained 66 Stat Points and every required opponent Mega
  Stone was equipped.
- The combined Champions and Emerald Champions suite passed 48/48, including
  all-set legality and the explicit coverage report.
- Production ROM build: clean
- Padded ROM SHA-256:
  `86b25057ada4cea5664123fc3a3b18bd6c0ddca3ff8a95026612129f2bce6804`
- Linked ROM used: 26,789,804 bytes (79.84 percent)
- EWRAM used: 226,436 / 262,144 bytes (86.38 percent)
- IWRAM used: 28,388 / 32,768 bytes (86.63 percent)

## Explicitly open

The modern species table contains 262 non-legendary/non-battle-transformation
IDs without imported defaults. Many are cosmetic, partner, starter, or other
form endpoints that will never be ordinary wild encounters; the remainder are
the full-generation additions absent from the preserved curated roster. The
final availability contract must classify every ID, author every obtainable
one, and reduce the missing count for obtainable Pokemon to zero before this
feature is complete.

Real-emulator menu, item-preservation, wild hostility, capture, evolution, and
Mega-preset interaction tests also remain required.
