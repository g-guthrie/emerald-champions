# Regional encounters and Mega Stone rewards

Mega Stones come from exploration, NPC gifts and three one-time berry trades. The Mega Ring still arrives after Brawly and enables Mega Evolution; it no longer unlocks a free Mega Stone shop. The evolution-item archive remains free with the Ring. Free battle supplies and Poké Ball prices remain as before.

## Opening and regional identity

- Route 104: Mienfoo replaces a duplicate Sentret slot at 11%. Sentret remains on Route 101. Mienfoo offers a capable unevolved partner near Petalburg's training grounds.
- Petalburg Woods: Caterpie appears at 10% and Ferroseed at 5%, using two former cocoon slots. Wurmple stays at 11%; its level-10 Beautifly/Dustox evolutions remain accessible. Caterpie gives another level-10 evolution. Shroomish, Slakoth, Buneary, Foongus and Blipbug remain useful woodland choices.
- The forest's Ether and Paralyze Heal pickups become Butterfrenite and Lopunnite, matching families in the forest. Dedicated flags let the new rewards appear even if the old supplies were collected.
- Magnemite remains associated with New Mauville; the Duskull line remains a Mt. Pyre discovery rather than an opening-route addition. Existing evolved-family encounters elsewhere were not removed.
- Granite Cave retains Abra/Timburr and its Hoenn cave anchors; Verdanturf Meadow retains Cottonee; Murkrow remains in the deeper forest and Mt. Pyre. No baby was removed from the wild in this pass.
- Coastal, volcanic, forest, technological, haunted and underwater rewards follow their corresponding families. Later discoveries still include New Mauville's technological species, the desert's Ground/Dragon options, Mt. Pyre's ghosts, the Safari Zone's distinctive roster, and the Dive rosters.
- Dondozo is exclusive to underwater encounters; Tatsugiri and its Mega Stone share the submerged submarine chamber. The two ordinary encounter slots are 10%. Feebas remains the only permitted sub-5% rarity exception.

## Rewards and timing

There are 99 native stone types with world reward routes, including 73 distinct physical stone pickups. Meaningful existing NPC gifts, such as Roxanne's Old Amber/Aerodactylite pair and Steven's chosen-starter stone, remain. Seventeen additional NPC gifts make houses and conversations worth visiting.

Seven existing physical stones were relocated nearer their families: Lopunnite, Metagrossite, Banettite, Chandelurite, Abomasite, Froslassite and Glalitite. Their original collection flags follow them, preserving ownership across the move. The vacated locations contain five-berry bundles with separate flags. No two physical pickups award the same stone.

The three new Seaspray stones stand on dry, nonblocking floor reachable without crossing the ice. They do not introduce new stopping points on the sliding puzzle. No map layouts or scripted movement paths were rewritten.

The source graph exposed an unattached Diancite gift. A sparkle in Diancie's actual room supplies the missing reward. This pass does not restore the unattached battle.

## Berry Master

Baxcalibrite, Dragoninite and Tyranitarite cost 20 garden berries each, once per stone. Their final evolutions become useful around the Berry Master's part of the journey. Acceptable currency is any mixture of Razz, Bluk, Nanab, Wepear, Pinap, Pomeg, Kelpsy, Qualot, Hondew, Grepa and Tamato. These are not supplied by free held-item menus or competitive presets.

The current initial berry script and live tree objects provide 38 eligible trees with a combined minimum harvest of 94 berries, before the seven five-berry bundles and renewable growth. Collecting across the adventure can cover all three trades; planting can accelerate progress. No planting wait is required to advance the main story.

Each purchase requires confirmation. The native trade function delivers the stone before charging; insufficient berries, a full reward pocket, an invalid choice or a completed trade cannot deduct payment. Daily berry gifts remain available separately.

## Early babies and the Daycare

The September 8 revision restores baby Pokémon to the low-cap opening. Happiny appears on Route102 (7%); Munchlax (7%) and Mime Jr. (12%) appear on Route116; Chingling returns to Rusturf Tunnel (5%). Mime Jr. also replaces its adult in Dewford Manor. Dewford's Old Rod now offers Mantyke alongside Magikarp; Mantine remains a later coastal Surf discovery. This early fishing path is deliberate: merely restoring Mantyke to Surf would miss its early-game role.

Displaced families remain available: Meowth replaces the recently added Snorlax on Route117; Skwovet moves to Route115's Wooloo slot, while Wooloo remains on Route117. Whismur remains in Rusturf Tunnel, Fomantis on Route115, and Ponyta on Route112. Snorlax is obtained by evolving early Munchlax. Mr. Mime remains in the Safari Zone; Chimecho remains at Mt. Pyre. Pichu, Bonsly, Azurill, Budew, Togepi, Riolu, Mienfoo, Ferroseed and the early bugs retain their earlier placements. Tyrogue remains catchable on Route112; there is no Hitmon breeding restriction.

