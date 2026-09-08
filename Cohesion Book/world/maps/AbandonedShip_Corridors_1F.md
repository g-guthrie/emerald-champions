# AbandonedShip_Corridors_1F

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/AbandonedShip_Corridors_1F/map.json) · [Scripts](../../baseline/source/data/maps/AbandonedShip_Corridors_1F/scripts.inc)

## Current map contract

`MAP_ABANDONED_SHIP_CORRIDORS_1F` · `LAYOUT_ABANDONED_SHIP_CORRIDORS_1F` · `WEATHER_SHADE` · `MUS_ABANDONED_SHIP`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `AbandonedShip_Corridors_1F:object_events:001` | 17,7 | [AbandonedShip_Corridors_1F_EventScript_Youngster](../../baseline/source/data/maps/AbandonedShip_Corridors_1F/scripts.inc#L4) — AbandonedShip_Corridors_1F_EventScript_Youngster at (17,7); Isn't it fun here? /  I get excited just being here! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `AbandonedShip_Corridors_1F:object_events:002` | 5,10 | [AbandonedShip_Corridors_1F_EventScript_Charlie](../../baseline/source/data/maps/AbandonedShip_Corridors_1F/scripts.inc#L8) — AbandonedShip_Corridors_1F_EventScript_Charlie at (5,10); What's so funny about having my inner /  tube aboard the ship? / Whoa, you overwhelmed me! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `AbandonedShip_Corridors_1F:warp_events:001` | 9,11 | Warp from (9,11, elevation 3) to MAP_ABANDONED_SHIP_DECK warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_1F:warp_events:002` | 8,11 | Warp from (8,11, elevation 3) to MAP_ABANDONED_SHIP_DECK warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_1F:warp_events:003` | 0,11 | Warp from (0,11, elevation 3) to MAP_ABANDONED_SHIP_DECK warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_1F:warp_events:004` | 1,11 | Warp from (1,11, elevation 3) to MAP_ABANDONED_SHIP_DECK warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_1F:warp_events:005` | 11,9 | Warp from (11,9, elevation 3) to MAP_ABANDONED_SHIP_ROOMS_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_1F:warp_events:006` | 14,9 | Warp from (14,9, elevation 3) to MAP_ABANDONED_SHIP_ROOMS_1F warp 3. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_1F:warp_events:007` | 11,3 | Warp from (11,3, elevation 3) to MAP_ABANDONED_SHIP_ROOMS_1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_1F:warp_events:008` | 14,3 | Warp from (14,3, elevation 3) to MAP_ABANDONED_SHIP_ROOMS_1F warp 4. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_1F:warp_events:009` | 3,9 | Warp from (3,9, elevation 3) to MAP_ABANDONED_SHIP_ROOMS2_1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_1F:warp_events:010` | 16,2 | Warp from (16,2, elevation 3) to MAP_ABANDONED_SHIP_CORRIDORS_B1F warp 7. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_1F:warp_events:011` | 5,2 | Warp from (5,2, elevation 3) to MAP_ABANDONED_SHIP_CORRIDORS_B1F warp 6. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AbandonedShip_Corridors_1F:warp_events:012` | 3,3 | Warp from (3,3, elevation 3) to MAP_ABANDONED_SHIP_ROOMS2_1F warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
