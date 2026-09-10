# NPC-A dialogue review — September 9, 2026

Read **all 1,382 supplied dialogue blocks in 117 assigned files**, including inactive text. Revised **77 blocks in 27 files**. The original 117-file dialogue assignment changed only `.string` lines. The subsequently authorized extension restored three missing NPC actors, added five form tools to the existing free archive, and corrected three C guide strings, as documented below. No teams, battle mechanics, new state IDs or new gates were introduced.

## Main changes

- Wally's Victory Road scenes follow his growing confidence, his uncle's concern, and his friendship with the player. Removed the false claim that Togekiss pulls every attack, announced tactical scripts, and descriptions of actions the player might never take.
- May and Brendan retain friendly rivalry, recognize the journey since Route 103, and look ahead to Briney's voyage. Declining a battle no longer earns an unnecessary rebuke.
- Magma guards talk about excavation, posts, heat, the Blue Orb and uncertainty about Maxie. Aqua guards talk about waiting, warp panels and the submarine. Shelly remembers the Institute and why she joined Aqua. Removed abstract formulations about currents, formations, freedom and choosing balance.
- Route 123 and Victory Road now describe rare wild Tapu Bulu and Victini, with the correct six/eight-badge gates and Sweet Scent clue. Their removed encounter stones and Lucario condition are no longer advertised.
- Leaf no longer assumes the player is Champion, says Mewtwo is in this cavern, and explains Mewtwonite X. Wallace's separate Champion-only exhibition remains intact and explicitly permits exploring for Diancie/Diancite without winning it.
- Route 117 and Altering Cave use local sighting language. Dewford's warden describes Cosmog, the Sing performance, permanent Meloetta access, Heat Badge Munkidori and Sweet Scent without reciting encounter percentages already available through the dynamic local guide.
- Sweet Apple and Moon Stone gifts explain the actual item. Furfrou's stylist talks about grooming. The Rock Smash giver explains the badge, compatible partner and unused battle slot. Catching advice avoids encouraging an avoidable poison/burn knockout.
- Live Ranking Hall dialogue now discusses personal Circuit records instead of a nonexistent list of Trainer names. The Circuit locked message matches the Feather Badge. The Pike visitor recognizes all desks share the Circuit. Free Frontier tutors regain their individual rivalry and former Beauty/Swimmer identities.
- The manor sign now agrees with its sea-captain/research history. Seaspray's explorer attributes exposure to shifting tides without assuming a particular Aqua scene has already happened.

## Source evidence and deliberate retention

All supplied blocks were read in order in batches; an initially truncated overlap was reread before marking completion. Every assigned file's service/reward/special-call inventory was inspected. Relevant branches were then traced in the original files, including rivals, the two Vial upgrades, Cozmo's exchange, friendship-based weekly stones, trading, gifts, field moves, local discovery special calls, Wally, Leaf, Wallace and Circuit entry/boards. This is targeted source review, not a claim that every instruction in all 117 scripts was manually traced.

The rare-wild definitions in `src/data/pokemon/legendary_signs.h` support the corrected habitats/gates. The Goomy, Dewott, Bergmite and Ursaring evolution definitions support the retained regional-evolution clues. `include/pokemon_storage_system.h` still specifies 14 boxes of 30, so Lanette's 420-capacity research note was retained. The final Trick House puzzle is still Champion-gated, so its Champion remark was retained. The Surf-deletion guard calls the actual current field-user check; it was not mistaken for a requirement to teach Surf in the ordinary licensed-field-move flow.

Many Frontier rules, Brain monologues, old three/four-Pokémon registration messages, and BP-tutor strings remain in disconnected legacy branches. Entry desks/boards route to the Champions Circuit, the Lounge2 maniac gives current Circuit news, and Lounge7 tutors route directly to the free specialist. Verdanturf's corridor explicitly labels its four Contest strings unused. Those blocks were read and retained as inactive source; no claim is made that the retired facilities were exercised. Local jokes, town history, Contest personalities, family banter and accurate services were retained without a rewrite quota.

## Integration finding sent to main reviewer

