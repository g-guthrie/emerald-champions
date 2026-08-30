# Emerald Champions Wild Distribution Source Blueprint

Status: implemented source audit and retained design rationale. The executable
tables, scripts, Signs, acquisition paths, and release gates now materialize
the decisions summarized here; `src/data/wild_encounters.json` remains ground
truth.

Source snapshot: `e7df953860be1a250f6720fa4579aebe30436a04`, read from the executable inputs in `src/data/wild_encounters.json`, `src/wild_encounter.c`, `src/emerald_champions_battle_sets.c`, `src/data/pokemon/legendary_signs.h`, `src/legendary_signs.c`, `src/starter_choose.c`, `src/caps.c`, `src/data/pokemon/form_change_tables.h`, map JSON, and live map scripts. Campaign documents were not treated as ground truth.

Implementation checkpoint: the complete opening ecosystem through Wattson is now authored directly in `wild_encounters.json`. Routes 101-110 and 116, Petalburg Woods 1-3, Rusturf Tunnel, Granite Cave, Dewford Meadow and Manor, Seaspray Cave, and New Mauville have distinct live tables. The verified common-slot generation mix is 91.3%/8.7%/0.0% for Gen 1-3/4-6/7-9 in the Stone act and 73.5%/21.4%/5.1% from Dewford through Wattson. Twelve required doubles roles remain available at 4% or better; Scyther, Beldum, Larvitar, Frigibax, and restored Ultra Beasts are 5%, while Bagon and Dreepy remain optional 1% surprises with more practical later sources. All ordinary fossil families were removed in favor of Devon's finite fossil-revival loop.

Midgame checkpoint: Routes 111-123 (excluding completed Route 116) and 49 connected desert, volcano, ruins, meadow, Meteor Falls, Scorched Slab, Mt. Pyre, and Safari maps are now directly authored. Numbered-route common slots ease from 87.1%/8.6%/4.3% in the Dynamo-Heat band to 66.2%/25.0%/8.8% by Balance-Feather. Eleven midgame role contracts remain non-grindy, 21 Mega-base timing contracts are proven, every connected floor has a distinct species signature, and nine major trophies stay at intentional 1-5% rates. Kubfu, Chi-Yu, Ting-Lu, Type: Null, and Ogerpon are now one-time 8% conditional Signs with locally obtainable partner requirements instead of repeatable ordinary catches. Safari remains a deliberate Kanto-Johto nostalgia exception and contains no campaign-mandatory role exclusive to its flee rules.

Final checkpoint: Routes 124-134 and 66 connected town-water, underwater, Abandoned Ship, Shoal Cave, Seafloor Cavern, Cave of Origin, Magma Hideout, Sky Pillar, Victory Road, and postgame maps are source-covered. Sixty-five have directly authored tables; the canonical nine-set Altering Cave rotation is deliberately preserved and pinned. Excluding canonical all-Wynaut Route 130, late common slots are 60.0%/30.0%/10.0% Gen 1-3/4-6/7-9; the trophy layer is 43.9%/29.8%/26.3%, preserving nostalgia while visibly advancing. Twelve League-role contracts, 16 final Mega-base contracts, 24 biome anchors, and 14 trophy rates are proven. Chien-Pao, Manaphy, Suicune, Tapu Fini, Terrakion, Volcanion, and Keldeo are finite 8% conditional Signs, leaving only seven curated Ultra Beasts as ordinary Legendary Signs. No ordinary Milotic remains; Feebas keeps its native path, gains a non-grindy 5% underwater Route 126 source, and appears as a 1% Seafloor approach trophy.

## Product contract

Emerald Champions is not a collection simulator with hard battles attached. It is a competitive doubles campaign that removes preparation grind while preserving Emerald's adventure, nostalgia, discovery, and sense of place.

The wild distribution therefore has five simultaneous jobs:

1. Make a viable doubles toolbox available before every difficult chapter.
2. Keep early Hoenn recognizably Kanto, Johto, and Hoenn, then gradually introduce later generations without hard generation walls.
3. Preserve special species as rewards for special places, quests, fossils, trades, and the Game Corner.
4. Keep every ordinary catch immediately usable through its authored competitive preset.
5. Ensure a Mega Stone is never a dead reward because its species is unavailable for the rest of that campaign.

## Historical source-grounded diagnosis

This section records the pre-rewrite defects that motivated the implemented
tables. Present-tense statements below describe that audited input snapshot,
not the current executable distribution summarized by the checkpoints above.

The central criticism is correct, but several exact numbers describe a different snapshot or a different counting method.

