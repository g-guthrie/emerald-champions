# Underwater_SeafloorCavern

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/Underwater_SeafloorCavern/map.json) · [Scripts](../../baseline/source/data/maps/Underwater_SeafloorCavern/scripts.inc)

## Current map contract

`MAP_UNDERWATER_SEAFLOOR_CAVERN` · `LAYOUT_UNDERWATER_SEAFLOOR_CAVERN` · `WEATHER_UNDERWATER_BUBBLES` · `MUS_UNDERWATER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Underwater_SeafloorCavern:object_events:001` | 5,4 | [Underwater_SeafloorCavern_EventScript_CheckStolenSub](../../baseline/source/data/maps/Underwater_SeafloorCavern/scripts.inc#L39) — Underwater_SeafloorCavern_EventScript_CheckStolenSub at (5,4); “SUBMARINE EXPLORER 1” is painted /  on the hull. //  This is the submarine TEAM AQUA /  stole in SLATEPORT! //  TEAM AQUA must have gone /  ashore here. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Underwater_SeafloorCavern:object_events:002` | 6,4 | [Underwater_SeafloorCavern_EventScript_CheckStolenSub](../../baseline/source/data/maps/Underwater_SeafloorCavern/scripts.inc#L39) — Underwater_SeafloorCavern_EventScript_CheckStolenSub at (6,4); “SUBMARINE EXPLORER 1” is painted /  on the hull. //  This is the submarine TEAM AQUA /  stole in SLATEPORT! //  TEAM AQUA must have gone /  ashore here. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Underwater_SeafloorCavern:object_events:003` | 7,4 | [Underwater_SeafloorCavern_EventScript_CheckStolenSub](../../baseline/source/data/maps/Underwater_SeafloorCavern/scripts.inc#L39) — Underwater_SeafloorCavern_EventScript_CheckStolenSub at (7,4); “SUBMARINE EXPLORER 1” is painted /  on the hull. //  This is the submarine TEAM AQUA /  stole in SLATEPORT! //  TEAM AQUA must have gone /  ashore here. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Underwater_SeafloorCavern:object_events:004` | 8,4 | [Underwater_SeafloorCavern_EventScript_CheckStolenSub](../../baseline/source/data/maps/Underwater_SeafloorCavern/scripts.inc#L39) — Underwater_SeafloorCavern_EventScript_CheckStolenSub at (8,4); “SUBMARINE EXPLORER 1” is painted /  on the hull. //  This is the submarine TEAM AQUA /  stole in SLATEPORT! //  TEAM AQUA must have gone /  ashore here. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Underwater_SeafloorCavern:object_events:005` | 9,5 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_TATSUGIRINITE; root Common_EventScript_FindItem; flag FLAG_EC_MEGA_REWARD_TATSUGIRINITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `Underwater_SeafloorCavern:object_events:006` | 8,6 | [Common_EventScript_LegendaryLandmark](../../baseline/source/data/event_scripts.s#L1575) — Common_EventScript_LegendaryLandmark at (8,6); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `Underwater_SeafloorCavern:warp_events:001` | 6,7 | Warp from (6,7, elevation 0) to MAP_UNDERWATER_ROUTE128 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Underwater_SeafloorCavern:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [Underwater_SeafloorCavern_OnResume](../../baseline/source/data/maps/Underwater_SeafloorCavern/scripts.inc#L35) — MAP_SCRIPT_ON_RESUME calls Underwater_SeafloorCavern_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Underwater_SeafloorCavern:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [Underwater_SeafloorCavern_OnTransition](../../baseline/source/data/maps/Underwater_SeafloorCavern/scripts.inc#L7) — MAP_SCRIPT_ON_TRANSITION calls Underwater_SeafloorCavern_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Underwater_SeafloorCavern:map_scripts:003` | MAP_SCRIPT_ON_LOAD | [Underwater_SeafloorCavern_OnLoad](../../baseline/source/data/maps/Underwater_SeafloorCavern/scripts.inc#L16) — MAP_SCRIPT_ON_LOAD calls Underwater_SeafloorCavern_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
