# Route114

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../04-volcano-and-desert.md) · [Map source](../../baseline/source/data/maps/Route114/map.json) · [Scripts](../../baseline/source/data/maps/Route114/scripts.inc)

## Current map contract

`MAP_ROUTE114` · `LAYOUT_ROUTE114` · `WEATHER_SUNNY` · `MUS_ROUTE110`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route114:object_events:001` | 31,43 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (31,43); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route114:object_events:002` | 31,44 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (31,44); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route114:object_events:003` | 15,65 | [Route114_EventScript_Lenny](../../baseline/source/data/maps/Route114/scripts.inc#L110) — Route114_EventScript_Lenny at (15,65); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route114:object_events:004` | 30,72 | [Route114_EventScript_Lucas](../../baseline/source/data/maps/Route114/scripts.inc#L115) — Route114_EventScript_Lucas at (30,72); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route114:object_events:005` | 22,50 | [Route114_EventScript_Shane](../../baseline/source/data/maps/Route114/scripts.inc#L120) — Route114_EventScript_Shane at (22,50); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route114:object_events:006` | 19,35 | [Route114_EventScript_Nancy](../../baseline/source/data/maps/Route114/scripts.inc#L125) — Route114_EventScript_Nancy at (19,35); I packed for every weather. /  My bag weighs more than I do. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route114:object_events:007` | 20,56 | [Route114_EventScript_Steve](../../baseline/source/data/maps/Route114/scripts.inc#L129) — Route114_EventScript_Steve at (20,56); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route114:object_events:008` | 31,45 | [BerryTreeScript](../../baseline/source/data/scripts/berry_tree.inc#L1) — BerryTreeScript at (31,45); shared behavior BERRY | **KEEP** · [W-C-BERRY](../common-contracts.md#w-c-berry) |
| `Route114:object_events:009` | 7,6 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ULTRA_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_114_ULTRA_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route114:object_events:010` | 11,37 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_TIMER_BALL; root Common_EventScript_FindItem; flag FLAG_ITEM_ROUTE_114_TIMER_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route114:object_events:011` | 12,43 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (12,43); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route114:object_events:012` | 19,11 | [Route114_EventScript_RoarGentleman](../../baseline/source/data/maps/Route114/scripts.inc#L71) — Route114_EventScript_RoarGentleman at (19,11); All my POKéMON does is ROAR… /  No one dares to come near me… //  I found this DRAGON SCALE nearby. /  Please, take it off my hands. / A Dragon Scale evolves Seadra. /  Use it from the Bag when ready. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route114:object_events:013` | 19,12 | [Route114_EventScript_Poochyena](../../baseline/source/data/maps/Route114/scripts.inc#L88) — Route114_EventScript_Poochyena at (19,12); Bow! Bowwow! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route114:object_events:014` | 27,42 | [Route114_EventScript_Man](../../baseline/source/data/maps/Route114/scripts.inc#L53) — Route114_EventScript_Man at (27,42); shared behavior STORY | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route114:object_events:015` | 25,6 | [Route114_EventScript_Nolan](../../baseline/source/data/maps/Route114/scripts.inc#L176) — Route114_EventScript_Nolan at (25,6); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route114:object_events:016` | 19,26 | [Route114_EventScript_Claude](../../baseline/source/data/maps/Route114/scripts.inc#L171) — Route114_EventScript_Claude at (19,26); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route114:object_events:017` | 30,58 | [Route114_EventScript_Bernie](../../baseline/source/data/maps/Route114/scripts.inc#L150) — Route114_EventScript_Bernie at (30,58); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route114:object_events:018` | 29,53 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (29,53); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route114:object_events:019` | 30,54 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (30,54); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route114:object_events:020` | 22,69 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (22,69); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route114:object_events:021` | 11,64 | [EventScript_RockSmash](../../baseline/source/data/scripts/field_move_scripts.inc#L65) — EventScript_RockSmash at (11,64); shared behavior FIELD | **KEEP** · [W-C-FIELD](../common-contracts.md#w-c-field) |
| `Route114:object_events:022` | 24,44 | [Route114_EventScript_Ivy](../../baseline/source/data/maps/Route114/scripts.inc#L186) — Route114_EventScript_Ivy at (24,44); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route114:object_events:023` | 23,44 | [Route114_EventScript_Tyra](../../baseline/source/data/maps/Route114/scripts.inc#L181) — Route114_EventScript_Tyra at (23,44); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route114:object_events:024` | 28,20 | [Route114_EventScript_Charlotte](../../baseline/source/data/maps/Route114/scripts.inc#L196) — Route114_EventScript_Charlotte at (28,20); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route114:object_events:025` | 26,72 | [Route114_EventScript_Angelina](../../baseline/source/data/maps/Route114/scripts.inc#L191) — Route114_EventScript_Angelina at (26,72); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route114:object_events:026` | 31,19 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ABSOLITE_Z; root Common_EventScript_FindItem; flag FLAG_EC_MEGA_REWARD_ABSOLITE_Z. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Route114:object_events:027` | 28,16 | [Route114_EventScript_Kai](../../baseline/source/data/maps/Route114/scripts.inc#L201) — Route114_EventScript_Kai at (28,16); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route114:object_events:028` | 22,29 | [Route114_EventScript_GoodRodFisherman](../../baseline/source/data/maps/Route114/scripts.inc#L7) — Route114_EventScript_GoodRodFisherman at (22,29); These pools hide stronger partners than /  an OLD ROD can reach. Take my GOOD ROD? / A GOOD ROD opens another layer of each /  route's encounter pool. Try every shore. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route114:bg_events:001` | 7,64 | [Route114_EventScript_MeteorFallsSign](../../baseline/source/data/maps/Route114/scripts.inc#L98) — Route114_EventScript_MeteorFallsSign at (7,64); METEOR FALLS /  RUSTBORO CITY THROUGH HERE | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route114:bg_events:002` | 31,7 | [Route114_EventScript_FossilManiacsHouseSign](../../baseline/source/data/maps/Route114/scripts.inc#L102) — Route114_EventScript_FossilManiacsHouseSign at (31,7); FOSSIL MANIAC'S HOUSE /  “Fossils from every era studied!” | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route114:bg_events:003` | 9,47 | None at (9,47); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route114:bg_events:004` | 30,51 | None at (30,51); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route114:bg_events:005` | 11,62 | None at (11,62); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route114:bg_events:006` | 19,70 | None at (19,70); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route114:bg_events:007` | 11,27 | None at (11,27); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route114:bg_events:008` | 12,27 | None at (12,27); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route114:bg_events:009` | 25,38 | [Route114_EventScript_LanettesHouseSign](../../baseline/source/data/maps/Route114/scripts.inc#L106) — Route114_EventScript_LanettesHouseSign at (25,38); LANETTE'S HOUSE | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route114:bg_events:010` | 20,57 | Hidden ITEM_STAR_PIECE at (20,57); persistent flag FLAG_HIDDEN_ITEM_ROUTE_114_STAR_PIECE. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route114:bg_events:011` | 32,57 | None at (32,57); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route114:bg_events:012` | 7,30 | Hidden ITEM_REVIVE at (7,30); persistent flag FLAG_HIDDEN_ITEM_ROUTE_114_REVIVE. | **KEEP** · [W-C-HIDDEN](../common-contracts.md#w-c-hidden) |
| `Route114:warp_events:001` | 8,63 | Warp from (8,63, elevation 0) to MAP_METEOR_FALLS_1F_1R warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route114:warp_events:002` | 29,5 | Warp from (29,5, elevation 0) to MAP_ROUTE114_FOSSIL_MANIACS_HOUSE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route114:warp_events:003` | 27,36 | Warp from (27,36, elevation 0) to MAP_ROUTE114_LANETTES_HOUSE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route114:warp_events:004` | 6,46 | Warp from (6,46, elevation 0) to MAP_TERRA_CAVE_ENTRANCE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route114:warp_events:005` | 7,4 | Warp from (7,4, elevation 0) to MAP_TERRA_CAVE_ENTRANCE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route114:connections:001` | left | left connection to MAP_ROUTE115, offset 40. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route114:connections:002` | right | right connection to MAP_FALLARBOR_TOWN, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route114:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route114_OnTransition](../../baseline/source/data/maps/Route114/scripts.inc#L38) — MAP_SCRIPT_ON_TRANSITION calls Route114_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route114:map_scripts:002` | MAP_SCRIPT_ON_LOAD | [Route114_OnLoad](../../baseline/source/data/maps/Route114/scripts.inc#L44) — MAP_SCRIPT_ON_LOAD calls Route114_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route114:map_scripts:003` | MAP_SCRIPT_ON_FRAME_TABLE | [Route114_OnFrame](../../baseline/source/data/maps/Route114/scripts.inc#L49) — MAP_SCRIPT_ON_FRAME_TABLE calls Route114_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
