# NPC B dialogue review — September 9, 2026

Read every assigned source dialogue block: **1,589 blocks in 107 files**. Edited **43 blocks in 23 files**. Every changed game file differs from the provided before snapshot only in `.string` contents; no script command, battle team, condition, reward, map, or mechanic was changed.

## Material fixes

- Replaced the Rusturf thief’s nonexistent Dondozo with the actual party’s Mightyena. Verified the materialized trainer party; Sharpedo, Toxapex, Claydol/Garbodor and Scrafty references in other assigned grunt scenes matched their current parties and were retained.
- Corrected Reshiram’s obsolete Dialga implication and Zygarde’s obsolete Landorus implication against the current legendary definitions. Entei is explicitly a Fiery Path wild sighting after the Dynamo Badge.
- Separated the Mt. Chimney meteorite machinery from later Orb awakenings. Ashen Woods no longer blames an event that has not happened on its first visit; local weather disturbance foreshadows Rayquaza.
- Aqua’s hideout dialogue no longer prematurely announces the submarine departure. The Seafloor guards now agree with the submarine parked outside and describe natural passages rather than invented moving geography.
- Scott’s S.S. Tidal invitation works before becoming Champion. Frontier descriptions reflect one Circuit and remember the old Dome tournaments as history.
- Evie’s Heart Scale exchange is not described as free; Cynthia’s repeat reward speech acknowledges ownership and her defeat line does not invent the player’s exact strategy.
- Retained local humor and character voices; replaced generic Ashen Woods battle lectures with interests in insects, rain, ash and the forest. Updated the quiz host to match its actual questions.

## Source context examined

Direct script/consumer inspection covered Route102’s appended discovery guide, Route112 meteorite/Chansey/board branches, Route113 ash and clue callbacks, Route114 fishing and gift branches, Route116 seeker conditions, Route120 sign continuation, Route104 Briney delivery branches, Day Care egg/reward behavior, Berry Master payments, Seashore House reward and price, fossil tunnel opening, Cynthia gate/repeat reward, Aqua hideout guard branches and static encounters, Seafloor entry/Dive and submarine scenes, Ashen Woods chase, Ember Path Moltres, Scorched Slab Heatran room, Mt. Pyre Pecharunt requirements, Cave of Origin Carbink map flag/ladder, Mirage Tower passage, Trainer Hill completion branch, current Circuit Battle Point calculation, Dream Ball catch behavior, current NPC trade species, Nature Girl message selection, regional evolution definitions, and current legendary definitions. Other blocks received complete dialogue reading and local/cohesion review; this is not a claim that every underlying C branch was audited.

## Retained historical source and limits

The Fallarbor/Slateport corridor Contest Hall prose is explicitly unused old source. Retired Frontier internal rooms, partner recruitment, rental rules, symbols and wager branches remain in source; current entry/rules/pass desks redirect to the Circuit. I read their dialogue but did not rewrite unreachable historical strings into fictitious new mechanics. No builds, tests, native battles, map traversal, or rendered dialogue were run. All findings and retention decisions here are source review, not runtime proof. No known active dialogue mismatch remains from this assigned reading. Source reachability is not exhaustively proven for every historical entry label.

## File coverage

