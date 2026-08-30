# Emerald Champions trainer campaign — fresh second pass

Date: 2026-08-29

Scope: every one of the 513 finite physical encounter groups and all 561 reachable trainer branches was re-read in campaign order. `src/data/trainers.party` and the live battle-script dialogue labels were treated as authority. The three first-pass ledgers were used only after independent review to cross-check prior concerns. No compile, commit, map edit, wild-table edit, battle-set generation, Circuit edit, or release-script run was performed.

`difficulty_observed` remains `UNPLAYED`; this is static/source closure, not a human playthrough claim.

## Measurable closure

- Coverage: 513/513 encounter groups, 561/561 branches, and 2147/2,147 authored Pokemon inspected.
- Formats: 87 single, 460 double, 14 multi; 84.49% are doubles-like.
- Roster breadth: 754 unique species/forms; all 92 Mega Stones and all 57 Legendary Sign species occur in opponent parties.
- Difficulty: mean 7.57, median 7.5; ordinary bands {6: 143, 7: 267, 8: 64}.
- Dialogue linkage: 561/561 campaign trainer IDs resolve to defined source text labels.
- Rolling ten encounters: 0 windows contain a species three or more times; 0 repeated two-species cores remain.
- Static gates: full 513-encounter materialization verify-only PASS; master audit PASS; Ability legality PASS; runtime coherence PASS; battle-format alignment PASS; story/dialogue-width PASS; `git diff --check` PASS.

## Exact fresh-pass changes

- 35 encounters changed in this pass: 16 executable-party encounters, 3 dialogue encounters, and 17 master-only corrections (one party encounter also has dialogue repair).
- Executable parties: 12 species-stage/roster substitutions, 18 move identity replacements, 6 Ability selections, 3 nature orientations, 3 Stat Point spreads, and 1 held-item selection across 16 branches.
- Dialogue: 4 source text blocks across encounters 0226, 0238, and 0445 now describe their mixed rosters instead of claiming every member is a bird.
- Master: 17 stale encounter dossiers were corrected independently of party edits, all 16 changed parties were synchronized, and 4 global count fields were repaired (formats, ordinary bands, 6.x share, and 57-species legendary coverage).
- Closure verdicts: 478 RETAIN and 35 evidence-backed CHANGE/DIALOGUE FIX/DOSSIER FIX encounters.

## Campaign-wide findings

### Caps, evolution, and preparation

All branch levels materialize exactly from the documented cap plus offset, stay within cap-5..cap+5, and the early level-evolution gate remains clean. Six unsupported late pre-evolutions were advanced to Seadra, Electabuzz, Whiscash, Wailord, Octillery, and Lanturn. Remaining conspicuously young late species are either Eviolite-equipped or explicitly define a named puzzle such as Magikarp escalation, evolution-chain fishing, Day Care youth, or the postgame tiny-survivor team.

Items, natures, Stat Points, and moves remain legal and category-aligned under the source gates. The pass also made Simple Skill Swap, Coaching/Stamina, Hydration rain, and the physical Manaphy/Drednaw rain lead executable without adding trainer-specific AI.

### Difficulty, valleys, and fatigue

Ordinary trainers retain 143 6.x valleys (30.17%), 267 7.x standards, and 64 8.x route aces; none are 9.x. The lowest ten-battle window is 0214-0223 at 6.75. The highest is 0490-0499 at 9.25, driven by the League/postgame sequence rather than ordinary filler.

Magma Hideout's 19-encounter completionist path retains explicit 6.8 valleys at 0324, 0328, 0332, and 0336. Aqua Hideout uses valleys at 0340 and 0344 but is later and deliberately sharper. Mossdeep Gym/Space Center 0400-0409 and the five consecutive 10.0 League fights 0493-0497 remain the principal fatigue/attrition hotspots; only a fresh-save human run can decide whether their recovery spacing feels fair.

### Rolling diversity and showcase coverage

The fresh fixes remove every three-use species cluster and every repeated two-species core from every rolling ten-encounter window. Isolated one-slot repeats remain where a route school, Gym vocabulary, faction progression, or named-character continuity benefits from recognition; none recreates an entire two-member engine.

The 92/92 Mega and 57/57 legendary coverage counts survive every roster change. No Mega appears before the post-Brawly bracelet, no branch carries two Mega Stones, and the Elite Four/Champion sequence keeps one transformation decision per team.

### Gym, faction, and rival identity

Gym specialty ratios across all executable branches are: Rustboro Rock 15/18, Dewford Fighting 20/26, Mauville Electric 18/22, Lavaridge Fire 26/33, Petalburg Normal 29/34, Fortree Flying 28/30, Mossdeep Psychic 43/54, Sootopolis B1F Water 35/37, and Juan 5/6. Every branch remains at or above a type majority, including the Sootopolis basement that the generic Gym map table does not cover.

Magma's 24-story-battle corpus is led by Ground (33 typings) and Fire (27), using sand, sun, slower machinery, and controlled positioning. Aqua's 24-story-battle corpus is led by Water (49), Poison (28), and Dark (25), using rain, speed, redirection, and current-like pivots. The contrast remains visible without forcing every grunt into a monotype.

The rival ladder remains stage-correct at 0001, 0032, 0059, 0249, and 0302: base starter counter, middle starter, middle starter, then two final-stage teams with an evolving shared core. Wally remains coherent at 0069, 0476, and 0498: Roselia/Azumarill and Mega Gallade establish his identity, Victory Road broadens it, and the postgame rematch makes Gallade the explicit Mega again.

### Elite Four attrition and broad counterplay

Sidney, Phoebe, Glacia, Drake, and Wallace remain five consecutive six-Pokemon 10.0 doubles with distinct Dark balance, Ghost offense, snow, Dragon weather sequencing, and rain control. Source story gates confirm a one-survivor retirement/retry path in every room and Bag healing only between rooms. This proves recovery availability, not actual attrition feel.

Every encounter still exposes at least one broad answer class through speed denial, weather replacement, focus fire, status, item removal, category switching, setup reset, Wide Guard/Protect, or typed pressure. Friendly-fire spread moves were retained only where they are a visible counterplay seam or an authored activation such as Gulp Missile, Water Compaction, Steam Engine, Flash Fire, Anger Point, Telepathy, or a protected partner turn.

## AI executability and release-only runtime calls

No encounter now requires a new trainer-specific AI routine to express its static party plan. Current generic source contains handlers for Anger Point critical activation, Round, Pledge combinations, Ability transfer/suppression, ally-beneficial spread attacks, Commander, Decorate, Instruct, and speed-field scoring. The following exact behaviors still require scenario execution before release:

| Encounter(s) | Required runtime behavior | Current evidence / risk |
| --- | --- | --- |
| 0011 | Spinarak traps a foe while Celebi uses Perish Song, then the AI switches its own counted Pokemon sensibly. | Generic trapping and Perish switching exist; formation-level targeting is not proven here. |
| 0020 | Cubchoo targets allied Anger Point Crabrawler with Frost Breath before Crabrawler acts. | Explicit generic-AI scoring exists; no campaign scenario was run. |
| 0028, 0090, 0165, 0394 | Instruct targets the ally whose last/current move is legal and useful under the current order. | At this trainer pass, source scoring existed while broad Instruct understanding was still TODO. The later engine/UI second pass converted it into executable positive/negative coverage; see `FRESH_ENGINE_UI_CI_SECOND_PASS.md`. |
| 0031, 0117 | Dondozo/Tatsugiri lead order activates Commander and prevents an invalid AI switch. | Engine and switching tests exist; campaign parties were not executed. |
| 0048, 0144, 0159 | Weak Surf/Flamethrower correctly activates Steam Engine, Water Compaction, Flash Fire, or Weakness Policy without self-sabotaging Protect. | Generic ally-activation scoring and focused tests exist; exact parties remain scenario calls. |
| 0054 | Both halves choose compatible Round actions in joint and split approaches. | Round combo tests exist; native paired map approach remains unplayed. |
| 0060, 0173 | Beat Up reaches the intended Justified ally without a conflicting `Attacks Partner` override. | The 65-test doubles-AI corpus now covers Justified targeting and left/right ally Protect coordination; these exact campaign formations remain scenario calls. |
| 0067 | Pledge users choose compatible partners after different knockout orders. | Generic Pledge coordination is tested; this exact reserve sequence is not. |
| 0084, 0481 | Gastro Acid or Simple Beam targets Truant and does not waste the support turn on a foe/healthy Ability. | Negative Ability ratings drive source scoring; exact campaign scenarios remain unrun. |
| 0092 | Wattson's adaptive reserves do not deploy mutually hostile speed controls on the same turn. | Static approved pairing guard passes; dynamic reserve order needs runtime proof. |
| 0113, 0147, 0410 | Dancer copies the intended ally dance and preserves target/order semantics. | Ability engine support exists; authored formations remain unplayed. |
| 0142, 0149, 0154 | Decorate or Skill Swap selects the intended ally (Meloetta, Contrary Pyroar, Simple-to-Kommo-o). | Generic Decorate/Contrary/Simple benefit scoring exists; exact turn-one decisions need scenarios. |
| 0479 | Tatsugirinite Mega selection occurs on entry and Storm Drain remains the live, self-contained Ability. | Source loadout is coherent; transformation timing remains a runtime call. |

Confirmed new custom-AI requirement: none. Conditional code/config risk: if 0060/0173 do not choose the Justified ally, the materializer's enforced `Attacks Partner` flag must be reconciled with the generic Justified tests, which exercise the no-flag path. If the four Instruct scenarios fail, their existing generic scorer needs correction; party order alone cannot guarantee a useful last move under every board state.

Human-only risks remain difficulty calibration, fatigue, reserve-selection feel, paired sightline presentation, dialogue tone in context, and whether the 0493-0497 League attrition reads as demanding rather than exhausting. Static PASS cannot close those questions.

## Mega showcase placement (92/92)

- Abomasite: 0289, 0495
- Absolite: 0138, 0493, 0504
- Absolite Z: 0286
- Aerodactylite: 0127
- Aggronite: 0169
- Alakazite: 0211
- Altarianite: 0125, 0179
- Ampharosite: 0474
- Audinite: 0166
- Banettite: 0162
- Barbaracite: 0184
- Baxcalibrite: 0192
- Beedrillite: 0083, 0454
- Blastoisinite: 0182
- Blazikenite: 0483
- Cameruptite: 0163, 0409
- Chandelurite: 0144
- Charizardite X: 0277
- Charizardite Y: 0234
- Chesnaughtite: 0238
- Chimechite: 0252
- Clefablite: 0245
- Crabominite: 0254
- Darkranite: 0258
- Delphoxite: 0263
- Diancite: 0140
- Dragalgite: 0268
- Dragoninite: 0274
- Drampanite: 0280
- Eelektrossite: 0343
- Emboarite: 0150
- Excadrite: 0143
- Falinksite: 0346
- Feraligite: 0323
- Floettite: 0327
- Froslassite: 0332
- Galladite: 0069, 0498
- Garchompite: 0335
- Garchompite Z: 0388, 0513
- Gardevoirite: 0472
- Gengarite: 0053, 0494
- Glalitite: 0167
- Glimmoranite: 0392
- Golisopite: 0397
- Golurkite: 0306
- Greninjite: 0311
- Gyaradosite: 0178, 0227, 0348
- Hawluchanite: 0287
- Heatranite: 0328
- Heracronite: 0160
- Houndoominite: 0158, 0261
- Kangaskhanite: 0071, 0171
- Latiasite: 0295
- Latiosite: 0300
- Lopunnite: 0164
- Lucarionite: 0354
- Lucarionite Z: 0361
- Magearnite: 0367
- Malamarite: 0052
- Manectite: 0373
- Mawilite: 0315, 0501
- Medichamite: 0172
- Meganiumite: 0379
- Meowsticite: 0385
- Metagrossite: 0161, 0512
- Mewtwonite X: 0403
- Mewtwonite Y: 0417
- Pidgeotite: 0168
- Pinsirite: 0170
- Pyroarite: 0423
- Raichunite X: 0430
- Raichunite Y: 0092
- Sablenite: 0434
- Salamencite: 0439
- Sceptilite: 0157
- Scizorite: 0444
- Scolipite: 0449
- Scovillainite: 0453
- Scraftinite: 0458
- Sharpedonite: 0176, 0248
- Skarmorite: 0461
- Slowbronite: 0175
- Staraptite: 0466
- Starminite: 0510
- Steelixite: 0200
- Swampertite: 0320, 0476
- Tatsugirinite: 0479
- Tyranitarite: 0484
- Venusaurite: 0096
- Victreebelite: 0488
- Zeraorite: 0511
- Zygardite: 0505

