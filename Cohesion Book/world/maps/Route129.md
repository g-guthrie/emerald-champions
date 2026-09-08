# Route129

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/Route129/map.json) · [Scripts](../../baseline/source/data/maps/Route129/scripts.inc)

## Current map contract

`MAP_ROUTE129` · `LAYOUT_ROUTE129` · `WEATHER_SUNNY` · `MUS_ROUTE119`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route129:object_events:001` | 28,16 | [Route129_EventScript_Chase](../../baseline/source/data/maps/Route129/scripts.inc#L27) — Route129_EventScript_Chase at (28,16); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route129:object_events:002` | 10,14 | [Route129_EventScript_Allison](../../baseline/source/data/maps/Route129/scripts.inc#L32) — Route129_EventScript_Allison at (10,14); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route129:object_events:003` | 13,22 | [Route129_EventScript_Tisha](../../baseline/source/data/maps/Route129/scripts.inc#L42) — Route129_EventScript_Tisha at (13,22); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route129:object_events:004` | 35,9 | [Route129_EventScript_Reed](../../baseline/source/data/maps/Route129/scripts.inc#L37) — Route129_EventScript_Reed at (35,9); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route129:object_events:005` | 13,27 | [Route129_EventScript_Clarence](../../baseline/source/data/maps/Route129/scripts.inc#L47) — Route129_EventScript_Clarence at (13,27); shared behavior TRAINER | **REVISE** · [W-C-TRAINER](../common-contracts.md#w-c-trainer) · DIFF-01 |
| `Route129:connections:001` | up | up connection to MAP_ROUTE128, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route129:connections:002` | left | left connection to MAP_ROUTE130, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route129:connections:003` | dive | dive connection to MAP_UNDERWATER_ROUTE129, offset 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route129:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route129_OnTransition](../../baseline/source/data/maps/Route129/scripts.inc#L12) — MAP_SCRIPT_ON_TRANSITION calls Route129_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route129:map_scripts:002` | MAP_SCRIPT_ON_LOAD | [Route129_OnLoad](../../baseline/source/data/maps/Route129/scripts.inc#L7) — MAP_SCRIPT_ON_LOAD calls Route129_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `Route129:map_scripts:003` | MAP_SCRIPT_ON_FRAME_TABLE | [Route129_OnFrame](../../baseline/source/data/maps/Route129/scripts.inc#L23) — MAP_SCRIPT_ON_FRAME_TABLE calls Route129_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
