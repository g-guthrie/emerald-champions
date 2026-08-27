# Emerald Champions curated Gen 9 availability

Emerald Champions keeps Mega Evolution as its only battle gimmick. The curated
backport contains 25 competitive endpoints,
the pre-evolutions and alternate forms needed to use them normally, and four
Expansion-supplied Mega forms: 45 numeric species entries in total. Tera forms
are intentionally excluded.

Every family is available through a native wild encounter or evolution. Core
sources use encounter slots of at least 10 percent; Finizen is a 40-percent Old
Rod encounter and the three Tatsugiri forms share a 60/20/20 Good Rod pool.
Canonical catch rates below 45 are raised to 45, so the legendary and Paradox
showcases do not turn access into a ball-throwing grind. The same minimum now
applies to every legacy legendary that remains in an ordinary random wild
table; Darkrai and Marshadow are withheld from ordinary encounters for their
League showcases.

| Progression area | New source | Endpoint or purpose |
|---|---|---|
| Route 101 | Sprigatito | Meowscarada family |
| Route 102 | Nacli | Garganacl family |
| Route 103 | Fuecoco | Skeledirge family |
| Route 104, Old Rod | Finizen | Palafin and Hero Form |
| Rustboro City | Gimmighoul (Chest) | Gholdengo family |
| Granite Cave 1F | Glimmet | Glimmora and Mega Glimmora |
| Route 110 | Gimmighoul (Roaming) | Alternate Gimmighoul form |
| Route 111 desert | Great Tusk | Great Tusk |
| Route 113 / Jagged Pass / Route 111 Ruins | Pawniard / Primeape / Girafarig | Kingambit / Annihilape / Farigiraf |
| Route 115 | Duraludon | Archaludon |
| Route 118, Surf | Dondozo | Dondozo |
| Route 118, Good Rod | All three Tatsugiri forms | Tatsugiri and its three Mega forms |
| New Mauville | Iron Hands | Iron Hands |
| Route 119 | Raging Bolt | Raging Bolt |
| Route 120 | Ogerpon | All four non-Tera mask forms |
| Mt. Pyre Summit | Flutter Mane | Flutter Mane |
| Cave of Origin 1F | Walking Wake | Walking Wake |
| Magma Hideout 1F | Gouging Fire | Gouging Fire |
| Shoal Cave Ice Room | Iron Bundle and Chien-Pao | Two ice specialists |
| Desert Underpass | Ting-Lu | Ting-Lu |
| Ashen Woods | Chi-Yu | Chi-Yu |
| Meteor Falls B1F, Waterfall room | Roaring Moon | Roaring Moon before the League |
| Victory Road 1F | Iron Valiant | Iron Valiant |

## Consolidated showcase pools

The two 4-percent tail slots are weighting tools, not places to hide separate
headline species. Route 104 now offers Mareanie and Wimpod at 12 percent each;
Route 110 offers Heliolisk and Toxel at 5 percent each beside its common
Pachirisu and Stunky; and
Route 121 offers Zoroark and Sinistea at 12 percent each. Rotom is reserved for
its New Mauville generator encounter, Spiritomb for its Odd Keystone encounter,
Porygon remains a Mauville Game Corner prize, and Klefki remains on Route 113.

The late-ocean identities are likewise direct: Route 124 has 15-percent
Manaphy, Route 125 has 15-percent Suicune, Route 126 has 15-percent Tapu Fini,
Route 127 has 15-percent Keldeo, and Route 128 has 15-percent Primarina. Their
paired 5-percent Surf slots form one 10-percent source per route rather than
two separate hunts.

Magma Hideout consolidates Volcarona and Magmortar at 12 percent through the
approach rooms, then reserves 12-percent Volcanion and Emboar for 4F. Heatran
remains exclusive to its Scorched Slab encounter. Seafloor
Cavern removes repeated Tapu Fini, Suicune, and Keldeo rolls: its paired tail
slots make Greninja and Crobat 12-percent land encounters, Lapras a 10-percent
Surf encounter, and Manaphy a 15-percent Super Rod encounter.

## Native exploration rewards

Rare Candy remains sold by the medicine Poké Marts for $1,000, so nine
redundant Rare Candy balls now carry the progression items instead:

| Location | Item | Use |
|---|---|---|
| Route 111 | Cornerstone Mask | Rock Ogerpon form and Rock move boost |
| Route 114 | Leader's Crest | Evolves Bisharp |
| Granite Cave B2F | Gimmighoul Coin | Evolves either Gimmighoul form |
| Route 119, poison-move ball | Glimmoranite | Mega Evolves Glimmora once its level-35 evolution is legal |
| Route 119 | Metal Alloy | Evolves Duraludon |
| Route 127 | Wellspring Mask | Water Ogerpon form and Water move boost |
| Route 132 | Tatsugirinite | Mega Evolves any Tatsugiri form |
| Magma Hideout 1F | Booster Energy | Sustains a Paradox Pokémon's highest-stat boost |
| Magma Hideout 3F | Hearthflame Mask | Fire Ogerpon form and Fire move boost |

The Gimmighoul Coin, Leader's Crest, and Metal Alloy are reusable after discovery.
They are progression unlocks, not one-use constraints on how many legal teams
the player may build, and they cannot be sold or discarded. Booster Energy and
the three masks join the native unlimited battle-item shop after their first
discovery.

Existing saves are migrated on load: if one of these nine item-ball flags was
already set when the ball still contained a Rare Candy, the corresponding new
item is granted once. A full bag defers the grant until a later load rather than
discarding it.

`scripts/verdant_gen9_curated.py --check` verifies all assets, constants, wild
sources, encounter rates, and world rewards. The same script can idempotently
restore the placement table with `--apply-availability`.