## Legendary Sign species placement (57/57)

- Arceus: 0509
- Azelf: 0391
- Blacephalon: 0130, 0331
- Buzzwole: 0508
- Calyrex: 0393
- Celesteela: 0053, 0127, 0289
- Chien Pao: 0495
- Chi Yu: 0338
- Cobalion: 0507
- Cresselia: 0222, 0400, 0501
- Darkrai: 0258, 0493
- Dialga: 0481, 0496
- Entei: 0132
- Eternatus: 0489
- Genesect: 0506
- Giratina: 0494
- Glastrier: 0374, 0495
- Guzzlord: 0478
- Kartana: 0055, 0098, 0234, 0501
- Keldeo: 0049
- Koraidon: 0496
- Kubfu: 0036
- Kyurem: 0495
- Manaphy: 0052, 0463, 0497
- Marshadow: 0156, 0494
- Mesprit: 0504
- Miraidon: 0092
- Necrozma: 0395
- Nihilego: 0142
- Ogerpon: 0264
- Pheromosa: 0072, 0379
- Phione: 0046
- Poipole: 0340
- Raikou: 0513
- Regidrago: 0512
- Regieleki: 0164
- Spectrier: 0307
- Stakataka: 0066, 0163
- Suicune: 0282, 0498
- Tapu Bulu: 0152
- Tapu Fini: 0416
- Tapu Koko: 0053
- Tapu Lele: 0400
- Terrakion: 0060, 0173
- Ting Lu: 0329
- Type: Null: 0059
- Uxie: 0393
- Victini: 0338, 0400
- Volcanion: 0047, 0501
- Xerneas: 0491
- Xurkitree: 0131
- Yveltal: 0263, 0493
- Zacian: 0482
- Zamazenta: 0490
- Zarude: 0299
- Zekrom: 0420
- Zeraora: 0073, 0511

## Accepted isolated exact-set recurrences inside ten encounters

- 0187/0191: Raichu; one slot only, no repeated two-species core.
- 0196/0205: Lumineon; one slot only, no repeated two-species core.
- 0267/0269: Roserade; one slot only, no repeated two-species core.
- 0340/0349: Tentacruel; one slot only, no repeated two-species core.
- 0359/0364: Vaporeon; one slot only, no repeated two-species core.
- 0360/0368: Seaking; one slot only, no repeated two-species core.
- 0374/0377: Pyukumuku; one slot only, no repeated two-species core.
- 0390/0395: Meowstic F; one slot only, no repeated two-species core.
- 0390/0397: Mr. Mime; one slot only, no repeated two-species core.
- 0394/0403: Torkoal; one slot only, no repeated two-species core.
- 0404/0409: Crobat; one slot only, no repeated two-species core.
- 0414/0416: Unfezant; one slot only, no repeated two-species core.
- 0447/0451: Machamp; one slot only, no repeated two-species core.
- 0460/0467: Primarina; one slot only, no repeated two-species core.

## Per-encounter closure

