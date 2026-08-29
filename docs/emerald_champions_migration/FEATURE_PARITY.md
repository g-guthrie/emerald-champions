# Emerald Champions release-candidate ledger

This ledger records measurable source and runtime closure for the standalone
GBA ROM. `COMPLETE` means the implementation, deterministic verifier, clean
production build, and relevant mGBA regression path are closed. `MANUAL` means
the code is release-ready but still needs human campaign playtesting.

## Foundation

| Feature | Status | Evidence |
| --- | --- | --- |
| Champions battle standard | COMPLETE | 57 Champions-named mechanics tests pass |
| Species/forms through Gen 9 | COMPLETE | Modern expansion data and assets build in the 32 MiB ROM |
| Official and custom Megas | COMPLETE | All 92 stones appear in the campaign battle master; Tatsugiri/Glimmora asset tests pass |
| Mega-only selectable gimmick | COMPLETE | Mega allowed; Z-Move, Ultra Burst, Dynamax, and Tera rejected; approved Primals retained |
| Strict cap curve | COMPLETE | Every badge milestone and postgame cap passes runtime tests |
| Instant text and live difficulty | COMPLETE | Hard 0, Medium -2, Easy -4; enemy parties only |
| Modern save layout | COMPLETE | SaveBlock1/2/3 compatibility tests pass; state IDs are unique |
| Old Inclement-derived saves | UNSUPPORTED | Deliberate save-family boundary documented in `SAVE_COMPATIBILITY.md` |

## Preparation and economy

| Feature | Status | Evidence |
| --- | --- | --- |
| Poke Vial and capacity quest | COMPLETE | Native four-map Chansey chase and Route 133 third charge verified |
| Reusable Leveler and ten-level Candy | COMPLETE | Strict-cap behavior, no move interruption, all medicine marts verified |
| Complete legal move tutor | COMPLETE | Level, pre-evolution, egg, TM, and tutor sources enabled from the start |
| Competitive preset selector | COMPLETE | 1,461 executable presets; exact nature, Ability, item, moves, and 66 Stat Points |
| Battle-ready ordinary wild catches | COMPLETE | Every one of 613 current wild species and every valid ordinary form resolves to a preset |
| Native Ability switching | COMPLETE | Party-menu action enumerates unique legal abilities |
| Free held-item vendor | COMPLETE | Six categories, 111 ordinary items, all 16 Centers |
| Berry/progression economy | COMPLETE | Berries and transformation items excluded; Mega/evolution archives unlock at badge eight |
| Mart and reward progression | COMPLETE | Native Mart stock retained, 1,000 Candy added, 47 rewards plus all TM pickups audited |
| Single-player evolutions | COMPLETE | 30 trade evolutions and all required evolution items verified |

## Campaign, story, and encounters

| Feature | Status | Evidence |
| --- | --- | --- |
| Emerald Champions branding | COMPLETE | ROM title `EM CHAMPIONS`, game code `BPEE`; obsolete player-facing branding rejected |
| Restored side areas | COMPLETE | 22 maps, 129 objects, 40 pickups, 21 wild tables, directed reachability pass |
| Wild distribution and route signs | COMPLETE | 602 unique wild species; 89 early species; 32 method-grouped signs |
| Story/reward cohesion | COMPLETE | Magma/Aqua-Rayquaza-Wallace-Frontier arc preserved; restored areas and Signs discoverable |
| Legendary acquisition | COMPLETE | 101 legendary-class families rooted; all 81 dependency chains terminate |
| Visible legendary presentation | COMPLETE | 23 visible quests, four static sanctuaries, giant Regigigas, native follower assets |
| Regional starter selection | COMPLETE | Nine trios, 81 starter-stage presets, rival counter family verified |
| Rematch-free campaign | COMPLETE | Match Call/Gym escalation disabled; replayable League keeps final teams |

## Trainers, battles, dialogue, and AI

| Feature | Status | Evidence |
| --- | --- | --- |
| Canonical encounter index | COMPLETE | 513 physical encounters and 561 reachable branches |
| Materialized trainer parties | COMPLETE | Every branch source-locks exactly to `trainers.party` |
| Battle format | COMPLETE | 84 singles, 475 doubles, 2 multi; 85.03 percent doubles/multi |
| Fatigue curve | COMPLETE | Mean 7.56; 31.9 percent ordinary 6.x encounters; no ordinary 9.x compression |
| Boss and showcase coverage | COMPLETE | All marquee bosses at 10.0; all 92 Megas and 58 targeted legendary showcases used |
| Team diversity | COMPLETE | 751 unique species, no duplicate team fingerprints, rolling-repeat gate passes |
| Trainer Bag rules | COMPLETE | No campaign trainer carries healing inventory; player Bag disabled in trainer battles |
| Trainer dialogue/layout | COMPLETE | 10,471 dialogue lines fit the native 216 px window; stale party references rejected |
| Doubles AI | COMPLETE | Core Champions, Commander, multi, Mega, status, viability, Circuit, and 56 doubles-AI fixtures pass; two upstream fixtures remain explicitly known-failing; harmful friendly fire is corrected |

## Frontier and packaging

| Feature | Status | Evidence |
| --- | --- | --- |
| Live Showdown Champions Circuit | COMPLETE | 311 variants, 444 templates, sixteen seeded teams and exact party restoration pass |
| Native Battle Frontier | MANUAL | Original facilities remain available beside the Circuit and compile under Champions rules; full facility streaks need playtesting |
| Standalone ROM | COMPLETE | Clean 32 MiB build, valid header/checksum, 80.45% ROM, 86.77% EWRAM, 86.58% IWRAM |
| Headless emulator boot | COMPLETE | Production ROM reaches the first BIOS/VBlank boundary with exit 0 |
| Full human campaign playthrough | MANUAL | Automated progression and runtime gates pass; balance and late-story feel require human playtesting |
| Hosted web ROM/cloud save | OUT OF SCOPE | Separate deployment project; do not infer deployment from this standalone build |
| GitHub publication | NOT REQUESTED | Branch remains local until the user explicitly asks to push |

## Current release candidate

- ROM: `pokeemerald.gba`
- SHA-256:
  `d5a9a17ce23a00c449b84b2b28e983107f862adfa0c3427df01913e94aefa008`
- Consolidated release verifier: PASS
- Emerald Champions custom tests: 14 passed, 0 failed
- Champions Circuit tests: 3 passed, 0 failed
- All Champions-named tests: 57 passed, 0 failed
- Commander tests: 41 passed, 0 failed
- Save-layout tests: 4 passed, 0 failed
- Doubles AI tests: 56 passed, 0 unexpected failed, 2 known-failing

This is a standalone-ROM release candidate. The remaining work is playtest
feedback and any explicitly requested deployment/publication, not missing
campaign source.
