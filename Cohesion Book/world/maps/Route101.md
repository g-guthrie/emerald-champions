# Route101

**REVISE.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../01-opening.md) · [Map source](../../baseline/source/data/maps/Route101/map.json) · [Scripts](../../baseline/source/data/maps/Route101/scripts.inc)

## Current map contract

`MAP_ROUTE101` · `LAYOUT_ROUTE101` · `WEATHER_SUNNY` · `MUS_ROUTE101`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `Route101:object_events:001` | 16,8 | [Route101_EventScript_Youngster](../../baseline/source/data/maps/Route101/scripts.inc#L206) — Route101_EventScript_Youngster at (16,8); If POKéMON get tired, take them to /  a POKéMON CENTER. //  There's a POKéMON CENTER in OLDALE /  TOWN right close by. | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · INTRO-01 |
| `Route101:object_events:002` | 9,13 | Passive/staged OBJ_EVENT_GFX_PROF_BIRCH at (9,13); visibility flag FLAG_HIDE_ROUTE_101_BIRCH_ZIGZAGOON_BATTLE; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `Route101:object_events:003` | 7,14 | [Route101_EventScript_BirchsBag](../../baseline/source/data/maps/Route101/scripts.inc#L218) — Route101_EventScript_BirchsBag at (7,14); PROF. BIRCH: Whew… //  I was in the tall grass studying wild /  POKéMON when I was jumped. //  You saved me. /  Thanks a lot! //  Oh? //  Hi, you're {PLAYER}{KUN}! //  This is not the place to chat, so come /  by my POKéMON LAB later, okay? | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story) · INTRO-01, W-INTRO-GEOMETRY |
| `Route101:object_events:004` | 10,13 | Passive/staged OBJ_EVENT_GFX_ZIGZAGOON_1 at (10,13); visibility flag FLAG_HIDE_ROUTE_101_ZIGZAGOON; no direct interaction script. | **REVISE** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) · INTRO-01 |
| `Route101:object_events:005` | 5,11 | [ProfBirch_EventScript_RatePokedexOrRegister](../../baseline/source/data/scripts/prof_birch.inc#L35) — ProfBirch_EventScript_RatePokedexOrRegister at (5,11); shared behavior STORY | **REVISE** · [W-C-STORY](../common-contracts.md#w-c-story) · INTRO-01 |
| `Route101:object_events:006` | 2,13 | [Route101_EventScript_Boy](../../baseline/source/data/maps/Route101/scripts.inc#L210) — Route101_EventScript_Boy at (2,13); Wild POKéMON will jump out at you in /  tall grass. //  If you want to catch POKéMON, you have /  to go into the tall grass and search. | **REVISE** · [W-C-FLAVOR](../common-contracts.md#w-c-flavor) · INTRO-01 |
| `Route101:coord_events:001` | 10,19 | [Route101_EventScript_StartBirchRescue](../../baseline/source/data/maps/Route101/scripts.inc#L19) — Coordinate trigger at (10,19); VAR_ROUTE101_STATE == 1 invokes Route101_EventScript_StartBirchRescue. | **REVISE** · [W-C-COORD](../common-contracts.md#w-c-coord) · INTRO-01, W-INTRO-GEOMETRY |
| `Route101:coord_events:002` | 11,19 | [Route101_EventScript_StartBirchRescue](../../baseline/source/data/maps/Route101/scripts.inc#L19) — Coordinate trigger at (11,19); VAR_ROUTE101_STATE == 1 invokes Route101_EventScript_StartBirchRescue. | **REVISE** · [W-C-COORD](../common-contracts.md#w-c-coord) · INTRO-01, W-INTRO-GEOMETRY |
| `Route101:coord_events:003` | 10,18 | [Route101_EventScript_PreventExitSouth](../../baseline/source/data/maps/Route101/scripts.inc#L44) — Coordinate trigger at (10,18); VAR_ROUTE101_STATE == 2 invokes Route101_EventScript_PreventExitSouth. | **REVISE** · [W-C-COORD](../common-contracts.md#w-c-coord) · INTRO-01 |
| `Route101:coord_events:004` | 11,18 | [Route101_EventScript_PreventExitSouth](../../baseline/source/data/maps/Route101/scripts.inc#L44) — Coordinate trigger at (11,18); VAR_ROUTE101_STATE == 2 invokes Route101_EventScript_PreventExitSouth. | **REVISE** · [W-C-COORD](../common-contracts.md#w-c-coord) · INTRO-01 |
| `Route101:coord_events:005` | 6,16 | [Route101_EventScript_PreventExitWest](../../baseline/source/data/maps/Route101/scripts.inc#L53) — Coordinate trigger at (6,16); VAR_ROUTE101_STATE == 2 invokes Route101_EventScript_PreventExitWest. | **REVISE** · [W-C-COORD](../common-contracts.md#w-c-coord) · INTRO-01 |
| `Route101:coord_events:006` | 6,15 | [Route101_EventScript_PreventExitWest](../../baseline/source/data/maps/Route101/scripts.inc#L53) — Coordinate trigger at (6,15); VAR_ROUTE101_STATE == 2 invokes Route101_EventScript_PreventExitWest. | **REVISE** · [W-C-COORD](../common-contracts.md#w-c-coord) · INTRO-01 |
| `Route101:coord_events:007` | 6,17 | [Route101_EventScript_PreventExitWest](../../baseline/source/data/maps/Route101/scripts.inc#L53) — Coordinate trigger at (6,17); VAR_ROUTE101_STATE == 2 invokes Route101_EventScript_PreventExitWest. | **REVISE** · [W-C-COORD](../common-contracts.md#w-c-coord) · INTRO-01 |
| `Route101:coord_events:008` | 6,18 | [Route101_EventScript_PreventExitWest](../../baseline/source/data/maps/Route101/scripts.inc#L53) — Coordinate trigger at (6,18); VAR_ROUTE101_STATE == 2 invokes Route101_EventScript_PreventExitWest. | **REVISE** · [W-C-COORD](../common-contracts.md#w-c-coord) · INTRO-01 |
| `Route101:coord_events:009` | 7,13 | [Route101_EventScript_PreventExitNorth](../../baseline/source/data/maps/Route101/scripts.inc#L62) — Coordinate trigger at (7,13); VAR_ROUTE101_STATE == 2 invokes Route101_EventScript_PreventExitNorth. | **REVISE** · [W-C-COORD](../common-contracts.md#w-c-coord) · INTRO-01 |
| `Route101:bg_events:001` | 5,9 | [Route101_EventScript_RouteSign](../../baseline/source/data/maps/Route101/scripts.inc#L214) — Route101_EventScript_RouteSign at (5,9); ROUTE 101 /  {UP_ARROW} OLDALE TOWN | **REVISE** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) · GUIDE-01, INTRO-01 |
| `Route101:connections:001` | up | up connection to MAP_OLDALE_TOWN, offset 0. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · INTRO-01 |
| `Route101:connections:002` | down | down connection to MAP_LITTLEROOT_TOWN, offset 0. | **REVISE** · [W-C-WARP](../common-contracts.md#w-c-warp) · INTRO-01 |
| `Route101:map_scripts:001` | MAP_SCRIPT_ON_TRANSITION | [Route101_OnTransition](../../baseline/source/data/maps/Route101/scripts.inc#L6) — MAP_SCRIPT_ON_TRANSITION calls Route101_OnTransition. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · INTRO-01 |
| `Route101:map_scripts:002` | MAP_SCRIPT_ON_FRAME_TABLE | [Route101_OnFrame](../../baseline/source/data/maps/Route101/scripts.inc#L10) — MAP_SCRIPT_ON_FRAME_TABLE calls Route101_OnFrame. | **REVISE** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) · INTRO-01 |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