| File | Blocks read | Blocks edited |
|---|---:|---:|
| `data/maps/Route102/scripts.inc` | 7 | 1 |
| `data/maps/Route105/scripts.inc` | 3 | 0 |
| `data/maps/Route106/scripts.inc` | 2 | 0 |
| `data/maps/Route107/scripts.inc` | 1 | 0 |
| `data/maps/Route108/scripts.inc` | 2 | 0 |
| `data/maps/Route109/scripts.inc` | 19 | 0 |
| `data/maps/Route112/scripts.inc` | 10 | 3 |
| `data/maps/Route113/scripts.inc` | 6 | 0 |
| `data/maps/Route114/scripts.inc` | 10 | 2 |
| `data/maps/Route115/scripts.inc` | 6 | 0 |
| `data/maps/Route116/scripts.inc` | 19 | 0 |
| `data/maps/Route120/scripts.inc` | 19 | 0 |
| `data/maps/Route126/scripts.inc` | 2 | 0 |
| `data/maps/Route128/scripts.inc` | 9 | 0 |
| `data/maps/FallarborTown_BattleTentCorridor/scripts.inc` | 4 | 0 |
| `data/maps/FallarborTown_PokemonCenter_1F/scripts.inc` | 3 | 0 |
| `data/maps/FallarborTown_MoveRelearnersHouse/scripts.inc` | 9 | 1 |
| `data/maps/VerdanturfTown_BattleTentLobby/scripts.inc` | 9 | 0 |
| `data/maps/VerdanturfTown_WandasHouse/scripts.inc` | 13 | 0 |
| `data/maps/PacifidlogTown_House1/scripts.inc` | 2 | 0 |
| `data/maps/PacifidlogTown_House3/scripts.inc` | 6 | 0 |
| `data/maps/PacifidlogTown_House5/scripts.inc` | 3 | 0 |
| `data/maps/SlateportCity_BattleTentCorridor/scripts.inc` | 15 | 0 |
| `data/maps/SlateportCity_House/scripts.inc` | 2 | 1 |
| `data/maps/SlateportCity_PokemonCenter_1F/scripts.inc` | 2 | 0 |
| `data/maps/SlateportCity_Mart/scripts.inc` | 2 | 0 |
| `data/maps/MauvilleCity_BikeShop/scripts.inc` | 22 | 0 |
| `data/maps/MauvilleCity_House2/scripts.inc` | 7 | 0 |
| `data/maps/MauvilleCity_PokemonCenter_1F/scripts.inc` | 3 | 0 |
| `data/maps/MauvilleCity_PokemonCenter_2F/scripts.inc` | 1 | 0 |
| `data/maps/MauvilleCity_Mart/scripts.inc` | 2 | 0 |
| `data/maps/LilycoveCity_CoveLilyMotel_1F/scripts.inc` | 7 | 0 |
| `data/maps/LilycoveCity_LilycoveMuseum_1F/scripts.inc` | 24 | 0 |
| `data/maps/LilycoveCity_ContestLobby/scripts.inc` | 17 | 0 |
| `data/maps/LilycoveCity_PokemonCenter_1F/scripts.inc` | 3 | 0 |
| `data/maps/LilycoveCity_PokemonTrainerFanClub/scripts.inc` | 38 | 0 |
| `data/maps/LilycoveCity_House4/scripts.inc` | 4 | 0 |
| `data/maps/LilycoveCity_DepartmentStore_2F/scripts.inc` | 3 | 0 |
| `data/maps/LilycoveCity_DepartmentStore_3F/scripts.inc` | 3 | 0 |
| `data/maps/LilycoveCity_DepartmentStore_4F/scripts.inc` | 5 | 1 |
| `data/maps/MossdeepCity_House1/scripts.inc` | 10 | 2 |
| `data/maps/MossdeepCity_House3/scripts.inc` | 5 | 0 |
| `data/maps/MossdeepCity_House4/scripts.inc` | 5 | 0 |
| `data/maps/MossdeepCity_GameCorner_1F/scripts.inc` | 10 | 0 |
| `data/maps/Route104_MrBrineysHouse/scripts.inc` | 9 | 0 |
| `data/maps/Route111_OldLadysRestStop/scripts.inc` | 4 | 0 |
| `data/maps/Route114_FossilManiacsTunnel/scripts.inc` | 4 | 0 |
| `data/maps/Route116_TunnelersRestHouse/scripts.inc` | 4 | 0 |
| `data/maps/Route117_PokemonDayCare/scripts.inc` | 9 | 0 |
| `data/maps/RusturfTunnel/scripts.inc` | 18 | 1 |
| `data/maps/MtPyre_1F/scripts.inc` | 4 | 0 |
| `data/maps/MtPyre_2F/scripts.inc` | 19 | 0 |
| `data/maps/MtPyre_4F/scripts.inc` | 3 | 0 |
| `data/maps/MtPyre_5F/scripts.inc` | 3 | 0 |
| `data/maps/MtPyre_6F/scripts.inc` | 14 | 0 |
| `data/maps/AquaHideout_B1F/scripts.inc` | 12 | 2 |
| `data/maps/Underwater_SeafloorCavern/scripts.inc` | 1 | 0 |
| `data/maps/SeafloorCavern_Entrance/scripts.inc` | 2 | 2 |
| `data/maps/SeafloorCavern_Room1/scripts.inc` | 6 | 5 |
| `data/maps/SeafloorCavern_Room4/scripts.inc` | 6 | 0 |
| `data/maps/CaveOfOrigin_1F/scripts.inc` | 1 | 0 |
| `data/maps/VictoryRoad_B1F/scripts.inc` | 15 | 0 |
| `data/maps/VictoryRoad_B2F/scripts.inc` | 18 | 0 |
| `data/maps/AbandonedShip_Corridors_1F/scripts.inc` | 4 | 0 |
| `data/maps/AbandonedShip_Rooms_1F/scripts.inc` | 11 | 0 |
| `data/maps/AbandonedShip_Corridors_B1F/scripts.inc` | 7 | 0 |
| `data/maps/AbandonedShip_Rooms2_1F/scripts.inc` | 23 | 0 |
| `data/maps/MagmaHideout_2F_1R/scripts.inc` | 12 | 0 |
| `data/maps/MagmaHideout_3F_1R/scripts.inc` | 6 | 0 |
| `data/maps/MirageTower_4F/scripts.inc` | 4 | 0 |
| `data/maps/InsideOfTruck/scripts.inc` | 1 | 0 |
| `data/maps/SSTidalCorridor/scripts.inc` | 16 | 1 |
| `data/maps/BattleFrontier_OutsideWest/scripts.inc` | 37 | 4 |
| `data/maps/BattleFrontier_BattleTowerBattleRoom/scripts.inc` | 28 | 0 |
| `data/maps/BattleFrontier_OutsideEast/scripts.inc` | 32 | 1 |
| `data/maps/BattleFrontier_BattleTowerMultiPartnerRoom/scripts.inc` | 344 | 0 |
| `data/maps/BattleFrontier_BattlePyramidFloor/scripts.inc` | 129 | 0 |
| `data/maps/BattleFrontier_BattleArenaCorridor/scripts.inc` | 1 | 0 |
| `data/maps/BattleFrontier_BattleArenaBattleRoom/scripts.inc` | 33 | 0 |
| `data/maps/BattleFrontier_BattleFactoryLobby/scripts.inc` | 36 | 0 |
| `data/maps/BattleFrontier_BattlePikeCorridor/scripts.inc` | 1 | 0 |
| `data/maps/BattleFrontier_BattlePikeThreePathRoom/scripts.inc` | 25 | 0 |
| `data/maps/BattleFrontier_BattlePikeRoomNormal/scripts.inc` | 43 | 0 |
| `data/maps/BattleFrontier_ExchangeServiceCorner/scripts.inc` | 40 | 2 |
| `data/maps/BattleFrontier_Lounge3/scripts.inc` | 42 | 0 |
| `data/maps/BattleFrontier_Lounge5/scripts.inc` | 15 | 1 |
| `data/maps/BattleFrontier_Lounge6/scripts.inc` | 5 | 1 |
| `data/maps/BattleFrontier_ReceptionGate/scripts.inc` | 33 | 0 |
| `data/maps/TrainerHill_Roof/scripts.inc` | 1 | 0 |
| `data/maps/Route109_SeashoreHouse/scripts.inc` | 17 | 0 |
| `data/maps/Route110_TrickHouseEntrance/scripts.inc` | 26 | 0 |
| `data/maps/Route110_TrickHousePuzzle1/scripts.inc` | 10 | 1 |
| `data/maps/Route110_TrickHousePuzzle2/scripts.inc` | 10 | 0 |
| `data/maps/Route110_TrickHousePuzzle3/scripts.inc` | 10 | 0 |
| `data/maps/Route110_TrickHousePuzzle5/scripts.inc` | 25 | 1 |
| `data/maps/Route110_SeasideCyclingRoadSouthEntrance/scripts.inc` | 2 | 0 |
| `data/maps/Route110_SeasideCyclingRoadNorthEntrance/scripts.inc` | 2 | 0 |
| `data/maps/Route123_BerryMastersHouse/scripts.inc` | 14 | 0 |
| `data/maps/Route119_House/scripts.inc` | 2 | 0 |
| `data/maps/Route124_DivingTreasureHuntersHouse/scripts.inc` | 11 | 0 |
| `data/maps/AshenWoods/scripts.inc` | 19 | 7 |
| `data/maps/EmberPath/scripts.inc` | 1 | 1 |
| `data/maps/MirageTower_B1F/scripts.inc` | 1 | 0 |
| `data/maps/PetalburgWoods_3/scripts.inc` | 1 | 0 |
| `data/maps/SandstrewnRuins/scripts.inc` | 1 | 1 |
| `data/maps/ScorchedSlab_B2F/scripts.inc` | 1 | 1 |
| `data/maps/VerdanturfMeadow/scripts.inc` | 1 | 0 |

