# SafariZone_South

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/SafariZone_South/map.json) · [Scripts](../../baseline/source/data/maps/SafariZone_South/scripts.inc)

## Current map contract

`MAP_SAFARI_ZONE_SOUTH` · `LAYOUT_SAFARI_ZONE_SOUTH` · `WEATHER_NONE` · `MUS_SAFARI_ZONE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SafariZone_South:object_events:001` | 32,34 | [SafariZone_South_EventScript_ExitAttendant](../../baseline/source/data/maps/SafariZone_South/scripts.inc#L50) — SafariZone_South_EventScript_ExitAttendant at (32,34); shared behavior STORY | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `SafariZone_South:object_events:002` | 26,28 | [SafariZone_South_EventScript_Boy](../../baseline/source/data/maps/SafariZone_South/scripts.inc#L38) — SafariZone_South_EventScript_Boy at (26,28); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SafariZone_South:object_events:003` | 16,6 | [SafariZone_South_EventScript_Man](../../baseline/source/data/maps/SafariZone_South/scripts.inc#L42) — SafariZone_South_EventScript_Man at (16,6); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SafariZone_South:object_events:004` | 15,31 | [SafariZone_South_EventScript_Youngster](../../baseline/source/data/maps/SafariZone_South/scripts.inc#L46) — SafariZone_South_EventScript_Youngster at (15,31); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SafariZone_South:object_events:005` | 30,5 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_HONEY; root Common_EventScript_FindItem; flag FLAG_EC_ITEM_SAFARI_ZONE_SOUTH_HONEY. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SafariZone_South:warp_events:001` | 32,33 | Warp from (32,33, elevation 0) to MAP_ROUTE121_SAFARI_ZONE_ENTRANCE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SafariZone_South:connections:001` | up | up connection to MAP_SAFARI_ZONE_NORTH, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SafariZone_South:connections:002` | left | left connection to MAP_SAFARI_ZONE_SOUTHWEST, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SafariZone_South:connections:003` | right | right connection to MAP_SAFARI_ZONE_SOUTHEAST, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SafariZone_South:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [SafariZone_South_OnTransition](../../baseline/source/data/maps/SafariZone_South/scripts.inc#L21) — MAP_SCRIPT_ON_TRANSITION calls SafariZone_South_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `SafariZone_South:map_scripts:002` | MAP_SCRIPT_ON_FRAME_TABLE | [SafariZone_South_OnFrame](../../baseline/source/data/maps/SafariZone_South/scripts.inc#L6) — MAP_SCRIPT_ON_FRAME_TABLE calls SafariZone_South_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
