# DesertRuins

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/DesertRuins/map.json) · [Scripts](../../baseline/source/data/maps/DesertRuins/scripts.inc)

## Current map contract

`MAP_DESERT_RUINS` · `LAYOUT_DESERT_RUINS` · `WEATHER_NONE` · `MUS_SEALED_CHAMBER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `DesertRuins:object_events:001` | 8,7 | [DesertRuins_EventScript_Regirock](../../baseline/source/data/maps/DesertRuins/scripts.inc#L57) — DesertRuins_EventScript_Regirock at (8,7); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `DesertRuins:bg_events:001` | 8,20 | [DesertRuins_EventScript_CaveEntranceMiddle](../../baseline/source/data/maps/DesertRuins/scripts.inc#L39) — DesertRuins_EventScript_CaveEntranceMiddle at (8,20); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `DesertRuins:bg_events:002` | 7,20 | [DesertRuins_EventScript_CaveEntranceSide](../../baseline/source/data/maps/DesertRuins/scripts.inc#L51) — DesertRuins_EventScript_CaveEntranceSide at (7,20); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `DesertRuins:bg_events:003` | 9,20 | [DesertRuins_EventScript_CaveEntranceSide](../../baseline/source/data/maps/DesertRuins/scripts.inc#L51) — DesertRuins_EventScript_CaveEntranceSide at (9,20); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `DesertRuins:warp_events:001` | 8,29 | Warp from (8,29, elevation 3) to MAP_ROUTE111 warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `DesertRuins:warp_events:002` | 8,20 | Warp from (8,20, elevation 0) to MAP_DESERT_RUINS warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `DesertRuins:warp_events:003` | 8,11 | Warp from (8,11, elevation 3) to MAP_DESERT_RUINS warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `DesertRuins:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [DesertRuins_OnResume](../../baseline/source/data/maps/DesertRuins/scripts.inc#L7) — MAP_SCRIPT_ON_RESUME calls DesertRuins_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `DesertRuins:map_scripts:002` | MAP_SCRIPT_ON_LOAD | [DesertRuins_OnLoad](../../baseline/source/data/maps/DesertRuins/scripts.inc#L17) — MAP_SCRIPT_ON_LOAD calls DesertRuins_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `DesertRuins:map_scripts:003` | MAP_SCRIPT_ON_TRANSITION | [DesertRuins_OnTransition](../../baseline/source/data/maps/DesertRuins/scripts.inc#L30) — MAP_SCRIPT_ON_TRANSITION calls DesertRuins_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
