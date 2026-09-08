# MarineCave_End

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/MarineCave_End/map.json) · [Scripts](../../baseline/source/data/maps/MarineCave_End/scripts.inc)

## Current map contract

`MAP_MARINE_CAVE_END` · `LAYOUT_MARINE_CAVE_END` · `WEATHER_FOG_HORIZONTAL` · `MUS_PETALBURG_WOODS`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `MarineCave_End:object_events:001` | 9,22 | Passive/staged OBJ_EVENT_GFX_KYOGRE_FRONT at (9,22); visibility flag FLAG_HIDE_MARINE_CAVE_KYOGRE; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `MarineCave_End:coord_events:001` | 9,26 | [MarineCave_End_EventScript_Kyogre](../../baseline/source/data/maps/MarineCave_End/scripts.inc#L25) — Coordinate trigger at (9,26); VAR_TEMP_1 == 1 invokes MarineCave_End_EventScript_Kyogre. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `MarineCave_End:warp_events:001` | 20,4 | Warp from (20,4, elevation 0) to MAP_MARINE_CAVE_ENTRANCE warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `MarineCave_End:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [MarineCave_End_OnResume](../../baseline/source/data/maps/MarineCave_End/scripts.inc#L6) — MAP_SCRIPT_ON_RESUME calls MarineCave_End_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `MarineCave_End:map_scripts:002` | MAP_SCRIPT_ON_TRANSITION | [MarineCave_End_OnTransition](../../baseline/source/data/maps/MarineCave_End/scripts.inc#L16) — MAP_SCRIPT_ON_TRANSITION calls MarineCave_End_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