- 0001 **RETAIN** — 6 source branches; single; cap 14 (cap+1); Treecko, Mudkip, Torchic; setup; dialogue 6/6 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0002 **RETAIN** — 1 source branch; double; cap 14 (cap+0); Zorua, Jigglypuff, Smeargle, Mew; redirection/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0003 **RETAIN** — 1 source branch; double; cap 14 (cap+0); Dewpider, Anorith, Karrablast, Larvesta; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0004 **RETAIN** — 1 source branch; double; cap 14 (cap+0); Pikachu, Taillow, Paras, Zigzagoon; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0005 **RETAIN** — 1 source branch; double; cap 14 (cap-2); Amaura, Sandshrew Alola, Swinub, Arctozolt; snow/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0006 **RETAIN** — 1 source branch; double; cap 14 (cap+0); Ditto, Wimpod, Sandygast, Tirtouga; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0007 **RETAIN** — 1 source branch; double; cap 14 (cap+0); Remoraid, Bruxish, Qwilfish, Clamperl; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0008 **RETAIN** — 1 source branch; double; cap 14 (cap+0); Buneary, Gothita, Mawile, Furfrou Debutante; Trick Room; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0009 **RETAIN** — 1 source branch; double; cap 14 (cap-2); Pineco, Dottler, Dwebble, Joltik; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0010 **RETAIN** — 1 source branch; double; cap 14 (cap+0..+2); Purrloin, Skrelp, Corphish, Inkay; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0011 **RETAIN** — 1 source branch; double; cap 14 (cap+0); Celebi, Spinarak, Nincada, Heracross; redirection/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0012 **RETAIN** — 1 source branch; double; cap 14 (cap+0); Stonjourner, Porygon, Dedenne, Honedge; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0013 **RETAIN** — 1 source branch; double; cap 14 (cap-2); Eevee, Ralts, Snorunt, Morpeko; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0014 **RETAIN** — 1 source branch; double; cap 14 (cap+0); Oricorio, Axew, Cutiefly, Oddish; sun/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0015 **RETAIN** — 1 source branch; double; cap 14 (cap+0); Luvdisc, Stunfisk, Tentacool, Veluza; rain/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0016 **RETAIN** — 1 source branch; single; cap 14 (cap-2); Machop; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0017 **RETAIN** — 1 source branch; double; cap 14 (cap-3..+0); Oshawott, Jangmo O, Torchic; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0018 **RETAIN** — 1 source branch; double; cap 14 (cap-3); Zangoose, Seviper, Ferroseed, Roselia; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0019 **RETAIN** — 1 source branch; double; cap 14 (cap-3..+0); Meditite, Spoink, Tropius, Litwick; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0020 **RETAIN** — 1 source branch; double; cap 14 (cap+0); Cubchoo, Crabrawler, Pancham, Mankey; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0021 **RETAIN** — 1 source branch; double; cap 14 (cap+0); Vivillon, Grubbin, Sizzlipede, Scyther; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0022 **RETAIN** — 1 source branch; double; cap 14 (cap-2); Elgyem, Onix, Mantyke, Kecleon; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0023 **RETAIN** — 2 source branches; double; cap 14 (cap+0); Carbink, Rhyhorn, Minior, Clefairy, Rufflet, Darumaka Galar; redirection/spread; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0024 **RETAIN** — 1 source branch; double; cap 14 (cap+0); Hippopotas, Drilbur, Cacnea, Aron; sand/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0025 **RETAIN** — 2 source branches; double/single; cap 14 (cap+0); Meowth Galar, Wooloo, Minccino, Diancie, Sableye, Gimmighoul; Trick Room/spread; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0026 **RETAIN** — 2 source branches; double/single; cap 14 (cap-2); Pincurchin, Mimikyu, Swirlix, Klink, Togedemaru, Elekid; spread; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0027 **RETAIN** — 1 source branch; double; cap 14 (cap+0); Shuckle, Cranidos, Lileep, Glimmet; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0028 **RETAIN** — 1 source branch; double; cap 14 (cap-2); Oranguru, Tyrunt, Nosepass, Binacle; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0029 **RETAIN** — 1 source branch; double; cap 14 (cap+0); Shieldon, Woobat, Corsola Galar, Aerodactyl; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0030 **RETAIN** — 1 source branch; double; cap 14 (cap+3..+5); Carbink, Rockruff, Nacli, Bonsly, Lileep, Regirock; Trick Room/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0031 **RETAIN** — 1 source branch; double; cap 20 (cap+1..+2); Dondozo, Tatsugiri Stretchy, Salandit, Mareanie; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0032 **RETAIN** — 6 source branches; double; cap 20 (cap+1..+2); Castform, Grovyle, Sneasel, Rotom, Marshtomp, Combusken; rain/sun/spread; dialogue 6/6 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0033 **RETAIN** — 1 source branch; double; cap 20 (cap+0); Finneon, Arrokuda, Spheal, Cramorant; rain/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0034 **RETAIN** — 1 source branch; single; cap 20 (cap+0); Frillish, Clauncher, Shellder; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0035 **RETAIN** — 1 source branch; double; cap 20 (cap-2..+0); Mienfoo, Makuhita, Croagunk, Gumshoos; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0036 **RETAIN** — 2 source branches; double/single; cap 20 (cap+0); Kubfu, Solosis, Clobbopus, Meditite, Scraggy, Stufful; spread; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0037 **RETAIN** — 1 source branch; double; cap 20 (cap+0); Machop, Timburr, Jangmo O; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0038 **RETAIN** — 1 source branch; single; cap 20 (cap+0); Riolu, Farfetchd Galar, Throh; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0039 **RETAIN** — 1 source branch; double; cap 20 (cap-2); Tyrogue, Pancham, Crabrawler, Natu; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0040 **RETAIN** — 1 source branch; double; cap 20 (cap+2..+5); Pachirisu, Falinks, Hitmontop, Kirlia, Breloom, Hawlucha; redirection/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0041 **RETAIN** — 1 source branch; single; cap 30 (cap+0); Pelipper, Perrserker, Dhelmise; rain; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0042 **RETAIN** — 1 source branch; double; cap 30 (cap+0); Flaaffy, Palpitoad, Azumarill, Bibarel; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0043 **RETAIN** — 1 source branch; double; cap 30 (cap+0); Slowpoke, Koffing, Staravia; Trick Room/Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0044 **RETAIN** — 1 source branch; single; cap 30 (cap-2); Linoone, Greedent, Appletun; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0045 **RETAIN** — 1 source branch; double; cap 30 (cap+0); Cherrim, Leafeon, Comfey, Shaymin; sun/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0046 **RETAIN** — 1 source branch; double; cap 30 (cap+0); Phione, Electrode, Cryogonal; rain; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0047 **RETAIN** — 1 source branch; single; cap 30 (cap-2..+0); Jynx, Magmar, Volcanion; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0048 **RETAIN** — 1 source branch; double; cap 30 (cap+0); Feebas, Carkol, Sandygast, Tsareena; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0049 **RETAIN** — 1 source branch; double; cap 30 (cap+0); Tropius, Keldeo, Mudsdale, Lapras; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0050 **RETAIN** — 1 source branch; double; cap 30 (cap+0); Raticate Alola, Tentacruel, Basculin, Relicanth; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0051 **RETAIN** — 1 source branch; double; cap 30 (cap-2..+2); Clauncher, Sealeo, Zoroark, Grimer Alola; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0052 **RETAIN** — 1 source branch; double; cap 30 (cap+1..+4); Liepard, Hitmontop, Manaphy, Kingdra, Qwilfish, Malamar; rain; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0053 **RETAIN** — 1 source branch; double; cap 30 (cap+1..+5); Landorus Therian, Tapu Koko, Gengar, Celesteela, Urshifu Rapid Strike, Rillaboom; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0054 **RETAIN** — 2 source branches; multi; cap 30 (cap+0); Meloetta, Wigglytuff, Chatot, Audino, Slurpuff, Frogadier; spread; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0055 **RETAIN** — 1 source branch; double; cap 30 (cap+0); Floragato, Gallade, Zangoose, Kartana; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0056 **RETAIN** — 1 source branch; double; cap 30 (cap+0); Dugtrio, Misdreavus, Carnivine, Sigilyph; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0057 **RETAIN** — 1 source branch; double; cap 30 (cap-2..+2); Indeedee F, Magearna, Shuppet, Duosion; redirection/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0058 **RETAIN** — 1 source branch; double; cap 30 (cap+0); Gimmighoul Roaming, Stunky, Plusle, Heliolisk; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0059 **RETAIN** — 6 source branches; double; cap 30 (cap+1..+3); Type: Null, Dracovish, Grovyle, Metang, Alcremie, Duraludon, Marshtomp, Combusken; direct tempo; dialogue 6/6 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0060 **RETAIN** — 2 source branches; multi; cap 30 (cap-1..+0); Terrakion, Cottonee, Archen, Houndoom, Growlithe, Toxel; spread; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0061 **RETAIN** — 2 source branches; multi; cap 30 (cap+0..+2); Gligar, Gothorita, Drifblim, Lunatone, Hypno, Xatu; Tailwind/spread; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0062 **RETAIN** — 1 source branch; double; cap 30 (cap-2..+1); Lumineon, Arctovish, Octillery, Floatzel; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0063 **RETAIN** — 1 source branch; double; cap 30 (cap+0..+2); Hattrem, Kadabra, Chimecho, Swoobat; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0064 **RETAIN** — 1 source branch; double; cap 30 (cap+0); Doduo, Zebstrika, Rampardos, Porygon-Z; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0065 **RETAIN** — 1 source branch; double; cap 30 (cap+0); Gastrodon, Scolipede, Ponyta, Snorlax; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0066 **RETAIN** — 1 source branch; double; cap 30 (cap-2); Bronzor, Drampa, Crabominable, Stakataka; Trick Room/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0067 **RETAIN** — 1 source branch; double; cap 30 (cap+0); Thwackey, Raboot, Drizzile, Brionne; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0068 **RETAIN** — 1 source branch; double; cap 30 (cap+0); Ferroseed, Druddigon, Sharpedo, Moltres; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0069 **RETAIN** — 1 source branch; double; cap 30 (cap+0..+4); Weavile, Zapdos, Drilbur, Roselia, Azumarill, Gallade; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0070 **CHANGE** — 1 source branch; double; cap 30 (cap+0); Passimian, Mudsdale, Grimer Alola, Meditite; spread; dialogue 1/1 resolved. Replaced unsupported Beat Up-to-Stamina self-targeting with generic-AI Coaching while preserving the Receiver and Power of Alchemy knockout-order puzzle.
- 0071 **RETAIN** — 1 source branch; double; cap 30 (cap+1..+2); Munchlax, Togepi, Kangaskhan, Foongus, Abra, Staryu; redirection/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0072 **RETAIN** — 1 source branch; double; cap 30 (cap-2); Lopunny, Wishiwashi, Accelgor, Pheromosa; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0073 **RETAIN** — 1 source branch; double; cap 30 (cap+0); Zeraora, Froslass, Salandit, Lycanroc Dusk; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0074 **RETAIN** — 1 source branch; double; cap 30 (cap+0); Wormadam, Wormadam Sandy, Wormadam Trash, Mothim; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0075 **RETAIN** — 1 source branch; double; cap 30 (cap+1..+2); Froakie, Pichu, Magby, Smoochum, Happiny, Dratini; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0076 **RETAIN** — 3 source branches; single; cap 30 (cap-2..+0); Mienfoo, Hariyama, Meowstic, Musharna, Ponyta, Boltund; direct tempo; dialogue 3/3 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0077 **RETAIN** — 1 source branch; single; cap 30 (cap+0); Staravia, Diggersby, Linoone Galar, Miltank; Tailwind/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0078 **RETAIN** — 1 source branch; single; cap 30 (cap-2); Lilligant, Whimsicott, Blissey, Roserade; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0079 **RETAIN** — 1 source branch; single; cap 30 (cap+0..+5); Fomantis, Numel, Quagsire, Vibrava; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0080 **RETAIN** — 1 source branch; single; cap 30 (cap-2); Machamp; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0081 **RETAIN** — 1 source branch; double; cap 30 (cap-2); Gliscor, Zangoose, Vaporeon, Skorupi; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0082 **RETAIN** — 1 source branch; double; cap 30 (cap+0); Gourgeist Super, Gengar, Sudowoodo, Magmortar; Trick Room/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0083 **RETAIN** — 1 source branch; single; cap 30 (cap+0); Scyther, Kabuto, Beedrill; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0084 **DOSSIER FIX** — 1 source branch; double; cap 30 (cap+0); Victreebel, Slakoth, Golisopod, Durant; spread; dialogue 1/1 resolved. Limited Gastro Acid's executable claim to the opening Truant target; current generic AI does not promise suppression of beneficial Hustle or Emergency Exit.
- 0085 **RETAIN** — 1 source branch; double; cap 30 (cap-1..+0); Roserade, Vespiquen, Comfey; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0086 **RETAIN** — 1 source branch; double; cap 30 (cap-2); Eelektross, Kingdra, Crabominable, Drednaw; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0087 **RETAIN** — 1 source branch; single; cap 30 (cap-2); Noibat; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0088 **RETAIN** — 1 source branch; double; cap 30 (cap-2..-1); Diggersby, Aron, Simisear, Sawsbuck; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0089 **RETAIN** — 2 source branches; multi; cap 30 (cap+0); Toxtricity, Heliolisk, Electrode, Jolteon, Manectric, Electivire; spread; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0090 **RETAIN** — 1 source branch; double; cap 30 (cap-2); Oranguru, Charjabug, Magnezone, Pincurchin; Trick Room; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0091 **RETAIN** — 2 source branches; multi; cap 30 (cap+0); Plusle, Ampharos, Electabuzz, Minun, Klink, Dedenne; direct tempo; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0092 **RETAIN** — 1 source branch; double; cap 30 (cap+2..+5); Miraidon, Emolga, Iron Hands, Farigiraf, Porygon2, Raichu; Trick Room; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0093 **RETAIN** — 1 source branch; double; cap 40 (cap-2..+0); Magneton, Loudred, Alcremie Ruby Cream, Oricorio Pau; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0094 **RETAIN** — 1 source branch; double; cap 40 (cap+0..+3); Cofagrigus, Duosion, Gabite; Trick Room/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0095 **RETAIN** — 1 source branch; double; cap 40 (cap+0..+3); Empoleon, Infernape, Roserade, Fraxure; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0096 **RETAIN** — 1 source branch; single; cap 40 (cap-2); Ludicolo, Parasect, Venusaur; rain; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0097 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Spiritomb, Tyrantrum, Serperior, Carracosta; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0098 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Perrserker, Dhelmise, Sirfetchd, Kartana; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0099 **RETAIN** — 1 source branch; double; cap 40 (cap-1..+0); Gardevoir, Persian Alola, Garbodor, Copperajah; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0100 **RETAIN** — 1 source branch; double; cap 40 (cap-2..+2); Salazzle, Amoonguss, Skrelp; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0101 **RETAIN** — 1 source branch; double; cap 40 (cap-1..+0); Clefable, Beautifly, Jirachi, Metang; redirection/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0102 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Gigalith, Golem, Primeape; sand/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0103 **RETAIN** — 1 source branch; single; cap 40 (cap-2); Flapple, Trevenant, Exeggutor; sun; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0104 **RETAIN** — 1 source branch; double; cap 40 (cap+0..+3); Archeops, Carracosta, Golurk, Gogoat; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0105 **RETAIN** — 1 source branch; single; cap 40 (cap-2); Excadrill, Crustle; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0106 **RETAIN** — 1 source branch; double; cap 40 (cap+0..+2); Donphan, Aurorus, Rhyperior, Aggron; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0107 **RETAIN** — 1 source branch; single; cap 40 (cap-2); Victreebel, Cacturne; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0108 **RETAIN** — 1 source branch; single; cap 40 (cap-2); Noctowl, Vileplume; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0109 **RETAIN** — 2 source branches; double/single; cap 40 (cap+0); Magcargo, Typhlosion, Heatmor, Sunflora, Bellossom, Florges; sun/setup/spread; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0110 **RETAIN** — 3 source branches; double/single; cap 40 (cap+0); Claydol, Hippowdon, Cradily, Weezing, Ninjask, Shedinja, Gabite, Pupitar, +1 branch species; sand/setup/spread; dialogue 3/3 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0111 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Cryogonal, Magnezone, Magmortar, Darmanitan Galar; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0112 **RETAIN** — 1 source branch; single; cap 40 (cap-2); Noibat, Mienfoo, Vanillish; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0113 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Cutiefly, Oricorio Pom Pom, Frosmoth, Oricorio Sensu; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0114 **RETAIN** — 1 source branch; single; cap 40 (cap-2); Zoroark, Greninja, Cinderace; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0115 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Mamoswine, Vullaby, Garganacl, Torterra; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0116 **RETAIN** — 2 source branches; double; cap 40 (cap+0); Crobat, Toucannon, Honchkrow, Forretress, Runerigus, Delphox; Tailwind/spread; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0117 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Dondozo, Tatsugiri, Palafin, Clawitzer; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0118 **RETAIN** — 2 source branches; double; cap 40 (cap-2..+4); Beheeyem, Komala, Shiinotic, Whiscash, Grapploct, Walrein; Trick Room/setup/spread; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0119 **RETAIN** — 1 source branch; single; cap 40 (cap+0); Huntail, Gorebyss, Barraskewda; rain; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0120 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Lickilicky, Spinda, Furfrou; Trick Room/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0121 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Throh, Pangoro, Hitmonchan, Sirfetchd; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0122 **RETAIN** — 1 source branch; double; cap 40 (cap-2); Drednaw, Centiskorch, Mismagius; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0123 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Tyrantrum, Copperajah, Turtonator, Golem Alola; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0124 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Pyroar, Golduck, Marowak Alola, Vaporeon; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0125 **RETAIN** — 1 source branch; double; cap 40 (cap+0..+5); Noibat, Exploud, Kommo O, Altaria; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0126 **RETAIN** — 2 source branches; double; cap 40 (cap-2); Glaceon, Sandslash Alola, Magnezone, Ninetales Alola, Beartic, Articuno; snow/spread; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0127 **RETAIN** — 2 source branches; double; cap 40 (cap+1..+4); Lunatone, Jirachi, Aerodactyl, Solrock, Minior, Celesteela; setup/spread; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0128 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Exeggutor Alola, Dracozolt, Naganadel, Roaring Moon; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0129 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Plusle, Minun, Volbeat, Illumise; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0130 **RETAIN** — 2 source branches; single; cap 40 (cap+0..+3); Stonjourner, Blacephalon, Palossand, Golurk, Sandaconda, Krookodile; setup; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0131 **RETAIN** — 1 source branch; double; cap 40 (cap+1..+4); Coalossal, Klang, Electivire, Xurkitree, Rhyperior, Machamp; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0132 **RETAIN** — 1 source branch; double; cap 40 (cap+1..+5); Groudon, Crobat, Shiftry, Salazzle, Entei, Flygon; sun/Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0133 **RETAIN** — 1 source branch; single; cap 40 (cap-2); Pupitar, Metang, Hakamo O; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0134 **RETAIN** — 1 source branch; double; cap 40 (cap+1..+2); Stunfisk Galar, Thievul, Golisopod, Dugtrio; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0135 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Delibird, Vespiquen, Gourgeist, Eelektross; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0136 **RETAIN** — 2 source branches; double; cap 40 (cap+0); Chesnaught, Conkeldurr, Avalugg, Haunter, Jolteon, Dodrio; spread; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0137 **RETAIN** — 1 source branch; double; cap 40 (cap-2..+2); Toxtricity, Bouffalant, Rillaboom, Mr. Rime; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0138 **RETAIN** — 1 source branch; double; cap 40 (cap+1..+2); Galvantula, Ambipom, Cinccino, Drapion, Clefable, Absol; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0139 **RETAIN** — 1 source branch; double; cap 40 (cap-2..+5); Mienfoo, Wobbuffet, Slowbro, Aegislash; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0140 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Milotic, Furfrou, Lopunny, Diancie; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0141 **RETAIN** — 1 source branch; double; cap 40 (cap+0..+2); Tsareena, Cinccino, Pawniard, Escavalier; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0142 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Alcremie, Meloetta, Nihilego, Mr Mime Galar; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0143 **RETAIN** — 1 source branch; double; cap 40 (cap-2); Gigalith, Dracozolt, Lileep, Excadrill; sand/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0144 **RETAIN** — 2 source branches; double/single; cap 40 (cap+0); Chandelure, Incineroar, Simisear, Arcanine, Charizard, Reshiram; setup/spread; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0145 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Drifblim, Magcargo, Heatmor, Ho-Oh; Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0146 **RETAIN** — 1 source branch; single; cap 40 (cap-2); Vigoroth, Magmortar, Typhlosion; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0147 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Oricorio, Bellossom, Rapidash, Froslass; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0148 **RETAIN** — 2 source branches; multi; cap 40 (cap+0..+1); Claydol, Coalossal, Turtonator, Lampent, Flareon, Gouging Fire; Trick Room/spread; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0149 **RETAIN** — 1 source branch; double; cap 40 (cap+0); Spinda, Pyroar, Cinderace, Centiskorch; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0150 **RETAIN** — 1 source branch; double; cap 40 (cap+1..+4); Torkoal, Lilligant, Delphox, Skeledirge, Heatran, Emboar; sun/Trick Room/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0151 **RETAIN** — 1 source branch; double; cap 45 (cap+0); Sandslash, Cacturne, Dunsparce; sand/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0152 **RETAIN** — 1 source branch; double; cap 45 (cap+0); Tapu Bulu, Hawlucha, Mudsdale, Bellossom; sun/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0153 **RETAIN** — 1 source branch; double; cap 45 (cap+0); Persian Alola, Vullaby, Gastrodon, Lycanroc Dusk; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0154 **CHANGE** — 1 source branch; double; cap 45 (cap-2..+0); Swoobat, Kommo O, Jumpluff, Lycanroc; setup; dialogue 1/1 resolved. Replaced Cloud Nine Golduck with Simple Swoobat so Skill Swap can actually make Kommo-o's Dragon Dance count twice.
- 0155 **RETAIN** — 1 source branch; single; cap 45 (cap-2); Golurk, Sigilyph, Bastiodon; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0156 **RETAIN** — 1 source branch; single; cap 45 (cap+0); Ninjask, Shedinja, Marshadow; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0157 **RETAIN** — 1 source branch; double; cap 45 (cap-1..+5); Trevenant, Chesnaught, Goodra, Sceptile; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0158 **RETAIN** — 1 source branch; double; cap 45 (cap-2); Ninetales, Shiftry, Moltres, Houndoom; sun/Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0159 **RETAIN** — 1 source branch; double; cap 45 (cap+0); Staryu, Coalossal, Crustle, Rhyperior; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0160 **RETAIN** — 1 source branch; double; cap 45 (cap-1..+0); Ribombee, Masquerain, Araquanid, Heracross; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0161 **RETAIN** — 1 source branch; double; cap 45 (cap-2..+0); Kecleon, Malamar, Zoroark, Metagross; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0162 **RETAIN** — 1 source branch; single; cap 45 (cap-2..+0); Meowstic F, Gogoat, Banette; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0163 **RETAIN** — 1 source branch; double; cap 45 (cap-1..+0); Stakataka, Marowak Alola, Probopass, Camerupt; Trick Room/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0164 **RETAIN** — 1 source branch; double; cap 45 (cap+0); Regieleki, Ambipom, Dodrio, Lopunny; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0165 **RETAIN** — 1 source branch; double; cap 45 (cap-2); Spinda, Lickilicky, Oranguru, Girafarig; Trick Room/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0166 **RETAIN** — 1 source branch; double; cap 45 (cap+0); Chansey, Kecleon, Komala, Audino; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0167 **RETAIN** — 1 source branch; double; cap 45 (cap+0); Exploud, Mimikyu, Silvally, Glalie; Trick Room/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0168 **RETAIN** — 1 source branch; double; cap 45 (cap+0); Cinccino, Toucannon, Drampa, Pidgeot; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0169 **RETAIN** — 1 source branch; double; cap 45 (cap-2); Furfrou, Dubwool, Wigglytuff, Aggron; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0170 **RETAIN** — 1 source branch; double; cap 45 (cap+0); Ursaring, Bouffalant, Diggersby, Pinsir; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0171 **RETAIN** — 1 source branch; double; cap 45 (cap+1..+4); Porygon-Z, Swellow, Bewear, Meloetta, Regigigas, Kangaskhan; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0172 **RETAIN** — 1 source branch; single; cap 55 (cap-2..+0); Toxicroak, Hawlucha, Medicham; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0173 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Weavile, Terrakion, Hariyama, Scrafty; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0174 **RETAIN** — 1 source branch; single; cap 55 (cap-2..+0); Breloom, Pangoro, Poliwrath; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0175 **RETAIN** — 1 source branch; double; cap 55 (cap-2..+0); Omastar, Bronzong, Slowbro; Trick Room/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0176 **RETAIN** — 1 source branch; double; cap 55 (cap-1..+0); Lanturn, Mantine, Barraskewda, Sharpedo; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0177 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Wishiwashi, Alomomola, Clawitzer; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0178 **RETAIN** — 1 source branch; double; cap 55 (cap-1..+0); Pincurchin, Raichu Alola, Toxapex, Gyarados; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0179 **RETAIN** — 1 source branch; single; cap 55 (cap-2..+0); Kingdra, Milotic, Altaria; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0180 **RETAIN** — 1 source branch; double; cap 55 (cap-1..+0); Pelipper, Cramorant, Zapdos, Tornadus; rain/Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0181 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Relicanth, Runerigus, Regice; Trick Room/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0182 **RETAIN** — 1 source branch; single; cap 55 (cap-2); Tentacruel, Lapras, Blastoise; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0183 **RETAIN** — 1 source branch; single; cap 55 (cap-2); Nidoqueen, Golduck, Jellicent; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0184 **RETAIN** — 1 source branch; double; cap 55 (cap+0..+2); Pelipper, Tentacruel, Barraskewda, Rotom Wash, Kingdra, Barbaracle; rain/Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0185 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Sharpedo, Golisopod, Oricorio Pom Pom, Cryogonal; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0186 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Swanna, Seaking, Rotom Wash, Quagsire; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0187 **CHANGE** — 1 source branch; double; cap 55 (cap-2); Politoed, Ludicolo, Swampert, Raichu; rain/spread; dialogue 1/1 resolved. Replaced the third nearby Kingdra with Ludicolo, retaining rain offense while ending the rolling three-use species cluster.
- 0188 **RETAIN** — 1 source branch; double; cap 55 (cap-3); Clawitzer, Dragalge, Basculin, Alomomola; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0189 **RETAIN** — 1 source branch; single; cap 55 (cap-2); Starmie; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0190 **RETAIN** — 1 source branch; single; cap 55 (cap-2); Barbaracle, Beartic; snow/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0191 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Mantine, Raichu; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0192 **RETAIN** — 1 source branch; double; cap 55 (cap-3..+0); Seadra, Feraligatr, Baxcalibur; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0193 **RETAIN** — 1 source branch; single; cap 55 (cap-2); Gorebyss, Pyukumuku; rain; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0194 **RETAIN** — 1 source branch; single; cap 55 (cap-1); Manectric, Hawlucha, Sawsbuck; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0195 **RETAIN** — 1 source branch; single; cap 55 (cap-2); Stunfisk, Poliwrath; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0196 **RETAIN** — 1 source branch; single; cap 55 (cap-2); Lumineon, Octillery; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0197 **RETAIN** — 1 source branch; double; cap 55 (cap-2..+0); Machamp, Walrein, Dhelmise; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0198 **RETAIN** — 1 source branch; double; cap 55 (cap-1..+0); Plusle, Minun, Solrock, Lunatone; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0199 **RETAIN** — 1 source branch; single; cap 55 (cap-2..+0); Dragonair, Simipour, Furret; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0200 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Carracosta, Golem Alola, Steelix; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0201 **RETAIN** — 1 source branch; single; cap 55 (cap-2); Luxray, Stoutland; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0202 **RETAIN** — 1 source branch; double; cap 55 (cap-1..+0); Florges, Tsareena, Primarina; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0203 **RETAIN** — 1 source branch; double; cap 55 (cap-1..+0); Greninja, Palossand, Empoleon, Mamoswine; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0204 **RETAIN** — 1 source branch; single; cap 55 (cap-2..+0); Corsola Galar, Cloyster, Kingler; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0205 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Bibarel, Lumineon, Sealeo, Eiscue; Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0206 **RETAIN** — 1 source branch; double; cap 55 (cap-2..+0); Floatzel, Luvdisc, Quagsire; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0207 **RETAIN** — 1 source branch; double; cap 55 (cap-1..+0); Whiscash, Octillery, Dhelmise, Dracovish; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0208 **RETAIN** — 1 source branch; double; cap 55 (cap-1..+0); Illumise, Volbeat, Butterfree, Dustox; Tailwind/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0209 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Skarmory, Honchkrow, Unfezant, Sigilyph; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0210 **RETAIN** — 1 source branch; double; cap 55 (cap-3..-2); Cherubi, Roselia, Sawsbuck Autumn, Cherrim; sun; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0211 **RETAIN** — 1 source branch; double; cap 55 (cap-2..+0); Jigglypuff, Clefairy, Wigglytuff, Alakazam; redirection/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0212 **CHANGE** — 1 source branch; double; cap 55 (cap-3..-2); Barboach, Seadra, Qwilfish; setup/spread; dialogue 1/1 resolved. Evolved unsupported level-53 Horsea to Seadra while preserving Sniper special control.
- 0213 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Delcatty, Meowth, Ambipom, Stantler; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0214 **CHANGE** — 1 source branch; single; cap 55 (cap-2); Electabuzz, Kricketune; direct tempo; dialogue 1/1 resolved. Evolved unsupported level-53 Elekid to Electabuzz while preserving the two-member tempo single.
- 0215 **RETAIN** — 1 source branch; single; cap 55 (cap-3); Sawk; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0216 **RETAIN** — 1 source branch; single; cap 55 (cap-2); Blastoise, Wishiwashi; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0217 **DOSSIER FIX** — 1 source branch; double; cap 55 (cap-3); Toxapex, Lapras; direct tempo; dialogue 1/1 resolved. Corrected the reservation from removed Alomomola to the executable Lapras breather.
- 0218 **RETAIN** — 1 source branch; double; cap 55 (cap-1..+0); Hariyama, Dusclops, Conkeldurr, Melmetal; Trick Room; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0219 **RETAIN** — 1 source branch; double; cap 55 (cap-3); Lucario, Passimian, Pangoro; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0220 **RETAIN** — 1 source branch; double; cap 55 (cap-3); Dodrio, Flareon, Scolipede; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0221 **RETAIN** — 1 source branch; double; cap 55 (cap-3..-2); Ninjask, Swalot, Skuntank; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0222 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Musharna, Jynx, Mawile, Cresselia; Trick Room; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0223 **RETAIN** — 1 source branch; single; cap 55 (cap-2); Medicham, Tsareena; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0224 **RETAIN** — 1 source branch; double; cap 55 (cap-2..+0); Wailord, Huntail, Gorebyss; rain/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0225 **RETAIN** — 1 source branch; double; cap 55 (cap-3); Toucannon, Unfezant, Decidueye; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0226 **DIALOGUE FIX** — 1 source branch; double; cap 55 (cap-2..-1); Talonflame, Mandibuzz, Vileplume, Machoke; Tailwind/spread; dialogue 1/1 resolved. Rewrote Chester's all-bird claim to distinguish his two birds from the full mixed party.
- 0227 **DOSSIER FIX** — 1 source branch; double; cap 55 (cap-2); Magikarp, Feebas, Wishiwashi, Ditto, Kyogre, Gyarados; rain/setup/spread; dialogue 1/1 resolved. Corrected the counterplay target from removed Charizard to Mega Gyarados.
- 0228 **RETAIN** — 1 source branch; double; cap 55 (cap+0); Magnezone, Exploud, Kyurem Black, Komala; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0229 **RETAIN** — 1 source branch; single; cap 55 (cap+4); Volcarona; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0230 **RETAIN** — 1 source branch; double; cap 55 (cap-3); Leavanny, Butterfree, Galvantula; redirection; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0231 **RETAIN** — 1 source branch; double; cap 55 (cap-3); Ribombee, Araquanid, Vivillon; redirection/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0232 **RETAIN** — 1 source branch; double; cap 55 (cap-3); Masquerain, Ariados, Kricketune; redirection/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0233 **RETAIN** — 1 source branch; double; cap 55 (cap-3); Ledian, Beedrill, Escavalier; Tailwind/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0234 **RETAIN** — 1 source branch; double; cap 55 (cap-2..+0); Ninjask, Shedinja, Kartana, Charizard; sun/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0235 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Mantine, Rotom Fan, Parasect, Lapras; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0236 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Castform, Noivern, Goodra, Seismitoad; rain/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0237 **RETAIN** — 1 source branch; double; cap 55 (cap-3); Noctowl, Fearow, Tropius; sun/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0238 **DIALOGUE FIX** — 1 source branch; double; cap 55 (cap-2..+0); Staraptor, Farfetchd, Thundurus, Chesnaught; Tailwind/setup; dialogue 1/1 resolved. Rewrote Phil's all-bird claim to match his mixed Staraptor, Farfetch'd, Thundurus, and Chesnaught party.
- 0239 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Accelgor, Weezing, Palossand, Mudsdale; sand/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0240 **RETAIN** — 1 source branch; double; cap 55 (cap-4); Sharpedo, Basculin, Gyarados, Whiscash; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0241 **RETAIN** — 1 source branch; single; cap 55 (cap-2); Magcargo, Darmanitan; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0242 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Ludicolo, Heliolisk, Nuzleaf, Vespiquen; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0243 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Golbat, Dragalge, Liepard, Gourgeist Large; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0244 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Walrein, Floatzel, Malamar, Gourgeist Super; Trick Room/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0245 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Toxicroak, Eelektross, Scizor, Clefable; redirection/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0246 **RETAIN** — 1 source branch; double; cap 55 (cap-1); Pelipper, Ferrothorn, Gourgeist Small, Inteleon; rain/Tailwind/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0247 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Mightyena, Golisopod, Muk Alola, Vivillon Marine; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0248 **DOSSIER FIX** — 1 source branch; double; cap 55 (cap+0..+2); Ninetales Alola, Empoleon, Crawdaunt, Clefable, Ludicolo, Sharpedo; snow/redirection/setup/spread; dialogue 1/1 resolved. Corrected Shelly's counterplay target from removed Beedrill to Mega Sharpedo.
- 0249 **RETAIN** — 6 source branches; double; cap 55 (cap+0..+2); Araquanid, Swellow, Arcanine, Mimikyu, Vikavolt, Sceptile, Breloom, Swampert, +4 branch species; Trick Room/spread; dialogue 6/6 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0250 **RETAIN** — 1 source branch; double; cap 55 (cap-3); Greninja, Crobat, Drapion; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0251 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Bisharp, Muk, Grovyle, Archeops; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0252 **RETAIN** — 1 source branch; double; cap 55 (cap-3..+0); Manectric, Chatot, Kricketune, Chimecho; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0253 **RETAIN** — 1 source branch; double; cap 55 (cap-3); Dodrio, Xatu, Oricorio; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0254 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Fearow, Staraptor, Beautifly, Crabominable; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0255 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Ludicolo, Zangoose, Nidoqueen, Starmie; rain; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0256 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Shiftry, Seviper, Nidoking, Cloyster; Tailwind/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0257 **RETAIN** — 1 source branch; double; cap 55 (cap-3); Durant, Vespiquen, Masquerain; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0258 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Sableye, Kommo O, Whimsicott, Darkrai; Tailwind/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0259 **RETAIN** — 1 source branch; double; cap 55 (cap-3); Crustle, Claydol, Aerodactyl; Trick Room/Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0260 **DOSSIER FIX** — 1 source branch; double; cap 55 (cap-3..-2); Tangrowth, Wailord, Overqwil; rain; dialogue 1/1 resolved. Corrected Clarissa's counterplay target from removed Pyroar to Overqwil.
- 0261 **DOSSIER FIX** — 1 source branch; double; cap 55 (cap-2..+0); Meowstic, Castform, Emolga, Houndoom; sun/spread; dialogue 1/1 resolved. Corrected Angelica's counterplay target from removed Feraligatr to Mega Houndoom.
- 0262 **RETAIN** — 1 source branch; double; cap 55 (cap-3); Greninja, Weezing, Bisharp; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0263 **RETAIN** — 1 source branch; double; cap 55 (cap-2..+0); Skuntank, Mothim, Yveltal, Delphox; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0264 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Vikavolt, Ogerpon, Golduck; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0265 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Medicham, Lopunny, Mienshao; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0266 **RETAIN** — 1 source branch; double; cap 55 (cap-1..+0); Chatot, Kricketune, Exploud, Sylveon; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0267 **RETAIN** — 1 source branch; double; cap 55 (cap-1..+1); Mawile, Roserade, Swanna, Hydreigon, Mienshao, Drifblim; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0268 **RETAIN** — 1 source branch; double; cap 55 (cap-1..+2); Braviary, Garchomp, Scolipede, Magnezone, Blastoise, Dragalge; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0269 **RETAIN** — 1 source branch; single; cap 55 (cap-2); Roserade, Tropius; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0270 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Solrock, Exeggutor Alola, Delphox, Shaymin; Trick Room/Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0271 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Musharna, Girafarig, Espeon; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0272 **RETAIN** — 1 source branch; double; cap 55 (cap-3); Ninetales, Weepinbell, Ninetales Alola, Torterra; sun/snow/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0273 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Cofagrigus, Marowak Alola, Rotom Frost; Trick Room/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0274 **RETAIN** — 1 source branch; double; cap 55 (cap-1..+0); Aegislash, Comfey, Dragonite; Trick Room/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0275 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Pelipper, Altaria, Xatu; rain/Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0276 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Seviper, Zangoose, Glaceon; snow/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0277 **RETAIN** — 1 source branch; single; cap 55 (cap-2); Castform, Sawsbuck, Charizard; sun/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0278 **RETAIN** — 1 source branch; single; cap 55 (cap-2); Weezing, Salazzle; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0279 **RETAIN** — 1 source branch; double; cap 55 (cap-1); Absol, Goodra, Scizor, Jolteon; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0280 **RETAIN** — 1 source branch; double; cap 55 (cap-2..+0); Pinsir, Scolipede, Dustox, Drampa; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0281 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Zebstrika, Chatot, Kricketune; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0282 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Crobat, Slaking, Arcanine, Lapras, Venusaur, Suicune; Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0283 **RETAIN** — 1 source branch; double; cap 55 (cap+0); Swoobat, Noctowl, Noivern, Scyther; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0284 **RETAIN** — 1 source branch; double; cap 55 (cap+0); Staraptor, Honchkrow, Drifblim, Jumpluff; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0285 **RETAIN** — 1 source branch; double; cap 55 (cap-2..+0); Swellow, Yanmega, Mantine, Dragonite; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0286 **RETAIN** — 1 source branch; double; cap 55 (cap+0); Braviary, Pidgeot, Swanna, Absol; rain/Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0287 **RETAIN** — 1 source branch; double; cap 55 (cap+0); Gliscor, Mandibuzz, Hawlucha, Minior; Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0288 **RETAIN** — 1 source branch; double; cap 55 (cap+0); Tropius, Dodrio, Emolga, Archeops; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0289 **RETAIN** — 1 source branch; double; cap 55 (cap+0..+2); Tornadus, Landorus Therian, Zapdos, Celesteela, Altaria, Abomasnow; snow/Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0290 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Raichu, Togedemaru, Granbull, Rotom Fan; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0291 **RETAIN** — 1 source branch; double; cap 55 (cap+0); Noivern, Exploud, Magnezone, Sylveon; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0292 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Luxray, Linoone, Slowking; Trick Room/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0293 **RETAIN** — 1 source branch; single; cap 60 (cap-2); Banette, Mismagius; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0294 **RETAIN** — 1 source branch; double; cap 60 (cap+0); Swoobat, Slaking, Weezing, Regigigas; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0295 **RETAIN** — 1 source branch; double; cap 60 (cap-3..+0); Krookodile, Arbok, Latias; Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0296 **RETAIN** — 1 source branch; double; cap 60 (cap-3); Wormadam Trash, Wormadam Sandy, Wormadam; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0297 **RETAIN** — 1 source branch; double; cap 60 (cap-5..-4); Lucario, Togekiss, Magmortar, Serperior, Whiscash, Miltank; redirection/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0298 **RETAIN** — 1 source branch; double; cap 60 (cap-4); Swampert, Bouffalant, Electivire, Tropius, Simisear, Lycanroc Dusk; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0299 **RETAIN** — 1 source branch; double; cap 60 (cap+0); Shiftry, Zebstrika, Stoutland, Zarude; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0300 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Ninetales, Flygon, Pidgeot, Latios; sun/Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0301 **RETAIN** — 1 source branch; double; cap 55 (cap-2); Toxtricity, Kommo O, Primarina, Altaria; Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0302 **RETAIN** — 6 source branches; double; cap 60 (cap+0..+3); Araquanid, Swellow, Arcanine, Mimikyu, Vikavolt, Sceptile, Breloom, Swampert, +4 branch species; Trick Room/spread; dialogue 6/6 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0303 **RETAIN** — 1 source branch; double; cap 55 (cap+0); Meloetta, Exploud, Noivern, Gardevoir; Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0304 **CHANGE** — 1 source branch; double; cap 60 (cap-2); Mienshao, Milotic, Haxorus, Scizor, Togekiss, Volcarona; redirection/setup/spread; dialogue 1/1 resolved. Replaced Buffel's exact copy of the immediately preceding Gardevoir set with redirect-supported Quiver Dance Volcarona.
- 0305 **RETAIN** — 1 source branch; double; cap 60 (cap-3); Rhyperior, Haxorus, Lickilicky; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0306 **RETAIN** — 1 source branch; double; cap 60 (cap-1..+0); Cloyster, Steelix, Banette, Golurk; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0307 **RETAIN** — 1 source branch; double; cap 60 (cap-3); Gengar, Grumpig, Jynx, Spectrier; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0308 **RETAIN** — 1 source branch; double; cap 60 (cap-3); Hariyama, Hitmonlee, Emboar; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0309 **RETAIN** — 1 source branch; double; cap 60 (cap-3); Beheeyem, Gardevoir, Gallade, Deoxys Speed; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0310 **DOSSIER FIX** — 1 source branch; double; cap 60 (cap-3); Hypno, Hatterene, Wobbuffet; Trick Room/spread; dialogue 1/1 resolved. Removed the stale Alakazam comparison and described the actual Hypno-Hatterene-Wobbuffet room order.
- 0311 **RETAIN** — 1 source branch; double; cap 60 (cap-4..+2); Avalugg, Meganium, Raichu Alola, Donphan, Gorebyss, Greninja; rain/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0312 **RETAIN** — 1 source branch; double; cap 60 (cap-3); Samurott, Machamp, Aggron; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0313 **RETAIN** — 1 source branch; double; cap 60 (cap-3); Chandelure, Banette, Trevenant; Trick Room/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0314 **RETAIN** — 1 source branch; double; cap 60 (cap-3); Sableye, Dusknoir, Spiritomb; Trick Room; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0315 **DOSSIER FIX** — 1 source branch; double; cap 60 (cap-3..+0); Bronzong, Reuniclus, Mawile; Trick Room; dialogue 1/1 resolved. Corrected Cedric's counterplay target from removed Hawlucha to Mega Mawile.
- 0316 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Muk, Raticate Alola, Tentacruel, Slowking Galar; Trick Room; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0317 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Bruxish, Weavile, Wailord, Polteageist Antique; rain/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0318 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Jellicent, Amoonguss, Lanturn, Urshifu Rapid Strike; Trick Room/redirection; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0319 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Carracosta, Liepard, Qwilfish, Drapion; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0320 **DOSSIER FIX** — 1 source branch; double; cap 60 (cap+0..+2); Pelipper, Dhelmise, Heliolisk, Toxicroak, Seismitoad, Swampert; rain/Tailwind/spread; dialogue 1/1 resolved. Corrected Matt's counterplay target from removed Heatran to Mega Swampert.
- 0321 **CHANGE** — 1 source branch; double; cap 60 (cap-1); Swalot, Camerupt, Persian Alola, Houndoom; spread; dialogue 1/1 resolved. Replaced the middle of a three-use Liepard cluster with offensive Houndoom and removed the party's duplicated Fake Out/Foul Play support role.
- 0322 **RETAIN** — 1 source branch; double; cap 60 (cap-1); Hippowdon, Sandslash, Gastrodon, Kabuto; sand/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0323 **CHANGE** — 1 source branch; double; cap 60 (cap-1..+2); Liepard, Salazzle, Nidoqueen, Feraligatr; setup/spread; dialogue 1/1 resolved. Replaced Liepard's duplicated Encore with Thunder Wave because Salazzle already carries Encore in the same party.
- 0324 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Claydol, Garbodor, Umbreon, Arbok; Trick Room/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0325 **RETAIN** — 1 source branch; double; cap 60 (cap-1); Scrafty, Nidoking, Magcargo, Herdier; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0326 **RETAIN** — 1 source branch; double; cap 60 (cap-1); Talonflame, Bisharp, Venomoth, Dusknoir; Tailwind/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0327 **RETAIN** — 1 source branch; double; cap 60 (cap-1..+2); Krookodile, Victreebel, Incineroar, Floette; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0328 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Ariados, Heatran, Golurk, Infernape; redirection/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0329 **RETAIN** — 1 source branch; double; cap 60 (cap-1); Ting Lu, Skuntank, Weezing, Larvesta; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0330 **RETAIN** — 1 source branch; double; cap 60 (cap-1); Mightyena, Flygon, Drapion, Floette; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0331 **CHANGE** — 1 source branch; double; cap 60 (cap-1); Camerupt, Honchkrow, Shiftry, Blacephalon; Tailwind/spread; dialogue 1/1 resolved. Replaced Crobat with Moxie Honchkrow, removing the repeated Camerupt-Crobat core before Maxie's boss battle while retaining Tailwind.
- 0332 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Gliscor, Dustox, Magmortar, Froslass; Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0333 **RETAIN** — 1 source branch; double; cap 60 (cap-1); Cacturne, Pyroar, Palossand, Clefable; redirection/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0334 **RETAIN** — 1 source branch; double; cap 60 (cap-1); Mamoswine, Mandibuzz, Scolipede, Delcatty; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0335 **RETAIN** — 1 source branch; double; cap 60 (cap-1..+2); Sableye, Torkoal, Excadrill, Garchomp; sun/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0336 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Rhyperior, Amoonguss, Pangoro, Tauros; redirection/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0337 **RETAIN** — 1 source branch; double; cap 60 (cap+1..+2); Gigalith, Darmanitan, Excadrill, Coalossal, Dragapult, Gastrodon; sand/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0338 **RETAIN** — 1 source branch; double; cap 60 (cap+1..+2); Ninetales, Great Tusk, Chi Yu, Victini, Landorus, Chandelure; sun/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0339 **RETAIN** — 1 source branch; double; cap 60 (cap+0..+2); Groudon, Victreebel, Hydreigon, Crobat, Lycanroc Dusk, Camerupt; sun/Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0340 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Tentacruel, Dhelmise, Mightyena, Poipole; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0341 **RETAIN** — 1 source branch; double; cap 60 (cap-1); Sharpedo, Dragalge, Muk, Haunter; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0342 **RETAIN** — 1 source branch; double; cap 60 (cap-1); Seismitoad, Urshifu Rapid Strike, Scrafty, Drednaw; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0343 **RETAIN** — 1 source branch; double; cap 60 (cap-1..+2); Walrein, Floatzel, Malamar, Eelektross; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0344 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Cloyster, Kingler, Skuntank, Thievul; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0345 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Golbat, Clawitzer, Muk Alola, Eelektross; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0346 **RETAIN** — 1 source branch; double; cap 60 (cap-1..+2); Toxicroak, Toxapex, Wishiwashi, Falinks; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0347 **RETAIN** — 1 source branch; double; cap 60 (cap-1); Relicanth, Pangoro, Vileplume, Misdreavus; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0348 **RETAIN** — 1 source branch; double; cap 60 (cap+0..+2); Pelipper, Dhelmise, Heliolisk, Toxicroak, Seismitoad, Gyarados; rain/Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0349 **RETAIN** — 1 source branch; single; cap 60 (cap-2); Tentacruel, Pelipper; rain/Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0350 **RETAIN** — 1 source branch; single; cap 60 (cap-2); Octillery, Clawitzer; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0351 **RETAIN** — 1 source branch; double; cap 60 (cap-3); Wailord, Starmie, Primarina; rain/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0352 **CHANGE** — 1 source branch; double; cap 60 (cap-2); Lanturn, Gorebyss, Charjabug, Whiscash; rain/spread; dialogue 1/1 resolved. Replaced unsupported level-58 Palpitoad and its weak Mental Herb with Hydration Whiscash and a live Rindo Berry.
- 0353 **RETAIN** — 1 source branch; double; cap 60 (cap-3); Stoutland, Jellicent, Lumineon; rain/Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0354 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Gastrodon East, Lanturn, Nidorina, Lucario; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0355 **RETAIN** — 1 source branch; single; cap 60 (cap-2); Gyarados, Dewgong; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0356 **RETAIN** — 1 source branch; single; cap 60 (cap-2); Mantine, Azumarill; Tailwind/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0357 **RETAIN** — 1 source branch; double; cap 60 (cap-3); Poliwrath, Wishiwashi, Raichu; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0358 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Kingdra, Bibarel, Goodra, Seadra; rain/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0359 **RETAIN** — 1 source branch; double; cap 60 (cap-3); Luvdisc, Alomomola, Vaporeon, Empoleon; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0360 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Seaking, Slowbro, Bruxish, Dragonair; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0361 **RETAIN** — 1 source branch; double; cap 60 (cap-3..-2); Conkeldurr, Golisopod, Lucario; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0362 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Altaria, Rhyperior, Cryogonal, Rotom Wash; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0363 **RETAIN** — 1 source branch; single; cap 60 (cap-2); Staraptor, Gliscor; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0364 **RETAIN** — 1 source branch; double; cap 60 (cap-1); Manectric, Lucario, Vaporeon, Corsola; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0365 **RETAIN** — 1 source branch; single; cap 60 (cap-2); Swampert, Gogoat; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0366 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Haxorus, Ludicolo, Quagsire; rain/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0367 **RETAIN** — 1 source branch; double; cap 60 (cap-2..+0); Lapras, Audino, Zweilous, Magearna; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0368 **RETAIN** — 1 source branch; double; cap 60 (cap-3); Seaking, Raichu, Glaceon, Octillery; snow/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0369 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Floatzel, Barbaracle, Dragalge, Vullaby; Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0370 **RETAIN** — 1 source branch; single; cap 60 (cap-2); Beartic, Swanna; snow/Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0371 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Politoed, Huntail, Whiscash; rain/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0372 **RETAIN** — 1 source branch; double; cap 60 (cap-3); Kabutops, Golduck, Qwilfish; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0373 **RETAIN** — 1 source branch; double; cap 60 (cap-3..-2); Klinklang, Greninja, Manectric; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0374 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Yanmega, Blastoise, Glastrier, Pyukumuku; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0375 **RETAIN** — 1 source branch; single; cap 60 (cap-2); Corsola, Toxapex; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0376 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Tentacruel, Jellicent, Chatot, Fletchinder; rain/Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0377 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Crawdaunt, Pyukumuku, Kingler, Crabominable; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0378 **RETAIN** — 1 source branch; single; cap 60 (cap-2); Pidgeot, Talonflame; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0379 **RETAIN** — 1 source branch; double; cap 60 (cap-2..+0); Infernape, Accelgor, Pheromosa, Meganium; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0380 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Hariyama, Aggron, Basculin, Togekiss; redirection/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0381 **RETAIN** — 1 source branch; single; cap 60 (cap-2); Dodrio, Jolteon; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0382 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Fearow, Zebstrika, Butterfree, Corsola Galar; Tailwind/redirection; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0383 **RETAIN** — 1 source branch; double; cap 60 (cap-1..+1); Feraligatr, Marowak Alola, Scizor, Exeggutor, Ampharos, Cloyster; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0384 **RETAIN** — 1 source branch; double; cap 60 (cap-1..+1); Heatmor, Meganium, Dragonite, Lopunny, Milotic, Lumineon; Tailwind/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0385 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Wailord, Dhelmise, Lickilicky, Meowstic F; rain; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0386 **RETAIN** — 1 source branch; single; cap 60 (cap-2); Lickilicky, Omastar; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0387 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Seaking, Drampa, Alomomola, Diancie; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0388 **RETAIN** — 1 source branch; double; cap 60 (cap-2..+0); Slowking, Alakazam, Jirachi, Garchomp; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0389 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Medicham, Bronzong, Dusclops, Rhyperior; Trick Room/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0390 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Girafarig, Meowstic F, Metang, Mr. Mime; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0391 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Bruxish, Malamar, Solrock, Azelf; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0392 **RETAIN** — 1 source branch; double; cap 60 (cap-2..+2); Xatu, Gardevoir, Magearna, Glimmora; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0393 **RETAIN** — 1 source branch; double; cap 60 (cap-1..+0); Beheeyem, Espeon, Uxie, Calyrex; Trick Room/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0394 **RETAIN** — 1 source branch; double; cap 60 (cap-2..+0); Oranguru, Gothitelle, Torkoal, Drampa; sun/Trick Room/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0395 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Grumpig, Starmie, Necrozma, Meowstic F; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0396 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Jynx, Delphox, Reuniclus, Grumpig; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0397 **RETAIN** — 1 source branch; double; cap 60 (cap-2..+0); Pincurchin, Raichu Alola, Mr. Mime, Golisopod; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0398 **RETAIN** — 1 source branch; double; cap 60 (cap-2..+0); Indeedee F, Hatterene, Musharna, Wobbuffet; Trick Room/redirection/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0399 **RETAIN** — 1 source branch; double; cap 60 (cap-2); Hypno, Swoobat, Slurpuff, Comfey; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0400 **RETAIN** — 1 source branch; double; cap 60 (cap+0..+2); Tapu Lele, Lunala, Victini, Cresselia, Slowbro, Calyrex Ice; Trick Room; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0401 **DOSSIER FIX** — 1 source branch; double; cap 70 (cap-1); Salazzle, Palossand, Amoonguss, Hypno; redirection/spread; dialogue 1/1 resolved. Removed a nonexistent Palossand Sand Stream mode and synchronized Water Compaction, redirection, and Thunder Wave facts.
- 0402 **DOSSIER FIX** — 1 source branch; double; cap 70 (cap-1); Mightyena, Camerupt, Shiftry, Necrozma Dusk Mane; Tailwind/setup/spread; dialogue 1/1 resolved. Removed stale Trick Room role labels from a Tailwind and Dragon Dance party.
- 0403 **DOSSIER FIX** — 1 source branch; double; cap 70 (cap-2); Torkoal, Claydol, Rhyperior, Mewtwo; sun/Trick Room/spread; dialogue 1/1 resolved. Corrected Mewtwo X from stale Psystrike special prose to its physical Psycho Cut set.
- 0404 **RETAIN** — 1 source branch; double; cap 70 (cap-1); Flygon, Crobat, Drapion, Klefki; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0405 **DOSSIER FIX** — 1 source branch; double; cap 70 (cap-1); Duosion, Pangoro, Nidoking, Pyroar; Trick Room/spread; dialogue 1/1 resolved. Corrected source order: Duosion sets Trick Room first and Pyroar is the final Heat Wave attacker.
- 0406 **DOSSIER FIX** — 1 source branch; double; cap 70 (cap-1); Hippowdon, Sandslash, Claydol, Durant; sand/Trick Room/spread; dialogue 1/1 resolved. Corrected Sandslash's removed Earthquake to source High Horsepower.
- 0407 **DOSSIER FIX** — 1 source branch; double; cap 70 (cap-1); Liepard, Incineroar, Mamoswine, Miltank; spread; dialogue 1/1 resolved. Corrected Mamoswine's removed Earthquake to source High Horsepower.
- 0408 **RETAIN** — 1 source branch; double; cap 70 (cap+0..+2); Gigalith, Excadrill, Steelix, Gliscor, Darmanitan, Gastrodon; sand/Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0409 **RETAIN** — 2 source branches; multi; cap 70 (cap+0..+2); Ninetales, Krookodile, Houndoom, Victreebel, Crobat, Camerupt; sun/Tailwind/setup/spread; dialogue 2/2 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0410 **RETAIN** — 1 source branch; double; cap 70 (cap+0); Lurantis, Oricorio Pom Pom, Mudsdale, Reshiram; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0411 **RETAIN** — 1 source branch; double; cap 70 (cap-2..+0); Noctowl, Xatu, Sirfetchd; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0412 **RETAIN** — 1 source branch; double; cap 70 (cap-2); Scizor, Donphan, Dunsparce, Amoonguss; redirection/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0413 **RETAIN** — 1 source branch; single; cap 70 (cap-2); Slowking, Braviary; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0414 **RETAIN** — 1 source branch; single; cap 70 (cap-2); Unfezant, Galvantula; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0415 **RETAIN** — 1 source branch; single; cap 70 (cap-2); Walrein, Sharpedo; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0416 **RETAIN** — 1 source branch; double; cap 70 (cap-2); Lanturn, Slurpuff, Tapu Fini, Unfezant; Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0417 **RETAIN** — 1 source branch; double; cap 70 (cap-2..+0); Diggersby, Empoleon, Marshtomp, Mewtwo; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0418 **RETAIN** — 1 source branch; single; cap 70 (cap-2); Gyarados, Wishiwashi; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0419 **RETAIN** — 1 source branch; double; cap 70 (cap-2); Seaking, Beartic, Electabuzz, Aromatisse; snow; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0420 **RETAIN** — 1 source branch; double; cap 70 (cap-2); Gastrodon East, Poliwrath, Vanilluxe, Zekrom; snow/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0421 **RETAIN** — 1 source branch; single; cap 70 (cap-2); Tauros, Slowking; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0422 **DOSSIER FIX** — 1 source branch; double; cap 70 (cap-1..+0); Bronzong, Gourgeist, Heracross, Lycanroc, Florges, Kyurem White; spread; dialogue 1/1 resolved. Corrected Kyurem's removed Blizzard to source Draco Meteor and Ice Beam coverage.
- 0423 **RETAIN** — 1 source branch; double; cap 70 (cap-1..+2); Pidgeot, Probopass, Sceptile, Volcarona, Froslass, Pyroar; Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0424 **RETAIN** — 1 source branch; double; cap 70 (cap-2); Miltank, Azumarill, Furfrou Kabuki, Whiscash; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0425 **RETAIN** — 1 source branch; double; cap 70 (cap-2); Pelipper, Relicanth, Kabutops, Blastoise; rain/Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0426 **CHANGE** — 1 source branch; double; cap 70 (cap-2); Scyther, Dedenne, Palkia, Wailord; Tailwind; dialogue 1/1 resolved. Evolved unsupported level-68 Wailmer to legal Water Veil Wailord while preserving Water Spout pressure.
- 0427 **RETAIN** — 1 source branch; single; cap 70 (cap-2); Snorlax, Quagsire; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0428 **RETAIN** — 1 source branch; single; cap 70 (cap-2); Nidoking, Sharpedo; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0429 **RETAIN** — 1 source branch; double; cap 70 (cap-2); Nidoqueen, Primarina, Galvantula, Castform; sun/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0430 **RETAIN** — 1 source branch; double; cap 70 (cap-4..+2); Magikarp, Gyarados, Horsea, Tentacool, Seadra, Raichu; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0431 **RETAIN** — 1 source branch; double; cap 70 (cap-3); Hawlucha, Primeape, Throh; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0432 **RETAIN** — 1 source branch; double; cap 70 (cap-2); Breloom, Ursaring, Luxray; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0433 **RETAIN** — 1 source branch; double; cap 70 (cap-1..+0); Charizard, Lilligant, Porygon-Z; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0434 **RETAIN** — 1 source branch; double; cap 70 (cap-1..+0); Rampardos, Charizard, Sawsbuck, Sableye; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0435 **RETAIN** — 1 source branch; double; cap 70 (cap-1..+0); Amoonguss, Medicham, Flareon, Beartic; snow/redirection; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0436 **RETAIN** — 1 source branch; double; cap 70 (cap-3); Walrein, Vaporeon, Lugia, Sneasel; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0437 **RETAIN** — 1 source branch; double; cap 70 (cap-3); Qwilfish, Seismitoad, Crawdaunt, Swalot; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0438 **RETAIN** — 1 source branch; double; cap 70 (cap-3); Kingdra, Lumineon, Ninetales Alola, Arctozolt; rain/snow/Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0439 **RETAIN** — 1 source branch; double; cap 70 (cap-1..+2); Golem Alola, Musharna, Torterra, Cofagrigus, Aggron, Salamence; Trick Room/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0440 **RETAIN** — 1 source branch; double; cap 70 (cap-3); Honchkrow, Altaria, Noctowl, Emolga; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0441 **RETAIN** — 1 source branch; double; cap 70 (cap-1..+0); Gengar, Carracosta, Aromatisse, Rotom Frost; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0442 **RETAIN** — 1 source branch; double; cap 70 (cap-1..+0); Hariyama, Ferrothorn, Gothitelle, Avalugg; Trick Room/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0443 **RETAIN** — 1 source branch; single; cap 70 (cap-3); Linoone, Simipour; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0444 **RETAIN** — 1 source branch; double; cap 70 (cap-3..+0); Luvdisc, Alomomola, Omastar, Scizor; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0445 **CHANGE** — 1 source branch; double; cap 70 (cap-3); Cramorant, Togekiss, Oshawott, Octillery; Tailwind/redirection/setup/spread; dialogue 1/1 resolved. Evolved unsupported level-67 Remoraid to Octillery and corrected the Bird Keeper text to describe the mixed party truthfully.
- 0446 **RETAIN** — 1 source branch; double; cap 70 (cap-1); Druddigon, Tyrantrum, Noivern; Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0447 **RETAIN** — 1 source branch; double; cap 70 (cap-3); Hitmonchan, Machamp, Crabominable; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0448 **CHANGE** — 1 source branch; double; cap 70 (cap-3); Poliwrath, Raticate, Psyduck, Lanturn; spread; dialogue 1/1 resolved. Evolved unsupported level-67 Chinchou to Lanturn while preserving Volt Absorb special control.
- 0449 **RETAIN** — 1 source branch; double; cap 70 (cap-3..+0); Bewear, Passimian, Wooper, Scolipede; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0450 **RETAIN** — 1 source branch; double; cap 70 (cap-1..+0); Venusaur, Escavalier, Arcanine; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0451 **RETAIN** — 1 source branch; double; cap 70 (cap-3); Machamp, Dhelmise, Ducklett, Shellos; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0452 **RETAIN** — 1 source branch; double; cap 70 (cap-1); Mightyena, Crobat, Sharpedo, Barraskewda; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0453 **RETAIN** — 1 source branch; double; cap 70 (cap-1..+0); Huntail, Muk, Scrafty, Scovillain; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0454 **RETAIN** — 1 source branch; double; cap 70 (cap+0..+2); Ninetales Alola, Empoleon, Crawdaunt, Clefable, Ludicolo, Beedrill; snow/redirection/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0455 **RETAIN** — 1 source branch; double; cap 70 (cap-1); Mandibuzz, Gyarados, Arbok, Skuntank; Tailwind/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0456 **CHANGE** — 1 source branch; double; cap 70 (cap-2); Wailord, Skuntank, Seaking, Morpeko; rain; dialogue 1/1 resolved. Replaced the second consecutive exact Skuntank Taunt set with Haze support for the rain formation.
- 0457 **RETAIN** — 1 source branch; double; cap 70 (cap-1); Gorebyss, Purugly, Toxapex, Silvally Dark; rain/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0458 **RETAIN** — 1 source branch; double; cap 70 (cap+1..+2); Kyogre, Goodra, Tentacruel, Poliwrath, Eelektross, Scrafty; rain/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0459 **RETAIN** — 1 source branch; double; cap 70 (cap-1..+0); Cloyster, Floatzel, Jellicent, Simipour; setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0460 **RETAIN** — 1 source branch; double; cap 70 (cap-1..+0); Lanturn, Golisopod, Swanna, Primarina; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0461 **RETAIN** — 1 source branch; double; cap 70 (cap-2); Greninja, Dewgong, Quagsire, Skarmory; Tailwind; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0462 **RETAIN** — 1 source branch; double; cap 70 (cap-1..+0); Seaking, Starmie, Lumineon, Huntail; Tailwind/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0463 **CHANGE** — 1 source branch; double; cap 70 (cap-1..+0); Manaphy, Drednaw, Toxapex, Omastar; rain/setup/spread; dialogue 1/1 resolved. Replaced the repeated Gorebyss-Toxapex core with physical Swift Swim Drednaw beside Manaphy's turn-one rain.
- 0464 **RETAIN** — 1 source branch; double; cap 70 (cap-1); Carracosta, Araquanid, Wishiwashi; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0465 **RETAIN** — 1 source branch; double; cap 70 (cap-2); Vaporeon, Feraligatr, Gastrodon East, Barbaracle; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0466 **RETAIN** — 1 source branch; double; cap 70 (cap-1..+2); Slowking, Azumarill, Relicanth, Staraptor; Trick Room/Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0467 **RETAIN** — 1 source branch; double; cap 70 (cap-1); Milotic, Primarina, Golduck; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0468 **RETAIN** — 1 source branch; double; cap 70 (cap-1); Luvdisc, Lapras, Samurott; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0469 **RETAIN** — 1 source branch; double; cap 70 (cap+0..+2); Politoed, Gothitelle, Kyogre, Gastrodon, Palkia, Kingdra; rain/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0470 **RETAIN** — 1 source branch; double; cap 80 (cap-2..+0); Alakazam, Mr. Rime, Darmanitan; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0471 **RETAIN** — 1 source branch; double; cap 80 (cap-2..+0); Cofagrigus, Mismagius, Gothitelle; Trick Room/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0472 **RETAIN** — 1 source branch; double; cap 80 (cap+0); Indeedee F, Hatterene, Reuniclus, Gardevoir; Trick Room/redirection/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0473 **RETAIN** — 1 source branch; double; cap 80 (cap-2); Starmie, Chimecho, Pikachu; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0474 **RETAIN** — 1 source branch; double; cap 80 (cap-1..+0); Gallade, Electivire, Ampharos; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0475 **RETAIN** — 1 source branch; double; cap 80 (cap+0); Wobbuffet, Beheeyem, Hoopa, Musharna; Trick Room; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0476 **RETAIN** — 1 source branch; double; cap 80 (cap+1..+2); Togekiss, Garchomp, Magnezone, Roserade, Azumarill, Swampert; redirection/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0477 **RETAIN** — 1 source branch; double; cap 80 (cap-2); Trevenant, Sudowoodo, Venomoth, Klinklang, Chesnaught, Manectric; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0478 **RETAIN** — 1 source branch; double; cap 80 (cap-1..+1); Rhyperior, Sigilyph, Sableye, Weavile, Conkeldurr, Guzzlord; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0479 **DOSSIER FIX** — 1 source branch; double; cap 80 (cap-1..+2); Forretress, Lilligant, Salazzle, Aerodactyl, Vaporeon, Tatsugiri Curly; Tailwind/spread; dialogue 1/1 resolved. Removed a nonexistent Commander dependency; Tatsugiri has Storm Drain before and after its self-contained Mega reveal.
- 0480 **RETAIN** — 1 source branch; double; cap 80 (cap+0..+1); Slaking, Durant, Butterfree, Rayquaza; Tailwind/redirection/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0481 **RETAIN** — 1 source branch; double; cap 80 (cap-2); Audino, Slaking, Infernape, Dialga; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0482 **RETAIN** — 1 source branch; double; cap 80 (cap-1..+1); Galvantula, Nidoking, Porygon-Z, Gengar, Druddigon, Zacian; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0483 **RETAIN** — 1 source branch; double; cap 80 (cap-1..+1); Blaziken, Escavalier, Dragonite, Sawsbuck, Ribombee, Orbeetle; Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0484 **RETAIN** — 1 source branch; double; cap 80 (cap-1..+2); Chansey, Meganium, Machamp, Hydreigon, Probopass, Tyranitar; sand/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0485 **RETAIN** — 1 source branch; double; cap 80 (cap-2); Machamp, Shiinotic, Gothitelle, Mismagius; Trick Room/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0486 **RETAIN** — 1 source branch; double; cap 80 (cap-1..+0); Pinsir, Pachirisu, Lickilicky, Naganadel; redirection/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0487 **RETAIN** — 1 source branch; double; cap 80 (cap-1..+0); Bisharp, Delphox, Dodrio, Electivire, Lurantis, Flygon; Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0488 **RETAIN** — 1 source branch; double; cap 80 (cap-1..+2); Torterra, Staraptor, Sylveon, Lucario, Tentacruel, Victreebel; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0489 **RETAIN** — 1 source branch; double; cap 80 (cap-2); Tyranitar, Leavanny, Stoutland, Sandslash, Dugtrio Alola, Eternatus; sand/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0490 **RETAIN** — 1 source branch; double; cap 80 (cap-1..+2); Torkoal, Venusaur, Heliolisk, Lycanroc, Zamazenta, Tyrantrum; sun/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0491 **RETAIN** — 1 source branch; double; cap 80 (cap-1..+0); Jynx, Sceptile, Xerneas, Duraludon; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0492 **RETAIN** — 1 source branch; double; cap 80 (cap-1..+0); Abomasnow, Magmortar, Virizion, Bisharp; snow/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0493 **RETAIN** — 1 source branch; double; cap 80 (cap+0..+2); Incineroar, Yveltal, Darkrai, Urshifu, Hoopa Unbound, Absol; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0494 **RETAIN** — 1 source branch; double; cap 80 (cap+0..+2); Gengar, Marshadow, Lunala, Giratina, Shedinja, Calyrex Shadow; sun/Tailwind/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0495 **RETAIN** — 1 source branch; double; cap 80 (cap+0..+2); Ninetales Alola, Kyurem, Chien Pao, Articuno, Glastrier, Abomasnow; snow/Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0496 **RETAIN** — 1 source branch; double; cap 80 (cap+0..+2); Dialga, Zygarde 50 Power Construct, Reshiram, Koraidon, Rayquaza, Salamence; Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0497 **RETAIN** — 1 source branch; double; cap 80 (cap+0..+2); Kyogre, Zapdos, Ferrothorn, Manaphy, Palkia, Milotic; rain/Tailwind/setup; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0498 **RETAIN** — 1 source branch; double; cap 100 (cap+0); Indeedee F, Gallade, Dragapult, Heatran, Incineroar, Suicune; Tailwind/redirection/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0499 **RETAIN** — 1 source branch; double; cap 100 (cap+0); Sableye, Mienshao, Swanna, Gliscor, Vikavolt, Typhlosion; Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0500 **RETAIN** — 1 source branch; double; cap 100 (cap-3..-2); Gigalith, Aron, Magnemite, Swinub, Whimsicott, Rotom Wash; sand/Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0501 **RETAIN** — 1 source branch; double; cap 100 (cap+0); Cresselia, Volcanion, Mawile, Salamence, Kartana, Slurpuff; Trick Room/Tailwind/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0502 **RETAIN** — 1 source branch; single; cap 100 (cap-2); Machamp, Basculin; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0503 **RETAIN** — 1 source branch; single; cap 100 (cap-2); Carracosta, Sharpedo; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0504 **CHANGE** — 1 source branch; double; cap 100 (cap+0); Meowstic, Liepard, Persian Alola, Incineroar, Mesprit, Absol; direct tempo; dialogue 1/1 resolved. Replaced Colton's fourth Fake Out and exact Wally Incineroar copy with Will-O-Wisp in the all-cat formation.
- 0505 **RETAIN** — 1 source branch; double; cap 100 (cap-2..+0); Manectric, Lapras, Clawitzer, Zygarde 50 Power Construct; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0506 **RETAIN** — 1 source branch; double; cap 100 (cap-2..+0); Braviary, Genesect, Scolipede; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0507 **RETAIN** — 1 source branch; double; cap 100 (cap-2); Miltank, Tauros, Cobalion, Primeape; spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0508 **RETAIN** — 1 source branch; double; cap 100 (cap-2); Persian Alola, Buzzwole, Weavile; direct tempo; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0509 **RETAIN** — 1 source branch; double; cap 100 (cap-2..+0); Tsareena, Arceus, Salazzle; setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0510 **RETAIN** — 1 source branch; double; cap 100 (cap+0); Kyogre, Arceus Electric, Tornadus Therian, Latios, Omastar, Starmie; rain/Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0511 **RETAIN** — 1 source branch; double; cap 100 (cap-2); Ninetales, Kangaskhan, Gengar, Tangrowth, Mewtwo, Zeraora; sun/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0512 **RETAIN** — 1 source branch; double; cap 100 (cap+0); Skarmory, Aerodactyl, Cradily, Melmetal, Regidrago, Metagross; Tailwind/setup/spread; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.
- 0513 **RETAIN** — 1 source branch; double; cap 100 (cap+0); Roserade, Togekiss, Spiritomb, Lucario, Raikou, Garchomp; redirection; dialogue 1/1 resolved. No fresh source contradiction after individual party, order, cap, format, and dialogue review.

## Verification transcript

- `python3 scripts/implement_emerald_champions_master_battles.py --through-encounter 513 --verify-only` — PASS, 561 branches.
- `python3 scripts/audit_emerald_champions_master_battles.py` — PASS, 513 encounters / 561 branches / 84.49% doubles-like / 92 Megas / 57 legendaries.
- `python3 scripts/verify_trainer_ability_legality.py` — PASS, 2,147 authored Abilities.
- `python3 scripts/verify_trainer_runtime_coherence.py` — PASS, 2,147 authored Pokemon; five reviewed dual-speed parties.
- `python3 scripts/align_emerald_champions_battle_scripts.py` — PASS, all 561 branch formats.
- `python3 scripts/verify_emerald_champions_story.py` — PASS, 10,846 story lines and 47,623 literal Hoenn dialogue lines within 216px.
- `git diff --check` — PASS.

Compilation and runtime execution were intentionally not performed in this pass.