The Togepi Egg NPC now awards **Kangaskhanite for one egg produced by two deposited compatible parents and then hatched**. The NPC explains Kangaskhan's Parental Bond, the breeding requirement, and the later Safari Zone encounter. The gifted Togepi Egg is still available but explicitly does not qualify. A bred Togepi egg can qualify: origin, not species, determines eligibility. Ditto remains on Route117 at 7% to make compatible pairing straightforward.

Only the actual Daycare egg creation path writes the special origin marker to an egg's existing met-location field. The actual hatch callback checks that marker before replacing it with the true hatch location and setting the persistent completion flag. Gift eggs never receive the marker. The field already survives party/PC storage and saving, so no Pokémon or save-layout change is needed. The previous three-species tracker and its special were retired. Earlier any-hatch progress cannot prove breeding and does not unlock this reward; eggs collected before provenance tracking need to be replaced by a newly bred egg for this quest.

The reward accepts either historical Kangaskhanite collection flag as already claimed, preventing duplicates for existing owners. Its old Route120 pickup is five Wepear Berries under a new flag, preserving object IDs. The Winstrate gift is now five Razz Berries, so neither former source bypasses the nursery errand. Audinite is an ordinary gift from the woman among Route117's flowers, retaining its prior ownership flag. Mawilite and the other Mega Stone routes are unchanged.

Egg generation still checks compatible parents every 64 steps (first check at 63); compatibility and Oval Charm retain their effect. New and already-held eggs use at most five cycles: a lone egg takes up to 768 steps, or 512 with an incubator ability, depending on cycle alignment. Multiple ready eggs hatch sequentially. Free stat/move editing and Phione's research requirement remain unchanged.


## Native evolution access — September 8

| Pokémon | Hoenn method |
| --- | --- |
| Meltan → Melmetal | Level up in New Mauville, with the generator on or off. |
| Bisharp → Kingambit | Level up holding a Leader's Crest; evolution consumes the crest. The free Evolution Items archive supplies it. Pawniard remains on Route113 and evolves at level 52. |
| Galarian Yamask → Runerigus | Interact with the carved stone at Route111 (16,56), east of the hiker near Mirage Tower. Confirm the inscription with a conscious Galarian Yamask missing at least 49 HP in the party, without an Everstone. Every eligible party member can evolve. The stone remains after the tower collapses and can be used again. |
| Koffing → Galarian Weezing | Level up at level 35 or above inside Fiery Path. |
| Quilava → Hisuian Typhlosion | Level up at level 36 or above at Mt. Pyre. |
| Dewott → Hisuian Samurott | Level up at level 36 or above anywhere in Shoal Cave. |
| Dartrix → Hisuian Decidueye | Level up at level 34 or above in the main Petalburg Woods map. Both Decidueye branches use level 34, avoiding an accidental ordinary evolution before the regional branch becomes eligible. |
| Petilil → Hisuian Lilligant | Use a Sun Stone in Verdanturf Meadow. |
| Goomy → Hisuian Sliggoo | Level up at level 40 or above in Meteor Falls' main entrance chamber. Hisuian Sliggoo retains its level-50 rain-or-fog evolution into Hisuian Goodra. |
| Bergmite → Hisuian Avalugg | Level up at level 37 or above in Shoal Cave's low-tide ice room. |
| Rufflet → Hisuian Braviary | Level up at level 54 or above at Mt. Pyre. |
| Ursaring → Ursaluna | Use a Peat Block at night; no Hisui region requirement. |

For the location-specific regional alternatives, the ordinary form remains available by evolving elsewhere. Existing encounters supply Alolan Raichu (Safari South), Exeggutor (Safari North/Northwest), Marowak (Safari North), and Galarian Mr. Mime (Shoal's ice room). Local NPCs explain these encounters and the changed methods. The engine's existing first-matching evolution rule chooses the regional entries before their ordinary alternatives; no new evolution conditions or resolver were added.

Burmy appears in Petalburg Woods at 6% in ordinary encounters, replacing Impidimp's slot; Impidimp remains in Petalburg Woods 3. Female Burmy evolves into Wormadam at level 20 and male Burmy into Mothim. Existing post-battle cloak changes provide its other forms. Deerling appears in Verdanturf Meadow at 10%, replacing Ribombee's slot; Ribombee remains in Dewford Meadow. Deerling evolves into Sawsbuck at level 34. Sweet Scent's reversed tables still meet the 5% minimum.
