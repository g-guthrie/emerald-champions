# GraniteCave_1F

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/GraniteCave_1F/map.json) · [Scripts](../../baseline/source/data/maps/GraniteCave_1F/scripts.inc)

## Current map contract

`MAP_GRANITE_CAVE_1F` · `LAYOUT_GRANITE_CAVE_1F` · `WEATHER_NONE` · `MUS_PETALBURG_WOODS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `GraniteCave_1F:object_events:001` | 36,9 | [GraniteCave_1F_EventScript_Hiker](../../baseline/source/data/maps/GraniteCave_1F/scripts.inc#L4) — GraniteCave_1F_EventScript_Hiker at (36,9); Hey, you. /  It gets awfully dark ahead. /  It'll be tough trying to explore. //  That guy who came by earlier… /  STEVEN, I think it was. //  He knew how to use FLASH, so he ought /  to be all right, but… //  Well, for us HIKERS, helping out those /  that we meet is our motto. //  Here you go, I'll pass this on to you. / That HM registers FLASH as a field /  technique for your team. //  It lights up even the inky darkness /  of caves. //  With DEWFORD's GYM BADGE, caves /  light up as you enter them! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `GraniteCave_1F:object_events:002` | 17,7 | [Common_EventScript_FindItem](../../baseline/source/data/scripts/item_ball_scripts.inc#L1) — Pickup ITEM_ESCAPE_ROPE; root Common_EventScript_FindItem; flag FLAG_ITEM_GRANITE_CAVE_1F_ESCAPE_ROPE. | **KEEP** · [W-C-PICKUP](../common-contracts.md#w-c-pickup) |
| `GraniteCave_1F:warp_events:001` | 37,12 | Warp from (37,12, elevation 3) to MAP_ROUTE106 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `GraniteCave_1F:warp_events:002` | 35,3 | Warp from (35,3, elevation 3) to MAP_GRANITE_CAVE_B1F warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `GraniteCave_1F:warp_events:003` | 17,11 | Warp from (17,11, elevation 3) to MAP_GRANITE_CAVE_B1F warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `GraniteCave_1F:warp_events:004` | 5,10 | Warp from (5,10, elevation 3) to MAP_GRANITE_CAVE_STEVENS_ROOM warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
