# SafariZone_Northwest

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/SafariZone_Northwest/map.json) · [Scripts](../../baseline/source/data/maps/SafariZone_Northwest/scripts.inc)

## Current map contract

`MAP_SAFARI_ZONE_NORTHWEST` · `LAYOUT_SAFARI_ZONE_NORTHWEST` · `WEATHER_NONE` · `MUS_SAFARI_ZONE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SafariZone_Northwest:object_events:001` | 8,8 | [SafariZone_Northwest_EventScript_Man](../../baseline/source/data/maps/SafariZone_Northwest/scripts.inc#L4) — SafariZone_Northwest_EventScript_Man at (8,8); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SafariZone_Northwest:object_events:002` | 33,7 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_HONEY; root Common_EventScript_FindItem; flag FLAG_ITEM_SAFARI_ZONE_NORTH_WEST_HONEY. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SafariZone_Northwest:connections:001` | right | right connection to MAP_SAFARI_ZONE_NORTH, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SafariZone_Northwest:connections:002` | down | down connection to MAP_SAFARI_ZONE_SOUTHWEST, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
