# AbandonedShip_Rooms_1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/AbandonedShip_Rooms_1F/map.json) · [Scripts](../../baseline/source/data/maps/AbandonedShip_Rooms_1F/scripts.inc)

## Current map contract

`MAP_ABANDONED_SHIP_ROOMS_1F` · `LAYOUT_ABANDONED_SHIP_ROOMS_1F` · `WEATHER_SHADE` · `MUS_ABANDONED_SHIP`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `AbandonedShip_Rooms_1F:object_events:001` | 12,5 | [AbandonedShip_Rooms_1F_EventScript_Gentleman](../../baseline/source/data/maps/AbandonedShip_Rooms_1F/scripts.inc#L4) — AbandonedShip_Rooms_1F_EventScript_Gentleman at (12,5); Ships of this sort are rare, so I'm /  taking a look around. //  Hmhm… /  There appear to be other cabins… | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `AbandonedShip_Rooms_1F:object_events:002` | 4,5 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_HARBOR_MAIL; root Common_EventScript_FindItem; flag FLAG_ITEM_ABANDONED_SHIP_ROOMS_1F_HARBOR_MAIL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `AbandonedShip_Rooms_1F:object_events:003` | 10,11 | [AbandonedShip_Rooms_1F_EventScript_Thalia](../../baseline/source/data/maps/AbandonedShip_Rooms_1F/scripts.inc#L13) — AbandonedShip_Rooms_1F_EventScript_Thalia at (10,11); What on earth would compel you to /  come here? You must be curious! / Not just curious, but also strong… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AbandonedShip_Rooms_1F:object_events:004` | 10,16 | [AbandonedShip_Rooms_1F_EventScript_Demetrius](../../baseline/source/data/maps/AbandonedShip_Rooms_1F/scripts.inc#L8) — AbandonedShip_Rooms_1F_EventScript_Demetrius at (10,16); Waaah! /  I've been found! …Huh? / Oh, you're not my mom. | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AbandonedShip_Rooms_1F:warp_events:001` | 4,16 | Warp from (4,16, elevation 3) to MAP_ABANDONED_SHIP_CORRIDORS_1F warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Rooms_1F:warp_events:002` | 5,16 | Warp from (5,16, elevation 3) to MAP_ABANDONED_SHIP_CORRIDORS_1F warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Rooms_1F:warp_events:003` | 4,1 | Warp from (4,1, elevation 3) to MAP_ABANDONED_SHIP_CORRIDORS_1F warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Rooms_1F:warp_events:004` | 13,16 | Warp from (13,16, elevation 3) to MAP_ABANDONED_SHIP_CORRIDORS_1F warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Rooms_1F:warp_events:005` | 13,1 | Warp from (13,1, elevation 3) to MAP_ABANDONED_SHIP_CORRIDORS_1F warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Rooms_1F:warp_events:006` | 14,16 | Warp from (14,16, elevation 3) to MAP_ABANDONED_SHIP_CORRIDORS_1F warp 5. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