- Generation progression is backwards. Across numbered Hoenn routes 101-134, using every encounter method and one vote per unique species on a route, mean generation versus route number is `r = -0.573`. Weighting every physical encounter slot gives `r = -0.438`. The report's `-0.299` was not reproducible from this source snapshot, but its conclusion is conservative.
- Variety declines. Unique species per numbered route versus route number is `r = -0.456`. The exact report value `-0.587` was not reproducible, but the structural problem is real.
- Numbered Hoenn routes currently contain 240 land slots, 150 Surf slots, 300 fishing slots, 55 Rock Smash slots, and 48 hidden slots. Water plus fishing is 450 of 793 method slots, or 56.7%. The pasted 276/310 counts are stale, but the water-budget criticism is true.
- Starter families are improperly used as ordinary catches. The exact base-stage occurrences are Sprigatito on Route 101, Fuecoco on Route 103, Sobble on Route 104's Old Rod, Grookey and Scorbunny on Route 117, and Charmander in Fiery Path. Emboar, Primarina, and Greninja are also repeated as ordinary late-game catches.
- Route 109's Rock Smash `encounter_rate` is 255. `WildEncounterCheck` multiplies that by 16 and clamps it to the engine maximum, so smashing a valid rock effectively always starts a battle. This should be 20 unless deliberate UX testing proves another ordinary value.
- Ordinary wild competitive presets are real, not just documented intent. `CreateWildMon` calls `ApplyEmeraldChampionsRandomWildSet`; that routine selects uniformly with `RandomUniform` over every currently visible set and installs moves, item, nature, Ability, and Stat Points. There are 585 unique species/forms in Hoenn campaign wild tables. The only table form without a direct default-set row is Gimmighoul Chest, which resolves through its form family.
- Ogerpon is a concrete exception and bug. Route 120 contains `SPECIES_OGERPON` at a 10% land slot, but Ogerpon is `isSubLegendary`, is not an ordinary-wild Legendary Sign, and is therefore excluded from the competitive-wild path. It arrives unlike the game's promised ordinary catches. It should become a one-time quest encounter, not be special-cased into ordinary grass.
- The repeatable ordinary-legendary layer is too loose. Catching one records its Legendary Sign bit, but the underlying JSON slot remains and `IsLegendarySignOrdinaryWildSpecies` does not suppress later encounters. Ultra Beasts may intentionally remain repeatable wild trophies in restored maps. Named legendary and mythical species should not.
- The bad early distribution is actively enforced, not accidental residue. `scripts/emerald_champions_wild_distribution.py` imports the preserved table from commit `33202c1`, requires Sprigatito, Fuecoco, Dreepy, Scyther, and Axew in the first six areas, pins every restored Ultra Beast to slot 6, and injects global roster holes into fixed 5-10% slots. The release gate runs this script in check mode. Editing JSON without replacing those assertions will either fail CI or be undone the next time `--write` runs.
- The existing roster verifier passes but overstates single-save availability. It reports all 203 Champions families pre-League because `direct_species()` adds every species token in `starter_choose.c`; a real save receives only one of those 27 starters. Recomputing against only Hoenn map IDs and one chosen starter leaves 17 or 18 Champions families unavailable in a real save, depending on the starter. With all 27 starter opportunities included, the missing count returns to zero. The proposed Game Corner archive closes that exact gap.
- The verifier's map filter is also too weak: it rejects map names containing `_FRLG`, but many FireRed/LeafGreen wild IDs are unsuffixed names such as `MAP_ROUTE1`. Future availability proofs must build the allowed map-ID set from non-FRLG groups in `data/maps/map_groups.json`, not infer version from an encounter-table string.

## Encounter-layer rules

Use Emerald's existing slot rates rather than inventing a new allocator.

### Land

- Slots 0-5, 80% total: route identity and nostalgia. Mostly generations 1-3 in the opening, mixed generations in the middle, and modern species later.
- Slots 6-9, 18% total: reliable doubles tools. These are not trophy slots. A needed role must never be buried at 1%.
- Slots 10-11, 2% total: trophies and deliberate generational surprises. No species that the design expects a player to target for a required role or Mega may exist only here.
- Hidden encounters: chase variants and flavorful alternatives, not mandatory role coverage. `HIDDEN_MON_PROBABILTY` is currently 15, so hidden content must not be described as impossibly rare.

### Surf, rods, and Rock Smash

- Surf's 60% and 30% slots carry the area's identity; its 5%, 4%, and 1% slots carry tools and trophies.
- Old Rod must remain a usable early-game source, not a starter dispenser. Its two slots should be simple water species with distinct roles.
- Good Rod should contain evolutionary bridges and team-building tools.
- Super Rod may contain fully evolved trophies, but no repeatable named legendary.
- Rock Smash should reward checking terrain. It should not duplicate the land table and should not fire on every rock.

### Generation bias

