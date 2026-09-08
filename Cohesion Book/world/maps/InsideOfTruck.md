# InsideOfTruck

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/InsideOfTruck/map.json) · [Scripts](../../baseline/source/data/maps/InsideOfTruck/scripts.inc)

## Current map contract

`MAP_INSIDE_OF_TRUCK` · `LAYOUT_INSIDE_OF_TRUCK` · `WEATHER_NONE` · `MUS_NONE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `InsideOfTruck:object_events:001` | 0,0 | [InsideOfTruck_EventScript_MovingBox](../../baseline/source/data/maps/InsideOfTruck/scripts.inc#L50) — InsideOfTruck_EventScript_MovingBox at (0,0); The box is printed with a POKéMON logo. //  It's a POKéMON brand moving and /  delivery service. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `InsideOfTruck:object_events:002` | 0,3 | [InsideOfTruck_EventScript_MovingBox](../../baseline/source/data/maps/InsideOfTruck/scripts.inc#L50) — InsideOfTruck_EventScript_MovingBox at (0,3); The box is printed with a POKéMON logo. //  It's a POKéMON brand moving and /  delivery service. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `InsideOfTruck:object_events:003` | 2,3 | [InsideOfTruck_EventScript_MovingBox](../../baseline/source/data/maps/InsideOfTruck/scripts.inc#L50) — InsideOfTruck_EventScript_MovingBox at (2,3); The box is printed with a POKéMON logo. //  It's a POKéMON brand moving and /  delivery service. | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `InsideOfTruck:coord_events:001` | 3,1 | [InsideOfTruck_EventScript_SetIntroFlags](../../baseline/source/data/maps/InsideOfTruck/scripts.inc#L16) — Coordinate trigger at (3,1); VAR_LITTLEROOT_INTRO_STATE == 0 invokes InsideOfTruck_EventScript_SetIntroFlags. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `InsideOfTruck:coord_events:002` | 3,2 | [InsideOfTruck_EventScript_SetIntroFlags](../../baseline/source/data/maps/InsideOfTruck/scripts.inc#L16) — Coordinate trigger at (3,2); VAR_LITTLEROOT_INTRO_STATE == 0 invokes InsideOfTruck_EventScript_SetIntroFlags. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `InsideOfTruck:coord_events:003` | 3,3 | [InsideOfTruck_EventScript_SetIntroFlags](../../baseline/source/data/maps/InsideOfTruck/scripts.inc#L16) — Coordinate trigger at (3,3); VAR_LITTLEROOT_INTRO_STATE == 0 invokes InsideOfTruck_EventScript_SetIntroFlags. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `InsideOfTruck:bg_events:001` | 1,0 | [InsideOfTruck_EventScript_MovingBox](../../baseline/source/data/maps/InsideOfTruck/scripts.inc#L50) — InsideOfTruck_EventScript_MovingBox at (1,0); The box is printed with a POKéMON logo. //  It's a POKéMON brand moving and /  delivery service. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `InsideOfTruck:bg_events:002` | 3,4 | [InsideOfTruck_EventScript_MovingBox](../../baseline/source/data/maps/InsideOfTruck/scripts.inc#L50) — InsideOfTruck_EventScript_MovingBox at (3,4); The box is printed with a POKéMON logo. //  It's a POKéMON brand moving and /  delivery service. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `InsideOfTruck:bg_events:003` | 2,3 | [InsideOfTruck_EventScript_MovingBox](../../baseline/source/data/maps/InsideOfTruck/scripts.inc#L50) — InsideOfTruck_EventScript_MovingBox at (2,3); The box is printed with a POKéMON logo. //  It's a POKéMON brand moving and /  delivery service. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `InsideOfTruck:bg_events:004` | 0,1 | [InsideOfTruck_EventScript_MovingBox](../../baseline/source/data/maps/InsideOfTruck/scripts.inc#L50) — InsideOfTruck_EventScript_MovingBox at (0,1); The box is printed with a POKéMON logo. //  It's a POKéMON brand moving and /  delivery service. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `InsideOfTruck:bg_events:005` | 0,2 | [InsideOfTruck_EventScript_MovingBox](../../baseline/source/data/maps/InsideOfTruck/scripts.inc#L50) — InsideOfTruck_EventScript_MovingBox at (0,2); The box is printed with a POKéMON logo. //  It's a POKéMON brand moving and /  delivery service. | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `InsideOfTruck:warp_events:001` | 4,1 | Warp from (4,1, elevation 0) to MAP_DYNAMIC warp WARP_ID_DYNAMIC. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `InsideOfTruck:warp_events:002` | 4,2 | Warp from (4,2, elevation 0) to MAP_DYNAMIC warp WARP_ID_DYNAMIC. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `InsideOfTruck:warp_events:003` | 4,3 | Warp from (4,3, elevation 0) to MAP_DYNAMIC warp WARP_ID_DYNAMIC. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `InsideOfTruck:map_scripts:001` | MAP_SCRIPT_ON_LOAD | [InsideOfTruck_OnLoad](../../baseline/source/data/maps/InsideOfTruck/scripts.inc#L6) — MAP_SCRIPT_ON_LOAD calls InsideOfTruck_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `InsideOfTruck:map_scripts:002` | MAP_SCRIPT_ON_RESUME | [InsideOfTruck_OnResume](../../baseline/source/data/maps/InsideOfTruck/scripts.inc#L12) — MAP_SCRIPT_ON_RESUME calls InsideOfTruck_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