## Edited labels and findings

### `data/maps/Route102/scripts.inc`

- `Route102_Text_CatchWholeBunchOfPokemon`: Keep the early Shaymin clue and rates while giving the witness an excited, local voice.

### `data/maps/Route112/scripts.inc`

- `Route112_Text_LeaderGoingToAwakenThing`: Meteorite conversation concerns the Mt. Chimney machine, not an Orb substitute for awakening Groudon.
- `Route112_Text_YeahWeNeedMeteorite`: Separate meteorite machinery from later Orb awakening.
- `Route112_Text_MtChimneySign`: Replace old guardian-answer wording with the verified three-badge Fiery Path wild sighting.

### `data/maps/Route114/scripts.inc`

- `Route114_Text_GoodRodOffer`: Rod variety need not imply all Good Rod residents are stronger than Old Rod residents.
- `Route114_Text_GoodRodAdvice`: Replace encounter-pool implementation jargon with fisherman language.

### `data/maps/FallarborTown_MoveRelearnersHouse/scripts.inc`

- `FallarborTown_MoveRelearnersHouse_Text_IvyEvieIntro3`: The PP Up exchange costs a Heart Scale, so the whole house is not all free.

### `data/maps/SlateportCity_House/scripts.inc`

- `SlateportCity_House_Text_NatureToDoWithStatGains`: Replace vague comparison across different species with correct Nature tradeoff and free service awareness.

