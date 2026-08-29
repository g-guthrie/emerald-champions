# Emerald Champions feature-parity ledger

> **Historical snapshot.** This ledger benchmarks the modern branch against the
> superseded baseline `33202c162ebc34a1dbe2000acd26b0720baa109d` and is kept for
> its authored targets, not as a live defect list. Items it marks MISSING —
> wild distribution, route signs, trade evolutions, branding, reward economy,
> web integration — now pass the release gates. Current status lives in
> `FEATURE_PARITY.md`.

This ledger compares the preserved playable campaign at
`33202c162ebc34a1dbe2000acd26b0720baa109d` with the modern Champions branch.
An item is complete only when its source, build, regression, and representative
emulator path are closed. File presence alone is not parity.

Status values are `COMPLETE`, `PARTIAL`, `MISSING`, and `BLOCKED`.

## Foundation

| Feature | Preserved evidence | Modern status | Completion gate |
| --- | --- | --- | --- |
| Champions battle rules | New migration requirement | COMPLETE | 40/40 selected Champions regressions pass |
| All families and forms through Gen 9 | Curated subset in old ROM | COMPLETE | Modern expansion constants, data, graphics, icons, and cries compile |
| Official modern Mega roster | Hand-ported subset in old ROM | COMPLETE | Modern expansion Mega data and art compile |
| Custom Mega Tatsugiri and Glimmora | Old custom species, stones, art, and Commander presentation | PARTIAL | Preserve extension data; add legality, graphics, AI, faint/revive, and Commander tests |
| Mega-only selectable gimmick | Old campaign configuration | PARTIAL | Disable Z-Moves, Dynamax, Terastallization, and Ultra Burst in every player and trainer path; retain approved Primals |
| Strict cap curve | Old cap implementation and `src/caps.c` | PARTIAL | Cap values compile and every badge transition, Rare Candy, Leveler, Day Care, and postgame sentinel is tested |
| Instant text | Old option replacement | COMPLETE | Native instant text is forced in the modern configuration |
| Save layout and migration | Old Inclement-derived save blocks | PARTIAL | Compatibility boundary and stable modern state slots are documented; new game, save, reload, power-cycle, and build-update tests remain |

## Player preparation and economy

| Feature | Preserved evidence | Modern status | Completion gate |
| --- | --- | --- | --- |
| Live Hard/Medium/Easy setting | `fb8dfaaa190` | COMPLETE | Native Options row; Hard 0, Medium -2, Easy -4 enemy levels; allies unchanged |
| Poke Vial | Preserved campaign scripts and item behavior | PARTIAL | Granted and refillable; remaining capacity-upgrade and emulator paths must close |
| Reusable party Leveler | `476951be145` | PARTIAL | Source/build path is complete; evolution-chain emulator testing remains |
| No level-up move interruption | Preserved battle-ready workflow | COMPLETE | Bulk leveling never opens move-learning prompts; tutor remains complete |
| Legal move tutor from game start | Preserved Pokemon Center tutor | PARTIAL | All native legal move categories are ungated in every Center; competitive preset selector remains |
| Multiple competitive presets | `0c7b185494f` plus authored handbook corpus | PARTIAL | 1,356 sets, including two legal roles for every newly seeded modern campaign species, execute with full Champions fields; emulator UI paths remain |
| Battle-ready ordinary wild catches | `78e3716e353` and `712da9a722a` | PARTIAL | All 222 current wild-table species are covered; final redistributed tables and capture runtime paths remain |
| Protected progression items | Preserved Mega/progression exclusions | PARTIAL | Services block protected items and never gift Mega Stones; final evolution-item and reward audit remains |
| Free Pokemon Center battle vendor | Preserved economy contract | PARTIAL | Free non-Berry/non-progression vendor is in all 16 Centers; category UX and final policy audit remain |
| Berry economy retained | Preserved economy contract | PARTIAL | Free vendor excludes Berries and presets may attach them; final tree, pickup, and reward audit remains |
| Normal Mart progression plus Rare Candy | Preserved reward/economy rewrite | COMPLETE | Native Mart progression is unchanged and every Hoenn medicine list sells the 1,000 ten-level Rare Candy |
| Consumable held-item restoration | Preserved loadout restoration | PARTIAL | Champions restoration now includes Berries and captures; theft, Knock Off, and Frontier runtime fixtures remain |
| Reward and pickup rewrite | `0b2bc96c7d6` | MISSING | No reward is redundant with free moves/items; every removed TM/item has a coherent replacement |
| Trade and held-item evolutions | Preserved Inclement solutions | MISSING | Every enabled species has an obtainable native evolution method and required nonbattle item |

## Campaign, encounters, and endgame

