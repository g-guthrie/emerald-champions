# Gate suite audit (2026-09-01)

Scope: the 33 static gates run by `scripts/verify_emerald_champions_release.py`, its
four built-in checks, and `scripts/run_emerald_champions_runtime_gates.py`.
Every gate was timed individually; the entire static roster runs in under ten
seconds, so cost is not the problem. Value and failure modes are.

## Classification

**Harmful (gave false confidence or blocked legitimate work)**

1. `verify_build_freshness` compared modification times. On 2026-09-01 the
   container copy of the tree was older than the host (uppercase menu strings,
   old flag names) yet the ROM it produced was newer than every host file, so the
   gate passed against a ROM that did not contain the latest edits. Replaced by a
   content stamp: `scripts/stamp_release_inputs.py` writes a sha256 over every
   build input inside the tree that `make release` compiled, and the verifier
   now requires that stamp to match the host tree. The mtime check is kept only
   as a cheap early warning.
2. `verify_emerald_champions_visual_contracts.py` pins four exact totals
   (`MAP_SPECIAL_CALLS = 979`, `MAP_SPECIAL_NAMES = 255`,
   `SCRIPTED_WARP_LITERAL_COORDS = 192`, `LITERAL_LOCAL_ID_VISUAL_CALLS = 1844`).
   Adding one `applymovement` to a scene fails the release. The counts prove
   nothing about correctness; the classification checks in the same script
   already do the real work. Fixed: the four pins are now coverage floors
   (`*_MIN`) so a collapse of the parser still fails while ordinary scene edits
   pass.
3. `verify_inclement_overworld_parity.py` pins `tileset_asset_count == 1566`,
   and the generator counted gitignored `.4bpp` build outputs, so regenerating
   the manifest after a local build failed the gate. Generator fixed to skip
   git-ignored files. The `map_count == 540` pin had the same shape; both pins
   are now floors (500 maps, 1500 assets).
4. The curated runtime suite is not tied to the release ROM. Nothing checks that
   `pokeemerald-test.elf` was built from the same sources as
   `pokeemerald-release.gba`; today it predated 128 changed files. Fixed:
   `run_emerald_champions_runtime_gates.py` now writes
   `pokeemerald-test.inputs.json` after building the ELF and `--run-only`
   refuses to run without a matching stamp.

**Redundant (three or more gates hash the same bytes)**

5. `audit_verdant_visual_parity.py --check-fast`,
   `verify_inclement_visual_sources.py`, and
   `verify_inclement_overworld_parity.py` each keep their own sha256 inventory of
   overlapping graphics and layout files. The Verdant byte manifest records the
   *current* hash of every visual file, so its only signal is "a file changed",
   which git already reports; its `--write` step must be re-run after every
   graphics edit and produced a 1,400-line diff today for a one-map change.
   Recommendation: keep `verify_inclement_visual_sources.py` (it compares
   against Inclement, an external reference) and the overworld parity manifest
   (same), and retire the Verdant manifest or reduce it to files that have a
   frozen reference commit.
6. `generate_emerald_champions_campaign_evidence.py --check` fails whenever any
   script line number in any map shifts, because the evidence appendix embeds
   line references. It is a documentation snapshot, not an invariant.
   Recommendation: regenerate it in the release pipeline instead of gating on it.
7. Dialogue width is checked twice in `verify_emerald_champions_story.py`
   (story lines and all literal lines) and again for menus in
   `verify_emerald_champions_native_ui.py`. Harmless, but one width checker
   would do.
8. `emerald_champions_reward_rewrite.py`, `emerald_champions_route_signs.py`,
   `restore_poke_vial_quest.py`, `align_emerald_champions_battle_scripts.py`,
   and `prune_unreachable_trainer_parties.py` are generators run in check mode.
   That is fine, but each re-derives its expectation from the same
   `trainers.party` / map data that other gates already parse; a shared loader
   would cut the duplicated parsing and the risk of the parsers disagreeing.

**Defensive but useful (keep)**

9. `verify_trainer_ability_legality.py`, `verify_emerald_champions_battle_sets.py`,
   `verify_trainer_runtime_coherence.py`, `audit_emerald_champions_master_battles.py`
   and the materialized-trainer verifier catch real authoring errors and are
   cheap. Keep.
10. `verify_emerald_champions_progression.py`, `verify_rematch_free_match_call.py`,
    `verify_legendary_availability.py`, `verify_solo_evolution_access.py` prove
    reachability contracts that a playthrough would otherwise have to find.
    Keep, but see the blind spots below.
11. `verify_unique_state_ids`, `verify_branding`, `verify_rom` (header/checksum/
    memory) are correct and cheap. Keep.

**Blind spots found this week (gates that pass while the game is wrong)**

- Wild distribution had no level sanity; level 2 Hydreigon passed. Fixed:
  `emerald_champions_wild_distribution.py` now derives evolution-level floors
  from `species_info` and fails any evolved form on a sub-level-10 slot; the
  full below-floor list (intentional late-area cases) is written to
  `work/audits/WILD_LEVEL_FLOOR_REPORT.md`. The sweep found 29 surf/rock slots
  with level 5 evolved forms and 87 DexNav hidden slots still at upstream's
  level 2-3 sample values; both are corrected in `wild_encounters.json`.
- Solo-evolution ignored `IF_BAG_ITEM_COUNT` and Sweets.
- Reward economy accepted Rare Candy as a live finite reward.
- Nothing checked facility EV spreads; every Frontier Pokemon carried 252-point
  spreads under the Champions `2*ev-1` formula until
  `generate_emerald_champions_frontier_sets.py --check` was added.
- The overworld parity manifest checks sprite/position but not the
  `local_id -> graphics_id` binding scripts rely on (Meteor Falls Archie).
  Fixed: `verify_local_id_sprite_bindings.py` requires every named local ID to
  carry the sprite family its name promises.

## Recommendations in priority order

1. Require the content stamp for both the release ROM and the test ELF. Done.
2. Remove the four visual-contract count pins and the two parity count pins;
   keep classification and per-map contracts. Done (floors remain).
3. Retire the Verdant byte manifest gate or restrict it to files with a frozen
   reference; regenerate the campaign evidence appendix instead of gating on it.
4. Add the missing invariants: per-map wild level bands, `IF_BAG_ITEM_COUNT`
   evolution sources, and local-ID sprite bindings. Done (evolution floors,
   sprite bindings; the solo-evolution gate already covers `IF_BAG_ITEM_COUNT`).
