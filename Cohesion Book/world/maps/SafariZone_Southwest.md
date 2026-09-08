# SafariZone_Southwest

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/SafariZone_Southwest/map.json) · [Scripts](../../baseline/source/data/maps/SafariZone_Southwest/scripts.inc)

## Current map contract

`MAP_SAFARI_ZONE_SOUTHWEST` · `LAYOUT_SAFARI_ZONE_SOUTHWEST` · `WEATHER_NONE` · `MUS_SAFARI_ZONE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `SafariZone_Southwest:object_events:001` | 22,9 | [SafariZone_Southwest_EventScript_Woman](../../baseline/source/data/maps/SafariZone_Southwest/scripts.inc#L4) — SafariZone_Southwest_EventScript_Woman at (22,9); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `SafariZone_Southwest:object_events:002` | 0,37 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_MAX_REVIVE; root Common_EventScript_FindItem; flag FLAG_ITEM_SAFARI_ZONE_SOUTH_WEST_MAX_REVIVE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `SafariZone_Southwest:bg_events:001` | 32,7 | [SafariZone_Southwest_EventScript_RestHouseSign](../../baseline/source/data/maps/SafariZone_Southwest/scripts.inc#L8) — SafariZone_Southwest_EventScript_RestHouseSign at (32,7); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `SafariZone_Southwest:warp_events:001` | 29,7 | Warp from (29,7, elevation 3) to MAP_SAFARI_ZONE_REST_HOUSE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SafariZone_Southwest:connections:001` | up | up connection to MAP_SAFARI_ZONE_NORTHWEST, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `SafariZone_Southwest:connections:002` | right | right connection to MAP_SAFARI_ZONE_SOUTH, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
