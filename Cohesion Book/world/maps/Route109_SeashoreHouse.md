# Route109_SeashoreHouse

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../02-dewford-and-slateport.md) · [Map source](../../baseline/source/data/maps/Route109_SeashoreHouse/map.json) · [Scripts](../../baseline/source/data/maps/Route109_SeashoreHouse/scripts.inc)

## Current map contract

`MAP_ROUTE109_SEASHORE_HOUSE` · `LAYOUT_ROUTE109_SEASHORE_HOUSE` · `WEATHER_NONE` · `MUS_DEWFORD`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route109_SeashoreHouse:object_events:001` | 6,2 | [Route109_SeashoreHouse_EventScript_Owner](../../baseline/source/data/maps/Route109_SeashoreHouse/scripts.inc#L9) — Route109_SeashoreHouse_EventScript_Owner at (6,2); I'm the owner of the SEASHORE HOUSE. /  But you can call me MR. SEA! //  What I love above all is to see hot /  POKéMON battles. //  Let me see that your heart burns hot! //  If you can defeat all the TRAINERS /  here, I'll reward your efforts. / Show me some hot matches! //  I run this SEASHORE HOUSE just for /  that reason alone! | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route109_SeashoreHouse:object_events:002` | 2,3 | [Route109_SeashoreHouse_EventScript_Dwayne](../../baseline/source/data/maps/Route109_SeashoreHouse/scripts.inc#L77) — Route109_SeashoreHouse_EventScript_Dwayne at (2,3); If you're looking for a battle in the /  SEASHORE HOUSE, you'll find no /  hotter TRAINER than me, matey! / That was a hot battle! /  I can accept that loss, matey! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route109_SeashoreHouse:object_events:003` | 14,9 | [Route109_SeashoreHouse_EventScript_Simon](../../baseline/source/data/maps/Route109_SeashoreHouse/scripts.inc#L87) — Route109_SeashoreHouse_EventScript_Simon at (14,9); I'm going to show you how great /  my POKéMON are, but don't cry! / …I lost, but I won't cry… | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route109_SeashoreHouse:object_events:004` | 10,5 | [Route109_SeashoreHouse_EventScript_Johanna](../../baseline/source/data/maps/Route109_SeashoreHouse/scripts.inc#L82) — Route109_SeashoreHouse_EventScript_Johanna at (10,5); Boring battles aren't worth the effort. //  Fiery hot battles are what toughen up /  TRAINERS and POKéMON! / That's hot! | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route109_SeashoreHouse:warp_events:001` | 6,9 | Warp from (6,9, elevation 0) to MAP_ROUTE109 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route109_SeashoreHouse:warp_events:002` | 7,9 | Warp from (7,9, elevation 0) to MAP_ROUTE109 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route109_SeashoreHouse:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route109_SeashoreHouse_OnTransition](../../baseline/source/data/maps/Route109_SeashoreHouse/scripts.inc#L5) — MAP_SCRIPT_ON_TRANSITION calls Route109_SeashoreHouse_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
