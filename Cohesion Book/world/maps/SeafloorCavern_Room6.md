# SeafloorCavern_Room6

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/SeafloorCavern_Room6/map.json) · [Scripts](../../baseline/source/data/maps/SeafloorCavern_Room6/scripts.inc)

## Current map contract

`MAP_SEAFLOOR_CAVERN_ROOM6` · `LAYOUT_SEAFLOOR_CAVERN_ROOM6` · `WEATHER_NONE` · `MUS_MT_CHIMNEY`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SeafloorCavern_Room6:object_events:001` | 2,2 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_DUSK_BALL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_SEAFLOOR_CAVERN_ROOM6_DUSK_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SeafloorCavern_Room6:warp_events:001` | 11,21 | Warp from (11,21, elevation 3) to MAP_SEAFLOOR_CAVERN_ROOM2 warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SeafloorCavern_Room6:warp_events:002` | 4,1 | Warp from (4,1, elevation 3) to MAP_SEAFLOOR_CAVERN_ROOM3 warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SeafloorCavern_Room6:warp_events:003` | 14,8 | Warp from (14,8, elevation 1) to MAP_SEAFLOOR_CAVERN_ENTRANCE warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
