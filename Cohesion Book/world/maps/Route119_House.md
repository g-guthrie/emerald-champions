# Route119_House

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../05-rainforest.md) · [Map source](../../baseline/source/data/maps/Route119_House/map.json) · [Scripts](../../baseline/source/data/maps/Route119_House/scripts.inc)

## Current map contract

`MAP_ROUTE119_HOUSE` · `LAYOUT_HOUSE1` · `WEATHER_NONE` · `MUS_RUSTBORO`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route119_House:object_events:001` | 7,2 | [Route119_House_EventScript_Woman](../../baseline/source/data/maps/Route119_House/scripts.inc#L4) — Route119_House_EventScript_Woman at (7,2); I heard about a cave called the CAVE /  OF ORIGIN. //  People rumor that the spirits of /  POKéMON are revived there. Could /  something like that really happen? | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route119_House:object_events:002` | 1,6 | [Route119_House_EventScript_Wingull](../../baseline/source/data/maps/Route119_House/scripts.inc#L8) — Route119_House_EventScript_Wingull at (1,6); WINGULL: Pihyoh! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route119_House:object_events:003` | 0,4 | [Route119_House_EventScript_Wingull](../../baseline/source/data/maps/Route119_House/scripts.inc#L8) — Route119_House_EventScript_Wingull at (0,4); WINGULL: Pihyoh! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route119_House:object_events:004` | 2,2 | [Route119_House_EventScript_Wingull](../../baseline/source/data/maps/Route119_House/scripts.inc#L8) — Route119_House_EventScript_Wingull at (2,2); WINGULL: Pihyoh! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route119_House:object_events:005` | 8,5 | [Route119_House_EventScript_Wingull](../../baseline/source/data/maps/Route119_House/scripts.inc#L8) — Route119_House_EventScript_Wingull at (8,5); WINGULL: Pihyoh! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route119_House:object_events:006` | 6,6 | [Route119_House_EventScript_Wingull](../../baseline/source/data/maps/Route119_House/scripts.inc#L8) — Route119_House_EventScript_Wingull at (6,6); WINGULL: Pihyoh! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route119_House:object_events:007` | 5,3 | [Route119_House_EventScript_Wingull](../../baseline/source/data/maps/Route119_House/scripts.inc#L8) — Route119_House_EventScript_Wingull at (5,3); WINGULL: Pihyoh! | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route119_House:warp_events:001` | 3,8 | Warp from (3,8, elevation 0) to MAP_ROUTE119 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route119_House:warp_events:002` | 4,8 | Warp from (4,8, elevation 0) to MAP_ROUTE119 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