This is a sliding mood, not a quota.

- Stone through Dynamo: approximately 70% generation 1-3 in common slots, 25% generation 4-6, 5% generation 7-9.
- Heat through Balance: approximately 45% generation 1-3, 35% generation 4-6, 20% generation 7-9.
- Feather through Mind: approximately 30% generation 1-3, 35% generation 4-6, 35% generation 7-9.
- Rain and League approach: approximately 20% generation 1-3, 30% generation 4-6, 50% generation 7-9.
- Trophy slots can invert the current act. That is how a newer surprise appears early or a Kanto pseudo-legend appears late without destroying the route's overall identity.

## Chapter role guarantees

These guarantees are availability contracts, not species quotas. One species may cover several roles.

| Before milestone | Cap | Roles that must already be obtainable | Safe source examples |
|---|---:|---|---|
| Roxanne | 14 | Intimidate, Fake Out, redirection, Trick Room, Tailwind, priority, sleep, one bulky pivot | Shinx/Poochyena; Meowth or Makuhita; Foongus; Ralts; Scyther; Shroomish; Lotad/Seedot |
| Brawly | 20 | Wide Guard, screens, physical and special speed control, anti-setup | Geodude/Carbink; Abra; Bronzor; Makuhita; Onix |
| Wattson | 30 | Follow Me, Prankster support, Ground pressure, rain and sun cores | Pachirisu; Murkrow; Shellos; Torkoal; Pelipper/Lotad |
| Flannery | 40 | Sand, weather denial, spread Ground/Rock, Fighting pressure, priority | Hippopotas/Trapinch; Psyduck or Cloud Nine option; Machop/Tyrogue; Gible; Fletchinder |
| Norman | 45 | Ghost immunity pressure, burn, hard physical walls, mixed-speed modes | Shuppet/Duskull; Sableye; Bronzor; Rotom quest; bulky Water and Steel options |
| Winona | 55 | Electric and Ice answers, Tailwind mirrors, redirection, anti-weather | Manectric; Raichu family; Froslass line; Pachirisu; Tyranitar line; Wide Guard users |
| Tate and Liza | 60 | Dark/Ghost pressure, terrain control, Trick Room reversal, spread mitigation | Absol; Shuppet/Duskull; Indeedee line; Oranguru; Mienshao/Hitmontop; Steel types |
| Juan | 70 | Full weather war, priority, Haze, redirection, anti-setup, multiple Mega candidates | Torkoal/Pelipper/Tyranitar/Abomasnow lines; Murkrow; Amoonguss; Milotic; broad Mega roster |
| League | 80 | Every major doubles role and every non-postgame Mega base | Verified by an automated availability contract, not prose |

## Starter-family acquisition

Ordinary grass and rods are the wrong place for regional starters. The opening choice should remain meaningful, while the rest become explicit rewards.

### Remove these exact ordinary slots

| Current source | Remove | Safe replacement | Reason |
|---|---|---|---|
| Route 101 land slot 4 | Sprigatito | Pidgey | Restores early Kanto/Hoenn texture and supports the early Pidgeotite reward |
| Route 103 land slot 4 | Fuecoco | Growlithe | Keeps an early Intimidate/fire option without erasing starter exclusivity |
| Route 104 Old Rod slot 1 | Sobble | Tentacool | Restores a recognizable coastal Old Rod table |
| Route 117 land slot 8 | Grookey | Exeggcute | Preserves a Grass toolbox and adds Trick Room potential |
| Route 117 land slot 9 | Scorbunny | Ponyta | Preserves the pastoral/fire identity without a starter |
| Fiery Path land slot 9 | Charmander | Houndour | Makes the nearby Houndoominite usable and keeps the volcanic theme |
| Magma Hideout 4F slots 9 and 11 | Emboar | Heatmor and Magmar | Keeps high-end fire pressure without repeatable evolved starters |
| Route 126 Surf slots 2-4 | Primarina | Milotic, Gorebyss, Huntail | Makes the basin rare and elegant without handing out a starter line |
| Route 128 Super Rod slots 8-9 | Primarina | Dragalge and Dhelmise | Keeps a late deep-sea trophy identity |
| Seafloor Cavern rooms 1-8 slots 8 and 10 | Greninja | Room-specific Barraskewda, Grapploct, Basculegion, Dragalge, or Dhelmise | Removes sixteen repeated starter slots and differentiates the rooms |

### Add a native Game Corner starter archive