The Meadow warden, elevated-ruins archaeologist, and manor historian scripts had no actor/caller references found in the inspected source tree. In particular, Route111_RuinsExterior/map.json contained only the Steelixite pickup. Main reviewer then explicitly authorized this reviewer to restore these actors. The source wiring repair is complete; placements and evidence are documented below. The original discovery was a real callability defect, not merely obsolete prose.

## Verification limits

Performed a one-off lexical comparison of the 117 owned files with the saved before snapshot: after stripping `.string` lines, every file matched. All 77 edited labels were found and their text retained a `$` terminator. No build, ROM render, emulator, battle, traversal or runtime test was run. No tests were added. Line breaks were authored conservatively at 34 characters; actual font rendering remains for integrated QA.

## Per-file coverage

Each row counts the supplied assigned blocks; this does not claim ownership of shared external text referenced from that file. Exact reviewed and edited labels are in `npcs_a-reviewed.json`, and replacement prose is in `npcs_a-edits.json`.

| File | Read blocks | Edited blocks |
|---|---:|---:|
| `data/maps/PacifidlogTown/scripts.inc` | 4 | 0 |
| `data/maps/Route104/scripts.inc` | 32 | 7 |
| `data/maps/Route111/scripts.inc` | 44 | 2 |
| `data/maps/Route117/scripts.inc` | 8 | 2 |
| `data/maps/Route118/scripts.inc` | 15 | 3 |
| `data/maps/Route121/scripts.inc` | 9 | 1 |
| `data/maps/Route123/scripts.inc` | 8 | 2 |
| `data/maps/Route124/scripts.inc` | 4 | 0 |
| `data/maps/Route125/scripts.inc` | 2 | 0 |
| `data/maps/Route127/scripts.inc` | 2 | 0 |
| `data/maps/Route133/scripts.inc` | 5 | 0 |
| `data/maps/Route134/scripts.inc` | 1 | 0 |
| `data/maps/FallarborTown_Mart/scripts.inc` | 4 | 0 |
| `data/maps/FallarborTown_BattleTentLobby/scripts.inc` | 7 | 0 |
| `data/maps/FallarborTown_CozmosHouse/scripts.inc` | 11 | 0 |
| `data/maps/VerdanturfTown_BattleTentCorridor/scripts.inc` | 4 | 0 |
| `data/maps/VerdanturfTown_Mart/scripts.inc` | 4 | 0 |
| `data/maps/VerdanturfTown_PokemonCenter_1F/scripts.inc` | 2 | 0 |
| `data/maps/VerdanturfTown_FriendshipRatersHouse/scripts.inc` | 15 | 0 |
| `data/maps/VerdanturfTown_House/scripts.inc` | 3 | 1 |
| `data/maps/PacifidlogTown_PokemonCenter_1F/scripts.inc` | 3 | 0 |
| `data/maps/PacifidlogTown_House2/scripts.inc` | 11 | 0 |
| `data/maps/PacifidlogTown_House4/scripts.inc` | 5 | 0 |
| `data/maps/SlateportCity_BattleTentLobby/scripts.inc` | 7 | 0 |
| `data/maps/SlateportCity_NameRatersHouse/scripts.inc` | 10 | 0 |
| `data/maps/SlateportCity_PokemonFanClub/scripts.inc` | 27 | 3 |
| `data/maps/MauvilleCity_House1/scripts.inc` | 3 | 1 |
| `data/maps/MauvilleCity_GameCorner/scripts.inc` | 37 | 0 |
| `data/maps/LilycoveCity_CoveLilyMotel_2F/scripts.inc` | 13 | 1 |
| `data/maps/LilycoveCity_LilycoveMuseum_2F/scripts.inc` | 19 | 0 |
| `data/maps/LilycoveCity_ContestHall/scripts.inc` | 36 | 1 |
| `data/maps/LilycoveCity_MoveDeletersHouse/scripts.inc` | 9 | 0 |
| `data/maps/LilycoveCity_House1/scripts.inc` | 2 | 0 |
| `data/maps/LilycoveCity_House2/scripts.inc` | 2 | 2 |
| `data/maps/LilycoveCity_House3/scripts.inc` | 8 | 0 |
| `data/maps/LilycoveCity_DepartmentStore_1F/scripts.inc` | 8 | 0 |
| `data/maps/LilycoveCity_DepartmentStore_5F/scripts.inc` | 4 | 0 |
| `data/maps/LilycoveCity_DepartmentStoreRooftop/scripts.inc` | 8 | 0 |
| `data/maps/MossdeepCity_House2/scripts.inc` | 3 | 0 |
| `data/maps/MossdeepCity_PokemonCenter_1F/scripts.inc` | 2 | 0 |
| `data/maps/MossdeepCity_PokemonCenter_2F/scripts.inc` | 1 | 0 |
| `data/maps/MossdeepCity_Mart/scripts.inc` | 3 | 0 |
| `data/maps/MossdeepCity_StevensHouse/scripts.inc` | 9 | 0 |
| `data/maps/Route104_PrettyPetalFlowerShop/scripts.inc` | 1 | 0 |
| `data/maps/Route111_WinstrateFamilysHouse/scripts.inc` | 6 | 0 |
| `data/maps/Route112_CableCarStation/scripts.inc` | 3 | 0 |
| `data/maps/MtChimney_CableCarStation/scripts.inc` | 3 | 0 |
| `data/maps/Route114_FossilManiacsHouse/scripts.inc` | 4 | 0 |
| `data/maps/Route114_LanettesHouse/scripts.inc` | 7 | 0 |
| `data/maps/Route121_SafariZoneEntrance/scripts.inc` | 1 | 0 |
| `data/maps/MeteorFalls_1F_2R/scripts.inc` | 25 | 0 |
| `data/maps/GraniteCave_1F/scripts.inc` | 2 | 0 |
| `data/maps/JaggedPass/scripts.inc` | 28 | 0 |
| `data/maps/MtPyre_3F/scripts.inc` | 13 | 0 |
| `data/maps/AquaHideout_1F/scripts.inc` | 9 | 0 |
| `data/maps/AquaHideout_B2F/scripts.inc` | 13 | 8 |
| `data/maps/SeafloorCavern_Room3/scripts.inc` | 6 | 6 |
| `data/maps/VictoryRoad_1F/scripts.inc` | 21 | 7 |
| `data/maps/ShoalCave_LowTideEntranceRoom/scripts.inc` | 1 | 0 |
| `data/maps/ShoalCave_LowTideLowerRoom/scripts.inc` | 2 | 0 |
| `data/maps/NewMauville_Entrance/scripts.inc` | 3 | 0 |
| `data/maps/NewMauville_Inside/scripts.inc` | 11 | 0 |
| `data/maps/AbandonedShip_Rooms_B1F/scripts.inc` | 1 | 0 |
| `data/maps/AbandonedShip_Rooms2_B1F/scripts.inc` | 1 | 0 |
| `data/maps/AbandonedShip_Room_B1F/scripts.inc` | 2 | 0 |
| `data/maps/AbandonedShip_CaptainsOffice/scripts.inc` | 3 | 0 |
| `data/maps/AbandonedShip_HiddenFloorCorridors/scripts.inc` | 5 | 0 |
| `data/maps/AbandonedShip_HiddenFloorRooms/scripts.inc` | 1 | 0 |
| `data/maps/SealedChamber_InnerRoom/scripts.inc` | 2 | 0 |
| `data/maps/ShoalCave_LowTideIceRoom/scripts.inc` | 3 | 0 |
| `data/maps/MagmaHideout_1F/scripts.inc` | 6 | 6 |
| `data/maps/MagmaHideout_2F_2R/scripts.inc` | 12 | 4 |
| `data/maps/MagmaHideout_3F_2R/scripts.inc` | 3 | 0 |
| `data/maps/DesertUnderpass/scripts.inc` | 2 | 0 |
| `data/maps/MeteorFalls_StevensCave/scripts.inc` | 3 | 0 |
| `data/maps/SSTidalLowerDeck/scripts.inc` | 6 | 0 |
| `data/maps/SSTidalRooms/scripts.inc` | 26 | 0 |
| `data/maps/BattleFrontier_BattleTowerLobby/scripts.inc` | 90 | 1 |
| `data/maps/BattleFrontier_BattleTowerMultiBattleRoom/scripts.inc` | 14 | 0 |
| `data/maps/BattleFrontier_BattleDomeLobby/scripts.inc` | 42 | 0 |
| `data/maps/BattleFrontier_BattleDomePreBattleRoom/scripts.inc` | 26 | 0 |
| `data/maps/BattleFrontier_BattleDomeBattleRoom/scripts.inc` | 46 | 0 |
| `data/maps/BattleFrontier_BattlePalaceLobby/scripts.inc` | 37 | 0 |
| `data/maps/BattleFrontier_BattlePalaceCorridor/scripts.inc` | 5 | 0 |
| `data/maps/BattleFrontier_BattlePalaceBattleRoom/scripts.inc` | 26 | 0 |
| `data/maps/BattleFrontier_BattlePyramidLobby/scripts.inc` | 66 | 0 |
| `data/maps/BattleFrontier_BattlePyramidTop/scripts.inc` | 16 | 0 |
| `data/maps/BattleFrontier_BattleArenaLobby/scripts.inc` | 33 | 0 |
| `data/maps/BattleFrontier_BattleFactoryPreBattleRoom/scripts.inc` | 49 | 0 |
| `data/maps/BattleFrontier_BattleFactoryBattleRoom/scripts.inc` | 11 | 0 |
| `data/maps/BattleFrontier_BattlePikeLobby/scripts.inc` | 28 | 1 |
| `data/maps/BattleFrontier_BattlePikeRoomFinal/scripts.inc` | 1 | 0 |
| `data/maps/BattleFrontier_RankingHall/scripts.inc` | 7 | 3 |
| `data/maps/BattleFrontier_Lounge1/scripts.inc` | 4 | 1 |
| `data/maps/BattleFrontier_Lounge2/scripts.inc` | 36 | 0 |
| `data/maps/BattleFrontier_Lounge4/scripts.inc` | 3 | 0 |
| `data/maps/BattleFrontier_ScottsHouse/scripts.inc` | 17 | 0 |
| `data/maps/BattleFrontier_Lounge7/scripts.inc` | 34 | 2 |
| `data/maps/BattleFrontier_Lounge8/scripts.inc` | 3 | 0 |
| `data/maps/BattleFrontier_PokemonCenter_1F/scripts.inc` | 4 | 0 |
| `data/maps/BattleFrontier_Mart/scripts.inc` | 3 | 0 |
| `data/maps/FarawayIsland_Interior/scripts.inc` | 1 | 0 |
| `data/maps/TrainerHill_Entrance/scripts.inc` | 27 | 0 |
| `data/maps/Route110_TrickHouseEnd/scripts.inc` | 18 | 0 |
| `data/maps/Route110_TrickHousePuzzle4/scripts.inc` | 10 | 0 |
| `data/maps/Route110_TrickHousePuzzle6/scripts.inc` | 10 | 0 |
| `data/maps/Route110_TrickHousePuzzle7/scripts.inc` | 19 | 0 |
| `data/maps/Route110_TrickHousePuzzle8/scripts.inc` | 10 | 0 |
| `data/maps/Route113_GlassWorkshop/scripts.inc` | 15 | 0 |
| `data/maps/AlteringCave_1F/scripts.inc` | 1 | 1 |
| `data/maps/AlteringCave_B1F/scripts.inc` | 7 | 3 |
| `data/maps/CaveOfOrigin_DianciesRoom/scripts.inc` | 6 | 5 |
| `data/maps/DewfordManor_1F/scripts.inc` | 1 | 0 |
| `data/maps/DewfordMeadow/scripts.inc` | 3 | 2 |
| `data/maps/Route111_RuinsExterior/scripts.inc` | 2 | 0 |
| `data/maps/ScorchedSlab_HeatransRoom/scripts.inc` | 1 | 0 |
| `data/maps/Seaspray_Cave/scripts.inc` | 1 | 1 |


