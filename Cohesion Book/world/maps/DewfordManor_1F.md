# DewfordManor_1F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/DewfordManor_1F/map.json) · [Scripts](../../baseline/source/data/maps/DewfordManor_1F/scripts.inc)

## Current map contract

`MAP_DEWFORD_MANOR_1F` · `LAYOUT_DEWFORD_MANOR_1F` · `WEATHER_SHADE` · `MUS_MT_PYRE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `DewfordManor_1F:object_events:001` | 8,8 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_DUSK_BALL; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_WOODS2_DUSK_BALL. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `DewfordManor_1F:object_events:002` | 1,11 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_SABLENITE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_MANOR_SABLENITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `DewfordManor_1F:object_events:003` | 3,7 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_BANETTITE; root Common_EventScript_FindItem; flag FLAG_ITEM_MT_PYRE_6F_BANETTITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `DewfordManor_1F:object_events:004` | 3,9 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_CHANDELURITE; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_ROUTE121_CHANDELURITE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `DewfordManor_1F:warp_events:001` | 11,11 | Warp from (11,11, elevation 0) to MAP_DEWFORD_MEADOW warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `DewfordManor_1F:warp_events:002` | 12,11 | Warp from (12,11, elevation 0) to MAP_DEWFORD_MEADOW warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `DewfordManor_1F:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [DewfordManor_1F_OnTransition](../../baseline/source/data/maps/DewfordManor_1F/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls DewfordManor_1F_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