- Keep the initial nine-region selection exactly as it is.
- Add a region submenu to the Mauville prize counter and offer all 27 base starters once per save.
- Unlock the archive on first normal Game Corner access after Brawly; do not postpone Mega-capable starters until postgame.
- Price each at a no-grind amount, provisionally 500 coins. Test the total time to acquire one desired alternate starter; target under ten minutes without save-state abuse.
- Genesect and Poipole remain distinct high-value unique prizes.
- Each species needs its own claimed bit or a compact 27-bit save field. Do not reuse one trio-wide flag.
- The starter archive is also the clean fix for the currently choice-only Mega families: Venusaur, Blastoise, Meganium, Feraligatr, Sceptile, Blaziken, Swampert, Chesnaught, and Delphox.

## Historical numbered-route implementation blueprint

Every row below was a required review closure used to author the current
tables. Species under "Keep/build around" record the intended route identity;
the live JSON and release gate decide current truth.

| Route | Natural campaign state | Keep/build around | Required correction and safe replacements |
|---|---|---|---|
| 101 | Opening, cap 14 | Zigzagoon, Poochyena, Wurmple, Taillow | Remove Sprigatito. Replace the early Dreepy/Larvesta double trophy with Pidgey and one newer 1% surprise. Add Meowth in a 4-5% toolbox slot if Fake Out is not guaranteed on 102/116. |
| 102 | Opening farmland/pond, cap 14 | Lotad, Seedot, Ralts, Surskit, Marill | Reduce the modern psychic cluster. Keep Ralts reliable at 5% or better; add Pachirisu or Cleffa as reliable redirection and Meowth as Fake Out. Hatenna/Indeedee can move to Verdanturf Meadow. |
| 103 | Opening coast/rival, cap 14 | Wingull, Shellos, Shinx, Hoothoot, Mareep | Remove Fuecoco. Use Growlithe as the fire/Intimidate tool. Keep Shinx common enough that the advertised early toolbox is real. Move Toxel/Yamper toward Route 110. |
| 104 | Petalburg coast/woods, cap 14 | Taillow, Sentret, Azurill, Budew, Ledyba | Remove Sobble from Old Rod. Use Magikarp/Tentacool. Keep only one modern coastal trophy; move Mareanie/Wimpod into rarer coast slots. Put Scyther in the Woods, not roadside grass. |
| 105 | Dewford approach, cap 20; Surf later | Slowpoke, Exeggcute, Inkay, Krabby | Replace early fully evolved Malamar, Floatzel, and Exeggutor with their base forms. Differentiate this as the tidal/psychic shore rather than duplicating 106. |
| 106 | Granite coast, cap 20; Surf later | Makuhita, Machop, Crabrawler, Binacle, Wimpod | Keep the physical/rock toolbox here. Remove duplicate Exeggcute-heavy structure from 105/106. Reserve Dragalge/Clawitzer for rods or the Abandoned Ship. |
| 107 | Dewford channel; rod at cap 20, Surf later | Tentacool, Wingull, Remoraid, Chinchou, Mantyke | Current 107 and 108 are copies. Make 107 the bright channel and early special-water toolbox. |
| 108 | Abandoned Ship water; rod at cap 20, Surf later | Frillish, Skrelp, Wailmer, Dhelmise | Make 108 the haunted/wreck table. Dhelmise belongs in the 1% Super Rod slot, not on both routes. |
| 109 | Slateport beach, cap 30 | Krabby, Corsola, Staryu, Sandygast, Mareanie | Change Rock Smash rate 255 to 20. Rebuild the strongly Gen 6-8 table around Kanto/Johto/Hoenn beach nostalgia; keep Pincurchin or Clobbopus as the modern trophy. |
| 110 | Electric corridor, cap 30 | Electrike, Gulpin, Plusle, Minun, Magnemite, Pachirisu | Remove common Gimmighoul; it belongs in Mirage Tower. Keep Toxel/Helioptile as tools and Tadbulb/Morpeko as trophies. This route should guarantee redirection and Electric speed control before Wattson. |
| 111 | Desert and river, cap 40 | Sandshrew, Cacnea, Trapinch, Baltoy, Hippopotas, Gible | Remove Great Tusk from a 10% main-route slot and move it to Sandstrewn Ruins at 5%. Keep Gible/Sandile in toolbox slots and Stonjourner as a trophy. |
| 112 | Mountain ascent, cap 40 | Machop, Ponyta, Tyrogue, Numel, Sawk, Throh, Hawlucha | Remove repeatable Kubfu. Award one Kubfu after the Winstrate gauntlet or a native martial side quest. Replace its slot with Mienfoo or Primeape. |
| 113 | Ash route, cap 40 | Spinda, Skarmory, Slugma, Koffing | The existing Scraggy/Pawniard/Klefki toolbox is good. Keep Falinks or Mienfoo as the modern trophy; do not turn every ash slot into a steel/fighting set. |
| 114 | Meteor river, cap 40 | Swablu, Lombre/Nuzleaf, Zangoose/Seviper, Phanpy, Barboach | Current identity is mostly coherent. Preserve version-pair nostalgia. Put Beldum or Bagon only in a 1% Meteor Falls slot, not ordinary roadside grass. |
| 115 | Northern cliffs, cap 40-55 by section | Jigglypuff, Swellow, Tangela, Dodrio, Munchlax | Move Duraludon from a 10% common slot to 1%. Guarantee Meditite before its stone. Keep this route as high-cliff Normal/Flying/Steel preparation. |
| 116 | Rustboro foothills, cap 14 | Nincada, Whismur, Skitty, Taillow, Mareep, Makuhita | The current generation mean is too modern for a pre-Roxanne route. Keep Riolu/Eevee as 5% toolbox species and only one of Rookidee/Dreepy as a 1% trophy. |
| 117 | Day Care meadow, cap 30-40 | Roselia, Volbeat, Illumise, Meowth, Farfetch'd, Deerling | Remove Grookey/Scorbunny. Replace with Exeggcute/Ponyta. Tandemaus/Gossifleur can remain uncommon; Farfetch'd-Galar/Wooloo are trophies. |
| 118 | East-river transition, cap 55 | Linoone, Manectric, Lickitung, Raticate, Passimian, Carnivine, Zorua | Remove repeatable Type: Null and make it a one-time Devon/Weather Institute research reward after Norman. Keep Dondozo/Tatsugiri as this route's fishing signature. |
| 119 | Rainforest, cap 55 | Tropius, Goomy, Oranguru, Comfey, Amoonguss, Cramorant | Remove Raging Bolt from a 10% slot and make it a conditional one-time Sign after Raikou/Weather Institute progress. Preserve Feebas's native special mechanic; replace the table's 1% Milotic with another trophy so Milotic still means something. |
| 120 | Ancient forest, cap 55 | Absol, Venomoth, Tropius, Pumpkaboo, Mimikyu, Honedge | Remove 10% Ogerpon. Build a visible one-time mask quest here; this also fixes Ogerpon bypassing wild competitive presets. Keep Yanmega as the hidden chase. |
| 121 | Lilycove approach, cap 60 | Shuppet, Duskull, Hypno, Elgyem, Furfrou, Komala | Keep the ghost/urban transition. Put Sinistea and Zorua/Zoroark in 1-4% slots, not repeated common positions. |
| 122 | Mt. Pyre water, cap 60 | Tentacool, Frillish, Chinchou, Dhelmise | Increase identity and variety; it currently has only eight unique species. Make this haunted channel distinct from Route 121 and open-ocean tables. |
| 123 | Berry route, cap 60 | Gloom, Stantler, Karrablast, Shelmet, Tropius, Heracross | Keep the older-generation backbone but add later plant symbiosis such as Applin or Smoliv in toolbox/trophy slots. Do not merely repeat Karrablast/Shelmet/Gloom. |
| 124 | Open sea, cap 60 | Pelipper, Tentacool, Wailmer, Finneon, Frillish, Kingdra | Remove repeatable Manaphy from the 4% and 1% Super Rod slots and from extra Seafloor tables. Make Manaphy a one-time Egg or underwater Sign reward. Use Dhelmise/Kingdra as trophies. |
| 125 | Shoal sea, cap 60 | Seel, Spheal, Chinchou, Lapras | Remove repeatable Suicune. Make it a one-time Shoal quest linked to the local ice sanctuary. Keep Lapras rare but realistically findable. |
| 126 | Sootopolis basin, cap 70 | Luvdisc, Corsola, Relicanth, Milotic, Gorebyss, Huntail | Remove Primarina and repeatable Tapu Fini. Tapu Fini becomes a conditional one-time Sign after Mind Badge with a thematically appropriate partner. |
| 127 | Deep sea, cap 70 | Golisopod, Dragalge, Dhelmise, Toxapex | Remove repeatable Keldeo. Make Keldeo a one-time quest linked to Cobalion/Terrakion/Virizion progress. Keep Golisopod uncommon rather than 10% Surf saturation. |
| 128 | Seafloor approach, cap 70 | Kingdra, Dragalge, Sharpedo, Dhelmise | Remove Primarina from Super Rod. Use Dragalge/Dhelmise as late trophies and keep this as the strongest ordinary deep-sea table. |
| 129 | Optional late ocean, first reachable after Surf | Wailmer/Wailord, Wishiwashi, Dondozo, Relicanth | Stop copying Tentacruel/Wailord across 129-131. Make 129 the abyssal-giant route. |
| 130 | Mirage Island, optional after Surf | Wynaut on land | Preserve the canonical all-Wynaut land identity. Use the surrounding water for unusual gentle/dreamlike species; do not turn the island itself into a generic late table. |
| 131 | Pacifidlog reef, optional after Surf | Corsola, Chinchou, Mantine, Clamperl, Luvdisc | Make 131 the living-reef route. This is where Johto/Hoenn water nostalgia should intentionally return late. |
| 132 | Fast-current route, late optional | Sharpedo, Barraskewda, Floatzel, Finizen | Make 132 about speed and current riding rather than repeating Horsea/Wailord. |
| 133 | Deep-current route, late optional | Skrelp/Dragalge, Frillish/Jellicent, Dhelmise | Make 133 the eerie deep-current route. |
| 134 | Sealed Chamber approach, late optional | Relicanth, Wailord, Horsea/Kingdra, Clamperl | Preserve the ancient Hoenn identity and Sealed Chamber logic. A late older-generation route is a deliberate nostalgia callback, not a curve failure. |

