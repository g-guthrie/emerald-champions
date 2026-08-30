# Inclement Emerald Stat Port Audit

**Implementation status:** Port 1 is materialized in the configured species
table: 102 selected species/forms plus three required Mega HP companions. The
machine-readable contract is
`docs/emerald_champions_stat_rebalances.json`, enforced by
`scripts/verify_species_stat_rebalances.py`. The remaining 162 Inclement
entries are still excluded or deferred exactly as classified below.

## Verdict

Do **not** copy the entire Inclement Emerald `REBALANCED_VERSION` stat table wholesale. The source changes 264 species/forms, but it mixes three different things: modest rescue buffs, large redesigns that swap a Pokémon’s role, and changes to Megas or special Pokémon that would replace the modern Champions-facing identity Emerald Champions is trying to teach.

The source-derived recommendation is a staged port:

- **Port now: 102 species/forms.** This is 29 final-stage species whose Inclement Abilities and matching tutor identities are already present, plus 73 conservative final-stage buffs. These changes touch 398 authored trainer Pokémon across 287 trainer branches, 122 tutor presets, and 468 wild slots across 116 maps. Every affected battle must therefore be re-rated from executable data after the batch lands.
- **Review individually: 29 rescue cases.** These are genuinely weak final-stage Pokémon, but the Inclement jump is larger than 30 BST. Some are excellent ideas; some, such as Shedinja, have nonlinear mechanics and should not be accepted by a numeric rule.
- **Hold: 133 entries.** These are role swaps not already justified by the imported Ability/set identity, non-final evolution stages that can use Eviolite, already-high-power species, Megas/legendary identities, absent Inclement custom Megas, or two entries already at the target.

This preserves the core promise: weak and middling Pokémon get purposeful help, while official modern identities remain recognizable and no stat nerf enters merely because Inclement once used it.

## Ground truth and reproducibility

- Historical Emerald Champions comparison baseline: configured `gSpeciesInfo`
  from commit `8432feb0996ea830807a6f786f7008d54c1ebc91`
  (`release/full-source-audit`). This identifies the audit input, not the current
  working-tree revision.
- The machine-readable manifest carries the current configured-stat guard count
  and digest, and `scripts/verify_species_stat_rebalances.py` recomputes them.
  This narrative audit deliberately does not duplicate that drift-prone value.