| Feature | Preserved evidence | Modern status | Completion gate |
| --- | --- | --- | --- |
| Emerald Champions branding and title | `06d3882095f` | MISSING | Title, intro, credits, save metadata, UI strings, and ROM header contain no Verdant/Inclement branding |
| Wild encounter distribution | Preserved availability report and tables | MISSING | Every ordinary species has a deliberate acquisition path; bespoke species are not duplicated casually |
| Route-sign encounter lists | `f2f9fa3e962` and later fixes | MISSING | Signs list methods and species without false percentages or broken pagination |
| Story/reward cohesion | `3701ae0c046` and `0b2bc96c7d6` | MISSING | Required flags, gifts, shops, tutorials, dialogue, and one-time progression remain reachable and meaningful |
| Match Call and Gym rematch escalation | Vanilla infrastructure plus old rematch families | COMPLETE | Match Call contacts may remain for flavor, but no trainer can request or launch an escalated rematch; League replays reuse the finished League teams |
| Legendary Signs | `d38f0f470f1` and `88805ebfccf` | PARTIAL | All 53 source paths compile and static gates pass; discovery, catch, reload, and mastery emulator paths remain |
| Visible legendary encounters | Preserved prototype and final maps/scripts | PARTIAL | Native Darkrai, Cresselia, and Dialga objects/scripts compile; catch, loss, reload, and duplicate-prevention emulator paths remain |
| Legendary mastery reward | Preserved Arceus path | PARTIAL | Source count and level clamp pass; full acquisition and no-room runtime cases remain |
| Champions Circuit | Preserved concept plus pinned Showdown Champions generator | PARTIAL | Live 311-variant/444-template Showdown port, dedicated state, scaling, and rewards compile; multi-seed and full-run runtime gates remain |
| Native Battle Frontier | Preserved Frontier plus Circuit design | PARTIAL | Upstream facilities and new Circuit compile; every facility and Frontier Brain must still be completed under Champions mechanics |

## Trainers, AI, dialogue, and design source

| Feature | Preserved evidence | Modern status | Completion gate |
| --- | --- | --- | --- |
| Physical encounter index | Legacy master: 522 groups / 808 branches including rematches | MISSING | Regenerated from modern maps and scripts with every first-time required, optional, pair, story branch, and postgame invocation; Match Call/Gym rematch tiers excluded |
| Previously authored first encounters | Preserved trainer sources and checkpoint | MISSING | Retained first-time designs are revalidated under Champions legality and implemented in modern trainer format |
| Remaining first encounters | Preserved source baselines | MISSING | Every retained branch has exact team, format, levels, items, moves, ability, Stat Points, AI, dialogue intent, and difficulty |
| Approximately 85 percent doubles | New completion requirement | MISSING | Reachable trainer-branch report is 83-87 percent doubles; intentional singles are documented |
| Ordinary-trainer fatigue distribution | Master provisional bands | MISSING | Ordinary encounters approximate 30% at 6.0-6.9, 35% at 7.0-7.9, 25% at 8.0-8.9, and no more than 10% at 9.0-9.5 |
| Boss difficulty | Preserved anchor dossiers | MISSING | Gyms, rivals, factions, League, champion, Brains, and superbosses retain bespoke near-maximum designs |
| Species and strategy diversity | Preserved usage/reservation ledgers | MISSING | Repetition audit protects rarity, legendary coverage, all Megas, and rolling tactical variety without quota-forced teams |
| Doubles-aware AI | Preserved custom AI plus modern AI | PARTIAL | Partner targeting, spread moves, redirection, Protect, speed control, weather, terrain, setup, switching, Mega, and trainer-specific scripts pass fixtures |
| Trainer dialogue | Preserved rewrites through implemented range | MISSING | Every edited battle has pre/post/rematch text consistent with format, team identity, story, and GBA width limits |
| Master encounter document | Legacy document authored through Battle 203 | MISSING | The rematch-free campaign is reindexed, fully reauthored against final Champions data, globally audited, then source-locked to implementation |

## Packaging and runtime

| Feature | Modern status | Completion gate |
| --- | --- | --- |
| Clean standalone ROM | PARTIAL | Release build, header, checksum, memory report, and mGBA/Miyoo-compatible boot smoke test |
| Full playthrough readiness | MISSING | Automated progression audit plus representative saves at every badge, faction gate, League, and postgame |
| Web integration | MISSING | New ROM is packaged into the preserved interface and native/cloud saves survive compatible ROM updates |
| Release checkpoint | MISSING | Clean Git state, no unreviewed generated drift, packaged artifacts, and final parity report |

## Current truth

This checkpoint has a proven Champions battle foundation and a fully authored
campaign battle source, but it is not the final rebuilt campaign.

- The canonical master contains 509 rematch-free physical encounters and 557
  explicit trainer branches; 84.92 percent are doubles.
- All 557 parties are present in `trainers.party` with exact species, items,
  abilities, natures, Stat Points, legal moves, levels, and difficulty-tier AI.
- All 543 trainer IDs already reachable in the modern Hoenn source are wired.
- Fourteen disabled rematch slots were safely repurposed for selected one-time
  bespoke encounters without changing the save layout. Six are currently wired
  on retained maps; eight remain explicitly planned rather than falsely closed.
- The Champions Circuit generates teams live from the pinned Showdown
  Champions random-doubles corpus (311 variants, 444 role templates); it is not
  a finite seeded-team list.
- The next campaign phase restores the 22 missing Inclement-era map directories
  as coherent side-area clusters, then rewrites story, dialogue, rewards, wild
  availability, and Legendary Sign discovery around the expanded geography.

No remaining `PARTIAL` or `MISSING` row should be promoted merely because a
similarly named facility or source file exists. Runtime and playthrough gates
still control release status.
