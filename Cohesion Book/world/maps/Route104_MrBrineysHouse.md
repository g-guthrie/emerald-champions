# Route104_MrBrineysHouse

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/Route104_MrBrineysHouse/map.json) · [Scripts](../../baseline/source/data/maps/Route104_MrBrineysHouse/scripts.inc)

## Current map contract

`MAP_ROUTE104_MR_BRINEYS_HOUSE` · `LAYOUT_ROUTE104_MR_BRINEYS_HOUSE` · `WEATHER_NONE` · `MUS_PETALBURG`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route104_MrBrineysHouse:object_events:001` | 5,3 | [Route104_MrBrineysHouse_EventScript_Briney](../../baseline/source/data/maps/Route104_MrBrineysHouse/scripts.inc#L22) — Route104_MrBrineysHouse_EventScript_Briney at (5,3); MR. BRINEY: Hold on, lass! /  Wait up, PEEKO! / Hm? You're {PLAYER}{KUN}! /  You saved my darling PEEKO! /  We owe so much to you! //  What's that? /  You want to sail with me? //  Hmhm… //  You have a LETTER bound for DEWFORD /  and a package for SLATEPORT, then? //  Quite the busy life you must lead! //  But, certainly, what you're asking is /  no problem at all. //  You've come to the right man! /  We'll set sail for DEWFORD. | **KEEP** · [W-C-TRAVEL](../common-contracts.md#w-c-travel) |
| `Route104_MrBrineysHouse:object_events:002` | 6,3 | [Route104_MrBrineysHouse_EventScript_Peeko](../../baseline/source/data/maps/Route104_MrBrineysHouse/scripts.inc#L87) — Route104_MrBrineysHouse_EventScript_Peeko at (6,3); PEEKO: Pii piihyoro! | **KEEP** · [W-C-TRAVEL](../common-contracts.md#w-c-travel) |
| `Route104_MrBrineysHouse:warp_events:001` | 5,8 | Warp from (5,8, elevation 0) to MAP_ROUTE104 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route104_MrBrineysHouse:warp_events:002` | 6,8 | Warp from (6,8, elevation 0) to MAP_ROUTE104 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route104_MrBrineysHouse:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route104_MrBrineysHouse_OnTransition](../../baseline/source/data/maps/Route104_MrBrineysHouse/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route104_MrBrineysHouse_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
