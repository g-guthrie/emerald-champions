# Emerald Champions evidence ledger

This ledger reports what the current working source and its named gates can
measure. It deliberately does not call the game perfect, release-ready, or a
current release candidate.

Evidence labels are narrow:

- `SOURCE CHECKED` means a deterministic source/data verifier covers the stated
  invariant. It is not build, runtime, visual, balance, or playthrough proof.
- `RUNTIME CONTRACT` means the case is selected by the current curated manifest;
  a fresh run for the exact candidate is still required.
- `BUILD REQUIRED` and `MANUAL` name evidence that source checks cannot supply.
- `UNSUPPORTED` and `OUT OF SCOPE` are product boundaries, not missing claims.

## Foundation

| Feature | Evidence | Current measurable truth |
| --- | --- | --- |
| Champions battle standard | RUNTIME CONTRACT | `*Champions` selected 90/90 tests with zero accepted known-failing or TODO debt in the frozen one-shot run |
| Species/forms through Gen 9 | BUILD REQUIRED | Source/data/assets are present; a fresh 32 MiB ROM build must prove the candidate fits |
| Official and custom Megas | SOURCE CHECKED | 95 preset orientations cover all 92 campaign Mega Stones; all 92 appear in trainer showcases |
| Mega-only selectable gimmick | SOURCE CHECKED | Mega allowed; Z-Move, Ultra Burst, Dynamax, and Tera rejected; approved Primals retained |
| Strict cap curve | SOURCE CHECKED | Badge milestones and postgame cap contracts resolve; observed difficulty remains unplayed |
| Instant text and live difficulty | SOURCE CHECKED | Hard 0, Medium -2, Easy -4; enemy parties and Circuit use the same setting |
| Modern save layout | RUNTIME CONTRACT | Four save-layout tests are mandatory; state IDs are statically checked for uniqueness |
| Historical save families | SOURCE CHECKED | Raw 81e state without the colliding Zygarde marker has a bounded migration; native unversioned e7 state is stamped without rewrite; the indistinguishable 81e-Zygarde/e7-upgrade overlap and arbitrary layouts fail safe as documented in `SAVE_COMPATIBILITY.md` |

## Preparation and economy

| Feature | Evidence | Current measurable truth |
| --- | --- | --- |
| Poke Vial and capacity quest | SOURCE CHECKED | Native four-map Chansey chase and Route 133 third charge are wired |
| Reusable Leveler and ten-level Candy | SOURCE CHECKED | Strict-cap behavior, no move interruption, and all 21 medicine lists are checked |
| Complete legal move tutor | SOURCE CHECKED | Level, pre-evolution, egg, TM, and tutor sources are enabled from the start |
| Competitive preset selector | SOURCE CHECKED | 5,311 executable presets: 2,619 Doubles and 2,692 Singles; at least two non-Mega roles per format for every direct species/form; all 92 stones covered |
| Stat Point editor | SOURCE CHECKED | Free native 0-32-per-stat, 66-total editing in every Center and Fallarbor; IVs remain fixed-perfect |
| Battle-ready ordinary wild catches | SOURCE CHECKED | All 616 species/forms parsed from encounter data resolve a non-Mega preset |
| Native Ability switching | SOURCE CHECKED | Party-menu action enumerates unique legal abilities |
| Free held-item vendor | SOURCE CHECKED | Six categories, 111 ordinary items, all 16 Centers |
| Berry/progression economy | SOURCE CHECKED | Berries and transformation items excluded; Mega/evolution archives unlock at badge eight |
| Mart and reward progression | SOURCE CHECKED | Rare Candy has no producer; former slots are capture rewards, and scripted rewards plus map pickups are audited |
| Single-player evolutions | SOURCE CHECKED | 30 trade evolutions and required evolution items have solo paths |

## Campaign, story, and encounters