## Historical restored-area implementation blueprint

All 22 maps in `gMapGroup_EmeraldChampionsExpansion` are covered here, including maps without a random table.

| Restored map | Decision |
|---|---|
| Altering Cave 1F | Keep the fully evolved cave oddities as postgame/high-badge variety. Hoopa remains visible. Avoid using Blissey in every hidden slot; use one healing trophy and two form oddities. |
| Altering Cave B1F | Keep Guzzlord at the area's current 5% Ultra Beast slot. Mewtwo remains a visible postgame puzzle. |
| Ashen Woods | Keep Pinsir, Heracross, Hisuian Growlithe, and Buzzwole at 5%. Convert Chi-Yu from a repeatable 10% slot to a one-time conditional Sign. |
| Cave of Origin Diancie's Room | The all-Carbink table is thematically correct. Diancie remains visible and one-time. |
| Dewford Manor 1F | Keep Gastly, Solosis, Litwick, Hoothoot, and Mime Jr. as a haunted-psychic room. Meloetta/Munkidori remain visible quest targets, not random slots. |
| Dewford Meadow | Keep the flower/pollinator identity and Pheromosa at its current 5% restored-sanctuary slot. |
| Ember Path | Keep Magcargo, Magmar, Blaze Tauros, Larvesta, and Blacephalon at 5%. Moltres remains visible. Replace excess Larvesta repetition with Houndour so Houndoominite is useful. |
| Meteor Falls Jirachi's Room | Correctly has no random table. Preserve Jirachi/Cosmog as visible scripted rewards. |
| Mirage Tower B1F | Keep Sandshrew, Trapinch, Bronzor, Golett, Sigilyph, Yamask, and rare Gimmighoul Chest. The form-resolution code already gives Chest Form a valid preset. |
| Petalburg Woods 2 | Recenter common slots on Caterpie/Weedle lines, Venipede, Paras/Exeggcute, and one modern bug. Move the elemental monkeys and Applin into uncommon slots. |
| Petalburg Woods 3 | Keep Oddish/Bellsprout/Yanma/Murkrow/Croagunk and Kartana at its current 5% deep-woods Ultra Beast slot. Virizion/Celebi/Wo-Chien remain one-time visible quests. |
| Route 111 Ruins Exterior | Keep Xatu, Helioptile, Rockruff, Girafarig, Hawlucha, and Meditite. Landorus remains visible and one-time. |
| Sandstrewn Ruins | Keep Unown at 4% as Hoopa's permanently reachable prerequisite; unlike Mirage Tower, this source survives the fossil collapse. Remove wild Aerodactyl. Fossil items are rewards here, so living fossil species undermine the reward loop. Keep Yamask, Bronzor, Golett, Honedge, Sigilyph, Spiritomb, Claydol, and Gabite. Move Great Tusk here at 5%, because it is an optional restored-area feature rather than a mandatory main-route wall. |
| Sandstrewn Ruins 2F | Remove wild Cranidos for the same fossil-economy reason. Orthworm is a good floor-specific trophy. |
| Sandstrewn Ruins 3F | Remove wild Shieldon. Tinkatink is a good floor-specific trophy. |
| Sandstrewn Ruins B1F | Keep Stakataka at the current 5% Ultra Beast slot. Zygarde remains a visible postgame quest. |
| Scorched Slab B1F | Keep Golbat/Gurdurr/Dugtrio/Boldore and rare Zweilous. Differentiate its water from generic Golbat repetition. |
| Scorched Slab B2F | Keep Magmar/Turtonator as the heat escalation. Reduce repeated identical slots. |
| Scorched Slab Heatran's Room | Keep a restrained volcanic table and Heatran as the one-time visible encounter. |
| Seaspray Cave | Keep Psyduck, Tynamo, Zubat, Wooper, Wishiwashi, Frillish, and Stunfisk forms. Move Nihilego's 5% ordinary Ultra Beast slot to the Seafloor Cavern Dive approach, where its visual theme has a stronger payoff. Remove wild Omanyte so the Helix Fossil retains value. |
| Seaspray Cave B1F | Keep the ice progression through Seel, Swinub, Snorunt, Sneasel, Spheal, and Snover. Remove wild Amaura so the Sail Fossil retains value. Frigibax is a targetable 5% deep-ice Mega base; Bergmite occupies the optional 1% surprise slot. |
| Verdanturf Meadow | Keep Munna, Espurr, Cottonee, Flabebe colors, Floette Eternal, and Milcery. This is the right later home for Hatenna/Indeedee removed from Route 102. |

