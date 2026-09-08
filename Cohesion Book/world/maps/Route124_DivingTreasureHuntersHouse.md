# Route124_DivingTreasureHuntersHouse

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/Route124_DivingTreasureHuntersHouse/map.json) · [Scripts](../../baseline/source/data/maps/Route124_DivingTreasureHuntersHouse/scripts.inc)

## Current map contract

`MAP_ROUTE124_DIVING_TREASURE_HUNTERS_HOUSE` · `LAYOUT_ROUTE124_DIVING_TREASURE_HUNTERS_HOUSE` · `WEATHER_NONE` · `MUS_LILYCOVE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route124_DivingTreasureHuntersHouse:object_events:001` | 5,4 | [Route124_DivingTreasureHuntersHouse_EventScript_TreasureHunter](../../baseline/source/data/maps/Route124_DivingTreasureHuntersHouse/scripts.inc#L9) — Route124_DivingTreasureHuntersHouse_EventScript_TreasureHunter at (5,4); I'm the DIVING TREASURE HUNTER! //  I'm the awesome dude who makes /  deep-sea dives to gather treasures /  resting at the bottom. / Tell me, have you seen any SHARDS of /  tools made in ancient times? | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route124_DivingTreasureHuntersHouse:bg_events:001` | 7,1 | [Route124_DivingTreasureHuntersHouse_EventScript_ShardTradeBoard](../../baseline/source/data/maps/Route124_DivingTreasureHuntersHouse/scripts.inc#L282) — Route124_DivingTreasureHuntersHouse_EventScript_ShardTradeBoard at (7,1); {CLEAR_TO 10}Wanted item{CLEAR_TO 124}Trade item /  {CLEAR_TO 15}RED SHARD{CLEAR_TO 89}{LEFT_ARROW}{RIGHT_ARROW}{CLEAR_TO 123}FIRE STONE{CLEAR_TO 200} //  {CLEAR_TO 10}Wanted item{CLEAR_TO 124}Trade item /  {CLEAR_TO 6}YELLOW SHARD{CLEAR_TO 89}{LEFT_ARROW}{RIGHT_ARROW}{CLEAR_TO 115}THUNDERSTONE{CLEAR_TO 200} //  {CLEAR_TO 10}Wanted item{CLEAR_TO 124}Trade item /  {CLEAR_TO 12}BLUE SHARD{CLEAR_TO 89}{LEFT_ARROW}{RIGHT_ARROW}{CLEAR_TO 121}WATER STONE{CLEAR_TO 200} //  {CLEAR_TO 10}Wanted item{CLEAR_TO 124}Trade item /  {CLEAR_TO 8}GREEN SHARD{CLEAR_TO 89}{LEFT_ARROW}{RIGHT_ARROW}{CLEAR_TO 123}LEAF STONE | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `Route124_DivingTreasureHuntersHouse:warp_events:001` | 3,8 | Warp from (3,8, elevation 0) to MAP_ROUTE124 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route124_DivingTreasureHuntersHouse:warp_events:002` | 4,8 | Warp from (4,8, elevation 0) to MAP_ROUTE124 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route124_DivingTreasureHuntersHouse:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route124_DivingTreasureHuntersHouse_OnTransition](../../baseline/source/data/maps/Route124_DivingTreasureHuntersHouse/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route124_DivingTreasureHuntersHouse_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