### `data/maps/LilycoveCity_DepartmentStore_4F/scripts.inc`

- `LilycoveCity_DepartmentStore_4F_Text_AttackOrDefense`: Give the shopper a concrete dilemma and personality instead of generic battle-set jargon.

### `data/maps/MossdeepCity_House1/scripts.inc`

- `MossdeepCity_House1_Text_CynthiaDefeat`: Do not claim the player denied sleep or followed one specific tactical solution.
- `MossdeepCity_House1_Text_CynthiaAfter`: This shared reward/repeat branch must not repeatedly ask the player to take an already-owned charm.

### `data/maps/RusturfTunnel/scripts.inc`

- `RusturfTunnel_Text_GruntIntro`: The current Rusturf grunt has Mightyena, Qwilfish, Skrelp, and Zubat, not Dondozo.

### `data/maps/AquaHideout_B1F/scripts.inc`

- `AquaHideout_B1F_Text_Grunt2PostBattle`: The grunt can be defeated before or after departure; describe the permanent dock and warp panels rather than assert the submarine left.
- `AquaHideout_B1F_Text_Grunt3PostBattle`: Do not assert the submarine has left before the departure scene.

### `data/maps/SeafloorCavern_Entrance/scripts.inc`

- `SeafloorCavern_Entrance_Text_HearMagmaNearMossdeep`: Replace stale Mossdeep detour with Aqua expedition motive and recognition of the player arriving by Dive.
- `SeafloorCavern_Entrance_Text_HearMagmaNearMossdeepShort`: Keep repeat dialogue coherent with the new expedition speech and actual route forward.

### `data/maps/SeafloorCavern_Room1/scripts.inc`

- `SeafloorCavern_Room1_Text_Grunt1Intro`: Replace impossible shifting cavern geography with a boast about its real currents.
- `SeafloorCavern_Room1_Text_Grunt1Defeat`: Avoid claiming the floor physically changed during battle.
- `SeafloorCavern_Room1_Text_Grunt1PostBattle`: Ancient natural rooms were not built by Aqua to misdirect pursuit.
- `SeafloorCavern_Room1_Text_Grunt2Intro`: Give the guard a physical, local threat rather than formation jargon.
- `SeafloorCavern_Room1_Text_Grunt2PostBattle`: The stolen submarine remains visible underwater outside the cavern.

