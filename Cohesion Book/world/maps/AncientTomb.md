# AncientTomb

**KEEP.** Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

[Regional experience](../06-sea-and-crisis.md) · [Map source](../../baseline/source/data/maps/AncientTomb/map.json) · [Scripts](../../baseline/source/data/maps/AncientTomb/scripts.inc)

## Current map contract

`MAP_ANCIENT_TOMB` · `LAYOUT_ANCIENT_TOMB` · `WEATHER_NONE` · `MUS_SEALED_CHAMBER`

Map-specific script/dialogue digest and physical event records were reviewed against the region contract. Preserve the described map and its existing state handoffs, subject to the explicitly linked proposals and central battle/acquisition rules.

## Complete event ledger

Every row is a distinct snapshot event. A KEEP applies the examined common contract and this map’s context; it is not a claim that this scene has been traversed in an emulator.

| Event | Position / selector | Current behavior | Disposition / final dependency |
|---|---|---|---|
| `AncientTomb:object_events:001` | 8,7 | [AncientTomb_EventScript_Registeel](../../baseline/source/data/maps/AncientTomb/scripts.inc#L57) — AncientTomb_EventScript_Registeel at (8,7); shared behavior LEGEND | **KEEP** · [W-C-LEGEND](../common-contracts.md#w-c-legend) |
| `AncientTomb:bg_events:001` | 8,20 | [AncientTomb_EventScript_CaveEntranceMiddle](../../baseline/source/data/maps/AncientTomb/scripts.inc#L39) — AncientTomb_EventScript_CaveEntranceMiddle at (8,20); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `AncientTomb:bg_events:002` | 7,20 | [AncientTomb_EventScript_CaveEntranceSide](../../baseline/source/data/maps/AncientTomb/scripts.inc#L51) — AncientTomb_EventScript_CaveEntranceSide at (7,20); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `AncientTomb:bg_events:003` | 9,20 | [AncientTomb_EventScript_CaveEntranceSide](../../baseline/source/data/maps/AncientTomb/scripts.inc#L51) — AncientTomb_EventScript_CaveEntranceSide at (9,20); shared behavior BACKGROUND | **KEEP** · [W-C-BACKGROUND](../common-contracts.md#w-c-background) |
| `AncientTomb:warp_events:001` | 8,29 | Warp from (8,29, elevation 3) to MAP_ROUTE120 warp 0. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AncientTomb:warp_events:002` | 8,20 | Warp from (8,20, elevation 0) to MAP_ANCIENT_TOMB warp 2. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AncientTomb:warp_events:003` | 8,11 | Warp from (8,11, elevation 3) to MAP_ANCIENT_TOMB warp 1. | **KEEP** · [W-C-WARP](../common-contracts.md#w-c-warp) |
| `AncientTomb:map_scripts:001` | MAP_SCRIPT_ON_RESUME | [AncientTomb_OnResume](../../baseline/source/data/maps/AncientTomb/scripts.inc#L7) — MAP_SCRIPT_ON_RESUME calls AncientTomb_OnResume. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `AncientTomb:map_scripts:002` | MAP_SCRIPT_ON_LOAD | [AncientTomb_OnLoad](../../baseline/source/data/maps/AncientTomb/scripts.inc#L26) — MAP_SCRIPT_ON_LOAD calls AncientTomb_OnLoad. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |
| `AncientTomb:map_scripts:003` | MAP_SCRIPT_ON_TRANSITION | [AncientTomb_OnTransition](../../baseline/source/data/maps/AncientTomb/scripts.inc#L17) — MAP_SCRIPT_ON_TRANSITION calls AncientTomb_OnTransition. | **KEEP** · [W-C-MAP_CALLBACK](../common-contracts.md#w-c-map_callback) |

## Final specification and acceptance

Retain the complete baseline map record except the exact entries in [proposed edits](../changes.md) and its owning battle/acquisition chapters. Preserve numeric IDs and unrelated flags.
Required implementation evidence: enter from each actual neighbor; exercise locked/unlocked and before/after story states that change this map; verify objects, collision/elevation, trigger direction, destination and return route. For generic repetitions, execute the shared behavior once per meaningful engine case and inspect each instance’s specific placement. A source reference check does not replace this traversal.
The [machine ledger](../event-ledger.json) records complete event dictionaries, local/shared label sets, every referenced dialogue label, proposal IDs, and explicit evidence boundaries.