## Other rare-area closures

- Petalburg Woods: keep Shroomish, Slakoth, Pichu, Scyther, Foongus, and a small number of bugs as the first serious toolbox. Scyther is already 5% and should remain catchable.
- Rusturf Tunnel: reduce pseudo-legend saturation. Use Whismur/Geodude/Dunsparce/Teddiursa as the common layer, Drilbur as a tool, Larvitar as a targetable 5% Mega base, and Bagon as the 1% surprise with commoner Meteor Falls sources. Move Larvesta to Ember Path.
- Granite Cave: preserve Zubat/Geodude/Makuhita/Aron/Abra/Carbink/Bronzor. Axew and Glimmet are sufficient modern trophies; do not repeat Axew through 10% of the table.
- Fiery Path and Jagged Pass: remove wild starter lines, guarantee Numel/Torkoal/Houndour, and keep one dragon line per subarea rather than Bagon/Deino/Jangmo-o simultaneously.
- Meteor Falls: give Beldum immediately after the Mossdeep Gym, or place it at 5%, so Tate and Liza's Metagrossite is usable without a target-catch grind. Keep Dratini/Bagon rare, not common fishing repetition.
- New Mauville: preserve Magnemite/Klink/Elekid/Porygon. Iron Hands is a fitting 5% Paradox feature, not a common slot.
- Safari Zone: this should be the strongest nonlegendary collection payoff, emphasizing rare Kanto-Hoenn families, regional forms, Kangaskhan, Heracross, Scyther/Pinsir, Chansey, Tauros, and evolution-item users. No mandatory team role may exist only under Safari flee rules.
- Shoal Cave Ice Room: convert Chien-Pao from repeatable 10% grass to a one-time Sign. Keep Snorunt, Spheal, Sneasel, Alolan Vulpix, Snom, and 5% Iron Bundle/Frigibax slots.
- Magma Hideout: convert Volcanion from repeatable 5% combined slots to a one-time machine-awakening quest. Replace Emboar with Magmar/Heatmor.
- Seafloor Cavern: eliminate sixteen repeated Greninja slots and extra Manaphy slots. Each room gets a distinct deep-water role pair; the underwater approach adds its own Relicanth/Nihilego/Iron Bundle trophy habitat and finite Relicanth-gated Manaphy Sign.
- Cave of Origin: Walking Wake can remain a very rare Paradox trophy, but not at 10%. Sableye/Mawile/Carbink/Noivern carry the ordinary identity.
- Sky Pillar: preserve Claydol/Banette/Dusclops/Altaria/Golurk. This is a valid late nostalgic table.
- Victory Road: keep late pseudo and fully evolved tools. Convert Terrakion from repeatable 4% grass to a one-time justice-trio closure.

