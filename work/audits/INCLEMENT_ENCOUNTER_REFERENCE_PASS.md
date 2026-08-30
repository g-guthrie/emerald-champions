# Inclement Emerald Encounter Reference Pass

Status: reference audit only. Emerald Champions' compiled JSON, acquisition scripts, finite rewards, and verifier contracts remain authoritative.

Primary reference: [`jrci-dev/inclementemerald`](https://github.com/jrci-dev/inclementemerald), commit `93e96730fb3c771f7f08f9623ae36f0643c52d3d`, especially `src/data/wild_encounters.json`. A materialized archived Emerald Champions table generated from the older Inclement migration was used only as a secondary cross-check.

## Method

The source tables were compared map by map from Routes 101-134, then across Petalburg Woods, Granite Cave, the desert and volcano chain, Meteor Falls, Mt. Pyre, Safari Zone, Abandoned Ship, Shoal Cave, Cave of Origin, Sky Pillar, Seafloor Cavern, Victory Road, and restored side areas.

Nothing was copied mechanically. A placement was adopted only when it strengthened all of the following at once:

- Hoenn nostalgia or a clear biome identity;
- useful campaign availability without grinding;
- a reward or exploration incentive that still exists in Emerald Champions;
- the starter-archive, Devon-fossil, finite-Sign, competitive-preset, and Mega-timing contracts.

## Ideas retained or adopted

- Routes 101-104 retain Inclement's recognizable Hoenn backbone: Zigzagoon, Poochyena, Wurmple, Lotad, Seedot, Ralts, Surskit, Wingull, Shellos, Shinx, Taillow, Sentret, and Budew. Emerald Champions surrounds that backbone with explicit doubles roles and later-generation trophy slots.
- Routes 105-109 retain distinct tidal, granite-coast, channel, wreck-water, and beach-pool identities. Inclement's Inkay/Exeggcute/Crabrawler coast, Frillish wreck water, and Corsola/Mareanie/Sandygast beach logic informed the final split.
- Route 110 retains the Electrike/Gulpin/Plusle/Minun/Magnemite identity and Pachirisu support access.
- Routes 111-115 retain Inclement's desert, martial mountain, ash, Meteor river, and cliff identities: Sandshrew/Trapinch/Gible/Hippopotas; Numel/Ponyta/Tyrogue; Spinda/Skarmory; Swablu/Zangoose/Seviper; Tangela/Dodrio/Munchlax.
- Routes 118-123 retain the transition from Manectric riverland to rainforest, ancient forest, Lilycove ghosts, Mt. Pyre water, and berry-country bugs.
- Petalburg Woods preserves Shroomish, Slakoth, Scyther, and the evolving bug layers. Granite Cave preserves Zubat, Geodude, Makuhita, Aron, Mawile, Sableye, Onix, Carbink, and Bronzor.
- Underwater areas preserve Inclement's especially strong Clamperl/Chinchou/Relicanth/Gorebyss/Huntail/Kingdra/Golisopod idea. Emerald Champions separates coral, basin, and trench tables, makes Feebas a practical 5% Route 126 dive encounter, and keeps a second 1% trophy slot at the Seafloor Cavern approach instead of using wild Milotic.
- Abandoned Ship preserves the Frillish/Skrelp/Dragalge/Dhelmise/Jellicent theme while differentiating the flooded rooms from the hidden corridors.
- Shoal Cave preserves Spheal, Seel, Snorunt, Sneasel, Lapras, Cryogonal, and Jynx-family ice logic. Delibird was specifically restored to the entrance room at 4% after this reference pass because it is a clean nostalgic biome fit and does not disturb a progression reward.
- Cave of Origin preserves Sableye, Mawile, Boldore, Carbink, and Noivern as its mineral/dragon spine.
- Sky Pillar preserves Sableye, Mawile, Banette, Dusclops, Claydol, Golurk, and Altaria, then escalates toward late dragons and steels.
- Victory Road preserves Lairon, Medicham, Noivern, Golbat, Pupitar, Gabite, Steelix, Donphan, and Rhydon-family pressure while adding modern League-ready trophies.
- Route 130 remains an all-Wynaut land table. Route 134 deliberately returns to Relicanth, Wailord, Horsea, Seadra, and Kingdra as a Sealed Chamber nostalgia callback.

## Ideas deliberately rejected

- Inclement's cloned open-sea tables across Routes 124-127, 129-131, and 132-134 were not retained. Emerald Champions gives each route a separate identity: luminous open sea, Shoal water, Sootopolis basin, deep reef, seafloor approach, abyssal giants, Mirage water, living reef, fast current, ghost current, and Sealed Chamber approach.
- Inclement's cloned Seafloor Cavern rooms were rejected. Emerald Champions keeps recurring cave bats as Hoenn texture, but each room changes the surrounding toxic-reef, current, ruin, predator, and deep-water composition; the Dive approach now has a separate trophy habitat.
- Inclement's nearly identical Shoal rooms, Sky Pillar floors, Mt. Pyre floors, Mirage Tower floors, and Magma Hideout floors were rejected in favor of visible progression between floors.
- Wild Milotic was rejected even though Inclement used it on Route 119. It erased the Feebas incentive; Emerald Champions instead keeps the native Feebas path, a 5% underwater Route 126 source, and a 1% Seafloor approach trophy slot.
- Wild fossil families were rejected. Inclement used species such as Omanyte, Kabuto, Aerodactyl, Cranidos, Shieldon, Tyrunt, and Amaura in thematic places, but Emerald Champions uses their fossils as finite exploration rewards and Devon as the repeatable revival service.
- Regional starters remain excluded from ordinary wild tables. The Mauville Game Corner archive is the explicit, one-time acquisition system.
- Repeatable named legends were rejected even when their biome placement was attractive. Chien-Pao, Manaphy, Suicune, Tapu Fini, Terrakion, Volcanion, and Keldeo are finite conditional Signs; restored-area Ultra Beasts remain the only ordinary legendary-class wild trophies.
- Mandatory competitive roles were not placed only in Safari Zone or at 1%. Safari remains a collection reward with flee rules, not a fairness gate.
- Inclement's exact generation mix was not treated as a target. Emerald Champions uses an explicit easing curve and intentionally mixes modern trophies with older common layers.

## Source changes made because of this pass

One additional source change was justified: Delibird replaced Snom in Shoal Cave's entrance-room 4% slot. Snom remains available deeper in Shoal Cave, while Delibird restores a memorable Inclement/ice-cave placement with no cost to role coverage, Mega access, or finite rewards.

All other final-band choices already incorporated the strongest Inclement ideas before this comparison and were retained after review. No source was changed merely to match Inclement.