## Authorized extension: actor restoration

Preserved every existing object and appended one stationary, visible NPC (`flag: 0`) per map. Each graphics constant exists, each event script label exists, and no coordinate overlaps an object, warp, sign, hidden item or coordinate trigger.

| Actor | Map position | Packed cell | Collision / elevation | Source connection to adjacent interaction tile |
|---|---|---|---|---|
| Meadow warden, Pokéfan F | DewfordMeadow (27,12) | 0x3004; MB_NORMAL | 0 / 3 | From east boundary (39,3), matched to DewfordTown west boundary using its connection offset; 22 steps to (28,12). |
| Archaeologist, Hiker | Route111_RuinsExterior (15,12) | 0x3129; MB_NORMAL | 0 / 3 | From existing Sandstrewn Ruins entrance warp (13,11); 6 steps to (16,12). |
| Historian, Expert M | DewfordManor_1F (14,10) | 0x3238; MB_INDOOR_ENCOUNTER | 0 / 3 | From existing entrance warp (11,11); 5 steps to (15,10). |

All four adjacent interaction tiles are source-reachable for each new NPC. A conservative walkable-cell flood fill compared the original and amended placement: no formerly reachable cell except the NPC's occupied tile became disconnected. Decoding used production map.bin, tileset attribute binaries and the engine's collision/elevation masks; walkable ordinary ground, grass, sand and indoor encounter floors were distinguished from walls/water. This establishes local source geometry and script binding, not an emulated route traversal or visual NPC rendering. Exact cells and paths are in `npcs_a-actor-wiring.json`.

