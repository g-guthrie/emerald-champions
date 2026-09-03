# Independent audit tools (2026-09-03)

These compare the live tree against three reference trees and against itself. They are
deliberately independent of `scripts/verify_*` and of the docs. Extract the reference trees
first (they are not tracked):

```sh
mkdir -p work/audit-baselines/{inclement,oldbase}
git archive cf41a95b68a39ca74fefeb934c460f6f47eb0b3b data/maps data/scripts data/layouts data/event_scripts.s include/constants data/text | tar -x -C work/audit-baselines/inclement
git archive 0f801b110 data/maps data/scripts include/constants | tar -x -C work/audit-baselines/oldbase
git clone --depth 1 --branch expansion/1.16.3 https://github.com/rh-hideout/pokeemerald-expansion.git work/audit-baselines/vanilla
```

- `ec_baseline_diff.py` - every Hoenn map vs Inclement v1.13: header, connections, objects,
  warps, triggers, signs, layout collision, and semantically normalised scripts
  (compare+goto merged, local IDs resolved, text/movement labels hashed). Writes
  `baseline_diff.json` next to the tree.
- `threeway.py` / `fourway.py` - classify each label as kept / reverted-to-vanilla /
  modified / dropped relative to Inclement, Inclement's own upstream base, and 1.16.3.
- `reverted_detail.py` - shows exactly what Inclement had changed in every reverted label.
- `show_label.py MAP LABEL...` - normalised side-by-side diff of one script label.
- `reach.py` - decodes map.bin collision, treats permanent NPCs as blockers, flood-fills
  from every warp, and reports reachability regressions vs Inclement.
- `textwidth.py` - measures every dialogue line with the FONT_NORMAL glyph widths.
