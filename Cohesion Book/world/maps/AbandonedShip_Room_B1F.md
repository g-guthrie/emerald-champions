# AbandonedShip_Room_B1F

**REVISE.** W-SPIRITOMB-01 makes the eerie socket legible and uses Odd Keystone alone. Preserve the ordinary Spiritomb paths and the Keystone-only-on-capture transaction.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/AbandonedShip_Room_B1F/map.json) · [Scripts](../../baseline/source/data/maps/AbandonedShip_Room_B1F/scripts.inc)

## Current map contract

`MAP_ABANDONED_SHIP_ROOM_B1F` · `LAYOUT_ABANDONED_SHIP_ROOM_B1F` · `WEATHER_SHADE` · `MUS_ABANDONED_SHIP`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `AbandonedShip_Room_B1F:object_events:001` | 4,4 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_KELPSY_BERRY; root Common_EventScript_FindItem; flag FLAG_EC_GARDEN_BUNDLE_ABANDONEDSHIP_ROOM_B1F_KELPSY_BERRY. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AbandonedShip_Room_B1F:bg_events:001` | 7,7 | [AbandonedShip_Room_B1F_EventScript_Spiritomb](../../baseline/source/data/maps/AbandonedShip_Room_B1F/scripts.inc#L4) — AbandonedShip_Room_B1F_EventScript_Spiritomb at (7,7); Something compels you to place the /  ODD KEYSTONE in this trash can. //  Place the ODD KEYSTONE? | **REPAIR** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · W-SPIRITOMB-01 |
| `AbandonedShip_Room_B1F:warp_events:001` | 4,7 | Warp from (4,7, elevation 3) to MAP_ABANDONED_SHIP_CORRIDORS_B1F warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Room_B1F:warp_events:002` | 5,7 | Warp from (5,7, elevation 3) to MAP_ABANDONED_SHIP_CORRIDORS_B1F warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