The warden and archaeologist already call `TryUnlockLocalLegendaryDiscovery`, which checks exact map, badges, the Sing/Castform condition and permanent completion. Restoring their callers makes the existing quests accessible; no quest mechanics changed. The historian uses its existing lore-only script.

## Authorized extension: C guidance

Read all **286 translated string expressions** across the requested C files: **111** in `src/legendary_signs.c`, **175** in `src/field_specials.c`, and **0** in `src/caps.c`. Captured source locations/text are in `npcs_a-c-reviewed.json`; caps contains no player-facing strings. An additional literal inventory found only palette paths/extensions outside translation macros in field_specials, and no overlooked dialogue in the other files.

Changed three guide expressions: `Needs partner` became `Check clue`, because several requirements accept caught records rather than current party membership; the badge count no longer renders `1 Gym Badges`; and Sweet Scent now explicitly divides its 25% total equally among eligible, uncaught discoveries. Source inspection confirmed the underlying selector does exactly that. Stripping C string literals left each of the three files identical to its before snapshot. No C gameplay logic changed.

The existing EV editor strings accurately use 252 per stat, 510 total and resulting stats after Nature. Its numeric adjustment labels are EV deltas, not the retired Stat Points system. Retained old BP menu entries are legacy menu data, consistent with the disconnected Frontier tutor paths already reviewed; the active tutors open the free specialist.

