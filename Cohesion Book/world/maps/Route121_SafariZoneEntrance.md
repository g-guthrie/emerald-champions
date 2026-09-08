# Route121_SafariZoneEntrance

**REVISE.** Keep the Catching Charm welcome gift, explicit entry requirements, space check and optional Safari mechanics. Any species obtainable only here must have its actual final catch method reflected in acquisition coverage.

[Regional experience](../09-side-activities.md) · [Map source](../../baseline/source/data/maps/Route121_SafariZoneEntrance/map.json) · [Scripts](../../baseline/source/data/maps/Route121_SafariZoneEntrance/scripts.inc)

## Current map contract

`MAP_ROUTE121_SAFARI_ZONE_ENTRANCE` · `LAYOUT_ROUTE121_SAFARI_ZONE_ENTRANCE` · `WEATHER_NONE` · `MUS_FORTREE`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route121_SafariZoneEntrance:object_events:001` | 17,9 | [Route121_SafariZoneEntrance_EventScript_WelcomeAttendant](../../baseline/source/data/maps/Route121_SafariZoneEntrance/scripts.inc#L28) — Route121_SafariZoneEntrance_EventScript_WelcomeAttendant at (17,9); shared behavior FLAVOR | **KEEP** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) |
| `Route121_SafariZoneEntrance:object_events:002` | 10,2 | [Route121_SafariZoneEntrance_EventScript_InfoAttendant](../../baseline/source/data/maps/Route121_SafariZoneEntrance/scripts.inc#L32) — Route121_SafariZoneEntrance_EventScript_InfoAttendant at (10,2); First-time visitors get a CATCHING /  CHARM. Carry it and your throws will /  land critical captures more often. | **KEEP** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `Route121_SafariZoneEntrance:object_events:003` | 8,2 | Passive/staged OBJ_EVENT_GFX_CAMPER at (8,2); visibility flag 0; no direct interaction script. | **KEEP** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `Route121_SafariZoneEntrance:coord_events:001` | 8,4 | [Route121_SafariZoneEntrance_EventScript_EntranceCounterTrigger](../../baseline/source/data/maps/Route121_SafariZoneEntrance/scripts.inc#L52) — Coordinate trigger at (8,4); VAR_TEMP_1 == 0 invokes Route121_SafariZoneEntrance_EventScript_EntranceCounterTrigger. | **KEEP** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `Route121_SafariZoneEntrance:bg_events:001` | 15,1 | [Route121_SafariZoneEntrance_EventScript_TrainerTipSign](../../baseline/source/data/maps/Route121_SafariZoneEntrance/scripts.inc#L134) — Route121_SafariZoneEntrance_EventScript_TrainerTipSign at (15,1); shared behavior BACKGROUND | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01 |
| `Route121_SafariZoneEntrance:warp_events:001` | 2,5 | Warp from (2,5, elevation 0) to MAP_SAFARI_ZONE_SOUTH warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route121_SafariZoneEntrance:warp_events:002` | 3,5 | Warp from (3,5, elevation 0) to MAP_SAFARI_ZONE_SOUTH warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route121_SafariZoneEntrance:warp_events:003` | 14,13 | Warp from (14,13, elevation 0) to MAP_ROUTE121 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route121_SafariZoneEntrance:warp_events:004` | 15,13 | Warp from (15,13, elevation 0) to MAP_ROUTE121 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `Route121_SafariZoneEntrance:map_scripts:001` | MAP_SCRIPT_ON_FRAME_TABLE | [Route121_SafariZoneEntrance_OnFrame](../../baseline/source/data/maps/Route121_SafariZoneEntrance/scripts.inc#L5) — MAP_SCRIPT_ON_FRAME_TABLE calls Route121_SafariZoneEntrance_OnFrame. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
