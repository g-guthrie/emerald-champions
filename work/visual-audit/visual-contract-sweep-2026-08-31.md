# Emerald Champions Proactive Visual-Contract Sweep

## Scope

This sweep inventories and classifies every live Hoenn direct map effect, every shared healing entry path, every raw fixed placement in `field_effect.c`, literal world coordinates in map scripts, and the complete deterministic headless scene catalog.

## Step record

| Step | Check | Health |
|---:|---|:---:|
| 1 | Five live direct map-effect calls classified into three reviewed coordinate systems | PASS |
| 2 | Eight raw fixed-screen field-effect placements explicitly inventoried | PASS |
| 3 | Twenty-one Hoenn healer destinations and 18 shared nurse callers cross-validated | PASS |
| 4 | 779 literal script-world coordinates validated against their actual target layouts | PASS |
| 5 | Twelve sparkle anchors validated; Magma now follows Maxie's current position | PASS |
| 6 | One intentional off-map Battle Palace entrance sequence verified before its in-bounds move | PASS |
| 7 | Four mutations—camera shift, off-map sparkle, off-map metatile, unclassified effect—detected | PASS |
| 8 | All 125 deterministic UI/world scenarios rendered successfully | PASS |
| 9 | All 11 contact sheets inspected for clipping, overlap, wrong-state capture, and misplaced effects | PASS |
| 10 | Eight retained physical legendary encounters re-rendered | PASS |
| 11 | Dormant FRLG healer mismatch separated from the live Emerald build | INFO |

## Visual evidence

![Healing entry paths](rendered/healing-entry-paths-2026-08-31/healing-entry-path-audit.png)

![Magma sparkle comparison](rendered/screen-space-effect-audit-2026-08-31/magma-sparkle-comparison.png)

The complete rendered matrix and authoritative hash manifest are in `rendered/full-visual-contract-sweep-2026-08-31/`.

## Permanent enforcement

`scripts/verify_emerald_champions_visual_contracts.py` now fails when:

- A new direct map effect is not classified.
- A fixed-screen field-effect placement is added or changed without review.
- A Hoenn whiteout camera no longer aligns with its healer/machine contract.
- A shared nurse caller does not identify an actual nurse object.
- A literal sparkle, metatile, or object coordinate is outside its actual target map.
- The reviewed Battle Palace off-map entrance sequence changes.
- Any of the scanner's mutation probes stops being detected.

The visual-contract gate is part of `verify_emerald_champions_release.py`, so normal GitHub CI runs it automatically.

## Findings

The sweep confirmed and repaired two live defects: the 16 standard Center whiteout camera anchors and Magma Hideout's copied `(18,42)` sparkle. No additional live Hoenn visual-contract violation remained after those repairs.

One inactive FRLG-only issue remains informational: Indigo Plateau's heal-location row names `LOCALID_LEAGUE_NURSE`, while the FRLG map object is `LOCALID_INDIGO_LEAGUE_NURSE`. It is not executed by the Emerald build and was not changed without a separate FRLG render target.

## Evidence limits

The gate covers structural relationships and deterministic rendered states. It cannot decide whether an otherwise correctly anchored new composition is artistically tasteful, and it does not replace a human campaign playthrough.