## Authorized extension: five legendary form tools

Added **DNA Splicers, Zygarde Cube, N-Solarizer, N-Lunarizer and Reins of Unity** to the canonical `FORM_ACCESS_ITEMS` set in `scripts/generate_emerald_champions_mega_archive.py`. Regenerated only `src/data/emerald_champions_evolution_items.h` through that generator's `evolution_items()` function: its only new entries are those five items, for 64 total. The unrelated Mega Stone catalogue was not rewritten. Also corrected stale generator console wording that called the Mega catalogue a postgame archive.

The existing vendor opens Evolution Items when the player has the Mega Ring, so no new gate/flag was introduced. The same generated list is consumed by field_specials and battle_sets; no duplicate ownership was introduced. Native shop source supports these key items: their definitions have importance 1 and POCKET_KEY_ITEMS; the shop uses quantity one, rejects an already-owned Bag/PC copy, prices free-catalog entries at zero and calls ordinary AddBagItem, which selects the destination pocket. A full bag uses the existing failure path. These checks are source evidence, not an executed acquisition/fusion test. Main reviewer handles Birch/Steven/archive explanatory dialogue.

## Final endpoint audit extension

Completed a separate source-callability audit of all 54 rare-wild definitions, 46 dialogue candidates and 46 guide handler branches. Restored four further missing dedicated guide actors (Altering 1F, Woods3, Scorched B2F, Sandstrewn Ruins). Five duplicate MtPyre clue blocks remain dormant; no stale active instructions for the removed encounter stones were found. Full counts, call-chain limits and geometry evidence are in `npcs_a-endpoint-audit.md` and its JSON companions. No runtime claim is made.
