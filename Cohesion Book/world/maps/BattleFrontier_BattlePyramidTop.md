# BattleFrontier_BattlePyramidTop

**INERT/EXCLUDED.** Legacy facility staging/template map retained for identity and historical data support. Current native desks use central Circuit; no new native challenge may enter this old mode. Geometry and controller records are inventoried, not certified as a live attraction.

[Regional experience](../08-frontier.md) · [Map source](../../baseline/source/data/maps/BattleFrontier_BattlePyramidTop/map.json) · [Scripts](../../baseline/source/data/maps/BattleFrontier_BattlePyramidTop/scripts.inc)

## Current map contract

`MAP_BATTLE_FRONTIER_BATTLE_PYRAMID_TOP` · `LAYOUT_BATTLE_FRONTIER_BATTLE_PYRAMID_TOP` · `WEATHER_NONE` · `MUS_NONE`

Legacy facility staging/template map retained for identity and historical data support. Current native desks use central Circuit; no new native challenge may enter this old mode. Geometry and controller records are inventoried, not certified as a live attraction.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `BattleFrontier_BattlePyramidTop:object_events:001` | 17,11 | [BattleFrontier_BattlePyramidTop_EventScript_Attendant](../../baseline/source/data/maps/BattleFrontier_BattlePyramidTop/scripts.inc#L66) — BattleFrontier_BattlePyramidTop_EventScript_Attendant at (17,11); It is a delight to see you here! /  You have reached the summit of /  the BATTLE PYRAMID! //  Above here is the PYRAMID's /  lookout point. //  It is a place open only to those /  who have conquered the PYRAMID. //  Now, please! /  Up you go! / The PYRAMID's new conqueror! /  Let the name {PLAYER} be known! | **INERT/EXCLUDED** · [W-C-STORY](../common-contracts.md#w-c-story) |
| `BattleFrontier_BattlePyramidTop:object_events:002` | 17,7 | Passive/staged OBJ_EVENT_GFX_BRANDON at (17,7); visibility flag 0; no direct interaction script. | **INERT/EXCLUDED** · [W-C-PASSIVE](../common-contracts.md#w-c-passive) |
| `BattleFrontier_BattlePyramidTop:coord_events:001` | 17,9 | [BattleFrontier_BattlePyramidTop_EventScript_BattleBrandon](../../baseline/source/data/maps/BattleFrontier_BattlePyramidTop/scripts.inc#L102) — Coordinate trigger at (17,9); VAR_TEMP_2 == 0 invokes BattleFrontier_BattlePyramidTop_EventScript_BattleBrandon. | **INERT/EXCLUDED** · [W-C-COORD](../common-contracts.md#w-c-coord) |
| `BattleFrontier_BattlePyramidTop:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [BattleFrontier_BattlePyramidTop_OnResume](../../baseline/source/data/maps/BattleFrontier_BattlePyramidTop/scripts.inc#L25) — MAP_SCRIPT_ON_RESUME calls BattleFrontier_BattlePyramidTop_OnResume. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BattleFrontier_BattlePyramidTop:map_scripts:002` | MAP_SCRIPT_ON_FRAME_TABLE | [BattleFrontier_BattlePyramidTop_OnFrame](../../baseline/source/data/maps/BattleFrontier_BattlePyramidTop/scripts.inc#L42) — MAP_SCRIPT_ON_FRAME_TABLE calls BattleFrontier_BattlePyramidTop_OnFrame. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BattleFrontier_BattlePyramidTop:map_scripts:003` | MAP_SCRIPT_ON_TRANSITION | [BattleFrontier_BattlePyramidTop_OnTransition](../../baseline/source/data/maps/BattleFrontier_BattlePyramidTop/scripts.inc#L8) — MAP_SCRIPT_ON_TRANSITION calls BattleFrontier_BattlePyramidTop_OnTransition. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `BattleFrontier_BattlePyramidTop:map_scripts:004` | MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE | [BattleFrontier_BattlePyramidTop_OnWarp](../../baseline/source/data/maps/BattleFrontier_BattlePyramidTop/scripts.inc#L13) — MAP_SCRIPT_ON_WARP_INTO_MAP_TABLE calls BattleFrontier_BattlePyramidTop_OnWarp. | **INERT/EXCLUDED** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