## Mega-base and stone timing

### Current engine facts

- Steven gives the Mega Ring only after Brawly and letter delivery.
- The battle vendor opens the complete 92-stone Mega archive after badge eight, before the League.
- World and Gym rewards therefore provide early experimentation; the archive is a pre-League catch-up mechanism.

### Required availability corrections

- The Game Corner starter archive resolves all nine starter-choice-only Mega families.
- It also turns the current paper guarantee of 203 pre-League Champions families into a true single-save guarantee; without it, an actual player is missing 17-18 Champions families.
- Keep Frigibax in Seaspray Cave B1F at 5%; a new Mega base must be realistically targetable rather than merely available on paper.
- Darkrai is already available pre-League: after the Orb story flag, defeat all Mt. Pyre trainers and return with Musharna. Do not duplicate Darkrai in grass.
- Mewtwo and Zygarde are deliberately postgame visible quests. Their stones may appear in the complete archive, but they are documented postgame exceptions to the pre-League base-species contract.
- Give Beldum immediately after the Mossdeep Gym or add it at 5% in Meteor Falls. Do not leave Metagrossite dependent on Steven's postgame cave/gift or a 1% target-catch grind.
- Every world stone must pass a generated test: at the moment the stone can first be received, either its base family is already obtainable or the dialogue clearly identifies it as a future teaser no more than one chapter away.

