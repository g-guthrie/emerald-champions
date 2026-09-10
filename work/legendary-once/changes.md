Native wild legendary capture update — September 10, 2026

The 39 native wild legendary residents now stop appearing after capture. They keep their 3% standard chance and have no badge gate. The existing capture ledger is reused; captures recorded by the preceding build also count. Fleeing or defeating a wild legendary does not set this ledger: the production capture command marks it only after GiveCapturedMonToPlayer succeeds.

Sweet Scent shares its 25% total only among eligible, uncaught native and quest species. When no eligible legends remain, selection returns SPECIES_NONE and the ordinary encounter table is used. Quest requirements and their 1% ordinary chances are preserved.

Route signs retain species with a (Caught) suffix, including ordinary-table legendary entries. The suffix is included in native font-width wrapping. This exposed and fixed an existing wrap defect: trailing commas could exceed the 200-pixel line boundary. Research, Center guides, and Devon dialogue now reflect one-time capture completion; no selection menus were added.

Verification: three existing native regressions updated/executed successfully, covering exact normal/Sweet Scent selection before and after capture (including zero remaining), roster completion markers and font widths across all 39 native residents, Center guide completion and local lead text, and all six capture/unlock save groups through flash save/load. This is native function/save testing plus capture-consumer source review, not a complete walking/capture-animation playthrough. See final-native.log. Initial layout failure and fixed rerun are retained as evidence.

Final ROM: pokeemerald-legendary-once-20260910-release.gba. Release build and content-stamp verification evidence is in final-build.log, stamp.log, and verification.log.
