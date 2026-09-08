# RustboroCity_DevonCorp_1F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/RustboroCity_DevonCorp_1F/map.json) · [Scripts](../../baseline/source/data/maps/RustboroCity_DevonCorp_1F/scripts.inc)

## Current map contract

`MAP_RUSTBORO_CITY_DEVON_CORP_1F` · `LAYOUT_RUSTBORO_CITY_DEVON_CORP_1F` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `RustboroCity_DevonCorp_1F:object_events:001` | 2,6 | [RustboroCity_DevonCorp_1F_EventScript_Employee](../../baseline/source/data/maps/RustboroCity_DevonCorp_1F/scripts.inc#L14) — RustboroCity_DevonCorp_1F_EventScript_Employee at (2,6); Hey, those RUNNING SHOES! /  They're one of our products! //  It makes me happy when I see someone /  using something we made. / That stolen parcel… //  Well, sure it's important, but it's not /  anything that anyone can use. //  In my estimation, that robber must not /  have been very bright. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `RustboroCity_DevonCorp_1F:object_events:002` | 15,5 | [RustboroCity_DevonCorp_1F_EventScript_StairGuard](../../baseline/source/data/maps/RustboroCity_DevonCorp_1F/scripts.inc#L33) — RustboroCity_DevonCorp_1F_EventScript_StairGuard at (15,5); I'm sorry, only authorized people /  are allowed to enter here. / It's beyond stupid. /  How could we get robbed? | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `RustboroCity_DevonCorp_1F:object_events:003` | 5,3 | [RustboroCity_DevonCorp_1F_EventScript_Greeter](../../baseline/source/data/maps/RustboroCity_DevonCorp_1F/scripts.inc#L53) — RustboroCity_DevonCorp_1F_EventScript_Greeter at (5,3); Hello and welcome to the DEVON /  CORPORATION. //  We're proud producers of items and /  medicine that enhance your life. / One of our research staff stupidly /  got robbed of an important parcel. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `RustboroCity_DevonCorp_1F:bg_events:001` | 3,2 | [RustboroCity_DevonCorp_1F_EventScript_ProductsDisplay](../../baseline/source/data/maps/RustboroCity_DevonCorp_1F/scripts.inc#L77) — RustboroCity_DevonCorp_1F_EventScript_ProductsDisplay at (3,2); Prototypes and test products fill /  the glass display case. //  There's a panel with a description… //  “In addition to industrial products, /  DEVON now markets sundries and /  pharmaceuticals for better lifestyles. //  “Recently, DEVON has begun marketing /  tools for POKéMON TRAINERS, including /  POKé BALLS and POKéNAV systems.” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `RustboroCity_DevonCorp_1F:bg_events:002` | 8,2 | [RustboroCity_DevonCorp_1F_EventScript_RocksMetalDisplay](../../baseline/source/data/maps/RustboroCity_DevonCorp_1F/scripts.inc#L73) — RustboroCity_DevonCorp_1F_EventScript_RocksMetalDisplay at (8,2); Samples of rocks and metal are /  displayed in the glass case. //  There's a panel with some writing /  on it… //  “DEVON CORPORATION got its start as /  a producer of stones from quarries. //  “The company also produced iron from /  filings in the sand. //  “From that humble start as a producer /  of raw materials, DEVON developed. //  “DEVON is now a manufacturer of a wide /  range of industrial products.” | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `RustboroCity_DevonCorp_1F:warp_events:001` | 5,8 | Warp from (5,8, elevation 0) to MAP_RUSTBORO_CITY warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RustboroCity_DevonCorp_1F:warp_events:002` | 6,8 | Warp from (6,8, elevation 0) to MAP_RUSTBORO_CITY warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RustboroCity_DevonCorp_1F:warp_events:003` | 14,1 | Warp from (14,1, elevation 0) to MAP_RUSTBORO_CITY_DEVON_CORP_2F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `RustboroCity_DevonCorp_1F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [RustboroCity_DevonCorp_1F_OnTransition](../../baseline/source/data/maps/RustboroCity_DevonCorp_1F/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls RustboroCity_DevonCorp_1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