### `data/maps/SSTidalCorridor/scripts.inc`

- `SSTidalCorridor_Text_ScottBattleFrontierInvite`: Ferries and Circuit open after six badges; player may not be Champion or need a new ferry unlock.

### `data/maps/BattleFrontier_OutsideWest/scripts.inc`

- `BattleFrontier_OutsideWest_Text_ThisIsBattleTower`: Replace old elimination-style room description with current Circuit arena identity.
- `BattleFrontier_OutsideWest_Text_YoureOffToChallengeDome`: All desks enter the same Circuit; no separate Dome tournament.
- `BattleFrontier_OutsideWest_Text_DomeAceLookedBecauseOfMyCheering`: Frame retired Dome event fandom as a remembered tournament.
- `BattleFrontier_OutsideWest_Text_FansOverThereUsedToBeTrainers`: No daily active Dome tournament under current Circuit.

### `data/maps/BattleFrontier_OutsideEast/scripts.inc`

- `BattleFrontier_OutsideEast_Text_EnoughBattlePointsForDoll`: Give the reward shopper a personal goal while preserving the exchange.

### `data/maps/BattleFrontier_ExchangeServiceCorner/scripts.inc`

- `BattleFrontier_ExchangeServiceCorner_Text_MoreBattlePointsForRecord`: Replace separate facility record rewards with Circuit progression and personal flavor.
- `BattleFrontier_ExchangeServiceCorner_Text_DreamBallDesc`: Dream Ball catch bonus concerns sleep, not vaguely dreamlike species.

### `data/maps/BattleFrontier_Lounge5/scripts.inc`

- `BattleFrontier_Lounge5_Text_NatureGirlGreeting`: Preserve the child guessing personalities without presenting retired Palace autonomous move rules as current mechanics.

### `data/maps/BattleFrontier_Lounge6/scripts.inc`

- `BattleFrontier_Lounge6_Text_WouldYouLikeToTrade`: No Champion requirement for the six-badge Frontier trade; make the trader value its future.

### `data/maps/Route110_TrickHousePuzzle1/scripts.inc`

- `Route110_TrickHousePuzzle1_Text_SallyIntro`: Field Cut registration does not imply the battle team knows Cut.

### `data/maps/Route110_TrickHousePuzzle5/scripts.inc`

- `Route110_TrickHousePuzzle5_Text_Mechadoll3Intro`: Its current quiz asks about burn, Helping Hand, and Eviolite, not money.

### `data/maps/AshenWoods/scripts.inc`

- `AshenWoods_Text_MartinAfter`: Moltres actor is in Ember Path, not beyond it; remove nonexistent implied strength condition.
- `AshenWoods_Text_RomanIntro`: Ground the trainer challenge in the local environment instead of engine jargon.
- `AshenWoods_Text_RomanDefeat`: Do not presume the player stopped a combo before it activated.
- `AshenWoods_Text_RomanAfter`: Give the trainer local interests beyond doubles instructions.
- `AshenWoods_Text_ElmerIntro`: Remove arbitrary demand that the player show four answers.
- `AshenWoods_Text_ElmerAfter`: Replace generic Champion toolbox lecture with a bug enthusiast.
- `AshenWoods_Text_Caretaker`: This area is accessible before Groudon awakens; foreshadow weather balance without claiming a later event occurred.

### `data/maps/EmberPath/scripts.inc`

- `EmberPath_Text_Warden`: Remove an unsupported eastward heavier-creature clue and blanket claim about every fault.

### `data/maps/SandstrewnRuins/scripts.inc`

- `SandstrewnRuins_Text_Archaeologist`: Zygarde is a five-badge wild discovery, without a Landorus prerequisite.

### `data/maps/ScorchedSlab_B2F/scripts.inc`

- `ScorchedSlab_B2F_Text_Warden`: Reshiram is a six-badge local wild discovery, without a Dialga prerequisite.