| Feature | Evidence | Current measurable truth |
| --- | --- | --- |
| Emerald Champions branding | SOURCE CHECKED | Build identity is title `EM CHAMPIONS`, game code `BPEE`; a fresh ROM must still validate its header |
| Story/progression graph | SOURCE CHECKED | 540 maps, 4,086 events, 1,402 warps, 17,938 references across 104,701 assembled lines, and 391 value-returning special calls resolve |
| Restored side areas | SOURCE CHECKED | 22 maps, 129 objects, 40 pickups, 21 wild tables, and directed reachability contracts |
| Wild distribution and route signs | SOURCE CHECKED | 146 headers on 138 Hoenn maps expose 592 unique species/forms; 32 method-grouped route signs |
| Story and dialogue width | SOURCE CHECKED | 10,846 selected story lines and 47,623 literal Hoenn lines fit the static 216 px budget |
| Legendary acquisition | SOURCE CHECKED | 101 legendary-class families rooted; all 82 dependency chains terminate |
| Visible legendary presentation | SOURCE CHECKED | 25 visible Sign quests and 32 total physical one-off Pokémon objects are wired |
| Regional starter selection | SOURCE CHECKED | Nine trios, 81 starter-stage presets, and rival counter-family mapping |
| Rematch-free campaign | SOURCE CHECKED | Match Call/Gym escalation disabled; replayable League keeps final teams |

## Trainers, battles, dialogue, and AI

| Feature | Evidence | Current measurable truth |
| --- | --- | --- |
| Canonical encounter index | SOURCE CHECKED | 513 physical encounters, 561 branches, and 2,147 authored Pokémon |
| Materialized trainer parties | SOURCE CHECKED | Every branch source-locks exactly to `trainers.party` |
| Battle format | SOURCE CHECKED | 87 single, 460 double, 14 multi; 84.49% doubles-like |
| Authored difficulty curve | SOURCE CHECKED | Mean 7.57, median 7.5; ordinary bands are 143 at 6.x, 267 at 7.x, and 64 at 8.x |
| Boss and showcase coverage | SOURCE CHECKED | All 92 Megas and all 57 targeted legendary species appear in opponent parties |
| Team diversity | SOURCE CHECKED | 754 unique species/forms; rolling-repeat and duplicate-team gates pass |
| Trainer Ability legality | SOURCE CHECKED | All 2,147 authored Abilities resolve legally for the configured species/forms |
| Trainer dialogue/layout | SOURCE CHECKED | Every campaign trainer ID resolves to source dialogue; static dialogue width gates cover the counts above |
| Doubles AI | RUNTIME CONTRACT | The manifest requires at least 65 doubles-AI tests and accepts zero known-failing or TODO results |

## Frontier, packaging, and human evidence

| Feature | Evidence | Current measurable truth |
| --- | --- | --- |
| Live Showdown Champions Circuit | SOURCE CHECKED | 311 variants and 444 templates; generated legality, reward schedule, and party-restoration contracts are checked |
| Curated runtime suite | RUNTIME CONTRACT | 19 filters, summed minimum 256, 43 selected source files; the frozen one-shot run passed 256/256 with zero accepted or observed debt |
| Native Battle Frontier | MANUAL | Original facilities remain beside the Circuit; complete native streaks need playtesting |
| Standalone ROM | BUILD REQUIRED | No ROM hash, size, or memory percentage in this ledger is evidence for a changed working tree |
| Entrypoint smoke | BUILD REQUIRED | The exact production ROM must reach first VBlank in the release job |
| Full human campaign playthrough | MANUAL | Pacing, balance, clarity, save/retry behavior, and fun remain unobserved where the master says `UNPLAYED` |
| Visual/device coverage | MANUAL | Menus, dialogue, maps, sprites, collision, animations, audio, saving, and controls need hardware-like and intended-device checks |
| Hosted web ROM/cloud save | OUT OF SCOPE | Separate deployment project; do not infer deployment or cloud retention from the standalone source |

## Candidate boundary

A releasable candidate still needs a fresh clean build, the complete source and
artifact verifier, first-VBlank smoke, a zero-debt run of the current runtime
manifest, scenario saves, visual/device inspection, and a fresh-save human
campaign playthrough. Record the resulting ROM digest and memory report with
that candidate; do not carry an older artifact hash forward into this ledger.