- Inclement Emerald source: [`jrci-dev/inclementemerald` commit `93e96730fb3c771f7f08f9623ae36f0643c52d3d`](https://github.com/jrci-dev/inclementemerald/commit/93e96730fb3c771f7f08f9623ae36f0643c52d3d), with `REBALANCED_VERSION` enabled and `P_UPDATED_STATS = GEN_8`.
- The comparison used the literal compiled configuration, not campaign documentation. The current species table was preprocessed with this repository’s active configuration. Inclement was evaluated twice—standard and rebalanced—so the ledger contains only changes authored by Inclement, not every old-Gen-versus-modern-stat difference.
- Old Inclement form names were normalized to the current enum (`*_ALOLAN`→`*_ALOLA`, `MEOWSTIC_FEMALE`→`MEOWSTIC_F`, default seasonal/form aliases, and similar). Four Inclement custom Megas have no current species entry: Butterfree, Flygon, Kingler, and Lapras.

## Selection policy

### Port 1A: Ability-coupled identities (29)

The prior Ability pass intentionally imported 40 Inclement Ability identities. Thirty of those species also have Inclement stat changes; Girafarig is held because it now evolves into Farigiraf and can carry Eviolite. The remaining 29 are justified even when the stat change is a redistribution, because the current tutor presets have already been authored around that identity. Leaving the official stats in place creates half-ported designs such as special No Guard Pidgeot with only 70 SpA, physical Tough Claws Heatmor retaining 105 unused SpA, and fast physical Decidueye remaining at base 70 Speed.

| Species | Current → IE target (HP/Atk/Def/SpA/SpD/Spe) | Current tutor identity | Trainer / preset / wild-slot exposure |
|---|---|---|---:|
| `BEAUTIFLY` | `60/70/50/100/50/65` → `60/50/60/110/60/85` | Recommended: Quiver Dance special sweeper | 2 / 1 / 0 |
| `DECIDUEYE` | `78/107/75/100/100/70` → `78/107/75/70/90/110` | Recommended: Fast trapper and Tailwind setter | 1 / 1 / 0 |
| `DEWGONG` | `90/70/80/70/95/70` → `90/90/60/95/95/90` | Recommended: Fur Coat Fake Out speed control | 2 / 1 / 45 |
| `DODRIO` | `60/110/70/60/60/110` → `80/110/70/60/60/110` | Physical Attacker: Moxie physical cleaner<br>Wallbreaker: Doubles Wallbreaker | 7 / 2 / 2 |
| `DUSTOX` | `60/50/70/50/90/65` → `80/30/90/70/90/65` | Recommended: bulky Quiver Dance win condition | 3 / 1 / 0 |
| `EMOLGA` | `55/75/60/75/60/103` → `55/75/60/105/60/113` | Recommended: Lightning Rod fast control | 4 / 1 / 2 |
| `FEAROW` | `65/90/65/61/61/100` → `65/110/75/61/71/100` | Recommended: Intimidate physical pivot | 3 / 1 / 49 |
| `FLYGON` | `80/100/80/80/80/100` → `80/100/80/100/80/100` | Tailwind: Flygon Tailwind Attacker<br>Bulky Attacker: Doubles Bulky Attacker | 5 / 2 / 0 |
| `GOGOAT` | `123/100/62/97/81/68` → `123/100/62/79/81/95` | Recommended: Grassy Surge bulky setup attacker | 3 / 1 / 0 |
| `GOODRA` | `90/100/70/110/150/80` → `90/100/85/110/135/80` | Special Attacker: Poison Heal bulky special attacker<br>Special Attacker II: Special Attacker | 4 / 2 / 0 |
| `GOURGEIST_AVERAGE` | `65/90/122/58/75/84` → `65/58/122/90/75/84` | Special Attacker: Flare Boost special burn attacker<br>Trick Room: Trick Room Support — Gourgeist-Super | 2 / 2 / 0 |
| `GOURGEIST_LARGE` | `75/95/122/58/75/69` → `75/58/122/95/75/69` | Recommended: Bulky Flare Boost special attacker | 1 / 1 / 0 |
| `GOURGEIST_SMALL` | `55/85/122/58/75/99` → `55/58/122/85/75/99` | Recommended: Fast Flare Boost special attacker | 1 / 1 / 0 |
| `GOURGEIST_SUPER` | `85/100/122/58/75/54` → `85/58/122/100/75/54` | Recommended: Flare Boost Trick Room attacker | 2 / 1 / 0 |
| `HEATMOR` | `85/97/66/105/66/65` → `85/97/96/85/96/65` | Recommended: Heatmor source-backed doubles set | 3 / 1 / 0 |
| `LOPUNNY` | `65/76/84/54/96/105` → `65/96/84/54/96/105` | Recommended: Scrappy Fake Out disruptor<br>Choice Attacker: Choice Attacker | 5 / 2 / 0 |
| `LUVDISC` | `43/30/55/40/65/97` → `43/30/55/95/65/97` | Recommended: Soul-Heart speed-control cleaner | 5 / 1 / 10 |
| `MAGMORTAR` | `75/95/67/125/95/83` → `75/95/67/125/85/93` | Special Attacker: Mega Launcher special coverage attacker<br>Choice Attacker: Choice Attacker | 6 / 2 / 14 |
| `MEGANIUM` | `80/82/100/83/100/80` → `80/82/100/93/100/80` | Recommended: Priority sustain and special-screen anchor | 4 / 1 / 0 |
| `MISMAGIUS` | `60/60/60/105/105/105` → `60/60/60/110/110/110` | Recommended: Pixilate spread special attacker | 4 / 1 / 0 |
| `PALOSSAND` | `85/75/110/100/75/35` → `85/75/120/100/85/35` | Recommended: Self-sufficient Sand Stream wall | 5 / 1 / 0 |
| `PIDGEOT` | `83/80/75/70/70/101` → `83/70/75/95/70/101` | Recommended: No Guard Tailwind attacker | 5 / 1 / 0 |
| `PYROAR` | `86/68/72/109/66/106` → `86/68/72/129/66/106` | Recommended: Competitive spread special attacker | 6 / 1 / 0 |
| `RAPIDASH` | `65/100/70/80/80/105` → `85/110/70/80/80/105` | Recommended: Reckless fast physical attacker | 5 / 1 / 8 |
| `TORTERRA` | `95/109/105/75/85/56` → `95/109/114/75/85/56` | Recommended: Solid Rock Trick Room spread attacker | 4 / 1 / 0 |
| `TREVENANT` | `85/110/76/65/82/56` → `85/110/86/65/92/56` | Recommended: Grassy Surge priority bruiser | 4 / 1 / 4 |
| `TROPIUS` | `99/68/83/72/87/51` → `99/88/93/72/97/51` | Recommended: Aerilate Tailwind attacker | 6 / 1 / 2 |
| `VESPIQUEN` | `70/80/102/80/102/40` → `70/80/112/80/112/40` | Recommended: Intimidate Tailwind sustain attacker | 4 / 1 / 0 |
| `WAILORD` | `170/90/45/90/45/60` → `170/90/65/90/65/60` | Recommended: Drizzle Water Spout attacker | 6 / 1 / 22 |

One concrete preset defect should be fixed in the same batch: Flygon’s `Bulky Attacker` alternative is Jolly with full physical investment but carries Earth Power. The IE target raises SpA from 80 to 100, but Jolly still suppresses that move. Make the set physical or give it a mixed/special nature and investment; do not treat the stat port as the fix.

### Port 1B: Conservative buffs (73)

These are final-stage, non-special species with no stat reduction or role swap, current BST at most 485, target BST at most 515, and total gain at most 30. Five otherwise qualifying species are already in the Ability-coupled group above; the remaining 73 are the conservative batch.

| Species | Current → IE target | BST | Trainer mons | Tutor sets | Wild slots/maps |
|---|---|---:|---:|---:|---:|
| `ABSOL` | `65/130/60/75/60/75` → `65/130/65/75/65/85` | 465→485 | 5 | 2 | 1/1 |
| `AMBIPOM` | `75/100/66/60/66/115` → `75/100/66/60/66/125` | 482→492 | 3 | 1 | 2/1 |
| `ARAQUANID` | `68/70/92/50/132/42` → `68/90/92/50/132/42` | 454→474 | 7 | 1 | 0/0 |
| `BASCULIN_RED_STRIPED` | `70/92/65/80/55/98` → `70/92/65/80/55/118` | 460→480 | 5 | 2 | 9/3 |
| `BASCULIN_BLUE_STRIPED` | `70/92/65/80/55/98` → `70/92/65/80/55/118` | 460→480 | 0 | 1 | 0/0 |
| `BEEDRILL` | `65/90/40/45/80/75` → `65/100/40/45/80/90` | 395→420 | 4 | 1 | 1/1 |
| `BIBAREL` | `79/85/60/55/60/71` → `89/85/80/55/60/71` | 410→440 | 3 | 1 | 0/0 |
| `BUTTERFREE` | `60/45/50/90/80/70` → `60/45/50/100/80/90` | 395→425 | 4 | 1 | 5/1 |
| `CACTURNE` | `70/115/60/115/60/55` → `70/115/60/115/60/85` | 475→505 | 3 | 1 | 0/0 |
| `CHATOT` | `76/65/45/92/42/91` → `76/65/45/102/42/101` | 411→431 | 5 | 1 | 2/2 |
| `CHIMECHO` | `75/50/80/95/90/65` → `85/50/90/95/100/65` | 455→485 | 3 | 1 | 2/1 |
| `CINCCINO` | `75/95/60/65/60/115` → `75/115/60/65/60/115` | 470→490 | 3 | 1 | 0/0 |
| `CRUSTLE` | `70/105/125/65/75/45` → `70/115/135/65/75/45` | 485→505 | 3 | 1 | 6/2 |
| `DRUDDIGON` | `77/120/90/60/90/48` → `97/120/90/60/90/48` | 485→505 | 3 | 1 | 13/6 |
| `DURANT` | `58/109/112/48/48/109` → `58/109/112/48/78/109` | 484→514 | 4 | 1 | 2/1 |
| `GALVANTULA` | `70/77/60/97/60/108` → `70/77/60/107/60/108` | 472→482 | 5 | 1 | 0/0 |
| `GARBODOR` | `80/95/82/60/82/75` → `80/95/102/60/82/75` | 474→494 | 2 | 1 | 0/0 |
| `GOLURK` | `89/124/80/55/80/55` → `89/134/90/55/90/55` | 483→513 | 5 | 2 | 9/3 |
| `GOREBYSS` | `55/84/105/114/75/52` → `55/84/105/124/75/72` | 485→515 | 7 | 1 | 2/2 |
| `GRANBULL` | `90/120/75/60/60/45` → `90/130/75/60/60/65` | 450→480 | 1 | 1 | 3/1 |
| `GRUMPIG` | `80/45/65/90/110/80` → `100/45/65/100/110/80` | 470→500 | 3 | 1 | 1/1 |
| `HARIYAMA` | `144/120/60/40/60/50` → `144/130/70/40/70/50` | 474→504 | 6 | 2 | 0/0 |
| `HELIOLISK` | `62/55/52/109/94/109` → `62/55/52/119/94/109` | 481→491 | 6 | 1 | 1/1 |
| `HITMONCHAN` | `50/105/79/35/110/76` → `80/105/79/35/110/76` | 455→485 | 2 | 1 | 0/0 |
| `HITMONLEE` | `50/120/53/35/110/87` → `80/120/53/35/110/87` | 455→485 | 1 | 1 | 0/0 |
| `HITMONTOP` | `50/95/95/35/110/70` → `80/95/95/35/110/70` | 455→485 | 2 | 1 | 0/0 |
| `HUNTAIL` | `55/104/105/94/75/52` → `55/114/105/94/75/72` | 485→515 | 5 | 1 | 2/2 |
| `JYNX` | `65/50/35/115/95/95` → `65/50/35/115/95/115` | 455→475 | 5 | 1 | 1/1 |
| `KECLEON` | `60/90/70/60/120/40` → `80/100/70/60/120/40` | 440→470 | 3 | 1 | 0/0 |
| `KINGLER` | `55/130/115/50/50/75` → `80/130/115/50/50/75` | 475→500 | 3 | 1 | 22/21 |
| `LANTURN` | `125/58/58/76/76/67` → `125/58/58/86/86/67` | 460→480 | 6 | 2 | 3/3 |
| `LIEPARD` | `64/88/50/88/50/106` → `64/98/50/98/50/106` | 446→466 | 7 | 1 | 1/1 |
| `LUDICOLO` | `80/70/70/90/100/70` → `80/70/70/100/100/80` | 480→500 | 6 | 2 | 0/0 |
| `LURANTIS` | `70/105/90/80/90/45` → `80/115/95/80/95/45` | 480→510 | 2 | 2 | 2/1 |
| `MANECTRIC` | `70/75/60/105/60/105` → `70/75/60/125/60/105` | 475→495 | 7 | 2 | 1/1 |
| `MAROWAK` | `60/80/110/50/80/45` → `75/80/110/50/80/45` | 425→440 | 0 | 1 | 14/4 |
| `MAROWAK_ALOLA` | `60/80/110/50/80/45` → `75/80/110/50/80/45` | 425→440 | 4 | 1 | 1/1 |
| `MASQUERAIN` | `70/60/62/100/82/80` → `70/60/72/110/82/80` | 454→474 | 3 | 1 | 20/6 |
| `MAWILE` | `50/85/85/55/55/50` → `70/85/95/55/55/50` | 380→410 | 4 | 1 | 8/8 |
| `MINUN` | `60/40/50/75/85/95` → `60/40/50/75/105/105` | 405→435 | 3 | 1 | 1/1 |
| `MR_MIME` | `40/45/65/100/120/90` → `40/45/65/120/120/90` | 460→480 | 2 | 1 | 0/0 |
| `NINJASK` | `61/90/45/50/50/160` → `61/100/45/50/50/160` | 456→466 | 4 | 1 | 0/0 |
| `NOCTOWL` | `100/50/50/86/96/70` → `100/50/70/96/96/70` | 452→482 | 5 | 1 | 5/2 |
| `ORICORIO_BAILE` | `75/70/70/98/70/93` → `75/70/70/108/70/103` | 476→496 | 3 | 1 | 1/1 |
| `ORICORIO_PAU` | `75/70/70/98/70/93` → `75/70/70/108/70/103` | 476→496 | 1 | 1 | 0/0 |
| `ORICORIO_POM_POM` | `75/70/70/98/70/93` → `75/70/70/108/70/103` | 476→496 | 3 | 1 | 0/0 |
| `ORICORIO_SENSU` | `75/70/70/98/70/93` → `75/70/70/108/70/103` | 476→496 | 1 | 1 | 0/0 |
| `PLUSLE` | `60/50/40/85/75/95` → `60/50/40/105/75/105` | 405→435 | 4 | 1 | 1/1 |
| `QUAGSIRE` | `95/85/85/65/65/35` → `95/95/95/65/65/35` | 430→450 | 6 | 1 | 6/3 |
| `QWILFISH` | `65/95/85/55/55/85` → `65/95/115/55/55/85` | 440→470 | 6 | 1 | 11/10 |
| `RAICHU` | `60/90/55/90/80/110` → `60/90/55/90/80/115` | 485→490 | 7 | 3 | 0/0 |
| `RAICHU_ALOLA` | `60/85/50/95/85/110` → `60/85/50/95/85/115` | 485→490 | 3 | 1 | 3/1 |
| `RATICATE` | `55/81/60/50/70/97` → `55/101/60/50/70/102` | 413→438 | 1 | 1 | 31/8 |
| `RATICATE_ALOLA` | `75/71/70/40/80/77` → `75/86/80/40/80/77` | 413→438 | 2 | 1 | 3/1 |
| `RELICANTH` | `100/90/130/45/65/55` → `100/110/130/45/65/55` | 485→505 | 5 | 1 | 17/3 |
| `SABLEYE` | `50/75/75/65/65/50` → `70/75/80/65/70/50` | 380→410 | 7 | 2 | 11/11 |
| `SANDSLASH` | `75/100/110/45/55/65` → `95/110/110/45/55/65` | 450→480 | 4 | 1 | 4/4 |
| `SANDSLASH_ALOLA` | `75/100/120/25/65/65` → `95/110/120/25/65/65` | 450→480 | 1 | 1 | 0/0 |
| `SAWSBUCK_SPRING` | `80/100/70/60/70/95` → `80/110/70/60/70/105` | 475→495 | 5 | 1 | 0/0 |
| `SAWSBUCK_AUTUMN` | `80/100/70/60/70/95` → `80/110/70/60/70/105` | 475→495 | 1 | 1 | 0/0 |
| `SAWSBUCK_SUMMER` | `80/100/70/60/70/95` → `80/110/70/60/70/105` | 475→495 | 0 | 1 | 0/0 |
| `SAWSBUCK_WINTER` | `80/100/70/60/70/95` → `80/110/70/60/70/105` | 475→495 | 0 | 1 | 0/0 |
| `SHARPEDO` | `70/120/40/95/40/95` → `70/120/40/115/40/95` | 460→480 | 9 | 2 | 16/5 |
| `SHIFTRY` | `90/100/60/90/60/80` → `90/110/60/100/60/80` | 480→500 | 6 | 2 | 0/0 |
| `SKUNTANK` | `103/93/67/71/61/84` → `103/113/67/71/61/84` | 479→499 | 6 | 1 | 0/0 |
| `SUDOWOODO` | `70/100/115/30/65/30` → `70/110/125/30/65/30` | 410→430 | 2 | 1 | 0/0 |
| `SWANNA` | `75/87/63/87/63/98` → `75/87/63/107/63/103` | 473→498 | 6 | 2 | 0/0 |
| `SWELLOW` | `60/85/60/75/50/125` → `60/85/60/85/50/125` | 455→465 | 14 | 1 | 1/1 |
| `SWOOBAT` | `67/57/55/77/55/114` → `67/57/55/97/55/114` | 425→445 | 4 | 1 | 2/2 |
| `TURTONATOR` | `60/78/135/91/85/36` → `60/78/135/111/85/36` | 485→505 | 2 | 1 | 23/11 |
| `VENOMOTH` | `70/65/60/90/75/90` → `70/65/60/100/75/100` | 450→470 | 2 | 1 | 13/5 |
| `WHISCASH` | `110/78/73/76/71/60` → `110/98/73/76/71/60` | 468→488 | 6 | 1 | 25/9 |
| `XATU` | `65/75/70/95/70/95` → `65/75/70/115/70/105` | 470→500 | 5 | 1 | 2/2 |

This batch is numerically conservative, not automatically balance-proven. Ability multipliers still matter: Water Bubble Araquanid, Skill Link Cinccino, Speed Boost Sharpedo, Prankster Sableye, and the many Mega-capable bases need damage or durability checks even when the raw gain is small.

### Review 2: large rescue candidates (29)

These stay under 500 target BST and are currently at or below 450 BST, but gain more than 30 total points. They fit the design goal, yet each requires a deliberate competitive judgment rather than automatic import.

| Species | Current → IE target | BST gain | Primary risk |
|---|---|---:|---|
| `ARBOK` | `60/95/69/65/79/80` → `80/105/79/65/79/80` | +40 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `ARIADOS` | `70/90/70/60/70/40` → `80/110/80/60/80/40` | +50 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `AUDINO` | `103/60/86/60/86/50` → `103/60/96/80/96/50` | +40 | Regenerator/support bulk plus a Mega form; test attrition and Mega transition. |
| `CASTFORM_NORMAL` | `70/70/70/70/70/70` → `70/70/70/100/70/100` | +60 | All weather forms must remain stat-consistent and Forecast-safe. |
| `CHERRIM_OVERCAST` | `70/60/70/87/78/85` → `70/100/70/87/78/85` | +40 | Sunshine form has a separate, much larger Inclement redesign. |
| `CORSOLA` | `65/55/95/65/95/35` → `85/55/95/85/95/35` | +40 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `DEDENNE` | `67/58/57/81/67/101` → `67/58/57/101/87/101` | +40 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `DELCATTY` | `70/65/65/55/55/90` → `70/85/65/85/55/110` | +70 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `DELIBIRD` | `45/55/45/65/45/75` → `45/85/45/65/45/95` | +50 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `FARFETCHD` | `52/90/55/58/62/60` → `52/90/55/58/62/100` | +40 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `FURRET` | `85/76/64/45/55/90` → `85/76/64/45/75/110` | +40 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `GUMSHOOS` | `88/110/60/55/60/45` → `88/110/80/55/80/45` | +40 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `ILLUMISE` | `65/47/75/73/85/85` → `65/47/75/93/85/105` | +40 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `LINOONE` | `78/70/61/50/61/100` → `78/100/61/50/61/110` | +40 | Belly Drum plus priority can multiply an apparently modest offensive gain. |
| `MIGHTYENA` | `70/90/70/60/60/70` → `70/110/70/60/60/90` | +40 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `PACHIRISU` | `60/45/70/45/90/95` → `60/45/70/75/110/95` | +50 | Already has a proven doubles support niche; bulk changes affect redirection survival. |
| `PARASECT` | `60/95/80/60/80/30` → `80/115/95/60/95/30` | +70 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `PERSIAN` | `65/70/60/65/65/115` → `65/70/60/105/65/115` | +40 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `PERSIAN_ALOLA` | `65/60/60/75/65/115` → `65/60/70/105/65/115` | +40 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `SHEDINJA` | `1/90/45/30/30/40` → `1/100/45/100/30/40` | +80 | Wonder Guard makes ordinary BST reasoning invalid; +70 SpA creates a second real offense. |
| `SHIINOTIC` | `60/45/80/90/100/30` → `80/45/80/100/110/30` | +40 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `SPINDA` | `60/60/60/60/60/60` → `60/90/60/90/60/90` | +90 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `SUNFLORA` | `75/75/55/105/85/30` → `95/75/55/125/85/30` | +40 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `TOGEDEMARU` | `65/98/63/40/73/96` → `65/108/83/40/73/106` | +40 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `VOLBEAT` | `65/73/75/47/85/85` → `65/73/85/47/85/115` | +40 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `WATCHOG` | `60/85/69/60/69/77` → `75/95/79/60/79/77` | +45 | Large but pure positive rescue; validate damage, bulk, and role compression. |
| `WORMADAM_PLANT` | `60/59/85/79/105/36` → `80/59/85/99/105/36` | +40 | All three cloaks must be evaluated as a family. |
| `WORMADAM_SANDY` | `60/79/105/59/85/36` → `80/99/105/59/85/36` | +40 | All three cloaks must be evaluated as a family. |
| `WORMADAM_TRASH` | `60/69/95/69/95/36` → `80/69/105/69/105/36` | +40 | All three cloaks must be evaluated as a family. |

## Evolution and Mega constraints

- **Do not automatically port the 16 Inclement entries that are not final-stage in the current roster:** Boldore, Charjabug, Cherubi, Dunsparce, Girafarig, Jigglypuff, Ledyba, Pidgeotto, Pidgey, the four Pumpkaboo sizes, Roggenrola, Stantler, and Ursaring. Dunsparce, Girafarig, Stantler, and Ursaring were final forms in Inclement’s design context but now lead into Gen 9 evolutions; the others were already intermediates. All are Eviolite-capable and need family-level analysis.
- **Thirteen proposed base species have a current Mega:** Absol, Beedrill, Chimecho, Golurk, Lopunny, Manectric, Mawile, Meganium, Pidgeot, Pyroar, Raichu, Sableye, and Sharpedo. Their base-to-Mega stat deltas must be recomputed after the port.
- **HP must match across battle forms.** Chimecho (75→85), Mawile (50→70), and Sableye (50→70) change base HP in the proposed batch. Their Mega forms must receive the same HP value or the base species must be held. Do not import every Inclement Mega stat as a shortcut; pair only the required HP change, then review the Mega’s intended total and Champions identity explicitly.
- **Do not import Inclement’s four absent custom Mega stat blocks by implication.** They are separate content/design decisions, not missing lines in this port.

## Expected campaign impact

The proposed 102-species first port is not a cosmetic data change:

- 398 authored trainer Pokémon across 287 unique trainer branches use at least one proposed species.
- 122 current tutor presets cover the proposed species. Pure-buff entries retain their orientation; redistributed Ability-coupled entries must be checked against nature, stat points, and all four moves.
- 468 wild slots across 116 maps use proposed species. Because wild catches receive competitive presets, every one becomes immediately player-usable at the new power level.
- Highest trainer exposure includes Swellow (14 authored entries), Sharpedo (9), and Araquanid, Dodrio, Gorebyss, Liepard, Manectric, Raichu, and Sableye (7 each).
- Highest wild exposure includes Fearow (49 slots/13 maps), Dewgong (45/9), Raticate (31/8), Whiscash (25/9), Turtonator (23/11), Kingler (22/21), and Wailord (22/8). These should be the first playtest probes because their changes will be encountered often.

A stat buff applies symmetrically in the engine but not symmetrically in experience: a route trainer has a fixed team, while the player can draft any available species and set. The trainer audit must therefore reconsider both the battle’s own power and whether the new wild availability gives the player a dominant answer too early.

## Implementation and gate plan

1. Commit a machine-readable manifest containing the chosen species, exact six-stat target, source commit, decision class, and any paired-form rule. The manifest—not this prose document—becomes the stat-port source of truth.
2. Apply the 29 Ability-coupled species and 73 conservative species in one coherent batch. Apply Chimecho/Mawile/Sableye only with their paired Mega HP decision. Do not touch the 29 rescue or 133 held entries in that batch.
3. Add `verify_species_stat_policy.py` to preprocess the configured `gSpeciesInfo` exactly as the ROM does and fail if any manifest value drifts. It must also reject an unmanifested stat change to a legendary, mythical, Ultra Beast, Paradox Pokémon, or Mega.
4. Add form invariants: every form-change family must preserve base HP where the battle engine assumes it, every Mega must have the reviewed base-to-Mega delta, and no current evolution-stage change may silently make an Inclement final-form buff Eviolite-eligible.
5. Re-run the 122 affected tutor presets through legality plus nature/move/stat-point checks. Add a check for a nature lowering the primary attacking category and explicitly resolve the Flygon alternative.
6. Re-audit all 398 affected trainer entries from `trainers.party`: actual Ability, item, nature, stat points, move categories, speed order, weather/terrain partner, and cap-relative damage. Recompute—not copy—the battle’s difficulty appraisal.
7. Validate the 468 affected wild slots and their preset assignment path. Catch at least one one-set, two-set, and three-set species and prove equal preset selection, held item, Ability, nature, moves, and Leveler compatibility.
8. Run the complete release verifier, all battle tests, a clean release build, deterministic damage/speed spot checks for multiplier-sensitive species, headless mGBA boot, and a fresh-save smoke path through the first three badges before calling the port complete.
9. Only after observed playtest evidence should Review 2 rescue species be admitted one at a time or in small themed batches. Difficulty labels and static legality are not observation.

## Full exact Inclement-authored stat ledger (264 species/forms)

Stat order is **HP/Atk/Def/SpA/SpD/Spe**. “IE delta” is Inclement standard→Inclement rebalanced; “current→target” captures later official/current drift as well. Exposure is authored trainer Pokémon / tutor presets / wild slots.

| Inclement source key → current key | IE delta | Current → IE target | BST | Decision | Exposure |
|---|---|---|---:|---|---:|
| `ABOMASNOW` | Atk 92→102; SpA 92→102 | `90/92/75/92/85/60` → `90/102/75/102/85/60` | 494→514 | HOLD — high power/large delta | 3 / 2 / 0 |
| `ABOMASNOW_MEGA` | Atk 132→142; SpA 132→142 | `90/132/105/132/105/30` → `90/142/105/142/105/30` | 594→614 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `ABSOL` | Def 60→65; SpD 60→65; Spe 75→85 | `65/130/60/75/60/75` → `65/130/65/75/65/85` | 465→485 | PORT 1 — conservative | 5 / 2 / 1 |
| `ABSOL_MEGA` | Atk 150→160; SpA 115→125 | `65/150/60/115/60/115` → `65/160/60/125/60/115` | 565→585 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `ALTARIA` | Atk 70→90 | `75/70/90/70/105/80` → `75/90/90/70/105/80` | 490→510 | HOLD — high power/large delta | 7 / 2 / 2 |
| `ALTARIA_MEGA` | Spe 80→100 | `75/110/110/110/105/80` → `75/110/110/110/105/100` | 590→610 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `AMBIPOM` | Spe 115→125 | `75/100/66/60/66/115` → `75/100/66/60/66/125` | 482→492 | PORT 1 — conservative | 3 / 1 / 2 |
| `ARAQUANID` | Atk 70→90 | `68/70/92/50/132/42` → `68/90/92/50/132/42` | 454→474 | PORT 1 — conservative | 7 / 1 / 0 |
| `ARBOK` | HP 60→80; Atk 95→105; Def 69→79 | `60/95/69/65/79/80` → `80/105/79/65/79/80` | 448→488 | REVIEW 2 — rescue | 3 / 1 / 9 |
| `ARIADOS` | HP 70→80; Atk 90→110; Def 70→80; SpD 70→80 | `70/90/70/60/70/40` → `80/110/80/60/80/40` | 400→450 | REVIEW 2 — rescue | 2 / 1 / 2 |
| `ARMALDO` | Def 100→120 | `75/125/100/70/80/45` → `75/125/120/70/80/45` | 495→515 | HOLD — high power/large delta | 0 / 1 / 0 |
| `ARTICUNO` | SpA 95→125; SpD 125→95 | `90/85/100/95/125/85` → `90/85/100/125/95/85` | 580→580 | HOLD — Mega/legendary | 2 / 1 / 0 |
| `AUDINO` | Def 86→96; SpA 60→80; SpD 86→96 | `103/60/86/60/86/50` → `103/60/96/80/96/50` | 445→485 | REVIEW 2 — rescue | 4 / 2 / 0 |
| `AUDINO_MEGA` | SpA 80→120 | `103/60/126/80/126/50` → `103/60/126/120/126/50` | 545→585 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `AURORUS` | SpA 99→119 | `123/77/72/99/92/58` → `123/77/72/119/92/58` | 521→541 | HOLD — high power/large delta | 1 / 1 / 0 |
| `BANETTE` | Atk 115→125; Def 65→85; SpA 83→65; Spe 65→83 | `64/115/65/83/63/65` → `64/125/85/65/63/83` | 455→485 | HOLD — role change | 4 / 1 / 8 |
| `BANETTE_MEGA` | Def 75→95; SpA 93→75; Spe 75→103 | `64/165/75/93/83/75` → `64/165/95/75/83/103` | 555→585 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `BASCULIN` → `BASCULIN_RED_STRIPED` | Spe 98→118 | `70/92/65/80/55/98` → `70/92/65/80/55/118` | 460→480 | PORT 1 — conservative | 5 / 2 / 9 |
| `BASCULIN_BLUE_STRIPED` | Spe 98→118 | `70/92/65/80/55/98` → `70/92/65/80/55/118` | 460→480 | PORT 1 — conservative | 0 / 1 / 0 |
| `BASTIODON` | HP 60→80 | `60/52/168/47/138/30` → `80/52/168/47/138/30` | 495→515 | HOLD — high power/large delta | 1 / 1 / 0 |
| `BEARTIC` | Spe 50→70 | `95/130/80/70/80/50` → `95/130/80/70/80/70` | 505→525 | HOLD — high power/large delta | 6 / 1 / 1 |
| `BEAUTIFLY` | Atk 70→50; Def 50→60; SpA 100→110; SpD 50→60; Spe 65→85 | `60/70/50/100/50/65` → `60/50/60/110/60/85` | 395→425 | PORT 1 — ability coupled | 2 / 1 / 0 |
| `BEEDRILL` | Atk 90→100; Spe 70→90 | `65/90/40/45/80/75` → `65/100/40/45/80/90` | 395→420 | PORT 1 — conservative | 4 / 1 / 1 |
| `BEEDRILL_MEGA` | Def 40→60; SpD 80→90 | `65/150/40/15/80/145` → `65/150/60/15/90/145` | 495→525 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `BELLOSSOM` | HP 75→90; Atk 80→60; SpA 90→110 | `75/80/95/90/100/50` → `90/60/95/110/100/50` | 490→505 | HOLD — role change | 4 / 1 / 0 |
| `BIBAREL` | HP 79→89; Def 60→80 | `79/85/60/55/60/71` → `89/85/80/55/60/71` | 410→440 | PORT 1 — conservative | 3 / 1 / 0 |
| `BOLDORE` | Atk 105→50; SpA 50→105 | `70/105/105/50/40/20` → `70/50/105/105/40/20` | 390→390 | HOLD — now evolves | 0 / 1 / 25 |
| `BUTTERFREE` | SpA 90→100; Spe 70→90 | `60/45/50/90/80/70` → `60/45/50/100/80/90` | 395→425 | PORT 1 — conservative | 4 / 1 / 5 |
| `BUTTERFREE_MEGA` | SpA 150→160; SpD 90→120 | absent → `60/15/60/160/120/110` | —→525 | HOLD — absent form | 0 / 0 / 0 |
| `CACTURNE` | Spe 55→85 | `70/115/60/115/60/55` → `70/115/60/115/60/85` | 475→505 | PORT 1 — conservative | 3 / 1 / 0 |
| `CAMERUPT` | HP 70→90; Def 70→80; SpD 75→85 | `70/100/70/105/75/40` → `90/100/80/105/85/40` | 460→500 | HOLD — high power/large delta | 6 / 1 / 0 |
| `CAMERUPT_MEGA` | HP 70→90; Def 100→110; SpD 105→115 | `70/120/100/145/105/20` → `90/120/110/145/115/20` | 560→600 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `CARNIVINE` | Def 72→92; SpD 72→92 | `74/100/72/90/72/46` → `74/100/92/90/92/46` | 454→494 | HOLD — high power/large delta | 1 / 1 / 2 |
| `CARRACOSTA` | Atk 108→128 | `74/108/133/83/65/32` → `74/128/133/83/65/32` | 495→515 | HOLD — high power/large delta | 7 / 1 / 0 |
| `CASTFORM` → `CASTFORM_NORMAL` | SpA 70→100; Spe 70→100 | `70/70/70/70/70/70` → `70/70/70/100/70/100` | 420→480 | REVIEW 2 — rescue | 9 / 1 / 0 |
| `CHARJABUG` | Atk 82→102 | `57/82/95/55/75/36` → `57/102/95/55/75/36` | 400→420 | HOLD — now evolves | 2 / 1 / 0 |
| `CHATOT` | SpA 92→102; Spe 91→101 | `76/65/45/92/42/91` → `76/65/45/102/42/101` | 411→431 | PORT 1 — conservative | 5 / 1 / 2 |
| `CHERRIM` → `CHERRIM_OVERCAST` | Atk 60→100 | `70/60/70/87/78/85` → `70/100/70/87/78/85` | 450→490 | REVIEW 2 — rescue | 2 / 1 / 0 |
| `CHERRIM_SUNSHINE` | Atk 60→100; Spe 85→115 | `70/60/70/87/78/85` → `70/100/70/87/78/115` | 450→520 | HOLD — high power/large delta | 0 / 0 / 0 |
| `CHERUBI` | Atk 35→62 | `45/35/45/62/53/35` → `45/62/45/62/53/35` | 275→302 | HOLD — now evolves | 1 / 1 / 4 |
| `CHIMECHO` | HP 75→85; Def 80→90; SpD 90→100 | `75/50/80/95/90/65` → `85/50/90/95/100/65` | 455→485 | PORT 1 — conservative | 3 / 1 / 2 |
| `CINCCINO` | Atk 95→115 | `75/95/60/65/60/115` → `75/115/60/65/60/115` | 470→490 | PORT 1 — conservative | 3 / 1 / 0 |
| `CLAWITZER` | Def 88→108 | `71/73/88/120/89/59` → `71/73/108/120/89/59` | 500→520 | HOLD — high power/large delta | 6 / 1 / 4 |
| `CLAYDOL` | HP 60→70; SpA 70→90 | `60/70/105/70/120/75` → `70/70/105/90/120/75` | 500→530 | HOLD — high power/large delta | 6 / 1 / 17 |
| `CORSOLA` | HP 65→85; SpA 65→85 | `65/55/95/65/95/35` → `85/55/95/85/95/35` | 410→450 | REVIEW 2 — rescue | 2 / 1 / 8 |
| `CRABOMINABLE` | Def 77→97; SpD 67→87 | `97/132/77/62/67/43` → `97/132/97/62/87/43` | 478→518 | HOLD — high power/large delta | 5 / 2 / 0 |
| `CRADILY` | HP 86→96; SpA 81→91 | `86/81/97/81/107/43` → `96/81/97/91/107/43` | 495→515 | HOLD — high power/large delta | 2 / 1 / 0 |
| `CRUSTLE` | Atk 105→115; Def 125→135 | `70/105/125/65/75/45` → `70/115/135/65/75/45` | 485→505 | PORT 1 — conservative | 3 / 1 / 6 |
| `DECIDUEYE` | SpA 100→70; SpD 100→90; Spe 70→110 | `78/107/75/100/100/70` → `78/107/75/70/90/110` | 530→530 | PORT 1 — ability coupled | 1 / 1 / 0 |
| `DEDENNE` | SpA 81→101; SpD 67→87 | `67/58/57/81/67/101` → `67/58/57/101/87/101` | 431→471 | REVIEW 2 — rescue | 3 / 1 / 2 |
| `DELCATTY` | Atk 65→85; SpA 55→85; Spe 90→110 | `70/65/65/55/55/90` → `70/85/65/85/55/110` | 400→470 | REVIEW 2 — rescue | 2 / 1 / 0 |
| `DELIBIRD` | Atk 55→85; Spe 75→95 | `45/55/45/65/45/75` → `45/85/45/65/45/95` | 330→380 | REVIEW 2 — rescue | 1 / 1 / 14 |
| `DEWGONG` | Atk 70→90; Def 80→60; SpA 70→95; Spe 70→90 | `90/70/80/70/95/70` → `90/90/60/95/95/90` | 475→520 | PORT 1 — ability coupled | 2 / 1 / 45 |
| `DHELMISE` | Def 100→120; SpA 86→76 | `70/131/100/86/90/40` → `70/131/120/76/90/40` | 517→527 | HOLD — role change | 9 / 1 / 16 |
| `DODRIO` | HP 60→80 | `60/110/70/60/60/110` → `80/110/70/60/60/110` | 470→490 | PORT 1 — ability coupled | 7 / 2 / 2 |
| `DONPHAN` | Atk 120→130; Def 120→130 | `90/120/120/60/60/50` → `90/130/130/60/60/50` | 500→520 | HOLD — high power/large delta | 3 / 1 / 4 |
| `DRAPION` | Atk 90→100; Def 110→120 | `70/90/110/60/75/95` → `70/100/120/60/75/95` | 500→520 | HOLD — high power/large delta | 5 / 1 / 0 |
| `DRIFBLIM` | Def 44→54; SpD 54→74 | `150/80/44/90/54/80` → `150/80/54/90/74/80` | 498→528 | HOLD — high power/large delta | 4 / 1 / 4 |
| `DRUDDIGON` | HP 77→97 | `77/120/90/60/90/48` → `97/120/90/60/90/48` | 485→505 | PORT 1 — conservative | 3 / 1 / 13 |
| `DUNSPARCE` | HP 100→110; Atk 70→80; Def 70→80; SpD 65→75 | `100/70/70/65/65/45` → `110/80/80/65/75/45` | 415→455 | HOLD — now evolves | 2 / 1 / 25 |
| `DURANT` | SpD 48→78 | `58/109/112/48/48/109` → `58/109/112/48/78/109` | 484→514 | PORT 1 — conservative | 4 / 1 / 2 |
| `DUSKNOIR` | Atk 100→120; SpA 65→60 | `45/100/135/65/135/45` → `45/120/135/60/135/45` | 525→540 | HOLD — role change | 2 / 2 / 0 |
| `DUSTOX` | HP 60→80; Atk 50→30; Def 70→90; SpA 50→70 | `60/50/70/50/90/65` → `80/30/90/70/90/65` | 385→425 | PORT 1 — ability coupled | 3 / 1 / 0 |
| `EELEKTROSS` | Def 80→90; SpD 80→90 | `85/115/80/105/80/50` → `85/115/90/105/90/50` | 515→535 | HOLD — high power/large delta | 6 / 2 / 4 |
| `EMBOAR` | Def 65→80; SpA 100→70; SpD 65→80 | `110/123/65/100/65/65` → `110/123/80/70/80/65` | 528→528 | HOLD — role change | 2 / 1 / 2 |
| `EMOLGA` | SpA 75→105; Spe 103→113 | `55/75/60/75/60/103` → `55/75/60/105/60/113` | 428→468 | PORT 1 — ability coupled | 4 / 1 / 2 |
| `EXPLOUD` | SpA 91→111; SpD 73→83 | `104/91/63/91/73/68` → `104/91/63/111/83/68` | 490→520 | HOLD — high power/large delta | 6 / 1 / 1 |
| `FARFETCHD` | Spe 60→100 | `52/90/55/58/62/60` → `52/90/55/58/62/100` | 377→417 | REVIEW 2 — rescue | 1 / 1 / 1 |
| `FEAROW` | Atk 90→110; Def 65→75; SpD 61→71 | `65/90/65/61/61/100` → `65/110/75/61/71/100` | 442→482 | PORT 1 — ability coupled | 3 / 1 / 49 |
| `FERALIGATR` | SpA 79→69; Spe 78→88 | `85/105/100/79/83/78` → `85/105/100/69/83/88` | 530→530 | HOLD — role change | 5 / 2 / 0 |
| `FLAREON` | HP 65→95; SpA 95→65; SpD 110→65; Spe 65→110 | `65/130/60/95/110/65` → `95/130/60/65/65/110` | 525→525 | HOLD — role change | 3 / 1 / 0 |
| `FLOATZEL` | Atk 105→120; SpA 85→100 | `85/105/55/85/50/115` → `85/120/55/100/50/115` | 495→525 | HOLD — high power/large delta | 6 / 2 / 8 |
| `FLYGON` | SpA 80→100 | `80/100/80/80/80/100` → `80/100/80/100/80/100` | 520→540 | PORT 1 — ability coupled | 5 / 2 / 0 |
| `FLYGON_MEGA` | Def 90→100; SpD 90→100 | absent → `80/130/100/120/100/110` | —→640 | HOLD — absent form | 0 / 0 / 0 |
| `FORRETRESS` | Atk 90→100; SpD 60→80; Spe 40→20 | `75/90/140/60/60/40` → `75/100/140/60/80/20` | 465→475 | HOLD — role change | 2 / 1 / 1 |
| `FROSLASS` | Atk 80→70; SpA 80→110 | `70/80/70/80/70/110` → `70/70/70/110/70/110` | 480→500 | HOLD — role change | 4 / 2 / 0 |
| `FURRET` | SpD 55→75; Spe 90→110 | `85/76/64/45/55/90` → `85/76/64/45/75/110` | 415→455 | REVIEW 2 — rescue | 1 / 1 / 0 |
| `GALLADE` | SpA 65→60; Spe 80→100 | `68/125/65/65/115/80` → `68/125/65/60/115/100` | 518→533 | HOLD — role change | 5 / 2 / 0 |
| `GALLADE_MEGA` | SpD 115→125; Spe 110→115 | `68/165/95/65/115/110` → `68/165/95/65/125/115` | 618→633 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `GALVANTULA` | SpA 97→107 | `70/77/60/97/60/108` → `70/77/60/107/60/108` | 472→482 | PORT 1 — conservative | 5 / 1 / 0 |
| `GARBODOR` | Def 82→102 | `80/95/82/60/82/75` → `80/95/102/60/82/75` | 474→494 | PORT 1 — conservative | 2 / 1 / 0 |
| `GARCHOMP_MEGA` | Def 115→110; SpD 95→90; Spe 92→102 | `108/170/115/120/95/92` → `108/170/110/120/90/102` | 700→700 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `GARDEVOIR` | Atk 65→60; Spe 80→100 | `68/65/65/125/115/80` → `68/60/65/125/115/100` | 518→533 | HOLD — role change | 6 / 2 / 0 |
| `GARDEVOIR_MEGA` | Def 65→80 | `68/85/65/165/135/100` → `68/85/80/165/135/100` | 618→633 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `GIGALITH` | Atk 135→60; SpA 60→135 | `85/135/130/60/80/25` → `85/60/130/135/80/25` | 515→515 | HOLD — role change | 5 / 1 / 9 |
| `GIRAFARIG` | Atk 80→90; SpA 90→100; Spe 85→105 | `70/80/65/90/65/85` → `70/90/65/100/65/105` | 455→495 | HOLD — now evolves | 3 / 1 / 2 |
| `GLALIE` | Atk 80→100; Def 80→70; SpD 80→70; Spe 80→100 | `80/80/80/80/80/80` → `80/100/70/80/70/100` | 480→500 | HOLD — role change | 1 / 2 / 0 |
| `GLALIE_MEGA` | Atk 120→130; Spe 100→110 | `80/120/80/120/80/100` → `80/130/80/120/80/110` | 580→600 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `GOGOAT` | SpA 97→79; Spe 68→95 | `123/100/62/97/81/68` → `123/100/62/79/81/95` | 531→540 | PORT 1 — ability coupled | 3 / 1 / 0 |
| `GOLDUCK` | Atk 82→102; SpA 95→105 | `80/82/78/95/80/85` → `80/102/78/105/80/85` | 500→530 | HOLD — high power/large delta | 6 / 1 / 21 |
| `GOLEM` | HP 80→90; Atk 120→135 | `80/120/130/55/65/45` → `90/135/130/55/65/45` | 495→520 | HOLD — high power/large delta | 2 / 2 / 3 |
| `GOLEM_ALOLAN` → `GOLEM_ALOLA` | HP 80→90; Atk 120→135 | `80/120/130/55/65/45` → `90/135/130/55/65/45` | 495→520 | HOLD — high power/large delta | 3 / 1 / 0 |
| `GOLURK` | Atk 124→134; Def 80→90; SpD 80→90 | `89/124/80/55/80/55` → `89/134/90/55/90/55` | 483→513 | PORT 1 — conservative | 5 / 2 / 9 |
| `GOODRA` | Def 70→85; SpD 150→135 | `90/100/70/110/150/80` → `90/100/85/110/135/80` | 600→600 | PORT 1 — ability coupled | 4 / 2 / 0 |
| `GOREBYSS` | SpA 114→124; Spe 52→72 | `55/84/105/114/75/52` → `55/84/105/124/75/72` | 485→515 | PORT 1 — conservative | 7 / 1 / 2 |
| `GOURGEIST` → `GOURGEIST_AVERAGE` | Atk 90→58; SpA 58→90 | `65/90/122/58/75/84` → `65/58/122/90/75/84` | 494→494 | PORT 1 — ability coupled | 2 / 2 / 0 |
| `GOURGEIST_LARGE` | Atk 95→58; SpA 58→95 | `75/95/122/58/75/69` → `75/58/122/95/75/69` | 494→494 | PORT 1 — ability coupled | 1 / 1 / 0 |
| `GOURGEIST_SMALL` | Atk 85→58; SpA 58→85 | `55/85/122/58/75/99` → `55/58/122/85/75/99` | 494→494 | PORT 1 — ability coupled | 1 / 1 / 0 |
| `GOURGEIST_SUPER` | Atk 100→58; SpA 58→100 | `85/100/122/58/75/54` → `85/58/122/100/75/54` | 494→494 | PORT 1 — ability coupled | 2 / 1 / 0 |
| `GRANBULL` | Atk 120→130; Spe 45→65 | `90/120/75/60/60/45` → `90/130/75/60/60/65` | 450→480 | PORT 1 — conservative | 1 / 1 / 3 |
| `GRUMPIG` | HP 80→100; SpA 90→100 | `80/45/65/90/110/80` → `100/45/65/100/110/80` | 470→500 | PORT 1 — conservative | 3 / 1 / 1 |
| `GUMSHOOS` | Def 60→80; SpD 60→80 | `88/110/60/55/60/45` → `88/110/80/55/80/45` | 418→458 | REVIEW 2 — rescue | 1 / 1 / 1 |
| `HARIYAMA` | Atk 120→130; Def 60→70; SpD 60→70 | `144/120/60/40/60/50` → `144/130/70/40/70/50` | 474→504 | PORT 1 — conservative | 6 / 2 / 0 |
| `HEATMOR` | Def 66→96; SpA 105→85; SpD 66→96 | `85/97/66/105/66/65` → `85/97/96/85/96/65` | 484→524 | PORT 1 — ability coupled | 3 / 1 / 0 |
| `HELIOLISK` | SpA 109→119 | `62/55/52/109/94/109` → `62/55/52/119/94/109` | 481→491 | PORT 1 — conservative | 6 / 1 / 1 |
| `HITMONCHAN` | HP 50→80 | `50/105/79/35/110/76` → `80/105/79/35/110/76` | 455→485 | PORT 1 — conservative | 2 / 1 / 0 |
| `HITMONLEE` | HP 50→80 | `50/120/53/35/110/87` → `80/120/53/35/110/87` | 455→485 | PORT 1 — conservative | 1 / 1 / 0 |
| `HITMONTOP` | HP 50→80 | `50/95/95/35/110/70` → `80/95/95/35/110/70` | 455→485 | PORT 1 — conservative | 2 / 1 / 0 |
| `HOUNDOOM` | Spe 95→105 | `75/90/50/110/80/95` → `75/90/50/110/80/105` | 500→510 | HOLD — high power/large delta | 3 / 2 / 1 |
| `HOUNDOOM_MEGA` | Atk 90→100 | `75/90/90/140/90/115` → `75/100/90/140/90/115` | 600→610 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `HUNTAIL` | Atk 104→114; Spe 52→72 | `55/104/105/94/75/52` → `55/114/105/94/75/72` | 485→515 | PORT 1 — conservative | 5 / 1 / 2 |
| `HYPNO` | HP 85→95; SpA 73→115 | `85/73/70/73/115/67` → `95/73/70/115/115/67` | 483→535 | HOLD — high power/large delta | 4 / 1 / 5 |
| `ILLUMISE` | SpA 73→93; Spe 85→105 | `65/47/75/73/85/85` → `65/47/75/93/85/105` | 430→470 | REVIEW 2 — rescue | 2 / 1 / 1 |
| `JIGGLYPUFF` | HP 115→105; SpA 45→65; SpD 25→35 | `115/45/20/45/25/20` → `105/45/20/65/35/20` | 270→290 | HOLD — now evolves | 2 / 1 / 7 |
| `JUMPLUFF` | Atk 55→75; Spe 110→130 | `75/55/70/55/95/110` → `75/75/70/55/95/130` | 460→500 | HOLD — high power/large delta | 2 / 2 / 0 |
| `JYNX` | Spe 95→115 | `65/50/35/115/95/95` → `65/50/35/115/95/115` | 455→475 | PORT 1 — conservative | 5 / 1 / 1 |
| `KECLEON` | HP 60→80; Atk 90→100 | `60/90/70/60/120/40` → `80/100/70/60/120/40` | 440→470 | PORT 1 — conservative | 3 / 1 / 0 |
| `KINGLER` | HP 55→80 | `55/130/115/50/50/75` → `80/130/115/50/50/75` | 475→500 | PORT 1 — conservative | 3 / 1 / 22 |
| `KINGLER_MEGA` | HP 55→80 | absent → `80/170/135/50/80/85` | —→600 | HOLD — absent form | 0 / 0 / 0 |
| `KOMALA` | HP 65→85; Def 65→95; SpA 75→55 | `65/115/65/75/95/65` → `85/115/95/55/95/65` | 480→510 | HOLD — role change | 3 / 1 / 1 |
| `KRICKETUNE` | Atk 85→65; SpA 55→100; Spe 65→85 | `77/85/51/55/51/65` → `77/65/51/100/51/85` | 384→429 | HOLD — role change | 5 / 1 / 0 |
| `LANTURN` | SpA 76→86; SpD 76→86 | `125/58/58/76/76/67` → `125/58/58/86/86/67` | 460→480 | PORT 1 — conservative | 6 / 2 / 3 |
| `LAPRAS` | SpA 85→95 | `130/85/80/85/95/60` → `130/85/80/95/95/60` | 535→545 | HOLD — high power/large delta | 8 / 1 / 23 |
| `LAPRAS_MEGA` | SpA 115→125 | absent → `130/105/110/125/125/50` | —→645 | HOLD — absent form | 0 / 0 / 0 |
| `LEAVANNY` | Atk 103→113; Spe 92→102 | `75/103/80/70/80/92` → `75/113/80/70/80/102` | 500→520 | HOLD — high power/large delta | 2 / 2 / 0 |
| `LEDIAN` | Atk 35→95; SpA 55→35; Spe 85→105 | `55/35/50/55/110/85` → `55/95/50/35/110/105` | 390→450 | HOLD — role change | 1 / 1 / 1 |
| `LEDYBA` | Atk 20→40; SpA 40→20 | `40/20/30/40/80/55` → `40/40/30/20/80/55` | 265→265 | HOLD — now evolves | 0 / 1 / 5 |
| `LICKILICKY` | Atk 85→95; SpA 80→90 | `110/85/95/80/95/50` → `110/95/95/90/95/50` | 515→535 | HOLD — high power/large delta | 6 / 1 / 0 |
| `LIEPARD` | Atk 88→98; SpA 88→98 | `64/88/50/88/50/106` → `64/98/50/98/50/106` | 446→466 | PORT 1 — conservative | 7 / 1 / 1 |
| `LINOONE` | Atk 70→100; Spe 100→110 | `78/70/61/50/61/100` → `78/100/61/50/61/110` | 420→460 | REVIEW 2 — rescue | 3 / 1 / 2 |
| `LOPUNNY` | Atk 76→96 | `65/76/84/54/96/105` → `65/96/84/54/96/105` | 480→500 | PORT 1 — ability coupled | 5 / 2 / 0 |
| `LOPUNNY_MEGA` | SpA 54→74 | `65/136/94/54/96/135` → `65/136/94/74/96/135` | 580→600 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `LUDICOLO` | SpA 90→100; Spe 70→80 | `80/70/70/90/100/70` → `80/70/70/100/100/80` | 480→500 | PORT 1 — conservative | 6 / 2 / 0 |
| `LUMINEON` | SpA 69→109 | `69/69/76/69/86/91` → `69/69/76/109/86/91` | 460→500 | HOLD — high power/large delta | 7 / 1 / 24 |
| `LUNATONE` | SpA 95→125; SpD 85→115 | `90/55/65/95/85/70` → `90/55/65/125/115/70` | 460→520 | HOLD — high power/large delta | 3 / 1 / 13 |
| `LURANTIS` | HP 70→80; Atk 105→115; Def 90→95; SpD 90→95 | `70/105/90/80/90/45` → `80/115/95/80/95/45` | 480→510 | PORT 1 — conservative | 2 / 2 / 2 |
| `LUVDISC` | SpA 40→95 | `43/30/55/40/65/97` → `43/30/55/95/65/97` | 330→385 | PORT 1 — ability coupled | 5 / 1 / 10 |
| `LUXRAY` | HP 80→90; SpA 95→75; Spe 70→90 | `80/120/79/95/79/70` → `90/120/79/75/79/90` | 523→533 | HOLD — role change | 3 / 1 / 0 |
| `LYCANROC` → `LYCANROC_MIDDAY` | Atk 115→125; Def 65→70; SpD 65→70 | `75/115/65/55/65/112` → `75/125/70/55/70/112` | 487→507 | HOLD — high power/large delta | 3 / 2 / 0 |
| `LYCANROC_DUSK` | Atk 117→127; Def 65→70; SpD 65→70 | `75/117/65/55/65/110` → `75/127/70/55/70/110` | 487→507 | HOLD — high power/large delta | 4 / 1 / 0 |
| `LYCANROC_MIDNIGHT` | HP 85→95; Def 75→85 | `85/115/75/55/75/82` → `95/115/85/55/75/82` | 487→507 | HOLD — high power/large delta | 0 / 1 / 0 |
| `MAGMORTAR` | SpD 95→85; Spe 83→93 | `75/95/67/125/95/83` → `75/95/67/125/85/93` | 540→540 | PORT 1 — ability coupled | 6 / 2 / 14 |
| `MANECTRIC` | SpA 105→125 | `70/75/60/105/60/105` → `70/75/60/125/60/105` | 475→495 | PORT 1 — conservative | 7 / 2 / 1 |
| `MANECTRIC_MEGA` | Def 80→90; SpD 80→90 | `70/75/80/135/80/135` → `70/75/90/135/90/135` | 575→595 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `MARACTUS` | Atk 86→56; Def 67→87; SpD 67→87 | `75/86/67/106/67/60` → `75/56/87/106/87/60` | 461→471 | HOLD — role change | 0 / 1 / 1 |
| `MAROWAK` | HP 60→75 | `60/80/110/50/80/45` → `75/80/110/50/80/45` | 425→440 | PORT 1 — conservative | 0 / 1 / 14 |
| `MAROWAK_ALOLAN` → `MAROWAK_ALOLA` | HP 60→75 | `60/80/110/50/80/45` → `75/80/110/50/80/45` | 425→440 | PORT 1 — conservative | 4 / 1 / 1 |
| `MASQUERAIN` | Def 62→72; SpA 100→110 | `70/60/62/100/82/80` → `70/60/72/110/82/80` | 454→474 | PORT 1 — conservative | 3 / 1 / 20 |
| `MAWILE` | HP 50→70; Def 85→95 | `50/85/85/55/55/50` → `70/85/95/55/55/50` | 380→410 | PORT 1 — conservative | 4 / 1 / 8 |
| `MAWILE_MEGA` | HP 50→70; Def 125→130; SpD 95→100 | `50/105/125/55/95/50` → `70/105/130/55/100/50` | 480→510 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `MEGANIUM` | SpA 83→93 | `80/82/100/83/100/80` → `80/82/100/93/100/80` | 525→535 | PORT 1 — ability coupled | 4 / 1 / 0 |
| `MEOWSTIC` → `MEOWSTIC_M` | HP 74→94; SpA 83→103 | `74/48/76/83/81/104` → `94/48/76/103/81/104` | 466→506 | HOLD — high power/large delta | 3 / 2 / 0 |
| `MEOWSTIC_FEMALE` → `MEOWSTIC_F` | SpA 83→123 | `74/48/76/83/81/104` → `74/48/76/123/81/104` | 466→506 | HOLD — high power/large delta | 4 / 1 / 0 |
| `MIGHTYENA` | Atk 90→110; Spe 70→90 | `70/90/70/60/60/70` → `70/110/70/60/60/90` | 420→460 | REVIEW 2 — rescue | 5 / 1 / 1 |
| `MILTANK` | Atk 100→80 | `95/80/105/40/70/100` → `95/80/105/40/70/100` | 490→490 | NO CHANGE — already exact | 5 / 1 / 1 |
| `MINUN` | SpD 85→105; Spe 95→105 | `60/40/50/75/85/95` → `60/40/50/75/105/105` | 405→435 | PORT 1 — conservative | 3 / 1 / 1 |
| `MISMAGIUS` | SpA 105→110; SpD 105→110; Spe 105→110 | `60/60/60/105/105/105` → `60/60/60/110/110/110` | 495→510 | PORT 1 — ability coupled | 4 / 1 / 0 |
| `MOTHIM` → `MOTHIM_PLANT` | Atk 94→54; Def 50→70; SpD 50→70; Spe 66→86 | `70/94/50/94/50/66` → `70/54/70/94/70/86` | 424→444 | HOLD — role change | 2 / 1 / 0 |
| `MR_MIME` | SpA 100→120 | `40/45/65/100/120/90` → `40/45/65/120/120/90` | 460→480 | PORT 1 — conservative | 2 / 1 / 0 |
| `MUK` | Atk 105→125 | `105/105/75/65/100/50` → `105/125/75/65/100/50` | 500→520 | HOLD — high power/large delta | 4 / 1 / 5 |
| `MUSHARNA` | SpA 107→117 | `116/55/85/107/95/29` → `116/55/85/117/95/29` | 487→497 | HOLD — high power/large delta | 6 / 1 / 0 |
| `NINETALES` | SpA 81→91 | `73/76/75/81/100/100` → `73/76/75/91/100/100` | 505→515 | HOLD — high power/large delta | 6 / 2 / 0 |
| `NINETALES_ALOLAN` → `NINETALES_ALOLA` | SpA 81→91 | `73/67/75/81/100/109` → `73/67/75/91/100/109` | 505→515 | HOLD — high power/large delta | 6 / 1 / 0 |
| `NINJASK` | Atk 90→100 | `61/90/45/50/50/160` → `61/100/45/50/50/160` | 456→466 | PORT 1 — conservative | 4 / 1 / 0 |
| `NOCTOWL` | Def 50→70; SpA 86→96 | `100/50/50/86/96/70` → `100/50/70/96/96/70` | 452→482 | PORT 1 — conservative | 5 / 1 / 5 |
| `OCTILLERY` | Atk 105→65; Def 75→95; SpA 105→125; SpD 75→95 | `75/105/75/105/75/45` → `75/65/95/125/95/45` | 480→500 | HOLD — role change | 5 / 1 / 2 |
| `ORICORIO` → `ORICORIO_BAILE` | SpA 98→108; Spe 93→103 | `75/70/70/98/70/93` → `75/70/70/108/70/103` | 476→496 | PORT 1 — conservative | 3 / 1 / 1 |
| `ORICORIO_PAU` | SpA 98→108; Spe 93→103 | `75/70/70/98/70/93` → `75/70/70/108/70/103` | 476→496 | PORT 1 — conservative | 1 / 1 / 0 |
| `ORICORIO_POM_POM` | SpA 98→108; Spe 93→103 | `75/70/70/98/70/93` → `75/70/70/108/70/103` | 476→496 | PORT 1 — conservative | 3 / 1 / 0 |
| `ORICORIO_SENSU` | SpA 98→108; Spe 93→103 | `75/70/70/98/70/93` → `75/70/70/108/70/103` | 476→496 | PORT 1 — conservative | 1 / 1 / 0 |
| `PACHIRISU` | SpA 45→75; SpD 90→110 | `60/45/70/45/90/95` → `60/45/70/75/110/95` | 405→455 | REVIEW 2 — rescue | 2 / 1 / 1 |
| `PALOSSAND` | Def 110→120; SpD 75→85 | `85/75/110/100/75/35` → `85/75/120/100/85/35` | 480→500 | PORT 1 — ability coupled | 5 / 1 / 0 |
| `PARASECT` | HP 60→80; Atk 95→115; Def 80→95; SpD 80→95 | `60/95/80/60/80/30` → `80/115/95/60/95/30` | 405→475 | REVIEW 2 — rescue | 2 / 1 / 17 |
| `PERSIAN` | SpA 65→105 | `65/70/60/65/65/115` → `65/70/60/105/65/115` | 440→480 | REVIEW 2 — rescue | 0 / 1 / 36 |
| `PERSIAN_ALOLAN` → `PERSIAN_ALOLA` | Def 60→70; SpA 75→105 | `65/60/60/75/65/115` → `65/60/70/105/65/115` | 440→480 | REVIEW 2 — rescue | 5 / 1 / 3 |
| `PIDGEOT` | Atk 80→70; SpA 70→95 | `83/80/75/70/70/101` → `83/70/75/95/70/101` | 479→494 | PORT 1 — ability coupled | 5 / 1 / 0 |
| `PIDGEOTTO` | Atk 60→50; SpA 50→60 | `63/60/55/50/50/71` → `63/50/55/60/50/71` | 349→349 | HOLD — now evolves | 0 / 1 / 26 |
| `PIDGEOT_MEGA` | Def 80→95 | `83/80/80/135/80/121` → `83/80/95/135/80/121` | 579→594 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `PIDGEY` | Atk 45→35; SpA 35→45 | `40/45/40/35/35/56` → `40/35/40/45/35/56` | 251→251 | HOLD — now evolves | 0 / 1 / 89 |
| `PLUSLE` | SpA 85→105; Spe 95→105 | `60/50/40/85/75/95` → `60/50/40/105/75/105` | 405→435 | PORT 1 — conservative | 4 / 1 / 1 |
| `POLITOED` | HP 90→110; SpA 90→100 | `90/75/75/90/100/70` → `110/75/75/100/100/70` | 500→530 | HOLD — high power/large delta | 3 / 1 / 1 |
| `POLIWRATH` | Atk 95→115 | `90/95/95/70/90/70` → `90/115/95/70/90/70` | 510→530 | HOLD — high power/large delta | 6 / 1 / 1 |
| `PROBOPASS` | SpA 75→95 | `60/55/145/75/150/40` → `60/55/145/95/150/40` | 525→545 | HOLD — high power/large delta | 3 / 2 / 0 |
| `PUMPKABOO` → `PUMPKABOO_AVERAGE` | Atk 66→44; SpA 44→66 | `49/66/70/44/55/51` → `49/44/70/66/55/51` | 335→335 | HOLD — now evolves | 0 / 1 / 1 |
| `PUMPKABOO_LARGE` | Atk 66→44; SpA 44→66 | `54/66/70/44/55/46` → `49/44/70/66/55/51` | 335→335 | HOLD — now evolves | 0 / 1 / 2 |
| `PUMPKABOO_SMALL` | Atk 66→44; SpA 44→66 | `44/66/70/44/55/56` → `49/44/70/66/55/51` | 335→335 | HOLD — now evolves | 0 / 1 / 1 |
| `PUMPKABOO_SUPER` | Atk 66→44; SpA 44→66 | `59/66/70/44/55/41` → `49/44/70/66/55/51` | 335→335 | HOLD — now evolves | 0 / 1 / 1 |
| `PURUGLY` | Atk 82→102; Def 64→74; SpD 59→69; Spe 112→119 | `71/82/64/64/59/112` → `71/102/74/64/69/119` | 452→499 | HOLD — high power/large delta | 1 / 1 / 0 |
| `PYROAR` | SpA 109→129 | `86/68/72/109/66/106` → `86/68/72/129/66/106` | 507→527 | PORT 1 — ability coupled | 6 / 1 / 0 |
| `QUAGSIRE` | Atk 85→95; Def 85→95 | `95/85/85/65/65/35` → `95/95/95/65/65/35` | 430→450 | PORT 1 — conservative | 6 / 1 / 6 |
| `QWILFISH` | Def 85→115 | `65/95/85/55/55/85` → `65/95/115/55/55/85` | 440→470 | PORT 1 — conservative | 6 / 1 / 11 |
| `RAICHU` | Spe 100→115 | `60/90/55/90/80/110` → `60/90/55/90/80/115` | 485→490 | PORT 1 — conservative | 7 / 3 / 0 |
| `RAICHU_ALOLAN` → `RAICHU_ALOLA` | Spe 100→115 | `60/85/50/95/85/110` → `60/85/50/95/85/115` | 485→490 | PORT 1 — conservative | 3 / 1 / 3 |
| `RAMPARDOS` | Def 60→80 | `97/165/60/65/50/58` → `97/165/80/65/50/58` | 495→515 | HOLD — high power/large delta | 2 / 1 / 0 |
| `RAPIDASH` | HP 65→85; Atk 100→110 | `65/100/70/80/80/105` → `85/110/70/80/80/105` | 500→530 | PORT 1 — ability coupled | 5 / 1 / 8 |
| `RATICATE` | Atk 81→101; Spe 97→102 | `55/81/60/50/70/97` → `55/101/60/50/70/102` | 413→438 | PORT 1 — conservative | 1 / 1 / 31 |
| `RATICATE_ALOLAN` → `RATICATE_ALOLA` | Atk 71→86; Def 70→80 | `75/71/70/40/80/77` → `75/86/80/40/80/77` | 413→438 | PORT 1 — conservative | 2 / 1 / 3 |
| `RELICANTH` | Atk 90→110 | `100/90/130/45/65/55` → `100/110/130/45/65/55` | 485→505 | PORT 1 — conservative | 5 / 1 / 17 |
| `ROGGENROLA` | Atk 75→25; SpA 25→75 | `55/75/85/25/25/15` → `55/25/85/75/25/15` | 280→280 | HOLD — now evolves | 0 / 1 / 2 |
| `SABLEYE` | HP 50→70; Def 75→80; SpD 65→70 | `50/75/75/65/65/50` → `70/75/80/65/70/50` | 380→410 | PORT 1 — conservative | 7 / 2 / 11 |
| `SABLEYE_MEGA` | HP 50→70; Def 125→130; SpD 115→120 | `50/85/125/85/115/20` → `70/85/130/85/120/20` | 480→510 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `SANDSLASH` | HP 75→95; Atk 100→110 | `75/100/110/45/55/65` → `95/110/110/45/55/65` | 450→480 | PORT 1 — conservative | 4 / 1 / 4 |
| `SANDSLASH_ALOLAN` → `SANDSLASH_ALOLA` | HP 75→95; Atk 100→110 | `75/100/120/25/65/65` → `95/110/120/25/65/65` | 450→480 | PORT 1 — conservative | 1 / 1 / 0 |
| `SAWSBUCK` → `SAWSBUCK_SPRING` | Atk 100→110; Spe 95→105 | `80/100/70/60/70/95` → `80/110/70/60/70/105` | 475→495 | PORT 1 — conservative | 5 / 1 / 0 |
| `SAWSBUCK_AUTUMN` | Atk 100→110; Spe 95→105 | `80/100/70/60/70/95` → `80/110/70/60/70/105` | 475→495 | PORT 1 — conservative | 1 / 1 / 0 |
| `SAWSBUCK_SUMMER` | Atk 100→110; Spe 95→105 | `80/100/70/60/70/95` → `80/110/70/60/70/105` | 475→495 | PORT 1 — conservative | 0 / 1 / 0 |
| `SAWSBUCK_WINTER` | Atk 100→110; Spe 95→105 | `80/100/70/60/70/95` → `80/110/70/60/70/105` | 475→495 | PORT 1 — conservative | 0 / 1 / 0 |
| `SEAKING` | Atk 92→112; SpD 80→100; Spe 68→98 | `80/92/65/65/80/68` → `80/112/65/65/100/98` | 450→520 | HOLD — high power/large delta | 7 / 1 / 37 |
| `SEISMITOAD` | Atk 95→105; SpA 85→95 | `105/95/75/85/75/74` → `105/105/75/95/75/74` | 509→529 | HOLD — high power/large delta | 5 / 1 / 4 |
| `SEVIPER` | Spe 65→100 | `73/100/60/100/60/65` → `73/100/60/100/60/100` | 458→493 | HOLD — high power/large delta | 3 / 1 / 13 |
| `SHARPEDO` | SpA 95→115 | `70/120/40/95/40/95` → `70/120/40/115/40/95` | 460→480 | PORT 1 — conservative | 9 / 2 / 16 |
| `SHARPEDO_MEGA` | Atk 140→150; Spe 105→115 | `70/140/70/110/65/105` → `70/150/70/110/65/115` | 560→580 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `SHEDINJA` | Atk 90→100; SpA 30→100 | `1/90/45/30/30/40` → `1/100/45/100/30/40` | 236→316 | REVIEW 2 — rescue | 4 / 1 / 0 |
| `SHIFTRY` | Atk 100→110; SpA 90→100 | `90/100/60/90/60/80` → `90/110/60/100/60/80` | 480→500 | PORT 1 — conservative | 6 / 2 / 0 |
| `SHIINOTIC` | HP 60→80; SpA 90→100; SpD 100→110 | `60/45/80/90/100/30` → `80/45/80/100/110/30` | 405→445 | REVIEW 2 — rescue | 2 / 1 / 1 |
| `SILVALLY` → `SILVALLY_NORMAL` | HP 95→100; Atk 95→100; Def 95→100; SpA 95→100; SpD 95→100; Spe 95→100 | `95/95/95/95/95/95` → `100/100/100/100/100/100` | 570→600 | HOLD — Mega/legendary | 1 / 2 / 0 |
| `SIMIPOUR` | Atk 98→108; SpA 98→108 | `75/98/63/98/63/101` → `75/108/63/108/63/101` | 498→518 | HOLD — high power/large delta | 3 / 1 / 0 |
| `SIMISAGE` | Atk 98→108; SpA 98→108 | `75/98/63/98/63/101` → `75/108/63/108/63/101` | 498→518 | HOLD — high power/large delta | 0 / 1 / 0 |
| `SIMISEAR` | Atk 98→108; SpA 98→108 | `75/98/63/98/63/101` → `75/108/63/108/63/101` | 498→518 | HOLD — high power/large delta | 3 / 1 / 0 |
| `SKUNTANK` | Atk 93→113 | `103/93/67/71/61/84` → `103/113/67/71/61/84` | 479→499 | PORT 1 — conservative | 6 / 1 / 0 |
| `SOLROCK` | Atk 95→125; Def 85→115 | `90/95/85/55/65/70` → `90/125/115/55/65/70` | 460→520 | HOLD — high power/large delta | 3 / 1 / 9 |
| `SPINDA` | Atk 60→90; SpA 60→90; Spe 60→90 | `60/60/60/60/60/60` → `60/90/60/90/60/90` | 360→450 | REVIEW 2 — rescue | 3 / 1 / 1 |
| `SPIRITOMB` | HP 50→70 | `50/92/108/92/108/35` → `70/108/108/108/108/35` | 485→537 | HOLD — high power/large delta | 3 / 1 / 1 |
| `STANTLER` | Atk 95→115; Spe 85→95 | `73/95/62/85/65/85` → `73/115/62/85/65/95` | 465→495 | HOLD — now evolves | 1 / 1 / 42 |
| `STEELIX` | Atk 85→105; SpD 65→75; Spe 30→20 | `75/85/200/55/65/30` → `75/105/200/55/75/20` | 510→530 | HOLD — role change | 3 / 2 / 17 |
| `STEELIX_MEGA` | Atk 125→145; SpD 95→105; Spe 30→20 | `75/125/230/55/95/30` → `75/145/230/55/105/20` | 610→630 | HOLD — Mega/legendary | 0 / 0 / 0 |
| `STOUTLAND` | Atk 100→120 | `85/110/90/45/90/80` → `85/120/90/45/90/80` | 500→510 | HOLD — high power/large delta | 4 / 1 / 0 |
| `SUDOWOODO` | Atk 100→110; Def 115→125 | `70/100/115/30/65/30` → `70/110/125/30/65/30` | 410→430 | PORT 1 — conservative | 2 / 1 / 0 |
| `SUNFLORA` | HP 75→95; SpA 105→125 | `75/75/55/105/85/30` → `95/75/55/125/85/30` | 425→465 | REVIEW 2 — rescue | 1 / 1 / 0 |
| `SWALOT` | Atk 73→83; Def 83→93; SpA 73→83; SpD 83→93 | `100/73/83/73/83/55` → `100/83/93/83/93/55` | 467→507 | HOLD — high power/large delta | 3 / 1 / 0 |
| `SWANNA` | SpA 87→107; Spe 98→103 | `75/87/63/87/63/98` → `75/87/63/107/63/103` | 473→498 | PORT 1 — conservative | 6 / 2 / 0 |
| `SWELLOW` | SpA 75→85 | `60/85/60/75/50/125` → `60/85/60/85/50/125` | 455→465 | PORT 1 — conservative | 14 / 1 / 1 |
| `SWOOBAT` | SpA 77→97 | `67/57/55/77/55/114` → `67/57/55/97/55/114` | 425→445 | PORT 1 — conservative | 4 / 1 / 2 |
| `TAUROS` | Atk 100→120 | `75/100/95/40/70/110` → `75/120/95/40/70/110` | 490→510 | HOLD — high power/large delta | 3 / 2 / 5 |
| `TOGEDEMARU` | Atk 98→108; Def 63→83; Spe 96→106 | `65/98/63/40/73/96` → `65/108/83/40/73/106` | 435→475 | REVIEW 2 — rescue | 2 / 1 / 5 |
| `TORTERRA` | Def 105→114 | `95/109/105/75/85/56` → `95/109/114/75/85/56` | 525→534 | PORT 1 — ability coupled | 4 / 1 / 0 |
| `TOUCANNON` | Def 75→95; SpD 75→95 | `80/120/75/75/75/60` → `80/120/95/75/95/60` | 485→525 | HOLD — high power/large delta | 3 / 1 / 0 |
| `TOXICROAK` | Spe 85→95 | `83/106/65/86/65/85` → `83/106/65/86/65/95` | 490→500 | HOLD — high power/large delta | 5 / 1 / 1 |
| `TREVENANT` | Def 76→86; SpD 82→92 | `85/110/76/65/82/56` → `85/110/86/65/92/56` | 474→494 | PORT 1 — ability coupled | 4 / 1 / 4 |
| `TROPIUS` | Atk 68→88; Def 83→93; SpD 87→97 | `99/68/83/72/87/51` → `99/88/93/72/97/51` | 460→500 | PORT 1 — ability coupled | 6 / 1 / 2 |
| `TSAREENA` | Def 98→90; SpD 98→90; Spe 72→108 | `72/120/98/50/98/72` → `72/120/90/50/90/108` | 510→530 | HOLD — role change | 9 / 1 / 0 |
| `TURTONATOR` | SpA 91→111 | `60/78/135/91/85/36` → `60/78/135/111/85/36` | 485→505 | PORT 1 — conservative | 2 / 1 / 23 |
| `TYRANTRUM` | SpD 59→69; Spe 71→81 | `82/121/119/69/59/71` → `82/121/119/69/69/81` | 521→541 | HOLD — high power/large delta | 4 / 1 / 0 |
| `UNFEZANT` | HP 80→100 | `80/115/80/65/55/93` → `100/115/80/65/55/93` | 488→508 | HOLD — high power/large delta | 4 / 1 / 0 |
| `URSARING` | Spe 55→75 | `90/130/75/75/75/55` → `90/130/75/75/75/75` | 500→520 | HOLD — now evolves | 2 / 1 / 0 |
| `VANILLUXE` | Atk 95→69; SpA 110→120; Spe 79→95 | `71/95/85/110/95/79` → `71/69/85/120/95/95` | 535→535 | HOLD — role change | 1 / 1 / 0 |
| `VENOMOTH` | SpA 90→100; Spe 90→100 | `70/65/60/90/75/90` → `70/65/60/100/75/100` | 450→470 | PORT 1 — conservative | 2 / 1 / 13 |
| `VESPIQUEN` | Def 102→112; SpD 102→112 | `70/80/102/80/102/40` → `70/80/112/80/112/40` | 474→494 | PORT 1 — ability coupled | 4 / 1 / 0 |
| `VIKAVOLT` | Atk 70→95; Def 90→99; Spe 43→109 | `77/70/90/145/75/43` → `77/95/99/145/75/109` | 500→600 | HOLD — high power/large delta | 14 / 1 / 0 |
| `VILEPLUME` | HP 75→85; Atk 80→60; Def 85→95; SpA 110→120; SpD 90→100 | `75/80/85/110/90/50` → `85/60/95/120/100/50` | 490→510 | HOLD — role change | 3 / 1 / 0 |
| `VOLBEAT` | Def 75→85; Spe 85→115 | `65/73/75/47/85/85` → `65/73/85/47/85/115` | 430→470 | REVIEW 2 — rescue | 2 / 1 / 1 |
| `WAILORD` | Def 45→65; SpD 45→65 | `170/90/45/90/45/60` → `170/90/65/90/65/60` | 500→540 | PORT 1 — ability coupled | 6 / 1 / 22 |
| `WALREIN` | Atk 90→80 | `110/80/90/95/90/65` → `110/80/90/95/90/65` | 530→530 | NO CHANGE — already exact | 6 / 1 / 0 |
| `WATCHOG` | HP 60→75; Atk 85→95; Def 69→79; SpD 69→79 | `60/85/69/60/69/77` → `75/95/79/60/79/77` | 420→465 | REVIEW 2 — rescue | 0 / 1 / 1 |
| `WEEZING` | SpA 85→95 | `65/90/120/85/70/60` → `65/90/120/95/70/60` | 490→500 | HOLD — high power/large delta | 6 / 1 / 12 |
| `WHISCASH` | Atk 78→98 | `110/78/73/76/71/60` → `110/98/73/76/71/60` | 468→488 | PORT 1 — conservative | 6 / 1 / 25 |
| `WIGGLYTUFF` | HP 140→120; SpA 85→105; SpD 50→60 | `140/70/45/85/50/45` → `120/70/45/105/60/45` | 435→445 | HOLD — role change | 3 / 1 / 0 |
| `WORMADAM` → `WORMADAM_PLANT` | HP 60→80; SpA 79→99 | `60/59/85/79/105/36` → `80/59/85/99/105/36` | 424→464 | REVIEW 2 — rescue | 2 / 1 / 0 |
| `WORMADAM_SANDY_CLOAK` → `WORMADAM_SANDY` | HP 60→80; Atk 79→99 | `60/79/105/59/85/36` → `80/99/105/59/85/36` | 424→464 | REVIEW 2 — rescue | 2 / 1 / 0 |
| `WORMADAM_TRASH_CLOAK` → `WORMADAM_TRASH` | HP 60→80; Def 95→105; SpD 95→105 | `60/69/95/69/95/36` → `80/69/105/69/105/36` | 424→464 | REVIEW 2 — rescue | 2 / 1 / 0 |
| `XATU` | SpA 95→115; Spe 95→105 | `65/75/70/95/70/95` → `65/75/70/115/70/105` | 470→500 | PORT 1 — conservative | 5 / 1 / 2 |
| `YANMEGA` | SpA 116→126 | `86/76/86/116/56/95` → `86/76/86/126/56/95` | 515→525 | HOLD — high power/large delta | 2 / 2 / 0 |
| `ZANGOOSE` | Def 60→70; SpD 60→70; Spe 90→105 | `73/115/60/60/60/90` → `73/115/70/60/70/105` | 458→493 | HOLD — high power/large delta | 5 / 1 / 1 |
| `ZEBSTRIKA` | HP 75→85; Atk 100→110 | `75/100/63/80/63/116` → `85/110/63/80/63/116` | 497→517 | HOLD — high power/large delta | 4 / 2 / 0 |

## Bottom line

Inclement’s balance work is valuable, but its entire stat table is not a compatible modern standard. The right import is evidence-led: complete the Ability identities already chosen, add conservative rescue power where it does not overwrite a Pokémon’s modern role, preserve evolution and Mega invariants, and force every affected trainer and wild preset back through the campaign audit. The 102-species first port is broad enough to materially improve roster diversity without turning Emerald Champions into an opaque second ruleset.