## Fossil and evolution-item incentives

The Devon machine now restores eleven fossil species, and Sandstrewn Ruins distributes the fossil items. Random wild fossil species currently invalidate that loop.

Remove Omanyte from Seaspray Cave, Amaura from Seaspray B1F, Aerodactyl/Cranidos/Shieldon from Sandstrewn Ruins floors, and any other fossil family from ordinary campaign tables unless its corresponding fossil reward is removed. Fossils should remain finite exploration rewards; the badge-eight evolution archive does not include fossils and therefore does not replace this incentive.

Trade and held-item evolutions remain meaningful because the badge-eight archive is only a catch-up. Earlier Linking Cord, Metal Coat, Electirizer, Magmarizer, Protector, Reaper Cloth, Upgrade, Dubious Disc, Deep Sea Tooth/Scale, and similar finds should be checked against the first availability of their species family.

## Legendary and Ultra Beast policy

- Visible and conditional Legendary Signs are one-time and should remain the default for named legendary/mythical encounters.
- Ultra Beasts may be repeatable wild trophies only in restored or strongly bespoke areas. Their current 5% land slots are appropriate: rare enough to surprise, but not a grind for a player who deliberately explores the sanctuary.
- Convert Chien-Pao, Chi-Yu, Kubfu, Manaphy, Suicune, Tapu Fini, Terrakion, Ting-Lu, Type: Null, Volcanion, and Keldeo from repeatable JSON slots to one-time native quests/rewards.
- Great Tusk, Raging Bolt, Walking Wake, Iron Hands, Iron Bundle, and Iron Valiant may remain ordinary Paradox trophies, but they belong at 4-5%, hidden, or in restored areas rather than 10% main-route slots. Use 1% only when the species is a surprise rather than a targetable campaign tool.
- Ogerpon must be a visible one-time quest because it currently violates both uniqueness and battle-ready catch behavior.
- Add a release check that no `isSubLegendary`, `isRestrictedLegendary`, or `isMythical` species appears in a normal wild table unless its definition explicitly opts into `LEGENDARY_SOURCE_ORDINARY_WILD`.

## Completed implementation order

1. Replace the current distribution generator's design assertions before changing data. Remove the required-early Sprigatito/Fuecoco/Dreepy/Axew set, retain Scyther as the intentional early anchor, replace fixed global roster insertion with the route plan, and preserve 5% restored-area Ultra Beasts.
2. Add source verifiers before changing JSON: starter-family wild ban; special-species wild allowlist; fossil-family wild ban; every wild form resolves a preset; every Mega family has a timed single-save acquisition source; route method slot counts and rates are valid.
3. Fix hard defects only: Route 109 rate 255, Ogerpon ordinary slot, and ordinary named-legendaries that bypass uniqueness.
4. Add the Game Corner starter archive and its save flags. Prove all 27 prizes are exactly-once, battle-set compatible, and non-grindy.
5. Rebuild Routes 101-104, Petalburg Woods, Route 116, and Rusturf Tunnel as the opening ecosystem. Compile once and play through Roxanne before touching the rest.
6. Rebuild Dewford/Slateport through Wattson, including Routes 105-110, Granite Cave, the restored Dewford areas, and Seaspray Cave.
7. Rebuild the desert/volcano block through Norman, including the fossil incentive cleanup.
8. Rebuild the rainforest/Mt. Pyre block through Mossdeep, converting Ogerpon, Raging Bolt, Type: Null, and dream/justice quests.
9. Rebuild ocean routes and deep-sea areas through the League, giving 129-134 distinct identities.
10. Recompute every metric from source, but treat metrics as alarms. Manually close every route by asking whether it supplies useful ammunition, preserves place identity, and makes its rare slot desirable.
11. Run a fresh-save human playthrough. Static availability proves fairness and consistency, not encounter joy.

## Release acceptance gates

- Zero ordinary starter-family entries in campaign wild JSON.
- Zero non-allowlisted legendary/mythical entries in normal tables.
- Zero fossil-family entries in normal tables while their fossil item remains a reward.
- Every ordinary wild species/form resolves at least one legal competitive preset.
- Set-choice distribution is uniform for one, two, three, and four-set species in a deterministic RNG test.
- Every required doubles role is obtainable before its chapter boss at 5% or better through at least one non-Safari source.
- Every non-postgame Mega base and all 203 Champions families are obtainable in one save before the League, independent of initial starter choice. The verifier must use real Hoenn map IDs and one-save starter state.
- Every world Mega Stone has a source-family timing record.
- Route signs are regenerated directly from the final tables.
- The first three badges and the complete ocean act are manually played on a fresh save before release.
